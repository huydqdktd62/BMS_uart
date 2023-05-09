/*
 * soc_ukf.h
 *
 *  Created on: Feb 15, 2023
 *      Author: ADMIN
 */

#ifndef SERVICE_SOC_AUKF_SOC_UKF_H_
#define SERVICE_SOC_AUKF_SOC_UKF_H_

#include <stdint.h>
#include "math_util.h"
#include "matrix.h"

#include "../soc_ukf_refactor/battery_model.h"
#include "../soc_ukf_refactor/soc_vffrls.h"

typedef struct SOC_Input_t SOC_Input;
struct SOC_Input_t{
	uint32_t pack_voltage;
	int32_t pack_current;
};

typedef struct SOC_Output_t SOC_Output;
struct SOC_Output_t{
	uint32_t SOC;
	float OCV;
	float R0;
	float R1;
	float C1;
	float SOC_f;
};

typedef enum SOC_State_t SOC_State;
enum SOC_State_t{
	SOC_ST_INIT = 0,
	SOC_ST_IDLE,
	SOC_ST_UKF,
	SOC_ST_COULOMB_COUNTER,
	SOC_ST_CALIB,
	SOC_ST_SLEEP,
	SOC_ST_FAULT
};


typedef enum SOC_err_t SOC_err;
enum SOC_err_t{
	SOC_SUCCESS = 0,
	SOC_FAIL
};

typedef struct SOC_Filter_t SOC_Filter;
struct SOC_Filter_t{
	uint32_t total_pack_voltage;
	int32_t total_pack_current;
	uint32_t avg_pack_voltage;
	int32_t avg_pack_current;
	uint32_t avg_cnt;
};

typedef struct SOC_parameter_t SOC_parameter;
struct SOC_parameter_t{
	Matrix est_state;
	Matrix state_cov;
	Matrix sigma_points;
	Matrix priori_est_state;
	Matrix matrix_A;
	Matrix matrix_B;
	Matrix matrix_C;
	Matrix matrix_D;
	Matrix sigma_state_err;
	Matrix observed_measurement;
	Matrix sigma_measurements;
	Matrix sigma_measurement_err;
	Matrix cross_cov;
	Matrix kalman_gain;
	float eta;
	float est_measurement;
	float measurement_cov;
	float RC_param;
	float H_param;
	float cell_voltage, cell_current;
	Matrix g1_create_sigma_point;
	Matrix g2_create_sigma_point;
	Matrix m_update_sigma_state;
	Matrix g_update_sigma_state;
	Matrix m1_update_state_cov;
	Matrix m2_update_state_cov;
	Matrix m3_update_state_cov;
	Matrix g_update_sigma_measurement;
	Matrix g_update_measurement_cov;
	Matrix m1_update_cross_cov;
	Matrix m_update_state;
	Matrix t_update_state_cov;
	Matrix m_update_state_cov;
};

typedef struct SOC_Parameter_Entries_t SOC_Parameter_Entries;
struct SOC_Parameter_Entries_t{
	float estimate_state_entries[UKF_STATE_DIM * 1];
	float state_covariance_entries[UKF_STATE_DIM * UKF_STATE_DIM];
	float sigma_points_entries[UKF_STATE_DIM * UKF_SIGMA_FACTOR];
	float priori_estimate_state_entries[UKF_STATE_DIM * 1];
	float matrix_A_entries[UKF_STATE_DIM * UKF_STATE_DIM];
	float matrix_B_entries[UKF_STATE_DIM * UKF_DYNAMIC_DIM];
	float matrix_C_entries[UKF_MEASUREMENT_DIM * UKF_STATE_DIM];
	float matrix_D_entries[UKF_MEASUREMENT_DIM * UKF_DYNAMIC_DIM];
	float sigma_state_error_entries[UKF_STATE_DIM * UKF_SIGMA_FACTOR];
	float observed_measurement_entries[UKF_DYNAMIC_DIM * 1];
	float sigma_measurements_entries[UKF_MEASUREMENT_DIM * UKF_SIGMA_FACTOR];
	float sigma_measurement_error_entries[UKF_MEASUREMENT_DIM * UKF_SIGMA_FACTOR];
	float cross_covariance_entries[UKF_STATE_DIM * UKF_MEASUREMENT_DIM];
	float aukf_kalman_gain_entries[UKF_STATE_DIM * 1];
	float g1_create_sigma_point_entries[UKF_STATE_DIM * UKF_STATE_DIM];
	float g2_create_sigma_point_entries[UKF_STATE_DIM * UKF_STATE_DIM];
	float m_update_sigma_state_entries[UKF_STATE_DIM * 1];
	float g_update_sigma_state_entries[UKF_STATE_DIM * UKF_SIGMA_FACTOR];
	float m1_time_update_state_covariance_entries[UKF_STATE_DIM * 1];
	float m2_time_update_state_covariance_entries[1 * UKF_STATE_DIM];
	float m3_time_update_state_covariance_entries[UKF_STATE_DIM * UKF_STATE_DIM];
	float g_update_sigma_measurement_entries[UKF_MEASUREMENT_DIM * UKF_SIGMA_FACTOR];
	float g_update_measurement_covariance_entries[UKF_MEASUREMENT_DIM * UKF_SIGMA_FACTOR];
	float m1_update_cross_covariance_entries[UKF_STATE_DIM * 1];
	float m_update_state_entries[UKF_STATE_DIM * 1];
	float t_update_state_covariance_entries[1 * UKF_STATE_DIM];
	float m_update_state_covariance_entries[UKF_STATE_DIM * UKF_STATE_DIM];

};

typedef struct SOC_UKF_t SOC_UKF;
struct SOC_UKF_t{
	SOC_Input input;
	SOC_Output output;
	SOC_State state;
	SOC_Filter filter;
	SOC_parameter param;
	SOC_err err;
};

void load_soc(SOC_UKF* battery_soc, const float soc);
void ukf_init(const uint32_t pack_voltage, const int32_t pack_current, SOC_UKF* battery_soc);
uint8_t ukf_update(SOC_UKF* battery_soc, const float soh);

extern SOC_Parameter_Entries soc_entries;
extern SOC_UKF bms_soc;

#endif /* SERVICE_SOC_AUKF_SOC_UKF_H_ */
