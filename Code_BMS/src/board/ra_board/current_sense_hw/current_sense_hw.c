/*
 * current_sense_hardware.c
 *
 *  Created on: Jan 24, 2022
 *      Author: quangnd
 */

#include "current_sense_hw.h"

#include "stdbool.h"

#include "string.h"
#include "stdio.h"
#include "hal_data.h"
#include "debug_print.h"
bool scan_adc_end = false;
volatile uint32_t filtered_adc_values[ADC_NUM_CHANNEL];
uint16_t current_adc_sample[ADC_NUM_CHANNEL] = { 0 };
int32_t ts5_temp_sense_read_adc(void) {
	return (int32_t) filtered_adc_values[ADC_TS5_ID];
}

int32_t ts6_temp_sense_read_adc(void) {
	return (int32_t) filtered_adc_values[ADC_TS6_ID];
}

uint16_t ts5_ntc_read_impedance(void) {
	uint32_t temp_sense_adc = 0;
	temp_sense_adc = filtered_adc_values[ADC_TS5_ID];
	uint32_t v_temp_fet_uV = 805 * (uint32_t) temp_sense_adc / 1000; // 1 LSB approximate 805uV
	uint32_t v_ref_uV = 3300;
	if (v_temp_fet_uV == v_ref_uV) {
		return 0;
	}
	return (uint16_t)((10000 * v_temp_fet_uV) / (v_ref_uV - v_temp_fet_uV));
}

uint16_t ts6_ntc_read_impedance(void) {
	uint32_t temp_sense_adc = 0;
	temp_sense_adc = filtered_adc_values[ADC_TS6_ID];
	uint32_t v_temp_fet_uV = 805 * (uint32_t) temp_sense_adc / 1000; // 1 LSB approximate 805uV
	uint32_t v_ref_uV = 3300;
	if (v_temp_fet_uV == v_ref_uV) {
		return 0;
	}
	return (uint16_t) ((10000 * v_temp_fet_uV) / (v_ref_uV - v_temp_fet_uV));
}

uint8_t get_scan_status() {
	if (scan_adc_end == true)
		return 1;
	return 0;
}
void set_scan_status(uint8_t state) {
	switch (state) {
	case 0:
		scan_adc_end = false;
		break;
	default:
		scan_adc_end = true;
		break;
	}
}
void temp_sense_hw_init(void) {
	R_ADC_Open(&g_adc0_ctrl, &g_adc0_cfg);
	R_ADC_ScanCfg(&g_adc0_ctrl, &g_adc0_channel_cfg);
	R_ADC_ScanStart(&g_adc0_ctrl);
}
void ts_ntc_update(void){
	//if (get_scan_status() == 1) {
		R_ADC_ScanStart(&g_adc0_ctrl);
		R_ADC_Read(&g_adc0_ctrl, ADC_CHANNEL_0, &current_adc_sample[0]);
//		R_ADC_ScanStart(&g_adc0_ctrl);
		R_ADC_Read(&g_adc0_ctrl, ADC_CHANNEL_1, &current_adc_sample[1]);
		//set_scan_status(0);

		filtered_adc_values[0] = (uint32_t) current_adc_sample[0];
		filtered_adc_values[1] = (uint32_t) current_adc_sample[1];
		//debug_printf("ADC0: %d\r\n",current_adc_sample[0]);
		//debug_printf("ADC1: %d\r\n",current_adc_sample[1]);
	//}
}
/* Callback function */
void adc_end_scan_handle(adc_callback_args_t *p_args) {
	/* TODO: add your own code here */
	(void) p_args;
	set_scan_status(1);
}
