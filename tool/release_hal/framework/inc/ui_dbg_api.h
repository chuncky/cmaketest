#ifndef _UI_DBG_API_H_
#define _UI_DBG_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#define HAL_DBG_ASS_DEADLOOP	2

enum {
	UI_ASS_DEBUG_MODE,
	UI_ASS_RELEASE_MODE
};

enum {
	UI_INIT_STATUS_START,
	UI_INIT_STATUS_DONE
};

typedef int UI_INIT_STATUS_E;

typedef struct hal_dbg_flag_t{
	//int log;
	int dbg;
	int ass;
}HAL_DBG_FLAG_T;

extern HAL_DBG_FLAG_T g_hal_dbg_flag;

extern void hal_dbg_enable(void);
extern void hal_dbg_disable(void);
extern int  is_hal_dbg_enable(void);
extern void hal_ass_enable(void);
extern void hal_ass_disable(void);
extern int  is_hal_ass_enable(void);

extern void UI_SendTestPointRequest(UINT32, UINT32);
extern void UI_Assert(const char*, const char*, int, char*);
extern char *UI_SaveAssertInfo (const char *format, ...);
extern uint32 UI_GetAssertMode(uint8 *assert_mode);

extern void UI_InitNvitemParam(void);
extern void UI_SyncNvitemParam(void);
extern uint32 UI_GetAssertMode(uint8 *);
extern uint32 UI_SetAssertMode(uint8  );
extern uint8  UI_GetAssertCounter(void);

extern UI_INIT_STATUS_E UI_GetInitStatus(void);
extern void UI_SetInitStatus(UI_INIT_STATUS_E);
extern void UI_SetInitStatusStart(void);
extern void UI_SetInitStatusDone (void);

extern BOOL hal_IsInIrqContext(void);
extern BOOL hal_IsInTaskContext(void);

extern void usk_stop(void);
extern void usk_handle(uint8*);
extern void usk_test(void);
extern BOOL usk_IsRxInt(void);
extern BOOL usk_IsRxEnd(void);
extern BOOL usk_IsRxOff(void);
extern int  usk_GetRxDataLength(void);
extern void* usk_GetRxData(void);

extern void mcchk_stats_init(void);
extern void mcchk_stats_enable(int type);
extern void mcchk_stats_disable(int type);
extern void* mcchk_stats_get_info(int type);
extern void mcchk_stats_print_status(int type);

#ifdef __cplusplus
}
#endif

#endif // _UI_DBG_API_H_


