/*
 * soc_soh_logger.c
 *
 *  Created on: Oct 27, 2022
 *      Author: Admin
 */


#include "soc_soh_logger.h"
#include "soh_lsb_config.h"
#include "sm_filesystem.h"
#include "sm_fileconfig.h"
#include "../bms_soc_soh_app/bms_soc_soh_init.h"
#include "stdio.h"
#include "cJSON.h"

uint8_t default_buff[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
#define KEY_VALUE 101504

static const char *file_name = "soc.json";

void SOC_SOH_Logger_Init(SOC_SOH_Logger *p_logger){
	/*	p_logger->addr = SECTOR_SAVE_SOC_SOH;
	p_logger->capacity = exflash.SectorSize;
	p_logger->memory = &exflash;
	p_logger->offset_byte = 0;
	while(p_logger->offset_byte < p_logger->capacity){
		uint8_t read_buff[sizeof(SOC_SOH_Value)];
	 bool result = W25qxx_ReadSector(p_logger->memory, (uint8_t*)read_buff,p_logger->addr, p_logger->offset_byte, sizeof(SOC_SOH_Value));
		//assert(result == true);
		SOC_SOH_Value *read_value = (SOC_SOH_Value*)read_buff;

		if(read_value->key != KEY_VALUE){
			break;
		}

		p_logger->offset_byte += sizeof(SOC_SOH_Value);
	}

	if(p_logger->offset_byte > p_logger->capacity || p_logger->offset_byte == 0){
		W25qxx_EraseSector(p_logger->memory,p_logger->addr);
		p_logger->offset_byte = 0;
	 }*/
}
int SOC_SOH_Save(SOC_SOH_Logger *p_logger, SOH_Save_Data value){
	/*	if((p_logger->offset_byte) > p_logger->capacity){
		W25qxx_EraseSector(p_logger->memory,p_logger->addr);
	}

	value.key = KEY_VALUE;
	if(W25qxx_WriteSector(p_logger->memory,(uint8_t*)&value,p_logger->addr, p_logger->offset_byte,sizeof(SOC_SOH_Value))){
		SOC_SOH_Value result;
		W25qxx_ReadSector(p_logger->memory,(uint8_t*)&result,p_logger->addr, p_logger->offset_byte, sizeof(SOC_SOH_Value));
		if(result.soc == value.soc && result.soh == value.soh && result.key == value.key){
			p_logger->offset_byte += sizeof(SOC_SOH_Value);
			return 0;
		}
	 }*/
	remove(file_name);
	FILE *file = fopen(file_name, "w");

	assert(file != NULL);

	cJSON *json = cJSON_CreateObject();
	// User code
	cJSON_AddNumberToObject(json, "last_soc", (double)value.last_soc);
	cJSON_AddNumberToObject(json, "delta_x", (double)value.delta_x);
	cJSON_AddNumberToObject(json, "delta_y", (double)value.delta_y);
	cJSON_AddNumberToObject(json, "est_capacity", (double)value.est_capacity);
	cJSON_AddNumberToObject(json, "c1", (double)value.c1);
	cJSON_AddNumberToObject(json, "c2", (double)value.c2);
	cJSON_AddNumberToObject(json, "c3", (double)value.c3);
	cJSON_AddNumberToObject(json, "soh", (double)value.soh);
	cJSON_AddNumberToObject(json, "cnt", (double)value.cnt);
	cJSON_AddNumberToObject(json, "key", (double)value.key);
	// End User code
	char json_buff[256];

	cJSON_PrintPreallocated(json, json_buff, 256, false);
	fwrite(json_buff, sizeof(char), strlen(json_buff), file);
	fclose(file);

	cJSON_Delete(json);

	return 0;
}
int SOC_SOH_Load(SOC_SOH_Logger *p_logger, SOH_Save_Data *value){
	/*	if(p_logger->offset_byte == 0){
		return -1;
	}
	SOC_SOH_Value read_buff;
	if(W25qxx_ReadSector(p_logger->memory, (uint8_t*)&read_buff,p_logger->addr, p_logger->offset_byte - sizeof(SOC_SOH_Value), sizeof(SOC_SOH_Value))){
		value->soc = read_buff.soc;
		value->soh = read_buff.soh;
		value->key = read_buff.key;
		return 0;
	 }*/
	FILE *file = fopen(file_name, "r");

	if (file == NULL) {
		// User code
		value->c1 = SOH_LSB_C1_INIT;
		value->c2 = SOH_LSB_C2_INIT;
		value->c3 = SOH_LSB_C3_INIT;
		value->est_capacity = SOH_LSB_EST_CAPACITY_INIT;
		value->soh = SOH_LSB_SOH_INIT;
		value->delta_x = 0;
		value->delta_y = 0;
		value->last_soc = (int64_t)(bms_soc.output.SOC_f*10);
		value->cnt = 0;
		value->key = KEY_VALUE;
		// End User code
		SOC_SOH_Save(p_logger, *value);
		return 1;
	}

	char json_buff[256];
	memset(json_buff, 0, 256);
	fread(json_buff, sizeof(char), 256, file);
	fclose(file);
	cJSON *json = NULL;
	json = cJSON_Parse(json_buff);
	assert(json);
	// User code
	cJSON *last_soc = cJSON_GetObjectItem(json, "last_soc");
	cJSON *delta_x = cJSON_GetObjectItem(json, "delta_x");
	cJSON *delta_y = cJSON_GetObjectItem(json, "delta_y");
	cJSON *est_capacity = cJSON_GetObjectItem(json, "est_capacity");
	cJSON *c1 = cJSON_GetObjectItem(json, "c1");
	cJSON *c2 = cJSON_GetObjectItem(json, "c2");
	cJSON *c3 = cJSON_GetObjectItem(json, "c3");
	cJSON *soh = cJSON_GetObjectItem(json, "soh");
	cJSON *cnt = cJSON_GetObjectItem(json, "cnt");
	cJSON *key = cJSON_GetObjectItem(json, "key");

	value->last_soc = (int64_t) cJSON_GetNumberValue(last_soc);
	value->delta_x = (int64_t) cJSON_GetNumberValue(delta_x);
	value->delta_y = (int64_t) cJSON_GetNumberValue(delta_y);
	value->est_capacity = (int64_t) cJSON_GetNumberValue(est_capacity);
	value->c1 = (int64_t) cJSON_GetNumberValue(c1);
	value->c2 = (int64_t) cJSON_GetNumberValue(c2);
	value->c3 = (int64_t) cJSON_GetNumberValue(c3);
	value->soh = (int64_t) cJSON_GetNumberValue(soh);
	value->cnt = (int16_t) cJSON_GetNumberValue(cnt);
	value->key = (uint64_t) cJSON_GetNumberValue(key);

	if(value->key != KEY_VALUE || value->soh > SOH_LSB_SOH_INIT || value->soh <= 0){
		// User code
		value->c1 = SOH_LSB_C1_INIT;
		value->c2 = SOH_LSB_C2_INIT;
		value->c3 = SOH_LSB_C3_INIT;
		value->est_capacity = SOH_LSB_EST_CAPACITY_INIT;
		value->soh = SOH_LSB_SOH_INIT;
		value->delta_x = 0;
		value->delta_y = 0;
		value->last_soc = (int64_t)(bms_soc.output.SOC_f*10);
		value->cnt = 0;
		value->key = KEY_VALUE;
		// End User code
		SOC_SOH_Save(p_logger, *value);
		cJSON_Delete(json);
		return 1;
	}
	// End User code
	cJSON_Delete(json);
	return 0;
}
SOC_SOH_Logger bms_soc_logger;
