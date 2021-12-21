/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/

#ifndef __CP_PMIC_CHARGER_API_H__
#define __CP_PMIC_CHARGER_API_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*ChgIntCallback)(void);

typedef enum {
	CHG_DISCONNECTED,
	CHG_LOW,
	CHG_NORMAL,
	CHG_HIGH
}CHG_STATUS;

void PM812_CHARGER_BIND_INTC(ChgIntCallback callback);
void PM812_CHARGER_INTC_ENABLE(BOOL onoff);
BOOL PM812_CHARGER_IS_DETECTED(void);
BOOL PM812_VBUS_IS_AVAILABLE(void);
CHG_STATUS PM812_CHARGER_STATUS(void);
void PM812_ENABLE_CHG_MEAS(void);
void PM812_DISABLE_CHG_MEAS(void);
UINT32 PM812_CHARGER_GET_VCHG_MEAS_VOL_MV(void);
void PM812_SET_VCHG_LOW_TH_VOL_MV(UINT32 vol_mv);
void PM812_SET_VCHG_UPP_TH_VOL_MV(UINT32 vol_mv);
UINT32 PM812_GET_VCHG_UPP_TH_VOL_MV(void);
UINT32 PM812_GET_VCHG_LOW_TH_VOL_MV(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* __CP_PMIC_CHARGER_API_H__*/
