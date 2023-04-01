/*
 * cabinet.c
 *
 *  Created on: Nov 23, 2022
 *      Author: quangnd
 */

#include "cabinet.h"


void cabinet_init(Cabinet *p_cabin, Battery *p_bp, uint8_t id) {

	p_cabin->battery = p_bp;
	cab_set_state(p_cabin, CAB_ST_INIT);
	p_bp->base.id=id;
	p_cabin->id = id;
	p_cabin->temp = 0;
	p_cabin->time_switch = 0;
	p_cabin->time_hold_charger = 0;
	p_cabin->time_charge = 0;
}

void cab_set_state(Cabinet* p_cab,const CAB_STATE new_state){
	switch(new_state){

	case CAB_ST_INIT:
		co_slave_set_assign_state((CO_Slave*)p_cab->battery,CO_ASSIGN_ST_INIT);
		break;
	case CAB_ST_LOST_CONNECT:
		co_slave_set_assign_state((CO_Slave*)p_cab->battery,CO_ASSIGN_ST_LOST_CONNECTED);
		p_cab->battery->base.inactive_time_ms=0;
		co_slave_select((CO_Slave*)p_cab->battery);
		break;
	case CAB_ST_EMPTY:
		co_slave_select((CO_Slave*)p_cab->battery);
		co_slave_set_assign_state((CO_Slave*)p_cab->battery,CO_ASSIGN_ST_DISCONNECTED);
		bat_set_soc(p_cab->battery,0);
		p_cab->battery->base.inactive_time_ms=0;
		break;
	case CAB_ST_STANDBY:
		p_cab->battery->base.inactive_time_ms=0;
		break;
	case CAB_ST_SWAP_OLD_BAT:
		p_cab->battery->base.inactive_time_ms=0;
		break;
	case CAB_ST_SWAP_RETURN:
		p_cab->time_switch = 0;
		break;
	case CAB_ST_SWAP_NEW_BAT:
		p_cab->time_switch = 0;
		break;
	case CAB_ST_CHARGING:
		p_cab->time_hold_charger = 0;
		break;
	case CAB_ST_FULL:
		break;
	case CAB_ST_CAN_ERR:
		co_slave_select((CO_Slave*)p_cab->battery);
		break;
	case CAB_ST_BP_ERROR_SN:
		break;
	case CAB_ST_UPDATE_FIRMWARE:
		co_slave_select((CO_Slave*)p_cab->battery);
		break;
	case CAB_ST_OTA_END:
		co_slave_set_assign_state((CO_Slave*)p_cab->battery,CO_ASSIGN_ST_LOST_CONNECTED);
		p_cab->battery->base.inactive_time_ms=0;
		co_slave_select((CO_Slave*)p_cab->battery);
		break;
	default:
		break;
	}
	if(p_cab->state !=new_state){
		p_cab->state=new_state;
	}
}

bool cab_update_lost_connect_status(Cabinet *cab,
		const uint32_t timediff) {
	cab->battery->base.inactive_time_ms +=timediff;
	if (cab->battery->base.inactive_time_ms >= 2*SLAVE_INACTIVE_TIME_THRESHOLD_mS) {
		cab_set_state(cab, CAB_ST_LOST_CONNECT);
		return true;
	}
	return false;
}

void cabinet_set_bp(Cabinet *p_cabin, Battery *p_bp) {
	p_cabin->battery = p_bp;
}

void cabinet_set_id(Cabinet *p_cabin, uint8_t id) {
	p_cabin->id = id;
}

void cabinet_set_temp(Cabinet *p_cabin, int32_t temp){
	p_cabin->temp = temp;
}

Battery* cabinet_get_bp(Cabinet *p_cabin) {
	return p_cabin->battery;
}

int32_t cabinet_get_temp(Cabinet *p_cabin) {
	return p_cabin->temp;
}

uint8_t cabinet_get_id(Cabinet *p_cabin) {
	return p_cabin->id;
}
