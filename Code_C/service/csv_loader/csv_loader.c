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

    	line_id++;
    	if(line_id >= max_size){
    		return line_id;
    	}
    }

    return line_id;
}
#if 0
int32_t load_battery_data_from_csv_file(const char* path,BMS_Input_Vector *buff,const uint32_t max_size){

    FILE* stream = fopen(path, "r");
    if(NULL==stream){
    	return -1;
    }
    int i;
    uint32_t line_id=0;
    int32_t index;
    char line[2048];
    const char* field;
	while (fgets(line, 2048, stream)) {
		index = 1;
		char *tmp;

        tmp = strdup(line);
        field=getfield(tmp, index++);
        if(NULL==field) return -1;
        buff[line_id].data_logger.pack_voltage= (int32_t)atof(field);
        free(tmp);

        tmp = strdup(line);
        field=getfield(tmp, index++);
        if(NULL==field) return -1;
        buff[line_id].data_logger.pack_current= (int32_t)atof(field);
        free(tmp);

        for (i = 0; i < UKF_STATE_DIM; i++){
            tmp = strdup(line);
            field=getfield(tmp, index++);
            if(NULL==field) return -1;
            buff[line_id].data_logger.estimate_state[i]= (int32_t)atof(field);
            free(tmp);
        }

        for (i = 0; i < UKF_STATE_DIM*UKF_STATE_DIM; i++){
            tmp = strdup(line);
            field=getfield(tmp, index++);
            if(NULL==field) return -1;
            buff[line_id].data_logger.state_covariance[i]= (int32_t)atof(field);
            free(tmp);
        }

        for (i = 0; i < UKF_STATE_DIM*UKF_SIGMA_FACTOR; i++){
            tmp = strdup(line);
            field=getfield(tmp, index++);
            if(NULL==field) return -1;
            buff[line_id].data_logger.sigma_points[i]= (int32_t)atof(field);
            free(tmp);
        }

        for (i = 0; i < UKF_STATE_DIM; i++){
            tmp = strdup(line);
            field=getfield(tmp, index++);
            if(NULL==field) return -1;
            buff[line_id].data_logger.priori_estimate_state[i]= (int32_t)atof(field);
            free(tmp);
        }

        for (i = 0; i < UKF_STATE_DIM*UKF_SIGMA_FACTOR; i++){
            tmp = strdup(line);
            field=getfield(tmp, index++);
            if(NULL==field) return -1;
            buff[line_id].data_logger.sigma_state_error[i]= (int32_t)atof(field);
            free(tmp);
        }

        tmp = strdup(line);
        field=getfield(tmp, index++);
        if(NULL==field) return -1;
        buff[line_id].data_logger.measurement_cov= (int32_t)atof(field);
        free(tmp);

        tmp = strdup(line);
        field=getfield(tmp, index++);
        if(NULL==field) return -1;
        buff[line_id].data_logger.est_measurement= (int32_t)atof(field);
        free(tmp);

        for (i = 0; i < UKF_STATE_DIM; i++){
            tmp = strdup(line);
            field=getfield(tmp, index++);
            if(NULL==field) return -1;
            buff[line_id].data_logger.cross_covariance[i]= (int32_t)atof(field);
            free(tmp);
        }

        for (i = 0; i < UKF_STATE_DIM; i++){
            tmp = strdup(line);
            field=getfield(tmp, index++);
            if(NULL==field) return -1;
            buff[line_id].data_logger.aukf_kalman_gain[i]= (int32_t)atof(field);
            free(tmp);
        }

        for (i = 0; i < UKF_STATE_DIM*UKF_STATE_DIM; i++){
            tmp = strdup(line);
            field=getfield(tmp, index++);
            if(NULL==field) return -1;
            buff[line_id].data_logger.matrix_A[i]= (int32_t)atof(field);
            free(tmp);
        }
        for (i = 0; i < UKF_STATE_DIM*UKF_DYNAMIC_DIM; i++){
            tmp = strdup(line);
            field=getfield(tmp, index++);
            if(NULL==field) return -1;
            buff[line_id].data_logger.matrix_B[i]= (int32_t)atof(field);
            free(tmp);
        }
        for (i = 0; i < UKF_STATE_DIM; i++){
            tmp = strdup(line);
            field=getfield(tmp, index++);
            if(NULL==field) return -1;
            buff[line_id].data_logger.matrix_C[i]= (int32_t)atof(field);
            free(tmp);
        }

        for (i = 0; i < UKF_DYNAMIC_DIM; i++){
            tmp = strdup(line);
            field=getfield(tmp, index++);
            if(NULL==field) return -1;
            buff[line_id].data_logger.matrix_D[i]= (int32_t)atof(field);
            free(tmp);
        }

        for (i = 0; i < UKF_DYNAMIC_DIM; i++){
            tmp = strdup(line);
            field=getfield(tmp, index++);
            if(NULL==field) return -1;
            buff[line_id].data_logger.observed_measurement[i]= (int32_t)atof(field);
            free(tmp);
        }

        for (i = 0; i < UKF_SIGMA_FACTOR; i++){
            tmp = strdup(line);
            field=getfield(tmp, index++);
            if(NULL==field) return -1;
            buff[line_id].data_logger.sigma_measurements[i]= (int32_t)atof(field);
            free(tmp);
        }

        for (i = 0; i < UKF_SIGMA_FACTOR; i++){
            tmp = strdup(line);
            field=getfield(tmp, index++);
            if(NULL==field) return -1;
            buff[line_id].data_logger.sigma_measurement_error[i]= (int32_t)atof(field);
            free(tmp);
        }


    	line_id++;
    	if(line_id >= max_size){
    		return line_id;
    	}
    }
    return line_id;
}
#endif

int32_t load_soh_data_from_csv_file(const char* path, SOH_Save_Data *buff,const uint32_t max_size){
    FILE* stream = fopen(path, "r");
    if(NULL==stream){
    	return -1;
    }
    uint32_t line_id=0;
    char line[1024];
    int32_t index;
    const char* field;
    while (fgets(line, 1024, stream))
    {
		index = 1;
        char* tmp;
        tmp = strdup(line);
        field=getfield(tmp, index++);
        if(NULL==field) return -1;
        buff[line_id].c1= (int64_t)atof(field);
        free(tmp);

        tmp = strdup(line);
        field=getfield(tmp, index++);
        if(NULL==field) return -1;
        buff[line_id].c2= (int64_t)atof(field);
        free(tmp);

        tmp = strdup(line);
        field=getfield(tmp, index++);
        if(NULL==field) return -1;
        buff[line_id].c3= (int64_t)atof(field);
        free(tmp);

        tmp = strdup(line);
        field=getfield(tmp, index++);
        if(NULL==field) return -1;
        buff[line_id].delta_x= (int64_t)atof(field);
        free(tmp);

        tmp = strdup(line);
        field=getfield(tmp, index++);
        if(NULL==field) return -1;
        buff[line_id].delta_y= (int64_t)atof(field);
        free(tmp);

        tmp = strdup(line);
        field=getfield(tmp, index++);
        if(NULL==field) return -1;
        buff[line_id].est_capacity= (int64_t)atof(field);
        free(tmp);

        tmp = strdup(line);
        field=getfield(tmp, index++);
        if(NULL==field) return -1;
        buff[line_id].last_soc= (int64_t)atof(field);
        free(tmp);

        tmp = strdup(line);
        field=getfield(tmp, index++);
        if(NULL==field) return -1;
        buff[line_id].soh= (int64_t)atof(field);
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
