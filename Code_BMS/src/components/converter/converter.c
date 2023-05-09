/*
 * converter.c
 *
 *  Created on: Mar 27, 2023
 *      Author: Admin
 */
#include "converter.h"

void CAN2MB_Init(Can2Modbus *p_can2mb){

}

bool CAN2MB_is_battery_assigned(const Can2Modbus* const p_mbs,const Battery* const bat){
	return true;
}

Cabinet* CAN2MB_get_first_init_cab(const Can2Modbus* const p_mbs){
	for(int i=0;i<p_mbs->cab_num;i++){
		if(cab_get_state(&p_mbs->cabinets[i])==CAB_ST_INIT){
			return &p_mbs->cabinets[i];
		}
	}
	return NULL;
}
Cabinet* CAN2MB_get_first_empty_cab(const Can2Modbus* const p_mbs){
	for(int i=0;i<p_mbs->cab_num;i++){
		if(cab_get_state(&p_mbs->cabinets[i])==CAB_ST_EMPTY){
			return &p_mbs->cabinets[i];
		}
	}
	return NULL;
}
