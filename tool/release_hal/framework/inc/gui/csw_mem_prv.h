/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/

#if !defined(__UHAL_MEM_PRV_H__)
#define __UHAL_MEM_PRV_H__

#include <ts.h>
#include <base_prv.h>
#include "ui_sym_def.h"
#if 1
#define UHAL_SMS_MALLOC(size)	UOS_MALLOC(size)
#define UHAL_SMS_FREE(p)                 		\
	do {										\
		BOOL is_in_image_heap = IS_IN_IMAGE_HEAP(p); \
		if (!is_in_image_heap) raw_uart_log("UHAL_SMS_FREE ERROR %s %d, %p is not in image heap! \r\n", __FUNCTION__, __LINE__, p); \
		if (p != NULL && is_in_image_heap)UOS_FREE((void*)(p)); 	\
	} while (0)
#define UHAL_SMS_DBG
#else
extern void* UOS_SMS_DBG_Malloc(unsigned long nSize, const char* file, const char* function, unsigned long line);
extern unsigned char UOS_SMS_DBG_Free(void* pMemBlock, const char* file, const char* function, unsigned long line);

#define UHAL_SMS_MALLOC(size)		UOS_SMS_DBG_Malloc((size_t)(size), __FILE__, __func__, __LINE__)
#define UHAL_SMS_FREE(p)			UOS_SMS_DBG_Free((void*)(p), __FILE__, __func__, __LINE__)

#define UHAL_SMS_DBG

#endif
#endif  // H

