#include "same70.h"
#include "main.h"

/* ------------ */
void systick_init(void)
/* ------------ */
{
    
	SysTick->LOAD = core_clk_hz/SYSTICK_FREQUENCY;
	SysTick->CTRL  = 
			  SysTick_CTRL_ENABLE_Msk 
			| SysTick_CTRL_TICKINT_Msk 
			| SysTick_CTRL_CLKSOURCE_Msk;

}

/* ------------ */
void irq_handler_sys_tick(void)
/* ------------ */
{
	extern void main_tick(void);
    
    sys_tick++;
	main_tick();
}
