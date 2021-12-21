#include "plat_config_defs.h"
#include "ui_os_api.h"

#include "ui_log_api.h"
#include "task_cnf.h"
#include "tp_trace.h"
#include "fs.h"

extern  int psram_perf_init(void);
extern void maxcpu_task_init(void);
extern void shell_rxint_init(void);

void ui_os_api_init(void)
{
	static int flag = 0;

	if (1 == flag)
		return;

	flag = 1;

	// do not change the init sequence
	UOS_MutexInit();

	UOS_Semaphore_init();

	UOS_MessageQueueInit ();

	UOS_TaskInit ();

	UOS_InitTimer();

	UOS_FlagInit();
}

extern void hal_task_create(void);
#ifndef CONFIG_RELEASE_WITH_NO_ASR_UI
extern void ui_task_create(void);
#else
#define ui_task_create()
#endif

int ui_main(void)
{	
#ifdef		TP_LOG_GUI_APP
	tp_log_enable(TP_LOG_GUI_APP);
#endif

	ui_os_api_init();
#ifdef DBG_PERIOD_MONITOR
	perf_monitor_task_init();
#endif

#ifdef DBG_CPU_MAX_TEST
	maxcpu_task_init();
#endif
#ifdef CONFIG_SDK_GENERAL_MMI 
#ifdef SUPPORT_CHARGER
	hal_task_create();
#endif
#else
	hal_task_create();
#endif
#ifdef ATA_TEST
	if (!isSysRestartByRdProduction()) {
		ui_task_create();
	}
#else
	ui_task_create();
#endif
	FS_PowerOn();

#ifndef CONFIG_SDK_GENERAL_MMI 
	do_restore_factory();
#endif

#ifdef CONFIG_DEBUG_SHELL
	shell_rxint_init();
#endif

	return 0;
}


enum {
	UI_UDID_NONE,
	UI_UDID_NOUI,
	UI_UDID_NGUX,
	UI_UDID_MAX
};
static int s_ui_udid = 0;
void ui_set_udid(int udid)
{
	if(UI_UDID_MAX > udid)
		s_ui_udid = udid;
}

int ui_get_udid(void)
{
	return s_ui_udid;
}
BOOL __attribute__((weak)) pm_BatteryInit(VOID)
{
	
	return 0;
}
BOOL __attribute__((weak)) pm_BatteryMonitorProc(void)
{
	while(1) {
		UOS_SleepMs(100000);
	}
	return 0;
}
VOID __attribute__((weak)) pm_UsbMonitorProc(VOID)
{
	return;
}
volatile UINT32 __attribute__((weak))  Time_To_MMI_Evt;