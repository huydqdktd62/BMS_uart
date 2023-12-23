/*
 * interrupt_hw.h
 *
 *  Created on: Jan 24, 2022
 *      Author: quangnd
 */

#ifndef BOARD_RA_BOARD_INTERRUPT_HW_INTERRUPT_HW_H_
#define BOARD_RA_BOARD_INTERRUPT_HW_INTERRUPT_HW_H_
#define STATE_MACHINE_UPDATE_TICK SysTick_Handler
#define DISABLE_STM_UPDATE_TICK         SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |\
                   SysTick_CTRL_TICKINT_Msk   |\
                   SysTick_CTRL_ENABLE_Msk

#define ENABLE_STM_UPDATE_TICK SysTick->CTRL &= ~(SysTick_CTRL_CLKSOURCE_Msk |\
                   SysTick_CTRL_TICKINT_Msk   |\
                   SysTick_CTRL_ENABLE_Msk)


void SysTick_Handler(void);
#endif /* BOARD_RA_BOARD_INTERRUPT_HW_INTERRUPT_HW_H_ */
