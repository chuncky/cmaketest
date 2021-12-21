#ifndef __PLAT_TYPES_H__
#define __PLAT_TYPES_H__

#include "plat_arm_defs.h"

#ifdef CONFIG_SDK_GENERAL_MMI
#include "global_types.h"
#endif
#undef SWAP16
#undef SWAP32
#undef ABS
#undef MAX
#undef MIN
#undef TRUE
#undef FALSE
#undef NULL
#undef PNULL
#undef CONST
#undef SIZEOF
#undef TEXT
#undef NIL
#undef INT_MAX
#undef LONG_MAX
#undef LONG_MIN
#undef ULONG_MAX

#undef INTMAX_MIN
#undef INTMAX_MAX
#undef UINTMAX_MAX
#undef ALIGN
#undef ALIGN_SIZE
#undef offsetof

#define TRUE	(0 == 0)
#define FALSE	(0 == 1)

#define NIL		((void *) 0)
#define NULL	NIL
#define PNULL	NIL
#ifndef CONFIG_SDK_GENERAL_MMI
#define _HAVE_TYPE_BOOL /*for GUI */
#ifndef __cplusplus
typedef unsigned char               bool;
#endif
typedef char                        boolean;
typedef unsigned char				BOOLEAN;
#ifndef __cplusplus
typedef unsigned char				BOOL;
#else
typedef int               BOOL;
#endif
#endif //MMI
typedef unsigned char				u8;
typedef unsigned short				u16;
typedef unsigned char				uint8;
typedef unsigned short				uint16;

typedef unsigned int				u32;
typedef unsigned int				uint;
typedef unsigned int				uint32;

typedef unsigned long long 			u64;
typedef unsigned long long 			uint64;

typedef unsigned int				uint_t;
typedef unsigned char				uint8_t;
typedef unsigned short				uint16_t;
typedef unsigned int				uint32_t;
typedef unsigned long long 			uint64_t;

typedef unsigned char				uint_8;
typedef unsigned short				uint_16;
typedef unsigned int				uint_32;
typedef unsigned long long 			uint_64;

typedef signed char					int8;
typedef signed short				int16;
typedef signed int					int32;
typedef signed int					int_32;
typedef signed long long            int64;

typedef signed char                 int8_t;
typedef signed short                int16_t;
typedef signed int                  int32_t;
typedef signed long long            int64_t;
//typedef long                        intptr_t;

typedef unsigned char               UINT8;
typedef unsigned short              UINT16;
typedef unsigned int                UINT;
#ifndef CONFIG_SDK_GENERAL_MMI
typedef unsigned int                UINT32;
#endif
typedef unsigned long long          UINT64;
typedef int                         INT;
#ifndef CONFIG_SDK_GENERAL_MMI
typedef char                        INT8;
#endif
typedef short                       INT16;
#ifndef CONFIG_SDK_GENERAL_MMI
typedef int                         INT32;
#endif
typedef long long                   INT64;

typedef unsigned char				U8;
typedef unsigned short				U16;
typedef unsigned int                U32;
typedef unsigned long long          U64;

typedef char                        S8;
typedef short                       S16;
typedef int                         S32;
typedef long long                   S64;

typedef char                        s8;
typedef short                       s16;
typedef int                         s32;

typedef unsigned int                size_t;
typedef int                         ssize_t;

typedef char                        ascii;
typedef unsigned char               byte;           /*  unsigned 8-bit data     */
typedef unsigned short              word;           /*  unsigned 16-bit data    */
typedef unsigned long               dword;          /*  unsigned 32-bit data    */

typedef unsigned char               BYTE;
typedef char                        CHAR;
typedef unsigned short              WCHAR;
typedef unsigned short              WORD;
typedef signed int                  WORD32;
typedef unsigned int                UWORD32;
typedef unsigned long               DWORD;

typedef unsigned short      		RESID;
#ifndef CONFIG_SDK_GENERAL_MMI
#ifdef WITH_LONG_LONG
typedef unsigned long long          uintmax_t;
typedef long long                   intmax_t;
#else
typedef unsigned long               uintmax_t;
typedef long                        intmax_t;
#endif
#endif
typedef float                       float32;
typedef float                       FLOAT;
typedef double                      DOUBLE;
typedef UINT32                      HANDLE;
typedef UINT8*                      PUINT8;
typedef UINT32*                     PUINT32;
typedef INT32*                      PINT32;
typedef UINT16*                     PUINT16;
typedef INT16*                      PINT16;
typedef CHAR *                      PCHAR;
typedef void*                       PVOID;
typedef char *                      PS8;
typedef unsigned char *             PU8;
typedef short *                     PS16; 
typedef unsigned short *            PU16;
typedef int *                       PS32;
typedef unsigned int *              PU32;
typedef unsigned short              pBOOL; 

typedef volatile unsigned char      REG8;
typedef volatile unsigned short     REG16;
typedef volatile unsigned int       REG32;

typedef unsigned char           kal_uint8;
typedef signed char             kal_int8;
typedef char                    kal_char;
typedef unsigned short          kal_wchar;

typedef unsigned short int      kal_uint16;
typedef signed short int        kal_int16;

typedef unsigned int            kal_uint32;
typedef signed int              kal_int32;

typedef unsigned int        	HRES;
typedef unsigned int        	HAO;

#ifdef WIN32
   //typedef ULONG64              kal_uint64;
   //typedef LONG64               kal_int64;
   typedef unsigned __int64      kal_uint64;
   typedef __int64               kal_int64;
#else
   typedef unsigned long long   kal_uint64;
   typedef signed long long     kal_int64;
#endif


typedef enum {
   KAL_FALSE,
   KAL_TRUE
} kal_bool;

#define BIT_0               0x00000001
#define BIT_1               0x00000002
#define BIT_2               0x00000004
#define BIT_3               0x00000008
#define BIT_4               0x00000010
#define BIT_5               0x00000020
#define BIT_6               0x00000040
#define BIT_7               0x00000080
#define BIT_8               0x00000100
#define BIT_9               0x00000200
#define BIT_10              0x00000400
#define BIT_11              0x00000800
#define BIT_12              0x00001000
#define BIT_13              0x00002000
#define BIT_14              0x00004000
#define BIT_15              0x00008000
#define BIT_16              0x00010000
#define BIT_17              0x00020000
#define BIT_18              0x00040000
#define BIT_19              0x00080000
#define BIT_20              0x00100000
#define BIT_21              0x00200000
#define BIT_22              0x00400000
#define BIT_23              0x00800000
#define BIT_24              0x01000000
#define BIT_25              0x02000000
#define BIT_26              0x04000000
#define BIT_27              0x08000000
#define BIT_28              0x10000000
#define BIT_29              0x20000000
#define BIT_30              0x40000000
#define BIT_31              0x80000000
#define BIT_(__n)           (1U<<(__n))
#define BIT(__n)            (1U<<(__n))
#define MASK_(__n,__w)      (((1U<<(__w))-1)<<(__n))
#define VALUE_(__n,__v)     ((__v)<<(__n))


#define RND8( _x )       ((((_x) + 7) / 8 ) * 8 ) /*rounds a number up to the nearest multiple of 8 */


#define  UPCASE( _c ) ( ((_c) >= 'a' && (_c) <= 'z') ? ((_c) - 0x20) : (_c) )

#define  DECCHK( _c ) ((_c) >= '0' && (_c) <= '9')

#define  DTMFCHK( _c ) ( ((_c) >= '0' && (_c) <= '9') ||\
                       ((_c) >= 'A' && (_c) <= 'D') ||\
                       ((_c) >= 'a' && (_c) <= 'd') ||\
					   ((_c) == '*') ||\
					   ((_c) == '#'))

#define  HEXCHK( _c ) ( ((_c) >= '0' && (_c) <= '9') ||\
                       ((_c) >= 'A' && (_c) <= 'F') ||\
                       ((_c) >= 'a' && (_c) <= 'f') )

#define  ARR_SIZE( _a )  ( sizeof( (_a) ) / sizeof( (_a[0]) ) )


#ifndef REG_ADDR
#define REG_ADDR(_b_, _o_) ( (uint32)(_b_) + (_o_) )
#endif

#define REG32(x)                                    (*((volatile uint32 *)(x)))
#define REG64(x)                                    (*((volatile uint64 *)(x)))
#define REG16(x)                                    (*((volatile uint16*) (x)))
#define REG8(x)                                     (*((volatile uint8*) (x)))

#define write64(addr, val)                          (*(volatile unsigned long long *)(addr) = (val))
#define read64(addr)                                (*(volatile unsigned long long *)(addr))

#define write32(addr, val)                          (*(volatile unsigned int *)(addr) = (val))
#define read32(addr)                                (*(volatile unsigned int *)(addr))

#define write16(addr, val)                          (*(volatile uint16 *)(addr) = (val))
#define read16(addr)                                (*(volatile uint16 *)(addr))


#define CHIP_REG_OR(reg_addr, value)    (*(volatile uint32 *)(reg_addr) |= (uint32)(value))
#define CHIP_REG_AND(reg_addr, value)   (*(volatile uint32 *)(reg_addr) &= (uint32)(value))
#define CHIP_REG_GET(reg_addr)          (*(volatile uint32 *)(reg_addr))
#define CHIP_REG_SET(reg_addr, value)   (*(volatile uint32 *)(reg_addr)  = (uint32)(value))

#define SET_BIT(reg,bit)    ((reg) |= (1<<(bit)))
#define CLR_BIT(reg,bit)    ((reg) &= ~(1<<(bit)))
#define TST_BIT(a_bit,shift_number)        (((a_bit)>>(shift_number))&1)
#define XOR_BIT(a_bit,shift_number)        ((a_bit) ^= (1<<(shift_number)))

#define DRV_MACRO_START do {
#define DRV_MACRO_END   } while (0)

#define DRV_EMPTY_STATEMENT DRV_MACRO_START DRV_MACRO_END

#define DRV_UNUSED_PARAM( _type_, _name_ ) DRV_MACRO_START      \
    _type_ __tmp1 = (_name_);                                     \
    _type_ __tmp2 = __tmp1;                                       \
    __tmp1 = __tmp2;                                              \
    DRV_MACRO_END

#define MAX_INT8                    127
#define MIN_INT8                    (-MAX_INT8 - 1)
#define MAX_UINT8                   255
#define MIN_UINT8                   0
#define MAX_INT16                   32767
#define MIN_INT16                   (-MAX_INT16 - 1)
#define MAX_UINT16                  65535
#define MIN_UINT16                  0
#define MAX_INT32                   2147483647L
#define MIN_INT32                   (-MAX_INT32 - 1)
#define MAX_UINT32                  4294967295U
#define MIN_UINT32                  0U
#ifdef WITH_LONG_LONG
#define INTMAX_MAX                  0x7fffffffffffffff
#define INTMAX_MIN                  -(0x7fffffffffffffff-1)
#define UINTMAX_MAX                 0xffffffffffffffff
#else
#define INTMAX_MAX                  0x7fffffff
#define INTMAX_MIN                  -(0x7fffffff-1)
#define UINTMAX_MAX                 0xffffffff
#endif
#define INT_MAX                     0x7FFFFFFF
#define LONG_MAX                    0x7fffffff
#define LONG_MIN                    -(0x7fffffff-1)
#define ULONG_MAX                   0xffffffff

#define PUBLIC
#define SRVAPI
#define PROTECTED
#ifndef CONFIG_SDK_GENERAL_MMI
#define LOCAL                       static
#endif
#define PRIVATE                     static
#define EXPORT                      extern
#ifndef CONFIG_SDK_GENERAL_MMI
#define REG                         register
#endif
#define CONST                       const
#define VOLATILE                    volatile
#define VOID                        void
#ifndef CONFIG_SDK_GENERAL_MMI
#define INLINE                      static __inline // Do compiler directives
#endif
#ifdef ENABLE_DEPRECATED
#define DEPRECATED __attribute__ ((deprecated))
#else
#define DEPRECATED
#endif /* ENABLE_DEPRECATED */

#define HNULL                       0
#define NULL_CHAR                   '\0'

#define OFFSETOF(s,m)                   ((UINT32)&(((s *)0)->m)) // Get the address offset of the specified member.
#define ALIGN(val,exp)                  (((val) + ((exp)-1)) & ~((exp)-1))
#define ARRAY_SIZE(a)                   (sizeof(a)/sizeof(a[0]))
#define LAST_ELEMENT(x)                 (&x[ARRAY_SIZE(x)-1])
#define TCHAR_SIZEOF(sz)                (sizeof(sz)/sizeof(TCHAR))
#define BOUND(x, min, max)              ( (x) < (min) ? (min) : ((x) > (max) ? (max):(x)) )
#define ROUND_SIZEOF(t)                 ((sizeof(t)+sizeof(int)-1)&~(sizeof(int)-1))
#define IS_32BIT_ALIGN(nAdd)            ((UINT32)(((UINT32)(nAdd)) & 0x3)==0)
#define IS_16_ALIGN(nSize)              ((UINT32)(((UINT32)(nSize))& 0x0f) == 0)
#define S_LAST_MEMBER_ADDR(ptr, type)   ((UINT8*)(ptr) + SIZEOF(type))
#define S_MEMBER_ADDR(s_prt, offset)    ((UINT8*)(s_prt) + offset)
#define MSB(x)                          (((x) >> 8) & 0xff) // most signif byte of 2-byte integer
#define LSB(x)                          ((x) & 0xff)        // least signif byte of 2-byte integer
#define MAKE_WORD(p)                    (((UINT16)(p)[0] << 8) | (UINT16)(p)[1])
#define MAKE_DWORD(p)                   (((UINT32)(p)[0] << 24) | ((UINT32)(p)[1] << 16) | ((UINT32)(p)[2] << 8) | (UINT32)(p)[3])
#define SWAP16(p)                       (((UINT16)((UINT16*)(p))[1] << 8) | (UINT16)((UINT16*)(p))[0])
#define SWAP32(p)                       (((UINT32)((UINT32*)(p))[3] << 24) | ((UINT32)((UINT32*)(p))[2] << 16) | ((UINT32)((UINT32*)(p))[1] << 8) | (UINT32)((UINT32*)(p))[0])
#define SWAPT(a,b,type)                	{type v; v = a; a = b; b = v; }

/* align size withe the specified bits.*/
#define ALIGN_SIZE(x, align)            (((UINT32)(x)+align-1)&~(align-1))
#define Arg(arg, type)                  (*(type *)(arg))
#define NextArg(arg, type)              ((arg) = (TCHAR *)(arg) + roundedsizeof(type))
// Get the data from "arg" to "in".
#define ARG_IN(r,arg,type)              (memcpy(&(r), (type *) (arg), sizeof (arg)))
// Put the data from "w" to "arg".
#define ARG_OUT(arg, w, type)           (memcpy((type *) (arg), &(w), sizeof (w)))

#define ABS(x) 							((x<0)?(-(x)):(x))
#define MAX(a,b)                        (((a) > (b)) ? (a) : (b))
#define MIN(a,b)                        (((a) < (b)) ? (a) : (b))
#define MAKEINT16(a, b)                 ((INT16)(((UINT8)(a)) | ((INT16)((UINT8)(b))) << 8))
#define MAKEINT32(a, b)                 ((INT32)(((UINT16)(a)) | ((INT32)((UINT16)(b))) << 16))
#define MAKEUINT16(a, b)                ((UINT16)(((UINT8)(a)) | ((UINT16)((UINT8)(b))) << 8))
#define MAKEUINT32(a, b)                ((UINT32)(((UINT16)(a)) | ((UINT32)((UINT16)(b))) << 16))
#define LOINT8(w)                       ((INT8)(w))
#define HIINT8(w)                       ((INT8)((UINT16)(w) >> 8))
#define LOUINT8(w)                      ((UINT8)(w))
#define HIUINT8(w)                      ((UINT8)((UINT16)(w) >> 8))
#define LOINT16(l)                      ((INT16)(l))
#define HIINT16(l)                      ((INT16)((UINT32)(l) >> 16))
#define LOUINT16(l)                     ((UINT16)(l))
#define HIUINT16(l)                     ((UINT16)((UINT32)(l) >> 16))
#define offsetof(TYPE, MEMBER)          ((u32) &((TYPE *)0)->MEMBER)
#define offsetofvar(VAR, MEMBER)        (((u32) &(VAR.MEMBER)) - ((u32) &VAR))
/* Remove const cast-away warnings from gcc -Wcast-qual */
#define __UNCONST(a)                    ((void *)(unsigned long)(const void *)(a))
#define __P(protos)  protos
#define ALIGN(val,exp)                  (((val) + ((exp)-1)) & ~((exp)-1))
#define ARRAY_SIZE(a)                   (sizeof(a)/sizeof(a[0]))
#define LAST_ELEMENT(x)                 (&x[ARRAY_SIZE(x)-1])
#define TCHAR_SIZEOF(sz)                (sizeof(sz)/sizeof(TCHAR))
#define BOUND(x, min, max)              ( (x) < (min) ? (min) : ((x) > (max) ? (max):(x)) )
#define roundedsizeof(t)                ((sizeof(t)+sizeof(int)-1)&~(sizeof(int)-1))
#define IS_32BIT_ALIGN(nAdd)            ((UINT32)(((UINT32)(nAdd)) & 0x3)==0)
#define IS_16_ALIGN(nSize)              ((UINT32)(((UINT32)(nSize))& 0x0f) == 0)

// Preprocessor macros.
#define STRINGIFY_VALUE(s) STRINGIFY(s)
#define STRINGIFY(s) #s

#define SIZEOF(type) sizeof(type)

typedef char* PSTR;
typedef CONST UINT8* PCSTR;
typedef UINT8* PTCHAR;

#if defined(UNICODE)
typedef UINT16 TCHAR;
#define TEXT(x) L ## x
#else
typedef UINT8 TCHAR;
#define TEXT(x) x
#endif

#define TSTXT(x)  x

/// Macro to use in a if statement to tell the compiler this branch
/// is likely taken, and optimize accordingly.
#define LIKELY(x)   __builtin_expect(!!(x), 1)
/// Macro to use in a if statement to tell the compiler this branch
/// is unlikely take, and optimize accordingly.
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

/// For packing structure
#define PACKED      __attribute__((packed))

/// To describe alignment
#define ALIGNED(a)  __attribute__((aligned(a)))

#ifndef __MICRON_ALIGN__
#define MICRON_ALIGN(n) __attribute__ ((aligned (n)))
#endif

/// For possibly unused functions or variables (e.g., debugging stuff)
#define POSSIBLY_UNUSED  __attribute__((unused))

// C++ needs to know that types and declarations are C, not C++.
#ifdef __cplusplus
#define EXTERN_C_START extern "C" {
#define EXTERN_C_END }
#else
#define EXTERN_C_START
#define EXTERN_C_END
#endif

/* Here include platform header files for GUI, normally it is inappropriate,
** But it is the least modification, if here have a suitable chance, need to
** change it and move into proper position.
*/
#include "plat_assert.h"
#include "plat_basic_api.h"

#endif

