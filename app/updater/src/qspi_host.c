#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "qspi_dma.h"
#include "qspi_flash.h"
#include "qspi_host.h"
#include "qspi_nor.h"




#define uart_printf(...)



struct qspi_host qspi_host;
__align(16) DMA_DESCRIPTOR dma_desc;
P_DMA_DESCRIPTOR tx_desc = &dma_desc;


void *qspi_memcpy(void *dst, const void *src, int num)
{
	char * psrc = (char *)src;
	char * pdst = (char *)dst;

	while (num-- > 0) *pdst++ = *psrc++;
	return dst;
}

void _writel(unsigned int data , unsigned int addr )
{
	*(volatile unsigned int*)(addr)=data;
}

unsigned int _readl(unsigned int addr)
{

	return *(volatile unsigned int*)(addr);
}

void udelay(unsigned int us)
{
	volatile UINT32 i;
	for(i=0; i<us*60;i++);

}

void dcache_clean_invalidate_range(unsigned int addr, unsigned int len)
{

}



static void qspi_config_mfp(int cs)
{
	if (cs == QSPI_CS_A1 || cs == QSPI_CS_A2) {
		_writel(0x1000, 0xd401e2c4); // QSPI_DAT3
		_writel(0x1000, 0xd401e2c8); // QSPI_DAT2
		_writel(0x1000, 0xd401e2cc); // QSPI_DAT1
		_writel(0x1000, 0xd401e2d0); // QSPI_DAT0
		_writel(0x1000, 0xd401e2d4); // QSPI_CLK
		_writel(0x1000, 0xd401e2d8); // QSPI_CS1
		_writel(0x1000, 0xd401e2dc); // QSPI_CS2
	}

	if (cs == QSPI_CS_B1 || cs == QSPI_CS_B2) {
#if 0
		_writel(0x1007, 0xd401e2e8); // QSPI2_DAT3
		_writel(0x1007, 0xd401e2ec); // QSPI2_DAT2
		_writel(0x1007, 0xd401e2f0); // QSPI2_DAT1
		_writel(0x1007, 0xd401e2f4); // QSPI2_DAT0
		_writel(0x1007, 0xd401e2f8); // QSPI2_CLK
		_writel(0x1007, 0xd401e2fc); // QSPI2_CS1
		_writel(0x1007, 0xd401e300); // QSPI2_CS2
#else
		_writel(0x1007, 0xd401e2ec); // QSPI2_DAT3
		_writel(0x1007, 0xd401e2f0); // QSPI2_DAT2
		_writel(0x1007, 0xd401e2f4); // QSPI2_DAT1
		_writel(0x1007, 0xd401e2f8); // QSPI2_DAT0
		_writel(0x1007, 0xd401e2fc); // QSPI2_CLK
		_writel(0x1007, 0xd401e300); // QSPI2_CS1
		_writel(0x1007, 0xd401e2e8); // QSPI2_CS2
#endif
	}
}


static void qspi_write_rbct(struct qspi_host *host, uint32_t val)
{
	uint32_t fr;
	(void) host;

	do {
		_writel(val, QSPI0_RBCT);
		fr = _readl(QSPI0_FR);
		if (!(fr & QSPI0_FR_IPIEF))
			break;
		fr &= QSPI0_FR_IPIEF;
		_writel(fr, QSPI0_FR);
	} while (1);
}

static void qspi_write_sfar(struct qspi_host *host, uint32_t val)
{
	uint32_t fr;
	(void) host;

	do {
		_writel(val, QSPI0_SFAR);
		fr = _readl(QSPI0_FR);
		if (!(fr & QSPI0_FR_IPIEF))
			break;

		fr &= QSPI0_FR_IPIEF;
		_writel(fr, QSPI0_FR);
	} while (1);
}



static void qspi_enter_mode(uint32_t mode)
{
	uint32_t mcr;

	mcr = _readl(QSPI0_MCR);
	if (mode == QSPI_NORMAL_MODE)
		mcr &= ~(0x1 << 14);
	else if (mode == QSPI_DISABLE_MODE)
		mcr |= (0x1 << 14);
	_writel(mcr, QSPI0_MCR);
}

void qspi_init_ahb(int lut, int page_size)
{
#if 0
	//Top index of BUFs
	_writel(0x20, QSPI0_BUF0IND);
	_writel(0x40, QSPI0_BUF1IND);
	_writel(0x60, QSPI0_BUF2IND);

	//128Byte, master ID?
	_writel(0x00001001, QSPI0_BUF0CR);
	_writel(0x1006, QSPI0_BUF1CR);
	_writel(0x1003, QSPI0_BUF2CR);
	_writel(0x80001002, QSPI0_BUF3CR);
#else
	uint32_t buf_cfg;
	int data_size;
	int i = 0;

	do {
		i++;
		data_size = page_size/(1 << i);
		if (data_size <= QSPI_AHB_BUFF_MAX_SIZE)
			break;
	} while(1);

	buf_cfg = 0x1 << 31 | (data_size / 8) << 8;

	/*
	 * Config the ahb buffer
	 * Disable BUF0~BUF1, use BUF3 for all masters
	 */
	_writel(0, QSPI0_BUF0IND);
	_writel(0, QSPI0_BUF1IND);
	_writel(0, QSPI0_BUF2IND);

	/* AHB Master port */
	_writel(0xe, QSPI0_BUF0CR);
	_writel(0xe, QSPI0_BUF1CR);
	_writel(0xe, QSPI0_BUF2CR);
	_writel(buf_cfg, QSPI0_BUF3CR); // other masters
#endif
	_writel(lut << 12, QSPI0_BFGENCR);
	uart_printf("AHB data transfer size: %d\r\n", data_size);
}

static void qspi_lock_lut(void)
{
	uint32_t lckcr;

	lckcr = _readl(QSPI0_LCKCR);
	if (lckcr & 0x1)
		return;
	_writel(0x5af05af0, QSPI0_LUTKEY);
	_writel(0x1, QSPI0_LCKCR);
}

static void qspi_unlock_lut(void)
{
	uint32_t lckcr;

	lckcr = _readl(QSPI0_LCKCR);
	if (lckcr & 0x2)
		return;

	_writel(0x5af05af0, QSPI0_LUTKEY);
	_writel(0x2, QSPI0_LCKCR);
}

void qspi_config_lookup_tbl(struct spi_flash_chip *chip,
				   struct spi_flash_cmd_cfg *cmd_cfg,
				   uint8_t mode, int seq_id)
{
	uint32_t lut_value;
	uint16_t lut_entry[8];
	uint8_t pins[] = {0, QSPI_PAD_1X, QSPI_PAD_2X, 0, QSPI_PAD_4X};
	uint8_t seq = 0, i;
	uint8_t mode_instr, dummy_cycles, addr_bytes;

	/* Set Lookup table entry: CMD, ADDR, MODE, DUMMY, DATA, etc */
	if (chip->qpi_enabled)
		lut_entry[seq++] = QSPI_INSTR_CMD << 10 |
				   QSPI_PAD_4X << 8 | cmd_cfg->opcode;
	else
		lut_entry[seq++] = QSPI_INSTR_CMD << 10 |
				   QSPI_PAD_1X << 8 | cmd_cfg->opcode;

	if (cmd_cfg->addr_bytes) {
		if (chip->en_addr_4byte && cmd_cfg->addr_bytes == 3)
			addr_bytes = 4;
		else
			addr_bytes = cmd_cfg->addr_bytes;

		lut_entry[seq++] = QSPI_INSTR_ADDR << 10 |
				   pins[cmd_cfg->addr_pins] << 8 |
				   addr_bytes*8;
		//if (addr_bytes == 1)
		//	lut_entry[seq - 1] |= 0x4000;
	}

	if (cmd_cfg->mode_bits) {
		if (cmd_cfg->mode_bits == 2) {
			mode_instr = QSPI_INSTR_MODE2;
			mode &= 0x3;
		} else if (cmd_cfg->mode_bits == 4) {
			mode_instr = QSPI_INSTR_MODE4;
			mode &= 0xf;
		} else {
			mode_instr = QSPI_INSTR_MODE;
		}
		lut_entry[seq++] = mode_instr << 10 |
				   pins[cmd_cfg->mode_pins] << 8 |
				   mode;
	}

	if (cmd_cfg->dummy_pins) {
		if (chip->qpi_enabled)
			dummy_cycles = chip->qpi_dummy;
		else
			dummy_cycles = cmd_cfg->dummy_cycles;
		lut_entry[seq++] = QSPI_INSTR_DUMMY << 10 |
				   pins[cmd_cfg->dummy_pins] << 8 |
				   dummy_cycles;
	}

	if (cmd_cfg->data_pins) {
		if (cmd_cfg->type == CMD_W_RX_DATA) {
			lut_entry[seq++] = QSPI_INSTR_READ << 10 |
					   pins[cmd_cfg->data_pins] << 8;
			/* Add JMP_ON_CS for read */
			lut_entry[seq++] = QSPI_INSTR_JMP_ON_CS << 10;

		} else if (cmd_cfg->type == CMD_W_TX_DATA) {
			lut_entry[seq++] = QSPI_INSTR_WRITE << 10 |
					pins[cmd_cfg->data_pins] << 8;
		} else {
			uart_printf("err: type of cmd %d is wrong in table\r\n",
				   cmd_cfg->opcode);
			return;
		}
	}

	/* Add stop at the end */
	lut_entry[seq++] = QSPI_INSTR_STOP << 10;
	for (i = 0; i < seq/2; i++) {
		lut_value = lut_entry[i*2] | (lut_entry[i*2 + 1] << 16);
		_writel(lut_value, QSPI0_LUT0 + seq_id*0x10 + i*0x4);
	}

	if (seq % 2) {
		lut_value =lut_entry[seq - 1];
		_writel(lut_value, QSPI0_LUT0 + seq_id*0x10 + (seq/2)*0x4);
	}

//	for (i = 0; i < seq; i++)
//		uart_printf("seq=%d, lut_entry[%d]=0x%x\n\r", seq, i, lut_entry[i]);
	return;
}

static int qspi_update_shared_lut(struct spi_flash_chip *chip,
				  struct spi_flash_cmd *cmd)
{
	struct spi_flash_cmd_cfg *cmd_cfg = cmd->cmd_cfg;
	uint32_t seq_id = QSPI_LUT_SEQID1;

	qspi_config_lookup_tbl(chip, cmd_cfg, cmd->mode, seq_id);
	return seq_id;
}

/*
 * Reserved for future optimization
 * Pre-init some lookup tables for special commands to accelerate
 */
int qspi_preinit_lookup_tbl(struct spi_flash_chip *chip)
{
	struct spi_flash_cmd_cfg *cmd_cfg = chip->table;
	int lut_map = 0;

	for (; cmd_cfg->opcode != 0x0; cmd_cfg++) {
		if (cmd_cfg->seq_id != 0xFF) {
			if (lut_map & 1 << cmd_cfg->seq_id) {
				uart_printf("err: LUT %d already used\r\n",
					cmd_cfg->seq_id);
				return -1;
			}

			qspi_config_lookup_tbl(chip, cmd_cfg, 0xff,cmd_cfg->seq_id);
			lut_map |= 1 << cmd_cfg->seq_id;

			uart_printf("config 0x%x, 0x%x\r\n",cmd_cfg->seq_id, lut_map);

			
		}
	}

	chip->host->lut_map = lut_map;
	uart_printf("Fixed LUT bit-map: 0x%x\r\n", lut_map);
	return 0;
}

void qspi_enable_ahb_preempt(int on)
{
	uint32_t mcr;

	mcr = _readl(QSPI0_MCR);
	if(on)
		mcr |= QSPI0_MCR_SW_PROG_ERASE;
	else
		mcr &= ~QSPI0_MCR_SW_PROG_ERASE;
	_writel(mcr, QSPI0_MCR);
}

void qspi_enter_xip_mode(int enable)
{
	uint32_t mcr;

	mcr = _readl(QSPI0_MCR);
	if (enable)
		mcr |= QSPI0_MCR_XIP_EN;
	else
		mcr &= ~(QSPI0_MCR_XIP_EN | QSPI0_MCR_SW_PROG_ERASE);
	_writel(mcr, QSPI0_MCR);
}


int qspi_enable_xip(struct spi_flash_chip *chip,
		    struct spi_flash_cmd_cfg *cmd_cfg)
{
	struct qspi_host *host = chip->host;

	qspi_config_lookup_tbl(chip, cmd_cfg, 0xff, QSPI_LUT_SEQID0);

	qspi_enter_mode(QSPI_DISABLE_MODE);
	qspi_init_ahb(QSPI_LUT_SEQID0, chip->page_size);
	qspi_enter_mode(QSPI_NORMAL_MODE);

	if (host->use_xip) {
		host->xip_read = 1;
		uart_printf("XIP Read mode enabled\r\n");
	} else {
		host->xip_read = 0;
		uart_printf("IPS Read mode enabled\r\n");
	}

	return 0;
}

static void qspi_reset(void)
{
	uint32_t reg;

	/* qspi softreset first */
	reg = _readl(QSPI0_MCR);
	reg |= 0x3;
	_writel(reg, QSPI0_MCR);
	reg = _readl(QSPI0_MCR);
	if ((reg & 0x3) != 0x3)
		uart_printf("reset ignored 0x%x\r\n", reg);
	udelay(1);
	//cpu_cycle_delay(10);
	reg &= ~0x3;
	_writel(reg, QSPI0_MCR);
}


static void qspi_clk_enable(void)
{

//	PMUA->QSPI_CLK_RES_CTRL |= 0x1 << 4 | 0x1 << 3;;
//	PMUA->QSPI_CLK_RES_CTRL |= 0x3;

	UINT32 val;
	UINT32 QSPI_CLK_RES_CTRL;

	QSPI_CLK_RES_CTRL=0xD4282800+0x60;
	
	val=_readl(QSPI_CLK_RES_CTRL);

	val |= ((0x1 << 4) | (0x1 << 3));
	_writel(val,QSPI_CLK_RES_CTRL);

	val |= 0x3;
	_writel(val,QSPI_CLK_RES_CTRL);
	
	

	
}

static void qspi_clk_disable(void)
{
//	PMUA->QSPI_CLK_RES_CTRL &= ~(0x1 << 4 | 0x1 << 3);
//	PMUA->QSPI_CLK_RES_CTRL &= ~0x3;

	UINT32 val;
	UINT32 QSPI_CLK_RES_CTRL;

	QSPI_CLK_RES_CTRL=0xD4282800+0x60;

	val=_readl(QSPI_CLK_RES_CTRL);

	val &= ~(0x1 << 4 | 0x1 << 3);
	_writel(val,QSPI_CLK_RES_CTRL);

	val &= ~0x3;
	_writel(val,QSPI_CLK_RES_CTRL);


	
}


static void qspi_set_func_clk(struct qspi_host *host, int mhz)
{
	int clk_sel;
	int freq;

	UINT32 val;
	UINT32 QSPI_CLK_RES_CTRL;



	/* Default qspi clock is divided by 4 in PMU */
	freq = mhz << 2;
	freq *= 1000000;

	if (freq >= 416000000)
		clk_sel = QSPI_FUNC_CLK_416MHZ;
	else if (freq >= 312000000)
		clk_sel = QSPI_FUNC_CLK_312MHZ;
	else if (freq >= 208000000)
		clk_sel = QSPI_FUNC_CLK_208MHZ;
	else if (freq >= 156000000)
		clk_sel = QSPI_FUNC_CLK_156MHZ;
	else if (freq >= 104000000)
		clk_sel = QSPI_FUNC_CLK_104MHZ;
	else if (freq >= 78000000)
		clk_sel = QSPI_FUNC_CLK_78MHZ;
	else if (freq >= 52000000)
		clk_sel = QSPI_FUNC_CLK_52MHZ;
	else
		clk_sel = QSPI_FUNC_CLK_26MHZ;

	qspi_clk_disable();
//	PMUA->QSPI_CLK_RES_CTRL &= ~(0x7 << 6);
//	PMUA->QSPI_CLK_RES_CTRL |= clk_sel << 6;

	QSPI_CLK_RES_CTRL=0xD4282800+0x60;

	val=_readl(QSPI_CLK_RES_CTRL);

	val &= ~(0x7 << 6);
	_writel(val,QSPI_CLK_RES_CTRL);

	val |= clk_sel << 6;
	_writel(val,QSPI_CLK_RES_CTRL);

	
	qspi_clk_enable();

	host->bus_clk = mhz;
//	uart_printf("Bus clock: %dMHz QSPI_CLK_RES_CTRL: 0x%x\r\n",mhz, PMUA->QSPI_CLK_RES_CTRL);
}

static void qspi_enable_dma(struct qspi_host *host)
{

	uint32_t resr;

	resr = _readl(QSPI0_RSER);
	resr |= QSPI0_RSER_TBFDE;
	_writel(resr, QSPI0_RSER);
}

static void qspi_disable_dma(struct qspi_host *host)
{

	uint32_t resr;

	resr = _readl(QSPI0_RSER);
	resr &= ~QSPI0_RSER_TBFDE;
	_writel(resr, QSPI0_RSER);
}

static void qspi_enable_xip_on_int(struct qspi_host *host)
{
	uint32_t resr;


	resr = _readl(QSPI0_RSER);
	resr |= QSPI0_RSER_XIP_ON;
	_writel(resr, QSPI0_RSER);
}

static void qspi_disable_xip_on_int(struct qspi_host *host)
{
	uint32_t resr;


	resr = _readl(QSPI0_RSER);
	resr &= ~QSPI0_RSER_XIP_ON;
	_writel(resr, QSPI0_RSER);
}
static void qspi_config_interrupt(struct qspi_host *host)
{
	uint32_t resr;


	resr = _readl(QSPI0_RSER);
	resr |= QSPI0_RSER_ILLINIE | QSPI0_RSER_ABSEIE | QSPI0_RSER_AITIE |
		QSPI0_RSER_AIBSIE | QSPI0_RSER_ABOIE |QSPI0_RSER_IUEIE |
		QSPI0_RSER_IPIEIE | QSPI0_RSER_IPGEIE | QSPI0_RSER_XIP_ON;

	_writel(resr, QSPI0_RSER);
}
static void qspi_enable_interrupt(struct qspi_host *host)
{
	struct spi_flash_cmd *cmd = host->cmd;
	uint32_t resr;

	resr = _readl(QSPI0_RSER);

	resr |= QSPI0_RSER_TFIE;
	if (cmd->n_tx) {
		resr |= QSPI0_RSER_TBUIE;
		if (!host->use_dma && host->bytes_left > 0) {
			resr |= QSPI0_RSER_TBFIE;
		}
	} else if (cmd->n_rx) {
		resr |= QSPI0_RSER_RBOIE | QSPI0_RSER_RBDIE;
	}

	_writel(resr, QSPI0_RSER);
}

static void qspi_disable_interrupt(struct qspi_host *host)
{
	struct spi_flash_cmd *cmd = host->cmd;
	uint32_t resr;


	resr = _readl(QSPI0_RSER);

	resr &= ~QSPI0_RSER_TFIE;
	if (cmd->n_tx)
		resr &= ~(QSPI0_RSER_TBUIE | QSPI0_RSER_TBFIE);
	else if (cmd->n_rx)
		resr &= ~(QSPI0_RSER_RBOIE | QSPI0_RSER_RBDIE);

	if (host->use_dma)
		resr &= ~QSPI0_RSER_TBFDE;

	_writel(0, QSPI0_RSER);
}

static int qspi_fill_to_txbuff(struct qspi_host *host)
{
	struct spi_flash_cmd *cmd = host->cmd;
	int left_bytes = host->bytes_left;
	int total_cnt;
	uint32_t reg;
	uint32_t data;

	if (!cmd) {
		uart_printf("err: receive tx interrupt while no cmd sent\n");
		return 1;
	}

	total_cnt = (cmd->n_tx + QSPI_TX_BUFF_POP_MIN - 1) / QSPI_TX_BUFF_POP_MIN;
	total_cnt = total_cnt*QSPI_TX_BUFF_POP_MIN;

	while (left_bytes > 0) {
		_writel(QSPI0_FR_TBFF, QSPI0_SR);
		reg = _readl(QSPI0_SR);
		if (reg & QSPI0_FR_TBFF)
			break;

		if (cmd->n_tx - total_cnt + left_bytes <= 0) {
			data = 0;
		} else if (cmd->n_tx - total_cnt + left_bytes < 4) {
			data = 0;
			qspi_memcpy(&data, &cmd->tx_buf[total_cnt - left_bytes],
				left_bytes);
		} else {
			data = (*(uint32_t *)&cmd->tx_buf[total_cnt - left_bytes]);
		}
		_writel(data, QSPI0_TBDR);
		left_bytes -= 4;
	}

	host->bytes_left = left_bytes;
	if (left_bytes <= 0)
		return 0;
	return 1;
}

static void qspi_read_from_rxbuff(struct qspi_host *host)
{
	struct spi_flash_cmd *cmd = host->cmd;
	int left_bytes = host->bytes_left;
	uint32_t sr;
	uint32_t data;
	int i;

	while (left_bytes > 0) {
		sr = _readl(QSPI0_FR);
		if (!(sr & QSPI0_FR_RBDF))
			break;

		/* Check RXWE flag for data comming */
		for (i = 0; i <= host->wmrk; i++) {
			data = _readl(QSPI0_RBDR0 + i*4);
			//uart_printf("i=%d data=0x%x\r\n", i , data);
			if (left_bytes >= 4)
				qspi_memcpy(&cmd->rx_buf[cmd->n_rx - left_bytes],
					&data, 4);
			else
				qspi_memcpy(&cmd->rx_buf[cmd->n_rx - left_bytes],
					&data, left_bytes);
			left_bytes -= 4;
		}

		/* Set RBDF to trigger RX Buffer POP */
		_writel(0x1 << 16, QSPI0_FR);
	}

	host->bytes_left = left_bytes;
	return;
}

static void qspi_xfer_done(struct qspi_host *host)
{
	struct spi_flash_cmd *cmd = host->cmd;
	int left_bytes = host->bytes_left;
	uint32_t rdbfl;
	uint32_t data;
	uint32_t i;

	if (host->use_dma)
		left_bytes = 0;

	if (cmd->n_rx && left_bytes > 0) {
		rdbfl = _readl(QSPI0_RBSR);
		rdbfl = (rdbfl >> 8) & 0x3f;

		for (i = 0; i <= rdbfl; i++) {
			data = _readl(QSPI0_RBDR0 + i*4);
			if (left_bytes >= 4)
				qspi_memcpy(&cmd->rx_buf[cmd->n_rx - left_bytes],
					&data, 4);
			else
				qspi_memcpy(&cmd->rx_buf[cmd->n_rx - left_bytes],
					&data, left_bytes);
			left_bytes -= 4;
		}

		if (left_bytes > 0) {
			uart_printf("Error: Not read enough data: "
				"left_bytes=%d, cmd->n_rx=%d\r\n",
				left_bytes, cmd->n_rx);
		}
	}

	host->bytes_left = left_bytes;
	host->complete = 1;
	return;
}

static void qspi_irq_handler(void *data)
{
	struct qspi_host *host = &qspi_host;
	struct spi_flash_cmd *cmd = host->cmd;
	uint32_t fr, resr;
	(void)data;

	fr = _readl(QSPI0_FR);
	_writel(fr & ~QSPI0_FR_RBDF, QSPI0_FR);
	if (fr & QSPI0_FR_XIP_ON)
		uart_printf("XIP suspend/resume happen-----fr=0x%x->\r\n", fr);

	if (!cmd) {
		uart_printf("Interrupt happen while no cmd sent, fr=0x%x\r\n", fr);
		return;
	}

	resr = _readl(QSPI0_RSER);
	_writel(0x0, QSPI0_RSER);

	if (fr & (QSPI0_FR_ILLINE | QSPI0_FR_IUEF | QSPI0_FR_IPAEF |
		QSPI0_FR_IPIEF | QSPI0_FR_IPGEF |
		QSPI0_FR_RBOF | QSPI0_FR_TBUF)) {
		if (fr & QSPI0_FR_ILLINE)
			uart_printf("Err: Illegal Instruction Error Flag\r\n");
		if (fr & QSPI0_FR_IUEF)
			uart_printf("Err: IP Command Usage Error Flag\r\n");
		if (fr & QSPI0_FR_IPAEF)
			uart_printf("Err: IP Command Trigger during AHB Access Error Flag\r\n");
		if (fr & QSPI0_FR_IPIEF)
			uart_printf("Err: IP Command Trigger could not be executed Error Flag\r\n");
		if (fr & QSPI0_FR_IPGEF)
			uart_printf("Err: IP Command Trigger during AHB Grant Error Flag\r\n");
		if (fr & QSPI0_FR_RBOF)
			uart_printf("Error: RX Buffer Overflow\r\n");
		if (fr & QSPI0_FR_TBUF)
			uart_printf("Error: TX Buffer Underrun Flag\r\n");
		host->complete = 1;		
	}

	if (!host->use_dma && host->bytes_left > 0) {
		if (cmd->n_tx && (fr & QSPI0_FR_TBFF)) {
			if (!qspi_fill_to_txbuff(host))
				resr &= ~QSPI0_RSER_TBFIE;
		}
		if (cmd->n_rx && (fr & QSPI0_FR_RBDF))
			qspi_read_from_rxbuff(host);
	}

	if (fr & QSPI0_FR_TFF)
		qspi_xfer_done(host);

	_writel(resr, QSPI0_RSER);
	return;
}


struct qspi_host * qspi_host_init(int cs, int mhz, int use_xip)
{
	struct qspi_host *host = &qspi_host;
	uint32_t reg;

	memset(host, 0, sizeof(struct qspi_host));
	host->cs_addr[QSPI_CS_A1] = QSPI0_FLASH_A1_BASE;
	host->cs_addr[QSPI_CS_A2] = QSPI0_FLASH_A2_BASE;
	host->cs_addr[QSPI_CS_B1] = QSPI0_FLASH_B1_BASE;
	host->cs_addr[QSPI_CS_B2] = QSPI0_FLASH_B2_BASE;

	host->use_intr = 0;
	host->en_tx_dma = 0;
	host->use_xip = use_xip;

	qspi_config_mfp(cs);

	/* Enable qspi clk, and release reset */
	qspi_set_func_clk(host, mhz);

	/* qspi softreset first */
	qspi_reset();

	/* clock settings */
	qspi_enter_mode(QSPI_DISABLE_MODE);
	/* sampled by sfif_clk_b; half cycle delay; */

	/* NOT SETTING SMPR[5] FOR QSPI @104M SINCE SOME FLASH DEVICE MAY GET STUCK ON INIT,
	KEEP PAYING ATTENTION TO THIS WHEN APPLYING CRANE A0 OR OTHER FLASH DEVICES.*/
#if 1 
    _writel(0x0, QSPI0_SMPR);
    _writel(0x8, QSPI0_SOCCR); //good for erase/write stability
#else
	if (mhz < 104)
		_writel(0x0, QSPI0_SMPR);
	else
		_writel(0x1 << 5, QSPI0_SMPR);
#endif		
	//_writel(0x202, QSPI0_FLSHCR);

	/* Give the default source address */
	qspi_write_sfar(host, host->cs_addr[QSPI_CS_A1]);
	_writel(0x0, QSPI0_SFACR);

	//qspi_init_ahb(0); /* config ahb */

	/* Set flash memory map */
	_writel(QSPI0_FLASH_A1_TOP & 0xfffffc00, QSPI0_SFA1AD);
	_writel(QSPI0_FLASH_A2_TOP & 0xfffffc00, QSPI0_SFA2AD);
	_writel(QSPI0_FLASH_B1_TOP & 0xfffffc00, QSPI0_SFB1AD);
	_writel(QSPI0_FLASH_B2_TOP & 0xfffffc00, QSPI0_SFB2AD);

	/*
	 * ISD3FB, ISD2FB, ISD3FA, ISD2FA = 1; ENDIAN = 0x3; END_CFG=0x3 
	 * DELAY_CLK4X_EN = 1
	 */
	reg = _readl(QSPI0_MCR);
	reg &= ~0x000f000c;
	reg |= 0x000f000c;
	_writel(reg, QSPI0_MCR);

	/* Module enabled */
	qspi_enter_mode(QSPI_NORMAL_MODE);

	/* Read using the IP Bus registers QSPI_RBDR0 to QSPI_RBDR31*/
	qspi_write_rbct(host, 0x1 << 8);

	if (host->use_intr) {
		uint32_t fr;

//		ISR_Connect(45, qspi_irq_handler, 0);
//		INT_Enable(45, 0, 15);

		fr = _readl(QSPI0_FR);
		if (fr)
			_writel(fr, QSPI0_FR);

		qspi_config_interrupt(host);
	}

    /*
    uart_printf("tx_desc: 0x%x\r\n", tx_desc);
	uart_printf("use_intr=%d en_tx_dma=%d use_xip=%d\r\n",
		   host->use_intr, host->en_tx_dma, host->use_xip);
    */
	return host;
}

static void qspi_wait_cmd_done(struct qspi_host *host)
{
	struct spi_flash_cmd *cmd = host->cmd;
	uint32_t fr;

	do {
		fr = _readl(QSPI0_FR);
		if (fr & QSPI0_FR_ILLINE) {
			uart_printf("Err: Illegal Instruction Error Flag\r\n");
			break;
		}

		if (fr & QSPI0_FR_IUEF) {
			uart_printf("Err: IP Command Usage Error Flag\r\n");
			break;
		}

		if (fr & QSPI0_FR_IPAEF) {
			uart_printf("Err: IP Command Trigger during AHB Access Error Flag\r\n");
			break;
		}

		if (fr & QSPI0_FR_IPIEF) {
			uart_printf("Err: IP Command Trigger could not be executed Error Flag\r\n");
			break;
		}

		if (fr & QSPI0_FR_IPGEF) {
			uart_printf("Err: IP Command Trigger during AHB Grant Error Flag\r\n");
			break;
		}

		if (fr & QSPI0_FR_TFF) {
			_writel(0x1, QSPI0_FR);
			break;
		}

	} while(1);

	if (cmd->rx_buf && (fr & QSPI0_FR_RBOF))
		uart_printf("Error: RX Buffer Overflow\r\n");
	if (cmd->tx_buf && (fr & QSPI0_FR_TBUF))
		uart_printf("Error: TX Buffer Underrun Flag\r\n");
}

static void qspi_poll_rx_buff(struct qspi_host *host)
{
	struct spi_flash_cmd *cmd = host->cmd;
	int left_bytes = host->bytes_left;
	uint32_t sr;
	uint32_t data;
	int rdbfl, i;

	do {
		/* Check RXWE flag for data comming */
		sr = _readl(QSPI0_FR);
		if (!(sr & (0x1 << 16)))
			continue;

		for (i = 0; i <= host->wmrk; i++) {
			data = _readl(QSPI0_RBDR0 + i*4);
			if (left_bytes >= 4)
				qspi_memcpy(&cmd->rx_buf[cmd->n_rx - left_bytes],
					&data, 4);
			else
				qspi_memcpy(&cmd->rx_buf[cmd->n_rx - left_bytes],
					&data, left_bytes);
			left_bytes -= 4;
		}

		/* Set RBDF to trigger RX Buffer POP */
		_writel(0x1 << 16, QSPI0_FR);

		if (left_bytes < 0) {
			break;
		} else if ((left_bytes + 3)/4 < (host->wmrk + 1)) {
			/* Left bytes < wmrk will not trigger RXWE */
			break;
		}
	} while(1);

	/* Wait cmd to be finished */
	qspi_wait_cmd_done(host);
	if (left_bytes > 0) {
		rdbfl = _readl(QSPI0_RBSR);
		rdbfl = (rdbfl >> 8) & 0x3f;
		for (i = 0; i <= rdbfl; i++) {
			data = _readl(QSPI0_RBDR0 + i*4);
			if (left_bytes >= 4)
				qspi_memcpy(&cmd->rx_buf[cmd->n_rx - left_bytes],
					&data, 4);
			else
				qspi_memcpy(&cmd->rx_buf[cmd->n_rx - left_bytes],
					&data, left_bytes);
			left_bytes -= 4;
		}
	}

	if (left_bytes > 0) {
		uart_printf("Error: Not read enough data: left_bytes=%d, cmd->n_rx=%d\r\n",
			left_bytes, cmd->n_rx);
	}
	return;
}

static void qspi_fill_tx_buff(struct qspi_host *host)
{
	struct spi_flash_cmd *cmd = host->cmd;
	int left_bytes = host->bytes_left;
	int total_cnt;
	uint32_t reg;
	uint32_t data;

	total_cnt = (cmd->n_tx + QSPI_TX_BUFF_POP_MIN - 1) / QSPI_TX_BUFF_POP_MIN;
	total_cnt = total_cnt*QSPI_TX_BUFF_POP_MIN;

	while (left_bytes > 0) {
		reg = _readl(QSPI0_SR);
		if (reg & (0x1 << 27))
			continue;

		if (cmd->n_tx - total_cnt + left_bytes <= 0) {
			data = 0;
		} else if (cmd->n_tx - total_cnt + left_bytes < 4) {
			data = 0;
			qspi_memcpy(&data, &cmd->tx_buf[total_cnt - left_bytes],
				left_bytes);
		} else {
			data = (*(uint32_t *)&cmd->tx_buf[total_cnt - left_bytes]);
		}
		_writel(data, QSPI0_TBDR);
		left_bytes -= 4;
	}

	host->bytes_left = left_bytes;
	qspi_wait_cmd_done(host);
	return;
}

int qspi_cmd_done_pio(struct qspi_host *host)
{
	struct spi_flash_cmd *cmd = host->cmd;

	/* receive rx data */
	if (cmd->n_rx)
		qspi_poll_rx_buff(host);
	else if (cmd->n_tx)
		qspi_fill_tx_buff(host);
	else
		qspi_wait_cmd_done(host);

	return 0;
}

int qspi_cmd_done_interrupt(struct qspi_host *host)
{
	while (!(host->complete));

	qspi_disable_interrupt(host);
	return 0;
}

int qspi_start_dma_xfer(struct qspi_host *host)
{
	struct spi_flash_cmd *cmd = host->cmd;
	DMA_CMDx_T TX_data;

	TX_data.value = 0;
	TX_data.bits.IncSrcAddr = 1;
	TX_data.bits.FlowTrg = 1;
	TX_data.bits.Width = 3;
	TX_data.bits.MaxBurstSize = 3;
	TX_data.bits.Length = host->bytes_left;

	dma_map_device_to_channel(99, QSPI_DMA_TX_CHANNEL); //TX
	/* alignment is handled in qspi, should not set this in DMAC */
	//set_user_aligment(QSPI_DMA_TX_CHANNEL);

#if 0
	dma_config_descriptor((uint32_t *)tx_desc, 0, (uint32_t)cmd->tx_buf,
				QSPI0_TBDR, TX_data.value, 1);
	dma_load_descriptor((void *)tx_desc, QSPI_DMA_TX_CHANNEL);
#else
	dma_set_mode(DMA_MODE_NONFETCH, QSPI_DMA_TX_CHANNEL);
	dma_set_reg_nf((uint32_t)cmd->tx_buf, QSPI0_TBDR, &TX_data,
			QSPI_DMA_TX_CHANNEL);
#endif

	dcache_clean_invalidate_range((uint32_t)cmd->tx_buf, cmd->n_tx);
	dma_xfer_start(QSPI_DMA_TX_CHANNEL);
	return 0;
}

static int __qspi_start_cmd(struct spi_flash_chip *chip,
				struct spi_flash_cmd *cmd)
{
	struct qspi_host *host = chip->host;
	struct spi_flash_cmd_cfg *cmd_cfg = cmd->cmd_cfg;
	uint32_t ipcr, sfar, mcr, sr, fr;
	uint32_t tmp, data, cpsr;
	int seq_id = cmd_cfg->seq_id;
	int left_bytes, tx_cnt, i;
	int wmrk = 0;

	host->cmd = cmd;
	host->bytes_left = 0;
	host->complete = 0;
	host->use_dma = 0;

//	if (seq_id < 0 || !(host->lut_map & 1 << seq_id))
    if ((seq_id == 0xff) || (!(host->lut_map & (1 << seq_id))))
	{
//		uart_printf("qspi_update_shared_lut 0x%x 0x%x\r\n", seq_id, (host->lut_map & 1 << seq_id));
		seq_id = qspi_update_shared_lut(chip, cmd);
	}

	/* Reset the IP sequence pointers */
	tmp = _readl(QSPI0_SPTRCLR);
	tmp |= 0x1 << 8;
	_writel(tmp, QSPI0_SPTRCLR);

	if (cmd->n_rx) {
		/* Clear RX FIFO. Invalidate the RX Buffer */
		mcr = _readl(QSPI0_MCR);
		mcr |= 0x1 << 10;  //CLR_RXF
		_writel(mcr, QSPI0_MCR);

		/* Set RX Buffer Watermark */
		if (cmd->n_rx <= (QSPI_RX_BUFF_MAX*4))
			wmrk = (cmd->n_rx + 3)/4 - 1;
		else
			wmrk = 0x1; /* Water Mark: 16*4byte */
		tmp = _readl(QSPI0_RBCT);
		tmp &= ~0x1f;
		tmp |= wmrk;
		qspi_write_rbct(host, tmp);
		host->wmrk = wmrk;
		host->bytes_left = cmd->n_rx;
	} else if (cmd->n_tx) {
		/* Clear TX FIFO/Buffer */
		mcr = _readl(QSPI0_MCR);
		mcr |= 0x1 << 11;  //CLR_TXF
		_writel(mcr, QSPI0_MCR);
		/* Set QSPI_SFACR[CAS] QSPI_SFACR[WA] if needed */

		tx_cnt = (cmd->n_tx + QSPI_TX_BUFF_POP_MIN - 1) / QSPI_TX_BUFF_POP_MIN;
		host->bytes_left = tx_cnt*QSPI_TX_BUFF_POP_MIN;

		if (host->en_tx_dma && host->bytes_left >= 32) {
			host->use_dma = 1;
			wmrk = 0x7; //0x1; // 32bytes watermark
			tmp = _readl(QSPI0_TBCT);
			tmp &= ~0x1f;
			tmp |= wmrk;
			_writel(tmp, QSPI0_TBCT);
			host->wmrk = wmrk;
			qspi_enable_dma(host);
			qspi_start_dma_xfer(host);
#if 0
			/*
			 * Before trigger qspi to send data to externl bus,FIFO
			 * need to have some data, or FIFO underflow error may happen.
			 * DMA need some time to write data to TX FIFO, but
			 * poll QSPI register may lead to bus hang(known bug), so we add
			 * a delay here for this requirement.
			 */
			udelay(5);
			//cpu_cycle_delay(20);
#else
			while(1) {
				uint32_t tbsr;

				tbsr = _readl(QSPI0_TBSR);
				tbsr &= QSPI0_TBSR_TRBFL_MASK;
				tbsr = tbsr >> QSPI0_TBSR_TRBFL_SHIFT;
				if (tbsr >= min(host->bytes_left/4, QSPI_TX_BUFF_MAX))
					break;
				//uart_printf("====tbsr=%d\r\n", tbsr);
			}
#endif
		} else {
			/*
			 * Provide initial data for the program
			 * command into the circular buffer
			 */
			host->use_dma = 0;
			tx_cnt = (tx_cnt*QSPI_TX_BUFF_POP_MIN)/4;
			if (tx_cnt > QSPI_TX_BUFF_MAX)
				tx_cnt = QSPI_TX_BUFF_MAX;
			for (i = 0; i < tx_cnt; i++) {
				left_bytes = cmd->n_tx - i*4;
				if (left_bytes <= 0) {
					data = 0;
				} else if (left_bytes < 4) {
					data = 0;
					qspi_memcpy(&data, &cmd->tx_buf[i*4], left_bytes);
				} else {
					data = (*(uint32_t *)&cmd->tx_buf[i*4]);
				}
				_writel(data, QSPI0_TBDR);
			}
			host->bytes_left -= tx_cnt*4;
		}
	}

	/* Set flash address to be accessed */
	sfar = 0;
	for (i = 0; i < cmd->n_addr; i++) {
		sfar <<= 8;
		sfar |= cmd->addr[i];
	}
	sfar += host->cs_addr[chip->cs];
	qspi_write_sfar(host, sfar);

	if (cmd->flag & RST_AHB_DOMAIN) {
		int size;

		if (cmd->tx_buf)
			size = cmd->n_tx;
		else
			size = chip->block_size;
//		dcache_clean_invalidate_range(sfar, size);
	}

	/* trigger command */
	ipcr = (seq_id & 0xf) << 24;
	if (cmd->rx_buf)
		ipcr |= (cmd->n_rx & 0xffff);
	else if (cmd->tx_buf)
		ipcr |= (cmd->n_tx & 0xffff);


///    cpsr=disableInterrupts();


	do {
		sr = _readl(QSPI0_SR);
		if (sr & QSPI0_SR_BUSY)
			continue;

		fr = _readl(QSPI0_FR);
		if (fr & QSPI0_FR_XIP_ON)
			continue;

		/* Resume Done */
		_writel(QSPI0_FR_TFF, QSPI0_FR);

		/* trigger command */
		_writel(ipcr, QSPI0_IPCR);
		fr = _readl(QSPI0_FR);
		fr &= QSPI0_FR_IPAEF | QSPI0_FR_IPIEF | QSPI0_FR_IPGEF;
		if (!fr)
			break;

		_writel(fr, QSPI0_FR);
	} while(1);

	if (host->use_intr) {
		qspi_enable_interrupt(host);
		qspi_cmd_done_interrupt(host);
	} else {
		if (host->use_dma) {
			qspi_wait_cmd_done(host);
			qspi_disable_dma(host);
			host->bytes_left = 0;
		} else {
			qspi_cmd_done_pio(host);
		}
	}

//#ifndef CONFIG_XIP
//	if (cmd->flag & RST_AHB_DOMAIN) {
//		qspi_reset();
//	}
//#endif



	host->cmd = NULL;
	return 0;
}

int qspi_start_cmd(struct spi_flash_chip *chip, struct spi_flash_cmd *cmd)
{
#if 0
	struct spi_flash_cmd_cfg *cmd_cfg = cmd->cmd_cfg;
	int ret;

	do {
	ret = __qspi_start_cmd(chip, cmd);
		if (!ret || (ret != EAGAIN && ret != ETIMEDOUT))
			break;
		uart_printf("cmd%d error %d, retry...\r\n", cmd_cfg->opcode,ret);
	} while(1);

	return ret;
#else

	struct spi_flash_cmd_cfg *cmd_cfg = cmd->cmd_cfg;
	int ret;

	ret = __qspi_start_cmd(chip, cmd);

	return ret;

#endif
}
