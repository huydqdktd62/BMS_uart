/*
 * board.c
 *
 *  Created on: Mar 30, 2023
 *      Author: Admin
 */


#include "board.h"
#include "timer_hw.h"
#include "can_hw.h"
#include "uart_hw.h"
#include "gpio_hw.h"

void board_init(){
    gpio_init();
    can_hardware_init();
    timer_init(&bsp_timer0);
    timer_init(&bsp_timer1);
}
