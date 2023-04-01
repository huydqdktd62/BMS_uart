/*
 * can_master.h
 *
 *  Created on: Apr 1, 2021
 *      Author: KhanhDinh
 */

#ifndef SERVICE_CAN_MASTER_CAN_MASTER_H_
#define SERVICE_CAN_MASTER_CAN_MASTER_H_

#include <stdlib.h>
#include <stdint.h>
#include "CO.h"
#include "can_hw.h"
#include "CO_slave.h"

#define SLAVE_AUTHORIZING_WAIT_TIME_mS					(50)

#define BMS_MAINSWITCH_SDO_TIMEOUT_mS					500
#define CONFIG_DRIVE_MODE_SDO_TIMEOUT_mS				500
#define CONFIG_PROTECT_MOTOR_SDO_TIMEOUT_mS				500

#define HMI_READ_BP_SN_COBID							0x90
#define HMI_TRANS_APP_UPDATE							0x91
#define DRIVE_MODE_ECO_SPORT							0x2025
#define DRIVE_MODE_LOCK_MOTOR							0x2026
#define BMS_MAINSWITCH_INDEX							0x200301

#define CAN_NODE_ID_ASSIGN_COBID						0x70
#define SYNC_BP_DATA_COBID								0x80
#define SLAVE_SERIAL_NUMBER_OBJECT_INDEX				0x200300
#define SLAVE_ID_NUMBER_OBJECT_INDEX					0x200302
#define MC_NODE_NUMBER_OBJECT_INDEX						0x100800


typedef struct CAN_master_t CAN_master;
typedef void (*CAN_Master_Slave_Select)(const CAN_master*,const uint32_t);

typedef struct CO_Assign_Sv_t CO_Assign_Sv;
struct CO_Assign_Sv_t{

	CO_CAN_Msg rx_msg;
	CO_CAN_Msg tx_msg;
	CO_Slave* slave;
	uint32_t sync_mask;
	uint32_t sync_counter;
};

struct CAN_master_t{
	CO base;
	CO_Assign_Sv assign_sv;
	uint8_t is_active;
	uint32_t slave_num;
	uint32_t slave_start_node_id;
	uint32_t node_id_scan_cobid;
	CO_Slave**		slaves;
	uint32_t 		time_stamp;
	uint32_t pdo_sync_timestamp;
	uint32_t cnt;
	void (*on_slave_assign_success)(CAN_master* p_cm, const uint32_t timestamp);
	void (*on_slave_assign_fail)(CAN_master* p_cm, const uint32_t timestamp);
};

void cm_init(CAN_master* p_cm,CO_Slave** slaves, const uint32_t slave_num,CAN_Hw* p_hw);
void cm_process(CAN_master* p_cm,const uint32_t timediff);
void co_sdo_read_object(CAN_master* p_cm,const uint32_t mux,const uint32_t node_id,uint8_t* rx_buff,const uint64_t timeout);
void co_sdo_write_object(CAN_master* p_cm,const uint32_t mux,const uint32_t node_id,
		uint8_t* tx_buff,const uint32_t len,const uint64_t timeout);
/* find first unassign node and start assign can node id */
CO_Slave* cm_find_next_slave_to_assign(CAN_master* p_cm);

void cm_start_assign_slave(CAN_master *p_cm, CO_Slave *slave,
		const uint32_t timestamp);
void cm_update_slave_assign_state(CAN_master *p_cm,
		const uint64_t timediff);
void cm_send_sync(CAN_master *p_cm);

static inline CO_Slave* cm_get_assigning_slave(const CAN_master* const cm){
	return cm->assign_sv.slave;
}


static inline CO_Slave* cm_get_slave(const CAN_master* const cm, const uint8_t node_id){

	for(int i=0;i<cm->slave_num;i++){
		if(cm->slaves[i]->base.node_id==node_id){
			return (cm->slaves[i]);
		}
	}

	return NULL;
}



/* loop all connected node and check inactive time to see if the node has lost connection */
void cm_update_slave_inactive_state(CAN_master* p_cm,const uint32_t timediff);

#endif /* SERVICE_CAN_MASTER_CAN_MASTER_H_ */
