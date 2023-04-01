/*
 * soc_ukf.h
 *
 *  Created on: Feb 15, 2023
 *      Author: ADMIN
 */

#ifndef SERVICE_SOC_AUKF_SOC_UKF_H_
#define SERVICE_SOC_AUKF_SOC_UKF_H_

#include <stdint.h>

#define STATE_DIM							3
#define SIGMA_FACTOR						(STATE_DIM*2+1)
#define MEASUREMENT_DIM						1
#define DYNAMIC_DIM							2
#define MEASUREMENT_COVARIANCE_DEFAULT		(0.4472f)

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

void ukf_init(const uint32_t pack_voltage, const int32_t pack_current);
uint8_t ukf_update(const SOC_Input input, const float soh);

extern float estimate_state_entries[STATE_DIM * 1];
extern float state_covariance_entries[STATE_DIM * STATE_DIM];
extern float sigma_points_entries[STATE_DIM * SIGMA_FACTOR];
extern float priori_estimate_state_entries[STATE_DIM * 1];
extern float matrix_A_entries[STATE_DIM * STATE_DIM];
extern float matrix_B_entries[STATE_DIM * DYNAMIC_DIM];
extern float matrix_C_entries[MEASUREMENT_DIM * STATE_DIM];
extern float matrix_D_entries[MEASUREMENT_DIM * DYNAMIC_DIM];
extern float sigma_state_error_entries[STATE_DIM * SIGMA_FACTOR];
extern float observed_measurement_entries[DYNAMIC_DIM * 1];
extern float sigma_measurements_entries[MEASUREMENT_DIM * SIGMA_FACTOR];
extern float sigma_measurement_error_entries[MEASUREMENT_DIM * SIGMA_FACTOR];
extern float cross_covariance_entries[STATE_DIM * MEASUREMENT_DIM];
extern float aukf_kalman_gain_entries[STATE_DIM * 1];
extern float est_measurement;
extern float measurement_cov;
extern float RC_param;
extern float H_param;
extern float cell_voltage, cell_current;

enum SOC_State{
	SOC_ST_INIT = 0,
	SOC_ST_IDLE,
	SOC_ST_UKF,
	SOC_ST_COULOMB_COUNTER,
	SOC_ST_CALIB,
	SOC_ST_SLEEP,
	SOC_ST_FAULT
};
typedef enum SOC_State SOC_State;
extern SOC_State state;

extern SOC_Input soc_input;
extern SOC_Output soc_output;





#endif /* SERVICE_SOC_AUKF_SOC_UKF_H_ */
