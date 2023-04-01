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

extern void* tpdo_build_data_impl[TPDO_NUMBER];

void app_co_init(void);
void app_co_can_receive(const uint32_t can_id, uint8_t* data);

#endif /* APP_APP_CANOPEN_APP_CO_INIT_H_ */

