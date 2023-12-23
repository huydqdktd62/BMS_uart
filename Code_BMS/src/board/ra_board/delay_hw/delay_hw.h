/*
 * delay_hw.h
 *
 *  Created on: Aug 22, 2020
 *      Author: quangnd
 */

#ifndef BOARD_STM32_BSP_DELAY_HW_DELAY_HW_H_
#define BOARD_STM32_BSP_DELAY_HW_DELAY_HW_H_
#include "stdint.h"

void hw_delay_us(uint32_t us);
void hw_delay_ms(uint32_t ms);
void sw_delay_ms(uint32_t ms);
void sw_delay_us(uint32_t us);

#endif /* BOARD_STM32_BSP_DELAY_HW_DELAY_HW_H_ */
