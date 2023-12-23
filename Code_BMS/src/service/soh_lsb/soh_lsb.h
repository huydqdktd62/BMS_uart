/*
 * soh_lsb.h
 *
 *  Created on: Jul 14, 2023
 *      Author: ADMIN
 */

#ifndef SERVICE_SOH_LSB_SOH_LSB_H_
#define SERVICE_SOH_LSB_SOH_LSB_H_

#include "stdint.h"
#include "soh_lsb_config.h"

typedef struct SOH_Save_Data_t SOH_Save_Data;
struct SOH_Save_Data_t{
	int64_t last_soc;
	int64_t delta_x;
	int64_t delta_y;
	int64_t est_capacity;
	int64_t c1, c2, c3;
	int64_t soh;
	int16_t cnt;
	uint64_t key;
};

typedef struct SOH_Estimator_t SOH_Estimator;
struct SOH_Estimator_t{
	float input_current;
	float input_soc;
	float last_soc;
	float delta_x;
	float delta_y;
	float est_capacity;
	float c1, c2, c3;
	float soh;
	int16_t cnt;
};

void bms_set_soh(SOH_Estimator* p_est, SOH_Save_Data* save_data);
void bms_update_soh(SOH_Estimator* p_est, float soc, int32_t current);
void bms_save_soh(SOH_Estimator* p_est, SOH_Save_Data* save_data);
uint32_t bms_get_soh_i(SOH_Estimator* p_est);
float bms_get_soh_f(SOH_Estimator* p_est);

//extern SOH_Estimator bms_soh;
//extern SOH_Save_Data soh_save_data;

#endif /* SERVICE_SOH_LSB_SOH_LSB_H_ */
