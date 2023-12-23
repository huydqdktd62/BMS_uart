/*
 * bms_soc_init.c
 *
 *  Created on: May 31, 2023
 *      Author: ADMIN
 */
#include "../bms_soc_soh_app/bms_soc_soh_init.h"

SOC_UKF bms_soc;
SOC_Parameter soc_parameter;
SOH_Save_Data soh_save_data;
SOH_Estimator bms_soh;
void bms_soc_init(const uint32_t pack_voltage){
	bms_soc.input.pack_voltage = pack_voltage;
    ukf_parameters_create(&soc_parameter);
    bms_soc.battery_model = &LG_Model;
	load_soc(&bms_soc, 100.0f * model_get_soc_from_ocv(bms_soc.battery_model,
							(float) bms_soc.input.pack_voltage
									/ PACK_VOLTAGE_NORMALIZED_GAIN));
	ukf_init(&bms_soc, 100.0f);
}

//void bms_soh_init(SOH_Estimator* p_est, float soc_value){
//	p_est->input_current = 0;
//	p_est->input_soc = soc_value;
//}

void bms_soh_init(SOH_Estimator* p_est, float soc_value, SOH_Save_Data* save_data){
	p_est->input_current = 0;
	p_est->input_soc = soc_value;
	bms_set_soh(p_est, save_data);
}
