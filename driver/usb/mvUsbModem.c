/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                mvUsbModem.c


GENERAL DESCRIPTION

    This file is for USB modem.

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
#include "mvUsbModem.h"
#include "mvUsbDevApi.h"
#include "mvUsbDevPrv.h"
//#include "diag_nvm.h"
#include "usb_macro.h"
#include "teldef.h"

#include <stdint.h>

//#include "cmux.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Modem 0 Rx HISR */
OS_HISR Modem0RxHISR;
OS_HISR Modem1RxHISR;

/* Modem uart Rx HISR */
OS_HISR ModemUartRxHISR;

/* Modem 0 Tx HISR */
OS_HISR Modem0TxHISR;
OS_HISR Modem1TxHISR;

/* Modem 0 Tx HISR */
OS_HISR ModemUartTxHISR;


/* Modem 0 Control HISR */
OS_HISR Modem0CtrlHISR;

/* Modem 0 Control HISR */
OS_HISR ModemUartCtrlHISR;

/* Modem 1 Control HISR */
OS_HISR Modem1CtrlHISR;

/* Usb modem 0 control write index*/
UINT8 Modem0CtrlSt_w = 0;

/* Usb modem 0 control read index*/
UINT8 Modem0CtrlSt_r = 0;
UINT8 ModemUartCtrlSt_r = 0;

/* Usb modem 0 control message buffer. */
mvUsbModemMsg mvUsbModem0CtrlSt[mvUsbModemCtrlNum];
mvUsbModemMsg mvUsbModemUartCtrlSt[mvUsbModemCtrlNum];

/* Usb modem 0 Rx write index*/
UINT8 Modem0RxSt_w = 0;
UINT8 Modem1RxSt_w = 0;

UINT8 ModemUartRxSt_w = 0;

/* Usb modem 0 Rx read index*/
UINT8 Modem0RxSt_r = 0;
UINT8 Modem1RxSt_r = 0;

UINT8 ModemUartRxSt_r = 0;

UINT32 ATModemSelect = 1;


/* Usb modem 0 Rx message buffer. */
mvUsbModemMsg mvUsbModem0RxSt[mvUsbModemRxNum];
mvUsbModemMsg mvUsbModem1RxSt[mvUsbModemRxNum];

mvUsbModemMsg mvUsbModemUartRxSt[mvUsbModemRxNum];

/* Usb modem 0 Tx write index*/
UINT8 Modem0TxSt_w = 0;
UINT8 Modem1TxSt_w = 0;

UINT8 ModemUartTxSt_w = 0;

/* Usb modem 0 Tx read index*/
UINT8 Modem0TxSt_r = 0;
UINT8 Modem1TxSt_r = 0;
UINT8 ModemUartTxSt_r = 0;

/* Usb modem 0 Tx message buffer. */
mvUsbModemMsg mvUsbModem0TxSt[mvUsbModemTxNum];
mvUsbModemMsg mvUsbModem1TxSt[mvUsbModemTxNum];

mvUsbModemMsg mvUsbModemUartTxSt[mvUsbModemTxNum];

/* Usb modem 1 control write index*/
UINT8 Modem1CtrlSt_w = 0;

/* Usb modem 1 control read index*/
UINT8 Modem1CtrlSt_r = 0;

/* Usb modem 1 control message buffer. */
mvUsbModemMsg mvUsbModem1CtrlSt[mvUsbModemCtrlNum];

/* Usb modem 0 Rx ring buffer write index*/
UINT32 Modem0RxRingBuffer_W = 0;
UINT32 Modem1RxRingBuffer_W = 0;

UINT32 ModemUartRxRingBuffer_W = 0;

/* Usb modem 0 Rx ring buffer write index*/
UINT32 Modem0RxRingBuffer_R = 0;
UINT32 Modem1RxRingBuffer_R = 0;

UINT32 ModemUartRxRingBuffer_R = 0;

/* Usb modem 0 Tx pending tick */
UINT32 Modem0TxPendingTick = mvUsbInvalidTicks;
UINT32 Modem1TxPendingTick = mvUsbInvalidTicks;

UINT32 ModemUartTxPendingTick = mvUsbInvalidTicks;

#ifdef YMODEM_EEH_DUMP
/* Usb Rx buffer for Ymodem. */
UINT8 usbRxbuffer[10];

/* Usb modem Tx INT for Ymodem. */
UINT8 usbmodemtxint = 0;

/* Usb modem Rx INT for Ymodem. */
UINT8 usbmodemrxint = 0;
#endif
/* The control endpoint */
UINT8 mvUsbATP2CtrlEP[mvUsbATPNum]=
{
    DIAG_CTRL_ENDPOINT,                 /* Modem 0 ---> Diag  */
	MODEM_CTRL_ENDPOINT,                /* Modem 1 ---> AT    */
    USB_NET_CRTL_ENDPOINT               /* Modem 2 ---> Rndis */
};

/* The Rx endpoint */
UINT8 mvUsbATP2RxEP[mvUsbATPNum]=
{
    DIAG_RX_ENDPOINT,                   /* Modem 0 ---> Diag  */
	MODEM_RX_ENDPOINT,                  /* Modem 1 ---> AT    */
    USB_NET_RX_ENDPOINT                 /* Modem 2 ---> Rndis */
};

/* The Tx endpoint */
UINT8 mvUsbATP2TxEP[mvUsbATPNum]=
{
	DIAG_TX_ENDPOINT,                   /* Modem 0 ---> Diag  */
    MODEM_TX_ENDPOINT,                  /* Modem 1 ---> AT    */
    USB_NET_TX_ENDPOINT                 /* Modem 2 ---> Rndis */
};


/* The interface */
UINT8 mvUsbATP2Interface[mvUsbATPNum] =
{
    0x02,                              /* Modem 0 ---> Diag  */
	0x04,                              /* Modem 1 ---> AT    */
    0x00                               /* Modem 2 ---> Rndis */
};

/* Usb ATP open flag */
UINT8 mvUsbATPOpen[NUM_OF_TEL_ATP]={0};




#pragma arm section rwdata="UsbModem", zidata="UsbModem"
/* Comm feature */
UINT8 mvUsbCommFeature[mvUsbATPNum]=
{
    2,
    2,
    2
};

/* Control line state */
UINT16 mvUsbControlLineSt[mvUsbATPNum]=
{
    0,
    0,
    0
};

/* Modem0 serial state notification */
UINT8 ModemComm0SerialStateNotification [mvUsbNotifySerialStateLen] =
{
    /* bmRequestType */
    0xa1,

    /* Serial state */
    0x20,

    /* wValue: 0x00 */
    0x00,
    0x00,

    /* wLndex: Interface */
    0x04,
    0x00,

    /* wLength: 2 */
    0x02,
    0x00,

    /* Uart state bitmap */
    0x02,
    0x00
};

/* Modem1 serial state notification */
UINT8 ModemComm1SerialStateNotification [mvUsbNotifySerialStateLen]=
{
    /* bmRequestType */
    0xa1,

    /* Serial state */
    0x20,

    /* wValue: 0x00 */
    0x00,
    0x00,

    /* wLndex: Interface */
    0x02,
    0x00,

    /* wLength: 2 */
    0x02,
    0x00,

    /* Uart state bitmap */
    0x02,
    0x00
};

UINT8 mvUsbLineCoding[mvUsbATPNum][7]=
{
    {0x00, 0x10, 0x0e, 0x00, 0x00, 0x00, 0x08},
    {0x00, 0x10, 0x0e, 0x00, 0x00, 0x00, 0x08},
    {0x00, 0x10, 0x0e, 0x00, 0x00, 0x00, 0x08}
};

/* Usb modem 0 Rx ring buffer. */
__align(8) UINT8 mvUsbModem0RxRingBuffer[mvUsbModem0MaxRxSize]={0};
__align(8) UINT8 mvUsbModem1RxRingBuffer[mvUsbModem0MaxRxSize]={0};

__align(8) UINT8 mvUsbModemUartRxRingBuffer[mvUsbModem0MaxRxSize]={0};

#pragma arm section rwdata, zidata

//#ifdef SPI_MUX
#if (defined SPI_MUX)&&(defined SPI_MUX_AT)

__align(4) unsigned char SpiATTaskStack[2048];
OSTaskRef		SPIATRxTaskRef;
void receive_AT_SPI_Task(VOID *argv);
#endif

/*===========================================================================

            EXTERN DECLARATIONS FOR MODULE

===========================================================================*/

/* USB Support CDC flag.*/
extern BOOL USBCDCSupport;

/* At command server ready flag.*/
extern UINT8 ATCmdSvrRdy;

/* At command message queue.*/
//extern OSMsgQRef gATMsgQ;

/* CIP to AT point.*/
extern UINT8 gsATP2CID[];

/* At command server handle.*/
//extern CiServiceHandle gAtciSvgHandle[];

/* At command message control.*/
TelAtpCtrl gAtpCtrl[];

#ifdef CMUX_ENABLE
/* Cmux mode type. */
extern cmux_mode_type CmuxMode;

/* Cmux Phy Type.*/
extern cmux_hal_type MuxPhyType;
#endif

/*===========================================================================

            EXTERN DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      M_KiOsGetSignalInt                                               */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function intializes the usb net related message queue and    */
/*      memory pool.                                                     */
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
extern int M_KiOsGetSignalInt(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      USB2GetDTDEntry                                                  */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function get the usb DTD count.                              */
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
extern int USB2GetDTDEntry(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Get_sATP_Mode                                                    */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function get the Modem mode.                                 */
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
extern UINT8 Get_sATP_Mode(UINT8 sATP);

#ifdef PPP_ENABLE
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      send_data_to_modem_router                                        */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function send data the Modem router.                         */
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
void send_data_to_modem_router(char *data, int len, int atp_index, int from);
#endif

#ifdef LWIP_IPNETBUF_SUPPORT
extern BOOL getATRdyPhase1(void);
#endif

/*===========================================================================

                          INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbSetModemParameters                                          */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function set the modem parameters.                           */
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
/*      sATP                                modem point                  */
/*      para                                modem parameters             */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void mvUsbSetModemParameters(UINT8 sATP, mvUsbModemPara *para)
{
    ASSERT((sATP < mvUsbATPNum)&&(para != NULL));

    /* Set Rx endpoint of this sATP */
    mvUsbATP2RxEP[sATP] = para->RxEndPoint;

    /* Set Tx endpoint of this sATP */
    mvUsbATP2TxEP[sATP] = para->TxEndPoint;

    /* Set Control endpoint of this sATP */
    mvUsbATP2CtrlEP[sATP] = para->CtrlEndPoint;

    /* Set interface of this sATP */
    mvUsbATP2Interface[sATP] = para->Interface;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbGetCommFeature                                              */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function get the GetComm Feature.                            */
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
/*      interface                           device interface             */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
UINT8 mvUsbGetCommFeature(UINT32 interface)
{
    UINT8 sATP;
    UINT8 CommFeature = 0;

    for(sATP = 0; sATP < mvUsbATPNum; sATP++)
	{
		if(mvUsbATP2Interface[sATP] == interface)
		{
			break;
		}
	}

    switch(sATP)
	{
		case 0:
		{
			CommFeature = mvUsbCommFeature[0];
			break;
		}

		case 1:
		{
			CommFeature = mvUsbCommFeature[1];
			break;
		}

		default:
		{
			ErrorLogPrintf("%s: Error sATP: %d", sATP);
			ASSERT(0);
			break;
		}
	}

    return CommFeature;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbSetCommFeature                                              */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function Set the Comm Feature.                               */
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
/*      interface                           device interface             */
/*      Feature                             Comm Feature                 */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void mvUsbSetCommFeature(UINT32 interface,UINT8 Feature)
{
    UINT8 sATP;

    for(sATP = 0; sATP < mvUsbATPNum; sATP++)
	{
		if(mvUsbATP2Interface[sATP] == interface)
		{
			break;
		}
	}

    switch(sATP)
	{
		case 0:
		{
			mvUsbCommFeature[0] = Feature;
			break;
		}

		case 1:
		{
			mvUsbCommFeature[1] = Feature;
			break;
		}

		default:
		{
			ErrorLogPrintf("%s: Error sATP: %d", sATP);
			ASSERT(0);
			break;
		}
	}
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbClearCommFeature                                            */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function Clear the Comm Feature.                             */
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
/*      interface                           device interface             */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void mvUsbClearCommFeature(UINT32 interface)
{
    UINT8 sATP;

    for(sATP = 0; sATP < mvUsbATPNum; sATP++)
	{
		if(mvUsbATP2Interface[sATP] == interface)
		{
			break;
		}
	}

    switch(sATP)
	{
		case 0:
		{
			mvUsbCommFeature[0] = 2;
			break;
		}

		case 1:
		{
			mvUsbCommFeature[1] = 2;
			break;
		}

		default:
		{
			ErrorLogPrintf("%s: Error sATP: %d", sATP);
			ASSERT(0);
			break;
		}
	}
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNotifySerialState                                           */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function Clear the Comm Feature.                             */
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
/*      interface                           device interface             */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void mvUsbNotifySerialState(UINT8 interface, UINT8 bmp0, UINT8 bmp1,UINT8 orig)
{
	UINT8 *payload = NULL;
	UINT8 *txPtr = NULL;
	UINT16 pkt_size;
	USBCDevice_ReturnCodeE txStatus;
	UINT32 Tx_EP = 0xFF;
	UINT8 sATP;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    ASSERT(USBCDCSupport);

	for(sATP = 0; sATP < mvUsbATPNum; sATP++)
	{
		if(mvUsbATP2Interface[sATP] == interface)
		{
			break;
		}
	}

	pkt_size = mvUsbNotifySerialStateLen;

	switch(sATP)
	{
		case 0:
		{
			txPtr   = malloc(pkt_size);
			payload = ModemComm0SerialStateNotification;
			Tx_EP   = mvUsbATP2CtrlEP[sATP];
			break;
		}

		case 1:
		{
			txPtr   = malloc(pkt_size);
			payload = ModemComm1SerialStateNotification;
			Tx_EP   = mvUsbATP2CtrlEP[sATP];
			break;
		}

		default:
		{
			ErrorLogPrintf("%s: Error sATP: %d", sATP);
			ASSERT(0);
			break;
		}
	}

	ASSERT((payload != NULL)&&(txPtr != NULL));

    /* Set Interface according to sAtp.*/
    payload[4] = mvUsbATP2Interface[sATP];

    /* Set Uart state bitmap. */
	if(!orig)
	{
		payload[mvUsbNotifySerialStateLen-1] = bmp1;
		payload[mvUsbNotifySerialStateLen-2] = bmp0;
	}

	//if(!ATCmdSvrRdy)
	{
		payload[mvUsbNotifySerialStateLen-1] = 0;
		payload[mvUsbNotifySerialStateLen-2] = 0;
	}

	memcpy(txPtr, payload, pkt_size);

    if(_usb_device_get_dtd_cnt(Tx_EP, 1, NULL) >= 1)
    {
        _usb_dci_vusb20_free_ep_transfer( Tx_EP, 1, free );
    }

	txStatus = USBDeviceEndpointTransmit((USBDevice_EndpointE)Tx_EP,
    									(UINT8 *)txPtr,
    									(UINT16)pkt_size,
    									TRUE);

    switch(txStatus)
    {
        case USB_DEVICE_RC_OK:
        {
            break;
        }

        case USB_DEVICE_RC_NOT_CONNECTED:
        {
            free(txPtr);
            break;
        }

        default:
        {
            ASSERT(0);
            break;
        }
    }
}


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbCommSetLineCoding                                           */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function set Comm line coding.                               */
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
/*      interface                           device interface             */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void mvUsbCommSetLineCoding(UINT8 interface, UINT8* inLineCode)
{
	UINT8 *pLinecode;
	UINT8 sATP;
	UINT8 TxEp;
	UINT8 *tbufPtr;
	UINT8 length;
	USBCDevice_ReturnCodeE txStatus;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    if(!USBCDCSupport)
    {
        ASSERT(0);
    }


	for(sATP = 0; sATP < mvUsbATPNum; sATP++)
	{
		if(mvUsbATP2Interface[sATP] == interface)
		{
			break;
		}
	}

    switch(sATP)
	{
		case 0:
		case 1:
		{
			break;
		}

		default:
		{
			ErrorLogPrintf("%s: Error sATP: %d", sATP);
			ASSERT(0);
			return ;
		}
	}

	pLinecode = mvUsbLineCoding[sATP];

	memcpy(pLinecode, inLineCode, 7);

	if(mvUsbATPOpen[sATP])
	{
		TxEp = mvUsbATP2TxEP[sATP];
		length = 8;
		tbufPtr = malloc(length);

		memset(tbufPtr, 0x00, length);
		txStatus = USBDeviceEndpointTransmit((USBDevice_EndpointE)TxEp,
										 tbufPtr,
										 length,
										 TRUE);

		switch(txStatus)
        {
            case USB_DEVICE_RC_OK:
            {
                break;
            }

            case USB_DEVICE_RC_NOT_CONNECTED:
            {
                free(tbufPtr);
                break;
            }

            default:
            {
                ASSERT(0);
                break;
            }
        }
	}

}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbModemClearDummyFlag                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function clear modem Dummy Flag.                             */
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
void mvUsbModemClearDummyFlag(void)
{
	UINT8 i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

	for(i = 0; i< mvUsbATPNum; i++)
	{
		mvUsbATPOpen[i] = 0;
	}

    Modem0TxPendingTick = mvUsbInvalidTicks;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbCommGetLineCoding                                           */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function get modem line coding.                              */
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
void mvUsbCommGetLineCoding(UINT8 interface, UINT8* outLineCode)
{
	UINT8 *pLinecode;
	UINT8 sATP;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    ASSERT(USBCDCSupport);

	for(sATP = 0; sATP < mvUsbATPNum; sATP++)
	{
		if(mvUsbATP2Interface[sATP] == interface)
		{
			break;
		}
	}

    switch(sATP)
	{
		case 0:
		case 1:
		{
			break;
		}

		default:
		{
			ErrorLogPrintf("%s: Error sATP: %d", sATP);
			ASSERT(0);
			return ;
		}
	}

	pLinecode = mvUsbLineCoding[sATP];

	memcpy(outLineCode, pLinecode, 7);

}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNotifySerialStatereq                                        */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function notify usb serial state.                            */
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
void mvUsbNotifySerialStatereq(UINT8 interface, UINT8 bmp0, UINT8 bmp1,UINT8 orig)
{
	mvUsbModemReq req = MODEM_EP0_SERIALSTATE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


	switch(req)
	{
		case MODEM_EP0_SERIALSTATE:
        {
            mvUsbNotifySerialState(interface, bmp0, bmp1, orig);
            break;
        }

		case MODEM_EP0_SETLINECODE:
		{
			break;
		}

		default:
		{
			break;
		}
	}


}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbCommNotifyControlLineSt                                     */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function notify usb control line state.                      */
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
void mvUsbCommNotifyControlLineSt(UINT8 interface, UINT16 LineStatus)
{
	UINT8 sATP;
	UINT16 oldStatus;
    mvUsbDescriptorInfo *pUsbDesInfo = USB2GetDescriptorInfo();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    ASSERT(USBCDCSupport);

	for(sATP = 0; sATP < mvUsbATPNum; sATP++)
	{
		if(mvUsbATP2Interface[sATP] == interface)
		{
			break;
		}
	}

    switch(sATP)
	{
		case 0:
		case 1:
		{
			break;
		}

		default:
		{
			ErrorLogPrintf("%s: Error sATP: %d", sATP);
			ASSERT(0);
			return ;
		}
	}


	oldStatus = mvUsbControlLineSt[sATP];
	mvUsbControlLineSt[sATP] = LineStatus;

    /* Send notification when the DTE is present */
	if((LineStatus & 3)==3 && (oldStatus & 3) != (LineStatus & 3))
	{
		mvUsbNotifySerialStatereq(interface, 0x02, 0x00,0);
	}
	else
	{
		if((LineStatus & 3)==0 && (oldStatus & 3) != (LineStatus & 3))
		{
			mvUsbNotifySerialStatereq(interface, 0x00, 0x00,0);
		}
	}

#if 0
	switch(pUsbDesInfo->CurrentDesc)
	{
        case USB_SSG_MIFI_DESCRIPTOR:
        {
            if (LineStatus & 3)
            {
                mvUsbATPOpen[sATP] = 1;
            }
            else
            {
                mvUsbATPOpen[sATP] = 0;
            }
            break;
        }

        default:
        {
            break;
        }
	}
#endif
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbModemInitialize                                             */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function initialize usb modem device.                        */
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
void mvUsbModemInitialize(void)
{
	USBCDevice_ReturnCodeE usbStatus;
//#ifdef SPI_MUX
#if (defined SPI_MUX)&&(defined SPI_MUX_AT)
	OS_STATUS   status;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	status = OSATaskCreate(&SPIATRxTaskRef, SpiATTaskStack, 2048, 60, "SpiATRx_T", receive_AT_SPI_Task, NULL);
	ASSERT(status == OS_SUCCESS);
#endif
    /* Modem HISR initialize */
	OS_Create_HISR(&Modem0RxHISR, "Mdm0RxH", mvUsbModem0RxHISR, HISR_PRIORITY_2);
	OS_Create_HISR(&Modem0TxHISR, "Mdm0TxH", mvUsbModem0TxHISR, HISR_PRIORITY_2);
	OS_Create_HISR(&Modem1RxHISR, "Mdm0RxH", mvUsbModem1RxHISR, HISR_PRIORITY_2);
	OS_Create_HISR(&Modem1TxHISR, "Mdm0TxH", mvUsbModem1TxHISR, HISR_PRIORITY_2);
	
	OS_Create_HISR(&Modem0CtrlHISR, "Mdm0CtH", mvUsbModem0CtrlHISR, HISR_PRIORITY_2);

	/* Diag contrl HISR initialize */
	OS_Create_HISR(&Modem1CtrlHISR, "Mdm1CtH", mvUsbModem1CtrlHISR, HISR_PRIORITY_2);

    /* Modem Endpiont initialize */
	usbStatus = USBDeviceEndpointOpen((USBDevice_EndpointE)mvUsbATP2CtrlEP[AT_MODEM_CHANNLE],
			(USBDevice_UsageTypeE)(USB_DEVICE_USAGE_INTERRUPT | USB_DEVICE_NEED_ZLP),
			0,
			(UINT8 *)NULL,
			0,
			mvUsbModem0CtrlLISR);

	ASSERT(usbStatus == USB_DEVICE_RC_OK);

	usbStatus = USBDeviceEndpointOpen((USBDevice_EndpointE)mvUsbATP2TxEP[AT_MODEM_CHANNLE],
			(USBDevice_UsageTypeE)(USB_DEVICE_USAGE_INTERRUPT | USB_DEVICE_NEED_ZLP),
			0,
			(UINT8 *)NULL,
			0,
			mvUsbModem0TxLISR);

	ASSERT(usbStatus == USB_DEVICE_RC_OK);

	usbStatus = USBDeviceEndpointOpen((USBDevice_EndpointE)mvUsbATP2RxEP[AT_MODEM_CHANNLE],
			USB_DEVICE_USAGE_INTERRUPT,
			0,
			(UINT8 *)NULL,
			0,
			mvUsbModem0RxLISR);
	ASSERT(usbStatus == USB_DEVICE_RC_OK);

	usbStatus = USBDeviceEndpointReceive((USBDevice_EndpointE)mvUsbATP2RxEP[AT_MODEM_CHANNLE],
        	(UINT8*)(mvUsbModem0RxRingBuffer),
        	mvUsbModemRxSize,
        	TRUE);
	ASSERT(usbStatus == USB_DEVICE_RC_OK);

#ifndef PLAT_USE_ALIOS	
	usbStatus = USBDeviceEndpointOpen((USBDevice_EndpointE)MODEM_1_TX_ENDPOINT,
			(USBDevice_UsageTypeE)(USB_DEVICE_USAGE_INTERRUPT | USB_DEVICE_NEED_ZLP),
			0,
			(UINT8 *)NULL,
			0,
			mvUsbModem1TxLISR);

	ASSERT(usbStatus == USB_DEVICE_RC_OK);

	usbStatus = USBDeviceEndpointOpen((USBDevice_EndpointE)MODEM_1_RX_ENDPOINT,
			USB_DEVICE_USAGE_INTERRUPT,
			0,
			(UINT8 *)NULL,
			0,
			mvUsbModem1RxLISR);
	ASSERT(usbStatus == USB_DEVICE_RC_OK);

	usbStatus = USBDeviceEndpointReceive((USBDevice_EndpointE)MODEM_1_RX_ENDPOINT,
        	(UINT8*)(mvUsbModem1RxRingBuffer),
        	mvUsbModemRxSize,
        	TRUE);
	ASSERT(usbStatus == USB_DEVICE_RC_OK);
#endif

    /* Diag endpoint initialize */
	usbStatus = USBDeviceEndpointOpen((USBDevice_EndpointE)mvUsbATP2CtrlEP[0],
    		(USBDevice_UsageTypeE)(USB_DEVICE_USAGE_INTERRUPT | USB_DEVICE_NEED_ZLP),
			0,
			(UINT8 *)NULL,
			0,
			mvUsbModem1CtrlLISR);

	ASSERT(usbStatus == USB_DEVICE_RC_OK);

    uart_printf("mvUsbModemInitialize complete.");
}
void mvUsbModemUartInitialize(void)
{
	USBCDevice_ReturnCodeE usbStatus;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Modem HISR initialize */
	OS_Create_HISR(&ModemUartRxHISR, "Mdm0RxH", mvUsbModemUartRxHISR, HISR_PRIORITY_2);
	OS_Create_HISR(&ModemUartTxHISR, "Mdm0TxH", mvUsbModemUartTxHISR, HISR_PRIORITY_2);
	OS_Create_HISR(&ModemUartCtrlHISR, "Mdm0CtH", mvUsbModemUartCtrlHISR, HISR_PRIORITY_2);

	usbStatus = USBDeviceEndpointOpen(USB_UART_TX_ENDPOINT,
			(USBDevice_UsageTypeE)(USB_DEVICE_USAGE_INTERRUPT | USB_DEVICE_NEED_ZLP),
			0,
			(UINT8 *)NULL,
			0,
			mvUsbModemUartTxLISR);

	ASSERT(usbStatus == USB_DEVICE_RC_OK);
	
	usbStatus = USBDeviceEndpointOpen(USB_UART_RX_ENDPOINT,
			USB_DEVICE_USAGE_INTERRUPT,
			0,
			(UINT8 *)NULL,
			0,
			mvUsbModemUartRxLISR);
	ASSERT(usbStatus == USB_DEVICE_RC_OK);

	usbStatus = USBDeviceEndpointReceive(USB_UART_RX_ENDPOINT,
			(UINT8*)(mvUsbModemUartRxRingBuffer),
			mvUsbModemRxSize,
			TRUE);
	ASSERT(usbStatus == USB_DEVICE_RC_OK);

    uart_printf("mvUsbModemUartInitialize complete.");
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbModem0CtrlHISR                                              */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The modem 0 control High-Level Interrupt Service Routine(HISR).  */
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
void mvUsbModem0CtrlHISR(void)
{
	UINT8 *TxPtr;
	UINT32 length;
	UINT8 TxNum = Modem0CtrlSt_r++;

	if(Modem0CtrlSt_r >= mvUsbModemCtrlNum)
	{
		Modem0CtrlSt_r=0;
	}

	TxPtr  = mvUsbModem0CtrlSt[TxNum].ptr;
	length = mvUsbModem0CtrlSt[TxNum].len;

	/* Free the Tx memory. */
	if(TxPtr)
	{
		free(TxPtr);
	}
	else
	{
		ASSERT(length != 0);
	}
}
void mvUsbModemUartCtrlHISR(void)
{
	UINT8 *TxPtr;
	UINT32 length;
	UINT8 TxNum = ModemUartCtrlSt_r++;

	if(ModemUartCtrlSt_r >= mvUsbModemCtrlNum)
	{
		ModemUartCtrlSt_r=0;
	}

	TxPtr  = mvUsbModemUartCtrlSt[TxNum].ptr;
	length = mvUsbModemUartCtrlSt[TxNum].len;

	/* Free the Tx memory. */
	if(TxPtr)
	{
		free(TxPtr);
	}
	else
	{
		ASSERT(length != 0);
	}
}


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbModem0CtrlLISR                                              */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The modem 0 control Low-Level Interrupt Service Routine(LISR).   */
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
void mvUsbModem0CtrlLISR(USBDevice_EndpointE ep ,UINT8 *data ,UINT32 Len ,BOOL dummy)
{
	STATUS osStatus;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

	mvUsbModem0CtrlSt[Modem0CtrlSt_w].Ep   = ep;
	mvUsbModem0CtrlSt[Modem0CtrlSt_w].ptr  = data;
	mvUsbModem0CtrlSt[Modem0CtrlSt_w].len  = Len;

	Modem0CtrlSt_w++;

	if(Modem0CtrlSt_w >= mvUsbModemCtrlNum)
	{
		Modem0CtrlSt_w=0;
	}

	ASSERT(Modem0CtrlSt_w != Modem0CtrlSt_r);

	osStatus = OS_Activate_HISR(&Modem0CtrlHISR);
	ASSERT( osStatus == OS_SUCCESS ) ;

}


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbModem0RxHISR                                                */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The modem 0 Rx High-Level Interrupt Service Routine(HISR).       */
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
void mvUsbModem0RxHISR(void)
{
    UINT8 *RxPtr = NULL;
    UINT32 length;
    UINT8  rxNum = Modem0RxSt_r++;
    UINT8  sATPMode;
    OSA_STATUS   osa_status;
    UINT32 txLength;
    char *buf_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    if(Modem0RxSt_r >= mvUsbModemRxNum)
    {
        Modem0RxSt_r = 0;
    }

    RxPtr  = mvUsbModem0RxSt[rxNum].ptr;
    length = mvUsbModem0RxSt[rxNum].len;

    Modem0RxRingBuffer_W += mvUsbModemRxSize;
    if(Modem0RxRingBuffer_W >= mvUsbModem0RxNum*mvUsbModemRxSize)
    {
        Modem0RxRingBuffer_W=0;
    }

    ASSERT(Modem0RxRingBuffer_W != Modem0RxRingBuffer_R);    

#ifdef COM_STREAM_ENABLE

	if(get_com_stream_flag()==1)
	{
	
		com_store_rx_data(RxPtr, length);

        Modem0RxRingBuffer_R += mvUsbModemRxSize;

        if(Modem0RxRingBuffer_R >= mvUsbModem0RxNum*mvUsbModemRxSize)
        {
            Modem0RxRingBuffer_R=0;
        }

        USBDeviceEndpointReceiveCompletedExt((USBDevice_EndpointE)(mvUsbModem0RxSt[rxNum].Ep),
                                                mvUsbModem0RxRingBuffer+Modem0RxRingBuffer_W,
                                                mvUsbModemRxSize,
                                                TRUE);

		ComStreamRxTrigger();
		
        return;


	}

#endif	

#ifdef CMUX_ENABLE
    /* Cmux USB mode enable. */
    if(CmuxMode == CMUX_USB_ENABLE)
    {
        cmux_hal_rx_msg(RxPtr, length);

        Modem0RxRingBuffer_R += mvUsbModemRxSize;

        if(Modem0RxRingBuffer_R >= mvUsbModem0RxNum*mvUsbModemRxSize)
        {
            Modem0RxRingBuffer_R=0;
        }

        USBDeviceEndpointReceiveCompletedExt((USBDevice_EndpointE)(mvUsbModem0RxSt[rxNum].Ep),
                                                mvUsbModem0RxRingBuffer+Modem0RxRingBuffer_W,
                                                mvUsbModemRxSize,
                                                TRUE);
        return;
    }
#endif

#ifdef YMODEM_EEH_DUMP
	if (useYmodem() == 1 && SysIsAssert() == 1)
	{
		memcpy(usbRxbuffer, RxPtr, (length>10?10:length));
 		Modem0RxRingBuffer_R += mvUsbModemRxSize;

        if(Modem0RxRingBuffer_R >= mvUsbModem0RxNum*mvUsbModemRxSize)
        {
            Modem0RxRingBuffer_R=0;
        }

        USBDeviceEndpointReceiveCompletedExt((USBDevice_EndpointE)(mvUsbModem0RxSt[rxNum].Ep),
                                                mvUsbModem0RxRingBuffer+Modem0RxRingBuffer_W,
                                                mvUsbModemRxSize,
                                                TRUE);
		return;
	}
#endif

    //Get the sATP mode to decide where to send the buffer
    if(!getATRdyPhase1())
    {
        Modem0RxRingBuffer_R += mvUsbModemRxSize;

        if(Modem0RxRingBuffer_R >= mvUsbModem0RxNum*mvUsbModemRxSize)
        {
            Modem0RxRingBuffer_R=0;
        }

        USBDeviceEndpointReceiveCompletedExt((USBDevice_EndpointE)(mvUsbModem0RxSt[rxNum].Ep),
                                                mvUsbModem0RxRingBuffer+Modem0RxRingBuffer_W,
                                                mvUsbModemRxSize,
                                                TRUE);
        return;
    }


    while(length)
    {
        if (length > mvUsbModemRxSize)
        {
            txLength = mvUsbModemRxSize;
        }
        else
        {
            txLength = length;
        }

#ifdef CMUX_ENABLE
        MuxPhyType = MUX_USB;
#endif        
        buf_ptr = (char *)malloc(txLength);
        ASSERT(buf_ptr != NULL);

        memcpy(buf_ptr, RxPtr, txLength);

        sATPMode = Get_sATP_Mode(AT_MODEM_CHANNLE);

        if(sATPMode == MODEM_CONTROL_MODE)
    	{
#ifndef PPP_ENABLE
            ATParserMsg atMsg;

            atMsg.data = (char *)buf_ptr;
            atMsg.length = txLength;

            atMsg.sATPInd = AT_MODEM_CHANNLE;

            if(!gAtpCtrl[(TelAtParserID)AT_MODEM_CHANNLE].bEnable)
            {
                uart_printf("%s: openAtChannel 0\n", __FUNCTION__);
		        openAtChannel((TelAtParserID)AT_MODEM_CHANNLE);
		    }

            uart_printf("sATP%d->AtChanThread\n", atMsg.sATPInd);

            //osa_status = OSAMsgQSend(gATMsgQ, sizeof(atMsg), (UINT8*)&atMsg, OSA_NO_SUSPEND);
            //ASSERT(osa_status == OS_SUCCESS);
#else
            send_data_to_modem_router(buf_ptr, txLength, AT_MODEM_CHANNLE, 0);
#endif
    	}
    	else
    	{
			sendData2SACReq(gsATP2CID[AT_MODEM_CHANNLE], buf_ptr, txLength);

    	}

        length -= txLength;
        RxPtr  += txLength;
    }

    Modem0RxRingBuffer_R+=mvUsbModemRxSize;

	if(Modem0RxRingBuffer_R >= mvUsbModem0RxNum*mvUsbModemRxSize)
	{
		Modem0RxRingBuffer_R=0;
	}


    USBDeviceEndpointReceiveCompletedExt((USBDevice_EndpointE)(mvUsbModem0RxSt[rxNum].Ep),
                                            mvUsbModem0RxRingBuffer+Modem0RxRingBuffer_W,
                                            mvUsbModemRxSize,
                                            TRUE);
	/*mischecked by coverity*/
	/*coverity[leaked_storage]*/

}

void mvUsbModemUartRxHISR(void)
{
    UINT8 *RxPtr = NULL;
    UINT32 length;
    UINT8  rxNum = ModemUartRxSt_r++;
    OSA_STATUS   osa_status;
    UINT32 txLength;
    char *buf_ptr = NULL;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    if(ModemUartRxSt_r >= mvUsbModemRxNum)
    {
        ModemUartRxSt_r = 0;
    }

    RxPtr  = mvUsbModemUartRxSt[rxNum].ptr;
    length = mvUsbModemUartRxSt[rxNum].len;

    ModemUartRxRingBuffer_W += mvUsbModemRxSize;
    if(ModemUartRxRingBuffer_W >= mvUsbModem0RxNum*mvUsbModemRxSize)
    {
        ModemUartRxRingBuffer_W=0;
    }

    ASSERT(ModemUartRxRingBuffer_W != ModemUartRxRingBuffer_R);

    //Get the sATP mode to decide where to send the buffe    

    while(length)
    {
        if (length > mvUsbModemRxSize)
        {
            txLength = mvUsbModemRxSize;
        }
        else
        {
            txLength = length;
        }

#if 0
        buf_ptr = (char *)malloc(txLength);
        ASSERT(buf_ptr != NULL);

        memcpy(buf_ptr, RxPtr, txLength);

		usb_uart_printf(buf_ptr,txLength);
#else
		extern void usb_uart_rx_data_in(uint8_t* data, uint32_t size);
		usb_uart_rx_data_in(RxPtr, txLength);
#endif
        length -= txLength;
        RxPtr  += txLength;
    }

    ModemUartRxRingBuffer_R+=mvUsbModemRxSize;

	if(ModemUartRxRingBuffer_R >= mvUsbModem0RxNum*mvUsbModemRxSize)
	{
		ModemUartRxRingBuffer_R=0;
	}

    USBDeviceEndpointReceiveCompletedExt(USB_UART_RX_ENDPOINT,
                                            mvUsbModemUartRxRingBuffer+ModemUartRxRingBuffer_W,
                                            mvUsbModemRxSize,
                                            TRUE);

}

void mvUsbModem1RxHISR(void)
{
    UINT8 *RxPtr = NULL;
    UINT32 length;
    UINT8  rxNum = Modem1RxSt_r++;
    UINT8  sATPMode;
    OSA_STATUS   osa_status;
    UINT32 txLength;
    char *buf_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    if(Modem1RxSt_r >= mvUsbModemRxNum)
    {
        Modem1RxSt_r = 0;
    }

    RxPtr  = mvUsbModem1RxSt[rxNum].ptr;
    length = mvUsbModem1RxSt[rxNum].len;

    Modem1RxRingBuffer_W += mvUsbModemRxSize;
    if(Modem1RxRingBuffer_W >= mvUsbModem0RxNum*mvUsbModemRxSize)
    {
        Modem1RxRingBuffer_W=0;
    }

    ASSERT(Modem1RxRingBuffer_W != Modem1RxRingBuffer_R);

    /* Cmux USB mode enable. */

    //Get the sATP mode to decide where to send the buffer
    if(!getATRdyPhase1())
    {
        Modem1RxRingBuffer_R += mvUsbModemRxSize;

        if(Modem1RxRingBuffer_R >= mvUsbModem0RxNum*mvUsbModemRxSize)
        {
            Modem1RxRingBuffer_R=0;
        }

        USBDeviceEndpointReceiveCompletedExt((USBDevice_EndpointE)MODEM_1_RX_ENDPOINT,
                                                mvUsbModem1RxRingBuffer+Modem1RxRingBuffer_W,
                                                mvUsbModemRxSize,
                                                TRUE);
        return;
    }


    while(length)
    {
        if (length > mvUsbModemRxSize)
        {
            txLength = mvUsbModemRxSize;
        }
        else
        {
            txLength = length;
        }

        buf_ptr = (char *)malloc(txLength);
        ASSERT(buf_ptr != NULL);

        memcpy(buf_ptr, RxPtr, txLength);

        sATPMode = Get_sATP_Mode(AT_MODEM_CHANNLE_1);

        if(sATPMode == MODEM_CONTROL_MODE)
    	{
            ATParserMsg atMsg;

            atMsg.data = (char *)buf_ptr;
            atMsg.length = txLength;
            atMsg.sATPInd = AT_MODEM_CHANNLE_1;

            if(!gAtpCtrl[(TelAtParserID)AT_MODEM_CHANNLE_1].bEnable)
            {
                uart_printf("%s: openAtChannel 0\n", __FUNCTION__);
		        openAtChannel((TelAtParserID)AT_MODEM_CHANNLE_1);
		    }

            uart_printf("sATP%d->AtChanThread\n", atMsg.sATPInd);

            //osa_status = OSAMsgQSend(gATMsgQ, sizeof(atMsg), (UINT8*)&atMsg, OSA_NO_SUSPEND);
            //ASSERT(osa_status == OS_SUCCESS);

    	}
    	else
    	{
			sendData2SACReq(gsATP2CID[AT_MODEM_CHANNLE_1], buf_ptr, txLength);

    	}

        length -= txLength;
        RxPtr  += txLength;
    }

    Modem1RxRingBuffer_R+=mvUsbModemRxSize;

	if(Modem1RxRingBuffer_R >= mvUsbModem0RxNum*mvUsbModemRxSize)
	{
		Modem1RxRingBuffer_R=0;
	}


    USBDeviceEndpointReceiveCompletedExt((USBDevice_EndpointE)MODEM_1_RX_ENDPOINT,
                                            mvUsbModem1RxRingBuffer+Modem1RxRingBuffer_W,
                                            mvUsbModemRxSize,
                                            TRUE);
	/*mischecked by coverity*/
	/*coverity[leaked_storage]*/

}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbModem0RxLISR                                                */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The modem 0 Rx Low-Level Interrupt Service Routine(LISR).        */
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
void mvUsbModem0RxLISR(USBDevice_EndpointE ep ,UINT8 *data ,UINT32 Len ,BOOL end)
{
	STATUS osStatus;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

	mvUsbModem0RxSt[Modem0RxSt_w].Ep  = ep;
	mvUsbModem0RxSt[Modem0RxSt_w].ptr = data;
	mvUsbModem0RxSt[Modem0RxSt_w].len = Len;
	Modem0RxSt_w++;

	if(Modem0RxSt_w >= mvUsbModemRxNum)
	{
		Modem0RxSt_w = 0;
	}

	ASSERT(Modem0RxSt_w != Modem0RxSt_r);

	mvUsbATPOpen[AT_MODEM_CHANNLE] = 1;
#ifdef PLAT_USE_ALIOS
	mvUsbATPOpen[AT_MODEM_CHANNLE_1] = 1;
#endif
#ifdef YMODEM_EEH_DUMP
	if (useYmodem() == 1)
	{
		if (SysIsAssert() == 0)
		{
			osStatus = OS_Activate_HISR(&Modem0RxHISR);
			ASSERT( osStatus == OS_SUCCESS ) ;
		}
		else
			mvUsbModem0RxHISR();

		usbmodemrxint = 1;
	}
	else
#endif
	{
		osStatus = OS_Activate_HISR(&Modem0RxHISR);
		ASSERT( osStatus == OS_SUCCESS ) ;
	}
}

void mvUsbModem1RxLISR(USBDevice_EndpointE ep ,UINT8 *data ,UINT32 Len ,BOOL end)
{
	STATUS osStatus;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	mvUsbModem1RxSt[Modem1RxSt_w].Ep  = ep;
	mvUsbModem1RxSt[Modem1RxSt_w].ptr = data;
	mvUsbModem1RxSt[Modem1RxSt_w].len = Len;
	Modem1RxSt_w++;

	if(Modem1RxSt_w >= mvUsbModemRxNum)
	{
		Modem1RxSt_w = 0;
	}

	ASSERT(Modem1RxSt_w != Modem1RxSt_r);

	mvUsbATPOpen[AT_MODEM_CHANNLE_1] = 1;

	{
		osStatus = OS_Activate_HISR(&Modem1RxHISR);
		ASSERT( osStatus == OS_SUCCESS ) ;
	}
}

volatile UINT32 USB_uart_start = 0; 

void mvUsbModemUartRxLISR(USBDevice_EndpointE ep ,UINT8 *data ,UINT32 Len ,BOOL end)
{
	STATUS osStatus;
	
	USB_uart_start = 1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

	mvUsbModemUartRxSt[ModemUartRxSt_w].Ep  = ep;
	mvUsbModemUartRxSt[ModemUartRxSt_w].ptr = data;
	mvUsbModemUartRxSt[ModemUartRxSt_w].len = Len;
	ModemUartRxSt_w++;

	if(ModemUartRxSt_w >= mvUsbModemRxNum)
	{
		ModemUartRxSt_w = 0;
	}

	ASSERT(ModemUartRxSt_w != ModemUartRxSt_r);

	uart_printf("mvUsbModemUartRxLISR 1\r\n");

	{
		osStatus = OS_Activate_HISR(&ModemUartRxHISR);
		ASSERT( osStatus == OS_SUCCESS ) ;
	}
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbModem0TxHISR                                                */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The modem 0 Tx High-Level Interrupt Service Routine(HISR).       */
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
void mvUsbModem0TxHISR(void)
{
	UINT8 *TxPtr;
	UINT32 length;
	UINT8 TxNum = Modem0TxSt_r++;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

	if(Modem0TxSt_r >= mvUsbModemTxNum)
	{
		Modem0TxSt_r=0;
	}

	TxPtr = mvUsbModem0TxSt[TxNum].ptr;
	length = mvUsbModem0TxSt[TxNum].len;

	/* Free the Tx memory. */
	if(TxPtr)
	{
	    free(TxPtr);
	}
	else
	{
		ASSERT(length != 0);
	}

	Modem0TxPendingTick = mvUsbInvalidTicks;
}
void mvUsbModem1TxHISR(void)
{
	UINT8 *TxPtr;
	UINT32 length;
	UINT8 TxNum = Modem1TxSt_r++;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

	if(Modem1TxSt_r >= mvUsbModemTxNum)
	{
		Modem1TxSt_r=0;
	}

	TxPtr = mvUsbModem1TxSt[TxNum].ptr;
	length = mvUsbModem1TxSt[TxNum].len;

	/* Free the Tx memory. */
	if(TxPtr)
	{
	    free(TxPtr);
	}
	else
	{
		ASSERT(length != 0);
	}

	Modem1TxPendingTick = mvUsbInvalidTicks;
}

void mvUsbModemUartTxHISR(void)
{
	UINT8 *TxPtr;
	UINT32 length;
	UINT8 TxNum = ModemUartTxSt_r++;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

	if(ModemUartTxSt_r >= mvUsbModemTxNum)
	{
		ModemUartTxSt_r=0;
	}

	TxPtr = mvUsbModemUartTxSt[TxNum].ptr;
	length = mvUsbModemUartTxSt[TxNum].len;

	/* Free the Tx memory. */
	if(TxPtr)
	{
	    free(TxPtr);
	}
	else
	{
		ASSERT(length != 0);
	}

	ModemUartTxPendingTick = mvUsbInvalidTicks;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbModem0TxLISR                                                */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The modem 0 Tx Low-Level Interrupt Service Routine(LISR).       */
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
void mvUsbModem0TxLISR(USBDevice_EndpointE ep ,UINT8 *data ,UINT32 Len ,BOOL dummy)
{
    STATUS osStatus;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef YMODEM_EEH_DUMP
	if (useYmodem() == 1)
	{
		usbmodemtxint = 1;

		if (SysIsAssert() == 1)
		{
			return;
        }
	}
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

	mvUsbModem0TxSt[Modem0TxSt_w].Ep  = ep;
	mvUsbModem0TxSt[Modem0TxSt_w].ptr = data;
	mvUsbModem0TxSt[Modem0TxSt_w].len = Len;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

	Modem0TxSt_w++;

	if(Modem0TxSt_w >= mvUsbModemTxNum)
	{
		Modem0TxSt_w = 0;
	}

	ASSERT(Modem0TxSt_w != Modem0TxSt_r);

	osStatus = OS_Activate_HISR(&Modem0TxHISR);
	ASSERT( osStatus == OS_SUCCESS );

}

void mvUsbModem1TxLISR(USBDevice_EndpointE ep ,UINT8 *data ,UINT32 Len ,BOOL dummy)
{
    STATUS osStatus;
	
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

	mvUsbModem1TxSt[Modem1TxSt_w].Ep  = ep;
	mvUsbModem1TxSt[Modem1TxSt_w].ptr = data;
	mvUsbModem1TxSt[Modem1TxSt_w].len = Len;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

	Modem1TxSt_w++;

	if(Modem1TxSt_w >= mvUsbModemTxNum)
	{
		Modem1TxSt_w = 0;
	}

	ASSERT(Modem1TxSt_w != Modem1TxSt_r);

	osStatus = OS_Activate_HISR(&Modem1TxHISR);
	ASSERT( osStatus == OS_SUCCESS );

}

void mvUsbModemUartTxLISR(USBDevice_EndpointE ep ,UINT8 *data ,UINT32 Len ,BOOL dummy)
{
    STATUS osStatus;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

	mvUsbModemUartTxSt[ModemUartTxSt_w].Ep  = ep;
	mvUsbModemUartTxSt[ModemUartTxSt_w].ptr = data;
	mvUsbModemUartTxSt[ModemUartTxSt_w].len = Len;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

	ModemUartTxSt_w++;

	if(ModemUartTxSt_w >= mvUsbModemTxNum)
	{
		ModemUartTxSt_w = 0;
	}

	ASSERT(ModemUartTxSt_w != ModemUartTxSt_r);

	osStatus = OS_Activate_HISR(&ModemUartTxHISR);
	ASSERT( osStatus == OS_SUCCESS );

}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbModem1CtrlHISR                                              */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The modem 1 control High-Level Interrupt Service Routine(HISR).  */
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
void mvUsbModem1CtrlHISR(void)
{
	UINT8 *TxPtr;
	UINT32 length;
	UINT8 TxNum = Modem1CtrlSt_r++;

	if(Modem1CtrlSt_r >= mvUsbModemCtrlNum)
	{
		Modem1CtrlSt_r=0;
	}

	TxPtr  = mvUsbModem1CtrlSt[TxNum].ptr;
	length = mvUsbModem1CtrlSt[TxNum].len;

	/* Free the Tx memory. */
	if(TxPtr)
	{
		free(TxPtr);
	}
	else
	{
		ASSERT(length != 0);
	}
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbModem1CtrlLISR                                              */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The modem 1 control Low-Level Interrupt Service Routine(LISR).  */
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
void mvUsbModem1CtrlLISR(USBDevice_EndpointE ep ,UINT8 *data ,UINT32 Len ,BOOL dummy)
{
	STATUS osStatus;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

	mvUsbModem1CtrlSt[Modem1CtrlSt_w].Ep  = ep;
	mvUsbModem1CtrlSt[Modem1CtrlSt_w].ptr = data;
	mvUsbModem1CtrlSt[Modem1CtrlSt_w].len = Len;

	Modem1CtrlSt_w++;

	if(Modem1CtrlSt_w >= mvUsbModemCtrlNum)
	{
		Modem1CtrlSt_w = 0;
	}

	ASSERT(Modem1CtrlSt_w != Modem1CtrlSt_r);

	osStatus = OS_Activate_HISR(&Modem1CtrlHISR);
	ASSERT( osStatus == OS_SUCCESS );

}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbModemSendData                                               */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      Send modem packet data to PC via USB.                            */
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
void  mvUsbModemSendData(UINT8 sATP,UINT8 *bufPtr, UINT32 length)
{
	UINT8 TxEp;
	UINT32 cpsr = 0;
	UINT32 UsedTick = 0, CurrentTick = 0;
	USBCDevice_ReturnCodeE txStatus = USB_DEVICE_RC_ERROR;

#ifdef COM_STREAM_ENABLE

	if(get_com_stream_flag()==1)
	{
		free(bufPtr);
		return;
	}

#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    if(sATP != AT_MODEM_CHANNLE && sATP != AT_MODEM_CHANNLE_1)
	{
    	free(bufPtr);
		return;
	}


	if(!mvUsbATPOpen[sATP])
	{
		free(bufPtr);
		return;
	}
#ifdef PLAT_USE_ALIOS
    if(ATModemSelect == 1 && sATP == AT_MODEM_CHANNLE){
        TxEp = mvUsbATP2TxEP[sATP]; 
    }
    else if(ATModemSelect == 2 && sATP == AT_MODEM_CHANNLE_1){
        TxEp = MODEM_TX_ENDPOINT;

    }
    else{
    	free(bufPtr);
		return;
    }
#else
	if(sATP == AT_MODEM_CHANNLE)
		TxEp = mvUsbATP2TxEP[sATP];
	else
		TxEp = MODEM_1_TX_ENDPOINT;
#endif
    if(Get_sATP_Mode(sATP) == MODEM_CONTROL_MODE)
    {
        if(_usb_device_get_dtd_cnt(TxEp, 1, NULL) > 3)
        {
            CurrentTick = OSAGetTicks();

            cpsr = disableInterrupts();

            if(Modem0TxPendingTick == mvUsbInvalidTicks)
            {
                /* Record osa tick. */
                Modem0TxPendingTick = CurrentTick;
            }

            if (CurrentTick < Modem0TxPendingTick)
            {
                UsedTick = CurrentTick + (0xFFFFFFFF - Modem0TxPendingTick);
            }
            else
            {
                UsedTick = CurrentTick - Modem0TxPendingTick;
            }

            restoreInterrupts(cpsr);

            uart_printf("[sATP 0] PendingTick 0x%x, CurrentTick 0x%x, UsedTick: 0x%x",
                                Modem0TxPendingTick, CurrentTick, UsedTick);

            if (UsedTick >= 200*3)
            {
                mvUsbATPOpen[sATP] = 0;

                free(bufPtr);

                _usb_dci_vusb20_free_ep_transfer(TxEp, 1, free);

                Modem0TxPendingTick = mvUsbInvalidTicks;

                uart_printf("[sATP %d] blocked", sATP);
                return;
            }
        }
    }

	txStatus = USBDeviceEndpointTransmit((USBDevice_EndpointE)TxEp,
										 (UINT8 *)bufPtr,
										 length,
										 TRUE);

    switch(txStatus)
    {
        case USB_DEVICE_RC_OK:
        {
            break;
        }

        case USB_DEVICE_RC_NOT_CONNECTED:
        {
            free(bufPtr);
            break;
        }

        default:
        {
            ASSERT(0);
            break;
        }
    }

}
void  mvUsbModemUartSendData(UINT8 *bufPtr, UINT32 length)
{
	UINT8 TxEp;
	UINT32 cpsr = 0;
	UINT32 UsedTick = 0, CurrentTick = 0;
	USBCDevice_ReturnCodeE txStatus = USB_DEVICE_RC_ERROR;

	TxEp =14;

	txStatus = USBDeviceEndpointTransmit((USBDevice_EndpointE)TxEp,
										 (UINT8 *)bufPtr,
										 length,
										 TRUE);

    switch(txStatus)
    {
        case USB_DEVICE_RC_OK:
        {
            break;
        }

        case USB_DEVICE_RC_NOT_CONNECTED:
        {
            free(bufPtr);
            break;
        }

        default:
        {
            ASSERT(0);
            break;
        }
    }

}



#ifdef COM_STREAM_ENABLE

void  ComStreamTransmitToUsb(UINT8 *bufPtr, UINT32 length)
{
	UINT8 TxEp;
	UINT32 cpsr = 0;
	UINT32 UsedTick = 0, CurrentTick = 0;
	USBCDevice_ReturnCodeE txStatus = USB_DEVICE_RC_ERROR;

	UINT8 sATP;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

	sATP=AT_MODEM_CHANNLE;

	TxEp = mvUsbATP2TxEP[sATP];

	txStatus = USBDeviceEndpointTransmit((USBDevice_EndpointE)TxEp,
										 (UINT8 *)bufPtr,
										 length,
										 TRUE);

    switch(txStatus)
    {
        case USB_DEVICE_RC_OK:
        {
            break;
        }

        case USB_DEVICE_RC_NOT_CONNECTED:
        {
            free(bufPtr);
            break;
        }

        default:
        {
            ASSERT(0);
            break;
        }
    }

}
#endif


#ifdef YMODEM_EEH_DUMP
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbModemSendDataYmodem                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      Send modem packet data to Ymodem via USB.                        */
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
void  mvUsbModemSendDataYmodem(UINT8 sATP,UINT8 *bufPtr, UINT32 length)
{
	UINT8 TxEp;
	UINT32 cpsr = 0;
	UINT32 UsedTick = 0, CurrentTick = 0;
	USBCDevice_ReturnCodeE txStatus = USB_DEVICE_RC_ERROR;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	if(!mvUsbATPOpen[sATP])
	{
		uart_printf("mvUsbModemSendData0");
		return;
	}

	TxEp = mvUsbATP2TxEP[sATP];


	txStatus = USBDeviceEndpointTransmit((USBDevice_EndpointE)TxEp,
										 (UINT8 *)bufPtr,
										 length,
										 TRUE);
    switch(txStatus)
    {
        case USB_DEVICE_RC_OK:
        {
            break;
        }

        case USB_DEVICE_RC_NOT_CONNECTED:
        {
            uart_printf("not connect");
            break;
        }

        default:
        {
            ASSERT(0);
            break;
        }
    }

}
#endif

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbQueryATPort                                                 */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function query AT Port Status.                              */
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
/*      TRUE                                Connected                    */
/*      FALSE                               Disconnected                 */
/*                                                                       */
/*************************************************************************/
BOOL mvUsbQueryATPort(void)
{
	if(mvUsbATPOpen[AT_MODEM_CHANNLE] == 1)
	{
		return TRUE;
	}
	else
	{
        return FALSE;
	}
}

#ifdef YMODEM_EEH_DUMP
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbChangeAtInterface                                           */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function Change At Interface.                               */
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
/*      TRUE                                Connected                    */
/*      FALSE                               Disconnected                 */
/*                                                                       */
/*************************************************************************/
void mvUsbChangeAtInterface(void)
{
	mvUsbATP2Interface[AT_MODEM_CHANNLE] = 0;
}
#endif
#ifdef SPI_MUX
UINT32 SPIMasterOpen = 0;
void  modemSendDataSPI(UINT8 sATP,UINT8 *bufPtr, UINT32 length)
{
	
	uart_printf("SPI sendAT, %s\r\n",bufPtr);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    if(sATP != AT_MODEM_SPI_CHANNLE_1)
	{
    	free(bufPtr);
		return;
	}
#if 0
	if(!mvUsbATPOpen[sATP] && !SPIMasterOpen)
	{
		free(bufPtr);
		return;
	}
#endif    
	amux_write_data(atDLCI, (char *)bufPtr, length);

}

void receive_AT_SPI_Task(VOID *argv)
{

	UINT8 *RxPtr = NULL;
	int length ;
	UINT8  sATPMode;
	uart_printf("receive_AT_SPI_Task 0 \r\n");
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	RxPtr = (UINT8 *)malloc(2048);
	while(1)
	{
		memset(RxPtr,0x0,2048);
		length = amux_read_data(atDLCI, (char *)RxPtr, 2048);


		
		sATPMode = Get_sATP_Mode(AT_MODEM_SPI_CHANNLE_1);
		//mvUsbATPOpen[AT_MODEM_SPI_CHANNLE_1] = 1;
		//SPIMasterOpen = 1;

		ATRecv(AT_MODEM_SPI_CHANNLE_1,RxPtr,length);

	}

}
#endif
