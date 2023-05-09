/*
 * model.h
 *
 *  Created on: Jul 1, 2022
 *      Author: ADMIN
 */

#if 0
#ifndef SERVICE_BATTERY_MODEL_H_
#define SERVICE_BATTERY_MODEL_H_

#define SAMPLE_TIME						(0.7f)
#define GAMMA							(0.017320508f)
#define ETA								(1.03092783505f)
#define NOMIMAL_CAPACITY				(17280.0f)//LG cell 17280, Molicel 14400
#define HYSTERESIS						(0.062f)
#define HYSTERESIS_ZERO					(0.0025f)
#define CAPACITY_RATIO					(61.7498f)

#define MODEL_SOC_LUT_SIZE				200//201
#define MODEL_OCV_LUT_SIZE				200//201
#define MODEL_DERIVATIVE_LUT_SIZE		200//201
#define R0_INIT							(0.0082f)
#define R1_INIT							(0.0303f)
#define C1_INIT							(25.8403f*SAMPLE_TIME)

#define PACK_VOLTAGE_NORMALIZED		(16000.0f)
#define CELL_VOLTAGE_NORMALIZED		(1000.0f)
#define CURRENT_NORMALIZED			(-400.0f)

float get_soc_from_ocv(float ocv);
float get_ocv_from_soc(float soc);
float get_ratio_from_soc(float soc);

#endif /* SERVICE_BATTERY_MODEL_H_ */
#endif
