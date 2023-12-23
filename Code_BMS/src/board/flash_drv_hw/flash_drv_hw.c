/*
 * flash_drv_hw.c
 *
 *  Created on: Jul 28, 2022
 *      Author: Admin
 */


#include <wire_hw/wire_hw.h>
#include "flash_drv_hw.h"
#include "hal_data.h"
#include "fsp_common_api.h"

static void exflash_selected(Ex_Flash *flash_drv);
static void exflash_unselected(Ex_Flash *flash_drv);
static int exflash_open(Ex_Flash *flash_drv);
static int exflash_close(Ex_Flash *flash_drv);
static int exflash_write(Ex_Flash *flash_drv,uint8_t *src,uint32_t length);
static int exflash_writeread(Ex_Flash *flash_drv,uint8_t *src,uint8_t* dest,uint32_t length);
static int exflash_read(Ex_Flash *flash_drv,uint8_t *src,uint32_t length);
Ex_Flash flash_DRV;
void flash_drv_init(Ex_Flash *flash_drv,uint16_t nss_pin){
	flash_drv->nss_pin = nss_pin;
	flash_drv->selected = exflash_selected;
	flash_drv->unselected = exflash_unselected;
	flash_drv->open = exflash_open;
	flash_drv->close = exflash_close;
	flash_drv->write = exflash_write;
	flash_drv->writeread = exflash_writeread;
	flash_drv->read = exflash_read;
	int err = FSP_SUCCESS;
	err = R_IOPORT_PinWrite(&g_ioport_ctrl,flash_drv->nss_pin,NSS_PIN_HIGH);
	if(err != FSP_SUCCESS) return;
	flash_drv->wire = &my_wire_spi;
	wire_begin(flash_drv->wire,(void*)&g_spi0);
/*	if(flash_drv->open(flash_drv) != FSP_SUCCESS){
		debug_prints("spi driver error\r\n");
	}*/
}


static void exflash_selected(Ex_Flash *flash_drv){
	R_IOPORT_PinWrite(&g_ioport_ctrl,flash_drv->nss_pin,NSS_PIN_LOW);
}
static void exflash_unselected(Ex_Flash *flash_drv){
	R_IOPORT_PinWrite(&g_ioport_ctrl,flash_drv->nss_pin,NSS_PIN_HIGH);
}
static int exflash_open(Ex_Flash *flash_drv){
	Wire *spi_drv = flash_drv->wire;
	return spi_drv->open(spi_drv);
}
static int exflash_close(Ex_Flash *flash_drv){
	Wire *spi_drv = flash_drv->wire;
	return spi_drv->close(spi_drv);
}
static int exflash_write(Ex_Flash *flash_drv,uint8_t *src,uint32_t length){
	Wire *spi_drv = flash_drv->wire;
	return spi_drv->write(spi_drv,src,length);
}
static int exflash_writeread(Ex_Flash *flash_drv,uint8_t *src,uint8_t* dest,uint32_t length){
	Wire *spi_drv = flash_drv->wire;
	return spi_drv->write_read(spi_drv,src,dest,length);
}
static int exflash_read(Ex_Flash *flash_drv,uint8_t *src,uint32_t length){
	Wire *spi_drv = flash_drv->wire;
	return spi_drv->read(spi_drv,src,length);
}
