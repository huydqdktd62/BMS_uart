/*
 * hc05_hw.h
 *
 *  Created on: Jan 24, 2022
 *      Author: quangnd
 */

#ifndef BOARD_RA_BOARD_HC05_HW_HC05_HW_H_
#define BOARD_RA_BOARD_HC05_HW_HC05_HW_H_

#include "stdint.h"
#include "hal_data.h"

typedef struct HC05_HW_t HC05_HW;
struct HC05_HW_t {
        uart_ctrl_t* const p_uart_ctrl;
        const uart_cfg_t* const p_uart_cfg;
};

extern HC05_HW hc05_port;

#define HC05_COM_IRQn_Priority      3

typedef void (*Uart_Receive_Handle)(const char c);

//void debug_port_set_receive_handle(Uart_Receive_Handle handle);
//void USART_Write_String(char *a);
void hc05_hw_init(void);
void hc05_hw_sends(HC05_HW *p_hw, const uint8_t * const s);
void hc05_send(HC05_HW *p_hw, const char c);

#endif /* BOARD_RA_BOARD_HC05_HW_HC05_HW_H_ */
