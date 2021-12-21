/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/

#ifndef __EVENT_H__
#define __EVENT_H__

#include "plat_config_defs.h"
#include "cmid_multi_sim.h"

//
// System level Events
//
#define EV_SYS_BASE                   0
#define EV_SYS_END_                   50

#define EV_SHL_CMID_STATUS_IND         (EV_SYS_BASE+10)

//
// DM Events.
//
#define EV_DM_BASE                   (50)
#define EV_DM_END_                   (100)

//
// PM Evetns.
//
#define EV_PM_BASE                   (100)
#define EV_PM_END_                   (150)

//
// DBG Evetns.
//
#define EV_DBG_BASE                  (150)
#define EV_DBG_END_                  (200)

//
// CMID Evetns.
//
#define EV_CMID_START_                (500)

#define EV_CMID_NW_RSP_BASE           (500)
#define EV_CMID_NW_RSP_END_           (550)

#define EV_CMID_CC_RSP_BASE           (550)
#define EV_CMID_CC_RSP_END_           (600)

#define EV_CMID_SS_RSP_BASE           (600)
#define EV_CMID_SS_RSP_END_           (650)

#define EV_CMID_SIM_RSP_BASE          (700)
#define EV_CMID_SIM_RSP_END_          (760)

#define EV_CMID_SMS_RSP_BASE          (760)
#define EV_CMID_SMS_RSP_END_          (800)

#define EV_CMID_PBK_RSP_BASE          (800)
#define EV_CMID_PBK_RSP_END_          (850)

#define EV_CMID_GPRS_RSP_BASE         (850)
#define EV_CMID_GPRS_RSP_END_         (900)

#define EV_CMID_FAX_RSP_BASE          (900)
#define EV_CMID_FAX_RSP_END_          (950)

#define EV_CMID_TCPIPUDP_RSP_BASE     (1000)
#define EV_CMID_TCPIPUDP_RSP_END_     (1050)

#define EV_CMID_HTTP_RSP_BASE         (1050)
#define EV_CMID_HTTP_RSP_END_         (1100)

#define EV_CMID_SHELL_RSP_BASE        (1100)
#define EV_CMID_SHELL_RSP_END_        (1150)

#define EV_CMID_TM_RSP_BASE           (1150) // Test Module
#define EV_CMID_TM_RSP_END_           (1200)

#define EV_RMC_BASE                  (1200) // Remote Control Module
#define EV_RMC_END_                  (1300)
#ifdef WIFI_SUPPORT
#define EV_CMID_WIFI_RSP_BASE           (1300) // Test Module
#define EV_CMID_WIFI_RSP_END_           (1350)
#define EV_WIFI_RSP_BASE           (1300) 
#define EV_WIFI_RSP_END_           (1350)
#define EV_WIFI_IND_BASE           (1351) 
#define EV_WIFI_IND_END_           (1400)

#endif

#ifdef BT_TETHER_SUPPORT
#define EV_BT_TETHER_BASE           (1400) 
#define EV_BT_TETHER_END_           (1450)

#endif


//#if defined(__SPI_FLSH_WITH_SUSPEND__)	
//#define EV_FMT_VDS_SAP_BASE           (1451) 
//#define EV_FMT_VDS_SAP_END_           (1460)
//#endif
// ... 
// 1200 -- 5000 reserved.
//

#define EV_CMID_IND_BASE              (5000)
#define EV_CMID_IND_END_              (5050)

#define EV_CMID_END_                  (6000)

//
// AFW event.
//
#define AFW_EV_BASE                  (7000)
#define AFW_EV_END_                  (8000)

//
// UI FW event
//
#define UI_FW_EV_BASE               (9000)
#define UI_FW_EV_END_               (10000)

#define CSW_SRV_EV_END_             (0x000FFFFF)

//
// User Event: 0x00100000 -- 0xA0000000
//
#define EV_MMI_EV_BASE              0x00100000 
#define EV_MMI_EV_END_              0xA0000000 

// ... 
// 0xA0000000 -- 0xF0000000 reserved by system.
// 

//
// CSW Private Event 0xF0000001 --  0xF000F000
// used by CSW internal only
// 
#define EV_CSW_PRV_BASE             0xF0000001
#define EV_CSW_PRV_END_             0xF000F000

//
// CSW Private Event 0xF000F001 --  0xFFFFF000
// 
#define EV_CSW_REQ_BASE             0xF000F001
#define EV_CSW_REQ_END_             0xFFFFF000 // don't change this value!


// ... 
// 0xFFFFF000 -- 0xFFFFFFF reserved by system.
//

#define EV_EMU_BASE                 0xF0000000

//
// Helper Macros
//
#define IS_CMID_EVNET(evId)                       (((evId)>=EV_CMID_START_) && (evId)<EV_CMID_END_)

//
// DM & TIM Event.
// 
#define EV_DM_POWER_ON_IND                       (EV_DM_BASE+1)
#define EV_CSW_PRV_GPIO             			 (EV_CSW_PRV_BASE+3)
#define EV_DM_POWER_OFF_IND                      (EV_DM_BASE+5) 
#define EV_TIM_ALRAM_IND                         (EV_DM_BASE+6)
#define EV_DM_UART_RECEIVE_DATA_IND              (EV_DM_BASE+8)
#define EV_DM_EXTI_IND                           (EV_DM_BASE+9) 
#define EV_DM_GPIO_IND                           (EV_DM_BASE+10)
#define EV_DM_RECORD_END_IND                     (EV_DM_BASE+11) 
#define EV_DM_STREAM_END_IND                     (EV_DM_BASE+12)
///#define EV_DM_GPADC_IND                          (EV_DM_BASE+13)
#define EV_DM_IOCTL_IND							(EV_DM_BASE+13)
#define EV_TIM_SET_TIME_IND                      (EV_DM_BASE+14)
			
//guoh add speech indication, if start call, this indication will send to mmi task 			
#define EV_DM_SPEECH_IND                         (EV_DM_BASE+15)

#define EV_DM_AUDIO_OPER_MSG_START               (EV_DM_BASE+16)

#define EV_DM_AUDIO_CONTROL_IND                  (EV_DM_BASE+16)
#define EV_DM_SET_AUDIO_VOLUME_IND               (EV_DM_BASE+17)
#define EV_DM_SET_MIC_GAIN_IND                   (EV_DM_BASE+18)
#define EV_DM_SET_AUDIO_MODE_IND                 (EV_DM_BASE+19)

#define EV_DM_AUD_SETUP_IND                      (EV_DM_BASE+20)
#define EV_DM_AUD_STREAM_START_IND               (EV_DM_BASE+21)
#define EV_DM_AUD_STREAM_STOP_IND                (EV_DM_BASE+22)
#define EV_DM_AUD_STREAM_PAUSE_IND               (EV_DM_BASE+23)
#define EV_DM_AUD_STREAM_RECORD_IND              (EV_DM_BASE+24)
#define EV_DM_AUD_TEST_MODE_SETUP_IND            (EV_DM_BASE+25)
#define EV_DM_AUD_FORCE_RECV_MIC_SEL_IND         (EV_DM_BASE+26)
#define EV_DM_AUD_SET_CUR_OUTPUT_DEVICE_IND      (EV_DM_BASE+27)
#define EV_DM_AUD_MUTE_OUTPUT_DEVICE_IND         (EV_DM_BASE+28)
///#define EV_DM_AUD_LOUDSPK_WITH_EP_IND            (EV_DM_BASE+29)
#define EV_DM_AUD_PLAY_TONE                      (EV_DM_BASE+29)
#define EV_DM_VOIS_RECORD_START_IND         (EV_DM_BASE+30)
#define EV_DM_VOIS_RECORD_STOP_IND         (EV_DM_BASE+31)

#define EV_DM_ARS_SETUP_IND                      (EV_DM_BASE+33)
#define EV_DM_ARS_RECORD_IND                     (EV_DM_BASE+34)
#define EV_DM_ARS_PAUSE_IND                      (EV_DM_BASE+35)
#define EV_DM_ARS_STOP_IND                       (EV_DM_BASE+36)

#define EV_DM_AUDIO_OPER_MSG_END                 (EV_DM_BASE+39)

// don't change the following definition
#define EV_KEY_DOWN                              (EV_DM_BASE+40)  
#define EV_KEY_PRESS                             (EV_DM_BASE+41)
#define EV_KEY_UP                                (EV_DM_BASE+42)
#define EV_TIMER                                 (EV_DM_BASE+43)
#define EV_DOUBLE_KEY_DOWN                       (EV_DM_BASE+44)  
#define EV_DOUBLE_KEY_UP                         (EV_DM_BASE+45)
#define EV_DM_XXX_END                            (EV_DM_BASE+46)
#define EV_DM_UART_MUX_RECEIVE_DATA_IND          (EV_DM_BASE+47)

#if (EV_DM_XXX_END >= (EV_DM_END_))
#error 'Error: invalid EV_DM_XXX_END definition!'
#endif

//
// PM Event
//
#define EV_PM_BC_IND                             (EV_PM_BASE+1)
#define EV_PM_POWER_DOWN_ID                    (EV_PM_BASE+2)
#define EV_PM_XXX_IND_END                        (EV_PM_BASE+3)

#if (EV_PM_XXX_IND_END >= (EV_PM_END_))
#error 'Error: invalid EV_CMID_XXX_IND definition!'
#endif

//
// DBG Event
//
#define EV_DBG_IND                             	 (EV_DBG_BASE+1)
#define EV_DBG_TXCHK_IND                         (EV_DBG_BASE+2)
#define EV_DBG_SHELL_IND                         (EV_DBG_BASE+3)
#define EV_DBG_RXUSK_IND                         (EV_DBG_BASE+4)
#define EV_DBG_XXX_IND_END                       (EV_DBG_BASE+5)
#if (EV_DBG_XXX_IND_END >= (EV_DBG_END_))
#error 'Error: invalid EV_DBG_XXX_IND definition!'
#endif

//
// CMID IND Event.
//
#define EV_CMID_INIT_IND                          (EV_CMID_IND_BASE+1)
#define EV_CMID_SRV_STATUS_IND                    (EV_CMID_IND_BASE+2) 
#define EV_CMID_NW_SIGNAL_QUALITY_IND             (EV_CMID_IND_BASE+3)
#define EV_CMID_NW_REG_STATUS_IND                 (EV_CMID_IND_BASE+5)
#define EV_CMID_CC_SPEECH_CALL_IND                (EV_CMID_IND_BASE+6)
#define EV_CMID_CC_CRSSINFO_IND                   (EV_CMID_IND_BASE+7)
#define EV_CMID_NEW_SMS_IND                       (EV_CMID_IND_BASE+8)
#define EV_CMID_SMS_INFO_IND                      (EV_CMID_IND_BASE+9)
#define EV_CMID_CC_CALL_INFO_IND                  (EV_CMID_IND_BASE+10)
#define EV_CMID_SIM_STATUS_IND                    (EV_CMID_IND_BASE+11)
#define EV_CMID_SS_USSD_IND                       (EV_CMID_IND_BASE+12)
#define EV_CMID_CC_RELEASE_CALL_IND               (EV_CMID_IND_BASE+13)
#define EV_CMID_CC_ERROR_IND                      (EV_CMID_IND_BASE+14)
#define EV_CMID_SAT_CMDTYPE_IND                   (EV_CMID_IND_BASE+15)
#define EV_CMID_PBK_IND                           (EV_CMID_IND_BASE+16)
#define EV_CMID_NW_NETWORKINFO_IND                (EV_CMID_IND_BASE+17)
#define EV_CMID_GPRS_CXT_ACTIVE_IND               (EV_CMID_IND_BASE+18)
#define EV_CMID_GPRS_CXT_DEACTIVE_IND             (EV_CMID_IND_BASE+19)
#define EV_CMID_GPRS_MOD_IND                      (EV_CMID_IND_BASE+20) 
#define EV_CMID_GPRS_STATUS_IND                   (EV_CMID_IND_BASE+21)
#define EV_CMID_GPRS_DATA_IND                     (EV_CMID_IND_BASE+22)
#define EV_CMID_TCPIP_REV_DATA_IND                     (EV_CMID_IND_BASE+23)  // TCP/IP IND
#define EV_CMID_TCPIP_CLOSE_IND                     (EV_CMID_IND_BASE+24)  // TCP/IP close IND
#define EV_CMID_TCPIP_ERR_IND                     (EV_CMID_IND_BASE+25)  // TCP/IP ERROR IND
#define EV_CMID_TCPIP_ACCEPT_IND                     (EV_CMID_IND_BASE+26)  // TCP/IP accept IND
#define EV_CMID_DNS_RESOLV_SUC_IND                (EV_CMID_IND_BASE+27)
#define EV_CMID_DNS_RESOLV_ERR_IND                (EV_CMID_IND_BASE+28)
#define EV_CMID_PPP_DATA_IND                      (EV_CMID_IND_BASE+29)
#define EV_CMID_PPP_TERMINATE_IND                 (EV_CMID_IND_BASE+30)
//Frank add used MMS/PUSH
#define EV_CMID_NEW_EMS_IND                       (EV_CMID_IND_BASE+31)
#define EV_CMID_ATT_STATUS_IND                    (EV_CMID_IND_BASE+32)
#define EV_CMID_CC_STATUS_IND                    (EV_CMID_IND_BASE+33)
#define EV_CMID_CC_CALL_STATE_CHANGED_IND         (EV_CMID_IND_BASE+34)
#define EV_CMID_CB_MSG_IND                      (EV_CMID_IND_BASE+35)
#define EV_CMID_CB_PAGE_IND                      (EV_CMID_IND_BASE+36)
#ifdef WIFI_SUPPORT
//#define EV_CMID_WIFI_CONNECTED_IND                    (EV_CMID_IND_BASE+34)
//#define EV_CMID_WIFI_DATA_IND                    (EV_CMID_IND_BASE+35)
//#define EV_CMID_WIFI_TEST_MODE_DATA_IND (EV_CMID_IND_BASE+36)
#define EV_CMID_EXIT_IND                          (EV_CMID_IND_BASE+38)
#define EV_CMID_XX_IND_END                        (EV_CMID_IND_BASE+39)
#else
#define EV_CMID_EXIT_IND                          (EV_CMID_IND_BASE+37)
#define EV_CMID_XX_IND_END                        (EV_CMID_IND_BASE+38)

#endif
#define EV_CMID_REG_UPDATE_IND                          (EV_CMID_IND_BASE+40)

#ifdef __SUPPORT_SMSCLASS__
#define EV_CMID_NEW_SMS_PDU_IND                    (EV_CMID_IND_BASE+41)
#endif

#define EV_CMID_CC_CALL_LIST_IND                    (EV_CMID_IND_BASE+42)


#if (EV_CMID_XX_IND_END >= (EV_CMID_IND_END_)) 
#error 'Error: invalid EV_CMID_XXX_IND definition!'.
#endif

//
// Emulator Event 
//
#define EV_EMU_SCM_REQ                       (EV_EMU_BASE+1)
#define EV_EMU_SCL_RSP                       (EV_EMU_BASE+2)
#define EV_EMU_SCL_REQ                       (EV_EMU_BASE+3)
#define EV_EMU_SCL_TST                       (EV_EMU_BASE+4)
#define EV_EMU_TRACE                         (EV_EMU_BASE+5)
#define EV_EMU_SYSTEM                        (EV_EMU_BASE+6)
#define EV_EMU_EXIT                          (EV_EMU_BASE+7)
#define EV_EMU_SCL_AUDIO                     (EV_EMU_BASE+10)
#define EV_EMU_LCD                           (EV_EMU_BASE+11)
#define EV_EMU_INPUT                         (EV_EMU_BASE+12)



//
// NW Response Event.
//
#define EV_CMID_NW_SET_BAND_RSP               (EV_CMID_NW_RSP_BASE+1)
#define EV_CMID_NW_GET_IMEI_RSP               (EV_CMID_NW_RSP_BASE+2)
#define EV_CMID_NW_GET_SIGNAL_QUALITY_RSP     (EV_CMID_NW_RSP_BASE+3)
#define EV_CMID_NW_SET_REGISTRATION_RSP       (EV_CMID_NW_RSP_BASE+4)
#define EV_CMID_NW_DEREGISTER_RSP             (EV_CMID_NW_RSP_BASE+5)
#define EV_CMID_NW_GET_AVAIL_OPERATOR_RSP     (EV_CMID_NW_RSP_BASE+6)
#define EV_CMID_NW_ABORT_LIST_OPERATORS_RSP   (EV_CMID_NW_RSP_BASE+7)
#ifdef CMID_MULTI_SIM
#define EV_CMID_SET_COMM_RSP                                  (EV_CMID_NW_RSP_BASE + 8)
#define EV_CMID_SET_COMM_EXT_RSP                              (EV_CMID_NW_RSP_BASE + 9)
#else
#define EV_CMID_NW_SET_FM_RSP                                  (EV_CMID_NW_RSP_BASE + 8)
#endif


//
// GPRS Response Event.
//
#define EV_CMID_GPRS_CTX_MODIFY_ACC_RSP       (EV_CMID_GPRS_RSP_BASE+1)
#define EV_CMID_GPRS_ATT_RSP                  (EV_CMID_GPRS_RSP_BASE+2)
#define EV_CMID_GPRS_ACT_RSP                  (EV_CMID_GPRS_RSP_BASE+3)
#define EV_CMID_GPRS_CXT_ACTIVE_RSP           (EV_CMID_GPRS_RSP_BASE+4)
#define EV_CMID_GPRS_MOD_RSP                  (EV_CMID_GPRS_RSP_BASE+5)
#define EV_CMID_WIFI_CONNECTED_RSP      (EV_CMID_GPRS_RSP_BASE+6)

//
// CC Response Event
//
#define EV_CMID_CC_ACCEPT_SPEECH_CALL_RSP     (EV_CMID_CC_RSP_BASE+1)	//0x227
#define EV_CMID_CC_CALL_HOLD_MULTIPARTY_RSP   (EV_CMID_CC_RSP_BASE+2)	//0x228
#define EV_CMID_CC_INITIATE_SPEECH_CALL_RSP   (EV_CMID_CC_RSP_BASE+3)	//0x229
#define EV_CMID_CC_RELEASE_CALL_RSP           (EV_CMID_CC_RSP_BASE+4)	//0x22a
#define EV_CMID_CC_GET_CURRENT_CALL_RSP       (EV_CMID_CC_RSP_BASE+5)	//0x22b
#define EV_CMID_CC_ATH_RSP       			  (EV_CMID_CC_RSP_BASE+6)	//0x22c

//
// SS Response Event
//
#define EV_CMID_SS_QUERY_CLIP_RSP             (EV_CMID_SS_RSP_BASE+1)
#define EV_CMID_SS_SET_CALL_WAITING_RSP       (EV_CMID_SS_RSP_BASE+2)
#define EV_CMID_SS_QUERY_CALL_WAITING_RSP     (EV_CMID_SS_RSP_BASE+3)
#define EV_CMID_SS_SET_CALL_FORWARDING_RSP    (EV_CMID_SS_RSP_BASE+4)
#define EV_CMID_SS_QUERY_CALL_FORWARDING_RSP  (EV_CMID_SS_RSP_BASE+5)
#define EV_CMID_SS_SET_FACILITY_LOCK_RSP      (EV_CMID_SS_RSP_BASE+6)
#define EV_CMID_SS_GET_FACILITY_LOCK_RSP      (EV_CMID_SS_RSP_BASE+7)
#define EV_CMID_SS_CHANGE_PWD_RSP             (EV_CMID_SS_RSP_BASE+8)
#define EV_CMID_SS_QUERY_FACILITY_LOCK_RSP    (EV_CMID_SS_RSP_BASE+9)
#define EV_CMID_SS_QUERY_CLIR_RSP             (EV_CMID_SS_RSP_BASE+10)
#define EV_CMID_SS_QUERY_COLP_RSP             (EV_CMID_SS_RSP_BASE+11)
#define EV_CMID_SS_SEND_USSD_RSP              (EV_CMID_SS_RSP_BASE+12)
#define EV_CMID_SS_TERMINATE_RSP              (EV_CMID_SS_RSP_BASE+13)
#define EV_CMID_SS_QUERY_COLR_RSP             (EV_CMID_SS_RSP_BASE+14)
#define EV_CMID_SS_QUERY_CNAP_RSP             (EV_CMID_SS_RSP_BASE+15)

//
// SIM Response Event
//
#define EV_CMID_SIM_ADD_PBK_RSP                   (EV_CMID_SIM_RSP_BASE+1)
#define EV_CMID_SIM_DELETE_PBK_ENTRY_RSP          (EV_CMID_SIM_RSP_BASE+2)
#define EV_CMID_SIM_GET_PBK_ENTRY_RSP             (EV_CMID_SIM_RSP_BASE+3)
#define EV_CMID_SIM_LIST_PBK_ENTRY_RSP            (EV_CMID_SIM_RSP_BASE+4)
#define EV_CMID_SIM_GET_PBK_STRORAGE_RSP          (EV_CMID_SIM_RSP_BASE+5)
#define EV_CMID_SIM_GET_PBK_STRORAGE_INFO_RSP     (EV_CMID_SIM_RSP_BASE+6)
#define EV_CMID_SIM_GET_PROVIDER_ID_RSP           (EV_CMID_SIM_RSP_BASE+7)
#define EV_CMID_SIM_CHANGE_PWD_RSP                (EV_CMID_SIM_RSP_BASE+8)
#define EV_CMID_SIM_GET_AUTH_STATUS_RSP           (EV_CMID_SIM_RSP_BASE+9)
#define EV_CMID_SIM_ENTER_AUTH_RSP                (EV_CMID_SIM_RSP_BASE+11)
#define EV_CMID_SIM_SET_FACILITY_LOCK_RSP         (EV_CMID_SIM_RSP_BASE+12)
#define EV_CMID_SIM_GET_FACILITY_LOCK_RSP         (EV_CMID_SIM_RSP_BASE+13)
#define EV_CMID_SIM_DELETE_MESSAGE_RSP            (EV_CMID_SIM_RSP_BASE+15)
#define EV_CMID_SIM_WRITE_MESSAGE_RSP             (EV_CMID_SIM_RSP_BASE+16)
#define EV_CMID_SIM_READ_MESSAGE_RSP              (EV_CMID_SIM_RSP_BASE+17)
#define EV_CMID_SIM_LIST_MESSAGE_RSP              (EV_CMID_SIM_RSP_BASE+18)
#define EV_CMID_SIM_GET_SMS_STORAGE_INFO_RSP      (EV_CMID_SIM_RSP_BASE+19)
#define EV_CMID_SIM_READ_BINARY_RSP               (EV_CMID_SIM_RSP_BASE+20)
#define EV_CMID_SIM_UPDATE_BINARY_RSP             (EV_CMID_SIM_RSP_BASE+21)
#define EV_CMID_SIM_GET_SMS_PARAMETERS_RSP        (EV_CMID_SIM_RSP_BASE+22)
#define EV_CMID_SIM_SET_SMS_PARAMETERS_RSP        (EV_CMID_SIM_RSP_BASE+23)
#define EV_CMID_SIM_GET_MR_RSP                    (EV_CMID_SIM_RSP_BASE+24)
#define EV_CMID_SIM_SET_MR_RSP                    (EV_CMID_SIM_RSP_BASE+25)
#define EV_CMID_SIM_COMPOSE_PDU_RSP               (EV_CMID_SIM_RSP_BASE+26)
#define EV_CMID_SAT_ACTIVATION_RSP                (EV_CMID_SIM_RSP_BASE+27)
#define EV_CMID_SAT_GET_STATUS_RSP                (EV_CMID_SIM_RSP_BASE+28)
#define EV_CMID_SAT_RESPONSE_RSP                  (EV_CMID_SIM_RSP_BASE+29)
#define EV_CMID_SAT_GET_INFORMATION_RSP           (EV_CMID_SIM_RSP_BASE+30)
#define EV_CMID_SIM_SET_PREF_OPERATOR_LIST_RSP    (EV_CMID_SIM_RSP_BASE+31)
#define EV_CMID_SIM_GET_PREF_OPERATOR_LIST_RSP    (EV_CMID_SIM_RSP_BASE+32)
#define EV_CMID_SIM_GET_PREF_OPT_LIST_MAXNUM_RSP  (EV_CMID_SIM_RSP_BASE+33)
#define EV_CMID_SIM_SET_PBKMODE_RSP               (EV_CMID_SIM_RSP_BASE+34)
#define EV_CMID_SIM_GET_PBKMODE_RSP               (EV_CMID_SIM_RSP_BASE+35)
#define EV_CMID_SIM_INIT_SMS_RSP                  (EV_CMID_SIM_RSP_BASE+36)
#define EV_CMID_SIM_LIST_COUNT_PBK_ENTRY_RSP      (EV_CMID_SIM_RSP_BASE+37)
#define EV_CMID_SIM_RESET_RSP                     (EV_CMID_SIM_RSP_BASE+38)
#define EV_CMID_SIM_GET_PUCT_RSP                  (EV_CMID_SIM_RSP_BASE+39)
#define EV_CMID_SIM_SET_PUCT_RSP                  (EV_CMID_SIM_RSP_BASE+40)
#define EV_CMID_SIM_GET_ACMMAX_RSP                (EV_CMID_SIM_RSP_BASE+41)
#define EV_CMID_SIM_SET_ACMMAX_RSP                (EV_CMID_SIM_RSP_BASE+42)
#define EV_CMID_SIM_SET_ACM_RSP                   (EV_CMID_SIM_RSP_BASE+43)
#define EV_CMID_SIM_GET_ACM_RSP                   (EV_CMID_SIM_RSP_BASE+44)
#define	EV_CMID_SIM_READ_EF_RSP                   (EV_CMID_SIM_RSP_BASE+45)	
#define EV_CMID_SIM_GET_FILE_STATUS_RSP           (EV_CMID_SIM_RSP_BASE+46)
#define EV_CMID_SIM_READ_RECORD_RSP               (EV_CMID_SIM_RSP_BASE+47)
#define EV_CMID_SIM_UPDATE_RECORD_RSP             (EV_CMID_SIM_RSP_BASE+48)
#define EV_CMID_SIM_GET_SPN_RSP                   (EV_CMID_SIM_RSP_BASE+49)	
#define EV_CMID_SIM_GET_SMS_TOTAL_NUM_RSP         (EV_CMID_SIM_RSP_BASE+50)	
#define EV_CMID_SIM_GET_ICCID_RSP                 (EV_CMID_SIM_RSP_BASE+51)	

//
// SMS Response Event.
//
#define EV_CMID_SMS_DELETE_MESSAGE_RSP            (EV_CMID_SMS_RSP_BASE+1)
#define EV_CMID_SMS_GET_STORAGE_RSP               (EV_CMID_SMS_RSP_BASE+2)
#define EV_CMID_SMS_SET_STORAGE_RSP               (EV_CMID_SMS_RSP_BASE+3)
#define EV_CMID_SMS_LIST_MESSAGE_RSP              (EV_CMID_SMS_RSP_BASE+4)
#define EV_CMID_SMS_READ_MESSAGE_RSP              (EV_CMID_SMS_RSP_BASE+5)
#define EV_CMID_SMS_SEND_MESSAGE_RSP              (EV_CMID_SMS_RSP_BASE+6)
#define EV_CMID_SMS_SET_CENTER_ADDR_RSP           (EV_CMID_SMS_RSP_BASE+7)
#define EV_CMID_SMS_GET_CENTER_ADDR_RSP           (EV_CMID_SMS_RSP_BASE+8)
#define EV_CMID_SMS_SET_TEXT_MODE_PARAM_RSP       (EV_CMID_SMS_RSP_BASE+9)
#define EV_CMID_SMS_GET_TEXT_MODE_PARAM_RSP       (EV_CMID_SMS_RSP_BASE+10)
#define EV_CMID_SMS_WRITE_MESSAGE_RSP             (EV_CMID_SMS_RSP_BASE+11)
#define EV_CMID_SMS_LIST_MESSAGE_HRD_RSP          (EV_CMID_SMS_RSP_BASE+12)
#define EV_CMID_SMS_COPY_MESSAGE_RSP              (EV_CMID_SMS_RSP_BASE+13)
#define EV_CMID_SMS_SET_UNREAD2READ_RSP           (EV_CMID_SMS_RSP_BASE+14)
#define EV_CMID_SMS_SET_UNSENT2SENT_RSP           (EV_CMID_SMS_RSP_BASE+15)
#define EV_CMID_SMS_SET_READ2UNREAD_RSP           (EV_CMID_SMS_RSP_BASE+16)
#define EV_CMID_SMS_SET_PARAM_RSP                 (EV_CMID_SMS_RSP_BASE+17)
#define	EV_CMID_SIM_PATCH_RSP				             (EV_CMID_SMS_RSP_BASE+18)	
#define EV_CMID_CB_READ_SIM_RSP                      (EV_CMID_SMS_RSP_BASE+19)
//#endif
//#ifdef __MMI_MESSAGES_MOVE__
#define EV_CMID_SMS_MOVE_RSP                      (EV_CMID_SMS_RSP_BASE+20)
//#endif
#define EV_CMID_CB_SET_MID_SIM_RSP                (EV_CMID_SMS_RSP_BASE+21)

//
// PBK Response Event
//
#define EV_CMID_PBK_ADD_RSP                        (EV_CMID_PBK_RSP_BASE+1)
#define EV_CMID_PBK_UPDATE_ENTRY_RSP               (EV_CMID_PBK_RSP_BASE+2)
#define EV_CMID_PBK_DELETE_RSP                     (EV_CMID_PBK_RSP_BASE+3)
#define EV_CMID_PBK_DELETE_BATCH_ENTRIES_RSP       (EV_CMID_PBK_RSP_BASE+4)
#define EV_CMID_PBK_FIND_ENTRY_RSP                 (EV_CMID_PBK_RSP_BASE+5)
#define EV_CMID_PBK_LIST_ENTRIES_RSP               (EV_CMID_PBK_RSP_BASE+6)
#define EV_CMID_PBK_GET_ENTRY_RSP                  (EV_CMID_PBK_RSP_BASE+7)
#define EV_CMID_PBK_COPY_ENTRIES_RSP               (EV_CMID_PBK_RSP_BASE+8)
#define EV_CMID_PBK_ADD_ENTRY_ITEM_RSP             (EV_CMID_PBK_RSP_BASE+9)
#define EV_CMID_PBK_GET_ENTRY_ITEMS_RSP            (EV_CMID_PBK_RSP_BASE+10)
#define EV_CMID_PBK_DELETE_ENTRY_ITEM_RSP          (EV_CMID_PBK_RSP_BASE+11)
#define EV_CMID_PBK_ADD_CALLLOG_ENTTRY_RSP         (EV_CMID_PBK_RSP_BASE+12)
#define EV_CMID_PBK_GET_CALLLOG_ENTRY_RSP          (EV_CMID_PBK_RSP_BASE+13)
#define EV_CMID_PBK_LIST_CALLLOG_ENTRIES_RSP       (EV_CMID_PBK_RSP_BASE+14)
#define EV_CMID_PBK_DELETE_CALLLOG_ALL_ENTRIES_RSP (EV_CMID_PBK_RSP_BASE+15)
#define EV_CMID_PBK_CANCEL_RSP                     (EV_CMID_PBK_RSP_BASE+16)
#define EV_CMID_PBK_DELETE_CALLLOG_OLDEST_ENTRY_RSP (EV_CMID_PBK_RSP_BASE+17)
//
// Shell Response Event
//
#define EV_CMID_ME_SET_FACILITY_LOCK_RSP           (EV_CMID_SHELL_RSP_BASE+1)
#define EV_CMID_ME_GET_FACILITY_LOCK_RSP           (EV_CMID_SHELL_RSP_BASE+2)
#define EV_CMID_ME_CHANGE_PWD_RSP                  (EV_CMID_SHELL_RSP_BASE+3)


//
//TCP(UDP) / IP Event
//
#define EV_CMID_TCPIP_SOCKET_CONNECT_RSP           (EV_CMID_TCPIPUDP_RSP_BASE+1)
#define EV_CMID_TCPIP_SOCKET_CLOSE_RSP             (EV_CMID_TCPIPUDP_RSP_BASE+2)
#define EV_CMID_TCPIP_SOCKET_SEND_RSP              (EV_CMID_TCPIPUDP_RSP_BASE+3)

//
//test mode event
//
#define EV_CMID_TSM_INFO_IND							(EV_CMID_TM_RSP_BASE+1)
#define EV_CMID_TSM_INFO_END_RSP	                    (EV_CMID_TM_RSP_BASE+2)
#define EV_CMID_TSM_SYNC_INFO_IND					(EV_CMID_TM_RSP_BASE+3)
#define EV_CMID_TSM_SYNC_INFO_END_RSP	            (EV_CMID_TM_RSP_BASE+4)
#define EV_RMT_STTPC_REQ                            (EV_RMC_BASE+1)
#define EV_RMT_STTPC_RSP                            (EV_RMC_BASE+2)

#define EV_RMT_APP_REQ                              (EV_RMC_BASE+51)
#define EV_RMT_APP_RSP                              (EV_RMC_BASE+52)

#ifdef WIFI_SUPPORT
#define EV_CMID_WIFI_INIT_RSP			(EV_WIFI_RSP_BASE +1)
#define EV_CMID_WIFI_DEINIT_RSP		(EV_WIFI_RSP_BASE +2)
#define EV_CMID_WIFI_SCAN_RSP                 (EV_WIFI_RSP_BASE +3)
#define EV_CMID_WIFI_CONN_RSP			(EV_WIFI_RSP_BASE +4)
#define EV_CMID_WIFI_DISCONN_RSP		(EV_WIFI_RSP_BASE +5)
#define EV_CMID_WIFI_DISCONNECT_IND    (EV_WIFI_RSP_BASE +6)

#define EV_CMID_WIFI_SCAN_REQ    (EV_WIFI_RSP_BASE +7)
#define EV_CMID_WIFI_CONNECT_REQ    (EV_WIFI_RSP_BASE +8)
#define EV_CMID_WIFI_POWER_ON_REQ    (EV_WIFI_RSP_BASE +9)
#define EV_MMI_WIFI_IPADDR_CHANGE_REQ    (EV_WIFI_RSP_BASE +10)


#define EV_CMID_WIFI_CONNECTED_IND                    (EV_WIFI_IND_BASE+1)
#define EV_CMID_WIFI_DATA_IND                    (EV_WIFI_IND_BASE+2)
#define EV_CMID_WIFI_TEST_MODE_DATA_IND (EV_WIFI_IND_BASE+3)
#define EV_MAC_WIFI_DATA_IND (EV_WIFI_IND_BASE+4)
#define EV_MAC_WIFI_DATA_REQ (EV_WIFI_IND_BASE+5)
#define EV_CMID_WIFI_EAPOL_IND (EV_WIFI_IND_BASE+6)
#define EV_MMI_WIFI_CONNECTED_IND  (EV_WIFI_IND_BASE+7)
#define EV_MMI_WIFI_NEED_POWEROFF_IND  (EV_WIFI_IND_BASE+8)
#define EV_CMID_WIFI_IPADDR_UPDATE_IND  (EV_WIFI_IND_BASE+9)

#define EV_CMID_WIFI_IPADDR_CONFLICT_IND  (EV_WIFI_IND_BASE + 10)

#define EV_CMID_WIFI_CURR_AP_INFO_IND  (EV_WIFI_IND_BASE + 11)


#define EV_WIFI_WIFI_RESET_REQ (EV_WIFI_IND_BASE+12)

#define EV_WIFI_MMI_RESET_IND (EV_WIFI_IND_BASE+13)

#define EV_CMID_WIFI_DISCONNECT_REQ    (EV_WIFI_RSP_BASE +14)
#define EV_WIFI_WIFI_POWERON_CONTINUE_REQ (EV_WIFI_IND_BASE+15)
#define EV_CMID_WIFI_POWER_OFF_REQ (EV_WIFI_IND_BASE+16)
#define EV_MAC_WIFI_KET_SET_REQ (EV_WIFI_IND_BASE+17)

#define EV_CMID_WIFI_TEST_MODE_INFO_IND (EV_WIFI_IND_BASE+18)
#define EV_CMID_WIFI_GET_NETWORKINFO_REQ (EV_WIFI_IND_BASE+19)
#endif

#ifdef BT_TETHER_SUPPORT
#define EV_BT_TETHER_DATA_IND           (EV_BT_TETHER_BASE +1) 
#define EV_BT_TETHER_END_           (1450)
#endif

//#if defined(__SPI_FLSH_WITH_SUSPEND__)	
//#define EV_FMT_VDS_DATA_IND	(EV_FMT_VDS_SAP_BASE+1)
//#endif

#endif // __EVENTS_H__

