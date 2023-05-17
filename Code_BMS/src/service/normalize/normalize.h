/*
 * normalized.h
 *
 *  Created on: May 9, 2023
 *      Author: ADMIN
 */

#ifndef SERVICE_NORMALIZE_NORMALIZE_H_
#define SERVICE_NORMALIZE_NORMALIZE_H_

#include "data_logger.h"

void synchronize_in(const Data_Logger* input, SOC_UKF* logger);
int normalize(const SOC_UKF* logger, SOC_UKF* processor, const Parameter type);
void synchronize_out(const SOC_UKF* processor, Data_Logger* output);



#endif /* SERVICE_NORMALIZE_NORMALIZE_H_ */
