/*
 * ffrls.c
 *
 *  Created on: Jul 1, 2022
 *      Author: ADMIN
 */

#if 0
#include "vffrls.h"
//#include "vffrls.init.h"

#define VFFRLS_RANK			4

#define PHI_MATRIX_UNIT_INDEX				0
#define PHI_MATRIX_UK_MINUS1_INDEX			1
#define PHI_MATRIX_IK_INDEX					2
#define PHI_MATRIX_IK_MINUS1_INDEX			3

#define THETA_MATRIX_A0_INDEX				0
#define THETA_MATRIX_A1_INDEX				1
#define THETA_MATRIX_A2_INDEX				2
#define THETA_MATRIX_A3_INDEX				3

const float lamda = 0.9992f;

#define COVARIANCE_INIT						(0.001f)

const float vffrls_covariance_default[VFFRLS_RANK * VFFRLS_RANK] = {
COVARIANCE_INIT, 0.0f, 0.0f, 0.0f, 0.0f, COVARIANCE_INIT, 0.0f, 0.0f, 0.0f,
		0.0f, COVARIANCE_INIT, 0.0f, 0.0f, 0.0f, 0.0f, COVARIANCE_INIT };
const float vffrls_theta_default[VFFRLS_RANK] = { 3.25f, 0.22f, -0.02f, -0.01f };
const float vffrls_kalmanGain_default[VFFRLS_RANK] =
		{ 0.001f, 0.0f, 0.0f, 0.0f };
const float vffrls_phi_default[VFFRLS_RANK] = { 1.0f, 0.0f, 0.0f, 0.0f };

static float covariance[VFFRLS_RANK * VFFRLS_RANK];
static float theta[VFFRLS_RANK];
static float kalmanGain[VFFRLS_RANK];
static float phi[VFFRLS_RANK];
static float identity_matrix_default[VFFRLS_RANK * VFFRLS_RANK];
static float m_covariance_update_default[VFFRLS_RANK * VFFRLS_RANK];
static float t_covariance_update_default[VFFRLS_RANK * VFFRLS_RANK];
static float t_kalmanGain_update_default[VFFRLS_RANK];
static float d_theta_update_default[VFFRLS_RANK];

static Matrix covariance_matrix;
static Matrix kalmanGain_matrix;
static Matrix phi_matrix;
static Matrix theta_matrix;

static Matrix identity_matrix;
static Matrix m_covariance_update;
static Matrix t_covariance_update;
static Matrix t_kalmanGain_update;
static Matrix d_theta_update;

static void vffrls_malloc(VFFRLS_State_Vector *p_state);
static void vffrls_phi_update(VFFRLS_State_Vector *p_state);
static void vffrls_estimate_voltage_update(VFFRLS_State_Vector *p_state);
static void vffrls_error_voltage_update(VFFRLS_State_Vector *p_state);
static void vffrls_covariance_update(VFFRLS_State_Vector *p_state);
static void vffrls_kalmanGain_update(VFFRLS_State_Vector *p_state);
static void vffrls_theta_update(VFFRLS_State_Vector *p_state);
static void vffrls_parameters_update(VFFRLS_State_Vector *p_state);

void vffrls_update_input(VFFRLS_State_Vector *p_state,
		const float terminalVoltage, const float current,
		const float hysteresis) {
	p_state->p_input->terminalVoltage = terminalVoltage;
	p_state->p_input->current = current;
	p_state->p_input->hysteresis = hysteresis; //0.0f;//
}

void vffrls_init(VFFRLS_State_Vector *p_state) {
	vffrls_malloc(p_state);
	vffrls_parameters_init(p_state);
}

void vffrls_update(VFFRLS_State_Vector *p_state) {
	vffrls_phi_update(p_state);
	vffrls_estimate_voltage_update(p_state);
	vffrls_error_voltage_update(p_state);
	vffrls_kalmanGain_update(p_state);
	vffrls_theta_update(p_state);
	vffrls_covariance_update(p_state);
	vffrls_parameters_update(p_state);
}

static void vffrls_phi_update(VFFRLS_State_Vector *p_state) {
	p_state->phi.entries[PHI_MATRIX_IK_MINUS1_INDEX] =
			p_state->phi.entries[PHI_MATRIX_IK_INDEX];
	p_state->phi.entries[PHI_MATRIX_IK_INDEX] = p_state->p_input->current;
	p_state->phi.entries[PHI_MATRIX_UK_MINUS1_INDEX] = p_state->priorVoltage;
	p_state->priorVoltage = p_state->p_input->terminalVoltage
			- p_state->p_input->hysteresis;
}

static void vffrls_estimate_voltage_update(VFFRLS_State_Vector *p_state) {
	p_state->estimateVoltage = inner_multiply(p_state->phi, p_state->theta);
}

static void vffrls_error_voltage_update(VFFRLS_State_Vector *p_state) {
	p_state->errorVoltage = p_state->p_input->terminalVoltage
			- p_state->estimateVoltage;
}

static void vffrls_covariance_update(VFFRLS_State_Vector *p_state) {

	p_state->covariance = multiply(
			minus(eye(identity_matrix),
					multiply(p_state->vffrls_kalmanGain, p_state->phi,
							m_covariance_update), identity_matrix),
			p_state->covariance, p_state->covariance);
//	p_state->covariance = chol(p_state->covariance, p_state->covariance);
//	t_covariance_update = transpose(p_state->covariance, t_covariance_update);
//	p_state->covariance = multiply(t_covariance_update, p_state->covariance,
//			p_state->covariance);
}

static void vffrls_kalmanGain_update(VFFRLS_State_Vector *p_state) {

	for (int i = 0; i < VFFRLS_RANK; i++)
		t_kalmanGain_update.entries[i] = p_state->phi.entries[i];
	p_state->vffrls_kalmanGain = multiply(p_state->covariance,
			t_kalmanGain_update, p_state->vffrls_kalmanGain);
	p_state->vffrls_kalmanGain = scalar_multiply(p_state->vffrls_kalmanGain,
			1.0f
					/ (lamda
							+ inner_multiply(p_state->phi,
									p_state->vffrls_kalmanGain)),
			p_state->vffrls_kalmanGain);
}

static void vffrls_theta_update(VFFRLS_State_Vector *p_state) {

	p_state->theta = sum(p_state->theta,
			scalar_multiply(p_state->vffrls_kalmanGain, p_state->errorVoltage,
					d_theta_update), p_state->theta);
}

static void vffrls_parameters_update(VFFRLS_State_Vector *p_state) {
	p_state->p_output->resistor0 =
			(p_state->theta.entries[THETA_MATRIX_A3_INDEX]
					- p_state->theta.entries[THETA_MATRIX_A2_INDEX])
					/ (1.0f + p_state->theta.entries[THETA_MATRIX_A1_INDEX]);
	p_state->p_output->resistor1 = 2.0f
			* (p_state->theta.entries[THETA_MATRIX_A3_INDEX]
					+ p_state->theta.entries[THETA_MATRIX_A1_INDEX]
							* p_state->theta.entries[THETA_MATRIX_A2_INDEX])
			/ (p_state->theta.entries[THETA_MATRIX_A1_INDEX]
					* p_state->theta.entries[THETA_MATRIX_A1_INDEX] - 1.0f);
	p_state->p_output->RC_parameter = -SAMPLE_TIME
			* (1.0f + p_state->theta.entries[THETA_MATRIX_A1_INDEX])
			/ (2.0f * (-1.0f + p_state->theta.entries[THETA_MATRIX_A1_INDEX]));
}

VFFRLS_Output_Vector* vffrls_get_output(VFFRLS_State_Vector *p_state) {
	return p_state->p_output;
}

static void vffrls_malloc(VFFRLS_State_Vector *p_state) {

	covariance_matrix.entries = &covariance[0];
	kalmanGain_matrix.entries = &theta[0];
	phi_matrix.entries = &kalmanGain[0];
	theta_matrix.entries = &phi[0];

	identity_matrix.entries = &identity_matrix_default[0];
	m_covariance_update.entries = &m_covariance_update_default[0];
	t_covariance_update.entries = &t_covariance_update_default[0];
	t_kalmanGain_update.entries = &t_kalmanGain_update_default[0];
	d_theta_update.entries = &d_theta_update_default[0];

	p_state->covariance = covariance_matrix;
	p_state->vffrls_kalmanGain = kalmanGain_matrix;
	p_state->phi = phi_matrix;
	p_state->theta = theta_matrix;
}

void vffrls_parameters_init(VFFRLS_State_Vector *p_state) {

	int i;

	p_state->covariance.row = VFFRLS_RANK;
	p_state->covariance.col = VFFRLS_RANK;
	for (i = 0; i < VFFRLS_RANK * VFFRLS_RANK; i++)
		p_state->covariance.entries[i] = vffrls_covariance_default[i];

	p_state->vffrls_kalmanGain.row = VFFRLS_RANK;
	p_state->vffrls_kalmanGain.col = 1;
	for (i = 0; i < VFFRLS_RANK; i++)
		p_state->vffrls_kalmanGain.entries[i] = vffrls_kalmanGain_default[i];

	p_state->phi.row = 1;
	p_state->phi.col = VFFRLS_RANK;
	for (i = 0; i < VFFRLS_RANK; i++)
		p_state->phi.entries[i] = vffrls_phi_default[i];
	p_state->phi.entries[2] = p_state->p_input->current;

	p_state->theta.row = VFFRLS_RANK;
	p_state->theta.col = 1;
	for (i = 0; i < VFFRLS_RANK; i++)
		p_state->theta.entries[i] = vffrls_theta_default[i];

	p_state->estimateVoltage = p_state->p_input->terminalVoltage;
	p_state->errorVoltage = 0;

	p_state->priorVoltage = p_state->p_input->terminalVoltage
			- p_state->p_input->hysteresis;

	p_state->p_output->resistor0 = R0_INIT;
	p_state->p_output->resistor1 = R1_INIT;
	p_state->p_output->RC_parameter = C1_INIT * R1_INIT;

	identity_matrix.row = VFFRLS_RANK;
	identity_matrix.col = VFFRLS_RANK;

	m_covariance_update.row = VFFRLS_RANK;
	m_covariance_update.col = VFFRLS_RANK;

	t_covariance_update.row = VFFRLS_RANK;
	t_covariance_update.col = VFFRLS_RANK;

	t_kalmanGain_update.row = VFFRLS_RANK;
	t_kalmanGain_update.col = 1;

	d_theta_update.row = VFFRLS_RANK;
	d_theta_update.col = 1;
}
#endif
