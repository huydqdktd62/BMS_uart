/*
 * temp_sense_hardware.h
 *
 *  Created on: Aug 24, 2020
 *      Author: quangnd
 */

#ifndef BOARD_STM32_BSP_TEMP_SENSE_HARDWARE_TEMP_SENSE_HARDWARE_H_
#define BOARD_STM32_BSP_TEMP_SENSE_HARDWARE_TEMP_SENSE_HARDWARE_H_

#include "stdint.h"
#include "math.h"

int32_t temp_sense_read_adc(void);
int32_t temp_sense_read_offset(void);
uint16_t mosfet_ntc_read_impedance(void);

#endif /* BOARD_STM32_BSP_TEMP_SENSE_HARDWARE_TEMP_SENSE_HARDWARE_H_ */
