#ifndef _UI_OS_SEMAPHORE_H_
#define _UI_OS_SEMAPHORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define UOS_NB_MAX_SEM             64  /* Number max of semaphores.*/
#define INVALID_SEMAPHORE_ID UOS_NB_MAX_SEM

void UOS_Semaphore_init(void);

//u8 UOS_CreateSemaphore (u8 InitValue)
HANDLE UOS_CreateSemaphore (UINT32	InitValue);

//void UOS_DeleteSemaphore (u8 Id)
BOOL UOS_DeleteSemaphore(HANDLE Id);

//void UOS_WaitForSemaphore (u8 Id)
OSA_STATUS UOS_WaitForSemaphore(HANDLE Id, UINT32 nTimeOut);

//void UOS_ReleaseSemaphore (u8 Id)
OSA_STATUS UOS_ReleaseSemaphore(HANDLE Id);

#ifdef __cplusplus
}
#endif


#endif
