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

#define TEST_STR 0



volatile bool tx_complete = true;
FIFO_Handle uart_rx_buff;

void uart_write(const uint8_t *buff,uint32_t len);
void uart_print(const char *Format,...);
void process_handle(char* buff,uint32_t len);
int string_split(char* source,char* str,char *dest[]);

char *buff_soc[3];
char buff_handle[512];
uint32_t len_handle = 0;
fsp_err_t err;



void hal_entry(void)
{
    /* TODO: add your own code here */
    R_IOPORT_Open (g_ioport.p_ctrl, g_ioport.p_cfg);
    err = R_SCI_UART_Open (&g_uart0_ctrl, &g_uart0_cfg);
    uart_print ("Hello guy %d\r\n", 1234);
    memset (buff_handle, 0, 512);
    len_handle = 0;
    fifo_init (&uart_rx_buff, 512, sizeof(char));
    ukf_init(66208, -1);
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

char print_buff[128];
void process_handle(char* buff,uint32_t len){

    SOC_Input input;
    string_split(buff,",",buff_soc);
    input.pack_voltage = (uint32_t)atoi(buff_soc[0]);
    input.pack_current = (int32_t)atoi(buff_soc[1]);

    for(uint16_t i = 0; i< SOC_PERIOD;i++){
        ukf_update(input, 1);
    }
    uart_print("%d,%d,%d,%d,",soc_output.SOC,(int32_t)(soc_output.SOC_f*1000000000.0f),input.pack_voltage,input.pack_current);
    uart_print("%d,%d,",(int32_t)(RC_param*1000000000.0f), (int32_t)(H_param*1000000000.0f));
    int i;
    for (i = 0; i < 3; i++)
        uart_print("%d,", (int32_t)(estimate_state_entries[i]*1000000000.0f));
    for (i = 0; i < 9; i++)
        uart_print("%d,", (int32_t)(state_covariance_entries[i]*1000000000.0f));
    for (i = 0; i < 21; i++)
        uart_print( "%d,", (int32_t)(sigma_points_entries[i]*1000000000.0f));
    uart_print( "%d,%d,%d,", (int32_t)(priori_estimate_state_entries[0]*1000000000.0f),
            (int32_t)(priori_estimate_state_entries[1]*1000000000.0f),
            (int32_t)(priori_estimate_state_entries[2]*1000000000.0f));
    for (i = 0; i < 21; i++)
        uart_print( "%d,", (int32_t)(sigma_state_error_entries[i]*1000000000.0f));
    uart_print( "%d,%d,", (int32_t)(measurement_cov*1000000000.0f),
            (int32_t)(est_measurement*1000000000.0f));
    for (i = 0; i < 3; i++)
        uart_print( "%d,", (int32_t)(cross_covariance_entries[i]*1000000000.0f));
    for (i = 0; i < 3; i++)
        uart_print( "%d,", (int32_t)(aukf_kalman_gain_entries[i]*1000000000.0f));
    for (i = 0; i < 9; i++)
        uart_print( "%d,", (int32_t)(matrix_A_entries[i]*1000000000.0f));
    for (i = 0; i < 6; i++)
        uart_print( "%d,", (int32_t)(matrix_B_entries[i]*1000000000.0f));
    for (i = 0; i < 3; i++)
        uart_print( "%d,", (int32_t)(matrix_C_entries[i]*1000000000.0f));
    for (i = 0; i < 2; i++)
        uart_print( "%d,", (int32_t)(matrix_D_entries[i]*1000000000.0f));
    uart_print( "%d,%d,", (int32_t)(observed_measurement_entries[0]*1000000000.0f),
            (int32_t)(observed_measurement_entries[1]*1000000000.0f));
    for (i = 0; i < 7; i++)
        uart_print( "%d,", (int32_t)(sigma_measurements_entries[i]*1000000000.0f));
    for (i = 0; i < 7; i++)
        uart_print( "%d,", (int32_t)(sigma_measurement_error_entries[i]*1000000000.0f));
    uart_print("\n");

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
