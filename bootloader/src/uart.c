#include "common.h"

/****************************************************************
 * console output
 ****************************************************************/
 
#define CP_UART2_BASE   0xd4036000
#define CP_UART_BASE   0xd4017000	
 
void cp_uart_init()
{
	volatile unsigned long *r;
	
	/* PMUM_ACGR */
	writel(0xffffffff,0xd4050024); 

	writel(0x03,0xd401503c); //AIB

	/* UART clock control/UCER */
	writel(0x13,0xd4015000);

	/* MFPR GPIO 29 */
	r = (volatile unsigned long*)0xd401e150;
	*r = 0xd0c1;

	/* MFPR GPIO 30 */
	r = (volatile unsigned long*)0xd401e154;
	*r = 0xd0c1;

	// UALCR
	r = (volatile unsigned long*)(CP_UART_BASE + 0xc);
	*r = 0x83;

	// UADLL
	r = (volatile unsigned long*)(CP_UART_BASE + 0x0);
	*r = 0x08; //0x08 for 14.7456M on DKB, 0x7 for 13M on FPGA;

	// UADLH
	r = (volatile unsigned long*)(CP_UART_BASE + 0x4);
	*r = 0;

	// UALCR
	r = (volatile unsigned long*)(CP_UART_BASE + 0xC);
	*r &= ~0x80;

	// UAFCR
	r = (volatile unsigned long*)(CP_UART_BASE + 0x8);
	*r = 0x07;

	// UAIER
	r = (volatile unsigned long*)(CP_UART_BASE + 0x4);
	*r = 0x40;
}
 
void cp_uart_putc(const char ch)
{
	volatile unsigned long* ualsr;
	volatile unsigned long* uathr;

	ualsr = (volatile unsigned long*)(CP_UART_BASE + 0x14);
	uathr = (volatile unsigned long*)(CP_UART_BASE);

	while (!(*ualsr & 0x20));

	*uathr = ch;
}
 
int uart_printf(const char *fmt, ...)
{
	va_list ap;
	char buffer[128], *ptr;
	void (*console_output)(const char ch);

	memset(buffer, 0, sizeof(buffer));
	va_start(ap, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, ap);
	va_end(ap);

	console_output = cp_uart_putc;

	ptr = &buffer[0];
	while (*ptr)
	{
		console_output(*ptr);
		ptr ++;
	}

	return ptr - buffer;
}

void display_binary(void *data, size_t len)
{
#define XK_BYTE_COUNT_EACH_LINE 8

    static const char hex[] = "0123456789ABCDEF";
    static const char seperator = ' ';
    unsigned char *p = (unsigned char *)data;
    char stash[XK_BYTE_COUNT_EACH_LINE * (2 + 2 + 1) + 1];
    size_t j = 0;
    size_t i;

	uart_printf("\r\n----------------------------\r\n");
    uart_printf("BASE=[%0.8x]LEN=[%0.8x]",data,len);

    for (i = 0; i < len; i=i+2) {
        //stash[j++] = '0';
        //stash[j++] = 'x';
        stash[j++] = hex[(p[i] >> 4) & 0xF];
        stash[j++] = hex[p[i] & 0xF];

        stash[j++] = hex[(p[i+1] >> 4) & 0xF];
        stash[j++] = hex[p[i+1] & 0xF];

        stash[j++] = seperator;
        if (j == sizeof(stash) - 1) {
            stash[j] = '\0';
            uart_printf("\r\n[%0.8x]%s",((unsigned int)data + i - 0xe), stash);
            j = 0;
        }
    }
    if (j) {
        stash[j] = '\0';
        uart_printf("\r\n[%0.8x]%s",((unsigned int)data + i - 0xe + XK_BYTE_COUNT_EACH_LINE), stash);
    }
	uart_printf("\r\n----------------------------\r\n");
}



void cp_uart2_init()
{
	volatile unsigned long *r;

	/* PMUM_ACGR */
	writel(0xdffefffe,0xd4051024); //0x2DFFFF; //0xdffefffe;

	writel(0x03,0xd401503c); //AIB

	/* UART clock control/UCER */
	writel(0x3,0xd403b01c);

	/* MFPR GPIO 120 */
	r = (volatile unsigned long*)0xd401e384;
	*r = 0xd0c2;

	/* MFPR GPIO 121 */
	r = (volatile unsigned long*)0xd401e388;
	*r = 0xd0c2;

	// UALCR
	r = (volatile unsigned long*)(CP_UART2_BASE + 0xc);
	*r = 0x83;

	// UADLL
	r = (volatile unsigned long*)(CP_UART2_BASE + 0x0);
	*r = 0x08; //0x08 for 14.7456M on DKB, 0x7 for 13M on FPGA;

	// UADLH
	r = (volatile unsigned long*)(CP_UART2_BASE + 0x4);
	*r = 0;

	// UALCR
	r = (volatile unsigned long*)(CP_UART2_BASE + 0xC);
	*r &= ~0x80;

	// UAFCR
	r = (volatile unsigned long*)(CP_UART2_BASE + 0x8);
	*r = 0x07;

	// UAIER
	r = (volatile unsigned long*)(CP_UART2_BASE + 0x4);
	*r = 0x40;
}
 
void cp_uart2_putc(const char ch)
{
	volatile unsigned long* ualsr;
	volatile unsigned long* uathr;

	ualsr = (volatile unsigned long*)(CP_UART2_BASE + 0x14);
	uathr = (volatile unsigned long*)(CP_UART2_BASE);

	while (!(*ualsr & 0x20));

	*uathr = ch;
}
 
int uart2_printf(const char *fmt, ...)
{
	va_list ap;
	char buffer[128], *ptr;
	void (*console_output)(const char ch);

	memset(buffer, 0, sizeof(buffer));
	va_start(ap, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, ap);
	va_end(ap);

	console_output = cp_uart2_putc;

	ptr = &buffer[0];
	while (*ptr)
	{
		console_output(*ptr);
		ptr ++;
	}

	return ptr - buffer;
}

