#ifndef _MVUSBLOG_H_
#define _MVUSBLOG_H_
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                mvUsbLog..h


GENERAL DESCRIPTION

    This file is for usb log.

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
10/22/2013   zhoujin    Created module
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "common.h"
//#include "osa.h"
//#include "diag.h"
//#include "UART.h"
#include <stdlib.h>
#include <string.h>
//#include "platform.h"
#include "utilities.h"

/*===========================================================================

                                LOCAL MACRO
===========================================================================*/

/* Usb log initialize flag. */
#define USB_LOG_INIT_DONE               0xEBEDBEDE

/* Usb log rotation flag. */
#define USB_LOG_ROTATION_FLAG           0xDEAFBEEF

/* Usb log length. */
#define USB_LOG_LENGTH                  0x00008000

/* Usb max trace length. */
#define USB_MAX_TRACE_LEN               128

/* Usb trace enable flag. */
#define USB_LOG_ENABLE                  mvUsbTraceEnable

/* Usb trace enable flag. */
#define USB_TRACE(fmt,args...)                                           \
{                                                                        \
    if (USB_LOG_ENABLE)                                                  \
    {                                                                    \
        uart_printf(fmt"\r\n", ##args);                                  \
    }                                                                    \
}

/*===========================================================================

                          Struct definition.

===========================================================================*/

/* Utility result code */
typedef enum mvUsbResultCode
{
    USB_LOG_RESULT_OK,
    USB_LOG_DISABLE,
	USB_LOG_NOT_INIT,
	USB_LOG_INVALID_DIR,
	USB_LOG_INVALID_NAME,
	USB_LOG_ALREADY_INIT,
	USB_LOG_NO_FREE_SPACE,
	USB_LOG_INVALID_LENGTH,
	USB_LOG_INVALID_BUFFER,
	USB_LOG_OPEN_ERROR,
	USB_LOG_WRITE_ERROR,
	USB_LOG_OTHER_ERROR
}mvUsbResultCode;

/* Utility log information */
typedef struct mvUsbLogInfo{
	UINT32      Log_Begin;
	UINT32      Log_Index;
	UINT32      Rota_Flag;
	UINT32      Init_Flag;
	UINT16      UniName[30];
	char        LogName[40];
}mvUsbLogInfo;

/*===========================================================================

            EXTERN DECLARATIONS FOR MODULE

===========================================================================*/
/*===========================================================================

            EXTERN DECLARATIONS FOR MODULE

===========================================================================*/
extern BOOL mvUsbTraceEnable;

/*===========================================================================

                        EXTERN FUNCTION DECLARATIONS

===========================================================================*/
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      sd_log_index_get                                                 */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function get the sd log index.                              */
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
extern UINT32 sd_log_index_get(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      sdcard_is_ready                                                  */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function get the status of SD card.                          */
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
extern BOOL sdcard_is_ready(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      DumpFileToSDEnable                                               */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function Check whether dump bin files to SD card is enable  */
/*      or not.                                                          */
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
extern BOOL DumpFileToSDEnable(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      MacCodeToUniCode                                                 */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function switch mac code to unicode.                        */
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
extern int MacCodeToUniCode(UINT16 *Out, int nLen, char *strMac);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      FS_Open                                                          */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function Open or create a binary file for read/write.       */
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
extern void * FS_Open(UINT16 * lpszFileName, UINT32 nMode);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      FS_Read                                                          */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function Read data from a file.                             */
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
extern UINT32 FS_Read(void * hFile, void * lpBuf, UINT32 nLen);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      FS_Read                                                          */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function write data to a file.                              */
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
extern UINT32  FS_Write(void* hFile, void * lpBuf, UINT32 nLen);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      FS_Seek                                                          */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function Move file pointer to specified location.           */
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
extern UINT32  FS_Seek(void* hFile, int pos, UINT32 seek_mode);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      FS_Close                                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function Close a file.                                      */
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
extern UINT32  FS_Close(void* hFile);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      CreateDirectory                                                  */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function create a directory.                                */
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
extern UINT32 CreateDirectory(UINT16 *wsPathName);

/*===========================================================================

                          INTERNAL FUNCTION DECLARATIONS

===========================================================================*/
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbLoggingInit                                                 */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function initialize the usb logging.                         */
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
/*      Result                              Result code                  */
/*                                                                       */
/*************************************************************************/
mvUsbResultCode mvUsbLoggingInit(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbLoggingUninit                                               */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function uninitialize the usb logging.                       */
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
/*      Result                              Result code                  */
/*                                                                       */
/*************************************************************************/
mvUsbResultCode mvUsbLoggingUninit(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbLogGetInfo                                                  */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function get the usb log information.                        */
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
mvUsbLogInfo *mvUsbLogGetInfo(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbLogRecord                                                   */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function record the usb log to ring buffer.                  */
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
/*      Data                                Diag message                 */
/*      Length                              Message length               */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void mvUsbLogRecord(char* data, unsigned int length);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbLogSaveToFileSystem                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function save the usb log to file system.                    */
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
void mvUsbLogSaveToFileSystem(void);
#endif
