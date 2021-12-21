/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/

#ifndef __CMID_MULTI_SIM_H__
#define __CMID_MULTI_SIM_H__

#include "uhaltype.h"

#ifndef NUMBER_OF_SIM
# define NUMBER_OF_SIM   2
#endif

#define CMID_SIM_NUMBER NUMBER_OF_SIM

#if (CMID_SIM_NUMBER!=1)
#define CMID_MULTI_SIM
#endif

#ifdef CMID_MULTI_SIM
typedef enum {
    CMID_SIM_0 = 0x00,
    CMID_SIM_1 = 0x01,
    CMID_SIM_COUNT = CMID_SIM_NUMBER,
    CMID_SIM_END = 0xFF
}CMID_SIM_ID;
#endif


#ifdef __cplusplus
extern "C" {
#endif

VOID cmid_IMSItoASC(UINT8 *InPut,UINT8 *OutPut, UINT8 *OutLen);

#ifdef CMID_MULTI_SIM
void CMID_CfgGetIMSI(UINT8* pIMSI, CMID_SIM_ID nSimID);
void CMID_CfgSetIMSI(UINT8* pIMSI, CMID_SIM_ID nSimID);
#else
void CMID_CfgGetIMSI(UINT8* pIMSI);
void CMID_CfgSetIMSI(UINT8* pIMSI);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __CMID_MULTI_SIM_H__ */
