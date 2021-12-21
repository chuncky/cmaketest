/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/

#ifndef __CP_PMIC_BATTERY_API_H__
#define __CP_PMIC_BATTERY_API_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*BatIntCallback)(void);

typedef enum {
	BAT_DETEC,
	BAT_NOT_DETEC,
}BAT_STATUS;

void PM812_BATTERY_BIND_INTC(BatIntCallback callback);
void PM812_BATTERY_INTC_ENABLE(BOOL onoff);
void PM812_ENABLE_BAT_DET(BOOL onoff);
BAT_STATUS PM812_BATTERY_STATUS(void);
void PM812_BATTERY_TID_MEAS_ENABLE(BOOL onoff);
UINT32 PM812_BATTERY_TID_MEAS(void);
UINT32 PM812_BATTERY_GET_VBAT_MEAS_VOL_MV(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* __CP_PMIC_BATTERY_API_H__*/
