/*
 * can_hw.c
 *
 *  Created on: Jan 24, 2022
 *      Author: quangnd
 */
#include "can_hw.h"
#include "common_utils.h"

static void can_frame_init(can_frame_t* p_frame);
static void can_send_impl(CAN_Hw* p_hw);

CAN_Hw can1={
                .p_can_cfg=&g_can0_cfg,
                .p_can_ctrl=&g_can0_ctrl,
                .baudrate=500UL,
                .receive_handle=NULL,
                .send=can_send_impl,
                .node_id=0,
                .set_baudrate=NULL,
};


void can_hw_init(void){

    can_frame_init(&can1.tx_msg);
    can_frame_init(&can1.rx_msg);
    R_CAN_Open(can1.p_can_ctrl, can1.p_can_cfg);
}
void can_hw_deinit(void){

    R_CAN_Close(can1.p_can_ctrl);
}

static void can_frame_init(can_frame_t* p_frame){
    p_frame->data_length_code = 8;
    p_frame->id_mode = CAN_ID_MODE_STANDARD;
    p_frame->type = CAN_FRAME_TYPE_DATA;
}

/* Callback function */
void can_callback(can_callback_args_t *p_args)
{
    /* TODO: add your own code here */
    if(p_args->event != CAN_EVENT_RX_COMPLETE) return;
    memcpy(can1.rx_msg.data, p_args->frame.data, 8);
    can1.rx_msg.id = p_args->frame.id;
    can1.receive_handle(&can1);
}

static void can_send_impl(CAN_Hw* p_hw){
    R_CAN_Write(p_hw->p_can_ctrl, p_hw->mailbox, &p_hw->tx_msg);
}

