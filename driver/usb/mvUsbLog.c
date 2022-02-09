/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                mvUsbLog.c


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

#include "mvUsbLog.h"
//#include "FDI_TYPE.h"
//#include "FDI_FILE.h"
//#include "nvm_header.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* utility log */
static mvUsbLogInfo mvUsbLog;

/* Usb trace flag*/
BOOL mvUsbTraceEnable = FALSE;

/* Duster Pool array */
#pragma arm section rwdata="UsbLog", zidata="UsbLog"
__align(32) UINT8 UsbLogMemory[USB_LOG_LENGTH] = {0};
#pragma arm section rwdata, zidata

/*===========================================================================

            EXTERN DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* Dump uart log to FS */
BOOL bDumpErrLogToFs;

/* FAT system Semaphore*/
extern OSSemaRef FatSysRef;

/*===========================================================================

                          INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/***********************************************************************
*
* Name:        mvUsbGetTraceMode
*
* Description: This function get USB trace mode.
*
* Parameters:
*  void
*
* Returns:
*  OSA_STATUS  OSA Complition Code.
*
* Notes:
*
***********************************************************************/
void mvUsbGetTraceMode(void)
{
    //if(rti_get_mode() == rti_usbtrace_mode)
    //{
       // mvUsbTraceEnable = TRUE;
    //}
    //else
    //{
        mvUsbTraceEnable = FALSE;
    //}
}

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
mvUsbResultCode mvUsbLoggingInit(void)
{
    mvUsbLogInfo *info = mvUsbLogGetInfo();
    uart_printf("%s-01\r\n",__func__);
    /* Get Usb trace mode. */
    mvUsbGetTraceMode();

    /* If diag is already initialized, do nothing. */
    if (info->Init_Flag == USB_LOG_INIT_DONE)
    {
        /* Unlock diag comm FS mutex.*/
        return USB_LOG_ALREADY_INIT;
    }

    /* Initialize usb log information. */
    memset(info, 0x00, sizeof(mvUsbLogInfo));

    /* alloc memory to save log. */
#if 0
    info->Log_Begin  = (unsigned int)malloc(USB_LOG_LENGTH);
#else
    info->Log_Begin  = (unsigned int)UsbLogMemory;
#endif

    /* Initialize log memory. */
    memset((UINT32 *)info->Log_Begin, 0x00, USB_LOG_LENGTH);

    /* Set init flag to done. */
    info->Init_Flag  = USB_LOG_INIT_DONE;
    uart_printf("%s-02\r\n",__func__);
    return USB_LOG_RESULT_OK;

}

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
mvUsbResultCode mvUsbLoggingUninit(void)
{
    mvUsbLogInfo *info = mvUsbLogGetInfo();

    /* If diag is not initialized, do nothing. */
    if (info->Init_Flag != USB_LOG_INIT_DONE)
    {
        return USB_LOG_NOT_INIT;
    }

    /* Initialize diag comm sd log information. */
    memset(info, 0x00, sizeof(mvUsbLogInfo));

    return USB_LOG_RESULT_OK;
}

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
mvUsbLogInfo *mvUsbLogGetInfo(void)
{
    return &mvUsbLog;
}

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
void mvUsbLogRecord(char* data, unsigned int length)
{
    UINT32 cpsr;
    UINT32 SaveSize = 0;
    mvUsbLogInfo *info = mvUsbLogGetInfo();

     /* If diag is not initialized, do nothing. */
    if (info->Init_Flag != USB_LOG_INIT_DONE)
    {
        return;
    }

    /* If log buffer is invalid, do nothing. */
    if(info->Log_Begin == 0)
    {
        return;
    }

    ASSERT ((data != NULL)&&(length != 0));

    cpsr = disableInterrupts();

    ASSERT (info->Log_Index < USB_LOG_LENGTH);

    if ((info->Log_Index + length) >= USB_LOG_LENGTH)
    {
        SaveSize = USB_LOG_LENGTH - info->Log_Index;
        memcpy((void *)(info->Log_Begin + info->Log_Index), data, SaveSize);
        data   = data + SaveSize;
        length = length - SaveSize;

        info->Rota_Flag = USB_LOG_ROTATION_FLAG;
        info->Log_Index = 0;
    }

    if (length != 0)
    {
        memcpy((void *)(info->Log_Begin + info->Log_Index), data, length);
        info->Log_Index += length;

        if(info->Log_Index == USB_LOG_LENGTH)
        {
            info->Log_Index = 0;
        }
    }

    restoreInterrupts(cpsr);
}

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
void mvUsbLogSaveToFileSystem(void)
{
    UINT32 count = 0;
    UINT32 SaveSize = 0;
    mvUsbLogInfo *info = mvUsbLogGetInfo();

    /* If diag is not initialized, do nothing. */
    if (info->Init_Flag != USB_LOG_INIT_DONE)
    {
        return;
    }

    /* If log buffer is invalid, do nothing. */
    if (info->Log_Begin == 0)
    {
        return;
    }

    /* Log index must smaller than log length. */
    if (info->Log_Index > USB_LOG_LENGTH)
    {
        return;
    }

    /* Stop recording log. */
    info->Init_Flag = 0;
#if 0
    /* Save log to file system. */
    if (bDumpErrLogToFs)
    {
        FILE_ID    fdiID;

        /* Get FatSysRef semaphore count. */
        OSASemaphorePoll(FatSysRef, &count);
        if(count == 0)
        {
            //MIFI_LOG_TRACE(MIFI, LOG, SaveToFS, "EE LOG: FAT mutex lock");
            return;
        }

        if((fdiID = FDI_fopen("com_errlog.bin", "wb"))!=0)
        {
            if(info->Rota_Flag != USB_LOG_ROTATION_FLAG)
            {
               /* Save log to fs. */
               count = FDI_fwrite((void *)info->Log_Begin, sizeof(char), info->Log_Index, fdiID);
               if(count != info->Log_Index)
               {
                   return;
               }
            }
            else
            {
                /* Save the data before rotation to fs. */
                if(info->Log_Index < USB_LOG_LENGTH)
                {
                    SaveSize = USB_LOG_LENGTH - info->Log_Index;

                    count = FDI_fwrite((void *)(info->Log_Begin + info->Log_Index), sizeof(char), SaveSize, fdiID);
                    if(count != SaveSize)
                    {
                        return;
                    }
                }

                /* Save the data after rotation to fs. */
                if(info->Log_Index != 0)
                {
                    SaveSize = info->Log_Index;

                    count = FDI_fwrite((void *)info->Log_Begin, sizeof(char), SaveSize, fdiID);
                    if(count != SaveSize)
                    {
                        return;
                    }
                }
            }
        }

        /* close file */
        FDI_fclose(fdiID);
    }
#endif
    /* Recover init flag*/
    info->Init_Flag = USB_LOG_INIT_DONE;
}
