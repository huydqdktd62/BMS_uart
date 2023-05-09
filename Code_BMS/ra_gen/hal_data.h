/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_sci_uart.h"
#include "r_uart_api.h"
#include "r_gpt.h"
#include "r_timer_api.h"
#include "r_can.h"
#include "r_can_api.h"
FSP_HEADER
/** UART on SCI Instance. */
extern const uart_instance_t g_uart0;

/** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_uart_instance_ctrl_t g_uart0_ctrl;
extern const uart_cfg_t g_uart0_cfg;
extern const sci_uart_extended_cfg_t g_uart0_cfg_extend;

#ifndef UART_IRQ_Handle
void UART_IRQ_Handle(uart_callback_args_t *p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t timer2;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t timer2_ctrl;
extern const timer_cfg_t timer2_cfg;

#ifndef Timer2IRQ_Callback
void Timer2IRQ_Callback(timer_callback_args_t *p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t timer1;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t timer1_ctrl;
extern const timer_cfg_t timer1_cfg;

#ifndef Timer1IRQ_Callback
void Timer1IRQ_Callback(timer_callback_args_t *p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t timer0;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t timer0_ctrl;
extern const timer_cfg_t timer0_cfg;

#ifndef Timer0IRQ_Callback
void Timer0IRQ_Callback(timer_callback_args_t *p_args);
#endif
/** CAN on CAN Instance. */
extern const can_instance_t can;
/** Access the CAN instance using these structures when calling API functions directly (::p_api is not used). */
extern can_instance_ctrl_t can_ctrl;
extern const can_cfg_t can_cfg;
extern const can_extended_cfg_t can_cfg_extend;

#ifndef can_callback
void can_callback(can_callback_args_t *p_args);
#endif
#define CAN_NO_OF_MAILBOXES_can (32)
/** UART on SCI Instance. */
extern const uart_instance_t com;

/** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_uart_instance_ctrl_t com_ctrl;
extern const uart_cfg_t com_cfg;
extern const sci_uart_extended_cfg_t com_cfg_extend;

#ifndef NULL
void NULL(uart_callback_args_t *p_args);
#endif
/** UART on SCI Instance. */
extern const uart_instance_t rs485;

/** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_uart_instance_ctrl_t rs485_ctrl;
extern const uart_cfg_t rs485_cfg;
extern const sci_uart_extended_cfg_t rs485_cfg_extend;

#ifndef rs485_callback
void rs485_callback(uart_callback_args_t *p_args);
#endif
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
