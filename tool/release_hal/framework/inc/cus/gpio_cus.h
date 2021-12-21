#ifndef __GPIO_CUS_H__
#define __GPIO_CUS_H__

extern void GpioCusConfig(void);
extern void LcdBacklightonoff(unsigned int on_off);
extern void CameraFlashPowerOnOff(unsigned char on_off);
extern void CameraPwdn0OnOff(unsigned int on_off);
extern void CameraPwdn1OnOff(unsigned int on_off);
extern void CameraPwdn2OnOff(unsigned int on_off);
extern void CameraRstOnOff(unsigned int on_off);
extern void BtLdoEnonoff(unsigned int on_off);
extern void KeypadLedonoff(unsigned int on_off);
extern void ChargerEnonoff(unsigned int on_off);
extern void KeypadPinmuxCfg(void);
extern void VibratorSetLevel(unsigned int level);
extern void LcdBlPinmuxcfg(void);
extern void CameraFlashPinmuxcfg(void);
extern void ChargerStsPinmuxCfg(void);
extern void CameraI2CPinmuxCfg(unsigned int flag);
extern void Camera_Pinmuxcfg(unsigned int interface);
extern void LCD_Pinmuxcfg(uint32 interface);
extern void TorchlightSwitch(unsigned char on_off);
extern void TorchlightSwitchOn(void);
extern void TorchlightSwitchOff(void);
extern void LcdBacklightSwitchOn(void);
extern void LcdBacklightSwitchOff(void);
extern void LcdBacklightSetLevel(unsigned char level);
extern void KeypadBlackLightSwitchOn(void);
extern void KeypadBlackLightSwitchOff(void);
extern void VibratorSwitchOn(void);
extern void VibratorSwitchOff(void);
extern void VibratorSetLevel(unsigned int level);
extern void SpeakerExtPaEnable(uint32 on);
extern void EnableSpeakerPA(uint32 on);
extern BOOL get_lcdbacklight_status(void);
extern BOOL get_keypadbacklight_status(void);
extern BOOL get_vibrator_status(void);
extern BOOL get_torchlight_status(void);
extern void Evb1601_Pm802ChargerEnonoff(unsigned int on_off);
extern void Evb1601_Pm802Backlightonoff(unsigned int on_off);
#endif
