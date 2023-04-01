/*
 * soc_ukf.c
 *
 *  Created on: Feb 15, 2023
 *      Author: ADMIN
 */
/* USER CODE END Header */

/* USER CODE BEGIN Includes */
#include "math_util.h"
#include "matrix.h"
#include "soc_ukf.h"
#include "battery_model.h"
#include "soc_vffrls.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
//SOC_UKF bms_soc;
SOC_Input soc_input;
SOC_Output soc_output;

static float eta;

static float m_weight[SIGMA_FACTOR] = { 	-599.0f,
									100.0f,
									100.0f,
									100.0f,
									100.0f,
									100.0f,
									100.0f };

const Matrix m_weight_matrix = { .row = SIGMA_FACTOR, .col = 1, .entries =
		&m_weight[0] };

static const float c_weight[SIGMA_FACTOR] = { 	-599.0f - 3.0f - 0.0002f,
									100.0f,
									100.0f,
									100.0f,
									100.0f,
									100.0f,
									100.0f };

static const float default_state_covariance[STATE_DIM * STATE_DIM] = {
		0.001f, 0.0f, 0.0f,
		0.0f, 0.0001f, 0.0f,
		0.0f, 0.0f, 0.0141f
};

static const float default_system_covariance[STATE_DIM * STATE_DIM] = {
		0.000001f, 0.0f, 0.0f,
		0.0f, 0.00000001f, 0.0f,
		0.0f, 0.0f, 0.0002f
};

float estimate_state_entries[STATE_DIM * 1] = {};
static Matrix est_state = { .row = STATE_DIM, .col = 1, .entries =
		&estimate_state_entries[0] };

float state_covariance_entries[STATE_DIM * STATE_DIM] = {};
static Matrix state_cov = { .row = STATE_DIM, .col = STATE_DIM, .entries =
		&state_covariance_entries[0] };

float sigma_points_entries[STATE_DIM * SIGMA_FACTOR] = {};
static Matrix sigma_points = { .row = STATE_DIM, .col = SIGMA_FACTOR, .entries =
		&sigma_points_entries[0] };

float priori_estimate_state_entries[STATE_DIM * 1] = {};
static Matrix priori_est_state = { .row = STATE_DIM, .col = 1, .entries =
		&priori_estimate_state_entries[0] };

float matrix_A_entries[STATE_DIM * STATE_DIM] = {};
static Matrix matrix_A = { .row = STATE_DIM, .col = STATE_DIM, .entries =
		&matrix_A_entries[0] };

float matrix_B_entries[STATE_DIM * DYNAMIC_DIM] = {};
static Matrix matrix_B = { .row = STATE_DIM, .col = DYNAMIC_DIM, .entries =
		&matrix_B_entries[0] };

float matrix_C_entries[MEASUREMENT_DIM * STATE_DIM] = {};
static Matrix matrix_C = { .row = MEASUREMENT_DIM, .col = STATE_DIM, .entries =
		&matrix_C_entries[0] };

float matrix_D_entries[MEASUREMENT_DIM * DYNAMIC_DIM] = {};
static Matrix matrix_D = { .row = MEASUREMENT_DIM, .col = DYNAMIC_DIM, .entries =
		&matrix_D_entries[0] };

float sigma_state_error_entries[STATE_DIM * SIGMA_FACTOR] = {};
static Matrix sigma_state_err = { .row = STATE_DIM, .col = SIGMA_FACTOR, .entries =
		&sigma_state_error_entries[0] };

float observed_measurement_entries[DYNAMIC_DIM * 1] = {};
static Matrix observed_measurement = { .row = DYNAMIC_DIM, .col = 1, .entries =
		&observed_measurement_entries[0] };

float sigma_measurements_entries[MEASUREMENT_DIM * SIGMA_FACTOR] = {};
static Matrix sigma_measurements = { .row = MEASUREMENT_DIM, .col = SIGMA_FACTOR, .entries =
		&sigma_measurements_entries[0] };

float sigma_measurement_error_entries[MEASUREMENT_DIM * SIGMA_FACTOR] = {};
static Matrix sigma_measurement_err = { .row = MEASUREMENT_DIM, .col = SIGMA_FACTOR, .entries =
		&sigma_measurement_error_entries[0] };

float cross_covariance_entries[STATE_DIM * MEASUREMENT_DIM] = {};
static Matrix cross_cov = { .row = STATE_DIM, .col = MEASUREMENT_DIM, .entries =
		&cross_covariance_entries[0] };

float aukf_kalman_gain_entries[STATE_DIM * 1] = {};
static Matrix kalman_gain = { .row = STATE_DIM, .col = 1, .entries =
		&aukf_kalman_gain_entries[0] };

float est_measurement;
float measurement_cov;
float RC_param;
float H_param;
float cell_voltage, cell_current;

static float g1_create_sigma_point_entries[STATE_DIM * STATE_DIM] = {};
static Matrix g1_create_sigma_point = { .row = STATE_DIM, .col = STATE_DIM, .entries =
		&g1_create_sigma_point_entries[0] };

static float g2_create_sigma_point_entries[STATE_DIM * STATE_DIM] = {};
static Matrix g2_create_sigma_point = { .row = STATE_DIM, .col = STATE_DIM, .entries =
		&g2_create_sigma_point_entries[0] };

static float m_update_sigma_state_entries[STATE_DIM * 1] = {};
static Matrix m_update_sigma_state = { .row = STATE_DIM, .col = 1, .entries =
		&m_update_sigma_state_entries[0] };

static float g_update_sigma_state_entries[STATE_DIM * SIGMA_FACTOR] = {};
static Matrix g_update_sigma_state = { .row = STATE_DIM, .col = SIGMA_FACTOR, .entries =
		&g_update_sigma_state_entries[0] };

//static float g_update_predict_state_entries[STATE_DIM * 1] = {};
//static Matrix g_update_predict_state = { .row = STATE_DIM, .col = 1, .entries =
//		&g_update_predict_state_entries[0] };




static float m1_time_update_state_covariance_entries[STATE_DIM * 1] = {};
static Matrix m1_update_state_cov = { .row = STATE_DIM, .col = 1, .entries =
		&m1_time_update_state_covariance_entries[0] };

static float m2_time_update_state_covariance_entries[1 * STATE_DIM] = {};
static Matrix m2_update_state_cov = { .row = 1, .col = STATE_DIM, .entries =
		&m2_time_update_state_covariance_entries[0] };

static float m3_time_update_state_covariance_entries[STATE_DIM * STATE_DIM] = {};
static Matrix m3_update_state_cov = { .row = STATE_DIM, .col = STATE_DIM, .entries =
		&m3_time_update_state_covariance_entries[0] };

static float g_update_sigma_measurement_entries[MEASUREMENT_DIM * SIGMA_FACTOR] = {};
static Matrix g_update_sigma_measurement = { .row = MEASUREMENT_DIM, .col = SIGMA_FACTOR, .entries =
		&g_update_sigma_measurement_entries[0] };

static float g_update_measurement_covariance_entries[MEASUREMENT_DIM * SIGMA_FACTOR] = {};
static Matrix g_update_measurement_cov = { .row = MEASUREMENT_DIM, .col = SIGMA_FACTOR, .entries =
		&g_update_measurement_covariance_entries[0] };

static float m1_update_cross_covariance_entries[STATE_DIM * 1] = {};
static Matrix m1_update_cross_cov = { .row = STATE_DIM, .col = 1, .entries =
		&m1_update_cross_covariance_entries[0] };

static float m_update_state_entries[STATE_DIM * 1] = {};
static Matrix m_update_state = { .row = STATE_DIM, .col = 1, .entries =
		&m_update_state_entries[0] };

static float t_update_state_covariance_entries[1 * STATE_DIM] = {};
static Matrix t_update_state_cov = { .row = 1, .col = STATE_DIM, .entries =
		&t_update_state_covariance_entries[0] };

static float m_update_state_covariance_entries[STATE_DIM * STATE_DIM] = {};
static Matrix m_update_state_cov = { .row = STATE_DIM, .col = STATE_DIM, .entries =
		&m_update_state_covariance_entries[0] };

/*
 * format
 static float matrix_name_entries[STATE_DIM * STATE_DIM];
 static Matrix matrix_name = {.row = , .col = , .entries = &[0]};
 */

static uint32_t soc_update_cnt_10ms = 1;
static uint32_t soc_sleep_cnt_10ms = 0;


SOC_State state;

enum SOC_ERR{
	SOC_SUCCESS = 0,
	SOC_FAIL
};

struct SOC_UKF_t{
	SOC_Input *input;
	SOC_Output *output;
	SOC_State state;
	uint32_t total_pack_voltage;
	int32_t total_pack_current;
	uint32_t avg_pack_voltage;
	int32_t avg_pack_current;
	uint32_t avg_cnt;
};
typedef struct SOC_UKF_t SOC_UKF;
static SOC_UKF bms_soc;

void check_calib_condition(const SOC_Input input);
void parameter_init(const SOC_Input input, const float soh);
void soc_update_ukf(const SOC_UKF p_soc, const float soh);
void soc_calib(const SOC_Input input, const float soh);
void soc_update_coulomb_counter(const SOC_UKF p_soc, const float soh);

void soc_set_state(SOC_UKF* soc, const SOC_State soc_state);
static inline SOC_State soc_get_state(const SOC_UKF *const soc);

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

void soc_set_state(SOC_UKF* soc, const SOC_State soc_state){
	//TODO: add the average voltage and current feature
	//TODO: add the transaction feature
	soc->state = soc_state;
}
static inline SOC_State soc_get_state(const SOC_UKF *const soc){
	return soc->state;
}

void ukf_init(const uint32_t pack_voltage, const int32_t pack_current) {
	bms_soc.input = &soc_input;
	bms_soc.output = &soc_output;
	bms_soc.state = state;
	bms_soc.total_pack_voltage = 0;
	bms_soc.total_pack_current = 0;
	bms_soc.avg_pack_voltage = pack_voltage;
	bms_soc.avg_pack_current = pack_current;
	bms_soc.avg_cnt = 0;
	soc_set_state(&bms_soc, SOC_ST_INIT);
}

void check_calib_condition(const SOC_Input input){
	if (absolute_f((float) input.pack_current) < 25.0f) {
		soc_sleep_cnt_10ms++;
	}
	if (absolute_f((float) input.pack_current) > 25.0f) {
		soc_sleep_cnt_10ms = 0;
	}
	if (soc_sleep_cnt_10ms == CNT_3_MINUTE_10mS) {
		soc_sleep_cnt_10ms = 0;
		soc_set_state(&bms_soc, SOC_ST_CALIB);
	}
}

uint8_t ukf_update(const SOC_Input input, const float soh) {


	if(bms_soc.avg_cnt == SOC_PERIOD){
		bms_soc.avg_pack_voltage = (uint32_t)(bms_soc.total_pack_voltage/SOC_PERIOD);
		bms_soc.avg_pack_current = (int32_t)(bms_soc.total_pack_current/SOC_PERIOD);
		bms_soc.total_pack_voltage = 0;
		bms_soc.total_pack_current = 0;
		bms_soc.avg_cnt = 0;
	}
	bms_soc.total_pack_voltage += input.pack_voltage;
	bms_soc.total_pack_current += input.pack_current;
	bms_soc.avg_cnt++;

	check_calib_condition(input);

	switch (soc_get_state(&bms_soc)) {
	case SOC_ST_INIT:
		vffrls_init(input.pack_voltage);
		parameter_init(input, soh);
		if (absolute_f(
				soc_output.SOC_f
						- get_soc_from_ocv(
								(float) input.pack_voltage
										/ PACK_VOLTAGE_NORMALIZED)
								* 100.0f)>SOC_MAX_ERROR) {
			soc_set_state(&bms_soc, SOC_ST_CALIB);
			break;
		}
		soc_set_state(&bms_soc, SOC_ST_IDLE);
		break;
	case SOC_ST_IDLE:
		if (soc_update_cnt_10ms++ == SOC_PERIOD) {
			soc_update_cnt_10ms = 1;
			soc_set_state(&bms_soc, SOC_ST_UKF);
			if (input.pack_voltage > 66500 && input.pack_current > 0
					&& input.pack_current < 300)
				soc_set_state(&bms_soc, SOC_ST_COULOMB_COUNTER);
		}
		break;
	case SOC_ST_UKF:
		soc_set_state(&bms_soc, SOC_ST_IDLE);
		if (est_state.entries[0] < 0.0145 && input.pack_current < 20){
			break;
		}
		soc_update_ukf(bms_soc, soh);
		break;
	case SOC_ST_COULOMB_COUNTER:
		soc_set_state(&bms_soc, SOC_ST_IDLE);
		if (est_state.entries[0] > 0.9975)
			break;
		soc_update_coulomb_counter(bms_soc, soh);
		break;
	case SOC_ST_CALIB:
		soc_calib(input, soh);
		parameter_init(input, soh);
		soc_set_state(&bms_soc, SOC_ST_SLEEP);
		break;
	case SOC_ST_SLEEP:
		if (absolute_f((float) input.pack_current) > 25.0f) {
			soc_set_state(&bms_soc, SOC_ST_IDLE);
		}
		break;
	case SOC_ST_FAULT:
		soc_set_state(&bms_soc, SOC_ST_INIT);
		return SOC_FAIL;
	default:
		soc_set_state(&bms_soc, SOC_ST_INIT);
		return SOC_FAIL;
	}
	return SOC_SUCCESS;
}

void parameter_init(const SOC_Input input, const float soh){

	uint8_t i;

	vffrls_init(input.pack_voltage);
	cell_voltage = (float)input.pack_voltage/PACK_VOLTAGE_NORMALIZED;

	est_state.entries[0] = soc_output.SOC_f/100;
	est_state.entries[1] = 0.0f;
	est_state.entries[2] = 0.0f;

	for (i = 0; i < STATE_DIM * STATE_DIM; i++)
		state_cov.entries[i] = default_state_covariance[i];

	for (i = 0; i < STATE_DIM * STATE_DIM; i++)
		matrix_A.entries[i] = 0.0f;
	matrix_A.entries[0] = 1.0f;
	matrix_A.entries[4] = exponent_f((float)(-SAMPLE_TIME_s / (R1_INIT * C1_INIT)));
	matrix_A.entries[8] = 1.0f;

	for (i = 0; i < STATE_DIM * DYNAMIC_DIM; i++)
		matrix_B.entries[i] = 0.0f;
	matrix_B.entries[0] = (float)(ETA * SAMPLE_TIME_s / NOMIMAL_CAPACITY);
	matrix_B.entries[2] = 1.0f - exponent_f((float)(-SAMPLE_TIME_s / (R1_INIT * C1_INIT)));

	for (i = 0; i < MEASUREMENT_DIM * STATE_DIM; i++)
		matrix_C.entries[i] = 0.0f;
	matrix_C.entries[STATE_DIM - 1] = -HYSTERESIS; //here

	for (i = 0; i < MEASUREMENT_DIM * DYNAMIC_DIM; i++)
		matrix_D.entries[i] = 0.0f;
	matrix_D.entries[1] = -HYSTERESIS_ZERO; //here

	observed_measurement.entries[0] = 0.0f;
	observed_measurement.entries[1] = 0.0f;

	est_measurement = cell_voltage;

    soc_output.SOC = (uint32_t) roundf(soc_output.SOC_f);
}

void soc_update_ukf(const SOC_UKF p_soc, const float soh){
	cell_voltage = (float)p_soc.avg_pack_voltage/PACK_VOLTAGE_NORMALIZED;
	cell_current = (float)p_soc.avg_pack_current/PACK_CURRENT_NORMALIZED;
//	aukf_create_sigma_points();
	hgenerate(est_state, STATE_DIM, g1_create_sigma_point);
	chol(state_cov, g2_create_sigma_point);
	scalar_multiply(g2_create_sigma_point, GAMMA, g2_create_sigma_point);
	sum(g1_create_sigma_point, g2_create_sigma_point, g1_create_sigma_point);
	scalar_multiply(g2_create_sigma_point, 2, g2_create_sigma_point);
	minus(g1_create_sigma_point, g2_create_sigma_point, g2_create_sigma_point);
	htri_concat(est_state, g1_create_sigma_point, g2_create_sigma_point,
			sigma_points);

//	aukf_time_update_sigma_state();
	multiply(matrix_B, observed_measurement, m_update_sigma_state);
	hgenerate(m_update_sigma_state, SIGMA_FACTOR, g_update_sigma_state);
	multiply(matrix_A, sigma_points, sigma_points);
	sum(sigma_points, g_update_sigma_state, sigma_points);

	uint8_t i,j;
//	aukf_time_update_predict_state();
	/*

	for (i = 0; i < STATE_DIM; i++) {
		priori_est_state.entries[i] = 0.0f;
	}
	for (i = 0; i < SIGMA_FACTOR; i++) {
		priori_est_state.entries[0] += m_weight[i] * sigma_points.entries[i];
	}
	for (i = 0; i < SIGMA_FACTOR; i++) {
		priori_est_state.entries[1] += m_weight[i]
				* sigma_points.entries[SIGMA_FACTOR + i];
	}
	for (i = 0; i < SIGMA_FACTOR; i++) {
		priori_est_state.entries[2] += m_weight[i]
				* sigma_points.entries[2 * SIGMA_FACTOR + i];
	}
	*/

	multiply(sigma_points, m_weight_matrix, priori_est_state);

//	aukf_time_update_sigmaStateError();
	hgenerate(priori_est_state, SIGMA_FACTOR, sigma_state_err);
	minus(sigma_points, sigma_state_err, sigma_state_err);

//	aukf_time_update_state_covariance();
	for (i = 0; i < (STATE_DIM * STATE_DIM); i++){
		state_cov.entries[i] = default_system_covariance[i];
	}
	for (i = 0; i < SIGMA_FACTOR; i++) {
		for (j = 0; j < STATE_DIM; j++) {
			m1_update_state_cov.entries[j] =
					sigma_state_err.entries[j * SIGMA_FACTOR + i];
			m2_update_state_cov.entries[j] =
					sigma_state_err.entries[j * SIGMA_FACTOR + i];
		}
		multiply(m1_update_state_cov,
				m2_update_state_cov,
				m3_update_state_cov);
		scalar_multiply(m3_update_state_cov, c_weight[i],
				m3_update_state_cov);
		sum(state_cov, m3_update_state_cov, state_cov);
	}

//	aukf_create_sigma_points();
	hgenerate(priori_est_state, STATE_DIM, g1_create_sigma_point);
	chol(state_cov, g2_create_sigma_point);
	scalar_multiply(g2_create_sigma_point, GAMMA, g2_create_sigma_point);
	sum(g1_create_sigma_point, g2_create_sigma_point, g1_create_sigma_point);
	scalar_multiply(g2_create_sigma_point, 2, g2_create_sigma_point);
	minus(g1_create_sigma_point, g2_create_sigma_point, g2_create_sigma_point);
	htri_concat(priori_est_state, g1_create_sigma_point, g2_create_sigma_point,
			sigma_points);

//	aukf_update_system_parameters();
	observed_measurement.entries[0] = cell_current;
	observed_measurement.entries[1] = (float) sign_f(cell_current);
	if (absolute_f(cell_current) < 0.001f) { //here 0.1f
		observed_measurement.entries[1] = 0.0f;
	}
	eta = ETA;
	if (cell_current < 0.0f) {
		eta = 1.0f;
	}
	RC_param = exponent_f(-absolute_f((float)SAMPLE_TIME_s / (battery_param.R1*battery_param.C1)));
	H_param = exponent_f(
			-(absolute_f(
					(float) (eta * CAPACITY_RATIO * SAMPLE_TIME_s * cell_current
							/ (soh * NOMIMAL_CAPACITY)))));

//	aukf_update_matrix_a();
	matrix_A.entries[4] = RC_param;
	matrix_A.entries[8] = H_param;

//	aukf_update_matrix_b();
	matrix_B.entries[0] = -(eta * (float)SAMPLE_TIME_s)
			/ (soh * NOMIMAL_CAPACITY);
	matrix_B.entries[2] = 1.0f - RC_param;
	matrix_B.entries[5] = H_param - 1.0f;

//	aukf_update_matrix_c();
//	float d = 0.0f;
//	for (i = 0; i < SIGMA_FACTOR; i++) {
//		d += get_ratio_from_soc(sigma_points.entries[i]);
//	}
//	matrix_C.entries[0] = d / SIGMA_FACTOR;
//	matrix_C.entries[1] = -battery_param.R1;

	float d[SIGMA_FACTOR] = {0.0f};
	for (i = 0; i < SIGMA_FACTOR; i++) {
		d[i] = get_ratio_from_soc(sigma_points.entries[i]);
	}
	matrix_C.entries[0] = (d[0]+d[1]+d[2]+d[3]+d[4]+d[5]+d[6]) / SIGMA_FACTOR;
	matrix_C.entries[1] = -battery_param.R1;

//	aukf_update_matrix_d();
	matrix_D.entries[0] = -battery_param.R0;

//	aukf_time_update_sigma_measurements();
	multiply(matrix_C, sigma_points, sigma_measurements);
	float deltaMeasurement;
	deltaMeasurement = inner_multiply(matrix_D, observed_measurement);
	for (i = 0; i < SIGMA_FACTOR; i++){
		g_update_sigma_measurement.entries[i] = deltaMeasurement;
	}
	sum(sigma_measurements, g_update_sigma_measurement, sigma_measurements);

//	aukf_time_update_measurement();
	est_measurement = inner_multiply(sigma_measurements, m_weight_matrix);

//	aukf_update_sigma_measurement_error();
	for (i = 0; i < SIGMA_FACTOR; i++){
		g_update_measurement_cov.entries[i] = est_measurement;
	}
	minus(sigma_measurements, g_update_measurement_cov,
			sigma_measurement_err);

//	aukf_update_measurement_covariance();
	measurement_cov = MEASUREMENT_COVARIANCE_DEFAULT;
	for (i = 0; i < SIGMA_FACTOR; i++){
		measurement_cov += c_weight[i] * sigma_measurement_err.entries[i]
				* sigma_measurement_err.entries[i];
	}

//	aukf_update_cross_covariance();
	for (i = 0; i < STATE_DIM; i++){
		cross_cov.entries[i] = 0;
	}
	for (i = 0; i < SIGMA_FACTOR; i++) {
		for (j = 0; j < STATE_DIM; j++)
			m1_update_cross_cov.entries[j] = sigma_state_err.entries[j
					* SIGMA_FACTOR + i];
		scalar_multiply(m1_update_cross_cov,
				(c_weight[i] * sigma_measurement_err.entries[i]),
				m1_update_cross_cov);
		sum(cross_cov, m1_update_cross_cov, cross_cov);
	}

//	aukf_update_kalman_gain();
	scalar_multiply(cross_cov, (1.0f / measurement_cov),
			kalman_gain);

//	aukf_update_state();
	scalar_multiply(kalman_gain,
			(cell_voltage - est_measurement), m_update_state);
	sum(priori_est_state, m_update_state, est_state);

//	aukf_estimate_state_bounder();
	if (est_state.entries[0] > 1.0f) {
		est_state.entries[0] = 1.0f;
	}
	if (est_state.entries[2] < -1.0f) {
		est_state.entries[2] = -1.0f;
	}
	if (est_state.entries[2] > 1.0f) {
		est_state.entries[2] = 1.0f;
	}
	if (est_state.entries[0] < 0) {
		soc_set_state(&bms_soc, SOC_ST_CALIB);
	}


//	aukf_update_state_covariance();
	for (i = 0; i < STATE_DIM; i++)
		t_update_state_cov.entries[i] = kalman_gain.entries[i];

	multiply(kalman_gain, t_update_state_cov,
			m_update_state_cov);
	scalar_multiply(m_update_state_cov, measurement_cov,
			m_update_state_cov);
	minus(state_cov, m_update_state_cov, state_cov);
	soc_output.SOC_f = est_state.entries[0]*100.0f;

	soc_output.SOC = (uint32_t) roundf(soc_output.SOC_f);
    if(soc_output.SOC > 100){
    	soc_output.SOC = 100;
    }
}

void soc_calib(const SOC_Input input, __attribute__((unused)) const float soh){
	est_state.entries[0] = get_soc_from_ocv(
			(float) input.pack_voltage / PACK_VOLTAGE_NORMALIZED);
	est_state.entries[2] = 0.0f;
    soc_output.SOC_f = est_state.entries[0]*100.0f;
    soc_output.SOC = (uint32_t) roundf(soc_output.SOC_f);
}

void soc_update_coulomb_counter(const SOC_UKF p_soc, __attribute__((unused)) const float soh){

	est_state.entries[0] -= (float)(p_soc.avg_pack_current*SAMPLE_TIME_s)/(PACK_CURRENT_NORMALIZED*NOMIMAL_CAPACITY);
	if(est_state.entries[0]>1.0f){
		est_state.entries[0] =1.0f;
	}
    soc_output.SOC_f = est_state.entries[0]*100.0f;
    soc_output.SOC = (uint32_t) roundf(soc_output.SOC_f);
}
