#ifndef FOTA_ASRSPI
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "NSSP.h"
#include "ssp_host.h"
#include "spi_nor.h"
#define fatal_printf uart_printf

extern int ssp_index;
extern int ssp_init_clk;
struct spinor_chip_info spinor_chip = {0};
extern struct spinor_chip_info *spinor_init(const char *name, int clk);

/* NOTE: double check command sets and memory organization when you add
 * more nor chips.  This current list focusses on newer chips, which
 * have been converging on command sets which including JEDEC ID.
 *
 * All newly added entries should describe *hardware* and should use SECT_4K
 * (or SECT_4K_PMC) if hardware supports erasing 4 KiB sectors. For usage
 * scenarios excluding small sectors there is config option that can be
 * disabled: CONFIG_MTD_SPI_NOR_USE_4K_SECTORS.
 * For historical (and compatibility) reasons (before we got above config) some
 * old entries may be missing 4K flag.
 */
static const struct nor_info spi_nor_ids[] = {
    /* GigaDevice */
    {
        "gd25lq128d", INFO(0xc86018, 0, 64 * 1024, 256,
            SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ |
            SPI_NOR_HAS_LOCK | SPI_NOR_HAS_TB)
    },
    /* Winbond */
    {
        "w25q128jw", INFO(0xef4018, 0, 64 * 1024, 256,
            SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ |
            SPI_NOR_HAS_LOCK | SPI_NOR_HAS_TB)
    },
    /* Macronix */
    { "mx25l3239e",  INFO(0xc22536, 0, 64 * 1024, 64,
            SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ |
            SPI_NOR_HAS_LOCK | SPI_NOR_HAS_TB) 
    },
    {
        "common", INFO(0, 0, 64 * 1024, 256,
            SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ |
            SPI_NOR_HAS_LOCK | SPI_NOR_HAS_TB)
    },
    { },
};

/* Standard SPI-NOR flash normal commands */
static const struct spinor_cmd_cfg cmd_table[] = {
    /* GigaDevice */
    {
        "gd25lq128d", SPI_NOR_CMD(0xc86019, 0xb7, 0xe9, 0x05, 0x01, 0x20, 0x52, 0xd8, 0xc7, 
            0x02, 0x06, 0x04, 0x9f, 0x66, 0x99, 0xab, 0x03, 0x00)
    },
    /* Winbond */
    {
        "w25q128jw", SPI_NOR_CMD(0xef4018, 0xb7, 0xe9, 0x05, 0x01, 0x20, 0x52, 0xd8, 0xc7, 
            0x02, 0x06, 0x04, 0x9f, 0x66, 0x99, 0xab, 0x03, 0x00)
    },
    /* Macronix */
    {
        "mx25l3239e", SPI_NOR_CMD(0xc22536, 0xb7, 0xe9, 0x05, 0x01, 0x20, 0x52, 0xd8, 0xc7, 
            0x02, 0x06, 0x04, 0x9f, 0x66, 0x99, 0xab, 0x03, 0x00)
    },
    {
        "common", SPI_NOR_CMD(0, 0xb7, 0xe9, 0x05, 0x01, 0x20, 0x52, 0xd8, 0xc7, 
            0x02, 0x06, 0x04, 0x9f, 0x66, 0x99, 0xab, 0x03, 0x00)
    },   
    { },
};
#ifdef SPI_RW_DMA
#pragma arm section zidata="SPI_NONCACHE_SECTION"
__align(32) UINT8 SPI_TX_DESC[SSP_READ_DMA_DESC*sizeof(DMA_DESCRIPTOR)]={0};
__align(32) UINT8 SPI_RX_DESC[SSP_READ_DMA_DESC*sizeof(DMA_DESCRIPTOR)]={0};
__align(32) UINT8 SPI_TX_BUFF[SSP_READ_DMA_SIZE]={0};
__align(32) UINT8 SPI_RX_BUFF[SSP_READ_DMA_SIZE]={0};
#pragma arm section zidata
#endif
        
int spinor_page_program_pio(struct spinor_chip_info *chip, struct spinor_cmd *cmd)
{
    uint8_t dummy;
    uint8_t cmdsz = 1 + chip->addr_width;
    int port_index = ssp_index;
    int tx_len= cmd->n_tx;
    uint8_t *tx_buffer = cmd->tx_buf;
	int i = 0;

    //spi_dss_switch(port_index, SPI_DSS8);
    spi_xmit_start(port_index);

	for(i = 0; i < cmdsz; i++){
        spi_xmit_data(port_index, cmd->cmdtmp[i]);

        if(spi_wait_complete(port_index)){
            fatal_printf("tx timeout\n\r");
            return -1;
        }
        dummy = spi_recv_data(port_index);
    }

	for(i = 0; i < tx_len; i++){
        spi_xmit_data(port_index, tx_buffer[i]);
        
        if(spi_wait_complete(port_index)){
            fatal_printf("tx timeout\n\r");
            return -1;
        }
        dummy = spi_recv_data(port_index);
    }

    return 0;
}
#ifdef SPI_RW_DMA
int spinor_cache_read_dma(struct spinor_chip_info *chip, struct spinor_cmd *cmd)
{
    uint8_t dummy = 0;
    uint8_t cmdsz = 1 + chip->addr_width;
    int rx_len = cmd->n_rx;
    int tx_len= cmd->n_tx;
    int port_index = ssp_index;
    int i = 0;
    uint8_t *tx_buffer = cmd->tx_buf;
    uint8_t *rx_buffer = cmd->rx_buf;

    DMA_CMDx_T RX_Cmd,TX_Cmd;
	DMA_DESCRIPTOR* RX_Desc = NULL;
	DMA_DESCRIPTOR* TX_Desc = NULL;
    

	RX_Desc = (DMA_DESCRIPTOR*)(SPI_RX_DESC);
	TX_Desc = (DMA_DESCRIPTOR*)(SPI_TX_DESC);

	RX_Cmd.value = 0;
	RX_Cmd.bits.IncSrcAddr = 0;
	RX_Cmd.bits.IncTrgAddr = 1;
	RX_Cmd.bits.FlowSrc = 1;
	RX_Cmd.bits.FlowTrg = 0;
	RX_Cmd.bits.Width = 3;
	RX_Cmd.bits.MaxBurstSize = 1;
    RX_Cmd.bits.Length = rx_len;
    
	TX_Cmd.value = 0;
	TX_Cmd.bits.IncSrcAddr = 1;
	TX_Cmd.bits.IncTrgAddr = 0;
	TX_Cmd.bits.FlowSrc = 0;
	TX_Cmd.bits.FlowTrg = 1;
	TX_Cmd.bits.Width = 3;
	TX_Cmd.bits.MaxBurstSize = 1;
    TX_Cmd.bits.Length = rx_len;

    ssp_map_channel(port_index, SSP_DMA_TX_CHANNEL, SSP_DMA_RX_CHANNEL);
    set_user_aligment(SSP_DMA_RX_CHANNEL);
    set_user_aligment(SSP_DMA_TX_CHANNEL);

    spi_xmit_start(port_index);
    spi_dss_switch(port_index, SPI_DSS8);
    for(i = 0; i < cmdsz; i++){
        spi_xmit_data(port_index, cmd->cmdtmp[i]);

        if(spi_wait_complete(port_index)){
            fatal_printf("tx timeout\n\r");
            return -1;
        }
        dummy = spi_recv_data(port_index);
    }

    spi_dma_enable(port_index);
    spi_rx_endian(port_index, 2);
    spi_dss_switch(port_index, SPI_DSS32); 
    dma_config_descriptor(&RX_Desc[0], NULL, &(ssp_port(port_index)->DATAR), rx_buffer, RX_Cmd.value, 1);
    dma_config_descriptor(&TX_Desc[0], NULL, tx_buffer, &(ssp_port(port_index)->DATAR), TX_Cmd.value, 1);
    dma_load_descriptor(&RX_Desc[0], SSP_DMA_RX_CHANNEL);
    dma_load_descriptor(&TX_Desc[0], SSP_DMA_TX_CHANNEL);
    dma_xfer_start(SSP_DMA_RX_CHANNEL);
    dma_xfer_start(SSP_DMA_TX_CHANNEL);
    dma_wait_done(SSP_DMA_TX_CHANNEL, 0x0fffffff, DMA_STOP);
    dma_wait_done(SSP_DMA_RX_CHANNEL, 0x0fffffff, DMA_STOP);
    ssp_unmap_channel(2);

    return 0;

}
#endif

static int spinor_cache_read_pio(struct spinor_chip_info *chip, struct spinor_cmd *cmd)
{
	uint8_t dummy = 0;
	uint8_t cmdsz = 1 + chip->addr_width;
	int rx_len = cmd->n_rx;
	int tx_len = cmd->n_tx;
	int port_index = ssp_index;
	int i = 0;
	uint8_t *rx_buffer = cmd->rx_buf;


	spi_xmit_start(port_index);

	//read cmd(03H) + address
	spi_dss_switch(port_index,SPI_DSS8);
	for(i = 0; i < cmdsz; i++){
		spi_xmit_data(port_index, cmd->cmdtmp[i]);

		if(spi_wait_complete(port_index)){
			fatal_printf("tx timeout\n\r");
			return -1;
		}
		dummy = spi_recv_data(port_index);
	}

	//read data
	for(i = 0; i < tx_len; i++){
		spi_xmit_data(port_index, 0x5A);
		
		if(spi_wait_complete(port_index)){
			fatal_printf("tx timeout\n\r");
			return -1;
		}
		rx_buffer[i] = spi_recv_data(port_index);
	}

	return 0;
}


int spi_start_cmd(struct spinor_chip_info *chip, struct spinor_cmd *cmd)
{
	int ret = 0;
	int i = 0;
    int port_index = ssp_index;
    uint8_t *tx_buffer = cmd->tx_buf;
    uint8_t *rx_buffer = cmd->rx_buf;
    uint32_t dummy;
    int rx_len = cmd->n_rx, tx_len= cmd->n_tx;
    int dummy_len = 0, recv_len = 0;

    spi_disable_ssp(port_index);
    spi_xmit_config(port_index, cmd->mode); 
    if(chip->table->cache_read == cmd->command){
//		CP_LOGD("spi_start_cmd 1\r\n");
#ifdef SPI_RW_DMA
	ret = spinor_cache_read_dma(chip, cmd);
#else
	ret = spinor_cache_read_pio(chip,cmd);
#endif
        if(0 != ret)
            goto xmit_stop;
    }
    else if(chip->table->prog == cmd->command)
    {
    	
//		CP_LOGD("spi_start_cmd 2\r\n");
        ret = spinor_page_program_pio(chip, cmd);
        if(0 != ret)
           goto xmit_stop;
    }
    else
    {
        spi_xmit_start(port_index);
//		CP_LOGD("spi_start_cmd 3\r\n");

        for(i = 0; i < tx_len; i++)
        {   
            spi_xmit_data(port_index, *tx_buffer++);

            if(spi_wait_complete(port_index)){
                fatal_printf("tx timeout\n\r");
                ret = -1;
				goto xmit_stop;
            }
            if(rx_buffer != 0 ){
                //sometimes rx_len is smaller that tx_len
                //which means the first (tx_len-rx_len) byte
                //is dummy byte received 
                dummy_len = tx_len - rx_len;
                if(dummy_len){
                    dummy = spi_recv_data(port_index);
                    dummy_len--;
                }else{
                    *rx_buffer++ = spi_recv_data(port_index);
                    recv_len++;
                }
            }else{
                dummy = spi_recv_data(port_index);
            }
        }
        if(recv_len != rx_len)
        {
            fatal_printf("recv error! recv_len: %d, rx_len: %d\n\r", recv_len, rx_len);
            spi_xmit_stop(port_index);
            ret = -2;
			goto xmit_stop;
        }
    }

xmit_stop:

	spi_xmit_stop(ssp_index);
    spi_disable_ssp(port_index);
    return ret;

}

static int spinor_reset_enable(struct spinor_chip_info *chip)
{ 
    struct spinor_cmd cmd;
    uint8_t command[1] = {0};
    uint8_t data[1] = {0};
    
    memset(&cmd, 0, sizeof(struct spinor_cmd));
    command[0] = chip->table->en_rst;
    
    cmd.rx_buf = data;
    cmd.n_rx = 1;
    cmd.n_tx = 1;
    cmd.tx_buf = command;
    cmd.mode = SPI_DSS8;
    cmd.command = chip->table->en_rst;
    
    if (spi_start_cmd(chip, &cmd) < 0){
		fatal_printf("spi_nor reset enable failed!\n");
        return -1;
    }

	return 0;
}

static int spinor_reset_memory(struct spinor_chip_info *chip)
{
    struct spinor_cmd cmd;
    uint8_t command[1] = {0};
    
    memset(&cmd, 0, sizeof(struct spinor_cmd));
    command[0] = chip->table->reset;
    
    cmd.rx_buf = NULL;
    cmd.n_rx = 0;
    cmd.n_tx = 1;
    cmd.tx_buf = command;
    cmd.mode = SPI_DSS8;
    cmd.command = chip->table->reset;
    
    if (spi_start_cmd(chip, &cmd) < 0){
		fatal_printf("spi_nor reset memory failed!\n");
        return -1;
    }

	return 0;
}

uint32_t spinor_read_reg(struct spinor_chip_info *chip)
{
    struct spinor_cmd cmd;
    uint8_t buf[2] = {0};
    uint32_t status;
    uint8_t command[2] = {0};
    
    memset(&cmd, 0, sizeof(struct spinor_cmd));
    command[0] = chip->table->read_status;
    
    cmd.rx_buf = buf;
    cmd.n_rx = 2;
    cmd.n_tx = 2;
    cmd.tx_buf = command;
    cmd.mode = SPI_DSS8;
    cmd.command = chip->table->read_status;
    
    if (spi_start_cmd(chip, &cmd) < 0){
       fatal_printf("spi_nor read reg failed!\n");
       return 1;
    }

	status = buf[1];
    
	return status;
}

void spinor_read_status(struct spinor_chip_info *chip, uint32_t Wait)
{
	uint32_t i=0;
	uint32_t read, ready, status;

	status = 0;
	read = FALSE;	//this flag gets set when we read first entry from fifo
	//if the caller waits to 'Wait' for the BUSY to be cleared, start READY off as FALSE
	//if the caller doesn't wait to wait, set READY as true, so we don't wait on the bit
	ready = (Wait) ? FALSE : TRUE;

	do{
		//set the READ flag, and read the status
		read = TRUE;
        status = spinor_read_reg(chip);
        //fatal_printf("status: 0x%x\n\r", status);
        
		//set the READY flag if the status wait bit is cleared
		if((status & 1) == 0)		// operation complete (eg. not busy)?
			ready = TRUE;

		//we need to wait until we read at least 1 valid status entry
		//if we're waiting for the Write, wait till WIP bits goes to 0
	}while ((!read) || (!ready));

	return;
}

static int spinor_write_enable(struct spinor_chip_info *chip)
{
	struct spinor_cmd cmd;
    uint8_t command[1] = {0};
    uint8_t data[1] = {0};
    uint32_t status = 0;
    
    memset(&cmd, 0, sizeof(struct spinor_cmd));
    command[0] = chip->table->wr_enable;
    
    cmd.rx_buf = data;
    cmd.n_rx = 1;
    cmd.n_tx = 1;
    cmd.tx_buf = command;
    cmd.mode = SPI_DSS8;
    cmd.command = chip->table->wr_enable;
    
    if (spi_start_cmd(chip, &cmd) < 0){
		fatal_printf("spi_nor write enable failed!\n");
        return -1;
    } 

    status = spinor_read_reg(chip);
	if (status & BIT_1 !=BIT_1)
	{
		fatal_printf("Write enable failed!!!\n\r");
	}

    return 0;
}

static int spinor_reset(struct spinor_chip_info *chip)
{
	struct spinor_cmd cmd;
    uint8_t command[1] = {0};
    
    memset(&cmd, 0, sizeof(struct spinor_cmd));
    command[0] = chip->table->en_rst;
    
    cmd.rx_buf = NULL;
    cmd.n_rx = 0;
    cmd.n_tx = 1;
    cmd.tx_buf = command;
    cmd.mode = SPI_DSS8;
    cmd.command = chip->table->en_rst;
    
	if (spi_start_cmd(chip, &cmd) < 0){
		fatal_printf("spi_nor enable reset failed!\n");
        return -1;
    }

    (void)spinor_reset_memory(chip);
	spinor_read_status(chip, TRUE);
    
	return 0;
}

static int spinor_read_id(struct spinor_chip_info *chip, uint8_t *buf)
{
    struct spinor_cmd cmd;
    uint8_t command[4] = {0};
    
    memset(&cmd, 0, sizeof(struct spinor_cmd));
    command[0] = 0x9F;
    
    cmd.rx_buf = buf;
    cmd.n_rx = 4;
    cmd.n_tx = 4;
    cmd.tx_buf = command;
    cmd.mode = SPI_DSS8;
    cmd.command = 0x9F;

    if (spi_start_cmd(chip, &cmd) < 0){
        fatal_printf("spi nor read id failed!\n");
        return -1;
    }
    
    return 0;
}

 int spinor_erase(struct spinor_chip_info *chip)
{
    struct spinor_cmd cmd;
    uint8_t command[1] = {0};
    int ret = 0;
    
    memset(&cmd, 0, sizeof(struct spinor_cmd));
    command[0] = chip->table->chip_erase;
    
    cmd.rx_buf = NULL;
    cmd.n_rx = 0;
    cmd.n_tx = 1;
    cmd.tx_buf = command;
    cmd.mode = SPI_DSS8;
    
    //make sure the device is ready for the command
	spinor_read_status(chip, TRUE);

	//enable program mode. this is required for any program, eg: erase, program page or otp or write status reg.
	ret = spinor_write_enable(chip);
    if (spi_start_cmd(chip, &cmd) < 0){
        fatal_printf("spi_nor erase failed!\n");
        return -1;
    }
   
	//don't return until the operation completes.
	spinor_read_status(chip, TRUE);	// wait for the last erase operation to complete.
	
	return ret;	
}

static int spinor_erase_sector(struct spinor_chip_info *chip, int offset, int size)
{
    struct spinor_cmd cmd;
    uint8_t command[MAX_CMD_SIZE] = {0};
    uint8_t buf[MAX_CMD_SIZE] = {0};
    int ret = 0;
    
    memset(&cmd, 0, sizeof(struct spinor_cmd));
    
    cmd.command = chip->table->sector_erase_4k;
    cmd.rx_buf = buf;
    cmd.n_rx = 1 + chip->addr_width;
    cmd.n_tx = 1 + chip->addr_width;
    cmd.tx_buf = command;
    cmd.mode = SPI_DSS8;
    
//    uart_printf("spinor_erase_sector off:%d,size:%d",offset,size);
    size = ADDR_ALIGN(size, SZ_4K);//make size 4kbytes-align
    offset = (offset) & (~(SZ_4K - 1));
    
    while(size > 0)
    {   
        command[0] = chip->table->sector_erase_4k;
        if(4 == chip->addr_width){
            command[1] = offset >> 24;
            command[2] = offset >> 16;
            command[3] = offset >> 8;
            command[4] = offset >> 0;
        }else{
            command[1] = offset >> 16;
            command[2] = offset >> 8;
            command[3] = offset >> 0;
        }

        offset += SZ_4K;
        size   -= SZ_4K;
        //make sure the device is ready for the command
    	spinor_read_status(chip, TRUE);

    	//enable program mode. this is required for any program, eg: erase, program page or otp or write status reg.
    	ret = spinor_write_enable(chip);
        if (spi_start_cmd(chip, &cmd) < 0){
            fatal_printf("spi_nor erase sector failed!\n");
            return -1;
        }
       
    	//don't return until the operation completes.
    	spinor_read_status(chip, TRUE);	// wait for the last erase operation to complete.
    }
    
	
	return ret;	
}

static int spinor_erase_blk_32k(struct spinor_chip_info *chip, int offset, int size)
{
    struct spinor_cmd cmd;
    uint8_t command[MAX_CMD_SIZE] = {0};
    uint8_t buf[MAX_CMD_SIZE] = {0};
    int ret = 0;
    
    memset(&cmd, 0, sizeof(struct spinor_cmd));
    
    cmd.command = chip->table->blk_erase_32k;
    cmd.rx_buf = buf;
    cmd.n_rx = 1 + chip->addr_width;
    cmd.n_tx = 1 + chip->addr_width;
    cmd.tx_buf = command;
    cmd.mode = SPI_DSS8;
    
    size = ADDR_ALIGN(size, SZ_32K);//make size 32kbytes-align
    offset = (offset) & (~(SZ_32K - 1));
    
    while(size > 0)
    {
        command[0] = chip->table->blk_erase_32k;
        if(4 == chip->addr_width){
            command[1] = offset >> 24;
            command[2] = offset >> 16;
            command[3] = offset >> 8;
            command[4] = offset >> 0;
        }else{
            command[1] = offset >> 16;
            command[2] = offset >> 8;
            command[3] = offset >> 0;
        }

        offset += SZ_32K;
        size   -= SZ_32K;
        //make sure the device is ready for the command
    	spinor_read_status(chip, TRUE);

    	//enable program mode. this is required for any program, eg: erase, program page or otp or write status reg.
    	ret = spinor_write_enable(chip);
        if (spi_start_cmd(chip, &cmd) < 0){
            fatal_printf("spi_nor erase sector failed!\n");
            return -1;
        }
       
    	//don't return until the operation completes.
    	spinor_read_status(chip, TRUE);	// wait for the last erase operation to complete.
    }
    
	
	return ret;	
}

static int spinor_erase_blk_64k(struct spinor_chip_info *chip, int offset, int size)
{
    struct spinor_cmd cmd;
    uint8_t command[MAX_CMD_SIZE] = {0};
    uint8_t buf[MAX_CMD_SIZE] = {0};
    int ret = 0;
    
    memset(&cmd, 0, sizeof(struct spinor_cmd));
    
    cmd.command = chip->table->blk_erase_64k;
    cmd.rx_buf = buf;
    cmd.n_rx = 1 + chip->addr_width;
    cmd.n_tx = 1 + chip->addr_width;
    cmd.tx_buf = command;
    cmd.mode = SPI_DSS8;
    
    size = ADDR_ALIGN(size, SZ_64K);//make size 64kbytes-align
    offset = (offset) & (~(SZ_64K - 1));
    
    while(size > 0)
    {
        command[0] = chip->table->blk_erase_64k;
        if(4 == chip->addr_width){
            command[1] = offset >> 24;
            command[2] = offset >> 16;
            command[3] = offset >> 8;
            command[4] = offset >> 0;
        }else{
            command[1] = offset >> 16;
            command[2] = offset >> 8;
            command[3] = offset >> 0;
        }

        offset += SZ_64K;
        size   -= SZ_64K;
        //make sure the device is ready for the command
    	spinor_read_status(chip, TRUE);

    	//enable program mode. this is required for any program, eg: erase, program page or otp or write status reg.
    	ret = spinor_write_enable(chip);
        if (spi_start_cmd(chip, &cmd) < 0){
            fatal_printf("spi_nor erase sector failed!\n");
            return -1;
        }
       
    	//don't return until the operation completes.
    	spinor_read_status(chip, TRUE);	// wait for the last erase operation to complete.
    }
    
	
	return ret;	
}
UINT8 spirxbuf[PAGE_SIZE];

int spinor_program_load(struct spinor_chip_info *chip, 
								uint32_t offset, 
								uint32_t *buffer, 
								int size)
{
    struct spinor_cmd cmd = {0};
    int Retval = 0;
    uint8_t *rx_buffer;
    uint8_t command[MAX_CMD_SIZE] = {0};
    unsigned int i, convert_size;
    
    rx_buffer = spirxbuf;
    
    command[0] = chip->table->prog;
    if(4 == chip->addr_width){
        command[1] = offset >> 24;
        command[2] = offset >> 16;
        command[3] = offset >> 8;
        command[4] = offset >> 0;
    }else{
        command[1] = offset >> 16;
        command[2] = offset >> 8;
        command[3] = offset >> 0;
    }
	cmd.command = chip->table->prog;
	cmd.n_tx = size;
	cmd.tx_buf = buffer;
	cmd.mode = SPI_DSS8;
	cmd.cmdtmp = command;
	cmd.rx_buf = rx_buffer;
	cmd.n_rx = size;
	Retval = spi_start_cmd(chip, &cmd);

	return Retval;
}
int spinor_cache_read(struct spinor_chip_info *chip, 
                                uint32_t offset, 
                                uint32_t *buffer, 
                                int size)
{
    struct spinor_cmd cmd = {0};
    int Retval = 0;
    uint8_t command[MAX_CMD_SIZE] = {0};
    command[0] = chip->table->cache_read;

    if(4 == chip->addr_width){
        command[1] = offset >> 24;
        command[2] = offset >> 16;
        command[3] = offset >> 8;
        command[4] = offset >> 0;
    }else{
        command[1] = offset >> 16;
        command[2] = offset >> 8;
        command[3] = offset >> 0;
    }

	cmd.command = chip->table->cache_read;
	cmd.n_tx = size;
#ifdef SPI_RW_DMA	
	cmd.tx_buf = SPI_TX_BUFF;
	cmd.mode = SPI_DSS32;
#else
	cmd.mode = SPI_DSS8;
#endif
	cmd.cmdtmp = command;
	cmd.rx_buf = buffer;
	cmd.n_rx = size;

	Retval = spi_start_cmd(chip, &cmd);
	return Retval;
}                        

static int spinor_read(struct spinor_chip_info *chip, uint32_t FlashOffset, int Size, uint32_t Buffer)
{
	int i=1;
    int port_index = ssp_index;
    int Retval = 0;
    uint32_t read_size = 0;

//	CP_LOGD("function:%s,FlashOffset:0x%x,Size:0x%x,Buffer:0x%x\r\n",__func__,FlashOffset,Size,Buffer);
#ifdef SPI_RW_DMA
    if(Size%SSP_READ_DMA_SIZE || FlashOffset%SSP_READ_DMA_SIZE) 
		ASSERT(0);
    while(Size > 0 && Size >= SSP_READ_DMA_SIZE){
	    Retval = spinor_cache_read(chip
				,FlashOffset
				,SPI_RX_BUFF 		//Buffer
				,SSP_READ_DMA_SIZE);
	    memcpy(Buffer,SPI_RX_BUFF,SSP_READ_DMA_SIZE);

	    Size   		-= SSP_READ_DMA_SIZE;
	    Buffer   	+= SSP_READ_DMA_SIZE;
	    FlashOffset	+= SSP_READ_DMA_SIZE;
    }
#else
	while (Size > 0){
		read_size = Size > SSP_READ_DMA_SIZE ? SSP_READ_DMA_SIZE : Size;
		Retval = spinor_cache_read(chip, FlashOffset, Buffer, read_size);
		FlashOffset += read_size;
		Buffer += read_size;
		Size -= read_size;
	}
#endif
    
	return Retval;
}
extern void mdelay(int ms);

static int spinor_write(struct spinor_chip_info *chip, uint32_t addr, int Size, uint32_t Buffer)
{
	uint32_t Retval, i, total_size, write_size, status;
	uint32_t *buff = (uint32_t *) Buffer;

	do{
		//make sure the device is ready to be written to
		spinor_read_status(chip, TRUE);

		//get device ready to Program
		spinor_write_enable(chip);

		spinor_read_status(chip, TRUE);
		write_size = Size > PAGE_SIZE? PAGE_SIZE : Size;
        
		if (write_size == PAGE_SIZE)
		{
			Retval = spinor_program_load(chip, addr, Buffer, PAGE_SIZE);
			//update counters
			addr   += PAGE_SIZE;
			Buffer += PAGE_SIZE;
			Size   -= PAGE_SIZE;
		}
		else
		{
			Retval = spinor_program_load(chip, addr, Buffer, write_size);
			Size=0;
		}

		spinor_read_status(chip, TRUE);
	} while( (Size > 0) && (Retval == 0) );
    
	return Retval;
}

static const struct spinor_ops nor_ops = {
    .reset      = spinor_reset,
    .read_id    = spinor_read_id,
    .read       = spinor_read,
    .write      = spinor_write,
    .erase      = spinor_erase,
    .erase_sector= spinor_erase_sector,
    .erase_block= spinor_erase_blk_64k,
};

static const struct nor_info *spinor_get_flash_info_by_name(const char *name)
{
	const struct nor_info *id = spi_nor_ids;

	while (id->name) {
		if (!strcmp(name, id->name))
			return id;
        if (!strcmp((const char *)"common", id->name))
			return id;
		id++;
	}
	return NULL;
}

static const struct nor_info *spinor_get_flash_info_by_jedecid(uint8_t *id)
{
	int			tmp;
	const struct nor_info	*info;
    uint8_t id_common[SPINOR_MAX_ID_LEN] = {0};
    
	for (tmp = 0; tmp < ARRAY_SIZE(spi_nor_ids) - 1; tmp++) {
		info = &spi_nor_ids[tmp];
		if (info->id_len) {
			if (!memcmp(info->id, id, info->id_len))
				return &spi_nor_ids[tmp];
            if (!memcmp(info->id, id_common, info->id_len))
				return &spi_nor_ids[tmp];
		}
	}
	fatal_printf("unrecognized JEDEC id bytes: %02x, %02x, %02x, %02x\r\n",id[0], id[1], id[2], id[3]);
	return NULL;
}

static const struct spinor_cmd_cfg *spinor_get_cmd_info_by_name(const char *name)
{
	const struct spinor_cmd_cfg *cmd_cfg = cmd_table;

	while (cmd_cfg->name) {
		if (!strcmp(name, cmd_cfg->name))
			return cmd_cfg;
        if (!strcmp((const char *)"common", cmd_cfg->name))
			return cmd_cfg;
        
		cmd_cfg++;
	}
	return NULL;
}

static const struct spinor_cmd_cfg *spinor_get_cmd_info_by_jedecId(uint8_t *id)
{
	int			tmp;
	const struct spinor_cmd_cfg	*cmd_cfg;
    uint8_t id_common[SPINOR_MAX_ID_LEN] = {0};

	for (tmp = 0; tmp < ARRAY_SIZE(cmd_table) - 1; tmp++) {
		cmd_cfg = &cmd_table[tmp];
		if (!memcmp(cmd_cfg->id, id, SPINOR_MAX_ID_LEN))
			return &cmd_table[tmp];
        if (!memcmp(cmd_cfg->id, id_common, SPINOR_MAX_ID_LEN))
			return &cmd_table[tmp];
	}
    
	return NULL;
}

static void spi_nor_set_3byte_opcodes(struct spinor_chip_info *chip)
{
    chip->table->cache_read = 0x03;//SPINOR_OP_READ_4B;
    chip->table->prog = 0x02;//SPINOR_OP_PP_4B;
    chip->table->sector_erase_4k = 0x20;//SPINOR_OP_BE_4K_4B;
    chip->table->blk_erase_32k = SPINOR_OP_BE_32K_4B;
    chip->table->blk_erase_64k = SPINOR_OP_SE_4B;
}


struct spinor_chip_info *spinor_init(const char *name, int clk)
{
	int ret = 0;
	struct spinor_chip_info *chip = &spinor_chip;
	struct spinor_cmd_cfg *cmd_cfg = NULL;
	const struct nor_info *info = NULL;
	uint8_t	id[SPINOR_MAX_ID_LEN];
	uint64_t size = 0;

	fatal_printf("spinor_init\r\n");
	ssp_init_clk = clk;
	memset(chip, 0, sizeof(struct spinor_chip_info));

	ssp_port_init(ssp_index, clk, 1);
	ssp_init_master(ssp_index, clk,1);

	ret = spinor_read_id(chip, id);
	CP_LOGI("Manufacture ID: 0x%x, Device ID: 0x%x, retval: %d.\n\r", id[1], id[2], ret);

	if (name){
		info    = spinor_get_flash_info_by_name(name);
		cmd_cfg = spinor_get_cmd_info_by_name(name);
	}
	/* Try to auto-detect if chip name wasn't specified or not found */
	else{
		info    = spinor_get_flash_info_by_jedecid(id);
		cmd_cfg = spinor_get_cmd_info_by_jedecId(id);
	}

	if( (!info) || (!cmd_cfg) ){
		fatal_printf("spi nor flash is not in the slot.\n\r");    
		return -1;
	}
//	fatal_printf("size=%ld,sector size:%ld, sectors:%ld,page_size:%ld\n\r",size,info->sector_size,info->n_sectors,info->page_size);

	chip->info = info;
	chip->ops = (void *)&nor_ops;
	chip->table = cmd_cfg;

	if(!chip->ops->reset)
		return -1;
	ret = chip->ops->reset(chip);

	size = info->sector_size * info->n_sectors;
//	fatal_printf("size=%ld,sector size:%ld, sectors:%ld,page_size:%ld\n\r",size,info->sector_size,info->n_sectors,info->page_size);
		chip->addr_width = 3;

	fatal_printf("init done,ret:%d\n\r",ret);
//	return ret;
	return chip;
}

#if 1
void spi_nor_dump_buf(uint8_t * buffer, uint32_t size)
{
	int i;

	uart_printf("spi_nor_test dump ++\r\n");
	for(i=0;i<size;i++){
		if(i%0x20 == 0)uart_printf("\r\n[%0.8x]:",i);
		uart_printf("%0.2x,",buffer[i]);
	}
	uart_printf("\r\nspi_nor_test dump --");
}


void spi_nor_test(void)
{
#define READ_SIZE 0x1000
	int ret,i;
	uint8_t tmp_buf[READ_SIZE];

	asr3601s_spinor_flashinit();

//>>>>>>>>>>>>>>>>READ TEST
	uart_printf("spi_nor_test read ++\r\n");
	asr3601s_spinor_do_read(0x00,tmp_buf,READ_SIZE);
	uart_printf("spi_nor_test read --\r\n");
	spi_nor_dump_buf(tmp_buf,READ_SIZE);

//>>>>>>>>>>>>>>>>ERASE & READ
	uart_printf("spi_nor_test erase ++\r\n");
	asr3601s_spinor_do_erase_4k(0x00,0xfff);
	uart_printf("spi_nor_test erase --\r\n");
	asr3601s_spinor_do_read(0x00,tmp_buf,READ_SIZE);
	spi_nor_dump_buf(tmp_buf,READ_SIZE);

//>>>>>>>>>>>>>>>>WRITE & READ
	uart_printf("\r\nspi_nor_test write ++\r\n");
	memset(tmp_buf,0x18,READ_SIZE);
	asr3601s_spinor_do_write(0x00,tmp_buf,READ_SIZE);
	uart_printf("spi_nor_test write --\r\n");
	asr3601s_spinor_do_read(0x00,tmp_buf,READ_SIZE);
	spi_nor_dump_buf(tmp_buf,READ_SIZE);
}
#endif

struct spinor_chip_info *spichip=NULL;

int asr3601s_spinor_flashinit(void)
{

	uart_printf("function:%s\r\n",__func__);
	spichip=spinor_init(NULL,2);
	if (!spichip) {
		uart_printf("SPI Nor flash init failed\r\n");
	}
}


unsigned int asr3601s_spinor_do_erase(unsigned int addr, unsigned int size)
{

	int ret = 0;
	//uart_printf("function:%s,offset:0x%x,len:0x%x\r\n",__func__,addr,size);

	ret = spichip->ops->erase_sector(spichip, addr, size);

	return 0;

}
unsigned int asr3601s_spinor_do_erase_4k(unsigned int addr, unsigned int size)
{

	int ret = 0;
	//uart_printf("function:%s,offset:0x%x,len:0x%x\r\n",__func__,addr,size);

	ret = spichip->ops->erase_sector(spichip, addr, size);
	return ret;

}
unsigned int asr3601s_spinor_do_erase_64k(unsigned int addr, unsigned int size)
{

	int ret = 0;
	//uart_printf("function:%s,offset:0x%x,len:0x%x\r\n",__func__,addr,size);

	ret = spichip->ops->erase_block(spichip, addr, size);
	return ret;

}

unsigned int asr3601s_spinor_do_read(unsigned int addr, unsigned int buf_addr, unsigned int size)
{
	int ret = 0;
//	CP_LOGD("function:%s,offset:0x%x,buf_addr:0x%x,size:0x%x\r\n",__func__,addr,buf_addr,size);
//	CP_LOGD("spinor_read:0x%x\r\n",spinor_read);
//	CP_LOGD("spichip:0x%x\r\n",spichip);
//	CP_LOGD("spichip->ops:0x%x\r\n",spichip->ops);
//	CP_LOGD("spichip->ops->read:0x%x\r\n",spichip->ops->read);
	ret = spichip->ops->read(spichip, addr, size, (uint8_t *)buf_addr);

	return ret;
}

unsigned int asr3601s_spinor_do_write(unsigned int addr, unsigned int buf_addr,unsigned int size)
{
	int ret = 0;
	//uart_printf("function:%s,offset:0x%x,len:0x%x\r\n",__func__,addr,size);
	ret = spichip->ops->write(spichip, addr, size, (uint8_t *)buf_addr);

	return ret;

}


#else
#include "Typedef.h"
#include "Errors.h"
//#include "bsp.h"
#include "PlatformConfig.h"
#include "NSSP.h"
#include <string.h>
#include "common.h"

//#include "global_types.h"
//#include "timer.h"

#include "ssp_host.h"
#include "spi_nor.h"
#include "SSP.h"
//#include "diag_API.h"

#define fatal_printf uart_printf

extern int ssp_index;
extern int ssp_init_clk;
struct spinor_chip_info spinor_chip = {0};

/* NOTE: double check command sets and memory organization when you add
 * more nor chips.  This current list focusses on newer chips, which
 * have been converging on command sets which including JEDEC ID.
 *
 * All newly added entries should describe *hardware* and should use SECT_4K
 * (or SECT_4K_PMC) if hardware supports erasing 4 KiB sectors. For usage
 * scenarios excluding small sectors there is config option that can be
 * disabled: CONFIG_MTD_SPI_NOR_USE_4K_SECTORS.
 * For historical (and compatibility) reasons (before we got above config) some
 * old entries may be missing 4K flag.
 */
void spi_irq_handler(void)
{

}

void spi_master_init(void)
{
    ssp_port_init(0, 2, 1);
	ssp_init_master(0, 2, 1);
}

void spi_slave_init(void)
{
    ssp_port_init(2, 2, 0);
	ssp_init_master(2, 2, 0);
}
UINT32 spi_rx_buffer[64];
UINT32 spi_tx_buffer[64];

void spi_rw_test_init(void)
{
	int i;

	spi_master_init();
	spi_slave_init();
	
    spi_disable_ssp(2);
    spi_disable_ssp(0);
    spi_xmit_config(2, SPI_DSS32); 
    spi_xmit_config(0, SPI_DSS32); 
	ssp_config_master(2, 0);
    spi_xmit_start(0);
	ssp_port(2)->TOP_CTRL = 0x143f8;
    ssp_port(2)->FIFO_CTRL = 0xa0f6;
	ssp_port(2)->TOP_CTRL |= SSP_SSE;
	for(i = 0;i< 64;i++)
	{
		spi_rx_buffer[i] = 0;
		spi_tx_buffer[i] = 0xaabbcc00+i;
	}
}


UINT32 spi_read_rx_buffer[64];
UINT32 spi_read_tx_buffer[64];


void enableSlaveSSP(void)
{
	ssp_port(2)->TOP_CTRL |= SSP_SSE;
}

void slaveRxTx(UINT32* RXaddr, UINT32* TXaddr, UINT32 length)
{
	UINT32 i = 0;
	UINT32 dummy = 0;
	 
	while(length--)
	{		
		//fatal_printf("slave w\r\n");
		if(TXaddr)
			ssp_write_word(2, *(TXaddr+i));
		else
			ssp_write_word(2, 0xbbbbbbbb);
		//fatal_printf("slave r\r\n");
					
		if(RXaddr)
		{
			*(RXaddr+i) = ssp_read_word(2);
			//fatal_printf("rrx %ld",*(RXaddr+i));
		}
		else
			dummy = ssp_read_word(2);
		i++;
		
		//fatal_printf("slave done\r\n");
	}
}


void masterRxTx(UINT32* RXaddr, UINT32* TXaddr, UINT32 length)
{
	UINT32 i = 0;
	UINT32 dummy = 0;
	while(length--)
	{
		mdelay(20);
		
		if(TXaddr){
			//fatal_printf("mtx %ld",*(TXaddr+i));
			ssp_write_word(0, *(TXaddr+i));
		}
		else
			ssp_write_word(0, 0xaaaaaaaa);
			
		if(spi_wait_complete(0))
			fatal_printf("tx timeout\r\n");
		
		if(RXaddr)
			*(RXaddr+i) = ssp_read_word(0);
		else
			dummy = ssp_read_word(0);
		i++;
	}
	fatal_printf("MASTER RXTX done\r\n");
}
int spi_read_write_word(unsigned int* read, unsigned int *write)
{
   ssp_write_word(2,*write);
   *read = ssp_read_word(2);
}


extern __align(32) UINT8 P1dmaDesc[(SSP_READ_DMA_DESC*sizeof(DMA_DESCRIPTOR) +16)];
extern __align(32) UINT8 P2dmaDesc[(SSP_READ_DMA_DESC*sizeof(DMA_DESCRIPTOR) +16)];

int spi_dma_transfer(SSP_Port port_index,void *pRxBuffer, void *pTxBuffer, UINT32 numberOfElements)
{
    uint8_t dummy;
    int rx_len = numberOfElements, tx_len= numberOfElements;
    uint8_t *tx_buffer = pTxBuffer;
	uint8_t *rx_buffer = pRxBuffer;
    uint32_t *p1, *p2;
	DMA_DESCRIPTOR* RX_Desc = NULL;
	DMA_DESCRIPTOR* TX_Desc = NULL;

    int i;
    //spi_xmit_stop(port_index);
	spi_xmit_stop(port_index);
	ssp_port(port_index)->TOP_CTRL = 0x143f8;
	ssp_port(port_index)->FIFO_CTRL = 0x1a3f8;
	ssp_port(port_index)->INT_EN = 0;
	ssp_port(port_index)->RWOT_CTRL = 0;

	
    #define TOP_CTRL_TRAIL_DMA(reg)               		(   (reg) = ((reg) | BIT_13)        )
    TOP_CTRL_TRAIL_DMA(ssp_port(port_index)->TOP_CTRL);
   // p1 = malloc(SSP_READ_DMA_DESC*sizeof(DMA_DESCRIPTOR) +16);
	//p2 = malloc(SSP_READ_DMA_DESC*sizeof(DMA_DESCRIPTOR) +16);
	
	RX_Desc = (DMA_DESCRIPTOR*)(((uint32_t)P1dmaDesc & 0xFFFFFFF0) + 0x10);
	TX_Desc = (DMA_DESCRIPTOR*)(((uint32_t)P2dmaDesc & 0xFFFFFFF0) + 0x10);
    //use DMA for this
    DMA_CMDx_T tx_cmd, rx_cmd;
    tx_cmd.value = 0;
    tx_cmd.bits.IncSrcAddr = 1;
    tx_cmd.bits.IncTrgAddr = 0;
    tx_cmd.bits.FlowSrc = 0;
    tx_cmd.bits.FlowTrg = 1;
    tx_cmd.bits.Width = 3;
    tx_cmd.bits.MaxBurstSize = 3;
    tx_cmd.bits.Length = rx_len*4;

    rx_cmd.value = 0;
    rx_cmd.bits.IncSrcAddr = 0;
    rx_cmd.bits.IncTrgAddr = 1;
    rx_cmd.bits.FlowSrc = 1;
    rx_cmd.bits.FlowTrg = 0;
    rx_cmd.bits.Width = 3;
    rx_cmd.bits.MaxBurstSize = 3;
    rx_cmd.bits.Length = rx_len*4;
	//fatal_printf("%x,%x,%x,%x\r\n",tx_buffer[0],tx_buffer[1],tx_buffer[2],tx_buffer[3]);
	
    extern void sspRegDump(int port);
    //sspRegDump(2);
#if 1
    ssp_map_channel(port_index, SSP_DMA_TX_CHANNEL, SSP_DMA_RX_CHANNEL);
    dma_config_descriptor(&RX_Desc[0], NULL, &(ssp_port(port_index)->DATAR), rx_buffer, rx_cmd.value, 1);
	dma_config_descriptor(&TX_Desc[0], NULL, tx_buffer, &(ssp_port(port_index)->DATAR), tx_cmd.value, 1);
	dma_load_descriptor (&RX_Desc[0], SSP_DMA_RX_CHANNEL);
	dma_load_descriptor (&TX_Desc[0], SSP_DMA_TX_CHANNEL);

    spi_dma_enable(port_index);
    //spi_dss_switch(port_index, SPI_DSS32); 
    ssp_port(port_index)->FIFO_CTRL = 0xacf6;
	ssp_port(port_index)->TO = 0x800;
    spi_xmit_start(port_index);
    //fatal_printf("TX FIFO: 0x%x\n\r",  ssp_port(2)->FIFO_CTRL);
    dma_xfer_start(SSP_DMA_TX_CHANNEL);
    dma_xfer_start(SSP_DMA_RX_CHANNEL);
    
	//uart_printf(" 6\n");
    //ssp_unmap_channel(2);
    
	//uart_printf(" 7\n");
   // free(p1);
    //free(p2);
#endif
    return 0;
}
void dmaWaitDone(void)
{
	//uart_printf(" 4\n");
    dma_wait_done(SSP_DMA_TX_CHANNEL, 10000000, DMA_STOP);
    
	//uart_printf(" 5\n");
    dma_wait_done(SSP_DMA_RX_CHANNEL, 10000000, DMA_STOP);
    
	//uart_printf("spi_dma_transfer done\n");

}
#endif
