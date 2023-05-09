/*
 * soc_aukf.c
 *
 *  Created on: Feb 6, 2023
 *      Author: ADMIN
 */

#include "../soc_ukf_refactor/soc_vffrls.h"

#include "math_util.h"
#include "matrix.h"

#include "../soc_ukf_refactor/battery_model.h"

#if 0

#define IS_VECTOR							1
#define VFFRLS_DIM							4

#define MATRIX_INDEX_1						0
#define MATRIX_INDEX_2						1
#define MATRIX_INDEX_3						2
#define MATRIX_INDEX_4						3

#define PHI_MATRIX_UNIT_INDEX				0
#define PHI_MATRIX_UK_MINUS1_INDEX			1
#define PHI_MATRIX_IK_INDEX					2
#define PHI_MATRIX_IK_MINUS1_INDEX			3
#define THETA_MATRIX_A0_INDEX				0
#define THETA_MATRIX_A1_INDEX				1
#define THETA_MATRIX_A2_INDEX				2
#define THETA_MATRIX_A3_INDEX				3

#define LAMDA								(0.9992f)
#define COVARIANCE_INIT						(0.001f)

const float vffrls_covariance_default[VFFRLS_DIM * VFFRLS_DIM] = {
		COVARIANCE_INIT, 0.0f, 0.0f, 0.0f,
		0.0f, COVARIANCE_INIT, 0.0f, 0.0f,
		0.0f, 0.0f, COVARIANCE_INIT, 0.0f,
		0.0f, 0.0f, 0.0f, COVARIANCE_INIT
};

const float vffrls_theta_default[VFFRLS_DIM] = {
		3.25f, 0.22f, -0.02f, -0.01f
};
const float vffrls_kalmanGain_default[VFFRLS_DIM] = {
		0.001f, 0.0f, 0.0f, 0.0f
};
const float vffrls_phi_default[VFFRLS_DIM] = {
		1.0f, 0.0f, 0.0f, 0.0f
};

Battery_param battery_param;

static float priorVoltage;
static float estimateVoltage;
static float errorVoltage;
static float resistor0;
static float resistor1;
static float RC_parameter;

static float covariance[VFFRLS_DIM * VFFRLS_DIM];
static float theta[VFFRLS_DIM];
static float kalmanGain[VFFRLS_DIM];
static float phi[VFFRLS_DIM];

static Matrix covariance_matrix = {.row=VFFRLS_DIM, .col=VFFRLS_DIM, .entries=&covariance[0]};
static Matrix kalmanGain_matrix = {.row=VFFRLS_DIM, .col=IS_VECTOR, .entries=&kalmanGain[0]};
static Matrix phi_matrix = {.row=IS_VECTOR, .col=VFFRLS_DIM, .entries=&phi[0]};
static Matrix theta_matrix = {.row=VFFRLS_DIM, .col=IS_VECTOR, .entries=&theta[0]};

static float identity_matrix_default[VFFRLS_DIM * VFFRLS_DIM];
static float m_covariance_update_default[VFFRLS_DIM * VFFRLS_DIM];
//static float t_covariance_update_default[VFFRLS_DIM * VFFRLS_DIM];
static float t_kalmanGain_update_default[VFFRLS_DIM];
static float d_theta_update_default[VFFRLS_DIM];

static Matrix identity_matrix = { .row = VFFRLS_DIM, .col = VFFRLS_DIM,
		.entries = &identity_matrix_default[0] };
static Matrix m_covariance_update = {.row = VFFRLS_DIM, .col=VFFRLS_DIM, .entries=&m_covariance_update_default[0]};
//static Matrix t_covariance_update = {.row = VFFRLS_DIM, .col=VFFRLS_DIM, .entries=&t_covariance_update_default[0]};
static Matrix t_kalmanGain_update = {.row = VFFRLS_DIM, .col=IS_VECTOR, .entries=&t_kalmanGain_update_default[0]};
static Matrix d_theta_update = {.row = VFFRLS_DIM, .col=IS_VECTOR, .entries=&d_theta_update_default[0]};
void vffrls_init(const uint32_t voltage){

	int i;

	for (i = 0; i < VFFRLS_DIM * VFFRLS_DIM; i++)
		covariance_matrix.entries[i] = vffrls_covariance_default[i];

	for (i = 0; i < VFFRLS_DIM; i++)
		kalmanGain_matrix.entries[i] = vffrls_kalmanGain_default[i];

	for (i = 0; i < VFFRLS_DIM; i++)
		phi_matrix.entries[i] = vffrls_phi_default[i];

	for (i = 0; i < VFFRLS_DIM; i++)
		theta_matrix.entries[i] = vffrls_theta_default[i];

	estimateVoltage = ((float)voltage)/16000.0f;
	errorVoltage = 0;

	priorVoltage = ((float)voltage)/16000.0f;

	resistor0 = R0_INIT;
	resistor1 = R1_INIT;
	RC_parameter = C1_INIT * R1_INIT;
	battery_param.R0 = R0_INIT;
	battery_param.R1 = R1_INIT;
	battery_param.C1 = C1_INIT;

}

void vffrls_update(const uint32_t pack_voltage, const int32_t pack_current) {

	phi_matrix.entries[MATRIX_INDEX_4] = phi_matrix.entries[MATRIX_INDEX_3];
	phi_matrix.entries[MATRIX_INDEX_3] = ((float)pack_current)/PACK_CURRENT_NORMALIZED;
	phi_matrix.entries[MATRIX_INDEX_2] = priorVoltage;
	priorVoltage = ((float)pack_voltage)/PACK_VOLTAGE_NORMALIZED;

	estimateVoltage = inner_multiply(phi_matrix, theta_matrix);

	errorVoltage = ((float)pack_voltage)/PACK_VOLTAGE_NORMALIZED - estimateVoltage;

	eye(identity_matrix);
	multiply(kalmanGain_matrix, phi_matrix, m_covariance_update);
	minus(identity_matrix, m_covariance_update, identity_matrix);
	multiply(identity_matrix, covariance_matrix, covariance_matrix);

	uint8_t i;
	for (i = 0; i < VFFRLS_DIM; i++)
		t_kalmanGain_update.entries[i] = phi_matrix.entries[i];
	multiply(covariance_matrix, t_kalmanGain_update, kalmanGain_matrix);
	scalar_multiply(kalmanGain_matrix,
			1.0f / (LAMDA + inner_multiply(phi_matrix, kalmanGain_matrix)),
			kalmanGain_matrix);

	scalar_multiply(kalmanGain_matrix, errorVoltage, d_theta_update);
	sum(theta_matrix, d_theta_update, theta_matrix);

	estimateVoltage = theta_matrix.entries[MATRIX_INDEX_1]/(1.0f-theta_matrix.entries[MATRIX_INDEX_2]);
	resistor0 = (theta_matrix.entries[MATRIX_INDEX_4]
			- theta_matrix.entries[MATRIX_INDEX_3])
			/ (1.0f + theta_matrix.entries[MATRIX_INDEX_2]);
	resistor1 = 2.0f
			* (theta_matrix.entries[MATRIX_INDEX_4]
					+ theta_matrix.entries[MATRIX_INDEX_2]
							* theta_matrix.entries[MATRIX_INDEX_3])
			/ (theta_matrix.entries[MATRIX_INDEX_2]
					* theta_matrix.entries[MATRIX_INDEX_2] - 1.0f);
	RC_parameter = -(float)SAMPLE_TIME_s
			* (1.0f + theta_matrix.entries[MATRIX_INDEX_2])
			/ (2.0f * (-1.0f + theta_matrix.entries[MATRIX_INDEX_2]));


//	battery_param.R0 = resistor0;
//	battery_param.R1 = resistor1;
//	battery_param.C1 = RC_parameter/resistor1;
	battery_param.R0 = R0_INIT;
	battery_param.R1 = R1_INIT;
	battery_param.C1 = C1_INIT;

}
#endif
