#ifndef __RS_UA_PORTING_H__
#define __RS_UA_PORTING_H__

#include "rsplatform.h"
#ifdef WIN32
#include "rs_bl_config.h"
#endif



#ifdef __cplusplus
extern "C" {
#endif


// TODO: 配置升级是否放到flash中，如果不配置此项，则放置于文件系统中
#define RS_DIFF_PACKAGE_ON_FLASH



// TODO: 配置差分升级所需内存的大小
#define RS_FOTA_UA_RAM_BUFFER_SIZE		0x180000

// TODO: 配置Flash的block的大小 blocksize
//#define RS_FOTA_UA_FLASH_BLCOK_SIZE		0x1000
#define RS_FOTA_UA_FLASH_BLCOK_SIZE		0x10000


#ifdef RS_DIFF_PACKAGE_ON_FLASH

// TODO: 配置升级包区域的起始地址

#define KB(n)  (n * 1024)
#define MB(n)  (n * 1024 * 1024)


// TODO: 配置存放updateinfo的block的大小,一般为一个blocksize
#define RS_FOTA_UA_UPDATEINFO_BLCOK_SIZE	(RS_FOTA_UA_FLASH_BLCOK_SIZE)

#else

// TODO: 配置升级包、信息文件、GPS文件所在路径
#define RS_UA_UPDATEINFO1_FILE			L"Z:\\updateinfo1"
#define RS_UA_UPDATEINFO2_FILE			L"Z:\\updateinfo2"
#define RS_UA_DELTA_FILE				L"Z:\\update.bin"
#define RS_UA_EXTEND_FILE				L"Z:\\gps_image.bin"


#define RS_UA_UPDATEINFO1_FILE_INDEX	1
#define RS_UA_UPDATEINFO2_FILE_INDEX	2
#define RS_UA_DELTA_FILE_INDEX			3
#define RS_UA_EXTEND_FILE_INDEX			4

void rs_ua_setUpdateInfo1File(const rs_s8* file);
void rs_ua_setUpdateInfo2File(const rs_s8* file);
void rs_ua_setDeltaFile(const rs_s8* file);
void rs_ua_setExtendFile(const rs_s8* file);

#endif



#define PROGRESS_EXTEND_PRE  (20)
#define PROGRESS_NORMAL_PRE (100 - PROGRESS_EXTEND_PRE)

typedef rs_bool (* OUTPUT_DATA_STREAM)(rs_u8 unitID, rs_u8* buff, rs_u32 size, rs_u32 offset, rs_bool isFinal);

typedef enum 
{
	UPSTATE_INVALID = -1,
	UPSTATE_IDLE , /*idle*/
	UPSTATE_CHECK, /* check  */
	UPSTATE_CHECKED, /* checked  */
	UPSTATE_UPDATE, /* update */
	UPSTATE_UPDATEED,/* update finish */
}e_updateState;

typedef enum 
{
	DELTA_DATA_TYPE_IMAGE_UPDATE,
	DELTA_DATA_TYPE_FS_UPDATE,
	DELTA_DATA_TYPE_COMPRESSED_BIN,
	DELTA_DATA_TYPE_RAW_BIN,
	DELTA_DATA_TYPE_NOT_SUPPORT = 100
}DELTA_DATA_TYPE;


e_updateState rs_ua_getUpdateState(void);
rs_bool rs_ua_setRecoveryPhase(void);

rs_u32 rs_ua_sdk_heap_size(void);
void rs_ua_postUpdateProgress(rs_u32 current , rs_u32 total);
void rs_ua_kick_watchdog(void);
rs_u32 rs_ua_sdk_heap_addr(void);
rs_u32 rs_ua_ram_block(void);
rs_u32 rs_ua_ram_block_size(void);
void rs_ua_set_deltaPartitionAddr(rs_u32 addr);

rs_u32 rs_ua_getPackageAddr(void);
rs_u32 rs_ua_getUpdateInfo1Addr(void);
rs_u32 rs_ua_getUpdateInfo2Addr(void);
rs_u8 rs_ua_getUpdateInfo1File(void);
rs_u8 rs_ua_getUpdateInfo2File(void);
rs_u8 rs_ua_getOTAPackageFile(void);
rs_s32 rs_ua_openFileRD(rs_u32 fileIndex);
rs_s32 rs_ua_openFileCreate(rs_u32 fileIndex);
rs_s32 rs_ua_openFileRW(rs_u32 fileIndex);
rs_bool rs_ua_removeFile(rs_u32 fileIndex);
rs_u32 rs_ua_getFileSize(rs_u32 fileIndex);

rs_u32 rs_ua_processExtendFile(void);
void rs_ua_cleanExtendFile(void);
rs_u32 rs_ua_saveExtendDataToFile(rs_u8* buff, rs_u32 size, rs_u32 offset, rs_bool isFinal);

rs_bool rs_ua_outPutDataStream(rs_u8 unitID, rs_u8* buff, rs_u32 size, rs_u32 offset, rs_bool isFinal);

rs_u8 rs_ua_packageStore(void);

rs_bool rs_trace(const rs_s8 *format,...);

rs_u32 rs_ua_validation(rs_bool multiUnit, rs_u32 index);

rs_u32 rs_ua_upgrade(rs_bool multiUnit, rs_u32 index);

rs_bool rs_ua_hasMultiUnit(void);

rs_u32 rs_ua_getMultiUnitCount(void);

rs_bool rs_ua_getCurrentUnitIndex(rs_u32 *index);

rs_bool rs_ua_setCurrentUnitIndex(rs_u32 index);

rs_bool rs_ua_getUnitDataTypeAndID(rs_u32 index, rs_u8* dataType, rs_u8* unitID);

rs_bool rs_ua_validateMultiUnitPkg(void);

rs_bool rs_ua_decodeUnitData(rs_u32 index, OUTPUT_DATA_STREAM outputFunction);

rs_bool rs_ua_getUnitRawData(rs_u32 index, OUTPUT_DATA_STREAM outputFunction);

rs_s32 rs_porting_validation(rs_bool multiUnit, rs_u32 unitCount);

rs_s32 rs_porting_update(rs_bool multiUnit, rs_u32 unitCount);

rs_bool rs_ua_updatePhaseComplete(void);

rs_bool rs_ua_validationPhaseComplete(void);

rs_s32 rs_ua_execute();


#ifdef __cplusplus
}
#endif

#endif

