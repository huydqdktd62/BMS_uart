/*
 * bsp_can.h
 *
 *  Created on: Mar 27, 2023
 *      Author: Admin
 */

#ifndef BOARD_CAN_CAN_HW_H_
#define BOARD_CAN_CAN_HW_H_

#include <stdio.h>
#include "hal_data.h"

typedef struct CAN_Hw_t CAN_Hw;

typedef void (*CAN_Receive_Handle)(CAN_Hw *p_hw);
typedef void (*CAN_Set_Baudrate)(CAN_Hw *p_hw, const uint32_t baud);
typedef void (*CAN_Send)(CAN_Hw *p_hw);



struct CAN_Hw_t {
        uint32_t mailbox;
        uint8_t node_id;
        uint32_t baudrate;
        CAN_Receive_Handle receive_handle;
        CAN_Set_Baudrate set_baudrate;
        CAN_Send send;
        can_frame_t tx_msg;
        can_frame_t rx_msg;
        void* p_driver;
};

extern CAN_Hw can1;

void can_hardware_deinit(void);
void can_hardware_init(void);

#endif /* BOARD_CAN_CAN_HW_H_ */
