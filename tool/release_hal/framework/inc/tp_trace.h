#ifndef _TP_TRACE_H_
#define _TP_TRACE_H_


#ifndef TP_LOG_TAG
#define TP_LOG_TAG "tplog"
#endif

#include "ui_log_api.h"

#define	tp_log_info		raw_uart_log

#define TP_LOGI(x, ...) do{tp_log_info(x "\n", ##__VA_ARGS__);}while(0)

//tracepoint enable bit
#define TP_LOG_ALL    	(1 << 0)	//enable all print info
#define TP_LOG_GUI_APP 	(1 << 1)	// short log for GUI task & APPs

#define TP_LOG_ALLMASK  ((TP_LOG_GUI_APP<<1)-1)

#ifdef __cplusplus
extern "C" {
#endif

//flag:TP_LOG_xxx
int tp_log_enable(int flag);
int tp_log_disable(int flag);

extern void tp_log_dump_appname(void);
extern void tp_log_query_update_app_bitinfo(char *pname, int setbit);
extern int tp_log_set_bitinfo(unsigned int type, unsigned int bitX, unsigned int set);
extern unsigned int tp_log_getus(void);


#ifdef		TP_LOG_GUI_APP

#endif	//TP_LOG_GUI_APP


#ifdef __cplusplus
	}
#endif	/* __cplusplus */


#endif	//_TP_TRACE_H_

