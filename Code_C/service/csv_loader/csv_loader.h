/*
 * csv_loader.h
 *
 *  Created on: May 6, 2022
 *      Author: quangnd
 */

#ifndef CSV_LOADER_H_
#define CSV_LOADER_H_

#include "stdint.h"
#include "soh_lsb.h"

typedef struct BMS_Input_Vector_t BMS_Input_Vector;

struct BMS_Input_Vector_t{
	float terminalVoltage;
	float current;
};

typedef struct BMS_SOH_Vector_t BMS_SOH_Vector;
struct BMS_SOH_Vector_t{

};
#if 0
struct BMS_Input_Vector_t{
	Data_Logger data_logger;
};
#endif

int32_t load_battery_data_from_csv_file(const char* path,BMS_Input_Vector *buff,const uint32_t max_size);
int32_t load_soh_data_from_csv_file(const char* path, SOH_Save_Data *buff,const uint32_t max_size);

#endif /* CSV_LOADER_H_ */
