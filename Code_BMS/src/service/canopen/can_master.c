/*
 * can_master.c
 *
 *  Created on: Apr 1, 2021
 *      Author: KhanhDinh
 */

#include "can_master.h"


#define ASSIGN_TIMEOUT_mS								600
#define ASSIGN_START_TIMEOUT_mS							1000
#define ACK_TIMEOUT_mS									1000
#define NODEID_CONFIRM_TIMEOUT_mS						1000
#define SLAVE_SELECT_TIMEOUT_mS							1000
#define AUTHORIZING_TIMEOUT_mS							5000
#define READ_SN_SDO_TIMEOUT_mS							5000
#define PDO_SYNC_TIME_INTERVAL_mS						5000

static void cm_assign_sv_process(CAN_master* p_cm,const uint32_t timediff);

CAN_master* can_master_construct(void) {
	CAN_master *p_cm = (CAN_master*) malloc(sizeof(CAN_master));
	while (p_cm == NULL)
		;
	return p_cm;
}

void cm_init(CAN_master *p_cm, CO_Slave **slaves,
		const uint32_t slave_num, CAN_Hw *p_hw) {

	p_cm->node_id_scan_cobid = CAN_NODE_ID_ASSIGN_COBID;
	p_cm->slaves = slaves;
	p_cm->slave_num = slave_num;
}

void cm_process(CAN_master* p_cm,const uint32_t timediff){
	CO_process((CO*)&p_cm->base,timediff);
	cm_assign_sv_process(p_cm, timediff);
}

void cm_update_slave_inactive_state(CAN_master* p_cm,const uint32_t timediff){

	for(uint8_t i=0;i<p_cm->slave_num;i++){
		if(co_slave_get_assign_state(p_cm->slaves[i])==CO_ASSIGN_ST_CONNECTED){
			p_cm->slaves[i]->inactive_time_ms+=timediff;
			if(p_cm->slaves[i]->inactive_time_ms>SLAVE_INACTIVE_TIME_THRESHOLD_mS){
				co_slave_set_assign_state(p_cm->slaves[i], CO_ASSIGN_ST_LOST_CONNECTED);
			}
		}
	}
}

void cm_send_sync(CAN_master *p_cm) {

	CO_CAN_send(&p_cm->base.sync.msg);
	//can_send_func(p_cm->p_hw, SYNC_BP_DATA_COBID, 0, p_cm->p_hw->tx_data);
}

CO_Slave* cm_find_next_slave_to_assign(CAN_master *p_cm) {

	for (uint8_t i = 0; i < p_cm->slave_num; i++) {
		if (co_slave_get_assign_state(p_cm->slaves[i])
				== CO_ASSIGN_ST_INIT) {
			return (p_cm->slaves[i]);
		}
	}
	return NULL;
}

void cm_start_assign_slave(CAN_master *p_cm, CO_Slave *slave,
		const uint32_t timestamp) {
	co_slave_set_assign_state(slave,CO_ASSIGN_ST_WAIT_REQUEST);
	/* clear serial number */
	memset((void*)slave->base.dev_id,0,DEVICE_ID_LEN);
	slave->assign_timeout=timestamp+ASSIGN_TIMEOUT_mS;
	slave->assign_time_ms=0;
	co_slave_deselect(slave);
	p_cm->assign_sv.slave=slave;
}

void co_sdo_read_object(CAN_master *p_cm, const uint32_t mux,
		const uint32_t node_id, uint8_t *rx_buff, const uint64_t timeout) {

	CO_Sub_Object tx_obj = { .p_data = rx_buff,	//<< Address variable receiving data
			.attr = ODA_SDO_RW,	//<< [skip] set ODA_SDO_RW
			.len = 32,			//<< data size
			.p_ext = NULL		//<< [skip] set NULL
	};

	CO_SDOclient_start_upload(&p_cm->base.sdo_client, node_id, (mux >> 8),
			(uint8_t)mux, &tx_obj, timeout);
}

void co_sdo_write_object(CAN_master *p_cm, const uint32_t mux,
		const uint32_t node_id, uint8_t *tx_buff, const uint32_t len,
		const uint64_t timeout) {

	CO_Sub_Object tx_obj = { .p_data = tx_buff,	//<< Address variable receiving data
			.attr = ODA_SDO_RW,	//<< [skip] set ODA_SDO_RW
			.len = len,			//<< data size
			.p_ext = NULL		//<< [skip] set NULL
	};
	CO_SDOclient_start_download(&p_cm->base.sdo_client, node_id, (mux >> 8),
			mux, &tx_obj, timeout);
}

static void cm_assign_sv_process(CAN_master* p_cm,const uint32_t timediff){

	bool in_window = (p_cm->base.sync.sync_counter == p_cm->assign_sv.sync_counter)
					&& CO_SYNC_is_match_sync_mask_reg(&(p_cm->base.sync), p_cm->assign_sv.sync_mask);

	if (!in_window){
		return;
	}

	if(p_cm->assign_sv.tx_msg.is_new == true){
		CO_CAN_send(&p_cm->assign_sv.tx_msg);

		if(co_slave_get_assign_state(p_cm->assign_sv.slave)==
				CO_ASSIGN_ST_WAIT_REQUEST) {

			co_slave_set_assign_state(p_cm->assign_sv.slave,CO_ASSIGN_ST_START);
			p_cm->assign_sv.slave->assign_timeout=
					p_cm->assign_sv.slave->assign_time_ms+ASSIGN_START_TIMEOUT_mS;
		}
		else if(co_slave_get_assign_state(p_cm->assign_sv.slave)==
				CO_ASSIGN_ST_SELECT_ACK){

			co_slave_set_assign_state(p_cm->assign_sv.slave,CO_ASSIGN_ST_NODEID_CONFIRM);
			p_cm->assign_sv.slave->assign_timeout=
					p_cm->assign_sv.slave->assign_time_ms+ACK_TIMEOUT_mS;
		}
		can_msg_set_new_flag(&p_cm->assign_sv.tx_msg,false);
		return;
	}

	if(p_cm->assign_sv.rx_msg.is_new == false) return;

	if (p_cm->assign_sv.slave != NULL) {
		switch (co_slave_get_assign_state(p_cm->assign_sv.slave)) {
		case CO_ASSIGN_ST_WAIT_REQUEST:
			p_cm->assign_sv.tx_msg.id.cob_id=0x70;
			p_cm->assign_sv.tx_msg.data_len=0;
			can_msg_set_new_flag(&(p_cm->assign_sv.tx_msg),true);

			break;
		case CO_ASSIGN_ST_SLAVE_SELECT:
			co_slave_set_assign_state(p_cm->assign_sv.slave,
					CO_ASSIGN_ST_SELECT_ACK);
			p_cm->assign_sv.slave->assign_timeout=
					p_cm->assign_sv.slave->assign_time_ms+SLAVE_SELECT_TIMEOUT_mS;

			break;
		case CO_ASSIGN_ST_NODEID_CONFIRM:
			p_cm->assign_sv.slave->base.node_id =
					p_cm->assign_sv.rx_msg.data[0];
			co_slave_set_assign_state(p_cm->assign_sv.slave,
					CO_ASSIGN_ST_AUTHORIZING_REQ);
			p_cm->assign_sv.slave->assign_timeout=
					p_cm->assign_sv.slave->assign_time_ms+NODEID_CONFIRM_TIMEOUT_mS;
			p_cm->assign_sv.slave->authorizing_wait_time=0;
			printf("node id: %d\n",p_cm->assign_sv.slave->base.node_id);
			break;
		default:
			break;
		}
	}
	can_msg_set_new_flag(&p_cm->assign_sv.rx_msg, false);
}

void cm_update_slave_assign_state(CAN_master *p_cm,
		const uint64_t timediff) {

	CO_Slave* slave=p_cm->assign_sv.slave;
	CO_CAN_Msg* tx_msg=&p_cm->assign_sv.tx_msg;

	if(slave==NULL) return;

	if(CO_ASSIGN_ST_CONNECTED==co_slave_get_assign_state(slave)) return;

	slave->assign_time_ms+=timediff;
	if(co_slave_is_assign_timeout(slave)){
		/* if the node is in disconnected before */
		if(co_slave_get_assign_state(slave)==CO_ASSIGN_ST_WAIT_REQUEST){
			co_slave_set_assign_state(slave, CO_ASSIGN_ST_DISCONNECTED);
		}else{
			co_slave_set_assign_state(slave, CO_ASSIGN_ST_FAIL);
		}
	}

	switch (co_slave_get_assign_state(slave)) {
	case CO_ASSIGN_ST_WAIT_REQUEST:
		/* timeout*/
		break;
	case CO_ASSIGN_ST_START:
		co_slave_set_assign_state(slave, CO_ASSIGN_ST_SLAVE_SELECT);
		/* set node id pin to low to select slave*/
		co_slave_select(slave);
		break;
	case CO_ASSIGN_ST_SLAVE_SELECT:
		/* do nothing */
		/* wait for slave select confirm message */
		break;
	case CO_ASSIGN_ST_SELECT_ACK:

		/* after slave has selected and got acknowledge */
		/* prepare set node id message */
		tx_msg->id.can_id= CAN_NODE_ID_ASSIGN_COBID;
		tx_msg->data_len=1;
		tx_msg->data[0]=slave->base.node_id;
		can_msg_set_new_flag(tx_msg, true);
		break;
	case CO_ASSIGN_ST_NODEID_CONFIRM:

		/* do nothing */
		/* wait for set node id message response from slave */
		break;
	case CO_ASSIGN_ST_AUTHORIZING_REQ:

		/* after rising node id pin, wait 50ms for slave to switch state */
		slave->authorizing_wait_time+=timediff;
		if(slave->authorizing_wait_time>=SLAVE_AUTHORIZING_WAIT_TIME_mS){
			co_slave_set_assign_state(slave, CO_ASSIGN_ST_AUTHORIZING_START);
			slave->assign_timeout=slave->assign_time_ms+AUTHORIZING_TIMEOUT_mS;
		}
		CO_SDO_reset_status(&p_cm->base.sdo_client);
		co_slave_deselect(slave);
	break;
	case CO_ASSIGN_ST_AUTHORIZING_START:
		/* wait for previous sdo session to finish */
		if (CO_SDO_get_status(&p_cm->base.sdo_client) != CO_SDO_RT_idle)
			return;
		/* start sdo session to read serial number of slave node */
		co_sdo_read_object(p_cm, SLAVE_SERIAL_NUMBER_OBJECT_INDEX,
						slave->base.node_id,
						(uint8_t*)slave->base.dev_id,READ_SN_SDO_TIMEOUT_mS);
		co_slave_set_assign_state(slave, CO_ASSIGN_ST_AUTHORIZING);
		slave->assign_timeout=slave->assign_time_ms+AUTHORIZING_TIMEOUT_mS;
		break;
	case CO_ASSIGN_ST_AUTHORIZING:
		if (CO_SDO_get_status(&p_cm->base.sdo_client) == CO_SDO_RT_abort) {
			CO_SDO_reset_status(&p_cm->base.sdo_client);
			co_slave_set_assign_state(slave, CO_ASSIGN_ST_FAIL);

		} else if (CO_SDO_get_status(&p_cm->base.sdo_client)== CO_SDO_RT_success) {
			size_t id_len = strlen(slave->base.dev_id);
			uint16_t num_swap = id_len/2;
			char swap = 0;
			for(uint16_t i = 0;i<num_swap;i++){
				id_len = id_len - 1;
				swap = slave->base.dev_id[id_len];
				slave->base.dev_id[id_len] = slave->base.dev_id[i];
				slave->base.dev_id[i] = swap;
			}
			CO_SDO_reset_status(&p_cm->base.sdo_client);

			co_slave_set_assign_state(slave, CO_ASSIGN_ST_CONNECTED);
		}
		break;
	case CO_ASSIGN_ST_CONNECTED:

		break;
	case CO_ASSIGN_ST_FAIL:

		break;
	default:
		break;
	}
}


