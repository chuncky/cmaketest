/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/

#ifndef _HAL_FS_H_
#define _HAL_FS_H_

#include "uhaltype.h"
#include "media/med_main.h"

#define	false	0
#define	true	1
#define FS_NO_ERROR			0
#define	FILENAME_LEN_MAX	(256)
#define	FILENAME_PHONE_LEN_MAX	(59)

//FS_GetDiskInfo Parameter
#define FS_DI_BASIC_INFO		0x00000001L
#define FS_DI_FREE_SPACE		0x00000002L
#define FS_DI_FAT_STATISTICS	0x00000004L

// 分区类型
#define	FS_NO_ALT_DRIVE			0x00000001
#define	FS_ONLY_ALT_SERIAL		0x00000002
#define	FS_DRIVE_I_SYSTEM		0x00000004
#define	FS_DRIVE_V_NORMAL		0x00000008
#define	FS_DRIVE_V_REMOVABLE	0x00000010

// 打开文件模式
#define	FS_READ_ONLY			0x00000100L
#define	FS_OPEN_SHARED			0x00000200L
#define	FS_OPEN_NO_DIR			0x00000400L
#define	FS_OPEN_DIR				0x00000800L
#define	FS_READ_WRITE			0x00001000L
#define	FS_CREATE				0x00010000L
#define	FS_CREATE_ALWAYS		0x00020000L 
#define	FS_COMMITTED			0x01000000L
#define	FS_CACHE_DATA			0x02000000L
#define	FS_LAZY_DATA			0x04000000L
#define	FS_NONBLOCK_MODE		0x10000000L
#define	FS_PROTECTION_MODE		0x20000000L

// 文件拷贝操作参数
#define FS_MOVE_COPY             0x00000001
#define FS_MOVE_KILL             0x00000002
#define FS_MOVE_OVERWRITE        0x00000004

// 格式化操作参数
#define FS_FORMAT_HIGH           0x00000001
#define FS_FORMAT_LOW            0x00000002

// 文件移动参数
#define FS_MOVE_COPY             0x00000001
#define FS_MOVE_KILL             0x00000002
#define FS_MOVE_OVERWRITE        0x00000004

// 文件查询参数
#define FS_FILE_TYPE             0x00000004     //0x00000001 & 2 is reserved for internal
#define FS_DIR_TYPE              0x00000008
#define FS_RECURSIVE_TYPE        0x00000010
#define FS_NO_SORT               0x00000020     //Sort Parameter
#define FS_SORT_NAME             0x00000040     //Sort Parameter
#define FS_SORT_SIZE             0x00000080     //Sort Parameter
#define FS_SORT_TYPE             0x00000200     //Sort Parameter
#define FS_SORT_TIME             0x00000400     //Sort Parameter
#define FS_SORT_DTIME            0x00000100     //Sort Parameter

typedef	long	FS_HANDLE;

typedef	struct _DATATIME
{
	INT32 year;
	INT32 mon;
	INT32 day;
	INT32 hour;
	INT32 min;
	INT32 sec;
} DATETIME;

typedef	struct	__FILE_HAL_STAT
{
	UINT16		mode;
	UINT16		nlink;
	UINT32		size;
	UINT32		atim;
	UINT32		mtim;
	UINT32		ctim;
	DATETIME	dt;
	UINT8		name[FILENAME_LEN_MAX];
	UINT32		attributes;
} _FILE_HAL_STAT;
typedef	_FILE_HAL_STAT	FILE_HAL_STAT;

typedef struct _DIRENT_INFO 
{
	long 			d_ino;			// 索引节点号
	long 			d_off;			// 在目录文件中的偏移
	unsigned short	d_reclen;		// 文件名长
	unsigned char	d_type;			// 文件类型，目录还是文件
	char 			d_name[256];	// 文件名，最长255字符
	unsigned int	type;			// 扩展名决定的文件类型
	unsigned int	grouptype;		// 扩展名决定的文件属类，音频、视频等
} DIRENT_INFO;

// 遍历目录返回的文件类型
typedef	enum
{
	FILE_UNKNOWN,			// 不知道的类型
	FILE_BLK,				// 块设备
	FILE_CHR,				// 字符设备
	FILE_DIR,				// 目录
	FILE_FIFO,				// 管道
	FILE_LNK,				// 链接
	FILE_REG,				// 普通文件
	FILE_SOCK,				// socket文件
	FILE_PARENT,			// . 和 ..
} FILE_TYPE;


// 文件指针位置
typedef enum
{
	FS_FILE_BEGIN,
	FS_FILE_CURRENT,
	FS_FILE_END
} FS_SEEK_POS_ENUM;

// 文件操作模式
typedef enum
{
	MODE_TXT_READ,
	MODE_TXT_WRITE,
	MODE_TXT_WPLUS,
	MODE_TXT_RW,
	MODE_TXT_APPEND,
	MODE_TXT_PLUS,
	MODE_BIN_READ,
	MODE_BIN_WRITE,
	MODE_BIN_WPlUS,
	MODE_BIN_RW,
	MODE_BIN_APPEND,
	MODE_BIN_PLUS
} FileModeType;

#ifdef gengyue
typedef enum {
    MED_RES_OK=0,		            /* 0 */
    MED_RES_FAIL,		            /* 1 */
    MED_RES_BUSY,	               /* 2 */
    MED_RES_DISC_FULL,	         /* 3 */
    MED_RES_OPEN_FILE_FAIL,    /* 4 */
    MED_RES_END_OF_FILE,	      	/* 5 */
    MED_RES_TERMINATED,		      /* 6 */    
    MED_RES_BAD_FORMAT,	      /* 7 */
    MED_RES_INVALID_FORMAT,    /* 8 */
    MED_RES_ERROR,	               /* 9 */
    MED_RES_NO_DISC,              /* 10 */
    MED_RES_NOT_ENOUGH_SPACE,	   /* 11 */
    MED_RES_INVALID_HANDLE,	         /* 12 */
    MED_RES_NO_HANDLE,               /* 13 */
    MED_RES_RESUME,                  /* 14 */
    MED_RES_BLOCKED,	               /* 15 */
    MED_RES_MEM_INSUFFICIENT,     /* 16 */ 
    MED_RES_BUFFER_INSUFFICIENT,  /* 17 */
    MED_RES_FILE_EXIST,              /* 18 */
    MED_RES_WRITE_PROTECTION,     /* 19 */
    MED_RES_PARAM_ERROR,	         /* 20 */
    /* MP4,AAC */
    MED_RES_UNSUPPORTED_CHANNEL,	         /* 21 */
    MED_RES_UNSUPPORTED_FREQ,
    MED_RES_UNSUPPORTED_TYPE,
    MED_RES_UNSUPPORTED_OPERATION,
    MED_RES_PARSER_ERROR,
    MED_RES_VIDEO_ERROR,
    MED_RES_AUDIO_ERROR,
    MED_RES_FSAL_ERROR,
    MED_RES_MP4_PRODUCER_ERROR,
    MED_RES_MP4_SAMPLEDATA_ERROR,
    MED_RES_MP4_NO_VIDEO_TRACK,
    MED_RES_MP4_NO_AUDIO_TRACK,
    /* VR */
    MED_RES_ID_MISMATCH,	         /* 33 */
    MED_RES_ID_EXIST,
    MED_RES_TRAINING_CONTINUE,
    MED_RES_NO_SOUND,
    MED_RES_TOO_SIMILAR,
    MED_RES_TOO_DIFFERENT,
    MED_RES_NO_MATCH,
    MED_RES_SPEAK_TOO_LONG,
    MED_RES_SPEAK_TOO_SHORT,
    /* MP4,AAC */
    MED_RES_AUDIO_END,                  /* 42 */
    MED_RES_VIDEO_END,
    MED_RES_VIDEO_IMAGE_TOO_LARGE,
    MED_RES_VIDEO_FILE_TOO_LARGE,
    MED_RES_VIDEO_FRAME_RATE_TOO_HIGH,
    MED_RES_VIDEO_SEEK_PARTIAL_DONE,
    /* VRSI */ 
    MED_RES_VRSI_OK, 
    MED_RES_VRSI_ERR_UNINITIALIZED,
    MED_RES_VRSI_ERR_BUSY,
    MED_RES_VRSI_ERR_MEM_INSUFFICIENT,
    MED_RES_VRSI_ERR_DATA_ERROR,   
    MED_RES_VRSI_ERR_LIBRARY_CORRUPT,
    MED_RES_VRSI_ERR_BAD_GRAMMAR,
    MED_RES_VRSI_ERR_LANG_NOT_SUPPORT,
    MED_RES_VRSI_ERR_NO_MATCH_TAG,
    MED_RES_VRSI_ERR_TTS_TOO_LONG,
    MED_RES_VRSI_ERR_SESSION,
    MED_RES_VRSI_ERR_NO_SOUND,
    MED_RES_VRSI_ERR_CMD_TRN_FAIL,
    MED_RES_VRSI_ERR_CMD_RCG_FAIL,
    MED_RES_VRSI_ERR_DGT_RCG_FAIL,
    MED_RES_VRSI_ERR_DGT_ADP_FAIL,
    MED_RES_VRSI_ERR_UNKNOWN, 
    /* A2DP */
    MED_RES_A2DP_CLOSE,
    MED_RES_A2DP_DISCOVER_SEP_FAIL,
    MED_RES_A2DP_GET_SEP_CAP_FAIL,
    MED_RES_A2DP_NO_MATCH_CAP,
    MED_RES_A2DP_CONFIG_STREAM_FAIL,
    MED_RES_A2DP_RECONFIG_STREAM_FAIL,
    MED_RES_A2DP_OPEN_STREAM_FAIL,
    MED_RES_A2DP_START_STREAM_FAIL,
    MED_RES_A2DP_PAUSE_STREAM_FAIL,  /* 75 */
    /* HFP */
    MED_RES_HFP_CLOSE,
    MED_NO_OF_RES
} med_result_enum;
#endif

typedef struct
{
	char            Label[24];
	char            DriveLetter;
	unsigned char   WriteProtect;
	char            Reserved[2];
	kal_uint32      SerialNumber;
	kal_uint32      FirstPhysicalSector;
	kal_uint32      FATType;
	kal_uint32      FATCount;
	kal_uint32      MaxDirEntries;
	kal_uint32      BytesPerSector;
	kal_uint32      SectorsPerCluster;
	kal_uint64      TotalClusters;
	kal_uint32      BadClusters;
	kal_uint64      FreeClusters;
	kal_uint32      Files;
	kal_uint32      FileChains;
	kal_uint32      FreeChains;
	kal_uint32      LargestFreeChain;
}   FS_DiskInfo;
																		
typedef int FS_ProgressCallback(int Action, unsigned int Total, unsigned int Completed, unsigned int Info);


#ifdef __cplusplus
extern "C" {
#endif

BOOL  MMI_FS_IsDirFile(const UINT8 *pszFileName);
INT32 MMI_FS_CreateDir(const UINT8 * DirName);
INT32 MMI_FS_RemoveDir(const UINT8 * DirName);
INT32 MMI_FS_SetCurrentDir(const UINT8 * DirName);
BOOL  MMI_FS_IsExist(const UINT8 * FileName);
INT32 MMI_FS_GetCurrentDir(UINT8 * DirName, UINT32 MaxLength);
INT32 MMI_FS_GetDirSize(const UINT8 * dirName, long long* size);
INT32 MMI_FS_GetFullPath(UINT8 * buf, int buf_len);
INT32 MMI_FS_Move(const UINT8 * SrcFullPath, const UINT8 * DstFullPath, INT32 Flag, FS_ProgressCallback Progress,INT32 IfExist);
INT32 MMI_FS_Rename(const UINT8 * FileName, const UINT8 * NewName);
INT32 MMI_FS_GetFileInfo(const UINT8 * FileName, _FILE_HAL_STAT *info);
long long MMI_FS_GetFileSizeByName(const UINT8 * FileName);
INT32 MMI_FS_GetDrive(INT32 Type, INT32 Serial, INT32 AltMask);
BOOL  MMI_FS_TFCardExist(void);
BOOL  MMI_FS_TFCardAvailable(void);
INT32 MMI_FS_GetDiskInfo(const UINT8 * DriveName, FS_DiskInfo * DiskInfo, INT32 Flags);
INT32 MMI_FS_DirCount(const UINT8 * path);
INT32 MMI_FS_DirCountEx(const UINT8 * path, unsigned int* nrFiles, unsigned int* nrSubDirs);
INT32 MMI_FS_GeneralFormat(const UINT8 * DriveName, INT32 Level, void * pointer);
INT32 MMI_FS_Delete(const UINT8 * FileName);
INT32 MMI_FS_Truncate(FS_HANDLE FileHandle, INT64 nFileSize);
INT32 MMI_FS_Seek(FS_HANDLE FileHandle, INT64 Offset, INT32 Whence);
INT32 MMI_FS_Write(FS_HANDLE FileHandle, void * DataPtr, INT32 Length, INT32 * Written);
INT32 MMI_FS_Read(FS_HANDLE FileHandle, void *DataPtr, INT32 Length, INT32*ReadLen);
INT32 MMI_FS_Close(FS_HANDLE FileHandle);
FS_HANDLE MMI_FS_Open(const UINT8 * FileName, INT32 Flag);
FS_HANDLE MMI_FS_Open_DIR(const UINT8 * DirName);
INT32 MMI_FS_Close_DIR(FS_HANDLE handle);
INT32 MMI_FS_Get_Next_File(FS_HANDLE handle, DIRENT_INFO * dirinfo);
INT32 MMI_FS_Tell(FS_HANDLE FileHandle);
INT32 MMI_IsEndOfFile(FS_HANDLE FileHandle);
INT32 MMI_FS_GetFileSize(FS_HANDLE FileHandle, long long * Size);
INT32 MMI_FS_Abort(INT32 ActionHandle);
INT32 MMI_FS_Get_Abort(void);

#ifdef __cplusplus
}
#endif


#if 0
#include "fs_type.h"

#ifndef _FS_TYPE_H
#error "Please include fs_type.h first..."
#endif

#ifdef __BOARD_Z_TEST__
#include "mmi_data_types.h"
#endif

// General I/O
#define __FS_SORT_SUPPORT__
#define __FS_MOVE_SUPPORT__

#ifdef FILESYSTEMSRC_C_
#define FSAPI_EXTERN
#else
#define FSAPI_EXTERN extern 
#endif
FSAPI_EXTERN const char* MMI_FS_GetFirstFileInfoAfterSort(FS_DOSDirEntry* fs_entry);
FSAPI_EXTERN const char* MMI_FS_GetNextFileInfoAfterSort(FS_DOSDirEntry* fs_entry);
FSAPI_EXTERN INT32 MMI_FS_OpenHint(const UINT8 * FileName, UINT32 Flag, FS_FileLocationHint * DSR_Hint);
FSAPI_EXTERN INT32 MMI_FS_CloseAll(void);
//add 2010-0205 for QQ
FSAPI_EXTERN INT32 MMI_FS_Flush(FS_HANDLE FileHandle);  
//add end
FSAPI_EXTERN INT32 MMI_FS_SetSeekHint(FS_HANDLE FileHandle, UINT32 HintNum, FS_FileLocationHint * Hint);
FSAPI_EXTERN INT32 MMI_FS_Commit(FS_HANDLE FileHandle);
FSAPI_EXTERN INT32 MMI_FS_ReleaseFH(void * TaskId);
FSAPI_EXTERN INT32 MMI_FS_Abort(INT32 ActionHandle);
FSAPI_EXTERN INT32 MMI_FS_ParseFH(FS_HANDLE FileHandle);

//Information
FSAPI_EXTERN INT32 MMI_FS_GetFileSize(FS_HANDLE FileHandle, INT32 * Size);
FSAPI_EXTERN INT32 MMI_FS_GetFilePosition(FS_HANDLE FileHandle, UINT32 * Position);
FSAPI_EXTERN INT32 MMI_FS_SetFileTime(FS_HANDLE FileHandle, const FS_DOSDateTime * Time);
FSAPI_EXTERN INT32 MMI_FS_GetAttributes(const UINT8 * FileName);
FSAPI_EXTERN int MMI_FS_GetFirstCluster(FS_HANDLE FileHandle, unsigned * Cluster);
FSAPI_EXTERN INT32 MMI_FS_GetLastModifiedTime(const UINT8 * FileName);
extern INT32 MMI_FS_GetFileName(FS_HANDLE FileHandle, INT32 iNameBufferLen, UINT8 *FileName );

//File Only Operation
FSAPI_EXTERN INT32 MMI_FS_SetAttributes(const UINT8 * FileName, BYTE Attributes);
FSAPI_EXTERN INT32 MMI_FS_PseudoMerge(const UINT8 * FileName1, const UINT8 * FileName2);
FSAPI_EXTERN INT32 MMI_FS_CheckFile(const UINT8 * FileName);

//Folder Only Operation
FSAPI_EXTERN INT32 MMI_FS_GetFolderSize(const UINT8 *DirName, UINT32 Flag);

//File and Folder Operations
FSAPI_EXTERN INT32 MMI_FS_Extend(FS_HANDLE FileHandle, UINT32 Length);
FSAPI_EXTERN INT32 MMI_FS_MakeFileName(FS_HANDLE FileHandle, UINT32 Flag, UINT8 * FileName, UINT32 MaxLength);
BOOL FS_IsDirFile(UINT8 *pszFileName);
#ifdef __FS_MOVE_SUPPORT__
#endif

FSAPI_EXTERN INT32 MMI_FS_Count(const UINT8 * FullPath, INT32 Flag);
FSAPI_EXTERN INT32 MMI_FS_XDelete(const UINT8 * FullPath, INT32 Flag);

//Find File
FSAPI_EXTERN INT32 MMI_FS_FindFirst(const UINT8 * NamePattern, BYTE Attr, BYTE AttrMask, FS_DOSDirEntry * FileInfo, UINT8 * FileName, INT32 MaxLength);
FSAPI_EXTERN INT32 MMI_FS_FindNext(FS_HANDLE FileHandle, FS_DOSDirEntry * FileInfo, UINT8 * FileName, INT32 MaxLength);
FSAPI_EXTERN INT32 MMI_FS_FindFirstN(const UINT8 * NamePattern, FS_Pattern_Struct * PatternArray, INT32 PatternNum, BYTE ArrayMask, BYTE Attr, BYTE AttrMask, FS_DOSDirEntry * FileInfo, UINT8 * FileName, INT32 MaxLength, INT32 EntryIndex, INT32 Flag);
FSAPI_EXTERN INT32 MMI_FS_FindNextN(FS_HANDLE Handle, FS_Pattern_Struct * PatternArray, INT32 PatternNum, BYTE ArrayMask, FS_DOSDirEntry * FileInfo, UINT8 * FileName, INT32 MaxLength, INT32 Flag);
FSAPI_EXTERN INT32 MMI_FS_FindClose(FS_HANDLE FileHandle);

#ifdef __FS_SORT_SUPPORT__
FSAPI_EXTERN INT32 MMI_FS_XFindReset(const UINT8 * Pattern, FS_Pattern_Struct * PatternArray, INT32 PatternNum, BYTE ArrayMask, BYTE AttrMask, INT32 Flag, INT32 Timeout, INT32 * Position);
FSAPI_EXTERN INT32 MMI_FS_XFindStart(const UINT8 * Pattern, FS_DOSDirEntry * FileInfo, UINT8 * FileName, INT32 MaxLength, INT32 Index, INT32 * Position, INT32 Flag);
FSAPI_EXTERN INT32 MMI_FS_XFindClose(INT32 * Position);
#endif

//Drive Management
FSAPI_EXTERN INT32 MMI_FS_CreateBootSector(void * BootSector, const FS_PartitionRecord * Partition, BYTE MediaDescriptor, UINT32 MinSectorsPerCluster, UINT32 Flags);
FSAPI_EXTERN INT32 MMI_FS_GetPartitionInfo(const UINT8 * DriveName, FS_PartitionRecord * PartitionInfo);
FSAPI_EXTERN INT32 MMI_FS_CommitAll(const UINT8 * DriveName);
FSAPI_EXTERN INT32 MMI_FS_MappingDrive(UINT32 UpperOldDrv, UINT32 UpperNewDrv);

#ifdef __FS_QM_SUPPORT__
FSAPI_EXTERN INT32 MMI_FS_QmGetFree(const BYTE * Path);
#endif

FSAPI_EXTERN INT32 MMI_FS_GetClusterSize(UINT32 DriveIdx);

#if (defined __FS_CHECKDRIVE_SUPPORT__) || (defined _NAND_FLASH_BOOTING_) || (defined __FS_SYSDRV_ON_NAND__)
FSAPI_EXTERN INT32 MMI_FS_GetCheckBufferSize(const UINT8 * DriveName);
FSAPI_EXTERN INT32 MMI_FS_CheckDrive(const UINT8 * DriveName, BYTE * Checkbuffer);
#endif

//Disk Management
FSAPI_EXTERN INT32 MMI_FS_CreateMasterBootRecord(void * SectorBuffer, const FS_PartitionRecord * DiskGeometry);
FSAPI_EXTERN INT32 MMI_FS_SplitPartition(void * MasterBootRecord, UINT32 Sectors);
FSAPI_EXTERN void MMI_FS_ShutDown(void);
FSAPI_EXTERN INT32 MMI_FS_UnlockAll(void);
FSAPI_EXTERN INT32 MMI_FS_SanityCheck(void);
FSAPI_EXTERN INT32 MMI_FS_SetDiskFlag(void);
FSAPI_EXTERN INT32 MMI_FS_CheckDiskFlag(void);
FSAPI_EXTERN INT32 MMI_FS_ClearDiskFlag(void);
FSAPI_EXTERN INT32 MMI_FS_GetDevType(const UINT8 * Path);
FSAPI_EXTERN INT32 MMI_FS_GetDevStatus(INT32 DriveIdx, INT32 Flag);
FSAPI_EXTERN INT32 MMI_FS_LockFAT(UINT32 Type);

//Card management
FSAPI_EXTERN INT32 MMI_FS_TestMSDC(INT32 MSDCIndex);
FSAPI_EXTERN INT32 MMI_FS_CloseMSDC(INT32 MSDCIndex, INT32 Mode);
//OTG Card Reader Management 
FSAPI_EXTERN INT32 MMI_FS_ConfigExtDevice(INT32 Action, FS_Driver *DrvFuncs, INT32 Slots, void **SlotIdArray, INT32 *SlotsConfiged);

//Debug
#ifdef __FS_TRACE_SUPPORT__
FSAPI_EXTERN INT32 MMI_FS_SetTrace(UINT32 Flag, UINT32 Timeout);
FSAPI_EXTERN INT32 MMI_FS_DumpFHTable(void);
#endif

//Copyright
#ifdef __P_PROPRIETARY_COPYRIGHT__
FSAPI_EXTERN INT32 MMI_FS_SweepCopyrightFile(void);
FSAPI_EXTERN INT32 MMI_FS_GetCopyrightList(void);
FSAPI_EXTERN INT32 MMI_FS_GetCopyrightConfig(UINT32 *Protect, UINT32 *NonProtect, const UINT8 **Folder,
                          const FS_Pattern_Struct **Pattern, UINT32 **List1, UINT32 **List2);
#endif
void ConvertRelative2Full(U8 *pFullName, U8 *pRelativeName);
INT32 ConvertDosName2Unix(U8 *pUnixName, CONST U8 *pDosName);

#endif // gengyue

#endif //_HAL_FS_H_

