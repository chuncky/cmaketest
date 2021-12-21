#ifndef __CAM_UTIL_H__
#define __CAM_UTIL_H__

//#include "gbl_types.h"
//#include "osa.h"
//#include "utils.h"
#include <ui_os_api.h>
#include "cam_list.h"
#include "plat_types.h"
#include "ui_log_api.h"

#ifndef NULL
#define NULL ((void*)0)
#endif
#if 0
typedef signed   char   int_8;   /* 8-bit signed integer   */
typedef unsigned char  uint_8;  /* 8-bit signed integer   */
typedef          short int_16;  /* 16-bit signed integer  */
typedef unsigned short uint_16; /* 16-bit unsigned integer*/
typedef          long  int_32;  /* 32-bit signed integer  */
typedef unsigned long  uint_32; /* 32-bit unsigned integer*/
#endif

//#define OS_WAIT_FORERVER (0xFFFFFFFFUL)
#define OS_WAIT_100MS (0xFFUL)

//#define BIT(nr)			(1UL << (nr))
#define ALIGN_TO(x, iAlign) ( (((unsigned int)(x)) + (iAlign) - 1) & (~((iAlign) - 1)) )


#define GROUP_INITED 1
#define MAX_LOG_SZIE 80
#define MAX_QUEUE_SZIE 500
#define TX_NO_WAIT          0
#define TX_WAIT_FOREVER     0xFFFFFFFFUL

#define find_entry(pos, head, member, index) \
{ \
	int iii = 0; \
	for (pos = list_entry((head)->next, typeof(*pos), member);	\
	     &pos->member != (head); \
	     pos = list_entry(pos->member.next, typeof(*pos), member)) { \
		if (iii == index) { \
			break; \
		} \
		iii++; \
	} \
}

void msleep(unsigned int i);
#define BU_REG_READ(x) (*(volatile uint32_t *)(x))
#define BU_REG_WRITE(x,y) ((*(volatile uint32_t *)(x)) = (y) )


typedef struct log_queue {
	struct list_head log_head;
	UINT32 timestamp;
	char format[MAX_LOG_SZIE];
} LOG_QUEUE;

typedef struct log_group {
	LOG_QUEUE *log_queue;
	int log_queue_cnt;
	int group_init;
	u8 queue_lock;
} LOG_GROUP;

/*
* Name: 	   log_init
*
* Description:	init log source
*
* Parameters:
*
* Returns:
*
*/
int cam_mem_log_init(void);

/*
* Name: 	   log_print
*
* Description:	print log in log queue
*
* Parameters:
*
* Returns:
*
*/
void cam_mem_log_print(void);

/*
* Name: 	   log_send
*
* Description:	send log to log queue.before use this function,should call log_init first.
*
* Parameters:
* 	format:		the formated log buf.use sprintf format the log to buf.
* Returns:
* 	-1:			log buf is NULL
* 	-2:			log buf size > MAX_LOG_SZIE
*/
int cam_mem_log_send(const char *format);

#endif
