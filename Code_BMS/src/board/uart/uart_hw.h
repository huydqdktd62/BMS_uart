/*
 * uart_hw.h
 *
 *  Created on: Mar 30, 2023
 *      Author: Admin
 */

#ifndef BOARD_UART_UART_HW_H_
#define BOARD_UART_UART_HW_H_

#include "hal_data.h"

typedef struct UART_Type_t UART_Type;

struct UART_Type_t{
    void *p_driver;
};


void uart_init(UART_Type *p_uart);
void uart_write(UART_Type *p_uart,uint8_t *data,uint16_t length);
void uart_read(UART_Type *p_uart,uint8_t *data,uint16_t length);

#endif /* BOARD_UART_UART_HW_H_ */
