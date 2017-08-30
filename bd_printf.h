#ifndef __BD_PRINTF_H__
#define __BD_PRINTF_H__

extern int tstrnlen(const char * s, int count);
extern void tprintf (const char *fmt, ...);
extern int  xsprintf(char * buf, const char *fmt, ...);

#endif	/* #ifndef __BD_PRINTF_H__ */
