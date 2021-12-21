#ifndef _FS_ASYN_H_
#define _FS_ASYN_H_

#define HAL_SUPPORT_FS_ASYN_TASK	0

// FS asynchronization api error code defining.

// Base
#define ERR_FS_ASYN_BASE                           100000

// Success.
#define ERR_FS_ASYN_SUCCESS                     0

// Parameter is invalid.
#define ERR_FS_ASYN_PARA_INVALID            ERR_FS_ASYN_BASE + 1

// Malloc failed.
#define ERR_FS_ASYN_MALLOC_FAILED         ERR_FS_ASYN_BASE + 2

// Send event failed.
#define ERR_FS_ASYN_SENDEVENT_FAILED   ERR_FS_ASYN_BASE + 3 

// fs asyn task not start.
#define ERR_FS_ASYN_TASK_NOT_START      ERR_FS_ASYN_BASE + 4 
// Event Id define.

// Base.
#define FS_ASYN_EVENT_ID_BASE     0x1000

// Read request.
#define FS_ASYN_EVENT_ID_READ_REQ       (FS_ASYN_EVENT_ID_BASE + 1)

// Write request.
#define FS_ASYN_EVENT_ID_WRITE_REQ      (FS_ASYN_EVENT_ID_BASE + 2)

// Callback function define.
typedef VOID (*FS_ASNY_FUNCPTR)(void*); 

// fs asyn read requst stucture.
typedef struct _fs_asyn_read_req
{
  INT32 iFd;                                      // File desciption,received call the function FS_Open.
  UINT8* pBuff;                                // Point to a buffer of to read.
  UINT32 uSize;                               // the size of to read.
  FS_ASNY_FUNCPTR pCallback;          // call back fuction, it shall been called when completed reading.
}FS_ASYN_READ_REQ;   

// fs asyn write requst stucture.
typedef struct _fs_asyn_write_req
{
  INT32 iFd;                                     // File desciption,received call the function FS_Open.
  UINT8* pBuff;                               // Point to a buffer of to write.
  UINT32 uSize;                              // the size of to write.
  FS_ASNY_FUNCPTR pCallback;       // call back fuction, it shall been called when completed writing.
}FS_ASYN_WRITE_REQ;

// fs asyn read result structure. 
typedef struct _fs_asyn_read_result
{  
    UINT32   uSize;		                 // size of really readed.
    UINT8*   pBuff;                        // point to the buffer of to read.
    INT32 	 iResult;                       // read option process result.if success, it is zero,else,it is the return value of FS_Read.
} FS_ASYN_READ_RESULT;

// fs asyn write result structure. 
typedef struct _fs_asyn_write_result
{  
    UINT32   uSize;		                // size of really writed.
    UINT8*   pBuff;                        // point to the buffer of to write.
    INT32 	 iResult;                        // write option process result.if success, it is zero,else,it is the error code  of FS_Write.
} FS_ASYN_WRITE_RESULT;

//fs asyn task entry.
VOID HAL_AsynFsTask(void *pData);
INT32 FS_AsynReadReq(INT32 iFd,UINT8* pBuff,UINT32 uSize,FS_ASNY_FUNCPTR pCallback);
INT32 FS_AsynWriteReq(INT32 iFd,UINT8* pBuff,UINT32 uSize,FS_ASNY_FUNCPTR pCallback);

VOID HAL_AsynFsTask(void *pData);

extern HANDLE g_hHalAsynFsTask;


#endif
