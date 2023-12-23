/*
 * rgb_hw.c
 *
 *  Created on: Oct 25, 2020
 *      Author: nguyenquang
 */
#include "rgb_hw.h"
#include "hal_data.h"
#include "timer_hw.h"



static volatile bool blink_enable = 0;
static volatile uint32_t indication_state = 0;
static volatile uint32_t is_change_state = 0;
static volatile uint32_t soc_st = 0;
static volatile uint32_t fault_status = 0;
static volatile uint8_t blink_state=0;
static uint32_t delay_cnt=0;

static volatile uint8_t led1_st;
static volatile uint8_t led2_st;
static volatile uint8_t led3_st;
static volatile uint8_t led4_st;

void rgb_hw_init(void) {

	led1_st=0;
	led2_st=0;
	led3_st=0;
	led4_st=0;
}

void rgb_set_color(const uint8_t a, const uint8_t b, const uint8_t c,const uint8_t d,
		const bool blink) {
	blink_enable = blink;
	led1_st = a;
	led2_st = b;
	led3_st = c;
	led4_st = d;

}
void set_indication_state(uint8_t state, uint32_t soc, uint32_t status) {
	uint32_t indication_state_old = indication_state;
	indication_state = state;
	if(indication_state_old != indication_state){
		is_change_state = 1;
	}
	soc_st = soc;
	fault_status = status;
}
void rgb_blink_enable() {
	if (blink_enable == 0) {
		blink_enable = 1;
	}
}

void rgb_blink_disable(void) {
	blink_enable = 0;
}

void rgb_off(void) {
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
	LED4_OFF;
}
static void set_led_state(uint8_t led1, uint8_t led2, uint8_t led3, uint8_t led4 ) {
	if(led1 == 1) LED1_ON;
	else if (led1 == 2) LED1_TOGGLE;
	else LED1_OFF;

	if(led2 == 1) LED2_ON;
	else if (led2 == 2) LED2_TOGGLE;
	else LED2_OFF;

	if(led3 == 1) LED3_ON;
	else if (led3 == 2) LED3_TOGGLE;
	else LED3_OFF;

	if(led4 == 1) LED4_ON;
	else if (led4 == 2) LED4_TOGGLE;
	else LED4_OFF;

}
void update_interface_task(void)
{
	if(is_change_state == 1) {
		delay_cnt = 0;
		rgb_off();
		is_change_state = 0;
	}
	switch (indication_state) {
	case 5: //BMS_ST_FAULT:
		if( fault_status == 0x80)set_led_state(2,2,2,2);
		else if( fault_status == 0x40)set_led_state(2,2,2,0);
		else if( fault_status == 0x10)set_led_state(2,2,2,0);
		else if( fault_status == 0x30)set_led_state(2,2,2,0);
		else if( fault_status == 0x20)set_led_state(2,2,2,0);
		else if( fault_status == 0x08)set_led_state(0,2,2,0);
		else if( fault_status == 0x04)set_led_state(0,2,2,0);
		else if( (fault_status >> 8) == 0x10)set_led_state(2,2,0,0);
		else if( (fault_status >> 8) == 0x20)set_led_state(2,2,0,0);
		else set_led_state(0,0,0,2);
		break;
	case 4: //BMS_ST_CHARGING:
		if(delay_cnt++ < 20 ){
			set_led_state(0,0,0,1);
			break;
		}
		else if(delay_cnt++ < 23 ){
			if(soc_st >= 25 )set_led_state(1,0,0,0);
			break;
		}
		else if(delay_cnt++ < 26 ){
			if(soc_st >= 50 )set_led_state(1,1,0,0);
			break;
		}
		else if(delay_cnt++ < 29 ){
			if(soc_st > 75 )set_led_state(1,1,1,0);
			break;
		}
		if(soc_st >= 95) set_led_state(1,1,1,1);

		else if(soc_st < 95 && soc_st >= 75) set_led_state(1,1,1,2);

		else if(soc_st < 75 && soc_st >= 50) set_led_state(1,1,2,0);

		else if(soc_st < 50 && soc_st >= 25) set_led_state(1,2,0,0);

		else set_led_state(2,0,0,0);
		break;
	case 3: //BMS_ST_DISCHARGING:
		if(delay_cnt++ < 20 ){
			set_led_state(0,0,0,1);
			break;
		}
		else if(delay_cnt++ < 23 ){
			if(soc_st >= 25 )set_led_state(1,0,0,0);
			break;
		}
		else if(delay_cnt++ < 26 ){
			if(soc_st >= 50 )set_led_state(1,1,0,0);
			break;
		}
		else if(delay_cnt++ < 29 ){
			if(soc_st > 75 )set_led_state(1,1,1,0);
			break;
		}
		if(soc_st >= 75) set_led_state(1,1,1,1);

		else if(soc_st < 75 && soc_st >= 50) set_led_state(1,1,1,0);

		else if(soc_st < 50 && soc_st >= 25) set_led_state(1,1,0,0);

		else set_led_state(1,0,0,0);
		break;
	case 2: //BMS_ST_SOFTSTART:
		set_led_state(0,1,2,0);
		break;
	case 1: //BMS_ST_IDLE:
			set_led_state(0,1,0,0);
		break;
	case 7: //BMS_ST_SYSTEM_BOOST_UP:

		if(delay_cnt++ < 20 ){
			set_led_state(0,1,1,0);
			break;
		}
		else if(delay_cnt++ < 23 ){
			if(soc_st >= 25 )set_led_state(1,0,0,0);
			break;
		}
		else if(delay_cnt++ < 26 ){
			if(soc_st >= 50 )set_led_state(1,1,0,0);
			break;
		}
		else if(delay_cnt++ < 29 ){
			if(soc_st > 75 )set_led_state(1,1,1,0);
			break;
		}
		if(soc_st >= 95) set_led_state(1,1,1,1);

		else if(soc_st < 95 && soc_st >= 75) set_led_state(1,1,1,2);

		else if(soc_st < 75 && soc_st >= 50) set_led_state(1,1,2,0);

		else if(soc_st < 50 && soc_st >= 25) set_led_state(1,2,0,0);

		else set_led_state(2,0,0,0);
		break;
	case 8: //BMS_ST_ID_ASSIGN_START:
	case 9: //BMS_ST_ID_ASSIGN_WAIT_CONFIRM:
	case 10: //BMS_ST_ID_ASSIGN_CONFIRMED:
	case 11: //BMS_ST_ID_ASSIGN_WAIT_SLAVE_SELECT:
	case 12: //BMS_ST_START_AUTHENTICATE:
	case 13: //BMS_ST_AUTHENTICATING:
		set_led_state(0,0,0,2);
		break;
	case 14: //BMS_ST_STANDBY:
		set_led_state(0,0,1,0);
		break;
	case 17:
	case 18:
		set_led_state(0, 0, 2, 0);
		break;
	default:
		rgb_set_color(0, 0, 0, 0, 0);
		break;
	}

}

