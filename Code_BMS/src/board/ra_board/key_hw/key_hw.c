/*
 * key_hw.c
 *
 *  Created on: Oct 20, 2020
 *      Author: quangnd
 */

#include "stdint.h"
#include "key_hw.h"
#include "hal_data.h"

#define KEY_PIN                             BSP_IO_PORT_01_PIN_04

void key_hw_init(void) {

}

KEY_STATE key_read(void) {

        bsp_io_level_t pin_level=BSP_IO_LEVEL_LOW;
        R_IOPORT_PinRead(&g_ioport_ctrl,KEY_PIN,&pin_level);

        return pin_level;

}
