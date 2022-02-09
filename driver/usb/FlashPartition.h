#ifndef __FLASHPARTITION_H__
#define __FLASHPARTITION_H__

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                FlashPartition.h


GENERAL DESCRIPTION

    This file is for FLash layout.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2013 by Marvell, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who        what, where, why
--------   ------     ----------------------------------------------------------
03/18/2013   zhoujin    Created module
===========================================================================*/

/*===========================================================================

            LOCAL MACRP DEFINITIONS FOR MODULE

===========================================================================*/

/* The nand sector size */
#define NAND_SECTOR_SIZE				    512

/* The nand sector shit bit */
#define FLASH_SECTOR_BITSHIFT               9

/* The unused flash address */
#define UNUSED                              0xFFFFFFFF

/*===========================================================================

            TYPE DEFINITIONS FOR MODULE

===========================================================================*/
#if 0
/*Flash Zone IDs*/
typedef enum
{
    FLASHZONE_LUN_0     = 0,
    FLASHZONE_LUN_1     = 1,
    FLASHZONE_LUN_2     = 2,
    FLASHZONE_LUN_MAX   = 0x7FFFFFFF
} FLASH_ZoneID;
#endif
/* Flash block cache type. */
typedef struct FlashBlkCacheType
{
    unsigned int        blocknum;
    unsigned int        dirty;
    unsigned char      *BlockBuf;
} FlashBlkCacheType;

/* Flash partition struct. */
typedef struct FlashLayoutConfInfo {
    unsigned int        OBMStartAddress;
    unsigned int        OBMEndAddress;
    unsigned int        CPStartAddress;
    unsigned int        CPEndAddress;
    unsigned int        MSAStartAddress;
    unsigned int        MSAEndAddress;
    unsigned int        MRDStartAddress;
    unsigned int        WEBStartAddress;
    unsigned int        WEBEndAddress;
    unsigned int        WIFIFirmAddress;
    unsigned int        WIFICalFirmAddress;
    unsigned int        PORRFStartAddress;
    unsigned int        PORRFEndAddress;
    unsigned int        SbliStartAddress;
    unsigned int        MoggStartAddress;
    unsigned int        ParaStartAddress;
    unsigned int        APNListStartAddress;
    unsigned int        FSysMapTabAddress;
    unsigned int        FATStartAddress;
    unsigned int        FATEndAddress;
    unsigned int        PSMStartAddress;
    unsigned int        PSMEndAddress;
	unsigned int        BackupPSMStartAddress;
	unsigned int        BackupPSMEndAddress;
    unsigned int        FlashFsysAddress;
    unsigned int        MSC0StartAddress;
    unsigned int        MSC0EndAddress;
    unsigned int        MSC1StartAddress;
    unsigned int        MSC1EndAddress;
    unsigned int        MSC2StartAddress;
    unsigned int        MSC2EndAddress;
    unsigned int        FBFStartAddress;
    unsigned int        FBFEndAddress;
} FlashLayoutConfInfo;

/* block properties */
typedef struct FlashBlockProperties
{
    unsigned int        SectorMaxCount;
    unsigned int        FlashBlockSize;
    unsigned int        MapTabStartBlock;
    unsigned int        FatSysStartBlock;
    unsigned int        FatSysEndBlock;
    unsigned int        FatSysResvBlock;
} FlashBlockProperties;

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
/*      GetFlashLayoutConfig                                             */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function get the configuration of flash layout.              */
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
FlashLayoutConfInfo *GetFlashLayoutConfig(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      GetFlashBlockProperties                                          */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function get the block properties.                           */
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
FlashBlockProperties *GetFlashBlockProperties(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      FlashLayoutConfigure                                             */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function initialize flash layout configuration.              */
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
void FlashLayoutConfigure(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      FlashInitBlockProperties                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function initialize flash block properties.                  */
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
void FlashInitBlockProperties(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      CheckIf32MNOR                                                    */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      If Manfactur ID = 0xXX19, this is 32M Nor Flash.                 */
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
BOOL CheckIf32MNOR(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      DumpFileToSDEnable                                               */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function check whether dump bin to sd card is enbale or not.*/
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
BOOL DumpFileToSDEnable(void);

#endif  /*__FLASHPARTITION_H__*/
