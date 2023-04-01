/*
 * app_co_init.c
 *
 *  Created on: Jun 17, 2022
 *      Author: Dakaka
 */

#include "app_co_init.h"
#include "od.h"
#include <stdbool.h>

/********************************************************************************************
 * 									MANDATORY: Edit carefully !!!
 * *******************************************************************************************/
/* Declare for build TPDO message function when using "tpdo_build_data_manually" mode*/
static void tpdo1_build_data_impl(uint8_t* buffer);
static void tpdo2_build_data_impl(uint8_t* buffer);
static void tpdo3_build_data_impl(uint8_t* buffer);
static void tpdo4_build_data_impl(uint8_t* buffer);

void* tpdo_build_data_impl[TPDO_NUMBER] =
{
		tpdo1_build_data_impl,
		tpdo2_build_data_impl,
		tpdo3_build_data_impl,
		tpdo4_build_data_impl
};

static void tpdo1_build_data_impl(uint8_t* buffer)
{
	(void)buffer;
}

static void tpdo2_build_data_impl(uint8_t* buffer)
{
	(void)buffer;
}

static void tpdo3_build_data_impl(uint8_t* buffer)
{
	(void)buffer;
}

static void tpdo4_build_data_impl(uint8_t* buffer)
{
	(void)buffer;
}

/* Define can_send message function */
static void app_co_can_send_impl(CO_CAN_Msg* p_msg)
{

}

/* Call in receive can interrupt */
void app_co_can_receive(const uint32_t can_id, uint8_t* data)
{
	if(CO_can_receive_basic_handle(&CO_DEVICE, can_id, data)) ;
	/* USER CODE BEGIN */

	/* USER CODE END */
}

/* Define erase_and_write_data_to_flash function
 * Return: success(0), fail(-1)
 */
int8_t app_co_erase_and_write_data_to_flash_impl(uint32_t addr, uint32_t buff, uint32_t len)
{

}

/* Define flash_error_handle function */
void app_co_flash_error_handle_impl(void)
{

}

/* Define set CAN baud-rate function */
void app_co_set_can_baudrate_impl(uint32_t br_khz)
{

}

void app_co_init(void)
{
	/* [Mandatory] Set CO_CAN_send interface */
	CO_CAN_set_can_send_interface(app_co_can_send_impl);

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

	CO_init_basic(&CO_DEVICE,
			od_temp_comm_profile_para.x1000_device_type,
			&od_temp_comm_profile_para.x1018_identity,
			p_co_od);

	CO_SYNC_init(&CO_DEVICE.sync, &od_temp_comm_profile_para);

	for(uint8_t i = 0; i < TPDO_NUMBER; i++)
	{
		CO_TPDO_init(&CO_DEVICE.tpdos[i],
				&CO_DEVICE.sync,
				&od_temp_comm_profile_para.x1A0x_tpdo_map_para[i],
				CO_DEVICE.p_od,
				&od_temp_comm_profile_para.x180x_tpdo_comm_para[i],
				tpdo_build_data_impl[i]);
	}
	CO_SDOserver_init(&CO_DEVICE.sdo_server,
			&od_temp_comm_profile_para,
			&CO_DEVICE.sync,
			CO_DEVICE.p_od);
	CO_SDOclient_init(&CO_DEVICE.sdo_client,
			&od_temp_comm_profile_para,
			&CO_DEVICE.sync);

	CO_set_node_id(&CO_DEVICE, od_temp_manu_profile_para.x500300_node_ID);
	app_co_set_can_baudrate_impl(od_temp_manu_profile_para.x500301_baud_rate_kHz);


	/* USER CODE BEGIN */

	/* USER CODE END */
}


/********************************************************************************************
 * 									USER CODE
 * *******************************************************************************************/

