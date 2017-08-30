#include <stdint.h>

#include "same70.h"
#include "main.h"
#include "bd_hw.h"
#include "bd_timer.h"

#define PERIOD_FAST     100
#define PERIOD_SLOW     500

//-----------------------------------------------------------------------------
void timer_init(void)
{
	PMC->PMC_PCER0 = PMC_PCER0_PID23;		// enable clock to TC0
	PIOA->PIO_PDR  = 0x00000001 ;			// pin to peripheral
	PIOA->PIO_ABCDSR[0] |= 0x00000001;		// peripheral[B]
	// PIOA->PIO_ABCDSR[1] |= 0x00000000;
	
	TC0->TC_CHANNEL[0].TC_CMR = 
		TC_CMR_TCCLKS_TIMER_CLOCK5 | TC_CMR_CPCTRG |
		(2<< 14) | (1<<15) | 		// wavesel, wave
		(1<<16) | (2<<18)  ;		// acpa, acpc
		
	timer_set_period(10);
	TC0->TC_CHANNEL[0].TC_IER = TC_IER_CPCS;
	TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;

	NVIC_EnableIRQ(TC0_IRQn);
}

//-----------------------------------------------------------------------------
void timer_set_period(uint16_t i)
{
	uint32_t t;
	
	t = 32768ul * i / 1000ul;
	t /= 2 ;
	TC0->TC_CHANNEL[0].TC_RC = t ;
	TC0->TC_CHANNEL[0].TC_RA = (t>>2);
	TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

//-----------------------------------------------------------------------------
void irq_handler_tc0(void)
{
  	if (TC0->TC_CHANNEL[0].TC_SR & TC_SR_CPCS)
  	{
  	}
}

