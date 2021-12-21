
#ifndef __TM_H__
#define __TM_H__

#include "plat_types.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __CALENDAR_ARABIC__ // SINJI

#define WEEKSTARTDAY_FRI      0x00
#define WEEKSTARTDAY_SAT      0x01
#define WEEKSTARTDAY_SUN      0x02
#define WEEKSTARTDAY_MON      0x03

typedef enum 
{
   DATE_GREGORIAN =0,
   DATE_HIJRI   =1,
   DATE_SHAMSI  = 2,
} DATE_SYS_ENUM;

typedef enum 
{
   WORD_BEGIN, WORD_MIDDLE, WORD_END
} enum_pos;

typedef enum
{
   SHAPE_ISOLATED, SHAPE_INITIAL, SHAPE_MEDIAL, SHAPE_FINAL
} enum_shape;

typedef struct t_ArabicTrans
{
   u16   isolated;
   u16   initial;
   u16   medial;
   u16   final;
} t_ArabicTrans;

typedef struct t_CharDefine
{
   unsigned char      isValid;
   unsigned char      isArabic;
   unsigned char      isNonBinding;
} t_CharDefine;

typedef struct t_LigatureTrans
{
   UINT16   code;
   UINT16   isolated;
   UINT16   initial;
   UINT16   medial;
   UINT16   final;
} t_LigatreTrans;

#endif  // __CALENDAR_ARABIC__

#define TM_STARTYEAR            2000
#define TM_ENDYEAR              2037

typedef struct {
   UINT16   year;
   UINT8   month;
   UINT8   day;
   UINT8   weekday;   
} Date_t;


typedef struct _FILETIME_TYPE_ { 
	unsigned int DateTime;
}TM_FILETIME, *TM_PFILETIME;

typedef struct _TM_SYSTEMTIME { 
    unsigned short uYear;
    unsigned char  uMonth;
    unsigned char  uDayOfWeek;
    unsigned char  uDay;
    unsigned char  uHour;
    unsigned char  uMinute;
    unsigned char  uSecond;
    unsigned short uMilliseconds;
} TM_SYSTEMTIME, *TM_PSYSTEMTIME;

typedef struct _TM_ALARM {
  unsigned int nFileTime;
  unsigned char* pText;
  unsigned char nTextLength;
  unsigned char nRecurrent;
  unsigned char padding[2];
  unsigned short nIndex;
  unsigned short nType;
} TM_ALARM;

typedef struct _TM_SMS_TIME_STAMP { 
	unsigned short uYear;
	unsigned char uMonth;
	unsigned char uDay;
	unsigned char uHour;
	unsigned char uMinute;
	unsigned char uSecond;
	char iZone;
}TM_SMS_TIME_STAMP;

#define  TM_SET_ALARM_RECUR(mon, tue, wed, thu, fri, sta, sun ) \
 ((1<<mon|1<<tue|1<<wed|1<<thu|1<<fri|1<<sta|1<<sun)&0xFE)

#define TM_FMT_DOT_TIME 		(1<<0) 	//0000000000000001--->10.33
#define TM_FMT_COLON_TIME 		(1<<1) 	//0000000000000010--->10:33

#define TM_FMT_TWELVE_TIME 		(1<<2) 	//0000000000000100--->2:33pm
#define TM_FMT_TWENTYFOUR_TIME	(1<<3) 	//0000000000001000--->14:33 

#define TM_FMT_DMY_DATE 		(1<<4) 	//0000000000010000--->13/06/2005
#define TM_FMT_MDY_DATE 		(1<<5) 	//0000000000100000--->06/13/2005
#define TM_FMT_YMD_DATE 		(1<<6) 	//0000000001000000--->2005/06/13

#define TM_FMT_DOT_DATE 		(1<<7) 	//0000000010000000--->2005.26.13
#define TM_FMT_COLON_DATE 		(1<<8) 	//0000000100000000--->2005:26:13
#define TM_FMT_SLASH_DATE 		(1<<9) 	//0000001000000000--->2005/06/13
#define TM_FMT_BASELINE_DATE 	(1<<10) //0000010000000000--->2005-06-13

#define TM_MODULE_BASE_ERR      0x0
//read register error
#define TM_OPEN_REG_ERROR             (TM_MODULE_BASE_ERR+1)
#define TM_CLOSE_REG_ERROR            (TM_MODULE_BASE_ERR+2)
#define TM_READ_REG_ERROR             (TM_MODULE_BASE_ERR+3)
#define TM_WRITE_REG_ERROR            (TM_MODULE_BASE_ERR+4)
#define TM_PARA_NULL_ERROR            (TM_MODULE_BASE_ERR+5)
#define TM_ALARM_TIME_MIN_ERROR       (TM_MODULE_BASE_ERR+6)
#define TM_ALARM_TIME_EQUAL_ERROR     (TM_MODULE_BASE_ERR+7)
#define TM_ALARM_RECURRENT_ERROR      (TM_MODULE_BASE_ERR+8)
#define TM_ALARM_INDEX_ERROR          (TM_MODULE_BASE_ERR+9)
#define TM_ALARM_TEXT_LEN_ERROR       (TM_MODULE_BASE_ERR+10)
#define TM_WRITE_FLASH_ERROR          (TM_MODULE_BASE_ERR+11)
#define TM_READ_FLASH_ERROR           (TM_MODULE_BASE_ERR+12)
#define TM_ALARM_UNKNOWN_ERROR        (TM_MODULE_BASE_ERR+13)
#define TM_NOT_VALID_YEAR_ERROR       (TM_MODULE_BASE_ERR+14)
#define TM_NOT_VALID_MONTH_ERROR      (TM_MODULE_BASE_ERR+15)
#define TM_NOT_VALID_DAY_ERROR        (TM_MODULE_BASE_ERR+16)
#define TM_NOT_VALID_HOUR_ERROR       (TM_MODULE_BASE_ERR+17)
#define TM_NOT_VALID_MINNUTE_ERROR    (TM_MODULE_BASE_ERR+19)
#define TM_NOT_VALID_SECOND_ERROR     (TM_MODULE_BASE_ERR+20)
#define TM_NOT_VALID_FORMATE_ERROR    (TM_MODULE_BASE_ERR+21)

BOOL  TM_SystemTimeToFileTime(
    CONST TM_SYSTEMTIME * pSystemTime,
    TM_FILETIME * pFileTime
);

BOOL  TM_FileTimeToSystemTime (
    CONST TM_FILETIME nFileTime,
    TM_SYSTEMTIME * pSystemTime
);

BOOL  TM_FormatSystemTime(
    CONST TM_SYSTEMTIME * pSystemTime,
    unsigned short uFormat,
    RESID nResFormatID,
    unsigned char * pDateString,
    unsigned char uDateStringSize,
    unsigned char * pTimeString,
    unsigned char uTimeStringSize
);



BOOL  TM_FormatFileTime(
    CONST TM_FILETIME nFileTime,
    unsigned short uFormat, 
    RESID nResFormatID, 
    PSTR pDateString,
    unsigned char uDateStringSize,
    PSTR pTimeString, 
    unsigned char uTimeStringSize
);

unsigned int  TM_GetTime (
    VOID
);

BOOL  TM_SetSystemTime(
    TM_SYSTEMTIME * pSystemTime
);

VOID  TM_autoSyncSystemTime(
    VOID
);
BOOL  TM_NTPSyncRtcTimeFlag(
    VOID
);
BOOL  TM_PmicRtcInitFlag(
    VOID
);

VOID TM_SetDateSystem(UINT8 dateSystem);

BOOL  TM_GetSystemTime(
    TM_SYSTEMTIME * pSystemTime
);

BOOL  TM_SetTimeFormat(
  unsigned short nTimeFormat	  //The time format parameter to be set.
);
unsigned short   TM_GetTimeFormat(void);

char  TM_GetTimeZone(
    VOID
);


BOOL   TM_GetSystemFileTime(
    TM_FILETIME *pFileTime
);

BOOL  TM_FormatDateTime(
    TM_FILETIME *pFileTime, 
    unsigned short uFormat, 
    RESID nResFormatID,
    unsigned char * pDateString,
    unsigned char uDateStringSize,
    unsigned char * pTimeString,
    unsigned char uTimeStringSize
);

BOOL  TM_SetLocalTime(
                           TM_SYSTEMTIME* pSystemTime
                           );
BOOL  TM_GetLocalTime (
                            TM_SYSTEMTIME* pSystemTime
                            );
BOOL  TM_FormatDateTimeEx(
    TM_FILETIME *pFileTime,
    unsigned char * pString
);

BOOL  TM_FormatFileTimeToStringEx(
    TM_FILETIME FileTime,
    unsigned char String[18]
);


unsigned char  TM_SmsTimeStampToFileTime(
    TM_SMS_TIME_STAMP ts,
    TM_FILETIME* pFileTime
);

BOOL  TM_SetAlarm(
    TM_ALARM* pAlarm
);
PUBLIC VOID TM_GetAlarm(TM_SYSTEMTIME* alarm_time);
PUBLIC VOID TM_SetAlarmForce(TM_SYSTEMTIME* alarm_time);

BOOL  TM_ClearAllAlarm (
    VOID
);

void  TM_GetLastErr(unsigned int* pErrorCode);

PUBLIC unsigned int hal_TimGetUpTime(VOID);
PUBLIC UINT16 TM_GetWeeks(UINT16 v_Year, UINT8 v_Month, UINT8 v_Day);

#ifdef __CALENDAR_ARABIC__

UINT8 TM_GetDaysOneMonthByDateSystem(UINT16 nYear, UINT8 nMonth, DATE_SYS_ENUM nDateSystem);

PUBLIC VOID SetWeekStartDay(UINT8 start);
PUBLIC VOID SetCurrentDateSystem(UINT8 DateSystem);

PUBLIC UINT32 TM_GetDaysOneYear(UINT16 tmYear);
PUBLIC UINT8  TM_GetDaysOneMonth(UINT16 nYear,UINT8 nMonth);
PUBLIC int    TM_DateCompare( Date_t *pDate1, Date_t *pDate2);

PUBLIC UINT8    leap_persian(int iYear ); 

PUBLIC UINT16   GetYearPassedFromFirstDay(UINT32 FirstDate, UINT8 PassedDay, UINT8 DaysOfMonth);
PUBLIC UINT8      GetMonthPassedFromFirstDay(UINT32 FirstDate, UINT8 PassedDay, UINT8 DaysOfMonth);
PUBLIC UINT8      GetDayPassedFromFirstDay(UINT32 FirstDate, UINT8 PassedDay, UINT8 DaysOfMonth);
PUBLIC DATE_SYS_ENUM GetCurrentDateSystem(void);
PUBLIC VOID   TM_JalaliToGregorian(UINT16* g_y, UINT8* g_m, UINT8* g_d, UINT16 j_y, UINT8 j_m, UINT8 j_d);
PUBLIC VOID   TM_GregorianToJalali(UINT16* j_y, UINT8* j_m, UINT8* j_d, UINT16 g_y, UINT8 g_m, UINT8 g_d);

PUBLIC UINT16  TM_GetGregorianYearFromArabicDate(UINT16 aYear, UINT8 aMonth, UINT8 aDay);
PUBLIC UINT8   TM_GetGregorianMonthFromArabicDate(UINT16 aYear, UINT8 aMonth, UINT8 aDay);
PUBLIC UINT8   TM_GetGregorianDayFromArabicDate(UINT16 aYear, UINT8 aMonth, UINT8 aDay);
PUBLIC VOID    TM_SetGregorianDateFromArabicDate(UINT16* year, UINT8* month, UINT8* day, UINT16 aYear, UINT8 aMonth, UINT8 aDay);

PUBLIC UINT16  TM_GetArabicYear(UINT16 year, UINT8 month, UINT8 day);
PUBLIC UINT8   TM_GetArabicMonth(UINT16 year, UINT8 month, UINT8 day);
PUBLIC UINT8   TM_GetArabicDay(UINT16 year, UINT8 month, UINT8 day);
PUBLIC VOID    TM_SetArabicDate(UINT16* ArabicYear, UINT8* ArabicMonth, UINT8* ArabicDay, Date_t Date);
PUBLIC UINT8   TM_GetArabicDaysOfMonth(UINT16 aYear, UINT8 aMonth);

PUBLIC UINT16  TM_GetFarsiYear(UINT16 year, UINT8 month, UINT8 day);
PUBLIC UINT8   TM_GetFarsiMonth(UINT16 year, UINT8 month, UINT8 day);
PUBLIC UINT8   TM_GetFarsiDay(UINT16 year, UINT8 month, UINT8 day);
PUBLIC VOID    TM_SetFarsiDate(UINT16* FarsiYear, UINT8* FarsiMonth, UINT8* FarsiDay, Date_t date);
PUBLIC UINT8   TM_GetFarsiDaysOfMonth(UINT16 fYear, UINT8 fMonth);
PUBLIC UINT16  TM_GetGregorianYearFromFarsiDate(UINT16 fYear, UINT8 fMonth, UINT8 fDay);
PUBLIC UINT8   TM_GetGregorianMonthFromFarsiDate(UINT16 fYear, UINT8 fMonth, UINT8 fDay);
PUBLIC UINT8   TM_GetGregorianDayFromFarsiDate(UINT16 fYear, UINT8 fMonth, UINT8 fDay);
PUBLIC VOID    TM_SetGregorianDateFromFarsiDate(UINT16* year, UINT8* month, UINT8* day, UINT16 fYear, UINT8 fMonth, UINT8 fDay);

PUBLIC BOOL    TM_CheckDateAvailable(UINT16 gYear, UINT8 gMonth, UINT8 gDay);

PUBLIC UINT16  TM_GetWeeksByDateSystem(UINT16 AF_Year, UINT8 AF_Month, UINT8 AF_Day);

PUBLIC UINT8   TM_GetWhatFirstDay(UINT8 Day, UINT8 WhatToday);

PUBLIC VOID   SyncDateSystemDateFromGregorianDate(Date_t* p_InGregorianDate, Date_t* p_OutCurDateByDateSystem);
PUBLIC VOID   SyncGregorianDateFromDateSystemDate(Date_t* p_OutGregorianDate, Date_t* p_InCurDateByDateSystem);
PUBLIC VOID   SyncGregorianDateFromCurrentDateSystem(Date_t* p_OutGregorianDate);//MEA_insoon 090118  
PUBLIC UINT8     GetDaysOneMonthByCurrentDateSystem(UINT16 nYear, UINT8 nMonth);//MEA_insoon 090118  
PUBLIC Date_t *GetDateByGregorian(Date_t s_InGregorianDate, Date_t *pOutDate, DATE_SYS_ENUM vOutDateSystem);
PUBLIC VOID   GetGregorianDateByDateSystem(Date_t s_InDate, Date_t *p_OutGregorianDate, DATE_SYS_ENUM vInDateSystem);//MEA_insoon 090202  
PUBLIC UINT8     GetWhatDayByDateSystem(UINT16 Year, UINT8 Month, UINT8 Day, DATE_SYS_ENUM nDateSystem);//MEA_insoon 090202  

PUBLIC UINT8    GetRearrangedWhatDay(UINT8 a_Weekday);
PUBLIC UINT8    GetOriginalWhatDay(UINT8 a_WeekDay);

PUBLIC VOID   GetGregorianWeekDate(Date_t *p_CurDate, Date_t *p_WeekDate, UINT8 v_WeekDay);
PUBLIC VOID   GetArabicWeekDate(Date_t *p_CurArabicDate, Date_t *p_WeekDate, UINT8 v_WeekDay);
PUBLIC VOID   GetFarsiWeekDate(Date_t *p_CurFarsiDate, Date_t *p_WeekDate, UINT8 v_WeekDay);
#endif

#ifdef __cplusplus
}
#endif

#endif // __TM_H__

