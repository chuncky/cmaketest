#include "rsplatform.h"
#include "rs_ua_fs.h"
#include "rs_ua_flash.h"
#include "rs_ua_porting.h"
#include "common.h"
#include "qspi_flash.h"
#include "loadtable.h"
#include <stdint.h>
#include <stdbool.h>


//芯片系统平台下的头文件
//#include "ftl.h"
//#include "fs_gprot.h"
//#include <string.h>
//#include <bl_types.h>
//#include <bl_init.h>
//#include <bl_common.h>
//#include "wdt_hw.h"
//#include "dcl.h"
//#include <stdarg.h>



#define RS_MAX_PRINT_SIZE	250

#define PROGRESS_EXTEND_PRE  (20)
#define PROGRESS_NORMAL_PRE (100 - PROGRESS_EXTEND_PRE)

#define BL_FOTA_ENABLE_WATCHDOG
#define BL_FOTA_WATCHDOG_PERIOD 20

#pragma arm section zidata="RS_DATA"
// TODO: 配置差分升级所需内存
rs_u8 rs_ua_ram_buffer[RS_FOTA_UA_RAM_BUFFER_SIZE];
// TODO: 配置一个BLOCK大小的内存
rs_u8 rs_ua_block_data[RS_FOTA_UA_FLASH_BLCOK_SIZE];

//引用大内存的地址，底层代码配置了
//rs_u8 * rs_ram_buffer = 0;
//rs_u8 * block_data = 0;
#pragma arm section zidata


#ifdef RS_DIFF_PACKAGE_ON_FLASH
static rs_u32 g_deltaPartitionAddr;
#endif

/******************************************************************/

rs_s32 rs_porting_validation(rs_bool multiUnit, rs_u32 unitCount);
rs_s32 rs_porting_update(rs_bool multiUnit, rs_u32 unitCount);


#ifndef RS_DIFF_PACKAGE_ON_FLASH

rs_bool rs_ua_writeDataToFile(rs_u8* buffer, rs_s32 bufferSize, rs_u32 destFile);

static rs_s8* g_updateInfo1File = (rs_s8*)RS_UA_UPDATEINFO1_FILE;
static rs_s8* g_updateInfo2File = (rs_s8*)RS_UA_UPDATEINFO2_FILE;
static rs_s8* g_deltaFile = (rs_s8*)RS_UA_DELTA_FILE;
static rs_s8* g_extendFile = (rs_s8*)RS_UA_EXTEND_FILE;

void rs_ua_setUpdateInfo1File(const rs_s8* file)
{
	g_updateInfo1File = (rs_s8*)file;
}

void rs_ua_setUpdateInfo2File(const rs_s8* file)
{
	g_updateInfo2File = (rs_s8*)file;
}

void rs_ua_setDeltaFile(const rs_s8* file)
{
	g_deltaFile = (rs_s8*)file;
}

void rs_ua_setExtendFile(const rs_s8* file)
{
	g_extendFile = (rs_s8*)file;
}

#endif

/******************************************************************/
/**
 * rs_ua_postProgress
 *
 *
 */
 #ifdef LCD_USE_IN_CODE
 extern void OTA_Clear_LCD(void);
 #endif
void rs_ua_postUpdateProgress(rs_u32 current , rs_u32 total)
{

	rs_u32 present = current;
	
	if(total == PROGRESS_EXTEND_PRE)
	{
		present = (rs_u32)(PROGRESS_NORMAL_PRE + (current* total/100));
	}
	else if(total == PROGRESS_NORMAL_PRE)
	{
		present = (rs_u32)(current* total/100);
	}
	
	rs_trace("updating show progress, present:%d, current:%d, total:%d \r\n", present , current, total);
//+++++++++++++++++++++++++++++++++++++++++++
//Add LCD Info here
#ifdef LCD_USE_IN_CODE
	//LCD show update progress
	static rs_u32 last_current = 0;
	static rs_u8 clear_lcd = 0;
	//[1] Before show progress, clear the LCD info. Note: only do it once
	if(clear_lcd == 0){
		//clear lcd
		OTA_Clear_LCD();
		clear_lcd = 1;
	}
	//[2] show the update progress bar
	if((current != last_current) || (current == 0)){
		OTA_LCD_ShowProcess(current);
		last_current = current;
		/* delay*/
		if(current == 100){
			rs_u32 delay=10000;
			while(delay)
				delay--;
		}
	}
#endif
//+++++++++++++++++++++++++++++++++++++++++++
}

/**
 * 
 * rs_ua_kick_watchdog
 */
void rs_ua_kick_watchdog(void)
{
	//WacthDogRestart(); 

}

rs_u32 rs_ua_sdk_heap_addr()
{
	return (rs_u32 )rs_ua_ram_buffer;
}

/**
 * rs_ua_sdk_heap_size
 *
 *
 */
rs_u32 rs_ua_sdk_heap_size()
{
  return RS_FOTA_UA_RAM_BUFFER_SIZE;
}

rs_u32 rs_ua_ram_block()
{
	return (rs_u32 )rs_ua_block_data;
}

rs_u32 rs_ua_ram_block_size(void)
{
	return RS_FOTA_UA_FLASH_BLCOK_SIZE;
}

void rs_ua_set_deltaPartitionAddr(rs_u32 addr)
{
#ifdef RS_DIFF_PACKAGE_ON_FLASH
	if (addr == 0)
		g_deltaPartitionAddr = get_fota_pkg_start_addr();
	else
		g_deltaPartitionAddr = addr;
#endif
rs_trace("%s, 0x%x \r\n", __func__ , g_deltaPartitionAddr);

}

/*
	主要用于update block size小的情况，也就是升级单元和update info 的单元不一致的情况
	注意：虽然每一个updateinfo 占用了2个RS_FOTA_UA_FLASH_BLCOK_SIZE，但是有效数据小于
	一个RS_FOTA_UA_FLASH_BLCOK_SIZE，所以这里设置为一个RS_FOTA_UA_FLASH_BLCOK_SIZE
*/
rs_u32 rs_ua_getUpdateBlockSize()
{
#ifdef RS_DIFF_PACKAGE_ON_FLASH
	return RS_FOTA_UA_UPDATEINFO_BLCOK_SIZE;
#else
	return rs_ua_ram_block_size();
#endif
}

/**
 * rs_ua_getPackageAddr
 * return address of fota diff package on flash
 */
rs_u32 rs_ua_getPackageAddr()
{
#ifdef RS_DIFF_PACKAGE_ON_FLASH
	return g_deltaPartitionAddr + 2*RS_FOTA_UA_UPDATEINFO_BLCOK_SIZE;
#else
	return 0;
#endif
}

/**
 * rs_ua_getUpdateInfo1Addr
 * return address of storing fota update status info, main block
 */
rs_u32 rs_ua_getUpdateInfo1Addr()
{
#ifdef RS_DIFF_PACKAGE_ON_FLASH
	return g_deltaPartitionAddr;
#else
	return 0;
#endif
}

/**
 * rs_ua_getUpdateInfo2Addr
 * return address of storing fota update status info, second block
 */
rs_u32 rs_ua_getUpdateInfo2Addr()
{
#ifdef RS_DIFF_PACKAGE_ON_FLASH
	return g_deltaPartitionAddr + RS_FOTA_UA_UPDATEINFO_BLCOK_SIZE;
#else
	return 0;
#endif
}


rs_u8 rs_ua_getUpdateInfo1File()
{
#ifdef RS_DIFF_PACKAGE_ON_FLASH
	return 0;
#else
	return RS_UA_UPDATEINFO1_FILE_INDEX;
#endif
}

rs_u8 rs_ua_getUpdateInfo2File()
{
#ifdef RS_DIFF_PACKAGE_ON_FLASH
	return 0;
#else
	return RS_UA_UPDATEINFO2_FILE_INDEX;
#endif
}

rs_u8 rs_ua_getOTAPackageFile()
{
#ifdef RS_DIFF_PACKAGE_ON_FLASH
	return 0;
#else
	return RS_UA_DELTA_FILE_INDEX;
#endif
}

rs_s32 rs_ua_openFileRD(rs_u32 fileIndex)
{
#ifndef RS_DIFF_PACKAGE_ON_FLASH
	if (fileIndex == RS_UA_UPDATEINFO1_FILE_INDEX)
		return (rs_s32)rs_ua_fs_open((rs_s8 *)g_updateInfo1File, RS_FS_OPEN_READ);
	else if (fileIndex == RS_UA_UPDATEINFO2_FILE_INDEX)
		return (rs_s32)rs_ua_fs_open((rs_s8 *)g_updateInfo2File, RS_FS_OPEN_READ);
	else if (fileIndex == RS_UA_DELTA_FILE_INDEX)
		return (rs_s32)rs_ua_fs_open((rs_s8 *)g_deltaFile, RS_FS_OPEN_READ);
	else if (fileIndex == RS_UA_EXTEND_FILE_INDEX)
		return (rs_s32)rs_ua_fs_open((rs_s8 *)g_extendFile, RS_FS_OPEN_READ);
#endif
	return RS_FS_INVALID;
}


/*
函数说明：扩展数据从rsua输出完成之后，开始处理这些扩展数据，如果不需要处理则不做任何动作
*/
rs_u32 rs_ua_processExtendFile()
{
	// 这里和之前的区别是处理完成之后，需要自行删除扩展数据，sdk不会删除数据
	//return rs_ua_processGPS();
	return 0;
}

/*
函数说明：rsua分阶段输出扩展数据，适配层把输出的分段数据写入文件 flash 或者内存中
*/
rs_u32 rs_ua_saveExtendDataToFile(rs_u8* buff, rs_u32 size, rs_u32 offset, rs_bool isFinal)
{
	return 0;
}

rs_s32 rs_ua_openFileCreate(rs_u32 fileIndex)
{
#ifndef RS_DIFF_PACKAGE_ON_FLASH
	if (fileIndex == RS_UA_UPDATEINFO1_FILE_INDEX)
		return (rs_s32)rs_ua_fs_open((rs_s8 *)g_updateInfo1File, RS_FS_OPEN_CREATE);
	else if (fileIndex == RS_UA_UPDATEINFO2_FILE_INDEX)
		return (rs_s32)rs_ua_fs_open((rs_s8 *)g_updateInfo2File, RS_FS_OPEN_CREATE);
	else if (fileIndex == RS_UA_DELTA_FILE_INDEX)
		return (rs_s32)rs_ua_fs_open((rs_s8 *)g_deltaFile, RS_FS_OPEN_CREATE);
	else if (fileIndex == RS_UA_EXTEND_FILE_INDEX)
		return (rs_s32)rs_ua_fs_open((rs_s8 *)g_extendFile, RS_FS_OPEN_CREATE);
#endif
	return RS_FS_INVALID;
}

rs_s32 rs_ua_openFileRW(rs_u32 fileIndex)
{
#ifndef RS_DIFF_PACKAGE_ON_FLASH
	if (fileIndex == RS_UA_UPDATEINFO1_FILE_INDEX)
		return (rs_s32)rs_ua_fs_open((rs_s8 *)g_updateInfo1File, RS_FS_OPEN_WRITE);
	else if (fileIndex == RS_UA_UPDATEINFO2_FILE_INDEX)
		return (rs_s32)rs_ua_fs_open((rs_s8 *)g_updateInfo2File, RS_FS_OPEN_WRITE);
	else if (fileIndex == RS_UA_DELTA_FILE_INDEX)
		return (rs_s32)rs_ua_fs_open((rs_s8 *)g_deltaFile, RS_FS_OPEN_WRITE);
	else if (fileIndex == RS_UA_EXTEND_FILE_INDEX)
		return (rs_s32)rs_ua_fs_open((rs_s8 *)g_extendFile, RS_FS_OPEN_WRITE);
#endif
	return RS_FS_INVALID;
}

rs_bool rs_ua_removeFile(rs_u32 fileIndex)
{
#ifndef RS_DIFF_PACKAGE_ON_FLASH
	if (fileIndex == RS_UA_UPDATEINFO1_FILE_INDEX)
		return rs_ua_fs_remove((const rs_s8*)g_updateInfo1File);
	else if (fileIndex == RS_UA_UPDATEINFO2_FILE_INDEX)
		return rs_ua_fs_remove((const rs_s8*)g_updateInfo2File);
	else if (fileIndex == RS_UA_DELTA_FILE_INDEX)
		return rs_ua_fs_remove((const rs_s8*)g_deltaFile);
	else if (fileIndex == RS_UA_EXTEND_FILE_INDEX)
		return rs_ua_fs_remove((const rs_s8*)g_extendFile);
#endif
	return rs_false;
}

rs_u32 rs_ua_getFileSize(rs_u32 fileIndex)
{
#ifndef RS_DIFF_PACKAGE_ON_FLASH
	RS_FILE fileHandle = RS_FS_INVALID;
	rs_u32 filesize = 0;
	
	fileHandle = rs_ua_openFileRD(fileIndex);
	if (fileHandle == RS_FS_INVALID)
		return 0;
	
	filesize = rs_ua_fs_size(fileHandle);
	rs_ua_fs_close(fileHandle);
	
	return filesize;
#else
	return 0;
#endif
}
/**
 * rs_ua_packageStore
 * return value for SDK using
 */
rs_u8 rs_ua_packageStore()
{
#if defined(RS_DIFF_PACKAGE_ON_FLASH)
	return 1;
#else
	return 0;
#endif
}

#include "stdarg.h"

rs_s8 rs_debug_buffer[128];

rs_bool rs_trace(const rs_s8 *format,...)
{
	va_list ap;

	memset(rs_debug_buffer, 0x0, sizeof(rs_debug_buffer));	
	
	va_start (ap, format);
	vsnprintf(rs_debug_buffer, 128, format, ap);
	uart_printf("%s", rs_debug_buffer);	
	va_end (ap);

	return 0;	

}

rs_bool rs_ua_prepareEnv()
{	
	rs_s32 ret = 0;
	if (!rs_ua_flash_init())
	{
		rs_trace("%s, Flash init failed\n\r", __func__);
		return rs_false;
	}

	#ifndef RS_DIFF_PACKAGE_ON_FLASH
	//ret = rs_FS_SanityCheck();//FS_SanityCheck();
	//rs_trace("%s, FS_SanityCheck ret = %d,\n\r", __func__, ret);
	//如果升级包存储在文件系统，可能需要先做文件系统相关的
	#endif

	return rs_true;
}

void rs_ua_cleanEnv()
{
/*	SPIFFS_unmount(g_rsfota_fs);
	g_rsfota_fs = NULL;
	bl_watchdog_stop();
*/	
}

rs_s32 rs_ua_execute()
{
	e_updateState update_state = UPSTATE_IDLE;
	rs_u32 fotaRet = 1;
	rs_bool multiUnit = rs_false;
	rs_u32 unitCount = 0;
	rs_ua_set_deltaPartitionAddr(0);
	//g_deltaPartitionAddr = get_fota_pkg_start_addr();
	if(rs_ua_prepareEnv() != rs_true)
	{
		return 1;
	}


	update_state = rs_ua_getUpdateState();
	
	rs_trace("rs_ua_getUpdateState update_state = %d\n\r", update_state);
	if (update_state == UPSTATE_CHECK || update_state == UPSTATE_UPDATE)
	{
	   // mon_Event(0x60000426);
		//bl_init();
	}
	else
	{
		//mon_Event(0x60000422);
		rs_ua_cleanEnv();
		return 1;	
	}

	multiUnit = rs_ua_hasMultiUnit();
	rs_trace("rs_ua_hasMultiUnit multiUnit = %d\n\r", multiUnit);
	if (multiUnit)
	{
		unitCount = rs_ua_getMultiUnitCount();
	}
	
	rs_trace("rs_ua_getMultiUnitCount unitCount = %d\n\r", unitCount);

	// deltaAddr是从外部输入的差分包存储空间的位置
	// 如果不能从外部获取则输入0
	rs_ua_set_deltaPartitionAddr(0);

	switch(update_state)
	{
		case UPSTATE_CHECK:
			
			rs_trace("rs_porting_validation ... \n\r");
			fotaRet = rs_porting_validation(multiUnit, unitCount);
			if(fotaRet == 0)
			{
				rs_trace("rs_porting_validation OK \n\r");
				rs_trace("rs_porting_update start ... \n\r");
				fotaRet = rs_porting_update(multiUnit, unitCount);
				rs_trace("rs_porting_update finish \n\r");
			}
			else
			{
				rs_trace("rs_porting_validation fail return  = %d\n\r", fotaRet);
			}
			break;
			
		case UPSTATE_UPDATE:
			rs_ua_setRecoveryPhase();
			rs_trace("rs_porting_update continue ... \n\r");
			fotaRet = rs_porting_update(multiUnit, unitCount);
			rs_trace("rs_porting_update finish \n\r");
			break;
			
		default:
			rs_ua_cleanEnv();
			return 1;
	}

	rs_trace("rs_ua_execute completed，fotaRet = %d\n\r", fotaRet);

	rs_ua_cleanEnv();

	return fotaRet;
}

rs_s32 rs_porting_validation(rs_bool multiUnit, rs_u32 unitCount)
{
	rs_u32 index = 0;
	rs_u8 updateType = DELTA_DATA_TYPE_NOT_SUPPORT;
	rs_u8 uintID = 0;
	rs_s32 fotaRet = 0;


	if (multiUnit)
	{
		if(! rs_ua_getCurrentUnitIndex(&index))
			return 1;

		if (index == 0)
		{
		
			if(rs_ua_validateMultiUnitPkg() != rs_true)
			{
				rs_trace("rs_porting_validation return 1 \r\n");
				return 1;
			}
			
		}

		
		while (index < unitCount)
		{
			if(rs_ua_getUnitDataTypeAndID(index, &updateType, &uintID))
			{
				
				rs_trace("rs_ua_getUnitDataTypeAndID updateType=%d  \r\n" ,updateType );
				rs_trace("rs_ua_getUnitDataTypeAndID uintID=%d  \r\n" ,uintID );
				if (updateType == DELTA_DATA_TYPE_IMAGE_UPDATE)
				{
					
					rs_trace("rs_ua_validation before index = %d , multiUnit = %d  0 \r\n" ,index ,multiUnit);
					fotaRet = rs_ua_validation(multiUnit, index);
					if(fotaRet != 0)
					{
						
						rs_trace("rs_ua_validation return %d	\r\n" ,fotaRet );
						return 1;
					}
				}
			}
			else
			{
				
				rs_trace("rs_ua_getUnitDataTypeAndID return 0 \r\n");
				return 1;
			}
			
			
			index++;

			if (index < unitCount)
			{
				if(!rs_ua_setCurrentUnitIndex(index))
				{
					return 1;
				}
			}
		}
	}
	else
	{
		fotaRet = rs_ua_validation(rs_false, 0);
		if(fotaRet != 0)
		{
			return 1;
		}
	}

	if(!rs_ua_updatePhase())
	{
		return 1;
	}

	
	return 0;
}

rs_s32 rs_porting_update(rs_bool multiUnit, rs_u32 unitCount)
{
	rs_u32 index = 0;
	rs_u8 updateType = DELTA_DATA_TYPE_NOT_SUPPORT;
	rs_u8 uintID = 0;
	rs_s32 fotaRet = 0;


	if (multiUnit)
	{
		if(! rs_ua_getCurrentUnitIndex(&index))
		{
			return 1;
		}
		
		while (index < unitCount)
		{
			if(!rs_ua_getUnitDataTypeAndID(index, &updateType, &uintID))
			{
				return 1;
			}
			
			if (updateType == DELTA_DATA_TYPE_IMAGE_UPDATE)
			{
				fotaRet = rs_ua_upgrade(multiUnit, index);
				if(fotaRet != 0)
				{
					return 1;
				}
			}

			/* // 对于C216B只需要处理自身升级
			else if(updateType == DELTA_DATA_TYPE_COMPRESSED_BIN)
			{
				fotaRet = rs_ua_decodeUnitData(index, rs_ua_outPutDataStream);
				if (fotaRet != rs_true)
				{
					return 1;
				}
			}
			else if(updateType == DELTA_DATA_TYPE_RAW_BIN)
			{
				fotaRet = rs_ua_getUnitRawData(index, rs_ua_outPutDataStream);
				if (fotaRet != rs_true)
				{
					return 1;
				}
			}*/
			
			index++;
			if (index < unitCount)
			{
				if(!rs_ua_setCurrentUnitIndex(index))
				{
					return 1;
				}
			}
		}
	}
	else
	{
		fotaRet = rs_ua_upgrade(rs_false, 0);
		if(fotaRet != 0)
		{
			return 1;
		}
	}

	if(!rs_ua_updatePhaseComplete())
	{
		return 1;
	}

	
	return 0;
}
