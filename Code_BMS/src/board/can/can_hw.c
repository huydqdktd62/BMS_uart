/*
 * can_hw.c
 *
 *  Created on: Mar 28, 2023
 *      Author: Admin
 */

#include "can_hw.h"
#include "stdlib.h"
#include "string.h"

static void can_frame_init(can_frame_t* p_frame);
static void can_send_impl(CAN_Hw* p_hw);


/*START Variable*/
CAN_Hw can1={
                .baudrate=500000UL,
                .receive_handle=NULL,
                .send=can_send_impl,
                .node_id=0,
                .set_baudrate=NULL,
};
/*END Variable*/

void can_hardware_deinit(void){

}
void can_hardware_init(void){
	can1.p_driver = NULL;
    can_frame_init(&can1.tx_msg);
    can_frame_init(&can1.rx_msg);
    R_CAN_Open(&can_ctrl,&can_cfg);
}


static void can_frame_init(can_frame_t* p_frame){
	p_frame->id = 0;
	memset(p_frame->data,0,8);
	p_frame->data_length_code = 8;
}
static void can_send_impl(CAN_Hw* p_hw){
    R_CAN_Write(&can_ctrl, p_hw->mailbox,&p_hw->tx_msg);
}
/* Callback function */
void can_callback(can_callback_args_t *p_args)
{
    /* TODO: add your own code here */
    if(p_args->event != CAN_EVENT_RX_COMPLETE) return;
    memcpy(can1.rx_msg.data, p_args->frame.data, 8);
    can1.rx_msg.id = p_args->frame.id;
    if(can1.receive_handle != NULL)
        can1.receive_handle(&can1);
}
