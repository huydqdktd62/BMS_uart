/*
 * app.h
 *
 *  Created on: Mar 28, 2023
 *      Author: Admin
 */

#ifndef APP_APP_H_
#define APP_APP_H_

#include "app_co_init.h"
#include "CO_slave.h"
#include "can_master.h"
#include "can2mb_app_init.h"

void app_init();
void app_task(void *arg);
void app_scan_cabinets_task(void *arg);
#endif /* APP_APP_H_ */
