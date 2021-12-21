//***************************************************************
#ifndef _OTA_SDCARD_H
#define _OTA_SDCARD_H

/*
*****************************************************************************************
*											INCLUDE FILES
*****************************************************************************************
*/
#include "types.h"
#include "qspi_flash.h"
#include "ff.h"
#include "common.h"
#include "loadtable.h"
#include "lcd_test.h"

/*
*****************************************************************************************
*											MACRO AND typedef
*****************************************************************************************
*/

#define FILENAME "D:/update.bin"	//include cp.bin,dsp.bin and rf.bin; 4k alignment
#define SYSTEM_PART_OFFSET 0x24000		//The start address of system and ptable
#define DATA_BUF_SIZE (0x10000)
#define SDCARD_FAT_IS_OK 1

#define SPACE_FLAG_FEED_INFO (0x1000)	//4k Bytes
#define OTA_FLAG_SDCARD_UPDATE "ASROTA-S"
#define OTA_FLAG_SDCARD_KEY_FLAG ("ASROTAKS")
#define OTA_FLAG_SDCARD_NOT_UPDATE 0xFF
#define OTA_FLAG_LENGTH (9)
#define OTA_CRC_RESULT_OFFSET (0x10)
#define OTA_CRC_RESULT_LENGTH (0x02)
#define OTA_HEADER_LENGTH 0x20		//Adjust according to the Actual Header In the later stage


//Update_File_Header_Info
#define OTA_VERSION_LENGTH (0x10)



//define OTA Feedback info and length
#define OTA_FLAG_LENGTH (9)
#define SDCARD_MOUNT_FAIL "SDCard Mount Fail!"
#define SDCARD_MOUNT_FAIL_LENGTH (19)
#define OTA_UPDATE_FILE_NO_EXIST "No update.bin in sdcard!"
#define OTA_UPDATE_FILE_NO_EXIST_LENGTH (25)
#define OTA_UPDATE_FILE_CRC_INCORRECT "CRC res is different with header file!"
#define OTA_UPDATE_FILE_CRC_INCORRECT_LENGTH (39)

typedef enum{
	OFFSET_OTA_FEED_Flag,								//0, OTA_Flag is 9 Bytes
	OFFSET_OTA_FEED_PRE_VERSION = 9,					//9
	OFFSET_OTA_FEED_CURRENT_VERSION = 25,				//25
	OFFSET_OTA_SD_Mount = 41,							//41
	OFFSET_OTA_UPDATE_FILE_No_Exist = 60,				//60
	OFFSET_OTA_CRC_Incorrect = 85						//85
}OFFSET_OTA_FEED_INFO;


//define Error types
typedef enum
{
	OTA_ERR_NO = 0,					// 0
	OTA_ERR_Verify,					// 1
	OTA_ERR_CRC_Check,				// 2
	OTA_ERR_f_open,					// 3
	OTA_ERR_f_read,					// 4
	OTA_ERR_spi_nor_erase,			// 5
	OTA_ERR_spi_nor_erase_sector,	// 6
	OTA_ERR_spi_nor_write,			// 7
	OTA_ERR_spi_nor_read,			// 8
	OTA_VERSION_DIFFERENT,			// 9
	OTA_VERSION_SAME,				// 10
	OTA_CRC_DIFFERENT,				// 11
	OTA_CRC_SAME,					// 12
	OTA_MALLOC_FAIL,				// 13
	OTA_FLASH_NO_Write				// 14
}OTA_ERR_T;


/*
*****************************************************************************************
*											Log Priority Definition
*****************************************************************************************
*/
/*+++++++++++++Log_Priority Start++++++++++++++++++++++++*/

#ifndef LOG_TAG
#define LOG_TAG __func__
#endif

#define LOG_SEVERITY_V 1
#define LOG_SEVERITY_D 2
#define LOG_SEVERITY_I 3
#define LOG_SEVERITY_W 4
#define LOG_SEVERITY_E 5

#ifndef LOG_SEVERITY
#define LOG_SEVERITY LOG_SEVERITY_V
#endif

#ifndef LOG_NDEBUG
#ifdef NDEBUG
#define LOG_NDEBUG 1
#else
#define LOG_NDEBUG 0
#endif
#endif

#define LOG_PRINT_IF uart_printf

#if LOG_NDEBUG
#define ALOGV(x,...)
#define ALOGD(x,...)
#else
#define ALOGV(x,...) do{if (LOG_SEVERITY <= LOG_SEVERITY_V) LOG_PRINT_IF(" %s [%s]: " x "\r\n","V",LOG_TAG,##__VA_ARGS__);}while(0)
#define ALOGD(x,...) do{if (LOG_SEVERITY <= LOG_SEVERITY_D) LOG_PRINT_IF(" %s [%s]: " x "\r\n","D",LOG_TAG,##__VA_ARGS__);}while(0)
#endif

#define ALOGI(x,...) do{if (LOG_SEVERITY <= LOG_SEVERITY_I) LOG_PRINT_IF(" %s [%s]: " x "\r\n","I",LOG_TAG,##__VA_ARGS__);}while(0)
#define ALOGW(x,...) do{if (LOG_SEVERITY <= LOG_SEVERITY_W) LOG_PRINT_IF(" %s [%s],[Func:%s],[Line:%d]: " x "\r\n","W",LOG_TAG,__func__,__LINE__,##__VA_ARGS__);}while(0)
#define ALOGE(x,...) do{if (LOG_SEVERITY <= LOG_SEVERITY_E) LOG_PRINT_IF(" %s [%s],[Func:%s],[Line:%d]: " x "\r\n","E",LOG_TAG,__func__,__LINE__,##__VA_ARGS__);}while(0)

/*+++++++++++++Log_Priority End++++++++++++++++++++++++*/


/*
*****************************************************************************************
*											Extern Variables
*Note: These Variable define in OTA_sdcard, and other file will use, such as main.c
*****************************************************************************************
*/
extern char OTA_Sd_KeyFlag[OTA_FLAG_LENGTH];
extern FIL handle;
extern void OTA_LCD_ShowProcess(int per);
extern void OTA_LCD_DisplayText(const char *p_addtional_text,const OTA_LCD_DISPLAY_INFO_TYPE ota_info_type,const OTA_LANGUAGE_T ota_language);
extern void OTA_Clear_LCD(void);
extern void OTA_LCD_ShowImage(void);
extern VOID mci_LcdSetBrightness(UINT8 level);

/*
*****************************************************************************************
*											Extern Function
*Note:The function defines in keypad_drv.c, and will use in main.c
*****************************************************************************************
*/

/***************************************
Function:	  Keypad_OTA_Flag_Check_U
Description:  Detect OTA_Sdcard update keypad combination flag during "updater"
Parmeters:	  None

Return value: True: Have detected the update keypad combination flag
			  False: Not detect the update keypad combination flag
***************************************/
extern BOOL Keypad_OTA_Flag_Check_U(void);


/*
*****************************************************************************************
*											Global Function API
*****************************************************************************************
*/


/***************************************
Function:	  ASR_OTA_Sdcard
Description:  call the above function to update package by OTA_Sdcard
Parmeters:	  NULL
Return value: NULL	
Note:		   Enter the function by OTA_Flag:"ASROTA-S"	
***************************************/
void ASR_OTA_Sdcard(void);

/***************************************
Function:	   ASR_OTA_Key_Sdcard
Description:   After detecting the OTA_Sd_Keypad_Flag, update package
Parmeters:	   NULL
Return value:  NULL	
Note:		   Enter the function by Keypad Combination	
***************************************/
void ASR_OTA_Key_Sdcard(void);

#endif		//_OTA_SDCARD_H
//***************************************************************

