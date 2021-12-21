/*
*****************************************************************************************
*											Control Uart Log
*1. OTA_SPI_LOGTAG:                names as file name, for example, ota_spi.c ---> ota_spi
*
*Note:  the following MACRO default stae: "OPEN".
*2. OTA_SPI_NDEBUG              	If define MACRO OTA_NDEBUG, remove "OTA_SPI_ALOGV" and "OTA_SPI_ALOGD" log
*
*****************************************************************************************
*/
#define OTA_SPI_LOGTAG "ota_spi"

	/* OPEN */
#define OTA_SPI_NDEBUG

/*
*****************************************************************************************
                                             define some types
*****************************************************************************************
*/
typedef unsigned long long  	UINT64,   *PUINT64;

/*
*****************************************************************************************
                                             define some types
*****************************************************************************************
*/
typedef unsigned long long  	UINT64,   *PUINT64;
/*
*****************************************************************************************
*											INCLUDE FILES
*****************************************************************************************
*/
#include "ota_spi.h"

/*
*****************************************************************************************
*											Extern Variables
*****************************************************************************************
*/
extern struct spi_flash_chip *chip;

/*
*****************************************************************************************
*											GLOBAL VARIABLES
*****************************************************************************************
*/

UINT8  OTA_SPI_Cp2Apbuf[OTA_SPI_MAX_BUFFER_LEN];


/*
*****************************************************************************************
*											LOCAL GLOBAL VARIABLES
*****************************************************************************************
*/
static AMUX_INPKT_T	cp_amux_inpkt;
static AMUX_OUTPKT_T cp_amux_outpkt;

#ifdef OTA_SPI_DEBUG_FUNC
FIL ota_spi_handle;
#endif

#ifdef FOTA_ASRSPI
#define COMPRESSED_ALGORITHM_BUFFER_SIZE (0x00010000)
UINT8 g_compressed_buffer[COMPRESSED_ALGORITHM_BUFFER_SIZE];
#endif

#if 1
/* align 4 Bytes to work around memcpy bug, otherwise, "data abort" will orrcur when memcpy */
__align(4) static UINT8 cp_amux_inpkt_buf[AMUX_LEN];
__align(4) static UINT8 cp_amux_outpkt_buf[AMUX_LEN];

static UINT8 ota_spi_receive_update_buf[OTA_SPI_SEND_UPDATE_PKG_UNIT_LENGTH];	//1.5K
static UINT8 ota_spi_flash_data_buf[OTA_SPI_FLASH_UNIT_SIZE];					//64K
static UINT8 ota_spi_backup_flash_data_buf[OTA_SPI_FLASH_UNIT_SIZE];
static UINT32 ota_spi_update_file_size;
static UINT32 received_update_file_size = 0;
static BOOL is_receive_update_pkg_end = FALSE;

static UINT8 ota_spi_crc_res_receive[OTA_SPI_CRC_LENGTH];
#else
UINT8 cp_amux_inpkt_buf[AMUX_LEN];
UINT8 cp_amux_outpkt_buf[AMUX_LEN];
#endif
static unsigned int current_cp_packet_index = 0;

//CRC-16 table
static UINT16 crc16_table[256] = {
0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};


/*
*****************************************************************************************
*											Extern VARIABLES
*****************************************************************************************
*/


/*
*****************************************************************************************
*											Extern FUNCTION PROTOTYPES
*****************************************************************************************
*/
#ifdef OTA_SPI_DEBUG_FUNC
extern void ota_spi_print_amux_buf(const char *p_buf, const UINT32 buf_len);
#endif
extern unsigned long GetTimer0CNT(void);
extern void Timer0_Switch(unsigned char OnOff);


/*
*****************************************************************************************
*											LOCAL FUNCTION PROTOTYPES
*****************************************************************************************
*/
static OTA_SPI_ERR_T ota_spi_erase_all(const UINT32 offset,const UINT32 erase_size, const UINT32 erase_unit);

static OTA_SPI_ERR_T ota_spi_crc_check_Image_in_Flash(const UINT32 offset, const UINT32 update_file_size,const UINT16 receive_crc_res);

static UINT32 ota_spi_crc_check_16(UINT8* data, UINT32 length,UINT16 crc_reg);

static OTA_SPI_ERR_T ota_spi_reset_ota_spi_flag(const UINT32 offset);

static void ota_spi_slave_inpkt_handler(AMUX_INPKT_T *p_cp_amux_inpkt,OTA_SPI_ERR_T *p_ota_spi_res);

static void ota_spi_dump_input_packet(UINT8 *input_buf,const int buf_len );




//get the fota_param offset
static UINT32 get_fota_param_offset(void)
{
	return (get_fota_param_start_addr() - 0x80000000);
}

void OTA_SPI_UPDATE(void)
{	
	OTA_SPI_ERR_T ota_spi_res;
	UINT32 count_receive_update_package = 0;
	UINT32 sum_count_receive_update_pkg = 0;
	UINT16 ota_spi_ratio;
	UINT32 resolution_convert_num_files;
	Update_Package_T update_package_type;
	UINT32 UncompressedSystemSize;

	OTA_SPI_ALOGI("Start!");
	//OTA_SPI_ALOGD("len of AMUX_INPKT_T:%d,cp_amux_inpkt_buf:%d",sizeof(AMUX_INPKT_T),sizeof(cp_amux_inpkt_buf));

#ifdef OTA_SPI_DEBUG_UNIT_TEST	
	//goto debug_0;	//[0] CP send reboot flag to master
	//goto debug_1;	//[1] CP receive header info:crc_res from AP
	//goto debug_4;	//[4] CP crc check image has been written in flash
	//goto debug_5;	//[5] reset ota_spi_flag
#endif

#ifdef OTA_SPI_DEBUG_UNIT_TEST
debug_0:
#endif

	/* delay a while to wait MCU open spi interrupt */	
	//ota_spi_delay(200 * 10);

	/* [0] CP send reboot flag to master */
	uart_printf("\r\n===============%s===============\r\n","[0] CP updater handshake with AP");
	ota_spi_res = ota_spi_slave_receive_send_pkg(OTA_SPI_TYPE_UPDATER_HANDSHAKE);
	if(ota_spi_res != OTA_SPI_ERR_NO){
		OTA_SPI_ALOGE("slave send version info fail!");
		while(1);
		//TODO: retry or abandon upgrades?
	}
	
#ifdef OTA_SPI_DEBUG_UNIT_TEST
	uart_printf("debug_0 completed\r\n");
	//while(1);

debug_1:
#endif

	/* delay a while to wait MCU prepare write data. 200 ---> 1s */	
	//ota_spi_delay(200 * 5);


	/* [1] CP receive header info:crc_res from AP */
	uart_printf("\r\n===============%s===============\r\n","[1] CP receive header info:crc_res from AP");
	ota_spi_res = ota_spi_slave_receive_send_pkg(OTA_SPI_TYPE_GET_HEADER_CRC);
	if(ota_spi_res != OTA_SPI_ERR_NO){
		OTA_SPI_ALOGE("slave send version info fail!");
		while(1);
		//TODO: retry or abandon upgrades?
	}

#ifdef OTA_SPI_DEBUG_UNIT_TEST
	uart_printf("debug1 completed\r\n");
	while(1);	
	
debug_2:
#endif

	/* [2] CP receive file size of update pkg from AP */
	uart_printf("\r\n===============%s===============\r\n","[2] CP receive file size of update pkg from AP and erase flash");
	ota_spi_res = ota_spi_slave_receive_send_pkg(OTA_SPI_TYPE_RECEIVE_FILE_SIZE_OF_UPDATE_PKG);
	if(ota_spi_res != OTA_SPI_ERR_NO){
		OTA_SPI_ALOGE("slave receive package numbers fail!");
		while(1);
		//TODO: retry or abandon upgrades?
	}
	OTA_SPI_ALOGI("update_file_size:0x%x",ota_spi_update_file_size);
	
#ifdef OTA_SPI_DEBUG_UNIT_TEST
	uart_printf("debug2 completed\r\n");
	//while(1);	
	
debug_3:
#endif
	
	/* [3] CP receive update package from AP */
	uart_printf("\r\n===============%s===============\r\n","[3] CP receive update package from AP and write into flash");
	//1 some prepare work
	//1.1 init some variables
	count_receive_update_package = 0;
#ifdef OTA_SPI_SUPPORT_UI_API
	//1.2 calculate numbers of package when receive update file.
	if(ota_spi_update_file_size % OTA_SPI_MAX_BUFFER_LEN == 0)
		sum_count_receive_update_pkg = ota_spi_update_file_size / OTA_SPI_MAX_BUFFER_LEN;
	else
		sum_count_receive_update_pkg = ota_spi_update_file_size / OTA_SPI_MAX_BUFFER_LEN + 1;

	//1.3 calculate number of packages when increase one "OTA_SPI_RESOLUTION_RATIO" progress bar
	if(sum_count_receive_update_pkg % OTA_SPI_RESOLUTION_RATIO == 0)
		resolution_convert_num_files = sum_count_receive_update_pkg / OTA_SPI_RESOLUTION_RATIO;
	else
		resolution_convert_num_files = sum_count_receive_update_pkg / OTA_SPI_RESOLUTION_RATIO + 1;
#endif
	
	
	while(!is_receive_update_pkg_end){
#ifdef OTA_SPI_UPDATING_LOG
		uart_printf("  [3_%d]\r\n",count_receive_update_package);
#endif

#ifdef OTA_SPI_SUPPORT_UI_API
		//display info2 ---dispaly progress bar in lcd																					   
		if(count_receive_update_package == 0){
			ota_spi_ratio = 0;
			ota_spi_progress_ratio(ota_spi_ratio);
		}
#endif
		ota_spi_res = ota_spi_slave_receive_send_pkg(OTA_SPI_TYPE_RECEIVE_UPDATE_PKG);
		if(ota_spi_res != OTA_SPI_ERR_NO){
			OTA_SPI_ALOGE("slave send version info fail!");
			while(1);
			//TODO: retry or abandon upgrades?
		}
		count_receive_update_package++;

#ifdef OTA_SPI_SUPPORT_UI_API
		//display info2 ---dispaly progress bar in lcd																					   
		if(count_receive_update_package % resolution_convert_num_files == 0 || is_receive_update_pkg_end){
			ota_spi_ratio += OTA_SPI_RESOLUTION_RATIO;
			ota_spi_progress_ratio(ota_spi_ratio);
		}
#endif
	}
	
#if 0
debug_4:
	//ota_spi_update_file_size = 0x4d9000;
	ota_spi_update_file_size = 4964;
	*((UINT16 *)ota_spi_crc_res_receive) = 0xeb0a;
	
	ota_spi_res = ota_spi_crc_check_Image_in_Flash(OTA_SPI_SYSTEM_PART_OFFSET, ota_spi_update_file_size * OTA_SPI_MAX_BUFFER_LEN,
		*((UINT16 *)ota_spi_crc_res_receive));
	if(ota_spi_res != OTA_SPI_ERR_NO){
		OTA_SPI_ALOGE("crc check image has been written in flash fail!");
		while(1);
		//TODO: retry or abandon upgrades?
	}

	//debug
	OTA_SPI_ALOGI("completed");
	while(1);		
#endif

#ifdef OTA_SPI_DEBUG_FUNC
	ota_spi_d_print_flash_image_to_sd(OTA_SPI_SYSTEM_PART_OFFSET, ota_spi_update_file_size);
#endif
	/* [4] CP crc check image has been written in flash */
	uart_printf("\r\n===============%s===============\r\n","[4] CP crc check image has been written in flash");
	ota_spi_res = ota_spi_slave_receive_send_pkg(OTA_SPI_TYPE_CRC_CHECK_AFTER_UPDATING);
	if(ota_spi_res != OTA_SPI_ERR_NO){
		OTA_SPI_ALOGE("CP crc check image has been written in flash fail!");
		while(1);
		//TODO: retry or abandon upgrades?
	}
#ifdef OTA_SPI_DEBUG_UNIT_TEST
debug_5:
#endif
	uart_printf("\r\n===============%s===============\r\n","[4_temp] decompress compressed update package and write into external flash");
	update_package_type = GetUpdatePackageType(OTA_SPI_SYSTEM_PART_OFFSET + 0x80000000);
	if(update_package_type == Lzop_Compressed || update_package_type == Lzma_Compressed){
		if(update_package_type == Lzma_Compressed){
			/* If compressed type is LZMA, need config cache and improve cp core and psram frequency. */
			UINT32 poolLen;
			poolLen = COMPRESSED_ALGORITHM_BUFFER_SIZE;
			malloc_init(g_compressed_buffer, poolLen);
			OTA_SPI_ALOGI("malloc init done!");

			enable_cache(OTA_SPI_CACHE_START,OTA_SPI_CACHE_SIZE);	//16M
			CpCoreFreqChangeTo624();
			PsramPhyFreqChangeTo416();
		}
		
		/* [5.1] decompress compressed system.img from FLASH into PSRAM */
		UncompressedSystemSize = UncompressedUpdatePackage(update_package_type,OTA_SPI_SYSTEM_PART_OFFSET + 0x80000000,UPDATE_PACKAGE_UNCOMPRESS_ADDR);
		OTA_SPI_ALOGI("UncompressedSystemSize:0x%.08x,max length:0x%.08x",UncompressedSystemSize,UPDATE_PACKAGE_UNCOMPRESS_MAX_LENGTH);
		if(UncompressedSystemSize > UPDATE_PACKAGE_UNCOMPRESS_MAX_LENGTH){
			OTA_SPI_ALOGE("UncompressedSystemSize is more than psram max length");
			while(1);
		}

		/* [5.2] copy uncompressed  system.img from PSRAM into FLASH */
		if(0 != WriteFileFromPsramToFlash(OTA_SPI_SYSTEM_PART_OFFSET,UPDATE_PACKAGE_UNCOMPRESS_ADDR,UncompressedSystemSize)){
			OTA_SPI_ALOGI("copy system.img error");
			while(1);
		}

		OTA_SPI_ALOGI("crc check uncompressed image has been written in flash");
		ota_spi_res = ota_spi_crc_check_Image_in_Flash(OTA_SPI_SYSTEM_PART_OFFSET, UncompressedSystemSize,
			(*(UINT16 *)(ota_spi_crc_res_receive + sizeof(UINT16))));
		if(ota_spi_res != OTA_SPI_ERR_NO){
			OTA_SPI_ALOGE("crc check image has been written in flash fail!");
			while(1);
			//TODO: retry or abandon upgrades?
		}

		if(update_package_type == Lzma_Compressed){
			/* Recover the cache config and cp core frequency */
			disable_cache(OTA_SPI_CACHE_START,OTA_SPI_CACHE_SIZE);//16M
			CpCoreFreqChangeTo416();
		}
	}

	/* [5] reset ota_spi_flag */
	uart_printf("\r\n===============%s===============\r\n","[5] reset ota_spi_flag");
	ota_spi_res = ota_spi_slave_receive_send_pkg(OTA_SPI_TYPE_RESET_CP_FLAG);
	if(ota_spi_res != OTA_SPI_ERR_NO){
		OTA_SPI_ALOGE("reset ota_spi_flag fail!");
		while(1);
		//TODO: retry or abandon upgrades?
	}
	
	/* [6] prepare work before reboot cp */
	Timer0_Switch(0);
	
	uart_printf("OTA_SPI UPDATE SLAVE SUCCESS. WAITIN TO REBOOT...\r\n");
	while(1);
}

AMUX_OUTPKT_T* ota_spi_make_amux_packet(const OTA_SPI_PACKAGE_TYPE type, const UINT32 index)
{
	AMUX_OUTPKT_T *p_amux_outpkt = &cp_amux_outpkt;
	UINT32 outpkt_buf_len;
	UINT8 *p_outpkt_buf;
	char version[OTA_SPI_VERSION_PACKET_BUFFER_LEN];
	OTA_SPI_ERR_T ota_spi_res;
	UINT32 fota_param_offset;
	
#ifdef OTA_SPI_UPDATING_LOG
	uart_printf("func:%s,line:%d\r\n",__func__,__LINE__);
#else
	if(type != OTA_SPI_TYPE_RECEIVE_UPDATE_PKG)
		uart_printf("func:%s,line:%d\r\n",__func__,__LINE__);
#endif	
	
	/* [1] init variable */
	p_outpkt_buf = OTA_SPI_Cp2Apbuf;
	memset(p_outpkt_buf, 0, sizeof(OTA_SPI_Cp2Apbuf));
	
	memset(p_amux_outpkt, 0, sizeof(cp_amux_outpkt));

	//Get fota_param_offset
	fota_param_offset = get_fota_param_offset();
	if(type != OTA_SPI_TYPE_RECEIVE_UPDATE_PKG)
		OTA_SPI_ALOGD("fota_param_offset:0x%x",fota_param_offset);

	/* [2] fill "p_out_pkt", including:1. [type], 2. [len], 3. [idx], 4. [buf] and 5.  [fcs] */
	//buf_content and buf_len 
	switch(type){
		case OTA_SPI_TYPE_GET_HEADER_CRC:		
			*(UINT32 *)p_outpkt_buf = SLAVE_RECEIVE_UPDATE_CRC_CONF;
			outpkt_buf_len = sizeof(SLAVE_RECEIVE_UPDATE_CRC_CONF);
		break;

		case OTA_SPI_TYPE_RECEIVE_FILE_SIZE_OF_UPDATE_PKG:	
			*(UINT32 *)p_outpkt_buf = SLAVE_RECEIVE_FILE_SIZE_OF_UPDATE_PKG_CONF;
			outpkt_buf_len = sizeof(SLAVE_RECEIVE_FILE_SIZE_OF_UPDATE_PKG_CONF);
		break;

		case OTA_SPI_TYPE_RECEIVE_UPDATE_PKG:	
			*(UINT32 *)p_outpkt_buf = SLAVE_RECEIVE_UPDATE_PKG_CONF;
			outpkt_buf_len = sizeof(SLAVE_RECEIVE_UPDATE_PKG_CONF);
		break;

		case OTA_SPI_TYPE_CRC_CHECK_AFTER_UPDATING:	
			*(UINT32 *)p_outpkt_buf = SLAVE_CRC_CHECK_AFTER_UPDATING_CORRECT;
			outpkt_buf_len = sizeof(SLAVE_CRC_CHECK_AFTER_UPDATING_CORRECT);
		break;

		case OTA_SPI_TYPE_RESET_CP_FLAG:	
			*(UINT32 *)p_outpkt_buf = SLAVE_RESET_SLAVE_OTA_CONF;
			outpkt_buf_len = sizeof(SLAVE_RESET_SLAVE_OTA_CONF);
		break;

		case OTA_SPI_TYPE_UPDATER_HANDSHAKE:	
			*(UINT32 *)p_outpkt_buf = SLAVE_UPDATER_ACK_FLAG;
			outpkt_buf_len = sizeof(SLAVE_UPDATER_ACK_FLAG);
		break;

		default:
			OTA_SPI_ALOGE("input para:type err");
			ASSERT(0);
			//return OTA_SPI_ERR_PARAMETER;
	}

	//buf_idx and copy buf content 	
	p_amux_outpkt->type = type;
	p_amux_outpkt->len = outpkt_buf_len;
	p_amux_outpkt->idx = index;
	if(outpkt_buf_len != 0){
		memcpy(p_amux_outpkt->buf, p_outpkt_buf, outpkt_buf_len);
		memset(p_amux_outpkt->buf + outpkt_buf_len, 0, OTA_SPI_MAX_BUFFER_LEN - outpkt_buf_len);					
	}else{
		memset(p_amux_outpkt->buf, 0, OTA_SPI_MAX_BUFFER_LEN);	
	}
	p_amux_outpkt->fcs = ota_spi_calc_fcs(p_amux_outpkt);

	/* [3] fill amux header info and tail info */
	p_amux_outpkt->start_flag = MUX_FLAG;
	p_amux_outpkt->address = MUX_ADDRESS;
	p_amux_outpkt->control = MUX_CONTROL;
	p_amux_outpkt->length_indication = MUX_LENGTH_INDICATION;
	p_amux_outpkt->end_flag = MUX_FLAG;
	
	return p_amux_outpkt;
}

OTA_SPI_ERR_T ota_spi_amux_pkt_read(AMUX_INPKT_T *p_cp_amux_inpkt,const int pkt_len)
{
	INT32 ee_io_res;
	OTA_SPI_ERR_T ota_spi_res;
	UINT16 trytimes = 0;
	const UINT16  RETRY = OTA_SPI_RETRY;
	UINT8 *p_cp_amux_inpkt_buf;
	unsigned long t1;
	unsigned long t2;

	/* check input parameter */
	if(p_cp_amux_inpkt == NULL){
		OTA_SPI_ALOGE("input para err!");
		ASSERT(0);
	}

	//init param
	p_cp_amux_inpkt_buf = &cp_amux_inpkt_buf;

	while(1){
		/* [1] read the amux package data */
		t1 = GetTimer0CNT();
		while(1){
			memset(p_cp_amux_inpkt_buf,0,AMUX_HEADER_LENGTH);			
			ee_io_res = eeh_io_read(p_cp_amux_inpkt_buf);
			// 1.1 check AMUX HEADER and AMUX TAIL info
			if( *(UINT8 *)(p_cp_amux_inpkt_buf) == MUX_FLAG && 
				*(UINT8 *)(p_cp_amux_inpkt_buf+1) == MUX_ADDRESS &&
				*(UINT8 *)(p_cp_amux_inpkt_buf+2) == MUX_CONTROL &&
				*(UINT16 *)(p_cp_amux_inpkt_buf+3) == MUX_LENGTH_INDICATION &&
				*(UINT8 *)(p_cp_amux_inpkt_buf + pkt_len -1) == MUX_FLAG )
			{
				//OTA_SPI_ALOGD("t1:%d,t2:%d",t1,t1);
//				ota_spi_dump_input_packet((UINT8 *)(p_cp_amux_inpkt_buf),pkt_len);
				break;
			}

			//2. discard incorrect packets, and print debug info every 0.5s
			t2 = GetTimer0CNT();
			if( t2 - t1 > OTA_SPI_TIMER_DELAY){		
				OTA_SPI_ALOGW("read packet err!");
//				ota_spi_dump_input_packet((UINT8 *)(p_cp_amux_inpkt_buf),pkt_len);
				t1 = GetTimer0CNT();
			}
		}

#ifdef OTA_SPI_DEBUG_FUNC
		//dump amux package
		ota_spi_print_amux_buf(p_cp_amux_inpkt_buf,pkt_len);
#endif
		/* [2] parse amux read data */
		memset(p_cp_amux_inpkt,0,sizeof(p_cp_amux_inpkt));
		ota_spi_parse_amux_read_buf(p_cp_amux_inpkt_buf,p_cp_amux_inpkt);

		/* [3] check the amux package data */
		ota_spi_res = ota_spi_check_package(p_cp_amux_inpkt);
		if(ota_spi_res != OTA_SPI_ERR_NO){
			OTA_SPI_ALOGE("check package err,ota_spi_res: 0x%x",ota_spi_res);
			// try again
			trytimes++;
			if(trytimes > RETRY){
				OTA_SPI_ALOGE("try %d times, check package data fail!",RETRY);
				return OTA_SPI_ERR_READ_FAIL;
			}
			continue;	
		}
#ifdef OTA_SPI_UPDATING_LOG
		OTA_SPI_ALOGI("OTA_SPI package read success");
#else
		if(p_cp_amux_inpkt->type != OTA_SPI_TYPE_RECEIVE_UPDATE_PKG)
			OTA_SPI_ALOGI("OTA_SPI package read success");
#endif		
		break;
	}

	return OTA_SPI_ERR_NO;
}


OTA_SPI_ERR_T ota_spi_mux_pkt_write(AMUX_OUTPKT_T* p_amux_outpkt,const int pkt_len)
{
	INT32 amux_res;
	UINT16 trytimes = 0;
	const UINT16  RETRY = OTA_SPI_RETRY;
	UINT8 *p_amux_outokt_buf;
	OTA_SPI_ERR_T ota_spi_res;
	
	/* check input parameter */
	if(p_amux_outpkt == NULL){
		OTA_SPI_ALOGE("input para:p_amux_outpkt err");
		ASSERT(0);
		//return OTA_SPI_ERR_PARAMETER;
	}
	if(pkt_len > AMUX_LEN){
		OTA_SPI_ALOGE("input para:pkt_len err");
		ASSERT(0);
		//TODO:
	}

	/* init paramter */
	p_amux_outokt_buf = cp_amux_outpkt_buf;
	
	
	/* [1] write data */
	while(1){
		//1. amux_pkt switch to amux_buf
		ota_spi_res = fill_amux_write_buf(p_amux_outpkt,p_amux_outokt_buf);
		if(ota_spi_res != OTA_SPI_ERR_NO){
			OTA_SPI_ALOGE("amux_pkt switch to amux_buf err!");
			while(1);
		}
#ifdef OTA_SPI_DEBUG_FUNC
		//[debug] print buf before sending
		OTA_SPI_ALOGI("before sending, print the buffer content!");
		ota_spi_print_amux_buf((UINT8 *)p_amux_outokt_buf, pkt_len);
#endif		
		//2. write amux_buf
		amux_res = eeh_io_write((UINT8 *)p_amux_outokt_buf,pkt_len);
		if(amux_res < 0) {
			 OTA_SPI_ALOGE("pkt write err,amux_res:%d",amux_res);
			 // try again
			 trytimes++;
			 if(trytimes > RETRY){
				 OTA_SPI_ALOGE("try %d times, write data fail!",RETRY);
				 return OTA_SPI_ERR_READ_FAIL;
			 }
			 continue;
		}
#ifdef OTA_SPI_UPDATING_LOG
		OTA_SPI_ALOGI("amux write data success!");
#else
		if(p_amux_outpkt->type != OTA_SPI_TYPE_RECEIVE_UPDATE_PKG)
			OTA_SPI_ALOGI("amux write data success!");
#endif		
		break;
	}

	return OTA_SPI_ERR_NO;
}


OTA_SPI_ERR_T ota_spi_check_package(AMUX_INPKT_T *p_ota_spi_inpkt)
{
	UINT32 fcs_cal;

#ifdef OTA_SPI_UPDATING_LOG
	uart_printf("func:%s,line:%d\r\n",__func__,__LINE__);
#else
	if(p_ota_spi_inpkt->type != OTA_SPI_TYPE_RECEIVE_UPDATE_PKG)
		uart_printf("func:%s,line:%d\r\n",__func__,__LINE__);
#endif

	if(p_ota_spi_inpkt == NULL){
		OTA_SPI_ALOGE("input para err");
		return OTA_SPI_ERR_PARAMETER;
	}

	/* [1] check type , buffer len and buf content(flag) */
	switch(p_ota_spi_inpkt->type){
		case OTA_SPI_TYPE_UPDATER_HANDSHAKE:	
			if(*(UINT32 *)p_ota_spi_inpkt->buf != MASTER_HANDSHAKE_WITH_CP_UPDATER){
				OTA_SPI_ALOGE("OTA_SPI package read Fail, receive packet content:0x%x", *(UINT32 *)p_ota_spi_inpkt->buf);
				return OTA_SPI_ERR_READ_FAIL;
			}
			break;	
		case OTA_SPI_TYPE_GET_HEADER_CRC:	
			if(p_ota_spi_inpkt->len != OTA_SPI_CRC_LENGTH){
				OTA_SPI_ALOGE("OTA_SPI package read Fail, crc len:0x%x", p_ota_spi_inpkt->len);
				return OTA_SPI_ERR_READ_FAIL;
			}
			break;
		case OTA_SPI_TYPE_RECEIVE_FILE_SIZE_OF_UPDATE_PKG:	
			if(p_ota_spi_inpkt->len != sizeof(ota_spi_update_file_size)){
				OTA_SPI_ALOGE("OTA_SPI package read Fail, receive packet len:0x%x", p_ota_spi_inpkt->len);
				return OTA_SPI_ERR_READ_FAIL;
			}
			break;
		case OTA_SPI_TYPE_RECEIVE_UPDATE_PKG:	
			if(p_ota_spi_inpkt->len > OTA_SPI_MAX_BUFFER_LEN){
				OTA_SPI_ALOGE("OTA_SPI package read Fail, receive packet len:0x%x", p_ota_spi_inpkt->len);
				return OTA_SPI_ERR_READ_FAIL;
			}
			break;
		case OTA_SPI_TYPE_CRC_CHECK_AFTER_UPDATING:	
			if(*(UINT32 *)p_ota_spi_inpkt->buf != MASTER_CRC_CHECK_AFTER_UPDATING_COMMAND){
				OTA_SPI_ALOGE("OTA_SPI package read Fail, receive packet content:0x%x", *(UINT32 *)p_ota_spi_inpkt->buf);
				return OTA_SPI_ERR_READ_FAIL;
			}
			break;
		case OTA_SPI_TYPE_RESET_CP_FLAG:	
			if(*(UINT32 *)p_ota_spi_inpkt->buf != MASTER_RESET_SLAVE_OTA_FLAG){
				OTA_SPI_ALOGE("OTA_SPI package read Fail, receive packet content:0x%x", *(UINT32 *)p_ota_spi_inpkt->buf);
				return OTA_SPI_ERR_READ_FAIL;
			}
			break;			
		default:
			OTA_SPI_ALOGE("package type err,type:0x%x",p_ota_spi_inpkt->type);
			return OTA_SPI_ERR_READ_FAIL;
	}

	/* [2] check buffer data: by "checksum" */
    fcs_cal = ota_spi_calc_fcs(p_ota_spi_inpkt);
    if( fcs_cal != p_ota_spi_inpkt->fcs){
	    //TODO: if packet error, master send message to slave to acquire re_send/
	    OTA_SPI_ALOGE("fcs does not match, fcs caculated:%d, fcs of data :%d", fcs_cal,p_ota_spi_inpkt->fcs);
		return OTA_SPI_ERR_FCS;
    }

	/* [3] check index */
	if(p_ota_spi_inpkt->idx != current_cp_packet_index){
		OTA_SPI_ALOGE("index does not match, current_index:%d, index of data :%d", 
									   current_cp_packet_index,p_ota_spi_inpkt->idx);
		//TODO: if packet error, master send message to slave to acquire re_send/
		return OTA_SPI_ERR_INDEX;
	}
	current_cp_packet_index++;
	
#ifdef OTA_SPI_UPDATING_LOG
	OTA_SPI_ALOGI("receive package correct!");
#else
	if(p_ota_spi_inpkt->type != OTA_SPI_TYPE_RECEIVE_UPDATE_PKG)
			OTA_SPI_ALOGI("receive package correct!");
#endif

	return OTA_SPI_ERR_NO;
}


UINT32 ota_spi_calc_fcs(AMUX_OUTPKT_T* p_amux_outpkt)
{
	 UINT32 checksum = 0;
	 UINT32 len, i;
	 
	 len = p_amux_outpkt->len;
 
	 for(i=0; i<len; i++){
		 checksum += p_amux_outpkt->buf[i];
	 }
	 p_amux_outpkt->fcs = checksum;					 
 
	 return checksum;
}


OTA_SPI_ERR_T ota_spi_slave_receive_send_pkg(const OTA_SPI_PACKAGE_TYPE  pkt_type)
{
	AMUX_INPKT_T *p_cp_amux_inpkt;
	AMUX_OUTPKT_T *p_amux_outpkt;	 
	OTA_SPI_ERR_T ota_spi_res;
	
	/* check input parameter */


	/* init parameter */
	 
	p_cp_amux_inpkt = &cp_amux_inpkt;
	 
	/* [1] read pkt */
	ota_spi_res = ota_spi_amux_pkt_read(p_cp_amux_inpkt,AMUX_LEN);
	if(ota_spi_res != OTA_SPI_ERR_NO){
		OTA_SPI_ALOGE("read package err,ota_spi_res:0x%x", ota_spi_res);
		return OTA_SPI_ERR_READ_FAIL;
	}
#ifdef OTA_SPI_UPDATING_LOG
	uart_printf("------(1) slave has received packet from master,pkt type: %d------\r\n",(OTA_SPI_PACKAGE_TYPE)p_cp_amux_inpkt->type);
#else
	if((OTA_SPI_PACKAGE_TYPE)p_cp_amux_inpkt->type != OTA_SPI_TYPE_RECEIVE_UPDATE_PKG) 	/* Note: do not print this log when slave receive update package */
		uart_printf("------(1) slave has received packet from master,pkt type: %d------\r\n",(OTA_SPI_PACKAGE_TYPE)p_cp_amux_inpkt->type);
#endif

	/* [2] slave ececute different operations according to dirrerent inpkt type  */

#ifdef OTA_SPI_UPDATING_LOG
	uart_printf("------[2] slave ececute different operations according to dirrerent inpkt type------\r\n");
#else
	if((OTA_SPI_PACKAGE_TYPE)p_cp_amux_inpkt->type != OTA_SPI_TYPE_RECEIVE_UPDATE_PKG)	/* Note: do not print this log when slave receive update package */
		uart_printf("------[2] slave ececute different operations according to dirrerent inpkt type------\r\n");
#endif
	ota_spi_slave_inpkt_handler(p_cp_amux_inpkt,&ota_spi_res);

	/* [3] make out packet */
#ifdef OTA_SPI_UPDATING_LOG
	uart_printf("------(3) make out packet------\r\n");
#else
	if((OTA_SPI_PACKAGE_TYPE)p_cp_amux_inpkt->type != OTA_SPI_TYPE_RECEIVE_UPDATE_PKG) 	/* Note: do not print this log when slave receive update package */
		uart_printf("------(3) make out packet------\r\n");
#endif	
	p_amux_outpkt = ota_spi_make_amux_packet((OTA_SPI_PACKAGE_TYPE)p_cp_amux_inpkt->type, current_cp_packet_index++);

	/* [3] write pkt */
#ifdef OTA_SPI_UPDATING_LOG
	uart_printf("------(4) write pkt------\r\n");
#else
	if((OTA_SPI_PACKAGE_TYPE)p_cp_amux_inpkt->type != OTA_SPI_TYPE_RECEIVE_UPDATE_PKG) 	/* Note: do not print this log when slave receive update package */
		uart_printf("------(4) write pkt------\r\n");
#endif
	
	ota_spi_res = ota_spi_mux_pkt_write(p_amux_outpkt,AMUX_LEN);
	if(ota_spi_res != OTA_SPI_ERR_NO){
		OTA_SPI_ALOGE("pkt write err,ota_spi_res:0x%x",ota_spi_res);
		return OTA_SPI_ERR_WRITE_FAIL;
	} 
}

Update_Package_T GetUpdatePackageType(const UINT32 SystemLoadAddress)
{
	UINT32 type;
	Update_Package_T ret;

	memcpy((UINT8 *)&type,(UINT8 *)SystemLoadAddress,sizeof(type));
	OTA_SPI_ALOGI("type:0x%.08x",type);
	
	switch (type)
	{
		case LZOP_COMPRESSED_MAGIC:
			ret =  Lzop_Compressed;
			break;

		case LZMA_COMPRESSED_MAGIC:
			ret =  Lzma_Compressed;
			break;
		
		default:
			ret = Non_Compressed;
	}
	return ret;
}

UINT32 UncompressedUpdatePackage(UINT32 type,UINT32 src_addr,UINT32 dst_addr)
{
	UINT32 inLen = ota_spi_update_file_size;	/* Note: if is lzop, inLen can be arbitray value; if is lzma, inLen must be real input length */
	INT32 ret = -1;
	UINT32 outlen;
	
	switch(type)
	{
		case Lzop_Compressed:
			ret = LzopUncompress(src_addr,dst_addr,&inLen,&outlen);
			break;
		
		case Lzma_Compressed:
			OTA_SPI_ALOGI("decompress from [0x%.08x] to [0x%0.8x]", src_addr ,dst_addr);
			ret = LzmaUncompress((unsigned char *)dst_addr,(size_t *)&outlen,(const unsigned char *)src_addr,(size_t *)&inLen);
			break;
		
		default:
			OTA_SPI_ALOGE("type:%d; It is not compressed system img!",type);
			while(1);
	}
	if(0 != ret){
		OTA_SPI_ALOGE(" decompress system.img err!\r\n");
		while(1);
	}

	return outlen;
}

INT32 LzopUncompress(   const UINT32 flash_address,const UINT32 exec_address,UINT32 *p_inLen,UINT32 *p_outLen)
{
    INT32 ret;
	
    OTA_SPI_ALOGI("decompress from [0x%.08x] to [0x%0.8x]", flash_address ,exec_address);
    if(!lzop_decompress_safe((unsigned char *)flash_address,(unsigned char *)exec_address,p_outLen,p_inLen)){
        OTA_SPI_ALOGD("p_inLen=[0x%.08x], outLen=[0x%.08x]",*p_inLen,*p_outLen);
        ret = 0;
    }else{
        OTA_SPI_ALOGE("\r\n ** ERROR: LOGO image decompress failed.\r\n");
		ret = -1;
    }
	
    return ret;
}

INT32 WriteFileFromPsramToFlash(UINT32 dst_offset_addr,UINT32 src_addr,const UINT32 src_len)
{
	INT32 ret;
	UINT32 WriteSize;
	OTA_SPI_ERR_T ota_spi_res;

	OTA_SPI_ALOGI("Func:%s,line:%d",__func__,__LINE__);

	//erase flash
	ota_spi_res = ota_spi_erase_all(dst_offset_addr,src_len, WRITE_UNIT_64K);
	if(ota_spi_res != OTA_SPI_ERR_NO){
		OTA_SPI_ALOGE("erase flash err!");
		while(1);
	}
		
	//copy file from sd card to psram
	OTA_SPI_ALOGD("Start wirte");
		
	//write data from psram to flash
	WriteSize = src_len - src_len%WRITE_UNIT_64K;
	OTA_SPI_ALOGD("WriteSize:0x%.08x",WriteSize);
	ret = chip->ops->write( chip, dst_offset_addr, WriteSize, (UINT8 *)src_addr);
	if(0 != ret){
		OTA_SPI_ALOGE("spi_nor_wirte err:[ret=%d]",ret);
		while(1);
	}
	
	//Determine the written file is an integer multiple of "block"(64K) or not
	if(src_len % WRITE_UNIT_64K != 0) {
		src_addr += WriteSize;
		dst_offset_addr += WriteSize;
		OTA_SPI_ALOGD("src_addr:0x%.08x",src_addr);
		OTA_SPI_ALOGD("dst_offset_addr:0x%.08x",dst_offset_addr);
		//read the end update package data into buffer
		memcpy(ota_spi_backup_flash_data_buf,src_addr,src_len%WRITE_UNIT_64K);
		
		//write data from psram to flash
		ret = chip->ops->write( chip, dst_offset_addr, WRITE_UNIT_64K, (UINT8 *)ota_spi_backup_flash_data_buf);
		if(0!= ret){	//flash write FAIL				
			OTA_SPI_ALOGE("flash write err:%d", ret);
			while(1);
		}
	}
	OTA_SPI_ALOGI("Write end!");
	
	return 0;
}






#ifdef OTA_SPI_DEBUG_FUNC
OTA_SPI_ERR_T ota_spi_slave_send_pkg(const OTA_SPI_PACKAGE_TYPE  pkt_type)
{
	AMUX_OUTPKT_T *p_amux_outpkt;	 
	OTA_SPI_ERR_T ota_spi_res;

	/* [1] make out packet */
	uart_printf("------(1) make out packet------\r\n");
	p_amux_outpkt = ota_spi_make_amux_packet(pkt_type, current_cp_packet_index++);

	/* [2] write pkt */
	uart_printf("------(2) write pkt------\r\n");
	ota_spi_res = ota_spi_mux_pkt_write(p_amux_outpkt,AMUX_LEN);
	if(ota_spi_res != OTA_SPI_ERR_NO){
		OTA_SPI_ALOGE("pkt write err,ota_spi_res:0x%x",ota_spi_res);
		return OTA_SPI_ERR_WRITE_FAIL;
	} 
}
#endif

void ota_spi_delay(int ms)
{
	volatile int i = 0;
	int j;
	for(j = 0; j < ms; j++) {
		for(i =0; i< 50000;i++){
		}
	}
}

OTA_SPI_ERR_T ota_spi_parse_amux_read_buf(UINT8 *p_amux_buf,AMUX_INPKT_T *p_in_pkt)
{
	/* check input parameter */
	if(p_in_pkt == NULL){
		OTA_SPI_ALOGE("p_in_pkt error");
		ASSERT(0);
		//return OTA_SPI_ERR_PARAMETER;
	}
	if(p_amux_buf == NULL){
		OTA_SPI_ALOGE("p_in_pkt error");
		ASSERT(0);
		return OTA_SPI_ERR_PARAMETER;
	}

#ifdef OTA_SPI_UPDATING_LOG	/* Note: do not print this log when slave receive update package */
	uart_printf("func:%s,line:%d\r\n",__func__,__LINE__);
#endif

	/* [1] parse p_amux_buf */
	p_in_pkt->start_flag = *(UINT8 *)p_amux_buf;
	p_in_pkt->address = *(UINT8 *)(p_amux_buf+1);
	p_in_pkt->control = *(UINT8 *)(p_amux_buf+2);
	p_in_pkt->length_indication = *(UINT16 *)(p_amux_buf+3);

	p_amux_buf += AMUX_HEADER_LENGTH;

	//OTA_SPI_ALOGD("amux_header parse completed");
	
	p_in_pkt->type = *((UINT32 *)p_amux_buf); 
	p_amux_buf += sizeof(p_in_pkt->type);
	
	p_in_pkt->len  = *((UINT32 *)p_amux_buf); 
	p_amux_buf += sizeof(p_in_pkt->len);
	if(p_in_pkt->len > OTA_SPI_MAX_BUFFER_LEN){
		//check packet buffer len.
		OTA_SPI_ALOGE("pkt buffer len out of range,p_in_pkt->len:0x%x",p_in_pkt->len);
		return OTA_SPI_ERR_READ_FAIL;
	}
	//p_in_pkt->idx = *p_amux_buf + (* (p_amux_buf + 1))<<8 + (* (p_amux_buf + 2))<<16 + (* (p_amux_buf + 3))<<24;
	memcpy(&p_in_pkt->idx,p_amux_buf,sizeof(p_in_pkt->idx));
	//p_in_pkt->idx = *((UINT32 *)p_amux_buf);	
	//uart_printf("2,idx:0x%x\r\n",p_in_pkt->idx);
	
	p_amux_buf += sizeof(p_in_pkt->idx);
	
	memcpy(p_in_pkt->buf,p_amux_buf,p_in_pkt->len);
	p_amux_buf += sizeof(p_in_pkt->buf) + sizeof(p_in_pkt->reserved);

	memcpy(&p_in_pkt->fcs,p_amux_buf,sizeof(p_in_pkt->fcs));
	p_amux_buf += sizeof(p_in_pkt->fcs) + sizeof(p_in_pkt->amux_fcs);

	memcpy(&p_in_pkt->end_flag,p_amux_buf,sizeof(p_in_pkt->end_flag));

#ifdef OTA_SPI_UPDATING_LOG
	OTA_SPI_ALOGI("parse read buf success!");
#else
	if(p_in_pkt->type != OTA_SPI_TYPE_RECEIVE_UPDATE_PKG)
		OTA_SPI_ALOGI("parse read buf success!");
#endif	
	return OTA_SPI_ERR_NO;
}

OTA_SPI_ERR_T fill_amux_write_buf(AMUX_INPKT_T *p_out_amux_pkt,UINT8 *p_amux_out_buf)
{
	/* check input parameter */
	if(p_out_amux_pkt == NULL){
		OTA_SPI_ALOGE("p_out_amux_pkt error");
		ASSERT(0);
		//return OTA_SPI_ERR_PARAMETER;
	}
	if(p_amux_out_buf == NULL){
		OTA_SPI_ALOGE("p_out_amux_pkt error");
		ASSERT(0);
		return OTA_SPI_ERR_PARAMETER;
	}

	/* [1] fill p_amux_out_buf */
	*(UINT8 *)p_amux_out_buf = p_out_amux_pkt->start_flag;
	*(UINT8 *)(p_amux_out_buf+1) = p_out_amux_pkt->address;
	*(UINT8 *)(p_amux_out_buf+2) = p_out_amux_pkt->control;
	*(UINT16 *)(p_amux_out_buf+3) = p_out_amux_pkt->length_indication;

	p_amux_out_buf += AMUX_HEADER_LENGTH;
	
	*((UINT32 *)p_amux_out_buf) = p_out_amux_pkt->type; 
	p_amux_out_buf += sizeof(p_out_amux_pkt->type);
	
	*((UINT32 *)p_amux_out_buf) = p_out_amux_pkt->len; 
	p_amux_out_buf += sizeof(p_out_amux_pkt->len);

	
	*((UINT32 *)p_amux_out_buf) = p_out_amux_pkt->idx; 
	p_amux_out_buf += sizeof(p_out_amux_pkt->idx);

	memcpy(p_amux_out_buf,p_out_amux_pkt->buf,p_out_amux_pkt->len);
	p_amux_out_buf += sizeof(p_out_amux_pkt->buf) + sizeof(p_out_amux_pkt->reserved);
	
	memcpy((UINT32 *)p_amux_out_buf,&p_out_amux_pkt->fcs,sizeof(p_out_amux_pkt->fcs));
	p_amux_out_buf += sizeof(p_out_amux_pkt->fcs) + sizeof(p_out_amux_pkt->amux_fcs);

	memcpy(p_amux_out_buf,&p_out_amux_pkt->end_flag,sizeof(p_out_amux_pkt->end_flag));

#ifdef OTA_SPI_UPDATING_LOG
	OTA_SPI_ALOGI("fill write buf success!");
#else
	if( p_out_amux_pkt->type != OTA_SPI_TYPE_RECEIVE_UPDATE_PKG )
		OTA_SPI_ALOGI("fill write buf success!");
#endif
	return OTA_SPI_ERR_NO;
}

OTA_SPI_ERR_T ota_spi_write_flash(const UINT32 offset,const UINT32 write_size_unit,UINT8 *p_buffer,BOOL have_erased)
{
	INT32 ret;
	
#ifdef OTA_SPI_UPDATING_LOG
	uart_printf("func:%s,line:%d\r\n",__func__,__LINE__);
#endif
		
	//write data from psram to flash
	if(have_erased){
		ret = chip->ops->write( chip, offset, write_size_unit, p_buffer);
		if(OTA_SPI_ERR_NO != ret){
			OTA_SPI_ALOGE("spi_nor_wirte err:[ret=%d]",ret);
			return OTA_SPI_ERR_spi_nor_write;
		}
	}else{
		UINT32 save_data_offset;
		UINT32 save_data_size;
		
		save_data_size = write_size_unit-ota_spi_update_file_size%write_size_unit;
		save_data_offset = ota_spi_update_file_size%write_size_unit;
		OTA_SPI_ALOGI("erase end flash paras:update_file_size:[0x%x],save_data_size:[0x%x],save_data_offset:[0x%x]",
			ota_spi_update_file_size,save_data_size,save_data_offset);
		
		//save the orignal data of flash
		ret = chip->ops->read(chip, offset+save_data_offset, save_data_size,p_buffer+save_data_offset);
		if(OTA_SPI_ERR_NO != ret){
			OTA_SPI_ALOGE("spi_nor_read err:[ret=%d]",ret);
			return OTA_SPI_ERR_spi_nor_read;
		}
		
		//erase the writing flash
		ret = chip->ops->erase(chip,offset,write_size_unit);
		if(OTA_SPI_ERR_NO != ret){
			OTA_SPI_ALOGE("spi_nor_erase err:[ret=%d]",ret);
			return OTA_SPI_ERR_spi_nor_erase;
		}
	
		//write data from psram to flash
		ret = chip->ops->write( chip, offset, write_size_unit, p_buffer);
		if(OTA_SPI_ERR_NO!= ret){	//f_read FAIL				
			OTA_SPI_ALOGE("f_read err:%d", ret);
			return OTA_SPI_ERR_spi_nor_write;
		}

	}
	
	return OTA_SPI_ERR_NO;
}

void dump_flash_content(char *p_buffer)
{
	INT32 i;

	for(i=0;i<1024;i++)
	{
		uart_printf("i:%x",*(p_buffer+i));
	}
}


static OTA_SPI_ERR_T ota_spi_reset_ota_spi_flag(const UINT32 offset)
{
	INT32 ret;
	UINT8 data_source[OTA_SPI_SPACE_FLAG_FEED_INFO];
	INT8 ota_spi_flag[OTA_SPI_FLAG_LENGTH];
	int index;

	OTA_SPI_ALOGI("Func:%s,line:%d",__func__,__LINE__);
	
	//before resetting OTA_Flag, read the source OTA_Flag
	ret = chip->ops->read( chip, offset, OTA_SPI_FLAG_LENGTH*sizeof(UINT8), (uint8_t *)ota_spi_flag);
	if(OTA_SPI_ERR_NO != ret){
		OTA_SPI_ALOGE("spi_nor_read err:[ret=%d]",ret);
		return OTA_SPI_ERR_spi_nor_write;
	}
	OTA_SPI_ALOGD("Before resetting,ota_spi_flag=[%s]",ota_spi_flag);

	//before erase, need to save source data because size of OTA_Flag_SDCARD is only 9 Bytes
	ret = chip->ops->read(chip, offset, OTA_SPI_SPACE_FLAG_FEED_INFO,(uint8_t *)data_source);
	if(OTA_SPI_ERR_NO != ret){
		OTA_SPI_ALOGE("spi_nor_read err:[ret=%d]",ret);
		return OTA_SPI_ERR_spi_nor_read;
	}

	//Before writing flash, must erase flash
	ret = chip->ops->erase(chip,offset,OTA_SPI_SPACE_FLAG_FEED_INFO);
	if(OTA_SPI_ERR_NO != ret){
		OTA_SPI_ALOGE("spi_nor_read err:[ret=%d]",ret);
		return OTA_SPI_ERR_spi_nor_erase;
	}

#if 1 
	//Reset OTA_FLAG_SDCARD as: 0xFF (0 - 8 Bytes)
	for(index = 0; index < OTA_SPI_FLAG_LENGTH; index++)
		data_source[index] = OTA_SPI_FLAG_SDCARD_NOT_UPDATE;
#else
	strncpy(data_source,"OTA_PASS",8);	//update OTA_Flag for CP Power_up_reason fetch
#endif
	
	ret = chip->ops->write(chip,offset,OTA_SPI_SPACE_FLAG_FEED_INFO,(uint8_t *)data_source);
	if(OTA_SPI_ERR_NO != ret){
		OTA_SPI_ALOGE("spi_nor_read err:[ret=%d]",ret);
		return OTA_SPI_ERR_spi_nor_erase;
	}

	return OTA_SPI_ERR_NO;	
}

static OTA_SPI_ERR_T ota_spi_erase_all(const UINT32 offset,const UINT32 erase_size, const UINT32 erase_unit)
{
	INT32 ret = 0;
	UINT32 erase_size_modified;
	UINT32 save_data_size;
	UINT32 save_data_offset;
	
	OTA_SPI_ALOGI("Func:%s,line:%d",__func__,__LINE__);
	OTA_SPI_ALOGD("Begin to erase, erase_size:[0x%x],erase_unit:[0x%x],offset:[0x%x]",erase_size,erase_unit,offset);
	if(erase_size % erase_unit == 0){
		ret = chip->ops->erase(chip,offset,erase_size);	//erase overall
		if(OTA_SPI_ERR_NO != ret){
			OTA_SPI_ALOGE("spi_nor_erase err:[ret=%d]",ret);
			return OTA_SPI_ERR_spi_nor_erase;
		}
	}else{
		save_data_size = erase_unit-erase_size%erase_unit;
		save_data_offset = erase_size/erase_unit*erase_unit+erase_size%erase_unit;
		erase_size_modified = erase_size/erase_unit*erase_unit + erase_unit;
		OTA_SPI_ALOGD("Erase parmeter, save_data_size:[0x%x],save_data_offset:[0x%x],erase_size_modified:[0x%x]",
			save_data_size,save_data_offset,erase_size_modified);
		
		//save the orignal data of flash
		ret = chip->ops->read(chip, offset+save_data_offset, save_data_size,ota_spi_backup_flash_data_buf + erase_size%erase_unit);
		if(OTA_SPI_ERR_NO != ret){
			OTA_SPI_ALOGE("spi_nor_read err:[ret=%d]",ret);
			return OTA_SPI_ERR_spi_nor_read;
		}

		//Erase enough space save update package
		ret = chip->ops->erase(chip,offset,erase_size_modified);
		if(OTA_SPI_ERR_NO != ret){
			OTA_SPI_ALOGE("spi_nor_erase err:[ret=%d]",ret);
			return OTA_SPI_ERR_spi_nor_erase;
		}
	}
	
	OTA_SPI_ALOGI("Erase end!");
	return OTA_SPI_ERR_NO;
}

static OTA_SPI_ERR_T ota_spi_crc_check_Image_in_Flash(const UINT32 offset, const UINT32 update_file_size,const UINT16 receive_crc_res)
{
	INT32 temp = 0;
	INT32 ret;
	UINT16 cal_crc_res;
	UINT16 crc_reg = 0xFFFF;

	//CRC check flash file
	OTA_SPI_ALOGI("crc check file written in flash");
	for( temp = 0; temp < update_file_size / OTA_SPI_FLASH_UNIT_SIZE; temp++ ){
		//read data from flash to psram
		//uart_printf("Function:%s,Begin to read data from flash into psram\r\n",__func__);
		ret = chip->ops->read( chip, offset+temp*OTA_SPI_FLASH_UNIT_SIZE, OTA_SPI_FLASH_UNIT_SIZE, (uint8_t *)ota_spi_flash_data_buf);
		if(OTA_SPI_ERR_NO != ret){
			OTA_SPI_ALOGE("spi_nor_read err:[ret=%d]",ret);
			return OTA_SPI_ERR_spi_nor_read;
		}
		crc_reg = ota_spi_crc_check_16((UINT8 *)ota_spi_flash_data_buf,OTA_SPI_FLASH_UNIT_SIZE,crc_reg);
	}

	if(update_file_size % OTA_SPI_FLASH_UNIT_SIZE != 0){
		//read file from flash into psram
		ret = chip->ops->read( chip, offset+temp*OTA_SPI_FLASH_UNIT_SIZE, update_file_size%OTA_SPI_FLASH_UNIT_SIZE, (uint8_t *)ota_spi_flash_data_buf);
		if(OTA_SPI_ERR_NO != ret){
			OTA_SPI_ALOGE("spi_nor_read err:[ret=%d]",ret);
			return OTA_SPI_ERR_spi_nor_read;
		}

		crc_reg = ota_spi_crc_check_16((UINT8 *)ota_spi_flash_data_buf,update_file_size%OTA_SPI_FLASH_UNIT_SIZE,crc_reg);
	}
	cal_crc_res = crc_reg;
		
	if(receive_crc_res != cal_crc_res){
		OTA_SPI_ALOGE("[receive_crc_res=0x%x],[cal_crc_res=0x%x]",receive_crc_res,cal_crc_res);
		return OTA_SPI_ERR_CRC_Check;
	}	
	
	//Empty ota_spi_flash_data_buf
	memset(ota_spi_flash_data_buf,0,OTA_SPI_FLASH_UNIT_SIZE);			

	
	OTA_SPI_ALOGI("CRC-16 Check OK! [receive_crc_res=0x%x],[cal_crc_res=0x%x]",receive_crc_res,cal_crc_res);
	return OTA_SPI_ERR_NO;
}

static UINT32 ota_spi_crc_check_16(UINT8* data, UINT32 length,UINT16 crc_reg)
{
	while (length--){
		crc_reg = (crc_reg >> 8) ^ crc16_table[(crc_reg ^ *data++) & 0xff];
	}
	return (UINT32)(crc_reg) & 0x0000FFFF;
}

static void ota_spi_slave_inpkt_handler(AMUX_INPKT_T *p_cp_amux_inpkt,OTA_SPI_ERR_T *p_ota_spi_res)
{
	static UINT32 s_flash_buffer_offset = 0;
	static UINT32 s_flash_offset_index = 0;
	static UINT32 s_sum_receive_update_pkg_bytes = 0;

	UINT32 real_copy_data_size;
	BOOL have_erased = TRUE;

	/* init input parameter. */
	//Warning: default return ota_spi_res value is "OTA_SPI_ERR_NO"
	*p_ota_spi_res = OTA_SPI_ERR_NO;
	
	switch((OTA_SPI_PACKAGE_TYPE)p_cp_amux_inpkt->type)
	{
		case OTA_SPI_TYPE_UPDATER_HANDSHAKE:
			OTA_SPI_ALOGI("slave receive handshake from master!");
		break;
	
		case OTA_SPI_TYPE_GET_HEADER_CRC:
			memcpy(ota_spi_crc_res_receive,p_cp_amux_inpkt->buf,OTA_SPI_CRC_LENGTH);
			OTA_SPI_ALOGI("ota_spi_crc_res_receive:0x%x",*((UINT32 *)ota_spi_crc_res_receive));
		break;

		case OTA_SPI_TYPE_RECEIVE_FILE_SIZE_OF_UPDATE_PKG:
			//1. update update file size
			ota_spi_update_file_size = *(UINT32 *)p_cp_amux_inpkt->buf;
			
			//2. Erase all flash
			OTA_SPI_ALOGI("Erase all flash, waiting...");
			*p_ota_spi_res = ota_spi_erase_all(OTA_SPI_SYSTEM_PART_OFFSET,ota_spi_update_file_size,OTA_SPI_FLASH_UNIT_SIZE);
			if(*p_ota_spi_res != OTA_SPI_ERR_NO){
				OTA_SPI_ALOGE("erase all flash fail!");
				while(1);
				//TODO: retry or abandon upgrades?
			}
		break;

		case OTA_SPI_TYPE_RECEIVE_UPDATE_PKG:
			//1. count the sum of received update file size 
			received_update_file_size += p_cp_amux_inpkt->len;
			if(received_update_file_size == ota_spi_update_file_size){
				is_receive_update_pkg_end = TRUE;
				OTA_SPI_ALOGW("is the end of update pkg.");
			}
	
			//2. copy data to psram buffer when less than "OTA_SPI_FLASH_UNIT_SIZE"
			s_sum_receive_update_pkg_bytes += p_cp_amux_inpkt->len;
			if( s_sum_receive_update_pkg_bytes < OTA_SPI_FLASH_UNIT_SIZE ){
				/* case 1: the sum receive data size is less than or  multiple of "OTA_SPI_FLASH_UNIT_SIZE" */
				real_copy_data_size = p_cp_amux_inpkt->len;
				memcpy(ota_spi_flash_data_buf + s_flash_buffer_offset,p_cp_amux_inpkt->buf,real_copy_data_size);		
				s_flash_buffer_offset += real_copy_data_size;
			}else if(s_sum_receive_update_pkg_bytes == OTA_SPI_FLASH_UNIT_SIZE){
				/* case 2: the sum receive data size is less than or  multiple of "OTA_SPI_FLASH_UNIT_SIZE" */
				real_copy_data_size = p_cp_amux_inpkt->len;
				memcpy(ota_spi_flash_data_buf + s_flash_buffer_offset,p_cp_amux_inpkt->buf,real_copy_data_size);
				s_flash_buffer_offset = 0;
			}else if(s_sum_receive_update_pkg_bytes > OTA_SPI_FLASH_UNIT_SIZE){
				/* case 3: the sum receive data size is more than "OTA_SPI_FLASH_UNIT_SIZE" */
				real_copy_data_size = OTA_SPI_FLASH_UNIT_SIZE - s_flash_buffer_offset;
				memcpy(ota_spi_flash_data_buf + s_flash_buffer_offset,p_cp_amux_inpkt->buf,real_copy_data_size);
				s_flash_buffer_offset = 0;
			}
		
			
			//3. write data into flash
			if(s_sum_receive_update_pkg_bytes >= OTA_SPI_FLASH_UNIT_SIZE || is_receive_update_pkg_end){ 		
#ifdef OTA_SPI_UPDATING_LOG
				OTA_SPI_ALOGD("  write flash,s_flash_offset_index:%d",s_flash_offset_index);
				OTA_SPI_ALOGD("s_sum_receive_update_pkg_bytes:%d",s_sum_receive_update_pkg_bytes);
#endif
				//3.1 wirte flash content
				if(is_receive_update_pkg_end){
					/* Note: The flash for the data to be written has not been erased */
					have_erased = FALSE;
					OTA_SPI_ALOGW("flash not erase at the end of the update package.");
				}
				*p_ota_spi_res = ota_spi_write_flash(OTA_SPI_SYSTEM_PART_OFFSET+ s_flash_offset_index * OTA_SPI_FLASH_UNIT_SIZE,
					OTA_SPI_FLASH_UNIT_SIZE, ota_spi_flash_data_buf,have_erased);
				if(*p_ota_spi_res != OTA_SPI_ERR_NO){
					OTA_SPI_ALOGE("erase write flash err,*p_ota_spi_res:0x%x", *p_ota_spi_res);
					return OTA_SPI_ERR_SPI_ERASE_WRITE;
				}
				s_flash_offset_index++;
	
				//3.2 memset ota_spi_flash_data_buf
				memset(ota_spi_flash_data_buf,0,OTA_SPI_FLASH_UNIT_SIZE);
	
				//3.3 update some parameters
				if(!is_receive_update_pkg_end){
					if(s_sum_receive_update_pkg_bytes > OTA_SPI_FLASH_UNIT_SIZE){
						//case 1: the sum receive data size is more than OTA_SPI_FLASH_UNIT_SIZE"
						UINT32 ota_spi_buffer_offset;
						
						ota_spi_buffer_offset = real_copy_data_size;
						real_copy_data_size = p_cp_amux_inpkt->len - real_copy_data_size;
						memcpy(ota_spi_flash_data_buf + s_flash_buffer_offset,p_cp_amux_inpkt->buf + ota_spi_buffer_offset, real_copy_data_size);
						
						s_sum_receive_update_pkg_bytes = real_copy_data_size;
						s_flash_buffer_offset = real_copy_data_size;
					}else if(s_sum_receive_update_pkg_bytes == OTA_SPI_FLASH_UNIT_SIZE){
						//case 2: the sum receive data size is equal OTA_SPI_FLASH_UNIT_SIZE"
						s_sum_receive_update_pkg_bytes = 0;
					}
				}
			}
		break;

		case OTA_SPI_TYPE_CRC_CHECK_AFTER_UPDATING:
			OTA_SPI_ALOGI("CP crc check image has been written in flash");
			*p_ota_spi_res = ota_spi_crc_check_Image_in_Flash(OTA_SPI_SYSTEM_PART_OFFSET, ota_spi_update_file_size,
				*((UINT16 *)ota_spi_crc_res_receive));
			if(*p_ota_spi_res != OTA_SPI_ERR_NO){
				OTA_SPI_ALOGE("crc check image has been written in flash fail!");
				while(1);
				//TODO: retry or abandon upgrades?
			}
		break;

		case OTA_SPI_TYPE_RESET_CP_FLAG:		
			OTA_SPI_ALOGI("CP reset ota_spi_flag");
			*p_ota_spi_res = ota_spi_reset_ota_spi_flag(get_fota_param_start_addr() - 0x80000000);
			if(OTA_SPI_ERR_NO != *p_ota_spi_res){					
				OTA_SPI_ALOGE("reset_ota_spi_flag err:%d", *p_ota_spi_res);
				while(1);
			}
		break;

		default:
			OTA_SPI_ALOGE("slave receive error packet!");
			*p_ota_spi_res = OTA_SPI_ERR_INPKT_TYPE;
	}
}

#ifdef OTA_SPI_SUPPORT_UI_API
void ota_spi_progress_ratio(UINT16 ratio)
{
	OTA_SPI_ALOGI("ration: %d",ratio);
}
#endif

static void ota_spi_dump_input_packet(UINT8 *input_buf,const int buf_len )
{
	uart_printf("=========dump input packet=========\r\n");
	uart_printf("flag1:0x%04x,mux_address:0x%04x,control:0x%04x,mux_length:0x%04x,flag2:0x%x\r\n",
		*(UINT8 *)(input_buf)    , 
	    *(UINT8 *)(input_buf+1)  ,
	    *(UINT8 *)(input_buf+2)  ,
	    *(UINT16 *)(input_buf+3) ,
	    *(UINT8 *)(input_buf + buf_len -1)
   				);	
}



#ifdef OTA_SPI_DEBUG_FUNC
OTA_SPI_ERR_T ota_spi_d_print_flash_image_to_sd(const UINT32 offset, const UINT32 update_file_size)
{
	INT32 temp = 0;
	INT32 ret;
	FRESULT sd_res;
	UINT32 write_size;
	#define OTA_SPI_SD_FILENAME "D:/debug_update.bin"

	OTA_SPI_ALOGI("print file written in flash to sd");

	/* 1. create new file */
		
	sd_res = f_open(&ota_spi_handle, OTA_SPI_SD_FILENAME,FA_CREATE_ALWAYS | FA_WRITE);
	if(FR_OK != sd_res){	//f_open FAIL				
		OTA_SPI_ALOGE("f_open err:%d", sd_res);
		while(1);
	}
	
	/* 2. read file from flash and write into sd card */
	for( temp = 0; temp < update_file_size / OTA_SPI_FLASH_UNIT_SIZE; temp++ ){
		//read data from flash to psram
		//uart_printf("Function:%s,Begin to read data from flash into psram\r\n",__func__);
		ret = chip->ops->read( chip, offset+temp*OTA_SPI_FLASH_UNIT_SIZE, OTA_SPI_FLASH_UNIT_SIZE, (uint8_t *)ota_spi_flash_data_buf);
		if(OTA_SPI_ERR_NO != ret){
			OTA_SPI_ALOGE("spi_nor_read err:[ret=%d]",ret);
			return OTA_SPI_ERR_spi_nor_read;
		}

		sd_res = f_write(&ota_spi_handle, (void *)ota_spi_flash_data_buf, OTA_SPI_FLASH_UNIT_SIZE, &write_size);
		if(FR_OK != sd_res){
			OTA_SPI_ALOGE("f_read err:[ret=%d]",sd_res);
			return OTA_SPI_ERR_f_write;
		}
	}

	if(update_file_size % OTA_SPI_FLASH_UNIT_SIZE != 0){
		//read file from flash into psram
		ret = chip->ops->read( chip, offset+temp*OTA_SPI_FLASH_UNIT_SIZE, update_file_size%OTA_SPI_FLASH_UNIT_SIZE, (uint8_t *)ota_spi_flash_data_buf);
		if(OTA_SPI_ERR_NO != ret){
			OTA_SPI_ALOGE("spi_nor_read err:[ret=%d]",ret);
			return OTA_SPI_ERR_spi_nor_read;
		}

		sd_res = f_write(&ota_spi_handle, (void *)ota_spi_flash_data_buf, update_file_size%OTA_SPI_FLASH_UNIT_SIZE, &write_size);
		if(FR_OK != sd_res){
			OTA_SPI_ALOGE("f_read err:[ret=%d]",sd_res);
			return OTA_SPI_ERR_f_write;
		}
	}
	
	
	//Empty ota_spi_flash_data_buf
	memset(ota_spi_flash_data_buf,0,OTA_SPI_FLASH_UNIT_SIZE);

	//Lastly,close the opened file in SD card
	sd_res = f_close(&ota_spi_handle);
	if(FR_OK != sd_res){	//f_close FAIL				
		OTA_SPI_ALOGE("f_close err:%d", sd_res);
		while(1);
	}

	
	OTA_SPI_ALOGI("print file written in flash to sd OK");
	return OTA_SPI_ERR_NO;
}

/* 2. define some functions related to ASRSPI */
void ota_spi_print_amux_buf(const char *p_buf, const UINT32 buf_len)
{
	UINT32 i;

	if(p_buf == NULL){
		uart_printf("[ASRSPI] param err, p_buf err\r\n");
	}
	

	uart_printf("+++++++++++%s+++++++++++++++++\r\n","[debug] ota_spi_print_amux_buf start");

	//1. print amux header info: including:
/*
		-------------------------------------------------------------
	| Flag | Address | Control | Length indication | Information | FCS | Flag |
	-------------------------------------------------------------
	| 1B   | 1B      |  1B    |       2B          | Information | 1B  | 1B  |
	-------------------------------------------------------------
*/
#if 0
	uart_printf("1. amux_header info: \r\n");
	uart_printf("    flag:0x%x,addr:0x%x,Control:0x%x,length:0x%x\r\n",
					p_buf[0],p_buf[1],p_buf[2],*(UINT16 *)(p_buf+3));
#endif	
	//2. print ota_spi package header info
/*
	------------------------------------------------------
	| type | len buffer    | idx  | 	buf 	   Reserved |  FCS |
	------------------------------------------------------
	| 4B   | 4B 	   |4B   | max: 1536 B	       448 B 	|	4B	|
	------------------------------------------------------
*/
	uart_printf("2. ota_spi pkg header info: \r\n");
	uart_printf("    type:0x%x  ",*(UINT32 *)(p_buf + 5));
	uart_printf("    len:0x%x  ",*(UINT32 *)(p_buf + 5 + 4));
	uart_printf("    idx:0x%x  ",*(UINT32 *)(p_buf + 5 + 8));
	
	uart_printf("3. ota_spi pkg fcs:0x%x  \r\n",*(UINT32 *)(p_buf + 2001));

	//uart_printf("4. amux_pkg tail info: end_flag:0x%x\r\n",*(p_buf + 2006));
	
	uart_printf("------------%s-------------------\r\n","[debug] ota_spi_print_amux_buf end");
}

#endif
