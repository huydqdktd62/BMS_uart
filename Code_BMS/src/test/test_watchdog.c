/*
 * test.c
 *
 *  Created on: Mar 30, 2023
 *      Author: Admin
 */


#include "hal_data.h"
#include "fifo.h"
#include "stdbool.h"
#include "string.h"
#include "stdarg.h"
#include "service/soc_ukf/soc_ukf.h"
#include "service/soc_ukf/battery_model.h"
#include "math.h"
#include "stdio.h"
#include "soh_lsb.h"
#include "app/bms_soc_soh_app/bms_soc_soh_init.h"

volatile bool tx_complete = true;
FIFO_Handle uart_rx_buff;
char *buff_soc[3];
char buff_handle[512];
uint32_t len_handle = 0;
fsp_err_t err;
int32_t test_cycles = 1;
int init_flag = 0;

void uart_write(const uint8_t *buff,uint32_t len);
void uart_print(const char *Format,...);
void process_handle(char* buff,uint32_t len);
int string_split(char* source,char* str,char *dest[]);

void hal_entry(void)
{
    R_IWDT_Open(&g_wdt0_ctrl, &g_wdt0_cfg);
    R_DEBUG->DBGSTOPCR_b.DBGSTOP_IWDT = 0;
    R_IOPORT_Open (g_ioport.p_ctrl, g_ioport.p_cfg);
    err = R_SCI_UART_Open (&g_uart0_ctrl, &g_uart0_cfg);
    memset (buff_handle, 0, 512);
    len_handle = 0;
    fifo_init (&uart_rx_buff, 512, sizeof(char));
    uart_print("INIT SUCCESS!\n");
#if TEST_STR
#else
    for (uint16_t i = 0; i < 3; i++)
    {
        buff_soc[i] = malloc (20);
    }
#endif
    while (1)
    {
        if (fifo_get_num_item (&uart_rx_buff) > 0)
        {
            if (fifo_pop (&uart_rx_buff, &buff_handle[len_handle]) == true)
            {
                len_handle++;
            }
        }
        if (len_handle > 0 && (buff_handle[len_handle - 1] == '\n'))
        {
#if TEST_STR
                uart_write((uint8_t*)buff_handle, len_handle);
#else
                process_handle (buff_handle, len_handle);

#endif
            len_handle = 0;
        }
    }

#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
}
/* Callback function */
void UART_IRQ_Handle(uart_callback_args_t *p_args)
{
    if(p_args->event == UART_EVENT_RX_CHAR){
        char data = (char) p_args->data;
        fifo_push(&uart_rx_buff,(void*)&data);
    }
    if(p_args->event == UART_EVENT_TX_COMPLETE){
        tx_complete = true;
    }

}
void uart_write(const uint8_t *buff,uint32_t len){
    if(tx_complete == true){
        tx_complete = false;
        err = R_SCI_UART_Write(&g_uart0_ctrl, buff, len);
        uint32_t timeout = 0xffff;
        while(timeout){
            timeout--;
            if(tx_complete == true) break;
        }
    }
}


uint8_t uart_tx_buff[128];
int string_split(char* source,char* str,char *dest[]){
    char *token = strtok(source, str);
    int i = 0;
    while( token != NULL ) {
       memset(uart_tx_buff,0,128);
       strcpy(dest[i],token);
       token = strtok(NULL,str);
       i++;
    }
    return i;
}

void process_handle(char *buff, __attribute__((unused))  uint32_t len)
{
    if (init_flag == 0)
    {
        init_flag = 1;
    }
    else
    {
    }
}

char buff_tx[1024];

void uart_print(const char *Format,...){
    va_list args;
    char *buff = buff_tx;
    memset(buff, 0, 1024);
    va_start(args, Format);
    vsprintf(buff, Format, args);
    uart_write((uint8_t*) buff, strlen(buff));
}
