/*
 * soh_lsb.c
 *
 *  Created on: Jul 14, 2023
 *      Author: ADMIN
 */

#include "soh_lsb.h"
#include "math_util.h"
#include "stdlib.h"
#include "math.h"
#include "../bms_soc_soh_app/bms_soc_soh_init.h"

void bms_set_soh(SOH_Estimator* p_est, SOH_Save_Data* save_data){
	p_est->c1 = (float)save_data->c1/SOH_NORMALIZED_GAIN;
	p_est->c2 = (float)save_data->c2/SOH_NORMALIZED_GAIN;
	p_est->c3 = (float)save_data->c3/SOH_NORMALIZED_GAIN;
	p_est->last_soc = (float)save_data->last_soc/SOH_NORMALIZED_GAIN;
	p_est->delta_x = (float)save_data->delta_x/SOH_NORMALIZED_GAIN;
	p_est->delta_y = (float)save_data->delta_y/SOH_NORMALIZED_GAIN;
	p_est->est_capacity = (float)save_data->est_capacity/SOH_NORMALIZED_GAIN;
	p_est->cnt = save_data->cnt;
	p_est->soh = (float)save_data->soh/SOH_NORMALIZED_GAIN;
}
void bms_save_soh(SOH_Estimator* p_est, SOH_Save_Data* save_data){
	save_data->c1 = (int32_t)(p_est->c1 * SOH_NORMALIZED_GAIN);
	save_data->c2 = (int32_t)(p_est->c2 * SOH_NORMALIZED_GAIN);
	save_data->c3 = (int32_t)(p_est->c3 * SOH_NORMALIZED_GAIN);
	save_data->last_soc = (int32_t)(p_est->last_soc * SOH_NORMALIZED_GAIN);
	save_data->delta_x = (int32_t)(p_est->delta_x * SOH_NORMALIZED_GAIN);
	save_data->delta_y = (int32_t)(p_est->delta_y * SOH_NORMALIZED_GAIN);
	save_data->est_capacity = (int32_t)(p_est->est_capacity * SOH_NORMALIZED_GAIN);
	save_data->cnt = p_est->cnt;
	save_data->soh = (int32_t)(p_est->soh * SOH_NORMALIZED_GAIN);
}

float old_soh;
void bms_update_soh(SOH_Estimator* p_est, float soc, int32_t current) {
	float a;
	p_est->input_soc = roundf(soc/SOC_RESOLUTION)*SOC_RESOLUTION;
	p_est->input_current = (float)current/PACK_CURRENT_NORMALIZED_GAIN;
	if(current < 0){
		p_est->input_current = p_est->input_current * SOH_DISCHARGE_ETA_RATIO;
	}
	p_est->delta_y -= p_est->input_current*SOH_SAMPLE_TIME_s/HOUR_TO_SECOND;
	if(abs(current) > SOH_LOWER_CURRENT_THRESHOLD){
		p_est->cnt++;
	}
	if (p_est->cnt == SOH_PERIOD) {
		old_soh = p_est->soh;
		p_est->delta_x = p_est->input_soc - p_est->last_soc;
        p_est->last_soc = p_est->input_soc;
        p_est->c1 = FADING_FACTOR
                * p_est->c1 + p_est->delta_x*p_est->delta_x/SIGMA_Y;
        p_est->c2 = FADING_FACTOR
                * p_est->c2 + p_est->delta_x*p_est->delta_y/SIGMA_Y;
        p_est->c3 = FADING_FACTOR
                * p_est->c3 + p_est->delta_y*p_est->delta_y/SIGMA_Y;
        a = (p_est->c1 - K_SIGMA*K_SIGMA*p_est->c3) / (2*K_SIGMA*K_SIGMA*p_est->c2);
        p_est->est_capacity = -a + square_root_f(a * a + 1.0f/(K_SIGMA*K_SIGMA));
        p_est->soh = 100.0f * p_est->est_capacity * HOUR_TO_SECOND / SOH_NOMINAL_CAPACITY_AS;

		if (p_est->est_capacity > SOH_NOMINAL_CAPACITY_AS / HOUR_TO_SECOND) {
			p_est->est_capacity = SOH_NOMINAL_CAPACITY_AS / HOUR_TO_SECOND;
		}
		if (p_est->est_capacity <= 0){
			p_est->est_capacity = SOH_NOMINAL_CAPACITY_AS / (HOUR_TO_SECOND * 100.0f);
		}
        if(old_soh - p_est->soh > 1.0f)
        {
          p_est->soh = old_soh - 1.0f;
        }
        if(old_soh - p_est->soh < -1.0f)
        {
          p_est->soh = old_soh + 1.0f;
        }
        if (p_est->soh > UKF_SOH_UPPER_THRESHOLD)
        {
          p_est->soh = UKF_SOH_UPPER_THRESHOLD;
        }
        if (p_est->soh < 50.0f)
        {
          p_est->soh = 50.0f;
        }
//        bms_save_soh(&bms_soh, &soh_save_data);
//        uart_print ("%d,%d,%d,", (int32_t) soh_save_data.c1, (int32_t) soh_save_data.c2, (int32_t) soh_save_data.c3);
//        uart_print ("%d,%d,%d,%d,", (int32_t) soh_save_data.est_capacity, (int32_t) soh_save_data.soh,
//                    (int32_t) soh_save_data.delta_x, (int32_t) soh_save_data.delta_y);
//        uart_print ("%d,%d,%d,\n", (int32_t) soh_save_data.last_soc, (int32_t) soh_save_data.cnt,
//                    (int32_t) soh_save_data.key);

		p_est->delta_y = 0.0f;
		p_est->cnt = 0;


	}
}

uint32_t bms_get_soh_i(SOH_Estimator* p_est){
	return (uint32_t)(p_est->soh);
}
float bms_get_soh_f(SOH_Estimator* p_est){
	return p_est->soh;
}

