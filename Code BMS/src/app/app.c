/*
 * app.c
 *
 *  Created on: Mar 28, 2023
 *      Author: Admin
 */

#include "app.h"
#include "timer_hw.h"

#define APP_STATE_MACHINE_TICK_INTERVAL_mS 10
#define APP_CO_PROCESS_TICK_INTERVAL_mS 1
#define SLAVE_SW_VER 	  0x100A00

static void on_bp_assign_success(CAN_master *p_cm, const uint32_t timestamp);
static void on_bp_assign_fail(CAN_master *p_cm, const uint32_t timestamp);
static void mbs_update_assigning_process(Can2Modbus *p_mbs, CO_Slave *assigning_node);
static void mbs_on_new_battery_accepted(Can2Modbus *p_mbs, Cabinet *cab);
static void mbs_update_cabinet_state(Can2Modbus *p_mbs, Cabinet *cab);

static void on_bp_get_data_from_tpdo1(Can2Modbus *mbs, uint8_t bp_index);
static void on_bp_get_data_from_tpdo2(Can2Modbus *mbs, uint8_t bp_index);
static void on_bp_get_data_from_tpdo3(Can2Modbus *mbs, uint8_t bp_index);
static void on_bp_get_data_from_tpdo4(Can2Modbus *mbs, uint8_t bp_index);

void app_init(){
	app_co_init();
	can2mb_app_init();
	mb_app_init();
	selex_can2mb.base.on_slave_assign_success = on_bp_assign_success;
	selex_can2mb.base.on_slave_assign_fail = on_bp_assign_fail;
	timer_add_callback(&bsp_timer0,app_scan_cabinets_task,(void*)&selex_can2mb);
	timer_add_callback(&bsp_timer1,app_task,(void*)&selex_can2mb);
}

void app_task(void *arg){
	Can2Modbus *p_mbs = (Can2Modbus *)arg;
	static CO_Slave *assigning_node = NULL;
	static uint8_t next_cab_id = 0;
	switch (p_mbs->state) {
		case CAN2MB_ST_NORMAL:
			assigning_node = cm_get_assigning_slave((CAN_master*) p_mbs);
			if (NULL != assigning_node) {
				mbs_update_assigning_process(p_mbs, assigning_node);
				/* if the assigning process has finished, clear for next slot */
				if (co_slave_is_assign_finish(assigning_node)) {
					p_mbs->base.assign_sv.slave = NULL;
					/* move to next node */
					next_cab_id = assigning_node->id + 1;
					if (next_cab_id == p_mbs->base.slave_num) {
						next_cab_id = 0;
					}
				}
			} else {
				if(p_mbs->cabinets[next_cab_id].battery->base.base.dev_id[0] == 0){
				}
			}
			mbs_update_cabinet_state(p_mbs, &(p_mbs->cabinets[next_cab_id]));

			on_bp_get_data_from_tpdo1(p_mbs, next_cab_id);
			on_bp_get_data_from_tpdo2(p_mbs, next_cab_id);
			on_bp_get_data_from_tpdo3(p_mbs, next_cab_id);
			on_bp_get_data_from_tpdo4(p_mbs, next_cab_id);

			next_cab_id++;
			if (next_cab_id == p_mbs->base.slave_num) {
				next_cab_id = 0;
			}
			break;
		case CAN2MB_ST_INIT:
			CAN2MB_set_state(p_mbs, CAN2MB_ST_NOT_ACTIVE);
			break;
		case CAN2MB_ST_MAINTAINING:
			break;
		case CAN2MB_ST_NOT_ACTIVE:
			CAN2MB_set_state(p_mbs, CAN2MB_ST_NORMAL);
			break;
		default:
			break;
	}
}

static void on_bp_assign_success(CAN_master *p_cm, const uint32_t timestamp) {

    (void)timestamp;

	p_cm->assign_sv.slave = NULL;

}
static void on_bp_assign_fail(CAN_master *p_cm, const uint32_t timestamp) {
    (void) p_cm;
    (void)timestamp;
}
static void mbs_update_assigning_process(Can2Modbus *p_mbs, CO_Slave *assigning_node) {

	Cabinet *current_cab = NULL;
	CO_SLAVE_ASSIGN_STATE new_state = CO_ASSIGN_ST_FAIL;
	CO_SLAVE_ASSIGN_STATE old_state = co_slave_get_assign_state(assigning_node);

	cm_update_slave_assign_state((CAN_master*) p_mbs,
	APP_STATE_MACHINE_TICK_INTERVAL_mS);
	new_state = co_slave_get_assign_state(assigning_node);
	current_cab = &(p_mbs->cabinets[assigning_node->id]);

	if (new_state != old_state) {
		if (new_state == CO_ASSIGN_ST_CONNECTED) {
			CO_Slave* slave=p_mbs->base.assign_sv.slave;
			/*Read software version pack pin*/
			uint8_t version[32];
			co_sdo_read_object((CAN_master*)p_mbs, SLAVE_SW_VER,slave->base.node_id,(uint8_t*) version, 5000);

			while (p_mbs->base.base.sdo_client.state != CO_SDO_ST_IDLE) {
			}

			if (p_mbs->base.base.sdo_client.status == CO_SDO_RT_abort) {
				sprintf(current_cab->battery->version, "None");
			}

			if(p_mbs->base.base.sdo_client.status == CO_SDO_RT_success){
				sprintf(current_cab->battery->version,"v%d.%d.%d",version[2],version[1],version[0]);
				printf("bp version: %s\n",current_cab->battery->version);
			}
			CO_SDO_reset_status(&p_mbs->base.base.sdo_client);
			mbs_on_new_battery_accepted(p_mbs, current_cab);

		}
	}

	if ((new_state == CO_ASSIGN_ST_DISCONNECTED)
			|| (new_state == CO_ASSIGN_ST_FAIL)) {
		cab_set_state(current_cab, CAB_ST_EMPTY);
		//buzzer_setmode(p_mbs->buzzer_warning, WARNING_INSERT_BP);
	}

}
static void mbs_on_new_battery_accepted(Can2Modbus *p_mbs, Cabinet *cab){
    (void)p_mbs;
	cab_set_state(cab, CAB_ST_STANDBY);
}
static void mbs_update_cabinet_state(Can2Modbus *p_mbs, Cabinet *cab){
	switch (cab_get_state(cab)) {

	case CAB_ST_OFF:
		break;
	case CAB_ST_INIT:
		cab_set_state(cab, CAB_ST_EMPTY);
		break;
	case CAB_ST_EMPTY:
		/* when mbs is in init state, don't use process
		 * to detect newly inserted battery
		 */
		if (p_mbs->base.assign_sv.slave == NULL) {
			cm_start_assign_slave((CAN_master*) p_mbs, (CO_Slave*) cab->battery,
					0);
		}
		bat_set_state(cab->battery,BAT_EMPTY);
		break;

	case CAB_ST_STANDBY:
		/* if this cabinet has assigned and finished
		 * so clear current assigning node
		 */
		bat_set_state(cab->battery,BAT_STANDBY);
		if ((p_mbs->base.assign_sv.slave == (CO_Slave*) cab->battery)
				&& (co_slave_get_assign_state(p_mbs->base.assign_sv.slave)
						== CO_ASSIGN_ST_CONNECTED)) {
			p_mbs->base.assign_sv.slave = NULL;
			break;
		}
		cab_update_lost_connect_status(cab,
				((uint32_t) p_mbs->cab_num * APP_STATE_MACHINE_TICK_INTERVAL_mS));
		break;
	case CAB_ST_LOST_CONNECT:
		if (p_mbs->base.assign_sv.slave == NULL) {
			cm_start_assign_slave((CAN_master*) p_mbs, (CO_Slave*) cab->battery,
					0);
		}
		break;
	case CAB_ST_CAN_ERR:
	case CAB_ST_BP_ERROR_SN:
	case CAB_ST_BOOTUP:
	case CAB_ST_CHARGING:
	case CAB_ST_FULL:
	case CAB_ST_SWAP_OLD_BAT:
	case CAB_ST_SWAP_RETURN:
	case CAB_ST_SWAP_NEW_BAT:
	case CAB_ST_UPDATE_FIRMWARE:
	case CAB_ST_OTA_END:
		cab_update_lost_connect_status(cab,
				((uint32_t) p_mbs->cab_num * APP_STATE_MACHINE_TICK_INTERVAL_mS));
		break;
	default:
		break;
	}
}
void app_scan_cabinets_task(void *arg){
	Can2Modbus *p_mbs = (Can2Modbus*) arg;
	cm_process((CAN_master*) &p_mbs->base, APP_CO_PROCESS_TICK_INTERVAL_mS);
}
static void on_bp_get_data_from_tpdo1(Can2Modbus *mbs, uint8_t bp_index){
	CO_Slave *slave = mbs->base.slaves[bp_index];
	Battery *bat = mbs->cabinets[bp_index].battery;
	CO_CAN_Msg *mgs = &slave->base.tpdos[0].base.msg;
	if (mgs->is_new == true) {
		bat_set_state(bat, mgs->data[5]);
		bat_set_soc(bat, mgs->data[4]);
		bat_set_cur(bat,
				(int16_t) ((((uint16_t) mgs->data[3] << 8)
						| ((uint16_t) mgs->data[2]))));
		uint16_t vol = (uint16_t) ((((uint16_t) mgs->data[1] << 8) | ((uint16_t) mgs->data[0])));
		bat_set_vol(bat,vol);
		can_msg_set_new_flag(mgs, false);
	}
}
static void on_bp_get_data_from_tpdo2(Can2Modbus *mbs, uint8_t bp_index){
	CO_Slave *slave = mbs->base.slaves[bp_index];
	Battery *bat = mbs->cabinets[bp_index].battery;
	CO_CAN_Msg *mgs = &slave->base.tpdos[1].base.msg;

	for(uint8_t i = 0;i<4;i++){
		bat->cell_vol[i] = ((uint16_t)mgs->data[i*2+1]<<8) | ((uint16_t)mgs->data[i*2]);
	}
}
static void on_bp_get_data_from_tpdo3(Can2Modbus *mbs, uint8_t bp_index){
	CO_Slave *slave = mbs->base.slaves[bp_index];
	Battery *bat = mbs->cabinets[bp_index].battery;
	CO_CAN_Msg *mgs = &slave->base.tpdos[2].base.msg;
	for(uint8_t i = 0;i<4;i++){
		bat->cell_vol[i+4] = ((uint16_t)mgs->data[i*2+1]<<8) | ((uint16_t)mgs->data[i*2]);
	}
}
static void on_bp_get_data_from_tpdo4(Can2Modbus *mbs, uint8_t bp_index){
	CO_Slave *slave = mbs->base.slaves[bp_index];
	Battery *bat = mbs->cabinets[bp_index].battery;
	CO_CAN_Msg *mgs = &slave->base.tpdos[3].base.msg;
	if (mgs->is_new == true) {
		bat_set_temp(bat, (int16_t*) mgs->data);
		can_msg_set_new_flag(mgs, false);
	}
}
