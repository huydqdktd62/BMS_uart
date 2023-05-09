/*
 * ffrls.h
 *
 *  Created on: Jul 1, 2022
 *      Author: ADMIN
 */

#if 0
#ifndef SERVICE_VFFRLS_H_
#define SERVICE_VFFRLS_H_

#include "math_lib.h"
#include "battery_model.h"

typedef struct VFFRLS_State_Vector_t VFFRLS_State_Vector;
typedef struct VFFRLS_Input_Vector_t VFFRLS_Input_Vector;
typedef struct VFFRLS_Output_Vector_t VFFRLS_Output_Vector;

struct VFFRLS_Input_Vector_t{
	float terminalVoltage;
	float current;
	float hysteresis;
	float temp;
};

struct VFFRLS_Output_Vector_t{
	float resistor0;
	float resistor1;
	float RC_parameter;
};

struct VFFRLS_State_Vector_t{
	VFFRLS_Input_Vector* p_input;
	VFFRLS_Output_Vector* p_output;
	float priorVoltage;
	Matrix covariance;
	Matrix vffrls_kalmanGain;
	Matrix phi;
	Matrix theta;
	float estimateVoltage;
	float errorVoltage;
};

void vffrls_update_input(VFFRLS_State_Vector* p_state, const float terminalVoltage, const float current, const float hysteresis);
void vffrls_init(VFFRLS_State_Vector* p_state);
void vffrls_parameters_init(VFFRLS_State_Vector *p_state);
void vffrls_update(VFFRLS_State_Vector* p_state);
VFFRLS_Output_Vector* vffrls_get_output(VFFRLS_State_Vector* p_state);

#endif /* SERVICE_VFFRLS_H_ */
#endif
