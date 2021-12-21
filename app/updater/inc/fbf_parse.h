/******************************************************************************
 *
 *  (C)Copyright 2011 Marvell. All Rights Reserved.
 *
 *  THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF MARVELL.
 *  The copyright notice above does not evidence any actual or intended
 *  publication of such source code.
 *  This Module contains Proprietary Information of Marvell and should be
 *  treated as Confidential.
 *  The information in this file is provided for the exclusive use of the
 *  licensees of Marvell.
 *  Such users have the right to use, modify, and incorporate this code into
 *  products for purposes authorized by the license agreement provided they
 *  include this notice and the associated copyright notice with any such
 *  product.
 *  The information in this file is provided "AS IS" without warranty.
 *
 *
 *  FILENAME:	fbf_parse.h
 *
 *  PURPOSE: 	
 *
******************************************************************************/

#ifndef FBF_PARSE
#define FBF_PARSE

#include "common.h"
//#include "sha1.h"

#define DLCMD_IMAGE_TYPE_FIELD_BIT          4
#define DLCMD_IMAGE_TYPE_FIELD_SIZE_BITS    4
#define MAX_NUMBER_OF_FLASH_DEVICES_IN_MASTER_HEADER    4
#define MAX_NUMBER_OF_IMAGE_STRUCTS_IN_DEVICE_HEADER    150
#define BLOCK_DEVICE_SECTOR_SIZE                        (8*1024)                // 4 KB

#define MAX_RESEVERD_LEN	4
#define MAX_NUM_SKIP_BLOCKS      32



/**
 * General error code definitions	for FBF parse		0x0 - 0x1F
 **/
#define NoError            					0x0
#define NotFoundError      					0x1
#define GeneralError       					0x2
#define WriteError         					0x3
#define ReadError		   					0x4
#define NotSupportedError  					0x5
#define InvalidPlatformConfigError			0x6
#define InvalidPlatformState				0x7

#define InvalidSizeError					0x9
#define ProbeListExhaustError				0xA
#define DDR_NotInitializedError				0xB
#define UnknownReservedPackage				0xD
#define NULLPointer							0xE
#define NANDIDDISMATCH						0xF
#define FBF_VersionNotMatch					0x10
#define FBF_DeviceMoreThanOne				0x11
#define FBF_CheckSumError                   0x12
#define FBF_UniqueError                     0x13
#define NotEnoughMomeryError                0x14
#define NotEnoughFlashError                 0x15
#define CRCFailedError						0x16
#define DFota_ImageDFotaDecodeFailed		0x17
#define DFota_LZMADecompressFailed			0x18
#define DFota_NotFoundRelevantFile			0x19
#define DFota_GetOldFileSizeError			0x20
#define DFota_BspatchFailed					0x21
#define DFota_CRCFailedError				0x22
#define DFota_MINI_StatusError				0x23
#define DFota_MINI_MagicError				0x24




//download commands
#define DLCMD_RAW_BINARY_IMAGE_TYPE                 (0)
#define DLCMD_WINDOWS_MOBILE_IMAGE_TYPE             (1)
#define DLCMD_YAFFS_IMAGE_TYPE	                    (2)
#define DLCMD_UBIFS_IMAGE_TYPE                      (3)
#define DLCMD_JFFS2_IMAGE_TYPE                      (4)
#define DLCMD_ABSOLUTE_FLASH_OFFSET_WITH_SPARE_TYPE (5)
#define DLCMD_LZMA2_IMAGE_TYPE                      (8)

#define IMAGE_TYPE_MASK 0xF0

#define WEBDATA_IMAGE_ID 0x57454249 
#define WIFI_NOCAL_IMAGE_ID 0x57494649
#define WIFI_CAL_IMAGE_ID 0x5743414c
#define RSA_IMAGE_ID 0x52534149

#define PS_BUF_START 0x07280000
#define PS_BUF_MAX_LEN 0x700000

#define MSA_BUF_START 0x07D80000
#define MSA_BUF_MAX_LEN 0x280000

// error code for upgrade 
#define UPGRADE_NO_ERROR 				0
#define UPGRADE_ERR_IMAGE_SIZE 			1
#define UPGRADE_ERR_INVALID_VERSION 	2
#define UPGRADE_ERR_INVALID_IMAGE 		3
#define UPGRADE_ERR_LOW_BETTERY 		4
#define UPGRADE_ERR_IO 					5
#define UPGRADE_ERR_MEMORY 				6
#define UPGRADE_ERR_OTHER 				7
#define UPGRADE_ERR_NOT_SIGNATURE 		8
#define UPGRADE_ERR_UPLOAD 				9


typedef struct
{
	UINT32 Image_ID;				/* image id*/
	UINT32 Image_In_TIM;			/* indicate this image is in TIM or not*/
    UINT32 Flash_partition;        /* partition number of the image */
    UINT32 Flash_erase_size;      /* erase size of the image */
    UINT32 commands;                /* bit switches */
    UINT32 First_Sector;            /* First sector of the image in the block device */
    UINT32 length;                 /* Block length in bytes */
    UINT32 Flash_Start_Address;    /* start address in flash */
    UINT32 Old_File_Flash_Address;    /* old file start address in flash,this is only for dfota */
    UINT32 Old_Image_Checksum;    		/* this status record for bspatch already OK or not  */
	UINT32 Old_Image_len;
    UINT32 reserved[MAX_RESEVERD_LEN-3];
    UINT32 ChecksumFormatVersion2;  /* new format version image checksum (left for backwards compatibility) */
} __attribute__((packed)) ImageStruct_11;

typedef ImageStruct_11 *PImageStruct_11;

typedef struct
{
	UINT32 Total_Number_Of_SkipBlocks; // total numbers of skip blocks
	UINT32 Skip_Blocks[MAX_NUM_SKIP_BLOCKS];
} SkipBlocksInfoStruct;


typedef struct
{
	UINT32 EraseAll; // erase all flag for user partition
    UINT32 ResetBBT; // indicate if reset BBT
    UINT32 NandID;	 // nand flash ID
    UINT32 CheckSum;	 // old images checksum
    UINT32 Length;	 	// old images length
	UINT32 Reserved[MAX_RESEVERD_LEN - 3];
	SkipBlocksInfoStruct  Skip_Blocks_Struct;
}FlashOptStruct;

typedef struct
{
    UINT32 DeviceFlags;
    UINT32 DeviceParameters[16];        /*  Device Parameters, reserve 16 U32 here, will be defined depending on different devices */
	FlashOptStruct  FlashOpt;
	UINT32 Reserved[MAX_RESEVERD_LEN];
    UINT32 nOfImages;        /* number of images */
    ImageStruct_11 imageStruct_11[MAX_NUMBER_OF_IMAGE_STRUCTS_IN_DEVICE_HEADER]; /* array of image structs */
} __attribute__((packed)) DeviceHeader_11;

typedef DeviceHeader_11 *PDeviceHeader_11;


#define NUM_OF_SUPPORTED_FLASH_DEVS    4

#define UNIQUE_SIZE 24
typedef struct
{
    char Unique[UNIQUE_SIZE];
    UINT16 Flash_Device_Spare_Area_Size[NUM_OF_SUPPORTED_FLASH_DEVS];

    UINT16 Format_Version;                                /* FBF format version */
    UINT16 Size_of_Block;                                 /* un-used */
    UINT32 Bytes_To_Program;                              /* Total number of bytes to program */
    UINT32 Bytes_To_Verify;                               /* Total number of bytes to verify */
    UINT32 Number_of_Bytes_To_Erase;                      /* Total number of bytes to erase */
    UINT32 Main_Commands;

    UINT32 nOfDevices;                                    /* number of devices to burn in parallel */
    UINT32 DLerVersion;                                   /* Version of downloader current 1          */
    UINT32 deviceHeaderOffset[MAX_NUMBER_OF_FLASH_DEVICES_IN_MASTER_HEADER]; /* offset in Sector 0 for each flash device header  */
} __attribute__((packed)) MasterBlockHeader;
typedef MasterBlockHeader *PMasterBlockHeader;

struct imginfo_table
{
	UINT32 Img_Len;                           
	UINT32 Img_Commands;
	UINT32 Img_Start_Address;
	UINT32 Img_Checksum;
	UINT32 Flash_Start_Address;
	UINT32 Flash_Erase_Size;
	UINT32 Img_ID;
	struct imginfo_table * next;
}; 

typedef struct imginfo_table ImginfoTable; 

//type definition

typedef struct _fbf_buf_ctx
{
	UINT8* buffer_start;
	UINT8* buffer2_start;
	UINT32 buffer_len;
	UINT32 buffer2_len;
	UINT32 bytes_receive;
	
}s_fbf_buf_ctx;

//typedef sha1_context fbf_hash_context;

typedef struct _system_info_ctx
{
	char *start;
	UINT32 len;
}s_system_info_ctx;

typedef struct _firmware_status
{	
	UINT32 status;
	UINT32 progress;
	UINT32 cause;
}s_firmware_status;

#endif
