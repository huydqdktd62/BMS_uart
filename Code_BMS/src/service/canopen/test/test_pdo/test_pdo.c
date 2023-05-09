/*
 * test_pdo.c
 *
 *  Created on: Jun 29, 2019
 *      Author: quangnd
 */

#include "stdbool.h"

#include "CO_PDO.h"
#include "CO_PDO.h"
#include "CO_TPDO.h"
#include "CO_RPDO.h"

static CO_PDO test_pdo;
static CO_TPDO test_tpdo;
static CO_RPDO test_rpdo;

static void test_pdo_handle(CO_PDO* p_pdo);

static bool test_pdo_map(void);
static bool test_rpdo_process(void);
static bool test_tpdo_process(void);

static uint8_t sub1_data[]={0x11,0x22,0x44};
static uint8_t sub2_data[]={0xAA,0xBB,0xCC,0xEE};
static CO_Object_Sub subs[]={{&sub1_data[0],0,3},{&sub2_data[0],0,4}};

static CAN_Msg test_msg;

static void test_setup(void){

	test_msg.cob_id=0x281;
	test_msg.data_len=8;
	test_msg.is_new=true;

	test_msg.data[0]=0x00;
	test_msg.data[1]=0x11;
	test_msg.data[2]=0x22;
	test_msg.data[3]=0x33;
	test_msg.data[4]=0x44;
	test_msg.data[5]=0x55;
	test_msg.data[6]=0x66;
	test_msg.data[7]=0x77;
}


int main(int argc,char** args){
	test_setup();
	pdo_set_handle(&test_pdo,test_pdo_handle);
	test_pdo_map();
	test_tpdo_process();
	test_rpdo_process();
	while(1){

	}
	return 0;
}

static void test_pdo_handle(CO_PDO* p_pdo){

	return;
}

static bool test_pdo_map(void){

	CO_Object map_obj={0x1000,2,&subs[0]};
	pdo_map_object(&test_pdo,&map_obj,1,4);
	pdo_map_object((CO_PDO*)&test_tpdo,&map_obj,0,3);
	pdo_map_object((CO_PDO*)&test_tpdo,&map_obj,1,4);

	pdo_map_object((CO_PDO*)&test_rpdo,&map_obj,0,3);
	pdo_map_object((CO_PDO*)&test_rpdo,&map_obj,1,4);
	return true;
}

static bool test_tpdo_process(void){

	tpdo_init(&test_tpdo,0x281);

	pdo_set_msg((CO_PDO*)&test_tpdo,&test_msg);
	pdo_handle((CO_PDO*)&test_tpdo);
	return true;
}

static bool test_rpdo_process(void){
	rpdo_init(&test_rpdo,0x281);

	pdo_set_msg((CO_PDO*)&test_rpdo,&test_msg);
	pdo_handle((CO_PDO*)&test_rpdo);

	return true;
}
