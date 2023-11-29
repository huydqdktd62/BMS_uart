/*
 * soh_lsb_config.h
 *
 *  Created on: Jul 31, 2023
 *      Author: ADMIN
 */

#ifndef SERVICE_SOH_LSB_SOH_LSB_CONFIG_H_
#define SERVICE_SOH_LSB_SOH_LSB_CONFIG_H_

#include "soc_ukf_config.h"

#define SOH_PERIOD						(300)
#define SOC_RESOLUTION					(0.001f)
#define SIGMA_Y							((float)SOH_PERIOD*0.0025f*0.0025f)
#define SIGMA_X							(2.0f*SOC_RESOLUTION*SOC_RESOLUTION)
#define K_SIGMA							(square_root_f(SIGMA_X/SIGMA_Y))
#define FADING_FACTOR					(0.999f)
#define HOUR_TO_SECOND					(3600.0f)
#define SOH_NORMALIZED_GAIN				(1000.0f)
#define SOH_DISCHARGE_ETA_RATIO			UKF_DISCHARGE_ETA_RATIO
#define SOH_SAMPLE_TIME_s				UKF_SAMPLE_TIME_s
#define SOH_LOWER_CURRENT_THRESHOLD		UKF_LOWER_CURRENT_THRESHOLD
#define SOH_NOMIMAL_CAPACITY_AS			UKF_NOMIMAL_CAPACITY_AS

#define SOH_LSB_C1_INIT					(66667)
#define SOH_LSB_C2_INIT					(298667)
#define SOH_LSB_C3_INIT					(1338027)
#define SOH_LSB_EST_CAPACITY_INIT		(4800)
#define SOH_LSB_SOH_INIT				(100000)



#endif /* SERVICE_SOH_LSB_SOH_LSB_CONFIG_H_ */