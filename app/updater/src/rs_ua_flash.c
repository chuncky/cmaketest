/*
 *
 * 版权(c) 2017 红石阳光（北京）科技有限公司
 * 版权(c) 2011-2017 红石阳光（北京）科技有限公司版权所有
 * 
 * 本文档包含信息是私密的且为红石阳光私有，且在民法和刑法中定义为商业秘密信息。
 * 任何人士不得擅自复印，扫描或以其他任何方式进行传播，否则红石阳光有权追究法律责任。
 * 阅读和使用本资料必须获得相应的书面授权，承担保密责任和接受相应的法律约束。
 *
 */

#include "rs_ua_flash.h"
#include "rs_ua_porting.h"
#include "common.h"
#include "qspi_flash.h"
#include "loadtable.h"
#include "asr_nor_flash.h"


#define MAXFLASH_SIZE (0x2000000)

extern struct spi_flash_chip *chip;
extern unsigned char qspi_wr_buf[1024*64];
extern unsigned char qspi_rd_buf[1024*64];
#if 0
//#include "hal_flash.h"
unsigned int spi_nor_do_erase(unsigned int addr, unsigned int size)
{
    int ret;
	//rs_trace("%s-01,addr= 0x%x,size=0x%x\r\n",__func__,addr,size);
	addr-=FLASH_BASE_ADDR;

	if(chip == NULL)
		return -1;

#if 0
	while(size)
	{
		ret = chip->ops->erase(chip, addr, FLASH_BLOCK_SIZE);
		if(ret) return -1;

		if(size > FLASH_BLOCK_SIZE)
			size -= FLASH_BLOCK_SIZE;
		else
			size = 0;

		addr += FLASH_BLOCK_SIZE;
	}
#else
	ret = chip->ops->erase(chip, addr, size);
	if(ret) return -1;
#endif
	return 0;
}

#define READ_BLOCK_SIZE (0x00000400) //1024 
#define READ_BLOCK_MASK (READ_BLOCK_SIZE-0x1) //1024 - 1
unsigned char blockread_buf[READ_BLOCK_SIZE];

unsigned int spi_nor_do_read(unsigned int addr, unsigned int buf_addr, unsigned int size)
{
    int ret;
	int block_addr;
	int block_offset;
	int block_size;
	uint8_t *buf;
	//rs_trace("%s-01,addr= 0x%x,size=0x%x\r\n",__func__,addr,size);
	addr-=FLASH_BASE_ADDR;

	if((chip == NULL) || (buf_addr == NULL))
		return -1;

	if((addr + size) > chip->size)
		return -1;

#if 1
	block_addr	 = addr &(~READ_BLOCK_MASK);
	block_offset = addr &( READ_BLOCK_MASK);
	block_size   = size;
	//rs_trace("%s-02,block_addr= 0x%x,block_offset=0x%x\r\n",__func__,block_addr,block_offset);
	while(block_size)
	{	
		ret = chip->ops->read(chip, block_addr, READ_BLOCK_SIZE, (uint8_t *)blockread_buf);
		if(ret) return -1;	
		if((block_offset + block_size) < READ_BLOCK_SIZE)
		{	
			memcpy( (uint8_t *)buf_addr,(uint8_t *)blockread_buf + block_offset, block_size);
			return 0;
		}
		else
		{
			memcpy((uint8_t *)buf_addr,(uint8_t *)blockread_buf + block_offset, READ_BLOCK_SIZE - block_offset);

			block_addr += READ_BLOCK_SIZE;
			buf_addr += (READ_BLOCK_SIZE - block_offset);		
			block_size -= (READ_BLOCK_SIZE - block_offset);
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

unsigned int spi_nor_do_write(unsigned int addr, unsigned int buf_addr, unsigned int size)
{
    int ret;
	int block_addr;
	int block_offset;
	int block_size;
	//rs_trace("%s-01,addr= 0x%x,size=0x%x\r\n",__func__,addr,size);
	addr-=FLASH_BASE_ADDR;

	if((chip == NULL) || (buf_addr == NULL))
		return -1;

	if((addr + size) > chip->size)
		return -1;

#if 0
	block_addr	 = addr &(~FLASH_BLOCK_MASK);
	block_offset = addr &( FLASH_BLOCK_MASK);
	block_size   = size;

	while(block_size)
	{	
		ret = chip->ops->read(chip, block_addr, FLASH_BLOCK_SIZE, (uint8_t *)qspi_rd_buf);
		if(ret) return -1;
#if 0
		ret = chip->ops->erase(chip, block_addr, FLASH_BLOCK_SIZE);
		if(ret) return -1;		
#endif		
		if((block_offset + block_size) < FLASH_BLOCK_SIZE)
		{	
			memcpy((uint8_t *)qspi_rd_buf + block_offset, buf_addr, block_size);
			ret = chip->ops->write(chip, block_addr, FLASH_BLOCK_SIZE, (uint8_t *)qspi_rd_buf);
			if(ret) 
				return -1;
			else
				return 0;
		}
		else
		{
			memcpy((uint8_t *)qspi_rd_buf + block_offset, buf_addr, FLASH_BLOCK_SIZE - block_offset);
			ret = chip->ops->write(chip, block_addr, FLASH_BLOCK_SIZE, (uint8_t *)qspi_rd_buf);
			if(ret) 
				return -1;

			block_addr += FLASH_BLOCK_SIZE;
			buf_addr += (FLASH_BLOCK_SIZE - block_offset);		
			block_size -= (FLASH_BLOCK_SIZE - block_offset);
			block_offset = 0;
			addr = block_addr;
		}

	}
#else
	ret = chip->ops->write(chip, addr, size, (uint8_t *)buf_addr);
	if(ret) 
		return -1;
#endif

	return 0;
}


#endif
/**
* 函数说明：flash初始化
* 参数说明：无
* 返回值：成功返回rs_true，失败为rs_false
*/
rs_bool rs_ua_flash_init(void)
{
		
	 //hal_flash_status_t ret;
	 //ret = hal_flash_init();
	//if (ret < HAL_FLASH_STATUS_OK){
	//	 rs_trace("rs_ua_flash_init eror\n");
	//  return rs_false;
	//}
	//return rs_true;
	 rs_trace("%s,flash size= 0x%x \r\n",__func__,chip->size);

	 return rs_true;
}

/**
* 函数说明：读取flash中的数据到内存缓冲区中
* 参数说明：[IN OUT]destination 接收读取flash中数据的缓冲区
*           [IN]address flash的地址
*					  [IN]size 需要读取的长度
*
* 返回值：成功返回rs_true, 失败返回rs_false
*/
rs_bool rs_ua_flash_read(rs_u8 *destination,
                       volatile rs_u8 *address,
                       rs_u32 size)
{
    //hal_flash_status_t status = HAL_FLASH_STATUS_OK;

	//rs_trace("rs_ua_flash_read  address = 0x%x  size = %d \r\n", address , size);

	
	//status = hal_flash_read((uint32_t )address, (uint8_t *)destination, size);
	//if (status != HAL_FLASH_STATUS_OK) {
	//	rs_trace("hal_flash_read  address = 0x%x  size = %d status = %d \r\n", address , size , status);
	//	return rs_false;
	//}
	//return rs_true;


	rs_u32 status = -1;

#if 0
	rs_trace("rs_ua_flash_read  address = 0x%x  size = %d \r\n", address , size);
#endif
	if(address<MAXFLASH_SIZE)
		address+=FLASH_BASE_ADDR;
	//status = spi_nor_do_read((rs_u32 )address, (rs_u32)destination, size);
	status=asr_norflash_read((rs_u32 )address, (rs_u32)destination, size);
	if (status != 0) {
		rs_trace("%s  address = 0x%x  size = %d status = %d \r\n", __func__,address , size , status);
		return rs_false;
	}
#if 0
	{
		   int i = 0 ;
		   int len = 10;
		   if(size < len)
				  len = size;

		   for(i = 0 ;i < len ; i++)
		   {
				rs_trace("0x%02x " ,  destination[i]);
		   }
		   rs_trace("\r\n");
	}
#endif
	
	return rs_true;
}

/**
* 函数说明：把内存缓冲区数据写入到flash中
* 参数说明：[IN]address flash的地址
*           [IN]source 需要写入的数据缓冲区
*					  [IN]size  数据长度
*
* 返回值：成功返回rs_true, 失败返回rs_false
*
* 备注：address升级过程中都是block对齐的，size都是block的长度，不用考虑不对齐的情况。而在应用层下载过程中如果直接往
				往flash里面写的时候，都是page对齐的，size是一个page的大小。
*/
rs_bool rs_ua_flash_write(volatile rs_u8 *address,
                       rs_u8 *source,
                       rs_u32 size)
{
						   
    //hal_flash_status_t status = HAL_FLASH_STATUS_OK;
   //rs_trace("rs_ua_flash_write address = 0x%x  size = %d \r\n", address , size);
	//status = hal_flash_write((uint32_t)address, (uint8_t *)source, size);
	//if (status != HAL_FLASH_STATUS_OK) {
	//	rs_trace("hal_flash_write  address = 0x%x  size = %d status = %d \r\n", address , size , status);
	//	return rs_false;
	//}

	rs_u32 status = -1 ;
#if 0
	rs_trace("rs_ua_flash_write address = 0x%x  size = %d \r\n", address , size);
#endif

	//status =  spi_nor_do_write((rs_u32)address, (rs_u32)source, size);
	if(address<MAXFLASH_SIZE)
		address+=FLASH_BASE_ADDR;
	status =  asr_norflash_write((rs_u32)address, (rs_u32)source, size);
	if (status != 0) {
		rs_trace("%s  address = 0x%x  size = %d status = %d \r\n",__func__, address , size , status);
		return rs_false;
	}
   
   return rs_true;
}


/**
* 函数说明：按block擦除flash
* 参数说明：[IN]address flash的地址
*					  [IN]size 要擦出的数据的长度
*
* 返回值：成功返回rs_true, 失败返回rs_false
*
* 备注：不管在升级过程中或者应用层，address都是block对齐的，size都是block的长度。
*/
rs_bool rs_ua_flash_erase(volatile rs_u8 *address,
                        rs_u32 size)
{
						   
   //hal_flash_status_t status = HAL_FLASH_STATUS_OK;

   //	status = hal_flash_erase((uint32_t)address,  HAL_FLASH_BLOCK_4K);
   //	if (status != HAL_FLASH_STATUS_OK) {
   //	   rs_trace("hal_flash_erase	address = 0x%x	size = %d status = %d \r\n", address , size , status);
   //	   return rs_false;
   //	} 

	rs_u32 status = -1 ;
#if 0
	rs_trace("rs_ua_flash_erase address = 0x%x  size = %d \r\n", address , size);	
#endif
	//status = spi_nor_do_erase((rs_u32)address,  size);
	if(address<MAXFLASH_SIZE)
		address+=FLASH_BASE_ADDR;
	status = asr_norflash_erase((rs_u32)address,  size);
	if (status != 0) {
	   rs_trace("%s	address = 0x%x	size = %d status = %d \r\n",__func__,address , size , status);
	   return rs_false;
	} 

   return rs_true;
}


