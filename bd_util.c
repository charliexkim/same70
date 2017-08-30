#include <stdint.h>
#include <stdlib.h>

#include "bd_util.h"
#include "bd_comm.h"
#include "bd_printf.h"

int  util_err;

static uint32_t xval(char c);
static uint32_t read_as_decimal(uint32_t d, int n_x);
static uint32_t eval_hex(char *p);

#define PBUF_SZ    		128
#define is_digit(c)    ((c) >= '0' && (c) <= '9')

// convert the fisrt two chars of field[0] into a numerical id 
// str is expected to be space delimted.
// field[0] could be a single char
 
/*--------------*/
int eval_s2(const char *s)
/*--------------*/
{
	int t, v, k;
	char *p;
	
	p = s;
	
	t = tstrnlen(s, 64);
	v = 0;
	for (k=0; k<t; k++) {
		if ((*p == ' ') || (*p == '\n') || (k > 1)) break;
		v <<= 8;
		v += *p++;
	}
	return v;
}

/*--------------*/
int scan_x(int *parm)
/*--------------*/
{
	static short n_parm;
	
	short st, k, j;
	char  *s;
	char  *p[N_PARMS];
	char  c;

	util_err = 0;
	
	s=ibuf;
	
	if (*s<' ') {			// any funny char
		parm[0]=0;
		return n_parm;		// repeat last cmd entry
	}
	
	st=0;
	k=0;
	while ((c=*s)!=0) {		// string split
		if (c==' ') {
			if (st==1) {	// end of a field
				st=0;
				k++;
			}
		}
		else {
			if (st==0) {	// new field begins
				p[k]=s;		// store pointer
				st=1;
			}
		}
		s++;
	}

	if (st==1) k++;
	
	// string split done.  k=number of fields
	parm[0] = eval_s2(p[0]);
	
	for (j=1; j<k; j++) {
		parm[j]=eval_hex(p[j]);
	}
	n_parm=k;
	
	parm[0] = (util_err) ? UTIL_ERR : parm[0];

	return n_parm;
}

/*--------------*/
static uint32_t xval(char c)
/*--------------*/
{
    int k;

    c=(c<'a')? c: c-('a'-'A');		// to upper
	k=(c<'A')? c-'0': c-'A'+10;
	if (k > 0x0f) util_err = -1;
    return k;
}

/*--------------*/
static uint32_t read_as_decimal(uint32_t d, int n_x)
/*--------------*/
{
	int mult, k;
	uint32_t v, t;
	
	v=0;
	mult = 1;
	for (k=0; k<n_x; k++) {
		t = d & 0x0000000f;
		if (t>9) util_err = -1;
		v += t * mult;
		d >>= 4;
		mult *= 10;
	}
	return v;
}

/*--------------*/
static uint32_t eval_hex(char *p)
/*--------------*/
{
    uint32_t x, n=0, d=0;
	
    while (xval(*p++)>0x0f);    	// ignore first non-hex digits
    p--;                        	// back off
    while ((x=xval(*p++))<0x10) {
		d=(d<<4)+x;
		n++;
	}
	if (*--p=='.') 					// decimal has '.' at the end
		d=read_as_decimal(d, n);
			
    return d;
}

/*--------------*/
void dummy_wait(int n)
/*--------------*/
{
	for ( ; n>0 ; n--) ;
}


const uint8_t crc8_Table[ ] =
{
     0,  94, 188, 226,  97,  63, 221, 131, 194, 156, 126,  32, 163, 253,  31,  65,
   157, 195,  33, 127, 252, 162,  64,  30,  95,   1, 227, 189,  62,  96, 130, 220,
    35, 125, 159, 193,  66,  28, 254, 160, 225, 191,  93,   3, 128, 222,  60,  98,
   190, 224,   2,  92, 223, 129,  99,  61, 124,  34, 192, 158,  29,  67, 161, 255,
    70,  24, 250, 164,  39, 121, 155, 197, 132, 218,  56, 102, 229, 187,  89,   7,
   219, 133, 103,  57, 186, 228,   6,  88,  25,  71, 165, 251, 120,  38, 196, 154,
   101,  59, 217, 135,   4,  90, 184, 230, 167, 249,  27,  69, 198, 152, 122,  36,
   248, 166,  68,  26, 153, 199,  37, 123,  58, 100, 134, 216,  91,   5, 231, 185,
   140, 210,  48, 110, 237, 179,  81,  15,  78,  16, 242, 172,  47, 113, 147, 205,
    17,  79, 173, 243, 112,  46, 204, 146, 211, 141, 111,  49, 178, 236,  14,  80,
   175, 241,  19,  77, 206, 144, 114,  44, 109,  51, 209, 143,  12,  82, 176, 238,
   50, 108, 142, 208,  83,  13, 239, 177, 240, 174,  76,  18, 145, 207,  45, 115,
   202, 148, 118,  40, 171, 245,  23,  73,   8,  86, 180, 234, 105,  55, 213, 139,
    87,   9, 235, 181,  54, 104, 138, 212, 149, 203,  41, 119, 244, 170,  72,  22,
   233, 183,  85,  11, 136, 214,  52, 106,  43, 117, 151, 201,  74,  20, 246, 168,
   116,  42, 200, 150,  21,  75, 169, 247, 182, 232,  10,  84, 215, 137, 107,  53
} ;

/*--------------*/
uint8_t crc8(uint8_t *data, int nBytes)
/*--------------*/
{
       int i;
       uint8_t crc = 0;     	//initial value
       
       for (i = 0; i < nBytes; i++) {
          crc = crc8_Table[ crc ^ *data++ ] ;
       }
       return crc;
}

