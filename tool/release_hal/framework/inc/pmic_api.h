#ifndef __PMIC_API_H__
#define __PMIC_API_H__
#include "cp/__cp_pmic_api.h"
#include "cp/__cp_pmic_wdt.h"
#include "cp/__cp_ningbo_api.h"
#include "cp/__cp_pmic_onkey_api.h"
#include "cp/__cp_pmic_charger_api.h"
#include "cp/__cp_pmic_battery_api.h"
#include "cp/__cp_usb_cable.h"
#include "cp/__cp_EE_silentReset_api.h"
#include "cp/__cp_guilin_api.h"

#define ONKEY_WAKEUP 	(0x01<<0)
#define CHG_WAKEUP 		(0x01<<1)
#define EXTON_WAKEUP 	(0x01<<2)
#define RTC_ALARM_WAKEUP (0x01<<4)
#define FAULT_WAKEUP 	(0x01<<5)
#define BAT_WAKEUP 		(0x01<<6)
#define WAKEUP_MASK     0x77

#define PM813ONKEY_WAKEUP   0x1
#define PM813EXTON1_WAKEUP 	0x2
#define PM813EXTON2_WAKEUP 	0x4
#define PM813BAT_WAKEUP     0x8
#define PM813RTC_ALARM_WAKEUP 0x10
#define PM813FAULT_WAKEUP 	0x20
#define PM813USB_WAKEUP    0x40
#define PM813WAKEUP_MASK     0x7F

#define AutoTest_Flg	0x1
#define FactorySetting_Flg	0x2
#define Fota_Flag	0x4
#define UserData_Mask 0x7
#define Before_silentBootupCharger_Flag 0x8

typedef enum{
    CAMERA_AVDD = 0,
    CAMERA_IOVDD,
    CAMERA_DVDD,
}LDO_CHANNEL;

#ifdef __cplusplus
extern "C" {
#endif

/*Only support port 5,6,8,9,11-13,15-18*/
int pmic_ldo_enable(unsigned int ldo);

/*Only support port 5,6,8,9,11-13,15-18*/
int pmic_ldo_disable(unsigned int ldo);

/*Only support port 5,6,8,9,11-13,15-18*/
int pmic_set_ldo_vol(unsigned int ldo);
void pmic_power_down(void);
void pmic_sw_reset(void);
unsigned int pmic_onkey_is_detected(void);
UINT8 pmic_get_powerup_reason(void);
//void pmic_wdg_for_shell_enable(void);
//void pmic_wdg_for_shell_disable(void);
void pmic_wdg_enable(PMIC_WD_TIMEOUT timeout);
void pmic_wdg_disable(void);
void pmic_wd_kick(void);
void PmickeypadbacklightEnable(void);
void PmickeypadbacklightDisable(void);
void PmicTorchLightEnable(void);
void PmicVibrateSetLevel(UINT8 level);
void PmicTorchLightDisable(void);
void PmicLcdBackLightEnable(void);
void PmicLcdBackLightDisable(void);
BOOL Pmic_is_pm812(void);
BOOL Pmic_is_pm813(void);
BOOL Pmic_is_pm802(void);
void PmicLcdBackLightCtrl(UINT8 level);
unsigned int Pmic_if_charge_full(void);
void Pmic_ChargerFinished(BOOL stop);
UINT8 pmic_get_powerdown_reason(void);
void pmic_fault_clear(void);
CHGFSM_OUT pmic_get_charger_fsm(void);
void pmic_precharge_current_set(unsigned char value);
void pmic_charge_termination_current_set(unsigned char value);
void pmic_cc_current_set(unsigned char value);
void pmic_vbat_set(unsigned char value);
void pmic_trickle_timer_set(unsigned char value);
void pmic_precharge_timer_set(unsigned char value);
void pmic_cccv_timer_set(unsigned char value);
int pm812_get_charger_status(void);
unsigned int pm812_get_batt_vol(void);
int pm812_get_battert_status(void);
void PMIC_LONKEY_PRESS_TIME_SET(unsigned char value);
void PMIC_LONKEY_EN(LONGKEY_COUNT num,BOOL enable);
void fm_pmic_power_down(void);
void fm_pmic_power_up(void);
void PmicTorchLightCtrl(UINT8 level);//should be 0~10, 0 :switch off
void camera_ldo_cfg(unsigned int on_off,LDO_CHANNEL channel,Camera_Pwr_Val ldoVal);
BOOL charger_is_usb(VOID);
UINT16 pmic_get_vbus_vol(void);
BOOL startup_is_silent_reset(void);
unsigned char pm813_userdata_reg_read(void);
void pm813_userdata_reg_write(unsigned char value);
void pm813_userdata_reg_clear(unsigned char value);
UINT32 pm813_a3_get_charger_cur_mA(void);
BOOL Pmic_is_pm813_a1(void);
BOOL Pmic_is_pm813_a2(void);
BOOL Pmic_is_pm813_a3(void);
UINT32 Pmic_get_battemp_kohm(void);
void pm813_auto_set_max_cc_current(UINT32 cc_limit);
BOOL pm813_battery_current_direction(void);
UINT32 pm813_get_batteryId_vol(void);
BOOL isLcdEverInitedUponBoot(void);
BOOL get_pmic_lcdbacklight_status(void);
BOOL get_pmic_keypadbacklight_status(void);
BOOL get_pmic_vibrator_status(void);
BOOL get_pmic_torchlight_status(void);
//FORCE_CHG_STATE pm813_get_force_charger_fsm(void);
UINT32 PM802_BATTERY_GET_VBAT_MEAS_VOL_MV(void);
void pm802_ldo1_active(void);
void fm_rda5802_power_up(void);
BOOL pm813_get_charger_cv_mode(void);
BOOL GetBacklightStatusBeforeSilentreset(void);
BOOL PM813_get_Vbus_ov_uv_status(void);
void bt_fm_power_on(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif

