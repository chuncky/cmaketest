/******************************************************************************
**
**  COPYRIGHT (C) 2002, 2003 Intel Corporation.
**
**  This software as well as the software described in it is furnished under
**  license and may only be used or copied in accordance with the terms of the
**  license. The information in this file is furnished for informational use
**  only, is subject to change without notice, and should not be construed as
**  a commitment by Intel Corporation. Intel Corporation assumes no
**  responsibility or liability for any errors or inaccuracies that may appear
**  in this document or any software that may be provided in association with
**  this document. 
**  Except as permitted by such license, no part of this document may be 
**  reproduced, stored in a retrieval system, or transmitted in any form or by  
**  any means without the express written consent of Intel Corporation. 
**
**  FILENAME:	Typedef.h
**
**  PURPOSE: 	Contain boot ROM typedefs
**                  
******************************************************************************/
 
#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

/**
 * Useful constants
 **/
#define BIT0		0x00000001
#define BIT1		0x00000002
#define BIT2		0x00000004
#define BIT3		0x00000008
#define BIT4		0x00000010
#define BIT5		0x00000020
#define BIT6		0x00000040
#define BIT7		0x00000080
#define BIT8		0x00000100
#define BIT9		0x00000200
#define BIT10		0x00000400
#define BIT11		0x00000800
#define BIT12		0x00001000
#define BIT13		0x00002000
#define BIT14		0x00004000
#define BIT15		0x00008000
#define BIT16		0x00010000
#define BIT17		0x00020000
#define BIT18		0x00040000
#define BIT19		0x00080000
#define BIT20		0x00100000
#define BIT21		0x00200000
#define BIT22		0x00400000
#define BIT23		0x00800000
#define BIT24		0x01000000
#define BIT25		0x02000000
#define BIT26		0x04000000
#define BIT27		0x08000000
#define BIT28		0x10000000
#define BIT29		0x20000000
#define BIT30		0x40000000
#define BIT31		0x80000000
#define SET32		0xffffffff
#define CLEAR32		0x00000000

#define BU_U32 			unsigned int 
#define BU_U16 			unsigned short 
#define BU_U8 			unsigned char

typedef volatile unsigned long 	VUINT32_T;
typedef unsigned long 		   	UINT32_T;
typedef volatile unsigned int  	VUINT_T;
typedef unsigned int 		   	UINT_T;
typedef int			 		   	INT_T;
typedef unsigned short 		   	UINT16_T, USHORT;
typedef volatile unsigned short VUINT16_T;
typedef unsigned char 			UINT8_T;
typedef char		 			INT8_T;

#define PASSED   				0
#define FAILED   				1
//#define TRUE       				1
//#define FALSE      				0
#define LOCKED     				1
#define UNLOCKED   				0

//#define NULL     				0
#ifndef NULL
#define NULL       				0
#endif

typedef void(*FnPVOID)(void);

typedef unsigned int        	UINT,     *PUINT;    // The size is not important
typedef unsigned long long  	UINT64,   *PUINT64;
//typedef unsigned int        	UINT32,   *PUINT32;
typedef unsigned short      	UINT16,   *PUINT16;
typedef unsigned char       	UINT8,    *PUINT8;
typedef unsigned char       	UCHAR,BYTE,*PUCHAR;

typedef int                 	INT,      *PINT;    // The size is not important
typedef long long           	INT64,    *PINT64;
//typedef int                 	INT32,    *PINT32;
typedef short               	INT16,    *PINT16;
//typedef char                	INT8,     *PINT8;
typedef char                	CHAR,     *PCHAR;
//typedef void                	VOID,     *PVOID;

typedef volatile  UINT      	VUINT,    *PVUINT;    // The size is not important
typedef volatile  UINT64    	VUINT64,  *PVUINT64;
typedef volatile  unsigned int    	VUINT32,  *PVUINT32;
typedef volatile  UINT16    	VUINT16,  *PVUINT16;
typedef volatile  UINT8     	VUINT8,   *PVUINT8;
typedef volatile  UCHAR     	VUCHAR,   *PVUCHAR;

typedef volatile  INT       	VINT,     *PVINT;    // The size is not important
typedef volatile  INT64     	VINT64,   *PVINT64;
typedef volatile  int     	VINT32,   *PVINT32;
typedef volatile  INT16     	VINT16,   *PVINT16;
typedef volatile  char      	VINT8,    *PVINT8;
typedef volatile  CHAR      	VCHAR,    *PVCHAR;

typedef struct{
UINT_T	ErrorCode;
UINT_T	StatusCode;
UINT_T 	PCRCode;
}FUNC_STATUS, *pFUNC_STATUS;

#define BU_REG_READ(x) (*(volatile UINT_T *)(x))
#define BU_REG_WRITE(x,y) ((*(volatile UINT_T *)(x)) = y )

#define BU_REG_READ16(x) (*(volatile UINT16 *)(x) & 0xffff)
#define BU_REG_WRITE16(x,y) ((*(volatile UINT16 *)(x)) = y & 0xffff )

#define BU_REG_READ8(x) (*(volatile UINT8 *)(x) & 0xff)
#define BU_REG_WRITE8(x,y) ((*(volatile UINT8 *)(x)) = y & 0xff )

#define BU_REG_RDMDFYWR(x,y)  (BU_REG_WRITE(x,((BU_REG_READ(x))|y)))

//#define APUartLogPrintf UartLogPrintf
#define APUartLogPrintf uart_printf


#undef DBGPRINT

#ifdef DBGPRINT
	extern int armprintf(const char *fmt, ...);
	#define DEBUGMSG(cond,str) ((void)((cond)?(armprintf str),1:0))
#else
	#define DEBUGMSG(cond,str) ((void)0)
#endif // DEBUG

#endif //_TYPEDEF_H_
