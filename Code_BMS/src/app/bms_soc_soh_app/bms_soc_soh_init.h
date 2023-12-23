/*
 * bms_soc_init.h
 *
 *  Created on: May 31, 2023
 *      Author: ADMIN
 */

#ifndef APP_BMS_SOC_APP_BMS_SOC_INIT_H_
#define APP_BMS_SOC_APP_BMS_SOC_INIT_H_
#include "../soc_ukf/soc_ukf.h"
#include "../soh_lsb/soh_lsb.h"

void bms_soc_init(const uint32_t pack_voltage);
void bms_soh_init(SOH_Estimator* p_est, float soc_value, SOH_Save_Data* save_data);
extern SOC_UKF bms_soc;
extern SOC_Parameter soc_parameter;
extern SOH_Save_Data soh_save_data;
extern SOH_Estimator bms_soh;

#endif /* APP_BMS_SOC_APP_BMS_SOC_INIT_H_ */
