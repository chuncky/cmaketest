/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/

#ifndef _DEVICE_H
#define _DEVICE_H

#define UEM_GREETING_LEN      62
#define UEM_EQUIP_ID_LEN      30

/*
** Define the ring compose capability
*/
#define MAX_RING_COMPOSE_NUM        5
#define MAX_RING_COMPOSE_LEN        300

/*
** Define the display text maximum len
*/
#define UEM_DISPLAY_TEXT_LEN  30
#define UEM_MAX_GREETING_LEN  30

typedef enum {
   L4_BCD = 0x01,
   L4_ASCII = 0x04,
   L4_UCS2 = 0x08
} L4_charset_enum;

typedef enum {
   MMI_PS_AT_LANG_DEFAULT,
   MMI_PS_AT_LANG_UNKWN,
   MMI_PS_AT_LANG_EN,
   MMI_PS_AT_LANG_TW,
   MMI_PS_AT_LANG_ZH,
   
   MMI_PS_AT_LANG_LAST
} L4_AT_lang_enum;

typedef enum{
   PLAY_STOP,
   PLAY_REPEAT,
   PLAY_ONCE,
   PLAY_DUR,
   MAX_PATTERN_TYPE = PLAY_DUR
} PATTERN_TYPE;

typedef unsigned int UART_baudrate;

#define     UART_BAUD_AUTO          0
#define     UART_BAUD_75            75
#define     UART_BAUD_150           150
#define     UART_BAUD_300           300
#define     UART_BAUD_600           600
#define     UART_BAUD_1200          1200
#define     UART_BAUD_2400          2400    
#define     UART_BAUD_4800          4800
#define     UART_BAUD_7200          7200
#define     UART_BAUD_9600          9600
#define     UART_BAUD_14400         14400
#define     UART_BAUD_19200         19200
#define     UART_BAUD_28800         28800
#define     UART_BAUD_33900         33900
#define     UART_BAUD_38400         38400
#define     UART_BAUD_57600         57600
#define     UART_BAUD_115200        115200
#define     UART_BAUD_230400        230400
#define     UART_BAUD_460800        460800
#define     UART_BAUD_921600        921600

/**************************************************************
  **
  **      Audio Interface 
  ** 
  **************************************************************/
  
/* Define the audio device type */
typedef unsigned char audio_type_enum;
#define AUDIO_DEVICE_SPEAKER            0 /* Tone, Keypad sound to play on audio device. */
#define AUDIO_DEVICE_MICROPHONE     1 /* Microphone sound to send on audio device. */
#define AUDIO_DEVICE_BUZZER              2 /* Ring tone sound to play on audio device. */
#define AUDIO_DEVICE_GMI                    3 /* MIDI/Melody sound to play on audio device */
#define AUDIO_DEVICE_SPEAKER2          4 /* earphone, carkit */
#define AUDIO_DEVICE_LOUDSPEAKER    5 /* loudspeaker for free sound */
#define AUDIO_DEVICE_SPEAKER_BOTH  6
#define MAX_AUDIO_DEVICE_NUM           7


/* Define the audio device type */
typedef unsigned char audio_input_path_enum;
#define AUDIO_DEVICE_MIC1     0    /* ME */
#define AUDIO_DEVICE_MIC2     1   /* Headset */
#define AUDIO_DEVICE_FMRR     2 /* FM Radio */

/* Define volume category */
typedef unsigned char volume_category_enum;
#define VOL_NORMAL               0
#define VOL_HEADSET              1
#define VOL_HANDFREE             2
#define MAX_VOL_CATE             3


/* Define volume type.*/
typedef unsigned char volume_type_enum;
#define VOL_TYPE_CTN                0  /* MMI can apply to associate volume; call tone attribute */
#define VOL_TYPE_KEY                1  /* MMI can apply to associate volume; keypad tone attribute */
#define VOL_TYPE_MIC                2  /* microphone attribute */
#define VOL_TYPE_GMI                3  /* MMI can apply to associate volume; melody, imelody, midi attribute */
#define VOL_TYPE_SPH                4  /* MMI can apply to associate volume; speech sound attribute */
#define VOL_TYPE_SID				5  /* side tone attribute */
#define VOL_TYPE_MEDIA            6  /* MMI can apply to associate volume; As MP3, Wave,... attribute */
#define VOL_TYPE_VIDEO             7
#define VOL_TYPE_TV            8
#define MAX_VOL_TYPE                9


/* Volume level. based index is from 0..(MAX_VOL_LEVEL-1) */
#ifndef MAX_VOL_LEVEL
#define MAX_VOL_LEVEL 7
#endif


/* Define the audio play style. */
typedef unsigned char audio_play_style_enum;
#define DEVICE_AUDIO_PLAY_CRESCENDO    0 /* Play sound for crescendo. */
#define DEVICE_AUDIO_PLAY_INFINITE     1 /* Play sound for infinite. */
#define DEVICE_AUDIO_PLAY_ONCE         2 /* Play sound for once. */
#define DEVICE_AUDIO_PLAY_DESCENDO     3 /* Play sound for descendo. */


typedef struct {
   unsigned char first_out_amp[MAX_AUDIO_DEVICE_NUM][MAX_VOL_LEVEL];
   unsigned char second_out_amp[MAX_AUDIO_DEVICE_NUM][MAX_VOL_LEVEL];
   unsigned char side[MAX_AUDIO_DEVICE_NUM][MAX_VOL_LEVEL];
   unsigned char volume[MAX_AUDIO_DEVICE_NUM];
} acoustic_gain_struct;



/**************************************************************
  **
  **      Keypad  Interface 
  ** 
  **************************************************************/
/*
** Define the keypad status.
*/
typedef unsigned char keypad_status_enum;
#define KEY_PRESSED  0 /* Keypad is pressed. */
#define KEY_RELEASED 1 /* Keypad is released. */
#define KEY_LONGPRESS 2 /* Keypad is longpressed. */
#define KEY_ALWAYSPRESSED 3 /* Keypad is repeated. */
#ifdef TWO_KEY_ENABLE
#define KEY_PRESSED_TWOKEY  4 /* Two Keypad is pressed. */
#define KEY_RELEASED_TWOKEY 5 /* Two Keypad is released. */
#endif   /*TWO_KEY_ENABLE*/

/*
** Define the keypad code value.
*/
typedef unsigned char keypad_code_enum;


/**************************************************************
  **
  **     RTC  Interface 
  ** 
  **************************************************************/

/* Define the RTC type. */
typedef unsigned char rtc_type_enum;
#define RTC_TIME_CLOCK_IND 0 /* The time clock indication. */
#define RTC_ALARM_IND      1 /* The alarm indication */


/* Define the type to setting RTC. */
typedef unsigned char rtc_setting_type_enum;
#define RTC_SETTING_TYPE_DATETIME   0
#define RTC_SETTING_TYPE_DATE       1
#define RTC_SETTING_TYPE_TIME       2


/* Define the RTC alarm id. */
typedef unsigned char rtc_alarm_id_enum;
#define RTC_ALARM1_IND     0 /* The alarm indication */
#define RTC_ALARM2_IND     1 /* The alarm indication */
#define RTC_ALARM3_IND     2 /* The alarm indication */
#define MAX_ALARM_NUM      3

/*
** Define the Alarm type
*/
typedef unsigned char rtc_alarm_type_enum;
#define RTC_ALARM_VIBRATOR    0x01
#define RTC_ALARM_LED         0x02
#define RTC_ALARM_SOUND       0x04
#define RTC_ALARM_SILENT      0x08

/*
** Define the Alarm recurr
*/
typedef unsigned char rtc_alarm_recurr_enum;
#define RTC_ALARM_DAY1        0x01
#define RTC_ALARM_DAY2        0x02
#define RTC_ALARM_DAY3        0x04
#define RTC_ALARM_DAY4        0x08
#define RTC_ALARM_DAY5        0x10
#define RTC_ALARM_DAY6        0x20
#define RTC_ALARM_DAY7        0x40
#define RTC_ALARM_DAY_ALL     0x7F

/*
** Define the Date/Time format type.
*/
typedef unsigned char date_format_enum;
#define   DATE_DD_MMM_YYYY    1
/*
#define   DATE_DD_MM_YY_H     2
#define   DATE_MM_DD_YY       3
#define   DATE_DD_MM_YY_S     4
#define   DATE_DD_MM_YY_P     5
#define   DATE_YYMMDD         6
#define   DATE_YY_MM_DD       7
*/

#define DATE_DD_MM_YYYY_S 	8
#define DATE_MM_DD_YYYY_S		9
#define DATE_YYYY_MM_DD_S		10
#define DATE_YYYY_MM_DD_H		11
#define DATE_MMM_DD_YYYY		12

#define   DATE_AUX_YY_MM_DD   1
#define   DATE_AUX_YYYY_MM_DD 2


typedef unsigned char time_format_enum;
#define   TIME_HH_MM_24       1
#define   TIME_HH_MM_12       2

/*
** Define the RTC clock/alarm format type.
*/
/* Could be rtc_alarm_format_enum or rtc_clock_format_enum */
typedef unsigned char rtc_format_enum; 
/*Y:Year,M:Month,W:DOW,D:DOM,H:Hour,M:Min,S:SEC*/
typedef unsigned char rtc_alarm_format_enum;
#define   DEVICE_AL_EN_None   0
#define   DEVICE_AL_EN_ChkS   1
#define   DEVICE_AL_EN_ChkMS  2
#define   DEVICE_AL_EN_ChkHMS 3
#define   DEVICE_AL_EN_ChkDHMS   4
#define   DEVICE_AL_EN_ChkWHMS   5
#define   DEVICE_AL_EN_ChkMDHMS  6
#define   DEVICE_AL_EN_ChkYMDHMS 7
#define   DEVICE_AL_EN_NoChange  8

typedef unsigned char rtc_clock_format_enum;
#define   DEVICE_TC_EN_None   0
#define   DEVICE_TC_EN_ChkS   1
#define   DEVICE_TC_EN_ChkM   2
#define   DEVICE_TC_EN_ChkH   3
#define   DEVICE_TC_EN_ChkD   4
#define   DEVICE_TC_EN_ChkW   5
#define   DEVICE_TC_EN_ChkMon 6
#define   DEVICE_TC_EN_ChkY   7
#define   DEVICE_TC_EN_NoChange  8

/*
** Define the alarm status.
*/
typedef unsigned char alarm_status_enum ;
#define  ALARM_STATUS_ON_POWEROFF_CHARING 0 /* The alarm is started when it's power off with charging status. */
#define  ALARM_STATUS_ON_POWEROFF         1 /* The alarm is started when it's power off status without charging. */
#define  ALARM_STATUS_ON_POWERON          2 /* The alarm is started when it's power on status. */

/*
** Define the calling status
*/
#define  UEM_CALL_UNKNOW          0 
#define  UEM_CALL_SETUP              1 
#define  UEM_CALL_CONNECT         2 
#define  UEM_CALL_DISCONNECT   3 

/**************************************************************
  **
  **      GPIO / EINT  Interface 
  ** 
  **************************************************************/

/* Define the internal GPIO device type. */
typedef unsigned char gpio_device_enum;
#define GPIO_DEV_LED_MAINLCD                  0      /* Main LCD backlight */
#define GPIO_DEV_LED_SUBLCD                    1      /* Sub LCD backlight */
#define GPIO_DEV_LED_STATUS_1                2      /* Status LED  R*/
#define GPIO_DEV_LED_STATUS_2                3      /* Status LED  G*/
#define GPIO_DEV_LED_STATUS_3                4     /* Status LED   B*/
#define GPIO_DEV_LED_KEY                          5     /* Keypad backlight */
#define GPIO_DEV_VIBRATOR                        6      /* Vibrator */
#define GPIO_DEV_FLASHLIGHT                    7      /* FlashLight */
#define GPIO_DEV_RESERVED1                      8      /* Reserve_1 */
#define GPIO_DEV_RESERVED2                      9      /* Reserve_2 */
#define GPIO_DEV_RESERVED3                      10      /* Reserve_3 */
#define GPIO_DEV_RESERVED4                      11      /* Reserve_4 */
#define GPIO_DEV_RESERVED5                      12      /* Reserve_4 */
#define GPIO_DEV_RESERVED6                      13      /* Reserve_4 */
#define GPIO_DEV_RESERVED7                      14      /* Reserve_4 */
#define GPIO_DEV_RESERVED8                      15      /* Reserve_4 */
#define GPIO_DEV_RESERVED9                      16      /* Reserve_4 */
#define GPIO_DEV_RESERVED10                    17      /* Reserve_4 */
#define GPIO_DEV_RESERVED11                      18      /* Reserve_4 */
#define GPIO_DEV_RESERVED12                      19      /* Reserve_4 */
#define MAX_GPIO_DEVICE_NUM                   20


/* Define the LED light level. For LCD bright level, Keypad backlight,  Flashlight */
typedef unsigned char gpio_device_led_level_typedef;
#define LED_LIGHT_LEVEL0               0   /* turn off */
#define LED_LIGHT_LEVEL1               1
#define LED_LIGHT_LEVEL2               2
#define LED_LIGHT_LEVEL3               3
#define LED_LIGHT_LEVEL4               4
#define LED_LIGHT_LEVEL5               5
#define LED_LIGHT_LEVEL_MAX         6


/* Define the Vibrator level. */
typedef unsigned char gpio_device_vibrator_level_typedef;
#define VIBRATOR_OFF              0   /* turn off */
#define VIBRATOR_ON                1   /* turn on */


typedef unsigned char audio_mode_enum;
#define AUD_MODE_NORMAL_RECEIVER        0   /* Normal Mode */
#define AUD_MODE_HEADPHONE        1   /* HeadSet (Earphone) Mode */
#define AUD_MODE_LOUDSPK        2   /* Loudspeaker Mode */
#define AUD_MODE_BLUETOOTH   3
#define MAX_AUD_MODE_NUM      4   

/*
** Define the external GPIO device interrupt.
*/
typedef enum {
    EXT_DEV_NONE	  = 0  , /* None, if there is no external device */
    EXT_DEV_HANDFREE	   , /* Hand-free */
    EXT_DEV_EARPHONE	   , /* Earphone */
    EXT_DEV_CARKIT		   , /* Car-Kit */
    EXT_DEV_KEY_1		   , /* Send Key */
    EXT_DEV_KEY_2		   , /* Reserved */
    EXT_DEV_UART		   , 
    EXT_DEV_CLAM_OPEN  = 8 ,
    EXT_DEV_CLAM_CLOSE	   , 
    EXT_DEV_TFLASH_DETECT ,
    EXT_DEV_SLIDE_DOWN_OPEN 	 ,
    EXT_DEV_SLIDE_DOWN_CLOSE	 , 
    EXT_DEV_CAMERA_SLDE_OPEN	  ,
    EXT_DEV_CAMERA_SLDE_CLOSE	  , 
    //ugrec_tky 0409
    EXT_DEV_CAM_OPEN,
    EXT_DEV_CAM_CLOSE,
    EXT_DEV_FLIP_UP,
    EXT_DEV_FLIP_DOWN,
    EXT_DEV_FLIP_CENTER,
    //ugrec_tky 0409
    EXT_DEV_2D_SENSOR,
    MAX_EXT_DEVICE_NUM	 
}ext_device_enum;


/**************************************************************
  **
  **      PMIC  Interface 
  ** 
  **************************************************************/
typedef enum
{
   PMIC_VBAT_STATUS,    /* Notify the battery voltage, BMT_VBAT_STATUS */
   PMIC_CHARGER_IN,     /* Charger plug in, BMT_CHARGER_IN */
   PMIC_CHARGER_OUT,    /* Charger plug out, BMT_CHARGER_OUT */
   PMIC_OVERVOLPROTECT,    /* The voltage of battery is too high. BMT_OVERVOLPROTECT */
   PMIC_OVERBATTEMP,    /* The temperature of battery is too high. BMT_OVERBATTEMP */
   PMIC_LOWBATTEMP,     /* The temperature of battery is too low. BMT_LOWBATTEMP */
   PMIC_OVERCHARGECURRENT, /* Charge current is too large. BMT_OVERCHARGECURRENT */
   PMIC_CHARGE_COMPLETE,   /* Charge is completed. BMT_CHARGE_COMPLETE */
   PMIC_INVALID_BATTERY,    /* invalid battery  BMT_INVALID_BAT*/
   PMIC_INVALID_CHARGER,   /* invalid charger BMT_INVALID_CHARGER*/
   PMIC_CHARGING_TIMEOUT, /* Bad battery BMT_CHARGE_TIMEOUT */ 
   PMIC_LOWCHARGECURRENT, /* Charge current is too Low. BMT_LOWCHARGECURRENT */
   PMIC_CHARGE_BAD_CONTACT, /* Charger Bad Contact */
   PMIC_BATTERY_BAD_CONTACT, /* Battery Bad Contact */
   PMIC_USB_CHARGER_IN,   /* Usb Charger plug in */
   PMIC_USB_CHARGER_OUT   /* Usb Charger plug out */
} pmic_status_enum;
typedef pmic_status_enum battery_status_enum;

typedef enum 
{

   BATTERY_LOW_POWEROFF = 0,
   BATTERY_LOW_TX_PROHIBIT,
   BATTERY_LOW_WARNING,
   BATTERY_LEVEL_0,
   BATTERY_LEVEL_1,
   BATTERY_LEVEL_2,
   BATTERY_LEVEL_3, 
   BATTERY_LEVEL_4 , /* BATTERY_LEVEL_4 */
   BATTERY_LEVEL_5 , /* BATTERY_LEVEL_5 */
   BATTERY_LEVEL_6 , /* BATTERY_LEVEL_6 */
   BATTERY_LEVEL_LAST = 9

} battery_level_enum;


/* Define the power on status. */
typedef unsigned char power_on_status_enum ;
#define  POWER_ON_KEYPAD         0
#define  POWER_ON_ALARM          1
#define  POWER_ON_CHARGER_IN     2
#define  POWER_ON_EXCEPTION      3
#define  POWER_ON_USB               4
#define  POWER_ON_UNINIT           5
#define  POWERON(mode) ((mode == POWER_ON_KEYPAD) || \
	                                    (mode == POWER_ON_ALARM) || \
	                                    (mode == POWER_ON_CHARGER_IN) ||\
	                                    (mode == POWER_ON_EXCEPTION) ||\
	                                    (mode == POWER_ON_USB) )

/* Define the POWER reset type. This is to use for the power restablish, while it's on charging or alarm status. */
typedef enum
{
   NORMAL_RESET,
   CHARGING_RESET,
   ALARM_RESET
} pmic_reset_type_enum;


/**************************************************************
  **
  **      Misc  Interface 
  ** 
  **************************************************************/

/* Indicator data type */
typedef unsigned char indicator_type_enum;
#define IND_BATTCHG        0
#define IND_SIGNAL         1
#define IND_SERVICE        2
#define IND_SOUNDER        3
#define IND_MESSAGE        4
#define IND_CALL           5
#define IND_VOX            6
#define IND_ROAM           7
#define IND_SMSFULL        8

/*
** Define the equipment to return the string mapping.
*/
typedef unsigned char equip_id_enum;
#define EQ_ME_MANUFACTURE_ID        0
#define EQ_ME_MODEL_ID              1
#define EQ_ME_REVISION_ID           2
#define EQ_ME_SERIAL_NUMBER_ID      3
#define EQ_ME_GLOBAL_OBJECT_ID      4
#define EQ_ME_MANUFACTURE_INFO_ID   5
#define EQ_ME_COMP_CAP_LIST_ID      6
#define MAX_ME_ID_NUM               7
#define MAX_ME_ID_LEN               30

/*
** Define the device process status.
*/
typedef unsigned char dev_status_enum;
#define DEV_OK             0 /* The device status is ok to process. */
#define DEV_FAIL           1 /* The device status is fail to process*/
#define DEV_FAIL_VM_NO_FREE_ENTRY   2 /* voice mail fail cause */
#define DEV_FAIL_VM_BUFFER_FULL     3
#define DEV_FAIL_VM_PLAY_NO_DATA    4

/**************************************************************
  **
  **      USB  Interface 
  ** 
  **************************************************************/
#if 0
#define  DEVUSB_DETECT_ACTION_UNKNOWN        0
#define  DEVUSB_DETECT_ACTION_PLUGIN         1
#define  DEVUSB_DETECT_ACTION_PLUGOUT        2
#define  DEVUSB_DETECT_ACTION_PLUGOUT_TIP    3
#define  DEVUSB_DETECT_ACTION_PLUGOUT_NO_TIP 4 
#define  DEVCHARGER_DETECT_ACTION_PLUGIN         5
#define  DEVCHARGER_DETECT_ACTION_PLUGOUT        6
#define  DEVUSB_STORAGE_SELECT_TIP               7
#define  DEVUSB_DETECT_ACTION_MAX_ID         8

#define  DEVUSB_PLUGIN_TIP_OUT        		  0x61
#define  DEVUSB_STORAGE_SELECT_TIP_OUT        0x67
#define  DEVCHARGER_PLUGIN_TIP_OUT        	  0x65
#else
#define  DEVUSB_DETECT_ACTION_UNKNOWN        0
#define  DEVUSB_UNKNOWN_TIP_OUT              20
#define  DEVUSB_DETECT_ACTION_PLUGIN         1
#define  DEVUSB_PLUGIN_TIP_OUT        		 21
#define  DEVUSB_STORAGE_SELECT_TIP           2 //#define  DEVUSB_DETECT_ACTION_UDISK
//#define  DEVUSB_STORAGE_SELECT_TIP (DEVUSB_DETECT_ACTION_UDISK) & return value 22  //return selection value of USB tip dialog
#define  DEVUSB_STORAGE_TIP_ON               22  //return selection value of USB tip dialog
#define  DEVUSB_DETECT_ACTION_PLUGOUT        3  // request whether U disk tip is out ?
#define  DEVUSB_STORAGE_SELECT_TIP_OUT       23	// U disk tip is out
#define  DEVUSB_DETECT_ACTION_PLUGOUT_TIP    4 // to popup plugout tip

#define  DEVCHARGER_DETECT_ACTION_PLUGIN     31
#define  DEVCHARGER_PLUGIN_TIP_OUT        	 41
#define  DEVCHARGER_DETECT_ACTION_PLUGOUT    32

#endif

#define  DEVUSB_DETECT_FUNC_NONE            0
#define  DEVUSB_DETECT_FUNC_CHARGE          1
#define  DEVUSB_DETECT_FUNC_DATA            2
#define  DEVUSB_DETECT_FUNC_CHARGE_DATA     3

#define  DEVUSB_CFG_ACTION_UNKNOWN              0
#define  DEVUSB_CFG_ACTION_CDC_ACM               1
#define  DEVUSB_CFG_ACTION_MASS_STORAGE    2
#define  DEVUSB_CFG_ACTION_COMPOSITE           3
#define  DEVUSB_CFG_ACTION_WEBCAM           4
#define  DEVUSB_CFG_ACTION_MAX_ID                 5

#define USB_SWITCH_PORT_APP_DATA             0
#define USB_SWITCH_PORT_APP_DEBUG           1

#endif /* _DEVICE_H */

