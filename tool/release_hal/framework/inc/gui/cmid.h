/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/

#ifndef _CMID_INC_H_
#define _CMID_INC_H_

/* VincentWei > FIXME: we should remove all uses of this macro. */
#ifndef __NGUX_PLATFORM__
#define __NGUX_PLATFORM__
#endif
#include "plat_config_defs.h"

#include "uhaltype.h"
#include "errorcode.h"
#include "cmid_multi_sim.h"
#include "event.h"
#include "tm.h"

//#include "csw_mem_prv.h"

#if defined(CMID_SERVICE_DEV_SINGLE_DEMO)  //CT_PLATFORM!=CT_JADE
#pragma comment (lib,"cmid_shell.lib")
#pragma message ("Linking with cmid_sim.lib")
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define CMID_CC_IND_UTI       0x01
#define CMID_SS_IND_UTI       0x02
#define CMID_NW_IND_UTI       0x03
#define CMID_SIM_IND_UTI      0x04
#define CMID_PBK_IND_UTI      0x05

typedef struct _CMID_EVENT {
	UINT32 nEventId;
    UINT32 nParam1;
	UINT32 nParam2;
	UINT16 nUTI;
	UINT8 nType;
	UINT8 nFlag;
} CMID_EVENT;

typedef UINT32 (*PFN_CMID_EV_PORC)(CMID_EVENT* pEvent, VOID* pUserData);


#define CMID_NW_SRV_ID         0     // Indicate the network management  Service
#define CMID_SIM_SRV_ID        1     // Indicate the SIM management  Service
#define CMID_CC_SRV_ID         2     // Indicate the Call management Service
#define CMID_SS_SRV_ID         3     // Indicate the SS Service
#define CMID_SMS_MO_SRV_ID     4     // Indicate the SMS MO management  Service
#define CMID_SMS_MT_SRV_ID     5     // Indicate the SMS MT management  Service
#define CMID_SMS_CB_ID         6     // Indicate the SMS CB management  Service

#define CMID_GPRS_SRV_ID       7
#define CMID_PDP_SRV_ID        7
#define CMID_PSD_SRV_ID        7

#define CMID_CSD_SRV_ID        8
#define CMID_EMOD_SRV_ID       9   	 // Engineering Mode
#define CMID_APP_SRV_ID        10
#define CMID_TIMER_SRV_ID      11
#ifdef CMID_MULTI_SIM
#define CMID_MAX_SRV_ID        (11*CMID_SIM_COUNT)
#else
#define CMID_MAX_SRV_ID        11

#endif

#define CMID_PBK_SRV_ID        CMID_APP_SRV_ID

#define CMID_SRV_NW_EV_RSP     (0)
#define CMID_SRV_CC_EV_RSP     (1)
#define CMID_SRV_SS_EV_RSP     (2)
#define CMID_SRV_SIM_EV_RSP    (3)
#define CMID_SRV_SMS_EV_RSP    (4)
#define CMID_SRV_PBK_EV_RSP    (5)
#define CMID_SRV_GPRS_EV_RSP   (6)
#define CMID_SRV_FAX_EV_RSP    (7)
#define CMID_SRV_EV_IND        (8)
#define CMID_SRV_EV_MAX        (8)

#define CMID_SRV_EV_UNKNOW     (0xFF)

#define CMID_TELNUMBER_TYPE_INTERNATIONAL 145  //International number 
#define CMID_TELNUMBER_TYPE_NATIONAL      161  //National number. 
#define CMID_TELNUMBER_TYPE_UNKNOWN       129  //Unknown number type (default) 

//
// Basic Utility Macros
//
#define  CMID_SET_BIT(val, n)               ((val)|=(1<<n)) 
#define  CMID_CLR_BIT(val, n)               ((val)&=~(1<<n)) 
#define  CMID_IS_BIT_SET(val, n)            ((val) & (1<<n)) 
#define  CMID_IS_BIT_CLR(val, n)            (~((val) & (1<<n))) 
#define  CMID_SWITCH_BIT(val, n)            ((val)^=(1<<n))
#define  CMID_GET_BIT(val, n)               (((val) >> n) & 1)

#define  CMID_CFG_RECEIVE_STORAGE(sms, bm, sr) ((sms)|(bm)|(sr))
#define  CMID_CFG_SET_ALARM_RECUR(mon, tue, wed, thu, fri, sta, sun ) ((1<<mon|1<<tue|1<<wed|1<<thu|1<<fri|1<<sta|1<<sun)&0xFE)
#define CMID_CFG_CMER(mode,keyp,disp,ind,bfr) ((mode)|((keyp)<<2)|((disp)<<4)|((ind)<<6)|((bfr)<<8))
#define CMID_CFG_CMER_MODE(cmer)               (cmer&0x03)
#define CMID_CFG_CMER_KEYP(cmer)               (cmer>>2&0x03)
#define CMID_CFG_CMER_DISP(cmer)               (cmer>>4&0x03)
#define CMID_CFG_CMER_IND(cmer)                (cmer>>6&0x03)
#define CMID_CFG_CMER_BFR(cmer)                (cmer>>8&0x03)

#define  TEL_NUMBER_MAX_LEN  42

#define CMID_ASSERT(expr) \
if (!(expr)) {                       \
	char strAss[64] = {0,};	\
	sprintf(strAss, "File:%s Line:%d ASSERT", __FILE__, __LINE__);	\
	strAss[63] = '\0';\
	DBG_ASSERT(0, strAss); \
	}

typedef struct _CMID_DIALNUMBER { 
    UINT8* pDialNumber;
    UINT8 nDialNumberSize;
    UINT8 nType;
    UINT8 nClir;
    UINT8 padding[1];
} CMID_DIALNUMBER;

typedef struct _CMID_DIALNUMBER_ARRAY { 
    UINT8 nTelNumber[TEL_NUMBER_MAX_LEN];
    UINT8 nDialNumberSize;
    UINT8 nType;
    UINT8 nClir;
    UINT8 padding[1];
} CMID_DIALNUMBER_ARRAY;

typedef struct _CMID_TELNUMBER {
    UINT8 nTelNumber[TEL_NUMBER_MAX_LEN];
    UINT8 nType;
    UINT8 nSize;
    UINT8 padding[1];
} CMID_TELNUMBER;

UINT32  CMID_SetEventProc (
  UINT8 nServiceId,
  PVOID pUserData,
  PFN_CMID_EV_PORC pfnRspEvProc
);

#define CMID_PROC_DEFAULT           0
#define CMID_PROC_COMPLETE          1
#define CMID_PROC_CONTINUE          2 
#define CMID_PROC_NOT_CONSUME       4
#define CMID_PROC_DONT_FREE_MEM     8 // Tell CMID don't free the EVETN struct.
#define CMID_PROC_UNKOWN_ERR        0 

UINT32  CMID_BalSetEventPorc (PVOID pUserData, PFN_CMID_EV_PORC pfnProc);

UINT32  CMID_IsFreeUTI (UINT16 nUTI, UINT32 nServiceId);

UINT32  CMID_GetFreeUTI (UINT32 nServiceId, UINT8* pUTI);

#define CMID_CONTROL_CMD_POWER_ON   0
#define CMID_CONTROL_CMD_POWER_OFF  1
#define CMID_CONTROL_CMD_RESTART    2
#define CMID_CONTROL_CMD_RESET      3
#define CMID_CONTROL_CMD_RF_OFF     4

typedef struct _CMID_SPN_INFO{
  UINT8 nSPNameLen;
  UINT8 nSPNDisplayFlag;  
  UINT8 nSpnName[16];
} CMID_SPN_INFO;

UINT32  CMID_ShellControl (UINT8 nCommand);

//
// Configuration(CFG)  Management 
//

#define CMID_CHSET_UCS2 0 
#define CMID_CHSET_GSM  1 


#define CMID_CFG_IND_BC_EVENT           (1<<0)
#define CMID_CFG_IND_SQ_EVENT           (1<<1)
#define CMID_CFG_IND_NW_EVENT           (1<<2)
#define CMID_CFG_IND_SOUNDER_EVENT      (1<<3)
#define CMID_CFG_IND_SMS_RECEIVE_EVENT  (1<<4)
#define CMID_CFG_IND_CALL_EVENT         (1<<5)
#define CMID_CFG_IND_VOX_EVENT          (1<<6)
#define CMID_CFG_IND_ROAM_EVENT         (1<<7)
#define CMID_CFG_IND_SMS_FULL_EVENT     (1<<8)
#define CMID_CFG_IND_BIT_ERR_EVENT      (1<<9)
#define CMID_CFG_IND_USER_SQ_EVENT      (1<<10)

#define CMID_CFG_IND_EVENT(bc,sq,nw,sounder,sms_receive, call, vox, roam, sms_full, biterr) \
	((bc)|((sq)<<1)|((nw)<<2)|((sounder)<<3)|((sms_receive)<<4)|((call)<<5)|((vox)<<6)|((roam)<<7)|((sms_full)<<8|biterr<<9)

#define IS_CFG_IND_BC_EVENT(ind)           (ind>>0&1)
#define IS_CFG_IND_SQ_EVENT(ind)           (ind>>1&1)
#define IS_CFG_IND_NW_EVENT(ind)           (ind>>2&1)
#define IS_CFG_IND_SOUNDER_EVENT(ind)      (ind>>3&1)
#define IS_CFG_IND_SMS_RECEIVE_EVENT(ind)  (ind>>4&1)
#define IS_CFG_IND_CALL_EVENT(ind)         (ind>>5&1)
#define IS_CFG_IND_VOX_EVENT(ind)          (ind>>6&1)
#define IS_CFG_IND_ROAM_EVENT(ind)         (ind>>7&1)
#define IS_CFG_IND_SMS_FULL_EVENT(ind)     (ind>>8&1)
#define IS_CFG_IND_BIT_ERR_EVENT(ind)      (ind>>9&1)

#ifdef _WIN32
#pragma warning(disable: 4214)  // local variable *may* be used without init
#endif

typedef struct _CMID_IND_EVENT_INFO { 
  UINT32 bc:3;
  UINT32 sq:3;
  UINT32 nw_srv:1;
  UINT32 sms_receive:1;
  UINT32 call:1;
  UINT32 roam:1;
  UINT32 sms_full:1;
  UINT32 sounder :1;
  UINT32 bit_err:8;
} CMID_IND_EVENT_INFO;

typedef enum
{
    /// Chip product ID
    CMID_SYS_CHIP_PROD_ID,

    /// Chip bond ID
    CMID_SYS_CHIP_BOND_ID,

    /// Chip metal ID
    CMID_SYS_CHIP_METAL_ID,

    /// Chip ID whole
    CMID_SYS_CHIP_ID_WHOLE,

} CMID_SYS_CHIP_ID_T;

#ifdef CMID_MULTI_SIM

UINT32  CMID_CfgSetIndicatorEventInfo (CMID_IND_EVENT_INFO* pIndInfo, CMID_SIM_ID nSimID);

UINT32  CMID_CfgGetIndicatorEventInfo (CMID_IND_EVENT_INFO* pIndInfo, CMID_SIM_ID nSimID);
UINT32 CMID_ForceUpdateSignalLevel(CMID_SIM_ID nSimID);
UINT32 CMID_SetQualReport(BOOL bStart, CMID_SIM_ID nSimID);
UINT32 CMID_SetDataSimCard(CMID_SIM_ID nSimID);
UINT32 CMID_GetDataSimCard(VOID);
UINT32 CMID_SwitchDataSimCardSync(CMID_SIM_ID nSimID);
UINT32 CMID_RestoreDataSimCard(VOID);
BOOL CMID_IsDataEnabled(VOID);
BOOL CMID_IsDataConnectOK(VOID);
BOOL CMID_IsRoaming(CMID_SIM_ID sim_id);
BOOL CMID_IsDataOnRoaming(void);
void CMID_SetDataOnRoaming(BOOL is_data_on_roaming);
UINT32 CMID_SetDataEnabled(BOOL is_on);
UINT32 CMID_RestoreDataState(VOID);
UINT32 CMID_DisableDataTemp(VOID);
#else

UINT32  CMID_CfgSetIndicatorEventInfo (CMID_IND_EVENT_INFO* pIndInfo);

UINT32  CMID_CfgGetIndicatorEventInfo (CMID_IND_EVENT_INFO* pIndInfo);

UINT32 CMID_SetQualReport(BOOL bStart);

#endif

UINT32 CMID_CfgSetFirstPowerOnFlag(BOOL bFPO);

UINT32 CMID_CfgCheckFirstPowerOnFlag(BOOL *pbFPO);


typedef struct _CMID_EXTENDED_ERROR{ 
    UINT8 locationId;
    UINT8 reason;
    UINT8 ss_reason;
    UINT8 padding;
} CMID_EXTENDED_ERROR;







 UINT32  CMID_CfgSetConnectResultFormatCode(UINT8 nValue);

UINT32  CMID_CfgGetConnectResultFormatCode(UINT8 *nValue);

UINT32 CMID_CfgSetSoftReset(UINT8 cause);
UINT32 CMID_CfgGetSoftReset(UINT8 *cause);
UINT32 CMID_CfgSetResetCause(UINT8 cause);
UINT32 CMID_CfgGetResetCause(UINT8 *cause);
//SMS Configuration
#define CMID_SMS_STORAGE_AUTO    0x00 

#ifdef CMID_MULTI_SIM
// SMS module
//message storage
#define CMID_SMS_STORAGE_ME	0x01
#define CMID_SMS_STORAGE_SM	0x02
#define CMID_SMS_STORAGE_MT	0x20 
//message type
#define CMID_SMS_TYPE_PP	0X01
#define CMID_SMS_TYPE_CB	0X02
#define CMID_SMS_TYPE_SR 0X04
#else

#define CMID_SMS_STORAGE_ME      0x01 
#define CMID_SMS_STORAGE_SM      0x02 
#define CMID_SMS_STORAGE_BM      0x04 
#define CMID_SMS_STORAGE_SM_SR   0x08
#define CMID_SMS_STORAGE_ME_SR   0x10 
#define CMID_SMS_STORAGE_MT      0x20 
#define CMID_SMS_STORAGE_SM_ME   0x40


#endif

#define CMID_SMS_STORED_STATUS_UNREAD                     0x01
#define CMID_SMS_STORED_STATUS_READ                       0x02
#define CMID_SMS_STORED_STATUS_UNSENT                     0x04
#define CMID_SMS_STORED_STATUS_SENT_NOT_SR_REQ            0x08
#define CMID_SMS_STORED_STATUS_SENT_SR_REQ_NOT_RECV       0x10
#define CMID_SMS_STORED_STATUS_SENT_SR_REQ_NOT_STORE      0x20
#define CMID_SMS_STORED_STATUS_SENT_SR_REQ_RECV_STORE     0x40
#define CMID_SMS_STORED_STATUS_STORED_ALL                 0x7f 

#define CMID_SMS_COPY_ME2SM                               0
#define CMID_SMS_COPY_SM2ME                               1 

typedef struct _CMID_SMS_PARAMETER {
    UINT8 vp;
    UINT8 pid; 
    UINT8 dcs; 
    UINT8 mti; 
    UINT8 ssr;
    UINT8 bearer:2;
    UINT8 bSimChange:1;
    UINT8 bReserved:5;
    UINT8 mr;
    UINT8 rp;
    UINT8 nNumber[12]; // nNumber[0]=len,nNumber[1]=[91] nNumber[2]=[86] ... in BCD.
    UINT8 udhi;
    UINT8 rd;
    UINT8 vpf; 
    UINT8 nSaveToSimFlag;// 1 save sca to sim, 2 save pid to sim, 4 save dcs to sim, 8 save vp to sim, 0 not save to sim.
    UINT8 nIndex;//the index of param set, 0 to 4 is valuable
    BOOL bDefault;//if set param to current(Default) param, TRUE: save, FALSE, not save.
} CMID_SMS_PARAMETER;

typedef struct _CMID_NV_SMS_PARAMETER {
    UINT8 default_index[4];	/*sim0,sim1,sim2,sim3*/
	UINT8 option[4];		/*sim0,sim1,sim2,sim3*/
	UINT8 newsmsstorage[4];	/*sim0,sim1,sim2,sim3*/
	UINT8 fullfornew[4];	/*sim0,sim1,sim2,sim3*/
	CMID_SMS_PARAMETER nv_sms_param[MMI_SIM_NUMBER*5];
} CMID_NV_SMS_PARAMETER;

typedef struct _CMID_SMS_STORAGE_INFO {
    UINT16 usedSlot;
    UINT16 totalSlot;
    UINT16 unReadRecords;
    UINT16 readRecords;
    UINT16 sentRecords;
    UINT16 unsentRecords;
    UINT16 unknownRecords;
    UINT16 storageId;
} CMID_SMS_STORAGE_INFO;

typedef enum
{
    CMID_SMS_NOINITED = 0x00,
    CMID_SMS_WAITING_MSG = 0x01,
    CMID_SMS_INITED = 0x02
} CMID_SMS_INIT_STATUS;


#ifndef __CMID_PRV_H__

#define CMID_SIM_INIT_EV_TYPE_BASE    0x00
#define CMID_SMS_INIT_EV_TYPE_BASE    0x10
#define CMID_PBK_INIT_EV_TYPE_BASE    0x20
#define CMID_SAT_INIT_EV_TYPE_BASE    0x30

#define SIM_INIT_EV_TYPE0            CMID_SIM_INIT_EV_TYPE_BASE
#define SIM_INIT_EV_TYPE1            CMID_SIM_INIT_EV_TYPE_BASE+1
#define SIM_INIT_EV_TYPE2            CMID_SIM_INIT_EV_TYPE_BASE+2

#define SMS_INIT_EV_OK_TYPE          CMID_SMS_INIT_EV_TYPE_BASE
#define SMS_INIT_EV_ERR_TYPE         CMID_SMS_INIT_EV_TYPE_BASE+1

#define PBK_INIT_EV_OK_TYPE          CMID_PBK_INIT_EV_TYPE_BASE
#define PBK_INIT_EV_ERR_TYPE         CMID_PBK_INIT_EV_TYPE_BASE+1
#define PBK_EXIT_EV_OK_TYPE          CMID_PBK_INIT_EV_TYPE_BASE+2
#define PBK_EXIT_EV_ERR_TYPE         CMID_PBK_INIT_EV_TYPE_BASE+3

#ifndef _CMID_SMS_LONG_INFO_
#define _CMID_SMS_LONG_INFO_
typedef struct _CMID_SMS_LONG_INFO
{
  UINT16 index;
  UINT8 nStatus;
  UINT8 nIEIa;
  UINT8 count;
  UINT8 current;
  UINT16 id;
  UINT32* pNext;
} CMID_SMS_LONG_INFO;
#endif
#ifndef _CMID_SMS_INIT_INFO_
#define _CMID_SMS_INIT_INFO_
typedef struct _CMID_SMS_INIT_INFO
{
  UINT16 usedSlot;
  UINT16 totalSlot;
  UINT16 readRecords;
  UINT16 unReadRecords;
  UINT16 sentRecords;
  UINT16 unsentRecords;
  UINT16 unknownRecords;
  UINT16 storageId;
  UINT16 nLongStruCount;
  UINT8 padding[2];
    CMID_SMS_LONG_INFO* pLongInfo;
} CMID_SMS_INIT_INFO;
#endif
#endif

#ifndef __CMID_EV_PRIV__
#define __CMID_EV_PRIV__
typedef struct _CMID_EV
{
  UINT32 nEventId;
  UINT32 nTransId;  // only low 8bit is valid for all CMID applications.
  UINT32 nParam1;
  UINT32 nParam2;
  UINT8 nType;
  UINT8 nFlag;
  UINT8 padding[2];
} CMID_EV;
#endif

#define QUERY_AO_ILLEGAL(PAOC)\
    if(PAOC)\
    {    if(PAOC->nServiceID > CMID_MAX_SRV_ID)\
            return ERR_AOM_HAO_ILLEGAL;\
         else if(!(PAOC->UserData))\
            return ERR_AOM_HAO_ILLEGAL;\
    }\
    else\
        return ERR_AOM_HAO_ILLEGAL;


typedef UINT32 (*PFN_AO_PORC) (HAO hAo, CMID_EV* pEvent);

#define NADD_AO_TO_AOQUEUE	0
#define ADD_AO_TO_AOQUEUE	1
typedef struct _AO_CONTENT
{
    UINT32                UTI;
    UINT32                nProcCode;
    void                  *UserData;
    PFN_AO_PORC           pAO_Proc;
    UINT32                AO_State;
    UINT32                AO_NextState;////AO_NextState  1:we need processAO,add AO to AO_ControlBlock.0:we don't need add AO to AO_ControlBlock.
    
    PFN_CMID_EV_PORC    pfnEvProc;
    void                  *pEventUserData;
        
    struct _AO_CONTENT    *NextAO;
    UINT32                nServiceID;
#ifdef CMID_MULTI_SIM
    CMID_SIM_ID        nSimID;
#endif
} AO_CONTENT;




#ifdef CMID_MULTI_SIM

UINT32 CMID_CfgSetSmsStorage (UINT8 nStorage, CMID_SIM_ID nSimID);
                             
UINT32  CMID_CfgGetSmsStorage (UINT8* pStorage, CMID_SIM_ID nSimID);

UINT32  CMID_CfgSetNewSmsOption (UINT8 nOption, UINT8 nNewSmsStorage, CMID_SIM_ID nSimID);

UINT32  CMID_CfgGetNewSmsOption (UINT8* pOption, UINT8* pNewSmsStorage, CMID_SIM_ID nSimID);

UINT32  CMID_CfgSetSmsOverflowInd (UINT8 nMode, CMID_SIM_ID nSimID);

UINT32  CMID_CfgGetSmsOverflowInd (UINT8* pMode, CMID_SIM_ID nSimID);

UINT32  CMID_CfgSetSmsFormat (UINT8 nFormat, CMID_SIM_ID nSimID);

UINT32  CMID_CfgGetSmsFormat (UINT8* nFormat, CMID_SIM_ID nSimID);

UINT32  CMID_CfgSetSmsShowTextModeParam (UINT8 nShow, CMID_SIM_ID nSimID);

UINT32  CMID_CfgGetSmsShowTextModeParam (UINT8* pShow, CMID_SIM_ID nSimID);

UINT32  CMID_CfgSetSmsParam (CMID_SMS_PARAMETER * pInfo, UINT16 nUTI, CMID_SIM_ID nSimID);

UINT32  CMID_CfgGetSmsParam (CMID_SMS_PARAMETER* pInfo, UINT8 nIndex, CMID_SIM_ID nSimID);

UINT32  CMID_CfgSetSmsStorageInfo (CMID_SMS_STORAGE_INFO* pStorageInfo, UINT16 nStorage, CMID_SIM_ID nSimID);

UINT32  CMID_CfgGetSmsStorageInfo (CMID_SMS_STORAGE_INFO* pStorageInfo, UINT16 nStorage, CMID_SIM_ID nSimID);

#else

UINT32  CMID_CfgSetSmsStorageInfo (
  CMID_SMS_STORAGE_INFO* pStorageInfo,
  UINT16 nStorage
);

UINT32  CMID_CfgGetSmsStorageInfo (
  CMID_SMS_STORAGE_INFO* pStorageInfo,
  UINT16 nStorage
);

UINT32  CMID_CfgSetSmsStorage (
  UINT8 nStorage
);

UINT32  CMID_CfgGetSmsStorage (
  UINT8* pStorage
);

UINT32  CMID_CfgSetNewSmsOption (
  UINT8 nOption,
  UINT8 nNewSmsStorage
);

UINT32  CMID_CfgGetNewSmsOption (
  UINT8* pOption, 
  UINT8* pNewSmsStorage
);

UINT32  CMID_CfgSetSmsOverflowInd (
  UINT8 nMode
);

UINT32  CMID_CfgGetSmsOverflowInd (
  UINT8* pMode
);

UINT32  CMID_CfgSetSmsFormat (
  UINT8 nFormat
);

UINT32  CMID_CfgGetSmsFormat (
  UINT8* nFormat
);

UINT32  CMID_CfgSetSmsShowTextModeParam (
  UINT8 nShow
);

UINT32  CMID_CfgGetSmsShowTextModeParam (
  UINT8* pShow
);

UINT32  CMID_CfgSetSmsParam (
  CMID_SMS_PARAMETER* pInfo,//modified by fengwei 20080707 for save sca to sim 
  UINT16 nUTI
);

UINT32  CMID_CfgGetSmsParam (
  CMID_SMS_PARAMETER* pInfo,
  UINT8 nIndex
);

#endif



UINT32  CMID_CfgSelectSmsService(UINT8 nService, UINT8* pSupportedType);

UINT32  CMID_CfgQuerySmsService (UINT8* pService);

UINT32  CMID_CfgSetSmsCB (UINT8 mode, UINT8 mids[6], UINT8 dcss[6]);

UINT32  CMID_CfgGetSmsCB (UINT8* pMode, UINT8 mids[6], UINT8 dcss[6]);

// PBK Configuration
#define CMID_PBK_SIM                             0x01
#define CMID_PBK_ME                              0x02

#define CMID_PBK_ME_MISSED                       0x1
#define CMID_PBK_ME_RECEIVED                     0x2
#define CMID_PBK_SIM_FIX_DIALLING                0x4
#define CMID_PBK_SIM_LAST_DIALLING               0x8
#define CMID_PBK_ME_LAST_DIALLING                0x10
#define CMID_PBK_SIM_BARRED_DIALLING             0x11
#define CMID_PBK_SIM_SERVICE_DIALLING            0x12
#define CMID_PBK_ON                              0x20
#define CMID_PBK_EN                              0x40

#define CMID_PBK_AUTO                            0x00
#define CMID_PBK_ALL                             0x00

// Pbk Option
#define CMID_PBK_OPTION_BASIC                      0x00
#define CMID_PBK_OPTION_All                        0x01
#define CMID_PBK_OPTION_BASIC_NAME_DESCEND_ORDER   0x04
#define CMID_PBK_OPTION_BASIC_NAME_ASCEND_ORDER    0x08
#define CMID_PBK_OPTION_All_NAME_DESCEND_ORDER     0x10
#define CMID_PBK_OPTION_All_NAME_ASCEND_ORDER      0x20

typedef struct _CMID_PBK_STRORAGE_INFO { 
    UINT16 index;
    UINT8 iNumberLen;
    UINT8 txtLen;
} CMID_PBK_STRORAGE_INFO;

// NW Configuration

#define CMID_STORE_MAX_COUNT 33
typedef struct //this must be the same with api_NwStoreListInd_t
{
    u8 Plmn[3];
    u8 nCount;
    u16 ArfcnL[CMID_STORE_MAX_COUNT];
}CMID_StoredPlmnList;

#ifdef CMID_MULTI_SIM


UINT32 CMID_CfgSetStoredPlmnList(CMID_StoredPlmnList* pPlmnL, CMID_SIM_ID nSimId);

UINT32 CMID_CfgGetStoredPlmnList(CMID_StoredPlmnList* pPlmnL, CMID_SIM_ID nSimId);

UINT32  CMID_CfgNwSetFrequencyBand (UINT8 nBand, CMID_SIM_ID nSimID);

UINT32  CMID_CfgNwGetFrequencyBand (UINT8* pBand, CMID_SIM_ID nSimID);

UINT32  CMID_CfgSetNwStatus (UINT8 n, CMID_SIM_ID nSimID);

#else

UINT32  CMID_CfgNwSetFrequencyBand (
  UINT8 nBand
);

UINT32  CMID_CfgNwGetFrequencyBand (
  UINT8* pBand
);

UINT32  CMID_CfgSetNwStatus (
  UINT8 n
);

#endif


// CM Configuration
#ifdef CMID_MULTI_SIM



UINT32  CMID_CfgSetClip (UINT8 nEnable, CMID_SIM_ID nSimID);



void CMID_CfgGetIMSI(UINT8 * pIMSI, CMID_SIM_ID nSimID);
void CMID_CfgSetIMSI(UINT8 * pIMSI, CMID_SIM_ID nSimID);

#else


UINT32  CMID_CfgSetClip (
  UINT8 nEnable
);




void CMID_CfgGetIMSI(UINT8 * pIMSI);

void CMID_CfgSetIMSI(UINT8 * pIMSI);

#endif




UINT32 CMID_CfgGetIncomingCallResultMode(UINT8* nMode);

// Device Configuration
typedef struct _CMID_AUD_OUT_PARAMETER { 
    UINT8 outBbcGain;
    UINT8 outStep;
    UINT16 sideTone;
    UINT16 outCalibrate[4];
} CMID_AUD_OUT_PARAMETER;



//
// NW Service
//
#define CMID_NW_STATUS_NOTREGISTERED_NOTSEARCHING 0// Not registered and not searching. 
#define CMID_NW_STATUS_REGISTERED_HOME            1 //Registered and in the home area. 
#define CMID_NW_STATUS_NOTREGISTERED_SEARCHING    2 //Not registered and searching for a new operator. 
#define CMID_NW_STATUS_REGISTRATION_DENIED        3 //Registration denied. 
#define CMID_NW_STATUS_UNKNOW                     4 //Unknown registration. 
#define CMID_NW_STATUS_REGISTERED_ROAMING         5 

#define CMID_NW_AUTOMATIC_MODE              0
#define CMID_NW_MANUAL_MODE                 1
#define CMID_NW_MANUAL_AUTOMATIC_MODE       4

#define CMID_NW_OPERATOR_STATUS_UNKNOW      0 //unknown
#define CMID_NW_OPERATOR_STATUS_AVAILABLE   1 //available
#define CMID_NW_OPERATOR_STATUS_CURRENT     2 //current
#define CMID_NW_OPERATOR_STATUS_FORBIDDEN   3  //forbidden

typedef struct _CMID_NW_NETWORK_INFO
{
    UINT8 nLocalTimeZoneLen;
    UINT8 nLocalTimeZone;
    UINT8 nUniversalTimeZoneLen;
    UINT8 nUniversalTimeZone[7];
    UINT8 nDaylightSavingTimeLen;
    UINT8 nDaylightSavingTime;
    UINT8 nNwkFullNameLen;
    UINT8 nNwkShortNameLen;
    UINT8 nNwkNames[247];
}CMID_NW_NETWORK_INFO;

typedef struct _CMID_NW_OPERATOR_INFO {
    UINT8 nOperatorId[6];
    UINT8 nStatus;
	UINT8 nAct;
} CMID_NW_OPERATOR_INFO;

typedef struct _CMID_NW_STATUS_INFO {
    UINT8 nStatus;
    UINT8 nCause;     //only use PS no service
    UINT8 padding[1];
    UINT8 nAreaCode[5];
    UINT8 nCellId[2];
} CMID_NW_STATUS_INFO;

#ifdef CMID_MULTI_SIM

typedef enum {
  CMID_APPTYPE_UNKNOWN = 0,
  CMID_APPTYPE_SIM     = 1,
  CMID_APPTYPE_USIM    = 2,
  CMID_APPTYPE_RUIM    = 3,
  CMID_APPTYPE_CSIM    = 4,
  CMID_APPTYPE_ISIM    = 5
} CMID_AppType;

typedef enum {
    CMID_CARDSTATE_ABSENT     = 0,
    CMID_CARDSTATE_PRESENT    = 1,
    CMID_CARDSTATE_ERROR      = 2,
    CMID_CARDSTATE_RESTRICTED = 3  /* card is present but not usable due to carrier restrictions.*/
} CMID_CardState;


UINT32  CMID_NwGetAvailableOperators (UINT16 nUTI, CMID_SIM_ID nSimID);

UINT32  CMID_NwGetCurrentOperator (UINT8 OperatorId[6], UINT8* pMode, CMID_SIM_ID nSimID);
BOOL CMID_NwIsOperatorRegistered(CMID_SIM_ID nSimID);
BOOL CMID_NwIsNetworkRegistered(CMID_SIM_ID sim_id);
BOOL CMID_NwIsVolteEnabled(CMID_SIM_ID sim_id);

BOOL CMID_NwIsVolteRegistered(CMID_SIM_ID sim_id);

char* CMID_NwGetImei (CMID_SIM_ID nSimID);
void CMID_NwSetImei (CMID_SIM_ID sim_id, char* pstrIMEI);

UINT32  CMID_NwGetSignalQuality (UINT8* pSignalLevel, UINT8* pBitError, CMID_SIM_ID nSimID);

UINT8 CMID_NwGetNetworkTechMode (CMID_SIM_ID sim_id);

UINT32  CMID_NwGetStatus (CMID_NW_STATUS_INFO* pStatusInfo, CMID_SIM_ID nSimID);

BOOL CMID_NwIsSimOn(CMID_SIM_ID nSimID);

BOOL CMID_NwIsAirplaneMode(void);

UINT32  CMID_NwSetRegistration (UINT8* nOperatorId, UINT8 act, UINT8 nMode, UINT16 nUTI, CMID_SIM_ID nSimID);

UINT32  CMID_NwDeRegister (UINT16 nUTI, CMID_SIM_ID nSimID);
CMID_AppType CMID_GetSimType(CMID_SIM_ID sim_id);
CMID_CardState CMID_GetSimState(CMID_SIM_ID sim_id);

//cus api
char* cus_api_removeCountryCode(char* number);
UINT32 cus_api_TransferSignalBarLevel(int signal_value, int radioTech);

#ifdef CMID_MULTI_SIM
typedef enum  {
    CMID_DISABLE_COMM = 0x00,   //don't start
    CMID_ENABLE_COMM,  		// start stack
    CMID_CHECK_COMM 
}CMID_COMM_MODE;
#endif

UINT32  CMID_NwSetFrequencyBand (
  UINT8 nBand,
  UINT16 nUTI,
  CMID_SIM_ID nSimID
);


UINT32  CMID_NwAbortListOperators(
  UINT16 nUTI,
  CMID_SIM_ID nSimID
);
    
#else

UINT32  CMID_NwGetAvailableOperators (
  UINT16 nUTI
);


UINT32  CMID_NwGetCurrentOperator (
  UINT8 OperatorId[6],
  UINT8* pMode
);

char* CMID_NwGetImei (CMID_SIM_ID nSimID);

UINT32  CMID_NwGetSignalQuality (
  UINT8* pSignalLevel,
  UINT8* pBitError
);

UINT32  CMID_NwGetStatus (
  CMID_NW_STATUS_INFO* pStatusInfo
);


UINT32  CMID_NwSetRegistration (
  UINT8 nOperatorId[6],
  UINT8 nMode,
  UINT16 nUTI
);

UINT32  CMID_NwDeRegister (
  UINT16 nUTI
);

UINT32 CMID_SetFlightMode(
  UINT8 bFM, UINT8 nUTI
);

//FMode: 0 : nomal mode; 1: flight mode; 2: query mode
UINT32 CMID_WriteFlightMode(UINT8 fMode
);

UINT32 CMID_GetFlightMode( 
UINT8 *pFM 
);

UINT32  CMID_NwSetFrequencyBand (
  UINT8 nBand,
  UINT16 nUTI
);


UINT32  CMID_NwAbortListOperators(UINT16 nUTI);

#endif

#define CMID_NW_BAND_GSM_450   ( 1 << 0 )
#define CMID_NW_BAND_GSM_480   ( 1 << 1 )
#define CMID_NW_BAND_GSM_900P  ( 1 << 2 )
#define CMID_NW_BAND_GSM_900E  ( 1 << 3 )
#define CMID_NW_BAND_GSM_900R  ( 1 << 4 )
#define CMID_NW_BAND_GSM_850   ( 1 << 5 )
#define CMID_NW_BAND_DCS_1800  ( 1 << 6 )
#define CMID_NW_BAND_PCS_1900  ( 1 << 7 )


//
// CM Service
//

#define CMID_CM_NORMAL_CLEARING 16
#define CMID_CM_USER_BUSY    17 
#define CMID_CM_CALL_REJECTD    21
#define CMID_CM_HOLDMPTY_UTI 1


#define	CMID_CM_MODE_VOICE      0
#define	CMID_CM_MODE_DATA       1
#define	CMID_CM_MODE_FAX        2

//Call direction
#define CMID_DIRECTION_MOBILE_ORIGINATED 0
#define CMID_DIRECTION_MOBILE_TERMINATED 1

//Call state
#define	CMID_CM_STATUS_ACTIVE   0
#define	CMID_CM_STATUS_HELD     1
#define	CMID_CM_STATUS_DIALING  2  //MO CALL)
#define	CMID_CM_STATUS_ALERTING 3  //(MO CALL)
#define	CMID_CM_STATUS_INCOMING 4  //(MT CALL)
#define	CMID_CM_STATUS_WAITING  5  //(MT call)
#define	CMID_CM_STATUS_OFFERING 6  //(MT call)
#define CMID_CM_STATUS_RELEASED 7
#define CMID_CM_STATUS_PRE_DIALING	8

#define CC_STATE_NULL		0x00
#define CC_STATE_ACTIVE		0x01
#define CC_STATE_HOLD		0x02
#define CC_STATE_WAITING	0x04
#define CC_STATE_INCOMING	0x08
#define CC_STATE_DIALING	0x10
#define CC_STATE_ALERTLING	0x20
#define CC_STATE_RELEASE	0x40

typedef struct _CMID_SPEECH_CALL_IND { 
  CMID_TELNUMBER TelNumber;
  UINT8 nCode;
  UINT8 nCLIValidity;
  UINT8 nDisplayType;
  UINT8 padding;
} CMID_SPEECH_CALL_IND;

typedef struct _CMID_CC_CURRENT_CALL_INFO {
 CMID_TELNUMBER dialNumber;
 BOOL multiparty;
 UINT8 idx;
 UINT8 direction;//0:MO 1:MT
 UINT8 status;
 UINT8 mode;
 UINT8 padding[3];//0:;1:is emregency num;2:sim id;
#if defined(__NGUX_PLATFORM__)
 UINT8 ti;
#endif
} CMID_CC_CURRENT_CALL_INFO;

typedef struct _CMID_SS_QUERY_CALLFORWARDING_RSP { 
    CMID_TELNUMBER nNumber;
    UINT8 nStatus;
    UINT8 nClass;
    UINT8 nReason;
    UINT8 nTime;
} CMID_SS_QUERY_CALLFORWARDING_RSP ;
typedef struct _CMID_SS_SET_CALLFORWARDING_INFO { 
    UINT8 nReason;
    UINT8 nMode;
    UINT8 nClass;
    UINT8 nTime;
    CMID_DIALNUMBER_ARRAY nNumber;
} CMID_SS_SET_CALLFORWARDING_INFO;

typedef struct _CMID_SS_SET_CALLFORWARDING_RSP_INFO { 
    UINT8 nReason;
    UINT8 nMode;
    UINT8 nClass;
    UINT8 nTime;
    CMID_DIALNUMBER_ARRAY nNumber;
} CMID_SS_SET_CALLFORWARDING_RSP_INFO;


typedef struct _CM_SM_INFO
{
	BOOL bIsUseNow;
	UINT8 srcnode;
	UINT8 destnode;
	UINT8 srcparam;
	UINT8 destparam;
	BOOL bProgressIndFlag;      //TRUE: ProgressInd received
	BOOL bAudioOnFlag;           //TRUE :AUDIO ON event received
	BOOL bRelHeldCallFlag;      //indicate that release a held call
	BOOL bUsedAFlag;              //Used ATA to accept incoming call flag
	BOOL bUsedChldFlag;           //used CHLD=1,2 to accept incoming call flag
	BOOL bIsMpty;      //(TRUE,FALSE)
	BOOL nReleaseFlag;
	BOOL bMODisc;
	BOOL bNeedSplitFlag; //TRUE: need SplitMPTY after buildMPTY, otherwise,FALSE 
	UINT8 nAddressType;
	UINT8 nIndex;
	UINT8 nTI;
	UINT8 nDir;         //0  MO call1, 1 MT call
	UINT8 nCallState;   //0~5
	UINT8 nCallMode;     //(0~9)
	UINT8 nCalledFuncFlag;  //0: nothing function is called
	                        // 1: CMID_CmInitiateSpeec is called
	                        // 2: CMID_CmAcceptSpeechCall is called
	                        // 4: CMID_CmCallHoldMultiparty is called
	                        // 8: CMID_CmReleaseCall is called 
	                        // 16: CC_Release is called when received disconnect IND from network.
	UINT8 nHoldFunction;
	UINT8 nNumberSize;
	UINT8 bNeedRetrieve;
	UINT8 nCauseVal;
	VOID* pMsg;
	UINT32 nCurSubState;
	UINT32 nNextSubState;
	UINT8 pNumber[60];
}CM_SM_INFO ;

#ifndef CMID_MULTI_SIM
UINT32  CMID_CcAcceptSpeechCall (
  VOID
);

UINT32  CMID_CcGetCallStatus(
  VOID
);
   UINT8  CMID_GetCcCount(VOID);

UINT32  CMID_CcGetCurrentCall (
  CMID_CC_CURRENT_CALL_INFO CallInfo[7],
  UINT8* pCnt
);

UINT32  CMID_CcCallHoldMultiparty (
  UINT8 nCmd,
  UINT8 nIndex
);

UINT32  CMID_CcInitiateSpeechCall (
  CMID_DIALNUMBER* pDialNumber,
  UINT16 nUTI
);

UINT32  CMID_CcEmcDial (
                      UINT8* pNumber,
                      UINT8 nSize
                      );

UINT32  CMID_CcReleaseCall (
  VOID
);

UINT32  CMID_CcRejectCall (
  VOID
);

UINT32  CMID_CcPlayTone (
  INT8 iTone
);
UINT32  CMID_CcStopTone (
);
#else
UINT8 CMID_Cc_get_conference_call_nums(CMID_SIM_ID sim_id);

BOOL CMID_Cc_is_conference_call(UINT8 call_id, CMID_SIM_ID sim_id);

UINT8 CMID_Cc_get_call_status(UINT8 call_id, CMID_SIM_ID sim_id);

UINT32  CMID_CcAcceptSpeechCall (
  CMID_SIM_ID nSimId
);

UINT32  CMID_CcGetCallStatus(
  CMID_SIM_ID nSimId
);
UINT8  CMID_GetCcCount( CMID_SIM_ID nSimID);


UINT32  CMID_CcGetCurrentCall (
  CMID_SIM_ID nSimId,
  BOOL is_urgent
);

UINT32  CMID_CcCallHoldMultiparty (
  UINT8 nCmd,
  UINT8 nIndex,
  CMID_SIM_ID nSimId
);

UINT32  CMID_CcInitiateSpeechCall (
  CMID_DIALNUMBER* pDialNumber,
  UINT16 nUTI,
  CMID_SIM_ID nSimId
);


UINT32  CMID_CcInitiateSpeechCallEx(
	char * pDialNumber, 
	UINT8 * pIndex, 
	UINT16 is_emergency_num,
	CMID_SIM_ID nSimId
	);

UINT32  CMID_CcEmcDial (UINT8* pNumber, UINT8 nSize, CMID_SIM_ID nSimId);

UINT32  CMID_CcEmcDialEx (char * pDialNumber, UINT8 nDialNumberSize, UINT8 * pIndex, 
	CMID_SIM_ID nSimId);

UINT32  CMID_CcReleaseCall (
  CMID_SIM_ID nSimId
);

UINT32  CMID_CcRejectCall (
  CMID_SIM_ID nSimId
);

UINT32  CMID_CcPlayTone (
  INT8 iTone,
  CMID_SIM_ID nSimId
);

UINT32  CMID_CcStopTone (
		CMID_SIM_ID nSimId
);

#endif

#ifdef CMID_MULTI_SIM

UINT32  CMID_SsQueryCallWaiting (
  UINT8 nClass, 
  UINT16 nUTI,
  CMID_SIM_ID nSimID
);

UINT32  CMID_SsSetCallWaiting (
  UINT8 nMode, 
  UINT8 nClass,
  UINT16 nUTI,
  CMID_SIM_ID nSimID
);

UINT32  CMID_SsSetCallForwarding (
  CMID_SS_SET_CALLFORWARDING_INFO* pCallForwarding,
  UINT16 nUTI,
  CMID_SIM_ID nSimID
);

UINT32  CMID_SsQueryCallForwarding (
  UINT8 nReason,
  UINT8 nClass,
  UINT16 nUTI,
  CMID_SIM_ID nSimID
);

#else

UINT32  CMID_SsQueryCallWaiting (
  UINT8 nClass, 
  UINT16 nUTI
);

UINT32  CMID_SsSetCallWaiting (
  UINT8 nMode, 
  UINT8 nClass,
  UINT16 nUTI //Hameina 2006.10.27 [add]:the original one has no nUTI
);

UINT32  CMID_SsSetCallForwarding (
  CMID_SS_SET_CALLFORWARDING_INFO* pCallForwarding,
  UINT16 nUTI
);

UINT32  CMID_SsQueryCallForwarding (
  UINT8 nReason,
  UINT8 nClass,
  UINT16 nUTI
);

#endif


#define CMID_STY_FAC_TYPE_AO 0x414F //BAOC (Bar All Outgoing Calls)
#define CMID_STY_FAC_TYPE_OI 0x4F49 //BOIC (Bar Outgoing International Calls) 
#define CMID_STY_FAC_TYPE_OX 0x4F58 //BOIC-exHC (Bar Outgoing International Calls except to Home Country) 
#define CMID_STY_FAC_TYPE_AI 0x4149 //BAIC (Bar All Incoming Calls) 
#define CMID_STY_FAC_TYPE_IR 0x4952 //BIC-Roam (Bar Incoming Calls when Roaming outside the home country) 
#define CMID_STY_FAC_TYPE_AB 0x4142 //All Barring services (only applicable for mode=0) 
#define CMID_STY_FAC_TYPE_AG 0x4147 //All outGoing barring services (only applicable for mode=0) 
#define CMID_STY_FAC_TYPE_AC 0x4143 //All inComing barring services (only applicable for mode=0) 

#define CMID_CM_MODE_UNLOCK 0 //unlock 
#define CMID_CM_MODE_LOCK   1 //lock 

#define CMID_SS_FORWARDING_REASON_UNCONDITIONAL          0 //Forward unconditionally. 
#define CMID_SS_FORWARDING_REASON_MOBILEBUSY             1 //Forward if the mobile phone is busy 
#define CMID_SS_FORWARDING_REASON_NOREPLY                2 //Forward if there is no answer 
#define CMID_SS_FORWARDING_REASON_NOTREACHABLE           3 //Forward if the number is unreachable. 
#define CMID_SS_FORWARDING_REASON_ALL_CALLFORWARDING     4 //All call forwarding reasons(includes reasons 0, 1, 2 and 3). 
#define CMID_SS_FORWARDING_REASON_ALL_CONDCALLFORWARDING 5 //All conditional call forwarding reasons(includes reasons 1, 2 and 3). 

#define CMID_SS_FORWARDING_MODE_DISABLE       0
#define CMID_SS_FORWARDING_MODE_ENABLE        1
#define CMID_SS_FORWARDING_MODE_QUERY         2
#define CMID_SS_FORWARDING_MODE_REGISTRATION  3
#define CMID_SS_FORWARDING_MODE_ERASURE       4

#ifdef CMID_MULTI_SIM

UINT32  CMID_SsSetFacilityLock (UINT16 nFac, UINT8* pBufPwd, UINT8 nPwdSize,
        UINT8 nClass, UINT8 nMode, UINT16 nUTI, CMID_SIM_ID nSimID);

UINT32  CMID_SsQueryFacilityLock (UINT16 nFac, UINT8 nClass, UINT16 nUTI, CMID_SIM_ID nSimID);

UINT32 CMID_SsChangePassword (UINT16 nFac, UINT8* pBufOldPwd, UINT8 nOldPwdSize, 
	    UINT8* pBufNewPwd, UINT8 nNewPwdSize, UINT8* pBufNewPwdAgain, UINT8 nNewPwdAgainSize,
        UINT16 nUTI, CMID_SIM_ID nSimID);

UINT32  CMID_SsQueryClip (UINT16 nUTI, CMID_SIM_ID nSimID);

UINT32  CMID_SsQueryClir (UINT16 nUTI, CMID_SIM_ID nSimID);

UINT32  CMID_SsQueryCNAP (UINT16 nUTI, CMID_SIM_ID nSimID);

UINT32  CMID_SsQueryColp (UINT16 nUTI, CMID_SIM_ID nSimID);

UINT32  CMID_SsQueryColr (UINT16 nUTI, CMID_SIM_ID nSimID);

UINT32   CMID_SsSendUSSD (UINT8* pUsdString, UINT8  nUsdStringSize,
  UINT8  nOption, UINT8  nDcs, UINT16 nUTI, CMID_SIM_ID nSimID);

#else

UINT32  CMID_SsSetFacilityLock (
  UINT16 nFac,
  UINT8* pBufPwd,
  UINT8 nPwdSize,
  UINT8 nClass,
  UINT8 nMode,
  UINT16 nUTI
);

UINT32  CMID_SsQueryFacilityLock (
  UINT16 nFac,
  UINT8 nClass,
  UINT16 nUTI
);

UINT32 CMID_SsChangePassword(
	UINT16 nFac,
	UINT8* pBufOldPwd, 
	UINT8 nOldPwdSize, 
	UINT8* pBufNewPwd, 
	UINT8 nNewPwdSize,
	UINT8* pBufNewPwdAgain, 
	UINT8 nNewPwdAgainSize, 
	UINT16 nUTI
  )

UINT32  CMID_SsQueryClip (
  UINT16 nUTI
);

UINT32  CMID_SsQueryClir (
  UINT16 nUTI
);

UINT32  CMID_SsQueryCNAP (
  UINT16 nUTI
);


UINT32  CMID_SsQueryColp (
  UINT16 nUTI
);

UINT32  CMID_SsQueryColr (
  UINT16 nUTI
);

UINT32   CMID_SsSendUSSD (
  UINT8* pUsdString,
  UINT8  nUsdStringSize,
  UINT8  nOption,
  UINT8  nDcs,
  UINT16 nUTI
);

#endif 

typedef struct _CMID_SS_USSD_IND_INFO { 
	UINT8* pUsdString;
	UINT8 nStingSize;
	UINT8 nOption;
	UINT8 nDcs;
	UINT8 padding;
} CMID_SS_USSD_IND_INFO;

//
// SIM Service
//


typedef struct _CMID_SAT_RESPONSE
{
    UINT8 nSm_Act_prevStatus;
    UINT8 nSm_Act_currStatus;
    UINT8 nSAT_Mode;

    UINT8 nSm_Resp_prevStatus;
    UINT8 nSm_Resp_currStatus;

    UINT8 nSAT_CmdType;
    UINT8 nSAT_Status;        //command status
    UINT8 nSAT_ItemID;
    UINT8 nSAT_InnerProcess;
    UINT8 nSAT_InputSch;        //text code scheme.
    UINT8 nSAT_InputLen;        //text length
    UINT32 nSAT_InputStr[1];	   
}CMID_SAT_RESPONSE;


typedef struct _SAT_CHSTATUS_RSP
{
    UINT8 nChannelID;      //1..7
    UINT8 nStatus;         // 0: Link not established or PDP context not activated
                           // 1: Link established or PDP context activated
                           // 
    UINT8 nType;           //00 : No further info can be given
                           //05 : Link dropped
}SAT_CHSTATUS_RSP;

typedef struct _SAT_BASE_RSP        //for proactive command 26
{
    UINT8 nComID;                   //Command id   
    UINT8 nComQualifier;            //Command Qualifier
}SAT_BASE_RSP;

typedef struct _SAT_LOCALINFO_RSP
{
    UINT8 nMCC_MNC_Code[6];        //BCD code ,example:4,6,0,0,0,F for china mobile
    UINT16 nLocalAreaCode;         //Hex Local area code
    UINT16 nCellIdentify;          //Hex Cell identify
}SAT_LOCALINFO_RSP;

typedef struct _SAT_IMEI_RSP
{
    UINT8 nInterMobileId[8];        //BCD code,IMEI
}SAT_IMEI_RSP;

typedef struct _SAT_MEASURE_RSP
{
    UINT8 nMeasure[16];             //Net work measurement
}SAT_MEASURE_RSP;

typedef struct _SAT_DATETIME_RSP
{
    UINT8 nDateTimeZone[7];         //Date,Time,Zone
}SAT_DATETIME_RSP;

typedef struct _SAT_LANGUAGE_RSP
{
    UINT8 nLanguageCode[2];         //Language
}SAT_LANGUAGE_RSP;

typedef struct _SAT_TIMINGADV_RSP
{
    UINT8 nME_Status;               //Me status
    UINT8 nTimingAdv;               //Timing advance
}SAT_TIMINGADV_RSP;

//#if defined(__NGUX_PLATFORM__)		gengyue
typedef struct _CMID_SAT_LANGUAGE_NOTIFY_RSP{  //for proactive command 0x35
    UINT8  nComID ;             
    UINT8  nComQualifier;           
    UINT8  nLanguageLen;
    UINT8  nLanguageCode[2];         //Language
} CMID_SAT_LANGUAGE_NOTIFY_RSP;
//#endif


typedef struct _CMID_SAT_SMS_RSP{    //for proactive command 0x13
    UINT8  nComID ;             
    UINT8  nComQualifier;           
    UINT16 nAlphaLen;
    UINT8 *pAlphaStr;
    UINT16 nAddrLen;           
    UINT8  nAddrType;
    UINT8 *pAddrStr;
    UINT8  nPDULen;
    UINT8 *pPDUStr;
} CMID_SAT_SMS_RSP;

//#if defined(__NGUX_PLATFORM__)		gengyue
typedef struct _CMID_SAT_SEND_DTMF_RSP{       //for proactive command
    UINT8  nComID;             
    UINT8  nComQualifier;     
    UINT16 nAlphaConfirmLen;
    UINT8* pAlphaConfirmStr;    
    UINT8  nDTMFLen; 
    UINT8 *pDTMFStr;         
} CMID_SAT_SEND_DTMF_RSP;

typedef struct _CMID_SAT_SETUP_EVENT_LIST_RSP{       //for proactive command
    UINT8  nComID;             
    UINT8  nComQualifier;           
    UINT8  nEventListLen; 
    UINT8 *pEventListStr;          
} CMID_SAT_SETUP_EVENT_LIST_RSP;
//#endif

typedef struct _CMID_SAT_DTEXT_RSP{       //for proactive command 0x21
    UINT8  nComID;             
    UINT8  nComQualifier;           
    UINT8  nTextLen;
    UINT8 nTextSch; 
    UINT8 *pTextStr;          
} CMID_SAT_DTEXT_RSP;

//#if defined(__NGUX_PLATFORM__)		gengyue
typedef struct _CMID_SAT_SETUP_IDLE_DISPLAY_TEXT_RSP{       //for proactive command 0x28
    UINT8  nComID;             
    UINT8  nComQualifier;           
    UINT8  nTextLen;
    UINT8 nTextSch; 
    UINT8 *pTextStr;          
} CMID_SAT_SETUP_IDLE_DISPLAY_TEXT_RSP;


typedef struct _CMID_SAT_RUN_AT_COMMAND_RSP{
    UINT8  nComID;             
    UINT8  nComQualifier;           
    UINT16 nAlphaConfirmLen;
    UINT8* pAlphaConfirmStr;    
    UINT8  nATCmdLen; 
    UINT8 *pATCmdStr;             
}CMID_SAT_RUN_AT_COMMAND_RSP;

typedef struct _CMID_SAT_TIMER_MANAGEMENT_RSP{
    UINT8  nComID;             
    UINT8  nComQualifier;           
    UINT8  nTimerID;
    UINT8  nTimerValHou;
    UINT8  nTimerValMin;
    UINT8  nTimerValSec;
}CMID_SAT_TIMER_MANAGEMENT_RSP;

typedef struct _CMID_SAT_CARD_ON_RSP{
    UINT8  nComID;             
    UINT8  nComQualifier;    
    UINT8  nSrcDevID;
    UINT8  nDstDevID;    
}CMID_SAT_CARD_ON_RSP;

typedef struct _CMID_SAT_CARD_OFF_RSP{
    UINT8  nComID;             
    UINT8  nComQualifier;       
    UINT8  nSrcDevID;
    UINT8  nDstDevID;    
}CMID_SAT_CARD_OFF_RSP;

typedef struct _CMID_SAT_GET_CARD_STATUS_RSP{
    UINT8  nComID;             
    UINT8  nComQualifier;      
    UINT8  nSrcDevID;
    UINT8  nDstDevID;    
}CMID_SAT_GET_CARD_STATUS_RSP;

typedef struct _CMID_SAT_CARD_APDU_RSP{
    UINT8  nComID;             
    UINT8  nComQualifier;      
    UINT8  nSrcDevID;
    UINT8  nDstDevID;    
}CMID_SAT_CARD_APDU_RSP;

typedef struct _CMID_SAT_OPEN_CHANNEL_RSP{
    UINT8  nComID;             
    UINT8  nComQualifier;        
    UINT16 nAlphaConfirmLen;
    UINT8  nIconQual;
    UINT8  nIconIdentifer;
    UINT8  nChannelID;
    UINT8  nAddrLen;
    UINT8  nAddrTONNPI;
    UINT8  nSubAddrLen;
    UINT8  nNetAccessNameLen;
    UINT8  nDurationInterval1;
    UINT8  nDurationUnit1;
    UINT8  nDurationInterval2;
    UINT8  nDurationUnit2;
    UINT8  nBearerLen;
    UINT8  nBearerType;
    UINT8  nBufferLen;
    UINT8  nBufferSize1;
    UINT8  nBufferSize2;              
    UINT8  nOtherAddrLen;
    UINT8  nTextStrLoginLen;
    UINT8  nTextStrPassLen;
    UINT8  nTransportProtocol;
    UINT8  nTransportPort;
    UINT8  nDataDstAddrLen;
    
    UINT8 *pAlphaConfirmStr;
    UINT8 *pSubAddrStr;
    UINT8 *pAddrStr;
    UINT8 *pBearerStr;
    UINT8 *pNetAccessNameStr;
    UINT8 *pOtherAddrStr;
    UINT8 *pTextStrLoginStr;
    UINT8 *pDataDstAddrStr;
    UINT8 *pTextStrPassStr;
}CMID_SAT_OPEN_CHANNEL_RSP;

typedef struct _CMID_SAT_CLOSE_CHANNEL_RSP{
    UINT8  nComID;             
    UINT8  nComQualifier;    
    UINT8  nChannelID;    
    UINT16 nAlphaConfirmLen;
    UINT8* pAlphaConfirmStr;    
    UINT8  nIconQual;
    UINT8  nIconIdentifer;    
}CMID_SAT_CLOSE_CHANNEL_RSP;

typedef struct _CMID_SAT_RECV_DATA_RSP{
    UINT8  nComID;             
    UINT8  nComQualifier;  
    UINT8  nChannelID;    
    UINT16 nAlphaConfirmLen;
    UINT8* pAlphaConfirmStr;    
    UINT8  nIconQual;
    UINT8  nIconIdentifer;
    UINT32 nRecvDataLength;
}CMID_SAT_RECV_DATA_RSP;

typedef struct _CMID_SAT_SEND_DATA_RSP{
    UINT8  nComID;             
    UINT8  nComQualifier;  
    UINT8  nChannelID;    
    UINT16 nAlphaConfirmLen;
    UINT8* pAlphaConfirmStr;    
    UINT8  nIconQual;
    UINT8  nIconIdentifer;     
    UINT32 nSendDataLength;
    UINT8* pSendData;
}CMID_SAT_SEND_DATA_RSP;


typedef struct _CMID_SAT_GET_CHANNEL_STATUS_RSP{
    UINT8  nComID;             
    UINT8  nComQualifier;           
}CMID_SAT_GET_CHANNEL_STATUS_RSP;
//#endif

typedef struct _CMID_SAT_POLL_INTERVAL_RSP{       //for proactive command 0x21
    UINT8  nComID;             
    UINT8  nComQualifier;  
    UINT8 nTimeUnit;
    UINT8 nTimeInterval;
    UINT8 nPending[2];
	       
} CMID_SAT_POLL_INTERVAL_RSP;

//#if defined(__NGUX_PLATFORM__)		gengyue
typedef struct _CMID_SAT_PLAYTONE_RSP{       //for proactive command 0x20
    UINT8 nComID;             
    UINT8 nComQualifier;

    UINT8  nAlphaLen;
    UINT8*  pAlphaStr; 
    
    UINT8  nIconQual;
    UINT8  nIconIdentifer;
	// tone
	UINT8 nToneType;
	// duration
	UINT8 nDurationLen;
	UINT8 nDurationUnit;
	UINT8 nDurationInterval;
}CMID_SAT_PLAYTONE_RSP;

typedef struct _CMID_SAT_INKEY_RSP{       //for proactive command 0x22
    UINT8 nComID;             
    UINT8 nComQualifier;

    UINT8 nTextLen;
    UINT8 nTextSch; 
    UINT8 *pTextStr;

}CMID_SAT_INKEY_RSP;

typedef struct _CMID_SAT_REFRESH_RSP{       //for proactive command 0x22
    UINT8 nComID;             
    UINT8 nComQualifier;
    UINT8 nFileListLen;
    UINT8 nFileNumber;    
    UINT8 *pFileListStr;
}CMID_SAT_REFRESH_RSP;
//#endif

typedef struct _CMID_SAT_INPUT_RSP{       //for proactive command 0x23
    UINT8 nComID;             
    UINT8 nComQualifier;

    UINT8 nTextLen;
    UINT8 nTextSch; 
    UINT8 *pTextStr;
  
    UINT8 nResMinLen;
    UINT8 nResMaxLen;
  
    UINT8 nDefTextLen;
    UINT8 nDefTextSch;
    UINT8 *pDefTextStr;
}CMID_SAT_INPUT_RSP;

typedef struct _CMID_SAT_CALL_RSP        //for proactive command 0x10
{
  UINT8  nComID;             
  UINT8  nComQualifier;       
  
  UINT16 nAlphaConfirmLen;
  UINT8* pAlphaConfirmStr;

  UINT8  nAddrLen;
  UINT8  nAddrType; 
  UINT8 *pAddrStr;

  UINT8  nCapabilityCfgLen;
  UINT8 *pCapabilityCfgStr;

  UINT8  nSubAddrLen; 
  UINT8 *pSubAddrStr;

  UINT8  nTuint;
  UINT8  nTinterval;

  UINT16 nAlphaSetupLen;
  UINT8 *pAlphaSetupStr;          
} CMID_SAT_CALL_RSP;


typedef struct _CMID_SAT_SS_RSP        //for proactive command 0x10
{
  UINT8  nComID;             
  UINT8  nComQualifier;       
  
  UINT8  nAlphaLen;
  UINT8  nAlphaType; 
  
  UINT8  nSSLen;
  UINT8  nSSType; 

  UINT8  nIconQual;
  UINT8  nIconIdentifer;
  
  UINT8 *pAlphaStr;
  UINT8 *pSSStr;
      
} CMID_SAT_SS_RSP;


typedef struct _CMID_SAT_BROWSER_RSP        //for proactive command 0x15
{
  UINT8  nComID;             
  UINT8  nComQualifier;       
  UINT8  nBrowserIdentity;
  UINT8  nURLLength;
  
  UINT8  nBearerLength; 
  UINT8  nAlphaLenLength;
  UINT8  nAlphaType;   
  UINT8  nTextLength; //Gateway/Proxy 
  
  UINT8  nTextType; 
  UINT8  nIconQual;
  UINT8  nIconIdentifer;
  UINT8  nProvisioningFileReferenceNum;
#if defined(__NGUX_PLATFORM__)
  UINT8  nFirstProvisioningFileLen;  
#endif

  UINT8  nIconQualifier;
  UINT8  nIconIdentifier;

  UINT8 *pURL;
  UINT8 *pBearer;
  UINT8 *pText;
  UINT8 *pProvisioningFileReference;
  UINT8 *pAlpha;
      
} CMID_SAT_BROWSER_RSP;

#if defined(__NGUX_PLATFORM__)
UINT8* _SIMPLE_TLV_DATA_OBJECTS_ADDRESS(CMID_DIALNUMBER nNUM, UINT8 * pLen);
UINT8* _SIMPLE_TLV_DATA_OBJECTS_SUBADDRESS(CMID_DIALNUMBER nNUM, UINT8 *pLen);
UINT8* _SIMPLE_TLV_DATA_OBJECTS_TRANSACTION_IDENTIFIER(UINT8* pTIList, UINT8 *pLen);
UINT8* _SIMPLE_TLV_DATA_OBJECTS_CAUSE(UINT8* pCause, UINT8 *pLen);
UINT8* _SIMPLE_TLV_DATA_OBJECTS_CASE(UINT8 nLocationStatus, UINT8 *pLen);
UINT8* _SIMPLE_TLV_DATA_OBJECTS_CARD_READER_STATUS(UINT8 nCardReaderStatus, UINT8 *pLen);
UINT8* _SIMPLE_TLV_DATA_OBJECTS_CHANNEL_DATA_LENGTH(UINT8 nChannelDataLength, UINT8 *pLen);
UINT8* _SIMPLE_TLV_DATA_OBJECTS_BROWSER_TERMINATION_CAUSE(UINT8 nBrowserTerminationCause, UINT8 *pLen);
UINT8* _SIMPLE_TLV_DATA_OBJECTS_LANGUAGE(UINT8 nLanguage[2], UINT8 *pLen);
UINT8* _SIMPLE_TLV_DATA_OBJECTS_CHANNEL_STATUS(UINT8 nChannelStatus, UINT8 *pLen);
UINT8* _SIMPLE_TLV_DATA_OBJECTS_LOCATION_INFORMATION( UINT8* nMCCMNC, UINT16* nLAC, UINT16* nCellID, UINT8 *pLen );
UINT8* _SIMPLE_TLV_DATA_OBJECTS_LOCATION_STATUS(UINT8 nLocationStatus, UINT8 *pLen);
#endif
//These Macro will be add to system header files.


//This Macro definition is used for ParsSW1SW2 function.
#define ERR_CME_SIM_VERIFY_FAIL                 			0x20
#define ERR_CME_SIM_UNBLOCK_FAIL                			0x21
#define ERR_CME_SIM_CONDITION_NO_FULLFILLED    	0x22
#define ERR_CME_SIM_UNBLOCK_FAIL_NO_LEFT        	0x23
#define ERR_CME_SIM_VERIFY_FAIL_NO_LEFT         		0x24
#define ERR_CME_SIM_INVALID_PARAMETER           		0x25
#define ERR_CME_SIM_UNKNOW_COMMAND              		0x26
#define ERR_CME_SIM_WRONG_CLASS                 		0x27
#define ERR_CME_SIM_TECHNICAL_PROBLEM           		0x28
#define ERR_CME_SIM_CHV_NEED_UNBLOCK            		0x29


#define ERR_CME_SIM_NOEF_SELECTED                   		0x2A
#define ERR_CME_SIM_FILE_UNMATCH_COMMAND            0x2B
#define ERR_CME_SIM_CONTRADICTION_CHV                    0x2C
#define ERR_CME_SIM_CONTRADICTION_INVALIDATION  0x2D
#define ERR_CME_SIM_MAXVALUE_REACHED                    0x2E
#define ERR_CME_SIM_PATTERN_NOT_FOUND                  0x2F
#define ERR_CME_SIM_FILEID_NOT_FOUND				0x30
#define ERR_CME_SIM_STK_BUSY                        		0x31
#define ERR_CME_SIM_UNKNOW                      		       0x32
#define ERR_CME_SIM_PROFILE_ERROR						    0x33

//#define EV_CMID_SIM_LISTCOUNT_PBK_ENTRY_RSP      	16        //for List counter function event id
#define ERR_SCL_QUEUE_FULL                   				0x23    //
#define ERR_CMID_INVALID_COMMAND                 		1
//end


#define SIM_PBK_NAME_SIZE    50  //Lixp updated,20071225
#define SIM_PBK_NUMBER_SIZE  10
#define SIM_PBK_EXTR_SIZE    (SIM_PBK_NUMBER_SIZE+SIM_PBK_NAME_SIZE)

typedef struct _CMID_SIM_PBK_ENTRY_INFO {
  UINT8* pNumber;
  UINT8* pFullName;
  UINT8 iFullNameSize;
  UINT8 nNumberSize;
  UINT8 nType;
  UINT16 phoneIndex;
  UINT16 nextphoneIndex;
} CMID_SIM_PBK_ENTRY_INFO;

typedef struct _CMID_SIM_PUCT_INFO  
{
    UINT8  iCurrency[3];  //currency code
    UINT16 iEPPU; 
    UINT8  iEX;
    BOOL   iSign;          //the sign of EX,1 is the negetive sign
    UINT8  padding;
}CMID_SIM_PUCT_INFO;


#define CMID_SIM_PBKMODE_ADN  0
#define CMID_SIM_PBKMODE_FDN  1




typedef struct _CMID_PBK_STORAGE {
  UINT16 usedSlot;
  UINT16 totalSlot;
  UINT16 storageId;
  UINT8 padding[2];
} CMID_PBK_STORAGE; 

typedef struct _CMID_SPN {
  UINT8 flag;
  UINT8 spn[16];
} CMID_SPN; 

#define CMID_STY_FAC_TYPE_SC  0x5343 //SIM (lock SIM cards). SIM requests password upon ME power up and when this lock command is issued. "SC" lock is protected with SIM PIN1. The number can be modified with this function.  
#define CMID_STY_FAC_TYPE_PS  0x5053 //Phone locked to SIM (device code). The "PS" password may either be individually specified by the client or, depending on the subscription, supplied from the provider (e.g. with a prepaid mobile). 
#define CMID_STY_FAC_TYPE_P2  0x5032 //SIM PIN2. 
#define CMID_STY_FAC_TYPE_PF  0x5046 //lock Phone to the very first SIM card 
#define CMID_STY_FAC_TYPE_PN  0x504E //Network Personalisation 
#define CMID_STY_FAC_TYPE_PU  0x5055 //Network-subset Personalisation 
#define CMID_STY_FAC_TYPE_PP  0x5050 //Service-Provider Personalisation 
#define CMID_STY_FAC_TYPE_PC  0x5043 //Corporate Personalisation 
#define CMID_STY_FAC_TYPE_FD  0x4644 //the phone numbers stored to the "FD" memory can be dialled (depending on the SIM card, usually up to 7 numbers). If PIN2 authentication has not been performed during the current session, PIN2 is requested as <passwd>. 

#define CMID_STY_AUTH_READY                 0  //Phone is not waiting for any password. 
#define CMID_STY_AUTH_PIN1_READY            1  //Phone is not waiting for PIN1 password. 
#define CMID_STY_AUTH_SIMPIN                2  //Phone is waiting for the SIM Personal Identification Number (PIN) 
#define CMID_STY_AUTH_SIMPUK                3  //Phone is waiting for the SIM Personal Unlocking Key (PUK). 
#define CMID_STY_AUTH_PHONE_TO_SIMPIN       4  //Phone is waiting for the phone-to-SIM card password. 
#define CMID_STY_AUTH_PHONE_TO_FIRST_SIMPIN 5  //Phone is waiting for the phone-to-first-SIM card PIN. 
#define CMID_STY_AUTH_PHONE_TO_FIRST_SIMPUK 6  //Phone is waiting for the phone-to-first-SIM card PUK. 
#define CMID_STY_AUTH_SIMPIN2               7  //Phone is waiting for the SIM PIN2. 
#define CMID_STY_AUTH_SIMPUK2               8  //Phone is waiting for the SIM PUK2. 
#define CMID_STY_AUTH_NETWORKPIN            9  //Phone is waiting for the network personalization PIN. 
#define CMID_STY_AUTH_NETWORKPUK            10 //Phone is waiting for the network personalization PUK. 
#define CMID_STY_AUTH_NETWORK_SUBSETPIN     11 //Phone is waiting for the network subset personalization PIN. 
#define CMID_STY_AUTHNETWORK_SUBSETPUK      12 //Phone is waiting for the network subset personalization PUK. 
#define CMID_STY_AUTH_PROVIDERPIN           13 //Phone is waiting for the service provider personalization PIN. 
#define CMID_STY_AUTH_PROVIDERPUK           14 //Phone is waiting for the service provider personalization PUK. 
#define CMID_STY_AUTH_CORPORATEPIN          15 //Phone is waiting for the corporate personalization PIN.
#define CMID_STY_AUTH_CORPORATEPUK          16 //Phone is waiting for the corporate personalization PUK. 
#define CMID_STY_AUTH_NOSIM                 17 //No SIM inserted. 
//
//
#define CMID_STY_AUTH_PIN1BLOCK             18
#define CMID_STY_AUTH_PIN2BLOCK             19
#define CMID_STY_AUTH_PIN1_DISABLE          20
#define CMID_STY_AUTH_SIM_PRESENT           21
#define CMID_STY_AUTH_SIM_PHASE             22
#define CMID_STY_AUTH_SIM_END               23

// all retryleft will -1
//#define SIM_RETRYLEFT_DECREASE_ONE_FOR_DEBUG

typedef struct _CMID_PROFILE
{

//b0  == FDN
//b1  == BDN
//b2  == CC
//0: Not Support,1: Support
    UINT16 nStart_Profile;

    UINT8 nSat_ProfileLen;
    UINT8 pSat_ProfileData[19];			//  3gpp 11.14
}CMID_PROFILE;

#ifdef CMID_MULTI_SIM
UINT32  CMID_SimChangePassword (
  UINT16 nFac,
  UINT8* pBufOldPwd,
  UINT8 nOldPwdSize,
  UINT8* pBufNewPwd,
  UINT8 nNewPwdSize,
  UINT16 nUTI,
  CMID_SIM_ID nSimID
);

UINT32  CMID_SimGetAuthenticationStatus (
  UINT16 nUTI,
  CMID_SIM_ID nSimID
);

UINT32  CMID_SimEnterAuthentication (
  UINT8* pPin,
  UINT8 nPinSize,
  UINT8* pNewPin,
  UINT8 nNewPinSize,
  UINT8 nOption,
  UINT16 nUTI,
  CMID_SIM_ID nSimID
);

UINT32  CMID_SimSetFacilityLock (
  UINT16 nFac,
  UINT8* pBufPwd,
  UINT8 nPwdSize,
  UINT8 nMode,
  UINT16 nUTI,
  CMID_SIM_ID nSimID
);

UINT32  CMID_SimGetFacilityLock (
  UINT16 nFac,
  UINT16 nUTI,
  CMID_SIM_ID nSimID
);

UINT32  CMID_SimQueryFacilityLock (
  UINT16 nFac,
  CMID_SIM_ID nSimID
);

UINT32 CMID_SimUnlockPin (
	UINT16 nFac,
	UINT8* pPwd,
	UINT8 nPwdSize,
	CMID_SIM_ID sim_id
);

UINT32 CMID_SimBlockUnlockMEP(
	UINT8* pPwd,
	UINT8 nPwdSize,
	CMID_SIM_ID sim_id,
	void *pFun
);

// the block function, will remove later
UINT32 CMID_SimBlockUnlockPin1 (
	UINT8* pPwd,
	UINT8 nPwdSize,
	CMID_SIM_ID sim_id,
	void *pFun
);

UINT32 CMID_SimBlockUnlockPUK1 (
	UINT8* pPwd,
	UINT8 *pNewPin,
	CMID_SIM_ID sim_id,
	void *pFun
);

UINT32  CMID_SimGetProviderId (
  UINT16 nUTI,
  CMID_SIM_ID nSimID
);

UINT32  CMID_SimSetPrefOperatorList( 
  UINT8* pOperatorList,
  UINT8 nSize, 
  UINT16 nUTI,
  CMID_SIM_ID nSimID
); 



UINT32  CMID_SimGetPrefOperatorList(
  UINT16 nUTI,
  CMID_SIM_ID nSimID
);

UINT32  CMID_SimAddPbkEntry (
  UINT8 nStorage,
  CMID_SIM_PBK_ENTRY_INFO* pEntryInfo,
  UINT16 nUTI,
  CMID_SIM_ID nSimID
);

UINT32  CMID_SimDeletePbkEntry (
  UINT8 nStorage,
  UINT16 nIndex,
  UINT16 nUTI ,
  CMID_SIM_ID nSimID
);

UINT32  CMID_SimGetPbkEntry (
  UINT8 nStorage,
  UINT16 nIndex,
  UINT16 nUTI,
  CMID_SIM_ID nSimID
);



UINT32  CMID_SimListCountPbkEntries (
  UINT8 nStorage,
  UINT8 nIndexStart,
  UINT8 nCount,
  UINT16 nUTI,
  CMID_SIM_ID nSimID
);

UINT32  CMID_SimGetPbkStrorageInfo (
  UINT8 nStorage,
  UINT16 nUTI ,
  CMID_SIM_ID nSimID
);

UINT32  CMID_SimGetPbkStorage(
  UINT8 nStorage,
  UINT16 nUTI,
  CMID_SIM_ID nSimID
);


UINT32  CMID_SatResponse (
  UINT8 nCmdType,
  UINT8 nStatus,
  UINT8 nItemId,
  PVOID pInputString,
  UINT8 InputStrLen,
  UINT16 nUTI,
  CMID_SIM_ID nSimID
);

UINT32  CMID_SatGetInformation (
  UINT8 nCmdType,
  PVOID* pCmdData,
  CMID_SIM_ID nSimID
);
UINT8 CMID_SatGetCurCMD(CMID_SIM_ID nSimID);

UINT32 CMID_CfgSimGetMeProfile(CMID_PROFILE* pMeProfile, CMID_SIM_ID nSimID);

UINT32 CMID_CfgSimSetMeProfile(CMID_PROFILE* pMeProfile, CMID_SIM_ID nSimID);

VOID CMID_GetMNCLen(UINT8 *pLen,CMID_SIM_ID nSimID);
#else

UINT32  CMID_SimChangePassword (
  UINT16 nFac,
  UINT8* pBufOldPwd,
  UINT8 nOldPwdSize,
  UINT8* pBufNewPwd,
  UINT8 nNewPwdSize,
  UINT16 nUTI
);

UINT32  CMID_SimGetAuthenticationStatus (
  UINT16 nUTI
);

UINT32  CMID_SimEnterAuthentication (
  UINT8* pPin,
  UINT8 nPinSize,
  UINT8* pNewPin,
  UINT8 nNewPinSize,
  UINT8 nOption,
  UINT16 nUTI
);

UINT32  CMID_SimSetFacilityLock (
  UINT16 nFac,
  UINT8* pBufPwd,
  UINT8 nPwdSize,
  UINT8 nMode,
  UINT16 nUTI
);

UINT32  CMID_SimGetFacilityLock (
  UINT16 nFac,
  UINT16 nUTI
);

UINT32  CMID_SimGetProviderId (
  UINT16 nUTI
);

UINT32  CMID_SimSetPrefOperatorList( 
    UINT8* pOperatorList,
    UINT8 nSize,
    UINT16 nUTI
); 


UINT32  CMID_SimGetPrefOperatorList(
    UINT16 nUTI
);

UINT32  CMID_SimAddPbkEntry (
  UINT8 nStorage,
  CMID_SIM_PBK_ENTRY_INFO* pEntryInfo,
  UINT16 nUTI
);

UINT32  CMID_SimDeletePbkEntry (
  UINT8 nStorage,
  UINT16 nIndex,
  UINT16 nUTI 
);
UINT32  CMID_SimGetServiceProviderName(UINT16 nUTI);

UINT32  CMID_SimGetPbkEntry (
  UINT8 nStorage,
  UINT16 nIndex,
  UINT16 nUTI
);



UINT32  CMID_SimListCountPbkEntries (
    UINT8 nStorage,
    UINT8 nIndexStart,
    UINT8 nCount,
    UINT16 nUTI
);

UINT32  CMID_SimGetPbkStrorageInfo (
  UINT8 nStorage,
  UINT16 nUTI 
);

UINT32  CMID_SimGetPbkStorage(
  UINT8 nStorage,
  UINT16 nUTI
  );


UINT32  CMID_SatResponse (
  UINT8 nCmdType,
  UINT8 nStatus,
  UINT8 nItemId,
  PVOID pInputString,
  UINT8 InputStrLen,
  UINT16 nUTI
);

UINT32  CMID_SatGetInformation (
  UINT8 nCmdType,
  PVOID* pCmdData
);

UINT8 CMID_SatGetCurCMD(VOID);

UINT32 CMID_CfgSimGetMeProfile(CMID_PROFILE* pMeProfile );

UINT32 CMID_CfgSimSetMeProfile(CMID_PROFILE* pMeProfile );

VOID CMID_GetMNCLen(UINT8 *pLen );
#endif

#ifdef CMID_MULTI_SIM
UINT32 CMID_SimGetICCID(UINT16 nUTI,CMID_SIM_ID nSimID);

#else
UINT32 CMID_SimGetICCID(UINT16 nUTI); 
#endif

// SAT

typedef struct _CMID_SAT_ITEM_LIST
{
	UINT8 nItemID;
	UINT8 nItemLen;
    UINT8 padding[2];
	UINT8* pItemStr;
	struct _CMID_SAT_ITEM_LIST * pNextItem;
}CMID_SAT_ITEM_LIST;

//
//
#if 0
typedef struct _CMID_SAT_MENU_RSP_LIST 
{
	UINT8 nComID;
	UINT8 nComQualifier;
	UINT16 nAlphaLen;
	UINT8* pAlphaStr;
	CMID_SAT_ITEM_LIST * pItemList; //Link list
}CMID_SAT_MENU_RSP_LIST;
#endif


//
// SMS Service
//

typedef struct _CMID_SMS_TXT_DELIVERED_NO_HRD_INFO {
 UINT8 oa[TEL_NUMBER_MAX_LEN]; // BCD 
 UINT8 oa_size;
 UINT8 padding[2];
 TM_SMS_TIME_STAMP scts;
 UINT8 stat;
 UINT8 tooa;
 UINT16 length;
 UINT8 data[1];
} CMID_SMS_TXT_DELIVERED_NO_HRD_INFO;

typedef struct _CMID_SMS_TXT_DELIVERED_WITH_HRD_INFO {
 UINT8 oa[TEL_NUMBER_MAX_LEN];
 UINT8 oa_size;
 UINT8 sca[TEL_NUMBER_MAX_LEN];
 UINT8 sca_size; 
 TM_SMS_TIME_STAMP scts;
 UINT8 tooa;
 UINT8 tosca;
 UINT8 stat;
 UINT8 fo;
 UINT8 pid;
 UINT8 dcs;
 #ifdef __NGUX_PLATFORM__
 UINT8 rp;   // 1 is set on, 0, is set off, other is error.
 #endif
 UINT16 length;

 UINT8 isSOS;

 UINT8 data[1];
} CMID_SMS_TXT_DELIVERED_WITH_HRD_INFO;

typedef struct _CMID_SMS_TXT_REPORT_INFO {
 TM_SMS_TIME_STAMP scts;
 TM_SMS_TIME_STAMP dt;
 UINT8 stat;
 UINT8 fo;
 UINT8 mr;
 UINT8 padding;
 UINT32 st;
} CMID_SMS_TXT_REPORT_V0_INFO;

typedef struct _CMID_SMS_TXT_REPORT_V1_INFO {
  UINT8 ra[TEL_NUMBER_MAX_LEN];
  UINT8 ra_size;
  TM_SMS_TIME_STAMP scts;
  TM_SMS_TIME_STAMP dt;
  UINT8 tora;
  UINT8 stat;
  UINT8 fo;
  UINT8 mr;
  UINT32 st;
} CMID_SMS_TXT_REPORT_V1_INFO;

typedef struct _CMID_SMS_PDU_INFO {
 UINT16 nTpUdl; 
 UINT16 nDataSize; 
 UINT8 nStatus;
 UINT8 padding[3];
 UINT8 pData[1];
} CMID_SMS_PDU_INFO;

typedef struct _CMID_SMS_TXT_SUBMITTED_NO_HRD_INFO {
  UINT8 da[TEL_NUMBER_MAX_LEN];
  UINT8 da_size;
  UINT8 stat;
  UINT8 padding;
  UINT16 length;
  UINT8 data[1];
} CMID_SMS_TXT_SUBMITTED_NO_HRD_INFO;

typedef enum
{	
	SMS_SEND_PROC = 1,
	SMS_NEW_MSG_PROC,
	SMS_DLETE_PROC,
	SMS_UNSENT_2_SENT_PROC,
	SMS_UNREAD_2_READ_PROC,
	SMS_READ_2_UNREAD_PROC,
	SMS_READ_PROC,
	SMS_WRITE_PROC,
	SMS_LIST_PROC,
	SMS_INIT_PROC,
	SMS_SEND_PROC_ACK,
	SMS_CB_ACT_PROC,
	SMS_CB_DEACT_PROC,
	SMS_NEW_CBMSG_PROC,
} SMS_EVENT_ENUM;

typedef struct _CMID_SMS_TXT_SUBMITTED_WITH_HRD_INFO {
  UINT8 da[TEL_NUMBER_MAX_LEN];
  UINT8 da_size;
  UINT8 sca[TEL_NUMBER_MAX_LEN];
  UINT8 sca_size;
  UINT8 toda;
  UINT8 tosca;
  UINT8 stat;
  UINT8 fo;
  UINT8 pid;
  UINT8 dcs;
  UINT8 vp;
 TM_SMS_TIME_STAMP scts;
  #ifdef __NGUX_PLATFORM__
  UINT8 rp;   // 1 is set on, 0, is set off, other is error. 
  #endif
  UINT8 padding[3];
  UINT16 length;
  UINT8 data[1];
} CMID_SMS_TXT_SUBMITTED_WITH_HRD_INFO;

typedef struct _CMID_SMS_TXT_NO_HRD_IND { 
 UINT8 oa[TEL_NUMBER_MAX_LEN];
 UINT8 oa_size;
 UINT8 tooa;
 UINT8 dcs;
 TM_SMS_TIME_STAMP scts;
 UINT16 nStorageId;
 UINT16 nIndex;
 UINT8 padding;
 UINT16 nDataLen;
 UINT8 pData[1];
} CMID_SMS_TXT_NO_HRD_IND, CMID_SMS_TXT_HRD_V0_IND;

typedef struct _CMID_SMS_TXT_HRD_V1_IND { 
 UINT8 oa[TEL_NUMBER_MAX_LEN];
 UINT8 oa_size;
 UINT8 sca[TEL_NUMBER_MAX_LEN];
 UINT8 sca_size;
 TM_SMS_TIME_STAMP scts;
 UINT8 tosca;
 UINT8 tooa;
 UINT8 fo;
 UINT8 pid;
 UINT8 dcs;
// #ifdef __NGUX_PLATFORM__	gengyue
 UINT8 rp;   // 1 is set on, 0, is set off, other is error. 
 UINT16 WaitingIndication;
#ifdef __MMI_MESSAGE_SOS__
 UINT8 SOS;   // 1 is set on, 0, is set off, other is error.
#endif
// #endif
 UINT8 padding;
 UINT16 nStorageId;
 UINT16 nIndex;
 UINT16 nDataLen;
 UINT8 pData[1];
} CMID_SMS_TXT_HRD_V1_IND;

typedef struct _CMID_SMS_TXT_STATUS_IND { 
 UINT8 ra[TEL_NUMBER_MAX_LEN];
 UINT8 ra_size;
 UINT8 tora;
 UINT8 fo;
 UINT8 mr;
 UINT8 st;
 UINT16 nStorageId;
 UINT16 nIndex;
 TM_SMS_TIME_STAMP scts;
 TM_SMS_TIME_STAMP dt;
} CMID_SMS_TXT_STATUS_IND;

typedef struct _CMID_SMS_MULTIPART_INFO {
  UINT8 id;
  UINT8 count;
  UINT8 current;
  UINT8 padding;
} CMID_SMS_MULTIPART_INFO; 

typedef struct _CMID_SMS_LIST{
	UINT16 nCount;
	UINT16 nStartIndex;
	UINT8 nStorage;
	UINT8 nType;
	UINT8 nStatus;
	UINT8 nOption;
	UINT8 padding[3];
}CMID_SMS_LIST;

typedef struct _CMID_SMS_COPY{
	UINT8 nOption;
	UINT8 nStatus;
	UINT16 nCount;
	UINT16 nStartIndex;
	UINT8 padding[2];
}CMID_SMS_COPY;

typedef struct _CMID_SMS_PDU_PARAM {
 UINT8 vp;      //167 
 UINT8 pid;     // 0
 UINT8 dcs;     // 0
 UINT8 mti;     // 0x01
 UINT8 ssr;     // 0 
 UINT8 bearer;  // 0
 UINT8 mr;      // 0
 UINT8 SCAIndex; // default index 
 UINT8 nNumber[12]; // BCD format
 UINT8 rp;
} CMID_SMS_PDU_PARAM; 

typedef struct _CMID_SMS_WRITE{
CMID_DIALNUMBER sNumber;
UINT8* pData;
UINT16 nDataSize;
UINT16 nIndex;
UINT8 nStorage;
UINT8 nType;
UINT8 nStatus;
UINT8 padding[1];
}CMID_SMS_WRITE;

typedef struct _CMID_SMS_NODE {
 PVOID pNode;
 UINT16 nStorage;
 UINT16 nStatus;
 UINT16 nConcatPrevIndex;
 UINT16 nConcatNextIndex;
 UINT16 nConcatCurrentIndex;
 UINT8 nType;
 UINT8 padding;
} CMID_SMS_NODE;



typedef struct _CMID_NEW_SMS_NODE {
 PVOID pNode;
 UINT16 nStorage;
 UINT16 nConcatPrevIndex;
 UINT16 nConcatCurrentIndex;
 UINT8 nType;
} CMID_NEW_SMS_NODE;

#define CMID_SMS_LIST_OPTION__TIME_DESCEND_ORDER  0x01
#define CMID_SMS_STORED_STATUS_TIME_ASCEND_ORDER  0x02
#define CMID_SMS_MULTIPART_OPTION_ALL             0x04 
#define CMID_SMS_MULTIPART_OPTION_FIRST           0x08 
#define CMID_SMS_MULTIPART_OPTION_SIM_BACKUP      0x10
#define CMID_SMS_ROUT_LOCATION                    0x20
#define CMID_SMS_ROUT_DETAIL_INFO                 0x40

#define SCA_MAX_NUMBER  21 


typedef struct _CMID_SMS_HEADER{
  UINT8 pTelNumber[TEL_NUMBER_MAX_LEN];
  UINT8 nNumber_size;
  UINT8 nTelNumberType;
  UINT8 nStatus;
  UINT16 nStorage;
  UINT16 nConcatPrevIndex;
  UINT16 nConcatNextIndex;
  UINT16 nConcatCurrentIndex;
  TM_SMS_TIME_STAMP nTime;
} CMID_SMS_HEADER;


typedef struct _CMID_SMS_SCA {
  UINT8 nNumber[SCA_MAX_NUMBER];
  UINT8 nType;
  UINT8 nFlag;
  UINT8 padding[1];
} CMID_SMS_SCA;

typedef struct _CMID_SMS_INIT{   
    UINT8  n_PrevStatus;
    UINT8  n_CurrStatus;
    BOOL   bInitFlag;
	UINT8  padding[1];
}CMID_SMS_INIT;

#ifdef __NGUX_PLATFORM__
typedef struct _CB_CTX_
{
	UINT16 SimFileSize ;
	UINT8   CbActiveStatus;
	UINT8   CbType;
	UINT8   MidNb;
	UINT16   Mid[CMID_CB_MAX_CH];
	UINT8   MirNb;
	UINT16   Mir[CMID_CB_MAX_CH];	
	UINT8   DcsNb;
	UINT8   Dcs[CMID_CB_MAX_DCS];
}CB_CTX_T;

typedef struct
{
	u16		  sn;			//GS+MessageCode+UpdateNumber
	u16       MId;        // Message Id the SMS-CB belongs to
	u8        DCS;        // Data Coding Scheme applicable to the SMS-CB message
	u8        PageInfo;
	u8        nsimid;
	u8        reserved;
	u16       DataLen;
	u8        Data[1];    // In fact, DataLen elts must be allocated for this 
	            // array
} CB_MSG_IND_STRUCT ;
#endif
//[[hameina[+]2008.11.21 SMS data len
#define CMID_SMS_LEN_7BIT_NOMAL     160
#define CMID_SMS_LEN_UCS2_NOMAL      140
#define CMID_SMS_LEN_7BIT_LONG          153
#define CMID_SMS_LEN_UCS2_LONG         134

//]]hameina[+]2008.11.21
#ifdef __NGUX_PLATFORM__
//]]ylwang[+]2012.09.22 samsung generate data no sim
VOID  CMID_SmsSetWriteMessageNoCheckSim(BOOL check);
#endif
#ifndef CMID_MULTI_SIM
UINT32  CMID_SmsDeleteMessage (
  UINT16 nIndex,
  UINT8 nStatus,
  UINT16 nStorage,
  UINT16 nUTI
);


UINT32  CMID_SmsListMessages ( 
  UINT16 nStorage,
  UINT8 nStatus,
  UINT16 nCount,
  UINT16 nStartIndex,
  UINT8 nOption,
  UINT16 nUTI
);

UINT32  CMID_SmsCopyMessages ( 
  UINT8 nStatus,
  UINT16 nCount,
  UINT16 nStartIndex,
  UINT8 nOption,
  UINT16 nUTI
);

UINT32  CMID_SmsReadMessage ( 
  UINT16 nStorage,
  UINT16 nIndex,
  UINT16 nUTI
);


UINT32  CMID_SmsSendMessage ( 
  CMID_DIALNUMBER* pNumber,
  UINT8* pData,
  UINT16 nDataSize,
  UINT16 nUTI
);

UINT32  CMID_SmsWriteMessage ( 
  CMID_DIALNUMBER* pNumber,
  UINT8* pData,
  UINT16 nDataSize,
  UINT16 nStorage,
  UINT16 nIndex,
  UINT8 nStatus,
  UINT16 nUTI
);

UINT32  CMID_SmsSetUnSent2Sent(
    UINT16 nStorage,
    UINT16 nIndex,
    UINT8 nSendStatus,
    UINT16 nUTI
);


UINT32  CMID_SmsComposePdu(
                         UINT8* pTpUd, // "ABC"
                         UINT16 nTpUdl, // 3
                         CMID_DIALNUMBER* pNumber, // telephone number to send, 135XXX, 
                         CMID_SMS_PDU_PARAM* pSmsParam,  // NULL
                         UINT8** pSmsData,              // ouput need to maloc.
                         UINT16* pSmsDataSize          // size
                         );


UINT32  CMID_SmsGetMessageNode ( 
  PVOID pListResult,
  UINT16 nIndex,
  CMID_SMS_NODE **pNode
);

UINT32 CMID_SmsInitComplete(
#ifdef __NGUX_PLATFORM__
  BOOL bFinished,
#endif
  UINT16 nUTI
);

UINT32 CMID_SetSMSConcat (
  BOOL bConcat
);

UINT32 CMID_SmsAbortSendMessage(VOID);


#else

UINT32 CMID_SmsAbortSendMessage(CMID_SIM_ID nSimID);

PUBLIC UINT32 CMID_SmsDeleteMessage( UINT16 nIndex, UINT8 nStatus,UINT8 nStorage, UINT8 nType, UINT16 nUTI, CMID_SIM_ID nSimId);



UINT32  CMID_SmsListMessages(
	CMID_SMS_LIST * pListInfo, 
	UINT16 nUTI, 
	CMID_SIM_ID nSimId
);



UINT32  CMID_SmsCopyMessages(
	CMID_SMS_COPY * pCopyInfo, 
	UINT16 nUTI, 
	CMID_SIM_ID nSimId
);

UINT32 CMID_SmsReadMessage(
	UINT8 nStorage, 
	UINT8 nType, 
	UINT16 nIndex, 
	UINT16 nUTI, 
	CMID_SIM_ID nSimId
);
UINT32  CMID_SimGetServiceProviderName(UINT16 nUTI, CMID_SIM_ID nSimID);


UINT32  CMID_SmsSendMessage ( 
	CMID_DIALNUMBER* pNumber,
	UINT8* pData,
	UINT16 nDataSize,
	UINT16 nUTI,
	CMID_SIM_ID nSimId
);



UINT32  CMID_SmsWriteMessage(
	CMID_SMS_WRITE * pSMSWrite, 
	UINT16 nUTI, 
	CMID_SIM_ID nSimId
);


PUBLIC UINT32 CMID_SmsSetUnSent2Sent(UINT8 nStorage, UINT8 nType, UINT16 nIndex, UINT8 nSendStatus, UINT16 nUTI, CMID_SIM_ID nSimId);


UINT32  CMID_SmsComposePdu(
	UINT8* pTpUd, // "ABC"
	UINT16 nTpUdl, // 3
	CMID_DIALNUMBER* pNumber, // telephone number to send, 135XXX, 
	CMID_SMS_PDU_PARAM* pSmsParam,  // NULL
	UINT8** pSmsData,              // ouput need to maloc.
	CMID_SIM_ID nSimId,
	UINT16* pSmsDataSize          // size
 );


UINT32  CMID_SmsGetMessageNode ( 
	PVOID pListResult,
	UINT16 nIndex,
	CMID_SMS_NODE **pNode,
	CMID_SIM_ID nSimId
);

UINT32 CMID_SmsInitComplete(
#ifdef __NGUX_PLATFORM__
  BOOL bFinished,
#endif
  UINT16 nUTI,
  CMID_SIM_ID nSIMID
);

UINT32 CMID_SetSMSConcat (
  BOOL bConcat, 
  CMID_SIM_ID nSimID
);

#endif

typedef struct _CMID_EMS_INFO{
  UINT8   nMaxNum;      //Maximum number of short messages in the enhanced concatenated short message.
  UINT8   nCurrentNum;  //Sequence number of the current short message
  UINT16  nRerNum;      //reference number
  UINT16  nDesPortNum;  //Destination port  
  UINT16  nOriPortNum;  //Originator port
}CMID_EMS_INFO;



// CC Network Error code 
#define    CMID_CC_CAUSE_NOUSED_CAUSE                                        0
#define    CMID_CC_CAUSE_UNASSIGNED_NUMBER                                   1 
#define    CMID_CC_CAUSE_NO_ROUTE_IN_TRANSIT_NETWORK                         2
#define    CMID_CC_CAUSE_NO_ROUTE_TO_DESTINATION                             3 
#define    CMID_CC_CAUSE_CHANNEL_UNACCEPTABLE                                6 
#define    CMID_CC_CAUSE_CALL_AWARDED                                        7
#define    CMID_CC_CAUSE_OPERATOR_DETERMINED_BARRING                         8 
#define    CMID_CC_CAUSE_NORMAL_CALL_CLEARING                                16 
#define    CMID_CC_CAUSE_USER_BUSY                                           17 
#define    CMID_CC_CAUSE_NO_USER_RESPONDING                                  18 
#define    CMID_CC_CAUSE_USER_ALERTING_NO_ANSWER                             19 
#define    CMID_CC_CAUSE_CALL_REJECTED                                       21 
#define    CMID_CC_CAUSE_NUMBER_CHANGED                                      22 
#define    CMID_CC_CAUSE_PRE_EMPTION                                         25 
#define    CMID_CC_CAUSE_NON_SELECTED_USER_CLEARING                          26 
#define    CMID_CC_CAUSE_DESTINATION_OUT_OF_ORDER                            27 
#define    CMID_CC_CAUSE_INVALID_NUMBER_FORMAT                               28 
#define    CMID_CC_CAUSE_FACILITY_REJECTED                                   29 
#define    CMID_CC_CAUSE_RESPONSE_TO_STATUS_ENQUIRY                          30 
#define    CMID_CC_CAUSE_NORMAL_UNSPECIFIED                                  31 
#define    CMID_CC_CAUSE_NO_CIRCUIT_CHANNEL_AVAILABLE                        34 
#define    CMID_CC_CAUSE_NETWORK_OUT_OF_ORDER                                38 
#define    CMID_CC_CAUSE_TEMPORARY_FAILURE                                   41 
#define    CMID_CC_CAUSE_SWITCHING_EQUIPMENT_CONGESTION                      42 
#define    CMID_CC_CAUSE_ACCESS_INFORMATION_DISCARDED                        43 
#define    CMID_CC_CAUSE_CHANNEL_UNAVAILABLE                                 44 
#define    CMID_CC_CAUSE_RESOURCES_UNAVAILABLE                               47 
#define    CMID_CC_CAUSE_QOS_UNAVAILABLE                                     49 
#define    CMID_CC_CAUSE_FACILITY_NOT_SUBSCRIBED                             50 
#define    CMID_CC_CAUSE_MT_CALLS_BARRED                                     55 
#define    CMID_CC_CAUSE_BC_NOT_AUTHORIZED                                   57 
#define    CMID_CC_CAUSE_BC_NOT_PRESENTLY_AVAILABLE                          58 
#define    CMID_CC_CAUSE_SERVICE_UNAVAILABLE                                 63 
#define    CMID_CC_CAUSE_BEARER_SERVICE_NOT_IMPLEMENTED                      65 
#define    CMID_CC_CAUSE_CHANNEL_NOT_IMPLEMENTED                             66 
#define    CMID_CC_CAUSE_ACM_EQUAL_TO_OR_GREATER_THAN_ACMMAX                 68 
#define    CMID_CC_CAUSE_REQUESTED_FACILITY_NOT_IMPLEMENTED                  69 
#define    CMID_CC_CAUSE_ONLY_RESTRICTED_DIGITAL_INFORMATION_BC_AVAILABLE    70 
#define    CMID_CC_CAUSE_SERVICE_NOT_IMPLEMENTED                             71 
#define    CMID_CC_CAUSE_INVALID_TI_VALUE                                    81 
#define    CMID_CC_CAUSE_CHANNEL_DOES_NOT_EXIST                              82
#define    CMID_CC_CAUSE_CALL_IDENTITY_DOESNT_EXIST                          83
#define    CMID_CC_CAUSE_CALL_IDENTITY_IN_USE                                84
#define    CMID_CC_CAUSE_NO_CALL_SUSPENDED                                   85
#define    CMID_CC_CAUSE_CALL_CLEARED                                        86
#define    CMID_CC_CAUSE_USER_NOT_MEMBER_OF_CUG                              87 
#define    CMID_CC_CAUSE_INCOMPATIBLE_DESTINATION                            88 
#define    CMID_CC_CAUSE_INVALID_TRANSIT_NETWORK                             91 
#define    CMID_CC_CAUSE_SEMANTICALLY_INCORRECT_MESSAGE                      94 
#define    CMID_CC_CAUSE_INVALID_MESSAGE                                     95 
#define    CMID_CC_CAUSE_INVALID_MANDATORY_INFORMATION                       96 
#define    CMID_CC_CAUSE_MESSAGE_TYPE_NOT_IMPLEMENTED                        97 
#define    CMID_CC_CAUSE_MESSAGE_TYPE_NOT_COMPATIBLE                         98 
#define    CMID_CC_CAUSE_IE_NOT_IMPLEMENTED                                  99 
#define    CMID_CC_CAUSE_CONDITIONAL_IE_ERROR                                100 
#define    CMID_CC_CAUSE_MESSAGE_NOT_COMPATIBLE                              101 
#define    CMID_CC_CAUSE_RECOVERY_ON_TIMER_EXPIRY                            102 
#define    CMID_CC_CAUSE_PROTOCOL_ERROR_UNSPECIFIED                          111 
#define    CMID_CC_CAUSE_INTERWORKING_UNSPECIFIED                            127
#define    CMID_CM_CAUSE_SUCCESS                                             255


// CC local error
#define CMID_CC_ERR_NORMAL_RELEASE          0
#define CMID_CC_ERR_LOW_LAYER_FAIL          1
#define CMID_CC_ERR_REQUEST_IMPOSSIBLE      2
#define CMID_CC_ERR_INCOMING_CALL_BARRED    3
#define CMID_CC_ERR_OUTGOING_CALL_BARRED    4
#define CMID_CC_ERR_CALL_HOLD_REJECTED      5
#define CMID_CC_ERR_CALL_RTRV_REJECTED      6
#define CMID_CC_ERR_CC_TIMER_EXPIRY         7
#define CMID_CC_ERR_CC_PROTOCOL_ERROR       8
#define CMID_CC_ERR_NOT_USED                0xff

// Crss error
#define    CMID_CRSS_ERR_RETURN_ERROR                                    0x0002
#define    CMID_CRSS_ERR_GENERAL_PROBLEM_UNRECOGNIZED_COMPONENT          0x8000
#define    CMID_CRSS_ERR_GENERAL_PROBLEM_MISTYPED_COMPONENT              0x8001
#define    CMID_CRSS_ERR_GENERAL_PROBLEM_BADLY_STRUCTURED_COMPONENT      0x8002
#define    CMID_CRSS_ERR_INVOKE_PROBLEM_DUPLICATE_INVOKE_ID              0x8100
#define    CMID_CRSS_ERR_INVOKE_PROBLEM_UNRECOGNIZED_OPERATION           0x8101
#define    CMID_CRSS_ERR_INVOKE_PROBLEM_MISTYPED_PARAMETER               0x8102
#define    CMID_CRSS_ERR_INVOKE_PROBLEM_RESOURCE_LIMITATION              0x8103
#define    CMID_CRSS_ERR_INVOKE_PROBLEM_INITIATING_RELEASE               0x8104
#define    CMID_CRSS_ERR_INVOKE_PROBLEM_UNRECOGNIZED_LINKED_ID           0x8105
#define    CMID_CRSS_ERR_INVOKE_PROBLEM_LINKED_RESPONSE_UNEXPECTED       0x8106
#define    CMID_CRSS_ERR_INVOKE_PROBLEM_UNEXPECTED_LINKED_OPERATION      0x8107
#define    CMID_CRSS_ERR_RETURN_RESULT_PROBLEM_UNRECOGNIZED_INVOKE_ID    0x8200
#define    CMID_CRSS_ERR_RETURN_RESULT_PROBLEM_RETURN_RESULT_UNEXPECTED  0x8201
#define    CMID_CRSS_ERR_RETURN_RESULT_PROBLEM_MISTYPED_PARAMETER        0x8202
#define    CMID_CRSS_ERR_RETURN_ERROR_PROBLEM_UNRECOGNIZED_INVOKE_ID     0x8300
#define    CMID_CRSS_ERR_RETURN_ERROR_PROBLEM_RETURN_ERROR_UNEXPECTED    0x8301
#define    CMID_CRSS_ERR_RETURN_ERROR_PROBLEM_UNRECOGNIZED_ERROR         0x8302
#define    CMID_CRSS_ERR_RETURN_ERROR_PROBLEM_UNEXPECTED_ERROR           0x8303
#define    CMID_CRSS_ERR_RETURN_ERROR_PROBLEM_MISTYPED_PARAMETER         0x8304


//SS code
#define    CMID_SS_NO_CODE 
#define    CMID_SS_CLIP   30                       // CLIP
#define    CMID_SS_CLIR   31                       // CLIR
#define    CMID_SS_COLP   76                       // COLP
#define    CMID_SS_COLR   77                       // COLR
#define    CMID_SS_CNAP   300                      // CNAP
#define    CMID_SS_CFA   2                         // All forwarding
#define    CMID_SS_CFU   21                        // CF unconditional
#define    CMID_SS_CD   66                         // call deflection
#define    CMID_SS_CFC   4                         // CF conditional
#define    CMID_SS_CFB   67                        // CF on MS busy
#define    CMID_SS_CFNRY   61                      // CF on MS no reply
#define    CMID_SS_CFNRC   62                      // CF on MS not reachable
#define    CMID_SS_USSD_V2   100                   // USSD version 2 services  supports
                                                  // all USSD Operations except
                                                  // ss_ProcessUnstructuredSSData
#define    CMID_SS_USSD_V1   101                   // USSD version 1 services  supports
                                                  // ss_ProcessUnstructuredSSData
                                                  // Operations Only
#define    CMID_SS_ECT    96                       // explicit call transfer
#define    CMID_SS_CW     43                       // call waiting
#define    CMID_SS_HOLD   102                      // call hold  (defined by CIITEC)
#define    CMID_SS_MPTY   103                      // multiparty (defined by CIITEC)
#define    CMID_SS_AOCI   104                      // advice of charge information (defined by CIITEC)
#define    CMID_SS_AOCC   105                      // advice of charge charging    (defined by CIITEC)
#define    CMID_SS_UUS1   361                      // uus1
#define    CMID_SS_UUS2   362                      // uus2
#define    CMID_SS_UUS3   363                      // uus3
#define    CMID_SS_AllBarringSS   330              // all call barring
#define    CMID_SS_BarringOfOutgoingCalls   333    // barring of MO call
#define    CMID_SS_BAOC   33                       // barring of all MO call
#define    CMID_SS_BOIC   331                      // barring of international MO call
#define    CMID_SS_BOIC_ExHC   332                 // barring of international MO call
                                            // except those going to the home plmn
#define    CMID_SS_BarringOfIncomingCalls   353    // barring of MT call
#define    CMID_SS_BAIC   35                       // barring of all MT call
#define    CMID_SS_BIC_ROAM   351                  // barring of MT call when roaming
                                            // outside of home plmn

//SS operation code
#define    CMID_SS_REGISTER   10                       // register a SS valid for CF
#define    CMID_SS_ERASURE   11                        // erase a SS valid for CF
#define    CMID_SS_ACTIVATE   12                       // activate a SS valid for all
                                                // SS operations except line
                                                // identification
#define    CMID_SS_DEACTIVATE   13                     // deactivate a SS valid for all
                                                // SS operations except line
                                                // identification
#define    CMID_SS_INTERROGATE   14                    // interrogate a SS
#define    CMID_SS_NOTIFY   16 
#define    CMID_SS_REGISTERPASSWORD   17               // register a password valid for CB
#define    CMID_SS_GETCALLBARRINGPASSWORD   18         // this operation is invoked by
                                                // network only
#define    CMID_SS_PROCESSUNSTRUCTUREDSSDATA   19      // This operation is used only
                                                // for USSD Version 1
#define    CMID_SS_PROCESSUNSTRUCTUREDSSREQUEST   59   // This operation is used only
                                                // for USSD Version 2
#define    CMID_SS_UNSTRUCTUREDSSREQUEST   60          // This operation is used only
                                                      // for USSD Version 2
#define    CMID_SS_UNSTRUCTUREDSSNOTIFY   61           // This operation is used only
                                                // for USSD Version 2
#define    CMID_SS_CALLDEFLECTION    117 
#define    CMID_SS_USERUSERSERVICE   118 
#define    CMID_SS_SPLITMPTY         121 
#define    CMID_SS_RETRIEVEMPTY      122 
#define    CMID_SS_HOLDMPTY          123 
#define    CMID_SS_BUILDMPTY         124 
#define    CMID_SS_FORWARDCHARGEADVICE   125 
#define    CMID_SS_EXPLICITCT            126

//
// PBK Services 
//

#define CMID_PBK_DATA_MAX_SIZE       20
#define PBK_SIM_NUMBER_SIZE         SIM_PBK_NUMBER_SIZE
#define CMID_PBK_NAME_MAX_SIZE       20
#define CMID_PBK_NUMBER_MAX_SIZE     20 // BCD

#define PBK_HINT_SIZE               16

typedef struct _CMID_PBK_ENTRY_SIMPLE_HRD {
 UINT16 nPosition;
 UINT8 nStorageId;
 UINT8 nNameSize;
 UINT8 nNumberSize;
 UINT8 nNumberType;
 UINT8 padding[2];
 UINT8* pName;
 UINT8* pNumber;
} CMID_PBK_ENTRY_SIMPLE_HRD;

typedef struct _CMID_PBK_ENTRY_HRD {
 UINT16 nPosition;
 UINT16 nAppIndex;
 UINT8* pName;
 UINT8* pNumber;
 UINT8 nNameSize;
 UINT8 nNumberSize;
 UINT8 nNumberType;
 UINT8 nStorageId;
 UINT32 nItemGroupType;
 UINT32 nGroup;
} CMID_PBK_ENTRY_HRD;

typedef struct _CMID_PBK_ENTRY_ITEM {
 UINT32 nPosition; 
 UINT32 nItemType;
 UINT32 nItemId;
 UINT8* pItemName; 
 UINT8* pItemData;
 UINT8 nItemNameSize;
 UINT8 nItemDataSize;
 UINT8 nDataType;
 UINT8 padding;
} CMID_PBK_ENTRY_ITEM;

typedef struct _CMID_PBK_DETAIL_ENTRY
{
 CMID_PBK_ENTRY_HRD sHeader;
 UINT32 iItemNum;
 CMID_PBK_ENTRY_ITEM *pItemArray; 
}CMID_PBK_DETAIL_ENTRY;

#define CMID_PBK_SEARCH_NAME_FIRST_MATCH  0x01
#define CMID_PBK_SEARCH_NAME_FULL_MATCH   0x02
#define CMID_PBK_SEARCH_NAME_PY_MATCH     0x04
#define CMID_PBK_SEARCH_NUM_FIRST_MATCH   0x08
#define CMID_PBK_SEARCH_NUM_NEXT_MATCH    0x10
#define CMID_PBK_SEARCH_ENTRY_SIMPLE_INFO 0x20
#define CMID_PBK_SEARCH_ENTRY_HEADER_INFO 0x40
#define CMID_PBK_SEARCH_ENTRY_FULL_INFO   0x80

#define CMID_PBK_OPTION_ENTRY_SIMPLE_HEADER 0x01
#define CMID_PBK_OPTION_ENTRY_FULL_HEADER   0x02
#define CMID_PBK_OPTION_ENTRY_ITEM          0x04

#define CMID_PBK_COPY_ME2SM      0x00
#define CMID_PBK_COPY_SM2ME      0x01
#define CMID_PBK_COPY_ME2SM_STEP 0x02
#define CMID_PBK_COPY_SM2ME_STEP 0x04

#define CMID_PBK_ITEM_TYPE_PHONE_NUMBER   0x1 //Specify the phone number such as mobile number or home number or office numberW 
#define CMID_PBK_ITEM_TYPE_EAMIL          0x2 //
#define CMID_PBK_ITEM_TYPE_ADDERSS        0x4 // 
#define CMID_PBK_ITEM_TYPE_WEBSITE        0x8 //
#define CMID_PBK_ITEM_TYPE_STRING         0x10 // 
#define CMID_PBK_ITEM_TYPE_RES_ID         0x20 // Specify the resource identifier 
#define CMID_PBK_ITEM_TYPE_RES_PATH       0x40 // Specify the resource identifier or resource file path such as icon name. 

typedef struct _CMID_PBK_CAPACITY_INFO { 
 UINT16 nTotalNum;
 UINT8 nPhoneNumberLen;
 UINT8 nTextLen;
 UINT16 nUsedNum;
 UINT8 nStorageId;
 UINT8 padding;
} CMID_PBK_CAPACITY_INFO;

typedef struct _CMID_PBK_CALLLOG_ENTRY{
 UINT8* pNumber;
 UINT8 nNumberSize;
 UINT8 nNumberType;
 UINT16 nPosition;
 UINT8 nTimes;
 UINT8 nStorageId;
 UINT16 nDuration;
 UINT32 nStartTime;
} CMID_PBK_CALLLOG_ENTRY;

//
// GPRS
//
#define CMID_GPRS_DETACHED       0   //detached 
#define CMID_GPRS_ATTACHED       1   //attached 

#define CMID_GPRS_DEACTIVED      0    //deactivated 
#define CMID_GPRS_ACTIVED        1    //activated 

// error define
#define ERR_CMID_GPRS_INVALID_CID       0
#define ERR_CMID_GPRS_HAVE_ACTIVED      1
#define ERR_CMID_GPRS_HAVE_DEACTIVED      2 

// state self defined
#define CMID_GPRS_STATE_IDLE                 0
#define CMID_GPRS_STATE_CONNECTING           1
#define CMID_GPRS_STATE_DISCONNECTING        2
#define CMID_GPRS_STATE_ACTIVE               3
#define CMID_GPRS_STATE_MODIFYING            4
#define CMID_GPRS_STATE_ERROR                5

// Packet Data Protocol type
#define CMID_GPRS_PDP_TYPE_X25   0   // x.25   
#define CMID_GPRS_PDP_TYPE_IP    1   // IP
#define CMID_GPRS_PDP_TYPE_IPV6  1   // IP Version 6
#define CMID_GPRS_PDP_TYPE_OSPIH 1   // internet Hosted Octect Stream Protocol
#define CMID_GPRS_PDP_TYPE_PPP   1   // Point to Point Protocol

// PDP data compression
#define CMID_GPRS_PDP_D_COMP_OFF  0   // off (default if value is omitted)
#define CMID_GPRS_PDP_D_COMP_ON   1   // on(manufacturer preferred compression)
#define CMID_GPRS_PDP_D_COMP_V42  2   // V.42bis
#define CMID_GPRS_PDP_D_COMP_V44  3   // V.44

// PDP header compression
#define CMID_GPRS_PDP_H_COMP_OFF         0   // off (default if value is omitted)
#define CMID_GPRS_PDP_H_COMP_ON          1   // on (manufacturer preferred compression)
#define CMID_GPRS_PDP_H_COMP_RFC1144     2   // RFC1144 (applicable for SNDCP only)
#define CMID_GPRS_PDP_H_COMP_RFC2507     3   // RFC2507
#define CMID_GPRS_PDP_H_COMP_RFC3095     4   // RFC3095 (applicable for PDCP only)

//L2P
#define CMID_GPRS_L2P_NULL               0 // NULL (Obsolete)
#define CMID_GPRS_L2P_PPP                1 // PPP 
#define CMID_GPRS_L2P_PAD                2 // PAD (Obsolete)
#define CMID_GPRS_L2P_X25                3 // x25 (Obsolete)

// Automatic response to a network request for PDP context activation
#define CMID_GPRS_AUTO_RESPONSE_PACKET_DOMAIN_OFF 0	// turn off automatic response for Packet Domain only
#define CMID_GPRS_AUTO_RESPONSE_PACKET_DOMAIN_ON  1	// turn on automatic response for Packet Domain only
#define CMID_GPRS_AUTO_RESPONSE_PS_ONLY           2	// modem compatibility mode, Packet Domain only
#define CMID_GPRS_AUTO_RESPONSE_PS_CS             3  // modem compatibility mode, Packet Domain and circuit switched calls (default)
#ifdef CMID_GPRS_SUPPORT

typedef struct _CMID_GPRS_PDPCONT_INFO {
    UINT8  nPdpType;
    UINT8  nDComp;
    UINT8  nHComp;
    UINT8  nApnSize;
    UINT8  nPdpAddrSize;
    UINT8  nApnUserSize;
    UINT8  nApnPwdSize;
    UINT8  padding[1];
    UINT8* pApnUser;
    UINT8* pApnPwd;
    UINT8* pApn;
    UINT8* pPdpAddr;
} CMID_GPRS_PDPCONT_INFO;

typedef struct _CMID_GPRS_QOS {
    UINT8 nPrecedence;
    UINT8 nDelay;
    UINT8 nReliability;
    UINT8 nPeak;
    UINT8 nMean; 
    UINT8 padding[3];
} CMID_GPRS_QOS;

typedef struct _CMID_GPRS_DATA {
    UINT16 nDataLength;
    UINT8 padding;
    UINT8 pData[1];
}CMID_GPRS_DATA;
#ifdef CMID_MULTI_SIM
//synchronization function
// AT+CGDCONT
UINT32  CMID_GprsSetPdpCxt(
    UINT8 nCid, 
    CMID_GPRS_PDPCONT_INFO *pPdpCont,
    CMID_SIM_ID nSimID
); // [in]nCid, [in]pPdpCont

UINT32  CMID_GprsGetPdpCxt(
    UINT8 nCid, 
    CMID_GPRS_PDPCONT_INFO *pPdpCont,
    CMID_SIM_ID nSimID
); // [in]nCid, [out]pPdpCont

// AT+CGQMIN
// This command allows the TE to specify a Min Quality of Service Profile 
// that is used when the MT sends an Activate PDP Context Request message 
// to the network.
UINT32  CMID_GprsSetMinQos(UINT8 nCid, CMID_GPRS_QOS *pQos, CMID_SIM_ID nSimID);
    
UINT32  CMID_GprsGetMInQos(UINT8 nCid, CMID_GPRS_QOS* pQos, CMID_SIM_ID nSimID);


// AT+CGQREQ
// This command allows the TE to specify a Quality of Service Profile 
// that is used when the MT sends an Activate PDP Context Request message 
// to the network.
UINT32  CMID_GprsSetReqQos(
    UINT8 nCid, 
    CMID_GPRS_QOS *pQos,
    CMID_SIM_ID nSimID
    ); // [in]nCid, [in]pQos
    
UINT32  CMID_GprsGetReqQos(
    UINT8 nCid, 
    CMID_GPRS_QOS *pQos,
    CMID_SIM_ID nSimID); // [in]nCid, [out]pQos

//AT+CGPADDR
//get the address of the selected Pdp context of the gprs network
//[in]nCid: [1-7]
// Specify the PDP Context Identifier (a numeric) which specifies a particular PDP context definition. 
//[in/out]nLength: [in] nLength should be the length that malloc the pdpAdd.
//                 [out] nLength should be the length of the Pdp address.
//[out]pPdpAdd: Point to the buffer to hold the PDP address. 
// return: If the function succeeds, the return value is ERR_SUCCESS, 
// if the function fails, the following error code may be returned.
// ERR_CMID_INVALID_PARAMETER, ERR_CME_UNKNOWN, 

UINT32  CMID_GprsGetPdpAddr( 
    UINT8 nCid, UINT8 *nLength,
    UINT8 *pPdpAdd
    , CMID_SIM_ID nSimID
    );

// using net parameter
//AT+CGREG
// get the status of the gprs network.
// [out]pStatus: Pointer to CMID_NW_STATUS_INFO structure to contain the network status information
// return: If the function succeeds, the return value is ERR_SUCCESS, 
// if the function fails, the following error code may be returned.
// ERR_CMID_INVALID_PARAMETER, ERR_CME_UNKNOWN, 
UINT32  CMID_GprsGetstatus(
    CMID_NW_STATUS_INFO *pStatus,
    CMID_SIM_ID nSimID
);  // 

// AT+CGATT?
// retrieves the state of the GPRS service.
// [out]pState: Point to UINT8 to hold the state of GPRS attachment. 
// This parameter can be one of the following value:
// CMID_GPRS_DETACHED 0 detached 
// CMID_GPRS_ATTACHED 1 attached 
// return: If the function succeeds, the return value is ERR_SUCCESS, 
// if the function fails, the following error code may be returned.
// ERR_CMID_INVALID_PARAMETER, ERR_CME_UNKNOWN, 

UINT32  CMID_GetGprsAttState (
    UINT8* pState,
    CMID_SIM_ID nSimID
);

//
// AT+CGACT? 
// retrieves the state of the GPRS Service according to the selected cid
// [in]nCid:[1-7]
// Specify the PDP Context Identifier (a numeric) which specifies a particular PDP context definition. 
// [out]pState:Point to UINT8 to hold the state of GPRS activation.
// CMID_GPRS_DEACTIVED 0 deactivated 
// CMID_GPRS_ACTIVED 1 activated 
// return: If the function succeeds, the return value is ERR_SUCCESS, 
// if the function fails, the following error code may be returned.
// ERR_CMID_INVALID_PARAMETER, ERR_CME_UNKNOWN, 

UINT32  CMID_GetGprsActState (
    UINT8 nCid, 
    UINT8* pState,
    CMID_SIM_ID nSimID
);

// asynchronism
////------> move to Net

// AT+CGATT  
// ps attach and detach
// nState: 
// CMID_GPRS_DETACHED 0 detached 
// CMID_GPRS_ATTACHED 1 attached 
// event:EV_CMID_GPRS_ATT_NOTIFY
// parameter1: 0
// parameter2: 0
// type: 0 or 1 to distinguish the attach or detach
//UINT32  CMID_GprsAtt(UINT8 nState, UINT16 nUTI);    // notify: success or fail

// AT: AT+CGACT
// PDP context activate or deactivate
// nState:
// CMID_GPRS_DEACTIVED 0 deactivated 
// CMID_GPRS_ACTIVED 1 activated 
// event:EV_CMID_GPRS_ACT_NOTIFY
// parameter1:0
// parameter2:0
// type: 0 or 1 to distingush the active or deactive
UINT32  CMID_GprsAct(
    UINT8 nState, 
    UINT8 nCid, 
    UINT16 nUTI,
    CMID_SIM_ID nSimID
);     // [in] nCid  // notify: success or fail

UINT32 CMID_GprsActEx(
	UINT8 nState,
	UINT8 nCid, 
	UINT16 nUTI, 
	CMID_SIM_ID nSimID,
	BOOL SavedInApp) ;    // [in] nCid  // notify: success or fail

//
// AT: ATA
// Manual acceptance of a network request for PDP context activation 'A'
// [in]nUTI:[1-7]
// Specify the UTI (Unique Transaction Identifier) operation,
// which support multi-application in parallel. This parameter will be 
// return in notification event (see below). You also can get a free UTI by 
// CMID_GetFreeUTI function if you application is a dynamical module. 
// This parameter should be less than 255 for all upper layer application.
// [in]nCid: Specify the PDP Context Identifier (a numeric) which specifies a particular PDP context definition.  
// response event: EV_CMID_GPRS_CXT_ACTIVE_IND_NOTIFY
// parameter1: 0
// parameter2: 0
// return: If the function succeeds, the return value is ERR_SUCCESS, 
// if the function fails, the following error code may be returned.
// ERR_CMID_INVALID_PARAMETER, ERR_CME_UNKNOWN, 
UINT32  CMID_GprsManualAcc(
    UINT16 nUTI, 
    UINT8 nCid,
    CMID_SIM_ID nSimID
);


//
// ATH
// Manual rejection of a network request for PDP context activation 'H'
// The function is used to reject the request of a network for GPRS PDP 
// context activation which has been signaled to the TE by the RING or CRING 
// unsolicited result code.
// [in]UTI:
// [in]nCid:
// return: If the function succeeds, the return value is ERR_SUCCESS, 
// if the function fails, the following error code may be returned.
// ERR_CMID_INVALID_PARAMETER, ERR_CME_UNKNOWN, 
// event: None
// parameter1: None 
// parameter2: None
UINT32  CMID_GprsManualRej(
    UINT16 nUTI,
    UINT8 nCid,
    CMID_SIM_ID nSimID
);


// modify needed
// The function is used to modify the specified PDP context with QoS parameter. 
// After this function has completed, the MT returns to V.25ter online data state. 
// If the requested modification for any specified context cannot be achieved, 
// an event with ERROR information is returned. 
//nCid: 
//[in]a numeric parameter which specifies a particular PDP context definition;
//
//nUTI: 
//[in]
//
//QOS:
//[in] Point to CMID_GPRS_QOS structure to set the Quality of Service.
// return: If the function succeeds, the return value is ERR_SUCCESS, 
// if the function fails, the following error code may be returned.
// ERR_CMID_INVALID_PARAMETER, ERR_CME_UNKNOWN, 

UINT32  CMID_GprsCtxModify(
    UINT16 nUTI, 
    UINT8 nCid, 
    CMID_GPRS_QOS *Qos,
    CMID_SIM_ID nSimID
);

// no response
// This message is sent by MMI to accept the PDP context modification requested by the
// network in an api_PdpCtxModifyInd message.
// return: If the function succeeds, the return value is ERR_SUCCESS, 
// if the function fails, the following error code may be returned.
// ERR_CMID_INVALID_PARAMETER, ERR_CME_UNKNOWN, 
// event: NO
UINT32  CMID_GprsCtxModifyAcc(
    UINT16 nUTI, 
    UINT8 nCid,
    CMID_SIM_ID nSimID
);

// return: If the function succeeds, the return value is ERR_SUCCESS, 
// if the function fails, the following error code may be returned.
// ERR_CMID_INVALID_PARAMETER, ERR_CME_UNKNOWN, 
// event: NO
UINT32  CMID_GprsCtxModifyRej(
    UINT16 nUTI, 
    UINT8 nCid,
    CMID_SIM_ID nSimID
);

//The function is used to send PDP data to the network. The implementation of
// this function is always called by upper protocol 
// send data   //UINT8 nCid, UINT16 nDataLength, UINT8 *pData
// nCid: 
//[in]a numeric parameter which specifies a particular PDP context definition;
//pGprsData: point to the data that need to send
// return: If the function succeeds, the return value is ERR_SUCCESS, 
// if the function fails, the following error code may be returned.
// ERR_CMID_INVALID_PARAMETER,
// event: None
UINT32  CMID_GprsSendData(
    UINT8 nCid, 
    CMID_GPRS_DATA * pGprsData,
    CMID_SIM_ID nSimID
);

UINT32 CMID_GprsAtt(
    UINT8 nState, 
    UINT16 nUTI,
    CMID_SIM_ID nSimID
);
UINT32 CMID_GetGprsSum(INT32* upsum,INT32* downsum,CMID_SIM_ID nSimID );
UINT32 CMID_ClearGprsSum(CMID_SIM_ID nSimID );

#else

//synchronization function
// AT+CGDCONT
UINT32  CMID_GprsSetPdpCxt(
    UINT8 nCid, 
    CMID_GPRS_PDPCONT_INFO *pPdpCont
); // [in]nCid, [in]pPdpCont

UINT32  CMID_GprsGetPdpCxt(
    UINT8 nCid, 
    CMID_GPRS_PDPCONT_INFO *pPdpCont
); // [in]nCid, [out]pPdpCont

// AT+CGQMIN
// This command allows the TE to specify a Min Quality of Service Profile 
// that is used when the MT sends an Activate PDP Context Request message 
// to the network.
UINT32  CMID_GprsSetMinQos(
    UINT8 nCid, 
    CMID_GPRS_QOS *pQos
    ); // [in]nCid, [in]pQos
    
UINT32  CMID_GprsGetMinQos(
    UINT8 nCid, 
    CMID_GPRS_QOS *pQos); // [in]nCid, [out]pQos

// AT+CGQREQ
// This command allows the TE to specify a Quality of Service Profile 
// that is used when the MT sends an Activate PDP Context Request message 
// to the network.
UINT32  CMID_GprsSetReqQos(
    UINT8 nCid, 
    CMID_GPRS_QOS *pQos
    ); // [in]nCid, [in]pQos
    
UINT32  CMID_GprsGetReqQos(
    UINT8 nCid, 
    CMID_GPRS_QOS *pQos); // [in]nCid, [out]pQos

//AT+CGPADDR
//get the address of the selected Pdp context of the gprs network
//[in]nCid: [1-7]
// Specify the PDP Context Identifier (a numeric) which specifies a particular PDP context definition. 
//[in/out]nLength: [in] nLength should be the length that malloc the pdpAdd.
//                 [out] nLength should be the length of the Pdp address.
//[out]pPdpAdd: Point to the buffer to hold the PDP address. 
// return: If the function succeeds, the return value is ERR_SUCCESS, 
// if the function fails, the following error code may be returned.
// ERR_CMID_INVALID_PARAMETER, ERR_CME_UNKNOWN, 

UINT32  CMID_GprsGetPdpAddr( 
    UINT8 nCid, UINT8 *nLength,
    UINT8 *pPdpAdd
    );

// using net parameter
//AT+CGREG
// get the status of the gprs network.
// [out]pStatus: Pointer to CMID_NW_STATUS_INFO structure to contain the network status information
// return: If the function succeeds, the return value is ERR_SUCCESS, 
// if the function fails, the following error code may be returned.
// ERR_CMID_INVALID_PARAMETER, ERR_CME_UNKNOWN, 

UINT32  CMID_GprsGetstatus(
    CMID_NW_STATUS_INFO *pStatus
);  //

// AT+CGATT?
// retrieves the state of the GPRS service.
// [out]pState: Point to UINT8 to hold the state of GPRS attachment. 
// This parameter can be one of the following value:
// CMID_GPRS_DETACHED 0 detached 
// CMID_GPRS_ATTACHED 1 attached 
// return: If the function succeeds, the return value is ERR_SUCCESS, 
// if the function fails, the following error code may be returned.
// ERR_CMID_INVALID_PARAMETER, ERR_CME_UNKNOWN, 

UINT32  CMID_GetGprsAttState (
    UINT8* pState
);


//
// AT+CGACT? 
// retrieves the state of the GPRS Service according to the selected cid
// [in]nCid:[1-7]
// Specify the PDP Context Identifier (a numeric) which specifies a particular PDP context definition. 
// [out]pState:Point to UINT8 to hold the state of GPRS activation.
// CMID_GPRS_DEACTIVED 0 deactivated 
// CMID_GPRS_ACTIVED 1 activated 
// return: If the function succeeds, the return value is ERR_SUCCESS, 
// if the function fails, the following error code may be returned.
// ERR_CMID_INVALID_PARAMETER, ERR_CME_UNKNOWN, 

UINT32  CMID_GetGprsActState (
    UINT8 nCid, 
    UINT8* pState
);

// asynchronism
////------> move to Net

// AT+CGATT  
// ps attach and detach
// nState: 
// CMID_GPRS_DETACHED 0 detached 
// CMID_GPRS_ATTACHED 1 attached 
// event:EV_CMID_GPRS_ATT_NOTIFY
// parameter1: 0
// parameter2: 0
// type: 0 or 1 to distinguish the attach or detach
//UINT32  CMID_GprsAtt(UINT8 nState, UINT16 nUTI);    // notify: success or fail

// AT: AT+CGACT
// PDP context activate or deactivate
// nState:
// CMID_GPRS_DEACTIVED 0 deactivated 
// CMID_GPRS_ACTIVED 1 activated 
// event:EV_CMID_GPRS_ACT_NOTIFY
// parameter1:0
// parameter2:0
// type: 0 or 1 to distingush the active or deactive

UINT32  CMID_GprsAct(
    UINT8 nState, 
    UINT8 nCid, 
    UINT16 nUTI
);     // [in] nCid  // notify: success or fail

UINT32 CMID_GprsActEx(
	UINT8 nState,
	UINT8 nCid, 
	UINT16 nUTI, 
	BOOL SavedInApp);     // [in] nCid  // notify: success or fail

//
// AT: ATA
// Manual acceptance of a network request for PDP context activation 'A'
// [in]nUTI:[1-7]
// Specify the UTI (Unique Transaction Identifier) operation,
// which support multi-application in parallel. This parameter will be 
// return in notification event (see below). You also can get a free UTI by 
// CMID_GetFreeUTI function if you application is a dynamical module. 
// This parameter should be less than 255 for all upper layer application.
// [in]nCid: Specify the PDP Context Identifier (a numeric) which specifies a particular PDP context definition.  
// response event: EV_CMID_GPRS_CXT_ACTIVE_IND_NOTIFY
// parameter1: 0
// parameter2: 0
// return: If the function succeeds, the return value is ERR_SUCCESS, 
// if the function fails, the following error code may be returned.
// ERR_CMID_INVALID_PARAMETER, ERR_CME_UNKNOWN, 

UINT32  CMID_GprsManualAcc(
    UINT16 nUTI, 
    UINT8 nCid
);


//
// ATH
// Manual rejection of a network request for PDP context activation 'H'
// The function is used to reject the request of a network for GPRS PDP 
// context activation which has been signaled to the TE by the RING or CRING 
// unsolicited result code.
// [in]UTI:
// [in]nCid:
// return: If the function succeeds, the return value is ERR_SUCCESS, 
// if the function fails, the following error code may be returned.
// ERR_CMID_INVALID_PARAMETER, ERR_CME_UNKNOWN, 
// event: None
// parameter1: None 
// parameter2: None

UINT32  CMID_GprsManualRej(
    UINT16 nUTI,
    UINT8 nCid
);


// modify needed
// The function is used to modify the specified PDP context with QoS parameter. 
// After this function has completed, the MT returns to V.25ter online data state. 
// If the requested modification for any specified context cannot be achieved, 
// an event with ERROR information is returned. 
//nCid: 
//[in]a numeric parameter which specifies a particular PDP context definition;
//
//nUTI: 
//[in]
//
//QOS:
//[in] Point to CMID_GPRS_QOS structure to set the Quality of Service.
// return: If the function succeeds, the return value is ERR_SUCCESS, 
// if the function fails, the following error code may be returned.
// ERR_CMID_INVALID_PARAMETER, ERR_CME_UNKNOWN, 

UINT32  CMID_GprsCtxModify(
    UINT16 nUTI, 
    UINT8 nCid, 
    CMID_GPRS_QOS *Qos
);

// no response
// This message is sent by MMI to accept the PDP context modification requested by the
// network in an api_PdpCtxModifyInd message.
// return: If the function succeeds, the return value is ERR_SUCCESS, 
// if the function fails, the following error code may be returned.
// ERR_CMID_INVALID_PARAMETER, ERR_CME_UNKNOWN, 
// event: NO

UINT32  CMID_GprsCtxModifyAcc(
    UINT16 nUTI, 
    UINT8 nCid
);

// return: If the function succeeds, the return value is ERR_SUCCESS, 
// if the function fails, the following error code may be returned.
// ERR_CMID_INVALID_PARAMETER, ERR_CME_UNKNOWN, 
// event: NO

UINT32  CMID_GprsCtxModifyRej(
    UINT16 nUTI, 
    UINT8 nCid
);

//The function is used to send PDP data to the network. The implementation of
// this function is always called by upper protocol 
// send data   //UINT8 nCid, UINT16 nDataLength, UINT8 *pData
// nCid: 
//[in]a numeric parameter which specifies a particular PDP context definition;
//pGprsData: point to the data that need to send
// return: If the function succeeds, the return value is ERR_SUCCESS, 
// if the function fails, the following error code may be returned.
// ERR_CMID_INVALID_PARAMETER,
// event: None
UINT32  CMID_GprsSendData(
    UINT8 nCid, 
    CMID_GPRS_DATA * pGprsData
);

UINT32 CMID_GprsAtt(
    UINT8 nState, 
    UINT16 nUTI
);

#endif
#endif
//
//Tcpip
//
#ifdef CMID_TCPIP_SUPPORT
typedef INT8 SOCKET;
#define CMID_TCPIP_AF_UNSPEC       0
#define CMID_TCPIP_AF_INET         2


#define CMID_TCPIP_IPPROTO_IP      0
#define CMID_TCPIP_IPPROTO_TCP     6
#define CMID_TCPIP_IPPROTO_UDP     17

#define CMID_TCPIP_SOCK_STREAM     1
#define CMID_TCPIP_SOCK_DGRAM      2
#define CMID_TCPIP_SOCK_RAW        3

#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR               -1
struct ip_addr
{
	UINT32 addr;
};

typedef struct _in_addr
{
	UINT32 s_addr;
}in_addr;
typedef struct _CMID_TCPIP_SOCKET_ADDR
{
	UINT8 sin_len;
	UINT8 sin_family;
	UINT16 sin_port;
       in_addr sin_addr;
	INT8 sin_zero[8];
}CMID_TCPIP_SOCKET_ADDR;

//add for DNS
typedef enum e_resolv_result {
  RESOLV_QUERY_INVALID,
  RESOLV_QUERY_QUEUED,
  RESOLV_COMPLETE
} RESOLV_RESULT;

#define RESOLV_NETWORK_ERROR      0x01
#define RESOLV_HOST_NO_FOUND      0x02


typedef struct _CMID_WIFI_DATA {
    UINT16 nDataLength;
    UINT8 srcMac[6];
    UINT8 destMac[6];
    UINT8 *pData;
}CMID_WIFI_DATA;

typedef struct _CMID_WIFI_NETIF {
    UINT8 Mac[6];
  //  UINT8 gwMac[6];
    struct ip_addr ipaddr;
}CMID_WIFI_NETIF;
//add end



#define CMID_ERR_TCPIP_OK    0        /* No error, everything OK. */
#define CMID_ERR_TCPIP_MEM  1        /* Out of memory error.     */
#define CMID_ERR_TCPIP_BUF  2        /* Buffer error.            */

#define CMID_ERR_TCPIP_ABRT 3        /* Connection aborted.      */
#define CMID_ERR_TCPIP_RST  4        /* Connection reset.        */
#define CMID_ERR_TCPIP_CLSD 5        /* Connection closed.       */
#define CMID_ERR_TCPIP_CONN 6        /* Not connected.           */

#define CMID_ERR_TCPIP_VAL  7        /* Illegal value.           */

#define CMID_ERR_TCPIP_ARG  8        /* Illegal argument.        */

#define CMID_ERR_TCPIP_RTE  9        /* Routing problem.         */

#define CMID_ERR_TCPIP_USE  10        /* Address in use.          */

#define CMID_ERR_TCPIP_IF   11        /* Low-level netif error    */
#define CMID_ERR_TCPIP_ISCONN 12        /* Already connected.       */
#define CMID_ERR_TCPIP_CONN_TIMEOUT 13        /* connect time out       */

//add end

VOID CMID_SetPppSendFun(BOOL (* sendCallBack) (UINT8 *pData, UINT16 uDataSize, UINT8 nDLCI));
VOID CMID_SetGetSimCidFun(VOID (*get_simid_cid)(UINT8 *pSimId, UINT8 *pCid, UINT8 nDLCI));

UINT32 CMID_TcpipInetAddr(const INT8 *cp);
UINT16 CMID_TcpipAvailableBuffer(SOCKET nSocket);
UINT16 CMID_TcpipGetRecvAvailable(SOCKET nSocket);
UINT32 CMID_TcpipGetLastError(VOID);
SOCKET CMID_TcpipSocket(UINT8 nDomain, UINT8 nType, UINT8 nProtocol);
UINT32 CMID_TcpipSocketConnect(SOCKET nSocket,  CMID_TCPIP_SOCKET_ADDR *pName, UINT8 nNameLen);
UINT32 CMID_TcpipSocketSend(SOCKET nSocket, UINT8 *pData, UINT16 nDataSize, UINT32 nFlags);
UINT32 CMID_TcpipSocketRecv(SOCKET nSocket, UINT8* pData, UINT16 nDataSize, UINT32 nFlags);
UINT32 CMID_TcpipSocketClose(SOCKET nSocket);
UINT32 CMID_TcpipSocketShutdownOutput(SOCKET nSocket, INT32 how);
UINT32 CMID_TcpipSocketSendto(SOCKET nSocket, VOID *pData, UINT16 nDataSize, UINT32 nFlags, CMID_TCPIP_SOCKET_ADDR *to, INT32 tolen);
UINT32 CMID_TcpipSocketRecvfrom(SOCKET nSocket, VOID *pMem, INT32 nLen, UINT32 nFlags, CMID_TCPIP_SOCKET_ADDR *from, INT32 * fromlen);
UINT32 CMID_TcpipSocketBind(SOCKET nSocket,  CMID_TCPIP_SOCKET_ADDR *pName, UINT8 nNameLen);
UINT32 CMID_TcpipSocketListen(SOCKET nSocket, UINT32 backlog);
UINT32 CMID_TcpipSocketAccept(SOCKET nSocket,  CMID_TCPIP_SOCKET_ADDR *addr, UINT32 * addrlen);
UINT32 CMID_TcpipSocketGetsockname(SOCKET nSocket, CMID_TCPIP_SOCKET_ADDR *pName, INT32 * pNameLen);
UINT32 CMID_TcpipSocketGetpeername(SOCKET nSocket, CMID_TCPIP_SOCKET_ADDR *pName, INT32 * pNameLen);//
#ifdef CMID_MULTI_SIM
UINT32 CMID_Gethostbyname(char *hostname, struct ip_addr *addr, UINT8 nCid, CMID_SIM_ID nSimID);

#else
UINT32 CMID_Gethostbyname(char *hostname, struct ip_addr *addr, UINT8 nCid);

//UINT32 CMID_Gethostbyname(char *hostName, struct ip_addr *addr); 

#endif
UINT32 CMID_TcpipSocketConnectEx(SOCKET nSocket,  CMID_TCPIP_SOCKET_ADDR *pName, UINT8 nNameLen
#ifdef CMID_MULTI_SIM
                                     , CMID_SIM_ID nSimID
#endif
);


UINT32 CMID_SetDnsServer(INT8 *PriDnsServer, INT8 *SecDnsServer);

UINT32 CMID_GetDnsAddr( struct ip_addr *PriDnsSer, struct ip_addr *SecDnsSer );
#endif
// Shell 
//


#ifdef CMID_GPRS_SUPPORT
//Errors related to a failure to perform an Attach
#define ERR_CMID_GPRS_ILLEGAL_MS                                 103	//Illegal MS (#3)
#define ERR_CMID_GPRS_ILLEGAL_ME                                 106	//Illegal ME (#6)
#define ERR_CMID_GPRS_GPRS_SERVICES_NOT_ALLOWED                  107	//GPRS services not allowed (#7)
#define ERR_CMID_GPRS_PLMN_NOT_ALLOWED                           111	//PLMN not allowed (#11)
#define ERR_CMID_GPRS_LOCATION_AREA_NOT_ALLOWED                  112	//Location area not allowed (#12)
#define ERR_CMID_GPRS_ROAMING_NOT_ALLOWED_IN_THIS_LOCATION_AREA  113 //Roaming not allowed in this location area (#13)


//Errors related to a failure to Activate a Context
#define ERR_CMID_GPRS_SERVICE_OPTION_NOT_SUPPORTED                   132 //service option not supported (#32)
#define ERR_CMID_GPRS_REQUESTED_SERVICE_OPTION_NOT_SUBSCRIBED        133 // requested service option not subscribed (#33)
#define ERR_CMID_GPRS_SERVICE_OPTION_TEMPORARILY_OUT_OF_ORDER        134 // service option temporarily out of order (#34)
#define ERR_CMID_GPRS_PDP_AUTHENTICATION_FAILURE                     149 // PDP authentication failure
#define ERR_CMID_GPRS_UNSPECIFIED                                    148 // unspecified GPRS error
#endif
//
// Some functions need be supported by OS Kernerl(KL) based on the current target platform(MIPS).
//
typedef UINT32 (*PFN_KL_SENDMESSAGDE2APP)(UINT32 nEventId, VOID* pData, UINT32 nSize);
typedef UINT32 (*PFN_KL_SENDMESSAGDE_EX)(UINT32 nMailBox, UINT32 nEventId, VOID* pData, UINT32 nSize);
typedef PVOID (*PFN_KL_MALLOC)(UINT32 nSize);
typedef BOOL (*PFN_KL_FREE)(PVOID nAddr);
typedef BOOL (*PFN_KL_IS_VALIDHEAP)(VOID* pData, UINT32 nSize);
typedef BOOL (*PFN_AUD_START_SPK_MIC)(VOID);
typedef BOOL (*PFN_AUD_STOP_SPK_MIC)(VOID);
    
typedef struct _CMID_PFN_AUD_CBS {
    PFN_AUD_START_SPK_MIC m_pfnAudStartSpkMic;
    PFN_AUD_STOP_SPK_MIC m_pfnAudStopSpkMic;
}CMID_PFN_AUD_CBS;

typedef struct _CMID_PFN_KL_CBS {
    PFN_KL_SENDMESSAGDE2APP m_pfnKlSendMsg2App;
    PFN_KL_SENDMESSAGDE_EX m_pfnKlSendMsgEx;
    PFN_KL_MALLOC m_pfnKlMalloc;
    PFN_KL_FREE m_pfnKlFree;
    PFN_KL_IS_VALIDHEAP m_pfnKlIsValidHeap;
}CMID_PFN_KL_CBS;

//
//
typedef struct _CMID_SAT_MENU_RSP_LIST 
{
	UINT8 nComID;
	UINT8 nComQualifier;
	UINT16 nAlphaLen;
	UINT8* pAlphaStr;
	CMID_SAT_ITEM_LIST * pItemList; //Link list
}CMID_SAT_MENU_RSP_LIST, CMID_SAT_MENU_RSP;

#define CMID_PBK_HINT_MAX_SIZE       16

typedef struct _CMID_SMS_OUT_PARAM{
 UINT8 nType;
 UINT8 padding[3];
 UINT32 nParam1;
 UINT32 nParam2;
} CMID_SMS_OUT_PARAM;
// CMID Init status phase.
#define CMID_INIT_STATUS_NO_SIM      (0x01)
#define CMID_INIT_STATUS_SIM         (0x02)
#define CMID_INIT_STATUS_SAT         (0x04)
#define CMID_INIT_STATUS_SIMCARD     (0x08)
#define CMID_INIT_STATUS_SMS         (0x10)
#define CMID_INIT_STATUS_PBK         (0x20)
#define CMID_INIT_STATUS_NET         (0x40)
#define CMID_INIT_STATUS_AUTODAIL    (0x80)
#define CMID_INIT_STATUS_NO_MEMORY   (0x81)
#define CMID_INIT_STATUS_SIM_DROP    (0x82)
//
// CMID Init status 
//
#define CMID_INIT_SIM_CARD_BLOCK     0x4
#define CMID_INIT_SIM_WAIT_PIN1      0x8
#define CMID_INIT_SIM_WAIT_PUK1      0x10
#define CMID_INIT_SIM_PIN1_READY     0x20

#define CMID_INIT_SIM_WAIT_PS        0x40
#define CMID_INIT_SIM_WAIT_PF        0x80

#define CMID_INIT_SIM_CARD_BAD       0x100
#define CMID_INIT_SIM_CARD_ABNORMITY 0x200
#define CMID_INIT_SIM_TEST_CARD      0x400
#define CMID_INIT_SIM_NORMAL_CARD    0x800
#define CMID_INIT_SIM_CARD_CHANGED   0x1000
#define CMID_INIT_SIM_SAT            0x2000

//
// CMID Exit status 
//
#define CMID_EXIT_STATUS_NETWORK_DETACHMENT           (0x01)
#define CMID_EXIT_STATUS_SIM_POWER_OFF                (0x02)
#define CMID_EXIT_STATUS_CMID_EXIT                     (0x10)

#define CMID_SMS_MAX_REC_COUNT PHONE_SMS_ENTRY_COUNT
#define CMID_SMS_REC_DATA_SIZE 184
INT32 sms_creat_file(void);

// update the global sim pin retry struct define in SettingsSecurity.cpp, need to be remove later
void CMID_Sim_Update_Pin_Retry_Num(CMID_SIM_ID sim_id, int type, int retry_num);


#ifdef __cplusplus
}
#endif

#include "cmid_2.h"

#endif // _H

