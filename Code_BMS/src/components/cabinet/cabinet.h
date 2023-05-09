/*
 * cabinet.h
 *
 *  Created on: Nov 23, 2022
 *      Author: quangnd
 */

#ifndef MAIN_COMPONENTS_CABINET_CABINET_H_
#define MAIN_COMPONENTS_CABINET_CABINET_H_

#include <stdint.h>
#include "bp.h"
#include "node_id.h"


typedef enum CAB_STATE_t{
	CAB_ST_OFF				=10,
	CAB_ST_EMPTY			=1,
	CAB_ST_INIT				=6,
	CAB_ST_STANDBY			=4,
	CAB_ST_LOST_CONNECT		=41,
	CAB_ST_BOOTUP 			=42,
	CAB_ST_CAN_ERR			=43,
	CAB_ST_BP_ERROR_SN		=44,
	CAB_ST_CHARGING			=5,
	CAB_ST_FULL				=60,
	CAB_ST_SWAP_OLD_BAT		=61,
	CAB_ST_SWAP_RETURN		=62,
	CAB_ST_SWAP_NEW_BAT		=70,
	CAB_ST_UPDATE_FIRMWARE  =80,
	CAB_ST_OTA_END			=90,
}CAB_STATE;

typedef struct Cabinet_t Cabinet;

struct Cabinet_t{
	CAB_STATE state;
	Battery* battery;
	int32_t temp;
	uint8_t id;
	Node_ID *node;
	uint32_t time_switch;
	uint32_t time_hold_charger;
	uint32_t time_charge;
};

void cab_set_state(Cabinet* p_cab,const CAB_STATE new_state);

static inline CAB_STATE cab_get_state(const Cabinet* const p_cab){
	return p_cab->state;
}

bool cab_update_lost_connect_status(Cabinet *cab,
		const uint32_t timediff);

void cabinet_init(Cabinet *p_cabin,Battery *p_bp,uint8_t id);
void cabinet_set_bp(Cabinet *p_cabin,Battery *p_bp);
void cabinet_set_id(Cabinet *p_cabin,uint8_t id);
void cabinet_set_temp(Cabinet *p_cabin, int32_t temp);
Battery* cabinet_get_bp(Cabinet *p_cabin);
int32_t cabinet_get_temp(Cabinet *p_cabin);
uint8_t cabinet_get_id(Cabinet *p_cabin);

#endif /* MAIN_COMPONENTS_CABINET_CABINET_H_ */
