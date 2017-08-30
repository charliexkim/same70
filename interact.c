#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "same70.h"
#include "main.h"
#include "bd_systick.h"
#include "bd_comm.h"
#include "bd_util.h"
#include "bd_printf.h"

const char *prompt = "E70> ";

static void error(int ty);
static int peek_n_poke(int n, int *parm);
static int proc_cmd(int n, int *parm);

#define cmd_rw		0x7277		// "rw", read uint32_t
#define cmd_rh		0x7268		// "rh", read uint16_t
#define cmd_rb		0x7262		// "rb", read uint8_t
#define cmd_ww		0x7777		// "ww", write uint32_t
#define cmd_wh		0x7768		// "wh", write uint16_t
#define cmd_wb		0x7762		// "wb", write uint8_t

#define N_PARMS 8

/* ------------ */
void interact(void)
/* ----------- */
{
	static uint32_t prev_cmd;
	static int parm[N_PARMS];
	extern int LED(int on);
	
	short n;
	
	if (i_flag & INTERACTIVE) {
		i_in=i_out=0;
		i_flag &= ~INTERACTIVE;
		n=scan_x(parm);

		if (parm[0]==0) {
			parm[0]=prev_cmd;
		}
		else {
			prev_cmd=parm[0];
		}
		
        proc_cmd(n, parm);
        tprintf(prompt);
	}
}



/* ----------- */
static void error(int ty)
/* ----------- */
{
	tprintf("Error : %d\n", ty);
}

/* ----------- */
static int proc_cmd(int n, int *parm)
/* ----------- */
{
	peek_n_poke(n, parm);
	return 0;
}


static uint32_t addr_offs = 0;

/* ----------- */
static int peek_n_poke(int n, int *parm)
/* ----------- */
{
	int ret, k;
	
	uint32_t *addr;
	uint16_t *haddr;
	uint8_t  *baddr;

	
	ret = 0;
	
	switch (parm[0]) {
	
		// ----------temporary tests-------------			
		case 't' :
			if (n<2) n=1;
			else n=parm[1];
			for (k=0; k<n; k++)
				tprintf("%08x\n", TRNG->TRNG_ODATA);
			break ;
		// --------------------------------------
		
		case 'v' :		// show (or set) debug_v[]
			if (n<3) {
				for (k=0; k<8; k++) tprintf("v[%d]=%08x %d.\n", 
					k, debug_v[k], debug_v[k]);
			}
			else debug_v[parm[1]]=parm[2];
			break;
			
		case 'O' :		// show (or set) addr offset
			if (n>1) addr_offs=parm[1] & 0xfffffffc;
			tprintf("addr_offs = %08x\n", addr_offs);
			break;
						
		case 'r' :		/* read32. (addr, cnt) */
		case cmd_rw :
			if (n<3) parm[2]=1;
			addr = (uint32_t *)((parm[1] & 0xfffffffc) + addr_offs);
			for (k=0; k<parm[2]; k++, addr++) tprintf("%08x %08x\n", (uint32_t)addr, *addr);
			break;
			
		case cmd_rh :		/* read16. (addr, cnt) */
			if (n<3) parm[2]=1;
			haddr = (uint16_t *)((parm[1] & 0xfffffffe) + addr_offs);
			for (k=0; k<parm[2]; k++, haddr++) tprintf("%04x\n", *haddr);
			break;
			
		case cmd_rb :		/* read8. (addr, cnt) */
			if (n<3) parm[2]=1;
			baddr = (uint8_t *)(parm[1] + addr_offs);
			for (k=0; k<parm[2]; k++, baddr++) tprintf("%02x\n", *baddr);
			break;
			
		case 'w':		/* write32. (addr, dat, [cnt]) */
		case cmd_ww:
			if (n<3) {
				error(2);
				break;
			}
			if (n<4) parm[3] = 1;
			addr = (uint32_t *)((parm[1]&0xfffffffc) + addr_offs);
			for (k=0; k<parm[3]; k++) *addr++ = parm[2];
			break;

		case cmd_wh:		/* write16. (addr, dat, [cnt]) */
			if (n<3) {
				error(2);
				break;
			}
			if (n<4) parm[3] = 1;
			haddr = (uint16_t *)((parm[1]&0xfffffffe) + addr_offs );
			for (k=0; k<parm[3]; k++) *haddr++ = parm[2];
			break;

		case cmd_wb:		/* write8. (addr, dat, [cnt]) */
			if (n<3) {
				error(2);
				break;
			}
			if (n<4) parm[3] = 1;
			baddr = (uint8_t *)(parm[1] + addr_offs);
			for (k=0; k<parm[3]; k++) *baddr++ = parm[2];
			break;

		case 'S' :
			identify();
			tprintf("irq, bg_cnt : %d. %d. [/sec]\n", sys_irq_cnt, sys_bg_cnt);
			tprintf("uptime : %d. [sec]\n\n", sys_tick/1000);
			break;
	
		default :
			tprintf("Cmd not implemented\n");
			break;
	}
	return ret;
}

