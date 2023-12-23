/*
 * can_hw.h
 *
 *  Created on: Jan 24, 2022
 *      Author: quangnd
 */

#ifndef BOARD_RA_BOARD_CAN_HW_CAN_HW_H_
#define BOARD_RA_BOARD_CAN_HW_CAN_HW_H_

#include "stdint.h"
#include "hal_data.h"

#define ENABLE_CAN_IRQ
#define DISABLE_CAN_IRQ

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

        const can_cfg_t* const p_can_cfg;
        can_instance_ctrl_t* const p_can_ctrl;
};

extern CAN_Hw can1;

void can_hw_init(void);
void can_hw_deinit(void);



#endif /* BOARD_RA_BOARD_CAN_HW_CAN_HW_H_ */
