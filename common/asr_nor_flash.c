#include <string.h>
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "common.h"
#include "qspi_flash.h"
#include "loadtable.h"
#include "ssp_host.h"
#include "spi_nor.h"
#include "asr_nor_flash.h"
#include "Errors.h"

extern struct spi_flash_chip *chip;
extern struct spi_flash_chip *chip2;

unsigned char spi_sector_buf[FLASH_SECTOR_SIZE];
//extern unsigned char qspi_rd_buf[1024*64];
#define READ_BLOCK_SIZE (0x00001000) 	//4096
#define READ_BLOCK_MASK (READ_BLOCK_SIZE-0x1) //4096 - 1

unsigned char spi_rd_buf[READ_BLOCK_SIZE];
unsigned char spi_wr_buf[READ_BLOCK_SIZE];

#define NORFLASH_PAGESIZE (0x00000100) //256
#define NORFLASH_PAGE_MASK (NORFLASH_PAGESIZE-0x1) //256 - 1



int asr_norflash_erase_4k(unsigned int addr, unsigned int size)
{
	int ret;
	unsigned int lenth;

#ifndef FOTA_ASRSPI 
	//uart_printf("%s : 0x%x, 0x%x\r\n", __func__,addr, size);
	if(addr>=SPINOR_BASEADDR){
		addr-=SPINOR_BASEADDR;
		ret=asr3601s_spinor_do_erase_4k(addr,size);
	}
	else if(addr>=FLASH_BASE_ADDR){
		addr-=FLASH_BASE_ADDR;
		if(addr>=chip->size){
			if((addr+size)>(chip->size+chip2->size)){
				uart_printf("%s-03,0x%x,0x%x\r\n",__func__, chip->size,chip2->size);
				return -1;
			}
			ret = chip2->ops->erase(chip2, (addr-chip->size), size);
		}
		else if((addr+size)<=chip->size)
		{
			ret = chip->ops->erase(chip, addr, size);
		}
		else
		{
			lenth=(chip->size-addr);
			ret = chip->ops->erase(chip, addr, lenth);
			if (ret!=0)
				return -1;
			size-=lenth;
			ret = chip2->ops->erase(chip2, 0, size);
		}
	}
#else
	addr-=FLASH_BASE_ADDR;
	if((addr <0)||((addr+size)>chip->size)){
		CP_LOGE("%s: addr size error,addr=0x%x,size=0x%x\r\n", __func__,addr,size);
		return -1;
	}
	ret = chip->ops->erase(chip,addr, size);
	if(ret) return -1;	
#endif
	return ret;
}

int asr_norflash_eraseblock(unsigned int addr, unsigned int size)
{
	int ret;
	unsigned int lenth;
#ifndef FOTA_ASRSPI 
	//uart_printf("%s : 0x%x, 0x%x\r\n", __func__,addr, size);
	if(addr>=SPINOR_BASEADDR){
		addr-=SPINOR_BASEADDR;
		ret=asr3601s_spinor_do_erase_64k(addr,size);
	}
	else if(addr>=FLASH_BASE_ADDR){
		addr-=FLASH_BASE_ADDR;
		if(addr>=chip->size){
			if((addr+size)>(chip->size+chip2->size)){
				return -1;
			}
			ret = chip2->ops->erase(chip2, (addr-chip->size), size);
		}
		else if((addr+size)<=chip->size)
		{
			ret = chip->ops->erase(chip, addr, size);
		}
		else
		{
			lenth=(chip->size-addr);
			ret = chip->ops->erase(chip, addr, lenth);
			if (ret!=0)
				return -1;
			size-=lenth;
			ret = chip2->ops->erase(chip2, 0, size);
		}
	}
#else
	addr-=FLASH_BASE_ADDR;
	if((addr <0)||((addr+size)>chip->size)){
		CP_LOGE("%s: addr size error,addr=0x%x,size=0x%x\r\n", __func__,addr,size);
		return -1;
	}
	ret = chip->ops->erase(chip,addr, size);
	if(ret) return -1;	
#endif
	return ret;
}

int asr_norflash_erase(unsigned int addr, unsigned int len)
{
	int ret;
	unsigned int erase_len;
	//CP_LOGI("%s : 0x%x, 0x%x\r\n", __func__,addr, len);
	
	if((chip == NULL) || (addr < FLASH_BASE_ADDR)){
		CP_LOGE("%s: error,addr=0x%x,size=0x%x\r\n", __func__,addr,len);
		return -1;	
	}
	/* check address align on block boundary */
	if (addr & FLASH_SECTOR_MASK) {
		return -1;
	}
	if (len & FLASH_SECTOR_MASK) {
		CP_LOGE("%s: Length not 4K-Sector aligned\r\n", __func__);
		return -1;
	}
	while (len > 0) {

		if (len >= FLASH_BLOCK_SIZE &&
		    !(addr & (FLASH_BLOCK_SIZE - 1))) {
			erase_len = FLASH_BLOCK_SIZE;
			ret=asr_norflash_eraseblock(addr,FLASH_BLOCK_SIZE);
		} else {
			erase_len = 4*1024;
			ret=asr_norflash_erase_4k(addr, erase_len);
		}

		if (ret != 0) {
			CP_LOGE("%s,block erase fail,ret=0x%x \r\n",__func__,ret);
			return ret;
		}

		/* Increment page address and decrement length */
		len -= erase_len;
		addr += erase_len;

	}
	return 0;
}

int asr_norflash_readpage(uint32_t addr, uint8_t* buf_addr, uint32_t size)
{
	int ret;
#ifndef FOTA_ASRSPI 
	//CP_LOGD("%s : addr:0x%x,buf_addr:0x%x,size: 0x%x\r\n", __func__,addr,buf_addr, size);
	if(addr>=SPINOR_BASEADDR){
		addr-=SPINOR_BASEADDR;
		asr3601s_spinor_do_read(addr,buf_addr,size);
	}
	else if(addr>=FLASH_BASE_ADDR){
		addr-=FLASH_BASE_ADDR;
		if((addr <0)||((addr+size)>chip->size)){
			CP_LOGE("%s: addr size error,addr=0x%x,size=0x%x\r\n", __func__,addr,size);
			return -1;
		}
		ret = chip->ops->read(chip,addr, size, (uint8_t *)buf_addr);
		if(ret) return -1;	
	}
#else
	addr-=FLASH_BASE_ADDR;
	ret = chip->ops->read(chip,addr, size, (uint8_t *)buf_addr);
	if(ret) return -1;	
#endif
	return 0;
}

int asr_norflash_writepage(uint32_t addr,uint8_t* buf_addr, uint32_t size)
{
	int ret;
	unsigned int lenth;
#ifndef FOTA_ASRSPI 
	//CP_LOGD("%s : 0x%x, 0x%x\r\n", __func__,addr, size);
	if(addr>=SPINOR_BASEADDR){
		addr-=SPINOR_BASEADDR;
		ret=asr3601s_spinor_do_write(addr,buf_addr,size);
	}
	else if(addr>=FLASH_BASE_ADDR){
		addr-=FLASH_BASE_ADDR;
		if(addr>=chip->size){		
			if((addr+size)>(chip->size+chip2->size)){
				CP_LOGE("%s: addr size error,addr=0x%x,size=0x%x\r\n", __func__,addr,size);
				return -1;
			}	
			ret = chip2->ops->write(chip2, (addr-chip->size), size, (uint8_t *)buf_addr);
		}
		else if((addr+size)<=chip->size)
		{	
			ret = chip->ops->write(chip, addr, size, (uint8_t *)buf_addr);
		}
		else
		{
			lenth=(chip->size-addr);
			ret = chip->ops->write(chip, addr, lenth, (uint8_t *)buf_addr);
			if (ret!=0)
				return -1;
			size-=lenth;
			buf_addr+=lenth;
			ret = chip2->ops->write(chip2, 0, size, (uint8_t *)buf_addr);
		}
	}
#else
	addr-=FLASH_BASE_ADDR;
	ret = chip->ops->write(chip, addr, size, (uint8_t *)buf_addr);
	if(ret) return -1;	
#endif
	return ret;
}

int qspi_flash_read(uint32_t addr, uint8_t* buf_addr, uint32_t size)
{
	int ret;
	unsigned int lenth;

	addr-=FLASH_BASE_ADDR;
	//CP_LOGI("%s : addr=0x%x,buf_addr=0x%x,size=0x%x\r\n",__func__, addr,buf_addr,size);
	
	if(addr>=chip->size){
		if((addr+size)>(chip->size+chip2->size)){
			CP_LOGE("%s: addr size error,addr=0x%x,size=0x%x\r\n", __func__,addr,size);
			return -1;
		}
		ret = chip2->ops->read(chip2, (addr-chip->size), size, (uint8_t *)buf_addr);
	}
	else if((addr+size)<=chip->size)
	{
		ret = chip->ops->read(chip, addr, size, (uint8_t *)buf_addr);
	}
	else
	{
		lenth=(chip->size-addr);
		ret = chip->ops->read(chip, addr, lenth, (uint8_t *)buf_addr);
		if (ret!=0)
			return -1;
		size-=lenth;
		buf_addr+=lenth;
		ret = chip2->ops->read(chip2, 0, size, (uint8_t *)buf_addr);
	}
	return ret;
}

int asr_norflash_read(uint32_t addr, uint8_t* buf_addr, uint32_t size)
{
	int ret;	
	int block_addr;
	int block_offset;
	int block_size;
	uint8_t *buf;	
	//CP_LOGI("%s : addr=0x%x,buf_addr=0x%x,size=0x%x\r\n",__func__, addr,buf_addr,size);
	if((chip == NULL) || (buf_addr == NULL) || (addr < FLASH_BASE_ADDR))
	{
		CP_LOGE("%s: error,addr=0x%x,size=0x%x\r\n", __func__,addr,size);
		return -1;
	}
	if(addr<SPINOR_BASEADDR){
		ret=qspi_flash_read(addr,buf_addr,size);
		return ret;
	}
	block_addr	 = addr &(~READ_BLOCK_MASK);
	block_offset = addr &( READ_BLOCK_MASK);
	block_size	 = size;
	while(block_size)
	{	
		ret = asr_norflash_readpage(block_addr, (uint8_t *)spi_rd_buf, READ_BLOCK_SIZE);
		if(ret) return -1;	
		if((block_offset + block_size) < READ_BLOCK_SIZE)
		{	
			memcpy( (uint8_t *)buf_addr,(uint8_t *)spi_rd_buf + block_offset, block_size);
			return 0;
		}
		else
		{
			memcpy((uint8_t *)buf_addr,(uint8_t *)spi_rd_buf + block_offset, READ_BLOCK_SIZE - block_offset);

			block_addr += READ_BLOCK_SIZE;
			buf_addr += (READ_BLOCK_SIZE - block_offset);		
			block_size -= (READ_BLOCK_SIZE - block_offset);
			block_offset = 0;
		}
	}

	return 0;

}

int asr_norflash_write(uint32_t start, uint8_t* src, uint32_t size)
{
    int ret;
	int block_addr;
	int block_offset;
	int block_size;
	int write_size;

	//CP_LOGI("%s : 0x%x, 0x%x\r\n", __func__,start, size);

	if((chip == NULL) || (src == NULL) ||(start < FLASH_BASE_ADDR))
	{
		CP_LOGE("%s: error,addr=0x%x,size=0x%x\r\n", __func__,start,size);
		return -1;
	}

	if(size == 0) 
		return 0;

	block_addr	 = start&(~FLASH_SECTOR_MASK);
	block_offset = start&( FLASH_SECTOR_MASK);
	block_size   = size;

	while(block_size)
	{	
		if((block_offset + block_size) < FLASH_SECTOR_SIZE)
		{	
			ret = asr_norflash_read(block_addr,(uint8_t *)spi_sector_buf, FLASH_SECTOR_SIZE);
			if(ret) return -1;
			ret = asr_norflash_erase_4k(block_addr, FLASH_SECTOR_SIZE);
			if(ret) return -1;	
			memcpy((uint8_t *)spi_sector_buf + block_offset, src, block_size);
			ret = asr_norflash_writepage(block_addr,(uint8_t *)spi_sector_buf, FLASH_SECTOR_SIZE);
			if(ret) 
				return -1;
			else
				return 0;
		}
		else
		{
			if(block_offset!=0){
				ret = asr_norflash_read(block_addr,(uint8_t *)spi_sector_buf, block_offset);
				if(ret) return -1;
				ret = asr_norflash_erase(block_addr, FLASH_SECTOR_SIZE);
				if(ret) return -1;	
			}		
			memcpy((uint8_t *)spi_sector_buf + block_offset, src, FLASH_SECTOR_SIZE - block_offset);
			ret = asr_norflash_writepage( block_addr,(uint8_t *)spi_sector_buf, FLASH_SECTOR_SIZE);
			if(ret) 
				return -1;

			block_addr += FLASH_SECTOR_SIZE;
			src += (FLASH_SECTOR_SIZE - block_offset);		
			block_size -= (FLASH_SECTOR_SIZE - block_offset);
			block_offset = 0;
		}

	}
	//uart_printf("%s-02\r\n", __func__);

	return 0;
}

UINT32 CalcImageChecksum( UINT32* DownloadArea, UINT32 ImageLength,UINT32 checksum)
{
	//uart_printf("CalcImageChecksum DownloadArea[%08x],ImageLength[%08x]\r\n",DownloadArea,ImageLength);
    UINT32 ImageChecksum = checksum;
    UINT32* ptr32 = DownloadArea;
    UINT32* pEnd = ptr32 + (ImageLength / sizeof(UINT32));
    UINT32 BytesSummed = 0;
	
    while ( ptr32 < pEnd )
    {
        // checksum format version 2 algorithm as defined by flasher
        ImageChecksum ^= (*ptr32);
        ptr32++;
        BytesSummed += sizeof(UINT32);
    }
    return ImageChecksum;
}
#define WriteFlashCheckFailed			0x26


UINT32 FlashData_Check(UINT32 buf, UINT32 size, UINT32 flash_Addr)
{
   UINT32 old_checksum = 0,new_checksum = 0;
   char *temp=NULL;
   
   temp =(char *) malloc(size);
   memset(temp,0,size);
   asr_norflash_read(flash_Addr,temp,size);
   old_checksum = CalcImageChecksum((UINT32*)buf,size,0);
   new_checksum = CalcImageChecksum((UINT32*)temp,size,0);
   
   if(old_checksum != new_checksum){
       uart_printf("FlashData_Check,failed!!![0x%08X]:[0x%08X]\n\r",old_checksum,new_checksum);  
   }else{
       free(temp);
       return NoError;
   }
   free(temp);

   return WriteFlashCheckFailed;
    
}


int asr_norflash_erasewrite(uint32_t flash_Addr, uint8_t* buf, uint32_t size)
{
	UINT32 Retval = GeneralError;
	char *temp=NULL;
	UINT32 index=0;
	UINT32 length=0;
	UINT32 tempsize=size;
	int i = 0;
	CP_LOGD("OTA_Update_FotaImage: flash_Addr[0x%08x], size: 0x%08x\n\r", flash_Addr, size);

	//CP_LOGD("%s : 0x%x, 0x%x\r\n", __func__,start, size);

	if((chip == NULL) || (buf == NULL) ||
		(flash_Addr < FLASH_BASE_ADDR))		
		return -1;

	if(size == 0) 
		return 0;
start:
	
	temp =(char *) malloc(FLASH_BLOCK_SIZE);
	if(flash_Addr&(FLASH_BLOCK_SIZE-1)){
		length = FLASH_BLOCK_SIZE - flash_Addr&(FLASH_BLOCK_SIZE-1);
		if(size>=length){
			size -= length;
		}else{
			length = size;
			size = 0;
		}
		while(length >= FLASH_SECTOR_SIZE){
			memcpy(temp,(char*)(buf + index),FLASH_SECTOR_SIZE);
			
			Retval=asr_norflash_erase(flash_Addr + index, FLASH_SECTOR_SIZE);
			if (Retval != NoError)
				goto exit;
	
			if(index%FLASH_BLOCK_SIZE==0){
				uart_printf("...");
			}
			Retval = asr_norflash_write(flash_Addr  + index, (unsigned int)temp, FLASH_SECTOR_SIZE);
			if (Retval != NoError)
				goto exit;
			
			index += FLASH_SECTOR_SIZE;
			length -= FLASH_SECTOR_SIZE;
		}

		if(length > 0){
			memset(temp,0,FLASH_SECTOR_SIZE);
			memcpy(temp,(char*)(buf + index),length);
			
			Retval=asr_norflash_erase((flash_Addr+ index), FLASH_SECTOR_SIZE);
			if (Retval != NoError)
				goto exit;
			if(index%FLASH_BLOCK_SIZE==0){
				uart_printf("...");
			}
			Retval = asr_norflash_write(flash_Addr  + index, (unsigned int)temp, FLASH_SECTOR_SIZE);
			if (Retval != NoError)
				goto exit;
			
			index += length;
			length = 0;
		}
	}

	while(size >= FLASH_BLOCK_SIZE){
		memcpy(temp,(char*)(buf + index),FLASH_BLOCK_SIZE);
		
		Retval=asr_norflash_erase(flash_Addr	+ index, FLASH_BLOCK_SIZE);
		if (Retval != NoError)
			goto exit;
		
		if(index%FLASH_BLOCK_SIZE==0){
			uart_printf("...");
		}
		
		Retval = asr_norflash_write(flash_Addr  + index, (unsigned int)temp, FLASH_BLOCK_SIZE);
		if (Retval != NoError)
			goto exit;
		
		index += FLASH_BLOCK_SIZE;
		size -= FLASH_BLOCK_SIZE;
	}

	while(size >= FLASH_SECTOR_SIZE){
		memcpy(temp,(char*)(buf + index),FLASH_SECTOR_SIZE);
		Retval=asr_norflash_erase(flash_Addr + index, FLASH_SECTOR_SIZE);
		if (Retval != NoError)
			goto exit;
		if(index%FLASH_BLOCK_SIZE==0){
			uart_printf("...");
		}
		Retval = asr_norflash_write(flash_Addr  + index, (unsigned int)temp, FLASH_SECTOR_SIZE);
		if (Retval != NoError)
			goto exit;
		
		index += FLASH_SECTOR_SIZE;
		size -= FLASH_SECTOR_SIZE;
		
	}

	if(size > 0){
		memset(temp,0,FLASH_SECTOR_SIZE);
		memcpy(temp,(char *)(buf+index),size);
		
		Retval=asr_norflash_erase(flash_Addr	+ index, FLASH_SECTOR_SIZE);
		if (Retval != NoError)
			goto exit;
		
		if(index%FLASH_BLOCK_SIZE==0){
			uart_printf("...");
		}
		Retval = asr_norflash_write(flash_Addr  + index, (unsigned int)temp, FLASH_SECTOR_SIZE);
		if (Retval != NoError)
			goto exit;
		
		index += size;
		size = 0;
		
	}
	CP_LOGI("\r\n");


exit:
	free(temp);

	Retval=FlashData_Check(buf,tempsize,flash_Addr);
	if(NoError!=Retval ){
		if(i>10){
			CP_LOGE("asr_norflash_erasewrite fail.\n\r");
			return Retval;

		}
		CP_LOGW("asr_norflash_erasewrite fail,Wait (%d)sec ...\n\r",(i+1)*10);
		mdelay((i+1)*10*1000);
		CP_LOGW("asr_norflash_erasewrite fail,Wait (%d)sec end\n\r",(i+1)*10);
		i++;
		goto start;
	}	
	CP_LOGD("\r\nOTA_Update_FotaImage end\n\r");
	return Retval;
	
}

char fota_magic_buffer[4096];
void clear_fota_magic(void)
{
	char FotaMagic[8] = {0};

	asr_norflash_read(get_fota_param_start_addr(), fota_magic_buffer, 4096);
	asr_norflash_erase(get_fota_param_start_addr(), 4096);
	memcpy(fota_magic_buffer, FotaMagic, sizeof(FotaMagic));
	asr_norflash_write(get_fota_param_start_addr(), fota_magic_buffer, 4096);

}

void erase_fota_pkg(void)
{
	int  fota_pkg_addr;
	int lenth;
	fota_pkg_addr = get_fota_pkg_start_addr();
	lenth=get_fota_pkg_end_addr()-get_fota_pkg_start_addr();
	asr_norflash_erase(fota_pkg_addr, lenth);
}
