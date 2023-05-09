/*
 * aukf.h
 *
 *  Created on: Jul 1, 2022
 *      Author: ADMIN
 */

#if 0
#ifndef SERVICE_AUKF_H_
#define SERVICE_AUKF_H_

#include "../../soc_aukf_v1/soc_aukf_service/vffrls.h"
#include "math_lib.h"
#include "battery_model.h"

typedef struct AUKF_Input_Vector_t AUKF_Input_Vector;
typedef struct AUKF_Output_Vector_t AUKF_Output_Vector;
typedef struct AUKF_State_Vector_t AUKF_State_Vector;

struct AUKF_Input_Vector_t {
	float terminalVoltage;
	float current;
	float soh;
	float temp;
	float resistor0;
	float resistor1;
	float RC_parameter;
};

struct AUKF_Output_Vector_t {
	uint32_t SOC;
	float OCV;
	float terminalVoltage;
	float current;
	float hysteresisVoltage;
};

struct AUKF_State_Vector_t {

	AUKF_Input_Vector *p_input;
	AUKF_Output_Vector *p_output;
	Matrix estimateState;
	Matrix stateCovariance;
	Matrix sigmaPoints;
	Matrix prioriEstimateState;
	Matrix matrixA;
	Matrix matrixB;
	Matrix sigmaStateError;
	Matrix matrixC;
	Matrix matrixD;
	Matrix observedMeasurement;
	Matrix sigmaMeasurements;
	float estimateMeasurement;
	Matrix sigmaMeasureError;
	float measurementCovariance;
	Matrix crossCovariance;
	Matrix aukf_kalmanGain;
	float RCParameter;
	float HParameter;
};

void aukf_update_input(AUKF_State_Vector *p_state,
		const VFFRLS_Output_Vector *p_output, const float terminalVoltage,
		const float current, const float soh);
void aukf_init(AUKF_State_Vector *p_state);
void aukf_parameters_init(AUKF_State_Vector *p_state);
void aukf_update(AUKF_State_Vector *p_state);
AUKF_Output_Vector* aukf_get_output(AUKF_State_Vector *p_state);

#endif /* SERVICE_AUKF_H_ */
#endif
