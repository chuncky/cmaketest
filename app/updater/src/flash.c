/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
//#include "aos/hal/flash.h"
#include "flash.h"
#include "2ndboot.h"
#include "qspi_flash.h"		//porting AliOS_OTA
#include "loadtable.h"

/*+++++++++++++++
MACRO:          DEBUG_OPEN_FLAH
DESCRIPTION:    If define the above macro, will print uart_log in flash api
Author:         Hengshanyang
Data:           2019-10-25
Default:        Comment this macro
+++++++++++++++*/
//#define DEBUG_OPEN_FLAH

extern unsigned char* memory_base;
extern hal_logic_partition_t hal_partitions[];
extern const size_t hal_partitions_amount;

extern struct spi_flash_chip *chip;
extern unsigned char qspi_wr_buf[1024*64];
extern unsigned char qspi_rd_buf[1024*64];

//#include "hal_flash.h"
unsigned int ali_spi_nor_erase(unsigned int addr, unsigned int size)
{
    int ret;

	if(chip == NULL)
		return -1;

	ret = chip->ops->erase(chip, addr, size);
	if(ret) {
		uart_printf("[func:%s],[line:%d],flash erase error!\r\n",__func__,__LINE__);
		return -1;
	}

	return 0;
}

#define ALI_READ_BLOCK_SIZE (0x00000400) //1024 
#define ALI_READ_BLOCK_MASK (ALI_READ_BLOCK_SIZE-0x1) //1024 - 1
unsigned char ali_blockread_buf[ALI_READ_BLOCK_SIZE];

unsigned int ali_spi_nor_do_read(unsigned int addr, unsigned int buf_addr, unsigned int size)
{
    int ret;
	int block_addr;
	int block_offset;
	int block_size;
	unsigned char *buf;
	//uart_printf("%s-01,addr= 0x%x,size=0x%x\r\n",__func__,addr,size);
	
	if((chip == NULL) || (buf_addr == NULL))
		return -1;

	if((addr + size) > chip->size)
		return -1;

#if 1
		block_addr	 = addr &(~ALI_READ_BLOCK_MASK);
		block_offset = addr &( ALI_READ_BLOCK_MASK);
		block_size	 = size;
		while(block_size)
		{	
			ret = chip->ops->read(chip, block_addr, ALI_READ_BLOCK_SIZE, (uint8_t *)ali_blockread_buf);
			if(ret) return -1;	
			if((block_offset + block_size) < ALI_READ_BLOCK_SIZE)
			{	
				memcpy( (uint8_t *)buf_addr,(uint8_t *)ali_blockread_buf + block_offset, block_size);
				return 0;
			}
			else
			{
				memcpy((uint8_t *)buf_addr,(uint8_t *)ali_blockread_buf + block_offset, ALI_READ_BLOCK_SIZE - block_offset);
	
				block_addr += ALI_READ_BLOCK_SIZE;
				buf_addr += (ALI_READ_BLOCK_SIZE - block_offset);		
				block_size -= (ALI_READ_BLOCK_SIZE - block_offset);
				block_offset = 0;
				addr = block_addr;
			}
			//rs_trace("%s-03,block_size= 0x%x\r\n",__func__,block_size);
	
		}
	
#else
		ret = chip->ops->read(chip, addr, size, (uint8_t *)buf_addr);
		if(ret) return -1;
	
#endif
	return 0;
}

unsigned int ali_spi_nor_do_write(unsigned int addr, unsigned int buf_addr, unsigned int size)
{
    int ret;
	int block_addr;
	int block_offset;
	int block_size;
	
	if((chip == NULL) || (buf_addr == NULL))
		return -1;

	if((addr + size) > chip->size){
		return -1;
	}

	ret = chip->ops->write(chip, addr, size, (uint8_t *)buf_addr);
	if(ret) {
		uart_printf("[func:%s],[line:%d],flash write error!\r\n",__func__,__LINE__);
		return -1;
	}


	return 0;
}




char ali_fota_magic_buffer[4096];
void ali_clear_fota_magic()
{
	char FotaMagic[32] = {0};

	ali_spi_nor_do_read(get_fota_param_start_addr() - FLASH_BASE_ADDR, ali_fota_magic_buffer, 4096);
	ali_spi_nor_erase(get_fota_param_start_addr() - FLASH_BASE_ADDR, 4096);
	memcpy(ali_fota_magic_buffer, FotaMagic, sizeof(FotaMagic));
	ali_spi_nor_do_write(get_fota_param_start_addr() - FLASH_BASE_ADDR, ali_fota_magic_buffer, 4096);
}



/*
* Get the information of the specified flash area
* @param[in] pno: the target flash logical partition "ID"
*@return      ha_logic_partition pointer
*/

hal_logic_partition_t *flash_get_info(hal_partition_t pno)
{
	OTA_LOG_I("pno:%d",pno);

	if(pno >= hal_partitions_amount)
    {
        return NULL;
    }

	
    return &hal_partitions[pno];
}


int flash_read_data(unsigned int addr, unsigned char *buf, unsigned int len)
{
    int ret = 0;
	
	if(buf == NULL){
        return -1;
    }
#ifdef DEBUG_OPEN_FLAH
	OTA_LOG_I("addr:0x%x,len:0x%x",addr,len);
#endif
	
	ret = ali_spi_nor_do_read(addr-0x80000000, (unsigned int)buf, len);
	if(ret < 0)
		OTA_LOG_I("flash_read_data error, ret = %d",ret);
#ifdef DEBUG_OPEN_FLAH
	uart_printf("flash_read_data.ret:0x%x\r\n",ret);
#endif
    return ret;
}

int flash_write_data(unsigned int addr, unsigned char *buf, unsigned int len)
{
    int ret = 0;
	
    if(buf == NULL){
        return -1;
    }
#ifdef DEBUG_OPEN_FLAH
	OTA_LOG_I("addr:0x%x,size:0x%x",addr,len);
#endif
	ret =  ali_spi_nor_do_write(addr-0x80000000, (unsigned int)buf, len);
	if(ret < 0)
		OTA_LOG_I("flash_write_data error, ret = %d",ret);
    return ret;
}

int flash_erase(unsigned int addr,  unsigned int size)
{
    int ret = 0;

	//modify size --> 4K align
	if(size % 0x1000 !=0){
		size = size/(0x1000)* 0x1000+0x1000;
	}
#ifdef DEBUG_OPEN_FLAH
	OTA_LOG_I("addr:0x%x,size:0x%x",addr,size);
#endif
	ret = ali_spi_nor_erase(addr-0x80000000,  size);
	if(ret < 0)
		OTA_LOG_I("flash_erase error, ret = %d",ret);
    return ret;
}

extern void OTA_LCD_ShowProcess(int per);

int ota_patch_show_percent(int per)
{
   int ret = 0;
   OTA_LOG_I(" show upgrade percent:%d \n", per);

   //+++++++++++++++++++++++++++++++++++++++++++
   //Add LCD Info here
	#ifdef LCD_USE_IN_CODE
	   //LCD show update progress
	   static int last_current = 0;
	   static int clear_lcd = 0;
	   //[1] Before show progress, clear the LCD info. Note: only do it once
	   if(clear_lcd == 0){
		   //clear lcd
		   OTA_Clear_LCD();
		   clear_lcd = 1;
	   }
	   //[2] show progress bar
	   if((per != last_current) || (per == 0)){
		   OTA_LCD_ShowProcess(per);
		   last_current = per;
		   /* delay*/
		   if(per == 100){
			   unsigned int delay=10000;
			   while(delay)
				   delay--;
		   }
	   }
	#endif
   //+++++++++++++++++++++++++++++++++++++++++++

   return ret;
}
