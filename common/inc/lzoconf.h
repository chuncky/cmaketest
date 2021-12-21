/* lzoconf.h -- configuration of the LZO data compression library

   This file is part of the LZO real-time data compression library.

   Copyright (C) 1996-2017 Markus Franz Xaver Johannes Oberhumer
   All Rights Reserved.

   The LZO library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   The LZO library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the LZO library; see the file COPYING.
   If not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

   Markus F.X.J. Oberhumer
   <markus@oberhumer.com>
   http://www.oberhumer.com/opensource/lzo/
 */


#ifndef __LZOCONF_H_INCLUDED
#define __LZOCONF_H_INCLUDED 1
#include "common.h"
#define LZO_VERSION             0x20a0  /* 2.10 */
#define LZO_VERSION_STRING      "2.10"
#define LZO_VERSION_DATE        "Mar 01 2017"

//#include "global_types.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
typedef signed char     INT8;         /* Signed 8-bit quantity          */
typedef signed short    INT16;        /* Signed 16-bit quantity         */
typedef signed long     INT32;        /* Signed 32-bit quantity         */
typedef unsigned char   UINT8;        /* Unsigned 8-bit quantity        */
typedef unsigned short  UINT16;       /* Unsigned 16-bit quantity       */
typedef unsigned long   UINT32;       /* Unsigned 32-bit quantity       */
typedef unsigned long   BU_U32;       /* Unsigned 32-bit quantity       */
typedef unsigned char   BOOL;         /* Boolean                        */

#ifndef TRUE
  #define FALSE   0
  #define TRUE	  1
#endif	/* TRUE */
#endif
typedef unsigned int      lzo_uint32;
typedef unsigned long   lzo_uint;

typedef signed int        lzo_int32;
typedef signed long     lzo_int;

/* no typedef here because of const-pointer issues */
#define lzo_bytep               unsigned char *
#define lzo_charp               char  *
#define lzo_voidp               void  *
#define lzo_shortp              short  *
#define lzo_ushortp             unsigned short  *
#define lzo_uint32p             lzo_uint32  *
#define lzo_int32p              lzo_int32  *

#define lzo_uintp               lzo_uint  *
#define lzo_intp                lzo_int  *

#define lzo_voidpp              lzo_voidp  *
#define lzo_bytepp              lzo_bytep  *
/* deprecated - use 'lzo_bytep' instead of 'lzo_byte *' */
#define lzo_byte                unsigned char 

typedef int lzo_bool;
#define NULL 0

/***********************************************************************
// function types
************************************************************************/

/* name mangling */
#if !defined(__LZO_EXTERN_C)
#ifdef __cplusplus
	#define __LZO_EXTERN_C      extern "C"
#else
	#define __LZO_EXTERN_C      extern
#endif
#endif


/***********************************************************************
// error codes and prototypes
************************************************************************/

/* Error codes for the compression/decompression functions. Negative
 * values are errors, positive values will be used for special but
 * normal events.
 */
#define LZO_E_OK                    0
#define LZO_E_ERROR                 (-1)
#define LZO_E_OUT_OF_MEMORY         (-2)    /* [lzo_alloc_func_t failure] */
#define LZO_E_NOT_COMPRESSIBLE      (-3)    /* [not used right now] */
#define LZO_E_INPUT_OVERRUN         (-4)
#define LZO_E_OUTPUT_OVERRUN        (-5)
#define LZO_E_LOOKBEHIND_OVERRUN    (-6)
#define LZO_E_EOF_NOT_FOUND         (-7)
#define LZO_E_INPUT_NOT_CONSUMED    (-8)
#define LZO_E_NOT_YET_IMPLEMENTED   (-9)    /* [not used right now] */
#define LZO_E_INVALID_ARGUMENT      (-10)



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* already included */


/* vim:set ts=4 sw=4 et: */
