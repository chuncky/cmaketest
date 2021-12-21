#ifndef __LZOP_H
#define __LZOP_H 1
//#include "global_types.h"
#include "lzoconf.h"


#define ADLER32_INIT_VALUE  1
#define CRC32_INIT_VALUE    0

#define LZOP_VERSION            0x1030
#define LZOP_VERSION_STRING     "1.03"
#define LZOP_VERSION_DATE       "Nov 1st 2010"

/*************************************************************************
// lzop file header
**************************************************************************/

#define BLOCK_SIZE        (256*1024l)


/* header flags */
#define F_ADLER32_D     0x00000001L
#define F_ADLER32_C     0x00000002L
#define F_STDIN         0x00000004L
#define F_STDOUT        0x00000008L
#define F_NAME_DEFAULT  0x00000010L
#define F_DOSISH        0x00000020L
#define F_H_EXTRA_FIELD 0x00000040L
#define F_H_GMTDIFF     0x00000080L
#define F_CRC32_D       0x00000100L
#define F_CRC32_C       0x00000200L
#define F_MULTIPART     0x00000400L
#define F_H_FILTER      0x00000800L
#define F_H_CRC32       0x00001000L
#define F_H_PATH        0x00002000L
#define F_MASK          0x00003FFFL

/* operating system & file system that created the file [mostly unused] */
#define F_OS_FAT        0x00000000L         /* DOS, OS2, Win95 */
#define F_OS_AMIGA      0x01000000L
#define F_OS_VMS        0x02000000L
#define F_OS_UNIX       0x03000000L
#define F_OS_VM_CMS     0x04000000L
#define F_OS_ATARI      0x05000000L
#define F_OS_OS2        0x06000000L         /* OS2 */
#define F_OS_MAC9       0x07000000L
#define F_OS_Z_SYSTEM   0x08000000L
#define F_OS_CPM        0x09000000L
#define F_OS_TOPS20     0x0a000000L
#define F_OS_NTFS       0x0b000000L         /* Win NT/2000/XP */
#define F_OS_QDOS       0x0c000000L
#define F_OS_ACORN      0x0d000000L
#define F_OS_VFAT       0x0e000000L         /* Win32 */
#define F_OS_MFS        0x0f000000L
#define F_OS_BEOS       0x10000000L
#define F_OS_TANDEM     0x11000000L
#define F_OS_SHIFT      24
#define F_OS_MASK       0xff000000L

/* character set for file name encoding [mostly unused] */
#define F_CS_NATIVE     0x00000000L
#define F_CS_LATIN1     0x00100000L
#define F_CS_DOS        0x00200000L
#define F_CS_WIN32      0x00300000L
#define F_CS_WIN16      0x00400000L
#define F_CS_UTF8       0x00500000L         /* filename is UTF-8 encoded */
#define F_CS_SHIFT      20
#define F_CS_MASK       0x00f00000L

//#define LZOP_BLOCK_SIZE        (256*1024l)
#define LZOP_BLOCK_SIZE        (64*1024l)
#define MAX_BLOCK_SIZE      (64*1024l*1024l)        /* DO NOT CHANGE */
/* these bits must be zero */
#define F_RESERVED      ((F_MASK | F_OS_MASK | F_CS_MASK) ^ 0xffffffffL)

typedef struct
{
    unsigned version;
    unsigned lib_version;
    unsigned version_needed_to_extract;
    unsigned char method;
    unsigned char level;
    lzo_uint32 flags;
    lzo_uint32 filter;
    lzo_uint32 mode;
    lzo_uint32 mtime_low;
    lzo_uint32 mtime_high;
    lzo_uint32 header_checksum;

    lzo_uint32 extra_field_len;
    lzo_uint32 extra_field_checksum;

     /* info */
    const char *method_name;

    char name[255+1];
}header_t;




enum {
    M_LZO1X_1     =     1,
    M_LZO1X_1_15  =     2,
    M_LZO1X_999   =     3,
    M_NRV1A       =  0x1a,
    M_NRV1B       =  0x1b,
    M_NRV2A       =  0x2a,
    M_NRV2B       =  0x2b,
    M_NRV2D       =  0x2d,
    M_ZLIB        =   128,

    M_UNUSED
};


/***********************************************************************
 *
 * Name:		decompress_image
 *
 * Description: de-compress the data compressed with lzop tool (lzop -9).
 *
 * Parameters:
 *	UINT8		*src		 	[IN]	pointer to the data which will be decompressed
 *	UINT8		*dest	 	[OUT] pointer to the memory which used to store the decompressed data
 *
 * Returns:
 *		FAIL: 0
 *		SUCCESS: the length of decompressed data, should equals to expectLen
 *
 * Notes:
 *
 ***********************************************************************/

UINT32 decompress_image(UINT8 *src, UINT8 *dest);

#endif
