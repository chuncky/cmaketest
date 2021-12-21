/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/
#ifndef __GUI_CP_INTERNEL_HEADER_FILE__
#error "Please don't include this file directly, please include cp_include.h"
#endif

#ifndef __CP_MEM_API_H__
#define __CP_MEM_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef INTERNAL_SIZE_T
#define INTERNAL_SIZE_T size_t
#endif

#define SIZE_SZ                (sizeof(INTERNAL_SIZE_T))

#ifndef MALLOC_ALIGNMENT
#define MALLOC_ALIGNMENT       (1 * SIZE_SZ)
#endif

/* The corresponding bit mask value */
#define MALLOC_ALIGN_MASK      (MALLOC_ALIGNMENT - 1)

#define SIZEOF_CACHE_LINE               32
#define CACHE_LINE_MASK                 (SIZEOF_CACHE_LINE - 1)
#define INVALIDATE_MEMORY(PmEM,sIZE)

#define SIZEOF_MIN_BUF                  (SIZEOF_CACHE_LINE * 1)     //  Must be SIZEOF_CACHE_LINE * OddNumber
#define MIN_BUF_MASK                    (SIZEOF_MIN_BUF-1)

#define LARGEST_BUF_WITH_SPECIAL_LIST   (SIZEOF_MIN_BUF * 64)       //  Must be SIZEOF_MIN_BUF * n
#define BUF_SIZE(rEQsIZE)               ((rEQsIZE + MIN_BUF_MASK) & ~MIN_BUF_MASK)

#define NUMBER_OF_FREE_LISTS            (1 + (LARGEST_BUF_WITH_SPECIAL_LIST / SIZEOF_MIN_BUF))
#define FREE_LIST_INDEX(sIZE)           ((sIZE <= LARGEST_BUF_WITH_SPECIAL_LIST) ? ((sIZE - 1) / SIZEOF_MIN_BUF) : (NUMBER_OF_FREE_LISTS - 1))
#define LAST_FREE_LIST(PpOOLhEADER)     (&PpOOLhEADER->FreeList[NUMBER_OF_FREE_LISTS - 1])

#define BUF_HDR_GUARD                   2
#define GUARD_PATTERN                   0xDEADBEEF
#define GUARD_TAIL                      0xBABEFACE
#define FREE_MEM                        0x4D454D46
#define FREE_MSK                        0x4B534D46
#define MERG_MEM                        0x4D47454D
#define POOL_ID                         0x48287997          //  This is used for the ICAT EXPORTED FUNCTIONs

#define MAX_NAME_LENGTH                 20

typedef struct MemBufTag
{
    UINT32              Guard;
    UINT32              ReqSize;           //  This is used only for the DATA GUARD CHECK otherwise it can be added to the Header Guard.
    UINT32              UserParam;         //  Free for the user to use.
    void *              pPoolRef;
    struct MemBufTag    *pPrevBuf;         //  The buf that is physicaly befor this buf - not prev free.
    UINT32              AllocCount;
    UINT32              BufSize;
	UINT32              callerAddress;		//Saves the caller address
    struct MemBufTag    *pNextFreeBuf;     //  Next buf on free list. This is also the first address of the allocated buffer.
} OsaMem_BufHdr;

typedef struct MemBlkHdrTag
{
    UINT32                  FirstAddress;
    UINT32                  LastAddress;
    struct MemBlkHdrTag     *pNextMemBlk;
} OsaMem_MemBlkHdr;

typedef struct FreeListTag
{
    struct FreeListTag  *pNextList;
    OsaMem_BufHdr       *pFirstBuf;
    UINT32              TotalAllocReq;     //  Total Alloc request on this memory size.
    UINT32              TotalFreeReq;      //  Total Free  request on this memory size.
    UINT32              MaxAllocBuf;       //  Maximum allocated buffers at the same time = max(TotalAllocReq-TotalFreeReq).
    UINT32              nFreeBuf;          //  Number of free buffers on this list.
    UINT32              maxFreeBuf;        //  Maximum number of free buffers on this list.
} OsaMem_FreeList;

typedef struct PoolHeaderTag
{
    UINT32                      PoolID;        //  Uswd for the ICAT EXPORTED FUNCTIONs
    void *                      CriticalSectionHandle;
    OsaMem_FreeList             FreeList[NUMBER_OF_FREE_LISTS];
    OsaMem_MemBlkHdr            *pFirstMemBlk;
    struct PoolHeaderTag        *pNextPool;
    UINT32                      poolSize;
    UINT32                      BytesInUse;
    UINT32                      MaxBytesInUse;
    UINT32                      BytesAllocated;
    UINT32                      MaxBytesAllocated;
    UINT32                      LowWaterMark;
    void                        (*LowWaterMarkCbFunc)(UINT32);
    char                        poolName[MAX_NAME_LENGTH];
} OsaMem_PoolHeader;

#define     SIZEOF_HDR_OF_ALLOC_BUF         (offsetof(OsaMem_BufHdr,pNextFreeBuf))
#define     ALLOC_BUF_ADRS(PbUFhDR)         ((void *)((UINT32)(PbUFhDR) + SIZEOF_HDR_OF_ALLOC_BUF))
#define     NEXT_BUF_ADRS(PbUFhDR)          ((OsaMem_BufHdr *)((UINT32)ALLOC_BUF_ADRS(PbUFhDR) + PbUFhDR->BufSize))
#define     BUF_DHR_ADRS(PmEM)              ((OsaMem_BufHdr *)((UINT32)(PmEM) - SIZEOF_HDR_OF_ALLOC_BUF))

#define     POOL_HEADER_SIZE                (sizeof(OsaMem_PoolHeader))

typedef enum
{
    eNO_VALIDITY_CHK,
    eMIN_VALIDITY_CHK,
    eEXT_VALIDITY_CHK
} OsaMem_ValiditiyChkE;

typedef struct
{
	void	*poolBase;                     //          Pointer to start of pool memory.
	UINT32	poolSize;                      //          size of the pool.
	char	*name;                         //  [OP]    Pointer to a NULL terminated string.
	BOOL	bSharedForIpc;                 //  [OP]    TRUE - The object can be accessed from all processes.
	UINT32	LowWaterMark;                  //  [OP]    Number of bytes left in pool tp trigger the LowWaterMarkCbFunc.
	void	(*LowWaterMarkCbFunc)(UINT32); //  [OP]    Routine to call when the LowWaterMark is triggered.
}OsaMemCreateParamsT;

#ifdef __cplusplus
}
#endif

#endif /* __CP_TEMPLATE_API_H__ */
