/*-
 * Copyright 2003-2005 Colin Percival
 * Copyright 2012 Matthew Endsley
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providing that the following conditions 
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef BSPATCH_H
#define BSPATCH_H

//# include <stdint.h>
#include "common.h"
#include "fbf_parse.h"


//#define SUPPORT_MINI_SYSTEM
//#define SUPPORT_FOTA_NVM
#define SUPPORT_NEW_MINI_OTA_VERSION


#define FLASH_1M_SIZE 			0x00100000 //1M
#define CRANE_NOR_BLOCKSIZE 	0x00010000 //64K
#define FOTA_HEADER_MAGIC 		0x54524657
#define FLASH_4K_SIZE 			0x00001000
#define ALIGN_1K_SIZE 			0x0000000a


struct bspatch_stream
{
	void* opaque;
	int (*read)(const struct bspatch_stream* stream, void* buffer, int length);
};

int bspatch(const uint8_t* old, int64_t oldsize, uint8_t* new, int64_t newsize, struct bspatch_stream* stream);

enum {
	MINI_SYS_DFOTA_START,
	MINI_SYS_DFOTA_FIRST_DOING,
	MINI_SYS_DFOTA_SECOND_DOING,
	MINI_SYS_DFOTA_SECOND_BACKUP,
	MINI_SYS_DFOTA_SECOND_COPYING,
	MINI_SYS_DFOTA_SECOND_DONE
};

typedef struct _asr_non_diff_info{
	UINT32 roAddress;
	UINT32 roLen;
	UINT32 roChecksum;
	UINT32 nonfotaAddress;
	UINT32 nonfotaLen;
	UINT32 nonfotaChecksum;

	UINT32 app1PartionAddress;
	UINT32 app1PartionLen;
	UINT32 app1PartionChecksum;
	
	UINT32 app2PartionAddress;
	UINT32 app2PartionLen;
	UINT32 app2PartionChecksum;
	
	UINT32 app3PartionAddress;
	UINT32 app3PartionLen;
	UINT32 app3PartionChecksum;
	
	UINT32 reserved[6];

}asr_non_diff_info;


#ifdef SUPPORT_NEW_MINI_OTA_VERSION

typedef struct _asr_mini_ota_ImageStruct{
	UINT32 ImageID;
	UINT32 ImageOffset;
	UINT32 ImageLen;
	UINT32 ImageCheckSum;
	
	UINT32 oldDeviceFlashAddress;
	UINT32 oldDeviceLen;
	UINT32 oldDeviceCheckSum;
	UINT32 newDeviceFlashAddress;
	UINT32 newDeviceLen;
	UINT32 newDeviceCheckSum;
	UINT32 reserved[2];

}asr_mini_ota_ImageStruct;

#define MINI_OTA_MAX_NUMBER_OF_IMAGE	12

typedef struct _asr_mini_ota_head_info{
	UINT32 magic;
	UINT32 format_version;
	UINT32 reserved[2];
	asr_non_diff_info nonDiffInfo;
	asr_mini_ota_ImageStruct Image[MINI_OTA_MAX_NUMBER_OF_IMAGE];
}asr_mini_ota_head_info;

#else
#define FORWARD_MODE			0x01
#define REVERSE_MODE			0x02
#define MAX_NUMBER_OF_DEVICES_STRUCTS_IN_MASTER_HEADER 2
#define MAX_NUMBER_OF_IMAGE_STRUCTS_TWO_IN_DEVICE_HEADER	100
#define MAX_NUMBER_OF_IMAGE_STRUCTS_ONE_IN_DEVICE_HEADER	4

typedef struct _asr_ImageStruct{
	//unsigned int ImageID;
	unsigned int ImageOffset;
	unsigned int ImageLen;
	unsigned int ImageCheckSum;
	unsigned int BaseFlashAddress;

	unsigned int newImageLen;
	unsigned int newImageCheckSum;
	//unsigned int oldImageLen;
	//unsigned int oldImageCheckSum;
}asr_ImageStruct;

typedef struct _asr_deviceStruct_1{
	UINT32 deviceFlags;
	UINT32 nOfImages;
	UINT32 oldDeviceFlashAddress;
	UINT32 oldDeviceLen;
	UINT32 oldDeviceCheckSum;
	UINT32 newDeviceFlashAddress;
	UINT32 newDeviceLen;
	UINT32 newDeviceCheckSum;
	asr_ImageStruct Image[MAX_NUMBER_OF_IMAGE_STRUCTS_ONE_IN_DEVICE_HEADER];
}asr_deviceStruct_1;



typedef struct _asr_deviceStruct_2{
	UINT32 deviceFlags;
	UINT32 nOfImages;
	UINT32 oldDeviceFlashAddress;
	UINT32 oldDeviceLen;
	UINT32 oldDeviceCheckSum;
	UINT32 newDeviceFlashAddress;
	UINT32 newDeviceLen;
	UINT32 newDeviceCheckSum;
	asr_ImageStruct Image[MAX_NUMBER_OF_IMAGE_STRUCTS_TWO_IN_DEVICE_HEADER];
}asr_deviceStruct_2;

typedef struct _asr_rfStruct{
	UINT32 oldDeviceFlashAddress;
	UINT32 oldDeviceLen;
	UINT32 oldDeviceCheckSum;
	
	UINT32 newDeviceFlashAddress;
	UINT32 newDeviceLen;
	UINT32 newDeviceCheckSum;
}asr_rfStruct;

#define ASR_FOTA_UPDATE_SYSTEM		0x00000001
#define ASR_FOTA_UPDATE_RF			0x00000002
#define ASR_FOTA_UPDATE_UPDATER		0x00000004


typedef struct _asr_master_head_info{
	UINT32 magic;
	UINT32 format_version;
	asr_non_diff_info nonDiffInfo;
	UINT32 mode;
	UINT32 updateFlag;
	UINT32 allianSize;
	UINT32 allianEndAdress;
	UINT32 reserved[2];
	
	asr_deviceStruct_1 device_1;
	asr_deviceStruct_2 device_2;
	asr_ImageStruct updater;
	//UINT32 reserved[11];
}asr_master_head_info;
#endif


#if 1
struct fota_firmwar_flag {
	unsigned int header;
	unsigned int upgrade_flag;
	unsigned int fbf_flash_address;
	unsigned int fbf_file_size;
	//#ifdef SUPPORT_MINI_SYSTEM
	unsigned int second_flash_address;
	unsigned int second_file_size;
#ifdef SUPPORT_FOTA_NVM	
	unsigned int third_flash_address;
	unsigned int third_file_size;
#endif
	unsigned int mini_dfota_status;
	unsigned int non_dfota_address;
	unsigned int non_dfota_len;
	unsigned int non_dfota_checksum;
	//#endif
	unsigned int upgrade_method; // 1.full system; 4, DFOTA; 5, MINI SYSTEM DFOTA
	unsigned int version_Flag;
	unsigned char version[64];	
	unsigned int DFota_nOfImages;
	unsigned int DFota_NeedCopyOnly;
	unsigned int DFota_CopyLen;
	unsigned char image_status[MAX_NUMBER_OF_IMAGE_STRUCTS_IN_DEVICE_HEADER];
	unsigned int index;
	unsigned int checksum;
};
#else

struct fota_firmwar_flag {
	unsigned int header;
	unsigned int upgrade_flag;
	unsigned int fbf_flash_address;
	unsigned int fbf_file_size;
	unsigned int upgrade_method; // 1.full system; 4, DFOTA; 5, MINI SYSTEM DFOTA
	unsigned int version_Flag;
	unsigned char version[64];	
	unsigned int DFota_nOfImages;
	unsigned int DFota_NeedCopyOnly;
	unsigned int DFota_CopyLen;
	unsigned char image_status[MAX_NUMBER_OF_IMAGE_STRUCTS_IN_DEVICE_HEADER];
	unsigned char image_reserved[2];
	unsigned int index;
#ifdef SUPPORT_NEW_MINI_OTA_VERSION
	unsigned int second_flash_address;
	unsigned int second_file_size;
	unsigned int first_backup_address;
	unsigned int first_backup_len;
	unsigned int second_backup_address;
	unsigned int second_backup_len;
	unsigned int old_backup_len;
	unsigned int mini_dfota_status;
	unsigned int DFpta_CopyAddr;
	unsigned int DFpta_CopyFlag;
	unsigned int DFpta_TempAddr;
	asr_mini_ota_head_info diffhead;
	unsigned int reserved[779];
#else
	/*add for mini sys fota*/
	unsigned int second_flash_address;
	unsigned int second_file_size;
	unsigned int backup_address;
	unsigned int mini_dfota_status;
	unsigned int DFpta_CopyAddr;
	asr_master_head_info diffhead;
	unsigned int reserved[279];
#endif
	unsigned int checksum;
};

#endif


#define IMAGE_BSPATCH_READY		0x01
#define IMAGE_BSPATCH_NO_READY     0xFF

#define MAX_NUMBER_OF_DEVICES_STRUCTS_IN_MASTER_HEADER 2
#define MAX_NUMBER_OF_IMAGE_STRUCTS_IN_MASTER_HEADER 100
#define FOTA_MAGIC 0x464F5441
#define ASR_FOTA_FLAG_LEN_MAX 16

typedef struct _asr_ImageStruct{
	//unsigned int ImageID;
	unsigned int ImageOffset;
	unsigned int ImageLen;
	unsigned int ImageCheckSum;
	unsigned int BaseFlashAddress;

	unsigned int newImageLen;
	unsigned int newImageCheckSum;
	//unsigned int oldImageLen;
	//unsigned int oldImageCheckSum;
}asr_ImageStruct;

typedef struct _asr_deviceStruct{
	unsigned int deviceFlags;
	unsigned int nOfImages;
	unsigned int oldDeviceFlashAddress;
	unsigned int oldDeviceLen;
	unsigned int oldDeviceCheckSum;
	unsigned int newDeviceFlashAddress;
	unsigned int newDeviceLen;
	unsigned int newDeviceCheckSum;
	asr_ImageStruct Image[MAX_NUMBER_OF_IMAGE_STRUCTS_IN_MASTER_HEADER];
}asr_deviceStruct;


typedef struct _asr_master_head_info{
	unsigned int magic;
	unsigned int format_version;
	unsigned int nonfotaAddress;
	unsigned int nonfotaLen;
	unsigned int nonfotaChecksum;
	unsigned int nOfDevice;
	asr_deviceStruct device[MAX_NUMBER_OF_DEVICES_STRUCTS_IN_MASTER_HEADER];
}asr_master_head_info;

struct fota_param{
	unsigned char fotaFlag[ASR_FOTA_FLAG_LEN_MAX];
	unsigned int buadrate_flag;
	unsigned int buadrate;
	unsigned int mini_sys_enable;
};


#endif

