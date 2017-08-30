#include <stdint.h>

#include "same70.h"
#include "main.h"
#include "bd_comm.h"
#include "hal_gpio.h"

#define BUF_SZ 64

uint8_t  ibuf[BUF_SZ];
volatile uint32_t i_in=0;
volatile uint32_t i_out=0;
volatile uint32_t i_flag=0;

HAL_GPIO_PIN(UART_TX,  B, 4)
HAL_GPIO_PIN(UART_RX,  A, 21)

//-----------------------------------------------------------------------------
void uart_init(uint32_t baud)
{
  HAL_GPIO_UART_TX_abcd(3);
  HAL_GPIO_UART_RX_abcd(0);

  PMC->PMC_PCER0 = PMC_PCER0_PID14;

  USART1->US_CR = US_CR_RXEN | US_CR_TXEN;
  USART1->US_MR = US_MR_USART_MODE_NORMAL | US_MR_CHRL_8_BIT | US_MR_PAR_NO |
      US_MR_NBSTOP_1_BIT;
  USART1->US_BRGR = US_BRGR_CD(F_CPU / 16 / baud);
  USART1->US_IER = US_IER_RXRDY;
  NVIC_EnableIRQ(USART1_IRQn);
}

//-----------------------------------------------------------------------------
void uart_putc(char c)
{
  while (0 == (USART1->US_CSR & US_CSR_TXEMPTY));
  USART1->US_THR = c;
}

//-----------------------------------------------------------------------------
void uart_puts(char *s)
{
  while (*s)
	uart_putc(*s++);
	(void)uart_puts;
}

//-----------------------------------------------------------------------------
// int putchar(int c)
// {
//  uart_putc((char)c);
//  return 0;
// }

//-----------------------------------------------------------------------------
int puts(const char *ptr)
{
 for (; *ptr != 0; ptr++)
	 uart_putc(*ptr);
 return 0;
}


/* ------ */
int _write (int handle, char *buffer, int size)
/* ------ */
{
	static uint32_t dmy;
	
	int k, j;
	char *p;
	
    /* This function only writes to "standard out" and "standard err",*/
    /* for all other file handles it returns failure.*/
    
	if ((handle != 1) && (handle != 2)) {
        return -1;
    }

	for (k=0, p=buffer; k<size; k++, p++) {
		uart_putc(*p);
		for (j=0, dmy=0; j<100000; j++) dmy += 1;
	}
	return size;
}

//-----------------------------------------------------------------------------
// extern WEAK char *gets(char *ptr)
// {
// 	uint8_t ch = 0;
// 
// 	while (ch != '\r') {
// 		ch = DBG_GetChar();
// 		DBG_PutChar(ch);
// 		*(ptr++) = ch;
// 	}
// 
// 	*ptr = '\0';
// 	return 0;
// 
// }
// 

 
/* ------------ */
void irq_handler_usart1(void)
/* ------------ */
{

	uint8_t c_in;
	
	tmp_irq_cnt++;
	if (USART1->US_CSR & US_CSR_RXRDY) {
		c_in = USART1->US_RHR & 0x00ff;
		uart_putc(c_in);
		ibuf[i_in++]=c_in;
		i_in &= (BUF_SZ-1);
		if (c_in=='\n') {						// end of input cmd
			i_flag |= INTERACTIVE;				// mark cmd entered
			ibuf[i_in] = 0;						// end of input string
		}
	}
}
