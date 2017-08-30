#ifndef _BD_TIMER_H_
#define _BD_TIMER_H_

#include <stdint.h>

extern void timer_init(void) ;
extern void timer_set_period(uint16_t i);

extern void irq_handler_tc0(void) ;

#endif

