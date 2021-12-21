/* minilzo.c -- mini subset of the LZO real-time data compression library

   This file is part of the LZO real-time data compression library.

   Copyright (C) 2011 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 2010 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 2009 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 2008 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 2007 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 2006 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 2005 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 2004 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 2003 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 2002 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 2001 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 2000 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 1999 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 1998 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 1997 Markus Franz Xaver Johannes Oberhumer
   Copyright (C) 1996 Markus Franz Xaver Johannes Oberhumer
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

/*
 * NOTE:
 *   the full LZO package can be found at
 *   http://www.oberhumer.com/opensource/lzo/
 */

#define __LZO_IN_MINILZO 1

#include "minilzo.h"
#include "lzoconf.h"


#define LZO1X           1
#define LZO_EOF_CODE    1
#define M2_MAX_OFFSET   0x0800
#define pd(a,b)        ((lzo_uint)((a)-(b)))



int lzo1x_decompress ( const lzo_bytep in , lzo_uint  in_len,
                       lzo_bytep out, lzo_uintp out_len,
                       lzo_voidp wrkmem )

{
    lzo_bytep op;
    const lzo_bytep ip;
    lzo_uint t;
    const lzo_bytep m_pos;
    const lzo_bytep const ip_end = in + in_len;


    *out_len = 0;

    op = out;
    ip = in;

    if (*ip > 17)
    {
        t = *ip++ - 17;
        if (t < 4)
            goto match_next;

        do 
		{
			*op++ = *ip++;
		}while (--t > 0);
        goto first_literal_run;
    }

    for (;;)
    {
        t = *ip++;
        if (t >= 16)
            goto match;
        if (t == 0)
        {
            while (*ip == 0)
            {
                t += 255;
                ip++;
            }
            t += 15 + *ip++;
        }

        {
            *op++ = *ip++; 
			*op++ = *ip++; 
			*op++ = *ip++;
            do 
            {
				*op++ = *ip++; 
			}while (--t > 0);
        }


first_literal_run:

        t = *ip++;
        if (t >= 16)
            goto match;


        m_pos = op - (1 + M2_MAX_OFFSET);
        m_pos -= t >> 2;
        m_pos -= *ip++ << 2;

        *op++ = *m_pos++; 
		*op++ = *m_pos++; 
		*op++ = *m_pos;

        goto match_done;

        for (;;) {
match:
            if (t >= 64)
            {

                m_pos = op - 1;
                m_pos -= (t >> 2) & 7;
                m_pos -= *ip++ << 3;
                t = (t >> 5) - 1;

                goto copy_match;
            }
            else if (t >= 32)
            {
                t &= 31;
                if (t == 0)
                {
                    while (*ip == 0)
                    {
                        t += 255;
                        ip++;
                    }
                    t += 31 + *ip++;
                }

                m_pos = op - 1;
                m_pos -= (ip[0] >> 2) + (ip[1] << 6);

                ip += 2;
            }
            else if (t >= 16)
            {
                m_pos = op;
                m_pos -= (t & 8) << 11;

                t &= 7;
                if (t == 0)
                {
                    while (*ip == 0)
                    {
                        t += 255;
                        ip++;
                    }
                    t += 7 + *ip++;
                }

                m_pos -= (ip[0] >> 2) + (ip[1] << 6);

                ip += 2;
                if (m_pos == op)
                    goto eof_found;
                m_pos -= 0x4000;

            }
            else
            {
                m_pos = op - 1;
                m_pos -= t >> 2;
                m_pos -= *ip++ << 2;

                *op++ = *m_pos++; *op++ = *m_pos;
                goto match_done;
            }


            {
copy_match:
                *op++ = *m_pos++; 
				*op++ = *m_pos++;
                do 
				{
					*op++ = *m_pos++; 
				}while (--t > 0);
            }


match_done:
            t = ip[-2] & 3;

            if (t == 0)
                break;

match_next:

            *op++ = *ip++;
            if (t > 1) 
			{ 
				*op++ = *ip++; 
				if (t > 2) 
				{ 
					*op++ = *ip++; 
				} 
			}

            t = *ip++;
        }
    }

eof_found:
    *out_len = pd(op, out);
    return (ip == ip_end ? LZO_E_OK :
           (ip < ip_end  ? LZO_E_INPUT_NOT_CONSUMED : LZO_E_INPUT_OVERRUN));

}



#define LZO_NEED_DICT_H 1

#define D_BITS          14

#define LZO_SIZE(bits)      (1u << (bits))
#define LZO_MASK(bits)      (LZO_SIZE(bits) - 1)
#define D_HIGH          ((D_MASK >> 1) + 1)
#define D_MASK        LZO_MASK(D_BITS)

#define DMUL(a,b) ((lzo_uint) ((a) * (b)))

#define DX2(p,s1,s2) \
        (((((lzo_uint)((p)[2]) << (s2)) ^ (p)[1]) << (s1)) ^ (p)[0])
#define DX3(p,s1,s2,s3) ((DX2((p)+1,s2,s3) << (s1)) ^ (p)[0])
#define DMS(v,s)        ((lzo_uint) (((v) & (D_MASK >> (s))) << (s)))
#define DM(v)           DMS(v,0)

#define D_INDEX1(d,p)       d = DM(DMUL(0x21,DX3(p,5,5,6)) >> 5)
#define D_INDEX2(d,p)       d = (d & (D_MASK & 0x7ff)) ^ (D_HIGH | 0x1f)
#define DINDEX(dv,p)        DM(((DMUL(0x1824429d,dv)) >> (32-D_BITS)))

#define DINDEX1             D_INDEX1
#define DINDEX2             D_INDEX2


#define LZO_DICT_USE_PTR	0

#define PTR(a)              ((lzo_uintp) (a))

#define PTR_LT(a,b)         (PTR(a) < PTR(b))
#define PTR_GE(a,b)         (PTR(a) >= PTR(b))
#define PTR_DIFF(a,b)       (PTR(a) - PTR(b))

#if (LZO_DICT_USE_PTR)
#define DENTRY(p,in)                          (p)
#define GINDEX(m_pos,m_off,dict,dindex,in)    m_pos = dict[dindex]

#define LZO_CHECK_MPOS_DET(m_pos,m_off,in,ip,max_offset) \
        (m_pos == NULL || (m_off = pd(ip, m_pos)) > max_offset)

#define LZO_CHECK_MPOS_NON_DET(m_pos,m_off,in,ip,max_offset) \
    ( \
        m_pos = ip - (lzo_uint) PTR_DIFF(ip,m_pos), \
        PTR_LT(m_pos,in) || \
        (m_off = (lzo_uint) PTR_DIFF(ip,m_pos)) == 0 || \
         m_off > max_offset )
         
#define lzo_dict_t    const lzo_bytep
#define lzo_dict_p    lzo_dict_t *

#else
#define DENTRY(p,in)                          ((lzo_dict_t) pd(p, in))
#define GINDEX(m_pos,m_off,dict,dindex,in)    m_off = dict[dindex]

#define LZO_CHECK_MPOS_DET(m_pos,m_off,in,ip,max_offset) \
        (m_off == 0 || \
         ((m_off = pd(ip, in) - m_off) > max_offset) || \
         (m_pos = (ip) - (m_off), 0) )

#define LZO_CHECK_MPOS_NON_DET(m_pos,m_off,in,ip,max_offset) \
        (pd(ip, in) <= m_off || \
         ((m_off = pd(ip, in) - m_off) > max_offset) || \
         (m_pos = (ip) - (m_off), 0) )
#define lzo_dict_t    lzo_uint
#define lzo_dict_p    lzo_dict_t *

#endif


#define UPDATE_I(dict,drun,index,p,in)    dict[index] = DENTRY(p,in)



#define LZO_BYTE(x)       ((unsigned char) (x))

#define LZO_MEMOPS_SET1(dd,cc) \
    do{ \
    	volatile unsigned char *d__1 = ( volatile unsigned char *) (void*) (dd); \
    	d__1[0] = LZO_BYTE(cc); \
    }while(0)

#define LZO_MEMOPS_SET2(dd,cc) \
		do{ \
			volatile unsigned char *d__2 = (volatile unsigned char *) (void*) (dd); \
			d__2[0] = LZO_BYTE(cc); d__2[1] = LZO_BYTE(cc); \
		}while(0)

#define LZO_MEMOPS_MOVE4(dd,ss) \
			do{  \
				volatile unsigned char *d__4 = (volatile unsigned char *) (void*) (dd); \
				const volatile unsigned char *s__4 = (const volatile unsigned char *) (const void*) (ss); \
				d__4[0] = s__4[0]; d__4[1] = s__4[1]; d__4[2] = s__4[2]; d__4[3] = s__4[3]; \
			}while(0)
			
#define LZO_MEMOPS_MOVE8(dd,ss) \
			do{  \
			volatile unsigned char *d__8 = (volatile unsigned char *) (void*) (dd); \
			const volatile unsigned char *s__8 = (const volatile unsigned char *) (const void*) (ss); \
			d__8[0] = s__8[0]; d__8[1] = s__8[1]; d__8[2] = s__8[2]; d__8[3] = s__8[3]; \
			d__8[4] = s__8[4]; d__8[5] = s__8[5]; d__8[6] = s__8[6]; d__8[7] = s__8[7]; \
			}while(0)

#define LZO_MEMOPS_GET_LE32(ss)    (* (const volatile lzo_uint32*) (const void*) (ss))
#define UA_GET_LE32         LZO_MEMOPS_GET_LE32

#define UA_SET1             LZO_MEMOPS_SET1


#define LZO_MEMOPS_COPY4(dd,ss) LZO_MEMOPS_MOVE4(dd,ss)   
#define LZO_MEMOPS_COPY8(dd,ss) LZO_MEMOPS_MOVE8(dd,ss)

#define LZO_MEMOPS_COPYN(dd,ss,nn) \
    do { \
	    volatile unsigned char * d__n = (volatile unsigned char *) (void*) (dd); \
	    const volatile unsigned char * s__n = (const volatile unsigned char *) (const void*) (ss); \
	    lzo_uint n__n = (nn); \
	    while ((void)0, n__n >= 8) { LZO_MEMOPS_COPY8(d__n, s__n); d__n += 8; s__n += 8; n__n -= 8; } \
	    if ((void)0, n__n >= 4) { LZO_MEMOPS_COPY4(d__n, s__n); d__n += 4; s__n += 4; n__n -= 4; } \
	    if ((void)0, n__n > 0) do { *d__n++ = *s__n++; } while (--n__n > 0); \
    }while(0)

#ifndef __LZO_CONFIG1X_H
#define __LZO_CONFIG1X_H 1

#define LZO1X 1


#ifndef LZO_EOF_CODE
#define LZO_EOF_CODE 1
#endif

#define M1_MAX_OFFSET   0x0400
#ifndef M2_MAX_OFFSET
#define M2_MAX_OFFSET   0x0800
#endif
#define M3_MAX_OFFSET   0x4000
#define M4_MAX_OFFSET   0xbfff

#define MX_MAX_OFFSET   (M1_MAX_OFFSET + M2_MAX_OFFSET)

#define M1_MIN_LEN      2
#define M1_MAX_LEN      2
#define M2_MIN_LEN      3
#ifndef M2_MAX_LEN
#define M2_MAX_LEN      8
#endif
#define M3_MIN_LEN      3
#define M3_MAX_LEN      33
#define M4_MIN_LEN      3
#define M4_MAX_LEN      9

#define M1_MARKER       0
#define M2_MARKER       64
#define M3_MARKER       32
#define M4_MARKER       16

#ifndef MIN_LOOKAHEAD
#define MIN_LOOKAHEAD       (M2_MAX_LEN + 1)
#endif


#endif

static lzo_uint do_compress ( const lzo_bytep in , lzo_uint  in_len,
                    lzo_bytep out, lzo_uintp out_len,
                    lzo_uint  ti,  lzo_voidp wrkmem)
{
    const lzo_bytep ip;
    lzo_bytep op;
    const lzo_bytep const in_end = in + in_len;
    const lzo_bytep const ip_end = in + in_len - 20;
    const lzo_bytep ii;
    lzo_dict_p const dict = (lzo_dict_p) wrkmem;

    op = out;
    ip = in;
    ii = ip;

    ip += ti < 4 ? 4 - ti : 0;
    for (;;)
    {
        const lzo_bytep m_pos;

        lzo_uint m_off;
        lzo_uint m_len;
        {
        lzo_uint32 dv;
        lzo_uint dindex;
literal:
        ip += 1 + ((ip - ii) >> 5);
next:
        if(ip >= ip_end)
            break;

		dv = UA_GET_LE32(ip);
		#if 1
        dindex = DINDEX(dv,ip);
        GINDEX(m_off,m_pos,in+dict,dindex,in);
        UPDATE_I(dict,0,dindex,ip,in);
		#else
		m_off = ip -in;
		m_pos = ip;
		#endif
        if(dv != UA_GET_LE32(m_pos))
            goto literal;
        }


        ii -= ti; ti = 0;
        {
        lzo_uint t = pd(ip,ii);
        if (t != 0)
        {
            if (t <= 3)
            {
                op[-2] = LZO_BYTE(op[-2] | t);

                { do *op++ = *ii++; while (--t > 0); }

            }
            else
            {
                if (t <= 18)
                    *op++ = LZO_BYTE(t - 3);
                else
                {
                    lzo_uint tt = t - 18;
                    *op++ = 0;
                    while(tt > 255)
                    {
                        tt -= 255;
                        UA_SET1(op, 0);
                        op++;
                    }
                  //  assert(tt > 0);
                    *op++ = LZO_BYTE(tt);
                }
                { do *op++ = *ii++; while (--t > 0); }
            }
        }
        }
        m_len = 4;
        {

        	if(ip[m_len] == m_pos[m_len]) {
            do {
                m_len += 1;
                if (ip[m_len] != m_pos[m_len])
                    break;
                m_len += 1;
                if (ip[m_len] != m_pos[m_len])
                    break;
                m_len += 1;
                if (ip[m_len] != m_pos[m_len])
                    break;
                m_len += 1;
                if (ip[m_len] != m_pos[m_len])
                    break;
                m_len += 1;
                if (ip[m_len] != m_pos[m_len])
                    break;
                m_len += 1;
                if (ip[m_len] != m_pos[m_len])
                    break;
                m_len += 1;
                if (ip[m_len] != m_pos[m_len])
                    break;
                m_len += 1;
                if(ip + m_len >= ip_end)
                    goto m_len_done;
            } while (ip[m_len] == m_pos[m_len]);
        }

        }
m_len_done:
        m_off = pd(ip,m_pos);
        ip += m_len;
        ii = ip;
        if (m_len <= M2_MAX_LEN && m_off <= M2_MAX_OFFSET)
        {
            m_off -= 1;

            *op++ = LZO_BYTE(((m_len - 1) << 5) | ((m_off & 7) << 2));
            *op++ = LZO_BYTE(m_off >> 3);

        }
        else if (m_off <= M3_MAX_OFFSET)
        {
            m_off -= 1;
            if (m_len <= M3_MAX_LEN)
                *op++ = LZO_BYTE(M3_MARKER | (m_len - 2));
            else
            {
                m_len -= M3_MAX_LEN;
                *op++ = M3_MARKER | 0;
                while(m_len > 255)
                {
                    m_len -= 255;
                    UA_SET1(op, 0);
                    op++;
                }
                *op++ = LZO_BYTE(m_len);
            }
            *op++ = LZO_BYTE(m_off << 2);
            *op++ = LZO_BYTE(m_off >> 6);
        }
        else
        {
            m_off -= 0x4000;
            if (m_len <= M4_MAX_LEN)
                *op++ = LZO_BYTE(M4_MARKER | ((m_off >> 11) & 8) | (m_len - 2));
            else
            {
                m_len -= M4_MAX_LEN;
                *op++ = LZO_BYTE(M4_MARKER | ((m_off >> 11) & 8));
                while(m_len > 255)
                {
                    m_len -= 255;
                    UA_SET1(op, 0);
                    op++;
                }
                *op++ = LZO_BYTE(m_len);
            }
            *op++ = LZO_BYTE(m_off << 2);
            *op++ = LZO_BYTE(m_off >> 6);
        }
        goto next;
    }

    *out_len = pd(op, out);
    return pd(in_end,ii-ti);
}


int lzo1x_1_compress( const lzo_bytep in , lzo_uint  in_len, lzo_bytep out, lzo_uintp out_len,lzo_voidp wrkmem )
{
    const lzo_bytep src = in;
    lzo_bytep dest = out;
    lzo_uint src_len = in_len;
    lzo_uint t = 0;

    while (src_len > 20)
    {
        lzo_uint ll = src_len;
        lzo_uintp ll_end;
	//	memset(wrkmem, 0, ((lzo_uint)1 << D_BITS) * sizeof(lzo_dict_t));
        ll_end = (lzo_uintp)src + ll;
        if ((ll_end + ((t + ll) >> 5)) <= ll_end || (const lzo_bytep)(ll_end + ((t + ll) >> 5)) <= src + ll)
            break;

        t = do_compress(src,ll,dest,out_len,t,wrkmem);
        src += ll;
		
        dest += *out_len;
        src_len  -= ll;
    }
    t += src_len;

    if (t > 0)
    {
        const lzo_bytep tmp_in_buf = in + in_len - t;

        if (dest == out && t <= 238)
            *dest++ = LZO_BYTE(17 + t);
        else if (t <= 3)
            dest[-2] = LZO_BYTE(dest[-2] | t);
        else if (t <= 18)
            *dest++ = LZO_BYTE(t - 3);
        else
        {
            lzo_uint tt = t - 18;

            *dest++ = 0;
            while (tt > 255)
            {
                tt -= 255;
                UA_SET1(dest, 0);
                dest++;
            }
          //  assert(tt > 0);
            *dest++ = LZO_BYTE(tt);
        }
        memcpy(dest, tmp_in_buf, t);//shashal
        dest += t;
    }

    *dest++ = M4_MARKER | 1;
    *dest++ = 0;
    *dest++ = 0;

    *out_len = pd(dest, out);
    return LZO_E_OK;
}


/***** End of minilzo.c *****/

