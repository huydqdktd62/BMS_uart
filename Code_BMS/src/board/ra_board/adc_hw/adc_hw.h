/*
 * adc.h
 *
 *  Created on: Dec 15, 2021
 *      Author: VanCong
 */

#ifndef BOARD_RA4M2_BSP_ADC_HW_ADC_HW_H_
#define BOARD_RA4M2_BSP_ADC_HW_ADC_HW_H_

#include "hal_data.h"

void adc_hardware_init(void);

void adc_read_volt_charger( uint16_t * p_data );
void adc_read_volt_au_bat( uint16_t * p_data );


#endif /* BOARD_RA4M2_BSP_ADC_HW_ADC_HW_H_ */
