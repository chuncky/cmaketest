#ifndef FOTA_ASRSPI
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "NSSP.h"
#include "PMUA.h"
#include "PMUM.h"
#include "SQU.h"
#include "DMA_SPI.h"

#include "common.h"
#include "ssp_host.h"

#define GPIO_SHIFT(gpio) (1 << (gpio%32))

#define GPIO_GROUP_BASE0 (0xD4019000)
#define GPIO_GROUP_BASE1 (0xD4019004)
#define GPIO_GROUP_BASE2 (0xD4019008)


#define REG_GPIO_SDR0 (GPIO_GROUP_BASE0+0x54)
#define REG_GPIO_SDR1 (GPIO_GROUP_BASE1+0x54)
#define REG_GPIO_SDR2 (GPIO_GROUP_BASE2+0x54)


#define MFPR_BASE (0xD401E000)
#define MFPR_GPIO12 (MFPR_BASE+0x10c)
#define MFPR_GPIO13 (MFPR_BASE+0x110)
#define MFPR_GPIO14 (MFPR_BASE+0x114)
#define MFPR_GPIO15 (MFPR_BASE+0x118)


#define MFPR_GPIO77 (MFPR_BASE+0x1D0)
#define MFPR_GPIO78 (MFPR_BASE+0x1D4)



#define REG_GPIO_PSR0 (GPIO_GROUP_BASE0+0x18)
#define REG_GPIO_PCR0 (GPIO_GROUP_BASE0+0x24)
#define REG_GPIO_PSR1 (GPIO_GROUP_BASE1+0x18)
#define REG_GPIO_PCR1 (GPIO_GROUP_BASE1+0x24)
#define REG_GPIO_PSR2 (GPIO_GROUP_BASE2+0x18)
#define REG_GPIO_PCR2 (GPIO_GROUP_BASE2+0x24)

//#include "cgpio.h"
#define fatal_printf uart_printf

//limited for timing and stack concerns (see SPI_page_write routine)
#define SSP_READ_TIME_OUT_MILLI		0x2000
#define SSP_READ_DMA_DESC			0x10	//total RX descriptors
#ifdef SPI_MUX
#define SSP_READ_DMA_SIZE			0x100	//bytes per descriptor
#else
#define SSP_READ_DMA_SIZE			0x1ff8	//bytes per descriptor
#endif
#define SSP_MAX_TX_SIZE_WORDS		64
#define SSP_MAX_TX_SIZE_BYTES		SSP_MAX_TX_SIZE_WORDS << 2
//#define SSP_CS_USE_GPIO

#define SYSCLK_EN                   (0x1<<31)
#define SYSCLK_GEN_156M             (1 << 30)
#define SYSCLK_GEN_26M              (0 << 30)
#define BITCLK_EN                   (1 << 29)
#define DIV_2                       (0 << 27)
#define DIV_4                       (1 << 27)
#define DIV_6                       (2 << 27)
#define DIV_8                       (3 << 27)
#define DENOM                       (1 << 15)
#define NOM                         (3 << 0)

#define SSP_DATA_16_BIT             (15 << 5)
#define SSP_DATA_8_BIT		        (7 << 5)
#define SSP_DATA_32_BIT		        (31 << 5)
#define SSP_DATA_18_BIT		        (17 << 5)
#define TIMEOUT                     100000

int ssp_index = 2;
int ssp_init_clk = 0;

void uudelay(int us)
{
	volatile UINT32 i;
	for(i=0; i<us*20;i++);

}

void mdelay(int ms){
	volatile UINT32 i;
	for(i=0; i<ms;i++)
		uudelay(1000);
}

unsigned int Endian_Convert (unsigned int in)
{
    unsigned int out;
    out = in << 24;
    out |= (in & 0xFF00) << 8;
    out |= (in & 0xFF0000) >> 8;
    out |= (in & 0xFF000000) >> 24;
    return out;
}

void spi_assert_cs(int port)
{
#ifdef SSP_CS_USE_GPIO
	//gpio_output_low(SSP_CS_GPIO);
	//GpioSetLevel(SSP_CS_GPIO, 0);

	BU_REG_WRITE(REG_GPIO_PCR0,GPIO_SHIFT(SSP_CS_GPIO));

#else
	ssp_port(port)->TOP_CTRL |= SSP_TCR_HFL;
#endif
}

void spi_deassert_cs(int port)
{
#ifdef SSP_CS_USE_GPIO
//	gpio_output_high(SSP_CS_GPIO);
	//GpioSetLevel(SSP_CS_GPIO, 1);
	BU_REG_WRITE(REG_GPIO_PSR0,GPIO_SHIFT(SSP_CS_GPIO));
#else
	ssp_port(port)->TOP_CTRL &= ~SSP_TCR_HFL;
#endif
}

void ssp_pinmux_cfg(int port, uint32_t gpio_flag)
{
    switch(port){
        case 0:
			//BU_REG_WRITE(0xd401e114, 0x1081);	//RX	GPIO_14
        	//BU_REG_WRITE(0xd401e118, 0x1081);	//TX	GPIO_15
        	//BU_REG_WRITE(0xd401e10C, 0x1081);	//CLK	GPIO_12
			BU_REG_WRITE(0xd401e168, 0x1081);	//RX	GPIO_35
        	BU_REG_WRITE(0xd401e16c, 0x1081);	//TX	GPIO_36
        	BU_REG_WRITE(0xd401e160, 0x1081);	//CLK	GPIO_33
        	BU_REG_WRITE(0xd401e164, 0x1081);	//FRM (CS)

        	//gpio_set_output(SSP_CS_GPIO);
        	//GpioSetDirection(SSP_CS_GPIO_SLAVE, GPIO_IN_PIN);
            break;
        case 1:
            break;
        case 2:
			
        	//BU_REG_WRITE(0xd401e1D4, 0x1081);	//Hold QSPI_DQM  GPIO_78
        	//BU_REG_WRITE(0xd401e1D0, 0x1081);	//WP   QSPI_CS2  GPIO_77

			BU_REG_WRITE(MFPR_GPIO14, 0x1081);	//RX	GPIO_14
        	BU_REG_WRITE(MFPR_GPIO15, 0x1081);	//TX	GPIO_15
        	BU_REG_WRITE(MFPR_GPIO12, 0x1081);	//CLK	GPIO_12

            if(gpio_flag){
#ifdef SSP_CS_USE_GPIO
        	    BU_REG_WRITE(MFPR_GPIO13, 0x1000);	//FRM (CS): GPIO_13
				BU_REG_WRITE(REG_GPIO_SDR0,GPIO_SHIFT(SSP_CS_GPIO));//set output of GPIO_13
				
#else
        	    BU_REG_WRITE(MFPR_GPIO13, 0x1081);	//FRM (CS)
#endif
            }
            else
				BU_REG_WRITE(MFPR_GPIO13, 0x1081);	//FRM (CS)

        	//gpio_set_output(SSP_CS_GPIO);
		//GpioSetDirection(SSP_CS_GPIO, GPIO_OUT_PIN);
			//GpioSetDirection(77, GPIO_OUT_PIN);
			//GpioSetDirection(78, GPIO_OUT_PIN); 
			//BU_REG_WRITE(MFPR_GPIO77, 0x1001);	//FRM (CS): GPIO_77
			//BU_REG_WRITE(MFPR_GPIO78, 0x1001);	//FRM (CS): GPIO_78
			
			//BU_REG_WRITE(REG_GPIO_SDR2,GPIO_SHIFT(77));//set output of GPIO_13
			//BU_REG_WRITE(REG_GPIO_SDR2,GPIO_SHIFT(78));//set output of GPIO_13
			//GpioSetLevel(77, 1); //enable SDCARD power.
			//GpioSetLevel(78, 1); //enable SDCARD power. 
			//BU_REG_WRITE(REG_GPIO_PSR2,GPIO_SHIFT(77));//set gpio77 =1
			//BU_REG_WRITE(REG_GPIO_PSR2,GPIO_SHIFT(78));// set gpio78=1
	
        default:
            break;
    }
}

void ssp_clk_cfg(int port, int clk)
{
	switch(port){
		case 0:
			APBClock->SSP0_CLK_RST = SSP0_CLK_RST_APBCLK|SSP0_CLK_RST_FNCLK|SSP0_CLK_RST_RST|(clk<<4);
			udelay(1);
			APBClock->SSP0_CLK_RST = SSP0_CLK_RST_APBCLK|SSP0_CLK_RST_FNCLK|(clk<<4);
			break;
		case 1:
			PMUA->SQU_CLK_GATE_CTRL &= ~(0x1<<30); //[30]=0: audio_island_clk selects aclk, default 266Mhz for Real-Core)
			SQU->CTRL_0 |= (0x7<<2);   //[4:2]=0x7: APB Clock Div is 7
			PMUM->FCCR &= ~(0x1<<28);  //[28]=0: i2s clock divider select 312Mhz; [28]=1: select VCTCXO(default 26Mhz); 
			//Step1: disable sysclk and bitclck to set the related parameters
			PMUM->ISCCRX[1] = SYSCLK_GEN_26M | DIV_8 | DENOM | NOM;
			 //Step2: enable sysclk and bitclk
			PMUM->ISCCRX[1] |= (SYSCLK_EN | BITCLK_EN);
			//Step3: wait a little time here for new setting clock div valid. if we do step1&step2 in one step, the previous
			//DENOM & NOM setting will influnce the new clock div, which may need wait much more time for the new setting
			//clock div valid(500ns VS 10,000ns)
			udelay(1);

			APBClock->SSP1_CLK_RST = SSP1_CLK_RST_APBCLK|SSP1_CLK_RST_FNCLK|SSP1_CLK_RST_RST;
			udelay(1);
			APBClock->SSP1_CLK_RST = SSP1_CLK_RST_APBCLK|SSP1_CLK_RST_FNCLK;
			break;
		case 2:
			APBClock->SSP2_CLK_RST = SSP2_CLK_RST_APBCLK|SSP2_CLK_RST_FNCLK|SSP2_CLK_RST_RST|(clk<<4);
			udelay(1);
			APBClock->SSP2_CLK_RST = SSP2_CLK_RST_APBCLK|SSP2_CLK_RST_FNCLK|(clk<<4);
		default:
			break;
	}
}

void ssp_port_init(int port, int clk, uint32_t gpio_flag)
{
	ssp_pinmux_cfg(port, gpio_flag);
	ssp_clk_cfg(port, clk);
	
#ifdef SPI_MUX
	ssp_port(2)->TOP_CTRL |= SSP_SSE;
#endif
}
void ssp_reg_printf(void)
{
	spi_dbg("[TCTRL:%0.8x][FCTRL:%0.8x][INT:%0.8x][STAS:%0.8x]\r\n"
		,ssp_port(2)->TOP_CTRL
		,ssp_port(2)->FIFO_CTRL
		,ssp_port(2)->INT_EN
		,ssp_port(2)->STATUS
		);
}
void spi_xmit_config(int port, int tx_mode)
{
	//make sure SSP is disabled
	ssp_port(port)->TOP_CTRL &= ~SSP_SSE;
//    mdelay(1);
	//reset SSP CR's
	ssp_port(port)->TOP_CTRL  = SSP_TCR_INITIAL;
	ssp_port(port)->TOP_CTRL |= (1 << 13); //trailing byte
	ssp_port(port)->FIFO_CTRL = SSP_FCR_INITIAL;
	ssp_port(port)->INT_EN    = SSP_IER_INITIAL;
  
	ssp_port(port)->TOP_CTRL &= ~(0x1F << 5);
	ssp_port(port)->TOP_CTRL |= (tx_mode << 5);
}

void spi_dss_switch(int port, int dss)
{
	ssp_port(port)->TOP_CTRL &= ~SSP_SSE;
//    mdelay(1);
	ssp_port(port)->TOP_CTRL &= ~(0x1F << 5);
	ssp_port(port)->TOP_CTRL |= (dss << 5);
	ssp_port(port)->TOP_CTRL |= SSP_SSE;
//    mdelay(1);
}

void spi_xmit_start(int port)
{
	spi_assert_cs(port);
	ssp_port(port)->TOP_CTRL |= SSP_SSE;
 //   mdelay(1);
}

void spi_xmit_data(int port, uint32_t data)
{
	ssp_port(port)->DATAR = data;
}

void spi_rx_endian(int port, int rre)
{
	ssp_port(port)->FIFO_CTRL &= ~(3 << 12);
	ssp_port(port)->FIFO_CTRL |= (rre << 12);
}

void spi_tx_endian(int port, int twe)
{
	ssp_port(port)->FIFO_CTRL &= ~(3 << 14);
	ssp_port(port)->FIFO_CTRL |= (twe << 14);
}

uint32_t spi_recv_data(int port)
{
    return ssp_port(port)->DATAR;
}

void spi_xmit_stop(int port)
{
	spi_deassert_cs(port);
	ssp_port(port)->TOP_CTRL &= ~SSP_SSE;
//    mdelay(1);
}

void spi_disable_ssp(int port)
{
    ssp_port(port)->TOP_CTRL  = TOP_INIT_DATA8;
    ssp_port(port)->FIFO_CTRL = FIFO_INIT;
}

void spi_dma_enable(int port)
{
	uint32_t val;
	val = ssp_port(port)->FIFO_CTRL;
	val &= ~(SSP_FCR_RFT_MASK);
	val &= ~(SSP_FCR_TFT_MASK);
#ifdef SPI_MUX
	val |= (SSP_FCR_TFT(7) | SSP_FCR_RFT(8));
#else
	val |= (SSP_FCR_TFT(1) | SSP_FCR_RFT(1));
#endif
	val |= (SSP_FCR_RSRE | SSP_FCR_TSRE | SSP_FCR_RAFC);
     
	ssp_port(port)->FIFO_CTRL = val;
	ssp_port(port)->TOP_CTRL |= (1 << 13); //trailing byte
	ssp_port(port)->TO = 0xFFFFFFFF;
}

int spi_wait_complete(int port)
{
    int timeout = 0xFFFFFFFF;
    
    while(timeout)
    {
    	if((ssp_port(port)->STATUS & 0x8FC1) == 0x40)
    		break;
//    	mdelay(1);
    	timeout--;
    }

    if(timeout == 0)
    {
    	fatal_printf("SSP Timeout\n\r");
    	return -1;
    }

    return 0;
}

int ssp_write_word(int port, int data)
{
	int timeout = TIMEOUT;
	while (!(ssp_port(port)->STATUS & SSP_TNF)) {
#ifdef SPI_MUX
		//if (!--timeout){
			//fatal_printf("ssp_write_word read timeout\n\r");
			//return -1;
		//}
#else
	if (!--timeout) {
			fatal_printf("ssp_write_word read timeout\n\r");
		return -1;
	}

#endif
	}

	ssp_port(port)->DATAR = data;
	return 0;
}

int ssp_read_word(int port)
{
	int timeout = TIMEOUT;

#ifdef SPI_MUX
	while (!(ssp_port(port)->STATUS & SSP_RNE)) ;
		//if (!--timeout) {
			//fatal_printf("ssp_read_word read timeout\n\r");
			//return -1;
		//}
#else
while (!(ssp_port(port)->STATUS & SSP_RNE)) {
	if (!--timeout) {
		fatal_printf("ssp_read_word read timeout\n\r");
		return -1;
	}
}
#endif

	return ssp_port(port)->DATAR;
}

int ssp_flush(int port)
{
	volatile uint32_t val;
	int timeout = TIMEOUT * 2;

	/* Verify TX FIFO is empty */
	while( ( (ssp_port(port)->STATUS & SSP_SSSR_TFL_MSK) &&  (ssp_port(port)->STATUS & SSP_TNF) ) || 
		(!(ssp_port(port)->STATUS & SSP_SSSR_TFL_MSK) && !((ssp_port(port)->STATUS & SSP_TNF))));

	do {
		while (ssp_port(port)->STATUS & SSP_RNE) {
			if (!--timeout)
				return -1;
			val = ssp_port(port)->DATAR;
		}
		if (!--timeout)
			return -1;
	} while (ssp_port(port)->STATUS & SSP_BSY);

	return 0;
}

static void ssp_reset_port(int port)
{
	ssp_port(port)->TOP_CTRL = 0;
	ssp_port(port)->FIFO_CTRL = 0;
	ssp_port(port)->PSP_CTRL = 0;
	ssp_port(port)->INT_EN = 0x30;
}

void ssp_config_master(int port, int is_master)
{
	if(is_master)
		ssp_port(port)->TOP_CTRL &= ~(SSP_SFRMDIR | SSP_SCLKDIR);
	else
		ssp_port(port)->TOP_CTRL |= (SSP_SFRMDIR | SSP_SCLKDIR);
}

void ssp_init_master(int port, int ssp_clk,int master)
{
	ssp_clk_cfg(port, ssp_clk);
	ssp_reset_port(port);
	udelay(5);
	ssp_config_master(port, master);
}


void ssp_port_enable(int port, int is_enable)
{
	uint32_t reg;

	if (is_enable){
		ssp_port(port)->TOP_CTRL |= SSP_SSE;
//        mdelay(1);
    }
	else{
		ssp_port(port)->TOP_CTRL &= ~SSP_SSE;
//        mdelay(1);
    }
}

void ssp_set_selfloop(int port, int is_enable)
{
	if (is_enable)
		ssp_port(port)->TOP_CTRL |= SSP_LBM;
	else
		ssp_port(port)->TOP_CTRL &= ~SSP_LBM;
}

void ssp_set_dma_tx_enable(int port, int is_enable)
{
	if (is_enable)
		ssp_port(port)->FIFO_CTRL |= SSP_TSRE;
	else
		ssp_port(port)->FIFO_CTRL &= ~SSP_TSRE;
}

void ssp_set_dma_rx_enable(int port, int is_enable)
{
	if (is_enable)
		ssp_port(port)->FIFO_CTRL |= SSP_RSRE;
	else
		ssp_port(port)->FIFO_CTRL &= ~SSP_RSRE;
}


int ssp_config(int port, uint32_t mode, uint32_t flags, uint32_t psp_flags, uint32_t speed)
{
	uint32_t top_ctrl = ssp_port(port)->TOP_CTRL;
	uint32_t fifo_ctrl = ssp_port(port)->FIFO_CTRL;
	uint32_t psp_ctrl = ssp_port(port)->PSP_CTRL;

	top_ctrl &= ~(SSP_FRF(0x3) | SSP_DSS(0x1f));
	fifo_ctrl &= ~(SSP_TXFIFO_WR_ENDIAN(0x3) | SSP_RXFIFO_RD_ENDIAN(0x3) | SSP_RFT(0x1f) | SSP_TFT(0x1f));
	psp_ctrl &= ~(SSP_EDMYSTOP(0x7) | SSP_DMYSTOP(0x3) | SSP_EDMYSTRT(0x3) | SSP_DMYSTRT(0x3) | SSP_STRTDLY(0x7)
				| SSP_SFRMWDTH(0x3f) | SSP_SFRMDLY(0x7f) | SSP_SCMODE(0x3));

	ssp_port(port)->TOP_CTRL = speed | mode | top_ctrl;
	ssp_port(port)->FIFO_CTRL = flags | fifo_ctrl;
	ssp_port(port)->PSP_CTRL = psp_flags | psp_ctrl;
	ssp_port(port)->INT_EN = 0;

	return 0;
}

int ssp_get_txfifo_req(int port)
{
	return (ssp_port(port)->STATUS & SSP_TFS)? 1: 0;
}

int ssp_fifo_reset(int port)
{
	uint32_t sssr_reg = ssp_port(port)->STATUS;

	if (sssr_reg & SSP_SSSR_TFL_MSK)
		return -1;

	if ((sssr_reg & SSP_TNF) == 0)
		return -1;

	if ((sssr_reg & SSP_SSSR_RFL_MSK) >> 15 != 0x1f)
		return -1;

	if (sssr_reg & SSP_RNE)
		return -1;

	return 0;
}

void ssp_common_config(int port, ssp_init_param *param)
{
	uint32_t flags, speed, psp_flags, mode;
	flags = SSP_TFT(param->tx_watermark) | SSP_RFT(param->rx_watermark);
	speed = 0;
	psp_flags = 0;
	mode = param->mode;
	switch(param->data_length){
		case 8:
			speed |= SSP_DATA_8_BIT;
			break;
		case 18:
			speed |= SSP_DATA_18_BIT;
			break;
		case 32:
			speed |= SSP_DATA_32_BIT;
			break;
		case 16:
		default:
			speed |= SSP_DATA_16_BIT;
	}
	speed |= (SSP_TTELP | SSP_TTE);
	//disable int
	ssp_config(port, mode, flags, psp_flags, speed);

}

void ssp_map_channel( int port, int tx_ch, int rx_ch){
	int rx_req, tx_req;
	switch(port)
	{
		case 0:
			rx_req = 52;
			tx_req = 53;
			break;
		case 2:
			rx_req = 60;
			tx_req = 61;
	}

	DMA->DRCMR1[rx_req] = RCMR_MAPVLD | rx_ch;
	DMA->DRCMR1[tx_req] = RCMR_MAPVLD | tx_ch;
	return;
}

void ssp_unmap_channel( int port)
{
	int rx_req, tx_req;
	switch(port)
	{
		case 0:
			rx_req = 52;
			tx_req = 53;
			break;
		case 2:
			rx_req = 60;
			tx_req = 61;
	}

	DMA->DRCMR1[rx_req] = 0;
	DMA->DRCMR1[tx_req] = 0;
	return;
}

void ssp_trailing_owner_select(int port, int owner)
{
	if (owner)  //DMA
		ssp_port(port)->TOP_CTRL |= SSP_TRAIL;
	else
		ssp_port(port)->TOP_CTRL &= ~SSP_TRAIL;
}

void ssp_trailing_set_timeout(int port, uint32_t timeout)
{
	ssp_port(port)->TO = timeout;
}



#else
#include "common.h"

#include "Typedef.h"
#include "Errors.h"
//#include "bsp.h"
#include "PlatformConfig.h"
#include "NSSP.h"
#include "PMUA.h"
#include "PMUM.h"
#include "SQU.h"
//#include "DMA_SPI.h"
#include "ssp_host.h"
#include "qspi_dma.h"
//#include "cgpio.h"
#define fatal_printf uart_printf

//limited for timing and stack concerns (see SPI_page_write routine)
#define SSP_READ_TIME_OUT_MILLI		0x2000
#define SSP_READ_DMA_DESC			0x10	//total RX descriptors
#define SSP_READ_DMA_SIZE			0x100	//bytes per descriptor
#define SSP_MAX_TX_SIZE_WORDS		64
#define SSP_MAX_TX_SIZE_BYTES		SSP_MAX_TX_SIZE_WORDS << 2
#define SSP_CS_USE_GPIO

#define SYSCLK_EN                   (0x1<<31)
#define SYSCLK_GEN_156M             (1 << 30)
#define SYSCLK_GEN_26M              (0 << 30)
#define BITCLK_EN                   (1 << 29)
#define DIV_2                       (0 << 27)
#define DIV_4                       (1 << 27)
#define DIV_6                       (2 << 27)
#define DIV_8                       (3 << 27)
#define DENOM                       (1 << 15)
#define NOM                         (3 << 0)

#define SSP_DATA_16_BIT             (15 << 5)
#define SSP_DATA_8_BIT		        (7 << 5)
#define SSP_DATA_32_BIT		        (31 << 5)
#define SSP_DATA_18_BIT		        (17 << 5)
#define TIMEOUT                     100000

int ssp_index = 2;
int ssp_init_clk = 0;

void uudelay(int us)
{
	volatile UINT32 i;
	for(i=0; i<us*20;i++);
}

void mdelay(int ms){
	volatile UINT32 i;
	for(i=0; i<ms;i++)
		uudelay(1000);
}

unsigned int Endian_Convert (unsigned int in)
{
    unsigned int out;
    out = in << 24;
    out |= (in & 0xFF00) << 8;
    out |= (in & 0xFF0000) >> 8;
    out |= (in & 0xFF000000) >> 24;
    return out;
}

void ssp_pinmux_cfg(int port, uint32_t gpio_flag)
{
    switch(port){
        case 0:
			BU_REG_WRITE(0xd401e168, 0x1081);	//RX	GPIO_35
        	BU_REG_WRITE(0xd401e16c, 0x1081);	//TX	GPIO_36
        	BU_REG_WRITE(0xd401e160, 0x1081);	//CLK	GPIO_33
        	BU_REG_WRITE(0xd401e164, 0x1081);	//FRM (CS)
            break;
        case 1:
            break;
        case 2:
			
        	BU_REG_WRITE(0xd401e1D4, 0x1081);	//Hold QSPI_DQM  GPIO_78
        	BU_REG_WRITE(0xd401e1D0, 0x1081);	//WP   QSPI_CS2  GPIO_77

			BU_REG_WRITE(0xd401e114, 0x1081);	//RX	GPIO_14
        	BU_REG_WRITE(0xd401e118, 0x1081);	//TX	GPIO_15
        	BU_REG_WRITE(0xd401e10C, 0x1081);	//CLK	GPIO_12
            if(gpio_flag){
#ifdef SSP_CS_USE_GPIO
        	    BU_REG_WRITE(0xd401e110, 0x1080);	//FRM (CS): GPIO_13
#else
        	    BU_REG_WRITE(0xd401e110, 0x1081);	//FRM (CS)
#endif
            }
            else
                BU_REG_WRITE(0xd401e110, 0x10A1);	//FRM (CS)    
        default:
            break;
    }
}

void ssp_clk_cfg(int port, int clk)
{
	switch(port){
		case 0:
			APBClock->SSP0_CLK_RST = SSP0_CLK_RST_APBCLK|SSP0_CLK_RST_FNCLK|SSP0_CLK_RST_RST|(clk<<4);
			udelay(1);
			APBClock->SSP0_CLK_RST = SSP0_CLK_RST_APBCLK|SSP0_CLK_RST_FNCLK|(clk<<4);
			break;
		case 1:
			PMUA->SQU_CLK_GATE_CTRL &= ~(0x1<<30); //[30]=0: audio_island_clk selects aclk, default 266Mhz for Real-Core)
			SQU->CTRL_0 |= (0x7<<2);   //[4:2]=0x7: APB Clock Div is 7
			PMUM->FCCR &= ~(0x1<<28);  //[28]=0: i2s clock divider select 312Mhz; [28]=1: select VCTCXO(default 26Mhz); 
			//Step1: disable sysclk and bitclck to set the related parameters
			PMUM->ISCCRX[1] = SYSCLK_GEN_26M | DIV_8 | DENOM | NOM;
			 //Step2: enable sysclk and bitclk
			PMUM->ISCCRX[1] |= (SYSCLK_EN | BITCLK_EN);
			//Step3: wait a little time here for new setting clock div valid. if we do step1&step2 in one step, the previous
			//DENOM & NOM setting will influnce the new clock div, which may need wait much more time for the new setting
			//clock div valid(500ns VS 10,000ns)
			udelay(1);

			APBClock->SSP1_CLK_RST = SSP1_CLK_RST_APBCLK|SSP1_CLK_RST_FNCLK|SSP1_CLK_RST_RST;
			udelay(1);
			APBClock->SSP1_CLK_RST = SSP1_CLK_RST_APBCLK|SSP1_CLK_RST_FNCLK;
			break;
		case 2:
			APBClock->SSP2_CLK_RST = SSP2_CLK_RST_APBCLK|SSP2_CLK_RST_FNCLK|SSP2_CLK_RST_RST|(clk<<4);
			udelay(1);
			APBClock->SSP2_CLK_RST = SSP2_CLK_RST_APBCLK|SSP2_CLK_RST_FNCLK|(clk<<4);
		default:
			break;
	}
}

void ssp_port_init(int port, int clk, uint32_t gpio_flag)
{
	ssp_pinmux_cfg(port, gpio_flag);
	ssp_clk_cfg(port, clk);
	
	ssp_port(2)->TOP_CTRL |= SSP_SSE;
}
void ssp_reg_printf(void)
{
	fatal_printf("TOP_CTRL:0x%lx\r\n",ssp_port(2)->TOP_CTRL);
	fatal_printf("FIFO_CTRL:0x%lx\r\n",ssp_port(2)->FIFO_CTRL);
	fatal_printf("INT_EN:0x%lx\r\n",ssp_port(2)->INT_EN);
	fatal_printf("status:0x%lx\r\n",ssp_port(2)->STATUS);
}
void spi_xmit_config(int port, int tx_mode)
{
	//make sure SSP is disabled
	ssp_port(port)->TOP_CTRL &= ~SSP_SSE;
    mdelay(1);
	//reset SSP CR's
	ssp_port(port)->TOP_CTRL  = SSP_TCR_INITIAL;
	ssp_port(port)->TOP_CTRL |= (1 << 13); //trailing byte
	ssp_port(port)->FIFO_CTRL = SSP_FCR_INITIAL;
	ssp_port(port)->INT_EN    = SSP_IER_INITIAL;
  
	ssp_port(port)->TOP_CTRL &= ~(0x1F << 5);
	ssp_port(port)->TOP_CTRL |= (tx_mode << 5);
}

void spi_dss_switch(int port, int dss)
{
	ssp_port(port)->TOP_CTRL &= ~SSP_SSE;
    mdelay(1);
	ssp_port(port)->TOP_CTRL &= ~(0x1F << 5);
	ssp_port(port)->TOP_CTRL |= (dss << 5);
	ssp_port(port)->TOP_CTRL |= SSP_SSE;
    mdelay(1);
}

void spi_xmit_start(int port)
{
//	spi_assert_cs(port);
	ssp_port(port)->TOP_CTRL |= SSP_SSE;
    mdelay(1);
}
void sspRegDump(int port)
{
	volatile UINT32	TOP_CTRL,FIFO_CTRL;

	
	TOP_CTRL = ssp_port(port)->TOP_CTRL;
	FIFO_CTRL = ssp_port(port)->FIFO_CTRL;


	
	uart_printf("------------reg dump start-----------\r\n");
	uart_printf("TOP_CTRL:0x%lx, FIFO_CTRL:0x%lx\r\n", TOP_CTRL, FIFO_CTRL);
	uart_printf("------------reg dump end-----------\r\n");
	
}

void spi_xmit_data(int port, uint32_t data)
{
	ssp_port(port)->DATAR = data;
}

void spi_rx_endian(int port, int rre)
{
	ssp_port(port)->FIFO_CTRL &= ~(3 << 12);
	ssp_port(port)->FIFO_CTRL |= (rre << 12);
}

void spi_tx_endian(int port, int twe)
{
	ssp_port(port)->FIFO_CTRL &= ~(3 << 14);
	ssp_port(port)->FIFO_CTRL |= (twe << 14);
}

uint32_t spi_recv_data(int port)
{
    return ssp_port(port)->DATAR;
}

void spi_xmit_stop(int port)
{
//	spi_deassert_cs(port);
	ssp_port(port)->TOP_CTRL &= ~SSP_SSE;
    mdelay(1);
}

void spi_disable_ssp(int port)
{
    ssp_port(port)->TOP_CTRL  = TOP_INIT_DATA8;
    ssp_port(port)->FIFO_CTRL = FIFO_INIT;
}

void spi_dma_enable(int port)
{
	uint32_t val;
	val = ssp_port(port)->FIFO_CTRL;
	val &= ~(SSP_FCR_RFT_MASK);
	val &= ~(SSP_FCR_TFT_MASK);
	val |= (SSP_FCR_TFT(7) | SSP_FCR_RFT(8));
	val |= (SSP_FCR_RSRE | SSP_FCR_TSRE | SSP_FCR_RAFC);
     
	ssp_port(port)->FIFO_CTRL = val;
	ssp_port(port)->TOP_CTRL |= (1 << 13); //trailing byte
	ssp_port(port)->TO = 0xFFFFFFFF;
}

int spi_wait_complete(int port)
{
    int timeout = 0xFFFFFFFF;
    
    while(timeout)
    {
    	if((ssp_port(port)->STATUS & 0xFC1) == 0x40)
    		break;
		//mdelay(1);
    	timeout--;
    }

    if(timeout == 0)
    {
    	fatal_printf("SSP Timeout\n\r");
    	return -1;
    }

    return 0;
}

int ssp_write_word(int port, int data)
{
	int timeout = TIMEOUT;
	//fatal_printf("%s,par1:0x%x; par2:0x%x\r\n",__func__,ssp_port(port)->STATUS,SSP_TNF);
	/*************************************
	*1. Bit6  TNF: Transmit FIFO Not Full;
	*   	       0 --- TXFIFO is full
	*  	           1 --- TXFIFO is not full
	*************************************/
	while (!(ssp_port(port)->STATUS & SSP_TNF)) {
		//if (!--timeout){
			//fatal_printf("ssp_write_word read timeout\n\r");
			//return -1;
		//}
		//fatal_printf("%s\r\n",__func__);
	}

	ssp_port(port)->DATAR = data;
	return 0;
}

int ssp_read_word(int port)
{
	//int timeout = TIMEOUT;
	//fatal_printf("%s,par1:0x%x; par2:0x%x\r\n",__func__,ssp_port(port)->STATUS,SSP_RNE);
	/*************************************
	*1. Bit6  RNE: Receive FIFO Not Empty;
	*   	       0 --- RXFIFO is empty
	*  	           1 --- RXFIFO is not empty
	*************************************/
	while (!(ssp_port(port)->STATUS & SSP_RNE)){
		//if (!--timeout) {
			//fatal_printf("ssp_read_word read timeout\n\r");
			//return -1;
		//}
		//fatal_printf("%s\r\n",__func__);
	};

	

	return ssp_port(port)->DATAR;
}

int ssp_flush(int port)
{
	volatile uint32_t val;
	int timeout = TIMEOUT * 2;

	/* Verify TX FIFO is empty */
	while( ( (ssp_port(port)->STATUS & SSP_SSSR_TFL_MSK) &&  (ssp_port(port)->STATUS & SSP_TNF) ) || 
		(!(ssp_port(port)->STATUS & SSP_SSSR_TFL_MSK) && !((ssp_port(port)->STATUS & SSP_TNF))));

	do {
		while (ssp_port(port)->STATUS & SSP_RNE) {
			if (!--timeout)
				return -1;
			val = ssp_port(port)->DATAR;
		}
		if (!--timeout)
			return -1;
	} while (ssp_port(port)->STATUS & SSP_BSY);

	return 0;
}

static void ssp_reset_port(int port)
{
	ssp_port(port)->TOP_CTRL = 0;
	ssp_port(port)->FIFO_CTRL = 0;
	ssp_port(port)->PSP_CTRL = 0;
	ssp_port(port)->INT_EN = 0x30;
}

void ssp_config_master(int port, int is_master)
{
	if(is_master)
		ssp_port(port)->TOP_CTRL &= ~(SSP_SFRMDIR | SSP_SCLKDIR);
	else
		ssp_port(port)->TOP_CTRL |= (SSP_SFRMDIR | SSP_SCLKDIR);
}

void ssp_init_master(int port, int ssp_clk,int master)
{
	ssp_clk_cfg(port, ssp_clk);
	ssp_reset_port(port);
	udelay(5);
	ssp_config_master(port, master);
}


void ssp_port_enable(int port, int is_enable)
{
	uint32_t reg;

	if (is_enable){
		ssp_port(port)->TOP_CTRL |= SSP_SSE;
        mdelay(1);
    }
	else{
		ssp_port(port)->TOP_CTRL &= ~SSP_SSE;
        mdelay(1);
    }
}

void ssp_set_selfloop(int port, int is_enable)
{
	if (is_enable)
		ssp_port(port)->TOP_CTRL |= SSP_LBM;
	else
		ssp_port(port)->TOP_CTRL &= ~SSP_LBM;
}

void ssp_set_dma_tx_enable(int port, int is_enable)
{
	if (is_enable)
		ssp_port(port)->FIFO_CTRL |= SSP_TSRE;
	else
		ssp_port(port)->FIFO_CTRL &= ~SSP_TSRE;
}

void ssp_set_dma_rx_enable(int port, int is_enable)
{
	if (is_enable)
		ssp_port(port)->FIFO_CTRL |= SSP_RSRE;
	else
		ssp_port(port)->FIFO_CTRL &= ~SSP_RSRE;
}


int ssp_config(int port, uint32_t mode, uint32_t flags, uint32_t psp_flags, uint32_t speed)
{
	uint32_t top_ctrl = ssp_port(port)->TOP_CTRL;
	uint32_t fifo_ctrl = ssp_port(port)->FIFO_CTRL;
	uint32_t psp_ctrl = ssp_port(port)->PSP_CTRL;

	top_ctrl &= ~(SSP_FRF(0x3) | SSP_DSS(0x1f));
	fifo_ctrl &= ~(SSP_TXFIFO_WR_ENDIAN(0x3) | SSP_RXFIFO_RD_ENDIAN(0x3) | SSP_RFT(0x1f) | SSP_TFT(0x1f));
	psp_ctrl &= ~(SSP_EDMYSTOP(0x7) | SSP_DMYSTOP(0x3) | SSP_EDMYSTRT(0x3) | SSP_DMYSTRT(0x3) | SSP_STRTDLY(0x7)
				| SSP_SFRMWDTH(0x3f) | SSP_SFRMDLY(0x7f) | SSP_SCMODE(0x3));

	ssp_port(port)->TOP_CTRL = speed | mode | top_ctrl;
	ssp_port(port)->FIFO_CTRL = flags | fifo_ctrl;
	ssp_port(port)->PSP_CTRL = psp_flags | psp_ctrl;
	ssp_port(port)->INT_EN = 0;

	return 0;
}

int ssp_get_txfifo_req(int port)
{
	return (ssp_port(port)->STATUS & SSP_TFS)? 1: 0;
}

int ssp_fifo_reset(int port)
{
	uint32_t sssr_reg = ssp_port(port)->STATUS;

	if (sssr_reg & SSP_SSSR_TFL_MSK)
		return -1;

	if ((sssr_reg & SSP_TNF) == 0)
		return -1;

	if ((sssr_reg & SSP_SSSR_RFL_MSK) >> 15 != 0x1f)
		return -1;

	if (sssr_reg & SSP_RNE)
		return -1;

	return 0;
}

void ssp_common_config(int port, ssp_init_param *param)
{
	uint32_t flags, speed, psp_flags, mode;
	flags = SSP_TFT(param->tx_watermark) | SSP_RFT(param->rx_watermark);
	speed = 0;
	psp_flags = 0;
	mode = param->mode;
	switch(param->data_length){
		case 8:
			speed |= SSP_DATA_8_BIT;
			break;
		case 18:
			speed |= SSP_DATA_18_BIT;
			break;
		case 32:
			speed |= SSP_DATA_32_BIT;
			break;
		case 16:
		default:
			speed |= SSP_DATA_16_BIT;
	}
	speed |= (SSP_TTELP | SSP_TTE);
	//disable int
	ssp_config(port, mode, flags, psp_flags, speed);

}
extern volatile P_XLLP_DMAC_T DMA;

void ssp_map_channel( int port, int tx_ch, int rx_ch){

	int rx_req, tx_req;

	if((port != 0) && (port != 2))
		return;
	
	switch(port)
	{
		case 0:
			rx_req = 52;
			tx_req = 53;
			break;
		case 2:
			rx_req = 60;
			tx_req = 61;
	}

	DMA->DRCMR1[rx_req] = RCMR_MAPVLD | rx_ch;
	DMA->DRCMR1[tx_req] = RCMR_MAPVLD | tx_ch;
	return;
}

void ssp_unmap_channel( int port)
{
	int rx_req, tx_req;

	if((port != 0) && (port != 2))
		return;
	
	switch(port)
	{
		case 0:
			rx_req = 52;
			tx_req = 53;
			break;
		case 2:
			rx_req = 60;
			tx_req = 61;
	}

	DMA->DRCMR1[rx_req] = 0;
	DMA->DRCMR1[tx_req] = 0;
	return;
}

void ssp_trailing_owner_select(int port, int owner)
{
	if (owner)  //DMA
		ssp_port(port)->TOP_CTRL |= SSP_TRAIL;
	else
		ssp_port(port)->TOP_CTRL &= ~SSP_TRAIL;
}

void ssp_trailing_set_timeout(int port, uint32_t timeout)
{
	ssp_port(port)->TO = timeout;
}
#endif

