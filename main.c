/*
 * Copyright (c) 2017, Alex Taradov <alex@taradov.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "same70.h"
#include "bd_systick.h"
#include "bd_comm.h"
#include "bd_printf.h"
#include "bd_hw.h"
#include "bd_timer.h"

uint32_t debug_v[8];
uint32_t core_clk_hz ;
uint32_t peripheral_clk_hz ;
uint32_t sys_tick = 0;
uint32_t sys_flag = 0;

uint32_t sys_bg_cnt;
uint32_t sys_irq_cnt;
uint32_t tmp_irq_cnt;
	
//-----------------------------------------------------------------------------

const char *id_bd   = "SoC  : *** ATSAME70Q21  ***";
const char *id_code = "Code : $SAME70, "__DATE__" - "__TIME__;

static void sys_init(void);

/* ------------ */
void identify(void)
/* ------------ */
{
	tprintf("\n-------------------------------\n");
	tprintf("%s\n", id_bd);
	tprintf("%s\n", id_code);
	tprintf("core=300 MHz, peripherals=150 MHz\n");
	tprintf("-------------------------------\n");
}

//-----------------------------------------------------------------------------
int main(void)
{
  	extern void interact(void);
	extern void TRNG_Enable(void);
	extern char *prompt;
	
	static uint32_t bg_cnt;
	
	sys_init();
	core_clk_hz = 300000000;
	peripheral_clk_hz = 150000000;

	timer_init();
	uart_init(115200);

	identify();
	LED_init();
	GPIO_init();
	systick_init();
	TRNG_Enable();

	tprintf("%s", prompt);
	
  while (1)
  {
  	bg_cnt++;
	if (sys_flag & 0x0001 ) {
		sys_bg_cnt = bg_cnt;
		bg_cnt = 0;
		sys_irq_cnt = tmp_irq_cnt;
		tmp_irq_cnt = 0;
		sys_flag &= ~0x0001;
	}
	interact();
  }

  return 0;
}

/* -------- */
// int _exit(void) {}
void _sbrk(void) {}
void _close(void) {}
void _fstat(void) {}
void _isatty(void) {}
void _lseek(void) {}
void _read(void) {}
/* -------- */

//-----------------------------------------------------------------------------
static void sys_init(void)
{
	// Disable watchdog
	WDT->WDT_MR = WDT_MR_WDDIS;

	// Set flash wait states to maximum for 150 MHz operation
	EFC->EEFC_FMR = EEFC_FMR_FWS(5) | EEFC_FMR_CLOE;

	// Enable 12 MHz Xtal
	PMC->CKGR_MOR = CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCXTST(8) |
		CKGR_MOR_MOSCRCEN | CKGR_MOR_MOSCXTEN;
	while (!(PMC->PMC_SR & PMC_SR_MOSCXTS));

	PMC->CKGR_MOR = CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCXTST(8) |
		CKGR_MOR_MOSCRCEN | CKGR_MOR_MOSCXTEN | CKGR_MOR_MOSCSEL;
	while (!(PMC->PMC_SR & PMC_SR_MOSCSELS));

	// Setup PLL (12 MHz * 25 = 300 MHz)
	PMC->CKGR_PLLAR = CKGR_PLLAR_ONE | CKGR_PLLAR_MULA(25-1) |
		CKGR_PLLAR_PLLACOUNT(0x3f) | CKGR_PLLAR_DIVA(1);
	while (!(PMC->PMC_SR & PMC_SR_LOCKA));

	// Switch main clock to PLL (two step process)
	PMC->PMC_MCKR = PMC_MCKR_CSS_MAIN_CLK | PMC_MCKR_MDIV_PCK_DIV2;
	while (!(PMC->PMC_SR & PMC_SR_MCKRDY));

	PMC->PMC_MCKR = PMC_MCKR_CSS_PLLA_CLK | PMC_MCKR_MDIV_PCK_DIV2;
	while (!(PMC->PMC_SR & PMC_SR_MCKRDY));

	// Enable PIOA, PIOB, PIOC, PIOD and PIOE
	PMC->PMC_PCER0 = PMC_PCER0_PID10 | PMC_PCER0_PID11 | PMC_PCER0_PID12 |
		PMC_PCER0_PID16 | PMC_PCER0_PID17;

	// Disable altenate functions on some pins
	MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO4;
}

