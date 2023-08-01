/*
 * soc_ukf.c
 *
 *  Created on: Feb 15, 2023
 *      Author: ADMIN
 */
/* USER CODE END Header */

/* USER CODE BEGIN Includes */
#include "../soc_ukf/soc_ukf.h"

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

#if NORMALIZED_CODE
#define NORMALIZED_PREDICT_STATE_IN_GAIN					(1000000000000.0f)
#define NORMALIZED_PREDICT_STATE_OUT_GAIN					NORMALIZED_PREDICT_STATE_IN_GAIN
#define NORMALIZED_STATE_COVARIANCE_IN_GAIN					(1000000000000.0f)
#define NORMALIZED_STATE_COVARIANCE_OUT_GAIN				(NORMALIZED_STATE_COVARIANCE_IN_GAIN*NORMALIZED_STATE_COVARIANCE_IN_GAIN)
#define NORMALIZED_PREDICT_MEASUREMENT_IN_GAIN				(1000000000.0f)
#define NORMALIZED_PREDICT_MEASUREMENT_OUT_GAIN				NORMALIZED_PREDICT_MEASUREMENT_IN_GAIN
#define NORMALIZED_MEASUREMENT_COVARIANCE_IN_GAIN			(1000000000.0f)
#define NORMALIZED_MEASUREMENT_COVARIANCE_OUT_GAIN			(NORMALIZED_MEASUREMENT_COVARIANCE_IN_GAIN*NORMALIZED_MEASUREMENT_COVARIANCE_IN_GAIN)
#define NORMALIZED_CROSS_COVARIANCE_IN_GAIN					(1000000000000.0f)
#define NORMALIZED_CROSS_COVARIANCE_OUT_GAIN				(NORMALIZED_CROSS_COVARIANCE_IN_GAIN*NORMALIZED_CROSS_COVARIANCE_IN_GAIN)
static int64_t priori_est_state_64_bit[UKF_STATE_DIM];
static int64_t sigma_point_64_bit[UKF_STATE_DIM * UKF_SIGMA_FACTOR];
static const int64_t m_weight_64_bit[UKF_SIGMA_FACTOR] = {     -2999,
                                    500,
                                    500,
                                    500,
                                    500,
                                    500,
                                    500 };

static int64_t est_measurement_64_bit;
static int64_t sigma_measurement_64_bit[UKF_SIGMA_FACTOR];

static int64_t cross_covariance_64_bit[UKF_STATE_DIM];
static int64_t sigma_state_err_64_bit[UKF_STATE_DIM * UKF_SIGMA_FACTOR];
static int64_t sigma_measurement_err_64_bit[UKF_SIGMA_FACTOR];
static const int64_t c_weight_64_bit[UKF_SIGMA_FACTOR] = {
		-30020002,
		5000000,
		5000000,
		5000000,
		5000000,
		5000000,
		5000000
};

static const int64_t default_measurement_covariance_64_bit = 447200000000;
static int64_t measurement_covariance_64_bit;

static const int64_t default_system_covariance_64_bit[UKF_STATE_DIM * UKF_STATE_DIM] = {
		1000000, 0, 0,
		0, 10000, 0,
		0, 0, 200000000
};
static int64_t state_covariance_64_bit[UKF_STATE_DIM * UKF_STATE_DIM];
#else
static float m_weight[UKF_SIGMA_FACTOR] = { 	-2999.0f,
									500.0f,
									500.0f,
									500.0f,
									500.0f,
									500.0f,
									500.0f };
static Matrix m_weight_matrix = { .row = UKF_SIGMA_FACTOR, .col = 1, .entries =
		&m_weight[0] };
static const float c_weight[UKF_SIGMA_FACTOR] = { 	-2999.0f - 3.0f - 0.0002f,
									500.0f,
									500.0f,
									500.0f,
									500.0f,
									500.0f,
									500.0f };
static const float default_system_covariance[UKF_STATE_DIM * UKF_STATE_DIM] = {
		0.000001f, 0.0f, 0.0f,
		0.0f, 0.00000001f, 0.0f,
		0.0f, 0.0f, 0.0002f
};
#endif

static const float default_state_covariance[UKF_STATE_DIM * UKF_STATE_DIM] = {
		0.001f, 0.0f, 0.0f,
		0.0f, 0.0001f, 0.0f,
		0.0f, 0.0f, 0.0141f
};

static Matrix estimate_state = {.row = UKF_STATE_DIM, .col = 1};
static Matrix state_covariance = {.row = UKF_STATE_DIM, .col = UKF_STATE_DIM};
static Matrix sigma_points = {.row = UKF_STATE_DIM, .col = UKF_SIGMA_FACTOR};
static Matrix priori_estimate_state = {.row = UKF_STATE_DIM, .col = 1};
static Matrix Matrix_A = {.row = UKF_STATE_DIM, .col = UKF_STATE_DIM};
static Matrix Matrix_B = {.row = UKF_STATE_DIM, .col = UKF_DYNAMIC_DIM};
static Matrix Matrix_C = {.row = UKF_MEASUREMENT_DIM, .col = UKF_STATE_DIM};
static Matrix Matrix_D = {.row = UKF_MEASUREMENT_DIM, .col = UKF_DYNAMIC_DIM};
static Matrix sigma_state_error = {.row = UKF_STATE_DIM, .col = UKF_SIGMA_FACTOR};
static Matrix observed_measurement = {.row = UKF_DYNAMIC_DIM, .col = 1};
static Matrix sigma_measurements = {.row = UKF_MEASUREMENT_DIM, .col = UKF_SIGMA_FACTOR};
static Matrix sigma_measurement_error = {.row = UKF_MEASUREMENT_DIM, .col = UKF_SIGMA_FACTOR};
static Matrix cross_covariance = {.row = UKF_STATE_DIM, .col = UKF_MEASUREMENT_DIM};
static Matrix kalman_gain = {.row = UKF_STATE_DIM, .col = 1};
static float* eta;
static float* est_measurement;
static float* measurement_cov;
static float* RC_param;
static float* H_param;
static float* cell_voltage;
static float* cell_current;
static Matrix g1_create_sigma_point = {.row = UKF_STATE_DIM, .col = UKF_STATE_DIM};
static Matrix g2_create_sigma_point = {.row = UKF_STATE_DIM, .col = UKF_STATE_DIM};
static Matrix m_update_sigma_state = {.row = UKF_STATE_DIM, .col = 1};
static Matrix g_update_sigma_state = {.row = UKF_STATE_DIM, .col = UKF_SIGMA_FACTOR};
static Matrix m1_update_state_cov = {.row = UKF_STATE_DIM, .col = 1};
static Matrix m2_update_state_cov = {.row = 1, .col = UKF_STATE_DIM};
static Matrix m3_update_state_cov = {.row = UKF_STATE_DIM, .col = UKF_STATE_DIM};
static Matrix g_update_sigma_measurement = {.row = UKF_MEASUREMENT_DIM, .col = UKF_SIGMA_FACTOR};
static Matrix g_update_measurement_cov = {.row = UKF_MEASUREMENT_DIM, .col = UKF_SIGMA_FACTOR};
static Matrix m1_update_cross_cov = {.row = UKF_STATE_DIM, .col = 1};
static Matrix m_update_state = {.row = UKF_STATE_DIM, .col = 1};
static Matrix t_update_state_cov = {.row = 1, .col = UKF_STATE_DIM};
static Matrix m_update_state_cov = {.row = UKF_STATE_DIM, .col = UKF_STATE_DIM};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */


static void entries_init(SOC_UKF* battery_soc);

static void soc_calib(SOC_UKF* battery_soc);
static void soc_update_filter(SOC_UKF* battery_soc);
static void soc_update_ukf(SOC_UKF* battery_soc);
static void soc_update_coulomb_counter(SOC_UKF* battery_soc);
static void soc_set_state(SOC_UKF* battery_soc, const SOC_State soc_state);
static inline SOC_State soc_get_state(const SOC_UKF *const battery_soc);

void load_soc(SOC_UKF* battery_soc, const float soc){
    battery_soc->output.SOC_f = soc;
}

void ukf_init(SOC_UKF* battery_soc, const uint8_t soh) {
	battery_soc->soh = (float)soh/100.0f;
	battery_soc->filter.total_pack_voltage = 0;
	battery_soc->filter.total_pack_current = 0;

	battery_soc->filter.avg_pack_voltage = battery_soc->input.pack_voltage;
	battery_soc->filter.avg_pack_current = battery_soc->input.pack_current;
	battery_soc->filter.last_pack_voltage = battery_soc->input.pack_voltage;
	battery_soc->filter.last_pack_current = battery_soc->input.pack_current;
	battery_soc->filter.avg_cnt = 0;

	battery_soc->output.SOC = (uint32_t) roundf((float)battery_soc->output.SOC_f);

	soc_set_state(battery_soc, SOC_ST_INIT);
	soc_set_state(battery_soc, SOC_ST_IDLE);
	battery_soc->err = SOC_SUCCESS;
}

uint8_t ukf_update(SOC_UKF *battery_soc) {
	battery_soc->soc_update_cnt_10ms++;
	soc_update_filter(battery_soc);
	switch (soc_get_state(battery_soc)) {
	case SOC_ST_INIT:
		if (absolute_f(
				battery_soc->output.SOC_f
						- model_get_soc_from_ocv(battery_soc->battery_model,
								(float) battery_soc->input.pack_voltage
										/ PACK_VOLTAGE_NORMALIZED_GAIN)
								* SOC_NORMALIZED_GAIN)>UKF_SOC_MAX_ERROR_percent) {
			soc_set_state(battery_soc, SOC_ST_CALIB);
			break;
		}
		soc_set_state(battery_soc, SOC_ST_IDLE);
		break;
	case SOC_ST_UKF:
		soc_set_state(battery_soc, SOC_ST_IDLE);
		break;
	case SOC_ST_COULOMB_COUNTER:
		soc_set_state(battery_soc, SOC_ST_IDLE);
		break;
	case SOC_ST_CALIB:
		soc_set_state(battery_soc, SOC_ST_SLEEP);
		break;
	case SOC_ST_SLEEP:
		battery_soc->soc_update_cnt_10ms = 0;
		if (absolute_f((float) battery_soc->input.pack_current)
				> UKF_SLEEP_CURRENT_THRESHOLD) {
			soc_set_state(battery_soc, SOC_ST_IDLE);
		}
		//no break
	case SOC_ST_IDLE:
		if (absolute_f((float) battery_soc->input.pack_current)
				< UKF_CALIB_CURRENT_THRESHOLD) {
			battery_soc->soc_sleep_cnt_10ms++;
		}
		if (absolute_f((float) battery_soc->input.pack_current)
				> UKF_CALIB_CURRENT_THRESHOLD) {
			battery_soc->soc_sleep_cnt_10ms = 0;
		}
		if (battery_soc->soc_sleep_cnt_10ms == CNT_3_MINUTE_10mS) {
			battery_soc->soc_sleep_cnt_10ms = 0;
			soc_set_state(battery_soc, SOC_ST_CALIB);
			break;
		}
		if (battery_soc->soc_update_cnt_10ms >= SOC_PERIOD) {
			if (battery_soc->input.pack_voltage > COULOMBCOUNTER_VOLTAGE_THRESHOLD
					&& battery_soc->input.pack_current
							> COULOMBCOUNTER_LOWER_CURRENT_THRESHOLD
					&& battery_soc->input.pack_current
							< COULOMBCOUNTER_UPPER_CURRENT_THRESHOLD){
				soc_set_state(battery_soc, SOC_ST_COULOMB_COUNTER);
			}else{
				soc_set_state(battery_soc, SOC_ST_UKF);
			}
			battery_soc->soc_update_cnt_10ms = 1;
		}
		break;
	case SOC_ST_FAULT:
		soc_set_state(battery_soc, SOC_ST_INIT);
		return SOC_FAIL;
	default:
		soc_set_state(battery_soc, SOC_ST_INIT);
		return SOC_FAIL;
	}
	battery_soc->output.SOC_f = estimate_state.entries[0]*SOC_NORMALIZED_GAIN;
	battery_soc->output.SOC = (uint32_t) roundf(battery_soc->output.SOC_f);
    if(battery_soc->output.SOC > BMS_SOC_UPPER_LIMIT){
    	battery_soc->output.SOC = BMS_SOC_UPPER_LIMIT;
    }
	return SOC_SUCCESS;
}

void soc_set_state(SOC_UKF* battery_soc, const SOC_State soc_state){
	switch (soc_state) {
	case SOC_ST_INIT:
		entries_init(battery_soc);
		break;
	case SOC_ST_UKF:
		if (estimate_state.entries[0]
				< UKF_LOWER_EST_STATE_THRESHOLD
				&& battery_soc->input.pack_current
						< UKF_LOWER_CURRENT_THRESHOLD) {
			break;
		}
		soc_update_ukf(battery_soc);
		break;
	case SOC_ST_COULOMB_COUNTER:
		if (estimate_state.entries[0]
				> COULOMBCOUNTER_UPPER_EST_STATE_THRESHOLD)
			break;
		soc_update_coulomb_counter(battery_soc);
		break;
	case SOC_ST_CALIB:
		soc_calib(battery_soc);
		entries_init(battery_soc);
		break;
	case SOC_ST_SLEEP:
		break;
	case SOC_ST_IDLE:
		break;
	case SOC_ST_FAULT:
		break;
	default:
		break;
	}
	battery_soc->state = soc_state;
}
static inline SOC_State soc_get_state(const SOC_UKF *const battery_soc){
	return battery_soc->state;
}

void ukf_parameters_create(SOC_Parameter* parameter){
	estimate_state.entries = &parameter->estimate_state_entries[0];
	state_covariance.entries = &parameter->state_covariance_entries[0];
	sigma_points.entries = &parameter->sigma_points_entries[0];
	priori_estimate_state.entries = &parameter->priori_estimate_state_entries[0];
	Matrix_A.entries = &parameter->matrix_A_entries[0];
	Matrix_B.entries = &parameter->matrix_B_entries[0];
	Matrix_C.entries = &parameter->matrix_C_entries[0];
	Matrix_D.entries = &parameter->matrix_D_entries[0];
	sigma_state_error.entries = &parameter->sigma_state_error_entries[0];
	observed_measurement.entries = &parameter->observed_measurement_entries[0];
	sigma_measurements.entries = &parameter->sigma_measurements_entries[0];
	sigma_measurement_error.entries = &parameter->sigma_measurement_error_entries[0];
	cross_covariance.entries = &parameter->cross_covariance_entries[0];
	kalman_gain.entries = &parameter->aukf_kalman_gain_entries[0];
	eta = &parameter->eta;
	est_measurement = &parameter->est_measurement;
	measurement_cov = &parameter->measurement_cov;
	RC_param = &parameter->RC_param;
	H_param = &parameter->H_param;
	cell_voltage = &parameter->cell_voltage;
	cell_current = &parameter->cell_current;
	g1_create_sigma_point.entries = parameter->g1_create_sigma_point_entries;
	g2_create_sigma_point.entries = parameter->g2_create_sigma_point_entries;
	m_update_sigma_state.entries = parameter->m_update_sigma_state_entries;
	g_update_sigma_state.entries = &parameter->g_update_sigma_state_entries[0];
	m1_update_state_cov.entries = &parameter->m1_time_update_state_covariance_entries[0];
	m2_update_state_cov.entries = &parameter->m2_time_update_state_covariance_entries[0];
	m3_update_state_cov.entries = &parameter->m3_time_update_state_covariance_entries[0];
	g_update_sigma_measurement.entries = &parameter->g_update_sigma_measurement_entries[0];
	g_update_measurement_cov.entries = &parameter->g_update_measurement_covariance_entries[0];
	m1_update_cross_cov.entries = &parameter->m1_update_cross_covariance_entries[0];
	m_update_state.entries = &parameter->m_update_state_entries[0];
	t_update_state_cov.entries = &parameter->t_update_state_covariance_entries[0];
	m_update_state_cov.entries = &parameter->m_update_state_covariance_entries[0];
}

static void entries_init(SOC_UKF* battery_soc){
	uint8_t i;

	battery_soc->soc_update_cnt_10ms = 1;
	battery_soc->soc_sleep_cnt_10ms = 0;
	battery_soc->battery_model->R0 = UKF_R0_INIT_Omh;
	battery_soc->battery_model->R1 = UKF_R1_INIT_Omh;
	battery_soc->battery_model->C1 = UKF_C1_INIT_F;
	*cell_voltage = (float)battery_soc->input.pack_voltage/PACK_VOLTAGE_NORMALIZED_GAIN;

	estimate_state.entries[0] = battery_soc->output.SOC_f/SOC_NORMALIZED_GAIN;
	estimate_state.entries[1] = UKF_EST_STATE_ENTRY_1_INIT_VALUE;
	estimate_state.entries[2] = UKF_EST_STATE_ENTRY_2_INIT_VALUE;

	for (i = 0; i < UKF_STATE_DIM * UKF_STATE_DIM; i++)
		state_covariance.entries[i] = default_state_covariance[i];

	for (i = 0; i < UKF_STATE_DIM * UKF_STATE_DIM; i++){
		Matrix_A.entries[i] = ZERO;
	}

	Matrix_A.entries[0] = UKF_MATRIX_A_ENTRY_0_INIT_VALUE;
	Matrix_A.entries[4] = exponent_f((float)(-UKF_SAMPLE_TIME_s / (UKF_R1_INIT_Omh * UKF_C1_INIT_F)));
	Matrix_A.entries[8] = UKF_MATRIX_A_ENTRY_8_INIT_VALUE;

	for (i = 0; i < UKF_STATE_DIM * UKF_DYNAMIC_DIM; i++){
		Matrix_B.entries[i] = ZERO;
	}

	Matrix_B.entries[0] = -(float)(UKF_DISCHARGE_ETA_RATIO * UKF_SAMPLE_TIME_s / UKF_NOMIMAL_CAPACITY_AS);
	Matrix_B.entries[2] = UKF_UNIT_F - exponent_f((float)(-UKF_SAMPLE_TIME_s / (UKF_R1_INIT_Omh * UKF_C1_INIT_F)));

	for (i = 0; i < UKF_MEASUREMENT_DIM * UKF_STATE_DIM; i++){
		Matrix_C.entries[i] = ZERO;
	}

	Matrix_C.entries[UKF_STATE_DIM - 1] = -UKF_HYSTERESIS_V; //here

	for (i = 0; i < UKF_MEASUREMENT_DIM * UKF_DYNAMIC_DIM; i++){
		Matrix_D.entries[i] = ZERO;
	}

	Matrix_D.entries[1] = -UKF_HYSTERESIS_ZERO_V; //here

	observed_measurement.entries[0] = ZERO;
	observed_measurement.entries[1] = ZERO;

	*est_measurement = *cell_voltage;
}

static void soc_update_filter(SOC_UKF* battery_soc){

//	battery_soc->filter.avg_pack_voltage = battery_soc->input.pack_voltage;
//	battery_soc->filter.avg_pack_current = battery_soc->input.pack_current;
	if(battery_soc->filter.avg_cnt == SOC_PERIOD){
		battery_soc->filter.avg_pack_voltage = (uint32_t)((float)battery_soc->filter.total_pack_voltage/(float)SOC_PERIOD);
		battery_soc->filter.avg_pack_current = (int32_t)((float)battery_soc->filter.total_pack_current/(float)SOC_PERIOD);
		battery_soc->filter.total_pack_voltage = 0;
		battery_soc->filter.total_pack_current = 0;
		battery_soc->filter.avg_cnt = 0;
	}
	if (battery_soc->input.pack_voltage > BATTERY_UPPER_VOLTAGE_THRESHOLD
			|| battery_soc->input.pack_voltage < BATTERY_LOWER_VOLTAGE_THRESHOLD){
		battery_soc->filter.total_pack_voltage += battery_soc->filter.last_pack_voltage;
		battery_soc->filter.total_pack_current += battery_soc->filter.last_pack_current;
		battery_soc->filter.avg_cnt++;
	}else{
		battery_soc->filter.last_pack_voltage = battery_soc->input.pack_voltage;
		battery_soc->filter.last_pack_current = battery_soc->input.pack_current;
		battery_soc->filter.total_pack_voltage += battery_soc->input.pack_voltage;
		battery_soc->filter.total_pack_current += battery_soc->input.pack_current;
		battery_soc->filter.avg_cnt++;
	}


}

static void soc_update_ukf(SOC_UKF* battery_soc){
	uint8_t i,j,k;
    int32_t r,c;

	*cell_voltage = (float)battery_soc->filter.avg_pack_voltage/PACK_VOLTAGE_NORMALIZED_GAIN;
	*cell_current = (float)battery_soc->filter.avg_pack_current/PACK_CURRENT_NORMALIZED_GAIN;
//	aukf_create_sigma_points();
	hgenerate(estimate_state, UKF_STATE_DIM, g1_create_sigma_point);
	chol(state_covariance, g2_create_sigma_point);
	scalar_multiply(g2_create_sigma_point, UKF_GAMMA_RATIO, g2_create_sigma_point);
	sum(g1_create_sigma_point, g2_create_sigma_point, g1_create_sigma_point);
	scalar_multiply(g2_create_sigma_point, 2, g2_create_sigma_point);
	minus(g1_create_sigma_point, g2_create_sigma_point, g2_create_sigma_point);
	htri_concat(estimate_state, g1_create_sigma_point, g2_create_sigma_point,
			sigma_points);

//	aukf_time_update_sigma_state();
	multiply(Matrix_B, observed_measurement, m_update_sigma_state);
	hgenerate(m_update_sigma_state, UKF_SIGMA_FACTOR, g_update_sigma_state);
	multiply(Matrix_A, sigma_points, sigma_points);
	sum(sigma_points, g_update_sigma_state, sigma_points);

//	aukf_time_update_predict_state();
#if NORMALIZED_CODE
	for (j = 0; j < UKF_STATE_DIM * UKF_SIGMA_FACTOR; j++) {
		sigma_point_64_bit[j] =
				(int64_t) (sigma_points.entries[j]
						* NORMALIZED_PREDICT_STATE_IN_GAIN);
	}
	for (j = 0; j < UKF_STATE_DIM; j++) {
		priori_est_state_64_bit[j] = 0;
	}
	for (j = 0; j < UKF_SIGMA_FACTOR; j++) {
		priori_est_state_64_bit[0] += m_weight_64_bit[j]
				* sigma_point_64_bit[j];
		priori_est_state_64_bit[1] += m_weight_64_bit[j]
				* sigma_point_64_bit[UKF_SIGMA_FACTOR + j];
		priori_est_state_64_bit[2] += m_weight_64_bit[j]
				* sigma_point_64_bit[2 * UKF_SIGMA_FACTOR + j];
	}
	for (j = 0; j < UKF_STATE_DIM; j++) {
		priori_estimate_state.entries[j] =
				(float) priori_est_state_64_bit[j]
						/ NORMALIZED_PREDICT_STATE_OUT_GAIN;
	}
#else
	for (i = 0; i < UKF_STATE_DIM; i++) {
		priori_estimate_state.entries[i] = ZERO;
	}
	for (i = 0; i < UKF_SIGMA_FACTOR; i++) {
		priori_estimate_state.entries[0] += m_weight[i] * sigma_points.entries[i];
	}
	for (i = 0; i < UKF_SIGMA_FACTOR; i++) {
		priori_estimate_state.entries[1] += m_weight[i]
				* sigma_points.entries[UKF_SIGMA_FACTOR + i];
	}
	for (i = 0; i < UKF_SIGMA_FACTOR; i++) {
		priori_estimate_state.entries[2] += m_weight[i]
				* sigma_points.entries[2 * UKF_SIGMA_FACTOR + i];
	}
#endif

//	aukf_time_update_sigmaStateError();
	hgenerate(priori_estimate_state, UKF_SIGMA_FACTOR, sigma_state_error);
	minus(sigma_points, sigma_state_error, sigma_state_error);

//	aukf_time_update_state_covariance();
#if NORMALIZED_CODE
	for (j = 0; j < UKF_STATE_DIM * UKF_SIGMA_FACTOR; j++) {
		sigma_state_err_64_bit[j] =
				(int64_t) (sigma_state_error.entries[j]
						* NORMALIZED_STATE_COVARIANCE_IN_GAIN);
	}
	for (j = 0; j < UKF_STATE_DIM * UKF_STATE_DIM; j++) {
		state_covariance_64_bit[j] = default_system_covariance_64_bit[j];
	}
	for (j = 0; j < UKF_SIGMA_FACTOR; j++) {
		for (r = 0; r < UKF_STATE_DIM; r++) {
			for (c = 0; c < UKF_STATE_DIM; c++) {
				state_covariance_64_bit[r * UKF_STATE_DIM + c] +=
						sigma_state_err_64_bit[r * UKF_SIGMA_FACTOR + j]
								* c_weight_64_bit[j]
								* sigma_state_err_64_bit[c * UKF_SIGMA_FACTOR
										+ j];
			}
		}
	}
	for (j = 0; j < UKF_STATE_DIM * UKF_STATE_DIM; j++) {
		state_covariance.entries[j] =
				(float) state_covariance_64_bit[j]
						/ NORMALIZED_STATE_COVARIANCE_OUT_GAIN;
	}
#else
	for (i = 0; i < (UKF_STATE_DIM * UKF_STATE_DIM); i++){
		state_covariance.entries[i] = default_system_covariance[i];
	}
	for (i = 0; i < UKF_SIGMA_FACTOR; i++) {
		for (j = 0; j < UKF_STATE_DIM; j++) {
			m1_update_state_cov.entries[j] =
					sigma_state_error.entries[j * UKF_SIGMA_FACTOR + i];
			m2_update_state_cov.entries[j] =
					sigma_state_error.entries[j * UKF_SIGMA_FACTOR + i];
		}
		multiply(m1_update_state_cov,
				m2_update_state_cov,
				m3_update_state_cov);
		scalar_multiply(m3_update_state_cov, c_weight[i],
				m3_update_state_cov);
		sum(state_covariance, m3_update_state_cov, state_covariance);
	}
#endif

//	aukf_create_sigma_points();
	hgenerate(priori_estimate_state, UKF_STATE_DIM, g1_create_sigma_point);
	chol(state_covariance, g2_create_sigma_point);
	scalar_multiply(g2_create_sigma_point, UKF_GAMMA_RATIO, g2_create_sigma_point);
	sum(g1_create_sigma_point, g2_create_sigma_point, g1_create_sigma_point);
	scalar_multiply(g2_create_sigma_point, 2, g2_create_sigma_point);
	minus(g1_create_sigma_point, g2_create_sigma_point, g2_create_sigma_point);
	htri_concat(priori_estimate_state, g1_create_sigma_point, g2_create_sigma_point,
			sigma_points);

//	aukf_update_system_parameters();
	observed_measurement.entries[0] = *cell_current;
	observed_measurement.entries[1] = (float) sign_f(*cell_current);
	if (absolute_f(*cell_current) < UKF_OBSERVED_CURRENT_THRESHOLD) { //here 0.1f
		observed_measurement.entries[1] = ZERO;
	}
	*eta = UKF_DISCHARGE_ETA_RATIO;
	if (*cell_current < ZERO) {
		*eta = UKF_CHARGE_ETA_RATIO;
	}
	*RC_param = exponent_f(-absolute_f((float)UKF_SAMPLE_TIME_s / (battery_soc->battery_model->R1*battery_soc->battery_model->C1)));
	*H_param = exponent_f(
			-(absolute_f(
					(float) (*eta * UKF_CAPACITY_RATIO * UKF_SAMPLE_TIME_s * *cell_current
							/ (battery_soc->soh * UKF_NOMIMAL_CAPACITY_AS)))));

//	aukf_update_matrix_a();
	Matrix_A.entries[4] = *RC_param;
	Matrix_A.entries[8] = *H_param;

//	aukf_update_matrix_b();
	Matrix_B.entries[0] = -(*eta * (float)UKF_SAMPLE_TIME_s)
			/ (battery_soc->soh * UKF_NOMIMAL_CAPACITY_AS);
	Matrix_B.entries[2] = ONE - *RC_param;
	Matrix_B.entries[5] = *H_param - ONE;

//	aukf_update_matrix_c();

//	float d = 0.0f;
//	for (i = 0; i < SIGMA_FACTOR; i++) {
//		d += get_ratio_from_soc(sigma_points.entries[i]);
//	}
//	matrix_C.entries[0] = d / SIGMA_FACTOR;
//	matrix_C.entries[1] = -battery_param.R1;

	float d[UKF_SIGMA_FACTOR] = {ZERO};
	for (i = 0; i < UKF_SIGMA_FACTOR; i++) {
		d[i] = model_get_ratio_from_soc(battery_soc->battery_model, sigma_points.entries[i]);
	}
	Matrix_C.entries[0] = (d[0]+d[1]+d[2]+d[3]+d[4]+d[5]+d[6]) / UKF_SIGMA_FACTOR;
	Matrix_C.entries[1] = -battery_soc->battery_model->R1;

//	aukf_update_matrix_d();
	Matrix_D.entries[0] = -battery_soc->battery_model->R0;

//	aukf_time_update_sigma_measurements();
	multiply(Matrix_C, sigma_points, sigma_measurements);
	float deltaMeasurement;
	deltaMeasurement = inner_multiply(Matrix_D, observed_measurement);
	for (i = 0; i < UKF_SIGMA_FACTOR; i++){
		g_update_sigma_measurement.entries[i] = deltaMeasurement;
	}
	sum(sigma_measurements, g_update_sigma_measurement, sigma_measurements);

//	aukf_time_update_measurement();
#if NORMALIZED_CODE
	for (j = 0; j < UKF_SIGMA_FACTOR; j++) {
		sigma_measurement_64_bit[j] =
				(int64_t) (sigma_measurements.entries[j]
						* NORMALIZED_PREDICT_MEASUREMENT_IN_GAIN);
	}
	est_measurement_64_bit = 0;
	for (j = 0; j < UKF_SIGMA_FACTOR; j++) {
		est_measurement_64_bit += m_weight_64_bit[j]
				* sigma_measurement_64_bit[j];
	}
	*est_measurement = (float) est_measurement_64_bit
			/ NORMALIZED_PREDICT_MEASUREMENT_OUT_GAIN;
#else
	*est_measurement = inner_multiply(sigma_measurements, m_weight_matrix);
#endif

//	aukf_update_sigma_measurement_error();
	for (i = 0; i < UKF_SIGMA_FACTOR; i++){
		g_update_measurement_cov.entries[i] = *est_measurement;
	}
	minus(sigma_measurements, g_update_measurement_cov,
			sigma_measurement_error);

//	aukf_update_measurement_covariance();
#if NORMALIZED_CODE
	measurement_covariance_64_bit = default_measurement_covariance_64_bit;
	for (j = 0; j < UKF_SIGMA_FACTOR; j++) {
		sigma_measurement_err_64_bit[j] =
				(int64_t) (sigma_measurement_error.entries[j]
						* NORMALIZED_MEASUREMENT_COVARIANCE_IN_GAIN);
		measurement_covariance_64_bit += c_weight_64_bit[j]
				* sigma_measurement_err_64_bit[j]
				* sigma_measurement_err_64_bit[j];
	}
	*measurement_cov = (float) measurement_covariance_64_bit
			/ NORMALIZED_MEASUREMENT_COVARIANCE_OUT_GAIN;
#else
	*measurement_cov = UKF_DEFAULT_MEASUREMENT_COVARIANCE;
	for (i = 0; i < UKF_SIGMA_FACTOR; i++){
		*measurement_cov += c_weight[i] * sigma_measurement_error.entries[i]
				* sigma_measurement_error.entries[i];
	}
#endif

//	aukf_update_cross_covariance();
#if NORMALIZED_CODE
	for (j = 0; j < UKF_STATE_DIM * UKF_SIGMA_FACTOR; j++) {
		sigma_state_err_64_bit[j] =
				(int64_t) (sigma_state_error.entries[j]
						* NORMALIZED_CROSS_COVARIANCE_IN_GAIN);
	}
	for (j = 0; j < UKF_SIGMA_FACTOR; j++) {
		sigma_measurement_err_64_bit[j] =
				(int64_t) (sigma_measurement_error.entries[j]
						* NORMALIZED_CROSS_COVARIANCE_IN_GAIN);
	}
	for (j = 0; j < UKF_STATE_DIM; j++) {
		cross_covariance_64_bit[j] = 0;
	}
	for (j = 0; j < UKF_STATE_DIM; j++) {
		for (k = 0; k < UKF_SIGMA_FACTOR; k++) {
			cross_covariance_64_bit[j] += sigma_state_err_64_bit[j
					* UKF_SIGMA_FACTOR + k] * c_weight_64_bit[k]
					* sigma_measurement_err_64_bit[k];
		}
	}
	for (j = 0; j < UKF_STATE_DIM; j++) {
		cross_covariance.entries[j] =
				(float) cross_covariance_64_bit[j]
						/ NORMALIZED_CROSS_COVARIANCE_OUT_GAIN;
	}
#else
	for (i = 0; i < UKF_STATE_DIM; i++){
		cross_covariance.entries[i] = ZERO;
	}
	for (i = 0; i < UKF_SIGMA_FACTOR; i++) {
		for (j = 0; j < UKF_STATE_DIM; j++){
			m1_update_cross_cov.entries[j] = sigma_state_error.entries[j
					* UKF_SIGMA_FACTOR + i];
		}
		scalar_multiply(m1_update_cross_cov,
				(c_weight[i] * sigma_measurement_error.entries[i]),
				m1_update_cross_cov);
		sum(cross_covariance, m1_update_cross_cov, cross_covariance);
	}
#endif

//	aukf_update_kalman_gain();
	scalar_multiply(cross_covariance, (ONE / *measurement_cov),
			kalman_gain);

//	aukf_update_state();
	scalar_multiply(kalman_gain,
			(*cell_voltage - *est_measurement), m_update_state);
	sum(priori_estimate_state, m_update_state, estimate_state);

//	aukf_estimate_state_bounder();
	if (estimate_state.entries[0] > UKF_EST_STATE_ENTRY_0_UPPER_LIMIT) {
		estimate_state.entries[0] = UKF_EST_STATE_ENTRY_0_UPPER_LIMIT;
	}
	if (estimate_state.entries[2] < UKF_EST_STATE_ENTRY_2_LOWER_LIMIT) {
		estimate_state.entries[2] = UKF_EST_STATE_ENTRY_2_LOWER_LIMIT;
	}
	if (estimate_state.entries[2] > UKF_EST_STATE_ENTRY_2_UPPER_LIMIT) {
		estimate_state.entries[2] = UKF_EST_STATE_ENTRY_2_UPPER_LIMIT;
	}
	if (estimate_state.entries[0] < UKF_EST_STATE_ENTRY_0_LOWER_LIMIT) {
		soc_set_state(battery_soc, SOC_ST_CALIB);
	}

//	aukf_update_state_covariance();
	for (i = 0; i < UKF_STATE_DIM; i++){
		t_update_state_cov.entries[i] = kalman_gain.entries[i];
	}
	multiply(kalman_gain, t_update_state_cov,
			m_update_state_cov);
	scalar_multiply(m_update_state_cov, *measurement_cov,
			m_update_state_cov);
	minus(state_covariance, m_update_state_cov, state_covariance);
}

static void soc_calib(SOC_UKF* battery_soc){
	battery_soc->output.SOC_f = 100.0f * model_get_soc_from_ocv(
			battery_soc->battery_model, (float) battery_soc->input.pack_voltage / PACK_VOLTAGE_NORMALIZED_GAIN);
	estimate_state.entries[2] = UKF_EST_STATE_ENTRY_2_INIT_VALUE;
}

static void soc_update_coulomb_counter(SOC_UKF* battery_soc){
	estimate_state.entries[0] -= ((float)battery_soc->filter.avg_pack_current*UKF_SAMPLE_TIME_s)/(battery_soc->soh * PACK_CURRENT_NORMALIZED_GAIN*UKF_NOMIMAL_CAPACITY_AS);
	if(estimate_state.entries[0]>UKF_EST_STATE_ENTRY_0_UPPER_LIMIT){
		estimate_state.entries[0] =UKF_EST_STATE_ENTRY_0_UPPER_LIMIT;
	}
}
/* USER CODE END PFP */
