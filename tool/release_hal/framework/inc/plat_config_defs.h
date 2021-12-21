#ifndef __PLAT_CONFIG_DEFS_H__
#define __PLAT_CONFIG_DEFS_H__
#include "autoconf.h"

//#define CONFIG_USE_AUDIO_POLICY		1
#define CONFIG_BOARD_CRANE_EVB_Z2	1

//only for demo 
#define CONFIG_DEMO_SUPPORT_NUMBER_TONE	0

//only for alarm snooze feature 
#define CONFIG_UI_ALARM_SNOOZE_TIME	0

//only for set exalarm feature
#define CONFIG_EXALARM 0


#define    PHONE_SMS_ENTRY_COUNT              300
//define the number of contacts in flash.
#define    PHONE_PHB_ENTRY_COUNT              150
//define the number of blacklist
#define 	MMI_CALL_BLOCKNUMBER_COUNT 	15
//define the number of allowlist
#define 	MMI_CALL_ALLOWNUMBER_COUNT 	15
//enable on key function
#define PHONEBOOK_MEMO_OPT_CONTACTS   1


#if defined(__MMI_PHB_MAX_NAME_60__)
#define MAX_PB_NAME_LENGTH		60
#else
#define MAX_PB_NAME_LENGTH		20
#endif
#define MAX_PB_NAME_LENGTH_LIMIT		14
#define MAX_PB_NUMBER_LENGTH_LIMIT	20
#define MAX_PB_NAME_HZ_COUNT_LIMIT	6
#define MAX_PB_NUMBER_LENGTH		20

#define MAX_PB_NUMBER_CNT    1

#define TM_STARTYEAR            2000
#define TM_ENDYEAR              2037

//defaut init start time
#define TM_YEAR    TM_STARTYEAR + 8
#define TM_MONTH   1
#define TM_DAY     1

#define TM_HOUR     0
#define TM_MINUTE     0


/**
 * \def DEF_LPRESS_TIME
 * \brief Default long pressed time of a key.
 *
 * \sa MSG_KEYLONGPRESS
 */

#define DEF_LPRESS_TIME         200
#define DEF_LONGPRESS_TIME      2000 //2s send long press
#define DEF_ALWAYSPRESS_TIME    125


//hardware platform support features define
#if defined(CONFIG_BOARD_CUSTOM_B)

//hardware platform support vibrator: 0-no vibrator support
#define CONFIG_HARDWARE_VIBRATOR	0
//hardware platform support earphone: 0-no earphone support
#define CONFIG_HARDWARE_EARPHONE	0
//hardware platform support extenal PA:1-has external speaker pa
#define CONFIG_HARDWARE_EXT_PA		1

#elif defined(CONFIG_BOARD_CUSTOM_V)

//hardware platform support vibrator:1-support vibrator
#define CONFIG_HARDWARE_VIBRATOR	1
//hardware platform support vibrator:1-support earphone
#define CONFIG_HARDWARE_EARPHONE	1
//hardware platform support extenal PA:0-no external speaker pa
#define CONFIG_HARDWARE_EXT_PA		0

#else

//hardware platform support vibrator:1-support vibrator
#define CONFIG_HARDWARE_VIBRATOR	1
//hardware platform support earphone:1-support earphone
#define CONFIG_HARDWARE_EARPHONE	1
//hardware platform support extenal PA:0-no external speaker pa
#define CONFIG_HARDWARE_EXT_PA		0

#endif

//Jpeg buffer size
#define CONFIG_JPGE_MEMORY_SIZE		3/5 // size = 3/5 NV12(width x height *3/2)

#ifdef BUILD_CAMERA_MODUAL
#define CONFIG_UI_MODUAL_CAMERA     1
#endif
#if (CONFIG_UI_MODUAL_CAMERA)
#define CONFIG_UI_DUAL_CAMERA 		0
#define CAM_SUPPORT_BANDING         1
#endif

#define ONE_KEY_LOCK_SCREEN         1	// set to 0 for QA autotest case
#define CONFIG_MENUITEM_WITH_INDEX  0

#ifdef CONFIG_FMRADIO_PLAYER_SUPPORT
#define FM_RADIO_STEREO_SUPPORT     1
#endif
//calllog size

#define CALLLOG_MAX_RECORD	100
#define CALLLOG_MAX_SAME	100

//#define CONFIG_UI_TASK_EXAMPLE

/***CB  change the following must change the corresponding NV*****/
#define MMI_MAX_CB_CHNL_NUM					15
#define SMS_CB_MAX_ENTRY					MMI_MAX_CB_CHNL_NUM

#define MAX_CB_MSG_PAGE						4
#define MAX_STORE_ARCHIVE_CB_MSG            20 
#define MAX_STORE_TEMPORARY_CB_MSG          10
#define MAX_STORE_CB_MSG            		(MAX_STORE_ARCHIVE_CB_MSG +MAX_STORE_TEMPORARY_CB_MSG)

#define NVRAM_SMS_CB_SMS_PAGE_SIZE			94
#define NVRAM_SMS_CB_SMS_PAGE_NUM_TOTAL		40

/*sms*/
#define MMI_SMS_MAX_MSG_SEG					6			/* This value is to define the maximun segment number of EMS short message. */

#define CONFIG_SETTING_KPBKLT_TIMEOUT	1

/*settings: playback time(seconds) for changing volume of call-ring*/
#define SECONDS_CHANGE_CALLRING_VOLUME		5	/* 5-seconds */

#define DM_REGISTER_STATUS_SIZE	96

#if !(CONFIG_FWP)
//CONFIG_FP: still use old midi/mp3 keyring; you can set 1 to use dtmf-tone.
/*keyring: 1-use dtmf tone as keyring; 0-use midi/mp3 as keyring */
#define CONFIG_KEYRING_DTMF_TONE	0
#else	
//CONFIG_FWP
/*keyring: 1-use dtmf tone as keyring; 0-use midi/mp3 as keyring */
#define CONFIG_KEYRING_DTMF_TONE	1
#endif

//#if (CONFIG_KEYRING_DTMF_TONE == 1)
#define	DEF_VOL_KEYRING_DTMF 	MCI_AUDIO_TONE_0DB
//#endif

#define CONFIG_UI_DEFAULT_DATA_ON 1

//audioplayer playlist scan full disk
#define CONFIG_UI_AUDIOPLAYER_SCAN_FULLDISK	1

//videoplayer playlist scan full disk
#define CONFIG_UI_VIDEOPLAYER_SCAN_FULLDISK	1

//photos scan full disk
#define CONFIG_UI_PHOTOS_SCAN_FULLDISK	1

//ro define
#define SIM_SMS_ENTRY_COUNT 0

#define CMID_CB_MAX_CH 20
#define CMID_CB_MAX_DCS 17

#define LIB_FONT_ALL_NO12_NO16 1
#define LIB_FONT_OnlyCnEn_NO12_NO16 2
#define LIB_FONT_OnlyEn_NO12_NO16 3
#define INCORE_FONT_LIB_SELECT LIB_FONT_OnlyCnEn_NO12_NO16

#define ENCODING_LENGTH 2

#define MMI_SIM_NUMBER 2

#ifdef CONFIG_DISPLAY_240x320
//	list(APPEND MMI_GLOBLE_FEATURE_DEF  "CONFIG_DISPLAY_240x320=1")
//	list(APPEND MMI_GLOBLE_FEATURE_DEF  "LCDSIZE_240_320=1")
#define	LCD_WIDTH 240
#define	LCD_HEIGHT 320
//	list(APPEND MMI_GLOBLE_FEATURE_DEF  "LCDDEPTH_32")
#elif defined(CONFIG_DISPLAY_320x240)
//	list(APPEND MMI_GLOBLE_FEATURE_DEF  "CONFIG_DISPLAY_320x240=1")
//	list(APPEND MMI_GLOBLE_FEATURE_DEF  "LCDSIZE_320_240=1")
#define	LCD_WIDTH 320
#define	LCD_HEIGHT 240
//	list(APPEND MMI_GLOBLE_FEATURE_DEF  "LCDDEPTH_32")
#elif defined(CONFIG_DISPLAY_128x160)
//	list(APPEND MMI_GLOBLE_FEATURE_DEF  "CONFIG_DISPLAY_128x160=1")
//	list(APPEND MMI_GLOBLE_FEATURE_DEF  "LCDSIZE_128_160=1")
#define	LCD_WIDTH 128
#define	LCD_HEIGHT 160
//	list(APPEND MMI_GLOBLE_FEATURE_DEF  "LCDDEPTH_32")
//	### simu 128x160 on DKB's 240x320 lcd. only for demo.
//	list(APPEND MMI_GLOBLE_FEATURE_DEF  "SIMU_128_160_ON_LCD240x320=1")
#elif defined(CONFIG_DISPLAY_176x220)
//	list(APPEND MMI_GLOBLE_FEATURE_DEF  "CONFIG_DISPLAY_176x220=1")
//	list(APPEND MMI_GLOBLE_FEATURE_DEF  "LCDSIZE_176_220=1")
#define	LCD_WIDTH 176
#define	LCD_HEIGHT 220
#endif

#ifdef CONFIG_MMI_SUPPORT_BLUETOOTH
//range: valid: 0~0x7f; -1: not init.
#define		MAX_BTVOL		0x7F		//0~7F: match with the 'volume' parameter of appbt_a2dp_set_volume(volume)
#define		MAX_ITEM_BTVOL	12			//[0~11];
#endif

#endif	//__PLAT_CONFIG_DEFS_H__
