
#ifndef BOARD_RA2L1_BSP_TIMER_HW_TIMER_HW_H_
#define BOARD_RA2L1_BSP_TIMER_HW_TIMER_HW_H_

#include "hal_data.h"
typedef struct Timer_hw_t Timer_hw;
typedef void	(*irq_handle)(void);
struct Timer_hw_t{
	agt_instance_ctrl_t p_hw_timer0;
	gpt_instance_ctrl_t p_hw_timer1;
	agt_instance_ctrl_t p_hw_timer2;
	gpt_instance_ctrl_t p_hw_timer3;
	void	(*timer0_irq_handle)(void);
	void	(*timer1_irq_handle)(void);
	void	(*timer2_irq_handle)(void);
	void	(*timer3_irq_handle)(void);
};

extern Timer_hw timer;
void timer_hw_init(void);
void state_machine_update_timer(void);
void update_interface_task(void);
void sys_cnt_timer(void);
void enable_timer(void);
void enable_sys_cnt_timer(void);
void soc_update(void);
#endif /* BOARD_RA2L1_BSP_TIMER_HW_TIMER_HW_H_ */
