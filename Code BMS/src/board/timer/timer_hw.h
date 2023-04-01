/*
 * timer.h
 *
 *  Created on: Mar 30, 2023
 *      Author: Admin
 */

#ifndef BOARD_TIMER_TIMER_HW_H_
#define BOARD_TIMER_TIMER_HW_H_

#include "hal_data.h"

typedef void (*timer_handle)(void *arg);

typedef struct Timer_t Timer;

struct Timer_t{
    void *p_driver;
    void (*callback)(void* arg);
    void* arg;
    uint32_t freq;
};

void timer_init(Timer *p_timer);
void timer_start(Timer *p_timer);
void timer_stop(Timer *p_timer);

void timer_add_callback(Timer *p_timer,timer_handle handle,void* arg);

extern Timer bsp_timer0 ;
extern Timer bsp_timer1 ;

#endif /* BOARD_TIMER_TIMER_HW_H_ */
