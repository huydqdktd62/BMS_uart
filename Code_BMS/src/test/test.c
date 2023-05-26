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

volatile bool tx_complete = true;
FIFO_Handle uart_rx_buff;
char *buff_soc[3];
char buff_handle[512];
uint32_t len_handle = 0;
fsp_err_t err;
int32_t test_cycles = 1;
SOC_Parameter soc_parameter;
SOC_UKF bms_soc;

void uart_write(const uint8_t *buff,uint32_t len);
void uart_print(const char *Format,...);
void process_handle(char* buff,uint32_t len);
int string_split(char* source,char* str,char *dest[]);

void hal_entry(void)
{
    R_IOPORT_Open (g_ioport.p_ctrl, g_ioport.p_cfg);
    err = R_SCI_UART_Open (&g_uart0_ctrl, &g_uart0_cfg);
//    uart_print ("Hello guy %d\r\n", 1234);
    memset (buff_handle, 0, 512);
    len_handle = 0;
    fifo_init (&uart_rx_buff, 512, sizeof(char));
    uart_print("test_circle,SOC,SOC_f,");
    uart_print("terminalVoltage,current,");
    uart_print("H_param,");
    uart_print("a_est_state1,a_est_state2,a_est_state3,");

    uart_print("a_state_cov1,a_state_cov2,a_state_cov3,");
    uart_print("a_state_cov4,a_state_cov5,a_state_cov6,");
    uart_print("a_state_cov7,a_state_cov8,a_state_cov9,");

    uart_print("sig1_1,sig1_2,sig1_3,sig1_4,sig1_5,sig1_6,sig1_7,");
    uart_print("sig2_1,sig2_2,sig2_3,sig2_4,sig2_5,sig2_6,sig2_7,");
    uart_print("sig3_1,sig3_2,sig3_3,sig3_4,sig3_5,sig3_6,sig3_7,");
    uart_print("pri_state1,pri_state2,pri_state3,");
    uart_print("e_sig1_1,e_sig1_2,e_sig1_3,e_sig1_4,e_sig1_5,e_sig1_6,e_sig1_7,");
    uart_print("e_sig2_1,e_sig2_2,e_sig2_3,e_sig2_4,e_sig2_5,e_sig2_6,e_sig2_7,");
    uart_print("e_sig3_1,e_sig3_2,e_sig3_3,e_sig3_4,e_sig3_5,e_sig3_6,e_sig3_7,");
    uart_print("measur_cov,est_measur,");
    uart_print("a_cross1,a_cross2,a_cross3,");
    uart_print("a_kal1,a_kal2,a_kal3,");
    uart_print("A1,A2,A3,A4,A5,A6,A7,A8,A9,");
    uart_print("B1,B2,B3,B4,B5,B6,");
    uart_print("C1,C2,C3,");
    uart_print("D1,D2,");
    uart_print("I,sign(I),");
    uart_print("sig_m1,sig_m2,sig_m3,sig_m4,sig_m5,sig_m6,sig_m7,");
    uart_print("e_sig_m1,e_sig_m2,e_sig_m3,e_sig_m4,e_sig_m5,e_sig_m6,e_sig_m7");

    uart_print("\n");
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

int init_flag = 0;

void process_handle(char* buff, __attribute__((unused)) uint32_t len){
    string_split(buff,",",buff_soc);
    bms_soc.input.pack_voltage = (uint32_t)atoi(buff_soc[0]);
    bms_soc.input.pack_current = (int32_t)atoi(buff_soc[1]);
    if (init_flag == 0){
        load_soc(&bms_soc, 100.0f*get_soc_from_ocv((float)bms_soc.input.pack_voltage/16000.0f));
        ukf_parameters_create(&soc_parameter);
        ukf_init(&bms_soc);
        ukf_update(&bms_soc, 1.0f);
        init_flag = 1;
    }
    else{
        for (int j = 0; j < SOC_PERIOD; j++) {
             ukf_update(&bms_soc, 1.0f);
         }
        uart_print ("%d,", test_cycles++);
        uart_print ("%d,%d,%d,%d,", bms_soc.output.SOC, (int32_t) (bms_soc.output.SOC_f * 1000000.0f),
                    bms_soc.filter.avg_pack_voltage, bms_soc.filter.avg_pack_current);
        uart_print ("%d,", (int32_t) (soc_parameter.H_param * 1000000.0f));
        int i;
        for (i = 0; i < 3; i++)
            uart_print ("%d,", (int32_t) (soc_parameter.estimate_state_entries[i] * 1000000.0f));
        for (i = 0; i < 9; i++)
            uart_print ("%d,", (int32_t) (soc_parameter.state_covariance_entries[i] * 10000000000.0f));
        for (i = 0; i < 21; i++)
            uart_print ("%d,", (int32_t) (soc_parameter.sigma_points_entries[i] * 1000000.0f));
        uart_print ("%d,%d,%d,", (int32_t) (soc_parameter.priori_estimate_state_entries[0] * 1000000.0f),
                    (int32_t) (soc_parameter.priori_estimate_state_entries[1] * 1000000.0f),
                    (int32_t) (soc_parameter.priori_estimate_state_entries[2] * 1000000.0f));
        for (i = 0; i < 21; i++)
            uart_print ("%d,", (int32_t) (soc_parameter.sigma_state_error_entries[i] * 1000000000.0f));
        uart_print ("%d,%d,", (int32_t) (soc_parameter.measurement_cov * 1000000.0f),
                    (int32_t) (soc_parameter.est_measurement * 1000000.0f));
        for (i = 0; i < 3; i++)
            uart_print ("%d,", (int32_t) (soc_parameter.cross_covariance_entries[i] * 1000000000.0f));
        for (i = 0; i < 3; i++)
            uart_print ("%d,", (int32_t) (soc_parameter.aukf_kalman_gain_entries[i] * 1000000000.0f));
        for (i = 0; i < 9; i++)
            uart_print ("%d,", (int32_t) (soc_parameter.matrix_A_entries[i] * 1000000.0f));
        for (i = 0; i < 6; i++)
            uart_print ("%d,", (int32_t) (soc_parameter.matrix_B_entries[i] * 1000000.0f));
        for (i = 0; i < 3; i++)
            uart_print ("%d,", (int32_t) (soc_parameter.matrix_C_entries[i] * 1000000.0f));
        for (i = 0; i < 2; i++)
            uart_print ("%d,", (int32_t) (soc_parameter.matrix_D_entries[i] * 1000000.0f));
        uart_print ("%d,%d,", (int32_t) (soc_parameter.observed_measurement_entries[0] * 1000000.0f),
                    (int32_t) (soc_parameter.observed_measurement_entries[1] * 1000000.0f));
        for (i = 0; i < 7; i++)
            uart_print ("%d,", (int32_t) (soc_parameter.sigma_measurements_entries[i] * 1000000.0f));
        for (i = 0; i < 7; i++)
            uart_print ("%d,", (int32_t) (soc_parameter.sigma_measurement_error_entries[i] * 1000000.0f));
        uart_print ("\n");
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
