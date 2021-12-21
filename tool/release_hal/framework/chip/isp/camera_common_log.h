#ifndef _CAM_BASE_H_
#define _CAM_BASE_H_

#include "plat_types.h"
#include "ui_log_api.h"
#include "plat_config_defs.h"


/* streamon p2 (sensor full size) */
//#define CAM_FULLSIZE_STREAM

/**********************************************************************
*                           cam hw version                           *
**********************************************************************/
typedef enum _CamHw_Version {
	CAM_HW_VERSION_1	= 0x20180417,  /* z1 */
	CAM_HW_VERSION_2	= 0x20180813,  /* z2 */
	CAM_HW_VERSION_3	= 0x20181106,  /* A0 */
}_CamHw_Version;

extern int CONFIG_CAM_HW_VERSION;

#define BUREG_READ(x) (*(volatile uint32_t *)(x))
#define BUREG_WRITE(x,y) ((*(volatile uint32_t *)(x)) = (y))

#define CAM_LOG_RETURN_LINE "\n"

typedef enum _CamLogLevel {
	CAMLOG_LEVEL_ERROR		= 100,
	CAMLOG_LEVEL_WARNING	= 200,
	CAMLOG_LEVEL_INFO		= 300,
	CAMLOG_LEVEL_DEBUG		= 400,
	CAMLOG_LEVEL_VERBOSE	= 500
}CamLogLevel;

#define CAMLOG_GLOBAL_LEVEL 400

typedef enum {
	HAL_EFFECT_NONE     = -1,
	HAL_EFFECT_NORMAL=0,
	HAL_EFFECT_GRAYSCALE,
	HAL_EFFECT_SEPIA,
	HAL_EFFECT_SEPIAGREEN,
	HAL_EFFECT_SEPIABLUE,
	HAL_EFFECT_COLORINV=5,
	HAL_EFFECT_GRAYINV,
	HAL_EFFECT_BLACKBOARD,
	HAL_EFFECT_WHITEBOARD,
	HAL_EFFECT_COPPERPLATE,
	HAL_EFFECT_EMBOSS=10,
	HAL_EFFECT_BLUECARVING,
	HAL_EFFECT_CONTRAST,
	HAL_EFFECT_JEAN,
	HAL_EFFECT_SKETCH,
	HAL_EFFECT_OIL=15,
	HAL_EFFECT_NO_OF_EFFECT=16,
	HAL_EFFECT_ANTIQUE,
	HAL_EFFECT_RED,
	HAL_EFFECT_GREEN,
	HAL_EFFECT_BLUE=20,
	HAL_EFFECT_BLACKWHITE,
	HAL_EFFECT_NEGATIVE,

} CAM_HAL_EFFECT;

typedef enum {
	HAL_WB_AUTO = 0x0,
	HAL_WB_DAYLIGHT,
	HAL_WB_TUNGSTEN,
	HAL_WB_OFFICE,
	HAL_WB_CLOUDY,
	HAL_WB_INCANDESCENT,
} CAM_HAL_WB;

typedef enum {
	HAL_CONTRAST_n3,
	HAL_CONTRAST_n2,
	HAL_CONTRAST_n1,
	HAL_CONTRAST_mid, 
	HAL_CONTRAST_p1,  
	HAL_CONTRAST_p2,
	HAL_CONTRAST_p3,
} CAM_HAL_CONTRAST;

typedef enum {
	HAL_SATURATION_n3,
	HAL_SATURATION_n2,
	HAL_SATURATION_n1,
	HAL_SATURATION_mid, 
	HAL_SATURATION_p1,  
	HAL_SATURATION_p2,
	HAL_SATURATION_p3,
} CAM_HAL_SATURATION;

typedef enum {
	HAL_SCENE_AUTO,
	HAL_SCENE_NIGHT,
} CAM_HAL_SCENE_MODE;

typedef enum {
	NIGHT_FPS_1 = 1,
	NIGHT_FPS_2,
	NIGHT_FPS_3,
	NIGHT_FPS_4,
	NIGHT_FPS_5,
	NIGHT_FPS_6,
	NIGHT_FPS_7,
	NIGHT_FPS_8,
	NIGHT_FPS_9,
	NIGHT_FPS_10,
} CAM_HAL_NIGHT_FPS;

typedef enum {
	HAL_QTY_LOW,
	HAL_QTY_NORMAL,
	HAL_QTY_FINE,
	HAL_QTY_SUPER_FINE,	
} CAM_HAL_IMG_QTY;

typedef enum {
	HAL_BRIGHTNESS_1 = 1,
	HAL_BRIGHTNESS_2,
	HAL_BRIGHTNESS_3,
	HAL_BRIGHTNESS_4,
	HAL_BRIGHTNESS_mid,
	HAL_BRIGHTNESS_6,
	HAL_BRIGHTNESS_7,
	HAL_BRIGHTNESS_8,
	HAL_BRIGHTNESS_9,
} CAM_HAL_BRIGHTNESS;

typedef enum {
	HAL_FLASH_AUTO,
	HAL_FLASH_OFF = HAL_FLASH_AUTO,
	HAL_FLASH_ON,
} CAM_HAL_FLASH;

typedef enum {
	HAL_FLASH_LEVEL_1 = 1,
	HAL_FLASH_LEVEL_2,
	HAL_FLASH_LEVEL_3,
	HAL_FLASH_LEVEL_4,
	HAL_FLASH_LEVEL_5,
	HAL_FLASH_LEVEL_6,
	HAL_FLASH_LEVEL_7,
	HAL_FLASH_LEVEL_8,
	HAL_FLASH_LEVEL_9,
	HAL_FLASH_LEVEL_10,

} CAM_HAL_FLASH_LEVEL;


typedef enum {
	HAL_BANDING_AUTO,
	HAL_BANDING_50HZ,
	HAL_BANDING_60HZ,
} CAM_HAL_BANDING;

#ifndef CAM_LOG_PRINT
#define CAM_LOG_PRINT(logLevel, levelName, format, ...) 										\
	do {																						\
		if (logLevel <= CAMLOG_GLOBAL_LEVEL) {													\
			raw_uart_log("%s:" levelName ": " format CAM_LOG_RETURN_LINE, __func__, ##__VA_ARGS__);\
		}																						\
	} while(0)
#endif

#define CAMLOGE(fmt, ...) CAM_LOG_PRINT(CAMLOG_LEVEL_ERROR, "err", fmt, ##__VA_ARGS__)
#define CAMLOGW(fmt, ...) CAM_LOG_PRINT(CAMLOG_LEVEL_WARNING, "warning", fmt, ##__VA_ARGS__)
#define CAMLOGI(fmt, ...) CAM_LOG_PRINT(CAMLOG_LEVEL_INFO, "info", fmt, ##__VA_ARGS__)
#define CAMLOGD(fmt, ...) CAM_LOG_PRINT(CAMLOG_LEVEL_DEBUG, "debug", fmt, ##__VA_ARGS__)
#define CAMLOGV(fmt, ...) CAM_LOG_PRINT(CAMLOG_LEVEL_VERBOSE, "debug", fmt, ##__VA_ARGS__)

#endif //_CAM_CORE_H_
