#include <stdarg.h>

#include "bd_comm.h"
#include "bd_printf.h"

#define PBUF_SZ    		128
#define is_digit(c)    ((c) >= '0' && (c) <= '9')

static char *itoa(int i, int radix, int len, char *buf);
static int tsprintf(char *sout, const char *fmt, va_list argp);

/*--------*/
void tprintf(const char *fmt, ...)
/*--------*/
{
	char pbuf[128];
	va_list args;

	va_start (args, fmt);
	tsprintf (pbuf, fmt, args);
	va_end (args);
	uart_puts(pbuf);
}

/*--------*/
int xsprintf(char *buf, const char *fmt, ...)
/*--------*/
{
	va_list args;

	va_start (args, fmt);
	tsprintf (buf, fmt, args);
	va_end (args);
	return tstrnlen(buf, 16);
}

/*--------*/
int tstrnlen(const char * s, int count)
/*--------*/
{
	const char *sc;

	for (sc = s; count-- && *sc != '\0'; ++sc) ;
	
	return (int)(sc - s);
}


/*--------*/
static char *itoa(int i, int radix, int len, char *buf)
/*--------*/
// radix should be 10 or 16
{
    char *pos = buf + PBUF_SZ - 1;
    int negative = 0;
	int cnt=0;
    int c;
    unsigned int u;
    
    if ((radix == 10) && (i < 0 )) {
        negative = 1;
        u = ((unsigned int)(-(1+i))) + 1;
    } else u = i;

    *pos = 0;

    do {
		if (radix == 10 ) {
        	c = '0' + (u % 10);
        	u /= 10;
		}
		else {	// radix == 16
        	c = '0' + (u & 0x0f);
        	if (c>'9') c = c - '0' -10 + 'a';
        	u >>= 4;
		}
        *--pos = c;
        cnt++;
    } while (u);
    
    for ( ; cnt<len; cnt++) *--pos = '0';
	
    if (negative) *--pos = '-';

    return pos;
}


/*--------*/
static int tsprintf(char *sout, const char *fmt, va_list argp)
/*--------*/
{
    const char *p;
    const char *q;
    int i, len;
    char *s, *sbuf;
	char fmtbuf[PBUF_SZ];
	
    sbuf = sout;
	
	for(p = fmt; *p != '\0'; p++) {
        if(*p != '%') {
			*sout++ = *p;
            continue;
        }
        
        q=p+1;
        len=0;
        i=0;
		
        while (is_digit(*q)) {
            i++;
            len = len*10 + *q++ - '0';
        }
        p += i;

        switch(*++p) {
            case 'c':
                i = va_arg(argp, int);
				*sout++ = i;
                break;

            case 'd':
                i = va_arg(argp, int);
                s = itoa(i, 10, len, fmtbuf);
                for ( ; *s != 0; s++) *sout++ = *s;
				break;

            case 's':
                s = va_arg(argp, char *);
                for ( ; *s != 0; s++) *sout++ = *s;
                break;

            case 'x':
                i = va_arg(argp, int);
                if (len==0) len=8;
                s = itoa(i, 16, len, fmtbuf);
                for ( ; *s != 0; s++) *sout++ = *s;
                break;

            case '%':
				*sout++ = '%';
                break;
        }
    }
	*sout=0;
	
	return tstrnlen(sbuf, 128);
}
