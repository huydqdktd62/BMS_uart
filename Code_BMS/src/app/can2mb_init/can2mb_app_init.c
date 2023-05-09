/*
 * can2mb_app_init.c
 *
 *  Created on: Mar 28, 2023
 *      Author: Admin
 */


#include "can2mb_app_init.h"
#include "modbus_app.h"
#define MAX_CABINET 3
#define START_NODE_ID 5

#define MODBUS_BP_1_ADDR 0
#define MODBUS_BP_2_ADDR 12
#define MODBUS_BP_3_ADDR 24

static Battery battery_array[MAX_CABINET];
static CO_Slave* slave_bps[MAX_CABINET];
static Cabinet cabin_array[MAX_CABINET];
static Node_ID mbs_node[MAX_CABINET];

static void cab_node_select(CO_Slave* p_sl);
static void cab_node_deselect(CO_Slave* p_sl);

Can2Modbus selex_can2mb;

static void bat1_create_data();
static void bat2_create_data();
static void bat3_create_data();


void can2mb_app_init(){
	Can2Modbus *p_mbs = &selex_can2mb;
	p_mbs->cab_num = MAX_CABINET;
	p_mbs->cabinets = cabin_array;
	p_mbs->base.slaves=slave_bps;
	p_mbs->base.slave_num=p_mbs->cab_num;

	p_mbs->base.assign_sv.sync_counter=7;
	p_mbs->base.assign_sv.sync_mask=0x03;

	p_mbs->base.slave_start_node_id = START_NODE_ID;
	for (uint32_t i = 0; i < MAX_CABINET;i++) {
		p_mbs->cabinets[i].node = &mbs_node[i];
		bat_init(&battery_array[i]);
		cabinet_init(&p_mbs->cabinets[i],&battery_array[i],i);
		node_id_init(p_mbs->cabinets[i].node);
		slave_bps[i]=(CO_Slave*)(&battery_array[i].base);
		slave_bps[i]->base.node_id=i+START_NODE_ID;

		slave_bps[i]->interface.deselect=cab_node_deselect;
		slave_bps[i]->interface.select=cab_node_select;
		co_slave_set_assign_state(slave_bps[i], CO_ASSIGN_ST_INIT);
		slave_bps[i]->assign_timeout = 10000;
		slave_bps[i]->base.tpdos[0].comm_parameter.id.can_id=(uint32_t)CO_CAN_ID_TPDO_1+ p_mbs->base.slave_start_node_id +i;
		slave_bps[i]->base.tpdos[1].comm_parameter.id.can_id=(uint32_t)CO_CAN_ID_TPDO_2+ p_mbs->base.slave_start_node_id +i;
		slave_bps[i]->base.tpdos[2].comm_parameter.id.can_id=(uint32_t)CO_CAN_ID_TPDO_3+ p_mbs->base.slave_start_node_id +i;
		slave_bps[i]->base.tpdos[3].comm_parameter.id.can_id=(uint32_t)CO_CAN_ID_TPDO_4+ p_mbs->base.slave_start_node_id +i;
	}
	bat1_create_data();
	bat2_create_data();
	bat3_create_data();
	CAN2MB_set_state(p_mbs,CAN2MB_ST_INIT);
	p_mbs->base.assign_sv.slave=NULL;
}

static void cab_node_select(CO_Slave* p_sl){
	node_id_low(&mbs_node[p_sl->id]);
}
static void cab_node_deselect(CO_Slave* p_sl){
	node_id_high(&mbs_node[p_sl->id]);
}
static void bat1_create_data(){
	Battery *p_bat = &battery_array[0];
	p_bat->state = &usRegInputBuf[MODBUS_BP_1_ADDR];
	p_bat->vol = &usRegInputBuf[MODBUS_BP_1_ADDR+1];
	p_bat->cur = (int16_t*)&usRegInputBuf[MODBUS_BP_1_ADDR+2];
	p_bat->soc = &usRegInputBuf[MODBUS_BP_1_ADDR+3];
	p_bat->soh = &usRegInputBuf[MODBUS_BP_1_ADDR+4];
	p_bat->cycles = &usRegInputBuf[MODBUS_BP_1_ADDR+5];
	p_bat->temp = (int16_t*)&usRegInputBuf[MODBUS_BP_1_ADDR+6];
}
static void bat2_create_data(){
	Battery *p_bat = &battery_array[1];
	p_bat->state = &usRegInputBuf[MODBUS_BP_2_ADDR];
	p_bat->vol = &usRegInputBuf[MODBUS_BP_2_ADDR+1];
	p_bat->cur = (int16_t*)&usRegInputBuf[MODBUS_BP_2_ADDR+2];
	p_bat->soc = &usRegInputBuf[MODBUS_BP_2_ADDR+3];
	p_bat->soh = &usRegInputBuf[MODBUS_BP_2_ADDR+4];
	p_bat->cycles = &usRegInputBuf[MODBUS_BP_2_ADDR+5];
	p_bat->temp = (int16_t*)&usRegInputBuf[MODBUS_BP_2_ADDR+6];
}
static void bat3_create_data(){
	Battery *p_bat = &battery_array[2];
	p_bat->state = &usRegInputBuf[MODBUS_BP_3_ADDR];
	p_bat->vol = &usRegInputBuf[MODBUS_BP_3_ADDR+1];
	p_bat->cur = (int16_t*)&usRegInputBuf[MODBUS_BP_3_ADDR+2];
	p_bat->soc = &usRegInputBuf[MODBUS_BP_3_ADDR+3];
	p_bat->soh = &usRegInputBuf[MODBUS_BP_3_ADDR+4];
	p_bat->cycles = &usRegInputBuf[MODBUS_BP_3_ADDR+5];
	p_bat->temp = (int16_t*)&usRegInputBuf[MODBUS_BP_3_ADDR+6];
}
