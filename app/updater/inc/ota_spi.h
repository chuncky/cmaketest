/****************************************************************/
#ifndef _OTA_SPI_H
#define _OTA_SPI_H

/*
*****************************************************************************************
*											ota_spi MACROS
*Note:  the following MACRO default stae: "OPEN".
*1. OTA_SPI_SUPPORT_UI_API           			If define this MACRO, ADD some functions to support display progress info when updating
*
*Note:  the following MACRO default stae: "CLOSE".
*2. OTA_SPI_UPDATING_LOG			If define this MACRO, ADD uart_log when receive package from master and write into flash
*3. OTA_SPI_DEBUG_FUNC         		If define this MACRO, ADD the debug function.
*4. OTA_SPI_DEBUG_UNIT_TEST				If define this MACRO, ADD the debug unit test
*****************************************************************************************
*/

		/* OPEN */
#define OTA_SPI_SUPPORT_UI_API

	/* CLOSE */
//#define OTA_SPI_UPDATING_LOG
//#define OTA_SPI_DEBUG_FUNC
//#define OTA_SPI_DEBUG_UNIT_TEST


#include "types.h"
#include "qspi_flash.h"
#include "ff.h"
#include "common.h"
#include "loadtable.h"
#include "LzmaDec.h"



/*+++++++++++++Log_Priority Start++++++++++++++++++++++++*/

#ifndef OTA_SPI_LOGTAG
#define OTA_SPI_LOGTAG __func__
#endif

#define OTA_SPI_LOGSEVERITY_V 1
#define OTA_SPI_LOGSEVERITY_D 2
#define OTA_SPI_LOGSEVERITY_I 3
#define OTA_SPI_LOGSEVERITY_W 4
#define OTA_SPI_LOGSEVERITY_E 5

#ifndef OTA_SPI_LOGSEVERITY
#define OTA_SPI_LOGSEVERITY OTA_SPI_LOGSEVERITY_V
//#define OTA_SPI_LOGSEVERITY OTA_SPI_LOGSEVERITY_I
#endif

#ifndef OTA_SPI_LOGNDEBUG
#ifdef OTA_SPI_NDEBUG
#define OTA_SPI_LOGNDEBUG 1
#else
#define OTA_SPI_LOGNDEBUG 0
#endif
#endif

#define OTA_SPI_LOG_PRINT_IF uart_printf

#if OTA_SPI_LOGNDEBUG
#define OTA_SPI_ALOGV(x,...)
#define OTA_SPI_ALOGD(x,...)
#else
#define OTA_SPI_ALOGV(x,...) do{if (OTA_SPI_LOGSEVERITY <= OTA_SPI_LOGSEVERITY_V) OTA_SPI_LOG_PRINT_IF(" %s [%s]: " x "\r\n","V",OTA_SPI_LOGTAG,##__VA_ARGS__);}while(0)
#define OTA_SPI_ALOGD(x,...) do{if (OTA_SPI_LOGSEVERITY <= OTA_SPI_LOGSEVERITY_D) OTA_SPI_LOG_PRINT_IF(" %s [%s]: " x "\r\n","D",OTA_SPI_LOGTAG,##__VA_ARGS__);}while(0)
#endif

#define OTA_SPI_ALOGI(x,...) do{if (OTA_SPI_LOGSEVERITY <= OTA_SPI_LOGSEVERITY_I) OTA_SPI_LOG_PRINT_IF(" %s [%s]: " x "\r\n","I",OTA_SPI_LOGTAG,##__VA_ARGS__);}while(0)
#define OTA_SPI_ALOGW(x,...) do{if (OTA_SPI_LOGSEVERITY <= OTA_SPI_LOGSEVERITY_W) OTA_SPI_LOG_PRINT_IF(" %s [%s],[Func:%s],[Line:%d]: " x "\r\n","W",OTA_SPI_LOGTAG,__func__,__LINE__,##__VA_ARGS__);}while(0)
#define OTA_SPI_ALOGE(x,...) do{if (OTA_SPI_LOGSEVERITY <= OTA_SPI_LOGSEVERITY_E) OTA_SPI_LOG_PRINT_IF(" %s [%s],[Func:%s],[Line:%d]: " x "\r\n","E",OTA_SPI_LOGTAG,__func__,__LINE__,##__VA_ARGS__);}while(0)

/*+++++++++++++Log_Priority End++++++++++++++++++++++++*/


//define Error types
typedef enum
{
	OTA_SPI_ERR_NO = 0,					//0
	OTA_SPI_ERR_READ_FAIL,				//1
	OTA_SPI_ERR_READ_Check,				//2
	OTA_SPI_ERR_GET_VERSION_FAIL,		//3
	OTA_SPI_ERR_FCS,					//4
	OTA_SPI_ERR_INDEX,					//5
	OTA_SPI_VERSION_SAME,				//6
	OTA_SPI_VERSION_DIFFERENT,			//7
	OTA_SPI_ERR_FAT_fopen,				//8
	OTA_SPI_ERR_FAT_fread,				//9
	OTA_SPI_ERR_PARAMETER,				//10
	OTA_SPI_ERR_WRITE_FAIL,				//11
	OTA_SPI_ERR_Update_File_Size,		//12
	OTA_SPI_CRC_DIFFEREBT,              //13
	OTA_SPI_CRC_SAME,					//14
	OTA_SPI_MALLOC_FAIL,				//15
	OTA_SPI_ERR_spi_nor_read,			//16
	OTA_SPI_ERR_spi_nor_erase,			//17
	OTA_SPI_ERR_spi_nor_write,          //18
	OTA_SPI_ERR_SPI_ERASE_WRITE,		//19
	OTA_SPI_UPDATE_PACKAGE_END,			//20
	OTA_SPI_ERR_CRC_Check,				//21
	OTA_SPI_ERR_f_write,				//22
	OTA_SPI_ERR_INPKT_TYPE,				//23
}OTA_SPI_ERR_T;



typedef enum{
	OTA_SPI_TYPE_UPDATER_HANDSHAKE = 10,			//10
	OTA_SPI_TYPE_GET_HEADER_CRC,  				    //11
	OTA_SPI_TYPE_RECEIVE_FILE_SIZE_OF_UPDATE_PKG ,	//12
	OTA_SPI_TYPE_RECEIVE_UPDATE_PKG ,				//13	
	OTA_SPI_TYPE_CRC_CHECK_AFTER_UPDATING,			//14
	OTA_SPI_TYPE_RESET_CP_FLAG,						//15
}OTA_SPI_PACKAGE_TYPE;




/****** 2. update flag ******/
//10 cp send updater ack flag to ap
#define MASTER_HANDSHAKE_WITH_CP_UPDATER			(0x58863311)
#define SLAVE_UPDATER_ACK_FLAG 						(0x58863312)

//11 cp receive header crc from ap
#define SLAVE_RECEIVE_UPDATE_CRC_CONF 				(0x58863314)

//12 cp receive update file size from ap
#define SLAVE_RECEIVE_FILE_SIZE_OF_UPDATE_PKG_CONF (0x58863316)

//13 cp send update pkg conf to ap
#define SLAVE_RECEIVE_UPDATE_PKG_CONF (0x58863318)

//14 cp receive and send crc command
#define MASTER_CRC_CHECK_AFTER_UPDATING_COMMAND (0x58863319)
#define SLAVE_CRC_CHECK_AFTER_UPDATING_CORRECT (0x58863320)

//15 cp reset ota_flag command
#define MASTER_RESET_SLAVE_OTA_FLAG  (0x58863321)
#define SLAVE_RESET_SLAVE_OTA_CONF  (0x58863322)







#define OTA_SPI_VERSION_OR_CRC_SAME (0)
#define OTA_SPI_VERSION_PACKET_BUFFER_LEN (0x10)
#define OTA_SPI_CRC_LENGTH  (0x4)
#define OTA_SPI_FLASH_UNIT_SIZE (0x10000)
#define OTA_SPI_FAT_FAIL (0)
//#define OTA_SPI_UPDATER_PACKAGE_NAME ("updaterspi.bin")
#define OTA_SPI_RETRY (1000)
#define OTA_SPI_ADDRESS (0x11)		/* channel 4 ---> address:0x11 */

#define OTA_SPI_RESOLUTION_RATIO (10)



#define OTA_SPI_SLAVE_FLAG_UPDATE "ASROTA2S"
#define OTA_SPI_SLAVE_FLAG_LENGTH (0x9)
#define OTA_SPI_SPACE_FLAG_FEED_INFO (0x1000)
#define OTA_SPI_SYSTEM_PART_OFFSET (0x24000)


#define OTA_SPI_SPACE_FLAG_FEED_INFO (0x1000)	//4k Bytes
#define OTA_SPI_FLAG_LENGTH (0x9)
#define OTA_SPI_FLAG_SDCARD_NOT_UPDATE (0xFF)		//9 Bytes 0xFF

#define NUM_OF_OTA_SPI_UPDATE_PACKET (64)	//for write flash

#define OTA_SPI_TIMER_DELAY (5000000)		/* delay 5s to debug ee_io_read */



/*
******************************************************************
1. spi layer frame structure ( 2048 Bytes)
------------------------------------
| SPI Header  | amux layter | resserved |SPI Header  |
------------------------------------
|    4 Bytes  | amux layter |  33B      |4 Bytes     |
------------------------------------

2. mux layer frame structure( 2007 Bytes = 2048 Bytes - 4 Bytes - 4 Bytes - 33 Bytes)
-------------------------------------------------------------
| Flag | Address | Control | Length indication | Information | FCS | Flag |
-------------------------------------------------------------
| 1B   | 1B      |  1B    |       2B          | Information | 1B  | 1B  |
-------------------------------------------------------------

3. OTA_SPI layer frame structure( 2000 Bytes )  Limit by MACRO: "MAX_MUX_PAYLOAD_SIZE" 
------------------------------------------------------
| type | len buffer    | inx  |     buf        Reserved |  FCS |
------------------------------------------------------
| 4B   | 4B          |  4B | max: 1536B       448B      |  4B  |
------------------------------------------------------
Note: Considering "Bye alignment" when erase/write flash and high SPI transimission efficiency, set buf as 1.5K Bytes.
******************************************************************
*/

/* 1. spi flag parameters */
#define MUX_FLAG (0xF9)
#define MUX_ADDRESS (0x11)				/* channel 4 ---> address:0x11 */
#define MUX_CONTROL (0x13)
#define MUX_LENGTH_INDICATION (2000)	/*  ---decimal--> 2000 */


/* 2. spi lenth parameters */
//2.1 spi layer
#define SPI_HEADER_LENGTH (4)
#define SPI_TAIL_LENGTH   (4)
//2.2 amux layer
#define AMUX_LEN      (2007)
#define AMUX_PROTOCOL_LAYER_LENGTH (7)
#define AMUX_HEADER_LENGTH   (5)
//2.3 ota_spi layer
#define OTA_SPI_LEN		(2000)			/*Equal to "MUX_LENGTH_INDICATION"; Limit by MACRO: "MAX_MUX_PAYLOAD_SIZE"  */
#define OTA_SPI_PROTOCOL_LAYER_LEN	(16)
#define OTA_SPI_MAX_BUFFER_LEN 		(1536)	/* 1.5K Bytes*/
#define OTA_SPI_PACKET_RESERVED 	(448)

//flash transimit bytes/spi
#define OTA_SPI_SEND_UPDATE_PKG_UNIT_LENGTH (1536)	/* 1.5K Bytes */


typedef struct
{
	UINT8 start_flag;
	UINT8 address;
	UINT8 control;
	UINT16 length_indication;	
	
	UINT32	type;
	UINT32	len;
	UINT32 	idx;
	UINT8	buf[OTA_SPI_MAX_BUFFER_LEN];
	UINT8   reserved[OTA_SPI_PACKET_RESERVED];
	UINT32 	fcs;

	UINT8 amux_fcs;
	UINT8 end_flag;
}AMUX_OUTPKT_T;		//this packet is totally 2k - 4 - 4 Bytes

typedef struct
{	
	UINT8 start_flag;
	UINT8 address;
	UINT8 control;
	UINT16 length_indication;
	
	UINT32	type;
	UINT32	len;
	UINT32 	idx;
	UINT8	buf[OTA_SPI_MAX_BUFFER_LEN];
	UINT8   reserved[OTA_SPI_PACKET_RESERVED];
	UINT32 	fcs;

	UINT8 amux_fcs;
	UINT8 end_flag;
}AMUX_INPKT_T;		//this packet is totally 2k - 4 - 4 Bytes

typedef enum{
	Non_Compressed = 0,
	Lzop_Compressed,
	Lzma_Compressed,
}Update_Package_T;
extern UINT32 Image$$ASR_SPI_OTA_BUFFER$$Base;

#define LZOP_COMPRESSED_MAGIC (0x4f5a4c89)
#define LZMA_COMPRESSED_MAGIC (0x8000005D)
#define UPDATE_PACKAGE_UNCOMPRESS_ADDR       ((UINT32)&Image$$ASR_SPI_OTA_BUFFER$$Base)
#define UPDATE_PACKAGE_UNCOMPRESS_MAX_LENGTH (0x7F000000 - UPDATE_PACKAGE_UNCOMPRESS_ADDR)
#define WRITE_UNIT_64K (0x00010000)
#define WRITE_UNIT_4K  (0x00001000)

#define OTA_SPI_CACHE_START		(0x7E000000)
#define OTA_SPI_CACHE_SIZE   	(0x01000000)
//#define UncompressSystemImgSize (0x0008EC00)


/*Func 1*/
INT32 ota_spi_waitforAPDone(void);

UINT32 UncompressedUpdatePackage(UINT32 type,UINT32 src_addr,UINT32 dst_addr);
INT32 LzopUncompress(   const UINT32 flash_address,const UINT32 exec_address,UINT32 *p_inLen,UINT32 *p_outLen);
INT32 WriteFileFromPsramToFlash(UINT32 dst_offset_addr,UINT32 src_addr,const UINT32 src_len);
#endif		/* _OTA_SPI_H */
/****************************************************************/
