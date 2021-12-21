/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/
#ifndef __GUI_CP_INTERNEL_HEADER_FILE__
#error "Please don't include this file directly, please include cp_include.h"
#endif

#ifndef _CP_LZOP_API_H_
#define _CP_LZOP_API_H_

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************
 *
 * Name:		lzop_decompress_safe
 *
 * Description: de-compress the data compressed with lzop tool (lzop -9).
 *
 * Parameters:
 *	lzo_bytep		src			 [IN]	pointer to the data which will be decompressed
 *	lzo_bytep		dest	 	 [IN] pointer to the memory which used to store the decompressed data
 *	lzo_uint32p	dest_len		 [OUT] pointer to the length of the decompressed data
 *	lzo_uint32p cpz_src_len	 [OUT] pointer to the length of data which will be decompressed
 *
 * Returns:
 *		SUCCESS: 0
 *		FAIL: others
 *
 * Notes:
 *
 ***********************************************************************/
int lzop_decompress_safe(unsigned char *src,  unsigned char * dest, 
    unsigned int *dest_len , unsigned int * cpz_src_len);

#ifdef __cplusplus
}
#endif

#endif /*_CP_LZOP_API_H_*/
