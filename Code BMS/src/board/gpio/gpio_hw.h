/*
 * gpio_hw.h
 *
 *  Created on: Mar 30, 2023
 *      Author: Admin
 */

#ifndef BOARD_GPIO_GPIO_HW_H_
#define BOARD_GPIO_GPIO_HW_H_

#include "hal_data.h"

typedef struct GPIO_Type_t GPIO_Type;

struct GPIO_Type_t{
    bsp_io_port_pin_t pin;
};

void gpio_init();
void gpio_pin_write(GPIO_Type *gpio, int level);
int gpio_pin_read(GPIO_Type *gpio);

#endif /* BOARD_GPIO_GPIO_HW_H_ */
