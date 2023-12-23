/*
 * io_hw.h
 *
 *  Created on: Aug 4, 2022
 *      Author: Admin
 */

#ifndef BOARD_RA_BOARD_IO_HW_IO_HW_H_
#define BOARD_RA_BOARD_IO_HW_IO_HW_H_

#include "hal_data.h"

#define INPUT_SW_GET_STATE(x)	R_BSP_PinRead(x)
#define OUTPUT_SW_SET_HIGH(x)	R_IOPORT_PinWrite(&g_ioport_ctrl, x, 1)
#define OUTPUT_SW_SET_LOW(x)	R_IOPORT_PinWrite(&g_ioport_ctrl, x, 0)
#define OUTPUT_SW_TOGGLE(x)		R_IOPORT_PinWrite(&g_ioport_ctrl, x, !R_BSP_PinRead(x))


void io_hw_init(void);
typedef enum {
	SHUTDOWN_PIN = IOPORT_PORT_02_PIN_01
} IO_BMS;
#endif /* BOARD_RA_BOARD_IO_HW_IO_HW_H_ */
