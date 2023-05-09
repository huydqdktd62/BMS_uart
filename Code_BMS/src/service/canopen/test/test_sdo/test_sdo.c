/*
 * test_sdo.c
 *
 *  Created on: Jul 1, 2019
 *      Author: quangnd
 */
#include "CO_OD.h"
#include "CO_Object.h"
#include  "CO_SDO.h"
#include "stdbool.h"
#include "stdio.h"

#define EXPEDIT_WRITE_CCS 	((CCS_DOWNLOAD_INITIATE<<4)+ (3<<2) + (1<<1))
#define SEGMENT_WRITE_CCS_INIT 	((CCS_DOWNLOAD_INITIATE<<4)+ (3<<2))
#define SEGMENT_WRITE_CCS_CONT 	((CCS_DOWNLOAD_SEGMENT<<4)+ (3<<2))

#define EXPEDITED_READ_CCS 	(CCS_UPLOAD_INITIATE<<4)
#define SEGMENT_READ_CCS_INIT 	(CCS_UPLOAD_INITIATE<<4)
#define SEGMENT_READ_CCS_CONT 	((CCS_UPLOAD_SEGMENT<<4)+(3<<2))

static uint32_t var11=0x11223344;
static uint16_t var12=0x1212;

static uint16_t var21=0x2121;
static uint16_t var22=0x2222;

static uint8_t var31=0x31;
static uint8_t var32=0x32;
static uint8_t var33=0x33;
static uint8_t device_name[]={'S','M','C'};

static CO_OD od;

static CO_Object_Sub obj1_subs[]={
		{(void*)&var11,0,4},
		{(void*)&var12,0,2}
};

static CO_Object_Sub obj2_subs[]={
		{(void*)&var21,0,2},
		{(void*)&var22,0,2}
};

static CO_Object_Sub obj3_subs[]={
		{(void*)&var31,0,1},
		{(void*)&var32,0,1},
		{(void*)&var33,0,1}
};


static uint8_t long_obj_data1[]={0x22,0x11,0x22,0x33,0x44,0x55,0x66,0x11,0x22,0x33,0x44,0x55,0x66,0x33,0x22};
static uint8_t long_obj_data2[22]={0xFF,0xAA,0xBB,0xCC,0x44,0x55,0x66,0x11,0x22,0x33,0x44,0x55,0x66,0x33,0x22};

static CO_Object_Sub obj4_subs[]={
		{(void*)long_obj_data1,0,15},
		{(void*)long_obj_data2,0,15}
};


static CO_Object od_entries[]={
		{0x1000,2,obj1_subs},
		{0x2000,2,obj2_subs},
		{0x3000,3,obj3_subs},
		{0x6000,2,obj4_subs},
};

static CO_SDO test_sdo;

static CAN_Msg msgs[]={
		{0x281,0x02,{EXPEDIT_WRITE_CCS,1,2,2,4,0,2,2},0x08,true},
		{0x281,0x02,{EXPEDIT_WRITE_CCS,1,2,2,4,0,2,3},0x08,true},
		{0x281,0x02,{EXPEDIT_WRITE_CCS,1,2,2,4,4,3,5},0x08,true},
		{0x281,0x02,{EXPEDIT_WRITE_CCS,1,2,2,4,8,9,7},0x08,true}
};

static CAN_Msg write_segment_msgs[]={
		{0x281,0x02,{SEGMENT_WRITE_CCS_INIT,1,2,2,4,0,2,2},0x08,true},
		{0x281,0x02,{SEGMENT_WRITE_CCS_CONT,1,2,3,4,5,6,7},0x08,true},
		{0x281,0x02,{SEGMENT_WRITE_CCS_CONT,8,9,10,11,12,13,14},0x08,true},
		{0x281,0x02,{SEGMENT_WRITE_CCS_CONT,15,16,17,18,19,20,21},0x08,true},
		{0x281,0x02,{SEGMENT_WRITE_CCS_CONT,15,16,17,18,19,20,21},0x08,true}
};

static CAN_Msg read_expedited_msgs[]={
		{0x281,0x02,{EXPEDITED_READ_CCS},0x08,true},
};

static CAN_Msg read_segment_msgs[]={
		{0x281,0x02,{SEGMENT_READ_CCS_INIT},0x08,true},
		{0x281,0x02,{SEGMENT_READ_CCS_CONT},0x08,true},
		{0x281,0x02,{SEGMENT_READ_CCS_CONT},0x08,true},
		{0x281,0x02,{SEGMENT_READ_CCS_CONT},0x08,true},
		{0x281,0x02,{SEGMENT_READ_CCS_CONT},0x08,true},
};

static bool test_sdo_expedited_write(void);
static bool test_sdo_segmented_write(void);
static bool test_sdo_expedited_read(void);
static bool test_sdo_segmented_read(void);

static void od_init(void){
	od.number=4;
	od.list=od_entries;
}


int main(int argc,char** args){

	bool pass=false;
	od_init();

	pass=test_sdo_expedited_write();
	printf("Test pass %d",pass);
	pass=test_sdo_segmented_write();

	test_sdo_expedited_read();

	test_sdo_segmented_read();
	return 0;
}

static bool test_sdo_expedited_write(void){

	uint16_t index=0x2000;
	uint8_t sub=0x01;

	CO_memcpy(&msgs[0].data[1],(uint8_t*)&index,2);
	CO_memcpy(&msgs[1].data[1],(uint8_t*)&index,2);
	CO_memcpy(&msgs[2].data[1],(uint8_t*)&index,2);
	CO_memcpy(&msgs[3].data[1],(uint8_t*)&index,2);

	msgs[0].data[3]=sub;
	msgs[1].data[3]=sub;
	msgs[2].data[3]=sub;
	msgs[3].data[3]=sub;

	test_sdo.allowed_timeout_ms=100;
	for(int i=0;i<4;i++){
		sdo_set_msg(&test_sdo,&msgs[i]);
		CO_SDOserver_process(&test_sdo,&od,4);
	}
	return false;
}

static bool test_sdo_segmented_write(void){

	uint16_t index=0x6000;
	uint8_t sub=0x01;
	uint32_t len=22;

	CO_memcpy(&write_segment_msgs[0].data[1],(uint8_t*)&index,2);
	CO_memcpy(&write_segment_msgs[0].data[4],(uint8_t*)&len,4);
	write_segment_msgs[0].data[3]=sub;
	test_sdo.allowed_timeout_ms=100;
	for(int i=0;i<5;i++){
		sdo_set_msg(&test_sdo,&write_segment_msgs[i]);
		CO_SDOserver_process(&test_sdo,&od,4);
	}
	return true;
}

static bool test_sdo_expedited_read(void){

	uint16_t index=0x1000;
	uint8_t sub=0x00;

	CO_memcpy(&read_expedited_msgs[0].data[1],(uint8_t*)&index,2);
	read_expedited_msgs[0].data[3]=sub;
	sdo_set_msg(&test_sdo,&read_expedited_msgs[0]);
	CO_SDOserver_process(&test_sdo,&od,5);
	return true;
}

static bool test_sdo_segmented_read(void){

	static uint8_t tx_byte_buffers[25]={0};
	static uint8_t* dst=&tx_byte_buffers[0];
	uint16_t index=0x6000;
	uint8_t sub=0x01;

	CO_memcpy(&read_expedited_msgs[0].data[1],(uint8_t*)&index,2);
	read_expedited_msgs[0].data[3]=sub;

	for(int i=0;i<5;i++){
		sdo_set_msg(&test_sdo,&read_segment_msgs[i]);
		CO_SDOserver_process(&test_sdo,&od,6);
		if(i!=0){
			CO_memcpy(dst,&test_sdo.tx_msg.data[1],7);
			dst+=7;
		}
	}

	return true;
}
