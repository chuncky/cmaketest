#ifndef _UI_OS_MUTEX_H_
#define _UI_OS_MUTEX_H_


#ifdef __cplusplus
extern "C" {
#endif


#define MAX_MUTEX_SIZE	96
#define INVALID_MUTEX_ID	MAX_MUTEX_SIZE

void  UOS_MutexInit (void);

u8    UOS_NewMutex (const char *mutex_name);

// =============================================================================
// UOS_FreeMutex
// -----------------------------------------------------------------------------
/// Free a previously allocated mutex.
/// @param Id mutex Id.
// =============================================================================
void  UOS_FreeMutex (u8 Id);

// =============================================================================
// UOS_TakeMutex
// -----------------------------------------------------------------------------
/// Take the mutex.
/// @param Id mutex Id.
/// @return UserId to give to UOS_ReleaseMutex()
// =============================================================================
OSA_STATUS    UOS_TakeMutex (u8 Id);

OSA_STATUS    UOS_TryTakeMutex (u8 Id);

// =============================================================================
// UOS_ReleaseMutex
// -----------------------------------------------------------------------------
/// Release a mutex.
/// @param Id mutex Id.
/// @param UserId from UOS_TakeMutex()
// =============================================================================
OSA_STATUS  UOS_ReleaseMutex (u8 Id);

#ifdef __cplusplus
}
#endif


#endif
