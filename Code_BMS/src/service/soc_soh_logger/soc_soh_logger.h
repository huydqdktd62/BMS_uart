/*
 * soc_soh_logger.h
 *
 *  Created on: Oct 27, 2022
 *      Author: Admin
 */

#ifndef SERVICE_SOC_SOH_LOGGER_SOC_SOH_LOGGER_H_
#define SERVICE_SOC_SOH_LOGGER_SOC_SOH_LOGGER_H_

#include "exflash/exflash.h"
#include "soh_lsb.h"

#define SECTOR_SAVE_SOC_SOH 0

typedef struct SOC_SOH_Logger_t SOC_SOH_Logger;

struct SOC_SOH_Logger_t{
	w25qxx_t *memory;
	uint32_t addr;
	uint32_t capacity;
	uint32_t offset_byte;
};

typedef struct SOC_SOH_Value_t SOC_SOH_Value;

struct SOC_SOH_Value_t{
	uint32_t soc;
	uint32_t soh;
	uint64_t key;
};

void SOC_SOH_Logger_Init(SOC_SOH_Logger *p_logger);
int SOC_SOH_Save(SOC_SOH_Logger *p_logger,SOH_Save_Data value);
int SOC_SOH_Load(SOC_SOH_Logger *p_logger,SOH_Save_Data *value);

extern SOC_SOH_Logger bms_soc_logger;

#endif /* SERVICE_SOC_SOH_LOGGER_SOC_SOH_LOGGER_H_ */
