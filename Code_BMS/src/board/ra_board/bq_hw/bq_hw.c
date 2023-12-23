/*
 * bq_hw.c
 *
 *  Created on: Jan 24, 2022
 *      Author: quangnd
 */
#include "bq_hw.h"
#include "common_utils.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"
#include "delay_hal.h"
#include "hal_data.h"
#include "i2c.h"

#define CRC_KEY                                 7
#define LOW_BYTE(Data)                  (uint8_t)(0xff & Data)
#define HIGH_BYTE(Data)                 (uint8_t)(0xff & (Data >> 8))

#define BQ_RST_SHUT_PIN                                 BSP_IO_PORT_02_PIN_01
#define BQ_SDA_PIN                                      BSP_IO_PORT_04_PIN_07
#define BQ_SCL_PIN                                      BSP_IO_PORT_04_PIN_08



static uint8_t check_sum(uint8_t *ptr, uint8_t len);
static int32_t bq_i2c_data_ram_read(BQ_Hw *p_bq, uint16_t reg_addr,
		uint8_t *buffer, uint16_t const len);
static int32_t bq_i2c_subcommand_write(BQ_Hw *p_bq, uint16_t reg_addr,
		uint8_t *const buffer, uint16_t const len);

static int32_t bq_i2c_write_bytes(BQ_Hw *p_bq, uint8_t reg_addr,
		uint8_t *const buffer, const uint16_t len, bool const restarted);

/*
 * Global Variables
 */
struct BQ_Hw_t bq_hw = { .p_i2c_master_cfg = &g_i2c_master0_cfg,
		.p_i2c_master_ctrl = &g_i2c_master0_ctrl,
		.p_i2c_master = &g_i2c_master0, .handle = NULL, .reset_i2c_bus = NULL };
/* Reading I2C call back event through i2c_Master callback */
static volatile i2c_master_event_t i2c_event = I2C_MASTER_EVENT_ABORTED;

/*
 * private function declarations
 */
static fsp_err_t validate_i2c_event(void);

static int32_t bq_sub_command_start_read(BQ_Hw *p_bq, uint16_t const reg_addr);
static void bq_reset_i2c(BQ_Hw *p_bq);
static volatile uint16_t local_time_out = UINT16_MAX;
static void bq_reset_i2c(BQ_Hw *p_bq){
	p_bq->p_i2c_master->p_api->abort(p_bq->p_i2c_master->p_ctrl);
	p_bq->p_i2c_master->p_api->close(p_bq->p_i2c_master->p_ctrl);
	p_bq->p_i2c_master->p_api->open(p_bq->p_i2c_master->p_ctrl,
			p_bq->p_i2c_master->p_cfg);
}

int32_t bq_hw_init(void) {
	R_IOPORT_PinWrite(&g_ioport_ctrl, BQ_RST_SHUT_PIN, BSP_IO_LEVEL_LOW);
	/*
	 bq_hw.p_i2c_master_cfg = &g_i2c_master0_cfg;
	 bq_hw.p_i2c_master_ctrl = &g_i2c_master0_ctrl;
	 bq_hw.p_i2c_master = &g_i2c_master0;*/
	bq_hw.hw_err = HW_NO_ERR;
	Hw_Err err = HW_NO_ERR;

	/* reset bus if any in-progress transfer is occur */

	bq_hw.i2c_address = AFE_I2C_ADDRESS;
	bq_reset(&bq_hw);
	bq_reset_com_bus(&bq_hw);
	/* opening IIC master module */
	//err =(Hw_Err) R_IIC_MASTER_Open(bq_hw.p_i2c_master_ctrl, bq_hw.p_i2c_master_cfg);
	err = (Hw_Err) bq_hw.p_i2c_master->p_api->open(bq_hw.p_i2c_master->p_ctrl,
			bq_hw.p_i2c_master->p_cfg);

	if (HW_NO_ERR != err) {
		bq_hw_set_err(&bq_hw, err);
	}
	return err;
}

void bq_reset_com_bus(BQ_Hw *p_bq) {

	(void) p_bq;
	//switch to GPIO mode, N-channel open-drain with pull-up
	R_IOPORT_PinCfg(&g_ioport_ctrl, BQ_SCL_PIN,
			IOPORT_CFG_NMOS_ENABLE | IOPORT_CFG_PORT_DIRECTION_OUTPUT
			| IOPORT_CFG_PORT_OUTPUT_HIGH | IOPORT_CFG_PULLUP_ENABLE);
	R_IOPORT_PinCfg(&g_ioport_ctrl, BQ_SDA_PIN,
			IOPORT_CFG_NMOS_ENABLE | IOPORT_CFG_PORT_DIRECTION_OUTPUT
			| IOPORT_CFG_PORT_OUTPUT_HIGH | IOPORT_CFG_PULLUP_ENABLE);

	/* pull sda, scl pins to low in 20ms to reset bq i2c interface logic */
	R_IOPORT_PinWrite(&g_ioport_ctrl, BQ_SCL_PIN, BSP_IO_LEVEL_LOW);
	R_IOPORT_PinWrite(&g_ioport_ctrl, BQ_SDA_PIN, BSP_IO_LEVEL_LOW);
	sw_delay_ms(10);
	/* switch pins back to i2c peripheral mode */
	R_IOPORT_PinCfg(&g_ioport_ctrl, BQ_SCL_PIN,
			(uint32_t) IOPORT_CFG_PERIPHERAL_PIN
			| (uint32_t) IOPORT_PERIPHERAL_IIC);
	R_IOPORT_PinCfg(&g_ioport_ctrl, BQ_SDA_PIN,
			(uint32_t) IOPORT_CFG_PERIPHERAL_PIN
			| (uint32_t) IOPORT_PERIPHERAL_IIC);
}

void bq_reset(BQ_Hw *p_bq) {

	(void) p_bq;
	/* held rst_shut pin high in 500ms */
	R_IOPORT_PinWrite(&g_ioport_ctrl, BQ_RST_SHUT_PIN, BSP_IO_LEVEL_HIGH);
	sw_delay_ms(500);
	R_IOPORT_PinWrite(&g_ioport_ctrl, BQ_RST_SHUT_PIN, BSP_IO_LEVEL_LOW);
	bq_hw_set_err(p_bq, HW_NO_ERR);
	sw_delay_ms(250);
}

/*******************************************************************************************************************//**
 *  @brief      User callback function
 *  @param[in]  p_args
 *  @retval None
 **********************************************************************************************************************/
void bq_i2c_callback(i2c_master_callback_args_t *p_args) {

	if (p_args->event == I2C_MASTER_EVENT_TX_COMPLETE) {
		i2c_event = I2C_MASTER_EVENT_TX_COMPLETE;
	} else if (p_args->event == I2C_MASTER_EVENT_RX_COMPLETE) {
		i2c_event = I2C_MASTER_EVENT_RX_COMPLETE;
	} else {
		i2c_event = I2C_MASTER_EVENT_ABORTED;
	}
	/*	if (NULL != p_args) {
	 capture callback event for validating the i2c transfer event
		i2c_event = p_args->event;
	 }*/
	hal_i2c_interrup(&hal_i2c,p_args->event);
}

int32_t bq_i2c_read_reg_byte(BQ_Hw *p_bq, uint8_t reg_addr, uint8_t *buffer) {

	return bq_i2c_read(p_bq, reg_addr, buffer, ONE_BYTE);
}

int32_t bq_i2c_read_reg_word(BQ_Hw *p_bq, uint8_t reg_addr, uint16_t *buffer) {

	uint8_t data_buffer[TWO_BYTE] = { RESET_VALUE };
	fsp_err_t err = FSP_SUCCESS;
	err = bq_i2c_read(p_bq, reg_addr, data_buffer, TWO_BYTE);
	if (FSP_SUCCESS != err) {
		bq_reset_i2c(p_bq);
		return err;
	}

	*buffer = data_buffer[1];
	*buffer <<= 8;
	*buffer += data_buffer[0];

	return (err);
}
;
int32_t bq_i2c_read_reg_word_with_sign(BQ_Hw *p_bq, uint8_t reg_addr,
		int16_t *buffer) {

	return bq_i2c_read_reg_word(p_bq, reg_addr, (uint16_t*) buffer);
}

int32_t bq_i2c_read(BQ_Hw *p_bq, uint8_t reg_addr, uint8_t *buffer,
		uint16_t length) {
	(void) p_bq;
	fsp_err_t err = FSP_SUCCESS;

	/* Parameter checking */
	if (NULL == buffer) {
		err = FSP_ERR_INVALID_POINTER;
		return err;
	}

	/* Send register address to sensor */
	//err = R_IIC_MASTER_Write(p_bq->p_i2c_master_ctrl, &reg_addr, ONE_BYTE,true);
	err = bq_hw.p_i2c_master->p_api->write(bq_hw.p_i2c_master->p_ctrl,
			&reg_addr, ONE_BYTE, true);

	err = validate_i2c_event();
	/* handle error */
	if (FSP_ERR_TRANSFER_ABORTED == err) {
		bq_reset_i2c(p_bq);
	} else {
		/* Read only when I2C write and its callback event is successful */
		//err = R_IIC_MASTER_Read(p_bq->p_i2c_master_ctrl,buffer,length, false);
		err = bq_hw.p_i2c_master->p_api->read(bq_hw.p_i2c_master->p_ctrl,
				buffer, length, false);

		err = validate_i2c_event();
		/* handle error */
		if (FSP_ERR_TRANSFER_ABORTED == err) {
			bq_reset_i2c(p_bq);
		}
	}
	return err;
}


int32_t bq_i2c_read_reg_block_with_sign(BQ_Hw *p_bq, uint8_t reg_addr,
		int8_t *buffer, uint8_t length) {

	return bq_i2c_read(p_bq, reg_addr, (uint8_t*) buffer, length);

}
int32_t bq_i2c_subcommand_read_block(BQ_Hw* p_bq, uint16_t reg_addr, uint8_t* buffer, uint8_t length){
	(void)length;
	bq_i2c_write_reg_word(p_bq , WRITE_ADDR_REG , reg_addr);

	R_BSP_SoftwareDelay(2,BSP_DELAY_UNITS_MILLISECONDS);

	//bq_i2c_read_reg_block_with_sign(p_bq, READ_ADDR_REG, buffer, length);

	bq_i2c_read(p_bq, READ_ADDR_REG,  buffer, length);
	//bq_i2c_read_reg_byte(p_bq, READ_ADDR_REG, buffer);

	return 0;
}

static int32_t bq_i2c_write_bytes(BQ_Hw *p_bq, uint8_t reg_addr,
		uint8_t *const buffer, const uint16_t len, bool const restarted) {
	(void) p_bq;
	fsp_err_t err = FSP_SUCCESS;

	uint8_t data_buffer[I2C_WRITE_BUFFER_MAX_SIZE] = { RESET_VALUE };

	data_buffer[0] = reg_addr;
	for (int i = 0; i < len; i++) {
		data_buffer[i + 1] = buffer[i];
	}

	//err = R_IIC_MASTER_Write(p_bq->p_i2c_master_ctrl, data_buffer,len+1,restarted);
	err = bq_hw.p_i2c_master->p_api->write(bq_hw.p_i2c_master->p_ctrl,
			data_buffer, len + 1, restarted);

	err = validate_i2c_event();
	/* handle error */
	if (FSP_ERR_TRANSFER_ABORTED == err) {
		bq_reset_i2c(p_bq);
		APP_ERR_PRINT("** POWER_CTL reg I2C write failed!!! ** \r\n");
	}
	return err;
}

int32_t bq_i2c_write_reg_byte(BQ_Hw *p_bq, uint8_t reg_addr, uint8_t reg_data) {
	(void) p_bq;
	fsp_err_t err = FSP_SUCCESS;

	uint8_t data_buffer[2] = { RESET_VALUE };
	data_buffer[0] = reg_addr;
	data_buffer[1] = reg_data;

	//err = R_IIC_MASTER_Write(p_bq->p_i2c_master_ctrl, data_buffer,2,false);
	err = bq_hw.p_i2c_master->p_api->write(bq_hw.p_i2c_master->p_ctrl,
			data_buffer, 2, false);
	err = validate_i2c_event();
	/* handle error */
	if (FSP_ERR_TRANSFER_ABORTED == err) {
		bq_reset_i2c(p_bq);
		APP_ERR_PRINT("** POWER_CTL reg I2C write failed!!! ** \r\n");
	}

	return err;
}
;

int32_t bq_i2c_write_reg_word(BQ_Hw *p_bq, uint8_t reg_addr, uint16_t reg_data) {
	(void) p_bq;
	fsp_err_t err = FSP_SUCCESS;

	uint8_t data_buffer[3] = { RESET_VALUE };
	data_buffer[0] = reg_addr;
	data_buffer[1] = LOW_BYTE(reg_data);
	data_buffer[2] = HIGH_BYTE(reg_data);
	uint32_t timeout = 100;
	//	while (i2c_event != I2C_MASTER_EVENT_TX_COMPLETE){
	//		timeout--;
	//		err = bq_hw.p_i2c_master->p_api->write(bq_hw.p_i2c_master->p_ctrl,
	//				data_buffer, 3, false);
	//		if(timeout == 0){
	//			bq_reset_i2c(p_bq);
	//			timeout = 100;
	//		}
	//	}
	err = R_IIC_MASTER_Write(p_bq->p_i2c_master_ctrl, data_buffer,3,false);

	err = validate_i2c_event();
	/* handle error */
	if (FSP_ERR_TRANSFER_ABORTED == err) {
		bq_reset_i2c(p_bq);
	}

	return err;

}
;

static int32_t bq_i2c_data_ram_read(BQ_Hw *p_bq, uint16_t reg_addr,
		uint8_t *buffer, uint16_t length) {

	fsp_err_t err = FSP_SUCCESS;

	err = bq_sub_command_start_read(p_bq, reg_addr);
	if (FSP_SUCCESS != err) {
		return err;
	}

	err = bq_i2c_read(p_bq, SUB_COMMAND_MEM_START_ADDR, buffer, length);
	return err;
}

int32_t bq_i2c_data_ram_read_byte(BQ_Hw *p_bq, uint16_t reg_addr,
		uint8_t *buffer) {

	return bq_i2c_data_ram_read(p_bq, reg_addr, buffer, 1);
}
;
int32_t bq_i2c_data_ram_read_word(BQ_Hw *p_bq, uint16_t reg_addr,
		uint16_t *buffer) {

	return bq_i2c_data_ram_read(p_bq, reg_addr, (uint8_t*) buffer, TWO_BYTE);

}
;

int32_t bq_i2c_data_ram_read_word_with_sign(BQ_Hw *p_bq, uint16_t reg_addr,
		int16_t *buffer) {

	return bq_i2c_data_ram_read_word(p_bq, reg_addr, (uint16_t*) buffer);
}
;
int32_t bq_i2c_data_ram_read_reg_with_float(BQ_Hw *p_bq, uint16_t reg_addr,
		float *buffer) {

	fsp_err_t err = FSP_SUCCESS;
	uint8_t data[4];

	err = bq_i2c_write_reg_word(p_bq, SUB_COMMAND_WRITE_REQUEST_REG_ADDR,
			reg_addr);
	if (FSP_SUCCESS != err) {
		return err;
	}

	hw_delay_ms(2);

	err = bq_i2c_read(&bq_hw, SUB_COMMAND_MEM_START_ADDR, data, 4);
	if (FSP_SUCCESS != err) {
		return err;
	}
	*buffer = (float) ((data[3] << 24));
	*buffer += (float) ((data[2] << 16));
	*buffer += (float) ((data[1] << 8));
	*buffer += (float) (data[0]);

	return err;
}
;

int32_t bq_i2c_data_ram_write_byte(BQ_Hw *p_bq, uint16_t reg_addr,
		uint8_t reg_data) {

	fsp_err_t err = FSP_SUCCESS;
	uint8_t verify_data = RESET_VALUE;
	uint8_t checksum = RESET_VALUE;
	uint8_t checksum_buff[4] = { RESET_VALUE };
	uint8_t data_ram_buff[2] = { RESET_VALUE };
	uint16_t valid_check_code = RESET_VALUE;

	checksum_buff[0] = SUB_COMMAND_WRITE_REQUEST_REG_ADDR;
	checksum_buff[1] = LOW_BYTE(reg_addr);
	checksum_buff[2] = HIGH_BYTE(reg_addr);
	checksum_buff[3] = reg_data;

	data_ram_buff[0] = SUB_COMMAND_MEM_START_ADDR;
	data_ram_buff[1] = reg_data;

	err = bq_sub_command_start_read(p_bq, reg_addr);
	if (FSP_SUCCESS != err) {
		return err;
	}

	//err = R_IIC_MASTER_Write(p_bq->p_i2c_master_ctrl,data_ram_buff,2,false);
	err = bq_hw.p_i2c_master->p_api->write(bq_hw.p_i2c_master->p_ctrl,
			data_ram_buff, 2, false);

	err = validate_i2c_event();
	/* handle error */
	if (FSP_ERR_TRANSFER_ABORTED == err) {
		bq_reset_i2c(p_bq);
		APP_ERR_PRINT("** POWER_CTL reg I2C write failed!!! ** \r\n");
		return err;
	}

	checksum = check_sum(checksum_buff + 1, 3);

	valid_check_code = (5U << 8) + checksum;

	err = bq_i2c_write_reg_word(p_bq, DATA_RAM_CHECKSUM_REG_ADDR,
			valid_check_code);

	if (FSP_SUCCESS != err) {
		return err;
	}

	err = bq_i2c_data_ram_read_byte(p_bq, reg_addr, &verify_data);

	if (FSP_SUCCESS != err) {
		return err;
	}

	if (verify_data != reg_data) {
		return FSP_ERR_INVALID_RESULT;
	}

	return FSP_SUCCESS;
}
;

int32_t bq_i2c_data_ram_write_word(BQ_Hw *p_bq, uint16_t reg_addr,
		uint16_t reg_data) {

	fsp_err_t err = FSP_SUCCESS;
	uint16_t verify_data = RESET_VALUE;
	uint8_t checksum = RESET_VALUE;
	uint8_t checksum_buff[5] = { RESET_VALUE };
	uint8_t data_ram_buff[3] = { RESET_VALUE };
	uint16_t valid_check_code = RESET_VALUE;

	checksum_buff[0] = SUB_COMMAND_WRITE_REQUEST_REG_ADDR;
	checksum_buff[1] = LOW_BYTE(reg_addr);
	checksum_buff[2] = HIGH_BYTE(reg_addr);
	checksum_buff[3] = LOW_BYTE(reg_data);
	checksum_buff[4] = HIGH_BYTE(reg_data);

	data_ram_buff[0] = SUB_COMMAND_MEM_START_ADDR;
	data_ram_buff[1] = checksum_buff[3];
	data_ram_buff[2] = checksum_buff[4];

	err = bq_sub_command_start_read(p_bq, reg_addr);
	if (FSP_SUCCESS != err) {
		return err;
	}

	//err = R_IIC_MASTER_Write(p_bq->p_i2c_master_ctrl,data_ram_buff, 3,
	//              false);
	err = bq_hw.p_i2c_master->p_api->write(bq_hw.p_i2c_master->p_ctrl,
			data_ram_buff, 3, false);

	err = validate_i2c_event();
	/* handle error */
	if (FSP_ERR_TRANSFER_ABORTED == err) {
		bq_reset_i2c(p_bq);
		return err;
	}

	checksum = check_sum(checksum_buff + 1, 4);

	valid_check_code = (6U << 8) + checksum;

	err = bq_i2c_write_reg_word(p_bq, DATA_RAM_CHECKSUM_REG_ADDR,
			valid_check_code);

	if (FSP_SUCCESS != err) {
		return err;
	}

	err = bq_i2c_data_ram_read_word(p_bq, reg_addr, &verify_data);

	if (FSP_SUCCESS != err) {
		return err;
	}

	if (verify_data != reg_data) {
		return FSP_ERR_INVALID_RESULT;
	}

	return FSP_SUCCESS;
}
;

int32_t bq_i2c_data_ram_write_word_with_sign(BQ_Hw *p_bq, uint16_t reg_addr,
		int16_t reg_data) {

	return bq_i2c_data_ram_write_word(p_bq, reg_addr, (uint16_t) reg_data);
}
;

int32_t bq_i2c_subcommand_read_word(BQ_Hw *p_bq, uint16_t reg_addr,
		uint16_t *buffer) {

	fsp_err_t err = FSP_SUCCESS;
	if (FSP_SUCCESS != err) {
		return err;
	}

	err = bq_sub_command_start_read(p_bq, reg_addr);

	if (FSP_SUCCESS != err) {
		return err;
	}

	err = FSP_SUCCESS;
	err = bq_i2c_read_reg_word(p_bq, SUB_COMMAND_MEM_START_ADDR, buffer);
	if (FSP_SUCCESS != err) {
		return err;
	}
	return err;
}
;

int32_t bq_i2c_subcommand_write_word(BQ_Hw *p_bq, uint16_t reg_addr,
		uint16_t reg_data) {

	unsigned char data_buffer[TWO_BYTE] = { RESET_VALUE };

	data_buffer[0] = LOW_BYTE(reg_data);
	data_buffer[1] = HIGH_BYTE(reg_data);

	return (bq_i2c_subcommand_write(p_bq, reg_addr, data_buffer, TWO_BYTE));
}
int32_t bq_i2c_subcommand_write_byte(BQ_Hw *p_bq, uint16_t reg_addr,
		uint8_t reg_data){
	return bq_i2c_subcommand_write(p_bq, reg_addr,(uint8_t*)& reg_data,1);
}
static int32_t bq_i2c_subcommand_write(BQ_Hw *p_bq, uint16_t reg_addr,
		uint8_t *const buffer, uint16_t const len) {

	fsp_err_t err = FSP_SUCCESS;

	uint8_t data_buffer[DATA_RAM_BUFFER_MAX_BYTE] = { RESET_VALUE };
	uint8_t checksum = RESET_VALUE;

	data_buffer[0] = LOW_BYTE(reg_addr);
	data_buffer[1] = HIGH_BYTE(reg_addr);

	for (int i = 0; i < len; i++) {
		data_buffer[i + 2] = buffer[i];
	}

	err = bq_i2c_write_bytes(p_bq, SUB_COMMAND_WRITE_REQUEST_REG_ADDR,
			data_buffer, len + 2, false);

	if (FSP_SUCCESS != err) {
		return err;
	}

	checksum = check_sum(data_buffer, (uint8_t) (len + 2));

	data_buffer[0] = checksum;
	data_buffer[1] = (uint8_t) (len + 4);

	err = bq_i2c_write_bytes(p_bq, DATA_RAM_CHECKSUM_REG_ADDR, data_buffer, 2,
			false);

	return FSP_SUCCESS;
}

static int32_t bq_sub_command_start_read(BQ_Hw *p_bq, uint16_t const reg_addr) {

	fsp_err_t err = FSP_SUCCESS;
	uint16_t read_back_data = RESET_VALUE;
	uint16_t timeout = UINT16_MAX;

	err = bq_i2c_write_reg_word(p_bq, SUB_COMMAND_WRITE_REQUEST_REG_ADDR,
			reg_addr);
	if (FSP_SUCCESS != err) {
		return err;
	}

	while (timeout--) {
		err = bq_i2c_read_reg_word(p_bq, SUB_COMMAND_WRITE_REQUEST_REG_ADDR,
				&read_back_data);
#if 0
		if(FSP_SUCCESS != err){
			return err;
		}
#endif
		if (read_back_data == reg_addr) {
			return FSP_SUCCESS;
		}
	}

	return FSP_ERR_TIMEOUT;
}


/*******************************************************************************************************************//**
 *  @brief       Validate i2c receive/transmit  based on required write read operation
 *
 *  @param[in]   None
 *
 *  @retval      FSP_SUCCESS                       successful event receiving returns FSP_SUCCESS
 *               FSP_ERR_TRANSFER_ABORTED          Either on timeout elapsed or received callback event is
 *                                                 I2C_MASTER_EVENT_ABORTED
 **********************************************************************************************************************/


static fsp_err_t validate_i2c_event(void) {
	local_time_out = 0xffff;

	/* resetting call back event capture variable */
	i2c_event = (i2c_master_event_t) RESET_VALUE;

	do {
		/* This is to avoid infinite loop */
		local_time_out--;
		if (RESET_VALUE == local_time_out) {
			return FSP_ERR_TRANSFER_ABORTED;
		}
//		sw_delay_us(1);
	} while (i2c_event == RESET_VALUE);

	if (i2c_event != I2C_MASTER_EVENT_ABORTED) {
		i2c_event = (i2c_master_event_t) RESET_VALUE; // Make sure this is always Reset before return
		return FSP_SUCCESS;
	}
	i2c_event = (i2c_master_event_t) RESET_VALUE; // Make sure this is always Reset before return
	return FSP_ERR_TRANSFER_ABORTED;
}

static uint8_t check_sum(uint8_t *ptr, uint8_t len) {

	uint8_t i;
	uint8_t checksum = 0;

	for (i = 0; i < len; i++) {
		checksum += (uint8_t) (ptr[i]);
	}

	checksum = (uint8_t) (0xff & (~checksum));

	return (checksum);
}
int32_t bq_i2c_write_reg_block(BQ_Hw *p_bq, uint8_t reg_addr, uint8_t *buffer,
		uint8_t length){
	fsp_err_t err = FSP_SUCCESS;
	err = bq_i2c_write_bytes(p_bq, reg_addr, buffer, length, false);
	return err;
}
