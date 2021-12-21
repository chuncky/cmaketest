/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/

#ifndef _UI_LOG_API_H
#define _UI_LOG_API_H

#include "cp/cp_include.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 
** The next part is UI log module tag, maybe used these module tag to 
** set module logging priority.
**
*/
#define APP_ALARM_TAG			"APP_ALARM "
#define APP_AUDIO_TAG			"APP_AUDIO "
#define APP_BT_TAG				"APP_BT "
#define APP_CALC_TAG			"APP_CALC "
#define APP_CC_TAG				"APP_CC "
#define APP_CL_TAG				"APP_CL "
#define APP_CAMERA_TAG			"APP_CAMERA "
#define APP_COMMON_TAG			"APP_COMMON "
#define APP_FILE_TAG			"APP_FILE "
#define APP_FM_TAG				"APP_FM "
#define APP_GAME_TAG			"APP_GAME "
#define APP_LAUNCHER_TAG		"APP_LAUNCHER "
#define APP_PB_TAG				"APP_PB "
#define APP_SMS_TAG				"APP_SMS "
#define APP_MMS_TAG				"APP_MMS "

#define HAL_AUDIO_TAG			"HAL_AUDIO "
#define HAL_BT_TAG				"HAL_BT "
#define HAL_CAMERA_TAG			"HAL_CAMERA "
#define HAL_CHARGE_TAG			"HAL_CHARGE "
#define HAL_CHIP_TAG			"HAL_CHIP "
#define HAL_COMMON_TAG			"HAL_COMMON "
#define HAL_FILE_TAG			"HAL_FILE "
#define HAL_FM_TAG				"HAL_FM "
#define HAL_GPIO_TAG			"HAL_GPIO "
#define HAL_KEYPAD_TAG			"HAL_KEYPAD "
#define HAL_LCD_TAG				"HAL_LCD "
#define HAL_FLASH_TAG			"HAL_FLASH "
#define HAL_DBG_TAG				"HAL_DBG "

#define UI_LOG_TAG				"MMI:"

/* 
** The next part is UI log module inner used condition MACRO, maybe used to 
** switch onoff module logging.
**
*/

#define HAL_FS_DEBUG		0	/* hal file system debug */

/* 
** The next part is UI log priority, used to set logging level.
**
*/

/* UI log level value, in increasing order of priority */
#define UI_LOG_VAL_UNKNOWN	0	/* Reserved logging priority. */
#define UI_LOG_VAL_DEFAULT	1	/* Default logging priority. */
#define UI_LOG_VAL_VERBOSE	2	/* Verbose logging, should typically be disable for a release package. */
#define UI_LOG_VAL_DEBUG	3	/* Debug logging, should typically be disable for a release package. */
#define UI_LOG_VAL_INFO		4	/* Information logging, should typically be disable for a release package. */
#define UI_LOG_VAL_WARN		5	/* Warning logging, for use with recoverable failures. */
#define UI_LOG_VAL_ERROR	6	/* Error logging, for use with unrecoverable failures. */
#define UI_LOG_VAL_FATAL	7	/* Fatal logging, for use when aborting. */
#define UI_LOG_VAL_SILENT	8	/* Mask all log, for internal use only. */

/* UI log level string, in increasing order of priority */
#define UI_LOG_STR_UNKNOWN	"<0>"
#define UI_LOG_STR_DEFAULT	"<1>"
#define UI_LOG_STR_VERBOSE	"<2>"
#define UI_LOG_STR_DEBUG	"<3>"
#define UI_LOG_STR_INFO		"<4>"
#define UI_LOG_STR_WARN		"<5>"
#define UI_LOG_STR_ERROR	"<6>"
#define UI_LOG_STR_FATAL	"<7>"
#define UI_LOG_STR_SILENT	"<8>"

/* 
** The next part is UI logging MACRO.
** .
**
*/

void ui_log_init(void);
void ui_log_enable(unsigned int val);
void ui_log_disable(unsigned int val);
int  is_raw_log_enable(void);
int  is_hal_log_enable(void);
int  is_app_log_enable(void);
int  ui_log_get_flag(void);
int  ui_log_printf(const char* fmt, ...);
int  ui_log_filter(const char* fmt, ...);
void ui_log_set_filter(const char* str);
int  log_hal_printf(const char* fmt, ...);
int  ui_log_xprintf(int level, const char* module_tag, 
	const char* prefix_tag, const char* fmt, ...);

void ui_log_set_trace_level(int level);
int  ui_log_get_trace_level(void);
void ui_log_set_module_level(const char* module, int level);
int  ui_log_get_module_level(const char* module);

#ifdef CONFIG_SUPPORT_UILOG

#define hal_log(...)	do{if(is_hal_log_enable())if(ui_log_filter(__VA_ARGS__))log_printf(__VA_ARGS__);}while(0)
#define app_log(...)	do{if(is_app_log_enable())if(ui_log_filter(__VA_ARGS__))log_printf(__VA_ARGS__);}while(0)
#define raw_uart_log(...)	do{ui_log_printf(__VA_ARGS__);}while(0)

#define UILOG(pri, module_tag, prefix_tag, ...) 	do{ui_log_xprintf(pri, module_tag, prefix_tag, __VA_ARGS__);}while(0)

#define UILOGE(...) UILOG(UI_LOG_VAL_ERROR,		MODULE_TAG, "ERR: ", __VA_ARGS__)
#define UILOGW(...) UILOG(UI_LOG_VAL_WARN,		MODULE_TAG, "WNG: ", __VA_ARGS__)
#define UILOGI(...) UILOG(UI_LOG_VAL_INFO,		MODULE_TAG, "INF: ", __VA_ARGS__)
#define UILOGD(...) UILOG(UI_LOG_VAL_DEBUG,		MODULE_TAG, "DBG: ", __VA_ARGS__)
#define UILOGV(...) UILOG(UI_LOG_VAL_VERBOSE,	MODULE_TAG, "VBS: ", __VA_ARGS__)
#define MODULE_TAG	NULL

#else	//CONFIG_SUPPORT_UILOG

#define hal_log(...)
#define app_log(...)
#define raw_uart_log(...)

#define UILOG(pri, module_tag, prefix_tag, ...)

#define UILOGE(...)
#define UILOGW(...)
#define UILOGI(...)
#define UILOGD(...)
#define UILOGV(...)

#endif	//CONFIG_SUPPORT_UILOG

#ifdef __cplusplus
}
#endif

#endif /* _UI_LOG_API_H */
