/*
 * manu_od.h
 *
 *  Created on: Oct 12, 2022
 *      Author: Dakaka
 */

#ifndef APP_APP_CO_OD_MANU_OD_MANU_OD_H_
#define APP_APP_CO_OD_MANU_OD_MANU_OD_H_

#include "manu_od_default_value.h"

extern CO_Sub_Object obj_5000_basic_elec_specs[];
extern CO_Sub_Object obj_5001_basic_co_node_para[];
/* USER CODE 0 begin */

extern CO_Sub_Object obj_2110_maintnc[];
extern CO_Sub_Object obj_2120_throttle[];
extern CO_Sub_Object obj_2130_acc_profile_mode[];
extern CO_Sub_Object obj_2140_maximum_speed[];

/* Test */
typedef struct
{
	uint8_t test1;
	uint16_t test2;
	uint32_t test3;
}test_t;
extern test_t od_data_test_mapping_tpdo;
extern uint8_t od_data_test_expe_sdo[2];
extern uint8_t od_data_test_normal_sdo[100];

extern CO_Sub_Object obj_3FF7_test_mapping_tpdo[];
extern CO_Sub_Object obj_3FF8_test_expe_sdo[];
extern CO_Sub_Object obj_3FF9_test_normal_sdo[];

/* USER CODE 0 end */


#endif /* APP_APP_CO_OD_MANU_OD_MANU_OD_H_ */
