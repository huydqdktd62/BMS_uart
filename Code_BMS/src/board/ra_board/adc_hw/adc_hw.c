/*
 * adc.c
 *
 *  Created on: Dec 15, 2021
 *      Author: VanCong
 */

#include "adc_hw.h"

void adc_hardware_init(void){
    R_ADC_Open(&g_adc0_ctrl, &g_adc0_cfg);
    R_ADC_ScanCfg(&g_adc0_ctrl, &g_adc0_channel_cfg);
}

/* Callback function */
void ADC_ISR(adc_callback_args_t *p_args)
{
	(void)p_args;
	/* TODO: add your own code here */
}
void adc_read_volt_charger(uint16_t * p_data ){
	R_ADC_ScanStart(&g_adc0_ctrl);
	R_ADC_Read (&g_adc0_ctrl, ADC_CHANNEL_16, p_data);
}
void adc_read_volt_au_bat(uint16_t * p_data ){
	R_ADC_ScanStart(&g_adc0_ctrl);
	R_ADC_Read (&g_adc0_ctrl, ADC_CHANNEL_11, p_data);
}
