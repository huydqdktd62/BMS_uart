/*
 * node_id_hw.h
 *
 *  Created on: Apr 2, 2021
 *      Author: Admin
 */

#ifndef BOARD_STM32_BSP_NODE_ID_HW_NODE_ID_HW_H_
#define BOARD_STM32_BSP_NODE_ID_HW_NODE_ID_HW_H_

#include "stdint.h"

typedef enum NODE_ID_PIN_STATE_t{
        NODE_ID_PIN_ST_DESELECT                   =0,
        NODE_ID_PIN_ST_SELECT                     =1
}NODE_ID_PIN_STATE;


void node_id_hw_init(void);
NODE_ID_PIN_STATE node_id_pin_read_state(void);
#endif /* BOARD_STM32_BSP_NODE_ID_HW_NODE_ID_HW_H_ */
