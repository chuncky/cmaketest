#ifndef __APP_TARGET_DEFS_H__
#define __APP_TARGET_DEFS_H__

#include "plat_config_defs.h"

//minigui internal incore-font select
//#define        LIB_FONT_ALL                    0    //(size:3.5MB)include all languages, pixel size:12,16,18,22,26
#define        LIB_FONT_ALL_NO12_NO16            1    //(size:2.2MB)include all languages, pixel size:18,22,26
#define        LIB_FONT_OnlyCnEn_NO12_NO16        2    //(size:1.6MB)include only chinese/english, pixel size:18,22,26
#define        LIB_FONT_OnlyEn_NO12_NO16        3    //(size: 90KB)include only english, pixel size:18,22,26
#define        INCORE_FONT_LIB_SELECT            LIB_FONT_OnlyCnEn_NO12_NO16

//: refer to TARGET_DEFS of apps/makefile.ng.(rel-181105)
//  #define    MMI_SPEC_RETRO
  //#define    LCDSIZE_240_320
  //#define     LCDDEPTH_32
  //#define    LCD_WIDTH                            240
  //#define    LCD_HEIGHT                            320
//  #define    LCD_SYNC_UPDATE
//  #define    USE_NGUX
//  #define    USE_NGUX_FONT
//#define    USE_UCS2_ENCODING
#define    ENCODING_LENGTH                        2
//  #define    MAX_MCHAR_WIDTH                    3
//  #define    __MMI_LANGUAGE_ENGLISH_US__
//  #define    __MMI_LANGUAGE_CHINESE_CN__
//  #define    __MMI_LANGUAGE_CHINESE_TW__
//#define    __MMI_T9__         1
//  #define    __MMI_T9_V7__
//#ifdef CONFIG_ASR_SIM_DUAL
//#define    __ASR_UI_MULTI_SIM__                //supporting dual sim ui
//#else
//#endif
#define    __ADP_MULTI_SIM__
#define    __MMI_MULTI_SIM__
#define    MMI_SIM_NUMBER                        2
#define    __MOD_SMSAL__
  #define    __MOD_CSM__
  #define    __MOD_PHB__
#define    __MOD_RAC__
  #define    PHB_REJECTCALL_NO_RECORD_AND_MENU
  #define    SIM_SMS_ENTRY_COUNT                0

#define    __MMI_SMS_SUPPORT_SANMU_DRAFTBOX__
#define    __SUPPORT_SMSCLASS__
#define    __MMI_PHB_SUPPORT_ASR_NUMBERS__
#define    __MMI_SPREADSPECTRUM_STYLE__
  #define    __MMI_CALENDAR_WITH_EVENT__
  #define    __MMI_FILEMANAGER
#define    __MMI_NOTEPAD__
  #define    __MMI_PRAYERTIMES__
  #define    ENABLE_NVRAM
//#ifdef  CONFIG_MMI_SUPPORT_CALLAPP
//  #define    ENABLE_CALL
//#endif
//  #define    ENABLE_PHONEBOOK
//  #define    ENABLE_SMS
//  #define    ENABLE_CALLHISTORY
//  #define    ENABLE_SYMBOL_SELECTOR
  #define    __MMI_SMS_HAS_RECENT_SEND_LIST__
//  #define    __MMI_MESSAGES_MOVE__

  //rel-181218 new add
//  #define   ENABLE_MULTIMEDIA
//  #define   CSD_NEW_API
  #define   ENABLE_MCI
//  #define   __FF_AUDIO_SET__
//  #define   AUDIO_PLAYER_SUPPORT
//  #define   SOUND_RECORDER_SUPPORT
//  #define   VIDEO_PLAYER_SUPPORT
#if (CONFIG_UI_MODUAL_CAMERA)
  #define   __MMI_VIDEO_RECORDER__
  #define   VIDEO_RECORDER_SUPPORT
  #define   MMI_SUPPORT_CAMERA
  #define   CAMERA_SUPPORT
#endif
//#if (CONFIG_UI_FM_SUPPORT)
//  #define   FM_RADIO_ENABLE
//#endif
#if defined (__BOARD_CUSTOM_B__) ||defined (__BOARD_CUSTOM_V__)
  #define FM_RADIO_WITH_EXTERNAL_ANT
#endif

  #define     __FS_MOVE_SUPPORT__

  #define   __PHB_0x81_SUPPORT__


  #define     ENABLE_SETTINGS_CALLUTILS
#if 0
  #define   __MMI_BT_SUPPORT__
  #define   __MMI_AVRCP_SUPPORT__
  #define   __MMI_FTS_SUPPORT__
  #define   __MMI_SUPPORT_BLUETOOTH__
  #define   __MED_BT_A2DP_MOD__
  #define   __BT_HFG_PROFILE__
  #define   __MMI_HFP_SUPPORT__
#endif
  #define   __SUPPORT_NO_SIM_LOAD_ME_SMS__
//  #define   FM_RADIO_RECORD


//: refer to TARGET_DEFS of target/build/rules-armcc-threadx.ng
#define     __THREADX__
#define     __TARGET_ASR3601__
#define     USE_PLATFORM_TIMER
#define        __MMI_MESSAGES_CB__
#define CMID_CB_MAX_CH 20
#define CMID_CB_MAX_DCS 17


#endif    //__APP_TARGET_DEFS_H__
