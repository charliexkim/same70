#ifndef _MAIN_H_
#define _MAIN_H_

#define SYSTICK_FREQUENCY 	1000

extern uint32_t debug_v[];
extern uint32_t core_clk_hz ;
extern uint32_t peripheral_clk_hz ;
extern uint32_t sys_tick;
extern uint32_t sys_flag;
extern uint32_t sys_bg_cnt;
extern uint32_t sys_irq_cnt;
extern uint32_t tmp_irq_cnt;

extern void identify(void);

#endif
