/*
 * data_logger.h
 *
 *  Created on: May 9, 2023
 *      Author: ADMIN
 */

#ifndef SERVICE_DATA_LOGGER_DATA_LOGGER_H_
#define SERVICE_DATA_LOGGER_DATA_LOGGER_H_
#include "app_config.h"
#include "stdio.h"
#if 0

int32_t create_data(const char *file_path, FILE *out_file, const Parameter type);
int32_t save_data(const char *file_path, FILE *out_file, const Parameter type,
		const Data_Logger* logger, const Data_Logger* processor);

#endif

#endif /* SERVICE_DATA_LOGGER_DATA_LOGGER_H_ */
