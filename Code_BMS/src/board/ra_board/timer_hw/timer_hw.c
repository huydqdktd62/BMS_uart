/*
 * timer_hw.c
 *
 *  Created on: Nov 26, 2021
 *      Author: KhanhDinh
 */

#include "timer_hw.h"
#include "delay.h"
Timer_hw timer;
void timer_hw_init(void){

    R_GPT_Open(&timer.p_hw_timer1, &g_timer1_cfg);
    R_GPT_Start(&timer.p_hw_timer1);
    delay_ms(2);
    R_AGT_Open(&timer.p_hw_timer2, &g_timer2_cfg);
    R_AGT_Start(&timer.p_hw_timer2);
	g_timer4.p_api->open(g_timer4.p_ctrl, g_timer4.p_cfg);
    g_timer0.p_api->open(g_timer0.p_ctrl,g_timer0.p_cfg);

}
void g_timer1_isr(timer_callback_args_t *p_args)
{
	(void)p_args;
	if(timer.timer1_irq_handle != NULL){
		timer.timer1_irq_handle();
	}
}
void timer4_irq(timer_callback_args_t *p_args) {
	soc_update();
}
void g_timer2_isr(timer_callback_args_t *p_args)
{
	(void)p_args;
	if(timer.timer2_irq_handle != NULL){
		timer.timer2_irq_handle();
	}
}

void g_timer3_isr(timer_callback_args_t *p_args)
{
	(void)p_args;
	if(timer.timer3_irq_handle != NULL){
		timer.timer3_irq_handle();
	}
}

void enable_timer(void){
	timer_hw_init();
	timer.timer1_irq_handle = update_interface_task;
	timer.timer2_irq_handle = state_machine_update_timer;
	g_timer0.p_api->start(g_timer0.p_ctrl);
	g_timer4.p_api->start(g_timer4.p_ctrl);
}
void enable_sys_cnt_timer(void){
    R_GPT_Open(&timer.p_hw_timer3, &g_timer3_cfg);
    R_GPT_Start(&timer.p_hw_timer3);
	timer.timer3_irq_handle = sys_cnt_timer;
    delay_ms(2);
}
