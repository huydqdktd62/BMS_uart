/*
 * inrush_limiter_hardware.c
 *
 *  Created on: Aug 25, 2020
 *      Author: quangnd
 */

#include <inrush_limiter_hw/inrush_limiter_hw.h>
#include "hal_data.h"
#include "bq_hal.h"
#include "bq769x2.h"
#include "afe_init.h"
#define INRUSH_CURRENT_LIMITER_FET_CTRL_PIN BSP_IO_PORT_01_PIN_04
#define FET_CTRL_ON_LEVEL       BSP_IO_LEVEL_HIGH
#define FET_CTRL_OFF_LEVEL      BSP_IO_LEVEL_LOW
int32_t soft_cnt = 0;;
void inrush_limter_hw_init(void) {

}

void inrush_limiter_switch_on(void){
	//R_IOPORT_PinWrite(&g_ioport_ctrl, INRUSH_CURRENT_LIMITER_FET_CTRL_PIN,FET_CTRL_ON_LEVEL);
	bq_i2c_subcommand_write_byte(bq_afe.hw,FET_CONTROL,0x02);
	soft_cnt ++;
}

void inrush_limiter_switch_off(void){

}

