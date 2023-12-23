/*
 * interrupt_hw.c
 *
 *  Created on: Jan 24, 2022
 *      Author: quangnd
 */

#include "interrupt_hw.h"
#include "hal_data.h"
#include "current_sense_hal.h"

/* Callback function */
void sys_tick_hanlde(timer_callback_args_t *p_args)
{
	/* TODO: add your own code here */
	(void)p_args;
	ts_ntc_update();
	SysTick_Handler();
}
