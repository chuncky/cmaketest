/*------------------------------------------------------------
(C) Copyright [2019] ASR Microelectronics (Shanghai) Co., Ltd.
All Rights Reserved
------------------------------------------------------------*/
/**************************************************************
 *
 * header file for silentreset
 *
 **************************************************************/

#ifndef __EE_SILENTRESET_H__
#define __EE_SILENTRESET_H__


typedef enum
{
	/* @ENUM_DESC@ Configures reset mode*/
    STARTUP_POWERON,               /* @ENUM_VAL_DESC@ StartupMode STARTUP_POWERON */
    STARTUP_RESET,               /* @ENUM_VAL_DESC@ StartupMode STARTUP_RESET */
	STARTUP_SILENTRESET,            /* @ENUM_VAL_DESC@ StartupMode STARTUP_SILENTRESET */
#ifdef CRANE_OTA_SD_SUPPORT
	STARTUP_OTA_SD					/* @ENUM_VAL_DESC@ StartupMode STARTUP_OTA_SD */
#endif

}StartupMode_t;


extern StartupMode_t eeStartupMode;

StartupMode_t Startupmode_get(void);
BOOL silentReset_Save(void);
void silentReset_Check(void);
//extern EE_Configuration_t eeConfiguration;

BOOL Is_silentReset(void);

INT32 write_privatearea_keepdata(UINT8 *buf,INT32 len);
INT32 read_privatearea_keepdata(UINT8 *buf,INT32 len);
INT32 clear_privatearea_keepdata(void);
void set_cprunmode(int runmode);
int get_cprunmode(void);


#endif //__EE_SILENTRESET_H__
