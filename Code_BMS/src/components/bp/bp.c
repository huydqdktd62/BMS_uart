/*
 * bp.c
 *
 *  Created on: Mar 27, 2023
 *      Author: Admin
 */

#include "bp.h"

void bat_init(Battery *p_bp) {
	bat_set_vol(p_bp, 0);
	bat_set_cur(p_bp, 0);
	bat_set_soc(p_bp, 0);
	bat_set_soh(p_bp, 0);
	bat_set_cycles(p_bp, 0);
	bat_set_temp(p_bp, 0);
	bat_set_cell_vol(p_bp, 0);
}
void bat_set_vol(Battery *p_bp, uint16_t vol) {
	*p_bp->vol = vol;
}
void bat_set_cur(Battery *p_bp, int16_t cur) {
	*p_bp->cur = cur;
}
void bat_set_soc(Battery *p_bp, uint16_t soc) {
	*p_bp->soc = soc;
}
void bat_set_soh(Battery *p_bp, uint16_t soh) {
	*p_bp->soh = soh;
}
void bat_set_cycles(Battery *p_bp, uint16_t cycles) {
	*p_bp->cycles = cycles;
}
void bat_set_temp(Battery *p_bp, int16_t *temp) {
	for(uint16_t i = 0;i<6;i++){
		p_bp->temp[i] = temp[i];
	}
}
void bat_set_cell_vol(Battery *p_bp, uint16_t *cel_vol) {
	for(uint16_t i = 0;i<PACK_SERIAL_CELL_NUM;i++){
		p_bp->cell_vol[i] = cel_vol[i];
	}
}
void bat_set_state(Battery *p_bp,Battery_State state){
	*p_bp->state = state;
}
uint16_t bat_get_vol(Battery *p_bp) {
	return *p_bp->vol;
}
int16_t bat_get_cur(Battery *p_bp) {
	return p_bp->cur[0];
}
uint16_t bat_get_soc(Battery *p_bp) {
	return p_bp->soc[0];
}
uint16_t bat_get_soh(Battery *p_bp) {
	return p_bp->soh[0];
}
uint16_t bat_get_cycles(Battery *p_bp) {
	return p_bp->cycles[0];
}
int16_t* bat_get_temp(Battery *p_bp) {
	return p_bp->temp;
}
uint16_t* bat_get_cell_vol(Battery *p_bp) {
	return p_bp->cell_vol;
}
Battery_State bat_get_state(Battery *p_bp){
	return p_bp->state[0];
}
