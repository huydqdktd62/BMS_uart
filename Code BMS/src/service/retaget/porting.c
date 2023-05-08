/*
 * porting.c
 *
 *  Created on: Apr 2, 2023
 *      Author: MinhNhan
 */


#include "porting.h"


/****Include io platform*******/
#include "stdbool.h"
#include "string.h"
#include "hal_data.h"

/******************************/

extern bool tx_complete;

void serial_init(void *arg){
    (void)arg;
    R_SCI_UART_Open(&g_uart0_ctrl,&g_uart0_cfg);
}
void serial_putc(char c){
    uint32_t time_out = 0xffff;
    while(tx_complete != true || time_out > 0){
        time_out--;
    }
    R_SCI_UART_Write(&g_uart0_ctrl,(uint8_t*)&c, 1);
    tx_complete = false;
}
void serial_puts(char *s){
    uint32_t time_out = 0xffffff;
    while(tx_complete != true || time_out > 0){
        time_out--;
    }
    R_SCI_UART_Write(&g_uart0_ctrl,(uint8_t*)s,(uint32_t)strlen(s));
    tx_complete = false;
}
/* Callback function */
