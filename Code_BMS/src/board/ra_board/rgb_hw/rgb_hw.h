/*
 * rgb_hw.h
 *
 *  Created on: Oct 25, 2020
 *      Author: nguyenquang
 */

#ifndef BOARD_STM32_BSP_RGB_HW_RGB_HW_H_
#define BOARD_STM32_BSP_RGB_HW_RGB_HW_H_
#include "stdint.h"
#include "stdbool.h"
#include "hal_data.h"

#define LED1                            BSP_IO_PORT_03_PIN_02
#define LED2                           	BSP_IO_PORT_00_PIN_13
#define LED3                            BSP_IO_PORT_00_PIN_14
#define LED4                            BSP_IO_PORT_02_PIN_06

#define LED_CTRL_ON_LEVEL                       BSP_IO_LEVEL_HIGH
#define LED_CTRL_OFF_LEVEL                      BSP_IO_LEVEL_LOW


#define LED1_ON    R_IOPORT_PinWrite(&g_ioport_ctrl,LED1,LED_CTRL_ON_LEVEL)
#define LED2_ON    R_IOPORT_PinWrite(&g_ioport_ctrl,LED2,LED_CTRL_ON_LEVEL)
#define LED3_ON    R_IOPORT_PinWrite(&g_ioport_ctrl,LED3,LED_CTRL_ON_LEVEL)
#define LED4_ON    R_IOPORT_PinWrite(&g_ioport_ctrl,LED4,LED_CTRL_ON_LEVEL)

#define LED1_OFF    R_IOPORT_PinWrite(&g_ioport_ctrl,LED1,LED_CTRL_OFF_LEVEL)
#define LED2_OFF    R_IOPORT_PinWrite(&g_ioport_ctrl,LED2,LED_CTRL_OFF_LEVEL)
#define LED3_OFF    R_IOPORT_PinWrite(&g_ioport_ctrl,LED3,LED_CTRL_OFF_LEVEL)
#define LED4_OFF    R_IOPORT_PinWrite(&g_ioport_ctrl,LED4,LED_CTRL_OFF_LEVEL)

#define LED1_TOGGLE    R_IOPORT_PinWrite(&g_ioport_ctrl, LED1, !R_BSP_PinRead(LED1))
#define LED2_TOGGLE    R_IOPORT_PinWrite(&g_ioport_ctrl, LED2, !R_BSP_PinRead(LED2))
#define LED3_TOGGLE    R_IOPORT_PinWrite(&g_ioport_ctrl, LED3, !R_BSP_PinRead(LED3))
#define LED4_TOGGLE    R_IOPORT_PinWrite(&g_ioport_ctrl, LED4, !R_BSP_PinRead(LED4))

void rgb_hw_init(void);
void rgb_set_color(const uint8_t a, const uint8_t b, const uint8_t c,const uint8_t d,
                const bool blink);
void rgb_blink_enable(void);
void rgb_blink_disable(void);
void rgb_off(void);

#endif /* BOARD_STM32_BSP_RGB_HW_RGB_HW_H_ */
