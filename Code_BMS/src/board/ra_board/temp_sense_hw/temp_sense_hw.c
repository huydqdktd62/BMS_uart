/*
 * temp_sense_hardware.c
 *
 *  Created on: Aug 24, 2020
 *      Author: quangnd
 */
#include <current_sense_hw.h>
#include <temp_sense_hw.h>
int32_t temp_sense_read_adc(void) {
        return (0);
}

int32_t temp_sense_read_offset(void) {
        return 0;
}

uint16_t mosfet_ntc_read_impedance(void) {
        int32_t temp_sense_adc = 0;
        temp_sense_adc = (int32_t) filtered_adc_values[ADC_TS6_ID];
        uint32_t v_temp_fet_uV = 805 * (uint32_t) temp_sense_adc / 1000; // 1 LSB approximate 805uV
        uint32_t v_ref_uV = 3300;
        if (v_temp_fet_uV == v_ref_uV) {
                return 0;
        }
        return (uint16_t)( (10000 * v_temp_fet_uV) / (v_ref_uV - v_temp_fet_uV) );
}
