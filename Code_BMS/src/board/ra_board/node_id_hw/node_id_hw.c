/*
 * node_id_hw.c
 *
 *  Created on: Apr 2, 2021
 *      Author: Admin
 */

#include "node_id_hw.h"
#include "hal_data.h"

#define NODE_ID_PIN                             BSP_IO_PORT_02_PIN_07

void node_id_hw_init(void) {

}
NODE_ID_PIN_STATE node_id_pin_read_state(void) {
        bsp_io_level_t pin_level=BSP_IO_LEVEL_LOW;
        R_IOPORT_PinRead(&g_ioport_ctrl,NODE_ID_PIN,&pin_level);

        if(pin_level==BSP_IO_LEVEL_LOW){
                return NODE_ID_PIN_ST_DESELECT;
        }

        return NODE_ID_PIN_ST_SELECT;
}
