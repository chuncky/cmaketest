#ifndef _RNDIS_H_
#define _RNDIS_H_

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

#include "common.h"
//#include "global_types.h"
//#include "diag.h"
//#include "utils.h"
#include "usb_macro.h"
#include "usb_device.h"
#include "mvUsbNet.h"


#define MTU_MAX_SET 1500

#define ETH_PAD_SIZE                   0 /* 2 */
#define PBUF_LINK_HLEN                 (14 + ETH_PAD_SIZE)
#define SIZEOF_ETH_HDR                 (14 + ETH_PAD_SIZE)
#define ETH_HEADER_LEN                 (14 + ETH_PAD_SIZE)
#define RNDIS_HEADER_LEN               44 
#define ECM_HEADER_LEN                 0
#define MBIM_HEADER_LEN                64 
#define PPP_HEADER_LEN                 0 
#define WIFI_HEADER_LEN                108
#define EXTEND_HEADER_LEN              128 /*max set, can adapter to rndis,wifi hedaer*/

/*--- Macros ----------------------------------------------------------------*/
#define RNDIS_HEADER_LEN                                44

#define RNDIS_RX_NUM                                    256
#define RNDIS_TX_NUM                                    64
#define RNDIS_CTRL_NUM                                  16

#define USB_NET_ETH_ALEN	                            6		/* Octets in one ethernet addr	 */
#define RNDIS_MAXIMUM_FRAME_SIZE	                    1518
#define RNDIS_MAX_TOTAL_SIZE		                    1558
#define RNDIS_NOTIFICATIN_LENGTH                        8

/* Remote NDIS Versions */
#define RNDIS_MAJOR_VERSION		                        1
#define RNDIS_MINOR_VERSION		                        0

/* Status Values */
#define RNDIS_STATUS_SUCCESS		                    0x00000000U	/* Success              */
#define RNDIS_STATUS_FAILURE		                    0xC0000001U	/* Unspecified error    */
#define RNDIS_STATUS_INVALID_DATA	                    0xC0010015U	/* Invalid data         */
#define RNDIS_STATUS_NOT_SUPPORTED	                    0xC00000BBU	/* Unsupported request  */
#define RNDIS_STATUS_MEDIA_CONNECT	                    0x4001000BU	/* Device connected     */
#define RNDIS_STATUS_MEDIA_DISCONNECT	                0x4001000CU	/* Device disconnected  */
/* For all not specified status messages:
 * RNDIS_STATUS_Xxx -> NDIS_STATUS_Xxx
 */

/* Message Set for Connectionless (802.3) Devices */
#define REMOTE_NDIS_PACKET_MSG		                    0x00000001U
#define REMOTE_NDIS_INITIALIZE_MSG	                    0x00000002U	/* Initialize device */
#define REMOTE_NDIS_HALT_MSG		                    0x00000003U
#define REMOTE_NDIS_QUERY_MSG		                    0x00000004U
#define REMOTE_NDIS_SET_MSG		                        0x00000005U
#define REMOTE_NDIS_RESET_MSG		                    0x00000006U
#define REMOTE_NDIS_INDICATE_STATUS_MSG	                0x00000007U
#define REMOTE_NDIS_KEEPALIVE_MSG	                    0x00000008U

/* Message completion */
#define REMOTE_NDIS_INITIALIZE_CMPLT	                0x80000002U
#define REMOTE_NDIS_QUERY_CMPLT		                    0x80000004U
#define REMOTE_NDIS_SET_CMPLT		                    0x80000005U
#define REMOTE_NDIS_RESET_CMPLT		                    0x80000006U
#define REMOTE_NDIS_KEEPALIVE_CMPLT	                    0x80000008U

/* Device Flags */
#define RNDIS_DF_CONNECTIONLESS		                    0x00000001U
#define RNDIS_DF_CONNECTION_ORIENTED	                0x00000002U

#define RNDIS_MEDIUM_802_3		                        0x00000000U

/* from drivers/net/sk98lin/h/skgepnmi.h */
#define OID_PNP_CAPABILITIES			                0xFD010100
#define OID_PNP_SET_POWER			                    0xFD010101
#define OID_PNP_QUERY_POWER			                    0xFD010102
#define OID_PNP_ADD_WAKE_UP_PATTERN		                0xFD010103
#define OID_PNP_REMOVE_WAKE_UP_PATTERN	                0xFD010104
#define OID_PNP_ENABLE_WAKE_UP			                0xFD010106

/*Class Req                             */
#define REQ_SEND_ENCAPSULATED_COMMAND                   0x00
#define REQ_GET_ENCAPSULATED_RESPONSE                   0x01
#define REQ_SET_COMM_FEATURE				            0x02
#define REQ_GET_COMM_FEATURE				            0x03
#define REQ_CLEAR_COMM_FEATURE				            0x04
#define REQ_CLASS_SPECIFIC_RESERVED_5		            0x05
#define REQ_SET_AUX_LINE_STATE				            0x10
#define REQ_SET_HOOK_STATE					            0x11
#define REQ_PULSE_SETUP						            0x12
#define REQ_SEND_PULSE						            0x13
#define REQ_SET_PULSE_TIME					            0x14
#define REQ_RING_AUX_JACK					            0x15
#define REQ_SET_LINE_CODING					            0x20
#define REQ_GET_LINE_CODING					            0x21
#define REQ_SET_CONTROL_LINE_STATE			            0x22
#define REQ_SEND_BREAK						            0x23
#define REQ_SET_RINGER_PARMS				            0x30
#define REQ_GET_RINGER_PARMS				            0x31
#define REQ_SET_OPERATION_PARMS				            0x32
#define REQ_GET_OPERATION_PARMS				            0x33
#define REQ_SET_LINE_PARMS					            0x34
#define REQ_GET_MAX_LUN						            0xfe
#define REQ_BOT_MSC_RESET		                        0xff

/* Class-Specific Control Requests (6.2) */
#define REQ_SET_ETHERNET_MULTICAST_FILTERS	            0x40
#define REQ_SET_ETHERNET_PM_PATTERN_FILTER	            0x41
#define REQ_GET_ETHERNET_PM_PATTERN_FILTER	            0x42
#define REQ_SET_ETHERNET_PACKET_FILTER	                0x43
#define REQ_GET_ETHERNET_STATISTIC		                0x44
#define REQ_GET_NTB_PARAMETERS		                    0x80
#define REQ_GET_NET_ADDRESS			                    0x81
#define REQ_SET_NET_ADDRESS			                    0x82
#define REQ_GET_NTB_FORMAT			                    0x83
#define REQ_SET_NTB_FORMAT			                    0x84
#define REQ_GET_NTB_INPUT_SIZE		                    0x85
#define REQ_SET_NTB_INPUT_SIZE		                    0x86
#define REQ_GET_MAX_DATAGRAM_SIZE		                0x87
#define REQ_SET_MAX_DATAGRAM_SIZE		                0x88
#define REQ_GET_CRC_MODE			                    0x89
#define REQ_SET_CRC_MODE			                    0x8a


/* Required Object IDs (OIDs) */
#define OID_GEN_SUPPORTED_LIST                          0x00010101
#define OID_GEN_HARDWARE_STATUS                         0x00010102
#define OID_GEN_MEDIA_SUPPORTED                         0x00010103
#define OID_GEN_MEDIA_IN_USE                            0x00010104
#define OID_GEN_MAXIMUM_LOOKAHEAD                       0x00010105
#define OID_GEN_MAXIMUM_FRAME_SIZE                      0x00010106
#define OID_GEN_LINK_SPEED                              0x00010107
#define OID_GEN_TRANSMIT_BUFFER_SPACE                   0x00010108
#define OID_GEN_RECEIVE_BUFFER_SPACE                    0x00010109
#define OID_GEN_TRANSMIT_BLOCK_SIZE                     0x0001010A
#define OID_GEN_RECEIVE_BLOCK_SIZE                      0x0001010B
#define OID_GEN_VENDOR_ID                               0x0001010C
#define OID_GEN_VENDOR_DESCRIPTION                      0x0001010D
#define OID_GEN_CURRENT_PACKET_FILTER                   0x0001010E
#define OID_GEN_CURRENT_LOOKAHEAD                       0x0001010F
#define OID_GEN_DRIVER_VERSION                          0x00010110
#define OID_GEN_MAXIMUM_TOTAL_SIZE                      0x00010111
#define OID_GEN_PROTOCOL_OPTIONS                        0x00010112
#define OID_GEN_MAC_OPTIONS                             0x00010113
#define OID_GEN_MEDIA_CONNECT_STATUS                    0x00010114
#define OID_GEN_MAXIMUM_SEND_PACKETS                    0x00010115
#define OID_GEN_VENDOR_DRIVER_VERSION                   0x00010116
#define OID_GEN_SUPPORTED_GUIDS                         0x00010117
#define OID_GEN_NETWORK_LAYER_ADDRESSES                 0x00010118
#define OID_GEN_TRANSPORT_HEADER_OFFSET                 0x00010119
#define OID_GEN_MACHINE_NAME                            0x0001021A
#define OID_GEN_RNDIS_CONFIG_PARAMETER                  0x0001021B
#define OID_GEN_VLAN_ID                                 0x0001021C

/* Optional OIDs */
#define OID_GEN_MEDIA_CAPABILITIES                      0x00010201
#define OID_GEN_PHYSICAL_MEDIUM                         0x00010202

/* Required statistics OIDs */
#define OID_GEN_XMIT_OK                                 0x00020101
#define OID_GEN_RCV_OK                                  0x00020102
#define OID_GEN_XMIT_ERROR                              0x00020103
#define OID_GEN_RCV_ERROR                               0x00020104
#define OID_GEN_RCV_NO_BUFFER                           0x00020105

/* Optional statistics OIDs */
#define OID_GEN_DIRECTED_BYTES_XMIT                     0x00020201
#define OID_GEN_DIRECTED_FRAMES_XMIT                    0x00020202
#define OID_GEN_MULTICAST_BYTES_XMIT                    0x00020203
#define OID_GEN_MULTICAST_FRAMES_XMIT                   0x00020204
#define OID_GEN_BROADCAST_BYTES_XMIT                    0x00020205
#define OID_GEN_BROADCAST_FRAMES_XMIT                   0x00020206
#define OID_GEN_DIRECTED_BYTES_RCV                      0x00020207
#define OID_GEN_DIRECTED_FRAMES_RCV                     0x00020208
#define OID_GEN_MULTICAST_BYTES_RCV                     0x00020209
#define OID_GEN_MULTICAST_FRAMES_RCV                    0x0002020A
#define OID_GEN_BROADCAST_BYTES_RCV                     0x0002020B
#define OID_GEN_BROADCAST_FRAMES_RCV                    0x0002020C
#define OID_GEN_RCV_CRC_ERROR                           0x0002020D
#define OID_GEN_TRANSMIT_QUEUE_LENGTH                   0x0002020E
#define OID_GEN_GET_TIME_CAPS                           0x0002020F
#define OID_GEN_GET_NETCARD_TIME                        0x00020210
#define OID_GEN_NETCARD_LOAD                            0x00020211
#define OID_GEN_DEVICE_PROFILE                          0x00020212
#define OID_GEN_INIT_TIME_MS                            0x00020213
#define OID_GEN_RESET_COUNTS                            0x00020214
#define OID_GEN_MEDIA_SENSE_COUNTS                      0x00020215
#define OID_GEN_FRIENDLY_NAME                           0x00020216
#define OID_GEN_MINIPORT_INFO                           0x00020217
#define OID_GEN_RESET_VERIFY_PARAMETERS                 0x00020218

/* IEEE 802.3 (Ethernet) OIDs */
#define NDIS_802_3_MAC_OPTION_PRIORITY                  0x00000001

#define OID_802_3_PERMANENT_ADDRESS                     0x01010101
#define OID_802_3_CURRENT_ADDRESS                       0x01010102
#define OID_802_3_MULTICAST_LIST                        0x01010103
#define OID_802_3_MAXIMUM_LIST_SIZE                     0x01010104
#define OID_802_3_MAC_OPTIONS                           0x01010105
#define OID_802_3_RCV_ERROR_ALIGNMENT                   0x01020101
#define OID_802_3_XMIT_ONE_COLLISION                    0x01020102
#define OID_802_3_XMIT_MORE_COLLISIONS                  0x01020103
#define OID_802_3_XMIT_DEFERRED                         0x01020201
#define OID_802_3_XMIT_MAX_COLLISIONS                   0x01020202
#define OID_802_3_RCV_OVERRUN                           0x01020203
#define OID_802_3_XMIT_UNDERRUN                         0x01020204
#define OID_802_3_XMIT_HEARTBEAT_FAILURE                0x01020205
#define OID_802_3_XMIT_TIMES_CRS_LOST                   0x01020206
#define OID_802_3_XMIT_LATE_COLLISIONS                  0x01020207

/* OID_GEN_MINIPORT_INFO constants */
#define NDIS_MINIPORT_BUS_MASTER                        0x00000001
#define NDIS_MINIPORT_WDM_DRIVER                        0x00000002
#define NDIS_MINIPORT_SG_LIST                           0x00000004
#define NDIS_MINIPORT_SUPPORTS_MEDIA_QUERY              0x00000008
#define NDIS_MINIPORT_INDICATES_PACKETS                 0x00000010
#define NDIS_MINIPORT_IGNORE_PACKET_QUEUE               0x00000020
#define NDIS_MINIPORT_IGNORE_REQUEST_QUEUE              0x00000040
#define NDIS_MINIPORT_IGNORE_TOKEN_RING_ERRORS          0x00000080
#define NDIS_MINIPORT_INTERMEDIATE_DRIVER               0x00000100
#define NDIS_MINIPORT_IS_NDIS_5                         0x00000200
#define NDIS_MINIPORT_IS_CO                             0x00000400
#define NDIS_MINIPORT_DESERIALIZE                       0x00000800
#define NDIS_MINIPORT_REQUIRES_MEDIA_POLLING            0x00001000
#define NDIS_MINIPORT_SUPPORTS_MEDIA_SENSE              0x00002000
#define NDIS_MINIPORT_NETBOOT_CARD                      0x00004000
#define NDIS_MINIPORT_PM_SUPPORTED                      0x00008000
#define NDIS_MINIPORT_SUPPORTS_MAC_ADDRESS_OVERWRITE    0x00010000
#define NDIS_MINIPORT_USES_SAFE_BUFFER_APIS             0x00020000
#define NDIS_MINIPORT_HIDDEN                            0x00040000
#define NDIS_MINIPORT_SWENUM                            0x00080000
#define NDIS_MINIPORT_SURPRISE_REMOVE_OK                0x00100000
#define NDIS_MINIPORT_NO_HALT_ON_SUSPEND                0x00200000
#define NDIS_MINIPORT_HARDWARE_DEVICE                   0x00400000
#define NDIS_MINIPORT_SUPPORTS_CANCEL_SEND_PACKETS      0x00800000
#define NDIS_MINIPORT_64BITS_DMA                        0x01000000

#define NDIS_MEDIUM_802_3		                        0x00000000
#define NDIS_MEDIUM_802_5		                        0x00000001
#define NDIS_MEDIUM_FDDI		                        0x00000002
#define NDIS_MEDIUM_WAN			                        0x00000003
#define NDIS_MEDIUM_LOCAL_TALK		                    0x00000004
#define NDIS_MEDIUM_DIX			                        0x00000005
#define NDIS_MEDIUM_ARCENT_RAW		                    0x00000006
#define NDIS_MEDIUM_ARCENT_878_2	                    0x00000007
#define NDIS_MEDIUM_ATM			                        0x00000008
#define NDIS_MEDIUM_WIRELESS_LAN	                    0x00000009
#define NDIS_MEDIUM_IRDA		                        0x0000000A
#define NDIS_MEDIUM_BPC			                        0x0000000B
#define NDIS_MEDIUM_CO_WAN		                        0x0000000C
#define NDIS_MEDIUM_1394		                        0x0000000D

#define NDIS_PACKET_TYPE_DIRECTED	                    0x00000001
#define NDIS_PACKET_TYPE_MULTICAST	                    0x00000002
#define NDIS_PACKET_TYPE_ALL_MULTICAST	                0x00000004
#define NDIS_PACKET_TYPE_BROADCAST	                    0x00000008
#define NDIS_PACKET_TYPE_SOURCE_ROUTING	                0x00000010
#define NDIS_PACKET_TYPE_PROMISCUOUS	                0x00000020
#define NDIS_PACKET_TYPE_SMT		                    0x00000040
#define NDIS_PACKET_TYPE_ALL_LOCAL	                    0x00000080
#define NDIS_PACKET_TYPE_GROUP		                    0x00000100
#define NDIS_PACKET_TYPE_ALL_FUNCTIONAL	                0x00000200
#define NDIS_PACKET_TYPE_FUNCTIONAL	                    0x00000400
#define NDIS_PACKET_TYPE_MAC_FRAME	                    0x00000800

#define NDIS_MEDIA_STATE_CONNECTED	                    0x00000000
#define NDIS_MEDIA_STATE_DISCONNECTED	                0x00000001

#define NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA             0x00000001
#define NDIS_MAC_OPTION_RECEIVE_SERIALIZED              0x00000002
#define NDIS_MAC_OPTION_TRANSFERS_NOT_PEND              0x00000004
#define NDIS_MAC_OPTION_NO_LOOPBACK                     0x00000008
#define NDIS_MAC_OPTION_FULL_DUPLEX                     0x00000010
#define NDIS_MAC_OPTION_EOTX_INDICATION                 0x00000020
#define NDIS_MAC_OPTION_8021P_PRIORITY                  0x00000040
#define NDIS_MAC_OPTION_RESERVED                        0x80000000

/*Assert definition*/
#define RndisUnalignedAddress(ADDRESS, n)               (ADDRESS%n)
#define RNDISALIGN8(size)                               ((size + 7) & (~7))
#define RNDISALIGN32(size)                              ((size + 31) & (~31))
#define RNDISALIGN64(size)                              ((size + 63) & (~63))

/* memory header size used in pool */
#define RNDIS_MEM_HDR                                   64

/*WIFI_HDR_LEN - RNDIS_HDR_LEN*/
#define RNDIS_PKT_HDR                                   (122-74)

typedef __packed struct
{
	UINT8	    h_dest[USB_NET_ETH_ALEN];	    /* destination eth addr	    */
	UINT8 	    h_source[USB_NET_ETH_ALEN];	    /* source ether addr	    */
	UINT16      h_proto;		                /* packet type ID field	    */
} ethhdr;

#ifndef PBUF_PMSG_STRUCT_DEF
#define PBUF_PMSG_STRUCT_DEF
typedef struct _pmsg pmsg;
#define PMSG_SIZE   (sizeof(pmsg))

typedef void* (*alloc_fn)(size_t size);
typedef void (*free_fn)(pmsg* msg);
struct _pmsg{
    free_fn free;                               /* Free callback function.  */
    void*   phdr;                               /* Buf header.              */
    UINT16  plen;                               /* Buf total length.        */
    UINT16  rnum;                               /* Buf ref num.             */
    UINT16  type;                               /* recorder memory type. */
    UINT16  layer;                              /* recorder memory layer. */
#if 0
    int     pmsgpos;                            /* recorder operator posion for debug */
    UINT32  pmsgtick;                           /* recorder operator ticks when happened */   
#endif
}__attribute__((packed));

typedef struct _mac_pkt_ mac_pkt;
struct _mac_pkt_
{
    UINT8*      buf;
    UINT32      len;
    mac_pkt*    next;
    pmsg*       msg;
}__attribute__((packed));
#endif

typedef struct rndis_init_msg_type
{
	UINT32	MessageType;
	UINT32	MessageLength;
	UINT32	RequestID;
	UINT32	MajorVersion;
	UINT32	MinorVersion;
	UINT32	MaxTransferSize;
} rndis_init_msg_type;

typedef struct rndis_init_cmplt_type
{
	UINT32	MessageType;
	UINT32	MessageLength;
	UINT32	RequestID;
	UINT32	Status;
	UINT32	MajorVersion;
	UINT32	MinorVersion;
	UINT32	DeviceFlags;
	UINT32	Medium;
	UINT32	MaxPacketsPerTransfer;
	UINT32	MaxTransferSize;
	UINT32	PacketAlignmentFactor;
	UINT32	AFListOffset;
	UINT32	AFListSize;
} rndis_init_cmplt_type;

typedef struct rndis_query_msg_type
{
	UINT32	MessageType;
	UINT32	MessageLength;
	UINT32	RequestID;
	UINT32	OID;
	UINT32	InformationBufferLength;
	UINT32	InformationBufferOffset;
	UINT32	DeviceVcHandle;
} rndis_query_msg_type;

typedef struct rndis_query_cmplt_type
{
	UINT32	MessageType;
	UINT32	MessageLength;
	UINT32	RequestID;
	UINT32	Status;
	UINT32	InformationBufferLength;
	UINT32	InformationBufferOffset;
} rndis_query_cmplt_type;

typedef struct rndis_set_msg_type
{
	UINT32	MessageType;
	UINT32	MessageLength;
	UINT32	RequestID;
	UINT32	OID;
	UINT32	InformationBufferLength;
	UINT32	InformationBufferOffset;
	UINT32	DeviceVcHandle;
} rndis_set_msg_type;

typedef struct rndis_set_cmplt_type
{
	UINT32	MessageType;
	UINT32	MessageLength;
	UINT32	RequestID;
	UINT32	Status;
} rndis_set_cmplt_type;

typedef struct rndis_reset_msg_type
{
	UINT32	MessageType;
	UINT32	MessageLength;
	UINT32	Reserved;
} rndis_reset_msg_type;

typedef struct rndis_reset_cmplt_type
{
	UINT32	MessageType;
	UINT32	MessageLength;
	UINT32	Status;
	UINT32	AddressingReset;
} rndis_reset_cmplt_type;

typedef struct rndis_keepalive_msg_type
{
	UINT32	MessageType;
	UINT32	MessageLength;
	UINT32	RequestID;
} rndis_keepalive_msg_type;

typedef struct rndis_keepalive_cmplt_type
{
	UINT32	MessageType;
	UINT32	MessageLength;
	UINT32	RequestID;
	UINT32	Status;
} rndis_keepalive_cmplt_type;

typedef struct rndis_indicate_status_msg_type
{
	UINT32	MessageType;
	UINT32	MessageLength;
	UINT32	Status;
	UINT32	StatusBufferLength;
	UINT32	StatusBufferOffset;
} rndis_indicate_status_msg_type;

typedef __packed struct
{
	UINT32	MessageType;
	UINT32	MessageLength;
	UINT32	DataOffset;
	UINT32	DataLength;
	UINT32	OOBDataOffset;
	UINT32	OOBDataLength;
	UINT32	NumOOBDataElements;
	UINT32	PerPacketInfoOffset;
	UINT32	PerPacketInfoLength;
	UINT32	VcHandle;
	UINT32	Reserved;
} rndis_packet_msg_type;


typedef enum {
  PBUF_TRANSPORT = 0,
  PBUF_IP,
  PBUF_LINK,
  PBUF_RAW,

  /*extend setting for asr*/
  PBUF_RNDIS,
  PBUF_ECM,
  PBUF_WIFI,
  PBUF_MBIM,
  PBUF_PPP,
  PBUF_EXTEND,
  PBUF_PMSG
} pbuf_layer;

typedef enum {
  PBUF_RAM, /* pbuf data is stored in RAM */
  PBUF_ROM, /* pbuf data is stored in ROM */
  PBUF_REF, /* pbuf comes from the pbuf pool */
  PBUF_POOL, /* pbuf payload refers to RAM */
  PBUF_MEM  /* pbuf data is stored in LTE MEM */
} pbuf_type;



/*===========================================================================

                          EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      pc_netif_status                                                  */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function set the status of PC net interface.                */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      status                              Status of PC net interface   */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      status                                N/A                        */
/*                                                                       */
/*************************************************************************/
INT8 pc_netif_status(UINT8 status);
int netif_pc_get_status(void);

/*===========================================================================

                          INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      rndis_free                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function free the rndis tx memory.                           */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      buf                                 Tx buffer                    */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void rndis_free(void *buf);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbGetRndisbitrate                                             */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function get the rndis bitrate.                              */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      bitrate                             Rndis bitrate                */
/*                                                                       */
/*************************************************************************/
UINT32 mvUsbGetRndisbitrate(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      RndisMsgParser                                                   */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function parser all rndis related control message.           */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      buf                                 data buffer                  */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void RndisMsgParser(UINT8 *buf);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Rndis_init_response                                              */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function process REMOTE_NDIS_INITIALIZE_MSG message.         */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      buf                                 data buffer                  */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void Rndis_init_response (rndis_init_msg_type *buf);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Rndis_init_response                                              */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function process REMOTE_NDIS_QUERY_MSG message.              */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      buf                                 data buffer                  */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void Rndis_query_response (rndis_query_msg_type *buf);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Rndis_set_response                                               */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function process REMOTE_NDIS_SET_MSG message.                */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      buf                                 data buffer                  */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void Rndis_set_response (rndis_set_msg_type *buf);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Rndis_reset_response                                             */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function process REMOTE_NDIS_RESET_MSG message.              */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      buf                                 data buffer                  */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void Rndis_reset_response (rndis_reset_msg_type *buf);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Rndis_keepalive_response                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function process REMOTE_NDIS_KEEPALIVE_MSG message.          */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      buf                                 data buffer                  */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void Rndis_keepalive_response (rndis_keepalive_msg_type *buf);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Rndis_keepalive_response                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function process REMOTE_NDIS_KEEPALIVE_MSG message.          */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      OID                                 Rndis OID                    */
/*      buf                                 data buffer                  */
/*      len                                 data length                  */
/*      rsp                                 response pointer             */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
UINT32 Rndis_query_gen_resp (UINT32 OID, UINT8 *buf, unsigned len, UINT8 *rsp);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Rndis_indicate_status_msg                                        */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function send message to PC to indicate the current status.  */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void Rndis_indicate_status_msg(UINT8 *buf);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Rndis_set_gen_resp                                               */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function process REMOTE_NDIS_SET_MSG message.                */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      OID                                 Rndis OID                    */
/*      buf                                 data buffer                  */
/*      len                                 data length                  */
/*      rsp                                 response pointer             */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
UINT32 Rndis_set_gen_resp (UINT32 OID, UINT8 *buf, UINT32 len, rndis_set_cmplt_type *resp);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      RndisSendRspAvailableNotification.                               */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function send response avaliable notification message.       */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void RndisSendRspAvailableNotification(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      RndisParamInit.                                                  */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function initialize rndis parameters.                        */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void RndisParamInit(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Rndis_remove_hdr.                                                */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function remove the rndis head from data packet.             */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      data                                data buffer                  */
/*      msgLen                              message length               */
/*      dataLen                             data length                  */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
BOOL Rndis_remove_hdr(UINT8 **data, UINT32 *msgLen, UINT32 *dataLen);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Rndis_add_hdr.                                                   */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function add the rndis head to data packet.                  */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      data                                data buffer                  */
/*      length                              data length                  */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void Rndis_add_hdr (UINT8 *data, UINT32 length);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Rndis_check_mult_packet                                          */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function check whether it is multiple packet or not.         */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      data                                data pointer                 */
/*      NextData                            next data pointer            */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      status                              return status                */
/*                                                                       */
/*************************************************************************/
BOOL Rndis_check_mult_packet(UINT8 *data, UINT8 **NextData);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Rndis_merge_mult_packet                                          */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function merge multiple rndis packet to one big data packet. */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      data                                data pointer                 */
/*      NextData                            next data pointer            */
/*      PaddingLen                          Padding length               */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      status                              return status                */
/*                                                                       */
/*************************************************************************/
BOOL Rndis_merge_mult_packet(UINT8 *data, UINT8 *NextData, UINT32 *PaddingLen);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Rndis_send_packet.                                               */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function send rndis packet to PC via usb.                    */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      data                                data buffer                  */
/*      length                              data length                  */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void Rndis_send_packet(UINT8* data, UINT32 len);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Rndis_eh_indicate_disconnect_msg                                 */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function send disconnect message to PC after system assert.  */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void Rndis_eh_indicate_disconnect_msg(void);

#endif /* _RNDIS_H_ */
