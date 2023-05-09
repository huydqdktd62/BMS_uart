/*
 * converter.h
 *
 *  Created on: Mar 27, 2023
 *      Author: Admin
 */

#ifndef COMPONENTS_CONVERTER_CONVERTER_H_
#define COMPONENTS_CONVERTER_CONVERTER_H_

#include "bp.h"
#include "cabinet.h"
#include "can_master.h"

typedef enum{
	CAN2MB_ST_NOT_ACTIVE							=0,
	CAN2MB_ST_INIT									=4,
	CAN2MB_ST_NORMAL								=2,
	CAN2MB_ST_MAINTAINING							=3,
}CAN2MB_State;

typedef struct Can2Modbus_t Can2Modbus;

struct Can2Modbus_t{
	CAN_master base;
	Cabinet *cabinets;
	uint8_t cab_num;
	CAN2MB_State state;
};

void CAN2MB_Init(Can2Modbus *p_can2mb);

/* Get number of slave node which is int init state  */
static inline uint8_t CAN2MB_get_reamain_init_node(const Can2Modbus* const mbs){

	uint8_t result=0;
	for(int i=0;i<mbs->cab_num;i++){

		if(cab_get_state(&mbs->cabinets[i])==CAB_ST_INIT){
			result++;
		}
	}
	return result;
}

Cabinet* CAN2MB_get_first_init_cab(const Can2Modbus* const p_mbs);
Cabinet* CAN2MB_get_first_empty_cab(const Can2Modbus* const p_mbs);

static inline void CAN2MB_set_state(Can2Modbus *p_mbs,CAN2MB_State state){
	p_mbs->state=state;
}
static inline CAN2MB_State CAN2MB_get_state(const Can2Modbus* const p_mbs){
	return (p_mbs->state);
}

bool CAN2MB_is_battery_assigned(const Can2Modbus* const p_mbs,const Battery* const bat);


#endif /* COMPONENTS_CONVERTER_CONVERTER_H_ */
