/*
 * key_hw.h
 *
 *  Created on: Oct 20, 2020
 *      Author: quangnd
 */

#ifndef BOARD_STM32_BSP_KEY_HW_KEY_HW_H_
#define BOARD_STM32_BSP_KEY_HW_KEY_HW_H_
#include "stdint.h"

typedef enum KEY_STATE_t{
        KEY_ST_PRESSED                  =0,
        KEY_ST_RELEASED                 =1,
        KEY_ST_HOLD                     =2
}KEY_STATE;

void key_hw_init(void);
KEY_STATE key_read(void);
#endif /* BOARD_STM32_BSP_KEY_HW_KEY_HW_H_ */
