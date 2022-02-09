/*--------------------------------------------------------------------------------------------------------------------
(C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
-------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
*  INTEL CONFIDENTIAL
*  Copyright 2006 Intel Corporation All Rights Reserved.
*  The source code contained or described herein and all documents related to the source code (“Material? are owned
*  by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
*  its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
*  Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
*  treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
*  transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.
*
*  No license under any patent, copyright, trade secret or other intellectual property right is granted to or
*  conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
*  estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
*  Intel in writing.
*  -------------------------------------------------------------------------------------------------------------------
*
*  Filename: teldef.h
*
*  Authors:  Vincent Yeung
*
*  Description: Macro used for telephony controller
*
*  History:
*   May 19, 2006 - Creation of file
*
*  Notes:
*
******************************************************************************/

#ifndef TELDEF_H
#define TELDEF_H

#include "common.h"
#include "stdio.h"
//#include <utlAtParser.h>
//#include "utlTrace.h"
//#ifdef LWIP_IPNETBUF_SUPPORT
//#include "tftdef.h"
//#include "pdpdef.h"
//#endif

#define RESULT_CODE_NULL utlFAILED
#define ResultCode_t utlReturnCode_T

//#define AT_CMD_UNUSED_ENABLE


/******************************************************************************
*   AT Parser Number
******************************************************************************/

typedef enum
{
   TEL_AT_CMD_ATP_0,       //SIM1  indication
   TEL_AT_CMD_ATP_1,      // AT modem fpr SIM1
   TEL_AT_CMD_ATP_2,	//CC for SIM1
   TEL_AT_CMD_ATP_3,    //MM for SIM1
   TEL_AT_CMD_ATP_4,    //MSG for SIM1
   TEL_AT_CMD_ATP_5,    //SIM/PB  for SIM1
   TEL_AT_CMD_ATP_6,    //DEV for SIM1
   TEL_AT_CMD_ATP_7,	//PS for SIM1
   TEL_AT_CMD_ATP_8,	//SS for SIM1
   TEL_AT_CMD_ATP_9,
   TEL_AT_CMD_ATP_10,
   TEL_AT_CMD_ATP_11,
   TEL_AT_CMD_ATP_12,
   TEL_AT_CMD_ATP_13,
   TEL_AT_CMD_ATP_14,
   TEL_AT_CMD_ATP_15,
   TEL_AT_CMD_ATP_16,
   TEL_AT_CMD_ATP_17,
   TEL_AT_CMD_ATP_18,
   TEL_AT_CMD_ATP_19,
   TEL_AT_CMD_ATP_20,
   TEL_AT_CMD_ATP_21,
   TEL_AT_CMD_ATP_22,
   TEL_AT_CMD_ATP_23,
   TEL_AT_CMD_ATP_24,
   TEL_AT_CMD_ATP_25,

   TEL_AT_CMD_ATP_26,
   TEL_MODEM_AT_CMD_ATP,
   TEL_AT_CMD_ATP_28,
   TEL_AT_CMD_ATP_29,
   TEL_AT_CMD_ATP_30,
   TEL_AT_CMD_ATP_31,
   TEL_AT_CMD_ATP_32,
   TEL_AT_CMD_ATP_33,
   TEL_AT_CMD_ATP_34,
   TEL_AT_CMD_ATP_35,

   TEL_AT_CMD_ATP_36,		// SIM2  indication
   TEL_AT_CMD_ATP_37,		// AT modem fpr SIM2
   TEL_AT_CMD_ATP_38,		//CC for SIM2
   TEL_AT_CMD_ATP_39,		//MM for SIM2
   TEL_AT_CMD_ATP_40,		//MSG for SIM2
   TEL_AT_CMD_ATP_41,		//SIM/PB  for SIM2
   TEL_AT_CMD_ATP_42,		//DEV for SIM2
   TEL_AT_CMD_ATP_43,		//PS for SIM2
   TEL_AT_CMD_ATP_44,		//SS for SIM2
   TEL_AT_CMD_ATP_45,
   TEL_AT_CMD_ATP_46,
   TEL_AT_CMD_ATP_47,
   TEL_AT_CMD_ATP_48,
   TEL_AT_CMD_ATP_49,
   TEL_AT_CMD_ATP_50,
   TEL_AT_CMD_ATP_51,
   TEL_AT_CMD_ATP_52,
   TEL_AT_CMD_ATP_53,
   TEL_AT_CMD_ATP_54,
   TEL_AT_CMD_ATP_55,
   TEL_AT_CMD_ATP_56,
   TEL_AT_CMD_ATP_57,
   TEL_AT_CMD_ATP_58,
   TEL_AT_CMD_ATP_59,
   TEL_AT_CMD_ATP_60,
   TEL_AT_CMD_ATP_61,
   TEL_AT_CMD_ATP_62,
   TEL_MODEM_AT_CMD_ATP_1,

   TEL_AT_CMD_ATP_64,
   TEL_AT_CMD_ATP_65,
   TEL_AT_CMD_ATP_66,
   TEL_AT_CMD_ATP_67,
   TEL_AT_CMD_ATP_68,
   TEL_AT_CMD_ATP_69,
   TEL_AT_CMD_ATP_70,
   TEL_AT_CMD_ATP_71,
   NUM_OF_TEL_ATP
} TelAtParserID;

#define MAT_IND_CHANNEL TEL_AT_CMD_ATP_0

#define MAT_IND_CHANNEL_1 TEL_AT_CMD_ATP_36

#define AT_MODEM_CHANNLE TEL_AT_CMD_ATP_1

#define AT_MODEM_CHANNLE_1 TEL_AT_CMD_ATP_37
#define TEL_AT_CMD_ATP_MODEM AT_MODEM_CHANNLE
#define TEL_AT_CMD_ATP_MODEM_1 AT_MODEM_CHANNLE_1

#define AT_UART_CHANNEL TEL_AT_CMD_ATP_35

#define AT_MODEM_SPI_CHANNLE_1 TEL_AT_CMD_ATP_2


#define NUM_OF_IMS_ATP 20
/*ATP definition for IMS type, start*/
#define DIALER_ATP_INDEX TEL_AT_CMD_ATP_9
#define DIALER_ATP_INDEX_1 (DIALER_ATP_INDEX + TEL_AT_CMD_ATP_36)

#define EVENT_HANDLER_ATP_INDEX (DIALER_ATP_INDEX + 1)
#define EVENT_HANDLER_ATP_INDEX_1 (EVENT_HANDLER_ATP_INDEX + TEL_AT_CMD_ATP_36)

/* TEL_AT_CMD_ATP_11 for volte*/
#define TEL_AT_CMD_ATP_ATNET (TEL_AT_CMD_ATP_12)
#define TEL_AT_CMD_ATP_ATNET_1 (TEL_AT_CMD_ATP_ATNET + TEL_AT_CMD_ATP_36)

#define TEL_AT_CMD_ATP_UART  (TEL_AT_CMD_ATP_ATNET + 1) // 13
#define TEL_AT_CMD_ATP_UART_1  (TEL_AT_CMD_ATP_UART + TEL_AT_CMD_ATP_36)

#define TEL_AT_CMD_ATP_CMUX_0  (TEL_AT_CMD_ATP_UART + 1)
#define TEL_AT_CMD_ATP_CMUX_1  (TEL_AT_CMD_ATP_UART + 2)
#define TEL_AT_CMD_ATP_CMUX_2  (TEL_AT_CMD_ATP_UART + 3)
#define TEL_AT_CMD_ATP_CMUX_3  (TEL_AT_CMD_ATP_UART + 4)
#define TEL_AT_CMD_ATP_CMUX_4  (TEL_AT_CMD_ATP_UART + 5)
#define TEL_AT_CMD_ATP_CMUX_5  (TEL_AT_CMD_ATP_UART + 6) // 19
#define TEL_AT_CMD_ATP_CMUX_0_1  (TEL_AT_CMD_ATP_UART_1 + 1)
#define TEL_AT_CMD_ATP_CMUX_1_1  (TEL_AT_CMD_ATP_UART_1 + 2)
#define TEL_AT_CMD_ATP_CMUX_2_1  (TEL_AT_CMD_ATP_UART_1 + 3)
#define TEL_AT_CMD_ATP_CMUX_3_1  (TEL_AT_CMD_ATP_UART_1 + 4)
#define TEL_AT_CMD_ATP_CMUX_4_1  (TEL_AT_CMD_ATP_UART_1 + 5)
#define TEL_AT_CMD_ATP_CMUX_5_1  (TEL_AT_CMD_ATP_UART_1 + 6)

#define TEL_BIP_AT_CMD_ATP  (TEL_AT_CMD_ATP_CMUX_5 + 1)
#define AT_BT_CHANNLE     (TEL_BIP_AT_CMD_ATP + 1) // 21

/* define a new AT channel, should modify the define */
#define TEL_AT_CMD_IMS_TYPE_ATP_LAST (AT_BT_CHANNLE)
#define TEL_AT_CMD_IMS_TYPE_ATP_LAST_1 (TEL_AT_CMD_IMS_TYPE_ATP_LAST + TEL_AT_CMD_ATP_36)
/*ATP definition for IMS type, end*/

#define EXT_AT_MODEM_CHANNLE    (TEL_AT_CMD_IMS_TYPE_ATP_LAST + 1)
#define TEL_EXT_AT_CMD_ATP_MODEM EXT_AT_MODEM_CHANNLE

#define AT_FOAT_CHANNEL (TEL_AT_CMD_ATP_24)
#define AT_FOAT_CHANNEL_1 (AT_FOAT_CHANNEL + TEL_AT_CMD_ATP_36)

#define AT_FLUSH_CHANNEL (TEL_AT_CMD_ATP_25)
#define AT_FLUSH_CHANNEL_1 (AT_FLUSH_CHANNEL + TEL_AT_CMD_ATP_36)

#define AT_ACAT_CHANNEL (TEL_AT_CMD_ATP_26)
#define AT_ACAT_CHANNEL_1 (AT_ACAT_CHANNEL + TEL_AT_CMD_ATP_36)

#define NONE_IMS_FLAG  0
#define IMS_FLAG  1

#define REQUEST_OPTION_FLAG  0
#define CURRENT_OPTION_FLAG  1

//ICAT EXPORTED ENUM
enum
{
	SIM_0,
	SIM_1,
	NUM_OF_SIM
};

//ICAT EXPORTED ENUM
typedef enum
{
	CHANNEL_MMI,
	CHANNEL_IMS,
	NUM_OF_CHANNEL_TYPE
}AT_CHANNEL_TYPE;


typedef struct
{
	UINT8 firstChnlId;
	UINT8 lastChnlId;
	UINT8 channlNum;
}TelAtChannelInfo;

enum TelAtParserCmd
{
	TEL_ATP_CMD_EXIT_CHAN,
	TEL_ATP_CMD_TRIGGER_PARSER,
#ifdef AT_PRODUCTION_CMNDS
	TEL_ATP_CMD_EXIT_CHAN_AND_START_DIAG,
#endif
	TEL_ATP_CMD_NUM
};


typedef enum _AtpSimType
{
	SIM1_ONLY,
	SIM2_ONLY,
	DUAL_SIM,
	MASTER_SIM,
	INVALID_ATP_SIM_TYPE,
}AtpSimType;

typedef enum _SIMID
{
	SIM1,
	SIM2,
	SIMID_NUM
}SIMID;

typedef enum
{
	TEL_MSG_DATA_NONE,
	TEL_MSG_DATA_CMGS,
	TEL_MSG_DATA_CMGC,
	TEL_MSG_DATA_CNMA,
	TEL_MSG_DATA_CMGW,
	NUM_OF_TEL_MSG_DATA
}TelMsgDataMode;

typedef struct _TelAtpDesc
{
	TelAtParserID index;
	char *path;

	AtpSimType targetSim;

	/*Need receive indication from CP side*/
	BOOL needIndication;
	/*Need enable this channel when is a AP only product*/
	BOOL apOnly;
} TelAtpDesc;

//ICAT EXPORTED STRUCT
typedef struct _TelAtpCtrl
{
	void *taskRef;
	UINT32	 iFd;
	char *path;
	UINT32 index;
	TelMsgDataMode smsDataEntryMode;
	BOOL bEnable;
	BOOL bIsSinkMode;
#ifdef LWIP_IPNETBUF_SUPPORT

	TelAtpPdpCtx pdpCtx;
	TelAtpTftCtx tftCtx;
	
#endif
} TelAtpCtrl;

typedef struct
{
	UINT16 Rc;
	char*  RcStr;
} CircCodeMap;

typedef enum {
   MODEM_DATA_MODE,
   MODEM_CONTROL_MODE
} MODEM_STATE;

typedef struct _ATStubMsg
{
	UINT8  sATPInd;
	UINT32 length;
	char *data;
} ATParserMsg, *PATParserMsg;


#if 0
extern TelAtpCtrl gAtpCtrl[];

extern utlAtParser_P aParser_p[ NUM_OF_TEL_ATP ];
extern const char* const gAtpName[ NUM_OF_TEL_ATP ];
extern unsigned short gusXid;
extern BOOL GLFeatureFlag;

extern char revisionId[128];

#define ATM_MANUFACTURER_ID "ASR"
#define ATM_MODEL_ID "LINUX"
#define ATM_REVISION_ID "1.0"
#define ATM_SERIAL_NUMBER "12333"

#define ATM_ID "Marvell"
#define ATM_OBJ_ID      "OBJ ID"
#define ATM_COUNTRY_CODE        "(20,3C,3D,42,50,58,00,61,FE,A9,B4,B5)"


/******************************************************************************
*   OSA Task Priority
******************************************************************************/
#define ESP_TASK_PRIORITY 128
#define MODEM_TASK_PRIORITY 128
//#define AT_TASK_PRIORITY 128
#define AT_TASK_PRIORITY    70
#define OSA_STACK_SIZE		6144	//HYJFixMe:	2009-05-04		in order to fix the stack leakage the old value is 2048
#define AT_STACK_SIZE		(16*1024)//(8192*4)	//HYJFixMe:	2009-05-04		in order to fix the stack leakage the old value is 2048


#define AT_MESSAGE_Q_MAX	256
#define AT_MESSAGE_Q_SIZE	16

/******************************************************************************
*   Response (RESP) related Macro / inline functions
******************************************************************************/
extern int atRespStr(UINT32 reqHandle, UINT8 resultCode, UINT16 errCode, char *respString);

#define IND_REQ_HANDLE 1
#define IND_REQ_HANDLE_1 2
#define INVALID_REQ_HANDLE 0xFFFFFFFF

#define SIMULATE_SIM1_REQ_HANDLE 0x0    //0x0 << 30
#define SIMULATE_SIM2_REQ_HANDLE 0x40000000    //0x1 << 30

#define ATRESP(REQHANDLE, RESULTCODE, ERRCODE, RESPSTRING) atRespStr( REQHANDLE, RESULTCODE, ERRCODE, RESPSTRING )

/******************************************************************************
*   AT Parser related Macro / inline functions
******************************************************************************/


/*--------- Making Compatible to Intel AT Parser--------____-----------------*/
#define RETURNCODE_T utlReturnCode_T

#define TEL_INVALID_EXT_CMD 	utlAT_PARAMETER_OP_UNKNOWN
#define TEL_EXT_EXEC_CMD 		utlAT_PARAMETER_OP_EXEC 	//AT+CGMI
#define TEL_EXT_GET_CMD 		utlAT_PARAMETER_OP_GET	   	//?
#define TEL_EXT_SET_CMD 		utlAT_PARAMETER_OP_SET	   	//=
#define TEL_EXT_ACTION_CMD 		utlAT_PARAMETER_OP_ACTION  	//AT+CPAS
#define TEL_EXT_TEST_CMD 		utlAT_PARAMETER_OP_SYNTAX	//=?


#define INITIAL_RETURN_CODE	utlFAILED
#define AT_RESV_HANDLE(sAtpIndex, xid) \
	(UINT32)(((sAtpIndex << 16) & 0x000F0000) \
		|((sAtpIndex & 0x10) << 26) \
		|(xid))


// CiRequestHandle == UINT32
#define MAKE_CI_REQ_HANDLE( atHandle, reqId )				    \
	(UINT32)(														\
		(												      \
			( (UINT32)reqId << 20)   & 0x3FF00000		       \
		)												      \
		|												      \
		(												      \
			atHandle  & 0x400FFFFF					     \
		)												      \
		)

/* use highest bit to mark whether this request is origined from +CRSM cmd */
#define MAKE_CRSM_CI_REQ_HANDLE( atHandle, reqId )				\
	(UINT32)(														\
		(												      \
			( (UINT32)reqId << 20)   & 0x3FF00000		       \
		)												      \
		|  0x80000000 |										      \
		(												      \
			atHandle  & 0x400FFFFF					     \
		)												      \
		)
#if defined NEZHA3_1826
#define MAKE_AT_HANDLE( sAtpIndex )								\
	(UINT32)(														\
		(												      \
			( (UINT32)sAtpIndex << 13)   & 0x000FE000	       \
		)												      \
		|														\
		(														\
			( (UINT32)sAtpIndex>35? 0x40000000:0x0)			\
		)														\
		|														\
		(													      \
			( (gusXid == 0x1fff)? gusXid=11 : ++gusXid)							\
		)													      \
		)


#define GET_ATP_INDEX( atHandle ) ( (atHandle >> 13 ) & 0x7F )
#else
#define MAKE_AT_HANDLE( sAtpIndex )								\
	(UINT32)(														\
		(												      \
			( (UINT32)sAtpIndex << 16)   & 0x000F0000	       \
		)												      \
		|													      \
		(													      \
			( (UINT32)sAtpIndex & 0x10) << 26 	       \
		)												      \
		|													      \
		(													      \
			( (gusXid == 0xffff)? gusXid=11 : ++gusXid)							\
		)													      \
		)

#define GET_ATP_INDEX( atHandle ) ( ((atHandle >> 16 ) & 0xF) | ((atHandle >> 26) & 0x10) )
#endif
#define GET_AT_HANDLE( reqHandle ) ( reqHandle & 0x400FFFFF )
#define GET_REQ_ID( reqHandle ) ( (reqHandle >> 20 ) & 0x3FF )
#define GET_CRSM_FLAG( reqHandle ) ( (reqHandle >> 31) & 0x1 )
#define GET_SIM1_FLAG( reqHandle ) ( (reqHandle >> 30) & 0x1 )

extern RETURNCODE_T HANDLE_RETURN_VALUE(UINT32 ret);

/******************************************************************************
*   Audio Related Macro
******************************************************************************/


extern void enable_voicecall_audio(void);
extern void disable_voicecall_audio(void);
extern int tel_set_voicecall_mute(INT32 mute);
extern INT32 tel_get_voicecall_mute(void);
extern void tel_init_voicecall_mute(void);

extern INT32 tel_set_loopback_state(INT32 method, INT32 device);
extern INT32 tel_get_loopback_state(INT32 * method, INT32 * device);
#define MACRO_START_VOICE_CALL() //enable_voicecall_audio()

#define MACRO_END_VOICE_CALL() //disable_voicecall_audio()

void initCCContext(UINT32 atHandle);
void initPSContext(UINT32 atHandle);

/* Some global variables */
extern BOOL bLocalTest;


extern const utlAtCommand_T * const pshell_commands;
extern const unsigned int shell_commands_num;
extern ATCMD_TimeOut atcmd_timeout_table[];
extern const utlAtCommand_T * const pshell_commands_mmi;
extern const unsigned int shell_commands_num_mmi;
extern ATCMD_TimeOut atcmd_timeout_table_mmi[];

#endif

#endif


/* END OF FILE */
