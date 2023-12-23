/*
 * current_sense_hardware.h
 *
 *  Created on: Jan 24, 2022
 *      Author: quangnd
 */

#ifndef BOARD_RA_BOARD_CURRENT_SENSE_HW_CURRENT_SENSE_HW_H_
#define BOARD_RA_BOARD_CURRENT_SENSE_HW_CURRENT_SENSE_HW_H_
#include "stdint.h"

#define ADC_NUM_CHANNEL                 2
#define ADC_TS5_ID                      0
#define ADC_TS6_ID                      1
//#define ADC_TEMPERATURE_ID                2

extern volatile uint32_t filtered_adc_values[ADC_NUM_CHANNEL];

#define ADC_VREF_mV                     2500UL
#define CURRENT_SENSE_ADC_GAIN          1UL
#define CURRENT_SENSE_ADC_OFFSET        0UL
#define CURRENT_SENSE_R_SHUNT_mOhm          1
#define ADC_NUM_FILTERS                 32
#define                         VREF_ADR         ((uint16_t*)((uint32_t)0x1FFFF7BA))
#define                         FFT         0

#define ADC_VREF_uV                         3300000
#define ADC_RESOLUTION                      12
#define CURRENT_SENSE_OPAM_GAIN             20



int32_t ts5_temp_sense_read_adc(void);
int32_t ts6_temp_sense_read_adc(void);
uint16_t ts5_ntc_read_impedance(void);
uint16_t ts6_ntc_read_impedance(void);
void ts_ntc_update(void);
void temp_sense_hw_init(void);
uint8_t get_scan_status();
void set_scan_status(uint8_t state);
#endif /* BOARD_RA_BOARD_CURRENT_SENSE_HW_CURRENT_SENSE_HW_H_ */
