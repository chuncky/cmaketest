/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/

#ifndef _TS_H_
#define _TS_H_

#include "uhaltype.h" 

#define CMID_DRV_TS_ID         1
#define DM_TS_ID              2
#define CMID_DM_TS_ID          2
#define CMID_PM_TS_ID          3
#define BASE_BAL_TS_ID        10
#define BASE_COS_TS_ID        11
#define BASE_DB_TS_ID         12
#define BASE_REG_TS_ID        13
#define BASE_FFS_TS_ID        14
#define BASE_FAT_TS_ID        15
#define BASE_SUL_TS_ID        16
#define BASE_TM_TS_ID         17
#define BASE_VFS_TS_ID        18
#define TMS_TS_ID             19

#define CMID_AOM_TS_ID         30
#define CMID_CFG_TS_ID         31
#define CMID_NW_TS_ID          32
#define CMID_SIM_TS_ID         33
#define CMID_CC_TS_ID          34
#define CMID_SMS_TS_ID         35
#define CMID_SS_TS_ID          36
#define CMID_PBK_TS_ID         37
#define CMID_GPRS_TS_ID        38
#define CMID_CSD_TS_ID         39
#define CMID_TCPIP_TS_ID       40
#define CMID_PPP_TS_ID         41
#define CMID_SHELL_TS_ID       42
#define AT_TS_ID              50
#define MMI_TS_ID_BASE        100

#define MMI_TS_ID             (MMI_TS_ID_BASE)

#define TS_PORT_SERIAL      0x01

#define TS_PORT_USB           0x08
#define TS_PORT_FILE          0x10

#define TS_OUTPUT_FILE        0x01
#define TS_OUTPUT_VC          0x02 // VC Debug output window.
#define TS_OUTPUT_CONSOLE     0x04 // Console window.
#define TS_OUTPUT_MF32        0x08 // MF32 tools

#define TS_ERROR_LEVEL        0x01
#define TS_WARNING_LEVEL      0x02
#define TS_INFO_LEVEL         0x04
#define TS_ASSERT_LEVEL       0x08

#define CSW_TS_ERROR             0x0 // 0 means open C_ERROR TRC
#define CSW_TS_WARNING           0x4
#define CSW_TS_BRIEF             0x2
#define CSW_TS_DETAIL            0x1

#define C_ERROR             (CSW_TS_ERROR   <<8)
#define C_WARNING       	(CSW_TS_WARNING <<8) 
#define C_BRIEF             (CSW_TS_BRIEF   <<8) 
#define C_DETAIL            (CSW_TS_DETAIL  <<8) 

#endif // _H_

