/*
 * test1.c
 *
 *  Created on: Jun 30, 2022
 *      Author: Dakaka
 */

#include "app_co_init.h"
#include "unistd.h"


//#define TEST_PDO	1
//#define TEST_SDO_EXPEDITED_DOWNLOAD	1
//#define TEST_SDO_EXPEDITED_UPLOAD	1
#define TEST_SDO_NORMAL_DOWNLOAD	1
//#define TEST_SDO_NORMAL_UPLOAD	1

uint8_t test_tpdo1_data[8]={0};
uint8_t test_tpdo2_data[8]={0};
uint8_t test_tpdo3_data[8]={0};
uint8_t test_tpdo4_data[8]={0};

#ifdef TEST_PDO
int main(int argc,char** args)
{
	app_co_init();
	while(1)
	{

		CO_process(&canopen, 1);
		app_co_can_receive();
		usleep(1000);

		//test
		if(canopen.sync.sync_counter == canopen.tpdos[0].comm_parameter.transmission_type
				&&canopen.sync.time_in_cycle_ms == 5)
		{
			CO_memcpy(test_tpdo1_data, buffer_msg.data, 8);
		}
		if(canopen.sync.sync_counter == canopen.tpdos[1].comm_parameter.transmission_type
				&&canopen.sync.time_in_cycle_ms == 5)
		{
			CO_memcpy(test_tpdo2_data, buffer_msg.data, 8);
		}
		if(canopen.sync.sync_counter == canopen.tpdos[2].comm_parameter.transmission_type
				&&canopen.sync.time_in_cycle_ms == 5)
		{
			CO_memcpy(test_tpdo3_data, buffer_msg.data, 8);
		}
		if(canopen.sync.sync_counter == canopen.tpdos[3].comm_parameter.transmission_type
				&&canopen.sync.time_in_cycle_ms == 5)
		{
			CO_memcpy(test_tpdo4_data, buffer_msg.data, 8);
		}
	}
	return 0;
}
#endif

#ifdef TEST_SDO_EXPEDITED_DOWNLOAD
int main(int argc,char** args)
{
	canopen.tpdos[0].base.msg.id.valid = 0;
	app_co_init();
	uint32_t exam_var	= 1234;
	CO_Sub_Object exam_tx_obj =
	{
			.p_data = &exam_var,	//<< Address variable receiving data
			.attr	= ODA_SDO_RW,	//<< [skip] set ODA_SDO_RW
			.len	= 4,			//<< Maximum data size that can be received
			.p_ext	= NULL		//<< [skip] set NULL
	};
	CO_SDOclient_start_download(&canopen.sdo_client, 2, 0x1018, 0, &exam_tx_obj, 500);
	while(1)
	{

		CO_process(&canopen, 1);
		app_co_can_receive();
		usleep(1000);

		//check destination variable
		uint32_t check = CO_getUint32(canopen.p_od->list[17].subs[0].p_data);
		uint32_t stop = 0;
	}
	return 0;
}
#endif

#ifdef TEST_SDO_NORMAL_DOWNLOAD
int main(int argc,char** args)
{
	canopen.tpdos[0].base.msg.id.valid = 0;
	app_co_init();
	uint8_t clie_data_test_normal_sdo[100];
	for (int i = 0; i< 100; i++)
	{
		*(clie_data_test_normal_sdo + i) = i;
	}
	uint8_t serv_data_test_normal_sdo[100] = {0};
	CO_Sub_Object exam_tx_obj =
	{
			.p_data = &clie_data_test_normal_sdo,	//<< Address variable receiving data
			.attr	= ODA_SDO_RW,	//<< [skip] set ODA_SDO_RW
			.len	= 100,			//<< data size
			.p_ext	= NULL		//<< [skip] set NULL
	};
	CO_SDOclient_start_download(&canopen.sdo_client, 2, 0x30F9, 0, &exam_tx_obj, 500);
	while(1)
	{

		CO_process(&canopen, 1);
		app_co_can_receive();
		usleep(1000);

		//check destination variable
		CO_memcpy(serv_data_test_normal_sdo, od_data_test_normal_sdo, 100);
		uint32_t stop = 0;
	}
	return 0;
}
#endif

#ifdef TEST_SDO_EXPEDITED_UPLOAD
int main(int argc,char** args)
{
	canopen.tpdos[0].base.msg.id.valid = 0;
	app_co_init();
	uint8_t clie_data_test_expe_sdo[100];
	for (int i; i< 100; i++)
	{
		*(clie_data_test_expe_sdo + i) = i;
	}
	uint8_t serv_data_test_expe_sdo[2] = {0};
	CO_Sub_Object exam_rx_obj =
	{
			.p_data = &clie_data_test_expe_sdo,	//<< Address variable receiving data
			.attr	= ODA_SDO_RW,					//<< [skip] set ODA_SDO_RW
			.len	= 3,							//<< data size
			.p_ext	= NULL							//<< [option], set NULL if not used
	};
	CO_SDOclient_upload_obj_from_server_obj(&canopen.sdo_client, 2, 0x30F8, 0, &exam_rx_obj, 500);
	while(1)
	{

		CO_process(&canopen, 1);
		app_co_can_receive();
		usleep(1000);

		//check destination variable
		CO_memcpy(serv_data_test_expe_sdo, od_data_test_expe_sdo, 2);
		uint32_t stop = 0;
	}
	return 0;
}
#endif

#ifdef TEST_SDO_NORMAL_UPLOAD
int main(int argc,char** args)
{

	canopen.tpdos[0].base.msg.id.valid = 0;
	app_co_init();
	uint8_t clie_data_test_normal_sdo[100];
	for (int i; i< 100; i++)
	{
		*(clie_data_test_normal_sdo + i) = i;
	}
	uint8_t serv_data_test_normal_sdo[100] = {0};
	CO_Sub_Object exam_rx_obj =
	{
			.p_data = &clie_data_test_normal_sdo,	//<< Address variable receiving data
			.attr	= ODA_SDO_RW,					//<< [skip] set ODA_SDO_RW
			.len	= 100,							//<< data size
			.p_ext	= NULL							//<< [option], set NULL if not used
	};
	CO_SDOclient_upload_obj_from_server_obj(&canopen.sdo_client, 2, 0x30F9, 0, &exam_rx_obj, 500);
	while(1)
	{
		CO_process(&canopen, 1);
		app_co_can_receive();
		usleep(1000);

		//check destination variable
		CO_memcpy(serv_data_test_normal_sdo, od_data_test_normal_sdo, 100);
		uint32_t stop = 0;
	}
	return 0;
}
#endif
