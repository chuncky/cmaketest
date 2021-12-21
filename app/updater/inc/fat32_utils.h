/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
INTEL CONFIDENTIAL
Copyright 2006 Intel Corporation All Rights Reserved.
The source code contained or described herein and all documents related to the source code ("Material") are owned
by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
transmitted, distributed, or disclosed in any way without Intel's prior express written permission.

No license under any patent, copyright, trade secret or other intellectual property right is granted to or
conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
Intel in writing.
-------------------------------------------------------------------------------------------------------------------*/

#ifndef _FAT32_UTIL_
#define _FAT32_UTIL_
#define FS_SEEK_BEGIN   	0
#define FS_SEEK_CURREN	1
#define FS_SEEK_END		2
#define PATH_MAX				260
typedef struct _tagTIMEINFO
{
        UINT32    m_nMilliSec             : 10;   //Million second, 0-999
        UINT32    m_nYear                 : 12;   //Year, 0-4095
        UINT32    m_nReserved1    : 10;   //Reserved, Zero always
        UINT32    m_nMonth                : 4;    //Month, 1-12
        UINT32    m_nDate                 : 5;    //Day, 1-31
        UINT32    m_nHour                 : 5;    //Hour, 0-23
        UINT32    m_nMinute               : 6;    //Minute, 0-59
        UINT32    m_nSecond               : 6;    //Second, 0-59
        UINT32    m_nDayOfWeek    : 3;    //Day of week, 1-7
        UINT32    m_nReserved2    : 3;    //Reserved, Zero always
}TIMEINFO, *LPTIMEINFO;

//	File Info defination
typedef	struct	_tagFSFILEINFO
{
	UINT		m_nSize;			//The size of the struct
	UINT		m_nMask;			//The op mask
	TIMEINFO	m_tCreate;			//The create datetime
	TIMEINFO	m_tModify;			//The modify datetime
	TIMEINFO	m_tAccess;			//The access datetime
	UINT		m_nFileSize;		//The file size
	UINT		m_nAttr;			//The attribute
	UINT16  	m_lpszFullName[PATH_MAX + 4];	//The full path
}FSFILEINFO, *LPFSFILEINFO;
typedef	struct	_tagFAT32FILEINFO
{
    FSFILEINFO info;
    char name[100];
}FAT32FILEINFO, *LPFAT32FILEINFO;

#endif
