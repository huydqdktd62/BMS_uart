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
#include "stdio.h"

#define TEST_STR 0

typedef struct BMS_Input_t BMS_Input;
struct BMS_Input_t{
    int64_t sigma_point[21];
};

volatile bool tx_complete = true;
FIFO_Handle uart_rx_buff;

void uart_write(const uint8_t *buff,uint32_t len);
void uart_print(const char *Format,...);
void process_handle(char* buff,uint32_t len);
int string_split(char* source,char* str,char *dest[]);

char *buff_soc[21];
char buff_handle[512];
uint32_t len_handle = 0;
fsp_err_t err;
int64_t summation[3];
float result[3];
const int64_t m_weight[7] = {-2999, 500, 500, 500, 500, 500, 500};
const int64_t c_weight[7] = {-30020002, 5000000, 5000000, 5000000, 5000000, 5000000, 5000000};
uint32_t test_circle = 0;




void hal_entry(void)
{
    /* TODO: add your own code here */
    R_IOPORT_Open (g_ioport.p_ctrl, g_ioport.p_cfg);
    err = R_SCI_UART_Open (&g_uart0_ctrl, &g_uart0_cfg);
    uart_print ("Hello guy %d\r\n", 1234);
    memset (buff_handle, 0, 512);
    len_handle = 0;
    fifo_init (&uart_rx_buff, 512, sizeof(char));
#if TEST_STR
#else
    for (uint16_t i = 0; i < 21; i++)
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
    /* TODO: add your own code here */
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

void process_handle(char* buff,uint32_t len){

    BMS_Input input;
    uint16_t i;
    uint16_t k;
    string_split(buff,",",buff_soc);
    for(i = 0; i < 21; i++){
        input.sigma_point[i] = (int64_t)atoi(buff_soc[i]);
    }
    for(i = 0; i < 3; i++){
        summation[i] = 0;
    }
    for(k = 0; k < 7; k++){
        summation[0] += m_weight[k]*input.sigma_point[k];
        summation[1] += m_weight[k]*input.sigma_point[7 + k];
        summation[2] += m_weight[k]*input.sigma_point[14 + k];
    }
    result[0] = (float)summation[0] / 10000000.0f;
    result[1] = (float)summation[1] / 10000000.0f;
    result[2] = (float)summation[2] / 10000000.0f;
    test_circle++;

    uart_print ("%d,", test_circle);
    for (i = 0; i < 21; i++){
        uart_print("%d,", (int32_t)input.sigma_point[i]);
    }
    for (i = 0; i < 3; i++){
        uart_print("%d,", (int32_t)summation[i]);
    }
    for (i = 0; i < 3; i++){
        uart_print("%.6f,", result[i]);
    }
    uart_print ("\n");
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
