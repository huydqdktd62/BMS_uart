/*
 * int_flash_hw.c
 *
 *  Created on: Mar 28, 2022
 *      Author: khanh
 */

#include "int_flash_hw.h"
#include "delay.h"
/* Flags, set from Callback function */
static volatile _Bool g_b_flash_event_not_blank = false;
static volatile _Bool g_b_flash_event_blank = false;
static volatile _Bool g_b_flash_event_erase_complete = false;
static volatile _Bool g_b_flash_event_write_complete = false;

static fsp_err_t blankcheck_event_flag(void);


void int_flash_hw_init(void){
	R_FLASH_HP_Open(&g_flash0_ctrl, &g_flash0_cfg);
	R_FLASH_HP_Close(&g_flash0_ctrl);
	R_FLASH_HP_Open(&g_flash0_ctrl, &g_flash0_cfg);

	R_FLASH_HP_StartUpAreaSelect(&g_flash0_ctrl, FLASH_STARTUP_AREA_BLOCK0, true);
}

int8_t flash_hp_code_flash_erase_block(uint32_t addr, uint8_t block_num){

	flash_result_t blank_check_result = FLASH_RESULT_BLANK;

	/* Erase Block */
	if(R_FLASH_HP_Erase(&g_flash0_ctrl, addr, block_num) != FSP_SUCCESS){
		return (-1);
	}

	for(uint8_t i = 0; i < block_num; i++){
		/* Blank Check */
		if( R_FLASH_HP_BlankCheck(&g_flash0_ctrl, addr + FLASH_HP_CF_BLOCK_SIZE_32KB*i, FLASH_HP_CF_BLOCK_SIZE_32KB, &blank_check_result) != FSP_SUCCESS){
			return (-1);
		}
		/* Validate the blank check result */
		if (blank_check_result == FLASH_RESULT_NOT_BLANK){
			return (-1);
		}
	}
	return 0;
}

int8_t flash_hp_code_flash_write_memory(uint32_t addr, uint8_t* write_buff, uint16_t len){

	/* Write code flash data*/
	if( R_FLASH_HP_Write(&g_flash0_ctrl, (uint32_t) write_buff, addr, len) != FSP_SUCCESS){
		return (-1);
	}

	/*Read code flash data */
	uint8_t read_buff[len];
	memcpy(read_buff,(uint8_t *)addr, len);
	if( strcmp((char*)read_buff, (char*)write_buff) != 0 ){
		return (-1);
	}
	return 0;
}

int8_t flash_hp_data_flash_write_memory(uint32_t addr, uint8_t* write_buff, uint8_t len){

	flash_result_t blank_check_result = FLASH_RESULT_BLANK;
	uint8_t block_num = 1;                                  // Default: 1 block

	/* Erase Block */
	if(R_FLASH_HP_Erase(&g_flash0_ctrl, addr, block_num) != FSP_SUCCESS){
		return (-1);
	}
	/* Wait for the erase complete event flag, if BGO is SET  */
	if (g_flash0_cfg.data_flash_bgo == true){
		while (!g_b_flash_event_erase_complete);
		g_b_flash_event_erase_complete = false;
	}

	/* Data flash blank check */
	if(R_FLASH_HP_BlankCheck(&g_flash0_ctrl, addr, FLASH_HP_DF_BLOCK_SIZE, &blank_check_result) != FSP_SUCCESS){
		return (-1);
	}
	/* Validate the blank check result */
	if(blank_check_result == FLASH_RESULT_NOT_BLANK){
		return (-1);
	}
	else if(blank_check_result == FLASH_RESULT_BGO_ACTIVE){
		if(blankcheck_event_flag() != FSP_SUCCESS){
			return (-1);
		}
	}

	/* Write code flash data*/
	if(R_FLASH_HP_Write(&g_flash0_ctrl, (uint32_t)write_buff, addr, len) != FSP_SUCCESS){
		return (-1);
	}
	/* Wait for the write complete event flag, if BGO is SET  */
	if (g_flash0_cfg.data_flash_bgo == true)    {
		while (!g_b_flash_event_write_complete);
		g_b_flash_event_write_complete = false;
	}

	//    /*Read code flash data */
	//    uint8_t read_buff[len];
	//    memcpy(read_buff,write_buff, len);
	//    if( strcmp((char*)read_buff, (char*)write_buff) != 0 ){
	//        return (-1);
	//    }
	return 0;
}

uint32_t    int_flash_read_long(const uint32_t address){

	uint8_t  block_size = 4;
	uint8_t  read_buffer[block_size];
	memcpy(read_buffer, (uint8_t *) address, block_size);
	uint32_t value = CO_getUint32(read_buffer);
	return value;
}



static fsp_err_t blankcheck_event_flag(void){

	fsp_err_t err = FSP_SUCCESS;
	/* Wait for callback function to set flag */
	while (!(g_b_flash_event_not_blank || g_b_flash_event_blank));

	if (g_b_flash_event_not_blank){
		/* Reset Flag */
		g_b_flash_event_not_blank = false;
		return (fsp_err_t)FLASH_EVENT_NOT_BLANK;
	}
	else{
		/* Reset Flag */
		g_b_flash_event_blank = false;
	}
	return err;
}


/* Callback function */
void bgo_callback(flash_callback_args_t *p_args)
{
	/* TODO: add your own code here */

	switch(p_args->event){

	case FLASH_EVENT_NOT_BLANK:
		g_b_flash_event_not_blank = true;
		break;

	case FLASH_EVENT_BLANK:
		g_b_flash_event_blank = true;
		break;

	case FLASH_EVENT_ERASE_COMPLETE:
		g_b_flash_event_erase_complete = true;
		break;

	case FLASH_EVENT_WRITE_COMPLETE:
		g_b_flash_event_write_complete = true;
		break;

	default:
		break;
	}
}
fsp_err_t flash_hp_data_flash_operations(	uint8_t  write_buffer[],
		uint32_t const address,
		uint32_t const num_blocks,
		uint32_t     num_bytes){
	fsp_err_t err = FSP_SUCCESS;
	flash_result_t blank_check_result = FLASH_RESULT_BLANK;
	/* Erase Block */
	err = R_FLASH_HP_Erase(&g_flash0_ctrl, address, 1);
	/* Error Handle */

	if (FSP_SUCCESS != err)
	{
		return err;
	}
	/* Wait for the erase complete event flag, if BGO is SET  */
	if (true == g_flash0_cfg.data_flash_bgo)
	{
		while (!g_b_flash_event_erase_complete);
		g_b_flash_event_erase_complete = false;
	}
	/* Blank Check */
	err = R_FLASH_HP_BlankCheck(&g_flash0_ctrl, address, FLASH_HP_DF_BLOCK_SIZE, &blank_check_result);
	/* Error Handle */
	if (FSP_SUCCESS != err)
	{
		return err;
	}
	/* Validate the blank check result */
	if (FLASH_RESULT_BLANK == blank_check_result)
	{
	}
	else if (FLASH_RESULT_NOT_BLANK == blank_check_result)
	{
		return (fsp_err_t)FLASH_RESULT_NOT_BLANK;
	}
	else if (FLASH_RESULT_BGO_ACTIVE == blank_check_result)
	{
		/* BlankCheck will update in Callback */
		/* Event flag will be updated in the blank check function when BGO is enabled */
		err = blankcheck_event_flag();
		if(FSP_SUCCESS != err)
		{
			return err;
		}
	}
	else
	{
		/* No Operation */
	}
	/* Write code flash data*/
	err = R_FLASH_HP_Write(&g_flash0_ctrl, (uint32_t) write_buffer,address, num_bytes);
	/* Wait for the write complete event flag, if BGO is SET  */
	if (true == g_flash0_cfg.data_flash_bgo)
	{
		while (!g_b_flash_event_write_complete);
		g_b_flash_event_write_complete = false;
	}
	//sw_delay_ms(1000);
	/* Error Handle */
	if (FSP_SUCCESS != err)
	{
		return err;
	}
	return err;
}
