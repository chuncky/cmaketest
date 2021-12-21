/*
*****************************************************************************************
*											Control Uart Log
*OTA_SDCARD_DEBUG_STATEMENT: If define this MACRO, build the corresponding debug statement
*LOG_NDEBUG:       If define MACRO LOG_NDEBUG, remove "ALOGV" and "ALOGD" log
*LOG_TAG:         NDEBUG names as function name
*****************************************************************************************
*/

//#define OTA_SDCARD_DEBUG_STATEMENT
//#define NDEBUG
#define LOG_TAG "OTA_sdcard"


/*
*****************************************************************************************
*											INCLUDE FILES
*****************************************************************************************
*/
#include "OTA_sdcard.h"
#include "mci_lcd.h"

/*
*****************************************************************************************
*											Extern Variables
*****************************************************************************************
*/
extern struct spi_flash_chip *chip;	//hsy
extern MCI_ASR_LCD_INFO lcd_info_test;
/*
*****************************************************************************************
*											Extern FUNCTION PROTOTYPES
*****************************************************************************************
*/
extern int sdcard_fat_is_ok(void);

/*
*****************************************************************************************
*											GLOBAL VARIABLES
*****************************************************************************************
*/
FIL handle;
char OTA_Sd_KeyFlag[OTA_FLAG_LENGTH];


/*
*****************************************************************************************
*											LOCAL GLOBAL VARIABLES
*****************************************************************************************
*/
static unsigned char data_buf[DATA_BUF_SIZE];
static unsigned char bak_buf[DATA_BUF_SIZE];

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
*											LOCAL FUNCTION PROTOTYPES
*****************************************************************************************
*/
	
/*------------------------------------------------------------*/
//1. Function Prototypes that OTA_Sd_Flag and Ota_Sd_Keypad_Flag all use

/*
Function:	  get_fota_param_offset
Description:  get the fota_param offset in partition table
Parmeters:	  NONE	  
Return value: Return the fota_param offset			
*/
static UINT32 get_fota_param_offset(void);

/*
Function:	  Check_SdCard_Mount
Description:  Check SD card mount successful or not?
Parmeters:	  void	  
Return value:  If mount successful, return SDCARD_FAT_IS_OK	
*/
static int Check_SdCard_Mount(void);

/*
Function:	  Jump_To_Real_Update_File
Description:  Skip the header file
Parmeters:	  @fp:		The Skip File Pointer
			  @len: 	Skip length, unit:Bytes
Return value: NULL	
*/
static void Jump_To_Real_Update_File(FIL *fp, FSIZE_t len);


/*
Function:	  Crc_Check_16
Description:  CRC_CHECK_16
Parmeters:	  @data:Array name to be verified
			  @length: Array length to be verified
			  @crc_reg: CRC_16 Check Code
Return value:  CRC Check Value
*/
static UINT32 Crc_Check_16(UINT8* data, UINT32 length,UINT16 crc_reg);

/*
Function:	  Erase_Flash
Description:  Erase the specified area of nor flash
Parmeters:	  @erase_size:	The size of file to be written from SD card into norflash
			  @erase_unit:	Number of Bytes written to flash at a time.
			  @offset:	 The erasing area of starting address	  
Return value: If erase successfully, return OTA_ERR_NO			
*/
static int Erase_Flash(const UINT32 erase_size, const UINT32 erase_unit,const UINT32 offset);


/*
Function:	  Write_File_From_Sd_To_Flash
Description:  
Parmeters:	  @write_size: The size of file to be written from SD card into norflash
			  @offset:	   The file store address
Return value:  If write successfully, return OTA_ERR_NO 
*/
static int Write_File_From_Sd_To_Flash(const UINT32 write_size,const UINT32 offset);


/*
Function:	  CRC_Check_Image_Sd_And_Flash
Description:   After upgrading, check the data written in flash is same with the update file data in SD Card or not
Parmeters:	  @offset: The checked file in flash address
			  @file_size: The checked file size
Return value:  If the two Checked Code is same, return OTA_ERR_NO	
*/
static int CRC_Check_Image_Sd_And_Flash(const UINT32 offset, const UINT32 file_size);

/*------------------------------------------------------------*/
//2. Function Prototypes that only OTA_Sd_Keypad_Flag use 

/*
Function:	  CRC_Check_Update_File
Description:  Cpmpare crc check code in SD Card is same with crc check code in header file
Parmeters:	  NULL
Return value: If Crc check code	is same with header file
				return OTA_CRC_SAME
			  else if Crc check code is different with header file
			  	return OTA_CRC_DIFFEREBT
*/
static OTA_ERR_T CRC_Check_Update_File(void);

/*
Function:	  Erase_Feedback_Info_In_Flash
Description:  Erase the OTA_sdcard log from flash
Parmeters:	  @offset: The flash offset
			  @offset_char: offset corresponding flash
			  @feedback_info: feedback information erasing from flash
Return value: If Erase success, return OTA_ERR_NO
			  else
			  	erase error
*/
static INT8 Erase_Feedback_Info_In_Flash(const UINT32 offset,int offset_char, int length);


/*
Function:	  Get_Update_File_Version
Description:  Get_Update_File_Version
Parmeters:	  @p_update_file_version: return the update_file_version

Return value: If get success, return OTA_ERR_NO
			  else
			  	error
*/
static OTA_ERR_T Get_Update_File_Version(char *p_update_file_version);


/*
Function:	  Write_Feedback_Info_To_Flash
Description:  Write the OTA_sdcard log into flash
Parmeters:	  @offset: The flash offset
			  @offset_char: offset corresponding flash
			  @feedback_info: feedback information written into flash
Return value: If write info into flash success, return OTA_ERR_NO
			  else
			  	write error
*/
static INT8 Write_Feedback_Info_To_Flash(const UINT32 offset,int offset_char,const char *feedback_info);

/*
Function:	  Print_OTA_Feed_Info
Description:  Print the contents of the flash of the specified address 
Parmeters:	  @offset: The flash offset
			  @offset_char: offset corresponding flash
			  @length: The info length
Return value: If print info from flash success, return OTA_ERR_NO
			  else
			  	return OTA_ERR_spi_nor_read
*/
static OTA_ERR_T Print_OTA_Feed_Info(const UINT32 offset,int offset_char,int length);

/*--------------------------------------------------------------------*/
//3. Function Prototypes that only "OTA_Sd_Keypad_Flag" use 

/*
Function:	  Read_OTA_Flag
Description:  Read the OTA_Flag from flash
Parmeters:	  @offset: The OTA_Flag in flash offset address
			  @ota_flag_sdcard:  Save the ota_flag address as ota_flag_length modified from 1Bytes --> 9Bytes
Return value: If read success, return the OTA_ERR_NO	
*/
static OTA_ERR_T Read_OTA_Flag(const UINT32 offset, char *ota_flag_sdcard);

/*
Function:	  Reset_OTA_Flag
Description:  Reset ota_flag_sdcard as "OTA_PASS" (9 Bytes)
Parmeters:	  @offset: OTA_FLAG_SDCARD store in Flash
Return value: If reset OTA_Flag_sdcard successfully, return OTA_ERR_NO	
*/
static OTA_ERR_T Reset_OTA_Flag(const UINT32 offset);



/*
*****************************************************************************************
*											GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void ASR_OTA_Sdcard(void)
{
	FRESULT res;
	OTA_ERR_T ret;
	UINT32 size_sd_update_file;
	UINT32 size_sd_update_file_revision;
	UINT32 fota_param_offset;
	char ota_flag_sdcard[OTA_FLAG_LENGTH];
	char update_file_version[OTA_VERSION_LENGTH];
	INT8 Write_Feedback_Res;
	char OTA_LCD_info[10][40] = {	
									"Start Updating!",					//0   ----   ota status info
									"Start Erasing Flash",				//1   ----   ota status info
									"Start Writing Flash",				//2   ----   ota status info
									"Update Success!",					//3   ----   ota status info
									"OTA_Sd update Failed!",			//4   ----   ota status info
									"Update file Error!",				//5   ----   ota status info
									"Write End"							//6   ----   ota status info
								};		

	uart_printf("[%57s]\r\n","---------------------------------------------------------");
	ALOGD("Func:%s,line:%d",__func__,__LINE__);
#ifdef LCD_USE_IN_CODE	//do not turn on the blacklight
	mci_LcdSetBrightness(5);	//Turn on the blacklight later to fix "Blurred Screen"
#endif
	OTA_LCD_DisplayText(NULL,OTA_DISPLAY_INFO_TYPE_UPGRADE_PRE,g_langulage_type);
	
	
	//[0]get fota_param_offset
	fota_param_offset = get_fota_param_offset();

	//[1]Check OTA_Sd Flag
	uart_printf("----------%s----------\r\n","[1] Check OTA_Sd Flag ?");
	ret = Read_OTA_Flag(fota_param_offset,ota_flag_sdcard);
	if(OTA_ERR_NO != ret){
		ALOGE("ret=[%d]",ret);
		while(1);
	}
	if(0 != strcmp(ota_flag_sdcard,OTA_FLAG_SDCARD_UPDATE)){
		ALOGE("OTA_Sd Failed! No OTA_Sd upgrade Flag.");
		while(1);	
	}

	//[2]Ceck SD Mount Success?
	uart_printf("----------%s----------\r\n","[2] Ceck SD Mount Success?");
	if(SDCARD_FAT_IS_OK != Check_SdCard_Mount()){
		ALOGE("OTA_Sd Failed! SDcard Mount Fail.");
		while(1);
	}

	//Get Header file and Update File Size from update.bin
	ALOGD("SDcard FAT32 OK");		
	res = f_open(&handle, FILENAME, FA_READ);
	if(FR_OK != res){	//f_open FAIL				
		ALOGE("f_open err:%d", res);
		while(1);
	}
	//size_sd_update_file: ''header file size'' + ''size of *.bin''
	//size_sd_update_file_revision: ''size of *.bin''
	size_sd_update_file = f_size(&handle);
	size_sd_update_file_revision = size_sd_update_file - OTA_HEADER_LENGTH;
	ALOGD("[%s] size is %x,revision size is %x",FILENAME,size_sd_update_file,size_sd_update_file_revision);

	res = f_close(&handle);
	if(FR_OK != res){	//f_close FAIL				
		ALOGE("f_close err:%d", res);
		while(1);
	}

	//[3]Before upgrading, CRC check the file right?
	uart_printf("----------%s----------\r\n","[3] Before upgrading,CRC check the file right?");
	ret = (OTA_ERR_T)CRC_Check_Update_File();

	if(ret != OTA_CRC_DIFFERENT && ret != OTA_CRC_SAME){
		ALOGE("Error in CRC Check before updating");
		while(1);
	}
	
	if(OTA_CRC_DIFFERENT == ret){
		ALOGE("OTA_Sd Failed! CRC_Check_Update_File err:%d",ret);
		OTA_LCD_DisplayText(OTA_LCD_info[5],OTA_DISPLAY_INFO_TYPE_UPGRADE_FAIL,g_langulage_type);
		while(1);
#ifdef OTA_SDCARD_DEBUG_STATEMENT		
		//Reset OTA_flag and return to reboot
		ret = Reset_OTA_Flag((UINT32)(get_fota_param_start_addr() - 0x80000000));
		if(OTA_ERR_NO != ret){					
			ALOGE("Reset_OTA_Flag err:%d", ret);
			while(1);
		}
		return;
#endif
	}
	
	//[4]Erase flash
	uart_printf("----------%s----------\r\n","[4] Erase flash");
	
	OTA_LCD_DisplayText(NULL,OTA_DISPLAY_INFO_TYPE_UPGRADE_PRE,g_langulage_type);
	ret = (OTA_ERR_T)Erase_Flash(size_sd_update_file_revision, DATA_BUF_SIZE,SYSTEM_PART_OFFSET);
	if(OTA_ERR_NO != ret){					
		ALOGE("Erase flash err:%d", ret);
		while(1);
	}
	
	//[5]copy file from sd card to flash
	uart_printf("----------%s----------\r\n","[5] copy file from sd card to flash");
	ret = (OTA_ERR_T)Write_File_From_Sd_To_Flash(size_sd_update_file_revision, SYSTEM_PART_OFFSET);
	if(OTA_ERR_NO != ret){					
		ALOGE("Write_File_From_Sd_To_Flash err:%d", ret);
		while(1);
	}
	//OTA_LCD_DisplayText(NULL,OTA_DISPLAY_INFO_TYPE_UPDATING,g_langulage_type);
	
	//[6]CRC-16 Check cp.bin in flash and sd, avoid to update error
	uart_printf("----------%s----------\r\n","[6] After updating,CRC-16 Check cp.bin in flash and sd");
	ret = (OTA_ERR_T)CRC_Check_Image_Sd_And_Flash(SYSTEM_PART_OFFSET,size_sd_update_file_revision);
	if(OTA_ERR_NO != ret){					
		ALOGE("CRC_Check_Image_Sd_And_Flash err:%d", ret);
		while(1);
	}
	OTA_LCD_ShowImage();
	OTA_Clear_LCD();

		//[6] Write the Version Info Into Flash after OTA_Sd success 
		ret = Get_Update_File_Version(update_file_version);
		if(ret != OTA_ERR_NO){
			ALOGE("Get_Update_File_Version ERROR,ret:[0x%x]",ret);
			while(1);
		}
		ret = Write_Feedback_Info_To_Flash(fota_param_offset, OFFSET_OTA_FEED_CURRENT_VERSION, update_file_version);
		if(ret != OTA_ERR_NO){
			ALOGE("Write Current UUpdate File Version Err,ret:[0x%x]",ret);
			while(1);
		}

	//[7]reset OTA_Flag,value:  "OTA_PASS"
	uart_printf("----------%s----------\r\n","[7] Reset OTA_Flag");
	ret = Reset_OTA_Flag((UINT32)(get_fota_param_start_addr() - 0x80000000));
	if(OTA_ERR_NO != ret){					
		ALOGE("Reset_OTA_Flag err:%d", ret);
		while(1);
	}		
	ALOGI("OTA_Sd Success!");
	//OTA_LCD_DisplayText(OTA_LCD_info[3],OTA_DISPLAY_INFO_TYPE_UPGRADE_SUCCESS,g_langulage_type);
	uart_printf("[%57s]\r\n","---------------------------------------------------------");
		
	return;	
}

void ASR_OTA_Key_Sdcard(void)
{
	FRESULT res;
	OTA_ERR_T ret;
	UINT32 size_sd_update_file;
	UINT32 size_sd_update_file_revision;
	INT8 Write_Feedback_Res;
	INT8 Erase_Feedback_Res;
	UINT32 fota_param_offset;
	char update_file_version[OTA_VERSION_LENGTH];

	uart_printf("[%57s]\r\n","---------------------------------------------------------");
	ALOGI("Func:%s,line:%d",__func__,__LINE__);

	//[0]get fota_param_offset
	fota_param_offset = get_fota_param_offset();
	
	//[1] Check SD mount and open file Success?
	uart_printf("----------%s----------\r\n","[1] Check SD mount and Open file success ?");
	if(SDCARD_FAT_IS_OK !=	Check_SdCard_Mount()){
		ALOGW("SDcard FAT32 Fail!"); 
		memset(OTA_Sd_KeyFlag,0xFF,sizeof(OTA_Sd_KeyFlag)); 
		return;
	}		
	res = f_open(&handle, FILENAME, FA_READ);
	if(FR_OK != res){	//f_open FAIL				
		ALOGW("f_open [%s] err:%d",FILENAME,res);
		memset(OTA_Sd_KeyFlag,0xFF,sizeof(OTA_Sd_KeyFlag)); 
		return;
	}
	
	//size_sd_update_file: ''header file size'' + ''size of *.bin''
	//size_sd_update_file_revision: ''size of *.bin''
	size_sd_update_file = f_size(&handle);
	size_sd_update_file_revision = size_sd_update_file - OTA_HEADER_LENGTH;
	ALOGD("[%s] size is %x,revision size is %x",FILENAME,size_sd_update_file,size_sd_update_file_revision);
	res = f_close(&handle);
	if(FR_OK != res){	//f_close FAIL				
		ALOGE("f_close [%s] err:%d",FILENAME,res);
		while(1);
	}
	
	//[2]Before upgrading, CRC check the file right?
	uart_printf("----------%s----------\r\n","[2] check CRC-16");
	ret = (OTA_ERR_T)CRC_Check_Update_File();

	if(ret != OTA_CRC_DIFFERENT && ret != OTA_CRC_SAME){
		ALOGE("Error in CRC Check before updating");
		while(1);
	}
	
	if(OTA_CRC_DIFFERENT == ret){
		ALOGI("CRC_Check_Update_File err:%d",ret);
	
		//write feedback info into flash
		Write_Feedback_Res = Write_Feedback_Info_To_Flash(fota_param_offset,OFFSET_OTA_CRC_Incorrect,
															OTA_UPDATE_FILE_CRC_INCORRECT);
		ASSERT(OTA_ERR_NO == Write_Feedback_Res);
		ALOGI("CRC SD res different with Header File, and incorrect info saves in flash addr:0x%lx",fota_param_offset +
						OFFSET_OTA_CRC_Incorrect);
		ret = Print_OTA_Feed_Info(fota_param_offset,OFFSET_OTA_CRC_Incorrect,
								OTA_UPDATE_FILE_CRC_INCORRECT_LENGTH);
		if(OTA_ERR_NO != ret){
			ALOGE("Error in Print_OTA_Feed_Info");
			while(1);
		}
		
		memset(OTA_Sd_KeyFlag,0xFF,sizeof(OTA_Sd_KeyFlag)); 
		return;
	}else if(OTA_CRC_SAME == ret){
		//Erase the feedback info
		Erase_Feedback_Res = Erase_Feedback_Info_In_Flash(fota_param_offset,OFFSET_OTA_CRC_Incorrect,
															OTA_UPDATE_FILE_CRC_INCORRECT_LENGTH);
		if(OTA_ERR_NO == Erase_Feedback_Res){
			ALOGI("Have Erase info:[%s]",OTA_UPDATE_FILE_CRC_INCORRECT);
		}else if(OTA_FLASH_NO_Write == Erase_Feedback_Res){
			ALOGI("No Write info:[%s]",OTA_UPDATE_FILE_CRC_INCORRECT);
		}else{
			//Erase Fail
			while(1);
		}
	}
	
	//[3]Erase flash
	uart_printf("----------%s----------\r\n","[3] Erase flash");
	ret = (OTA_ERR_T)Erase_Flash(size_sd_update_file_revision, DATA_BUF_SIZE,SYSTEM_PART_OFFSET);
	if(OTA_ERR_NO != ret){					
		ALOGE("Erase flash err:%d", ret);
		while(1);
	}
	
	//[4]copy file from sd card to flash
	uart_printf("----------%s----------\r\n","[4] Write file from sd to flash");
	ret = (OTA_ERR_T)Write_File_From_Sd_To_Flash(size_sd_update_file_revision, SYSTEM_PART_OFFSET);
	if(OTA_ERR_NO != ret){					
		ALOGE("Write_File_From_Sd_To_Flash err:%d", ret);
		while(1);
	}
	
	//[5]CRC-16 Check cp.bin in flash and sd, avoid to update error
	uart_printf("----------%s----------\r\n","[5] Crc check .bin in flash with sd");
	ret = (OTA_ERR_T)CRC_Check_Image_Sd_And_Flash(SYSTEM_PART_OFFSET,size_sd_update_file_revision);
	if(OTA_ERR_NO != ret){					
		ALOGE("CRC_Check_Image_Sd_And_Flash err:%d", ret);
		while(1);
	}

	//[6] Write the Version Info Into Flash after OTA_Sd success 
	ret = Get_Update_File_Version(update_file_version);
	if(ret != OTA_ERR_NO){
		ALOGE("Get_Update_File_Version ERROR,ret:[0x%x]",ret);
		while(1);
	}
	ret = Write_Feedback_Info_To_Flash(fota_param_offset, OFFSET_OTA_FEED_CURRENT_VERSION, update_file_version);
	if(ret != OTA_ERR_NO){
		ALOGE("Write Current UUpdate File Version Err,ret:[0x%x]",ret);
		while(1);
	}
	
	//[7]reset OTA_Sd_KeyFlag, value:	OTA_FLAG_SDCARD_NOT_UPDATE(0xFF)
	memset(OTA_Sd_KeyFlag,0xFF,sizeof(OTA_Sd_KeyFlag));	
	
	ALOGI("OTA_Sd Success!");
	uart_printf("[%57s]\r\n","---------------------------------------------------------");		
	return; 
}


/*
*****************************************************************************************
*											LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static UINT32 get_fota_param_offset(void)
{
	//ALOGD("fota_param_offset:[0x%lx]",get_fota_param_start_addr() - 0x80000000);
	return (get_fota_param_start_addr() - 0x80000000);
}

static int Check_SdCard_Mount(void)
{
	int ret;

	ret = sdcard_fat_is_ok();

	return ret;
}

static UINT32 Crc_Check_16(UINT8* data, UINT32 length,UINT16 crc_reg)
{
	while (length--){
		crc_reg = (crc_reg >> 8) ^ crc16_table[(crc_reg ^ *data++) & 0xff];
	}
	return (UINT32)(~crc_reg) & 0x0000FFFF;
}

static OTA_ERR_T CRC_Check_Update_File(void)
{
	FRESULT res;
	int temp;
	int ret;
	UINT16 crc_reg_sd = 0xFFFF;
	unsigned int read_size;
	UINT16 SD_HEADER_OTA_CRC_Res = 0x0;
	UINT32 size_sd_update_file_revision;


	ALOGI("Func:%s,Line:%d",__func__,__LINE__);

	//[1] Firstly, open file in SD card and Jump to real location
	res = f_open(&handle,FILENAME,FA_READ);
	if(FR_OK != res){	//f_open FAIL
		ALOGE("f_open err:%d",res);
		return OTA_ERR_f_open;
	}
	size_sd_update_file_revision = f_size(&handle) - OTA_HEADER_LENGTH;
	
	Jump_To_Real_Update_File(&handle,OTA_HEADER_LENGTH);

	for(temp = 0; temp < size_sd_update_file_revision / DATA_BUF_SIZE; temp++){
		//read file from SD into psram
		ret = f_read(&handle,data_buf,DATA_BUF_SIZE,&read_size);
		if(FR_OK != ret){
			ALOGE("f_read err:[ret=%d]",ret);
			f_close(&handle);
			return OTA_ERR_f_read;
		}
		crc_reg_sd = Crc_Check_16((UINT8 *)data_buf, DATA_BUF_SIZE,crc_reg_sd);
	}

	if(size_sd_update_file_revision % DATA_BUF_SIZE != 0){
		//read file from SD in psram
		ret = f_read(&handle,data_buf,size_sd_update_file_revision % DATA_BUF_SIZE,&read_size);
		if(FR_OK != ret){
			ALOGE("f_read err:%d",ret);
			f_close(&handle);
			return OTA_ERR_f_read;
		}
		crc_reg_sd = Crc_Check_16((UINT8 *)data_buf, size_sd_update_file_revision % DATA_BUF_SIZE,crc_reg_sd);
	}

	//crc_reg_sd is the sd card's crc_result
	//compare with header file crc_result
	
	//f_lseek,jump to OTA_CRC_RESULT_LENGTH of update file
	Jump_To_Real_Update_File(&handle,OTA_CRC_RESULT_OFFSET);

	//read crc result info from SD in psram
	ret = f_read(&handle,&SD_HEADER_OTA_CRC_Res,OTA_CRC_RESULT_LENGTH,&read_size);
	if(FR_OK != ret){
		ALOGE("f_read err:%d",ret);
		f_close(&handle);
		return OTA_ERR_f_read;
	}

	ALOGD("SD_HEADER_OTA_CRC_Res:0x%x,crc_reg_sd:0x%x",SD_HEADER_OTA_CRC_Res,crc_reg_sd);
	if(crc_reg_sd != SD_HEADER_OTA_CRC_Res){
		f_close(&handle);
		return OTA_CRC_DIFFERENT;
	}else{
		return OTA_CRC_SAME;
	}
}

static INT8 Erase_Feedback_Info_In_Flash(const UINT32 offset,int offset_char, int length)
{
	int ret;
	char data_source_local[SPACE_FLAG_FEED_INFO];

	ALOGI("Func:%s,line:%d",__func__,__LINE__);

	//before erase, need to save source sector data because size of flag is only 9Bytes  
	ret = chip->ops->read( chip, offset, SPACE_FLAG_FEED_INFO, (uint8_t *)data_source_local);
	if(OTA_ERR_NO != ret){
		ALOGE("spi_nor_read err:[ret=%d]",ret);
		return OTA_ERR_spi_nor_read;
	}

	if(0xFF == data_source_local[offset_char]){
		//The flash area has not written feedback info
		return OTA_FLASH_NO_Write;
	}

	//Before OTA_Sdcard upgrade, must erase the previous feedback info	otherwise Version info
	ret = chip->ops->erase( chip, offset, SPACE_FLAG_FEED_INFO);
	if(OTA_ERR_NO != ret){
		ALOGE("spi_nor_erase err:[ret=%d]",ret);
		return OTA_ERR_spi_nor_erase;
	}

	//wirte relvant feedback info into flash
	memset(data_source_local + offset_char,0xFF,length);

	ret = chip->ops->write( chip, offset, SPACE_FLAG_FEED_INFO,(uint8_t *)data_source_local);
	if(OTA_ERR_NO != ret){
		ALOGE("spi_nor_erase err:[ret=%d]",ret);
		return OTA_ERR_spi_nor_write;
	}
	return OTA_ERR_NO;	
}

static INT8 Write_Feedback_Info_To_Flash(const UINT32 offset,int offset_char,const char *feedback_info)
{
	int ret;
	char data_source_local[SPACE_FLAG_FEED_INFO];

	ALOGI("Func:%s,line:%d",__func__,__LINE__);

	//before erase, need to save source sector data because size of flag is only 256Bytes  
	ret = chip->ops->read( chip, offset, SPACE_FLAG_FEED_INFO, (uint8_t *)data_source_local);
	if(OTA_ERR_NO != ret){
		ALOGE("spi_nor_read err:[ret=%d]",ret);
		return OTA_ERR_spi_nor_read;
	}
	
	ret = chip->ops->erase( chip, offset, SPACE_FLAG_FEED_INFO);
	if(OTA_ERR_NO != ret){
		ALOGE("spi_nor_erase err:[ret=%d]",ret);
		return OTA_ERR_spi_nor_erase;
	}

	//wirte relvant feedback info into flash
	strcpy(data_source_local + offset_char, feedback_info);

	ret = chip->ops->write( chip, offset, SPACE_FLAG_FEED_INFO,(uint8_t *)data_source_local);
	if(OTA_ERR_NO != ret){
		ALOGE("spi_nor_erase err:[ret=%d]",ret);
		return OTA_ERR_spi_nor_write;
	}

	return OTA_ERR_NO;	
}

static OTA_ERR_T Print_OTA_Feed_Info(const UINT32 offset,int offset_char,int length)
{
	char ota_info[256];
	int ret;

	ret = chip->ops->read( chip, offset + offset_char, length, (uint8_t *)ota_info);
	if(OTA_ERR_NO != ret){
		ALOGE("spi_nor_read err:[ret=%d]",ret);
		return OTA_ERR_spi_nor_read;
	}
	ALOGD("Func:%s,ota_info=[%s]",__func__,ota_info);
	return OTA_ERR_NO;
}




/*********************Modified Global Function --> Local Function *******************************/
static int Erase_Flash(const UINT32 erase_size, const UINT32 erase_unit,const UINT32 offset)
{
	int ret = 0;

	
	ALOGI("Func:%s,line:%d",__func__,__LINE__);
	ALOGD("Begin to erase, erase_size:[0x%x],erase_unit:[0x%x],offset:[0x%x]",erase_size,erase_unit,offset);
	if(erase_size % erase_unit == 0){
		ret = chip->ops->erase(chip,offset,erase_size);	//erase overall
		if(OTA_ERR_NO != ret){
			ALOGE("spi_nor_erase err:[ret=%d]",ret);
			return OTA_ERR_spi_nor_erase;
		}
	}
	else{	
		//The written file is not integer multiple of "block"(64K)
		#if 1
		UINT32 save_data_size = erase_unit-erase_size%erase_unit;
		UINT32 save_data_offset = erase_size/erase_unit*erase_unit+erase_size%erase_unit;
		UINT32 erase_size_modified = erase_size/erase_unit*erase_unit+erase_unit;
		ALOGD("Erase parmeter, save_data_size:[0x%x],save_data_offset:[0x%x],erase_size_modified:[0x%x]",
																save_data_size,save_data_offset,erase_size_modified);
		#else	//Shift to realize multiplication and division
		UINT32 save_data_size = erase_uint-erase_size&(erase_unit-1);
		UINT32 save_data_offset = erase_size | erase_uin
		
		#endif
		//save the orignal data of flash
		ret = chip->ops->read(chip, offset+save_data_offset, save_data_size,bak_buf);
		if(OTA_ERR_NO != ret){
			ALOGE("spi_nor_read err:[ret=%d]",ret);
			return OTA_ERR_spi_nor_read;
		}

		//Erase enough space to save sata in sdcard
		ret = chip->ops->erase(chip,offset,erase_size_modified);
		if(OTA_ERR_NO != ret){
			ALOGE("spi_nor_erase err:[ret=%d]",ret);
			return OTA_ERR_spi_nor_erase;
		}
	}
	ALOGD("Erase end!");
	return OTA_ERR_NO;
}


static int Write_File_From_Sd_To_Flash(const UINT32 write_size,const UINT32 offset)
{
	int temp = 0;
	unsigned int read_size;
	int ret;
	FRESULT res;
	int count_64k = 0;
	int count_1_10_update_file = 0;
	int count_temp = 0;
	int percent = 0;

	ALOGI("Func:%s,line:%d",__func__,__LINE__);
	//Firstly, open file in SD card and seek real location
	res = f_open(&handle, FILENAME, FA_READ);
	if(FR_OK != res){	//f_open FAIL				
		ALOGE("f_open err:%d", res);
		while(1);
	}	
	Jump_To_Real_Update_File(&handle,OTA_HEADER_LENGTH);

	//Count the write have ?? 64K
	count_64k = (write_size % DATA_BUF_SIZE == 0)?(write_size / DATA_BUF_SIZE) : (write_size / DATA_BUF_SIZE + 1);
	ALOGD("write_size:0x%x,count_64k:0x%x",write_size,count_64k);
	count_1_10_update_file = (count_64k % 10 == 0)?(count_64k / 10) : (count_64k / 10 + 1);
	ALOGD("count_1_10_update_file:0x%x",count_1_10_update_file);
	
	
	//copy file from sd card to psram
	ALOGD("Start wirte");
	for( temp = 0; temp < write_size / DATA_BUF_SIZE; temp++ ){
		ret = f_read(&handle, data_buf, DATA_BUF_SIZE, &read_size);
		if(FR_OK != ret){
			ALOGE("f_read err:[ret=%d]",ret);
			return OTA_ERR_f_read;
		}
		
		//write data from psram to flash
		ret = chip->ops->write( chip, offset+DATA_BUF_SIZE*temp, DATA_BUF_SIZE, (uint8_t *)data_buf);
		if(OTA_ERR_NO != ret){
			ALOGE("spi_nor_wirte err:[ret=%d]",ret);
			return OTA_ERR_spi_nor_write;
		}
		//When write 1/10 system.image, update LCD Progress Bar
		count_temp++;
		if(count_temp == count_1_10_update_file){
			percent += 10;
			ALOGD("percent1:%d",percent);
			OTA_LCD_ShowProcess(percent);
			count_temp = 0;
		}
		//Empty data_buf
		memset(data_buf,0,DATA_BUF_SIZE);			
	}
	
	//Determine the written file is an integer multiple of "block"(64K) or not
	if(write_size % DATA_BUF_SIZE != 0) {
		ret = f_read(&handle, data_buf, write_size%DATA_BUF_SIZE, &read_size);
		if(FR_OK != ret){	//f_read FAIL				
			ALOGE("f_read err:%d", ret);
			return OTA_ERR_f_read;
		}

		//restore the orignal data in flash
		memcpy(data_buf+write_size%DATA_BUF_SIZE,bak_buf,DATA_BUF_SIZE-write_size%DATA_BUF_SIZE);
		
		//write data from psram to flash
		ret = chip->ops->write( chip, offset+DATA_BUF_SIZE*temp, DATA_BUF_SIZE, (uint8_t *)data_buf);
		if(OTA_ERR_NO!= ret){	//f_read FAIL				
			ALOGE("f_read err:%d", ret);
			return OTA_ERR_spi_nor_write;
		}
		//Display the Progress Bar
		percent += 10;
		ALOGD("percent2:%d",percent);
		OTA_LCD_ShowProcess(percent);
		ALOGD("Write end!");

		//Empty data_buf
		memset(data_buf,0,DATA_BUF_SIZE);			
	}

	//Lastly,close the opened file in SD card
	res = f_close(&handle);
	if(FR_OK != res){	//f_close FAIL				
		ALOGE("f_close err:%d", res);
		while(1);
	}	
	return OTA_ERR_NO;
}





static int CRC_Check_Image_Sd_And_Flash(const UINT32 offset, const UINT32 file_size)
{
	int temp = 0;
	unsigned int read_size;
	int ret;
	UINT16 crc_reg_sd;
	UINT16 crc_reg_flash;
	UINT16 crc_reg = 0xFFFF;
	FRESULT res;

	ALOGI("Function:%s,CRC-16 Check:read data from SD and flash again!",__func__);

	//Firstly, open file in SD card and Jump to real location
	res = f_open(&handle, FILENAME, FA_READ);
	if(FR_OK != res){	//f_open FAIL				
		ALOGE("f_open err:%d", res);
		while(1);
	}	
	Jump_To_Real_Update_File(&handle,OTA_HEADER_LENGTH);

	for( temp = 0; temp < file_size / DATA_BUF_SIZE; temp++ ){		
		//read file from SD into psram
		ret = f_read(&handle, data_buf, DATA_BUF_SIZE, &read_size);
		if(FR_OK != ret){
			ALOGE("f_read err:[ret=%d]",ret);
			return OTA_ERR_f_read;
		}

		crc_reg = Crc_Check_16((UINT8 *)data_buf,DATA_BUF_SIZE,crc_reg);
	}

	if(file_size % DATA_BUF_SIZE != 0) {
		//read file from SD into psram
		ret = f_read(&handle, data_buf, file_size%DATA_BUF_SIZE, &read_size);
		if(FR_OK != ret){	//f_read FAIL				
			ALOGE("f_read err:%d", ret);
			return OTA_ERR_f_read;
		}

		crc_reg = Crc_Check_16((UINT8 *)data_buf,file_size%DATA_BUF_SIZE,crc_reg);
	}
	crc_reg_sd = crc_reg;
	crc_reg = 0xFFFF;
	
	//CRC check flash file
	ALOGD("crc check file written in flash");
	for( temp = 0; temp < file_size / DATA_BUF_SIZE; temp++ ){
		
		//read data from flash to psram
		//uart_printf("Function:%s,Begin to read data from flash into psram\r\n",__func__);
		ret = chip->ops->read( chip, offset+temp*DATA_BUF_SIZE, DATA_BUF_SIZE, (uint8_t *)data_buf);
		if(OTA_ERR_NO != ret){
			ALOGE("spi_nor_read err:[ret=%d]",ret);
			return OTA_ERR_spi_nor_write;
		}
		crc_reg = Crc_Check_16((UINT8 *)data_buf,DATA_BUF_SIZE,crc_reg);
	}

	if(file_size % DATA_BUF_SIZE != 0) {
		//read file from flash into psram
		ret = chip->ops->read( chip, offset+temp*DATA_BUF_SIZE, file_size%DATA_BUF_SIZE, (uint8_t *)data_buf);
		if(OTA_ERR_NO != ret){
			ALOGE("spi_nor_read err:[ret=%d]",ret);
			return OTA_ERR_spi_nor_write;
		}

		crc_reg = Crc_Check_16((UINT8 *)data_buf,file_size%DATA_BUF_SIZE,crc_reg);
	}
	crc_reg_flash = crc_reg;
	crc_reg = 0xFFFF;	
		
	if(crc_reg_sd != crc_reg_flash){
		ALOGE("[CRC_Check_SD=0x%x],[CRC_Check_flash=0x%x]",crc_reg_sd,crc_reg_flash);
		return OTA_ERR_CRC_Check;
	}	
	
	//Empty data_buf
	memset(data_buf,0,DATA_BUF_SIZE);			

	//Lastly,close the opened file in SD card
	res = f_close(&handle);
	if(FR_OK != res){	//f_close FAIL				
		ALOGE("f_close err:%d", res);
		while(1);
	}
	
	ALOGD("CRC-16 Check OK! [crc_reg_sd=0x%x],[crc_reg_flash=0x%x]",crc_reg_sd,crc_reg_flash);
	return OTA_ERR_NO;
}



static OTA_ERR_T Read_OTA_Flag(const UINT32 offset,char *ota_flag_sdcard)
{
	int ret;
	int index;
	UINT32 fota_param_offset;
	
	ALOGI("Func:%s,line:%d",__func__,__LINE__);

	//get fota_param offset
	fota_param_offset = get_fota_param_offset();
	
	ret = chip->ops->read( chip, (int)fota_param_offset, OTA_FLAG_LENGTH * sizeof(UINT8), (uint8_t *)ota_flag_sdcard);
	if(OTA_ERR_NO != ret){
		ALOGE("spi_nor_read err:[ret=%d]",ret);
		return OTA_ERR_spi_nor_read;
	}
	ALOGD("Current OTA_Flag is:");
	for(index =0; index < OTA_FLAG_LENGTH; index++){
		if(OTA_FLAG_SDCARD_NOT_UPDATE == ota_flag_sdcard[index]){	//If ota_flag is 0xff, need printf as format %x
			uart_printf("ota_flag[%d]=[0x%x]  ",index,ota_flag_sdcard[index]);
			if(index == OTA_FLAG_LENGTH/2)
				uart_printf("\r\n");
		}else
			uart_printf("%c",ota_flag_sdcard[index]);
	}
	uart_printf("\r\n");
	return OTA_ERR_NO;
}


static void Jump_To_Real_Update_File(FIL *fp, FSIZE_t len)
{
	FRESULT res;

	//f_lseek
	res = f_lseek(&handle,len);
	if(FR_OK != res){	//f_lseek FAIL				
		ALOGE("f_lseek err:%d", res);
		while(1);
	}
}

static OTA_ERR_T Reset_OTA_Flag(const UINT32 offset)
{
	int ret;
	UINT8 data_source[SPACE_FLAG_FEED_INFO];
	INT8 ota_flag_sdcard[OTA_FLAG_LENGTH];
	int index;

	ALOGI("Func:%s,line:%d",__func__,__LINE__);
	
	//before resetting OTA_Flag, read the source OTA_Flag
	ret = chip->ops->read( chip, offset, OTA_FLAG_LENGTH*sizeof(UINT8), (uint8_t *)ota_flag_sdcard);
	if(OTA_ERR_NO != ret){
		ALOGE("spi_nor_read err:[ret=%d]",ret);
		return OTA_ERR_spi_nor_write;
	}
	ALOGD("Before resetting,ota_flag_sdcard=[%s]",ota_flag_sdcard);

	//before erase, need to save source data because size of OTA_Flag_SDCARD is only 9 Bytes
	ret = chip->ops->read(chip, offset, SPACE_FLAG_FEED_INFO,(uint8_t *)data_source);
	if(OTA_ERR_NO != ret){
		ALOGE("spi_nor_read err:[ret=%d]",ret);
		return OTA_ERR_spi_nor_read;
	}

	//Before writing flash, must erase flash
	ret = chip->ops->erase(chip,offset,SPACE_FLAG_FEED_INFO);
	if(OTA_ERR_NO != ret){
		ALOGE("spi_nor_read err:[ret=%d]",ret);
		return OTA_ERR_spi_nor_erase;
	}

#if 0 //update OTA_Flag for CP Power_up_reason fetch
	//Reset OTA_FLAG_SDCARD as: 0xFF (0 - 8 Bytes)
	for(index = 0; index < OTA_FLAG_LENGTH; index++)
		data_source[index] = OTA_FLAG_SDCARD_NOT_UPDATE;
#else
	strncpy(data_source,"OTA_PASS",8);
#endif
	
	ret = chip->ops->write(chip,offset,SPACE_FLAG_FEED_INFO,(uint8_t *)data_source);
	if(OTA_ERR_NO != ret){
		ALOGE("spi_nor_read err:[ret=%d]",ret);
		return OTA_ERR_spi_nor_erase;
	}

	return OTA_ERR_NO;	
}

static OTA_ERR_T Get_Update_File_Version(char *p_update_file_version)
{
	FRESULT res;
	INT32 ret;
	UINT32 read_size;

	res = f_open(&handle,FILENAME,FA_READ);
	if(FR_OK != res){	//f_open FAIL				
		ALOGE("f_open err:%d", res);
		return OTA_ERR_f_open;
	}
	//print file size stored in SD card
	ALOGD("func:%s,line:%d,[%s] size is:0x%x",__func__,__LINE__,FILENAME,f_size(&handle));

	//read version info from SD's package
	ret = f_read(&handle,p_update_file_version,OTA_VERSION_LENGTH,&read_size);
	if(FR_OK != ret){	//f_read FAIL
		ALOGE("f_read err:[ret=%d]",ret);
		return OTA_ERR_f_read;
	}
	ALOGD("Read ok and Read_size = 0x%x",read_size);
	ALOGD("[Updater file version:%s]",p_update_file_version);

	return OTA_ERR_NO;
}
