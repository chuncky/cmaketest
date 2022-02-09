#ifndef _MVUSBNET_H_
#define _MVUSBNET_H_
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                MVUSBCDC.h


GENERAL DESCRIPTION

    This file is for USB CDC Device.

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
01/29/2013   zhoujin    Created module
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
//#include "UART.h"
#include <stdlib.h>
#include <string.h>
//#include "bsp_hisr.h"
#include "platform.h"
#include "usb_init.h"
#include "usb_device.h"

/*===========================================================================

                                LOCAL MACRO
===========================================================================*/

/* Usb Net maxmium queue count  */
#define  USBNREQ_MSG_Q_MAX	                            48

/* Usb Net request queue size */
#define  USBNREQ_MSG_Q_SIZE	                            4

//#ifdef MV_USB_MBIM
/* Usb Net request task stack size */
#define  USBNREQ_TASK_STACK_SIZE                        1024*8
//#else
//#define  USBNREQ_TASK_STACK_SIZE                        1024
//#endif
/* Usb Net request task priority */
#define  USBNREQ_TASK_PRIORITY                          68

#define  USB_QUEUE_RX_SLEEP_CNT                         1
/* Usb Net indicate message length */
#define  USBNET_IND_LEN                                 4

/* Usb Net indicate message ID */
#define  USBNET_IND_ID                                  0xBABEFC00

/* Usb Net message ID */
#define  USBNET_REQ_MSG_ID                              0xBEDBEDBB

/* Usb Net response queue ID */
#define  USBNET_QUEQUE_ID                               0xABBACDDC

/* Usb Net response queue guard */
#define  USBNET_QUEQUE_GUARD                            0x68686868

/* Usb Net response unlink ID */
#define  USBNET_QUNLINK_ID                              0xABBA0000

/* Usb Net response unlink guard */
#define  USBNET_QUNLINK_GUARD                           0x68680000

/*Non-setup data Req reserve flag */
#define REQ_RCV_COMM_FEATURE_DATA		                0xABCD0001
#define REQ_RCV_LINE_CODING_DATA                        0xABCD0002
#define REQ_RCV_ENCAPSULATED_DATA                       0xABCD0003
#define REQ_RCV_BOT_MSC_RESET_DATA                      0xABCD0004

/* Usb Net packet msg reserve flag */
#define USBNET_PACKET_MSG_DATA		                    0xABCD0005

/* Usb Net Req msg reserve flag */
#define USBNET_RNDIS_REQ_MSG		                    0xABCD0006
#define USBNET_MBIM_REQ_MSG		                        0xABCD0007
#define USBNET_ECM_REQ_MSG	                            0xABCD0008

/* Usb Net Rsp msg reserve flag */
#define USBNET_RNDIS_RSP_MSG		                    0xABCD0009
#define USBNET_MBIM_RSP_MSG                             0xABCD000A

/* Usb Net IND msg reserve flag */
#define USBNET_RNDIS_IND_MSG		                    0xABCD000B
#define USBNET_RNDIS_EEH_IND_MSG		                0xABCD000C

/*===========================================================================

                          Struct definition.

===========================================================================*/

/* usb return type */
typedef enum
{
    USB_NET_NO_ERROR    = 0,
    USB_NET_TYPE_ERROR  = 1,
    USB_NET_UNINT_ERROR = 2,
    USB_NET_PROC_ERROR  = 3,
    USB_NET_MAX_ERROR
}mvUsbRetType;

/* usb net type */
typedef enum
{
    USB_NET_RNDIS       = 0,
    USB_NET_MBIM        = 1,
    USB_NET_ECM         = 2,
    USB_NET_MAX
}mvUsbNetType;

/* usb net init state */
typedef enum mvUsbNetState
{
	USB_NET_UNINITED    = 0,
	USB_NET_ECM_INIT    = 1,
	USB_NET_RNDIS_INIT  = 2,
	USB_NET_MBIM_INIT  	= 3
} mvUsbNetState;

/* Usb net message */
typedef struct mvUsbNetMsg
{
	UINT32	Ep;
	UINT32	len;
	UINT8   *ptr;
} mvUsbNetMsg;

/* Usb net request message */
typedef struct mvUsbNetReqMsg
{
	UINT32	id;
} mvUsbNetReqMsg;

/* Usb net request message */
typedef struct mvUsbNetQ
{
	UINT32  id;
	UINT8  *data;
	UINT32  len;
    struct mvUsbNetQ *next;
    struct mvUsbNetQ *prev;
    int     cnt;
    UINT32  resv;
    UINT32  guard;
} mvUsbNetQ;

/* Usb net parameters */
typedef struct mvUsbNetParams
{
	UINT32  filter;
	UINT32  medium;
	UINT32  speed;
	UINT32  media_state;
	UINT32  tx_packets;
	UINT32  tx_errors;
	UINT32  tx_dropped;
	UINT32  rx_packets;
	UINT32  rx_errors;
	UINT32  rx_dropped;
	UINT32  vendorID;
	char   *vendorDescr;
	mvUsbNetState  state;
} mvUsbNetParams;

/* Usb net get descriptor infomation */
typedef struct mvUsbGetDescInfo
{
	UINT32  DesCnt;
	UINT32  DesType[32];
} mvUsbGetDescInfo;

/*===========================================================================

            EXTERN DECLARATIONS FOR MODULE

===========================================================================*/

/* Usb net response Queue head. */
extern mvUsbNetQ UsbNetRspQHdr;

/*===========================================================================

                        EXTERN FUNCTION DECLARATIONS

===========================================================================*/

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mem_malloc                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function malloc memory from Tcp/IP pool.                    */
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
/*      size                                Memory size                  */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
#if 0
extern void *mem_malloc(unsigned int size);
#endif

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mem_free                                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function free memory from Tcp/IP pool.                      */
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
/*      rmem                                Memory address               */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
#if 0
extern void mem_free(void *rmem);
#endif

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      USB2ReEnumerate                                                  */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function re-enumerate usb device.                            */
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
/*      Desc                                Usb descriptor               */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
extern void USB2ReEnumerate(PlatformUsbDescType Desc);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetGetPacket                                                */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function process the rndis packet and send to LWIP.          */
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
#if 0
signed char mvUsbNetGetPacket(UINT8 *data, UINT32 len, UINT8 usb_or_lte, UINT8 *org_buf_head);
#endif

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      get_wifi_mac                                                     */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function get the MAC address of WIFI.                        */
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
/*      wifi_mac                            The WIFI MAC address         */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
int netifapi_get_wifi_macaddr(UINT8 *mac_addr);

/*===========================================================================

                          INTERNAL FUNCTION DECLARATIONS

===========================================================================*/
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
void mvUsbNetMemInit (void);

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
void mvUsbNetFreeTxBuffer(void);

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
BOOL mvUsbNetIfEnable(void);

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
void mvUsbNetInit (void);

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
void mvUsbNetResetRingBuffer(void);

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
void mvUsbNetResetRspQ (void);

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
void mvUsbNetResetMsgQ (void);

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
void mvUsbNetResetParameters (void);

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
void mvUsbNetParasInit (void);

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
void mvUsbNetPrintMemInfo (void);

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
void mvUsbNetTaskInit (void);

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
void mvUsbNetHISRInit (void);

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
void mvUsbNetEndpointInit (void);

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
void mvUsbNetReqTask (void *input);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNetCtrlLISR                                                 */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The usb net Low-Level Interrupt Service Routine(LISR).          */
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
void mvUsbNetCtrlLISR(mvUsbNetType type ,UINT8 *data ,UINT32 len);

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
void mvUsbNetCtrlHISR(void);

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
void mvUsbNetRxHISR(void);

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
                           UINT32 dataRecLen ,BOOL endOfRecMsg);

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
void mvUsbNetTxHISR(void);

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
                                UINT32 dataRecLen ,BOOL dummy);

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
                                    UINT32 dataRecLen ,BOOL dummy);

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
/*      mvUsbNetQ                           usb net queue                */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      TRUE                                Valid resonse queue          */
/*      FALSE                               Invalid resonse queue        */
/*                                                                       */
/*************************************************************************/
BOOL mvUsbNetCheckRspQValidity(mvUsbNetQ *queue);

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
/*      mvUsbNetQ                           Queue head pointer           */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void mvUsbNetQInit(mvUsbNetQ *hdr);

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
/*      hdr                                 The response head pointer    */
/*      pRspQ                               The response queue pointer   */
/*      len                                 the response data length     */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void mvUsbNetQPut(mvUsbNetQ *hdr, mvUsbNetQ *queue);

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
mvUsbNetQ *mvUsbNetQGet(mvUsbNetQ *hdr);

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
int mvUsbGetTxNetQCnt(void);

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
                                        UINT8* msgPtr, UINT32 msgLength);

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
void mvUsbNetSendPacket(UINT8* data, UINT32 len);

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
void mvUsbNetTxMemFree(void *buf);

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
void mvUsbNetRxMemFree(void *buf);

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
mvUsbNetType mvUsbGetNetType(void);

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
void mvUsbSetNetType(mvUsbNetType type);

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
mvUsbGetDescInfo *mvUsbGetDescriptorInfo(void);

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
mvUsbNetQ* mvUsbNetCreateTxNode(UINT8* data, UINT32 len);

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
void mvUsbNetTxEnqueue(UINT8* data, UINT32 len);

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
BOOL mvUsbNetTxDequeueAndTransfer(void);

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
void mvUsbNetLwipSendPacket(UINT8* data, UINT32 len);

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
mvUsbNetParams *mvUsbGetNetParams(void);

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
UINT8 mvUsbNetMacCmp(UINT8 *mac);

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
/*      mac                                 The compared mac address     */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      address                             mac address                  */
/*                                                                       */
/*************************************************************************/
UINT8 *mvUsbNetGetMacAddress(void);

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
void mvUsbNetInitMacAddress(void);

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
void mvUsbNetSwitchTo(mvUsbNetType type);

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
BOOL mvUsbNetRemoveHdr(UINT8 **data, UINT32 *msgLen, UINT32 *dataLen);

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
void mvUsbNetIndicateStatus(BOOL connected);
void mvUsbQueueRXPacketTask(void *arg);
#endif /* _MVUSBNET_H_ */
