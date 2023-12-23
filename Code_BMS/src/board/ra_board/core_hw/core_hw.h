/*
 * core_hw.h
 *
 *  Created on: Dec 11, 2021
 *      Author: VanCong
 */

#ifndef BOARD_RA4M2_BSP_CORE_HW_CORE_HW_H_
#define BOARD_RA4M2_BSP_CORE_HW_CORE_HW_H_

#include "hal_data.h"
#include "int_flash_hw.h"
#define SYSTICK_FREQ_Hz                                     100
#define SERIAL_NUMBER_MEM_ADDR					FLASH_HP_DF_BLOCK_2
#define VEHICLE_SERIAL_NUMBER_MEM_ADDR			FLASH_HP_DF_BLOCK_3
#define SERIAL_NUMBER_MEM_SIZE					13
void core_hw_init(void);
int32_t core_read_id(char *id) ;

#endif /* BOARD_RA4M2_BSP_CORE_HW_CORE_HW_H_ */
