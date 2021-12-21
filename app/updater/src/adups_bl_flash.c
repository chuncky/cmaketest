#include <string.h>
#include "adups_bl_flash.h"
#include "adups_typedef.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "common.h"
#include "qspi_flash.h"
#include "loadtable.h"
#include "adups_bl_main.h"
#include "types.h"
#include "ff.h"
#include "asr_nor_flash.h"

#define FOTA_UPDATE_FILENAME "D:/fotaup.bin"
#define FOTA_BACKUP_FILENAME "D:/fotabk.bin"

extern struct spi_flash_chip *chip;
extern unsigned char qspi_wr_buf[1024*64];
extern unsigned char qspi_rd_buf[1024*64];

//#include "bl_wdt.h"
#define KB(n)  (n * 1024)
#define MB(n)  (n * 1024 * 1024)
#define DELATA_BLOCK_NUM  15  // (1024 - 64)KB / 64KB

#define DELTA_SIZE_IN_PSRAM  (MB(3)+KB(512))
#define DELTA_SIZE_IN_SD     (KB(10240))   //10M
#define DELTA_SIZE           (KB(1024 - 64))
#define LOG_DEBUG             NULL

unsigned char datastorein = 0;

adups_uint32 adups_bl_flash_block_size(void)
{
	return FLASH_BLOCK_SIZE;
}

ADUPS_BOOL AdupsUseFlash(void)
{
	return ADUPS_TRUE;
}

adups_uint32 fota_back_addr   = 0;
adups_uint32 fota_delta_start = 0;
#define FOTA_BACKUP_BASE_OFFSET (MB(1) - KB(64))

adups_uint32 adups_bl_flash_backup_base(void)
{
	if(datastorein==2)
		return 0;
	if(fota_back_addr == 0){
		if(datastorein==1)
			fota_back_addr = get_mmipool_start_address() + DELTA_SIZE_IN_PSRAM;
		else
			fota_back_addr = get_fota_pkg_end_addr() - adups_bl_flash_block_size();
		
		adups_bl_debug_print(LOG_DEBUG, "adups_bl_flash_backup_base : 0x%x\r\n", fota_back_addr);
	}
	
	return fota_back_addr;
	
}

adups_uint32 adups_bl_flash_delta_base(void)
{

	if(datastorein==2)
		return 0;

	if(fota_delta_start == 0){
		if(datastorein==1)
			fota_delta_start = get_mmipool_start_address();
		else
			fota_delta_start = get_fota_pkg_start_addr();
		
		adups_bl_debug_print(LOG_DEBUG, "adups_bl_flash_delta_base : 0x%x\r\n", fota_delta_start);
	}

	return fota_delta_start;

}

adups_uint32 adups_bl_flash_delta_size(void)
{
	adups_uint32 delta_size;
	if(datastorein==1)
		delta_size = DELTA_SIZE_IN_PSRAM;
	else if(datastorein==2)
		delta_size=DELTA_SIZE_IN_SD;
	else
		delta_size = (get_fota_pkg_size() - adups_bl_flash_block_size());
	
	adups_bl_debug_print(LOG_DEBUG, "delta_size : 0x%x\r\n", delta_size);
	return delta_size;

}


adups_uint32 AdupsGetFlashDiskSize(void)
{
	return adups_bl_flash_delta_size();
}


adups_int32 adups_bl_read_block(adups_uint8* dest, adups_uint32 start, adups_uint32 size)
{
    int ret;
	//adups_bl_debug_print(LOG_DEBUG, "adups_bl_read_block : 0x%x, 0x%x\r\n", 
	//	start, size);
	ret=asr_norflash_read(start,dest,size);
	return 0;		
}

adups_int32 adups_bl_write_block(adups_uint8* src, adups_uint32 start, adups_uint32 size)
{
    int ret;
	int erase_size;
	adups_bl_debug_print(LOG_DEBUG, "adups_bl_write_block : 0x%x, 0x%x\r\n", 
		start, size);
	erase_size=(size+FLASH_SECTOR_MASK)&(~FLASH_SECTOR_MASK);
	ret=asr_norflash_erase(start, erase_size);
	if(ret) return -1;
	ret=asr_norflash_write(start, src, size);
	if(ret) return -1;
	return size;
}

adups_int32 adups_bl_erase_block(adups_uint32 addr)
{
    int ret = -1;
	if((addr&(~FLASH_BLOCK_MASK))!=addr)
		return -1;
	adups_bl_debug_print(LOG_DEBUG, "adups_bl_erase_block : 0x%x\r\n", 
		addr);
	ret=asr_norflash_erase(addr, FLASH_BLOCK_SIZE);
	if(ret) ret = -1;
	return ret;
}

adups_int32 sddata_read(adups_uint32 offset, adups_uint8* dest, adups_uint32 size);
adups_uint32 sddata_erase_delata(void);
void sddata_erase_backup_region(void);
adups_int32 sddata_write_backup_region(adups_uint8* data_ptr, adups_uint32 len);
adups_int32  sddata_read_backup_region(adups_uint8* data_ptr, adups_uint32 len);


adups_int32 adups_bl_read_flash(adups_uint32 offset, adups_uint8* buf_addr, adups_uint32 size)
{
	int pFotaDelta;
	int ret;
	pFotaDelta=adups_bl_flash_delta_base()+offset;

	if(datastorein==1)
	{
		adups_bl_debug_print(LOG_DEBUG, "%s : addr=0x%x,size=0x%x\r\n",__func__, pFotaDelta,size);
		memcpy(buf_addr,pFotaDelta,size);
		return 0;
	}
	else if(datastorein==2)
	{
		ret=sddata_read(pFotaDelta,buf_addr,size);
		if(ret) ret = -1;
		return ret;
	}
	else
	{
		//adups_bl_debug_print(LOG_DEBUG, "%s : addr=0x%x,size=0x%x\r\n",__func__, addr,size);
		ret=asr_norflash_read(pFotaDelta,(uint8_t *)buf_addr,size);
		if(ret) ret = -1;
		return ret;

	}
}

adups_uint32 adups_bl_erase_delata(void)
{	
	if(datastorein==1)
	{
		adups_bl_debug_print(LOG_DEBUG, "%s \r\n",__func__);

		return 0;	
	}
	else if(datastorein==2)
	{
		sddata_erase_delata();
		return 0;
	}
	else
	{
	    int ret = -1;
		int block_addr;
		int delata_size;
	
		block_addr = adups_bl_flash_delta_base();
		delata_size=adups_bl_flash_delta_size();
		adups_bl_debug_print(LOG_DEBUG, "adups_bl_erase_delata : 0x%x, 0x%x\r\n", 
			block_addr, delata_size);

		if(block_addr!=(block_addr&(~FLASH_BLOCK_MASK)))
			return -1;
		ret = asr_norflash_erase(block_addr, delata_size);
		if(ret) ret = -1;

		return ret;
	}

}


void adups_bl_erase_backup_region(void)
{
	if(datastorein==1)
	{
		int pFotaBackup;
		pFotaBackup=adups_bl_flash_backup_base();
		adups_bl_debug_print(LOG_DEBUG, "%s \r\n",__func__);
		memset(pFotaBackup,0xff,adups_bl_flash_block_size());
		return 0;		
	}
	else if(datastorein==2)
	{
		sddata_erase_backup_region();
		return 0;
	}
	else
	{
	    int ret = -1;
		int block_addr;

		if(chip == NULL)
			return ret;
		
		block_addr = adups_bl_flash_backup_base();
		adups_bl_debug_print(LOG_DEBUG, "adups_bl_erase_backup_region : 0x%x\r\n", 
			block_addr);
		
		if(block_addr!=(block_addr&(~FLASH_BLOCK_MASK)))
			return -1;
		ret = asr_norflash_erase(block_addr, FLASH_BLOCK_SIZE);
		if(ret) ret = -1;

		return ret;
	}
}

adups_int32 adups_bl_write_backup_region(adups_uint8* data_ptr, adups_uint32 len)
{		
	int pFotaBackup;
	int ret=0;
	
	pFotaBackup=adups_bl_flash_backup_base();

	if(datastorein==1)
	{
		adups_bl_debug_print(LOG_DEBUG, "%s : addr=0x%x,size=0x%x\r\n",__func__, pFotaBackup,len);
		memcpy(pFotaBackup,data_ptr,len);

		return 0;
	}
	else if(datastorein==2)
	{
		ret=sddata_write_backup_region(data_ptr,len);
		if(ret) ret = -1;
		return ret;

	}
	else
	{
		if((chip == NULL) || (len > FLASH_BLOCK_SIZE))
			return -1;
		adups_bl_debug_print(LOG_DEBUG, "%s : addr=0x%x,size=0x%x\r\n",__func__, pFotaBackup,len);
		if(pFotaBackup!=(pFotaBackup&(~FLASH_BLOCK_MASK)))
			return -1;
		ret = asr_norflash_write(pFotaBackup,data_ptr,FLASH_BLOCK_SIZE);
		if(ret) ret = -1;

		return 0;
	}
}

adups_int32  adups_bl_read_backup_region(adups_uint8* data_ptr, adups_uint32 len)
{
	int ret;

	if(datastorein==1)
	{
		int pFotaBackup;
		
		pFotaBackup=adups_bl_flash_backup_base();
		adups_bl_debug_print(LOG_DEBUG, "%s : addr=0x%x,size=0x%x\r\n",__func__, pFotaBackup,len);
		memcpy(data_ptr,pFotaBackup,len);
		
		return 0;
	}
	else if(datastorein==2)
	{
		ret=sddata_read_backup_region(data_ptr,len);
		if(ret) return -1;
		return 0;

	}
	else
	{
	    int ret;
		int block_addr;
		block_addr = adups_bl_flash_backup_base();
		//adups_bl_debug_print(LOG_DEBUG, "%s : addr=0x%x,size=0x%x\r\n",__func__, block_addr,len);

		if(block_addr!=(block_addr&(~FLASH_BLOCK_MASK)))
			return -1;
		ret = asr_norflash_read(block_addr,(uint8_t *)data_ptr, len);
		if(ret) return -1;
		return 0;
	}
}


FIL filehandle;
FIL deltafilehandle;

adups_int32 sddata_read(adups_uint32 offset, adups_uint8* dest, adups_uint32 size)
{
	FRESULT ret;
	unsigned int read_size;
	ret=f_open(&deltafilehandle,FOTA_UPDATE_FILENAME,FA_READ);
	if(ret){
		adups_bl_debug_print(LOG_DEBUG,"function:%s,update file open fail.\r\n",__func__);
		return -1;
	}
	//adups_bl_debug_print(LOG_DEBUG,"function:%s,offset=0x%x,size=0x%x\r\n",__func__,offset,size);
	
	ret= f_lseek(&deltafilehandle,offset);
	if(ret){
		adups_bl_debug_print(LOG_DEBUG,"function:%s,update file lseek error.ret=%d\r\n",__func__,ret);
		return -1;
	}
	ret= f_read(&deltafilehandle,dest,size,&read_size);
	if(ret){
		adups_bl_debug_print(LOG_DEBUG,"update file read error: %d\r\n",ret);
		return -1;
	}
	//close file
	f_close(&deltafilehandle);

	//adups_bl_debug_print(LOG_DEBUG,"function:%s,size=0x%x,read_size=0x%x\r\n",__func__,size,read_size);

	if(size == read_size)
		return 0;
	//adups_bl_debug_print(LOG_DEBUG,"function:%s,size=0x%x,read_size=0x%x\r\n",__func__,size,read_size);
	return -1;	

	
}

adups_uint32 sddata_erase_delata(void)
{
	f_unlink(FOTA_UPDATE_FILENAME);
	//adups_bl_debug_print(LOG_DEBUG,"function:%s,remove the update file.\r\n",__func__);
	return 0;
}


void sddata_erase_backup_region(void)
{
	f_unlink(FOTA_BACKUP_FILENAME);
	//adups_bl_debug_print(LOG_DEBUG,"function:%s,remove the bakeup file.\r\n",__func__);
	return 0;

}

adups_int32 sddata_write_backup_region(adups_uint8* data_ptr, adups_uint32 len)
{
	FRESULT ret;
	adups_uint32 write_size;
	//open update file
	ret=f_open(&filehandle,FOTA_BACKUP_FILENAME,(FA_WRITE | FA_OPEN_ALWAYS) );
	if(ret){
		adups_bl_debug_print(LOG_DEBUG,"function:%s,backup file open fail.\r\n",__func__);
		return -1;
	}
	ret= f_write(&filehandle,data_ptr,len,&write_size);
	if(ret){
		adups_bl_debug_print(LOG_DEBUG,"function:%s,backup file write error: %d\r\n",__func__,ret);
	}
	//close file
	f_close(&filehandle);
	//adups_bl_debug_print(LOG_DEBUG,"function:%s,len=0x%x,write_size=0x%x\r\n",__func__,len,write_size);
	if(len == write_size)
		return 0;
	return ret;	

}

adups_int32  sddata_read_backup_region(adups_uint8* data_ptr, adups_uint32 len)
{
    FRESULT ret;
	unsigned int read_size;

	//open update file
	ret=f_open(&filehandle,FOTA_BACKUP_FILENAME,(FA_READ | FA_OPEN_ALWAYS));
	if(ret){
		adups_bl_debug_print(LOG_DEBUG,"function:%s,backup file open fail.\r\n",__func__);
		return -1;
	}
	ret= f_read(&filehandle,data_ptr,len,&read_size);
	if(ret){
		adups_bl_debug_print(LOG_DEBUG,"backup file read error: %d\r\n",ret);
		return -1;
	}
	//close file
	f_close(&filehandle);
	//adups_bl_debug_print(LOG_DEBUG,"function:%s,len=0x%x,read_size=0x%x\r\n",__func__,len,read_size);
	if(len == read_size)
		return 0;
	return -1;	

}


