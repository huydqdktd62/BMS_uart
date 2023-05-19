/*
 * soc_ukf.c
 *
 *  Created on: Feb 15, 2023
 *      Author: ADMIN
 */
/* USER CODE END Header */

/* USER CODE BEGIN Includes */
#include "../soc_ukf/soc_ukf.h"
#include "../soc_ukf/soc_ukf_config.h"
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
#define NORMALIZED_PREDICT_STATE_IN_GAIN					(1000000000000000.0f)
#define NORMALIZED_PREDICT_STATE_OUT_GAIN					(1000000000000000.0f)
#define NORMALIZED_STATE_COVARIANCE_IN_GAIN					(1000000000.0f)
#define NORMALIZED_STATE_COVARIANCE_OUT_GAIN				(1000000000000000000.0f)
#define NORMALIZED_PREDICT_MEASUREMENT_IN_GAIN				(1000000.0f)
#define NORMALIZED_PREDICT_MEASUREMENT_OUT_GAIN				(1000000.0f)
#define NORMALIZED_MEASUREMENT_COVARIANCE_IN_GAIN			(1000000.0f)
#define NORMALIZED_MEASUREMENT_COVARIANCE_OUT_GAIN			(1000000000000.0f)
#define NORMALIZED_CROSS_COVARIANCE_IN_GAIN					(1000000.0f)
#define NORMALIZED_CROSS_COVARIANCE_OUT_GAIN				(1000000000000.0f)
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

static int64_t cross_covariance_64_bit[UKF_SIGMA_FACTOR];
static int64_t sigma_state_err_64_bit[UKF_STATE_DIM * UKF_SIGMA_FACTOR];
static int64_t sigma_measurement_err_64_bit[UKF_SIGMA_FACTOR];
static const int64_t c_weight_64_bit[UKF_SIGMA_FACTOR] = {
		-3002,
		500,
		500,
		500,
		500,
		500,
		500
};

static const int64_t default_measurement_covariance_64_bit = 447200000000;
static int64_t measurement_covariance_64_bit;

static const int64_t default_system_covariance_64_bit[] = {
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

Battery_param battery_param;
static uint32_t soc_update_cnt_10ms = 1;
static uint32_t soc_sleep_cnt_10ms = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

static void filter_init(const uint32_t pack_voltage, const int32_t pack_current,
		SOC_UKF *battery_soc);
static void entries_init(SOC_UKF* battery_soc, const float soh);
static void output_init(SOC_UKF* battery_soc);
static void check_calib_condition(SOC_UKF* battery_soc);
static void soc_calib(SOC_UKF* battery_soc, const float soh);
static void soc_update_filter(SOC_UKF* battery_soc);
static void soc_update_ukf(SOC_UKF* battery_soc, const float soh);
static void soc_update_coulomb_counter(SOC_UKF* battery_soc, const float soh);
static void soc_set_state(SOC_UKF* battery_soc, const SOC_State soc_state);
static inline SOC_State soc_get_state(const SOC_UKF *const battery_soc);

void load_soc(SOC_UKF* battery_soc, const float soc){
    battery_soc->output.SOC_f = soc;
}

void ukf_init(const uint32_t pack_voltage, const int32_t pack_current, SOC_UKF* battery_soc) {
	battery_soc->input.pack_voltage = pack_voltage;
	battery_soc->input.pack_current = pack_current;
	filter_init(pack_voltage, pack_current, battery_soc);
	entries_init(battery_soc, 1.0f);
	output_init(battery_soc);
	battery_soc->err = SOC_SUCCESS;
	soc_set_state(battery_soc, SOC_ST_INIT);
}
uint8_t ukf_update(SOC_UKF* battery_soc, const float soh) {

	soc_update_filter(battery_soc);
	check_calib_condition(battery_soc);

	switch (soc_get_state(battery_soc)) {
	case SOC_ST_INIT:
		entries_init(battery_soc, soh);
		if (absolute_f(
				battery_soc->output.SOC_f
						- get_soc_from_ocv(
								(float) battery_soc->input.pack_voltage
										/ PACK_VOLTAGE_NORMALIZED_GAIN)
								* SOC_NORMALIZED_GAIN)>UKF_SOC_MAX_ERROR_percent) {
			soc_set_state(battery_soc, SOC_ST_CALIB);
			break;
		}
		soc_set_state(battery_soc, SOC_ST_IDLE);
		break;
	case SOC_ST_IDLE:
		if (soc_update_cnt_10ms++ == SOC_PERIOD) {
			soc_update_cnt_10ms = 1;
			soc_set_state(battery_soc, SOC_ST_UKF);
			if (battery_soc->input.pack_voltage
					> COULOMBCOUNTER_VOLTAGE_THRESHOLD
					&& battery_soc->input.pack_current
							> COULOMBCOUNTER_LOWER_CURRENT_THRESHOLD
					&& battery_soc->input.pack_current
							< COULOMBCOUNTER_UPPER_CURRENT_THRESHOLD)
				soc_set_state(battery_soc, SOC_ST_COULOMB_COUNTER);
		}
		break;
	case SOC_ST_UKF:
		soc_set_state(battery_soc, SOC_ST_IDLE);
		if (battery_soc->param.est_state.entries[0]
				< UKF_LOWER_EST_STATE_THRESHOLD
				&& battery_soc->input.pack_current
						< UKF_LOWER_CURRENT_THRESHOLD) {
			break;
		}
		soc_update_ukf(battery_soc, soh);
		break;
	case SOC_ST_COULOMB_COUNTER:
		soc_set_state(battery_soc, SOC_ST_IDLE);
		if (battery_soc->param.est_state.entries[0]
				> COULOMBCOUNTER_UPPER_EST_STATE_THRESHOLD)
			break;
		soc_update_coulomb_counter(battery_soc, soh);
		break;
	case SOC_ST_CALIB:
		soc_calib(battery_soc, soh);
		entries_init(battery_soc, soh);
		soc_set_state(battery_soc, SOC_ST_SLEEP);
		break;
	case SOC_ST_SLEEP:
		if (absolute_f((float) battery_soc->input.pack_current)
				> UKF_SLEEP_CURRENT_THRESHOLD) {
			soc_set_state(battery_soc, SOC_ST_IDLE);
		}
		break;
	case SOC_ST_FAULT:
		soc_set_state(battery_soc, SOC_ST_INIT);
		return SOC_FAIL;
	default:
		soc_set_state(battery_soc, SOC_ST_INIT);
		return SOC_FAIL;
	}
	return SOC_SUCCESS;
}

static void check_calib_condition(SOC_UKF* battery_soc){
	if (absolute_f((float) battery_soc->input.pack_current)
			< UKF_CALIB_CURRENT_THRESHOLD) {
		soc_sleep_cnt_10ms++;
	}
	if (absolute_f((float) battery_soc->input.pack_current)
			> UKF_CALIB_CURRENT_THRESHOLD) {
		soc_sleep_cnt_10ms = 0;
	}
	if (soc_sleep_cnt_10ms == CNT_3_MINUTE_10mS) {
		soc_sleep_cnt_10ms = 0;
		soc_set_state(battery_soc, SOC_ST_CALIB);
	}
}

static void filter_init(const uint32_t pack_voltage, const int32_t pack_current, SOC_UKF* battery_soc){
	battery_soc->filter.total_pack_voltage = 0;
	battery_soc->filter.total_pack_current = 0;
	battery_soc->filter.avg_pack_voltage = pack_voltage;
	battery_soc->filter.avg_pack_current = pack_current;
	battery_soc->filter.avg_cnt = 0;
}

void ukf_parameters_init(SOC_UKF* battery_soc, SOC_Parameter_Entries* init_entries){
	battery_soc->param.est_state.row = UKF_STATE_DIM;
	battery_soc->param.est_state.col = UKF_SINGLE_DIM;
	battery_soc->param.est_state.entries = &init_entries->estimate_state_entries[0];

	battery_soc->param.state_cov.row = UKF_STATE_DIM;
	battery_soc->param.state_cov.col = UKF_STATE_DIM;
	battery_soc->param.state_cov.entries = &init_entries->state_covariance_entries[0];

	battery_soc->param.sigma_points.row = UKF_STATE_DIM;
	battery_soc->param.sigma_points.col = UKF_SIGMA_FACTOR;
	battery_soc->param.sigma_points.entries = &init_entries->sigma_points_entries[0];

	battery_soc->param.priori_est_state.row = UKF_STATE_DIM;
	battery_soc->param.priori_est_state.col = UKF_SINGLE_DIM;
	battery_soc->param.priori_est_state.entries = &init_entries->priori_estimate_state_entries[0];

	battery_soc->param.matrix_A.row = UKF_STATE_DIM;
	battery_soc->param.matrix_A.col = UKF_STATE_DIM;
	battery_soc->param.matrix_A.entries = &init_entries->matrix_A_entries[0];

	battery_soc->param.matrix_B.row = UKF_STATE_DIM;
	battery_soc->param.matrix_B.col = UKF_DYNAMIC_DIM;
	battery_soc->param.matrix_B.entries = &init_entries->matrix_B_entries[0];

	battery_soc->param.matrix_C.row = UKF_MEASUREMENT_DIM;
	battery_soc->param.matrix_C.col = UKF_STATE_DIM;
	battery_soc->param.matrix_C.entries = &init_entries->matrix_C_entries[0];

	battery_soc->param.matrix_D.row = UKF_MEASUREMENT_DIM;
	battery_soc->param.matrix_D.col = UKF_DYNAMIC_DIM;
	battery_soc->param.matrix_D.entries = &init_entries->matrix_D_entries[0];

	battery_soc->param.sigma_state_err.row = UKF_STATE_DIM;
	battery_soc->param.sigma_state_err.col = UKF_SIGMA_FACTOR;
	battery_soc->param.sigma_state_err.entries = &init_entries->sigma_state_error_entries[0];

	battery_soc->param.observed_measurement.row = UKF_DYNAMIC_DIM;
	battery_soc->param.observed_measurement.col = UKF_SINGLE_DIM;
	battery_soc->param.observed_measurement.entries = &init_entries->observed_measurement_entries[0];

	battery_soc->param.sigma_measurements.row = UKF_MEASUREMENT_DIM;
	battery_soc->param.sigma_measurements.col = UKF_SIGMA_FACTOR;
	battery_soc->param.sigma_measurements.entries = &init_entries->sigma_measurements_entries[0];

	battery_soc->param.sigma_measurement_err.row = UKF_MEASUREMENT_DIM;
	battery_soc->param.sigma_measurement_err.col = UKF_SIGMA_FACTOR;
	battery_soc->param.sigma_measurement_err.entries = &init_entries->sigma_measurement_error_entries[0];

	battery_soc->param.cross_cov.row = UKF_STATE_DIM;
	battery_soc->param.cross_cov.col = UKF_MEASUREMENT_DIM;
	battery_soc->param.cross_cov.entries = &init_entries->cross_covariance_entries[0];

	battery_soc->param.kalman_gain.row = UKF_STATE_DIM;
	battery_soc->param.kalman_gain.col = UKF_SINGLE_DIM;
	battery_soc->param.kalman_gain.entries = &init_entries->aukf_kalman_gain_entries[0];

	battery_soc->param.g1_create_sigma_point.row = UKF_STATE_DIM;
	battery_soc->param.g1_create_sigma_point.col = UKF_STATE_DIM;
	battery_soc->param.g1_create_sigma_point.entries = &init_entries->g1_create_sigma_point_entries[0];

	battery_soc->param.g2_create_sigma_point.row = UKF_STATE_DIM;
	battery_soc->param.g2_create_sigma_point.col = UKF_STATE_DIM;
	battery_soc->param.g2_create_sigma_point.entries = &init_entries->g2_create_sigma_point_entries[0];

	battery_soc->param.m_update_sigma_state.row = UKF_STATE_DIM;
	battery_soc->param.m_update_sigma_state.col = UKF_SINGLE_DIM;
	battery_soc->param.m_update_sigma_state.entries = &init_entries->m_update_sigma_state_entries[0];

	battery_soc->param.g_update_sigma_state.row = UKF_STATE_DIM;
	battery_soc->param.g_update_sigma_state.col = UKF_SIGMA_FACTOR;
	battery_soc->param.g_update_sigma_state.entries = &init_entries->g_update_sigma_state_entries[0];

	battery_soc->param.m1_update_state_cov.row = UKF_STATE_DIM;
	battery_soc->param.m1_update_state_cov.col = UKF_SINGLE_DIM;
	battery_soc->param.m1_update_state_cov.entries = &init_entries->m1_time_update_state_covariance_entries[0];

	battery_soc->param.m2_update_state_cov.row = UKF_SINGLE_DIM;
	battery_soc->param.m2_update_state_cov.col = UKF_STATE_DIM;
	battery_soc->param.m2_update_state_cov.entries = &init_entries->m2_time_update_state_covariance_entries[0];

	battery_soc->param.m3_update_state_cov.row = UKF_STATE_DIM;
	battery_soc->param.m3_update_state_cov.col = UKF_STATE_DIM;
	battery_soc->param.m3_update_state_cov.entries = &init_entries->m3_time_update_state_covariance_entries[0];

	battery_soc->param.g_update_sigma_measurement.row = UKF_MEASUREMENT_DIM;
	battery_soc->param.g_update_sigma_measurement.col = UKF_SIGMA_FACTOR;
	battery_soc->param.g_update_sigma_measurement.entries = &init_entries->g_update_sigma_measurement_entries[0];

	battery_soc->param.g_update_measurement_cov.row = UKF_MEASUREMENT_DIM;
	battery_soc->param.g_update_measurement_cov.col = UKF_SIGMA_FACTOR;
	battery_soc->param.g_update_measurement_cov.entries = &init_entries->g_update_measurement_covariance_entries[0];

	battery_soc->param.m1_update_cross_cov.row = UKF_STATE_DIM;
	battery_soc->param.m1_update_cross_cov.col = UKF_SINGLE_DIM;
	battery_soc->param.m1_update_cross_cov.entries = &init_entries->m1_update_cross_covariance_entries[0];

	battery_soc->param.m_update_state.row = UKF_STATE_DIM;
	battery_soc->param.m_update_state.col = UKF_SINGLE_DIM;
	battery_soc->param.m_update_state.entries = &init_entries->m_update_state_entries[0];

	battery_soc->param.t_update_state_cov.row = UKF_SINGLE_DIM;
	battery_soc->param.t_update_state_cov.col = UKF_STATE_DIM;
	battery_soc->param.t_update_state_cov.entries = &init_entries->t_update_state_covariance_entries[0];

	battery_soc->param.m_update_state_cov.row = UKF_STATE_DIM;
	battery_soc->param.m_update_state_cov.col = UKF_STATE_DIM;
	battery_soc->param.m_update_state_cov.entries = &init_entries->m_update_state_covariance_entries[0];
}

static void entries_init(SOC_UKF* battery_soc, __attribute__((unused)) const float soh){
	uint8_t i;

	battery_param.R0 = UKF_R0_INIT_Omh;
	battery_param.R1 = UKF_R1_INIT_Omh;
	battery_param.C1 = UKF_C1_INIT_F;
	battery_soc->param.cell_voltage = (float)battery_soc->input.pack_voltage/PACK_VOLTAGE_NORMALIZED_GAIN;

	battery_soc->param.est_state.entries[0] = battery_soc->output.SOC_f/SOC_NORMALIZED_GAIN;
	battery_soc->param.est_state.entries[1] = UKF_EST_STATE_ENTRY_1_INIT_VALUE;
	battery_soc->param.est_state.entries[2] = UKF_EST_STATE_ENTRY_2_INIT_VALUE;

	for (i = 0; i < UKF_STATE_DIM * UKF_STATE_DIM; i++)
		battery_soc->param.state_cov.entries[i] = default_state_covariance[i];

	for (i = 0; i < UKF_STATE_DIM * UKF_STATE_DIM; i++){
		battery_soc->param.matrix_A.entries[i] = ZERO;
	}

	battery_soc->param.matrix_A.entries[0] = UKF_MATRIX_A_ENTRY_0_INIT_VALUE;
	battery_soc->param.matrix_A.entries[4] = exponent_f((float)(-UKF_SAMPLE_TIME_s / (UKF_R1_INIT_Omh * UKF_C1_INIT_F)));
	battery_soc->param.matrix_A.entries[8] = UKF_MATRIX_A_ENTRY_8_INIT_VALUE;

	for (i = 0; i < UKF_STATE_DIM * UKF_DYNAMIC_DIM; i++){
		battery_soc->param.matrix_B.entries[i] = ZERO;
	}

	battery_soc->param.matrix_B.entries[0] = -(float)(UKF_DISCHARGE_ETA_RATIO * UKF_SAMPLE_TIME_s / UKF_NOMIMAL_CAPACITY_AS);
	battery_soc->param.matrix_B.entries[2] = UKF_UNIT_F - exponent_f((float)(-UKF_SAMPLE_TIME_s / (UKF_R1_INIT_Omh * UKF_C1_INIT_F)));

	for (i = 0; i < UKF_MEASUREMENT_DIM * UKF_STATE_DIM; i++){
		battery_soc->param.matrix_C.entries[i] = ZERO;
	}

	battery_soc->param.matrix_C.entries[UKF_STATE_DIM - 1] = -UKF_HYSTERESIS_V; //here

	for (i = 0; i < UKF_MEASUREMENT_DIM * UKF_DYNAMIC_DIM; i++){
		battery_soc->param.matrix_D.entries[i] = ZERO;
	}

	battery_soc->param.matrix_D.entries[1] = -UKF_HYSTERESIS_ZERO_V; //here

	battery_soc->param.observed_measurement.entries[0] = ZERO;
	battery_soc->param.observed_measurement.entries[1] = ZERO;

	battery_soc->param.est_measurement = battery_soc->param.cell_voltage;
}

static void output_init(SOC_UKF* battery_soc){
	battery_soc->output.R0 = UKF_R0_INIT_Omh;
	battery_soc->output.R1 = UKF_R1_INIT_Omh;
	battery_soc->output.C1 = UKF_C1_INIT_F;
	battery_soc->output.SOC = (uint32_t) roundf(battery_soc->output.SOC_f);
}

static void soc_update_filter(SOC_UKF* battery_soc){
	if(battery_soc->filter.avg_cnt == SOC_PERIOD){
		battery_soc->filter.avg_pack_voltage = (uint32_t)(battery_soc->filter.total_pack_voltage/SOC_PERIOD);
		battery_soc->filter.avg_pack_current = (int32_t)(battery_soc->filter.total_pack_current/SOC_PERIOD);
		battery_soc->filter.total_pack_voltage = 0;
		battery_soc->filter.total_pack_current = 0;
		battery_soc->filter.avg_cnt = 0;
	}
	battery_soc->filter.total_pack_voltage += battery_soc->input.pack_voltage;
	battery_soc->filter.total_pack_current += battery_soc->input.pack_current;
	battery_soc->filter.avg_cnt++;
}

static void soc_update_ukf(SOC_UKF* battery_soc, const float soh){
	uint8_t i,j,k;
    int32_t r,c;

	battery_soc->param.cell_voltage = (float)battery_soc->filter.avg_pack_voltage/PACK_VOLTAGE_NORMALIZED_GAIN;
	battery_soc->param.cell_current = (float)battery_soc->filter.avg_pack_current/PACK_CURRENT_NORMALIZED_GAIN;
//	aukf_create_sigma_points();
	hgenerate(battery_soc->param.est_state, UKF_STATE_DIM, battery_soc->param.g1_create_sigma_point);
	chol(battery_soc->param.state_cov, battery_soc->param.g2_create_sigma_point);
	scalar_multiply(battery_soc->param.g2_create_sigma_point, UKF_GAMMA_RATIO, battery_soc->param.g2_create_sigma_point);
	sum(battery_soc->param.g1_create_sigma_point, battery_soc->param.g2_create_sigma_point, battery_soc->param.g1_create_sigma_point);
	scalar_multiply(battery_soc->param.g2_create_sigma_point, 2, battery_soc->param.g2_create_sigma_point);
	minus(battery_soc->param.g1_create_sigma_point, battery_soc->param.g2_create_sigma_point, battery_soc->param.g2_create_sigma_point);
	htri_concat(battery_soc->param.est_state, battery_soc->param.g1_create_sigma_point, battery_soc->param.g2_create_sigma_point,
			battery_soc->param.sigma_points);

//	aukf_time_update_sigma_state();
	multiply(battery_soc->param.matrix_B, battery_soc->param.observed_measurement, battery_soc->param.m_update_sigma_state);
	hgenerate(battery_soc->param.m_update_sigma_state, UKF_SIGMA_FACTOR, battery_soc->param.g_update_sigma_state);
	multiply(battery_soc->param.matrix_A, battery_soc->param.sigma_points, battery_soc->param.sigma_points);
	sum(battery_soc->param.sigma_points, battery_soc->param.g_update_sigma_state, battery_soc->param.sigma_points);

//	aukf_time_update_predict_state();
#if NORMALIZED_CODE
	for (j = 0; j < UKF_STATE_DIM * UKF_SIGMA_FACTOR; j++) {
		sigma_point_64_bit[j] =
				(int64_t) (battery_soc->param.sigma_points.entries[j]
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
		battery_soc->param.priori_est_state.entries[j] =
				(float) priori_est_state_64_bit[j]
						/ NORMALIZED_PREDICT_STATE_OUT_GAIN;
	}
#else
	for (i = 0; i < UKF_STATE_DIM; i++) {
		battery_soc->param.priori_est_state.entries[i] = ZERO;
	}
	for (i = 0; i < UKF_SIGMA_FACTOR; i++) {
		battery_soc->param.priori_est_state.entries[0] += m_weight[i] * battery_soc->param.sigma_points.entries[i];
	}
	for (i = 0; i < UKF_SIGMA_FACTOR; i++) {
		battery_soc->param.priori_est_state.entries[1] += m_weight[i]
				* battery_soc->param.sigma_points.entries[UKF_SIGMA_FACTOR + i];
	}
	for (i = 0; i < UKF_SIGMA_FACTOR; i++) {
		battery_soc->param.priori_est_state.entries[2] += m_weight[i]
				* battery_soc->param.sigma_points.entries[2 * UKF_SIGMA_FACTOR + i];
	}
#endif

//	aukf_time_update_sigmaStateError();
	hgenerate(battery_soc->param.priori_est_state, UKF_SIGMA_FACTOR, battery_soc->param.sigma_state_err);
	minus(battery_soc->param.sigma_points, battery_soc->param.sigma_state_err, battery_soc->param.sigma_state_err);

//	aukf_time_update_state_covariance();
#if NORMALIZED_CODE
	for (j = 0; j < UKF_STATE_DIM * UKF_SIGMA_FACTOR; j++) {
		sigma_state_err_64_bit[j] =
				(int64_t) (battery_soc->param.sigma_state_err.entries[j]
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
		battery_soc->param.state_cov.entries[j] =
				(float) state_covariance_64_bit[j]
						/ NORMALIZED_STATE_COVARIANCE_OUT_GAIN;
	}
#else
	for (i = 0; i < (UKF_STATE_DIM * UKF_STATE_DIM); i++){
		battery_soc->param.state_cov.entries[i] = default_system_covariance[i];
	}
	for (i = 0; i < UKF_SIGMA_FACTOR; i++) {
		for (j = 0; j < UKF_STATE_DIM; j++) {
			battery_soc->param.m1_update_state_cov.entries[j] =
					battery_soc->param.sigma_state_err.entries[j * UKF_SIGMA_FACTOR + i];
			battery_soc->param.m2_update_state_cov.entries[j] =
					battery_soc->param.sigma_state_err.entries[j * UKF_SIGMA_FACTOR + i];
		}
		multiply(battery_soc->param.m1_update_state_cov,
				battery_soc->param.m2_update_state_cov,
				battery_soc->param.m3_update_state_cov);
		scalar_multiply(battery_soc->param.m3_update_state_cov, c_weight[i],
				battery_soc->param.m3_update_state_cov);
		sum(battery_soc->param.state_cov, battery_soc->param.m3_update_state_cov, battery_soc->param.state_cov);
	}
#endif

//	aukf_create_sigma_points();
	hgenerate(battery_soc->param.priori_est_state, UKF_STATE_DIM, battery_soc->param.g1_create_sigma_point);
	chol(battery_soc->param.state_cov, battery_soc->param.g2_create_sigma_point);
	scalar_multiply(battery_soc->param.g2_create_sigma_point, UKF_GAMMA_RATIO, battery_soc->param.g2_create_sigma_point);
	sum(battery_soc->param.g1_create_sigma_point, battery_soc->param.g2_create_sigma_point, battery_soc->param.g1_create_sigma_point);
	scalar_multiply(battery_soc->param.g2_create_sigma_point, 2, battery_soc->param.g2_create_sigma_point);
	minus(battery_soc->param.g1_create_sigma_point, battery_soc->param.g2_create_sigma_point, battery_soc->param.g2_create_sigma_point);
	htri_concat(battery_soc->param.priori_est_state, battery_soc->param.g1_create_sigma_point, battery_soc->param.g2_create_sigma_point,
			battery_soc->param.sigma_points);

//	aukf_update_system_parameters();
	battery_soc->param.observed_measurement.entries[0] = battery_soc->param.cell_current;
	battery_soc->param.observed_measurement.entries[1] = (float) sign_f(battery_soc->param.cell_current);
	if (absolute_f(battery_soc->param.cell_current) < UKF_OBSERVED_CURRENT_THRESHOLD) { //here 0.1f
		battery_soc->param.observed_measurement.entries[1] = ZERO;
	}
	battery_soc->param.eta = UKF_DISCHARGE_ETA_RATIO;
	if (battery_soc->param.cell_current < ZERO) {
		battery_soc->param.eta = UKF_CHARGE_ETA_RATIO;
	}
	battery_soc->param.RC_param = exponent_f(-absolute_f((float)UKF_SAMPLE_TIME_s / (battery_param.R1*battery_param.C1)));
	battery_soc->param.H_param = exponent_f(
			-(absolute_f(
					(float) (battery_soc->param.eta * UKF_CAPACITY_RATIO * UKF_SAMPLE_TIME_s * battery_soc->param.cell_current
							/ (soh * UKF_NOMIMAL_CAPACITY_AS)))));

//	aukf_update_matrix_a();
	battery_soc->param.matrix_A.entries[4] = battery_soc->param.RC_param;
	battery_soc->param.matrix_A.entries[8] = battery_soc->param.H_param;

//	aukf_update_matrix_b();
	battery_soc->param.matrix_B.entries[0] = -(battery_soc->param.eta * (float)UKF_SAMPLE_TIME_s)
			/ (soh * UKF_NOMIMAL_CAPACITY_AS);
	battery_soc->param.matrix_B.entries[2] = ONE - battery_soc->param.RC_param;
	battery_soc->param.matrix_B.entries[5] = battery_soc->param.H_param - ONE;

//	aukf_update_matrix_c();

//	float d = 0.0f;
//	for (i = 0; i < SIGMA_FACTOR; i++) {
//		d += get_ratio_from_soc(battery_soc->param.sigma_points.entries[i]);
//	}
//	battery_soc->param.matrix_C.entries[0] = d / SIGMA_FACTOR;
//	battery_soc->param.matrix_C.entries[1] = -battery_param.R1;

	float d[UKF_SIGMA_FACTOR] = {ZERO};
	for (i = 0; i < UKF_SIGMA_FACTOR; i++) {
		d[i] = get_ratio_from_soc(battery_soc->param.sigma_points.entries[i]);
	}
	battery_soc->param.matrix_C.entries[0] = (d[0]+d[1]+d[2]+d[3]+d[4]+d[5]+d[6]) / UKF_SIGMA_FACTOR;
	battery_soc->param.matrix_C.entries[1] = -battery_param.R1;

//	aukf_update_matrix_d();
	battery_soc->param.matrix_D.entries[0] = -battery_param.R0;

//	aukf_time_update_sigma_measurements();
	multiply(battery_soc->param.matrix_C, battery_soc->param.sigma_points, battery_soc->param.sigma_measurements);
	float deltaMeasurement;
	deltaMeasurement = inner_multiply(battery_soc->param.matrix_D, battery_soc->param.observed_measurement);
	for (i = 0; i < UKF_SIGMA_FACTOR; i++){
		battery_soc->param.g_update_sigma_measurement.entries[i] = deltaMeasurement;
	}
	sum(battery_soc->param.sigma_measurements, battery_soc->param.g_update_sigma_measurement, battery_soc->param.sigma_measurements);

//	aukf_time_update_measurement();
#if NORMALIZED_CODE
	for (j = 0; j < UKF_SIGMA_FACTOR; j++) {
		sigma_measurement_64_bit[j] =
				(int64_t) (battery_soc->param.sigma_measurements.entries[j]
						* NORMALIZED_PREDICT_MEASUREMENT_IN_GAIN);
	}
	est_measurement_64_bit = 0;
	for (j = 0; j < UKF_SIGMA_FACTOR; j++) {
		est_measurement_64_bit += m_weight_64_bit[j]
				* sigma_measurement_64_bit[j];
	}
	battery_soc->param.est_measurement = (float) est_measurement_64_bit
			/ NORMALIZED_PREDICT_MEASUREMENT_OUT_GAIN;
#else
	battery_soc->param.est_measurement = inner_multiply(battery_soc->param.sigma_measurements, m_weight_matrix);
#endif

//	aukf_update_sigma_measurement_error();
	for (i = 0; i < UKF_SIGMA_FACTOR; i++){
		battery_soc->param.g_update_measurement_cov.entries[i] = battery_soc->param.est_measurement;
	}
	minus(battery_soc->param.sigma_measurements, battery_soc->param.g_update_measurement_cov,
			battery_soc->param.sigma_measurement_err);

//	aukf_update_measurement_covariance();
#if NORMALIZED_CODE
	measurement_covariance_64_bit = default_measurement_covariance_64_bit;
	for (j = 0; j < UKF_SIGMA_FACTOR; j++) {
		sigma_measurement_err_64_bit[j] =
				(int64_t) (battery_soc->param.sigma_measurement_err.entries[j]
						* NORMALIZED_MEASUREMENT_COVARIANCE_IN_GAIN);
		measurement_covariance_64_bit += c_weight_64_bit[j]
				* sigma_measurement_err_64_bit[j]
				* sigma_measurement_err_64_bit[j];
	}
	battery_soc->param.measurement_cov = (float) measurement_covariance_64_bit
			/ NORMALIZED_MEASUREMENT_COVARIANCE_OUT_GAIN;
#else
	battery_soc->param.measurement_cov = UKF_DEFAULT_MEASUREMENT_COVARIANCE;
	for (i = 0; i < UKF_SIGMA_FACTOR; i++){
		battery_soc->param.measurement_cov += c_weight[i] * battery_soc->param.sigma_measurement_err.entries[i]
				* battery_soc->param.sigma_measurement_err.entries[i];
	}
#endif

//	aukf_update_cross_covariance();
#if NORMALIZED_CODE
	for (j = 0; j < UKF_STATE_DIM * UKF_SIGMA_FACTOR; j++) {
		sigma_state_err_64_bit[j] =
				(int64_t) (battery_soc->param.sigma_state_err.entries[j]
						* NORMALIZED_CROSS_COVARIANCE_IN_GAIN);
	}
	for (j = 0; j < UKF_SIGMA_FACTOR; j++) {
		sigma_measurement_err_64_bit[j] =
				(int64_t) (battery_soc->param.sigma_measurement_err.entries[j]
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
		battery_soc->param.cross_cov.entries[j] =
				(float) cross_covariance_64_bit[j]
						/ NORMALIZED_CROSS_COVARIANCE_OUT_GAIN;
	}
#else
	for (i = 0; i < UKF_STATE_DIM; i++){
		battery_soc->param.cross_cov.entries[i] = ZERO;
	}
	for (i = 0; i < UKF_SIGMA_FACTOR; i++) {
		for (j = 0; j < UKF_STATE_DIM; j++){
			battery_soc->param.m1_update_cross_cov.entries[j] = battery_soc->param.sigma_state_err.entries[j
					* UKF_SIGMA_FACTOR + i];
		}
		scalar_multiply(battery_soc->param.m1_update_cross_cov,
				(c_weight[i] * battery_soc->param.sigma_measurement_err.entries[i]),
				battery_soc->param.m1_update_cross_cov);
		sum(battery_soc->param.cross_cov, battery_soc->param.m1_update_cross_cov, battery_soc->param.cross_cov);
	}
#endif

//	aukf_update_kalman_gain();
	scalar_multiply(battery_soc->param.cross_cov, (ONE / battery_soc->param.measurement_cov),
			battery_soc->param.kalman_gain);

//	aukf_update_state();
	scalar_multiply(battery_soc->param.kalman_gain,
			(battery_soc->param.cell_voltage - battery_soc->param.est_measurement), battery_soc->param.m_update_state);
	sum(battery_soc->param.priori_est_state, battery_soc->param.m_update_state, battery_soc->param.est_state);

//	aukf_estimate_state_bounder();
	if (battery_soc->param.est_state.entries[0] > UKF_EST_STATE_ENTRY_0_UPPER_LIMIT) {
		battery_soc->param.est_state.entries[0] = UKF_EST_STATE_ENTRY_0_UPPER_LIMIT;
	}
	if (battery_soc->param.est_state.entries[2] < UKF_EST_STATE_ENTRY_2_LOWER_LIMIT) {
		battery_soc->param.est_state.entries[2] = UKF_EST_STATE_ENTRY_2_LOWER_LIMIT;
	}
	if (battery_soc->param.est_state.entries[2] > UKF_EST_STATE_ENTRY_2_UPPER_LIMIT) {
		battery_soc->param.est_state.entries[2] = UKF_EST_STATE_ENTRY_2_UPPER_LIMIT;
	}
	if (battery_soc->param.est_state.entries[0] < UKF_EST_STATE_ENTRY_0_LOWER_LIMIT) {
		soc_set_state(battery_soc, SOC_ST_CALIB);
	}

//	aukf_update_state_covariance();
	for (i = 0; i < UKF_STATE_DIM; i++){
		battery_soc->param.t_update_state_cov.entries[i] = battery_soc->param.kalman_gain.entries[i];
	}
	multiply(battery_soc->param.kalman_gain, battery_soc->param.t_update_state_cov,
			battery_soc->param.m_update_state_cov);
	scalar_multiply(battery_soc->param.m_update_state_cov, battery_soc->param.measurement_cov,
			battery_soc->param.m_update_state_cov);
	minus(battery_soc->param.state_cov, battery_soc->param.m_update_state_cov, battery_soc->param.state_cov);
	battery_soc->output.SOC_f = battery_soc->param.est_state.entries[0]*SOC_NORMALIZED_GAIN;

	battery_soc->output.SOC = (uint32_t) roundf(battery_soc->output.SOC_f);
    if(battery_soc->output.SOC > BMS_SOC_UPPER_LIMIT){
    	battery_soc->output.SOC = BMS_SOC_UPPER_LIMIT;
    }
}

static void soc_calib(SOC_UKF* battery_soc, __attribute__((unused)) const float soh){
	battery_soc->param.est_state.entries[0] = get_soc_from_ocv(
			(float) battery_soc->input.pack_voltage / PACK_VOLTAGE_NORMALIZED_GAIN);
	battery_soc->param.est_state.entries[2] = UKF_EST_STATE_ENTRY_2_INIT_VALUE;
	battery_soc->output.SOC_f = battery_soc->param.est_state.entries[0]*SOC_NORMALIZED_GAIN;
	battery_soc->output.SOC = (uint32_t) roundf(battery_soc->output.SOC_f);
}

static void soc_update_coulomb_counter(SOC_UKF* battery_soc,__attribute__((unused)) const float soh){
	battery_soc->param.est_state.entries[0] -= (float)(battery_soc->filter.avg_pack_current*UKF_SAMPLE_TIME_s)/(PACK_CURRENT_NORMALIZED_GAIN*UKF_NOMIMAL_CAPACITY_AS);
	if(battery_soc->param.est_state.entries[0]>UKF_EST_STATE_ENTRY_0_UPPER_LIMIT){
		battery_soc->param.est_state.entries[0] =UKF_EST_STATE_ENTRY_0_UPPER_LIMIT;
	}
	battery_soc->output.SOC_f = battery_soc->param.est_state.entries[0]*SOC_NORMALIZED_GAIN;
	battery_soc->output.SOC = (uint32_t) roundf(battery_soc->output.SOC_f);
}

void soc_set_state(SOC_UKF* battery_soc, const SOC_State soc_state){
	battery_soc->state = soc_state;
}
static inline SOC_State soc_get_state(const SOC_UKF *const battery_soc){
	return battery_soc->state;
}
/* USER CODE END PFP */
