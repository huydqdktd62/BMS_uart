/*
 * modbus_app.h
 *
 *  Created on: Mar 28, 2023
 *      Author: Admin
 */

#ifndef APP_MODBUS_APP_H_
#define APP_MODBUS_APP_H_

#include "port.h"

#define REG_INPUT_START 1
#define REG_INPUT_NREGS 36

#define REG_HOLDING_START 1
#define REG_HOLDING_NREGS 10

void mb_app_init();
void mb_app_run();

extern USHORT usRegInputBuf[REG_INPUT_NREGS];
extern USHORT usRegHoldingBuf[REG_HOLDING_NREGS];

#endif /* APP_MODBUS_APP_H_ */
