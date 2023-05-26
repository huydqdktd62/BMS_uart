/*
 * battery_model.h
 *
 *  Created on: Feb 6, 2023
 *      Author: ADMIN
 */

#ifndef SERVICE_SOC_AUKF_BATTERY_MODEL_H_
#define SERVICE_SOC_AUKF_BATTERY_MODEL_H_

#include "../soc_ukf/soc_ukf_config.h"

#define MODEL_SOC_LUT_SIZE				201
#define MODEL_OCV_LUT_SIZE				201
#define MODEL_DERIVATIVE_LUT_SIZE		201

float get_soc_from_ocv(float ocv);
float get_ocv_from_soc(float soc);
float get_ratio_from_soc(float soc);

#endif /* SERVICE_SOC_AUKF_BATTERY_MODEL_H_ */
