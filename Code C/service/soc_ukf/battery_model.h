/*
 * battery_model.h
 *
 *  Created on: Feb 6, 2023
 *      Author: ADMIN
 */

#ifndef SERVICE_SOC_AUKF_BATTERY_MODEL_H_
#define SERVICE_SOC_AUKF_BATTERY_MODEL_H_

#define CNT_10_MINUTE_10mS					60000
#define CNT_3_MINUTE_10mS					18000
#define APP_STATE_MACHINE_UPDATE_TICK_mS	(10)
#define SOC_PERIOD							(70)
#define SOC_MAX_ERROR						(10.0f)
#define SAMPLE_TIME_s						((APP_STATE_MACHINE_UPDATE_TICK_mS*SOC_PERIOD)*0.001)
#define GAMMA								(0.017320508f)
#define ETA									(1.03092783505f)
#define NOMIMAL_CAPACITY					(16000.0f)//LG cell 17280, Molicel 14400
#define HYSTERESIS							(0.062f)
#define HYSTERESIS_ZERO						(0.0025f)
#define CAPACITY_RATIO						(61.7498f)

#define MODEL_SOC_LUT_SIZE				201
#define MODEL_OCV_LUT_SIZE				201
#define MODEL_DERIVATIVE_LUT_SIZE		201
#define R0_INIT							(0.082f)
#define R1_INIT							(0.0303f)
#define C1_INIT							(float)(25.8403f*SAMPLE_TIME_s)

#define PACK_VOLTAGE_NORMALIZED		(16000.0f)
#define CELL_VOLTAGE_NORMALIZED		(1000.0f)
#define PACK_CURRENT_NORMALIZED		(-400.0f)

float get_soc_from_ocv(float ocv);
float get_ocv_from_soc(float soc);
float get_ratio_from_soc(float soc);

#endif /* SERVICE_SOC_AUKF_BATTERY_MODEL_H_ */
