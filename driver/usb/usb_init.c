/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                usb_init.c


GENERAL DESCRIPTION

    This file is for USB initialize.

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
03/07/2013   zhoujin    Created module
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "usb_init.h"
#include "mvUsbNet.h"
#include "mvUsbLog.h"
#ifdef MV_USB2_MASS_STORAGE
#include "FlashPartition.h"
#endif
#include "platform.h"
#include "os_hal.h"
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* The USB CDC support flag */
BOOL USBCDCSupport = 0;

/* RNDIS multiple packet support flag */
BOOL RndisMultiplePacket = FALSE;

/* The Usb Select Task Reference */
OSTaskRef UsbSelectTaskRef;

/* The Usb connect flag */
OSAFlagRef usb_connect_flag;

/* Mass storage capacity. */
 MASS_STORAGE_DEVICE_INFO_STRUCT CDROM_device_information_data = {
    0x05,   //CD-ROM device
    0x80,   //removable
    0x00,   //ANSI-approved version
    0x02,   //SCSI INQUIRY data format : SCSI-2 standard
    0x20,   //Additional length
    0,
    0,
    0,
   /* Vendor information: "MARVELL.. " */
   {0x4D, 0x41, 0x52, 0x56, 0x45, 0x4C, 0x4C, 0,},
   /* Product information: "Mobile CMCC CD.." */
   {0x4d, 0x6f, 0x62, 0x69, 0x6c, 0x65, 0x20, 0x43,
   0x4d, 0x43, 0x43, 0x20, 0x43, 0x44, 0, 0},
   /* Product Revision level: "1.25" */
   {0x31, 0x2e, 0x32, 0x35}
};

/* AZW CDROM device information. */
 MASS_STORAGE_DEVICE_INFO_STRUCT AZW_CDROM_device_information_data = {
    0x05,   //CD-ROM device
    0x80,   //removable
    0x00,   //ANSI-approved version
    0x02,   //SCSI INQUIRY data format : SCSI-2 standard
    0x20,   //Additional length
    0,
    0,
    0,
   /* Vendor information: "PRIMEMOB.. " */
   {0x50, 0x52, 0x49, 0x4D, 0x45, 0x4D, 0x4F, 0x42},
   /* Product information: "PD508_CDROM" */
   {0x50, 0x44, 0x35, 0x30, 0x38, 0x5F, 0x43, 0x44,
   0x52, 0x4F, 0x4D, 0x0, 0x0, 0x0, 0, 0},
   /* Product Revision level: "1.25" */
   {0x31, 0x2e, 0x32, 0x35}
};

/*===========================================================================

            EXTERN DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* Mass storage media type */
extern USBMSC_MEDIA mvUsbMscMedia[];

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
extern BOOL diag_comm_sd_init(void);
extern void mvUsbModemUartInitialize(void);

/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

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
UINT32 mvUsbGetRndisbitrate(void)
{
    return 10 * 1000 * 1000;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbCheckProdctionMode                                          */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function check whether the production mode is enable or not. */
/*      When uAP KEY is pressdown, The 'PRMD' flag is set in memory.     */
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
/*      TRUE                                Flag is set.                 */
/*      FALSE                               Flag is not set.             */
/*                                                                       */
/*************************************************************************/
BOOL mvUsbCheckProdctionMode( void )
{

#if 0
    unsigned int flag =  *(volatile unsigned int *)DDR_USBMODE_FLAG_ADDR;

    if( flag == 0x50524D44 )
    {
        /* Clear the ProductionMode flag in the memory. */
        *(volatile unsigned int *)DDR_USBMODE_FLAG_ADDR = 0;

        return TRUE;
    }
    else
    {
        return FALSE;
    }
#else
    return FALSE;
#endif
}

#ifdef MV_USB2_MASS_STORAGE
//extern FlashLayoutConfInfo *GetFlashLayoutConfig(void);
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbStorageConfigure                                            */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function configurate the parameters of mass storage.         */
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
void mvUsbStorageConfigure(void)
{
    Usb_DriverS *pUsbDrvConfig = GetUsbDriverConfig();
    mvUsbMscProperties_T *pMscProp = GetMscProperties();
    //mvUsbDescriptorInfo *pUsbDesInfo = USB2GetDescriptorInfo();
    //FlashLayoutConfInfo *pFlashLayout = GetFlashLayoutConfig();

    memset(pMscProp, 0x00, sizeof(mvUsbMscProperties_T));

    /* Initialize MSC0 to invalid media type. */
    pMscProp->Media[0]  = USBMSC_MEDIA_U32;

    #ifndef CRANE_SD_NOT_SUPPORT
    if (sdcard_is_insert())
    {
        pMscProp->MscMaxLun = 0;

        /* Set the current Logical Unit disc to SD-Disk. */
		pMscProp->Media[pMscProp->MscMaxLun] = USBMSC_SDCARD;
    }
	#endif
    /* If MSC0 is invalid media, We should disable mass storage. */
    if (pMscProp->Media[0] == USBMSC_MEDIA_U32)
    {
        pUsbDrvConfig->mass_storage = MASS_STORAGE_DISABLE;
    }

    uart_printf("mass_storage %d, MscMaxLun %d, MscStartAddress [0x%x, 0x%x, 0x%x]\r\n",
                pUsbDrvConfig->mass_storage, pMscProp->MscMaxLun, pMscProp->Lun0StartAddress,
                pMscProp->Lun1StartAddress,  pMscProp->Lun2StartAddress);
    uart_printf("%s-01\r\n",__func__);
    ASSERT((pMscProp->MscMaxLun)< MSC_MAX_LUN);
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbMassStorageInit                                             */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function initialize the mass storage deivce.                 */
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
void mvUsbMassStorageInit(void)
{
    /* USB mass storage initialize. */
    mvUsbStorage_Init();
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbStorageRead                                                 */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function read data from mass storage.                        */
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
BOOL mvUsbStorageRead(unsigned int address, unsigned int length, unsigned char *buf, unsigned char lun)
{
    BOOL status = TRUE;
    unsigned int StartAddress = 0;
    mvUsbMscProperties_T *pMscProp = GetMscProperties();

    switch(lun)
    {
        case 0:
        {
            StartAddress = pMscProp->Lun0StartAddress;
            break;
        }

        case 1:
        {
            StartAddress = pMscProp->Lun1StartAddress;
            break;
        }

        case 2:
        {
            StartAddress = pMscProp->Lun2StartAddress;
            break;
        }

        default:
        {
            status = FALSE;
            StartAddress = pMscProp->Lun0StartAddress;
            break;
        }
    }

    if (status)
    {
        switch(pMscProp->Media[lun])
        {
            case USBMSC_CDROM:
            case USBMSC_FLASHDISK:
            {
                //if (NAND_ReadDirect(StartAddress + address, buf, 0, length) != 0)
                {
                    status = FALSE;
                }
                break;
            }

            default:
            {
                status = FALSE;
                break;
            }
        }
    }

    return status;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbStorageWrite                                                */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function write data to mass storage.                         */
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
BOOL mvUsbStorageWrite(unsigned int address, unsigned int length, unsigned char *buf, unsigned char lun)
{
    BOOL status = TRUE;
    unsigned int StartAddress = 0;
    mvUsbMscProperties_T *pMscProp = GetMscProperties();

    switch(lun)
    {
        case 0:
        {
            StartAddress = pMscProp->Lun0StartAddress;
            break;
        }

        case 1:
        {
            StartAddress = pMscProp->Lun1StartAddress;
            break;
        }

        case 2:
        {
            StartAddress = pMscProp->Lun2StartAddress;
            break;
        }

        default:
        {
            status = FALSE;
            StartAddress = pMscProp->Lun0StartAddress;
            break;
        }
    }

    if (status)
    {
        switch(pMscProp->Media[lun])
        {
            case USBMSC_FLASHDISK:
            {
                //if (NAND_WriteDirect(StartAddress + address, buf, 0, length) != 0)
                {
                    status = FALSE;
                }
                break;
            }

            default:
            {
                status = FALSE;
                break;
            }
        }
    }

    return status;
}
#endif /* MV_USB2_MASS_STORAGE */

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbSelectTaskInit                                              */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function intializes the usb select task.                     */
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
void mvUsbSelectTaskInit (void)
{
    OSA_STATUS	status;
    void *UsbSelectTaskStack = NULL;

    /* Usb net request task stack initialize. */
    UsbSelectTaskStack = malloc(USB_SELECT_TASK_STACK_SIZE);
    ASSERT(UsbSelectTaskStack != NULL);

    /* Create Usb connect flag. */
    status = OSAFlagCreate(&usb_connect_flag);
    ASSERT(status == OS_SUCCESS);

    /* Create Usb select task. */
    status = OSATaskCreate(&UsbSelectTaskRef,
                            UsbSelectTaskStack,
                            USB_SELECT_TASK_STACK_SIZE,
                            70,
                            "USBSelectT",
                            USBSelectTask,
                            NULL);
	if(status != OS_SUCCESS)
	{
		free(UsbSelectTaskStack);
		ASSERT(0);
	}

}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      usb_device_init                                                  */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function initialize the usb devices.                         */
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
UINT32 usbInitComplete = 0;
void usb_device_init(void )
{
    UINT32 CPSR;
    Usb_DriverS *pUsbDrvConfig = GetUsbDriverConfig();
    mvUsbDescriptorInfo *pUsbDesInfo = USB2GetDescriptorInfo();
    OS_STATUS   status;

    /* Get the default usb configuration. */
    Usb_driver_typeE usb_driver = pUsbDrvConfig->usb_driver;

    /* Get the setting of usb driver. */
    UsbDriverGetSetting();

    /* disable Interrupts. */
    CPSR = disableInterrupts();

    /* set usb descriptor according to usb setting. */
    switch(pUsbDrvConfig->usb_driver)
    {
        case USB_GENERIC_MIFI_DRIVER:
        {
            pUsbDesInfo->DefaultDesc = USB_GENERIC_MIFI_DESCRIPTOR;
            USBCDCSupport = 1;
            break;
        }

        case USB_MARVELL_MIFI_DRIVER:
        {
            pUsbDesInfo->DefaultDesc = USB_MARVELL_MIFI_DESCRIPTOR;
            break;
        }

        case USB_ASR_MIFI_DRIVER:
        {
            pUsbDesInfo->DefaultDesc = USB_ASR_MIFI_DESCRIPTOR;
            break;
        }

		case USB_ASR_MBIM_DRIVER:
		{
			pUsbDesInfo->DefaultDesc = USB_MBIM_DESCRIPTOR;
			USBCDCSupport = 1;
			break;
		}

        case USB_CDROM_ONLY_DRIVER:
        {
            pUsbDesInfo->DefaultDesc = USB_CDROM_ONLY_DESCRIPTOR;
            break;
        }

		case USB_CDROM_DIAG_DRIVER:
		{

			pUsbDesInfo->DefaultDesc = USB_CDROM_DIAG_DESCRIPTOR;
			break;
		}

		case USB_DIAG_ONLY_DRIVER:
		{

			pUsbDesInfo->DefaultDesc = USB_DIAG_ONLY_DESCRIPTOR;
			break;
		}

		case USB_MODEM_ONLY_DRIVER:
        {
            pUsbDesInfo->DefaultDesc = USB_MODEM_ONLY_DESCRIPTOR;
            break;
        }

        case USB_MODEM_DIAG_DRIVER:
        {
            pUsbDesInfo->DefaultDesc = USB_MODEM_DIAG_DESCRIPTOR;
            break;
        }

        default:
        {
            pUsbDesInfo->DefaultDesc = USB_ASR_MIFI_DESCRIPTOR;
            break;
        }
    }

     /* Initialize mass storage configuration. */
#ifdef MV_USB2_MASS_STORAGE
    if(pUsbDrvConfig->usb_driver != usb_driver)
#endif
    {
        pUsbDrvConfig->mass_storage = MASS_STORAGE_DISABLE;
        pUsbDrvConfig->auto_install = USB_AUTO_INSTALL_DISABLE;
    }

    uart_printf("%s-01\r\n",__func__);
   // if (PlatformSpiNorEnable() && (!CheckIf32MNOR()))
    //{
        //pUsbDrvConfig->auto_install = USB_AUTO_INSTALL_DISABLE;
    //}

    /* Reset Usb descriptor type. */
    if (USB2IsAutoInstallEnable())
    {
        pUsbDesInfo->ReConfigDesc = USB_CDROM_ONLY_DESCRIPTOR;
    }
    else
    {
        pUsbDesInfo->ReConfigDesc = pUsbDesInfo->DefaultDesc;
    }

    /* Set current usb Mode. */
    pUsbDesInfo->UsbMode = GetUsbMode();
    uart_printf("%s-02,%d\r\n",__func__,pUsbDesInfo->UsbMode);
    /* Check production Mode. */
	if(!mvUsbCheckProdctionMode())
	{
    	switch(pUsbDesInfo->UsbMode)
    	{
    		case USB_WIFI_CALIBRATION_MODE:
    		{
    			break;
    		}


    		case USB_RNDIS_ECM_ONLY_MODE:
    		{
    			pUsbDesInfo->ReConfigDesc = USB_RNDIS_ONLY_DESCRIPTOR;

    			break;
    		}

    		default:
    		{
    			break;
    		}
    	}
	}


    uart_printf("%s-03\r\n",__func__);
    /* Usb device log initialize. */
    mvUsbLoggingInit();
    uart_printf("%s-03-01,%d\r\n",__func__,pUsbDesInfo->ReConfigDesc);
    USB2MgrUpdateDescriptor(pUsbDesInfo->ReConfigDesc, pUsbDesInfo->UsbMode);
    uart_printf("%s-04\r\n",__func__);
    /* Usb device phase 2 initialize. */
    USBDevicePhase2Init();
#ifdef MV_USB2_MASS_STORAGE
    mvUsbMassStorageInit();
#endif
    uart_printf("%s-05\r\n",__func__);
    /* Rndis device initialize. */
    mvUsbNetInit();
    uart_printf("%s-06\r\n",__func__);
    /* Modem device initialize. */
    mvUsbModemInitialize();

    /* Diag device initialize. */
    //diagPhase2Init();

    uart_printf("%s-07\r\n",__func__);

    /* Restore Interrupts. */
    restoreInterrupts(CPSR);

    /* Initialize diag SD logging. */

    //diag_comm_sd_init();
    uart_printf("%s-08\r\n",__func__);
    /* Usb select task initialize. */
    mvUsbSelectTaskInit();
    uart_printf("%s-09\r\n",__func__);
    *(volatile unsigned long*)0xD428287C |= 0x1<<11; //enable usb wakeup source
    usbInitComplete = 1;
    uart_printf("Usb mode %u, Usb descriptor %u\r\n", pUsbDesInfo->UsbMode, pUsbDesInfo->ReConfigDesc );
    return;
}
