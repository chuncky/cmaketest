/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/
#ifndef __GUI_CP_INTERNEL_HEADER_FILE__
#error "Please don't include this file directly, please include cp_include.h"
#endif

#ifndef __CP_LOG_API_H__
#define __CP_LOG_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#define	LOG_EMERG	"<0>"	/* system is unusable				*/
#define	LOG_ALERT	"<1>"	/* action must be taken immediately	*/
#define	LOG_CRIT	"<2>"	/* critical conditions				*/
#define	LOG_ERR		"<3>"	/* error conditions					*/
#define	LOG_WARNING	"<4>"	/* warning conditions				*/
#define	LOG_NOTICE	"<5>"	/* normal but significant condition	*/
#define	LOG_INFO	"<6>"	/* informational					*/
#define	LOG_DEBUG	"<7>"	/* debug-level messages				*/
#define	LOG_DEFAULT	"<d>"	/* system is unusable				*/

#define LOG_EMERG_VALUE 	0
#define LOG_ALERT_VALUE		1
#define LOG_CRIT_VALUE		2
#define LOG_ERR_VALUE		3
#define LOG_WARNING_VALUE	4
#define LOG_NOTICE_VALUE	5
#define LOG_INFO_VALUE		6
#define LOG_DEBUG_VALUE		7

#define LOG_OUTPUT_MEMLOG	0
#define LOG_OUTPUT_NONE		1
#define LOG_OUTPUT_UART		2
#define LOG_OUTPUT_ACAT		3
#define LOG_OUTPUT_NVM		4

void log_dump(const unsigned char* ptr, unsigned int length);
void log_printf(const char* fmt, ...);
void log_flush(void);
void log_init(void);

void log_set_trace_level(int level);
void log_set_module_level(const char* module, int level);
int  log_get_module_level(const char* module);
void log_set_output(void *parameter);

int fatal_printf(const char* fmt, ...);
void seagull_uart_init(void);
void seagull_uart_putc(const char ch);

void set_uart_dev_uart(void);
void set_uart_dev_diag(void);

#ifdef __cplusplus
}
#endif

#endif /* __CP_LOG_API_H__ */
