/* LzmaLib.c -- LZMA library wrapper
2008-08-05
Igor Pavlov
Public domain */

#include "tinyalloc.h"

#include "LzmaEnc.h"
#include "LzmaDec.h"
#include "LzmaLib.h"

typedef struct _LZMA_EXT_HEADER
{
  unsigned int oldsize;
  unsigned int version;
} LZMA_EXT_HEADER;

#define LZMA_HEADER_SIZE   8
#if 0
void *MyAlloc(size_t size)
{
  if (size == 0)
    return 0;
  #ifdef _SZ_ALLOC_DEBUG
  {
    void *p = malloc(size);
    fprintf(stderr, "\nAlloc %10d bytes, count = %10d,  addr = %8X", size, g_allocCount++, (unsigned)p);
    return p;
  }
  #else
  return malloc(size);
  #endif
}

void MyFree(void *address)
{
  #ifdef _SZ_ALLOC_DEBUG
  if (address != 0)
    fprintf(stderr, "\nFree; count = %10d,  addr = %8X", --g_allocCount, (unsigned)address);
  #endif
  free(address);
}

#endif
static void *SzAlloc(void *p, size_t size) { p = p; return MyAlloc(size); }
static void SzFree(void *p, void *address) { p = p; MyFree(address); }
static ISzAlloc g_Alloc = { SzAlloc, SzFree };


/*
LzmaCompress_internal

------------

outPropsSize -
     In:  the pointer to the size of outProps buffer; *outPropsSize = LZMA_PROPS_SIZE = 5.
     Out: the pointer to the size of written properties in outProps buffer; *outPropsSize = LZMA_PROPS_SIZE = 5.

  LZMA Encoder will use defult values for any parameter, if it is
  -1  for any from: level, loc, lp, pb, fb, numThreads
   0  for dictSize
  
level - compression level: 0 <= level <= 9;

  level dictSize algo  fb
    0:    16 KB   0    32
    1:    64 KB   0    32
    2:   256 KB   0    32
    3:     1 MB   0    32
    4:     4 MB   0    32
    5:    16 MB   1    32
    6:    32 MB   1    32
    7+:   64 MB   1    64
 
  The default value for "level" is 5.

  algo = 0 means fast method
  algo = 1 means normal method

dictSize - The dictionary size in bytes. The maximum value is
        128 MB = (1 << 27) bytes for 32-bit version
          1 GB = (1 << 30) bytes for 64-bit version
     The default value is 16 MB = (1 << 24) bytes.
     It's recommended to use the dictionary that is larger than 4 KB and
     that can be calculated as (1 << N) or (3 << N) sizes.

lc - The number of literal context bits (high bits of previous literal).
     It can be in the range from 0 to 8. The default value is 3.
     Sometimes lc=4 gives the gain for big files.

lp - The number of literal pos bits (low bits of current position for literals).
     It can be in the range from 0 to 4. The default value is 0.
     The lp switch is intended for periodical data when the period is equal to 2^lp.
     For example, for 32-bit (4 bytes) periodical data you can use lp=2. Often it's
     better to set lc=0, if you change lp switch.

pb - The number of pos bits (low bits of current position).
     It can be in the range from 0 to 4. The default value is 2.
     The pb switch is intended for periodical data when the period is equal 2^pb.

fb - Word size (the number of fast bytes).
     It can be in the range from 5 to 273. The default value is 32.
     Usually, a big number gives a little bit better compression ratio and
     slower compression process.

numThreads - The number of thereads. 1 or 2. The default value is 2.
     Fast mode (algo = 0) can use only 1 thread.

Out:
  destLen  - processed output size
Returns:
  SZ_OK               - OK
  SZ_ERROR_MEM        - Memory allocation error
  SZ_ERROR_PARAM      - Incorrect paramater
  SZ_ERROR_OUTPUT_EOF - output buffer overflow
  SZ_ERROR_THREAD     - errors in multithreading functions (only for Mt version)
*/

MY_STDAPI LzmaCompress_internal(unsigned char *dest, size_t  *destLen, const unsigned char *src, size_t  srcLen,
  unsigned char *outProps, size_t *outPropsSize,
  int level, /* 0 <= level <= 9, default = 5 */
  unsigned dictSize, /* use (1 << N) or (3 << N). 4 KB < dictSize <= 128 MB */
  int lc, /* 0 <= lc <= 8, default = 3  */
  int lp, /* 0 <= lp <= 4, default = 0  */
  int pb, /* 0 <= pb <= 4, default = 2  */
  int fb,  /* 5 <= fb <= 273, default = 32 */
  int numThreads /* 1 or 2, default = 2 */
)
{
  CLzmaEncProps props;
  LzmaEncProps_Init(&props);
  props.level = level;
  props.dictSize = dictSize;
  props.lc = lc;
  props.lp = lp;
  props.pb = pb;
  props.fb = fb;
  props.numThreads = numThreads;

  return LzmaEncode(dest, destLen, src, srcLen, &props, outProps, outPropsSize, 0,
      NULL, &g_Alloc, &g_Alloc);
}


int LzmaCompress(unsigned char *dest, size_t  *destLen, const unsigned char *src, size_t  srcLen)
{
	size_t propsSize = LZMA_PROPS_SIZE;
	int res=0;
	LZMA_EXT_HEADER *lzma_header=NULL;
	size_t  dest_Len = 2*srcLen;
	
	res=LzmaCompress_internal(dest + LZMA_PROPS_SIZE + LZMA_HEADER_SIZE, &dest_Len, src, srcLen,
		  dest, &propsSize,
		  5, /* 0 <= level <= 9, default = 5 */
		  (1 << 19),/* use (1 << N) or (3 << N). 4 KB < dictSize <= 128 MB */
		  3, /* 0 <= lc <= 8, default = 3  */
		  0, /* 0 <= lp <= 4, default = 0  */
		  2, /* 0 <= pb <= 4, default = 2  */
		  32,  /* 5 <= fb <= 273, default = 32 */
		  2 /* 1 or 2, default = 2 */
		);
	//uart_printf("LzmaCompress[%d],len[%08x],destLen[%08x]compress file len[0x%08X]\r\n",res,srcLen,dest_Len,dest_Len+LZMA_PROPS_SIZE+LZMA_HEADER_SIZE);

	lzma_header =(LZMA_EXT_HEADER *) (dest + LZMA_PROPS_SIZE);
	lzma_header->oldsize = srcLen;
	*destLen = dest_Len+LZMA_PROPS_SIZE + LZMA_HEADER_SIZE;

	return res;
	
}


#if 0
MY_STDAPI LzmaUncompress(unsigned char *dest, size_t  *destLen, const unsigned char *src, size_t  *srcLen)
{
  SRes ret;
  ELzmaStatus status=LZMA_STATUS_NOT_SPECIFIED;
  const unsigned char *props = src;
  size_t propsSize = 13;
  unsigned char *temp=NULL;
  int ii;
  unsigned long dstLen=0;
  
  temp = (unsigned char *)src + 5;
  for (ii = 0; ii < 4; ii++)
  {
	  unsigned char b;
	  b = *temp;
	  temp ++;
	  dstLen += (unsigned int)(b) << (ii * 8);
  }
  //uart_printf("\r\n dstLen[%08x]\r\n",dstLen);
  ret = LzmaDecode(dest, (SizeT *)&dstLen, src+propsSize, srcLen, props, (unsigned)propsSize, LZMA_FINISH_ANY, &status, &g_Alloc);
  //uart_printf("\r\n LzmaDecode status[%08x]\r\n",status);
  *destLen = dstLen;
  return ret;
}

#endif