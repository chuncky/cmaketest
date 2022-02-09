/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                mvUsbNet.c


GENERAL DESCRIPTION

    This file is for USB Net.

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
//#include "ECM.h"
#include "common.h"
#include "os_hal.h"
#include "rndis.h"
#include "usb_init.h"
#include "mvUsbNet.h"
#include "mvUsbMemory.h"
#include "mvUsbDevPrv.h"
#include "mvUsbDevApi.h"
#include "mvUsbLog.h"
//#include "netif.h"
//#include "tcpip.h"
//#include "netif_td_api.h"
//#include "mem.h"
//#include "MBIM.h"
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Usb Net Rx Hisr. */
OS_HISR UsbNetRxHisr;

/* Usb Net Tx Hisr. */
OS_HISR UsbNetTxHisr;

/* Usb Net Contrl Hisr. */
OS_HISR UsbNetCtrlHisr;

/* Usb net type.*/
mvUsbNetType UsbNetType;

/* Rndis params */
mvUsbNetParams UsbNetParams;

/* Usb net get descriptor info.*/
mvUsbGetDescInfo UsbDescInfo;

/* Usb net Tx queue DTD threshold */
UINT32 UsbNetTxQDTDThr = 30;

/* Usb net Tx interupt count */
UINT32 UsbNetTxIntCnt = 0;

/* Usb net Tx interupt threshold */
UINT32 UsbNetTxIntThr = 26;

/* Usb net Tx tick */
UINT32 UsbNetOsTick = 0;

/* Usb net Rx debug */
volatile UINT32 UsbNetRxDbg[4];

/* Usb net rx write index*/
volatile UINT8  UsbNetRxSt_w = 0;

/* Usb net rx read index*/
volatile UINT8  UsbNetRxSt_r = 0;

/* Usb net Tx write index*/
volatile UINT8  UsbNetTxSt_w = 0;

/* Usb net Tx read index*/
volatile UINT8  UsbNetTxSt_r = 0;

/* Usb net rx buffer write index*/
volatile UINT32 UsbNetRx_W = 0;

/* Usb net rx buffer read index*/
volatile UINT32 UsbNetRx_R = 0;

/* Usb net request Queue head. */
mvUsbNetQ UsbNetReqQHdr;

/* Usb net response Queue head. */
mvUsbNetQ UsbNetRspQHdr;

/* Usb net rndis Tx Queue head. */
mvUsbNetQ UsbRndisTxQHdr;

/* Usb net merged Queue Node. */
mvUsbNetQ *pUsbMergedNode = NULL;

/* Usb net initialized flag. */
BOOL UsbNetInitsDone = FALSE;

/* Usb Net request message queue.*/
OSMsgQRef UsbNetReqMsgQ = NULL;

/* Usb Net request task Reference*/
OSTaskRef UsbNetReqTaskRef = NULL;



#define mem_free  free
#define mem_malloc malloc

#ifdef USB_REMOTEWAKEUP
/* Usb Net request task Reference*/
OSTaskRef UsbPacketSendTaskRef = NULL;

static OSFlagRef UsbDataflag   = NULL;

#define	USBTIMEOUTSIGFLAG	0x01
#define	USBDATASIGFLAG		0x02

typedef struct _usb_data_node
{
	unsigned char *buf;
	unsigned int  len;
	int  ep;
	void *next;
}usb_data_node;

static usb_data_node * usbdatalist = NULL;

void mvUsbSendTask(void *);
#endif
OSTaskRef UsbQueueRXTaskRef = NULL;
static OSFlagRef usbQueueRXTaskFlag   = NULL;
void mvUSBQueueRXPacketTask(void *arg);
/* Usb net Rx ring buffer*/
mvUsbNetMsg UsbNetRxSt[USBNET_MAX_RX_CNT];

/* Usb net Tx ring buffer*/
mvUsbNetMsg UsbNetTxSt[USBNET_MAX_TX_CNT];

/* Usb net mac address */
UINT8 UsbNetMacAddress[USB_NET_ETH_ALEN] =
{
    0xAC,
    0x0C,
    0x29,
    0xA3,
    0x9B,
    0x6D
};


/* MBIM response available notification message */
const UINT8 MBIMResAvailable[] = {
    0xa1,   0x01,   0x00,   0x00,
    0x00,   0x00,   0x00,   0x00
};

UINT8 MBIMNetworkConnection[] = {
	0xa1,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00
};

UINT8 MBIMSpeedChangeNotification[] = {
	0xa1,	0x2A,	0x00,	0x00,
	0x00,	0x00,	0x08,	0x00,
	0x00,	0x00,	0x64,	0x19,
	0x00,	0x00,	0x64,	0x19
};

/*===========================================================================

            EXTERN DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* usb busy flag */
extern UINT32 pm_usb_busy;

/* lwIP network interfaces for PC */
extern UINT32 lwip_alloc_lte_usbcnt;

/* RNDIS multiple packet support flag */
extern BOOL RndisMultiplePacket;
#ifdef USB_REMOTEWAKEUP
extern BOOL EnableRemoteWakeup;
#endif
/* Usb net receive buffer*/
extern UINT8 UsbNetPacketRxBuf[USBNET_RX_MAX_SIZE];
extern UINT8 IsMbimEnabled(void);
extern BOOL usb_uplink_is_reach_hwm(void);

signed char mvUsbNetGetPacket(UINT8 *data, UINT32 len, pmsg *pmsg);
void mvUsbRingbufFirstAlloc(UINT8 **ppRxBuf, UINT32 *pRxBuflen);

#ifdef MBIM_FUNCTION
extern void mbim_usbsetstate(int state);
extern int mbim_usbgetstate(void);
extern void mbimdata_free(void *data);
extern void MbimReadControlMsgFromUSB(char * data, int dataLen);
extern int mbim_parseNTB(unsigned char *data, int len, unsigned char ltemem);
#endif
/*===========================================================================

                          INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetIfEnable                                                 */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function check whether network interfaces is enable or not.  */
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
BOOL mvUsbNetIfEnable(void)
{
    if(netif_pc_get_status())
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetMemInit                                                  */
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
void mvUsbNetMemInit (void)
{
    /* Usb net request queue initialize. */
    mvUsbNetQInit(&UsbNetReqQHdr);

    /* Usb net rsp queue initialize. */
    mvUsbNetQInit(&UsbNetRspQHdr);

    /* Rndis tx queue initialize. */
    mvUsbNetQInit(&UsbRndisTxQHdr);

    /* Usb memory pool initialize. */
    mvUsbMemPoolInit();
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetFreeTxBuffer                                             */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function free Usb net Tx buffer.                             */
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
void mvUsbNetFreeTxBuffer(void)
{
    UINT8 *TxPtr = NULL;
    int   length = 0, i = 0;

    for(i = 0; i < USBNET_MAX_TX_CNT; i++)
    {
        TxPtr  = UsbNetTxSt[i].ptr;
        length = UsbNetTxSt[i].len;

        UsbNetTxSt[i].ptr = NULL;
        UsbNetTxSt[i].len = 0;
        UsbNetTxSt[i].Ep = 0;

        //Free the buffer provided by LWIP.
        if (TxPtr != NULL)
        {
            mvUsbNetTxMemFree(TxPtr);
        }
    }
}


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetResetRingBuffer                                          */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function reset Usb net ring buffer.                          */
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
void mvUsbNetResetRingBuffer(void)
{
    /* Reset Usb net Rx index*/
    UsbNetRxSt_w  = 0;
    UsbNetRxSt_r  = 0;

    /* Reset Usb net Tx index*/
    UsbNetTxSt_w  = 0;
    UsbNetTxSt_r  = 0;

    /* Reset Usb net Rx ring buffer. */
    memset( (void *)UsbNetRxSt, 0, sizeof(mvUsbNetMsg)*USBNET_MAX_RX_CNT );

    /* Reset Usb net Tx ring buffer. */
	memset( (void *)UsbNetTxSt, 0, sizeof(mvUsbNetMsg)*USBNET_MAX_TX_CNT );
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetResetRspQ                                                */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function reset the response queue of Usb net.                */
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
void mvUsbNetResetRspQ (void)
{
    mvUsbNetQ *ReqQ = NULL;

    /* Reset Usb net response queue. */
    while((ReqQ = mvUsbNetQGet(&UsbNetRspQHdr))!= NULL)
    {
        mvUsbDeallocate(ReqQ);
    }

    /* Clean up the DTD of control endpoint. */
    _usb_dci_vusb20_free_ep_transfer( USB_NET_CRTL_ENDPOINT, 1, NULL );
}


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetResetMsgQ                                                */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function reset the messgae queue of Usb net.                 */
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
void mvUsbNetResetMsgQ (void)
{
    mvUsbNetQ *ReqQ = NULL;

    /* If Usb net not initialized, Do nothing. */
    if(!UsbNetInitsDone)
    {
        ErrorLogPrintf("Usb net not initialized");
        return;
    }

    /* Reset usb net request queue. */
    while((ReqQ = mvUsbNetQGet(&UsbNetReqQHdr))!= NULL)
    {
        mvUsbDeallocate(ReqQ);
    }

    /* Reset usb net response queue. */
    while((ReqQ = mvUsbNetQGet(&UsbNetRspQHdr))!= NULL)
    {
        mvUsbDeallocate(ReqQ);
    }

    /* Reset usb net transmit queue. */
    while((ReqQ = mvUsbNetQGet(&UsbRndisTxQHdr))!= NULL)
    {
        mvUsbNetTxMemFree(ReqQ->data);
        mem_free(ReqQ);
    }

    /* Reset usb net Merged Node. */
    if(pUsbMergedNode != NULL)
    {
        mvUsbNetTxMemFree(pUsbMergedNode->data);

        mem_free(pUsbMergedNode);

        pUsbMergedNode = NULL;
    }
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetResetParameters                                          */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function reset the parameters of Usb net.                    */
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
void mvUsbNetResetParameters (void)
{
    mvUsbNetParams *mvUsbNetPar = mvUsbGetNetParams();
    mvUsbGetDescInfo *pUsbDesInfo = mvUsbGetDescriptorInfo();

	memset((void *)mvUsbNetPar, 0x00, sizeof(mvUsbNetParams));

    /* Reset USB Desc info. */
    memset((void *)pUsbDesInfo, 0x00, sizeof(mvUsbGetDescInfo));

    mvUsbNetFreeTxBuffer();

    mvUsbNetParasInit();
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetParasInit                                                */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function init the parameters of Usb net.                     */
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
void mvUsbNetParasInit (void)
{
    /* Reset Usb net ring buffer. */
    mvUsbNetResetRingBuffer();

    /* Reset the messgae queue. */
    mvUsbNetResetMsgQ();

    /* Reset Usb net tx tick. */
    UsbNetOsTick = 0;

    /* Reset Usb net Tx int count */
    UsbNetTxIntCnt = 0;
}


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetPrintMemInfo                                             */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function pint the memory information of Usb net.             */
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
void mvUsbNetPrintMemInfo (void)
{
    /* Usb net request queue cnt. */
    ErrorLogPrintf("Usb net ReqQ %d", UsbNetReqQHdr.cnt);

    /* Usb net response queue cnt. */
    ErrorLogPrintf("Usb net RspQ %d", UsbNetRspQHdr.cnt);

    /* Usb net Tx queue cnt. */
    ErrorLogPrintf("Usb net TxQ %d", UsbRndisTxQHdr.cnt);

    /* Usb net Tx queue cnt. */
    ErrorLogPrintf("Usb net memory %d", mvUsbAvailableMemoryCnt());
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetTaskInit                                                 */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function intializes the usb net related message queue, task. */
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
void mvUsbNetTaskInit (void)
{
    OSA_STATUS	status;
    void *pStack = NULL;

    /* Usb net message queue initialize. */
    status = OSAMsgQCreate(&UsbNetReqMsgQ,
#ifdef  OSA_QUEUE_NAMES
                          "UsbReqQ",
#endif
                          USBNREQ_MSG_Q_SIZE,
                          USBNREQ_MSG_Q_MAX,
                          OSA_PRIORITY);
	ASSERT(status == OS_SUCCESS);

    /* Usb net request task stack initialize. */
    pStack = malloc(USBNREQ_TASK_STACK_SIZE);
    ASSERT(pStack != NULL);

    /* Usb net request task initialize. */
	status = OSATaskCreate(&UsbNetReqTaskRef,
                           pStack,
                           USBNREQ_TASK_STACK_SIZE,
                           USBNREQ_TASK_PRIORITY,
                           "UsbReqTa",
                           mvUsbNetReqTask,
                           0);
	if(OS_SUCCESS != status)
	{
		free(pStack);
		ASSERT(0);
	}
#ifdef USB_REMOTEWAKEUP
	status = OSAFlagCreate(&UsbDataflag);
	ASSERT(status == OS_SUCCESS);

	pStack = malloc(1024);
	status = OSATaskCreate(&UsbPacketSendTaskRef,
                           pStack,
                           1024,
                           75,
                           "UsbSend",
                           mvUsbSendTask,
                           0);    
	if(OS_SUCCESS != status)
	{
		free(pStack);
		ASSERT(0);
	}    
#endif
    status = OSAFlagCreate(&usbQueueRXTaskFlag);
    ASSERT(status == OS_SUCCESS);

    pStack = malloc(1024);
    status = OSATaskCreate(&UsbQueueRXTaskRef,
                          (void *)pStack, 1024,
                          HISR_PRIORITY_2 + 1,
                          "UQueueRX",
                          mvUsbQueueRXPacketTask, NULL);   
	if(OS_SUCCESS != status)
	{
		free(pStack);
		ASSERT(0);
	}
    
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetInit                                                     */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function intializes the usb net related HISR.                */
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
void mvUsbNetHISRInit (void)
{
    /* Create usb net control HISR. */
    OS_Create_HISR(&UsbNetCtrlHisr,
                    "UsbReqH",
                    mvUsbNetCtrlHISR,
                    HISR_PRIORITY_2);

    /* Create usb net Rx HISR. */
	OS_Create_HISR(&UsbNetRxHisr,
	                "RndisRxH",
	                mvUsbNetRxHISR,
	                HISR_PRIORITY_2);

    /* Create usb net Tx HISR. */
	OS_Create_HISR(&UsbNetTxHisr,
	                "RndisTxH",
	                mvUsbNetTxHISR,
	                HISR_PRIORITY_2);
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetEndpointInit                                             */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function intializes the usb net related endpoint.            */
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
void mvUsbNetEndpointInit (void)
{
    USBCDevice_ReturnCodeE status;
    UINT8 *pRxBuf = NULL;
    UINT32 RxBuflen = 0;

    status = USBDeviceEndpointOpen((USBDevice_EndpointE)USB_NET_CRTL_ENDPOINT,
			(USBDevice_UsageTypeE)(USB_DEVICE_USAGE_INTERRUPT | USB_DEVICE_NEED_ZLP),
			0,
			(UINT8 *)NULL,
			0,
			mvUsbNetCtrlTxLISR);
	ASSERT(status == USB_DEVICE_RC_OK);

	status = USBDeviceEndpointOpen((USBDevice_EndpointE)USB_NET_TX_ENDPOINT,
			(USBDevice_UsageTypeE)(USB_DEVICE_USAGE_INTERRUPT | USB_DEVICE_NEED_ZLP),
			0,
			(UINT8 *)NULL,
			0,
			mvUsbNetTxLISR);
	ASSERT(status == USB_DEVICE_RC_OK);

	status = USBDeviceEndpointOpen((USBDevice_EndpointE)USB_NET_RX_ENDPOINT,
    		(USBDevice_UsageTypeE)USB_DEVICE_USAGE_INTERRUPT,
    		0,
    		(UINT8 *)NULL,
    		0,
    		mvUsbNetRxLISR);
	ASSERT(status == USB_DEVICE_RC_OK);

    mvUsbRingbufFirstAlloc(&pRxBuf, &RxBuflen);
	status = USBDeviceEndpointReceive((USBDevice_EndpointE)USB_NET_RX_ENDPOINT,
        	pRxBuf,
        	RxBuflen,
        	TRUE);
	ASSERT(status == USB_DEVICE_RC_OK);

	uart_printf("mvUsbNetEndpointInit\n");
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetTypeInit                                                 */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function intializes the usb net type                         */
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
void mvUsbNetTypeInit (void)
{
	Usb_DriverS *pCfg = GetUsbDriverConfig();

    switch(pCfg->usb_driver)
	{
		case USB_GENERIC_MOD_ECM_DRIVER:
		{
			mvUsbSetNetType(USB_NET_ECM);
			break;
		}

		case USB_GENERIC_MOD_DRIVER:
		{
			mvUsbSetNetType(USB_NET_RNDIS);
			break;
		}
/*
		case USB_MBIM_ONLY_DRIVER:
		case USB_MBIM_GENERIC_DRIVER:
		{
			mvUsbSetNetType(USB_NET_MBIM);
			break;
		}
*/
		default:
		{
			mvUsbSetNetType(USB_NET_RNDIS);
			break;
		}
	}
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetInit                                                     */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function intializes the usb net related message queue, task  */
/*      and HISR.                                                        */
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
void mvUsbNetInit (void)
{
	mvUsbNetParams *NetParams = mvUsbGetNetParams();

    /* Usb net memory initialize. */
    mvUsbNetMemInit();

    /* Usb net MAC Address initialize. */
    mvUsbNetInitMacAddress();

    /* Usb net parameters initialize. */
    memset(NetParams, 0x00, sizeof(mvUsbNetParams));

    /* usb net task initialize. */
    mvUsbNetTaskInit();

    /* usb net HISR initialize. */
    mvUsbNetHISRInit();

    /* usb net endpoint initialize. */
    mvUsbNetEndpointInit();

    /* Set rndis to be default type. */
    mvUsbNetTypeInit();

    /* Usb net initialized done. */
    UsbNetInitsDone = TRUE;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetReqTask                                                  */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The usb net request task, it will process the request message of */
/*      Rndis, ECM and MBIM.                                             */
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
/*      input                               Task parameters              */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void mvUsbNetReqTask (void *input)
{
    OSA_STATUS status;
    mvUsbNetQ *ReqQ = NULL;
    mvUsbNetReqMsg req;

    while(1)
    {
        status = OSAMsgQRecv(UsbNetReqMsgQ, (UINT8 *)&req, USBNREQ_MSG_Q_SIZE, OSA_SUSPEND);
        ASSERT(status == OS_SUCCESS);

        if(req.id != USBNET_REQ_MSG_ID)
        {
            ASSERT(0);
        }

        while((ReqQ = mvUsbNetQGet(&UsbNetReqQHdr))!= NULL)
        {
            switch(ReqQ->resv)
            {
                case USBNET_RNDIS_REQ_MSG:
                {
                    if(ReqQ->len == USBNET_IND_LEN)
                    {
                        Rndis_indicate_status_msg(ReqQ->data);
                    }
                    else
                    {
                        RndisMsgParser(ReqQ->data);
                    }
                    break;
                }

                case USBNET_MBIM_REQ_MSG:
                {
#ifdef MBIM_FUNCTION
					MbimReadControlMsgFromUSB((char *)ReqQ->data, ReqQ->len);
#endif
                    break;
                }

                case USBNET_ECM_REQ_MSG:
                {
                    if(ReqQ->len == USBNET_IND_LEN)
                    {
                        Ecm_indicate_status_msg(ReqQ->data);
                    }
                    break;
                }

                default:
                {
                    ErrorLogPrintf("Unknown usb net req 0x%x", ReqQ->resv);
                    break;
                }
            }

            mvUsbDeallocate(ReqQ);
        }
    }
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetCtrlLISR                                                 */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The usb net Low-Level Interrupt Service Routine(LISR).           */
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
/*      type                                Usb net type                 */
/*      data                                request packet               */
/*      len                                 packet length                */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void mvUsbNetCtrlLISR(mvUsbNetType type ,UINT8 *data ,UINT32 len)
{
	OSA_STATUS status;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

	status = OS_Activate_HISR(&UsbNetCtrlHisr);
	ASSERT( status == OS_SUCCESS ) ;

}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetCtrlHISR                                                 */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The usb net High-Level Interrupt Service Routine(HISR).          */
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
void mvUsbNetCtrlHISR(void)
{
    OSA_STATUS status;
    mvUsbNetReqMsg req;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Fill in the request message. */
    req.id = USBNET_REQ_MSG_ID;

    /* Send message to UsbNetReqMsgQ. */
 	status = OSAMsgQSend(UsbNetReqMsgQ, sizeof(mvUsbNetReqMsg), (UINT8*)&req, OSA_NO_SUSPEND);
	ASSERT(status == OS_SUCCESS);

}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetRxHISR                                                   */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The usb net High-Level Interrupt Service Routine(HISR).          */
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
UINT8 g_rndis_ep = 0;
void * mvUsbRingbufAlloc(size_t size)
{
    return(UsbNetPacketRxBuf + UsbNetRx_W);
}

void mvUsbRingbufFirstAlloc(UINT8 **ppRxBuf, UINT32 *pRxBuflen)
{
    UINT32 RxBuflen = MTU_MAX_SET + PBUF_LINK_HLEN + RNDIS_HEADER_LEN;
    UINT8 *pRxBuf = NULL;

    pRxBuf = (UINT8 *)lwip_in_buf_alloc(RxBuflen, PBUF_REF, PBUF_RNDIS, mvUsbRingbufAlloc, NULL);

    *ppRxBuf = pRxBuf;
    *pRxBuflen = RxBuflen;
}

void mvUsbNetRxHISR(void)
{
    mvUsbRetType  ret = USB_NET_MAX_ERROR;
    BOOL multpacket = FALSE, UselteBuf = FALSE;
    UINT32 TotalLen = 0, DataLen = 0, MsgLen = 0;
    UINT32 DataLen2 = 0, MsgLen2 = 0;
    UINT32 RxBuflen = MTU_MAX_SET + PBUF_LINK_HLEN + RNDIS_HEADER_LEN;
    UINT8  rxNum = UsbNetRxSt_r;
    UINT8 *pRxBuf = NULL;
    UINT8 *pRx2Buf = NULL;
    pmsg* pmsg_hdr = NULL;
    pbuf_layer layer = PBUF_RNDIS; /*default set*/
    mvUsbNetParams *param = mvUsbGetNetParams();
    mvUsbNetType type = mvUsbGetNetType();
	OSA_STATUS  osaStatus;


#ifndef MIN_SYS
    /* EnableRemoteWakeup should be disabled if host resume device */
#ifdef USB_REMOTEWAKEUP
	EnableRemoteWakeup = FALSE;
#endif

    switch(type)
    {
		case USB_NET_MBIM:
		{
#ifdef MBIM_FUNCTION
			if (IsMbimEnabled())
        	{
        		if (mbim_usbgetstate() == 0)
        		{
        			mbim_usbsetstate(1);
    			}
        	}
#endif
			break;
		}

        case USB_NET_ECM:
        {
            EcmParamInit();
        }

        default:
        {
            if(!mvUsbNetIfEnable())
            {
                USB_TRACE("PC Netif up");
                pc_netif_status(TRUE);
            }
            break;
        }
    }

    pRxBuf  = UsbNetRxSt[rxNum].ptr;
    TotalLen = UsbNetRxSt[rxNum].len;
    pmsg_hdr = (pmsg *)(pRxBuf - PMSG_SIZE);

    if((!pRxBuf) && (!TotalLen))
    {
        ErrorLogPrintf("Ignore expired Rx HISR.");
        return;
    }

    UsbNetRxSt_r++;
    if(UsbNetRxSt_r >= USBNET_MAX_RX_CNT)
    {
        UsbNetRxSt_r = 0;
    }

    UsbNetRxDbg[0] = (UINT32)rxNum;
    UsbNetRxDbg[1] = (UINT32)pRxBuf;
    UsbNetRxDbg[2] = TotalLen;
    UsbNetRxDbg[3] = (UINT32)UsbNetRxSt[rxNum].Ep;

    g_rndis_ep = (UINT8)UsbNetRxSt[rxNum].Ep;

    ASSERT((TotalLen <= USBNET_RX_BUF_SIZE)&&(pRxBuf != NULL));

    /* Record rx packet count. */
    param->rx_packets += TotalLen;

    if(param->state == USB_NET_UNINITED)
    {
        ret = USB_NET_UNINT_ERROR;
        goto usb_rx_ret;
    }

    UselteBuf = TRUE;
    if((pRxBuf >= UsbNetPacketRxBuf) &&
       (pRxBuf <= (UsbNetPacketRxBuf + USBNET_RX_MAX_SIZE)))
    {
        UselteBuf = FALSE;
    }

    switch(type)
    {
        case USB_NET_RNDIS:
        {
            layer = PBUF_RNDIS;
            RxBuflen = MTU_MAX_SET + PBUF_LINK_HLEN + RNDIS_HEADER_LEN;
            if(PBUF_RNDIS != pmsg_hdr->layer)
            {
                ret = USB_NET_TYPE_ERROR;
                USB_TRACE("RxNetHISR: %lx, %d, %d",  pRxBuf, type, pmsg_hdr->layer);
            }
            else
            {
                ret = USB_NET_NO_ERROR;
            }
            break;
        }

		case USB_NET_ECM:
		{
            layer = PBUF_ECM;
            RxBuflen = MTU_MAX_SET + PBUF_LINK_HLEN + ECM_HEADER_LEN;
			ret = USB_NET_NO_ERROR;
			break;
		}

		case USB_NET_MBIM:
		{
            layer = PBUF_MBIM;
            RxBuflen = MTU_MAX_SET + PBUF_LINK_HLEN + MBIM_HEADER_LEN;
			ret = USB_NET_NO_ERROR;
			break;
		}

        default:
        {
            layer = PBUF_RNDIS;
            RxBuflen = MTU_MAX_SET + PBUF_LINK_HLEN + RNDIS_HEADER_LEN;
            ret = USB_NET_NO_ERROR;
            break;
        }

    }

    if(ret != USB_NET_NO_ERROR)
    {
        goto usb_rx_ret;
    }

    if(type == USB_NET_MBIM)
    {
#ifdef MBIM_FUNCTION
		if (mbim_parseNTB(pRxBuf, TotalLen, pmsg_hdr) < 0) {
            ret = USB_NET_PROC_ERROR;
		} else {
		    ret = USB_NET_NO_ERROR;
		}
#else
        ret = USB_NET_PROC_ERROR;
#endif
        goto usb_rx_ret;
    }

    DataLen = MsgLen = TotalLen;

    /*Note: after mvUsbNetRemoveHdr, need report USB_NET_PROC_ERROR */
    if(!mvUsbNetRemoveHdr(&pRxBuf, &MsgLen, &DataLen))
    {
        /* error */
        ret = USB_NET_PROC_ERROR;
        goto usb_rx_ret;
    }

    /* Check whther the RX buffer contain multiple packet. */
    if(TotalLen > MsgLen && (!UselteBuf))
    {
        pRx2Buf = UsbNetRxSt[rxNum].ptr + MsgLen;

        if(mvUsbNetRemoveHdr(&pRx2Buf, &MsgLen2, &DataLen2))
        {
            multpacket = TRUE;
        }
    }

    if((DataLen > 80)||(DataLen2 > 80))
    {
        /* big packet */
        ret = USB_NET_NO_ERROR;
    }

    //if(mvUsbNetGetPacket(pRx, DataLen, UselteBuf, pmsg_hdr) != 0)
    if(mvUsbNetGetPacket(pRxBuf, DataLen, pmsg_hdr) != 0)
    {
        /* error */
        ret = USB_NET_PROC_ERROR;
        goto usb_rx_ret;
    }

    if(multpacket && (!UselteBuf))
    {
        //if(mvUsbNetGetPacket(pRx2, DataLen2, UselteBuf, pmsg_hdr) != 0)
        if(mvUsbNetGetPacket(pRx2Buf, DataLen2, NULL) != 0)
    	{
            /* error */
    	    if (DataLen > 80)
    	    {
    	        ret = USB_NET_NO_ERROR;
	        }
    	    else
    	    {
                ret = USB_NET_PROC_ERROR;
            }
        }
    }

usb_rx_ret:
    switch(ret)
    {
        case USB_NET_TYPE_ERROR:
        case USB_NET_UNINT_ERROR:
        case USB_NET_PROC_ERROR:
        {
            /* Record dropped packet count. */
            param->rx_dropped += TotalLen;
            lwip_in_buf_free(pmsg_hdr, PBUF_PMSG);
            break;
        }

        case USB_NET_NO_ERROR:
        {
            UsbNetRx_W += USBNET_RX_BUF_SIZE;
            if(UsbNetRx_W >= USBNET_RX_MAX_SIZE)
            {
                UsbNetRx_W = 0;
            }

            ASSERT(UsbNetRx_W != UsbNetRx_R);

            UsbNetRx_R += USBNET_RX_BUF_SIZE;
            if(UsbNetRx_R >= USBNET_RX_MAX_SIZE)
            {
                UsbNetRx_R = 0;
            }

            break;
        }

        default:
        {
            break;
        }
    }

    pRxBuf = NULL; /*pointer to rndis header*/

    if(lwip_get_ul_non_cpy())
    {
        pRxBuf = (UINT8 *)lwip_in_buf_alloc(RxBuflen, PBUF_MEM, layer, NULL, NULL);
    }
    else
    //if(pRxBuf == NULL)
    {
        pRxBuf = (UINT8 *)lwip_in_buf_alloc(RxBuflen, PBUF_REF, layer, mvUsbRingbufAlloc, NULL);
    }

    if (pRxBuf != NULL)
    {
        USBDeviceEndpointReceiveCompletedExt( (USBDevice_EndpointE)(g_rndis_ep),
                                                   pRxBuf,
                                                   RxBuflen,
                                                   TRUE );
    }
    else
    {
        osaStatus = OSAFlagSet(usbQueueRXTaskFlag, 0x1, OSA_FLAG_OR);
        ASSERT(osaStatus == OS_SUCCESS);
    }
#endif

}


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetRxLISR                                                   */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The Usb net Low-Level Interrupt Service Routine(LISR).           */
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
/*      endpoint                            Usb endpoint                 */
/*      dataRecPtr                          Receive packet               */
/*      dataRecLen                          packet length                */
/*      endOfRecMsg                         end message flag             */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void mvUsbNetRxLISR(USBDevice_EndpointE endpoint ,UINT8 *dataRecPtr ,
                           UINT32 dataRecLen ,BOOL endOfRecMsg)

{
    STATUS osStatus;

    UsbNetRxSt[UsbNetRxSt_w].Ep  = endpoint;
    UsbNetRxSt[UsbNetRxSt_w].ptr = dataRecPtr;
    UsbNetRxSt[UsbNetRxSt_w].len = dataRecLen;

    UsbNetRxSt_w++;

    if(UsbNetRxSt_w >= USBNET_MAX_RX_CNT)
    {
        UsbNetRxSt_w = 0;
    }

    ASSERT(UsbNetRxSt_w != UsbNetRxSt_r);

    osStatus = OS_Activate_HISR(&UsbNetRxHisr);
    ASSERT( osStatus == OS_SUCCESS ) ;

}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetTxHISR                                                   */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The Usb net High-Level Interrupt Service Routine(HISR).          */
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
void mvUsbNetTxHISR(void)
{
    UINT8 *TxPtr = NULL;
    int   length = 0;
    UINT8  TxNum = 0, DTDQCnt = 0;
    mvUsbNetParams *rndis_param_ptr = mvUsbGetNetParams();

    while(UsbNetTxSt_r != UsbNetTxSt_w)
    {
        TxNum = UsbNetTxSt_r++;

        if(UsbNetTxSt_r >= USBNET_MAX_TX_CNT)
        {
            UsbNetTxSt_r = 0;
        }

        TxPtr  = UsbNetTxSt[TxNum].ptr;
        length = UsbNetTxSt[TxNum].len;

        UsbNetTxSt[TxNum].ptr = NULL;
        UsbNetTxSt[TxNum].len = 0;

        //Free the buffer provided by LWIP.
        if (TxPtr)
        {
            mvUsbNetTxMemFree(TxPtr);
            rndis_param_ptr->tx_packets += length;
        }
        else
        {
            ASSERT(length == 0);
        }
    }

    DTDQCnt = _usb_device_get_dtd_cnt(USB_NET_TX_ENDPOINT, 1, NULL);

    while( (++DTDQCnt) < UsbNetTxQDTDThr)
    {
        if(!mvUsbNetTxDequeueAndTransfer())
        {
            break;
        }
    }
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetTxLISR                                                   */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The Usb net Low-Level Interrupt Service Routine(LISR).           */
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
/*      endpoint                            Usb endpoint                 */
/*      dataRecPtr                          Receive packet               */
/*      dataRecLen                          packet length                */
/*      dummy                               dummy flag                   */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void mvUsbNetTxLISR(USBDevice_EndpointE endpoint ,UINT8 *dataRecPtr,
                                UINT32 dataRecLen ,BOOL dummy)
{
    UINT8 DTDQCnt = 0;
    STATUS osStatus;

    UsbNetOsTick = 0;

    UsbNetTxSt[UsbNetTxSt_w].Ep = endpoint;
    UsbNetTxSt[UsbNetTxSt_w].ptr = dataRecPtr;
    UsbNetTxSt[UsbNetTxSt_w].len = dataRecLen;
    UsbNetTxSt_w++;

    if(UsbNetTxSt_w >= USBNET_MAX_TX_CNT)
    {
        UsbNetTxSt_w = 0;
    }

    if(UsbNetTxSt_w == UsbNetTxSt_r)
    {
        ASSERT(0);
    }

    DTDQCnt = _usb_device_get_dtd_cnt(USB_NET_TX_ENDPOINT, 1, NULL);

    if((DTDQCnt <= 3)||((++UsbNetTxIntCnt) >= UsbNetTxIntThr))
    {
        UsbNetTxIntCnt = 0;

        osStatus = OS_Activate_HISR(&UsbNetTxHisr);
        ASSERT( osStatus == OS_SUCCESS );
    }
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetCtrlTxLISR                                               */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The Usb net Low-Level Interrupt Service Routine(LISR).           */
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
/*      endpoint                            Usb endpoint                 */
/*      dataRecPtr                          Receive packet               */
/*      dataRecLen                          packet length                */
/*      dummy                               dummy flag                   */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void mvUsbNetCtrlTxLISR(USBDevice_EndpointE endpoint ,UINT8 *dataRecPtr,
                                    UINT32 dataRecLen ,BOOL dummy)
{
    return;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetCheckRspQValidity                                        */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function check the validity of response queque and return    */
/*      the status.                                                      */
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
/*      mvUsbNetQ                           usb net response queue       */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      TRUE                                Valid resonse queue          */
/*      FALSE                               Invalid resonse queue        */
/*                                                                       */
/*************************************************************************/
BOOL mvUsbNetCheckRspQValidity(mvUsbNetQ *queue)
{
    if (queue == NULL)
    {
        return FALSE;
    }

    if(queue->id != USBNET_QUEQUE_ID)
    {
        ErrorLogPrintf("Invalid Q id 0x%x", queue->id);
        return FALSE;
    }

    if(queue->guard != USBNET_QUEQUE_GUARD)
    {
        ErrorLogPrintf("Invalid Q guard 0x%x", queue->guard);
        return FALSE;
    }

    return TRUE;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetQInit                                                    */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function initializes net queue. It should be called         */
/*      on behalf of a queue prior to using the queue.                   */
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
/*      mvUsbNetQ                           Response head pointer        */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void mvUsbNetQInit(mvUsbNetQ *hdr)
{
    ASSERT(hdr != NULL);

    hdr->id     =   USBNET_QUEQUE_ID;
    hdr->next   =   hdr;
    hdr->prev   =   hdr;
    hdr->cnt    =   0;
    hdr->guard  =   USBNET_QUEQUE_GUARD;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetQPut                                                     */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function places a data block at the tail of a net queue on  */
/*      behalf of a queue prior to using the queue.                      */
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
/*      hdr                                 The queue head pointer       */
/*      pRspQ                               The queue pointer            */
/*      len                                 the data length              */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void mvUsbNetQPut(mvUsbNetQ *hdr, mvUsbNetQ *queue)
{
    UINT32 cpsr;

    ASSERT(mvUsbNetCheckRspQValidity(hdr));

    queue->id           =   USBNET_QUEQUE_ID;
    queue->guard        =   USBNET_QUEQUE_GUARD;

	cpsr = disableInterrupts();

    queue->next         =   hdr;
    queue->prev         =   hdr->prev;
    hdr->prev->next     =   queue;
    hdr->prev           =   queue;
    hdr->cnt++;

    restoreInterrupts(cpsr);

    return;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetQGet                                                     */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function removes the data block at head of a queue and      */
/*      returns a pointer to the data block. If the queue is empty, then */
/*      a NULL pointer is returned. queue on behalf of a queue prior to  */
/*      using the queue.                                                 */
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
/*      hdr                                 The queue head pointer       */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      pRspQ                               The returned queue pointer   */
/*                                                                       */
/*************************************************************************/
mvUsbNetQ *mvUsbNetQGet(mvUsbNetQ *hdr)
{
    UINT32 cpsr;
    mvUsbNetQ *pRspQ = NULL;

    ASSERT(mvUsbNetCheckRspQValidity(hdr));

    cpsr = disableInterrupts();

    if(hdr->cnt > 0)
    {
        pRspQ = hdr->next;
        ASSERT(mvUsbNetCheckRspQValidity(pRspQ));
        hdr->next = pRspQ->next;
        pRspQ->next->prev = hdr;
        hdr->cnt--;
    }

    restoreInterrupts(cpsr);

    if(pRspQ != NULL)
    {
        pRspQ->next  = NULL;
        pRspQ->prev  = NULL;
        pRspQ->id    = USBNET_QUNLINK_ID;
        pRspQ->guard = USBNET_QUNLINK_GUARD;
    }

    return pRspQ;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbGetTxNetQCnt                                                */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function Get the tx queque count of USB net.                */
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
/*      mvUsbNetQ                           Response head pointer        */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
int mvUsbGetTxNetQCnt(void)
{
   return UsbRndisTxQHdr.cnt;
}


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetTransmitToUsb                                            */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function transmit the data of usb net to PC via usb.        */
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
/*      endpoint                            Usb endpoint                 */
/*      msgPtr                              Data packet                  */
/*      msgLength                           Data length                  */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void mvUsbNetTransmitToUsb(USBDevice_EndpointE endpoint,
                                        UINT8* msgPtr, UINT32 msgLength)
{
	USBCDevice_ReturnCodeE txStatus;
#ifdef USB_REMOTEWAKEUP
	UINT32 cpsr;
    if(EnableRemoteWakeup)
	{
		_usb_dci_vusb20_remote_wakeup();
		pm_usb_busy = TRUE;

		usb_data_node * node = malloc(sizeof(usb_data_node));
		if (node) {
			node->buf = msgPtr;
			node->ep  = (int)endpoint;
			node->len = msgLength;
			node->next = NULL;
		}

		cpsr = disableInterrupts();
		if (usbdatalist != NULL)
		{
			usb_data_node * ptmp = usbdatalist;
			while(ptmp->next)
				ptmp = ptmp->next;

			ptmp->next = node;
		}
		else
			usbdatalist = node;
		restoreInterrupts(cpsr);

		OSAFlagSet(UsbDataflag, USBTIMEOUTSIGFLAG, OSA_FLAG_OR);
	}
	else
	{
		txStatus = USBDeviceEndpointTransmit(endpoint, (UINT8 *)msgPtr, (UINT16) msgLength, TRUE);
		if((txStatus != USB_DEVICE_RC_OK) && (txStatus != USB_DEVICE_RC_NOT_CONNECTED))
		{
	        uart_printf("mvUsbNetTransmitToUsb txStatus %d", txStatus);
		}

		ASSERT((txStatus == USB_DEVICE_RC_OK) || (txStatus == USB_DEVICE_RC_NOT_CONNECTED));

    /* If usb is not connected, free the tx buffer. */
		if(txStatus == USB_DEVICE_RC_NOT_CONNECTED)
		{
	        switch(endpoint)
	        {
	            case USB_NET_TX_ENDPOINT:
	            {
	                //rndis_free(msgPtr);
					mvUsbNetTxMemFree(msgPtr);
	                break;
	            }
	            default:
	            {
	                break;
	            }
	        }
		}
	}
#else
	txStatus = USBDeviceEndpointTransmit(endpoint, (UINT8 *)msgPtr, (UINT16) msgLength, TRUE);
	ASSERT((txStatus == USB_DEVICE_RC_OK) || (txStatus == USB_DEVICE_RC_NOT_CONNECTED));

    /* If usb is not connected, free the tx buffer. */
	if(txStatus == USB_DEVICE_RC_NOT_CONNECTED)
	{
        switch(endpoint)
        {
            case USB_NET_TX_ENDPOINT:
            {
                //rndis_free(msgPtr);
				mvUsbNetTxMemFree(msgPtr);
                break;
            }

            default:
            {
                break;
            }
        }
	}
#endif
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetSendPacket                                               */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function send data packet to PC via usb.                    */
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
/*      data                                Data packet                  */
/*      len                                 Data length                  */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void mvUsbNetSendPacket(UINT8* data, UINT32 len)
{
    UINT32 cpsr = 0;
    BOOL UsbTxTimeOut = FALSE;
    UINT32 CurrentTick = OSAGetTicks();
    mvUsbNetType type = mvUsbGetNetType();

    cpsr = disableInterrupts();

    if (UsbNetOsTick == 0 || UsbNetOsTick > CurrentTick)
    {
        UsbNetOsTick = CurrentTick;
    }
    else
    {
        if (CurrentTick > UsbNetOsTick + 1000)
        {
            UsbNetOsTick = 0;
            UsbTxTimeOut = TRUE;
        }
    }

    restoreInterrupts(cpsr);

    if (UsbTxTimeOut)
    {
        uart_printf("Usb Net Tx time out");
        USB2MgrDeviceUnplugPlug();
    }

    switch(type)
    {
        case USB_NET_RNDIS:
        {
            Rndis_send_packet( data, len );
            break;
        }

        case USB_NET_MBIM:
        {
            break;
        }

        case USB_NET_ECM:
        {
            Ecm_send_packet( data, len );
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
/*      mvUsbNetTxMemFree                                                */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function free the Tx memory of usb net.                     */
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
/*      buf                                 Free address                 */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void mvUsbNetTxMemFree(void *buf)
{
    mvUsbNetType type = mvUsbGetNetType();

    switch(type)
    {
        case USB_NET_RNDIS:
        {
            rndis_free( buf );
            break;
        }

#ifdef MBIM_FUNCTION
        case USB_NET_MBIM:
        {
            mbimdata_free(buf);
            break;
        }
#endif

        case USB_NET_ECM:
        {
            ecm_free( buf );
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
/*      mvUsbNetRxMemFree                                                */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function free the Rx memory of usb net.                     */
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
/*      buf                                 Free address                 */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void mvUsbNetRxMemFree(void *buf)
{
    ASSERT(buf != NULL);

#if 0
    if(((UINT32)buf < (UINT32)UsbNetPacketRxBuf) ||
       ((UINT32)buf > (UINT32)(UsbNetPacketRxBuf + USBNET_RX_MAX_SIZE)))
    {
        pmsg *ulMsg = (pmsg *)buf;

        ulMsg->phdr = buf;
        ulMsg->plen = PC_USB_UL_BUF_SIZE;
        lte_ul_buf_free((pmsg *)buf);
    }
#endif

    pmsg * pmsg_hdr = NULL;
    pmsg_hdr = (pmsg *)((u8_t *)buf - PMSG_SIZE);
    lwip_in_buf_free(pmsg_hdr, PBUF_PMSG);

    return;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbGetNetType                                                  */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function get the usb net type.                              */
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
/*      buf                                 Free address                 */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      type                                Usb net type                 */
/*                                                                       */
/*************************************************************************/
mvUsbNetType mvUsbGetNetType(void)
{
    return UsbNetType;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbSetNetType                                                  */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function set the usb net type.                              */
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
/*      type                                Usb net type                 */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      none                                N/A                          */
/*                                                                       */
/*************************************************************************/
void mvUsbSetNetType(mvUsbNetType type)
{
    UsbNetType = type;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbGetDescriptorInfo                                           */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function get usb descriptor infomation.                     */
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
/*      buf                                 Free address                 */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      type                                Usb net type                 */
/*                                                                       */
/*************************************************************************/
mvUsbGetDescInfo *mvUsbGetDescriptorInfo(void)
{
    return &UsbDescInfo;
}


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetCreateTxNode                                             */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function create new node for RNDIS Tx queue.                */
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
/*      data                                Data pointer                 */
/*      len                                 Data length                  */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      none                                N/A                          */
/*                                                                       */
/*************************************************************************/
mvUsbNetQ* mvUsbNetCreateTxNode(UINT8* data, UINT32 len)
{
    mvUsbNetQ *pNewNode = NULL;

    pNewNode = (mvUsbNetQ *)mem_malloc(sizeof(mvUsbNetQ));

    if(!pNewNode)
    {
        ErrorLogPrintf("pNode Null");
        //MIFI_LOG_TRACE(MIFI, USB_NET, MEM_MALLOC, "pNode Null");
        return NULL;
    }

    pNewNode->len   =   len;
    pNewNode->data  =   data;

    return pNewNode;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetTxEnqueue                                                */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function enqueue data packet into Tx queue.                 */
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
/*      data                                Data pointer                 */
/*      len                                 Data length                  */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      none                                N/A                          */
/*                                                                       */
/*************************************************************************/
void mvUsbNetTxEnqueue(UINT8* data, UINT32 len)
{
    mvUsbNetQ *pNode;
#if 0
	if (IsMbimEnabled() == 1 && UsbRndisTxQHdr.cnt > 600)
	{
		mvUsbNetTxMemFree(data);
		return;
	}
#endif
    pNode = mvUsbNetCreateTxNode(data, len);

    if(pNode)
    {
        pNode->resv = USBNET_PACKET_MSG_DATA;
        mvUsbNetQPut(&UsbRndisTxQHdr, pNode);
    }
    else
    {
        mvUsbNetTxMemFree(data);
    }
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetTxDequeueAndTransfer                                     */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function dequeue data packet from tx queue and transmit it. */
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
/*      none                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      none                                N/A                          */
/*                                                                       */
/*************************************************************************/
BOOL mvUsbNetTxDequeueAndTransfer(void)
{
    UINT32 cpsr;
    UINT32 PaddingLen = 0;
    mvUsbNetQ *pNode = NULL, *pNextNode = NULL;

    if(RndisMultiplePacket)
    {
        cpsr = disableInterrupts();

        if(pUsbMergedNode != NULL)
        {
            pNode = pUsbMergedNode;
            pUsbMergedNode = NULL;
        }

        restoreInterrupts(cpsr);

        if(pNode == NULL)
        {
            pNode = mvUsbNetQGet(&UsbRndisTxQHdr);
            if(pNode == NULL)
            {
                return FALSE;
            }
        }

        pNextNode = mvUsbNetQGet(&UsbRndisTxQHdr);
        if(pNextNode != NULL)
        {
            if(Rndis_merge_mult_packet(pNode->data, pNextNode->data, &PaddingLen))
            {
                pNode->len += (pNextNode->len + PaddingLen);
                mem_free(pNextNode);
            }
            else
            {
                pUsbMergedNode = pNextNode;
            }
        }

    }
    else
    {
        pNode = mvUsbNetQGet(&UsbRndisTxQHdr);
        if(pNode == NULL)
        {
            return FALSE;
        }
    }

    mvUsbNetTransmitToUsb((USBDevice_EndpointE)USB_NET_TX_ENDPOINT, pNode->data, pNode->len);
    mem_free(pNode);

    return TRUE;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetTxEnqueue                                                */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function enqueue data packet into Tx queue.                 */
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
/*      data                                Data pointer                 */
/*      len                                 Data length                  */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      none                                N/A                          */
/*                                                                       */
/*************************************************************************/
void mvUsbNetLwipSendPacket(UINT8* data, UINT32 len)
{
    UINT8 DTDQCnt = 0;
    //UINT32 RecordTick = 0xFFFFFFFF;

    DTDQCnt = _usb_device_get_dtd_cnt(USB_NET_TX_ENDPOINT, 1, NULL);

    if(((mvUsbGetTxNetQCnt() != 0) && (DTDQCnt != 0)) ||
       (DTDQCnt >= UsbNetTxQDTDThr))
    {
#if 0
        UINT32 CurrentTick = 0;

        /* Record osa tick. */
        CurrentTick = OSAGetTicks();

        if((CurrentTick > RecordTick) && ((CurrentTick - RecordTick) >= 200*2))
        {
            ErrorLogPrintf("Flush Rndis TX DTD");
            _usb_dci_vusb20_free_ep_transfer(USB_NET_TX_ENDPOINT, 1, mvUsbNetTxMemFree);
            _usb_dci_vusb20_reset_ep_data_toggle(USB_NET_TX_ENDPOINT, 1);
            mvUsbNetTransmitToUsb((USBDevice_EndpointE)USB_NET_TX_ENDPOINT, data, len);
        }
        else
#endif
        {

            mvUsbNetTxEnqueue(data, len);
        }
    }
    else
    {
        mvUsbNetTransmitToUsb((USBDevice_EndpointE)USB_NET_TX_ENDPOINT, data, len);
    }
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbGetNetParams                                                */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function get usb net parameters.                             */
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
mvUsbNetParams *mvUsbGetNetParams(void)
{
    return &UsbNetParams;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetMacCmp                                                   */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function comparing the MAC Address.                          */
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
/*      mac                                 The compared mac address     */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      result                              compared result              */
/*                                                                       */
/*************************************************************************/
UINT8 mvUsbNetMacCmp(UINT8 *mac)
{
    return !memcmp( UsbNetMacAddress, mac, USB_NET_ETH_ALEN );
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetGetMacAddress                                            */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function get the MAC Address of usb net.                     */
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
/*      mac                                 The Get mac address          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      address                             mac address                  */
/*                                                                       */
/*************************************************************************/
UINT8 *mvUsbNetGetMacAddress(void)
{
    return UsbNetMacAddress;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetInitMacAddress                                           */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function initialize the random MAC Address of usb net.       */
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
/*      address                             mac address                  */
/*                                                                       */
/*************************************************************************/
void mvUsbNetInitMacAddress(void)
{
    UINT8 wifi_mac[6] = {0};
	UINT8 *mac = mvUsbNetGetMacAddress();
#if 0
    if(netifapi_get_wifi_macaddr(wifi_mac) == 0)
    {
    	mac[0] = 0xAC;
    	mac[1] = wifi_mac[1];
    	mac[2] = wifi_mac[2];
    	mac[3] = ~wifi_mac[3];
    	mac[4] = ~wifi_mac[4];
    	mac[5] = ~wifi_mac[5];
    }

    /* Force ECM to be recognized as usb0 in linux. */
    mac[0] &= ~0x1;
    mac[0] |= 0x2;
#endif

    /* Printf usb mac address. */
    uart_printf("Usb net mac address: %x-%x-%x-%x-%x-%x",
                    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetSwitchTo                                                 */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function switch the usb net to destination type.             */
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
/*      type                                The usb net type             */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      address                             mac address                  */
/*                                                                       */
/*************************************************************************/
void mvUsbNetSwitchTo(mvUsbNetType type)
{
    mvUsbDescriptorInfo *pUsbDesInfo = USB2GetDescriptorInfo();
    PlatformUsbDescType desc = USB_MAX_DESCRIPTOR_TYPE;

    ErrorLogPrintf("mvUsbNetSwitchTo type %d", type);
    uart_printf("UN type %d", type);

    /* Reset the parameters of Usb net. */
    //mvUsbNetResetParameters();

    /* Set usb net type. */
    mvUsbSetNetType(type);

    /* Set usb descriptor. */
    switch(type)
    {
        case USB_NET_RNDIS:
        {
            desc = pUsbDesInfo->DefaultDesc;
            break;
        }

        case USB_NET_ECM:
        {
            desc = USB_MARVELL_ECM_DESCRIPTOR;
            break;
        }

        default:
        {
            break;
        }
    }

    /* Usb Re-enumerate. */
    USB2ReEnumerate(desc);
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetRemoveHdr                                                */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function remove the head of usb net from data packet.        */
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
BOOL mvUsbNetRemoveHdr(UINT8 **data, UINT32 *msgLen, UINT32 *dataLen)
{
    BOOL status = TRUE;
    mvUsbNetType type = mvUsbGetNetType();

    switch(type)
    {
        case USB_NET_RNDIS:
        {
            status = Rndis_remove_hdr(data, msgLen, dataLen);
            break;
        }

        case USB_NET_MBIM:
        {
            status = TRUE;
            break;
        }

        case USB_NET_ECM:
        {
            status = Ecm_remove_hdr(data, msgLen, dataLen);
            break;
        }

        default:
        {
            ASSERT(0);
            break;
        }

    }

    return status;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetIndicateStatus                                           */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function indicate the current usb net status.                */
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
void mvUsbNetIndicateStatus(BOOL connected)
{
    OSA_STATUS status;
    mvUsbNetReqMsg req;
    mvUsbNetQ *pIndQ = NULL;
    mvUsbNetType type = mvUsbGetNetType();
    mvUsbNetParams *pMvUsbNetParam = mvUsbGetNetParams();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Check whether USB net is intialized or not. */
    if (pMvUsbNetParam->state == USB_NET_UNINITED)
	{
		return;
	}

    /* Alloc memory for indication. */
    pIndQ = (mvUsbNetQ *)mvUsbAllocate(0);
    USB_ALLOC_MEM_STATUS(pIndQ);

    /* Set the data pointer. */
    pIndQ->data    = (UINT8 *)pIndQ + sizeof(mvUsbNetQ);

    /* Fill in connected status. */
    pIndQ->data[0] = connected;

    /* Fill in indication ID. */
    pIndQ->data[1] = (USBNET_IND_ID >> 8) & 0xFF;
    pIndQ->data[2] = (USBNET_IND_ID >> 16) & 0xFF;
    pIndQ->data[3] = (USBNET_IND_ID >> 24) & 0xFF;

    /* Fill in indication length. */
    pIndQ->len     = USBNET_IND_LEN;

    /* Fill in reserve flag. */
    switch(type)
    {
        case USB_NET_RNDIS:
        {
            pIndQ->resv = USBNET_RNDIS_REQ_MSG;
            break;
        }

        case USB_NET_MBIM:
        {
            pIndQ->resv = USBNET_MBIM_REQ_MSG;
            break;
        }

        case USB_NET_ECM:
        {
            pIndQ->resv = USBNET_ECM_REQ_MSG;
            break;
        }

        default:
        {
            ASSERT(0);
            break;
        }
    }

    /* Enqueue indication message. */
    mvUsbNetQPut(&UsbNetReqQHdr, pIndQ);

    /* Fill in the request message ID. */
    req.id = USBNET_REQ_MSG_ID;

    /* Send message to UsbNetReqMsgQ. */
 	status = OSAMsgQSend(UsbNetReqMsgQ, sizeof(mvUsbNetReqMsg), (UINT8*)&req, OSA_NO_SUSPEND);
	ASSERT(status == OS_SUCCESS);

}

void mvUsbCIDRespFromMbim(const char *data, int len)
{
	mvUsbNetQ *pRspQ = NULL;
	char *resp;

	pRspQ = (mvUsbNetQ *)mvUsbAllocate(1);
	USB_ALLOC_MEM_STATUS(pRspQ);

	resp = (char *)((UINT32)pRspQ + sizeof(mvUsbNetQ));
	ASSERT(resp != NULL);

	memcpy(resp, data, len);
	pRspQ->data = (UINT8 *)resp;
	pRspQ->len  = len;
    pRspQ->resv = USBNET_MBIM_RSP_MSG;

	mvUsbNetQPut(&UsbNetRspQHdr, pRspQ);
	return;
}

void MbimSendRspAvailableNotification(void)
{

	mvUsbNetTransmitToUsb((USBDevice_EndpointE)USB_NET_CRTL_ENDPOINT,
	                (UINT8 *)MBIMResAvailable, sizeof(MBIMResAvailable)/sizeof(MBIMResAvailable[0]));

}

void MbimNetworkConnectionNotification(UINT8 connect)
{
	if(connect == 1)
	{
		MBIMNetworkConnection[2] = 1;
	}
	else
		MBIMNetworkConnection[2] = 0;

	mvUsbNetTransmitToUsb((USBDevice_EndpointE)USB_NET_CRTL_ENDPOINT,
	                MBIMNetworkConnection, sizeof(MBIMNetworkConnection)/sizeof(MBIMNetworkConnection[0]));

}

void MbimConnectionSpeedChangeNotification(UINT8 connect)
{
	mvUsbNetTransmitToUsb((USBDevice_EndpointE)USB_NET_CRTL_ENDPOINT,
	                MBIMSpeedChangeNotification, sizeof(MBIMSpeedChangeNotification)/sizeof(MBIMSpeedChangeNotification[0]));

}

#ifdef USB_REMOTEWAKEUP
void mvUsbSendToUsb(USBDevice_EndpointE endpoint, UINT8* msgPtr, UINT32 msgLength)
{
	USBCDevice_ReturnCodeE txStatus;

	uart_printf("mvUsbSendToUsb");
	txStatus = USBDeviceEndpointTransmit(endpoint, (UINT8 *)msgPtr, (UINT16) msgLength, TRUE);
	if((txStatus != USB_DEVICE_RC_OK) && (txStatus != USB_DEVICE_RC_NOT_CONNECTED))
	{
        uart_printf("mvUsbSendToUsb txStatus %d", txStatus);
	}

	ASSERT((txStatus == USB_DEVICE_RC_OK) || (txStatus == USB_DEVICE_RC_NOT_CONNECTED));

    /* If usb is not connected, free the tx buffer. */
	if(txStatus == USB_DEVICE_RC_NOT_CONNECTED)
	{
        switch(endpoint)
        {
            case USB_NET_TX_ENDPOINT:
            {
                //rndis_free(msgPtr);
				mvUsbNetTxMemFree(msgPtr);
                break;
            }

            default:
            {
                break;
            }
        }
	}
}

void mvUsbDataSignal(void)
{
	OSAFlagSet(UsbDataflag, USBDATASIGFLAG, OSA_FLAG_OR);
}

void mvUsbDatalistfree()
{
    UINT32 cpsr;
	usb_data_node *pnode = usbdatalist;
	while(pnode)
	{
		cpsr = disableInterrupts();
		usbdatalist = pnode->next;
		restoreInterrupts(cpsr);
		if (pnode->ep == USB_NET_TX_ENDPOINT)
		{
			mvUsbNetTxMemFree(pnode->buf);
		}
		free(pnode);
		pnode = usbdatalist;
	}

	cpsr = disableInterrupts();
	usbdatalist = NULL;
	restoreInterrupts(cpsr);
}

void mvUsbSendTask(void *argv)
{
	OSA_STATUS  osaStatus;
	UINT32		flag  = 0;
	UINT32		timeout = OSA_SUSPEND;
	UINT32 		cpsr;

	while(1)
	{
		flag = 0;
		osaStatus = OSAFlagWait(UsbDataflag, USBDATASIGFLAG|USBTIMEOUTSIGFLAG, OSA_FLAG_OR_CLEAR, &flag, timeout);
		if (flag == USBTIMEOUTSIGFLAG) {
			uart_printf("wait 5s");
			timeout = 200*5; //5s
		}
		else
		{
			usb_data_node *pnode = usbdatalist;
			while (pnode)
			{
				cpsr = disableInterrupts();
				usbdatalist = pnode->next;
				restoreInterrupts(cpsr);
				if (osaStatus == OS_SUCCESS)
					mvUsbSendToUsb(pnode->ep, pnode->buf, pnode->len);
				else
				{
					//memory free if not receive clear remote wakeup in 5s
					if (pnode->ep == USB_NET_TX_ENDPOINT) {
						mvUsbNetTxMemFree(pnode->buf);
						uart_printf("mem free tx usb");
					}
				}
				uart_printf("mem free node");
				free(pnode);
				pnode = NULL;
				pnode = usbdatalist;
			}

			if (osaStatus == OS_TIMEOUT) {
				//clean rspQ if timeout
				mvUsbNetQ *ReqQ = NULL;
			    while((ReqQ = mvUsbNetQGet(&UsbNetRspQHdr))!= NULL)
			    {
			        mvUsbDeallocate(ReqQ);
			    }
			}

			timeout = OSA_SUSPEND;
		}
	}
}
#endif
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbQueueRXPacketTask                                           */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The Task queue USB net RX Packet.                                */
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
/*      mac                                 The compared mac address     */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      result                              compared result              */
/*                                                                       */
/*************************************************************************/
void mvUsbQueueRXPacketTask(void *arg)
{
	OSA_STATUS  osaStatus;
	UINT32		flag  = 0;
	UINT32		timeout = OSA_SUSPEND;
	UINT8 *pRxBuf   = NULL;
    UINT32 RxBuflen = MTU_MAX_SET + PBUF_LINK_HLEN + RNDIS_HEADER_LEN;
    pbuf_layer layer = PBUF_RNDIS; /*default set*/
    mvUsbNetType type = mvUsbGetNetType();
    UINT8 sleepCnt = 0;

    while(1)
    {

    	osaStatus = OSAFlagWait(usbQueueRXTaskFlag, 0x01, OSA_FLAG_AND_CLEAR, &flag, timeout);

    	for (sleepCnt = 0; sleepCnt < USB_QUEUE_RX_SLEEP_CNT; sleepCnt++)
        {
        	OSATaskSleep(1);

        	type = mvUsbGetNetType();
            switch(type)
            {
                case USB_NET_RNDIS:
                {
                    layer = PBUF_RNDIS;
                    RxBuflen = MTU_MAX_SET + PBUF_LINK_HLEN + RNDIS_HEADER_LEN;
                    break;
                }
            
                case USB_NET_ECM:
                {
                    layer = PBUF_ECM;
                    RxBuflen = MTU_MAX_SET + PBUF_LINK_HLEN + ECM_HEADER_LEN;
                    break;
                }
            
                case USB_NET_MBIM:
                {
                    layer = PBUF_MBIM;
                    RxBuflen = MTU_MAX_SET + PBUF_LINK_HLEN + MBIM_HEADER_LEN;
                    break;
                }
            
                default:
                {
                    layer = PBUF_RNDIS;
                    RxBuflen = MTU_MAX_SET + PBUF_LINK_HLEN + RNDIS_HEADER_LEN;
                    break;
                }
            }

            pRxBuf =  NULL;

            if(lwip_get_ul_non_cpy() && usb_uplink_is_reach_hwm() == 0)
            {
                pRxBuf = (UINT8 *)lwip_in_buf_alloc(RxBuflen, PBUF_MEM, layer, NULL, NULL);
            }

            if (pRxBuf == NULL && sleepCnt == USB_QUEUE_RX_SLEEP_CNT - 1)  // if not get buffer, after sleep several times. We will use USB buffer
            {
                if (pRxBuf == NULL) {
                    pRxBuf = (UINT8 *)lwip_in_buf_alloc(RxBuflen, PBUF_REF, layer, mvUsbRingbufAlloc, NULL);
                    //DIAG_FILTER(MIFI, USB, mvUSBQueueRXPacketTask3162, DIAG_INFORMATION)
                    //diagPrintf("lwip: mvUsbQueueRXPacketTask Not Get LTE buffer, will use USB BUffer: sleepCnt=%d; type=%d; buf=%lx, ep=%d",
                    //sleepCnt, type, pRxBuf, g_rndis_ep);
                }
            }

            if (pRxBuf != NULL)
            {
                USBDeviceEndpointReceiveCompletedExt( (USBDevice_EndpointE)g_rndis_ep,
                                                           pRxBuf,
                                                           RxBuflen,
                                                           TRUE );
                break;
            }
       }
    }
}
