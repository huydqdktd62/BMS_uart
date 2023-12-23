/*
 * inrush_limiter_hardware.h
 *
 *  Created on: Aug 25, 2020
 *      Author: quangnd
 */

#ifndef BOARD_STM32_BSP_INRUSH_LIMITER_HARDWARE_INRUSH_LIMITER_HARDWARE_H_
#define BOARD_STM32_BSP_INRUSH_LIMITER_HARDWARE_INRUSH_LIMITER_HARDWARE_H_

void inrush_limter_hw_init(void);
void inrush_limiter_switch_on(void);
void inrush_limiter_switch_off(void);

#endif /* BOARD_STM32_BSP_INRUSH_LIMITER_HARDWARE_INRUSH_LIMITER_HARDWARE_H_ */
