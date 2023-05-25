/*
 * app_config.h
 *
 *  Created on: May 8, 2023
 *      Author: ADMIN
 */

#ifndef SERVICE_APP_CONFIG_APP_CONFIG_H_
#define SERVICE_APP_CONFIG_APP_CONFIG_H_

#include "soc_ukf_config.h"
#include "stdint.h"

#define TEST_SOC					1
#define NORMALIZED_TYPE				SYSTEM_PARAMETER


typedef enum Parameter_t Parameter;
enum Parameter_t{
	SIGMA_POINT = 0,
	SIGMA_STATE,
	PRIOR_STATE,
	SIGMA_STATE_ERROR,
	PRIOR_STATE_COVARIANCE,
	SYSTEM_PARAMETER,
	SIGMA_MEASUREMENT,
	PREDICT_MEASUREMENT,
	SIGMA_MEASUREMENT_ERROR,
	MEASUREMENT_COVARIANCE,
	CROSS_COVARIANCE,
	KALMAN_GAIN,
	POSTERIOR_STATE,
	POSTERIOR_STATE_COVARIANCE
};

typedef struct Data_Logger_t Data_Logger;
struct Data_Logger_t{
	int32_t estimate_state[UKF_STATE_DIM * 1];
	int32_t state_covariance[UKF_STATE_DIM * UKF_STATE_DIM];
	int32_t sigma_points[UKF_STATE_DIM * UKF_SIGMA_FACTOR];
	int32_t priori_estimate_state[UKF_STATE_DIM * 1];
	int32_t matrix_A[UKF_STATE_DIM * UKF_STATE_DIM];
	int32_t matrix_B[UKF_STATE_DIM * UKF_DYNAMIC_DIM];
	int32_t matrix_C[UKF_MEASUREMENT_DIM * UKF_STATE_DIM];
	int32_t matrix_D[UKF_MEASUREMENT_DIM * UKF_DYNAMIC_DIM];
	int32_t sigma_state_error[UKF_STATE_DIM * UKF_SIGMA_FACTOR];
	int32_t observed_measurement[UKF_DYNAMIC_DIM * 1];
	int32_t sigma_measurements[UKF_MEASUREMENT_DIM * UKF_SIGMA_FACTOR];
	int32_t sigma_measurement_error[UKF_MEASUREMENT_DIM * UKF_SIGMA_FACTOR];
	int32_t cross_covariance[UKF_STATE_DIM * UKF_MEASUREMENT_DIM];
	int32_t aukf_kalman_gain[UKF_STATE_DIM * 1];
	int32_t est_measurement;
	int32_t measurement_cov;
	int32_t pack_voltage, pack_current;
};

extern int test_circle;
extern Data_Logger input_logger;
extern Data_Logger output_logger;

#endif /* SERVICE_APP_CONFIG_APP_CONFIG_H_ */
