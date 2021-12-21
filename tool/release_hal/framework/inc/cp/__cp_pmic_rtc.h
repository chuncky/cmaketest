#ifndef _PRTC_H_
#define _PRTC_H_

typedef struct rtc_time {
  int tm_sec;	//seconds [0,59]
  int tm_min;	//minutes [0,59]
  int tm_hour;  //hour [0,23]
  int tm_mday;  //day of month [1,31]
  int tm_mon;   //month of year [1,12]
  int tm_year; // since 1970
  int tm_wday; // sunday = 0
}t_rtc;

//20190513@xiaokeweng : RTC_APP_OFFSET implement ++
typedef enum {
	SYS_OFFSET,
	APP_OFFSET
}PMIC_RTC_OFFSET_TYPE;
//20190513@xiaokeweng : RTC_APP_OFFSET implement --


typedef enum {
	RTC_EXPIRE_1=1,
	RTC_EXPIRE_2=2 //RTC_EXPIRE AREA REUSED AS APP_OFFSET, SO ONLY RTC_EXPIRE_1 AVAILABLED
}RTC_EXPIRE_ID;

#define FEBRUARY		2
#define	STARTOFTIME		1970
#define SECDAY			86400L
#define SECYR			(SECDAY * 365)
#define UTC_8_DEFAULT_COUNT     (8*60*60)   //UTC+8 (Beijing) as default

typedef void (*AlCallback)(void);

static void PMIC_RTC_SetAlarm_id(RTC_EXPIRE_ID id, t_rtc *tmp,PMIC_RTC_OFFSET_TYPE type);
static void PMIC_RTC_GetAlarm_id(RTC_EXPIRE_ID id, t_rtc *tmp,PMIC_RTC_OFFSET_TYPE type);
static void PMIC_RTC_EnableAlarm_id(RTC_EXPIRE_ID id,BOOL onoff);


void PMIC_RTC_GetTime(t_rtc *tmp,PMIC_RTC_OFFSET_TYPE type);
void PMIC_RTC_SetTime(t_rtc *tmp,PMIC_RTC_OFFSET_TYPE type);
void PMIC_RTC_SetAlarm(t_rtc *tmp);
void PMIC_RTC_GetAlarm(t_rtc *tmp);
void PMIC_RTC_EnableAlarm(BOOL onoff);
void PMIC_RTC_init(AlCallback callback);
UINT32 PMIC_RTC_GetTime_Count(PMIC_RTC_OFFSET_TYPE type);
BOOL PMIC_RTC_is_alarm_powerup();
BOOL PMIC_RTC_IS_SYS_RTC_PEER_SYNCED();

t_rtc * PMIC_gmttime(const unsigned int * m_time);
//unsigned int time(unsigned int * retrievedTime);

UINT32 PMIC_RTC_GetTimezone(void);
void PMIC_RTC_tm_to_str(char *buf,UINT32 rtc_count);
#endif//_PRTC_H_
