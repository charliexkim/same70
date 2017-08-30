#include <stdint.h>
#include "main.h"
#include "bd_hw.h"

static void task_100hz(void);
static void task_10hz(void);
static void task_1hz(void);

static uint32_t cntr_100hz;
static uint32_t cntr_10hz;
static uint32_t cntr_1hz;


/* ------------ */
void main_tick(void)
/* ------------ */
{
	task_100hz();					// 100hz calls 10hz, 10hz calls 1 hz tasks
}

/* ------------ */
static void task_100hz(void)
/* ------------ */
{
	// distribute 100Hz stuffs here
	switch (cntr_100hz++) {
		case 9 :
			cntr_100hz = 0;
            task_10hz();
			break;
		default :
			break;
	}
}

/* ------------ */
static void task_10hz(void)
/* ------------ */
{
	// distribute 10Hz stuffs here
	switch (cntr_10hz++) {
		case 9 :
			cntr_10hz = 0;
            task_1hz();
			break;

		default :
			break;
	}
}


/* ------------ */
static void task_1hz(void)
/* ------------ */
{
	// distribute 1 Hz stuffs here
	switch (cntr_1hz++) {
		case 0 :
			LED_set(0);
			break;
		case 5 :
			LED_set(1);
			break;
		case 9 :
			cntr_1hz = 0;
			sys_flag |= 0x0001;
			break;
	}
}


