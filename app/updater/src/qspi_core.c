#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "common.h"
#include "qspi_dma.h"
#include "qspi_flash.h"
#include "qspi_host.h"
#include "qspi_nor.h"
#include "loadtable.h"

#define RW_BUF_LEN 2048

struct spi_flash_chip *chip=NULL;
//static float rd_speed, wr_speed;

struct qspi_mode qspi_tx_mode[] = {
	{SPI_OPM_TX, "write_x1"},
	{SPI_OPM_TX_QUAD, "write_x4"},
};

struct qspi_mode qspi_rx_mode[] = {
	{SPI_OPM_RX, "read_x1"},
	{SPI_OPM_RX_DUAL, "read_x2"},
	{SPI_OPM_RX_QUAD, "read_x4"},
};

char *qspi_tx_mode_name(int mode)
{
	uint8_t i;

	for (i = 0; i < 2; i++) {
		if (qspi_tx_mode[i].mode == mode)
			return qspi_tx_mode[i].name;
	}

	return NULL;
}

char *qspi_rx_mode_name(int mode)
{
	uint8_t i;

	for (i = 0; i < 3; i++) {
		if (qspi_rx_mode[i].mode == mode)
			return qspi_rx_mode[i].name;
	}

	return NULL;
}

char log_buf[1024*3+1];

void buf_dump(unsigned char* buf, unsigned int len)
{

	unsigned int i;

	if(len>1024)
		len=1024;

	memset(log_buf,0,sizeof(log_buf));

	for(i=0;i<len;i++)
	{
		sprintf(log_buf+3*i,"%02x ",buf[i]);
	}

	uart_printf("\r\n");
	uart_printf(log_buf, i*3);
	uart_printf("\r\n");

}




unsigned char qspi_wr_buf[1024*64];
unsigned char qspi_rd_buf[1024*64];

int bbu_qspi_init(void)
{
	struct qspi_host *host;
	int cs = QSPI_CS_A1;
	int mhz = 104;
	//int nand = 0;
	int use_xip = 1;
	int rx_mode = SPI_OPM_RX_QUAD;
	int tx_mode = SPI_OPM_TX_QUAD;
	int qpi = 0;



	host = qspi_host_init(cs, mhz, use_xip);

	chip = spi_nor_init(host, cs, rx_mode, tx_mode, qpi);

	if (!chip) {
		uart_printf("SPI Nor/NAND flash init failed\r\n");

	}

	return 0;
}

int crane_qspi_write_uint(unsigned int addr , unsigned int value){
	int block_addr   = (addr - FLASH_BASE_ADDR)&(~FLASH_BLOCK_MASK);
	int block_offset = (addr - FLASH_BASE_ADDR)&( FLASH_BLOCK_MASK);
    int ret;
    
    /*
    uart_printf("crane_qspi_write_uint block_addr   =[%.08x]\r\n",block_addr);
    uart_printf("crane_qspi_write_uint block_offset =[%.08x]\r\n",block_offset);
    uart_printf("crane_qspi_write_uint value        =[%.08x]\r\n",value);
    */

    //read from flash to buffer
	ret = chip->ops->read(chip, block_addr, FLASH_BLOCK_SIZE, (uint8_t *)qspi_rd_buf);
    //update buffer
    memcpy((uint8_t *)qspi_rd_buf + block_offset, &value, sizeof(value));
    //erase block and write buffer to flash
    //
    //TODO: any possiablity after erase succeed ,flash write failed or power shut?
    //      might seprate loadtable into a RO area and RW area to avoid this happen.
    //
	ret = chip->ops->erase(chip, block_addr, FLASH_BLOCK_SIZE);
	ret = chip->ops->write(chip, block_addr, FLASH_BLOCK_SIZE, (uint8_t *)qspi_rd_buf);

    return ret;
}

int crane_qspi_read_uint(unsigned int addr){
	int value = 0;
	chip->ops->read(chip, (addr-FLASH_BASE_ADDR), sizeof(value), (uint8_t *)&value);
    //uart_printf("crane_qspi_read_uint [%x]=[%x] !\r\n",addr,value);
    return value;
}

int bbu_qspi_test(unsigned int base_addr)
{

#if 1
	unsigned int flash_addr;
	unsigned int mem_addr;
	unsigned int size;
	int ret = 0;
	unsigned int i;

	flash_addr=base_addr;
	mem_addr=(unsigned int)qspi_wr_buf;
	size=64*1024;

	for(i=0;i<sizeof(qspi_wr_buf);i++)
		qspi_wr_buf[i]=i;

    //dump original empty read bufer
	uart_printf("1st dump\r\n");
	buf_dump(qspi_rd_buf,size);
	uart_printf("erase\r\n");

    //erase and write to target flash
	ret = chip->ops->erase(chip, flash_addr, 64*1024);
	uart_printf("write\r\n");
	ret = chip->ops->write(chip, flash_addr, size, (uint8_t *)mem_addr);

    //read out and dump the read buffer for local check
	mem_addr=(unsigned int)qspi_rd_buf;
	memset(qspi_rd_buf,0,sizeof(qspi_rd_buf));
	uart_printf("read\r\n");
	ret = chip->ops->read(chip, flash_addr, size, (uint8_t *)mem_addr);

	uart_printf("2nd dump\r\n");
	buf_dump(qspi_rd_buf,size);

	uart_printf("bbu_qspi_test baseaddr=[%x] done\r\n",base_addr);
	
	for (i=0;i<size;i++){
		if (qspi_rd_buf[i]!=qspi_wr_buf[i])
			uart_printf("qspi wrong @0x%x\r\n", i);
	}
	uart_printf("qspi good!!\r\n");

	return 0;

#endif

}
