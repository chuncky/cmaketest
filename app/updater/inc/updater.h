#ifndef OTA_UPDATE
#define OTA_UPDATE
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "libc.h"
#include "2ndboot.h"

#ifndef XZ_BUF_SIZE
#define XZ_BUF_SIZE 2048
#endif



/*+++++++++++++ALI_Log_Priority Start++++++++++++++++++++++++*/

#ifndef ALI_LOG_TAG
#define ALI_LOG_TAG __func__
#endif

#define ALI_LOG_SEVERITY_V 1
#define ALI_LOG_SEVERITY_D 2
#define ALI_LOG_SEVERITY_I 3
#define ALI_LOG_SEVERITY_W 4
#define ALI_LOG_SEVERITY_E 5

#ifndef ALI_LOG_SEVERITY
#define ALI_LOG_SEVERITY ALI_LOG_SEVERITY_V
#endif

#ifndef ALI_LOG_NDEBUG
#ifdef ALI_NDEBUG
#define ALI_LOG_NDEBUG 1
#else
#define ALI_LOG_NDEBUG 0
#endif
#endif

#define ALI_LOG_PRINT_IF uart_printf

#if ALI_LOG_NDEBUG
#define ALI_ALOGV(x,...)
#define ALI_ALOGD(x,...)
#else
#define ALI_ALOGV(x,...) do{if (ALI_LOG_SEVERITY <= ALI_LOG_SEVERITY_V) ALI_LOG_PRINT_IF(" %s [%s]: " x "\r\n","V",ALI_LOG_TAG,##__VA_ARGS__);}while(0)
#define ALI_ALOGD(x,...) do{if (ALI_LOG_SEVERITY <= ALI_LOG_SEVERITY_D) ALI_LOG_PRINT_IF(" %s [%s]: " x "\r\n","D",ALI_LOG_TAG,##__VA_ARGS__);}while(0)
#endif

#define ALI_ALOGI(x,...) do{if (ALI_LOG_SEVERITY <= ALI_LOG_SEVERITY_I) ALI_LOG_PRINT_IF(" %s [%s]: " x "\r\n","I",ALI_LOG_TAG,##__VA_ARGS__);}while(0)
#define ALI_ALOGW(x,...) do{if (ALI_LOG_SEVERITY <= ALI_LOG_SEVERITY_W) ALI_LOG_PRINT_IF(" %s [%s],[Func:%s],[Line:%d]: " x "\r\n","W",ALI_LOG_TAG,__func__,__LINE__,##__VA_ARGS__);}while(0)
#define ALI_ALOGE(x,...) do{if (ALI_LOG_SEVERITY <= ALI_LOG_SEVERITY_E) ALI_LOG_PRINT_IF(" %s [%s],[Func:%s],[Line:%d]: " x "\r\n","E",ALI_LOG_TAG,__func__,__LINE__,##__VA_ARGS__);}while(0)

/*+++++++++++++ALI_Log_Priority End++++++++++++++++++++++++*/


/* OTA update errorno. */
typedef enum {
     OTA_UPDATER_OK         = 0,
     OTA_NB_INVALID_PARAM   = -1,
     OTA_NB_HEADER_FAIL     = -2,
     OTA_NB_MEMORY_FAIL     = -3,
     OTA_NB_READ_CTRL_FAIL  = -4,
     OTA_NB_READ_DIFF_FAIL  = -5,
     OTA_NB_READ_OLD_FAIL   = -6,
     OTA_NB_READ_EXTRA_FAIL = -7,
     OTA_NB_WRITE_DATA_FAIL = -8,
     OTA_NB_CRC_COMP_FAIL   = -9,
     OTA_XZ_PARAM_FAIL      = -10,
     OTA_XZ_CRC_FAIL        = -11,
     OTA_XZ_UNCOMP_FAIL     = -12,
     OTA_XZ_VERIFY_FAIL     = -13,
     OTA_XZ_MEM_FAIL        = -14,
} OTA_UPDATER_E;

/* OTA upgrade magic <--> upg_flag */
#define OTA_UPGRADE_ALL    0x9669 /* upgrade all image: kernel+framework+app */
#define OTA_UPGRADE_XZ     0xA55A /* upgrade xz compressed image */
#define OTA_UPGRADE_DIFF   0xB44B /* upgrade diff compressed image */
#define OTA_UPGRADE_KERNEL 0xC33C /* upgrade kernel image only */
#define OTA_UPGRADE_APP    0xD22D /* upgrade app image only */

#define OTA_URL_LEN        256 /*OTA download url max len*/
#define OTA_HASH_LEN       66  /*OTA download file hash len*/
#define OTA_SIGN_LEN       256 /*OTA download file sign len*/
#define OTA_VER_LEN        64  /*OTA version string max len*/
typedef struct  {
    unsigned int   dst_adr;     /*Single Bank: Destination Address: APP partition.*/
    unsigned int   src_adr;     /*Single Bank: Copy from Source Address: OTA partition.*/
    unsigned int   len;         /*Single Bank: Download file len */
    unsigned short crc;         /*Single Bank: Download file CRC */
    unsigned short upg_flag;    /*Upgrade flag: OTA_UPGRADE_ALL OTA_UPGRADE_XZ OTA_UPGRADE_DIFF*/
    unsigned char  boot_count;  /*Boot count: When >=3 Rollback to old version in BL for dual-banker boot*/
    unsigned int   upg_status;  /*OTA upgrade status*/
    unsigned char  hash_type;   /*OTA download hash type*/
    char  url[OTA_URL_LEN];     /*OTA download url*/
    char  sign[OTA_SIGN_LEN];   /*OTA download file sign*/
    char  hash[OTA_HASH_LEN];   /*OTA download file hash*/
    char  ver[OTA_VER_LEN];     /*OTA get version*/
    unsigned int   old_size;    /*Diff upgrade: patch old data size*/
    unsigned short patch_num;   /*Diff upgrade: patch num*/
    unsigned short patch_status;/*Diff upgrade: patch status*/
    unsigned int   patch_off;   /*Diff upgrade: patch offset*/
    unsigned int   new_off;     /*Diff upgrade: patch new data offset*/
    unsigned int   new_size;    /*Diff upgrade: patch new data size*/
    unsigned char  reserve[18];
    unsigned short param_crc;   /*OTA Parameter crc*/
} ota_boot_param_t;

/* OTA updater component: User APIs. call it to complete FW uprade in your bootloader */
int ota_updater(void);                                         /* ota updater main entry:ota upgrade check */
int ota_nbpatch_main(void);                                    /* ota nbpatch main enrty. */
int ota_xz_main(void);                                         /* ota xz uncompress main enrty */
int ota_image_check(unsigned int addr, unsigned int size, unsigned int crc); /* ota raw image crc check */
int ota_image_copy(unsigned int dst, unsigned int src, unsigned int size);   /* ota raw image copy upgrade */
int ota_patch_read_param(ota_boot_param_t *ota_param);         /* ota patch read parameter */
int ota_patch_write_param(ota_boot_param_t *ota_param);        /* ota patch update parameter */
#endif
