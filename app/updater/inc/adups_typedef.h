#ifndef _ADUPS_TYPEDEF_H_
#define _ADUPS_TYPEDEF_H_


#include "stdlib.h"
#include "stdio.h"
#include "string.h"
//#include "bl_common.h"
//#include "timers.h"
//#include "abup_connection.h"
#ifndef adups_extern
#ifdef __cplusplus
#define adups_extern extern "C"
#else
#define adups_extern extern
#endif
#endif//adups_extern

#define adups_DebugPrint 	printf

#define ADUPS_TEST

//#define ADUPS_TEST_POWEROFF
 

#define ADUPS_USE_NV

typedef char                    adups_char;

typedef unsigned short          adups_wchar;

typedef unsigned char           adups_uint8;

typedef signed char             adups_int8;

typedef unsigned short int      adups_uint16;

typedef signed short int        adups_int16;

typedef unsigned int            adups_uint32;

typedef signed int              adups_int32;

typedef unsigned long long   adups_uint64;

typedef signed long long     adups_int64;

typedef   int   adups_socint;

#define PNULL NULL

//#ifndef ADUPS_BLOCK_SIZE
//#define ADUPS_BLOCK_SIZE (4096)
//#endif


typedef enum 
{
    ADUPS_FALSE,
    ADUPS_TRUE
} ADUPS_BOOL;


enum FotaState {
	STATE_INIT = 1,    
	STATE_CV,    
	STATE_DL,    
	STATE_RD,    
	STATE_UG,
	STATE_RU,
	STATE_RG
};

typedef enum
{
	FOTA_SUCCESS=1000,
	FOTA_PID_ERROR,
	FOTA_PROJECT_ERROR,
	FOTA_PARAM_INVAILD,
	FOTA_PARAM_LOST,
	FOTA_SYS_ERROR,
	FOTA_JSON_ERROR,
	FOTA_RG_SIGN_ERROR=2001,
	FOTA_CV_LAST_VERSION=2101,
	FOTA_CV_INVAILD_VERSION,
	FOTA_CV_UNREG_DEVICE,
	FOTA_DL_STATE_ERROR=2201,
	FOTA_DL_DELTAID_ERROR,
	FOTA_RP_DELTAID_ERROR=2301,
	FOTA_RP_UPGRADE_STATE_ERROR	
}FOTACODE;

#define DL_SUCCESS ADUPS_DL_SUCCESS
enum DownloadStatus {
	DL_SUCCESS = 1,  
	DL_NO_NETWORK,
	DL_CANCELED,
	DL_CACHE_READ_ERROR,
	DL_SERVER_ERROR,    
	DL_TIMEOUT, 
	DL_NETWORK_ERROR,
	DL_MD5_MIS_MATCH,
	DL_TIMER_CREAT_ERROR,
	DL_INIT_FLASH_ERROR,
	DL_FILESIZE_TOO_LARGE,
	DL_NO_SPACE,
	DL_PATH_NOT_EXIST,
	DL_ERRORS=99,
};



enum UpgradeStatus {
	UP_SUCCESS = 3000,    
	UP_UNZIP_ERROR,    
	UP_MD5_FAIL,    
	UP_NO_SPACE,    
	UP_WRITE_FILE_ERROR
};

typedef enum 
{
	CK_NEW_VERSION = 1000,    
	CK_PRODUCT_ID_INVALID,    
	CK_PROJECT_DOES_NOT_EXIST,    
	CK_PARAM_INVALID,   
	CK_PARAM_NEED_MORE, 
	CK_SYSTEM_ERROR,
	CK_JSON_PARSE_ERROR,
	CK_PARAM_ERROR,
	CK_MID_LENGTH_ERROR,
	CK_SIGN_ERROR=2001,
	CK_LAST_VERSION=2101,
	CK_END
}CKRESULT;

enum NetStatusMachine {
	NET_STAT_SUCCESS = 2000,    
	NET_STAT_DNS_REQ,		//2001
	NET_STAT_DNS_GETTING,	 
	NET_STAT_DNS_SUCC,	 
	NET_STAT_NEWSOC_REQ,    //2004
	NET_STAT_NEWSOC_SUCC,	
	NET_STAT_NEWSOC_ERR,    
	NET_STAT_DNS_ERR,
	NET_STAT_CONN_REQ,		//2008
	NET_STAT_CONNECTING,
	NET_STAT_CONN_SUCC,
	NET_STAT_CONN_ERR,
	NET_STAT_SEND_REQ,		//2012
	NET_STAT_SENDING,
	NET_STAT_SEND_SUCC,
	NET_STAT_SEND_ERR,
	NET_STAT_RECV_REQ,		//2016
	NET_STAT_RECVING,
	NET_STAT_RECV_SUCC,
	NET_STAT_RECV_SUCC2,
	NET_STAT_RECV_ERR,
	NET_STAT_CLOSE_REQ,		//2021
	NET_STAT_CLOSE_SUCC,
	NET_STAT_CLOSE_ERR
};

typedef struct
{
    adups_uint16 nYear;
    adups_uint8 nMonth;
    adups_uint8 nDay;
    adups_uint8 nHour;
    adups_uint8 nMin;
    adups_uint8 nSec;
    adups_uint8 DayIndex; /* 0=Sunday */
} Adups_time_struct;

typedef struct _VersionInfo {
	adups_int32  status;
	adups_char *msg;
	adups_char *versionName;
	adups_int32   fileSize;
	adups_char *deltaID;
	adups_char *md5sum;
	adups_char *deltaUrl;
	adups_char *publishDate;
	adups_char *releaseContent;
	adups_int32  storageSize;
	adups_int32 	battery;
	adups_int32  	download_status;
	adups_uint32 download_start;
	adups_uint32 download_end;
	adups_int32  update_status;
	adups_char *deviceSecret;
	adups_char *deviceId;
} VersionInfo;


typedef struct _MobileParamInfo {
	adups_char  *mid;
	adups_char  *version;
	adups_char  *oem;
	adups_char  *models;
	adups_char  *productid;
	adups_char  *productsec;	
	adups_char  *platform;
	adups_char  *device_type;
	adups_char  *mac;
} MobileParamInfo;

typedef struct _DownParamInfo {
	adups_char  *mid;
	adups_char  *token;
	adups_char  *deltaID;
	adups_char  *productid;
	adups_char  *productsec;	
	adups_int32         download_status;
	adups_int32      download_start;
	adups_int32      download_end;
	adups_uint32 download_size;
	adups_char *download_ip;
} DownParamInfo;

typedef struct _UpgradeParamInfo {
	adups_char  *mid;
	adups_char  *token;
	adups_char  *deltaID;
	adups_char  *productid;
	adups_char  *productsec;	
	adups_int32  update_status;
	adups_char   *extendString;
} UpgradeParamInfo;



typedef struct _streambuf {
    adups_char        *data;
    adups_int32         limit;
    adups_int32         size;
} streambuf;

typedef struct _request {
    streambuf   header;
    streambuf   body;
} Request;



#ifndef NULL
#define NULL  (void *)0           /*  NULL    :   Null pointer */
#endif


#define E_ADUPS_SUCCESS			0
#define E_ADUPS_FAILURE			-1
#define E_ADUPS_NOMEMORY			-2
#define E_ADUPS_NOTEXIST			-3
#define E_ADUPS_WOULDBLOCK		-4
#define E_ADUPS_SIM_NOTEXIST		-5
#define E_ADUPS_MID_ERROR		-6
#define E_ADUPS_NETWORK_ERROR		-7
#define E_ADUPS_DOWNLOAD_FAIL		-8
#define E_ADUPS_LAST_VERSION		-9
#define E_ADUPS_TOKEN_WORNG		-10
#define E_ADUPS_PROJECT_WORONG		-11
#define E_ADUPS_PARAM_WORONG		-12
#define E_ADUPS_NET_CONNECTING	-13
#define E_ADUPS_DOWNLOAD_CONNECTING		-14


#define ADUPS_MAX_DEBUG_BUF_LEN		256

#define RECV_BLOCK_SIZE 1024 //can not over the 1024 limit

typedef void (*af_callback)(VersionInfo *info, adups_int32 state);

typedef void (*adups_notify)(adups_int8 data);

typedef void (*adups_timer_callback)(void);


typedef enum 
{
	ADUPS_READ_NOTIFY,
	ADUPS_WRITE_NOTIFY,
	ADUPS_CLOSE_NOTIFY,
	ADUPS_ERROR_NOTIFY,
	ADUPS_SOCMGR_END
} ADUPS_SOCMGR_USR;

typedef enum 
{
	ADUPS_DIFF_PATCH,
	ADUPS_FULL_PATCH,
	ADUPS_PATCH_END
} ADUPS_PATCH_METHOD;

typedef struct 
{
	adups_notify adups_read_notify;
	adups_notify adups_write_notify;
	adups_notify adups_close_notify;
	adups_notify adups_net_error_notify;
	adups_int32 adups_soc_state;
	adups_int32 adups_request_id;
	adups_int32 adups_tcp_socket_id;
	adups_int32 adups_net_sm;

} ADUPS_SOCKET_MGR_STRUCT;


typedef enum 
{
	ADUPS_READONLY	= 0x0001,
	ADUPS_READWRITE	= 0x0002,
	ADUPS_CREATE 	= 0x0004,
	ADUPS_APPEND 	= 0x0008,
	ADUPS_TRUNCATE = 0x0010
}ADUPS_OpenMode;


typedef enum
{
	ADUPS_SEEK_START,
	ADUPS_SEEK_END,
	ADUPS_SEEK_CURRENT
}ADUPS_FileSeekType;


typedef struct
{
	adups_uint8 timerid;
	adups_uint8 timermode;
	adups_uint32 delay;
	adups_timer_callback func_ptr;
	void *timer_ptr;
}ADUPS_TIMER_STRUCT;



extern void adups_fota_upgrade(void);
extern ADUPS_BOOL adups_get_pre_patch_flag(void);
extern ADUPS_BOOL adups_get_pre_check_flag(void);
extern void adups_bl_debug_print(void* ctx, const adups_char* fmt, ...); 


//#if 1//#ifndef UNUSE_COMPRESS   //add by hzy for reduce rom region,20161223
//#define BL_PRINT(x, fmt, arg...)    bl_print(fmt, ##arg)
//#define adups_bl_debug_print(x, fmt, arg...) bl_print(0,fmt, ##arg)
//#define adups_extern extern
//#else
//#define adups_bl_debug_print( level, fmt, ...) 
//#endif

#endif

