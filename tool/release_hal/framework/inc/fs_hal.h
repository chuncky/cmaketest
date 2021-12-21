/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/

#ifndef _FS_HAL_H_
#define _FS_HAL_H_

#define	false	0
#define	true	1
#define FS_NO_ERROR			0
#define	FILENAME_LEN_MAX	(256)
#define	FILENAME_PHONE_LEN_MAX	(59)

/******************************************************************************/
//The operation completed successfully
#define ERR_SUCCESS                                       0x0
/******************************************************************************/

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

#endif //_FS_HAL_H_

