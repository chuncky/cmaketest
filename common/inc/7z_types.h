#ifndef __7Z_TYPES_H
#define __7Z_TYPES_H
#include <string.h>

typedef unsigned char uint8_t ;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
//typedef unsigned long uintptr_t;

#ifndef Byte
#define Byte unsigned char
#endif
	
#ifndef NULL
#define NULL                    0
#endif
typedef int SRes;
typedef int ptrdiff_t;
typedef int Bool;
#define True 1
#define False 0


#define MY_CDECL
#define MY_STD_CALL
#define MY_FAST_CALL

typedef struct
{
  void *(*Alloc)(void *p, size_t size);
  void (*Free)(void *p, void *address); /* address can be 0 */
} ISzAlloc;

#define IAlloc_Alloc(p, size) (p)->Alloc((p), size)
#define IAlloc_Free(p, a) (p)->Free((p), a)

#define SZ_OK 0

#define SZ_ERROR_DATA 1
#define SZ_ERROR_MEM 2
#define SZ_ERROR_CRC 3
#define SZ_ERROR_UNSUPPORTED 4
#define SZ_ERROR_PARAM 5
#define SZ_ERROR_INPUT_EOF 6
#define SZ_ERROR_OUTPUT_EOF 7
#define SZ_ERROR_READ 8
#define SZ_ERROR_WRITE 9
#define SZ_ERROR_PROGRESS 10
#define SZ_ERROR_FAIL 11
#define SZ_ERROR_THREAD 12

#define SZ_ERROR_ARCHIVE 16
#define SZ_ERROR_NO_ARCHIVE 17
#ifndef RINOK
#define RINOK(x) { int __result__ = (x); if (__result__ != 0) return __result__; }
#endif


#ifdef _LZMA_UINT32_IS_ULONG
typedef long Int32;
typedef unsigned long UInt32;
#else
typedef int Int32;
typedef unsigned int UInt32;
#endif

#define UInt16 unsigned short
typedef UInt32 SizeT;


#ifdef _SZ_NO_INT_64

/* define _SZ_NO_INT_64, if your compiler doesn't support 64-bit integers.
   NOTES: Some code will work incorrectly in that case! */

typedef long Int64;
typedef unsigned long UInt64;

#else

#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64 Int64;
typedef unsigned __int64 UInt64;
#else
typedef long long int Int64;
typedef unsigned long long int UInt64;
#endif

#endif


typedef struct
{
  size_t (*Write)(void *p, const void *buf, size_t size);
    /* Returns: result - the number of actually written bytes.
       (result < size) means error */
} ISeqOutStream;


typedef struct
{
  SRes (*Read)(void *p, void *buf, size_t *size);
    /* if (input(*size) != 0 && output(*size) == 0) means end_of_stream.
       (output(*size) < input(*size)) is allowed */
} ISeqInStream;

typedef struct
{
  SRes (*Progress)(void *p, UInt64 inSize, UInt64 outSize);
    /* Returns: result. (result != SZ_OK) means break.
       Value (UInt64)(Int64)-1 for size means unknown value. */
} ICompressProgress;

#endif

