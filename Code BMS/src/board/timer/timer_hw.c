/*
 * timer_hw.c
 *
 *  Created on: Mar 30, 2023
 *      Author: Admin
 */


#include "timer_hw.h"

Timer bsp_timer0 = {.p_driver = (void*)&timer0};
Timer bsp_timer1 = {.p_driver = (void*)&timer1};
void timer_init(Timer *p_timer){
    timer_instance_t *driver = (timer_instance_t *)p_timer->p_driver;
    R_GPT_Open(driver->p_ctrl,driver->p_cfg);
}
void timer_start(Timer *p_timer){
    timer_instance_t *driver = (timer_instance_t *)p_timer->p_driver;
    R_GPT_Start(driver->p_ctrl);
}
void timer_stop(Timer *p_timer){
    timer_instance_t *driver = (timer_instance_t *)p_timer->p_driver;
    R_GPT_Stop(driver->p_ctrl);
}

void timer_add_callback(Timer *p_timer,timer_handle handle,void* arg){
    p_timer->callback = handle;
    p_timer->arg = arg;
}



/* Callback function */
void Timer0IRQ_Callback(timer_callback_args_t *p_args)
{
    /* TODO: add your own code here */
    (void)p_args;
    if(bsp_timer0.callback != NULL){
        bsp_timer0.callback(bsp_timer0.arg);
    }
}
/* Callback function */
void Timer1IRQ_Callback(timer_callback_args_t *p_args)
{
    /* TODO: add your own code here */
    (void)p_args;
    if(bsp_timer1.callback != NULL){
        bsp_timer1.callback(bsp_timer1.arg);
    }
}
/* Callback function */
