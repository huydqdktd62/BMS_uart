/*
 * soc_ukf_config.h
 *
 *  Created on: May 8, 2023
 *      Author: ADMIN
 */

#ifndef SERVICE_SOC_UKF_REFACTOR_SOC_UKF_CONFIG_H_
#define SERVICE_SOC_UKF_REFACTOR_SOC_UKF_CONFIG_H_

#define LFP											0
#define INR											1
#define NORMALIZED_CODE								1

#define ZERO										(0.0f)
#define ONE											(1.0f)

#define CNT_10_MINUTE_10mS							(60000)
#define CNT_3_MINUTE_10mS							(18000)
#define APP_STATE_MACHINE_UPDATE_TICK_mS			(10)

/*
 * Dimensional parameters...
 */
#define UKF_STATE_DIM								3
#define UKF_SIGMA_FACTOR							(UKF_STATE_DIM*2+1)
#define UKF_MEASUREMENT_DIM							1
#define UKF_DYNAMIC_DIM								2
#define UKF_SINGLE_DIM								1

/*
 * Battery Model parameters...
 */
#if INR
#define UKF_R0_INIT_Omh								(0.082f)
#define UKF_R1_INIT_Omh								(0.0303f)
#define UKF_C1_INIT_F								(float)(25.8403f*UKF_SAMPLE_TIME_s)
#define UKF_GAMMA_RATIO								(0.017320508f)
#define UKF_NOMIMAL_CAPACITY_AS						(15552.0f)//LG cell 17280, Molicel 14400
#define UKF_CHARGE_ETA_RATIO						(1.0f)
#define UKF_DISCHARGE_ETA_RATIO						(1.03092783505f)
#define UKF_HYSTERESIS_V							(0.062f)
#define UKF_HYSTERESIS_ZERO_V						(0.0025f)
#define UKF_CAPACITY_RATIO							(61.7498f)
#define PACK_VOLTAGE_NORMALIZED_GAIN				(16000.0f)
#define CELL_VOLTAGE_NORMALIZED_GAIN				(1000.0f)
#define PACK_CURRENT_NORMALIZED_GAIN				(-400.0f)
#endif

#if LFP
#define UKF_R0_INIT_Omh								(0.012f)
#define UKF_R1_INIT_Omh								(0.0303f)
#define UKF_C1_INIT_F								(float)(25.8403f*UKF_SAMPLE_TIME_s)
#define UKF_GAMMA_RATIO								(0.017320508f)
#define UKF_NOMIMAL_CAPACITY_AS						(95760.0f)//LG cell 17280, Molicel 14400
#define UKF_CHARGE_ETA_RATIO						(1.0f)
#define UKF_DISCHARGE_ETA_RATIO						(1.03092783505f)
#define UKF_HYSTERESIS_V							(0.012f)
#define UKF_HYSTERESIS_ZERO_V						(0.0012f)
#define UKF_CAPACITY_RATIO							(61.7498f)
#define PACK_VOLTAGE_NORMALIZED_GAIN				(1000.0f)
#define CELL_VOLTAGE_NORMALIZED_GAIN				(1000.0f)
#define PACK_CURRENT_NORMALIZED_GAIN				(-1000.0f)
#endif

/*
 * UKF Algorithm parameters...
 */
#define SOC_NORMALIZED_GAIN							(100.0f)
#define SOC_PERIOD									(50)
#define UKF_DEFAULT_MEASUREMENT_COVARIANCE			(0.4472f)
#define UKF_SOC_MAX_ERROR_percent					(10.0f)
#define UKF_SAMPLE_TIME_s							((APP_STATE_MACHINE_UPDATE_TICK_mS*SOC_PERIOD)*0.001)

#define UKF_EST_STATE_ENTRY_1_INIT_VALUE			(0.0f)
#define UKF_EST_STATE_ENTRY_2_INIT_VALUE			(0.0f)
#define UKF_MATRIX_A_ENTRY_0_INIT_VALUE				(1.0f)
#define UKF_MATRIX_A_ENTRY_8_INIT_VALUE				(1.0f)
#define UKF_UNIT_F									(1.0f)
#define UKF_OBSERVED_CURRENT_THRESHOLD				(0.001f)
#define UKF_EST_STATE_ENTRY_0_UPPER_LIMIT			(1.0f)
#define UKF_EST_STATE_ENTRY_0_LOWER_LIMIT			(0.0f)
#define UKF_EST_STATE_ENTRY_2_UPPER_LIMIT			(1.0f)
#define UKF_EST_STATE_ENTRY_2_LOWER_LIMIT			(-1.0f)
#define BMS_SOC_UPPER_LIMIT							(100)

#define COULOMBCOUNTER_VOLTAGE_THRESHOLD			(66500)
#define COULOMBCOUNTER_LOWER_CURRENT_THRESHOLD		(0)
#define COULOMBCOUNTER_UPPER_CURRENT_THRESHOLD		(300)
#define COULOMBCOUNTER_UPPER_EST_STATE_THRESHOLD	(0.9975f)

#define UKF_LOWER_EST_STATE_THRESHOLD				(0.0145f)
#define UKF_LOWER_CURRENT_THRESHOLD					(20)

#define UKF_SLEEP_CURRENT_THRESHOLD					(55.0f)
#define UKF_CALIB_CURRENT_THRESHOLD					(60.0f)

#endif /* SERVICE_SOC_UKF_REFACTOR_SOC_UKF_CONFIG_H_ */
