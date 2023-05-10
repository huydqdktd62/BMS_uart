/*
 * csv_loader.h
 *
 *  Created on: May 6, 2022
 *      Author: quangnd
 */

#ifndef CSV_LOADER_H_
#define CSV_LOADER_H_
#include "app_config.h"
#include "data_logger.h"

#include "stdint.h"

typedef struct BMS_Input_Vector_t BMS_Input_Vector;
#if TEST_SOC
struct BMS_Input_Vector_t{
	float terminalVoltage;
	float current;
	uint32_t cycles;
};
#else
struct BMS_Input_Vector_t{
	Data_Logger data_logger;
};
#endif

int32_t load_battery_data_from_csv_file(const char* path,BMS_Input_Vector *buff,const uint32_t max_size);
int32_t load_soh_data_from_csv_file(const char* path, float *buff,const uint32_t max_size);

#endif /* CSV_LOADER_H_ */
