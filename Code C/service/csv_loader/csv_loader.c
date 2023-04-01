/*
 * csv_loader.c
 *
 *  Created on: May 6, 2022
 *      Author: quangnd
 */
#include "csv_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "stdint.h"

static const char* getfield(char* line, int num);

int32_t load_battery_data_from_csv_file(const char* path,BMS_Input_Vector *buff,const uint32_t max_size){

    FILE* stream = fopen(path, "r");

    if(NULL==stream){
    	return -1;
    }

    uint32_t line_id=0;
    char line[1024];
    const char* field;
    while (fgets(line, 1024, stream))
    {
        char* tmp;

        tmp = strdup(line);
        field=getfield(tmp, 1);
        if(NULL==field) return -1;
        buff[line_id].terminalVoltage= (float)atof(field);
        free(tmp);

        tmp=strdup(line);
        field=getfield(tmp, 2);
        if(NULL==field) return -1;
        buff[line_id].current=(float)atof(field);
        free(tmp);

        tmp=strdup(line);
        field=getfield(tmp, 3);
        if(NULL==field) return -1;
        buff[line_id].cycles=(uint32_t)atoi(field);
        free(tmp);

    	line_id++;
    	if(line_id >= max_size){
    		return line_id;
    	}
    }

    return line_id;
}

int32_t load_soh_data_from_csv_file(const char* path, float *buff,const uint32_t max_size){

    FILE* stream = fopen(path, "r");

    if(NULL==stream){
    	return -1;
    }

    uint32_t line_id=0;
    char line[1024];
    const char* field;
    while (fgets(line, 1024, stream))
    {
        char* tmp;

        tmp = strdup(line);
        field=getfield(tmp, 1);
        if(NULL==field) return -1;
        buff[line_id]= (float)atof(field);
        free(tmp);

    	line_id++;
    	if(line_id >= max_size){
    		return line_id;
    	}
    }

    return line_id;
}

static const char* getfield(char* line, int num)
{
    const char* tok=strtok(line,",");
    while(tok) {
    	if(!(--num)) return tok;
        tok = strtok(NULL, ",");
    }
    return NULL;
}
