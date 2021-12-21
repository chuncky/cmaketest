#include "lcd_predefine.h"
#include "lcdd_esd.h"

#ifdef LCD_ESD_SUPPORT
#include "../../chip/lcd/panel_drv.h"
#include "ui_os_api.h"
#include "ui_mem_api.h"
#include "lcd_drv.h"
#include "lcd_common.h"
#include "task_cnf.h"

#ifdef LCD_INTERRUPT_MODE
extern u8	g_lcd_interrupt_flag;
extern int g_lcd_irq_waiter;
#endif

//#define LCD_ESD_TE

/*unit: S*/
#define ESD_INTERVAL_TIME 1

enum{
	ESD_MSG_TYPE_SYNC,
	ESD_MSG_TYPE_LIMIT
};

#ifdef LCD_ESD_TE
static int do_est_check_te(struct s_lcd_esd* esd_ctx)
{
	int i, te_count = 5;
	int ret = 0;
	uint32_t actual_flags = 0;

	LCDLOGI("INFO: do_est_check_te +++\r\n");
	lcd_enable_te_irq(esd_ctx->lcd, 1);
	g_lcd_irq_waiter ++;
	while(te_count > 0) {
		ret = UOS_WaitFlag(g_lcd_interrupt_flag, LCD_INTER_FLAG_TEDONE, OSA_FLAG_OR_CLEAR,
			&actual_flags, MS_TO_TICKS(20));
		if ((ret == 0) && (0 != (actual_flags & LCD_INTER_FLAG_TEDONE))){
			LCDLOGD("DBG: do_est_check_te: got one te irq!\r\n");
			ret = 0;
			break;
		}else{
			LCDLOGW("Warning: do_est_check_te:wait for te (0x%x)(%d) timeout!\r\n", actual_flags, i);
			ret = -1;
		}
		te_count--;
	}
	g_lcd_irq_waiter --;
	lcd_enable_te_irq(esd_ctx->lcd, 0);
	LCDLOGI("INFO: do_est_check_te ---\r\n");
	return ret;
}
#endif

static int do_esd_check_readpower(struct s_lcd_esd* esd_ctx)
{
	struct panel_spec* panel = esd_ctx->panel;
	int ret;

	LCDLOGI("INFO: do_esd_check_readpower +++\r\n");

	if(INVALID_MUTEX_ID != esd_ctx->esd_mutex){
		LCDLOGI("INFO: Jessica: read power take esd mutex\r\n");
		UOS_TakeMutex(esd_ctx->esd_mutex);
	}

	if(NULL != panel->ops->panel_checkesd)
		ret = panel->ops->panel_checkesd(panel);

	if(INVALID_MUTEX_ID != esd_ctx->esd_mutex){
		LCDLOGI("INFO: Jessica: read power release esd mutex\r\n");
		UOS_ReleaseMutex(esd_ctx->esd_mutex);
	}

	if(0 != ret){
		LCDLOGE("ERR: check esd fail!\r\n");
	} else {
		LCDLOGD("DBG: check esd OK!\r\n");
	}

	LCDLOGI("INFO: do_esd_check_readpower ---\r\n");
	return ret;
}

static int do_esd_check(struct s_lcd_esd* esd_ctx)
{
#ifdef LCD_ESD_TE
	LCDLOGD("DBG: do_esd_check: panel->cap : %d!\r\n",esd_ctx->panel->cap);
	if(0 == (esd_ctx->panel->cap & LCD_CAP_NOTE)){
		return  do_est_check_te(esd_ctx);
	}
#endif
	return do_esd_check_readpower(esd_ctx);
}

static int do_esd_reset(struct s_lcd_esd* esd_ctx)
{
	int ret = 0;
	LCDLOGI("INFO: do_esd_reset +++\r\n");

	if(INVALID_MUTEX_ID != esd_ctx->esd_mutex){
		LCDLOGI("INFO: esd reset take esd mutex +++\r\n");
		UOS_TakeMutex(esd_ctx->esd_mutex);
	}

	ret = panel_esd_reset(esd_ctx->panel);
	if(INVALID_MUTEX_ID != esd_ctx->esd_mutex){
		LCDLOGI("INFO: esd reset release esd mutex +++\r\n");
		UOS_ReleaseMutex(esd_ctx->esd_mutex);
	}

	if(0 != ret){
		LCDLOGE("ERROR: do_esd_reset: reset fail\r\n");
	}

	LCDLOGI("INFO: do_esd_reset ---\r\n");
	return 0;
}

static void lcd_esd_handler(void* argv)
{
	struct s_lcd_esd* esd_ctx = (struct s_lcd_esd*)argv;
	uint8_t ret;
	uint32_t wait_time;
	unsigned int event[4];
	void* pmsg;

	LCDLOGI("INFO: lcd_esd_handler +++(0x%x)\r\n", esd_ctx);
	if(NULL == esd_ctx){
		LCDLOGE("ERROR: lcd_esd_handler: Invalid param!\r\n");
		return;
	}

	while(0 == esd_ctx->esd_need_exit){		
		LCDLOGD("DBG: lcd_esd_handler: esd_need_exit:%d\r\n", esd_ctx->esd_need_exit);
		if(LCD_ESD_STATUS_START == esd_ctx->esd_status)		
			wait_time = MS_TO_TICKS(ESD_INTERVAL_TIME * 1000);
		else
			wait_time = OSA_SUSPEND;

		pmsg = UOS_WaitMsg(event, esd_ctx->esd_message, wait_time);
		LCDLOGD("DBG: lcd_esd_handler:Got one message or wait message timeout!\r\n");

		if((NULL != pmsg) && (event[0] != ESD_MSG_TYPE_SYNC)){
			LCDLOGE("ERROR: lcd_esd_handler: Invalid event (0x%x)!\r\n", event[0]);
			continue;
		}

		if(LCD_ESD_STATUS_STOP == esd_ctx->esd_status){
			LCDLOGD("DBG: lcd_esd_handler:esd_status is stopped!! Do nothing!\r\n");
			continue;
		}

		if(0 !=  esd_ctx->esd_need_exit){
			LCDLOGD("DBG: lcd_esd_handler: esd task need to exit!\r\n");
			break;
		}

		if(0 != do_esd_check(esd_ctx)){
			LCDLOGI("INFO: lcd_esd_handler: do_esd_check fail, need reset!\r\n");
			if(0 != esd_ctx->esd_need_exit){
				LCDLOGD("DBG: lcd_esd_handler: esd task need to exit!!\r\n");
				break;
			}
			if(LCD_ESD_STATUS_STOP == esd_ctx->esd_status){
				LCDLOGD("DBG: lcd_esd_handler:esd_status is stopped!! Do nothing!!\r\n");
				continue;
			}

			if(0 != do_esd_reset(esd_ctx)){
				LCDLOGE("ERROR: lcd_esd_handler: do_esd_reset fail!\r\n");			
			} else {
				LCDLOGI("INFO: lcd_esd_handler: do_esd_reset OK!\r\n");			
			}
		} else {
			LCDLOGD("DBG: lcd_esd_handler: esd check OK!\r\n");
		}
		
	}

	LCDLOGD("DBG: lcd_esd_handler: esd task exit\r\n");
	LCDLOGD("DBG: lcd_esd_handler: esd_need_exit:%d\r\n", esd_ctx->esd_need_exit);
	ret = UOS_SetFlag(esd_ctx->esd_flag, 0x1, OSA_FLAG_OR);
	if(0 != ret){
		LCDLOGE("ERROR: lcd_esd_handler: flag set error\r\n");
	}
	LCDLOGI("INFO: lcd_esd_handler ---\r\n");
}

struct s_lcd_esd* lcd_esd_init(struct s_lcd_ctx* lcd, struct panel_spec* panel)
{
	struct s_lcd_esd* esd_ctx = NULL;

	LCDLOGI("INFO: lcd_esd_init +++\r\n");

	if((NULL == panel) || (NULL == lcd)){
		LCDLOGE("ERROR: lcd_esd_init: invalid param!\r\n");
		return NULL;
	}

	esd_ctx = UOS_MALLOC(sizeof(struct s_lcd_esd));
	if(NULL == esd_ctx){
		LCDLOGE("ERROR: lcd_esd_init: malloc buffer fail!\r\n");
		return NULL;
	}

	esd_ctx->panel = panel;
	esd_ctx->lcd = lcd;
	esd_ctx->esd_status = LCD_ESD_STATUS_STOP;
	esd_ctx->esd_need_exit = 0;

	esd_ctx->esd_mutex = UOS_NewMutex("lcd_esd_mutex");
	if(INVALID_MUTEX_ID == esd_ctx->esd_mutex){
		LCDLOGE("ERROR: lcd_esd_init: get mutex fail!\r\n");
		UOS_FREE(esd_ctx);
		esd_ctx = NULL;
		return NULL;
	}

	esd_ctx->esd_message = UOS_NewMessageQueue("LcdEsdMsgQue", TASK_DEFAULT_MSGQ_SIZE);
	esd_ctx->esd_flag = UOS_CreateFlag();

	LCDLOGD("DBG: lcd_esd_init: create task with 0x%x!\r\n", esd_ctx);
	esd_ctx->esd_task = UOS_CreateTask(lcd_esd_handler, esd_ctx, TASK_WITHOUT_MSGQ, LCD_TASK_SIZE, LCD_ESD_TASK_PRIORITY, "uiLcdEsdThd");
	if(NULL == esd_ctx->esd_task){
		LCDLOGE("ERROR: lcd_esd_init: create task fail!\r\n");
		UOS_FreeMutex(esd_ctx->esd_mutex);
		UOS_DeleteFlag(esd_ctx->esd_flag);
		UOS_FreeMessageQueue(esd_ctx->esd_message);
		UOS_FREE(esd_ctx);
		esd_ctx = NULL;
		return NULL;
	}
	
	LCDLOGI("INFO: lcd_esd_init ---\r\n");
	return esd_ctx;
}

int lcd_esd_start(struct s_lcd_esd* esd_ctx)
{
	unsigned int event[4];

	LCDLOGI("INFO: lcd_esd_start +++\r\n");
	if(NULL == esd_ctx){
		LCDLOGE("ERROR: lcd_esd_start: Invalid param!\r\n");
		return -1;
	}

	if(LCD_ESD_STATUS_START == esd_ctx->esd_status){
		LCDLOGW("Warning: lcd_esd_start: esd has been started!!\r\n");
		return 0;
	}

	esd_ctx->esd_status = LCD_ESD_STATUS_START;
	event[0] = ESD_MSG_TYPE_SYNC;
 	UOS_SendMsg((void*)event, esd_ctx->esd_message, UOS_SEND_EVT);

	LCDLOGI("INFO: lcd_esd_start ---\r\n");
	return 0;
}

int lcd_esd_stop(struct s_lcd_esd* esd_ctx)
{
	unsigned int event[4];

	LCDLOGI("INFO: lcd_esd_stop +++\n");
	if(NULL == esd_ctx){
		LCDLOGE("ERROR: lcd_esd_stop: Invalid param!\n");
		return -1;
	}

	if(LCD_ESD_STATUS_STOP == esd_ctx->esd_status){
		LCDLOGW("WARNING: lcd_esd_stop: esd has been stopped!!\n");
		return 0;
	}

	esd_ctx->esd_status = LCD_ESD_STATUS_STOP;
	event[0] = ESD_MSG_TYPE_SYNC;
 	UOS_SendMsg((void*)event, esd_ctx->esd_message, UOS_SEND_EVT);

	LCDLOGI("INFO: lcd_esd_stop ---\n");
	return 0;
}

int lcd_esd_uninit(struct s_lcd_esd* esd_ctx)
{
	uint8_t ret;
    uint32_t   actual_flags = 0;
	unsigned int event[4];

	LCDLOGI("INFO: lcd_esd_uninit +++\n");
	if(NULL == esd_ctx){
		LCDLOGE("ERROR: lcd_esd_uninit: Invalid param!\n");
		return -1;
	}

	esd_ctx->esd_need_exit = 1;

	event[0] = ESD_MSG_TYPE_SYNC;
 	UOS_SendMsg((void*)event, esd_ctx->esd_message, UOS_SEND_EVT);

	ret = UOS_WaitFlag(esd_ctx->esd_flag, 1, OSA_FLAG_OR_CLEAR, &actual_flags, OS_SUSPEND);
	if (ret == OS_SUCCESS && (actual_flags & 1)){
		LCDLOGI("INFO: lcd_esd_stop: lcd esd function exited!\r\n");
	}else{
		LCDLOGW("Warning: lcd_esd_stop: Wait for lcd esd function exit (0x%x) fail!\r\n", actual_flags);
	}

	if(NULL != esd_ctx->esd_task){
		UOS_DeleteTask((TASK_HANDLE *)esd_ctx->esd_task);
	}
	
	UOS_FreeMutex(esd_ctx->esd_mutex);
	UOS_DeleteFlag(esd_ctx->esd_flag);
	UOS_FreeMessageQueue(esd_ctx->esd_message);

	UOS_FREE(esd_ctx);

	LCDLOGI("INFO: lcd_esd_uninit ---\n");
	return 0;
}

#else
struct s_lcd_esd* lcd_esd_init(struct s_lcd_ctx* lcd, struct panel_spec* panel)
{
	LCDLOGI("INFO: lcd_esd_init: ESD is not enabled!\n");
	return NULL;
}

int lcd_esd_start(struct s_lcd_esd* esd_ctx)
{
	LCDLOGI("INFO: lcd_esd_start: ESD is not enabled!\n");
	return -1;
}

int lcd_esd_stop(struct s_lcd_esd* esd_ctx)
{
	LCDLOGI("INFO: lcd_esd_stop: ESD is not enabled!\n");
	return -1;
}

int lcd_esd_uninit(struct s_lcd_esd* esd_ctx)
{
	LCDLOGI("INFO: lcd_esd_uninit: ESD is not enabled!\n");
	return -1;
}

#endif /*LCD_ESD_SUPPORT*/
