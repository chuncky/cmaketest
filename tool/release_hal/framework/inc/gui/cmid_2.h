
#ifndef _CMID_2_H_
#define _CMID_2_H_

/* VincentWei > FIXME: we should remove all uses of this macro. */
#ifndef __NGUX_PLATFORM__
#define __NGUX_PLATFORM__
#endif

#include "uhaltype.h"
#include "errorcode.h"
#include "cmid_multi_sim.h"
#include "event.h"
#include "tm.h"
#include "csw_mem_prv.h"
#include "ui_pm.h"

#if defined(CMID_SERVICE_DEV_SINGLE_DEMO)  //CT_PLATFORM!=CT_JADE
#pragma comment (lib,"cmid_shell.lib")
#pragma message ("Linking with cmid_sim.lib")
#endif

#ifdef __cplusplus
extern "C" {
#endif

/************************************************ENGINERRING TEST MODE**************************************************/
//
//add  for emod at 20060819
//

typedef struct _CMID_EMOD_BATTERY_CHARGE_INFO{
  UINT8 nBcs;								//Point to the unsigned 8-bit variable  to retrieve the connection status of battery pack. 
  											//This parameter can be one of the following:
											//0 No charging adapter is connected 
											//1 Charging adapter is connected 
											//2 Charging adapter is connected, charging in progress 
											//3 Charging adapter is connected, charging has finished 
											//4 Charging error, charging is interrupted 
											//5 False charging temperature, charging is interrupted while temperature is beyond allowed range 
  UINT8 nBcl;								//Point to the unsigned 8-bit variable to retrieve the battery level. This parameter can be
  											//one of the following:
											//0 battery is exhausted, or does not have a battery connected 
											// 20, 40, 60, 80, 100 percent of remaining capacity.
											//The percent is not accurate ,but a estimated expression. 
  UINT16 nMpc;								//Point to the unsigned 16-bit variable to retrieve the average power consumption.
  											//This parameter can be one of the following value:
  											//0-5000 Value (0...5000) of average power consumption (mean value over a couple of seconds) in mA. 
} CMID_EMOD_BATTERY_CHARGE_INFO;

typedef struct _CMID_EMOD_BATTERY_INFO
{
	
	PM_BATTERY_INFO				 nBatState; 
	CMID_EMOD_BATTERY_CHARGE_INFO nChargeInfo;
	UINT8						 nChemistry;//
}CMID_EMOD_BATTERY_INFO;



void CMID_EmodAudioTestStart(UINT8 type);			//type value: 0 is normal; 1 is mic AUX and speak AUX; 2 is mic loud and speak loud.	

void CMID_EmodAudioTestEnd(void);

#ifdef CMID_MULTI_SIM
void CMID_EmodGetIMEI(UINT8* pImei,UINT8* pImeiLen,CMID_SIM_ID nSimID);				//pImei is the buffer that
#else
void CMID_EmodGetIMEI(UINT8* pImei,UINT8* pImeiLen);				//pImei is the buffer that 
#endif
//
//bind 
//

typedef struct _CMID_EMOD_BIND_SIM
{
	UINT8 nIndex;							
	UINT8 nImsiLen;
	UINT8 pImsiNum[15];
	UINT8 pad[3];
}CMID_EMOD_BIND_SIM;

typedef struct _CMID_EMOD_BIND_NW
{
	UINT8 nIndex;
	UINT8 pNccNum[3];
}CMID_EMOD_BIND_NW;

typedef struct _CMID_EMOD_READ_BIND_SIM
{
	UINT8 nNumber; 
	UINT32 nPointer;
	UINT8 pad[3];
}CMID_EMOD_READ_BIND_SIM;

typedef struct _CMID_EMOD_READ_BIND_NW
{
	UINT8 nNumber;
	UINT32 nPointer;
	UINT8 pad[3];
}CMID_EMOD_READ_BIND_NW;





typedef struct _CMID_EMOD_UPDATE_RETURN
{
	UINT32 nErrCode;
	UINT8  nIndex;
}CMID_EMOD_UPDATE_RETURN;

//SN number
typedef struct _CMID_EMOD_LOCT_PRODUCT_INFO
{
	UINT8 nBoardSNLen;
	UINT8 nBoardSN[64];
	UINT8 nPhoneSNLen;
	UINT8 nPhoneSN[64];
	UINT8 nUniqueSNLen;
	UINT8 nUniqueSN[64];
}CMID_EMOD_LOCT_PRODUCT_INFO;

typedef struct _CMID_EMOD_RF_CALIB_INFO
{

	UINT16 nLowDacLimit[6];
	UINT16 nLowVoltLimit[6];
	UINT32 nName;
	INT16   nPcl2dbmArfcn_d[30];
	INT16   nPcl2dbmArfcn_g[30];
	INT16   nPcl2dbmArfcn_p[30];
	INT16  nProfiledBmMax_dp;
	INT16  nProfiledBmMax_g;
	INT16  nProfiledBmMin_dp;
	INT16  nProfiledBmMin_g;
	UINT16 nProfileInterp_dp[16];
	UINT16 nProfileInterp_g[16];
	UINT16 nRampHigh[32];
	UINT16 nRampLow[32];
	UINT16 nRampSwap_dp;
	UINT16 nRampSwap_g;
	INT32  nRaram[15];
	INT16  nTimes[15];
}CMID_EMOD_RF_CALIB_INFO;


//
//  test mode of outfield
//

#define CMID_TSM_CURRENT_CELL   				0x01
#define CMID_TSM_NEIGHBOR_CELL				0x02


/*********************************************************************************************************/
typedef struct _CMID_TSM_CURR_CELL_INFO
{  
	UINT16	 nTSM_Arfcn;			//Absolute Radio Frequency Channel Number ,[0..1023].
    UINT8 	 nTSM_LAI[5];			//Location Area Identification.
    UINT8	 nTSM_RAC;				//Routing Area Code ( GPRS only),  [0..255].
    UINT8    nTSM_CellID[2];		//Cell Identity.
    UINT8 	 nTSM_Bsic;				//Synchronisation code,[0..63].
    BOOL     nTSM_BCCHDec;			//Indicates if the BCCH info has been decoded on the cell.
    UINT8    nTSM_NetMode;			//Network Mode of Operation (GPRS only),[0..2].
    UINT8    nTSM_AvRxLevel;		//Average Rx level in Abs Val of dBm ,[0..115].
    UINT8    nTSM_MaxTxPWR;			//Max allowed transmit power, [0..31].
    INT8     nTSM_C1Value;			//Cell quality ,[-33..110].
    UINT8    nTSM_BER;				//Bit Error Rate [0..255],255 = non significant.
    UINT8 	 nTSM_TimeADV;			//Timing Advance (valid in GSM connected mode only).[0..255] 255 = non significant.
	UINT8    nTSM_CCCHconf ;		//ccch configuration
									//0:CCCH_1_CHANNEL_WITH_SDCCH meanning 1 basic physical channel used for CCCH, combined with SDCCH
									//1:CCCH_1_CHANNEL 	meanning 1 basic physical channel used for CCCH, not combined with SDCCH
									//2:CCCH_2_CHANNEL	meanning 2 basic physical channel used for CCCH, not combined with SDCCH
									//3:CCCH_3_CHANNEL	meanning 3 basic physical channel used for CCCH, not combined with SDCCH
									//4:CCCH_4_CHANNEL	meanning 4 basic physical channel used for CCCH, not combined with SDCCH
									//0xFF:INVALID_CCCH_CHANNEL_NUM	meanning Invalid value
	UINT8    nTSM_RxQualFull ;		//receive quality full	[0бн7]
	UINT8    nTSM_RxQualSub;		//receive quality sub	[0бн7]
	INT16    nTSM_C2;				//Value of the cell reselection criterium	[-96бн173]
	UINT8    nTSM_CurrChanType;		//Current channel type 
									//0:CHANNEL_TYPE_SDCCH meanning Current channel type is SDCCH
									//1:CHANNEL_TYPE_TCH_H0 meanning Current channel type is TCH ,half rate,Subchannel 0
									//2:CHANNEL_TYPE_TCH_H1 meanning Current channel type is TCH ,half rate,Subchannel 1
									//3:CHANNEL_TYPE_TCH_F meanningCurrent channel type is TCH ,full rate
									//0xff:INVALID_CHANNEL_TYPE	meanning Invalid channel type
	UINT32   nTSM_CurrChanMode;		//current channel mode ,only valid in decicated mode
									//API_SIG_ONLY	1<<0	signalling only
									//API_SPEECH_V1	1<<1	speech full rate or half rate version 1
									//API_SPEECH_V2	1<<2	speech full rate or half rate version 2
									//API_SPEECH_V3	1<<3	speech full rate or half rate version 3
									//API_43_14_KBS	1<<4	data, 43.5kbit/s dwnlnk, 14.5kbit/s uplnk
									//API_29_14_KBS	1<<5	data, 29.0kbit/s dwnlnk, 14.5kbit/s uplnk
									//API_43_29_KBS	1<<6	data, 43.5kbit/s dwnlnk, 29.0kbit/s uplnk
									//API_14_43_KBS	1<<7	data, 14.5kbit/s dwnlnk, 43.5kbit/s uplnk
									//API_14_29_KBS 1<<8	data, 14.5kbit/s dwnlnk, 29.0kbit/s uplnk
									//API_29_43_KBS	1<<9	data, 29.0kbit/s dwnlnk, 43.5kbit/s uplnk
									//API_43_KBS	1<<10	data, 43.5kbit/s
									//API_32_KBS	1<<11	data, 32.0kbit/s
									//API_29_KBS	1<<12	data, 29.0kbit/s
									//API_14_KBS	1<<13	data, 14.5kbit/s
									//API_12_KBS	1<<14	data, 12.0kbit/s
									//API_6_KBS  	1<<15	data, 6.0kbit/s
									//API_3_KBS		1<<16	data, 3.6kbit/s
									//API_INVALID_CHAN_MODE	0xFFFFFFFF	Invalid channel mode
	UINT8    nTSM_CurrBand;			//current band
									//9:BAND_GSM900	meanning Band GSM900P, GSM900E and GSM900R
									//18:BAND_DCS1800 meanning Band DCS1800
									//19:BAND_PCS1900 meanning Band PCS 1900
									//0xFF:BAND_INVALID	meanning Invalid band value
	UINT8    nTSM_MaxRetrans;		//max number of random access retransmission	1,2,4,7
	UINT8    nTSM_BsAgBlkRes;		//block number kept for AGCH in common channel	[0бн7]
	UINT8    nTSM_AC[2];			//Access Class	[0бн255]
	UINT8    nTSM_RxLevAccMin;		//receive level access minimum	[0бн63]
	BOOL   	 bTSM_EC; 				//Indicates if emergency call has been allowed
	BOOL     bTSM_SI7_8;			//Indicates if SI7 and 8 are broadcast
	BOOL   	 bTSM_ATT;   			//Indicates if IMSI attach has been allowed
	UINT8 	 pad[3];
}CMID_TSM_CURR_CELL_INFO;



typedef struct _CMID_TSM_NEIGHBOR_CELL_INFO
{  
    UINT16 nTSM_Arfcn;			//Absolute Radio Frequency Channel Number ,[0..1023].
    UINT8 nTSM_Bsic;			//Synchronisation code,[0..63].
    BOOL  nTSM_BCCHDec;			//Indicates if the BCCH info has been decoded on the cell.
    UINT8 nTSM_AvRxLevel;		//Average Rx level in Abs Val of dBm ,[0..115].
    UINT8 nTSM_RxLevAM;			//RxLev Access Min in RxLev [0..63].
    UINT8 nTSM_MaxTxPWR;		//Max allowed transmit power, [0..31].
    INT8 nTSM_C1Value;			//Cell quality ,[-33..110].
    UINT8 	 nTSM_LAI[5];			//Location Area Identification.
    UINT8    nTSM_CellID[2];		//Cell Identity.
}CMID_TSM_NEIGHBOR_CELL_INFO;

typedef struct _CMID_TSM_ALL_NEBCELL_INFO
{  
   UINT8 						nTSM_NebCellNUM;	//neighbor cell number.
   CMID_TSM_NEIGHBOR_CELL_INFO 	nTSM_NebCell[6];  	//the max of the neighbor cell is 6.
   UINT8						pad[3];
}CMID_TSM_ALL_NEBCELL_INFO;



typedef struct _CMID_TSM_FUNCTION_SELECT
{
	BOOL	nServingCell;							//true:enable the function ,false:disable.
	BOOL 	nNeighborCell;							//true:enable the function ,false:disable.
	UINT8 	pad[2];
}CMID_TSM_FUNCTION_SELECT;

/*
================================================================================
  API FUNCTION OF TSM
================================================================================
*/

#ifdef CMID_MULTI_SIM
UINT32 CMID_EmodOutfieldTestStart(CMID_TSM_FUNCTION_SELECT* pSelecFUN,UINT16 nUTI,CMID_SIM_ID nSimID);
UINT32  CMID_EmodOutfieldTestEnd(UINT16 nUTI,CMID_SIM_ID nSimID);
#else
UINT32  CMID_EmodOutfieldTestStart(CMID_TSM_FUNCTION_SELECT* pSelecFUN,UINT16 nUTI);	
UINT32  CMID_EmodOutfieldTestEnd(UINT16 nUTI);
#endif
#ifdef CMID_MULTI_SIM
UINT32 CMID_GetCellInfo(CMID_TSM_CURR_CELL_INFO *pCurrCellInfo, CMID_TSM_ALL_NEBCELL_INFO *pNeighborCellInfo,CMID_SIM_ID nSimID);
#else
UINT32 CMID_GetCellInfo(CMID_TSM_CURR_CELL_INFO *pCurrCellInfo, CMID_TSM_ALL_NEBCELL_INFO *pNeighborCellInfo);
#endif

#ifdef CMID_MULTI_SIM
UINT32 CMID_EmodSyncInfoTest(BOOL bStart,UINT16 nUTI,CMID_SIM_ID nSimID);
#else
UINT32 CMID_EmodSyncInfoTest(BOOL bStart,UINT16 nUTI);
#endif
UINT32 CMID_LockFullService(BOOL bLock,CMID_SIM_ID nSimID);

#define CMID_SYNC_INFO_IND	0x01  
#define CMID_POWER_LIST_IND	0x02

#define CMID_EMOD_POWER_LIST_COUNT  50

typedef struct
{
    UINT16  nArfcn;
	UINT16  nRssi;
}CMID_EmodPowerInfo;

typedef struct
{
   CMID_EmodPowerInfo nPowerList[CMID_EMOD_POWER_LIST_COUNT];
   UINT8   nCount;
}CMID_EmodPowerListInd;


typedef enum{
    PHY_VERSION,
    STACK_VERSION,
    PHY_CFG_VERSION,
    CSW_VERSION,
    MMI_VERSION
}SOFT_VERSION;

typedef enum{
    XCV_VERSION,
    PA_VERSION,
    SW_VERSION,
    CHIP_VERSION,
    FLASH_VERSION
}HW_VERSION;

typedef struct _Soft_Version{
  UINT8  nVersion[30];
  UINT32 nDate;
}Soft_Version;

typedef struct _HW_Version{
  UINT8  nVersion[30];
}HW_Version;

#define ML_CP437        "CP437"     
#define ML_CP737        "CP737"     
#define ML_CP775        "CP775"     
#define ML_CP850        "CP850"     
#define ML_CP852        "CP852"     
#define ML_CP855        "CP855"     
#define ML_CP857        "CP857"     
#define ML_CP860        "CP860"     
#define ML_CP861        "CP861"     
#define ML_CP862        "CP862"     
#define ML_CP863        "CP863"     
#define ML_CP864        "CP864"     
#define ML_CP865        "CP865"     
#define ML_CP869        "CP869"     
#define ML_CP874        "CP874"     
#define ML_CP932        "CP932"     
#define ML_CP936        "CP936"     
#define ML_CP950        "CP950"   
#define ML_CP1250				"CP1250"   
#define ML_CP1251       "CP1251"   
#define ML_CP1255       "CP1255"   
#define ML_CP1256       "CP1256"   
#define ML_CP1258       "CP1258"   
#define ML_ISO8859_1    "ISO8859_1" 
#define ML_ISO8859_2    "ISO8859_2" 
#define ML_ISO8859_3    "ISO8859_3" 
#define ML_ISO8859_4    "ISO8859_4" 
#define ML_ISO8859_5    "ISO8859_5" 
#define ML_ISO8859_6    "ISO8859_6" 
#define ML_ISO8859_7    "ISO8859_7" 
#define ML_ISO8859_9    "ISO8859_9" 
#define ML_ISO8859_13   "ISO8859_13"
#define ML_ISO8859_14   "ISO8859_14"
#define ML_ISO8859_15   "ISO8859_15"

UINT32 reg_GetRegSize(VOID);
INT32 REG_Read(UINT32 ulAddrOffset,UINT8* pBuffer, UINT32 ulBytesToRead, UINT32* pBytesRead);
INT32 REG_Write(UINT32 ulAddrOffset,CONST UINT8* pBuffer,UINT32 ulBytesToWrite,UINT32*  pBytesWritten);



VOID  CMID_CcSetCSSI (  BOOL nCSSI);
VOID CMID_CcSetCSSU ( BOOL nCSSU );

#ifdef CMID_MULTI_SIM
UINT32 CMID_GetFreeCID(UINT8* pCID, CMID_SIM_ID nSimID);
UINT32 CMID_ReleaseCID(UINT8 nCID, CMID_SIM_ID nSimID);
UINT32 CMID_SetCID(UINT8 nCID, CMID_SIM_ID nSimID);

#else

UINT32 CMID_GetFreeCID(UINT8* pCID);
UINT32 CMID_ReleaseCID(UINT8 nCID);
UINT32 CMID_SetCID(UINT8 nCID);

#endif

//#ifdef CMID_EXTENDED_API
#ifdef CMID_MULTI_SIM
UINT32 cmid_SimReadElementaryFile(UINT16 nUTI,UINT32 fileId,CMID_SIM_ID nSimID);
UINT32 CMID_SimGetACMMax(UINT16 nUTI,CMID_SIM_ID nSimID);
UINT32 CMID_SimSetACMMax(UINT32 iACMMaxValue, UINT8 *pPin2, UINT8 nPinSize, UINT16 Nuti,CMID_SIM_ID nSimID);
UINT32 CMID_SimGetACM(UINT16 nUTI,CMID_SIM_ID nSimID);
UINT32 CMID_SimSetACM(UINT32 iCurValue, UINT8 *pPin2, UINT8 nPinSize, UINT16 Nuti,CMID_SIM_ID nSimID);
UINT32 CMID_SimReadRecord(UINT8 nFileID,UINT8 nRecordNum,UINT16 nUTI,CMID_SIM_ID nSimID);
UINT32 CMID_SimGetPUCT(UINT16 nUTI, CMID_SIM_ID nSimID);
UINT32 CMID_SimSetPUCT(CMID_SIM_PUCT_INFO  *pPUCT,UINT8 *pPin2,UINT8 nPinSize,UINT16 nUTI,CMID_SIM_ID nSimID);
#else
UINT32 cmid_SimReadElementaryFile(UINT16 nUTI,UINT32 fileId);
UINT32 CMID_SimGetACMMax(UINT16 nUTI);
UINT32 CMID_SimSetACMMax(UINT32 iACMMaxValue, UINT8 *pPin2, UINT8 nPinSize, UINT16 Nuti);
UINT32 CMID_SimGetACM(UINT16 nUTI);
UINT32 CMID_SimSetACM(UINT32 iCurValue, UINT8 *pPin2, UINT8 nPinSize, UINT16 Nuti);
UINT32 CMID_SimReadRecord(UINT8 nFileID,UINT8 nRecordNum,UINT16 nUTI);
UINT32 CMID_SimGetPUCT(UINT16 nUTI);
UINT32 CMID_SimSetPUCT(CMID_SIM_PUCT_INFO  *pPUCT,UINT8 *pPin2,UINT8 nPinSize,UINT16 nUTI);

#endif //CMID_MULTI_SIM

VOID CSW_FacilityCrssDeflec(
		UINT8 nIndex,
#ifdef CMID_MULTI_SIM
		CMID_SIM_ID  nSimId,
#endif
        UINT8 DTN[20],   //Deflec number
        UINT8 DTNLen,
        UINT8 DTSA[21],  //sub number
        UINT8 DTSALen
		);

#ifdef __cplusplus
}
#endif

#endif //_CMID_2_H_

