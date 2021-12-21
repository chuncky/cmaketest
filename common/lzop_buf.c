#include "lzoconf.h"
#include "lzop.h"


static lzo_uint32 hdr_checksum_crc32=0;
static lzo_uint32 hdr_checksum_adler32=0;

static const unsigned char lzop_magic[9] =
{ 0x89, 0x4c, 0x5a, 0x4f, 0x00, 0x0d, 0x0a, 0x1a, 0x0a };


static const lzo_uint32 lzo_crc32_table[256] = {
  0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
  0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
  0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
  0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
  0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
  0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
  0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
  0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
  0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
  0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
  0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
  0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
  0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
  0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
  0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
  0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
  0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
  0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
  0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
  0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
  0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
  0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
  0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
  0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
  0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
  0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
  0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
  0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
  0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
  0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
  0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
  0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
  0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
  0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
  0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
  0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
  0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
  0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
  0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
  0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
  0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
  0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
  0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
  0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
  0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
  0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
  0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
  0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
  0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
  0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
  0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
  0x2d02ef8dL
};


#define LZO_DO1_CRC32(buf,i) \
    crc = lzo_crc32_table[((unsigned)crc ^ buf[i]) & 0xff] ^ (crc >> 8)

#define LZO_DO2_CRC32(buf,i)  LZO_DO1_CRC32(buf,i); LZO_DO1_CRC32(buf,i+1)
#define LZO_DO4_CRC32(buf,i)  LZO_DO2_CRC32(buf,i); LZO_DO2_CRC32(buf,i+2)
#define LZO_DO8_CRC32(buf,i)  LZO_DO4_CRC32(buf,i); LZO_DO4_CRC32(buf,i+4)
#define LZO_DO16_CRC32(buf,i) LZO_DO8_CRC32(buf,i); LZO_DO8_CRC32(buf,i+8)


lzo_uint32 lzo_crc32(lzo_uint32 c, const lzo_bytep buf, lzo_uint len)
{
    lzo_uint32 crc;

    if (buf == NULL)
        return 0;

    crc = (c & (0xffffffff)) ^ (0xffffffff);
    if (len >= 16)
    {
		do
	    {
	        LZO_DO16_CRC32(buf,0);
	        buf += 16;
	        len -= 16;
	    } while (len >= 16);
    }
	
	if (len != 0) 
	{
		do
	    {
	        LZO_DO1_CRC32(buf,0);
	        buf += 1;
	        len -= 1;
	    } while (len > 0);
	}
    return crc ^ (0xffffffff);
}


static int lzo_memcmp(const void *a, const void *b, lzo_uint32 size)
{
	const UINT8 *x = a;
	const UINT8 *y = b;
	lzo_uint32 i;
	for(i=0; i<size; ++i)
		if(x[i] != y[i])
			return 1;
		
	return 0;
}

#define LZO_BASE 65521u /* largest prime smaller than 65536 */
#define LZO_NMAX 5552
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

#define LZO_DO1_ADLER32(buf,i)  s1 += buf[i]; s2 += s1
#define LZO_DO2_ADLER32(buf,i)  LZO_DO1_ADLER32(buf,i); LZO_DO1_ADLER32(buf,i+1)
#define LZO_DO4_ADLER32(buf,i)  LZO_DO2_ADLER32(buf,i); LZO_DO2_ADLER32(buf,i+2)
#define LZO_DO8_ADLER32(buf,i)  LZO_DO4_ADLER32(buf,i); LZO_DO4_ADLER32(buf,i+4)
#define LZO_DO16_ADLER32(buf,i) LZO_DO8_ADLER32(buf,i); LZO_DO8_ADLER32(buf,i+8)


lzo_uint32 lzo_adler32(lzo_uint32 adler, const lzo_bytep buf, lzo_uint len)
{
    lzo_uint32 s1 = adler & 0xffff;
    lzo_uint32 s2 = (adler >> 16) & 0xffff;
    unsigned k;

    if (buf == NULL)
        return 1;

    while (len > 0)
    {
        k = len < LZO_NMAX ? (unsigned) len : LZO_NMAX;
        len -= k;
        if (k >= 16) 
        {
			do
	        {
	            LZO_DO16_ADLER32(buf,0);
	            buf += 16;
	            k -= 16;
	        } while (k >= 16);
        }
		
		if (k != 0) 
		{
			do
	        {
	            s1 += *buf++;
	            s2 += s1;
	        } while (--k > 0);
		}
        s1 %= LZO_BASE;
        s2 %= LZO_BASE;
    }
    return (s2 << 16) | s1;
}

lzo_uint32 set_crc32_checksum(lzo_uint32 val)
{
	hdr_checksum_crc32 = val;
    return 0;
}

lzo_uint32 set_adler32_checksum(lzo_uint32 val)
{
	hdr_checksum_adler32 = val;
    return 0;
}

lzo_uint32 get_crc32_checksum()
{
	return hdr_checksum_crc32;
}

lzo_uint32 get_adler32_checksum()
{
	return hdr_checksum_adler32;
}


void *lzo_memcpy(void *dest, const void *src, lzo_uint32 size)
{
	UINT8 *d = dest;
	const UINT8 *s = src;
	lzo_uint32 i;
	
	if(d<s)
	{
		for(i=0; i<size; ++i)
			d[i] = s[i];
	}
	else if(d >s)
	{
		i = size;
		while(i-- > 0)
			d[i]=s[i];
	}

	return dest;
}

static unsigned get_be16(const unsigned char *b)
{
    unsigned v;

    v  = (unsigned) b[1] <<  0;
    v |= (unsigned) b[0] <<  8;
    return v;
}

static void set_be16(unsigned char *b, unsigned v)
{
    b[1] = (unsigned char) (v >>  0);
    b[0] = (unsigned char) (v >>  8);
}

static lzo_uint32 get_be32(const unsigned char *b)
{
    lzo_uint32 v;

    v  = (lzo_uint32) b[3] <<  0;
    v |= (lzo_uint32) b[2] <<  8;
    v |= (lzo_uint32) b[1] << 16;
    v |= (lzo_uint32) b[0] << 24;
    return v;
}

static void set_be32(unsigned char *b, lzo_uint32 v)
{
    b[3] = (unsigned char) (v >>  0);
    b[2] = (unsigned char) (v >>  8);
    b[1] = (unsigned char) (v >> 16);
    b[0] = (unsigned char) (v >> 24);
}

void  lzo_read_buf(lzo_bytep  src,  lzo_voidp buffer, lzo_int cnt)
{
	lzo_memcpy(buffer,src,cnt);
	hdr_checksum_adler32= lzo_adler32(hdr_checksum_adler32,buffer,cnt);
	hdr_checksum_crc32 = lzo_crc32(hdr_checksum_adler32,buffer,cnt);
}

void lzo_write_buf(lzo_bytep  dst, const lzo_voidp buffer, lzo_int cnt)
{
	lzo_memcpy(dst,buffer,cnt);
	hdr_checksum_adler32= lzo_adler32(hdr_checksum_adler32,buffer,cnt);
	hdr_checksum_crc32 = lzo_crc32(hdr_checksum_adler32,buffer,cnt);
}


void lzo_read16(lzo_bytep  src, unsigned *v)
{
    unsigned char b[2];
    lzo_read_buf(src,b,2);
    *v = get_be16(b);

}

 
void lzo_write16(lzo_bytep  dst, unsigned v)
{
	unsigned char b[2];
	set_be16(b,v);
	lzo_write_buf(dst,b,2);


}
 
void lzo_read32(lzo_bytep  src, lzo_uint32 *v)
{
	unsigned char b[4];
	lzo_read_buf(src,b,4);
	*v = get_be32(b);
}

void lzo_write32(lzo_bytep dst, lzo_uint32 v)
{
    unsigned char b[4];
    set_be32(b,v);
	lzo_write_buf(dst,b,4);

}

int check_magic(const unsigned char *magic)
{
    const unsigned char *m;

    if (lzo_memcmp(magic,lzop_magic,sizeof(lzop_magic)) == 0)
        return 0;

    /* We have a bad magic signature. Try to figure what possibly
     * could have gone wrong. */

    /* look at bytes 1-3: "LZO" in hex and local text format */
    if (lzo_memcmp(&magic[1],&lzop_magic[1],3) != 0 &&
        lzo_memcmp(&magic[1],"LZO",3) != 0)
        return 1;

    /* look at byte 4 */
    if (magic[4] != lzop_magic[4])
        return 1;

    /* look at bytes 5-8 */
    m = &magic[5];
    if (lzo_memcmp(m,"\012\012\032",3) == 0)
        return 7;
    if (lzo_memcmp(m,"\012\012",2) == 0)
        return 4;
    if (lzo_memcmp(m,"\012\032",2) == 0)
        return 4;
    if (lzo_memcmp(m,"\015\012\012",3) == 0)
        return 10;
    if (lzo_memcmp(m,"\015\012\032\012",4) == 0)
        return 9;
    if (lzo_memcmp(m,"\015\012\032\015",4) == 0)
        return 8;
    if (lzo_memcmp(m,"\015\015\012\032",4) == 0)
        return 8;
    if (lzo_memcmp(m,"\015\015\032",3) == 0)
        return 6;
    if (lzo_memcmp(m,"\015\032",2) == 0)
        return 5;
    if (lzo_memcmp(m,&lzop_magic[5],4) != 0)
        return 12;

    /* look at byte 0 */
    if (magic[0] == (unsigned char) (lzop_magic[0] & 0x7f))
        return 11;
    if (magic[0] != lzop_magic[0])
        return 12;

    return 3;
}

// unsigned char magic_l[sizeof(lzop_magic)];
/* return 0 for valid magic, -1 for EOF, or positive value for error */
 int p_magic(lzo_bytep src, lzo_uint32p readLen)
{
    int r;
    unsigned char magic[sizeof(lzop_magic)];

    lzo_read_buf(src,magic,sizeof(magic));
    
    r = check_magic(magic);
	if(r == 0)
	{
		*readLen = sizeof(lzop_magic);
	}
	else
	{	
		*readLen=0;
	}
    return r;
}

int write_magic(lzo_bytep buf)
{
  
    lzo_write_buf(buf,lzop_magic,sizeof(lzop_magic));
    return sizeof(lzop_magic);
}


