#ifndef _UI_OS_FLAG_H_
#define _UI_OS_FLAG_H_


#ifdef __cplusplus
extern "C" {
#endif

#define MAX_FLAG_NUM 64
#define INVALID_FLAG_ID MAX_FLAG_NUM

void UOS_FlagInit(void);

// return the flag's ID
u8 UOS_CreateFlag (void);

void  UOS_DeleteFlag(u8 Id);

//Op = OSA_FLAG_AND/OSA_FLAG_AND_CLEAR/OSA_FLAG_OR/OSA_FLAG_OR_CLEAR
OSA_STATUS UOS_SetFlag(u8 Id, u32 Mask, u32 Op);

OSA_STATUS UOS_WaitFlag (u8 Id, u32 Mask, u32 Op, u32 *Flag, u32 TimeOut);

#ifdef __cplusplus
}
#endif

#endif
