#include<stdio.h>
#include "lzop.h"
#include "minilzo.h"
#include <string.h>
//#include "timer_config.h"
//#include "diag_api.h"
#include "asr_nor_flash.h"

extern void lzo_read16(lzo_bytep  src, unsigned *v);
void lzo_write16(lzo_bytep  dst, unsigned v);

extern int p_magic(lzo_bytep src, lzo_uint32p readLen);
extern  void lzo_read32(lzo_bytep  src, lzo_uint32 *v);
void lzo_write32(lzo_bytep dst, lzo_uint32 v);

extern  void  lzo_read_buf(lzo_bytep  src,  lzo_voidp buffer, lzo_int cnt);
void lzo_write_buf(lzo_bytep  dst, const lzo_voidp buffer, lzo_int cnt);
extern lzo_uint32 lzo_adler32(lzo_uint32 adler, const lzo_bytep buf, lzo_uint len);
extern lzo_uint32 lzo_adler32(lzo_uint32 adler, const lzo_bytep buf, lzo_uint len);
extern lzo_uint32 set_crc32_checksum(lzo_uint32 val);
extern lzo_uint32 set_adler32_checksum(lzo_uint32 val);
extern lzo_uint32 get_crc32_checksum(void);
extern lzo_uint32 get_adler32_checksum(void);
extern lzo_uint32 lzo_crc32(lzo_uint32 c, const lzo_bytep buf, lzo_uint len);

#define MAX_COMPRESSED_SIZE(x)  ((x) + (x) / 16 + 64 + 3)
#define LZO1X_1_MEM_COMPRESS    ((lzo_uint32) (16384L * sizeof(lzo_bytep)))
#define ALIGN_SIZE    4096
#define LZO_PTR_ALIGN_UP(p,size) \
    ((p) + (lzo_uint)((( p + size - 1) & ~(size - 1)) - p))

typedef struct
{
    /* public */
    lzo_bytep   mb_mem;
    lzo_uint32  mb_size;
    /* private */
    lzo_bytep   mb_mem_alloc;
    lzo_uint32  mb_size_alloc;
    lzo_uint32  mb_align;
}
mblock_t;
//static mblock_t wrkmem;

int lzo_get_method(header_t *h)
{
/* check method */
    if (h->method == M_LZO1X_1)
    {
        h->method_name = "LZO1X-1";
        if (h->level == 0)
            h->level = 3;
    }
    else if (h->method == M_LZO1X_1_15)
    {
        h->method_name = "LZO1X-1(15)";
        if (h->level == 0)
            h->level = 1;
    }
    else if (h->method == M_LZO1X_999)
    {
        static char s[11+1] = "LZO1X-999  ";
        s[9] = 0;
        if (h->level == 0)
            h->level = 9;
        else if (h->version >= 0x0950 && h->lib_version >= 0x1020)
        {
            s[9] = '/';
            s[10] = (char) (h->level + '0');
        }
        h->method_name = s;
    }
    else
        return -1;      /* not a LZO method */

/* check compression level */
    if (h->level < 1 || h->level > 9)
        return 15;

    return 0;
}

#if 0
lzo_bool mb_alloc(mblock_t *m, lzo_uint32 size, lzo_uint32 align)
{
    memset(m,0,sizeof(*m));
    m->mb_size = size;
    m->mb_align = (align > 1) ? align : 1;
	
    if (m->mb_size == 0)
        return 1;

    m->mb_size_alloc = m->mb_size + m->mb_align - 1;
    m->mb_mem_alloc = (lzo_bytep) malloc(m->mb_size_alloc);
    if (m->mb_mem_alloc == NULL)
        return 0;
    memset(m->mb_mem_alloc, 0, m->mb_size_alloc);

    m->mb_mem =(lzo_bytep) LZO_PTR_ALIGN_UP((lzo_uint32)m->mb_mem_alloc,m->mb_align);
    ASSERT(m->mb_mem >= m->mb_mem_alloc);
    ASSERT(m->mb_mem + m->mb_size <= m->mb_mem_alloc + m->mb_size_alloc);
    return 1;
}

void lzo_init_compress_header(header_t *h, lzo_bool opt_crc32)
{
	mb_alloc(&wrkmem, LZO1X_1_MEM_COMPRESS, ALIGN_SIZE);
	memset(h,0,sizeof(header_t));

	h->version = LZOP_VERSION & 0xffff;
	h->lib_version = LZO_VERSION & 0xffff;
    h->version_needed_to_extract = 0x0940;
	
    h->method = (unsigned char) M_LZO1X_1;
    h->level = (unsigned char) 1;
	h->name[0] ='\0';
	
	h->flags = 0;
	
    if (opt_crc32)
    {
    	if (h->version_needed_to_extract < 0x1001)
    	{
            h->version_needed_to_extract = 0x1001;
    	}
		h->flags |= F_CRC32_D;
    }
    else
    {
        h->flags |= F_ADLER32_D;
    }

}
#endif
extern int write_magic(lzo_bytep buf);


lzo_uint32 write_header(lzo_bytep dest, header_t *h)
{
    size_t len;
	lzo_bytep buf = dest;

	len =  write_magic(buf);
	buf += len;

	set_adler32_checksum(1);
	set_crc32_checksum(0);
	
    lzo_write16(buf,h->version);
	buf += 2;

    lzo_write16(buf,h->lib_version);
	buf += 2;
	
    lzo_write16(buf,h->version_needed_to_extract);
	buf += 2;

	lzo_write_buf(buf,&h->method,1);
	buf += 1;

	lzo_write_buf(buf,&h->level,1);
	buf += 1;

	lzo_write32(buf,h->flags);
	buf += 4;

    if (h->flags & F_H_FILTER)
    {
    	lzo_write32(buf,h->filter);
		dest += 4;
    }
	lzo_write32(buf,h->mode);
	buf += 4;

	lzo_write32(buf,h->mtime_low);
	buf += 4;

	lzo_write32(buf,h->mtime_high);
	buf += 4;

	
    len = strlen(h->name);
    //assert(len <= 255);

	lzo_write_buf(buf,&len,1);
	buf += 1;

    if (len > 0)
    {
    	lzo_write_buf(buf,h->name,len);
		buf += len;
    }

	//write checksum
	
	if (h->flags & F_H_CRC32)
	{
		lzo_write32(buf,get_crc32_checksum());

	}
    else
    {
       lzo_write32(buf,get_adler32_checksum());
    }
	
	buf += 4;

	return buf-dest;

}



static int read_header(lzo_bytep src, header_t *h, lzo_uint32p out_len)
{
    int r;
    int filename_len=0;
	lzo_bytep in = src;
	
    lzo_uint32 checksum;

    *out_len = 0;
	
    set_adler32_checksum(1);
	set_crc32_checksum(0);
	
    memset(h,0,sizeof(header_t));
    h->version_needed_to_extract = 0x0900;  /* first public lzop version */
    h->level = 0;
    h->method_name = "unknown";

    lzo_read16(src,&h->version);
    src += 2;
    if (h->version < 0x0900)
        return 3;
    
     lzo_read16(src,&h->lib_version);
     src += 2;
    if (h->version >= 0x0940)
    {
        lzo_read16(src,&h->version_needed_to_extract);
        src += 2;
    }
    
    lzo_read_buf(src,&h->method,1);
    src += 1;
    if (h->version >= 0x0940)
    {
    	 lzo_read_buf(src,&h->level,1);
    	 src += 1;
    }
    
    lzo_read32(src,&h->flags);
     src += 4;
    if (h->flags & F_H_FILTER)
    {
   	 lzo_read32(src,&h->filter);
   	  src += 4;
    }
    
    lzo_read32(src,&h->mode);
     src += 4;
    if (h->flags & F_STDIN) 
        h->mode = 0;
    
    lzo_read32(src,&h->mtime_low);
     src += 4;
    if (h->version >= 0x0940)
    {
  	 lzo_read32(src,&h->mtime_high);
  	 src += 4;
    }
	
    if (h->version < 0x0120) {
        if (h->mtime_low == 0xffffffffUL)
            h->mtime_low = 0;
        h->mtime_high = 0;
    }

    lzo_read_buf(src,&filename_len,1);
    src += 1;
    if (filename_len> 0) 
    {
        char name[255+1];
        
       lzo_read_buf(src,name,filename_len);
       name[filename_len] = 0;

       src +=filename_len;
    }

 	checksum = (h->flags & F_H_CRC32) ? get_crc32_checksum(): get_adler32_checksum();
    lzo_read32(src,&h->header_checksum);
     src += 4;
	
    if (h->header_checksum != checksum)
        return 2;
    r = lzo_get_method(h);
    if (r != 0)
        return r;

	*out_len =(lzo_uint32) (src-in);
	
    return 0;
}


/*************************************************************************
// decompress a file
**************************************************************************/
static UINT8 tmp_buf[LZOP_BLOCK_SIZE];

int lzop_decompress_safe(lzo_bytep src,  lzo_bytep dest, lzo_uint32p dest_len , lzo_uint32p cpz_src_len)
{
    int result = -1;
	lzo_bytep in=src;
	lzo_bytep out=dest;
	header_t header;
    lzo_uint32 src_len=0, dst_len=0;
    lzo_uint32 c_adler32 = ADLER32_INIT_VALUE, d_adler32 = ADLER32_INIT_VALUE;
    lzo_uint32 c_crc32 = CRC32_INIT_VALUE, d_crc32 = CRC32_INIT_VALUE;
	lzo_uint32 actualLen=0;
	
	lzo_uint32 in_off;
	result = p_magic(in,&in_off);
 	if (result > 0)
		return -1;
	
	//uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);	
	in += in_off;
 	
	result = read_header(in,&header,&in_off);
	if(result != 0)
		return -1;
	//uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);
	in += in_off;
	
    for (;;)
    {

        /* read uncompressed block size */
        lzo_read32(in,&dst_len);
		in +=4;

	    if( dst_len ==0 )
	    {
			break;
	    }
		else if(dst_len > LZOP_BLOCK_SIZE)
		{
			result = -1;
			break;
		}
		
        /* read compressed block size */
        lzo_read32(in,&src_len);
		in +=4;

		if(src_len ==0)
	    {
			break;
	    }
		else if(src_len > dst_len)
		{
			result = -1;
			break;
		}
         

        /* read checksum of uncompressed block */
        if (header.flags & F_ADLER32_D)
       	{
            lzo_read32(in,&d_adler32);
			in +=4;
        }
        if (header.flags & F_CRC32_D)
        {
            lzo_read32(in,&d_crc32);
			in +=4;
        }
        /* read checksum of compressed block */
        if (header.flags & F_ADLER32_C)
        {
            if (src_len < dst_len)
            {
                lzo_read32(in,&c_adler32);
				in +=4;
            }
			else
            {
                c_adler32 = d_adler32;
            }
        }
        if (header.flags & F_CRC32_C)
        {
            if (src_len < dst_len)
           	{
                lzo_read32(in,&c_crc32);
				in +=4;
            }
			else
            {
                c_crc32 = d_crc32;
            }
        }


        /* verify checksum of compressed block */
        if (header.flags & F_ADLER32_C)
        {
            lzo_uint32 c;
            c = lzo_adler32(ADLER32_INIT_VALUE,in,src_len);
            if (c != c_adler32)
            {
            	
            	//DIAG_FILTER(SW_PLAT, DECOMPRESS, ADLER32_0, DIAG_INFORMATION)
				//diagPrintf ("++ERR+++: compressed adler32 mismatch, expect=0x%lx,current =0x%lx ",c_adler32, c);
            	//uart_printf("++ERR+++: compressed adler32 mismatch\r\n");
				result = -1;
				break;
            }
        }
        if (header.flags & F_CRC32_C)
        {
            lzo_uint32 c;
            c = lzo_crc32(CRC32_INIT_VALUE,in,src_len);
            if (c != c_crc32)
            {
            	//DIAG_FILTER(SW_PLAT, DECOMPRESS, CRC32_0, DIAG_INFORMATION)
				//diagPrintf ("++ERR+++: compressed crc32 mismatch, expect=0x%lx,current =0x%lx ",c_crc32, c);
				
            	//uart_printf("++ERR+++: compressed crc32 mismatch\r\n");
				result = -1;
            	break;
            }
        }

	if(src_len == dst_len){
		memcpy((void *)out,(void *)in,src_len);
	}else{
        	memcpy((void *)tmp_buf,(void *)in,src_len);
        	result = lzo1x_decompress(tmp_buf,src_len,out,&dst_len,NULL);
	}
#if 1 
        if(dst_len > LZOP_BLOCK_SIZE)
        {
            //uart_printf("\r\nunzip dst_len > LZOP_BLOCK_SIZE!!\r\n");
            while(1);
        }
#else
		ASSERT(dst_len <= LZOP_BLOCK_SIZE);
#endif
        //DIAG_FILTER(SW_PLAT, DECOMPRESS, DECOMPRESS_LOOP, DIAG_INFORMATION)
		//diagPrintf ("dst_len=%d,result=%d",dst_len,result);
		
//		uart_printf("function:%s,line:%d,dst_len=%d,result=%d\r\n", __func__,__LINE__,dst_len,result);


        /* verify checksum of uncompressed block */
        if (header.flags & F_ADLER32_D)
        {
            lzo_uint32 c;
            c = lzo_adler32(ADLER32_INIT_VALUE,out,dst_len);
            if (c != d_adler32)
            {
				//DIAG_FILTER(SW_PLAT, DECOMPRESS, ADLER32_1, DIAG_INFORMATION)
				//diagPrintf ("++ERR+++: uncompressed adler32 mismatch, expect=0x%lx,current =0x%lx ",d_adler32, c);
            	//uart_printf("++ERR+++: uncompressed adler32 mismatch\r\n");
				result = -1;
 				break;
            }
        }
        if (header.flags & F_CRC32_D)
        {
            lzo_uint32 c;
            c = lzo_crc32(CRC32_INIT_VALUE,out,dst_len);
            if (c != d_crc32)
            {
            	//DIAG_FILTER(SW_PLAT, DECOMPRESS, CRC32_1, DIAG_INFORMATION)
				//diagPrintf ("++ERR+++: uncompressed crc32 mismatch, expect=0x%lx,current =0x%lx ",d_crc32, c);
            	//uart_printf("++ERR+++: uncompressed crc32 mismatch\r\n");
				result = -1;
            	break;
            }
        }
		
        
	    in += src_len;

			//memcpy(out, tmp_buf, dst_len);
		
			out += dst_len;
			actualLen += dst_len;
		

    }

    //uart_printf("original cpz size = [%08x] \r\n" , (in - src));
    *cpz_src_len = (in - src);
	*dest_len = actualLen;

	 return result;
}


int lzop_decompress_safe_internal_flash(lzo_bytep src,  lzo_bytep dest, lzo_uint32p dest_len , lzo_uint32p cpz_src_len)
{
    int result = -1;
	lzo_bytep in_ptr=src;
	lzo_bytep tmp_buf_ptr=tmp_buf;
	lzo_bytep out=dest;
	header_t header;
    lzo_uint32 src_len=0, dst_len=0;
    lzo_uint32 c_adler32 = ADLER32_INIT_VALUE, d_adler32 = ADLER32_INIT_VALUE;
    lzo_uint32 c_crc32 = CRC32_INIT_VALUE, d_crc32 = CRC32_INIT_VALUE;
	lzo_uint32 actualLen=0;
	
	asr_norflash_read(in_ptr, tmp_buf_ptr, 512);

	lzo_uint32 in_off;
	lzo_uint32 in_off_sum = 0;
	result = p_magic(tmp_buf_ptr,&in_off);
 	if (result > 0)
		return -1;
	
	//uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);	
	tmp_buf_ptr += in_off;
	in_off_sum += in_off;
 	
	result = read_header(tmp_buf_ptr,&header,&in_off);
	if(result != 0)
		return -1;
	//uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);
	in_off_sum += in_off;
	
    for (;;)
    {
		asr_norflash_read(in_ptr + in_off_sum, tmp_buf_ptr, 1024);

        /* read uncompressed block size */
        lzo_read32(tmp_buf_ptr,&dst_len);
		tmp_buf_ptr +=4;
		in_off_sum += 4;

	    if( dst_len ==0 )
	    {
			break;
	    }
		else if(dst_len > LZOP_BLOCK_SIZE)
		{
			result = -1;
			break;
		}
		
        /* read compressed block size */
        lzo_read32(tmp_buf_ptr,&src_len);
		tmp_buf_ptr +=4;
		in_off_sum += 4;

		if(src_len ==0)
	    {
			break;
	    }
		else if(src_len > dst_len)
		{
			result = -1;
			break;
		}
         

        /* read checksum of uncompressed block */
        if (header.flags & F_ADLER32_D)
       	{
            lzo_read32(tmp_buf_ptr,&d_adler32);
			tmp_buf_ptr +=4;
			in_off_sum += 4;
        }
        if (header.flags & F_CRC32_D)
        {
            lzo_read32(tmp_buf_ptr,&d_crc32);
			tmp_buf_ptr +=4;
			in_off_sum += 4;
        }
        /* read checksum of compressed block */
        if (header.flags & F_ADLER32_C)
        {
            if (src_len < dst_len)
            {
                lzo_read32(tmp_buf_ptr,&c_adler32);
				tmp_buf_ptr +=4;
				in_off_sum += 4;
            }
			else
            {
                c_adler32 = d_adler32;
            }
        }
        if (header.flags & F_CRC32_C)
        {
            if (src_len < dst_len)
           	{
                lzo_read32(tmp_buf_ptr,&c_crc32);
				tmp_buf_ptr +=4;
				in_off_sum += 4;
            }
			else
            {
                c_crc32 = d_crc32;
            }
        }

		asr_norflash_read(in_ptr + in_off_sum, tmp_buf_ptr, src_len);

        /* verify checksum of compressed block */
        if (header.flags & F_ADLER32_C)
        {
            lzo_uint32 c;
            c = lzo_adler32(ADLER32_INIT_VALUE,tmp_buf_ptr,src_len);
            if (c != c_adler32)
            {
            	
            	//DIAG_FILTER(SW_PLAT, DECOMPRESS, ADLER32_0, DIAG_INFORMATION)
				//diagPrintf ("++ERR+++: compressed adler32 mismatch, expect=0x%lx,current =0x%lx ",c_adler32, c);
            	//uart_printf("++ERR+++: compressed adler32 mismatch\r\n");
				result = -1;
				break;
            }
        }
        if (header.flags & F_CRC32_C)
        {
            lzo_uint32 c;
            c = lzo_crc32(CRC32_INIT_VALUE,tmp_buf_ptr,src_len);
            if (c != c_crc32)
            {
            	//DIAG_FILTER(SW_PLAT, DECOMPRESS, CRC32_0, DIAG_INFORMATION)
				//diagPrintf ("++ERR+++: compressed crc32 mismatch, expect=0x%lx,current =0x%lx ",c_crc32, c);
				
            	//uart_printf("++ERR+++: compressed crc32 mismatch\r\n");
				result = -1;
            	break;
            }
        }

	if(src_len == dst_len){
		memcpy((void *)out,(void *)tmp_buf_ptr,src_len);
	}else{
        	result = lzo1x_decompress(tmp_buf_ptr,src_len,out,&dst_len,NULL);
	}
#if 1 
        if(dst_len > LZOP_BLOCK_SIZE)
        {
            //uart_printf("\r\nunzip dst_len > LZOP_BLOCK_SIZE!!\r\n");
            while(1);
        }
#else
		ASSERT(dst_len <= LZOP_BLOCK_SIZE);
#endif
        //DIAG_FILTER(SW_PLAT, DECOMPRESS, DECOMPRESS_LOOP, DIAG_INFORMATION)
		//diagPrintf ("dst_len=%d,result=%d",dst_len,result);
		
//		uart_printf("function:%s,line:%d,dst_len=%d,result=%d\r\n", __func__,__LINE__,dst_len,result);


        /* verify checksum of uncompressed block */
        if (header.flags & F_ADLER32_D)
        {
            lzo_uint32 c;
            c = lzo_adler32(ADLER32_INIT_VALUE,out,dst_len);
            if (c != d_adler32)
            {
				//DIAG_FILTER(SW_PLAT, DECOMPRESS, ADLER32_1, DIAG_INFORMATION)
				//diagPrintf ("++ERR+++: uncompressed adler32 mismatch, expect=0x%lx,current =0x%lx ",d_adler32, c);
            	//uart_printf("++ERR+++: uncompressed adler32 mismatch\r\n");
				result = -1;
 				break;
            }
        }
        if (header.flags & F_CRC32_D)
        {
            lzo_uint32 c;
            c = lzo_crc32(CRC32_INIT_VALUE,out,dst_len);
            if (c != d_crc32)
            {
            	//DIAG_FILTER(SW_PLAT, DECOMPRESS, CRC32_1, DIAG_INFORMATION)
				//diagPrintf ("++ERR+++: uncompressed crc32 mismatch, expect=0x%lx,current =0x%lx ",d_crc32, c);
            	//uart_printf("++ERR+++: uncompressed crc32 mismatch\r\n");
				result = -1;
            	break;
            }
        }
		
        
	    in_off_sum += src_len;

			//memcpy(out, tmp_buf_ptr, dst_len);
		
			out += dst_len;
			actualLen += dst_len;
		

    }

    //uart_printf("original cpz size = [%08x] \r\n" , (in_ptr - src));
    *cpz_src_len = in_off_sum;
	*dest_len = actualLen;

	 return result;
}

#if 0
int lzop_decompress(lzo_bytep src, lzo_bytep dest)
{
	int result;
	lzo_bytep in=src;
	lzo_bytep out=dest;

	header_t header;
	lzo_uint src_len=0, dst_len=0;
	lzo_uint32 c_adler32 = ADLER32_INIT_VALUE, d_adler32 = ADLER32_INIT_VALUE;
    lzo_uint32 c_crc32 = CRC32_INIT_VALUE, d_crc32 = CRC32_INIT_VALUE;

    result = p_magic(in);
 	if (result > 0)
            return 0;
 	in += read_bytes;
 	
	result = read_header(in,&header);
	if(result != 0)
		return -1;
	in += read_bytes;
	
	for(;;)
	{
	        /* read uncompressed block size */
	        lzo_read32(in,&dst_len);
		in +=4;

		if(dst_len ==0 || dst_len > LZOP_BLOCK_SIZE)
			break;
			
	         /* read compressed block size */
	        lzo_read32(in,&src_len);
		in +=4;
		
	        /* read checksum of uncompressed block */
	        if (header.flags & F_ADLER32_D)
	        {
	       		lzo_read32(in,&d_adler32);
	       		in +=4;
	        }
	        if (header.flags & F_CRC32_D)
	        {
	       		lzo_read32(in,&d_crc32);
			in +=4;
	        }

	       	/* read checksum of compressed block */
	       if (header.flags & F_ADLER32_C)
	       {
	            	if (src_len < dst_len)
	            	{
	            		lzo_read32(in,&c_adler32);
	            		in +=4;
	            	}
	            	else
	           	{
	               		c_adler32 = d_adler32;
	            	}
	        }

	        if (header.flags & F_CRC32_C)
	        {
	           	if (src_len < dst_len)
	           	{
				lzo_read32(in,&c_crc32);
				in +=4;
	            	}
	            	else
	            	{
	                	c_crc32 = d_crc32;
	            	}
	        }

	      
	       result = lzo1x_decompress(in,src_len,out,&dst_len,NULL);
	       out += dst_len;
	       in += src_len;
	}
	return result;
}
#endif

#if 0
UINT32 decompress_image(UINT8 *src, UINT8 *dest)
{
	UINT32 outLen=0;
	int result;

	//DIAG_FILTER(SW_PLAT, DECOMPRESS, DECOMPRESS_IMAGE_START, DIAG_INFORMATION)
	//diagPrintf ("src=0x%lx, dest=0x%lx ",src,dest);
	
	result = lzop_decompress_safe(src, dest, &outLen);	
	
	//DIAG_FILTER(SW_PLAT, DECOMPRESS, DECOMPRESS_IMAGE_END, DIAG_INFORMATION)
	//diagPrintf ("decompress result: result=%d, outLen=%d ",result,outLen);
	
	uart_printf("decompress: from:0x%x, to:0x%x, result=%d, outLen=%d\r\n", (UINT32)src, (UINT32)dest,result,outLen);
	
	if(result != 0)
		return 0;

	
	return outLen;
}
#endif


#if 0
void decompress_test()
{

	UINT8 *dst_dsp = (UINT8 *) 0xD1000000;
	UINT8 *src_dsp = (UINT8 *)0xd1200000;
	UINT8 *dst_ddr = (UINT8 *)0x7e800000;
	UINT32 out_len_1,out_len_2;
	UINT32 start_time, end_time;
	start_time=timerCountRead(TS_TIMER_ID);
	out_len_1 = decompress_image(src_dsp, dst_ddr);
	ASSERT(0);
	memcpy(src_dsp,dst_ddr,out_len_1);
	end_time=timerCountRead(TS_TIMER_ID);
	uart_printf("decompress_test: out_len=%d, start 0x%lx, end 0x%lx\r\n", out_len_1,start_time, end_time);
	
}

lzo_bool lzop_compress(lzo_bytep src, lzo_bytep dest,lzo_uint32p out_len)
{
	FILE *pFile_dst;

    int ret = LZO_E_OK;
    lzo_uint32 src_len = 0;
    lzo_uint dst_len = 0;
    lzo_uint32 c_adler32 = ADLER32_INIT_VALUE, d_adler32 = ADLER32_INIT_VALUE;
    lzo_uint32 c_crc32 = CRC32_INIT_VALUE, d_crc32 = CRC32_INIT_VALUE;
    lzo_int len;

	const lzo_uint32 block_size = BLOCK_SIZE;
	header_t h;
	lzo_uint32 loop_count=0;
	lzo_bytep in_buf=src;
	lzo_bytep out = dest;
	lzo_byte out_buf[BLOCK_SIZE];
	size_t result;

	lzo_init_compress_header(&h, FALSE);
	len  = write_header(dest,&h);
	dest += len;

    for (;;)
    {
    	memset(out_buf, 0,sizeof(out_buf));
        src_len = block_size;

        /* write uncompressed block size */
        lzo_write32(dest,src_len);
		dest += 4;

        /* compute checksum of uncompressed block */
        if (h.flags & F_ADLER32_D)
            d_adler32 = lzo_adler32(ADLER32_INIT_VALUE,in_buf,src_len);
        if (h.flags & F_CRC32_D)
            d_crc32 = lzo_crc32(CRC32_INIT_VALUE,in_buf,src_len);

        /* compress */
        if (h.method == M_LZO1X_1)
            ret = lzo1x_1_compress(in_buf, src_len, out_buf, &dst_len,wrkmem.mb_mem);

        if (ret != LZO_E_OK)
            uart_printf("Internal error - compression failed\r\n");

		
  		loop_count++;

        /* write compressed block size */
        if (dst_len < src_len)
            lzo_write32(dest,(lzo_uint32) dst_len);
        else
            lzo_write32(dest, src_len);

		dest += 4;

        /* write checksum of uncompressed block */
        if (h.flags & F_ADLER32_D)
       	{
            lzo_write32(dest,d_adler32);
			dest += 4;
        }
		if (h.flags & F_CRC32_D)
		{
            lzo_write32(dest,d_crc32);
			dest += 4;
		}
		
        /* write checksum of compressed block */
        if (dst_len < src_len && (h.flags & F_ADLER32_C))
        {
            c_adler32 = lzo_adler32(ADLER32_INIT_VALUE,out_buf,dst_len);
            lzo_write32(dest,c_adler32);
			dest += 4;
        }
        if (dst_len < src_len && (h.flags & F_CRC32_C))
        {
            c_crc32 = lzo_crc32(CRC32_INIT_VALUE,out_buf,dst_len);
            lzo_write32(dest,c_crc32);
			dest += 4;
        }
		
        /* write compressed block data */
        if (dst_len < src_len)
        {
            lzo_write_buf(dest,out_buf,dst_len);
			dest += dst_len;
        }
		else
        {
            lzo_write_buf(dest,out_buf,src_len);
			dest += src_len;
        }
		in_buf += src_len;

		result = fwrite(out, 1, dst_len, pFile_dst);
		if(result != dst_len)
		{
		
			uart_printf("step 7 Writing error\r\n");
			break;
		}
		out += dst_len;
    }
	*out_len = dest -out;
    return ret;
}
#endif


