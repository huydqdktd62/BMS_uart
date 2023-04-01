/*
 * aukf.c
 *
 *  Created on: Jul 1, 2022
 *      Author: ADMIN
 */

#if 0
#include "aukf.h"
//#include "aukf.init.h"

#define STATE_DIMENSION					3
#define SIGMA_FACTOR					(STATE_DIMENSION*2+1)
#define MEASUREMENT_DIMENSION			1
#define DYNAMIC_DIMENSION				2
#define DEFAULT_MEASUREMENT_COVARIANCE		(0.4472f)

static float eta;

const float m_weight[SIGMA_FACTOR] = { 	-2999.0f,
									500.0f,
									500.0f,
									500.0f,
									500.0f,
									500.0f,
									500.0f };
const float c_weight[SIGMA_FACTOR] = { 	-2999.0f - 3.0f - 0.0002f,
									500.0f,
									500.0f,
									500.0f,
									500.0f,
									500.0f,
									500.0f };

const float default_state_covariance[STATE_DIMENSION * STATE_DIMENSION] = {
		0.04f, 0.0f, 0.0f,
		0.0f, 0.001f, 0.0f,
		0.0f, 0.0f, 1.0f
};

const float default_systemCovariance[STATE_DIMENSION * STATE_DIMENSION] = {
		0.04f, 0.0f, 0.0f,
		0.0f, 0.01f, 0.0f,
		0.0f, 0.0f, 0.1f
};

static float estimateState[STATE_DIMENSION];
static float stateCovariance[STATE_DIMENSION*STATE_DIMENSION];
static float sigmaPoints[STATE_DIMENSION*SIGMA_FACTOR];
static float prioriEstimateState[STATE_DIMENSION];
static float matrixA[STATE_DIMENSION*STATE_DIMENSION];
static float matrixB[STATE_DIMENSION*DYNAMIC_DIMENSION];
static float matrixC[MEASUREMENT_DIMENSION*STATE_DIMENSION];
static float matrixD[MEASUREMENT_DIMENSION*DYNAMIC_DIMENSION];

static float sigmaStateError[STATE_DIMENSION*SIGMA_FACTOR];
static float observedMeasurement[DYNAMIC_DIMENSION];
static float sigmaMeasurements[MEASUREMENT_DIMENSION*SIGMA_FACTOR];
static float sigmaMeasureError[MEASUREMENT_DIMENSION*SIGMA_FACTOR];
static float crossCovariance[STATE_DIMENSION*MEASUREMENT_DIMENSION];
static float aukf_kalmanGain[STATE_DIMENSION];

static float g1_create_sigma_point_default[STATE_DIMENSION*STATE_DIMENSION];
static float g2_create_sigma_point_default[STATE_DIMENSION*STATE_DIMENSION];
static float m_update_sigma_state_default[STATE_DIMENSION];
static float g_update_sigma_state_default[STATE_DIMENSION*SIGMA_FACTOR];
static float g_update_predict_state_default[STATE_DIMENSION];
static float m_weight_matrix_default[SIGMA_FACTOR];
static float m1_time_update_state_covariance_default[STATE_DIMENSION];
static float m2_time_update_state_covariance_default[STATE_DIMENSION];
static float m3_time_update_state_covariance_default[STATE_DIMENSION*STATE_DIMENSION];
static float g_update_sigma_measurement_default[MEASUREMENT_DIMENSION*SIGMA_FACTOR];
static float g_update_measurement_covariance_default[MEASUREMENT_DIMENSION*SIGMA_FACTOR];
static float m1_update_cross_covariance_default[STATE_DIMENSION];
static float m_update_state_default[STATE_DIMENSION];
static float t_update_state_covariance_default[STATE_DIMENSION];
static float m_update_state_covariance_default[STATE_DIMENSION*STATE_DIMENSION];

static Matrix estimateState_matrix;
static Matrix stateCovariance_matrix;
static Matrix sigmaPoints_matrix;
static Matrix prioriEstimateState_matrix;
static Matrix matrixA_matrix;
static Matrix matrixB_matrix;
static Matrix matrixC_matrix;
static Matrix matrixD_matrix;
static Matrix sigmaStateError_matrix;
static Matrix observedMeasurement_matrix;
static Matrix sigmaMeasurements_matrix;
static Matrix sigmaMeasureError_matrix;
static Matrix crossCovariance_matrix;
static Matrix kalmanGain_matrix;

static Matrix g1_create_sigma_point;
static Matrix g2_create_sigma_point;
static Matrix m_update_sigma_state;
static Matrix g_update_sigma_state;
static Matrix g_update_predict_state;
static Matrix m_weight_matrix;
static Matrix m1_time_update_state_covariance;
static Matrix m2_time_update_state_covariance;
static Matrix m3_time_update_state_covariance;
static Matrix g_update_sigma_measurement;
static Matrix g_update_measurement_covariance;
static Matrix m1_update_cross_covariance;
static Matrix m_update_state;
static Matrix t_update_state_covariance;
static Matrix m_update_state_covariance;

static void aukf_malloc(AUKF_State_Vector *p_state);
static void aukf_estimate_state_bounder(AUKF_State_Vector *p_state);
static void aukf_create_sigma_points(AUKF_State_Vector *p_state, Matrix index);
static void aukf_time_update_sigma_state(AUKF_State_Vector *p_state);
static void aukf_time_update_predict_state(AUKF_State_Vector *p_state);
static void aukf_time_update_sigmaStateError(AUKF_State_Vector *p_state);
static void aukf_time_update_state_covariance(AUKF_State_Vector *p_state);
static void aukf_update_system(AUKF_State_Vector *p_state);
static void aukf_time_update_sigma_measurements(AUKF_State_Vector *p_state);
static void aukf_time_update_measurement(AUKF_State_Vector *p_state);
static void aukf_update_sigma_measurement_error(AUKF_State_Vector *p_state);
static void aukf_update_measurement_covariance(AUKF_State_Vector *p_state);
static void aukf_update_cross_covariance(AUKF_State_Vector *p_state);
static void aukf_update_kalman_gain(AUKF_State_Vector *p_state);
static void aukf_update_state(AUKF_State_Vector *p_state);
static void aukf_update_state_covariance(AUKF_State_Vector *p_state);
static void aukf_update_system_parameters(AUKF_State_Vector *p_state);
static void aukf_update_matrix_a(AUKF_State_Vector *p_state);
static void aukf_update_matrix_b(AUKF_State_Vector *p_state);
static void aukf_update_matrix_c(AUKF_State_Vector *p_state);
static void aukf_update_matrix_d(AUKF_State_Vector *p_state);
static void aukf_update_output(AUKF_State_Vector *p_state);

void aukf_update_input(AUKF_State_Vector *p_state,
		const VFFRLS_Output_Vector *p_output, const float terminalVoltage,
		const float current, const float soh) {
	p_state->p_input->terminalVoltage = terminalVoltage;
	p_state->p_input->current = current;
	p_state->p_input->soh = soh;
	p_state->p_input->resistor0 = p_output->resistor0;
	p_state->p_input->resistor1 = p_output->resistor1;
	p_state->p_input->RC_parameter = p_output->RC_parameter;
}

void aukf_init(AUKF_State_Vector *p_state) {
	aukf_malloc(p_state);
	aukf_parameters_init(p_state);
}

void aukf_update(AUKF_State_Vector *p_state) {
	aukf_create_sigma_points(p_state, p_state->estimateState);
	aukf_time_update_sigma_state(p_state);
	aukf_time_update_predict_state(p_state);
	aukf_time_update_sigmaStateError(p_state);
	aukf_time_update_state_covariance(p_state);
	aukf_create_sigma_points(p_state, p_state->prioriEstimateState);
	aukf_update_system(p_state);
	aukf_time_update_sigma_measurements(p_state);
	aukf_time_update_measurement(p_state);
	aukf_update_sigma_measurement_error(p_state);
	aukf_update_measurement_covariance(p_state);
	aukf_update_cross_covariance(p_state);
	aukf_update_kalman_gain(p_state);
	aukf_update_state(p_state);
	aukf_estimate_state_bounder(p_state);
	aukf_update_state_covariance(p_state);
	aukf_update_output(p_state);
}

static void aukf_create_sigma_points(AUKF_State_Vector *p_state, Matrix index) {

	g1_create_sigma_point = hgenerate(index, STATE_DIMENSION,
			g1_create_sigma_point);
	g2_create_sigma_point = chol(p_state->stateCovariance,
			g2_create_sigma_point);
	g2_create_sigma_point = scalar_multiply(g2_create_sigma_point, GAMMA,
			g2_create_sigma_point);

	g1_create_sigma_point = sum(g1_create_sigma_point, g2_create_sigma_point,
			g1_create_sigma_point);
	g2_create_sigma_point = scalar_multiply(g2_create_sigma_point, 2,
			g2_create_sigma_point);
	g2_create_sigma_point = minus(g1_create_sigma_point, g2_create_sigma_point,
			g2_create_sigma_point);
	p_state->sigmaPoints = htri_concat(index,
			g1_create_sigma_point, g2_create_sigma_point, p_state->sigmaPoints);
}

static void aukf_time_update_sigma_state(AUKF_State_Vector *p_state) {

	m_update_sigma_state = multiply(p_state->matrixB,
			p_state->observedMeasurement, m_update_sigma_state);

	g_update_sigma_state = hgenerate(m_update_sigma_state, SIGMA_FACTOR,
			g_update_sigma_state);

	p_state->sigmaPoints = multiply(p_state->matrixA, p_state->sigmaPoints,
			p_state->sigmaPoints);
	p_state->sigmaPoints = sum(p_state->sigmaPoints, g_update_sigma_state,
			p_state->sigmaPoints);
}



static void aukf_time_update_predict_state(AUKF_State_Vector *p_state) {
	int i;
	for (i = 0; i < STATE_DIMENSION; i++) {
		p_state->prioriEstimateState.entries[i] = 0.0f;
	}
	for (i = 0; i < SIGMA_FACTOR; i++) {
		p_state->prioriEstimateState.entries[0] += m_weight[i]
				* p_state->sigmaPoints.entries[i];
	}
	for (i = 0; i < SIGMA_FACTOR; i++) {
		p_state->prioriEstimateState.entries[1] += m_weight[i]
				* p_state->sigmaPoints.entries[SIGMA_FACTOR + i];
	}
	for (i = 0; i < SIGMA_FACTOR; i++) {
		p_state->prioriEstimateState.entries[2] += m_weight[i]
				* p_state->sigmaPoints.entries[2 * SIGMA_FACTOR + i];
	}

//	int i, j;
//	for (i = 0; i < STATE_DIMENSION; i++) {
//		p_state->prioriEstimateState.entries[i] = 0.0f;
//	}
//	for (i = 0; i < SIGMA_FACTOR; i++) {
//		for (j = 0; j < STATE_DIMENSION; j++) {
//			g_update_predict_state.entries[j] = p_state->sigmaPoints.entries[j
//					* SIGMA_FACTOR + i];
//		}
//		g_update_predict_state = scalar_multiply(g_update_predict_state,
//				m_weight[i], g_update_predict_state);
//		p_state->prioriEstimateState = sum(p_state->prioriEstimateState,
//				g_update_predict_state, p_state->prioriEstimateState);
//	}

//	p_state->prioriEstimateState = multiply(p_state->sigmaPoints, m_weight_matrix,
//			p_state->prioriEstimateState);
}

static void aukf_time_update_sigmaStateError(AUKF_State_Vector *p_state) {
	p_state->sigmaStateError = hgenerate(p_state->prioriEstimateState, SIGMA_FACTOR,
			p_state->sigmaStateError);
	p_state->sigmaStateError = minus(p_state->sigmaPoints,
			p_state->sigmaStateError, p_state->sigmaStateError);
}

static void aukf_time_update_state_covariance(AUKF_State_Vector *p_state) {

	for (int i = 0; i < STATE_DIMENSION * STATE_DIMENSION; i++)
		p_state->stateCovariance.entries[i] = default_systemCovariance[i];
	for (int i = 0; i < SIGMA_FACTOR; i++) {
		for (int j = 0; j < STATE_DIMENSION; j++) {
			m1_time_update_state_covariance.entries[j] =
					p_state->sigmaStateError.entries[j * SIGMA_FACTOR + i];
			m2_time_update_state_covariance.entries[j] =
					p_state->sigmaStateError.entries[j * SIGMA_FACTOR + i];
		}
		m3_time_update_state_covariance = multiply(
				m1_time_update_state_covariance,
				m2_time_update_state_covariance,
				m3_time_update_state_covariance);
		m3_time_update_state_covariance = scalar_multiply(
				m3_time_update_state_covariance, c_weight[i],
				m3_time_update_state_covariance);
		p_state->stateCovariance = sum(p_state->stateCovariance,
				m3_time_update_state_covariance, p_state->stateCovariance);
	}
}

static void aukf_time_update_sigma_measurements(AUKF_State_Vector *p_state) {
	p_state->sigmaMeasurements = multiply(p_state->matrixC,
			p_state->sigmaPoints, p_state->sigmaMeasurements);
	float deltaMeasurement;
	deltaMeasurement = inner_multiply(p_state->matrixD,
			p_state->observedMeasurement);

	for (int i = 0; i < SIGMA_FACTOR; i++)
		g_update_sigma_measurement.entries[i] = deltaMeasurement;

	p_state->sigmaMeasurements = sum(p_state->sigmaMeasurements,
			g_update_sigma_measurement, p_state->sigmaMeasurements);
}

static void aukf_time_update_measurement(AUKF_State_Vector *p_state) {

	p_state->estimateMeasurement = inner_multiply(p_state->sigmaMeasurements,
			m_weight_matrix);
}

static void aukf_update_sigma_measurement_error(AUKF_State_Vector *p_state) {

	for (int i = 0; i < SIGMA_FACTOR; i++)
		g_update_measurement_covariance.entries[i] =
				p_state->estimateMeasurement;

	p_state->sigmaMeasureError = minus(p_state->sigmaMeasurements,
			g_update_measurement_covariance, p_state->sigmaMeasureError);
}

static void aukf_update_measurement_covariance(AUKF_State_Vector *p_state) {

	p_state->measurementCovariance = DEFAULT_MEASUREMENT_COVARIANCE;
	for (int i = 0; i < SIGMA_FACTOR; i++)
		p_state->measurementCovariance += c_weight[i]
				* p_state->sigmaMeasureError.entries[i]
				* p_state->sigmaMeasureError.entries[i];

}

static void aukf_update_cross_covariance(AUKF_State_Vector *p_state) {

	for (int i = 0; i < STATE_DIMENSION; i++)
		p_state->crossCovariance.entries[i] = 0;

	for (int i = 0; i < SIGMA_FACTOR; i++) {
		for (int j = 0; j < STATE_DIMENSION; j++)
			m1_update_cross_covariance.entries[j] =
					p_state->sigmaStateError.entries[j * SIGMA_FACTOR + i];
		m1_update_cross_covariance = scalar_multiply(m1_update_cross_covariance,
				c_weight[i] * p_state->sigmaMeasureError.entries[i],
				m1_update_cross_covariance);
		p_state->crossCovariance = sum(p_state->crossCovariance,
				m1_update_cross_covariance, p_state->crossCovariance);
	}
}

static void aukf_update_kalman_gain(AUKF_State_Vector *p_state) {
	p_state->aukf_kalmanGain = scalar_multiply(p_state->crossCovariance,
			(1.0f / p_state->measurementCovariance), p_state->aukf_kalmanGain);
}

static void aukf_update_state(AUKF_State_Vector *p_state) {

	m_update_state = scalar_multiply(p_state->aukf_kalmanGain,
			p_state->p_input->terminalVoltage - p_state->estimateMeasurement,
			m_update_state);
	p_state->estimateState = sum(p_state->prioriEstimateState, m_update_state,
			p_state->estimateState);
}

static void aukf_estimate_state_bounder(AUKF_State_Vector *p_state) {
	if (p_state->estimateState.entries[0] >= 100.0f) {
		p_state->estimateState.entries[0] = 100.0f;
	}
	if (p_state->estimateState.entries[0] <= 0.2811f) {//here 0.0f
		p_state->estimateState.entries[0] = 0.2811f;
	}
	if (p_state->estimateState.entries[2] >= 1.0f) {
		p_state->estimateState.entries[2] = 1.0f;
	}
	if (p_state->estimateState.entries[2] <= -1.0f) {
		p_state->estimateState.entries[2] = -1.0f;
	}
}

static void aukf_update_state_covariance(AUKF_State_Vector *p_state) {

	for (int i = 0; i < STATE_DIMENSION; i++)
		t_update_state_covariance.entries[i] =
				p_state->aukf_kalmanGain.entries[i];

	m_update_state_covariance = multiply(p_state->aukf_kalmanGain,
			t_update_state_covariance, m_update_state_covariance);
	m_update_state_covariance = scalar_multiply(m_update_state_covariance,
			p_state->measurementCovariance, m_update_state_covariance);
	p_state->stateCovariance = minus(p_state->stateCovariance,
			m_update_state_covariance, p_state->stateCovariance);
}

static void aukf_update_system(AUKF_State_Vector *p_state) {
	aukf_update_system_parameters(p_state);
	aukf_update_matrix_a(p_state);
	aukf_update_matrix_b(p_state);
	aukf_update_matrix_c(p_state);
	aukf_update_matrix_d(p_state);
}

static void aukf_update_system_parameters(AUKF_State_Vector *p_state){
	p_state->observedMeasurement.entries[0] = p_state->p_input->current;
	p_state->observedMeasurement.entries[1] = (float) sign(
			p_state->p_input->current);
	if(absolute(p_state->p_input->current)<0.004f){//here 0.1f
		p_state->observedMeasurement.entries[1]=0.0f;
	}
	eta=ETA;
	if(p_state->p_input->current<0.0f){
		eta=1.0f;
	}
	p_state->RCParameter = exponent(
			-absolute(SAMPLE_TIME
					/ (p_state->p_input->RC_parameter)));
	p_state->HParameter = exponent(
			-(absolute(
					eta * CAPACITY_RATIO * SAMPLE_TIME
							* p_state->p_input->current / (p_state->p_input->soh*NOMIMAL_CAPACITY))));
}

static void aukf_update_matrix_a(AUKF_State_Vector *p_state) {
	p_state->matrixA.entries[4] = p_state->RCParameter;
	p_state->matrixA.entries[8] = p_state->HParameter;
}

static void aukf_update_matrix_b(AUKF_State_Vector *p_state) {
	p_state->matrixB.entries[0] = (-eta*SAMPLE_TIME*100.0f)/(p_state->p_input->soh*NOMIMAL_CAPACITY);
	p_state->matrixB.entries[2] = 1.0f - p_state->RCParameter;
	p_state->matrixB.entries[5] = p_state->HParameter - 1.0f;
}

static void aukf_update_matrix_c(AUKF_State_Vector *p_state) {
	float d=0.0f;
	for(int i=0;i<SIGMA_FACTOR;i++){
		d+=get_ratio_from_soc(p_state->sigmaPoints.entries[i]);
	}
	p_state->matrixC.entries[0] = d/SIGMA_FACTOR;
	p_state->matrixC.entries[1] = -p_state->p_input->resistor1;
}

static void aukf_update_matrix_d(AUKF_State_Vector *p_state) {
	p_state->matrixD.entries[0] = -p_state->p_input->resistor0;
}

static void aukf_update_output(AUKF_State_Vector *p_state) {
//	p_state->p_output->OCV = get_ocv_from_soc(
//			p_state->estimateState.entries[0]);

	p_state->p_output->OCV = get_ratio_from_soc(
			p_state->estimateState.entries[0])*p_state->estimateState.entries[0]
			-p_state->estimateState.entries[1]*p_state->p_input->resistor1
			-p_state->p_input->current*p_state->p_input->resistor0
			+ HYSTERESIS * p_state->estimateState.entries[2]
			+ HYSTERESIS_ZERO * p_state->observedMeasurement.entries[1];

	p_state->p_output->SOC = (uint32_t) roundf(p_state->estimateState.entries[0]);
	if(p_state->estimateState.entries[0]>=100.0f){
		p_state->p_output->SOC=100;
	}
	if(p_state->estimateState.entries[0]<=0.0f){
		p_state->p_output->SOC=0;
	}
	p_state->p_output->hysteresisVoltage = p_state->estimateState.entries[2]
			* HYSTERESIS + HYSTERESIS_ZERO * p_state->observedMeasurement.entries[1];
}

AUKF_Output_Vector* aukf_get_output(AUKF_State_Vector *p_state) {
	return p_state->p_output;
}

void aukf_malloc(AUKF_State_Vector *p_state){

	estimateState_matrix.entries = &estimateState[0];
	stateCovariance_matrix.entries = &stateCovariance[0];
	sigmaPoints_matrix.entries = &sigmaPoints[0];
	prioriEstimateState_matrix.entries = &prioriEstimateState[0];
	matrixA_matrix.entries = &matrixA[0];
	matrixB_matrix.entries = &matrixB[0];
	matrixC_matrix.entries = &matrixC[0];
	matrixD_matrix.entries = &matrixD[0];
	sigmaStateError_matrix.entries = &sigmaStateError[0];
	observedMeasurement_matrix.entries = &observedMeasurement[0];
	sigmaMeasurements_matrix.entries = &sigmaMeasurements[0];
	sigmaMeasureError_matrix.entries = &sigmaMeasureError[0];
	crossCovariance_matrix.entries = &crossCovariance[0];
	kalmanGain_matrix.entries = &aukf_kalmanGain[0];

	g1_create_sigma_point.entries = &g1_create_sigma_point_default[0];
	g2_create_sigma_point.entries = &g2_create_sigma_point_default[0];
	m_update_sigma_state.entries = &m_update_sigma_state_default[0];
	g_update_sigma_state.entries = &g_update_sigma_state_default[0];
	g_update_predict_state.entries = &g_update_predict_state_default[0];
	m_weight_matrix.entries = &m_weight_matrix_default[0];
	m1_time_update_state_covariance.entries = &m1_time_update_state_covariance_default[0];
	m2_time_update_state_covariance.entries = &m2_time_update_state_covariance_default[0];
	m3_time_update_state_covariance.entries = &m3_time_update_state_covariance_default[0];
	g_update_sigma_measurement.entries = &g_update_sigma_measurement_default[0];
	g_update_measurement_covariance.entries = &g_update_measurement_covariance_default[0];
	m1_update_cross_covariance.entries = &m1_update_cross_covariance_default[0];
	m_update_state.entries = &m_update_state_default[0];
	t_update_state_covariance.entries = &t_update_state_covariance_default[0];
	m_update_state_covariance.entries = &m_update_state_covariance_default[0];

	p_state->estimateState = estimateState_matrix;
	p_state->stateCovariance = stateCovariance_matrix;
	p_state->sigmaPoints = sigmaPoints_matrix;
	p_state->prioriEstimateState = prioriEstimateState_matrix;
	p_state->matrixA = matrixA_matrix;
	p_state->matrixB = matrixB_matrix;
	p_state->matrixC = matrixC_matrix;
	p_state->matrixD = matrixD_matrix;
	p_state->sigmaStateError = sigmaStateError_matrix;
	p_state->observedMeasurement = observedMeasurement_matrix;
	p_state->sigmaMeasurements = sigmaMeasurements_matrix;
	p_state->sigmaMeasureError = sigmaMeasureError_matrix;
	p_state->crossCovariance = crossCovariance_matrix;
	p_state->aukf_kalmanGain = kalmanGain_matrix;

}

void aukf_parameters_init(AUKF_State_Vector *p_state) {
	int i;
	p_state->estimateState.row = STATE_DIMENSION;
	p_state->estimateState.col = 1;
	p_state->estimateState.entries[0] = get_soc_from_ocv(
			p_state->p_input->terminalVoltage);
	p_state->estimateState.entries[1] = 0.0f;
	p_state->estimateState.entries[2] = 0.0f;

	p_state->stateCovariance.row = STATE_DIMENSION;
	p_state->stateCovariance.col = STATE_DIMENSION;
	for (i = 0; i < STATE_DIMENSION * STATE_DIMENSION; i++)
		p_state->stateCovariance.entries[i] = default_state_covariance[i];

	p_state->sigmaPoints.row = STATE_DIMENSION;
	p_state->sigmaPoints.col = SIGMA_FACTOR;

	p_state->prioriEstimateState.row = STATE_DIMENSION;
	p_state->prioriEstimateState.col = 1;

	p_state->sigmaStateError.row = STATE_DIMENSION;
	p_state->sigmaStateError.col = SIGMA_FACTOR;

	p_state->matrixA.row = STATE_DIMENSION;
	p_state->matrixA.col = STATE_DIMENSION;
	for (i = 0; i < STATE_DIMENSION * STATE_DIMENSION; i++)
		p_state->matrixA.entries[i] = 0.0f;
	p_state->matrixA.entries[0] = 1.0f;
	p_state->matrixA.entries[4] = exponent(-SAMPLE_TIME / (R1_INIT * C1_INIT));
	p_state->matrixA.entries[8] = 1.0f;

	p_state->matrixB.row = STATE_DIMENSION;
	p_state->matrixB.col = DYNAMIC_DIMENSION;
	for (i = 0; i < STATE_DIMENSION * DYNAMIC_DIMENSION; i++)
		p_state->matrixB.entries[i] = 0.0f;
	p_state->matrixB.entries[0] = (-ETA * SAMPLE_TIME / NOMIMAL_CAPACITY);
	p_state->matrixB.entries[2] = 1.0f
			- exponent(-SAMPLE_TIME / (R1_INIT * C1_INIT));

	p_state->matrixC.row = MEASUREMENT_DIMENSION;
	p_state->matrixC.col = STATE_DIMENSION;
	for (i = 0; i < MEASUREMENT_DIMENSION * STATE_DIMENSION; i++)
		p_state->matrixC.entries[i] = 0.0f;
	p_state->matrixC.entries[STATE_DIMENSION - 1] = HYSTERESIS;//here

	p_state->matrixD.row = MEASUREMENT_DIMENSION;
	p_state->matrixD.col = DYNAMIC_DIMENSION;
	for (i = 0; i < MEASUREMENT_DIMENSION * DYNAMIC_DIMENSION; i++)
		p_state->matrixD.entries[i] = 0.0f;
	p_state->matrixD.entries[1] = HYSTERESIS_ZERO;//here

	p_state->observedMeasurement.row = DYNAMIC_DIMENSION;
	p_state->observedMeasurement.col = 1;
	p_state->observedMeasurement.entries[0] = p_state->p_input->current;
	p_state->observedMeasurement.entries[1] = 0.0f;

	p_state->sigmaMeasurements.row = MEASUREMENT_DIMENSION;
	p_state->sigmaMeasurements.col = SIGMA_FACTOR;

	p_state->estimateMeasurement = p_state->p_input->terminalVoltage;

	p_state->sigmaMeasureError.row = MEASUREMENT_DIMENSION;
	p_state->sigmaMeasureError.col = SIGMA_FACTOR;

	p_state->crossCovariance.row = STATE_DIMENSION;
	p_state->crossCovariance.col = MEASUREMENT_DIMENSION;

	p_state->aukf_kalmanGain.row = STATE_DIMENSION;
	p_state->aukf_kalmanGain.col = 1;

	p_state->p_output->OCV = p_state->p_input->terminalVoltage;
	p_state->p_output->SOC = (uint32_t) roundf(p_state->estimateState.entries[0]);
	p_state->p_output->hysteresisVoltage = 0.0f;

	m_weight_matrix.row = SIGMA_FACTOR;
	m_weight_matrix.col = 1;
	for (i = 0; i < SIGMA_FACTOR; i++)
		m_weight_matrix.entries[i] = m_weight[i];

	g1_create_sigma_point.row = STATE_DIMENSION;
	g1_create_sigma_point.col = STATE_DIMENSION;

	g2_create_sigma_point.row = STATE_DIMENSION;
	g2_create_sigma_point.col = STATE_DIMENSION;

	m_update_sigma_state.row = STATE_DIMENSION;
	m_update_sigma_state.col = 1;

	g_update_sigma_state.row = STATE_DIMENSION;
	g_update_sigma_state.col = SIGMA_FACTOR;

	g_update_predict_state.row = STATE_DIMENSION;
	g_update_predict_state.col = 1;

	m1_time_update_state_covariance.row = STATE_DIMENSION;
	m1_time_update_state_covariance.col = 1;

	m2_time_update_state_covariance.row = 1;
	m2_time_update_state_covariance.col = STATE_DIMENSION;

	m3_time_update_state_covariance.row = STATE_DIMENSION;
	m3_time_update_state_covariance.col = STATE_DIMENSION;

	g_update_sigma_measurement.row = MEASUREMENT_DIMENSION;
	g_update_sigma_measurement.col = SIGMA_FACTOR;

	g_update_measurement_covariance.row = MEASUREMENT_DIMENSION;
	g_update_measurement_covariance.col = SIGMA_FACTOR;

	m1_update_cross_covariance.row = STATE_DIMENSION;
	m1_update_cross_covariance.col = 1;

	m_update_state.row = STATE_DIMENSION;
	m_update_state.col = 1;

	t_update_state_covariance.row = 1;
	t_update_state_covariance.col = STATE_DIMENSION;

	m_update_state_covariance.row = STATE_DIMENSION;
	m_update_state_covariance.col = STATE_DIMENSION;

}
#endif
