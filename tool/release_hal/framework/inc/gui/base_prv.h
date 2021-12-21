/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/

#if !defined(__BASE_PRV_H__)
#define __BASE_PRV_H__

#include <event.h>

#include "ui_os_api.h"
#include "ui_mem_api.h"

VOID UI_DevMonitorTask (
  VOID* pData
);

#define CSW_TASK_ID_SYS            0
#define CSW_TASK_ID_APS            1
#define CSW_TASK_ID_DEV_MONITOR    2  
#define CSW_TASK_ID_MMI_DEFAULT    3   
#define CSW_TASK_ID_BG             4   

//BOOL DM_StartAudio ();
//BOOL DM_StopAudio ();

BOOL dm_SendToDefaultMMITask(UI_EVENT* pev);


//
// Internal Timer ID: 0xF1 --- 0xFF 
//
#define BAL_BATTERY_TIMER1          0xF1
#define BAL_BATTERY_TIMER2          0xF2
//#define PRV_AUDIO_TIMER_ID        0xF3
#define EV_AUDIO_TIMER_ID           0xF3
#define PRV_KEYPAD_TIMER_ID         0xF4

#define PRV_CMID_NW_TIMER_ID     	0xF8
#define MMI_LPCLKCALIB_TIM 			0xF9
#define EV_BUZZER_TIMER_ID		    0xFA	// Longman 06.09.11 edit 10.26
#define TM_POWERON_ALM_TIMER_ID     0xFB
//
//Add for define the range of timer.
//
//
//CSW BAL Timer 
//
#define PRV_BAL_BATTERY_TIMER1        ( UOS_CMID_TIMER_ID_BASE + 1 )
#define PRV_BAL_BATTERY_TIMER2        ( UOS_CMID_TIMER_ID_BASE + 2 )

//
//CSW DM Timer 
//
#define PRV_DM_AUDIO_TIMER_ID         ( UOS_CMID_TIMER_ID_BASE + 3 )
#define PRV_DM_KEYPAD_TIMER_ID        ( UOS_CMID_TIMER_ID_BASE + 4 )
#define PRV_DM_LPCLKCALIB_TIMER_ID    ( UOS_CMID_TIMER_ID_BASE + 5 )
#define PRV_DM_BUZZER_TIMER_ID        ( UOS_CMID_TIMER_ID_BASE + 6 )

//
//CSW CMID Timer 
//
#define PRV_CMID_IP_TIMER_ID           ( UOS_CMID_TIMER_ID_BASE + 7 )
#define PRV_CMID_SIM_TIMER_ID          ( UOS_CMID_TIMER_ID_BASE + 8 )
#define PRV_CMID_TCP_TIMER_ID          ( UOS_CMID_TIMER_ID_BASE + 9 )
#define PRV_CMID_DNS_TIMER_ID          ( UOS_CMID_TIMER_ID_BASE + 10 )
#ifdef CMID_MULTI_SIM
#define PRV_CMID_NW_SIM0_TIMER_ID      ( UOS_CMID_TIMER_ID_BASE + 11 )
#define PRV_CMID_NW_SIM1_TIMER_ID      ( UOS_CMID_TIMER_ID_BASE + 12 )
#define PRV_CMID_NW_SIM2_TIMER_ID      ( UOS_CMID_TIMER_ID_BASE + 13 )
#define PRV_CMID_NW_SIM3_TIMER_ID      ( UOS_CMID_TIMER_ID_BASE + 14 )
#define PRV_CMID_NW_CS_DELAY_SIM0_TIMER_ID      ( UOS_CMID_TIMER_ID_BASE + 15)
#define PRV_CMID_NW_CS_DELAY_SIM1_TIMER_ID      ( UOS_CMID_TIMER_ID_BASE + 16 )
#define PRV_CMID_NW_CS_DELAY_SIM2_TIMER_ID      ( UOS_CMID_TIMER_ID_BASE + 17)
#define PRV_CMID_NW_CS_DELAY_SIM3_TIMER_ID      ( UOS_CMID_TIMER_ID_BASE + 18 )
#ifdef CMID_GPRS_SUPPORT

#define CMID_START_GPRSDETACH_TIMER_ID     	  ( UOS_CMID_TIMER_ID_BASE + 19)   // add by wuys 2010-05-17
#define CMID_START_GPRSDETACH_TIMER_ID_SIM1     	  ( UOS_CMID_TIMER_ID_BASE + 20)   // add by wuys 2010-05-17
#define CMID_START_GPRSDETACH_TIMER_ID_SIM2     	  ( UOS_CMID_TIMER_ID_BASE + 21)   // add by wuys 2010-05-17
#define CMID_START_GPRSDETACH_TIMER_ID_SIM3     	  ( UOS_CMID_TIMER_ID_BASE + 22)   // add by wuys 2010-05-17
#endif
#else
#define PRV_CMID_NW_TIMER_ID           ( UOS_CMID_TIMER_ID_BASE + 11 )
#define PRV_CMID_NW_CS_DELAY_TIMER_ID      ( UOS_CMID_TIMER_ID_BASE + 12)
#ifdef CMID_GPRS_SUPPORT

#define CMID_START_GPRSDETACH_TIMER_ID     	  ( UOS_CMID_TIMER_ID_BASE + 13)   // add by wuys 2010-05-17
#endif
#endif


#define PRV_CMID_DISPLAY_WAITING_ANIMATION_TIMER_ID   ( UOS_CMID_TIMER_ID_BASE + 26 )
//
//CSW Trace 
//
#define PRV_CMID_TRACE_TO_FLASH_TIMER_ID    ( UOS_CMID_TIMER_ID_BASE + 25)

//
//CSW TM Timer 
//
#define PRV_TM_POWERON_ALM_TIMER_ID   ( UOS_CMID_TIMER_ID_BASE + 30 )


#define SDIO_DHCP_COARSE_TIMER_ID     	  ( UOS_CMID_TIMER_ID_BASE + 31)   // add by wuys 2010-05-17
#define SDIO_DHCP_FINE_TIMER_ID     	  ( UOS_CMID_TIMER_ID_BASE + 32)   // add by wuys 2010-05-17

#ifdef WIFI_SUPPORT

#define SDIO_POLL_TIMER_ID     	  ( UOS_CMID_TIMER_ID_BASE + 33)   // add by wuys 2010-05-17
#define WIFI_CONNECT_AP_TIMER_ID     	  ( UOS_CMID_TIMER_ID_BASE + 34)   // add by wuys 2010-05-17
#define WIFI_ASSOC_TIMER_ID     	  ( UOS_CMID_TIMER_ID_BASE + 35)   // add by wuys 2010-05-17
#define WIFI_PREASSO_SLEEP_WAIT_TIMER_ID ( UOS_CMID_TIMER_ID_BASE + 36) 

#define WIFI_TESTMODE_UART_RX_TIMER_ID ( UOS_CMID_TIMER_ID_BASE + 37) 
#define WIFI_SCAN_WAIT_TIMER ( UOS_CMID_TIMER_ID_BASE + 38) 
#define WIFI_GET_NETWORK_INFO_TIMER_ID     	  ( UOS_CMID_TIMER_ID_BASE + 39)   // add by wuys 2010-05-17

#endif


//
//CSW Max timer
//
#define COS_CSW_TIMER_ID_XXX_END      ( UOS_CMID_TIMER_ID_BASE + 50 )

#if (COS_CSW_TIMER_ID_XXX_END >= (UOS_CMID_TIMER_ID_END_))
#error 'Error: invalid COS_CSW_TIMER_ID_XXX_END definition!'
#endif



#define MMI_LPCLKCALIB_TIMER (3600 * 16384)



//
// Internal EV ID: 0xF0000001 --  0xFFFFF000.
//
#define EV_CSW_PRV_KEY              (EV_CSW_PRV_BASE+1)
#define EV_CSW_PRV_ALARM            (EV_CSW_PRV_BASE+2)
#define EV_CSW_PRV_GPIO             (EV_CSW_PRV_BASE+3)
#define EV_CSW_PRV_TONE             (EV_CSW_PRV_BASE+4)
#define EV_INTER_APS_TCPIP_REQ      (EV_CSW_PRV_BASE+5)
#define EV_CSW_PRV_RECORD_END   (EV_CSW_PRV_BASE+6)
#define EV_CSW_PRV_STREAM_END    (EV_CSW_PRV_BASE+7)
/*zj add spi*/
#define EV_CSW_PRV_SPI			 (EV_CSW_PRV_BASE+8 ) 
#define SYS_EV_MASK_BUZZER_FILE     (EV_CSW_PRV_BASE+9) //checked 11.23 Longman
#define SYS_EV_MASK_AUDIO_TONE      (EV_CSW_PRV_BASE+10) //checked 11.23
#define EV_INTER_APS_ACT_REQ        (EV_CSW_PRV_BASE+11)  //mabo added - 20071106
#define EV_INTER_APS_SMTP_REQ       (EV_CSW_PRV_BASE+12)  //add by wuys 2007-08-20
#define EV_INTER_APS_POP3_REQ       (EV_CSW_PRV_BASE+13) 
#define EV_CSW_PRV_LPS_WAKEUP       (EV_CSW_PRV_BASE+14)
#define EV_APS_PPP_DATA_REQ         (EV_CSW_PRV_BASE+15)  //add by wuys 2004-04-15
#define EV_CSW_PRV_XXX_END          (EV_CSW_PRV_BASE+16)

#if (EV_CSW_PRV_XXX_END >= (EV_CSW_PRV_END_))
#error 'Error: invalid EV_CSW_PRV_XXX_END definition!'
#endif

//
// Internal Malloc or Free by CSW only.
//
PVOID UOS_Malloc  (
  unsigned int nSize
);

PVOID CSW_StkMalloc  (
  unsigned int nSize
);

BOOL UOS_Free ( 
  PVOID pMemBlock
);
unsigned int CMID_SendSclData(
    unsigned char nNsapi,
    unsigned char nQueueId,
    BOOL bMode, 
    unsigned short nDataSize, 
    unsigned char *pData
);

unsigned int CMID_BalSendMessage(
    unsigned int nMailBoxId, 
    unsigned int nEventId, 
    VOID* pData,
    unsigned int nSize
);

#ifdef _DEBUG
PVOID  CSW_MallocDbg(unsigned int nModuleId, unsigned int nSize, CONST unsigned char* pszFileName, unsigned int nLine);
BOOL  CSW_FreeDbg(unsigned int nModuleId,VOID* pMemAddr, CONST unsigned char* pszFileName, unsigned int nLine);
#define CSW_MALLOC(m,size)  CSW_MallocDbg(m, (unsigned int)(size), (UINT8 *)__FILE__, __LINE__)
#define CSW_FREE(m,p)       CSW_FreeDbg(m, (PVOID)(p), __FILE__, __LINE__)
#else
#define CSW_MALLOC(m,size)  CSW_Malloc((unsigned int)(size))
#define CSW_FREE(m,p)       CSW_Free((PVOID)(p))

#endif

#define DRV_GPIO(n)			    (1<<(n))
#define DRV_CHRG_CMD     		DRV_GPIO(5)
#define DRV_DC_ON_DETECT	    DRV_GPIO(13)
#define DRV_ON_OFF_KEY	        DRV_GPIO(9)


BOOL dm_SendPMMessage( UI_EVENT* pEv);
BOOL dm_SendGpadcMessage( UI_EVENT* pEv);
BOOL dm_SendArlarmMessage( UI_EVENT* pEv);
BOOL dm_SendPowerOnMessage( UI_EVENT* pEv);
BOOL dm_SendKeyMessage( UI_EVENT* pEv);
BOOL dm_SendISRKeyMessage( UI_EVENT* pEv);

#endif // _H_ 

