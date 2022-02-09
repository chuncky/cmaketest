#ifndef _MVUSBMODEM_H_
#define _MVUSBMODEM_H_
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                mvUsbModem.h


GENERAL DESCRIPTION

    This file is for USB modem Device.

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

//#include "osa.h"
//#include "diag.h"
//#include "UART.h"
#include <stdlib.h>
#include <string.h>
//#include "bsp_hisr.h"
//#include "telatci.h"
//#include "telcontroller.h"
//#include "teldef.h"
#include "usb_device.h"
#include "usb_device_types.h"
#include "usb_config.h"
#include "usb_init.h"

/*===========================================================================

                                LOCAL MACRO
===========================================================================*/

/* Motify serial state length. */
#define mvUsbNotifySerialStateLen               10

/* Modem Rx number. */
#define mvUsbModemRxNum                         32

/* Modem control number. */
#define mvUsbModemCtrlNum                       16

/* Modem Tx number. */
#define mvUsbModemTxNum                         64

/* AT point number. */
#define mvUsbATPNum                             3

/* Modem Rx size. */
#define mvUsbModemRxSize 	                    256

/* Modem 0 Rx number. */
#ifdef CRANE_MCU_DONGLE
#define mvUsbModem0RxNum                        8
#else
#define mvUsbModem0RxNum                        16
#endif

/* Modem Rx size. */
#define mvUsbModem0MaxRxSize 	                (mvUsbModemRxSize * mvUsbModem0RxNum)

/* Invalid ticks. */
#define mvUsbInvalidTicks                       0xFFFFFFFF

/*===========================================================================

                          Struct definition.

===========================================================================*/

/* Usb Modem request type */
typedef enum
{
	MODEM_EP0_SERIALSTATE = 0,
	MODEM_EP0_SETLINECODE = 1,
	MODEM_EP0_NO_REQ
}mvUsbModemReq;

/* Usb Modem message ring buffer */
typedef struct mvUsbModemMsg{
	UINT32      Ep;
	UINT32      len;
	UINT8      *ptr;
}mvUsbModemMsg;

/* Usb Modem parameters */
typedef struct mvUsbModemPara{
	UINT8       CtrlEndPoint;
	UINT8       RxEndPoint;
	UINT8       TxEndPoint;
	UINT8       Interface;
}mvUsbModemPara;

/*===========================================================================

            EXTERN DECLARATIONS FOR MODULE

===========================================================================*/


/*===========================================================================

                        EXTERN FUNCTION DECLARATIONS

===========================================================================*/
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      sendData2SACReq                                                  */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function send request to SAC.                               */
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
extern void sendData2SACReq(unsigned char cid, char *buf, int len);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      useYmodem                                                        */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function get ymodem flag.                                   */
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
extern BOOL useYmodem(void);

/*===========================================================================

                          INTERNAL FUNCTION DECLARATIONS

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
void mvUsbSetModemParameters(UINT8 sATP, mvUsbModemPara *para);

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
UINT8 mvUsbGetCommFeature(UINT32 interface);

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
void mvUsbSetCommFeature(UINT32 interface,UINT8 Feature);

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
void mvUsbClearCommFeature(UINT32 interface);

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
void mvUsbNotifySerialState(UINT8 interface, UINT8 bmp0, UINT8 bmp1,UINT8 orig);

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
void mvUsbCommSetLineCoding(UINT8 interface, UINT8* inLineCode);

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
void mvUsbModemClearDummyFlag(void);

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
void mvUsbCommGetLineCoding(UINT8 interface, UINT8* outLineCode);

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
void mvUsbNotifySerialStatereq(UINT8 interface, UINT8 bmp0, UINT8 bmp1,UINT8 orig);

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
void mvUsbCommNotifyControlLineSt(UINT8 interface, UINT16 LineStatus);

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
void mvUsbModemInitialize(void);
void mvUsbModemUartInitialize(void);
void mvUsbModemUartRxHISR(void);
void mvUsbModemUartTxHISR(void);
void mvUsbModemUartCtrlHISR(void);
void mvUsbModemUartTxLISR(USBDevice_EndpointE ep ,UINT8 *data ,UINT32 Len ,BOOL dummy);

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
void mvUsbModem0CtrlHISR(void);

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
void mvUsbModem0CtrlLISR(USBDevice_EndpointE ep ,UINT8 *data ,UINT32 Len ,BOOL dummy);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbModem0RxHISR                                                */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The modem 0 Rx Low-Level Interrupt Service Routine(HISR).        */
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
void mvUsbModem0RxHISR(void);
void mvUsbModem1RxHISR(void);

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
void mvUsbModem0RxLISR(USBDevice_EndpointE ep ,UINT8 *data ,UINT32 Len ,BOOL end);
void mvUsbModem1RxLISR(USBDevice_EndpointE ep ,UINT8 *data ,UINT32 Len ,BOOL end);

void mvUsbModemUartRxLISR(USBDevice_EndpointE ep ,UINT8 *data ,UINT32 Len ,BOOL end);

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
void mvUsbModem0TxHISR(void);
void mvUsbModem1TxHISR(void);

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
void mvUsbModem0TxLISR(USBDevice_EndpointE ep ,UINT8 *data ,UINT32 Len ,BOOL dummy);
void mvUsbModem1TxLISR(USBDevice_EndpointE ep ,UINT8 *data ,UINT32 Len ,BOOL dummy);

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
void mvUsbModem1CtrlHISR(void);

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
void mvUsbModem1CtrlLISR(USBDevice_EndpointE ep ,UINT8 *data ,UINT32 Len ,BOOL dummy);

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
void  mvUsbModemSendData(UINT8 sATP,UINT8 *bufPtr, UINT32 length);

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
void  mvUsbModemSendDataYmodem(UINT8 sATP,UINT8 *bufPtr, UINT32 length);

#ifdef EXT_AT_MODEM_SUPPORT
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbModem2SendData                                               */
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
void  mvUsbModem2SendData(UINT8 sATP,UINT8 *bufPtr, UINT32 length);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNotifySerialState_DCD                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      Usb Notify Serial State_DCD.                                     */
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
void mvUsbNotifySerialState_DCD(UINT8 interface, UINT8 level);


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbNotifySerialState_DCD                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      Usb Notify Serial State RI.                                      */
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
void mvUsbNotifySerialState_RI(UINT8 interface, UINT8 level);

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
BOOL mvUsbQueryATPort(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbQueryATPort1                                                */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function query AT Port1 Status.                              */
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
BOOL mvUsbQueryATPort1(void);


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
void mvUsbChangeAtInterface(void);
void  mvUsbModemSendDataYmodem(UINT8 sATP,UINT8 *bufPtr, UINT32 length);
void  modemSendDataSPI(UINT8 sATP,UINT8 *bufPtr, UINT32 length);

#endif
