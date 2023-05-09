/*
 * app_co_init.c
 *
 *  Created on: Jun 17, 2022
 *      Author: Dakaka
 */

#include "app_co_init.h"
#include "od.h"
#include <stdbool.h>
#include "can_hw.h"
/********************************************************************************************
 * 									MANDATORY: Edit carefully !!!
 * *******************************************************************************************/
/* Declare for build TPDO message function when using "tpdo_build_data_manually" mode*/

const char* TAG = "CAN_PDO";

static void tpdo1_build_data_impl(uint8_t *buffer);
static void tpdo2_build_data_impl(uint8_t *buffer);
static void tpdo3_build_data_impl(uint8_t *buffer);
static void tpdo4_build_data_impl(uint8_t *buffer);

void *tpdo_build_data_impl[TPDO_NUMBER] = { tpdo1_build_data_impl,
		tpdo2_build_data_impl, tpdo3_build_data_impl, tpdo4_build_data_impl };

static void tpdo1_build_data_impl(uint8_t *buffer) {
	(void) buffer;
}

static void tpdo2_build_data_impl(uint8_t *buffer) {
	(void) buffer;
}

static void tpdo3_build_data_impl(uint8_t *buffer) {
	(void) buffer;
}

static void tpdo4_build_data_impl(uint8_t *buffer) {
	(void) buffer;
}

/* Define can_send message function */
static void app_co_can_send_impl(CO_CAN_Msg *p_msg) {
	CO_memcpy(can1.tx_msg.data, p_msg->data, 8);
	can1.tx_msg.id = p_msg->id.can_id;
	can1.tx_msg.data_length_code = p_msg->data_len;
	can1.send(&can1);
}

/* Call in receive can interrupt */
static void app_co_can_receive(CAN_Hw *p_hw) {
	uint32_t cob_id = p_hw->rx_msg.id;
	if (CO_can_receive_basic_handle(&CO_DEVICE, cob_id, p_hw->rx_msg.data)
			== 1) {
	}



	/* USER CODE BEGIN */
	if (cob_id == 0x70) {
		memcpy(selex_can2mb.base.assign_sv.rx_msg.data, p_hw->rx_msg.data,
				p_hw->rx_msg.data_length_code);
		selex_can2mb.base.assign_sv.rx_msg.data_len =
				p_hw->rx_msg.data_length_code;
		selex_can2mb.base.assign_sv.rx_msg.is_new = true;
	}

	uint8_t node_id=(uint8_t)(cob_id & 0x0000007F);
	CO_Slave* slave=cm_get_slave((CAN_master*)&selex_can2mb,node_id);

	//uint32_t func_code=cob_id & 0xFFFFFF80;
	if(slave !=NULL){

		for(int i=0;i<TPDO_NUMBER;i++){
			if(cob_id == slave->base.tpdos[i].comm_parameter.id.can_id ){
				memcpy(slave->base.tpdos[i].base.msg.data,p_hw->rx_msg.data,8);
				can_msg_set_new_flag(&(slave->base.tpdos[i].base.msg),true);
				//ESP_LOGI(TAG,"TPDO %d - slave %d\n",i,slave->id);
				//ESP_LOG_BUFFER_HEX(TAG,slave->base.tpdos[i].base.msg.data,8);
				slave->inactive_time_ms=0;
				break;
			}
		}
		if (cob_id == slave->base.node_id + 0x200) {
			uint16_t *p_cell_voll = selex_can2mb.cabinets[slave->base.node_id-5].battery->cell_vol;
			for(uint16_t i=0;i<4;i++){
				p_cell_voll[i+8] = ((uint16_t)p_hw->rx_msg.data[i*2+1]<<8) | ((uint16_t)p_hw->rx_msg.data[i*2]);
			}
		}
		if (cob_id == slave->base.node_id + 0x300) {
			uint16_t *p_cell_voll = selex_can2mb.cabinets[slave->base.node_id-5].battery->cell_vol;
			for(uint16_t i=0;i<4;i++){
				p_cell_voll[i+12] = ((uint16_t)p_hw->rx_msg.data[i*2+1]<<8) | ((uint16_t)p_hw->rx_msg.data[i*2]);
			}
		}
		if (cob_id == slave->base.node_id + 0x400) {
			uint32_t p_cycles = ((uint32_t)p_hw->rx_msg.data[1]<<8) + p_hw->rx_msg.data[0];
			uint8_t p_soh = p_hw->rx_msg.data[2];
			bat_set_cycles(selex_can2mb.cabinets[slave->base.node_id-5].battery, p_cycles);
			bat_set_soh(selex_can2mb.cabinets[slave->base.node_id-5].battery, p_soh);
		}
	}
	/* USER CODE END */
}


/* Define erase_and_write_data_to_flash function
 * Return: success(0), fail(-1)
 */
int8_t app_co_erase_and_write_data_to_flash_impl(uint32_t addr, uint32_t buff,
		uint32_t len) {
	return 0;
}

/* Define flash_error_handle function */
void app_co_flash_error_handle_impl(void) {

}

/* Define set CAN baud-rate function */
void app_co_set_can_baudrate_impl(uint32_t br_khz) {

}

void app_co_init(void) {
	/* [Mandatory] Set CO_CAN_send interface */
	CO_CAN_set_can_send_interface(app_co_can_send_impl);
	can1.receive_handle = app_co_can_receive;
	/* [Mandatory] Init CO object */
#if CO_FLASH__USE
	CO_FLASH_init(&CO_DEVICE.flash,
			app_co_erase_and_write_data_to_flash_impl,
			app_co_flash_error_handle_impl);
#endif
#if APP_OD_STORAGE__USE
#if CO_FLASH__USE & (SW_VERSION != 0)
	CO_OD_STORAGE_init(&CO_DEVICE.storage,
			APP_OD_STORAGE__STORAGE_REGION__ADDR,
			APP_OD_STORAGE__STORAGE_REGION__SIZE,
			&od_temp_comm_profile_para,
			&od_temp_manu_profile_para,
			sizeof(OD_Temp_Manufacturer_Profile_Para_t),
			&CO_DEVICE.flash,
			p_co_od,
			SW_VERSION,
			IS_MAIN_APP);
#else
	while(1); /* Code fail */
#endif
#endif

	CO_init_basic(&CO_DEVICE, od_temp_comm_profile_para.x1000_device_type,
			&od_temp_comm_profile_para.x1018_identity, p_co_od);

	CO_SYNC_init(&CO_DEVICE.sync, &od_temp_comm_profile_para);

	for (uint8_t i = 0; i < TPDO_NUMBER; i++) {
		CO_TPDO_init(&CO_DEVICE.tpdos[i], &CO_DEVICE.sync,
				&od_temp_comm_profile_para.x1A0x_tpdo_map_para[i],
				CO_DEVICE.p_od,
				&od_temp_comm_profile_para.x180x_tpdo_comm_para[i],
				tpdo_build_data_impl[i]);
	}
	CO_SDOserver_init(&CO_DEVICE.sdo_server, &od_temp_comm_profile_para,
			&CO_DEVICE.sync,
			CO_DEVICE.p_od);
	CO_SDOclient_init(&CO_DEVICE.sdo_client, &od_temp_comm_profile_para,
			&CO_DEVICE.sync);

	CO_set_node_id(&CO_DEVICE, od_temp_manu_profile_para.x500300_node_ID);
	app_co_set_can_baudrate_impl(
			od_temp_manu_profile_para.x500301_baud_rate_kHz);

	/* USER CODE BEGIN */

	/* USER CODE END */
}

/********************************************************************************************
 * 									USER CODE
 * *******************************************************************************************/
void co_send_ex_boot_request(CAN_Hw *p_hw){
    p_hw->tx_msg.id = 0x02;
    p_hw->tx_msg.data[0] = 1;
    p_hw->tx_msg.data_length_code = 1;
    p_hw->send(p_hw);
}
