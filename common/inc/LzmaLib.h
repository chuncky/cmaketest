/* LzmaLib.h -- LZMA library interface
2008-08-05
Igor Pavlov
Public domain */

#ifndef __LZMALIB_H
#define __LZMALIB_H

#include "7z_types.h"


#ifdef __cplusplus
  #define MY_EXTERN_C extern "C"
#else
  #define MY_EXTERN_C extern
#endif

#define MY_STDAPI MY_EXTERN_C int MY_STD_CALL

#define LZMA_PROPS_SIZE 5

/*
RAM requirements for LZMA:
  for compression:   (dictSize * 11.5 + 6 MB) + state_size
  for decompression: dictSize + state_size
    state_size = (4 + (1.5 << (lc + lp))) KB
    by default (lc=3, lp=0), state_size = 16 KB.

LZMA properties (5 bytes) format
    Offset Size  Description
      0     1    lc, lp and pb in encoded form.
      1     4    dictSize (little endian).
*/


/*
LzmaCompress

--------------
In:
  dest     - output data
  destLen  - output data size
  src      - input data
  srcLen   - input data size
Out:
  destLen  - processed output size
  srcLen   - processed input size
Returns:
  SZ_OK                - OK
  SZ_ERROR_DATA        - Data error
  SZ_ERROR_MEM         - Memory allocation arror
  SZ_ERROR_UNSUPPORTED - Unsupported properties
  SZ_ERROR_INPUT_EOF   - it needs more bytes in input buffer (src)
*/

MY_STDAPI LzmaCompress(unsigned char *dest, size_t  *destLen, const unsigned char *src, size_t  srcLen);

/*
LzmaUncompress
--------------
In:
  dest     - output data
  destLen  - output data size
  src      - input data
  srcLen   - input data size
Out:
  destLen  - processed output size
  srcLen   - processed input size
Returns:
  SZ_OK                - OK
  SZ_ERROR_DATA        - Data error
  SZ_ERROR_MEM         - Memory allocation arror
  SZ_ERROR_UNSUPPORTED - Unsupported properties
  SZ_ERROR_INPUT_EOF   - it needs more bytes in input buffer (src)
*/

MY_STDAPI LzmaUncompress(unsigned char *dest, size_t  *destLen, const unsigned char *src, size_t  *srcLen);

#endif
