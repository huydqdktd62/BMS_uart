/*
 * hc05_hw.c
 *
 *  Created on: Jan 24, 2022
 *      Author: quangnd
 */

#include "hc05_hw.h"
#include "uart_hw/huart.h"
#include "fifo/fifo.h"

HC05_HW hc05_port={
                .p_uart_cfg=&g_uart0_cfg,
                .p_uart_ctrl=&g_uart0_ctrl
};

FIFO_Handle uart_fifo;

void hc05_hw_init(void) {
        R_SCI_UART_Open(hc05_port.p_uart_ctrl, hc05_port.p_uart_cfg);
	fifo_init(&uart_fifo, 256, sizeof(char));
}

/* Callback function */
void uart0_callback(uart_callback_args_t *p_args)
{
    /* TODO: add your own code here */
    if (p_args->event == UART_EVENT_RX_CHAR)
    {
		char c = (char) p_args->data;
		fifo_push(&uart_fifo, (void*) &c);
    }
    if(p_args->event == UART_EVENT_TX_COMPLETE ){
        huart1.tx_complete = true;
    }
}

void hc05_hw_sends(HC05_HW *p_hw, const uint8_t * const s) {
        R_SCI_UART_Write(p_hw->p_uart_ctrl,s,strlen((char*)s));
}

void hc05_send(HC05_HW *p_hw, const char c) {
	R_SCI_UART_Write(p_hw->p_uart_ctrl, (uint8_t*) &c, 1);
}
