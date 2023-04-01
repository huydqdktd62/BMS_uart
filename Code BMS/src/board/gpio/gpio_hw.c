/*
 * gpio_hw.c
 *
 *  Created on: Mar 30, 2023
 *      Author: Admin
 */


#include "gpio_hw.h"


void gpio_init(){
    R_IOPORT_Open(g_ioport.p_ctrl,g_ioport.p_cfg);
}
void gpio_pin_write(GPIO_Type *gpio, int level){
    R_IOPORT_PinWrite(&g_ioport_ctrl, gpio->pin, level);
}
int gpio_pin_read(GPIO_Type *gpio){
    bsp_io_level_t level = BSP_IO_LEVEL_HIGH;
    R_IOPORT_PinRead(&g_ioport_ctrl, gpio->pin, &level);
    return (int)level;
}
