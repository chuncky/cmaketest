/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/
#ifndef __GUI_CP_INTERNEL_HEADER_FILE__
#error "Please don't include this file directly, please include cp_include.h"
#endif

#ifndef __CP_CACHE_API_H__
#define __CP_CACHE_API_H__

#ifdef __cplusplus
extern "C" {
#endif


void  CacheCleanMemory             (void *pMem, unsigned int size);
void  CacheInvalidateMemory        (void *pMem, unsigned int size);
void  CacheCleanAndInvalidateMemory(void *pMem, unsigned int size);

#ifdef __cplusplus
}
#endif

#endif /* __CP_TEMPLATE_API_H__ */
