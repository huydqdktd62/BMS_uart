/*
 * soc_aukf.h
 *
 *  Created on: Feb 6, 2023
 *      Author: ADMIN
 */

#ifndef SERVICE_SOC_AUKF_SOC_VFFRLS_H_
#define SERVICE_SOC_AUKF_SOC_VFFRLS_H_

#include <stdint.h>

typedef struct Battery_param_t Battery_param;

struct Battery_param_t {
	float R0;
	float R1;
	float C1;
};

extern Battery_param battery_param;

#if 0

void vffrls_init(const uint32_t pack_voltage);
void vffrls_update(const uint32_t pack_voltage, const int32_t pack_current);

#endif

#endif /* SERVICE_SOC_AUKF_SOC_VFFRLS_H_ */
