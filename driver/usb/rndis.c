/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                rndis.c


GENERAL DESCRIPTION

    This file is for rndis usb net.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2011 by Marvell, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who        what, where, why
--------   ------     ----------------------------------------------------------
07/08/2013   zhoujin    Created module
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "rndis.h"
//#include "ECM.h"
#include "platform.h"
#include "utilities.h"
#include "mvUsbDevPrv.h"
#include "mvUsbDevApi.h"
#include "mvUsbMemory.h"
#define CPUartLogPrintf uart_printf
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* RNDIS Dynamic multiple packet flag */
BOOL RndisDynamicMPacket = FALSE;

/* supported OIDs */
const UINT32 oid_supported_list[] =
{
	/* the general stuff */
	OID_GEN_SUPPORTED_LIST,
	OID_GEN_HARDWARE_STATUS,
	OID_GEN_MEDIA_SUPPORTED,
	OID_GEN_MEDIA_IN_USE,
	OID_GEN_MAXIMUM_FRAME_SIZE,
	OID_GEN_LINK_SPEED,
	OID_GEN_TRANSMIT_BLOCK_SIZE,
	OID_GEN_RECEIVE_BLOCK_SIZE,
	OID_GEN_VENDOR_ID,
	OID_GEN_VENDOR_DESCRIPTION,
	OID_GEN_VENDOR_DRIVER_VERSION,
	OID_GEN_CURRENT_PACKET_FILTER,
	OID_GEN_MAXIMUM_TOTAL_SIZE,
	OID_GEN_MEDIA_CONNECT_STATUS,
	OID_GEN_PHYSICAL_MEDIUM,

	/* the statistical stuff */
	OID_GEN_XMIT_OK,
	OID_GEN_RCV_OK,
	OID_GEN_XMIT_ERROR,
	OID_GEN_RCV_ERROR,
	OID_GEN_RCV_NO_BUFFER,

	/* mandatory 802.3 */
	/* the general stuff */
	OID_802_3_PERMANENT_ADDRESS,
	OID_802_3_CURRENT_ADDRESS,
	OID_802_3_MULTICAST_LIST,
	OID_802_3_MAC_OPTIONS,
	OID_802_3_MAXIMUM_LIST_SIZE,

	/* the statistical stuff */
	OID_802_3_RCV_ERROR_ALIGNMENT,
	OID_802_3_XMIT_ONE_COLLISION,
	OID_802_3_XMIT_MORE_COLLISIONS
};

/* RNDIS response available notification message */
UINT8 Rndis_rsp_available_notification[RNDIS_NOTIFICATIN_LENGTH] =
{
    0x01,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
};


/* RNDIS response available notification message */
UINT32 Rndis_Indicate_disconnet_msg[5] =
{
    REMOTE_NDIS_INDICATE_STATUS_MSG,
    20,
    RNDIS_STATUS_MEDIA_DISCONNECT,
    0x00,
    0x00
};

/*===========================================================================

            EXTERN DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* RNDIS multiple packet support flag */
extern BOOL RndisMultiplePacket;

/* Usb net rndis Tx Queue head. */
extern mvUsbNetQ UsbRndisTxQHdr;

/* Usb net mac address */
extern UINT8 UsbNetMacAddress[USB_NET_ETH_ALEN];


#define MIFI_LOG_TRACE(cat1, cat2, cat3, fmt, args...)      \
{                                                           \
}



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
void rndis_free(void *buf)
{
    UINT8 *NextPacket = NULL;
    pmsg *msg = NULL, *msg2 = NULL;
    rndis_packet_msg_type *header = NULL;

    header = (rndis_packet_msg_type *)buf;

    if((header->MessageType == REMOTE_NDIS_PACKET_MSG)&&
       (header->DataOffset == 36))
    {
        if (Rndis_check_mult_packet((UINT8 *)buf, &NextPacket))
        {
            msg2 = (pmsg*)((UINT8*)NextPacket - PMSG_SIZE);
        }

        msg = (pmsg*)((UINT8*)buf - PMSG_SIZE);

        if(msg2 && msg2->free)
        {
            msg2->free(msg2);
        }

        if(msg && msg->free)
        {
            msg->free(msg);
        }
    }
    else
    {
#if 0
        header = (rndis_packet_msg_type *)((UINT8 *)(buf) - RNDIS_HEADER_LEN);

        if(header->MessageType == ECM_TRANSMIT_MSG)
        {
            ecm_free(buf);
        }
        else
#endif
        {
            uart_printf("rndis_free unknown message!!!");
        }
    }
}

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
void RndisMsgParser(UINT8 *buf)
{
    UINT16 i = 0;
	UINT32 MsgType;
	mvUsbNetParams *rndis_param_ptr = mvUsbGetNetParams();

	ASSERT(buf != NULL);

    /* Read buffer to wait for DDR ready*/
    for (i=0; i<0x1000; i++)
    {
        if ((buf[0] != 0)&&(buf[3] == 0))
        {
            break;
        }
    }

    /* Calculate MsgType and MsgLength*/
	MsgType = buf[0] | buf[1] << 8 | buf[2] << 16 | buf[3] << 24;

	/* For USB: responses may take up to 10 seconds */
	switch (MsgType)
	{
	    case REMOTE_NDIS_INITIALIZE_MSG:
	    {
		    Rndis_init_response ((rndis_init_msg_type *) buf);

            MIFI_LOG_TRACE(MIFI, RNDIS, INITIALIZE_MSG, "RNDIS Initialize message");
		    break;
		}

    	case REMOTE_NDIS_HALT_MSG:
    	{
    		rndis_param_ptr->state = USB_NET_UNINITED;
    		mvUsbNetResetRspQ();
    		pc_netif_status(FALSE);

    		MIFI_LOG_TRACE(MIFI, RNDIS, HALT_MSG, "RNDIS Halt message");
    		break;
        }

    	case REMOTE_NDIS_QUERY_MSG:
    	{
    		Rndis_query_response ((rndis_query_msg_type *) buf);
    		break;
		}

    	case REMOTE_NDIS_SET_MSG:
    	{
    		Rndis_set_response ((rndis_set_msg_type *) buf);
    		break;
		}

    	case REMOTE_NDIS_RESET_MSG:
    	{
    		ErrorLogPrintf("R RST");
    		MIFI_LOG_TRACE(MIFI, RNDIS, RESET_MSG, "RNDIS Reset message");
    		mvUsbNetResetRspQ();
    		Rndis_reset_response ((rndis_reset_msg_type *) buf);
    		break;
		}

    	case REMOTE_NDIS_KEEPALIVE_MSG:
    	{
    		/* For USB: host does this every 5 seconds */
    		Rndis_keepalive_response ((rndis_keepalive_msg_type *)buf);
    		break;
	    }

    	default:
    	{
    		MIFI_LOG_TRACE(MIFI, RNDIS, UNKNOWN_MSG, "Unknown Rndis message: %x", MsgType);
    		break;
		}
	}

	return;
}

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
void Rndis_init_response (rndis_init_msg_type *buf)
{
    mvUsbNetQ *pRspQ = NULL;
	rndis_init_cmplt_type *resp = NULL;

	ASSERT(buf != NULL);

	pRspQ = (mvUsbNetQ *)mvUsbAllocate(1);
	USB_ALLOC_MEM_STATUS(pRspQ);

    resp = (rndis_init_cmplt_type*)((UINT32)pRspQ + sizeof(mvUsbNetQ));
	resp->MessageType = REMOTE_NDIS_INITIALIZE_CMPLT;
	resp->MessageLength = 52;
	resp->RequestID = buf->RequestID;
	resp->Status = RNDIS_STATUS_SUCCESS;
	resp->MajorVersion = RNDIS_MAJOR_VERSION;
	resp->MinorVersion = RNDIS_MINOR_VERSION;
	resp->DeviceFlags = RNDIS_DF_CONNECTIONLESS;
	resp->Medium = RNDIS_MEDIUM_802_3;

	if (RndisMultiplePacket)
	{
    	resp->MaxPacketsPerTransfer = 2;
    	resp->MaxTransferSize = (1500 + sizeof(ethhdr) + sizeof(rndis_packet_msg_type) + 22)*5;
    	resp->PacketAlignmentFactor = 3;

        MIFI_LOG_TRACE(MIFI, RNDIS, Rndis_init_response, "RNDIS support multiple packet mode");
	}
	else
	{
    	resp->MaxPacketsPerTransfer = 1;
    	resp->MaxTransferSize = 1500 + sizeof(ethhdr) + sizeof(rndis_packet_msg_type) + 22;
    	resp->PacketAlignmentFactor = 0;

        MIFI_LOG_TRACE(MIFI, RNDIS, Rndis_init_response1, "RNDIS support single packet mode");
	}

	resp->AFListOffset = 0;
	resp->AFListSize = 0;

    pRspQ->len   = resp->MessageLength;
    pRspQ->data  = (UINT8 *)resp;
    pRspQ->resv  = USBNET_RNDIS_RSP_MSG;

    mvUsbNetQPut(&UsbNetRspQHdr, pRspQ);

    /* Send response avaliable notification message */
    RndisSendRspAvailableNotification();

    /* Initialize rndis parameters */
    RndisParamInit();

    /* Set PC netif up */
    pc_netif_status(TRUE);

	return;
}

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
void Rndis_query_response (rndis_query_msg_type *buf)
{
    mvUsbNetQ *pRspQ = NULL;
	rndis_query_cmplt_type *resp = NULL;

    ASSERT(buf != NULL);

	pRspQ = (mvUsbNetQ *)mvUsbAllocate(1);
	USB_ALLOC_MEM_STATUS(pRspQ);

	resp = (rndis_query_cmplt_type *)((UINT32)pRspQ + sizeof(mvUsbNetQ));
	ASSERT(resp != NULL);

	resp->MessageType = REMOTE_NDIS_QUERY_CMPLT;
	resp->RequestID = buf->RequestID;

	if (Rndis_query_gen_resp (buf->OID,
			buf->InformationBufferOffset + 8 + (UINT8 *) buf,
			buf->InformationBufferLength,
			(UINT8 *)resp))
	{
		/* OID not supported */
		resp->Status = RNDIS_STATUS_NOT_SUPPORTED;
		resp->MessageLength = sizeof(rndis_query_cmplt_type);
		resp->InformationBufferLength = 0;
		resp->InformationBufferOffset = 0;
	}
	else
	{
		resp->Status = RNDIS_STATUS_SUCCESS;
	}

    pRspQ->len  = resp->MessageLength;
    pRspQ->data = (UINT8 *)resp;
    pRspQ->resv  = USBNET_RNDIS_RSP_MSG;

    mvUsbNetQPut(&UsbNetRspQHdr, pRspQ);

    /* Send response avaliable notification message */
    RndisSendRspAvailableNotification();

	return;
}

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
void Rndis_set_response (rndis_set_msg_type *buf)
{
    mvUsbNetQ *pRspQ = NULL;
	UINT32	BufLength = 0;
	UINT32  BufOffset = 0;
	rndis_set_cmplt_type *resp = NULL;

    ASSERT(buf != NULL);

	pRspQ = (mvUsbNetQ *)mvUsbAllocate(1);
	USB_ALLOC_MEM_STATUS(pRspQ);

    resp = (rndis_set_cmplt_type *)((UINT32)pRspQ + sizeof(mvUsbNetQ));
	ASSERT(resp != NULL);

	BufLength = buf->InformationBufferLength;
	BufOffset = buf->InformationBufferOffset;

	resp->MessageType = REMOTE_NDIS_SET_CMPLT;
	resp->MessageLength = 16;
	resp->RequestID = buf->RequestID; /* Still LE in msg buffer */

	if (Rndis_set_gen_resp(buf->OID, ((UINT8 *) buf) + 8 + BufOffset, BufLength, resp))
	{
		resp->Status = RNDIS_STATUS_NOT_SUPPORTED;
	}
	else
	{
		resp->Status = RNDIS_STATUS_SUCCESS;
	}

    pRspQ->len  = resp->MessageLength;
    pRspQ->data = (UINT8 *)resp;
    pRspQ->resv  = USBNET_RNDIS_RSP_MSG;

    mvUsbNetQPut(&UsbNetRspQHdr, pRspQ);

    /* Send response avaliable notification message */
    RndisSendRspAvailableNotification();

	return;
}

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
void Rndis_reset_response (rndis_reset_msg_type *buf)
{
    mvUsbNetQ *pRspQ = NULL;
	rndis_reset_cmplt_type	*resp = NULL;

	ASSERT(buf != NULL);

	pRspQ = (mvUsbNetQ *)mvUsbAllocate(1);
	USB_ALLOC_MEM_STATUS(pRspQ);

    resp = (rndis_reset_cmplt_type *)((UINT32)pRspQ + sizeof(mvUsbNetQ));
	ASSERT(resp != NULL);

	resp->MessageType = REMOTE_NDIS_RESET_CMPLT;
	resp->MessageLength = 16;
	resp->Status = RNDIS_STATUS_SUCCESS;


	/* Indicates if addressing information (multicast address list, packet filter)
	   has been lost during the concluded reset operation. If the device needs the
	   host to resend addressing information, it sets this field to 1;
	   Otherwise it sets this field to 0.
	*/
	resp->AddressingReset = 1;

    pRspQ->len  = resp->MessageLength;
    pRspQ->data = (UINT8 *)resp;
    pRspQ->resv  = USBNET_RNDIS_RSP_MSG;

    mvUsbNetQPut(&UsbNetRspQHdr, pRspQ);

    /* Send response avaliable notification message */
    RndisSendRspAvailableNotification();

	return;
}

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
void Rndis_keepalive_response (rndis_keepalive_msg_type *buf)
{
    mvUsbNetQ *pRspQ = NULL;
	rndis_keepalive_cmplt_type	*resp = NULL;

    ASSERT(buf != NULL);

	pRspQ = (mvUsbNetQ *)mvUsbAllocate(1);
	USB_ALLOC_MEM_STATUS(pRspQ);

    resp = (rndis_keepalive_cmplt_type *)((UINT32)pRspQ + sizeof(mvUsbNetQ));
	ASSERT(resp != NULL);

	resp->MessageType = REMOTE_NDIS_KEEPALIVE_CMPLT;
	resp->MessageLength = 16;
	resp->RequestID = buf->RequestID; /* Still LE in msg buffer */
	resp->Status = RNDIS_STATUS_SUCCESS;

    pRspQ->len  = resp->MessageLength;
    pRspQ->data = (UINT8 *)resp;
    pRspQ->resv  = USBNET_RNDIS_RSP_MSG;

    mvUsbNetQPut(&UsbNetRspQHdr, pRspQ);

    /* Send response avaliable notification message */
    RndisSendRspAvailableNotification();

	return;
}

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
UINT32 Rndis_query_gen_resp (UINT32 OID, UINT8 *buf, unsigned len, UINT8 *rsp)
{
	UINT32		 length = 4;	/* usually */
	UINT32		 i = 0;
	UINT32       count= 0;
	UINT32		*outbuf = NULL;
	mvUsbNetParams *rndis_param_ptr = NULL;
	rndis_query_cmplt_type	*resp = (rndis_query_cmplt_type *)rsp;

	rndis_param_ptr = mvUsbGetNetParams();
	ASSERT((resp != NULL)&&(rndis_param_ptr != NULL));

	/* response goes here, right after the header */
	outbuf = (UINT32 *)&resp[1];
	resp->InformationBufferOffset = 16;

	switch (OID)
	{

    	/* general oids (table 4-1) */

    	/* mandatory */
    	case OID_GEN_SUPPORTED_LIST:
    	{
    		length = sizeof(oid_supported_list);

    		count  = length / sizeof(UINT32);
    		for (i = 0; i < count; i++)
    		{
    			outbuf[i] = oid_supported_list[i];
    		}
    		break;
    	}

    	/* mandatory */
    	case OID_GEN_HARDWARE_STATUS:
    	{
    		/* Bogus question!
    		 * Hardware must be ready to receive high level protocols.
    		 * BTW:
    		 * reddite ergo quae sunt Caesaris Caesari
    		 * et quae sunt Dei Deo!
    		 */
            *outbuf = 0;
            break;
    	}

    	/* mandatory */
    	case OID_GEN_MEDIA_SUPPORTED:
    	{
    		*outbuf = rndis_param_ptr->medium;
    		break;
    	}

    	/* mandatory */
    	case OID_GEN_MEDIA_IN_USE:
    	{
    		/* one medium, one transport... (maybe you do it better) */
    		*outbuf = rndis_param_ptr->medium;
    		break;
    	}

    	/* mandatory */
    	case OID_GEN_MAXIMUM_FRAME_SIZE:
    	{
    		*outbuf = 1500;//1400;

    		break;
    	}

    	/* mandatory */
    	case OID_GEN_LINK_SPEED:
    	{
    		if (rndis_param_ptr->media_state == NDIS_MEDIA_STATE_DISCONNECTED)
    		{
			    *outbuf = 0;
			}
		    else
		    {
			    *outbuf = rndis_param_ptr->speed;
			}

    		break;
    	}

    	/* mandatory */
    	case OID_GEN_TRANSMIT_BLOCK_SIZE:
    	{
    		*outbuf = 1514;//1400;
    		break;
    	}

    	/* mandatory */
    	case OID_GEN_RECEIVE_BLOCK_SIZE:
    	{
    		*outbuf = 1514;//1400;
    		break;
    	}

    	/* mandatory */
    	case OID_GEN_VENDOR_ID:
    	{
    		*outbuf = rndis_param_ptr->vendorID;
    		break;
    	}

    	/* mandatory */
    	case OID_GEN_VENDOR_DESCRIPTION:
    	{
    		if(rndis_param_ptr->vendorDescr != NULL)
    		{
    		    length = strlen (rndis_param_ptr->vendorDescr);
		        memcpy (outbuf, rndis_param_ptr->vendorDescr, length);
		    }
    		break;
    	}

    	case OID_GEN_VENDOR_DRIVER_VERSION:
    	{
    		/* Created as LE */
    		*outbuf = 1;
    		break;
    	}

    	/* mandatory */
    	case OID_GEN_CURRENT_PACKET_FILTER:
    	{
		    *outbuf = rndis_param_ptr->filter;
    		break;
    	}

    	/* mandatory */
    	case OID_GEN_MAXIMUM_TOTAL_SIZE:
    	{
    		*outbuf = RNDIS_MAX_TOTAL_SIZE;
    		break;
    	}

    	/* mandatory */
    	case OID_GEN_MEDIA_CONNECT_STATUS:
    	{
    		*outbuf = rndis_param_ptr->media_state;
    		break;
    	}

    	case OID_GEN_PHYSICAL_MEDIUM:
    	{
    		*outbuf = 0;
    		break;
    	}

    	/* The RNDIS specification is incomplete/wrong.   Some versions
    	 * of MS-Windows expect OIDs that aren't specified there.  Other
    	 * versions emit undefined RNDIS messages. DOCUMENT ALL THESE!
    	 */
    	case OID_GEN_MAC_OPTIONS:		/* from WinME */
    	{
    		*outbuf = (NDIS_MAC_OPTION_RECEIVE_SERIALIZED| NDIS_MAC_OPTION_FULL_DUPLEX);
    		break;
    	}

    	/* statistics OIDs (table 4-2) */

    	/* mandatory */
    	case OID_GEN_XMIT_OK:
    	{
            *outbuf = rndis_param_ptr->tx_packets - rndis_param_ptr->tx_errors - rndis_param_ptr->tx_dropped;
    		break;
    	}

    	/* mandatory */
    	case OID_GEN_RCV_OK:
    	{
            *outbuf = rndis_param_ptr->rx_packets - rndis_param_ptr->rx_errors - rndis_param_ptr->rx_dropped;
    		break;
        }
    	/* mandatory */
    	case OID_GEN_XMIT_ERROR:
    	{
            *outbuf = rndis_param_ptr->tx_errors;
    		break;
    	}

    	/* mandatory */
    	case OID_GEN_RCV_ERROR:
    	{
            *outbuf = rndis_param_ptr->rx_errors;
    		break;
    	}

    	/* mandatory */
    	case OID_GEN_RCV_NO_BUFFER:
    	{
            *outbuf = rndis_param_ptr->rx_dropped;
    		break;
    	}

    	/* ieee802.3 OIDs (table 4-3) */

    	/* mandatory */
    	case OID_802_3_PERMANENT_ADDRESS:
    	{
			length = USB_NET_ETH_ALEN;
			memcpy (outbuf, UsbNetMacAddress, length);
    		break;
        }

    	/* mandatory */
    	case OID_802_3_CURRENT_ADDRESS:
    	{
			length = USB_NET_ETH_ALEN;
			memcpy (outbuf, UsbNetMacAddress, length);
    		break;
    	}

    	/* mandatory */
    	case OID_802_3_MULTICAST_LIST:
    	{
    		/* Multicast base address only */
    		*outbuf = 0xE0000000;
    		break;
    	}

    	/* mandatory */
    	case OID_802_3_MAXIMUM_LIST_SIZE:
    	{
    		/* Multicast base address only */
    		*outbuf = 1;
    		break;
    	}

    	case OID_802_3_MAC_OPTIONS:
    	{
    		break;
        }

    	/* ieee802.3 statistics OIDs (table 4-4) */

    	/* mandatory */
    	case OID_802_3_RCV_ERROR_ALIGNMENT:
    	{
            *outbuf = 0;
    		break;
        }
    	/* mandatory */
    	case OID_802_3_XMIT_ONE_COLLISION:
    	{
    		*outbuf = 0;
    		break;
    	}

    	/* mandatory */
    	case OID_802_3_XMIT_MORE_COLLISIONS:
    	{
    		*outbuf = 0;
    		break;
    	}

    	default:
    	{
    		length = 0;
    		break;
        }
    }

	resp->InformationBufferLength = length;
	resp->MessageLength = length + sizeof(rndis_query_cmplt_type);
	return 0;
}

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
UINT32 Rndis_set_gen_resp (UINT32 OID, UINT8 *buf, UINT32 len, rndis_set_cmplt_type *resp)
{
    mvUsbNetParams *rndis_param_ptr = mvUsbGetNetParams();

	ASSERT(resp != NULL)

	switch (OID)
    {
        case OID_GEN_CURRENT_PACKET_FILTER:
        {
            rndis_param_ptr->filter = buf[0]|(buf[1]<<8);
        	break;
    	}

        case OID_802_3_MULTICAST_LIST:
        {
        	/* I think we can ignore this */
        	break;
    	}

        default:
        {
        	ErrorLogPrintf("Set unknown OID 0x%x", OID);
        	break;
    	}
    }

	return 0;
}

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
void Rndis_indicate_status_msg(UINT8 *buf)
{
    UINT32 MsgType;
    mvUsbNetQ *pRspQ = NULL;
	rndis_indicate_status_msg_type	*resp = NULL;
    mvUsbNetParams *rndis_param_ptr = mvUsbGetNetParams();

    ASSERT(buf != NULL);

	MsgType = buf[1] << 8 | buf[2] << 16 | buf[3] << 24;

    ASSERT(MsgType == USBNET_IND_ID);

	if (rndis_param_ptr->state == USB_NET_UNINITED)
	{
		return;
	}

	pRspQ = (mvUsbNetQ *)mvUsbAllocate(1);
	USB_ALLOC_MEM_STATUS(pRspQ);

    resp = (rndis_indicate_status_msg_type *)((UINT32)pRspQ + sizeof(mvUsbNetQ));
	ASSERT(resp != NULL);

    switch(buf[0])
    {
        case 0:
        {
            rndis_param_ptr->media_state = NDIS_MEDIA_STATE_DISCONNECTED;
            resp->Status = RNDIS_STATUS_MEDIA_DISCONNECT;

            mvUsbNetResetRspQ();
            break;
        }

        case 1:
        {
            rndis_param_ptr->media_state = NDIS_MEDIA_STATE_CONNECTED;
            resp->Status = RNDIS_STATUS_MEDIA_CONNECT;
            break;
        }

        default:
        {
            ASSERT(0);
            break;
        }
    }

	resp->MessageType = REMOTE_NDIS_INDICATE_STATUS_MSG;
	resp->MessageLength = 20;
	resp->StatusBufferLength = 0;
	resp->StatusBufferOffset = 0;

    pRspQ->len  = resp->MessageLength;
    pRspQ->data = (UINT8 *)resp;
    pRspQ->resv = USBNET_RNDIS_IND_MSG;

    mvUsbNetQPut(&UsbNetRspQHdr, pRspQ);

    RndisSendRspAvailableNotification();
}

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
void Rndis_eh_indicate_disconnect_msg(void)
{
    mvUsbNetQ *pRspQ = NULL;
	rndis_indicate_status_msg_type	*resp = NULL;
    mvUsbNetParams *rndis_param_ptr = mvUsbGetNetParams();

	pRspQ = (mvUsbNetQ *)mvUsbAllocate(1);
	USB_ALLOC_MEM_STATUS(pRspQ);

    resp = (rndis_indicate_status_msg_type *)((UINT32)pRspQ + sizeof(mvUsbNetQ));
	ASSERT(resp != NULL);

	if (rndis_param_ptr->state == USB_NET_UNINITED)
	{
		return;
	}

    rndis_param_ptr->media_state = NDIS_MEDIA_STATE_DISCONNECTED;
    resp->Status = RNDIS_STATUS_MEDIA_DISCONNECT;
	resp->MessageType = REMOTE_NDIS_INDICATE_STATUS_MSG;
	resp->MessageLength = 20;
	resp->StatusBufferLength = 0;
	resp->StatusBufferOffset = 0;

    pRspQ->len  = resp->MessageLength;
    pRspQ->data = (UINT8 *)resp;
    pRspQ->resv = USBNET_RNDIS_EEH_IND_MSG;

    mvUsbNetQPut(&UsbNetRspQHdr, pRspQ);

    RndisSendRspAvailableNotification();
}

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
void RndisSendRspAvailableNotification(void)
{
    if(_usb_device_get_dtd_cnt(USB_NET_CRTL_ENDPOINT, 1, NULL) > 1)
    {
        _usb_dci_vusb20_free_ep_transfer( USB_NET_CRTL_ENDPOINT, 1, NULL );
        _usb_dci_vusb20_reset_ep_data_toggle(USB_NET_CRTL_ENDPOINT, 1);
    }

	mvUsbNetTransmitToUsb((USBDevice_EndpointE)USB_NET_CRTL_ENDPOINT,
	                Rndis_rsp_available_notification, RNDIS_NOTIFICATIN_LENGTH);

}

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
void RndisParamInit(void)
{
    mvUsbNetParams *rndis_param_ptr   = mvUsbGetNetParams();

    RndisDynamicMPacket = FALSE;
	memset( (void *)rndis_param_ptr, 0, sizeof(mvUsbNetParams) );

	rndis_param_ptr->medium         = NDIS_MEDIUM_802_3;
	rndis_param_ptr->state          = USB_NET_RNDIS_INIT;
	rndis_param_ptr->media_state    = NDIS_MEDIA_STATE_CONNECTED;
	rndis_param_ptr->speed          = mvUsbGetRndisbitrate()/ 100;
}

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
BOOL Rndis_remove_hdr(UINT8 **data, UINT32 *msgLen, UINT32 *dataLen)
{
	UINT8   *pData = NULL;

	/* tmp points to a struct rndis_packet_msg_type */
	rndis_packet_msg_type *packet_ptr = (rndis_packet_msg_type *) (*data);

	/* MessageType should be 0x01*/
	if (REMOTE_NDIS_PACKET_MSG != packet_ptr->MessageType)
	{
	    MIFI_LOG_TRACE(MIFI, RNDIS, Rndis_remove_hdr, "Wrong Rndis Msg Type: %x %d %d",
	                    packet_ptr->MessageType, packet_ptr->MessageLength, (*msgLen));
		return FALSE;
	}

	/* DataOffset */
    if((packet_ptr->DataOffset)%4 != 0)
    {
        MIFI_LOG_TRACE(MIFI, RNDIS, Rndis_remove_hdr1, "Wrong data Offset: %d", packet_ptr->DataOffset);
		return FALSE;
	}

	pData = (UINT8 *)((UINT32)(*data) + packet_ptr->DataOffset + 8);

    *msgLen = packet_ptr->MessageLength;
    *dataLen = packet_ptr->DataLength;
    *data = pData;
	return TRUE;
}

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
void Rndis_add_hdr (UINT8 *data, UINT32 length)
{
	rndis_packet_msg_type *header = NULL;

	ASSERT(data != NULL);

	header = (rndis_packet_msg_type *)data;
	memset(header, 0x00, sizeof(rndis_packet_msg_type));

	header->MessageType   = REMOTE_NDIS_PACKET_MSG;
	header->MessageLength = length;
	header->DataOffset    = 36;
	header->DataLength    = length - sizeof(rndis_packet_msg_type);
}

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
BOOL Rndis_merge_mult_packet(UINT8 *data, UINT8 *NextData, UINT32 *PaddingLen)
{
    UINT32 msgLen = 0, paddLen = 0;
	rndis_packet_msg_type *header = NULL;

    if (!RndisMultiplePacket)
    {
        return FALSE;
    }

    if (!RndisDynamicMPacket)
    {
        if(UsbRndisTxQHdr.cnt >= 400)
        {
            RndisDynamicMPacket = TRUE;
        }
    }
    else
    {
        if(UsbRndisTxQHdr.cnt <= 200)
        {
            RndisDynamicMPacket = FALSE;
        }
    }

    if (!RndisDynamicMPacket)
    {
        return FALSE;
    }

	ASSERT((data != NULL)&&(NextData != NULL));

    if (NextData <= data)
    {
        return FALSE;
    }

    if(mvUsbGetNetType() != USB_NET_RNDIS)
    {
        return FALSE;
    }

    msgLen = NextData - data;
	header = (rndis_packet_msg_type *)data;

    if(msgLen <= header->MessageLength)
    {
        return FALSE;
    }

    paddLen = msgLen - header->MessageLength;

    if((paddLen <= 2048)&&(!(msgLen & 0x07)))
    {
        header->MessageLength = msgLen;
        *PaddingLen = paddLen;
        return TRUE;
    }

    return FALSE;
}

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
BOOL Rndis_check_mult_packet(UINT8 *data, UINT8 **NextData)
{
    rndis_packet_msg_type *header = NULL;

    ASSERT(data != NULL);

    header = (rndis_packet_msg_type *)data;

    if((REMOTE_NDIS_PACKET_MSG == header->MessageType)
        && (header->MessageLength > (header->DataLength + 44)))
    {
    	if(!(header->MessageLength & 0x07))
    	{
            *NextData = data + header->MessageLength;
            return TRUE;
    	}
    }

    return FALSE;
}



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
void Rndis_send_packet(UINT8* data, UINT32 len)
{
    mvUsbNetParams *rndis_param_ptr = mvUsbGetNetParams();

    if((data != NULL)&&(len != 0))
    {
        Rndis_add_hdr(data, len);

        /* Add Header information.      */
        if(rndis_param_ptr->state == USB_NET_UNINITED)
        {
            rndis_free(data);
            return;
        }

        /* Transmit packet data to USB.*/
        mvUsbNetLwipSendPacket(data, len);
    }
    else
    {
        ErrorLogPrintf("%s: data:%x, len:%x", __FUNCTION__, data, len);
    }
}
