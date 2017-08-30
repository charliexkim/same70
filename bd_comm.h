#ifndef __BD_COMM_H__
#define __BD_COMM_H__

#include <stdint.h>

/* i_flag collection */
#define INTERACTIVE	0x0001	/* i_flag */

extern volatile uint32_t i_flag;
extern volatile uint32_t i_in;
extern volatile uint32_t i_out;
extern uint8_t ibuf[];

extern void uart_init(uint32_t baud);
extern void uart_putc(char c) ;
extern void uart_puts(char *s);

extern void irq_handler_usart1(void) ;
extern int _write (int handle, char *buffer, int size);

#endif	/* #ifndef __BD_COMM_H__ */
