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
#include "normalize.h"
#include "app_config.h"

volatile bool tx_complete = true;
FIFO_Handle uart_rx_buff;

void uart_write(const uint8_t *buff,uint32_t len);
void uart_print(const char *Format,...);
void process_handle(char* buff,uint32_t len);
int string_split(char* source,char* str,char *dest[]);
int32_t create_data_bms(const Parameter data_type);
int32_t load_battery_data_from_buff(const Parameter data_type);
int32_t save_data_bms(const Parameter data_type);

char *buff_soc[SOC_BUFF_SIZE];
char buff_handle[BUFF_HANDLE_SIZE];
uint32_t len_handle = 0;
fsp_err_t err;

SOC_Parameter_Entries logger_entries;
SOC_Parameter_Entries processor_entries;
SOC_UKF logger;
SOC_UKF processor;
Data_Logger input_logger;
Data_Logger output_logger;

//const int64_t m_weight[7] = {-2999, 500, 500, 500, 500, 500, 500};
//const int64_t c_weight[7] = {-30020002, 5000000, 5000000, 5000000, 5000000, 5000000, 5000000};
int32_t test_circle = 0;

void hal_entry(void)
{
    /* TODO: add your own code here */
    R_IOPORT_Open (g_ioport.p_ctrl, g_ioport.p_cfg);
    err = R_SCI_UART_Open (&g_uart0_ctrl, &g_uart0_cfg);
    uart_print ("Hello guy %d\r\n", 1234);
    memset (buff_handle, 0, BUFF_HANDLE_SIZE);
    len_handle = 0;
    fifo_init (&uart_rx_buff, BUFF_HANDLE_SIZE, sizeof(char));
    create_data_bms(NORMALIZED_TYPE);
    load_soc(&processor, 80);
    load_soc(&logger, 80);
    ukf_parameters_init(&logger, &logger_entries);
    ukf_parameters_init(&processor, &processor_entries);
    ukf_init(66000, 0, &logger);
    ukf_init(66000, 0, &processor);
#if TEST_STR
#else
    for (uint16_t i = 0; i < SOC_BUFF_SIZE; i++)
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

void process_handle(char* buff, __attribute__((unused)) uint32_t len){

    string_split(buff,",",buff_soc);
    load_battery_data_from_buff(NORMALIZED_TYPE);
    synchronize_in(&input_logger, &logger);
    normalize(&logger, &processor, NORMALIZED_TYPE);
    synchronize_out(&processor, &output_logger);
    save_data_bms(NORMALIZED_TYPE);
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

int32_t create_data_bms(const Parameter data_type){
    int32_t i;
    uart_print("test_circle,");
    switch(data_type){
    case SIGMA_POINT:
        for (i = 1; i < 4; i++){
            uart_print("state %d,", i);
        }
        for (i = 1; i < 10; i++){
            uart_print("state_covariance %d,", i);
        }
        for (i = 1; i < 22; i++){
            uart_print("normalized sigma_point %d,", i);
        }
        break;
    case SIGMA_STATE:
        for (i = 1; i < 10; i++){
            uart_print("A %d,", i);
        }
        for (i = 1; i < 22; i++){
            uart_print("sigma_point %d,", i);
        }
        for (i = 1; i < 7; i++){
            uart_print("B %d,", i);
        }
        for (i = 1; i < 3; i++){
            uart_print("observer %d,", i);
        }
        for (i = 1; i < 22; i++){
            uart_print("normalized sigma_state %d,", i);
        }
        break;
    case PRIOR_STATE:
        for (i = 1; i < 22; i++){
            uart_print("sigma_state %d,", i);
        }
        for (i = 1; i < 4; i++){
            uart_print("normalized proir_state %d,", i);
        }
        break;
    case SIGMA_STATE_ERROR:
        for (i = 1; i < 22; i++){
            uart_print("sigma_state %d,", i);
        }
        for (i = 1; i < 4; i++){
            uart_print("proir_state %d,", i);
        }
        for (i = 1; i < 22; i++){
            uart_print("normalized sigma_state_error %d,", i);
        }
        break;
    case PRIOR_STATE_COVARIANCE:
        for (i = 1; i < 10; i++){
            uart_print("state_covariance %d,", i);
        }
        for (i = 1; i < 22; i++){
            uart_print("sigma_state_error %d,", i);
        }
        for (i = 1; i < 10; i++){
            uart_print("normalized prior_state_covariance %d,", i);
        }
        break;
    case SYSTEM_PARAMETER:
        for (i = 1; i < 3; i++){
            uart_print("observer %d,", i);
        }
        for (i = 1; i < 8; i++){
            uart_print("est_soc %d,", i);
        }
        for (i = 1; i < 10; i++){
            uart_print("normalized A %d,", i);
        }
        for (i = 1; i < 7; i++){
            uart_print("normalized B %d,", i);
        }
        for (i = 1; i < 4; i++){
            uart_print("normalized C %d,", i);
        }
        for (i = 1; i < 3; i++){
            uart_print("normalized D %d,", i);
        }
        break;
    case SIGMA_MEASUREMENT:
        for (i = 1; i < 4; i++){
            uart_print("C %d,", i);
        }
        for (i = 1; i < 22; i++){
            uart_print("sigma_state %d,", i);
        }
        for (i = 1; i < 3; i++){
            uart_print("D %d,", i);
        }
        for (i = 1; i < 3; i++){
            uart_print("observer %d,", i);
        }
        for (i = 1; i < 8; i++){
            uart_print("normalized sigma_measurement %d,", i);
        }
        break;
    case PREDICT_MEASUREMENT:
        for (i = 1; i < 8; i++){
            uart_print("sigma_measurement %d,", i);
        }
        uart_print("normalized predict_measurement,");
        break;
    case SIGMA_MEASUREMENT_ERROR:
        for (i = 1; i < 8; i++){
            uart_print("sigma_measurement %d,", i);
        }
        uart_print("predict_measurement,");
        for (i = 1; i < 8; i++){
            uart_print("normalized sigma_measurement_error %d,", i);
        }
        break;
    case MEASUREMENT_COVARIANCE:
        for (i = 1; i < 8; i++){
            uart_print("sigma_measurement_error %d,", i);
        }
        uart_print("normalized measurement_covariance,");
        break;
    case CROSS_COVARIANCE:
        for (i = 1; i < 22; i++){
            uart_print("sigma_state_error %d,", i);
        }
        for (i = 1; i < 8; i++){
            uart_print("sigma_measurement_error %d,", i);
        }
        for (i = 1; i < 4; i++){
            uart_print("normalized cross_covariance %d,", i);
        }
        break;
    case KALMAN_GAIN:
        for (i = 1; i < 4; i++){
            uart_print("cross_covariance %d,", i);
        }
        uart_print("measurement_covariance,");
        for (i = 1; i < 4; i++){
            uart_print("normalized kalman_gain %d,", i);
        }
        break;
    case POSTERIOR_STATE:
        for (i = 1; i < 4; i++){
            uart_print("proir_state %d,", i);
        }
        for (i = 1; i < 4; i++){
            uart_print("kalman_gain %d,", i);
        }
        uart_print("U,");
        uart_print("predict_measurement,");
        for (i = 1; i < 4; i++){
            uart_print("normalized posterior_state %d,", i);
        }
        break;
    case POSTERIOR_STATE_COVARIANCE:
        for (i = 1; i < 10; i++){
            uart_print("prior_state_covariance %d,", i);
        }
        for (i = 1; i < 4; i++){
            uart_print("kalman_gain %d,", i);
        }
        uart_print("measurement_covariance,");
        for (i = 1; i < 10; i++){
            uart_print("normalized posterior_state_covariance %d,", i);
        }
        break;
    default:
        uart_print("\n");
        return -1;
    }
    uart_print("\n");
    return 0;
}

int32_t load_battery_data_from_buff(const Parameter data_type){
    int i;
    int32_t index;
    index = 0;
    switch(data_type){
        case SIGMA_POINT:
            for (i = 0; i < UKF_STATE_DIM; i++){
                input_logger.estimate_state[i] = (int32_t)atof(buff_soc[index++]);
            }
            for (i = 0; i < UKF_STATE_DIM*UKF_STATE_DIM; i++){
                input_logger.state_covariance[i] = (int32_t)atof(buff_soc[index++]);
            }
            break;
        case SIGMA_STATE:

            for (i = 0; i < UKF_STATE_DIM*UKF_STATE_DIM; i++){
                input_logger.matrix_A[i] = (int32_t)atof(buff_soc[index++]);
            }
            for (i = 0; i < UKF_STATE_DIM*UKF_SIGMA_FACTOR; i++){
                input_logger.sigma_points[i] = (int32_t)atof(buff_soc[index++]);
            }
            for (i = 0; i < UKF_STATE_DIM*UKF_DYNAMIC_DIM; i++){
                input_logger.matrix_B[i] = (int32_t)atof(buff_soc[index++]);
            }
            for (i = 0; i < UKF_DYNAMIC_DIM; i++){
                input_logger.observed_measurement[i] = (int32_t)atof(buff_soc[index++]);
            }
            break;
        case PRIOR_STATE:
            for (i = 0; i < UKF_STATE_DIM*UKF_SIGMA_FACTOR; i++){
                input_logger.sigma_points[i] = (int32_t)atof(buff_soc[index++]);
            }
            break;
        case SIGMA_STATE_ERROR:
            for (i = 0; i < UKF_STATE_DIM*UKF_SIGMA_FACTOR; i++){
                input_logger.sigma_points[i] = (int32_t)atof(buff_soc[index++]);
            }
            for (i = 0; i < UKF_STATE_DIM; i++){
                input_logger.priori_estimate_state[i] = (int32_t)atof(buff_soc[index++]);
            }
            break;
        case PRIOR_STATE_COVARIANCE:
            for (i = 0; i < UKF_STATE_DIM*UKF_STATE_DIM; i++){
                input_logger.state_covariance[i] = (int32_t)atof(buff_soc[index++]);
            }
            for (i = 0; i < UKF_STATE_DIM*UKF_SIGMA_FACTOR; i++){
                input_logger.sigma_state_error[i] = (int32_t)atof(buff_soc[index++]);
            }
            break;
        case SYSTEM_PARAMETER:
            for (i = 0; i < UKF_DYNAMIC_DIM; i++){
                input_logger.observed_measurement[i] = (int32_t)atof(buff_soc[index++]);
            }
            for (i = 0; i < UKF_STATE_DIM*UKF_SIGMA_FACTOR; i++){
                input_logger.sigma_points[i] = (int32_t)atof(buff_soc[index++]);
            }
            break;
        case SIGMA_MEASUREMENT:
            for (i = 0; i < UKF_STATE_DIM; i++){
                input_logger.matrix_C[i] = (int32_t)atof(buff_soc[index++]);
            }
            for (i = 0; i < UKF_STATE_DIM*UKF_SIGMA_FACTOR; i++){
                input_logger.sigma_points[i] = (int32_t)atof(buff_soc[index++]);
            }
            for (i = 0; i < UKF_DYNAMIC_DIM; i++){
                input_logger.matrix_D[i] = (int32_t)atof(buff_soc[index++]);
            }
            for (i = 0; i < UKF_DYNAMIC_DIM; i++){
                input_logger.observed_measurement[i] = (int32_t)atof(buff_soc[index++]);
            }
            break;
        case PREDICT_MEASUREMENT:
            for (i = 0; i < UKF_SIGMA_FACTOR; i++){
                input_logger.sigma_measurements[i] = (int32_t)atof(buff_soc[index++]);
            }
            break;
        case SIGMA_MEASUREMENT_ERROR:
            for (i = 0; i < UKF_SIGMA_FACTOR; i++){
                input_logger.sigma_measurements[i] = (int32_t)atof(buff_soc[index++]);
            }
            input_logger.est_measurement= (int32_t)atof(buff_soc[index++]);
            break;
        case MEASUREMENT_COVARIANCE:
            for (i = 0; i < UKF_SIGMA_FACTOR; i++){
                input_logger.sigma_measurement_error[i] = (int32_t)atof(buff_soc[index++]);
            }
            break;
        case CROSS_COVARIANCE:
            for (i = 0; i < UKF_STATE_DIM*UKF_SIGMA_FACTOR; i++){
                input_logger.sigma_state_error[i] = (int32_t)atof(buff_soc[index++]);
            }
            for (i = 0; i < UKF_SIGMA_FACTOR; i++){
                input_logger.sigma_measurement_error[i] = (int32_t)atof(buff_soc[index++]);
            }
            break;
        case KALMAN_GAIN:
            for (i = 0; i < UKF_STATE_DIM; i++){
                input_logger.cross_covariance[i] = (int32_t)atof(buff_soc[index++]);
            }
            input_logger.measurement_cov= (int32_t)atof(buff_soc[index++]);
            break;
        case POSTERIOR_STATE:
            for (i = 0; i < UKF_STATE_DIM; i++){
                input_logger.priori_estimate_state[i] = (int32_t)atof(buff_soc[index++]);
            }
            for (i = 0; i < UKF_STATE_DIM; i++){
                input_logger.aukf_kalman_gain[i] = (int32_t)atof(buff_soc[index++]);
            }
            input_logger.pack_voltage = (int32_t)atof(buff_soc[index++]);
            input_logger.est_measurement= (int32_t)atof(buff_soc[index++]);
            break;
        case POSTERIOR_STATE_COVARIANCE:
            for (i = 0; i < UKF_STATE_DIM * UKF_STATE_DIM; i++){
                input_logger.state_covariance[i] = (int32_t)atof(buff_soc[index++]);
            }
            for (i = 0; i < UKF_STATE_DIM; i++){
                input_logger.aukf_kalman_gain[i] = (int32_t)atof(buff_soc[index++]);
            }
            input_logger.measurement_cov= (int32_t)atof(buff_soc[index++]);
            break;
        default:
            return -1;
    }
    return 0;
}

int32_t save_data_bms(const Parameter data_type) {
    int32_t i;
    test_circle++;

    uart_print("%i,", test_circle);

    switch(data_type){
    case SIGMA_POINT:
        for (i = 0; i < 3; i++){
            uart_print("%d,", input_logger.estimate_state[i]);
        }
        for (i = 0; i < 9; i++){
            uart_print("%d,", input_logger.state_covariance[i]);
        }
        for (i = 0; i < 21; i++){
            uart_print("%d,", output_logger.sigma_points[i]);
        }
        break;
    case SIGMA_STATE:
        for (i = 0; i < 9; i++){
            uart_print("%d,", input_logger.matrix_A[i]);
        }
        for (i = 0; i < 21; i++){
            uart_print("%d,", input_logger.sigma_points[i]);
        }
        for (i = 0; i < 6; i++){
            uart_print("%d,", input_logger.matrix_B[i]);
        }
        for (i = 0; i < 2; i++){
            uart_print("%d,", input_logger.observed_measurement[i]);
        }
        for (i = 0; i < 21; i++){
            uart_print("%d,", output_logger.sigma_points[i]);
        }
        break;
    case PRIOR_STATE:
        for (i = 0; i < 21; i++){
            uart_print("%d,", input_logger.sigma_points[i]);
        }
        for (i = 0; i < 3; i++){
            uart_print("%d,", output_logger.priori_estimate_state[i]);
        }
        break;
    case SIGMA_STATE_ERROR:
        for (i = 0; i < 21; i++){
            uart_print("%d,", input_logger.sigma_points[i]);
        }
        for (i = 0; i < 3; i++){
            uart_print("%d,", input_logger.priori_estimate_state[i]);
        }
        for (i = 0; i < 21; i++){
            uart_print("%d,", output_logger.sigma_state_error[i]);
        }
        break;
    case PRIOR_STATE_COVARIANCE:
        for (i = 0; i < 9; i++){
            uart_print("%d,", input_logger.state_covariance[i]);
        }
        for (i = 0; i < 21; i++){
            uart_print("%d,", input_logger.sigma_state_error[i]);
        }
        for (i = 0; i < 9; i++){
            uart_print("%d,", output_logger.state_covariance[i]);
        }
        break;
    case SYSTEM_PARAMETER:
        for (i = 0; i < 2; i++){
            uart_print("%d,", input_logger.observed_measurement[i]);
        }
        for (i = 0; i < 7; i++){
            uart_print("%d,", input_logger.sigma_points[i]);
        }
        for (i = 0; i < 9; i++){
            uart_print("%d,", output_logger.matrix_A[i]);
        }
        for (i = 0; i < 6; i++){
            uart_print("%d,", output_logger.matrix_B[i]);
        }
        for (i = 0; i < 3; i++){
            uart_print("%d,", output_logger.matrix_C[i]);
        }
        for (i = 0; i < 2; i++){
            uart_print("%d,", output_logger.matrix_D[i]);
        }
        break;
    case SIGMA_MEASUREMENT:
        for (i = 0; i < 3; i++){
            uart_print("%d,", input_logger.matrix_C[i]);
        }
        for (i = 0; i < 21; i++){
            uart_print("%d,", input_logger.sigma_points[i]);
        }
        for (i = 0; i < 2; i++){
            uart_print("%d,", input_logger.matrix_D[i]);
        }
        for (i = 0; i < 2; i++){
            uart_print("%d,", input_logger.observed_measurement[i]);
        }
        for (i = 0; i < 7; i++){
            uart_print("%d,", output_logger.sigma_measurements[i]);
        }
        break;
    case PREDICT_MEASUREMENT:
        for (i = 0; i < 7; i++){
            uart_print("%d,", input_logger.sigma_measurements[i]);
        }
        uart_print("%d,", output_logger.est_measurement);
        break;
    case SIGMA_MEASUREMENT_ERROR:
        for (i = 0; i < 7; i++){
            uart_print("%d,", input_logger.sigma_measurements[i]);
        }
        uart_print("%d,", input_logger.est_measurement);
        for (i = 0; i < 7; i++){
            uart_print("%d,", output_logger.sigma_measurement_error[i]);
        }
        break;
    case MEASUREMENT_COVARIANCE:
        for (i = 0; i < 7; i++){
            uart_print("%d,", input_logger.sigma_measurement_error[i]);
        }
        uart_print("%d,", output_logger.measurement_cov);
        break;
    case CROSS_COVARIANCE:
        for (i = 0; i < 21; i++){
            uart_print("%d,", input_logger.sigma_state_error[i]);
        }
        for (i = 0; i < 7; i++){
            uart_print("%d,", input_logger.sigma_measurement_error[i]);
        }
        for (i = 0; i < 3; i++){
            uart_print("%d,", output_logger.cross_covariance[i]);
        }
        break;
    case KALMAN_GAIN:
        for (i = 0; i < 3; i++){
            uart_print("%d,", input_logger.cross_covariance[i]);
        }
        uart_print("%d,", input_logger.measurement_cov);
        for (i = 0; i < 3; i++){
            uart_print("%d,", output_logger.aukf_kalman_gain[i]);
        }
        break;
    case POSTERIOR_STATE:
        for (i = 0; i < 3; i++){
            uart_print("%d,", input_logger.priori_estimate_state[i]);
        }
        for (i = 0; i < 3; i++){
            uart_print("%d,", input_logger.aukf_kalman_gain[i]);
        }
        uart_print("%d,", input_logger.pack_voltage);
        uart_print("%d,", input_logger.est_measurement);
        for (i = 0; i < 3; i++){
            uart_print("%d,", output_logger.estimate_state[i]);
        }
        break;
    case POSTERIOR_STATE_COVARIANCE:
        for (i = 0; i < 9; i++){
            uart_print("%d,", input_logger.state_covariance[i]);
        }
        for (i = 0; i < 3; i++){
            uart_print("%d,", input_logger.aukf_kalman_gain[i]);
        }
        uart_print("%d,", input_logger.measurement_cov);
        for (i = 0; i < 9; i++){
            uart_print("%d,", output_logger.state_covariance[i]);
        }
        break;
    default:
        uart_print("\n");
        return -1;
    }

    uart_print("\n");
    return 0;
}
