/*
 * app_co_init.h
 *
 *  Created on: Jun 17, 2022
 *      Author: Dakaka
 */

#ifndef APP_APP_CANOPEN_APP_CO_INIT_H_
#define APP_APP_CANOPEN_APP_CO_INIT_H_

#include "CO.h"
#include "od_config.h"
#include "can_hw.h"
extern void* tpdo_build_data_impl[TPDO_NUMBER];
extern CO app_co;

void app_co_init(void);
void co_send_ex_boot_request(CAN_Hw *p_hw);
#endif /* APP_APP_CANOPEN_APP_CO_INIT_H_ */

