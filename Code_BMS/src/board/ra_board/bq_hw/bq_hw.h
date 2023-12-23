/*
 * bq_hw.h
 *
 *  Created on: Jan 24, 2022
 *      Author: quangnd
 */

#ifndef BOARD_RA_BOARD_BQ_HW_BQ_HW_H_
#define BOARD_RA_BOARD_BQ_HW_BQ_HW_H_

#include <board_common.h>
#include <stdint.h>

#include "hal_data.h"

//#define AFE_I2C_ADDRESS                 0x10
#define AFE_I2C_ADDRESS                 0x08
#define CRC_MODE                        0
#define SUB_COMMAND_WRITE_REQUEST_REG_ADDR                  0x3E
#define SUB_COMMAND_MEM_START_ADDR                   0x40
#define DATA_RAM_CHECKSUM_REG_ADDR                          0x60
#define DATA_RAM_LENGTH_REG_ADDR                            0x60

#define ONE_BYTE                0x01
#define TWO_BYTE                0x02

#define DATA_RAM_BUFFER_MAX_BYTE                            (34UL)
#define I2C_WRITE_BUFFER_MAX_SIZE                           (32UL)

#define WRITE_ADDR_REG					0x3E
#define READ_ADDR_REG					0x40
#define WRITE_RAM_ADDR_REG_CHECKSUM		0x60

typedef struct BQ_Hw_t BQ_Hw;
typedef void (*BQ_Interrupt_Handle)(BQ_Hw *p_bq);

struct BQ_Hw_t {
	uint8_t i2c_address;
	const i2c_master_cfg_t *p_i2c_master_cfg;
	const i2c_master_ctrl_t *p_i2c_master_ctrl;
	const i2c_master_instance_t *p_i2c_master;
	void (*reset_i2c_bus)(void);
	BQ_Interrupt_Handle handle;
	Hw_Err hw_err;
};
extern struct BQ_Hw_t nfc_hw;
extern struct BQ_Hw_t bq_hw;
int32_t nfc_i2c_hw_init(void);
int32_t bq_hw_init(void);

static inline Hw_Err bq_hw_get_err(BQ_Hw *p_hw) {
	return p_hw->hw_err;
}

static inline void bq_hw_set_err(BQ_Hw *p_hw, Hw_Err const new_result) {
	p_hw->hw_err = new_result;
}

static inline void bq_set_interrupt_handle(BQ_Hw *p_hw,
		BQ_Interrupt_Handle handle) {
	p_hw->handle = handle;
}

void bq_reset_com_bus(BQ_Hw *p_bq);
void bq_reset(BQ_Hw *p_bq);

int32_t bq_i2c_read(BQ_Hw *p_bq, uint8_t reg_addr, uint8_t *buffer,
		uint16_t length);
int32_t bq_i2c_read_reg_byte(BQ_Hw *p_bq, uint8_t reg_addr, uint8_t *buffer);
int32_t bq_i2c_read_reg_word(BQ_Hw *p_bq, uint8_t reg_addr, uint16_t *buffer);
int32_t bq_i2c_read_reg_word_with_sign(BQ_Hw *p_bq, uint8_t reg_addr,
		int16_t *buffer);
int32_t bq_i2c_read_reg_block_with_sign(BQ_Hw *p_bq, uint8_t reg_addr,
		int8_t *buffer, uint8_t length);
int32_t bq_i2c_subcommand_read_block(BQ_Hw *p_bq, uint16_t reg_addr,
                uint8_t *buffer, uint8_t length);
int32_t bq_i2c_write_reg_byte(BQ_Hw *p_bq, uint8_t reg_addr, uint8_t reg_data);
int32_t bq_i2c_write_reg_word(BQ_Hw *p_bq, uint8_t reg_addr, uint16_t reg_data);
int32_t bq_i2c_write_reg_block(BQ_Hw *p_bq, uint8_t reg_addr, uint8_t *buffer,
		uint8_t length);
int32_t bq_i2c_wirte_reg_block_with_crc(BQ_Hw *p_bq, uint8_t reg_addr,
		uint8_t *buffer, uint8_t length);

/* data ram functions */
int32_t bq_i2c_data_ram_read_byte(BQ_Hw *p_bq, uint16_t reg_addr,
		uint8_t *buffer);
int32_t bq_i2c_data_ram_read_word(BQ_Hw *p_bq, uint16_t reg_addr,
		uint16_t *buffer);
int32_t bq_i2c_data_ram_read_word_with_sign(BQ_Hw *p_bq, uint16_t reg_addr,
		int16_t *buffer);
int32_t bq_i2c_data_ram_read_reg_with_float(BQ_Hw *p_bq, uint16_t reg_addr,
		float *buffer);

int32_t bq_i2c_data_ram_write_byte(BQ_Hw *p_bq, uint16_t reg_addr,
		uint8_t reg_data);
int32_t bq_i2c_data_ram_write_word(BQ_Hw *p_bq, uint16_t reg_addr,
		uint16_t reg_data);
int32_t bq_i2c_data_ram_write_word_with_sign(BQ_Hw *p_bq, uint16_t reg_addr,
		int16_t reg_data);

/* Sub command data functions */
int32_t bq_i2c_subcommand_read_word(BQ_Hw *p_bq, uint16_t reg_addr,
		uint16_t *buffer);
int32_t bq_i2c_subcommand_write_word(BQ_Hw *p_bq, uint16_t reg_addr,
		uint16_t reg_data);
int32_t bq_i2c_subcommand_write_byte(BQ_Hw *p_bq, uint16_t reg_addr,
		uint8_t reg_data);
#endif /* BOARD_RA_BOARD_BQ_HW_BQ_HW_H_ */
