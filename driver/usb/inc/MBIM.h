#ifndef _MVUSBMBIM_H_
#define _MVUSBMBIM_H_

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     RNDIS MODULE

  Copyright (c) 2011 - 2015 by MARVELL Incorporated.
  All Rights Reserved.

  MARVELL Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who          what, where, why
--------   -------      ----------------------------------------------------------
07/19/11   zhoujin      Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
//#ifdef MV_USB_MBIM

//#include "utils.h"
//#include "mvUsbBuf.h"
//#include "mvUsbDevApi.h"
//#include "mvUsbDevPrv.h"
//#include "usb_device.h"

/*--- Macros ----------------------------------------------------------------*/

/* Max and Min*/
#define MAX(x,y) (( (x) > (y) ) ? (x) : (y))
#define MIN(x,y) (( (x) < (y) ) ? (x) : (y))


/* Control message sent from Host to client */
#define MBIM_OPEN_MSG       		        0x00000001U   /* Initialize the function. */
#define MBIM_CLOSE_MSG	                    0x00000002U	  /* Close the function */
#define MBIM_COMMAND_MSG     		        0x00000003U   /* Send a 'COMMAND' CID */
#define MBIM_HOST_ERROR_MSG		            0x00000004U   /* Indicates an error in the MBIM communication */

/* Control message sent from Client to Host */
#define MBIM_OPEN_DONE       		        0x80000001U   /* Device response to initialization request. */
#define MBIM_CLOSE_DONE	                    0x80000002U	  /* Device response to Close request */
#define MBIM_COMMAND_DONE     		        0x80000003U   /* Device response to command'CID request */
#define MBIM_FUNCTION_ERROR_MSG	            0x80000004U   /* Indicates an error in the MBIM communication*/
#define MBIM_INDICATE_STATUS_MSG            0x80000007U   /* Indicates unsolicited network or devices status changes*/

/* CDC NCM subclass Table 6-3: NTB Parameter Structure */
#define USB_CDC_NCM_NTB16_SUPPORTED			(1 << 0)
#define USB_CDC_NCM_NTB32_SUPPORTED			(1 << 1)

/* CDC NCM subclass 6.2.7 SetNtbInputSize */
#define USB_CDC_NCM_NTB_MIN_IN_SIZE			2048
#define USB_CDC_NCM_NTB_MIN_OUT_SIZE		2048

/* table 62; bits in multicast filter */
#define	USB_CDC_PACKET_TYPE_PROMISCUOUS		(1 << 0)
#define	USB_CDC_PACKET_TYPE_ALL_MULTICAST	(1 << 1)      /* no filter */
#define	USB_CDC_PACKET_TYPE_DIRECTED		(1 << 2)
#define	USB_CDC_PACKET_TYPE_BROADCAST		(1 << 3)
#define	USB_CDC_PACKET_TYPE_MULTICAST		(1 << 4)      /* filtered */
#define	USB_CDC_DEFAULT_FILTER	            (USB_CDC_PACKET_TYPE_BROADCAST \
			                                |USB_CDC_PACKET_TYPE_ALL_MULTICAST \
			                                |USB_CDC_PACKET_TYPE_PROMISCUOUS \
			                                |USB_CDC_PACKET_TYPE_DIRECTED)

/* MBIM sim class mask. */
#define MBIMSimClassSimLogical              0x01          /* Functions that do not have a physical SIM, must set this bit. */
#define MBIMSimClassSimRemovable            0x02          /* Functions supporting physical SIMs that are removable by the end-user must set this bit. */

/* MBIM sms caps mask. */
#define MBIMSmsCapsNone                     0x00
#define MBIMSmsCapsPduReceive               0x01
#define MBIMSmsCapsPduSend                  0x02
#define MBIMSmsCapsTextReceive              0x04
#define MBIMSmsCapsTextSend                 0x08

/* MBIM cellular class mask. */
#define MBIMCellularClassGsm                0x01
#define MBIMCellularClassCdma               0x02

/* MBIM ctrl caps. */
#define MBIMCtrlCapsNone                    0x00
#define MBIMCtrlCapsRegManual               0x01          /* Indicates whether the device allows manual network selection.  */
#define MBIMCtrlCapsHwRadioSwitch           0x02          /* Indicates the presence of a hardware radio power switch. */
#define MBIMCtrlCapsCdmaMobileIp            0x04          /* Indicates that the CDMA-based function is configured to support mobile IP.*/
#define MBIMCtrlCapsCdmaSimpleIp            0x08          /* Indicates that the CDMA-based function is configured for simple IP support. */
#define MBIMCtrlCapsMultiCarrier            0x10          /* Indicates that the device can work with multiple-providers. */

/* MBIM data class. */
#define MBIMDataClassNone                   0x00
#define MBIMDataClassGPRS                   0x01
#define MBIMDataClassEDGE                   0x02
#define MBIMDataClassUMTS                   0x04
#define MBIMDataClassHSDPA                  0x08
#define MBIMDataClassHSUPA                  0x10
#define MBIMDataClassLTE                    0x20
#define MBIMDataClass1XRTT                  0x10000
#define MBIMDataClass1XEVDO                 0x20000
#define MBIMDataClass1XEVDORevA             0x40000
#define MBIMDataClass1XEVDV                 0x80000
#define MBIMDataClass3XRTT                  0x100000
#define MBIMDataClass1XEVDORevB             0x200000
#define MBIMDataClassUMB                    0x400000
#define MBIMDataClassCustom                 0x80000000

/* MBIM data class mask. */
#define MBIM_DATA_CLASS_MASK                (MBIMDataClassUMTS\
	                                        |MBIMDataClassHSDPA\
								            |MBIMDataClassHSUPA\
								            |MBIMDataClassLTE)

/* The USB cdc format supported. */
#define USB_CDC_FORMATS_SUPPORTED           USB_CDC_NCM_NTB16_SUPPORTED

/* The MAX USB NCM NTB in or out size. */
#define USB_CDC_NCM_NTB_IN_MAX_SIZE         0x4000
#define USB_CDC_NCM_NTB_OUT_MAX_SIZE        0x1000

/* CDC NCM transfer headers, CDC NCM subclass 3.2 */
#define USB_CDC_NCM_NTH16_SIGN		        0x484D434E /* NCMH */
#define USB_CDC_NCM_NTH32_SIGN		        0x686D636E /* ncmh */

/* CDC NCM datagram pointers, CDC NCM subclass 3.3 */
#define USB_CDC_NCM_NDP16_CRC_SIGN	        0x314D434E /* NCM1 */
#define USB_CDC_NCM_NDP16_NOCRC_SIGN	    0x304D434E /* NCM0 */
#define USB_CDC_NCM_NDP32_CRC_SIGN	        0x316D636E /* ncm1 */
#define USB_CDC_NCM_NDP32_NOCRC_SIGN	    0x306D636E /* ncm0 */
#define USB_CDC_MBIM_NDP16_IPS_SIGN         0x00535049 /* IPS<sessionID> : IPS0 for now */
#define USB_CDC_MBIM_NDP32_IPS_SIGN         0x00737069 /* ips<sessionID> : ips0 for now */
#define USB_CDC_MBIM_NDP16_DSS_SIGN         0x00535344 /* DSS<sessionID> */
#define USB_CDC_MBIM_NDP32_DSS_SIGN         0x00737364 /* dss<sessionID> */

/* CDC NCM CRC */
#define NCM_NDP_HDR_CRC_MASK	            0x01000000 /* CRC mask */
#define NCM_NDP_HDR_CRC		                0x01000000 /* CRC mode */
#define NCM_NDP_HDR_NOCRC	                0x00000000 /* non-CRC mode */

/* Basic IP Connectivity: Basic_connect_uuid */
#define MBIM_BASIC_CONNECT_UUID_A           0x33cc89a2 /* a1a2a3a4 */
#define MBIM_BASIC_CONNECT_UUID_B           0xbbbc     /* b1b2 */
#define MBIM_BASIC_CONNECT_UUID_C           0x4f8b     /* c1c2 */
#define MBIM_BASIC_CONNECT_UUID_D           0xb0b6     /* d1d2 */
#define MBIM_BASIC_CONNECT_UUID_EH          0x3e13     /* e1e2 */
#define MBIM_BASIC_CONNECT_UUID_EL          0xdfe6aac2 /* e3e4e5e6 */

/* SMS: UUID_SMS */
#define MBIM_SMS_UUID_A                     0xebbe3f53 /* a1a2a3a4 */
#define MBIM_SMS_UUID_B                     0xfe14     /* b1b2 */
#define MBIM_SMS_UUID_C                     0x6744     /* c1c2 */
#define MBIM_SMS_UUID_D                     0x909f     /* d1d2 */
#define MBIM_SMS_UUID_EH                    0xa233     /* e1e2 */
#define MBIM_SMS_UUID_EL                    0x3f6ce523 /* e3e4e5e6 */

/* USSD (Unstructured Supplementary Service Data) */
#define MBIM_USSD_UUID_A                    0xc8a050e5 /* a1a2a3a4 */
#define MBIM_USSD_UUID_B                    0x825e     /* b1b2 */
#define MBIM_USSD_UUID_C                    0x9e47     /* c1c2 */
#define MBIM_USSD_UUID_D                    0xf782     /* d1d2 */
#define MBIM_USSD_UUID_EH                   0xab10     /* e1e2 */
#define MBIM_USSD_UUID_EL                   0x1f35c3f4 /* e3e4e5e6 */

/* Phonebook: uuid phonebook */
#define MBIM_PHONEBOOK_UUID_A               0x7684f34b /* a1a2a3a4 */
#define MBIM_PHONEBOOK_UUID_B               0x6a1e     /* b1b2 */
#define MBIM_PHONEBOOK_UUID_C               0xdb41     /* c1c2 */
#define MBIM_PHONEBOOK_UUID_D               0xd8b1     /* d1d2 */
#define MBIM_PHONEBOOK_UUID_EH              0xd2be     /* e1e2 */
#define MBIM_PHONEBOOK_UUID_EL              0xdb5bc289 /* e3e4e5e6 */

/* STK (SIM Toolkit): STK uuid */
#define MBIM_STK_UUID_A                     0x3101f2d8 /* a1a2a3a4 */
#define MBIM_STK_UUID_B                     0xb5fc     /* b1b2 */
#define MBIM_STK_UUID_C                     0x174e     /* c1c2 */
#define MBIM_STK_UUID_D                     0x0286     /* d1d2 */
#define MBIM_STK_UUID_EH                    0xedd6     /* e1e2 */
#define MBIM_STK_UUID_EL                    0x4c161638 /* e3e4e5e6 */

/* Authentication: AUTH uuid */
#define MBIM_AUTH_UUID_A                    0xf75f2b1d /* a1a2a3a4 */
#define MBIM_AUTH_UUID_B                    0xa10a     /* b1b2 */
#define MBIM_AUTH_UUID_C                    0xb248     /* c1c2 */
#define MBIM_AUTH_UUID_D                    0x52aa     /* d1d2 */
#define MBIM_AUTH_UUID_EH                   0xf150     /* e1e2 */
#define MBIM_AUTH_UUID_EL                   0x4e176757 /* e3e4e5e6 */

/* Device Service Stream: DSS uuid */
#define MBIM_DSS_UUID_A                     0xdd268ac0 /* a1a2a3a4 */
#define MBIM_DSS_UUID_B                     0x1877     /* b1b2 */
#define MBIM_DSS_UUID_C                     0x8243     /* c1c2 */
#define MBIM_DSS_UUID_D                     0x8284     /* d1d2 */
#define MBIM_DSS_UUID_EH                    0x0d6e     /* e1e2 */
#define MBIM_DSS_UUID_EL                    0x0e4d3c58 /* e3e4e5e6 */

/*===========================================================================

                          Struct definition.

===========================================================================*/

/*
 * For a complete list of the Status codes and their descriptions, please refer
 * to the MBIM_STATUS_CODES type.
 *
 */

typedef enum
{
    MBIM_STATUS_SUCCESS                                 = 0,       /* The operation succeeded */
    MBIM_STATUS_BUSY                                    = 1,       /* The operation failed because the device is busy. */
	MBIM_STATUS_FAILURE                                 = 2,       /* The operation failed (a generic failure).*/
	MBIM_STATUS_SIM_NOT_INSERTED                        = 3,       /* The operation failed because the SIM card was not fully inserted in to the device.*/
    MBIM_STATUS_BAD_SIM                                 = 4,       /* The operation failed because the SIM card is bad and cannot be used any further. */
    MBIM_STATUS_PIN_REQUIRED                            = 5,       /* The operation failed because a PIN must be entered to proceed. */
    MBIM_STATUS_PIN_DISABLED                            = 6,       /* The operation failed because the PIN is disabled. */
    MBIM_STATUS_NOT_REGISTERED                          = 7,       /* The operation failed because the device is not registered with any network. */
    MBIM_STATUS_PROVIDERS_NOT_FOUND                     = 8,       /* The operation failed because no network providers could be found. */
    MBIM_STATUS_NO_DEVICE_SUPPORT                       = 9,       /* The operation failed because the device does not support the operation. */
    MBIM_STATUS_PROVIDER_NOT_VISIBLE                    = 10,      /* The operation failed because the service provider is not currently visible. */
    MBIM_STATUS_DATA_CLASS_NOT_AVAILABLE                = 11,      /* The operation failed because the requested data-class was not available.*/
    MBIM_STATUS_PACKET_SERVICE_DETACHED                 = 12,      /* The operation failed because the packet service is detached.  */
    MBIM_STATUS_MAX_ACTIVATED_CONTEXTS                  = 13,      /* The operation failed because the maximum number of activated contexts has been reached.  */
    MBIM_STATUS_NOT_INITIALIZED                         = 14,      /* The operation failed because the device is in the process of initializing. */
    MBIM_STATUS_VOICE_CALL_IN_PROGRESS                  = 15,      /* The operation failed because a voice call is in progress. */
    MBIM_STATUS_CONTEXT_NOT_ACTIVATED                   = 16,      /* The operation failed because the context is not activated. */
    MBIM_STATUS_SERVICE_NOT_ACTIVATED                   = 17,      /* The operation failed because service is not activated. */
    MBIM_STATUS_INVALID_ACCESS_STRING                   = 18,      /* The operation failed because the access string is invalid. */
    MBIM_STATUS_INVALID_USER_NAME_PWD                   = 19,      /* The operation failed because the username and/or password supplied are invalid. */
    MBIM_STATUS_RADIO_POWER_OFF                         = 20,      /* The operation failed because the radio is currently powered off. */
    MBIM_STATUS_INVALID_PARAMETERS                      = 21,      /* The operation failed because of invalid parameters. */
    MBIM_STATUS_READ_FAILURE                            = 22,      /* The operation failed because of a read failure.  */
    MBIM_STATUS_WRITE_FAILURE                           = 23,      /* The operation failed because of a write failure. */
    MBIM_STATUS_NO_PHONEBOOK                            = 25,      /* The phonebook operation failed because there is no phone book. */
    MBIM_STATUS_PARAMETER_TOO_LONG                      = 26,      /* A parameter with dynamic size is larger than the function can handle. */
    MBIM_STATUS_STK_BUSY                                = 27,      /* The SIM Toolkit application on the SIM card is busy and the command could not be processed. */
    MBIM_STATUS_OPERATION_NOT_ALLOWED                   = 28,      /* The operation failed because the operation is not allowed. */
    MBIM_STATUS_MEMORY_FAILURE                          = 29,      /* The phonebook or sms operation failed because the because of device or SIM memory failure. */
    MBIM_STATUS_INVALID_MEMORY_INDEX                    = 30,      /* The phonebook or sms operation failed because of an invalid memory index.  */
    MBIM_STATUS_MEMORY_FULL                             = 31,      /* The phonebook or sms operation failed because the device or SIM memory is full. */
    MBIM_STATUS_FILTER_NOT_SUPPORTED                    = 32,      /* The phonebook or sms operation failed because the filter type is not supported. */
    MBIM_STATUS_DSS_INSTANCE_LIMIT                      = 33,      /* Attempt to open a device service failed because the number of opened streams has reached the device service instance limit for this service. */
    MBIM_STATUS_INVALID_DEVICE_SERVICE_OPERATION        = 34,      /* The device service operation attempted is invalid. */
    MBIM_STATUS_AUTH_INCORRECT_AUTN                     = 35,      /* The device sets this error on an AKA or AKAPrime challenge response when the AKA or AKAPrime challenge sent has incorrect AUTN.*/
    MBIM_STATUS_AUTH_SYNC_FAILURE                       = 36,      /* The device sets this error on a an AKA or AKAPrime challenge response when the AKA or AKAPrime challenge sent has synchronization failure.  When this error code is returned the AUTS field would be set.  */
    MBIM_STATUS_AUTH_AMF_NOT_SET                        = 37,      /* The device sets this error on a an AKA or AKAPrime challenge response when the AKA or AKAPrime challenge sent does not have  the AMF bit set to 1. */
    MBIM_STATUS_SMS_UNKNOWN_SMSC_ADDRESS                = 100,     /* The SMS operation failed because the service center address is either invalid or unknown. */
    MBIM_STATUS_SMS_NETWORK_TIMEOUT                     = 101,     /* The SMS operation failed because of a network timeout. */
    MBIM_STATUS_SMS_LANG_NOT_SUPPORTED                  = 102,     /* The SMS operation failed because the SMS language is not supported. This applies to CDMA based devices only. */
    MBIM_STATUS_SMS_ENCODING_NOT_SUPPORTED              = 103,     /* The SMS operation failed because the SMS encoding is not supported. This applies to CDMA based device only. */
    MBIM_STATUS_SMS_FORMAT_NOT_SUPPORTED                = 104,     /* The SMS operation failed because the SMS format is not supported. */
	MBIM_STATUS_MAX_CODE
}mbim_status_codes;

/* CID for Basic IP Connectivity. */
typedef enum
{
    MBIM_CID_DEVICE_CAPS                                = 1,
    MBIM_CID_SUBSCRIBER_READY_STATUS                    = 2,
	MBIM_CID_RADIO_STATE                                = 3,
	MBIM_CID_PIN                                        = 4,
	MBIM_CID_PIN_LIST                                   = 5,
	MBIM_CID_HOME_PROVIDER                              = 6,
	MBIM_CID_PREFERRED_PROVIDERS                        = 7,
	MBIM_CID_VISIBLE_PROVIDERS                          = 8,
	MBIM_CID_REGISTER_STATE                             = 9,
	MBIM_CID_PACKET_SERVICE                             = 10,
	MBIM_CID_SIGNAL_STATE                               = 11,
	MBIM_CID_CONNECT                                    = 12,
	MBIM_CID_PROVISIONED_CONTEXTS                       = 13,
	MBIM_CID_SERVICE_ACTIVATION                         = 14,
	MBIM_CID_IP_CONFIGURATION                           = 15,
	MBIM_CID_DEVICE_SERVICES                            = 16,
	MBIM_CID_DEVICE_SERVICE_SUBSCRIBE_LST               = 19,
	MBIM_CID_PACKET_STATISTICS                          = 20,
	MBIM_CID_NETWORK_IDLE_HINT                          = 21,
	MBIM_CID_EMERGENCY_MODE                             = 22,
	MBIM_CID_IP_PACKET_FILTERS                          = 23,
	MBIM_CID_MULTICARRIER_PROVIDERS                     = 24,
	MBIM_CID_BASIC_CONNECT_MAX_TYPE
}mbim_basic_connect_cid_type;

/* CID for SMS. */
typedef enum
{
    MBIM_CID_SMS_CONFIGURATION                          = 1,
    MBIM_CID_SMS_READ                                   = 2,
    MBIM_CID_SMS_SEND                                   = 3,
	MBIM_CID_SMS_DELETE                                 = 4,
	MBIM_CID_SMS_MESSAGE_STORE_STATUS                   = 5,
	MBIM_CID_SMS_MAX_TYPE
}mbim_sms_cid_type;

/* CID for USSD. */
typedef enum
{
    MBIM_CID_USSD                                       = 1,
	MBIM_CID_USSD_MAX_TYPE
}mbim_ussd_cid_type;

/* CID for PHONEBOOK. */
typedef enum
{
    MBIM_CID_PHONEBOOK_CONFIGURATION                    = 1,
	MBIM_CID_PHONEBOOK_READ                             = 2,
	MBIM_CID_PHONEBOOK_DELETE                           = 3,
	MBIM_CID_PHONEBOOK_WRITE                            = 4,
	MBIM_CID_PHONEBOOK_MAX_TYPE
}mbim_phonebook_cid_type;

/* CID for STK. */
typedef enum
{
    MBIM_CID_STK_PAC                                    = 1,
	MBIM_CID_STK_TERMINAL_RESPONSE                      = 2,
	MBIM_CID_STK_ENVELOPE                               = 3,
	MBIM_CID_STK_MAX_TYPE
}mbim_stk_cid_type;

/* CID for AUTH. */
typedef enum
{
    MBIM_CID_AKA_AUTH                                   = 1,
	MBIM_CID_AKAP_AUTH                                  = 2,
	MBIM_CID_SIM_AUTH                                   = 3,
	MBIM_CID_AUTH_MAX_TYPE
}mbim_auth_cid_type;

/* CID for DSS. */
typedef enum
{
    MBIM_CID_DSS_CONNECT                                = 1,
	MBIM_CID_DSS_MAX_TYPE
}mbim_dss_cid_type;

/* MBIM device type. */
typedef enum
{
    MBIMDeviceTypeUnknown                               = 0x00,       /* The function type is unknown. */
	MBIMDeviceTypeEmbedded                              = 0x01,       /* The function type is embedded in the system */
	MBIMDeviceTypeRemovable                             = 0x02,       /* The function is implemented within a device that is removable, for example, a pluggable USB device. */
	MBIMDeviceTypeRemote                                = 0x03,       /* The function type is remote, for example, a tethered cellular phone modem. */
	MBIMDeviceMaxType                                   = 0xFFFFFF
}mbim_device_type;

/* MBIM voice class. */
typedef enum
{
    MBIMVoiceClassUnknown                               = 0x00,       /* The device uses an unknown method to support voice connections. */
	MBIMVoiceClassNoVoice                               = 0x01,       /* The device does not support voice connections. */
	MBIMVoiceClassSeparateVoiceData                     = 0x02,       /* The device supports separate voice and data connections. */
	MBIMVoiceClassSimultaneousVoiceData                 = 0x03,       /* The device supports simultaneous voice and data connections. */
	MBIMVoiceClassMaxType                               = 0xFFFFFF
}mbim_voice_class_type;

/* MBIM subscriber ready state. */
typedef enum
{
    MBIMSubscriberReadyStateNotInitialized              = 0x00,         /* The SIM has not yet completed its initialization. */
	MBIMSubscriberReadyStateInitialized                 = 0x01,         /* The SIM is initialized. */
	MBIMSubscriberReadyStateSimNotInserted              = 0x02,         /* The SIM card is not inserted into the device. */
	MBIMSubscriberReadyStateBadSim                      = 0x03,         /* The SIM card inserted into the device is invalid*/
	MBIMSubscriberReadyStateFailure                     = 0x04,         /* A general SIM failure has occurred. */
	MBIMSubscriberReadyStateNotActivated                = 0x05,         /* The subscription is not activated.*/
	MBIMSubscriberReadyStateDeviceLocked                = 0x06,         /* The SIM is locked and requires PIN1 or PUK1 to unlock. */
	MBIMSubscriberReadyMaxState                         = 0xFFFFFF
}mbim_subscriber_ready_state;

/* MBIM unique id flags. */
typedef enum
{
    MBIMReadyInfoFlagsNone                              = 0x00,         /* The device is in normal mode. */
	MBIMReadyInfoFlagsProtectUniqueID                   = 0x01,         /* When this flag is specified, the host will not display the SubscriberId specified in the same CID. */
	MBIMReadyInfoMaxFlags                               = 0xFFFFFF
}mbim_unique_id_flags;

/* MBIM radio switch state. */
typedef enum
{
    MBIMRadioOff                                        = 0x00,
	MBIMRadioOn                                         = 0x01,
	MBIMRadioMaxState                                   = 0xFFFFFF
}mbim_radio_switch_state;

/* MBIM Pin Type. */
typedef enum
{
    MBIMPinTypeNone                                     = 0x00,         /* No PIN is pending to be entered. */
	MBIMPinTypeCustom                                   = 0x01,         /* The PIN type is a custom type and is none of the other PIN types listed in this enumeration. */
	MBIMPinTypePin1                                     = 0x02,         /* The PIN1 key. */
	MBIMPinTypePin2                                     = 0x03,         /* The PIN2 key. */
	MBIMPinTypeDeviceSimPin                             = 0x04,         /* The device to SIM key.*/
	MBIMPinTypeDeviceFirstSimPin                        = 0x05,         /* The device to very first SIM key.*/
	MBIMPinTypeNetworkPin                               = 0x06,         /* The network personalization key.*/
	MBIMPinTypeNetworkSubsetPin                         = 0x07,         /* The network subset personalization key.*/
	MBIMPinTypeServiceProviderPin                       = 0x08,         /* The service provider (SP) personalization key.*/
	MBIMPinTypeCorporatePin                             = 0x09,         /* The corporate personalization key.*/
	MBIMPinTypeSubsidyLock                              = 0x0A,         /* The subsidy unlock key.*/
	MBIMPinTypePuk1                                     = 0x0B,         /* The Personal Identification Number1 Unlock Key (PUK1).*/
	MBIMPinTypePuk2                                     = 0x0C,         /* The Personal Identification Number2 Unlock Key (PUK2).*/
	MBIMPinTypeDeviceFirstSimPuk                        = 0x0D,         /* The device to very first SIM PIN unlock key.*/
	MBIMPinTypeNetworkPuk                               = 0x0E,         /* The network personalization unlock key.*/
	MBIMPinTypeNetworkSubsetPuk                         = 0x0F,         /* The network subset personalization unlock key.*/
	MBIMPinTypeServiceProviderPuk                       = 0x10,         /* The service provider (SP) personalization unlock key.*/
	MBIMPinTypeCorporatePuk                             = 0x11,         /* The corporate personalization unlock key.*/
	MBIMPinMaxType                                      = 0xFFFFFF
}mbim_pin_type;

/* MBIM Pin State. */
typedef enum
{
    MBIMPinStateUnlocked                                = 0x00,         /* No PIN is pending to be entered. */
	MBIMPinStateLocked                                  = 0x01,         /* The device requires the user to enter a PIN. */
	MBIMPinMaxState                                     = 0xFFFFFF
}mbim_pin_state;

/* MBIM Pin Operation. */
typedef enum
{
    MBIMPinOperationEnter                               = 0x00,         /* Enter the specified PIN into the device. */
	MBIMPinOperationEnable                              = 0x01,         /* Enable the specified PIN. */
	MBIMPinOperationDisable                             = 0x02,         /* Disable the specified PIN.*/
	MBIMPinOperationChange                              = 0x03,         /* Change the specified PIN. */
	MBIMPinOperationMaxType                             = 0xFFFFFF
}mbim_pin_operation;

/* MBIM Pin Mode. */
typedef enum
{
    MBIMPinModeNotSupported                             = 0x00,
	MBIMPinModeEnabled                                  = 0x01,
	MBIMPinModeDisabled                                 = 0x02,
	MBIMPinMaxMode                                      = 0xFFFFFF
}mbim_pin_mode;

/* MBIM Pin Format. */
typedef enum
{
    MBIMPinFormatUnknown                                = 0x00,
	MBIMPinFormatNumeric                                = 0x01,
	MBIMPinFormatAlphaNumeric                           = 0x02,
	MBIMPinFormatMaxType                                = 0xFFFFFF
}mbim_pin_format;

/* MBIM Provider State. */
typedef enum
{
    MBIM_PROVIDER_STATE_UNKNOWN                         = 0x00,         /* The network provider state is unknown. */
	MBIM_PROVIDER_STATE_HOME                            = 0x01,         /* The network provider is a home operator. */
	MBIM_PROVIDER_STATE_FORBIDDEN                       = 0x02,	        /* The network provider is on the blocked list.*/
	MBIM_PROVIDER_STATE_PREFERRED                       = 0x04,         /* The network provider is on the preferred list.*/
	MBIM_PROVIDER_STATE_VISIBLE                         = 0x08,         /* The network provider is visible.*/
	MBIM_PROVIDER_STATE_REGISTERED                      = 0x10,         /* The network provider is currently registered by the device.*/
	MBIM_PROVIDER_STATE_PREFERRED_MULTICARRIER          = 0x20,         /* The network provider is a preferred multicarrier network. */
	MBIM_PROVIDER_MAX_STATE                             = 0xFFFFFF
}mbim_provider_state;

/* MBIM visible Provider action Type. */
typedef enum
{
    MBIMVisibleProvidersActionFullScan                  = 0x00,         /* Device should perform a full scan. */

	MBIMVisibleProvidersActionRestrictedScan            = 0x01,         /* Device should perform a restricted scan to locate preferred multicarrier providers.*/
	                                                                    /* The device may also report a static list in case a scan is not possible. */
	MBIMVisibleProvidersActionMaxType                   = 0xFFFFFF

}mbim_visible_provider_action_type;

/* MBIM emergency mode states. */
typedef enum
{
    MBIMEmergencyModeOff                                = 0x00,       /* Emergency service is not available.  */
	MBIMEmergencyModeOn                                 = 0x01,       /* mergency service is available. */
    MBIMEmergencyMaxMode                                = 0xFFFFFF
}mbim_emergency_mode_states;

/* MBIM activation command. */
typedef enum
{
    MBIMActivationCommandDeactivate                     = 0x00,
	MBIMActivationCommandActivate                       = 0x01,
    MBIMActivationCommandMaxType                        = 0xFFFFFF
}mbim_activation_command_type;

/* MBIM Compression state. */
typedef enum
{
    MBIMCompressionNone                                 = 0x00,
	MBIMCompressionEnable                               = 0x01,
    MBIMCompressionMaxType                              = 0xFFFFFF
}mbim_compression_state;

/* MBIM Auth Protocol. */
typedef enum
{
    MBIMAuthProtocolNone                                = 0x00,
	MBIMAuthProtocolPap                                 = 0x01,
	MBIMAuthProtocolChap                                = 0x02,
	MBIMAuthProtocolMsChapV2                            = 0x03,
    MBIMAuthProtocolMaxType                             = 0xFFFFFF
}mbim_auth_protocol_type;

/* MBIM Context IP type. */
typedef enum
{
    MBIMContextIPTypeDefault                            = 0x00,       /* It is up to the function to decide, the host does not care. */
	MBIMContextIPTypeIPv4                               = 0x01,       /* IPv4 context. */
	MBIMContextIPTypeIPv6                               = 0x02,       /* IPv6 context. */
	MBIMContextIPTypeIPv4v6                             = 0x03,       /* The context is IPv4, IPv6 or dual-stack IPv4v6. */
	MBIMContextIPTypeIPv4AndIPv6                        = 0x04,       /* Both an IPv4 and an IPv6 context. */
    MBIMContextIPMaxType                                = 0xFFFFFF
}mbim_context_ip_type;

/* MBIM Activation State. */
typedef enum
{
    MBIMActivationStateUnknown                          = 0x00,
	MBIMActivationStateActivated                        = 0x01,
	MBIMActivationStateActivating                       = 0x02,
	MBIMActivationStateDeactivated                      = 0x03,
	MBIMActivationStateDeactivating                     = 0x04,
    MBIMActivationMaxState                              = 0xFFFFFF
}mbim_activation_state;

/* MBIM Voice Call State. */
typedef enum
{
    MBIMVoiceCallStateNone                              = 0x00,
	MBIMVoiceCallStateInProgress                        = 0x01,
	MBIMVoiceCallStateHangUp                            = 0x02,
    MBIMVoiceCallMaxState                               = 0xFFFFFF
}mbim_voice_call_state;

/* MBIM SMS Storage state. */
typedef enum
{
    MBIMSmsStorageNotInitialized                        = 0x00,
	MBIMSmsStorageInitialized                           = 0x01,
    MBIMSmsStorageMaxState                              = 0xFFFFFF
}mbim_SMS_storage_state;

/* MBIM SMS Format. */
typedef enum
{
    MBIMSmsFormatPdu                                    = 0x00,
	MBIMSmsFormatCdma                                   = 0x01,
    MBIMSmsFormatMaxType                                = 0xFFFFFF
}mbim_SMS_format_type;

/*
 * A 16 byte UUID that identifies the device service the following CID value
 * applies. a UUID string example: {a1a2a3a4©\b1b2©\c1c2©\d1d2©\e1e2e3e4e5e6}
 *
 */
typedef struct mbim_device_service_id {

	UINT32	uuid_a;                         /* a1a2a3a4 */

	UINT16	uuid_b;                         /* b1b2 */

	UINT16	uuid_c;                         /* c1c2 */

	UINT16	uuid_d;                         /* d1d2 */

	UINT16	uuid_eh;                        /* e1e2 */

	UINT32	uuid_el;                        /* e3e4e5e6 */

}mbim_device_service_id;

/*
 * MBIM control messages must be supported by an MBIM function. In the more
 * detailed descriptions below, the messages include a TransactionId field.

 * This is used to match sent messages with responses.  With this mechanism,
 * a host can send multiple MBIM messages to a function concurrently without
 * concern for the ordering of responses. An MBIM function must maintain the
 * TransactionId field when returning a response.
 *
 */

typedef struct mbim_message_hdr {

	UINT32	MessageType;                    //Specifies the MBIM message type.
	                                        //See Table 9-3: Control messages sent from the host to the function
	                                        //and Table 9-9: Control Messages sent from function to host

	                                        //The Most Significant Bit in the MessageType indicates direction:
                                            // MSB set to 0: from host to function
                                            // MSB set to 1: from function to host

	UINT32	MessageLength;                  //Specifies the total length of this MBIM message in bytes

	UINT32  TransactionId;                  //Specifies the MBIM message id value.
	                                        //This value is used to match host-sent messages with function responses.
	                                        //This value must be unique among all outstanding transactions.
	                                        //For notifications, the TransactionId must be set to 0 by the function.
}mbim_message_hdr;


/*
 * If the size of a command or a response is larger than MaxControlTransfer,
 * the message may be split across multiple messages.  This header indicates
 * how many fragments there are in total.  For fragmentation considerations,
 * see section 9.5 (Fragmentation of messages).
 *
 */

typedef struct mbim_fragment_hdr {

	UINT32	TotalFragments;                 //This field indicates how many fragments there are in total.
	                                        //For fragmentation considerations, see section 9.5 (Fragmentation of messages)

	UINT32	CurrentFragment;                //This field indicates which fragment this message is.
	                                        //Values are 0 to TotalFragments-1
}mbim_fragment_hdr;

/*
 * Class Specific structures and constants
 *
 * CDC NCM NTB parameters structure, CDC NCM subclass 6.2.1
 *
 */

typedef struct mbim_ntb_parameters {
	UINT16	wLength;                        //Size of this structure, in bytes = 1Ch.

	UINT16	bmNtbFormatsSupported;          //Bit 0: 16-bit NTB supported (set to 1)
	                                        //Bit 1: 32-bit NTB supported
	                                        //Bits 2-15: reserved (reset to zero; must be ignored by host)

	UINT32	dwNtbInMaxSize;                 //IN NTB Maximum Size in bytes

	UINT16	wNdpInDivisor;                  //Divisor used for IN NTB Datagram payload alignment

	UINT16	wNdpInPayloadRemainder;         //Remainder used to align input datagram payload within the NTB:
	                                        //(Payload Offset) mod (wNdpInDivisor) = wNdpInPayloadRemainder

	UINT16	wNdpInAlignment;                //NDP alignment modulus for NTBs on the IN pipe.
	                                        //Shall be a power of 2, and shall be at least 4.

	UINT16	wPadding1;                      //Padding, shall be transmitted as zero by function, and ignored by host.

	UINT32	dwNtbOutMaxSize;                //OUT NTB Maximum Size

	UINT16	wNdpOutDivisor;                 //OUT NTB Datagram alignment modulus

	UINT16	wNdpOutPayloadRemainder;        //Remainder used to align output datagram payload offsets within the NTB:
                                            //(Payload Offset) mod (wNdpOutDivisor) = wNdpOutPayloadRemainder

	UINT16	wNdpOutAlignment;               //NDP alignment modulus for use in NTBs on the OUT pipe.
	                                        //Shall be a power of 2, and shall be at least 4.

	UINT16	wNtbOutMaxDatagrams;            //Maximum number of datagrams that the host may pack into a single OUT NTB.
	                                        //Zero means that the device imposes no limit.
}mbim_ntb_parameters;

/* The options for NCM Datagram Pointer table (NDP) parser. */
typedef struct ndp_parser_opts {
	UINT32	nth_sign;
	UINT32	ndp_sign;
	UINT16	nth_size;
	UINT16	ndp_size;
	UINT16	ndplen_align;
	UINT16	dgram_item_len;
	UINT16	block_length;
	UINT16	fp_index;
	UINT16	reserved1;
	UINT16	reserved2;
	UINT16	next_fp_index;
}ndp_parser_opts;

/* A 16-bit NTB must begin with an NTH16 structure. */
typedef struct mbim_ncm_nth16 {
	UINT32	dwSignature;
	UINT16	wHeaderLength;
	UINT16	wSequence;
	UINT16	wBlockLength;
	UINT16	wNdpIndex;
} mbim_ncm_nth16;

/* The 32-bit form of the NTH is described in Table 3-2. */
typedef struct mbim_ncm_nth32 {
	UINT32	dwSignature;
	UINT16	wHeaderLength;
	UINT16	wSequence;
	UINT32	dwBlockLength;
	UINT32	dwNdpIndex;
}mbim_ncm_nth32;

/* 16-bit NCM Datagram Pointer Entry */
typedef struct mbim_ncm_dpe16 {
	UINT16	wDatagramIndex;
	UINT16	wDatagramLength;
} mbim_ncm_dpe16;

/* 16-bit NCM Datagram Pointer Table */
typedef struct mbim_ncm_ndp16 {
	UINT32	dwSignature;
	UINT16	wLength;
	UINT16	wNextNdpIndex;
	mbim_ncm_dpe16 dpe16[0];
} mbim_ncm_ndp16;

/* 32-bit NCM Datagram Pointer Entry */
typedef struct mbim_ncm_dpe32 {
	UINT32	dwDatagramIndex;
	UINT32	dwDatagramLength;
} mbim_ncm_dpe32;

/* 32-bit NCM Datagram Pointer Table */
typedef struct mbim_ncm_ndp32 {
	UINT32	dwSignature;
	UINT16	wLength;
	UINT16	wReserved6;
	UINT32	dwNextNdpIndex;
	UINT32	dwReserved12;
	mbim_ncm_dpe32 dpe32[0];
} mbim_ncm_ndp32;

/* MBIM parameters */
typedef struct mbim_parameters {
	UINT32	                    header_len;
    UINT32                      fixed_in_len;
    UINT32                      fixed_out_len;
    ndp_parser_opts	            *parser_opts;
    mbim_radio_switch_state     RadioState;
    UINT16                      cdc_filter;
	boolean	                    is_open;
    boolean	                    is_crc;
}mbim_parameters;

/* MBIM NTB input size structure. */
typedef struct mbim_ntb_input_size_type {

	UINT32	dwNtbInMaxSize;             //IN NTB Maximum size in bytes. The host shall select a size that is at least 2048,
	                                    //and no larger than the maximum size permitted by the function,
	                                    //according to the value given in the NTB Parameter Structure

	UINT16  wNtbInMaxDatagrams;         //Maximum number of datagrams within the IN NTB. Zero means no limit.

	UINT16  reserved;                   //Shall be transmitted as zero and ignored upon receipt.

}mbim_ntb_input_size_type;

/* MBIM device caps information. */
typedef struct mbim_device_caps_info
{
    mbim_device_type            DeviceType;
    UINT32                      CellularClass;
    mbim_voice_class_type       VoiceClass;
    UINT32                      SimClass;
    UINT32                      DataClass;
    UINT32                      SmsCaps;
    UINT32                      ControlCaps;
    UINT32                      MaxSessions;
    UINT32                      CustomDataClassOffset;
    UINT32                      CustomDataClassSize;
    UINT32                      DeviceIdOffset;
    UINT32                      DeviceIdSize;
    UINT32                      FirmwareInfoOffset;
    UINT32                      FirmwareInfoSize;
    UINT32                      HardwareInfoOffset;
    UINT32                      HardwareInfoSize;
} mbim_device_caps_info;

/* MBIM subscriber ready information. */
typedef struct mbim_subscriber_ready_info
{
    mbim_subscriber_ready_state ReadyState;
    UINT32                      SubscriberIdOffset;
    UINT32                      SubscriberIdSize;
    UINT32                      SimIccIdOffset;
    UINT32                      SimIccIdSize;
    mbim_unique_id_flags        ReadyInfo;
    UINT32                      ElementCount;
} mbim_subscriber_ready_info;

/* MBIM device's radio power state information. */
typedef struct mbim_radio_state_info
{
    mbim_radio_switch_state     HwRadioState;
    mbim_radio_switch_state     SwRadioState;
} mbim_radio_state_info;

/* MBIM set pin structure */
typedef struct mbim_set_pin
{
    mbim_pin_type               PinType;
    mbim_pin_operation          PinOperation;
    UINT32                      PinOffset;
    UINT32                      PinSize;
    UINT32                      NewPinOffset;
    UINT32                      NewPinSize;
} mbim_set_pin;

/* MBIM PIN infomation structure. */
typedef struct mbim_pin_info
{
    mbim_pin_type               PinType;
    mbim_pin_state              PinState;
    UINT32                      RemainingAttempts;
} mbim_pin_info;

/* mbim_pin_desc structure. */
typedef struct mbim_pin_desc
{
    mbim_pin_mode               PinMode;
    mbim_pin_format             PinFormat;
    UINT32                      PinLengthMin;
    UINT32                      PinLengthMax;
} mbim_pin_desc;

/* MBIM PIN list infomation structure. */
typedef struct mbim_pin_list_info
{
    mbim_pin_desc               PinDescPin1;
    mbim_pin_desc               PinDescPin2;
    mbim_pin_desc               PinDescDeviceSimPin;
    mbim_pin_desc               PinDescDeviceFirstSimPin;
    mbim_pin_desc               PinDescNetworkPin;
    mbim_pin_desc               PinDescNetworkSubsetPin;
    mbim_pin_desc               PinDescServiceProviderPin;
    mbim_pin_desc               PinDescCorporatePin;
    mbim_pin_desc               PinDescSubsidyLock;
    mbim_pin_desc               PinDescCustom;
} mbim_pin_list_info;

/* MBIM provider infomation structure. */
typedef struct mbim_provider
{
    UINT32                      ProviderIdOffset;
    UINT32                      ProviderIdSize;
    mbim_provider_state         ProviderState;
    UINT32                      ProviderNameOffset;
    UINT32                      ProviderNameSize;
    UINT32                      CellularClass;
    UINT32                      Rssi;
    UINT32                      ErrorRate;
} mbim_provider;

/* MBIM visible provider infomation structure. */
typedef struct mbim_visible_providers_req
{
    mbim_visible_provider_action_type   Action;
} mbim_visible_providers_req;

/* MBIM device service element structure. */
typedef struct mbim_device_service_element
{
    mbim_device_service_id              DeviceServiceId;
    UINT32                              DssPayload;
    UINT32                              MaxDssInstances;
    UINT32                              CidCount;
} mbim_device_service_element;

/* MBIM device service information structure. */
typedef struct mbim_device_services_info
{
    UINT32                              DeviceServicesCount;
    UINT32                              MaxDssSessions;
} mbim_device_services_info;

/* MBIM emergency infomation structure. */
typedef struct mbim_emergency_mode_info
{
    mbim_emergency_mode_states          EmergencyMode;
} mbim_emergency_mode_info;

/*
 * This message is sent by the host to begin interacting with the function.  The message is
 * only sent when the device is in the Closed state.
 *
 * The function indicates that it has been successfully opened by responding with MBIM_OPEN_DONE.
 *
 * Right after the function has been successfully opened, the host cannot make any assumptions
 * about the function's current state other than what is implied by the function completing the
 * MBIM_CLOSE_MSG. Once the open message has been successfully completed it is recommended that
 * the host make sure that state-change notifications are enabled for all states the host finds
 * relevant. The host can then query the function's current state for all relevant states. This
 * will ensure that the host and function are synchronized.
 *
 * In case this message is sent to a function that is already opened, the function shall interpret
 * this as that the host and the function are out of synchronization. The function shall then
 * perform the actions dictated by the MBIM_CLOSE_MSG before it performs the actions dictated by
 * this command.
 *
 * The function shall not send the MBIM_CLOSE_DONE when the transition to the Closed state has been
 * completed.  Only the MBIM_OPEN_DONE message is sent upon successful completion of this message.
 *
 */

typedef struct mbim_open_msg_type
{
	mbim_message_hdr	        MessageHeader;          /* A message header as specified in Table 9-1: */
                                                        /* MBIM_MESSAGE_HEADER with MessageType set to MBIM_OPEN_MSG */

	UINT32	                    MaxControlTransfer;     /* Maximum control transfer the host supports. */
	                                                    /* This is the size the host will use for GetEncapsulatedResponse. */

	                                                    /* If UE does not support this, it returns MBIM_ERROR_MAX_TRANSFER  */
	                                                    /* per Table 9-8: MBIM_PROTOCOL_ERROR_CODES. This must not exceed   */
	                                                    /* wMaxControlMessage from Table 6-3: MBIM FUNCTIONAL DESCRIPTOR.   */
} mbim_open_msg_type;

/*
 * The function initializes itself.  Statistics per section 10.5.34 must be reset at this point.
 * Notifications for CIDs defined in this specification are enabled upon OPEN_DONE completion.
 * Vendor extension notifications are off by default.  All filters in section 10.5.30 (see Table
 * 10-142: MBIM_DEVICE_SERVICE_SUBSCRIBE_LIST) and section 10.5.37 (MBIM_CID_IP_PACKET_FILTERS)
 * are to be cleared at this point.
 *
 * The function shall respond to the MBIM_OPEN_MSG message with an MBIM_OPEN_DONE message in which
 * the TransactionId must match the TransactionId in the MBIM_OPEN_MSG.
 *
 * The Status field shall be set to MBIM_STATUS_SUCCESS if the function initialized successfully;
 * otherwise it shall be set to an error code indicating the failure.
 *
 */

typedef struct mbim_open_done_type
{
	mbim_message_hdr	        MessageHeader;          /* A message header as specified in Table 9-1: */
                                                        /* MBIM_MESSAGE_HEADER with MessageType set to MBIM_OPEN_MSG */

	UINT32	                    Status;                 /* MBIM_STATUS_CODES specifies the status of processing the Open request.*/
	                                                    /* See Table 9-15: MBIM_STATUS_CODES. */
} mbim_open_done_type;

/*
 * This message is sent by the host in order to terminate the host's session with the function.
 *
 * Between the host's sending this message and the function's completing this message (acknowledged
 * with MBIM_CLOSE_DONE), the function can expect that no MBIM control messages are sent by the host
 * on the control plane or data on the bulk pipes, and the function shall ignore any such messages.
 *
 * The function shall not send any MBIM control messages on the control plane or data on the bulk
 * pipes after completing this message (acknowledging it with the MBIM_CLOSE_DONE message) with one
 * exception and that is MBIM_ERROR_NOT_OPENED.  MBIM_ERROR_NOT_OPENED shall be sent by the function
 * if the host sends an MBIM_COMMAND_MSG message to the function while the function is in a "Closed"
 * state (e.g.,  prior to MBIM_OPEN_DONE or after an MBIM_CLOSE_DONE).
 *
 * When the function has completed this message and acknowledged it, the only MBIM control message the
 * host may send is the MBIM_OPEN_MSG. Any active context between the function and the host shall be
 * terminated.
 *
 * Note: this is specific to the link between device and host. This message may be sent at any time
 * the device is in the Opened state.
 *
 */

typedef struct mbim_close_msg_type
{
	mbim_message_hdr	        MessageHeader;          /* A message header as specified in Table 9-1:  */
                                                        /* MBIM_MESSAGE_HEADER with MessageType set to MBIM_CLOSE_MSG */
} mbim_close_msg_type;

/*
 * The device will respond to the MBIM_CLOSE_MSG message with an MBIM_CLOSE_DONE message.
 *
 * The TransactionId of the responding message must match the TransactionId in the MBIM_CLOSE_MSG.
 *
 */

typedef struct mbim_close_done_type
{
	mbim_message_hdr	        MessageHeader;          /* A message header as specified in Table 9-1: */
                                                        /* MBIM_MESSAGE_HEADER with MessageType set to MBIM_CLOSE_DONE */

    UINT32                      Status;                 /* MBIM_STATUS_CODES Specifies the status of processing the Close request.*/
                                                        /* This field shall always be set to MBIM_STATUS_SUCCESS. */
                                                        /* See Table 9-15: MBIM_STATUS_CODES */
} mbim_close_done_type;

/*
 * MBIM_COMMAND_MSG is sent to a MBIM device from a host when it needs to query or set state on device.
 *
 * The parameter or statistics being queried for is identified by means of a Command Identifier (CID).
 * Whether the command is setting or querying is determined by the CommandType.
 *
 * The host may send MBIM_COMMAND_MSG to the device via the control channel at any time that the device
 * is in the Opened state.
 *
 * A MBIM device will respond to MBIM_COMMAND_MSG with information about the desired capabilities or
 * status.
 *
 */
typedef struct mbim_command_msg_type
{
    mbim_message_hdr            MessageHeader;          /* A message header as specified in Table 9-1:  */
                                                        /* MBIM_MESSAGE_HEADER with MessageType set to MBIM_COMMAND_MSG */

    mbim_fragment_hdr           FragmentHeader;         /* A fragmentation header as specified in Table 9-2: MBIM_FRAGMENT_HEADER */
                                                        /* For fragmentation considerations, see section 9.5 (Fragmentation of messages). */

    mbim_device_service_id      uuid;                   /* A 16 byte UUID that identifies the device service the following CID value applies */

    UINT32                      CID;                    /* Specifies the CID that identifies the parameter being queried for. */

    UINT32                      Command_type;           /* 0 for a query operation, 1 for a Set operation. */

    UINT32                      InformationBufferLength;/* Length of the InformationBuffer. May exceed the length of this message if fragmented */

} mbim_command_msg_type;


/*
 * The function shall send MBIM_COMMAND_DONE to the host in response to MBIM_COMMAND_MSG. This message is
 * used to relay the result of executing the command, and may also contain additional result values.
 *
 * Table 9-12 defines the structure of the MBIM_COMMAND_DONE message. The TransactionId of the responding
 * message must match the TransactionId in the MBIM_COMMAND_MSG.
 *
 */

typedef struct mbim_command_done_type
{
	mbim_message_hdr            MessageHeader;          /* A message header as specified in Table 9-1:  */
                                                        /* MBIM_MESSAGE_HEADER with MessageType set to MBIM_COMMAND_DONE */

    mbim_fragment_hdr           FragmentHeader;         /* A fragmentation header as specified in Table 9-2: MBIM_FRAGMENT_HEADER */
                                                        /* For fragmentation considerations, see section 9.5 (Fragmentation of messages). */

    mbim_device_service_id      uuid;                   /* A 16 byte UUID that identifies the device service the following CID value applies */

    UINT32                      CID;                    /* Specifies the CID that identifies the parameter being that this message is completing. */

    UINT32                      Status;                 /* Specifies the status of processing the CID set/query request. See section 9.4.5. */

    UINT32                      InformationBufferLength;/* Length of the InformationBuffer. May exceed the length of this message if fragmented */

} mbim_command_done_type;

/*
 * The function may send MBIM_INDICATE_STATUS_MSG to the host via the control channel in an unsolicited
 * fashion at any time that the device has been opened.
 *
 * This message is used to indicate a change in the status of the device. The specific event that is being
 * indicated is identified DeviceServiceId and CID. The contents of the InformationBuffer are specific to
 * the CID..
 *
 */

typedef struct mbim_indicate_status_msg_type
{
	mbim_message_hdr            MessageHeader;          /* A message header as specified in Table 9-1:  */
                                                        /* MBIM_MESSAGE_HEADER with MessageType set to MBIM_CLOSE_MSG */

    mbim_fragment_hdr           FragmentHeader;         /* A fragmentation header as specified in Table 9-2: MBIM_FRAGMENT_HEADER */
                                                        /* For fragmentation considerations, see section 9.5 (Fragmentation of messages). */

    mbim_device_service_id      uuid;                   /* A 16 byte UUID that identifies the device service the following CID value applies */

    UINT32                      CID;                    /* Specifies the CID that identifies the parameter being that this message is completing. */

    UINT32                      InformationBufferLength;/* Length of the InformationBuffer. May exceed the length of this message if fragmented */

} mbim_indicate_status_msg_type;


/*===========================================================================

                        EXTERN FUNCTION DECLARATIONS

===========================================================================*/
/*==========================================================================

FUNCTION pc_netif_status

DESCRIPTION

  Set Netif PC status.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

=============================================================================*/
//extern char pc_netif_status(UINT8 status);

/*===========================================================================

                          INTERNAL FUNCTION DECLARATIONS

===========================================================================*/
#if 0
/* Please read CDC-ECM1.2 P19. */
void mvUsbCh9SetEthernetMulticastFilters( _usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr );



/*===========================================================================

FUNCTION mvUsbCh9SetEthernetPacketFilter

DESCRIPTION
  This request is used to configure device Ethernet packet filter settings.
  The Packet Filter is the inclusive OR of the bitmap shown in Table 8.
  Though network adapters for faster buses (e.g., PCI) may offer other hardware
  filters, the medium speed networking devices (< 10Mbit/s) attached via USB
  are only required to support promiscuous and all multicast modes.  The host
  networking software driver is responsible for performing additional filtering
  as required.

  Note that for some device types, the ability to run in promiscuous mode may
  be severely restricted or prohibited.

  For example, DOCSIS cable modems are only permitted to forward certain frames
  to its attached host.

  Even if forwarding of all frames were allowed, the raw cable modem downstream
  rate available on the RF interface can be many times the maximum USB throughput.

  Bit position  Description
    D15..D5  RESERVED  (Reset to zero)
    D4  PACKET_TYPE_MULTICAST
        1:  All multicast packets enumerated in the device's multicast address
            list are forwarded up to the host. (required)
        0:  Disabled.  The ability to disable forwarding of these multicast
            packets is optional.
    D3  PACKET_TYPE_BROADCAST
        1:  All broadcast packets received by the networking device are forwarded
            up to the host. (required)
        0:  Disabled.  The ability to disable forwarding of broadcast packets is
            optional.
    D2  PACKET_TYPE_DIRECTED
        1:  Directed packets received containing a destination address equal to
            the MAC address of the networking device are forwarded up to the host.
            (required)
        0:  Disabled.  The ability to disable forwarding of directed packets is
            optional. ***
    D1  PACKET_TYPE_ALL_MULTICAST
        1:  ALL multicast frames received by the networking device are forwarded
            up to the host, not just the ones enumerated in the device's multicast
            address list (required)
        0:  Disabled.
    D0  PACKET_TYPE_PROMISCUOUS:
        1:  ALL frames received by the networking device are forwarded up to the
            host (required)
        0: Disabled.

DEPENDENCIES

  -------------------------------------------------------------------------------------------------
 | bmRequestType | bRequestCode |     wValue      |      wIndex        |     wLength      |  Data  |
 | 00100001B     |    0x43      |  Packet Filter  |     Interface      |        0         |  None  |
 |               |              |     Bitmap      |                    |                  |        |
 --------------------------------------------------------------------------------------------------

RETURN VALUE
  return staus

SIDE EFFECTS
  none

===========================================================================*/
void mvUsbCh9SetEthernetPacketFilter(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);

/*=================================================================================================

FUNCTION mvUsbCh9GetNTBParameters

DESCRIPTION
  This request retrieves the parameters that describe NTBs for each direction. In response to this
  request, the function shall return these elements as listed  in Table 6-3.

DEPENDENCIES

 -----------------------------------------------------------------------------------------------
 | bmRequestType | bRequestCode | wValue |      wIndex        |     wLength      |     Data      |
 | 10100001B     |    0x80      |  zero  | NCM Communications |  Number of bytes |  NTB Parameter|
 |               |              |        |   Interface        |     to read      |   Structure   |
 ------------------------------------------------------------------------------------------------

RETURN VALUE
  return staus

SIDE EFFECTS
  none

=====================================================================================================*/

void mvUsbCh9GetNTBParameters(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);

/*=================================================================================================

FUNCTION mvUsbCh9GetNTBInputSize

DESCRIPTION
  This request returns NTB input size currently being used by the function.
  To get the NTB input size, the host should set wLength to at least 4.  To get the full NTB input
  size structure, the host should set wLength to at least 8. If bit D5 is set in field bmNetworkCapabilities
  of the function¡¯s NCM Functional Descriptor, the function shall never return more than 8 bytes in
  response to this command. If bit D5 is reset, the function shall never return more than 4 bytes in
  response to this command. The fields in the input size structure are returned in little-endian order.

DEPENDENCIES

 -----------------------------------------------------------------------------------------------
 | bmRequestType | bRequestCode | wValue |      wIndex        |     wLength      |   The NTB     |
 | 10100001B     |    0x85      |  zero  | NCM Communications |  Number of bytes |    input      |
 |               |              |        |   Interface        |     to read      |   Structure   |
 ------------------------------------------------------------------------------------------------

RETURN VALUE
  return staus

SIDE EFFECTS
  none

=====================================================================================================*/

void mvUsbCh9GetNTBInputSize(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);

/*=================================================================================================

FUNCTION mvUsbCh9SetNTBInputSize

DESCRIPTION
  This request selects the maximum size of NTB that the device is permitted to send to the host,
  and optionally the maximum number of datagrams that the device is permitted to encode into a single
  NTB.

  If bit D5 is set in the bmNetworkCapabilities field of function's NCM Functional Descriptor, the host
  may set wLength either to 4 or to 8. If wLength is 4, the function shall assume that wNtbInMaxDatagrams
  is to be set to zero. If wLength is 8, then the function shall use the provided value as the limit.
  The function shall return an error response (a STALL PID) if wLength is set to any other value.

  If bit D5 is reset in the bmNetworkCapabilities field of the function's NCM Functional Descriptor,
  the host shall set wLength to 4.  The function shall return an error response (a STALL PID) if wLength
  is set to any other value.

  If the value passed in the data phase is not valid, or if wLength is not valid, the function shall
  return an error response (a STALL PID) and shall not change the value it uses for preparing NTBs.

DEPENDENCIES

 -----------------------------------------------------------------------------------------------
 | bmRequestType | bRequestCode | wValue |      wIndex        |     wLength      |    DATA     |
 | 00100001B     |    0x86      |  zero  | NCM Communications |     4 or 8       |             |
 |               |              |        |   Interface        |                  |    Note:    |
 ------------------------------------------------------------------------------------------------

 Note: If wLength is 8, then this is the NTB Input Size Structure. If wLength is 4, then this is the
 dwNtbInMaxSize field of the NTB Input Size Structure.


                   Table 6-4. NTB Input Size Structure
 ------------------------------------------------------------------------------------------------
 |           |                  |               |                |                               |
 |  Offset   |      Field       |       Size    |       Value    |          Description          |
  ------------------------------------------------------------------------------------------------
 |           |                  |               |                |IN NTB Maximum size in bytes.  |
 |           |                  |               |                |The host shall select a size   |
 |           |                  |               |                |that is at least 2048, and no  |
 |     0     |  dwNtbInMaxSize  |       4       |      Number    |larger than the maximum size   |
 |           |                  |               |                |permitted by the function,     |
 |           |                  |               |                |according to the value given   |
 |           |                  |               |                |in the NTB Parameter Structure.|
 |           |                  |               |                |                               |
  -----------------------------------------------------------------------------------------------
 |           |                  |               |                |                               |
 |     4     |wNtbInMaxDatagrams|       2       |      Number    | Maximum number of datagrams   |
 |           |                  |               |                | within the IN NTB.            |
 |           |                  |               |                | Zero means no limit.          |
  ------------------------------------------------------------------------------------------------
 |           |                  |               |                |                               |
 |     6     |  reserved        |       2       |      Number    | Shall be transmitted as zero  |
 |           |                  |               |                | and ignored upon receipt.     |
  ------------------------------------------------------------------------------------------------

RETURN VALUE
  return staus

SIDE EFFECTS
  none

=====================================================================================================*/

void mvUsbCh9SetNTBInputSize(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);

/*=================================================================================================

FUNCTION mvUsbCh9GetNTBFormat

DESCRIPTION
  This request returns the NTB data format currently being used by the function.

  To get the full response, the host should set wLength to at least 2. The function shall never
  return more than 2 bytes in response to this command.

  This command must be supported by the function if it declares support for an NTB size other than
  16bit in bmNtbFormatsSupported.

  If the function does not support NTB sizes other than 16bit, then the host must not issue this
  command to the function.

DEPENDENCIES

 -----------------------------------------------------------------------------------------------
 | bmRequestType | bRequestCode | wValue |      wIndex        |     wLength      |    DATA     |
 | 10100001B     |    0x83      |  zero  | NCM Communications |  Number of bytes |             |
 |               |              |        |   Interface        |    to read       |    Note:    |
 ------------------------------------------------------------------------------------------------

 Note: The NTB format code (2 bytes, little-endian), as defined under wValue in SetNtbFormat (6.2.5).

RETURN VALUE
  return staus

SIDE EFFECTS
  none

=====================================================================================================*/

void mvUsbCh9GetNTBFormat(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);

/*=================================================================================================

FUNCTION mvUsbCh9SetNTBFormat

DESCRIPTION
  This request selects the format of NTB to be used for NTBs transmitted from the function to the host.
  The host must choose one of the available choices from the bmNtbFormatsSupported bitmap element from
  the GetNtbParameters command response (Table 6-3).

  The command format uses the same format, with a single choice selected. The host shall only send this
  command while the NCM Data Interface is in alternate setting 0.

  The function's NTB format setting may be changed by events beyond the scope of this command;
  see sec-tion 7.1 for details.

  If the value passed in wValue is not supported, the function shall return an error response (a STALL PID)
  and shall not change the format it is using to send and receive NTBs.

  This command must be supported by the function if it declares support for an NTB size other than 16bit
  in bmNtbFormatsSupported. If the function does not support NTB sizes other than 16bit, then the host must
  not issue this command to the function.

DEPENDENCIES

 --------------------------------------------------------------------------------------------------------------
 | bmRequestType | bRequestCode |           wValue                  |      wIndex        |  wLength   | DATA  |
 |               |              |      NTB Format Selection:        |                    |            |       |
 |  00100001B    |    0x84      |      0000h: NTB-16                | NCM Communications |   0        | None: |
 |               |              |      0001h: NTB-32                |      Interface     |            |       |
 |               |              | All other values are reserved     |                    |            |       |
 --------------------------------------------------------------------------------------------------------------

RETURN VALUE
  return staus

SIDE EFFECTS
  none

=====================================================================================================*/

void mvUsbCh9SetNTBFormat(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);

/*=================================================================================================

FUNCTION mvUsbCh9GetCRCMode

DESCRIPTION
  This request returns the currently selected CRC mode for NTBs formatted by the function.

  To get the full response, the host should set wLength to at least 2.  The function shall never
  return more than 2 bytes in response to this command.

  Two values are possible.  The function shall return 0000h if CRCs are not being appended to datagrams.
  The function shall return 0001h if CRCs are being appended to datagrams.

DEPENDENCIES

 --------------------------------------------------------------------------------------------------
 | bmRequestType | bRequestCode |    wValue   |      wIndex        |  wLength   |      DATA        |
 |               |              |             |                    |   Number   | The current CRC  |
 |  10100001B    |    0x89      |     Zero    | NCM Communications |of bytes to | mode in little   |
 |               |              |             |      Interface     |    read    | endian order.    |
 |               |              |             |                    |            |  (2 bytes)       |
 ---------------------------------------------------------------------------------------------------

RETURN VALUE
  return staus

SIDE EFFECTS
  none

=====================================================================================================*/
void mvUsbCh9GetCRCMode(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);

/*=================================================================================================

FUNCTION mvUsbCh9SetCRCMode

DESCRIPTION
  This request controls whether the function will append CRCs to datagrams when formatting NTBs to
  be sent to the host. If the value passed in wValue is not valid, the function shall return an error
  response (a STALL PID) and shall not change the CRC mode.

  The function's CRC mode is set to a default value by events outside the scope of this command;
  see section 7.1 for details.

DEPENDENCIES

 --------------------------------------------------------------------------------------------------------------------
 | bmRequestType | bRequestCode |             wValue                 |      wIndex        |  wLength   |   DATA     |
 |               |              |            CRC mode:               |                    |            |            |
 |  00100001B    |    0x8A      | 0000h: CRCs shall not be appended  | NCM Communications |     0      |    None    |
 |               |              | 0001h: CRCs shall be appended      |      Interface     |            |            |
 |               |              | All other values are reserved.     |                    |            |            |
 --------------------------------------------------------------------------------------------------------------------

RETURN VALUE
  return staus

SIDE EFFECTS
  none

=====================================================================================================*/
void mvUsbCh9SetCRCMode(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);

void mvUsbCh9SetNetAddress( _usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr );

void mvUsbCh9GetNetAddress( _usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr );

#endif

/*=================================================================================================

FUNCTION mvUsbMBIMControlMsgParser

DESCRIPTION

  A control message from the host to the device is sent in the payload of a SEND_ENCAPSULATED_COMMAND
  (see 8.1.1 on SendEncapsulatedCommand).

  The function is either in a Closed or Opened global state (as opposed to the Ready States in Table
  10-16: MBIM_SUBSCRIBER_READY_STATE), and these transitions occur as follows:

  (1)A device is initially (upon enumeration) in the Closed state.

  (2)The function transitions from the Closed to the Opened state when it responds to an MBIM_OPEN_MSG
     with an MBIM_OPEN_DONE whose Status field is set to MBIM_ERROR_STATUS_SUCCESS.

  (3)The function transitions from the Opened state to the Closed state when it sends an MBIM_CLOSE_DONE
     to the host.

DEPENDENCIES

  1: MBIM_OPEN_MSG : Initialize the function
  2: MBIM_CLOSE_MSG: Closes the function.
  3: MBIM_COMMAND_MSG: Send a 'COMMAND' CID.
  4: MBIM_HOST_ERROR_MSG: Indicates an error in the MBIM communication.

RETURN VALUE
  none

SIDE EFFECTS
  none

=====================================================================================================*/
void mvUsbMBIMControlMsgParser(UINT8 *buf);

/*=================================================================================================

FUNCTION mvUsbMBIMOpenResponse

DESCRIPTION

  The function initializes itself. Statistics per section 10.5.34 must be reset at this point.
  Notifications for CIDs defined in this specification are enabled upon OPEN_DONE completion.
  Vendor extension notifications are off by default.

  All filters in section 10.5.30 (see Table 10-142: MBIM_DEVICE_SERVICE_SUBSCRIBE_LIST) and section
  10.5.37 (MBIM_CID_IP_PACKET_FILTERS) are to be cleared at this point. The function shall respond
  to the MBIM_OPEN_MSG message with an MBIM_OPEN_DONE message in which the TransactionId must match
  the TransactionId in the MBIM_OPEN_MSG.

  The Status field shall be set to MBIM_STATUS_SUCCESS if the function initialized successfully;
  otherwise it shall be set to an error code indicating the failure.

DEPENDENCIES
  none.

RETURN VALUE
  none.

SIDE EFFECTS
  none

=====================================================================================================*/
void mvUsbMBIMOpenResponse(mbim_open_msg_type *buf);

/*=================================================================================================

FUNCTION mvUsbMBIMCloseResponse

DESCRIPTION

  The device will respond to the MBIM_CLOSE_MSG message with an MBIM_CLOSE_DONE message.
  The TransactionId of the responding message must match the TransactionId in the MBIM_CLOSE_MSG.

DEPENDENCIES
  none.

RETURN VALUE
  none.

SIDE EFFECTS
  none

=====================================================================================================*/
void mvUsbMBIMCloseResponse(mbim_close_msg_type *buf);

/*=================================================================================================

FUNCTION mvUsbMBIMCommandResponse

DESCRIPTION

  The function shall send MBIM_COMMAND_DONE to the host in response to MBIM_COMMAND_MSG.
  This message is used to relay the result of executing the command, and may also contain additional
  result values. Table 9-12 defines the structure of the MBIM_COMMAND_DONE message.

  The TransactionId of the responding message must match the TransactionId in the MBIM_COMMAND_MSG.

DEPENDENCIES
  none.

RETURN VALUE
  none.

SIDE EFFECTS
  none

=====================================================================================================*/
void mvUsbMBIMCommandResponse(mbim_command_msg_type *buf);

/*=================================================================================================

FUNCTION mvUsbMBIMProcessBasicConnectCommand

DESCRIPTION

  The function shall send MBIM_COMMAND_DONE to the host in response to MBIM_COMMAND_MSG.
  This message is used to relay the result of executing the command, and may also contain additional
  result values. Table 9-12 defines the structure of the MBIM_COMMAND_DONE message.

  The TransactionId of the responding message must match the TransactionId in the MBIM_COMMAND_MSG.

DEPENDENCIES
  none.

RETURN VALUE
  none.

SIDE EFFECTS
  none

=====================================================================================================*/
void mvUsbMBIMProcessBasicConnectCommand(mbim_command_msg_type *buf);

/*=================================================================================================

FUNCTION mvUsbMBIMProcessSMSCommand

DESCRIPTION

  The function shall send MBIM_COMMAND_DONE to the host in response to MBIM_COMMAND_MSG.
  This message is used to relay the result of executing the command, and may also contain additional
  result values. Table 9-12 defines the structure of the MBIM_COMMAND_DONE message.

  The TransactionId of the responding message must match the TransactionId in the MBIM_COMMAND_MSG.

DEPENDENCIES
  none.

RETURN VALUE
  none.

SIDE EFFECTS
  none

=====================================================================================================*/
void mvUsbMBIMProcessSMSCommand(mbim_command_msg_type *buf);

/*=================================================================================================

FUNCTION mvUsbMBIMProcessUnsupportedCommand

DESCRIPTION

  The function process unsupported command. The TransactionId and CID of the responding message must
  match the TransactionId and CID in the MBIM_COMMAND_MSG.

DEPENDENCIES
  none.

RETURN VALUE
  none.

SIDE EFFECTS
  none

=====================================================================================================*/
void mvUsbMBIMProcessUnsupportedCommand(mbim_command_msg_type *buf);

/*=================================================================================================

FUNCTION mvUsbNcmResetValues

DESCRIPTION
  Reset MBIM related parameters.

DEPENDENCIES
  void

RETURN VALUE
  return staus

SIDE EFFECTS
  none

=====================================================================================================*/
void mvUsbMBIMResetValues(void);

/*=================================================================================================

FUNCTION mvUsbMBIMSendRspAvailableNotification

DESCRIPTION

  When the MBIM function is ready to send a control message to the host, the function must return a
  RESPONSE_AVAILABLE notification on the Communication Class interface's Interrupt IN endpoint.

  The transfer from the function's interrupt IN endpoint to the host is a standard USB Interrupt IN
  transfer Upon receiving the RESPONSE_AVAILABLE notification, the host reads the control message
  from the Control endpoint using a GET_ENCAPSULATED_RESPONSE transfer, defined in [USBCDC12] chapter
  6.2.2.

  The function must use a separate GET_ENCAPSULATED_RESPONSE transfer for each control message it has
  to send to the host (that is, the function must not concatenate multiple messages into a single
  GET_ENCAPSULATED_RESPONSE transfer).  The function must send a RESPONSE_AVAILABLE notification for
  each available ENCAPSULATED_RESPONSE to be read from the default pipe. For example, if the function
  has 4 ENCAPSULATED_RESPONSES available, it would send 4 RESPONSE_AVAILABLE notifications over the
  interrupt IN pipe.

  Per [USB30] section 8.12.2.2, each RESPONSE_AVAILABLE notification must be Zero Length Packet (ZLP)
  terminated if the length is an exact multiple of the wMaxPacketSize of the Endpoint Descriptor
  (see [USB30], table 9-18) for interrupt endpoint. The ENCAPSULATED_RESPONSE must also be ZLP terminated
  if the size returned is a multiple of the bMaxPacketSize0 and is not equal to wLength in the
  GET_ENCAPSULATED_RESPONSE request (see [USB30]  Table 9-8)..

DEPENDENCIES

--------------------------------------------------------------------------------------------------------------------
| bmRequestType | bRequestCode |             wValue                 |      wIndex        |  wLength   |   DATA     |
|               |              |                                    |                    |            |            |
|  10100001B    |    0x01      |               0                    |      Interface     |     0      |    None    |
|               |              |                                    |                    |            |            |
|               |              |                                    |                    |            |            |
--------------------------------------------------------------------------------------------------------------------

RETURN VALUE
  return staus

SIDE EFFECTS
  none

=====================================================================================================*/

void mvUsbMBIMSendRspAvailableNotification(void);

/*=================================================================================================

FUNCTION mvUsbMBIMNetworkConnectionNotification

DESCRIPTION

  This notification allows the device to notify the host about network connection status.

DEPENDENCIES

--------------------------------------------------------------------------------------------------------------------
| bmRequestType | bRequestCode |             wValue                 |      wIndex        |  wLength   |   DATA     |
|               |              |                                    |                    |            |            |
|  10100001B    |    0x00      |         0 - Disconnect             |                    |     0      |    None    |
|               |              |         1 - Connected              |      Interface     |            |            |
|               |              |                                    |                    |            |            |
--------------------------------------------------------------------------------------------------------------------


RETURN VALUE
  return staus

SIDE EFFECTS
  none

=====================================================================================================*/
void mvUsbMBIMNetworkConnectionNotification(boolean connect);

/*=================================================================================================

FUNCTION mvUsbMBIConnectionSpeedChangeNotification

DESCRIPTION

  This notification allows the device to inform the host-networking driver that a change in either
  the upstream or thedownstream bit rate of the connection has occurred.

  The data phase for this notification contains a data structure with two 32 bit unsigned integers.
  The two values are the upstream bit rate, followed immediately by the downstream bit rate.

  To assure that the host networking driver can always report the correct link speed, the device must
  send this notification immediately after every NETWORK_CONNECTION notification is sent.

  This normally occurs when the physical layer makes or loses a connection, but additionally appears
  implicitly after the device is reset (see discussion in Section3.8.1 Common Data Plane Characteristics).

DEPENDENCIES

--------------------------------------------------------------------------------------------------------------------
| bmRequestType | bRequestCode |             wValue                 |      wIndex        |  wLength   |   DATA     |
|               |              |                                    |                    |            | Connection |
|  10100001B    |    0x2A      |                0                   |      Interface     |     8      |Speed Change|
|               |              |                                    |                    |            |   Data     |
|               |              |                                    |                    |            | Structure  |
--------------------------------------------------------------------------------------------------------------------


RETURN VALUE
  return staus

SIDE EFFECTS
  none

=====================================================================================================*/
void mvUsbMBIConnectionSpeedChangeNotification(void);

/*=================================================================================================

FUNCTION mvUsbMBIMTransmitMsgToUsb

DESCRIPTION

  Transmit USB CDC data to usb.

  endpoint        - USB_CDC_RNDIS_NCM_CRTL_ENDPOINT
  msgPtr          - buffer to transmit
  msgLength       - length of buffer

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

=====================================================================================================*/
void mvUsbMBIMTransmitMsgToUsb(USBDevice_EndpointE endpoint, UINT8* msgPtr, UINT32 msgLength);



//#endif/* MV_USB_MBIM.       */


#endif /* _MVUSBMBIM_H_     */



/*******************************************************************************
*                   End of File
********************************************************************************/



