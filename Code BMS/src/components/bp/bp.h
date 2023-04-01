/*
 * bp.h
 *
 *  Created on: Mar 27, 2023
 *      Author: Admin
 */

#ifndef COMPONENTS_BP_BP_H_
#define COMPONENTS_BP_BP_H_

#include <stdint.h>
#include "CO_slave.h"

#define TEMP_SENSOR_NUM												(6)
#define PACK_SERIAL_CELL_NUM										(16)

typedef enum{
	BAT_EMPTY 		= 0,
	BAT_STANDBY 	= 1,
	BAT_CHAGGING 	= 2
}Battery_State;

typedef struct Battery_t Battery;
struct Battery_t{
	CO_Slave base;
	uint16_t *state;
	uint16_t *vol;
	int16_t *cur;
	uint16_t *soc;
	uint16_t *soh;
	uint16_t *cycles;
	int16_t *temp;
	uint16_t cell_vol[PACK_SERIAL_CELL_NUM];
	char version[32];
};

void bat_init(Battery *p_bp);
void bat_set_vol(Battery *p_bp,uint16_t vol);
void bat_set_cur(Battery *p_bp,int16_t cur);
void bat_set_soc(Battery *p_bp,uint16_t soc);
void bat_set_soh(Battery *p_bp,uint16_t soh);
void bat_set_cycles(Battery *p_bp,uint16_t cycles);
void bat_set_temp(Battery *p_bp,int16_t *temp);
void bat_set_cell_vol(Battery *p_bp,uint16_t *cel_vol);
void bat_set_state(Battery *p_bp,Battery_State state);

uint16_t bat_get_vol(Battery *p_bp);
int16_t bat_get_cur(Battery *p_bp);
uint16_t bat_get_soc(Battery *p_bp);
uint16_t bat_get_soh(Battery *p_bp);
uint16_t bat_get_cycles(Battery *p_bp);
int16_t* bat_get_temp(Battery *p_bp);
uint16_t* bat_get_cell_vol(Battery *p_bp);
Battery_State bat_get_state(Battery *p_bp);

#endif /* COMPONENTS_BP_BP_H_ */
