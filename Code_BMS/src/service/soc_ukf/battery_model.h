/*
 * battery_model.h
 *
 *  Created on: Feb 6, 2023
 *      Author: ADMIN
 */

#ifndef SERVICE_SOC_AUKF_BATTERY_MODEL_H_
#define SERVICE_SOC_AUKF_BATTERY_MODEL_H_

#include "../soc_ukf/soc_ukf_config.h"
#include <stdint.h>

#define MODEL_LG_LUT_SIZE				201
#define MODEL_LG_R0_SIZE				11

typedef struct Battery_Model_t Battery_Model;
struct Battery_Model_t{
	const float* p_r0_lut_values;
	const float* p_terminal_lut_values;
	int16_t r0_size;
	float R1;
	float C1;
	const float* p_soc_lut_values;
	const float* p_ocv_lut_values;
	const float* p_derivative_lut_values;
	int16_t soc_size;
};

float model_get_r0_from_voltage(Battery_Model* model, float voltage);
float model_get_soc_from_ocv(Battery_Model* model, float ocv);
float model_get_ocv_from_soc(Battery_Model* model, float soc);
float model_get_ratio_from_soc(Battery_Model* model, float soc);

extern Battery_Model LG_Model;

#endif /* SERVICE_SOC_AUKF_BATTERY_MODEL_H_ */
