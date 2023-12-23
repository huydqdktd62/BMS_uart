/*
 * int_flash_hw.h
 *
 *  Created on: Mar 28, 2022
 *      Author: khanh
 */

#ifndef BOARD_INT_FLASH_HW_INT_FLASH_HW_H_
#define BOARD_INT_FLASH_HW_INT_FLASH_HW_H_

#include "bsp_api.h"
#include "common_data.h"
#include "hal_data.h"
#include "CO_utils.h"

/* Code Flash */
#define FLASH_HP_CF_BLOCK_SIZE_32KB       (32*1024)    /* Block Size 32 KB */
#define FLASH_HP_CF_BLOCK_SIZE_8KB        (8*1024)     /* Block Size 8KB */

#define FLASH_HP_CF_BLOCK_0               0x00000000U  /*    8 KB:  0x00000000 - 0x00001FFF */
#define FLASH_HP_CF_BLOCK_1               0x00002000U  /*    8 KB:  0x00002000 - 0x00003FFF */
#define FLASH_HP_CF_BLOCK_2               0x00004000U  /*    8 KB:  0x00004000 - 0x00005FFF */
#define FLASH_HP_CF_BLOCK_3               0x00006000U  /*    8 KB:  0x00006000 - 0x00007FFF */
#define FLASH_HP_CF_BLOCK_4               0x00008000U  /*    8 KB:  0x00008000 - 0x00009FFF */
#define FLASH_HP_CF_BLOCK_5               0x0000A000U  /*    8 KB:  0x0000A000 - 0x0000BFFF */
#define FLASH_HP_CF_BLOCK_6               0x0000C000U  /*    8 KB:  0x0000C000 - 0x0000DFFF */
#define FLASH_HP_CF_BLOCK_7               0x0000E000U  /*    8 KB:  0x0000E000 - 0x0000FFFF */
#define FLASH_HP_CF_BLOCK_8               0x00010000U  /*   32 KB: 0x00010000 - 0x00017FFF */
#define FLASH_HP_CF_BLOCK_9               0x00018000U  /*   32 KB: 0x00018000 - 0x0001FFFF */
#define FLASH_HP_CF_BLOCK_10              0x00020000U  /*   32 KB: 0x00020000 - 0x0004FFFF */
#define FLASH_HP_CF_BLOCK_11              0x00028000U  /*   32 KB: 0x00020000 - 0x0004FFFF */
#define FLASH_HP_CF_BLOCK_12              0x00030000U  /*   32 KB: 0x00020000 - 0x0004FFFF */
#define FLASH_HP_CF_BLOCK_13              0x00038000U  /*   32 KB: 0x00020000 - 0x0004FFFF */
#define FLASH_HP_CF_BLOCK_14              0x00040000U  /*   32 KB: 0x00020000 - 0x0004FFFF */
#define FLASH_HP_CF_BLOCK_15              0x00048000U  /*   32 KB: 0x00020000 - 0x0004FFFF */
#define FLASH_HP_CF_BLOCK_16              0x00050000U  /*   32 KB: 0x00020000 - 0x0004FFFF */
#define FLASH_HP_CF_BLOCK_17              0x00058000U  /*   32 KB: 0x00020000 - 0x0004FFFF */
#define FLASH_HP_CF_BLOCK_18              0x00060000U  /*   32 KB: 0x00020000 - 0x0004FFFF */
#define FLASH_HP_CF_BLOCK_19              0x00068000U  /*   32 KB: 0x00020000 - 0x0004FFFF */
#define FLASH_HP_CF_BLOCK_20              0x00070000U  /*   32 KB: 0x00020000 - 0x0004FFFF */
#define FLASH_HP_CF_BLOCK_21              0x00078000U  /*   32 KB: 0x00020000 - 0x0004FFFF */

/* Code Flash */
#define FLASH_HP_DF_BLOCK_SIZE            (64)

#define FLASH_HP_DF_BLOCK_0               0x08000000U /*   64 B:  0x40100000 - 0x4010003F */
#define FLASH_HP_DF_BLOCK_1               0x08000040U /*   64 B:  0x40100040 - 0x4010007F */
#define FLASH_HP_DF_BLOCK_2               0x08000080U /*   64 B:  0x40100080 - 0x401000BF */
#define FLASH_HP_DF_BLOCK_3               0x080000C0U /*   64 B:  0x401000C0 - 0x401000FF */


void        int_flash_hw_init(void);
int8_t      data_flash_write_long(const uint32_t value, const uint32_t address);
uint32_t    int_flash_read_long(const uint32_t address);
int8_t      flash_hp_code_flash_operations(const uint32_t value, uint32_t address);

int8_t flash_hp_code_flash_erase_block(uint32_t addr, uint8_t block_num);
int8_t flash_hp_code_flash_write_memory(uint32_t addr, uint8_t* write_buff, uint16_t len);
int8_t flash_hp_data_flash_write_memory(uint32_t addr, uint8_t* write_buff, uint8_t len);
fsp_err_t flash_hp_data_flash_operations(	uint8_t  write_buffer[],
		uint32_t const address,
		uint32_t const num_blocks,
		uint32_t     num_bytes);
#endif /* BOARD_INT_FLASH_HW_INT_FLASH_HW_H_ */
