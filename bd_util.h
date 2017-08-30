#ifndef __BD_UTIL_H__
#define __BD_UTIL_H__

#include <stdint.h>

#define N_PARMS		8
#define UTIL_ERR	-1

extern uint8_t crc8(uint8_t *data, int nBytes) ;
extern void dummy_wait(int n);

extern int scan_x(int *parm);
extern int eval_s2(const char *s);

#endif	/* __BD_UTIL_H__ */
