#ifndef _SDIO_H_
#define _SDIO_H_
#include "common.h"

//used for sd card
#define SD1_HOST_CTRL_ADDR	0xD4280000

/*Err code*/
#define MMC_ERR_NONE 	                                0
#define MMC_ERR_CMD_TOUT                                1
#define MMC_ERR_DATA_TOUT                               2
#define MMC_ERR_DATA_CRC                                3
#define MMC_ERR_INVALID                                 4
#define MMC_ERR_OUTRANGE                                5
#define MMC_ERR_IO                                      6
#define MMC_ERR_INVALIDARG                              7
#define MMC_ERR_OUTMEMORY                               8
#define STATUS_FAILURE                                  -1


#define ERR_INT			            ((UINT16)1 << 15) /* Error Interrupt*/
#define CARD_INT		            ((UINT16)1 << 8) /* Card Interrupt */
#define CARD_REM		            ((UINT16)1 << 7) /* Card Removal Interrupt */
#define CARD_INS		            ((UINT16)1 << 6) /* Card Insertion Interrupt */
#define RX_RDY			            ((UINT16)1 << 5) /* Buffer Read Ready */
#define TX_RDY			            ((UINT16)1 << 4) /* Buffer Write Ready */
#define DMA_INT                     ((UINT16)1 << 3) /* DMA interupt */
#define BLK_GAP_EVNT	            ((UINT16)1 << 2) /* Block Gap Event */
#define XFER_COMP		            ((UINT16)1 << 1) /* Transfer Complete */
#define CMD_COMP		            ((UINT16)1 << 0) /* Command Complete */

#define CRC_STATUS_ERR		        ((UINT16)1 << 15) /* CRC Status Error  Returned from Card in Write Transaction*/
#define CPL_TO_ERR		            ((UINT16)1 << 14) /* Command Completion Signal Timeout Error, for CE-ATA mode only*/
#define AXI_RESP_ERR		        ((UINT16)1 << 13) /* AXI Bus Response Error */
#define SPI_ERR			            ((UINT16)1 << 12) /* SPI Mode Error*/
#define ADMA_ERR		            ((UINT16)1 << 9) /* AMDA Error */
#define AUTO_CMD12_ERR		        ((UINT16)1 << 8) /* Auto CMD12 Error*/
#define CUR_LIMIT_ERR		        ((UINT16)1 << 7) /* Current Limit Error*/
#define RD_DATA_END_ERR		        ((UINT16)1 << 6) /* Read Data End Bit Error*/
#define RD_DATA_CRC_ERR		        ((UINT16)1 << 5) /* Read Data CRC Error*/
#define DATA_TO_ERR		            ((UINT16)1 << 4) /* Data Timeout Error*/
#define CMD_IDX_ERR		            ((UINT16)1 << 3) /* Command Index Error*/
#define CMD_END_BIT_ERR		        ((UINT16)1 << 2) /* Command End Bit Error*/
#define CMD_CRC_ERR		            ((UINT16)1 << 1) /* Command CRC Error*/
#define CMD_TO_ERR		            ((UINT16)1 << 0) /* Command Timeout Error*/
#define SD_ERR_INT_EN_RVD_MASK		((UINT16)0x0c00) /* Mask for SD_ERR_INT_EN/SD_ERR_I_STAT_EN Reserved Bits[11 :10] */
#define SD_ERR_INT_DATA_ERR_MASK	(DATA_TO_ERR | RD_DATA_CRC_ERR | RD_DATA_END_ERR) /*DATA Line Error*/
#define SD_ERR_INT_CMD_ERR_MASK		(CMD_TO_ERR | CMD_CRC_ERR | CMD_END_BIT_ERR | CMD_IDX_ERR) /* CMD Line Error*/

#define SD0_HOST_PMU_AXI_CLOCK                          0xd4282854
#define SD1_HOST_PMU_AXI_CLOCK                          0xd4282858
#define SD2_HOST_PMU_AXI_CLOCK                          0xd42828E0

#define APB_CLOCK_UNIT_PHYSICAL_BASE                    0xD4015000
#define APBC_GPIO_CLK_RST_offset                        0x0008

#define SD_SYSADDR_LOW_offset		                    0x00
#define SD_SYSADDR_HIGH_offset		                    0x02
#define SD_BLOCK_SIZE_offset		                    0x04
#define SD_BLOCK_COUNT_offset		                    0x06
#define SD_ARG_LOW_offset			                    0x08
#define SD_ARG_HIGH_offset			                    0x0A
#define SD_TRANSFER_MODE_offset		                    0x0C
#define SD_CMD_offset				                    0x0E
#define SD_RESP_0_offset			                    0x10
#define SD_RESP_1_offset			                    0x12
#define SD_RESP_2_offset			                    0x14
#define SD_RESP_3_offset			                    0x16
#define SD_RESP_4_offset			                    0x18
#define SD_RESP_5_offset			                    0x1A
#define SD_RESP_6_offset			                    0x1C
#define SD_RESP_7_offset			                    0x1E
#define SD_BUF_PORT_0_offset		                    0x20
#define SD_BUF_PORT_1_offset		                    0x22
#define SD_PRESENT_STAT_0_offset	                    0x24
#define SD_PRESENT_STAT_1_offset	                    0x26
#define SD_HOST_CTRL_offset			                    0x28
#define SDHCI_POWER_CONTROL			                    0x29
#define SD_BGAP_CTRL_offset			                    0x2A
#define SD_CLOCK_CTRL_offset		                    0x2C
#define SD_SW_RESET_CTRL_offset		                    0x2E
#define SD_NORM_INTR_STS_offset		                    0x30
#define SD_ERR_INTR_STS_offset		                    0x32
#define SD_NORM_INTR_STS_EBLE_offset	                0x34
#define SD_ERR_INTR_STS_EBLE_offset		                0x36
#define SD_NORM_INTR_STS_INTR_EBLE_offset	            0x38
#define SD_ERR_INTR_STS_INTR_EBLE_offset	            0x3A
#define SD_ADMA_SYS_ADDR_offset			                0x58
#define SD_ADMA_SYS_ADDR2_offset		                0x5C
#define SD_CE_ATA_1_offset                              0xE8
#define	SD_PAD_IO_SETUP					                0xEC
#define SDHCI_WAKE_UP_CONTROL	                        0x2B
#define SDHCI_ACMD12_ERR	                            0x3C
#define SDHCI_SLOT_INT_STATUS	                        0xFC
#define SDHCI_HOST_CTRL2                                0X3E
#define SDHCI_PRESET_VALUE_FOR_SDR50                    0x6A

#ifdef PLAT_NEZHAC
#define SD_CLOCK_AND_BURST_SIZE_SETUP_offset            0x104
#define SDHCI_TX_CFG                                    0X11C
#define SDHCI_RX_CFG_REG                                0x118
#else
#define SD_CLOCK_AND_BURST_SIZE_SETUP_offset            0x10A
#define SD_FIFO_PARA_offset                             0x104
#define SDHCI_TX_CFG                                    0X118
#define SDHCI_RX_CFG_REG                                0x114
#endif

#define SDHCI_MAX_CURRENT	0x48

#define  SDHCI_HOST_VERSION	0xFE
#define  SDHCI_VENDOR_VER_MASK	0xFF00
#define  SDHCI_VENDOR_VER_SHIFT	8
#define  SDHCI_SPEC_VER_MASK	0x00FF
#define  SDHCI_SPEC_VER_SHIFT	0
#define   SDHCI_SPEC_100	0
#define   SDHCI_SPEC_200	1
#define  SDHCI_SPEC_300         2

#define  SDHCI_CLOCK_CARD_EN    0x0004
#define  SDHCI_CLOCK_INT_STABLE 0x0002
#define  SDHCI_CLOCK_INT_EN     0x0001

#define  SDHCI_CAPABILITIES	    0x40
#define  SDHCI_CAPABILITIES2    0x42
#define  SDHCI_CAPABILITIES3    0x44
#define  SDHCI_CAPABILITIES4    0x46

#define  SDIO_CCCR_UHS          0x14
#define  SDR50_SUPPORT          1<<0
#define  SDR104_SUPPORT         1<<1
#define  DDR50_SUPPORT          1<<2
#define  SDIO_CCCR_DRIVE_STRENGTH   0x15
#define  SDIO_STTx_MASK             0x07
#define  SDIO_DRIVE_SDTA            (1<<0)
#define  SDIO_DRIVE_SDTC            (1<<1)
#define  SDIO_DRIVE_SDTD            (1<<2)
#define  SDIO_DRIVE_DTSx_MASK       0x03
#define  SDIO_DRIVE_DTSx_SHIFT      4
#define  SDIO_DTSx_SET_TYPE_B       (0<<SDIO_DRIVE_DTSx_SHIFT)
#define  SDIO_DTSx_SET_TYPE_A       (1<<SDIO_DRIVE_DTSx_SHIFT)
#define  SDIO_DTSx_SET_TYPE_C       (2<<SDIO_DRIVE_DTSx_SHIFT)
#define  SDIO_DTSx_SET_TYPE_D       (3<<SDIO_DRIVE_DTSx_SHIFT)

#define  SD_DRIVER_TYPE_B           0x01
#define  SD_DRIVER_TYPE_A           0x02
#define  SD_DRIVER_TYPE_C           0x04
#define  SD_DRIVER_TYPE_D           0x08

#define  MMC_SET_DRIVER_TYPE_B      0
#define  MMC_SET_DRIVER_TYPE_A      1
#define  MMC_SET_DRIVER_TYPE_C      2
#define  MMC_SET_DRIVER_TYPE_D      3

#define  UHS_SDR12_BUS_SPEED    0
#define  HIGH_SPEED_BUS_SPEED   1
#define  UHS_SDR25_BUS_SPEED    1
#define  UHS_SDR50_BUS_SPEED    2
#define  UHS_SDR104_BUS_SPEED   3
#define  UHS_DDR50_BUS_SPEED    4

#define  SD_MODE_HIGH_SPEED     (1<<HIGH_SPEED_BUS_SPEED)
#define  SD_MODE_UHS_SDR12      (1<<UHS_SDR12_BUS_SPEED)
#define  SD_MODE_UHS_SDR50      (1<<UHS_SDR50_BUS_SPEED)
#define  SD_MODE_UHS_SDR104     (1<<UHS_SDR104_BUS_SPEED)
#define  SD_MODE_UHS_DDR50      (1<<UHS_DDR50_BUS_SPEED)

#define  SD_DRIVER_A            (1<<4)
#define  SD_DRIVER_C            (1<<5)
#define  SD_DRIVER_D            (1<<6)

#define  SDHCI_CTRL_VDD_180     0x0008

#define  SDHCI_DATA_LVL_MASK    0x00F00000
#define  SDHCI_DATA_LVL_SHIFT   20

#define  SDHCI_SIGNAL_VOLTAGE_330   0
#define  SDHCI_SIGNAL_VOLTAGE_180   1
#define  SDHCI_SIGNAL_VOLTAGE_120   2
#define  SDHCI_POWER_ON		    0x01
#define  SDHCI_SOFTWARE_RESET	0x2F
#define  SDHCI_CMD_INHIBIT	    0x00000001
#define  SDHCI_DATA_INHIBIT	    0x00000002
#define  SDHCI_DOING_WRITE	    0x00000100
#define  SDHCI_DOING_READ	    0x00000200
#define  SDHCI_SPACE_AVAILABLE	0x00000400
#define  SDHCI_DATA_AVAILABLE	0x00000800
#define  SDHCI_CARD_PRESENT	    0x00010000
#define  SDHCI_WRITE_PROTECT	0x00080000

#define  SDHCI_TRNS_DMA		    0x01
#define  SDHCI_TRNS_BLK_CNT_EN	0x02
#define  SDHCI_TRNS_ACMD12	    0x04
#define  SDHCI_TRNS_READ	    0x10
#define  SDHCI_TRNS_MULTI	    0x20

#define  SDHCI_CTRL_DMA_MASK	0x18
#define  SDHCI_CTRL_SDMA	    0x00
#define  SDHCI_CTRL_ADMA1	    0x08
#define  SDHCI_CTRL_ADMA32	    0x10
#define  SDHCI_CTRL_ADMA64	    0x18
#define  SDHCI_CTRL_4BITBUS	    0x02
#define  SDHCI_CTRL_HISPD	    0x04

#define  SDHCI_MAKE_BLKSZ(dma, blksz) (((dma & 0x7) << 12) | (blksz & 0xFFF))

/* SD_TO_CTRL_SW_RST */
#define SW_RST_DAT          ((UINT16)1 << 10)
#define SW_RST_CMD		    ((UINT16)1 << 9)
#define SW_RST_ALL		    ((UINT16)1 << 8)

#define MMC_VDD_165_195		0x00000080	/* VDD voltage 1.65 - 1.95 */
#define MMC_VDD_20_21		0x00000100	/* VDD voltage 2.0 ~ 2.1 */
#define MMC_VDD_21_22		0x00000200	/* VDD voltage 2.1 ~ 2.2 */
#define MMC_VDD_22_23		0x00000400	/* VDD voltage 2.2 ~ 2.3 */
#define MMC_VDD_23_24		0x00000800	/* VDD voltage 2.3 ~ 2.4 */
#define MMC_VDD_24_25		0x00001000	/* VDD voltage 2.4 ~ 2.5 */
#define MMC_VDD_25_26		0x00002000	/* VDD voltage 2.5 ~ 2.6 */
#define MMC_VDD_26_27		0x00004000	/* VDD voltage 2.6 ~ 2.7 */
#define MMC_VDD_27_28		0x00008000	/* VDD voltage 2.7 ~ 2.8 */
#define MMC_VDD_28_29		0x00010000	/* VDD voltage 2.8 ~ 2.9 */
#define MMC_VDD_29_30		0x00020000	/* VDD voltage 2.9 ~ 3.0 */
#define MMC_VDD_30_31		0x00040000	/* VDD voltage 3.0 ~ 3.1 */
#define MMC_VDD_31_32		0x00080000	/* VDD voltage 3.1 ~ 3.2 */
#define MMC_VDD_32_33		0x00100000	/* VDD voltage 3.2 ~ 3.3 */
#define MMC_VDD_33_34		0x00200000	/* VDD voltage 3.3 ~ 3.4 */
#define MMC_VDD_34_35		0x00400000	/* VDD voltage 3.4 ~ 3.5 */
#define MMC_VDD_35_36		0x00800000	/* VDD voltage 3.5 ~ 3.6 */

#define MMC_OCR_AVAIL       (MMC_VDD_26_27 | MMC_VDD_27_28 | MMC_VDD_28_29 | MMC_VDD_30_31)


#define _readl(p)		                                (*(volatile unsigned long *)(p))
#define _writel(v,p)	                                (*(volatile unsigned long *)(p) = (v))
#define xlli_TWSI_PHYSICAL_BASE	                        0xD4011000
#define	APBC_TWSI_CLK_RST		                        (0xD4015000+0x002C)

/*
 * I2C registers and bit definitions
 */
#define IBMR	    (0x00)
#define IDBR	    (0x08)
#define ICR		    (0x10)
#define ISR		    (0x18)
#define ISAR	    (0x20)
#define ILCR	    (0x28)

#define SCL		    (1 << 1)
#define SDA		    (1 << 0)
#define ICR_START	(1 << 0)	   /* start bit */
#define ICR_STOP	(1 << 1)	   /* stop bit */
#define ICR_ACKNAK	(1 << 2)	   /* send ACK(0) or NAK(1) */
#define ICR_TB		(1 << 3)	   /* transfer byte bit */
#define ICR_MA		(1 << 4)	   /* master abort */
#define ICR_SCLE	(1 << 5)	   /* master clock enable */
#define ICR_IUE		(1 << 6)	   /* unit enable */
#define ICR_GCD		(1 << 7)	   /* general call disable */
#define ICR_ITEIE	(1 << 8)	   /* enable tx interrupts */
#define ICR_IRFIE	(1 << 9)	   /* enable rx interrupts */
#define ICR_BEIE	(1 << 10)	   /* enable bus error ints */
#define ICR_SSDIE	(1 << 11)	   /* slave STOP detected int enable */
#define ICR_ALDIE	(1 << 12)	   /* enable arbitration interrupt */
#define ICR_SADIE	(1 << 13)	   /* slave address detected int enable */
#define ICR_UR		(1 << 14)	   /* unit reset */
#define ICR_FM		(1 << 15)	   /* fast mode */

#define ISR_RWM		(1 << 0)	   /* read/write mode */
#define ISR_ACKNAK	(1 << 1)	   /* ack/nak status */
#define ISR_UB		(1 << 2)	   /* unit busy */
#define ISR_IBB		(1 << 3)	   /* bus busy */
#define ISR_SSD		(1 << 4)	   /* slave stop detected */
#define ISR_ALD		(1 << 5)	   /* arbitration loss detected */
#define ISR_ITE		(1 << 6)	   /* tx buffer empty */
#define ISR_IRF		(1 << 7)	   /* rx buffer full */
#define ISR_GCAD	(1 << 8)	   /* general call address detected */
#define ISR_SAD		(1 << 9)	   /* slave address detected */
#define ISR_BED		(1 << 10)	   /* bus error no ACK/NAK */

#define ICR_MODE_100	0x0000 		//100kbps standard mode
#define ICR_MODE_400	0x8000 		//400kbps fast mode
#define ICR_MODE_HS		0x10000 	//high speed mode

#define I2C_QUICK		        0
#define I2C_BYTE		        1
#define I2C_BYTE_DATA	        2
#define I2C_WORD_DATA	        3
#define I2C_PROC_CALL	        4
#define I2C_BLOCK_BYTE	        5
#define I2C_BLOCK_BROKEN        6
#define I2C_BLOCK_PROC_CALL     7		/* SMBus 2.0 */
#define I2C_BLOCK_DATA          8

union i2c_data
{
	UINT8  byte;
	UINT16 word;
	UINT8  block[32]; /* block[0] is used for length */
};

/*
 * Card Common Control Registers (CCCR)
 */
#define     SDIO_CCCR_CCCR		    0x00

#define     SDIO_CCCR_REV_1_00	    0	    /* CCCR/FBR Version 1.00 */
#define     SDIO_CCCR_REV_1_10	    1	    /* CCCR/FBR Version 1.10 */
#define     SDIO_CCCR_REV_1_20	    2	    /* CCCR/FBR Version 1.20 */
#define     SDIO_CCCR_REV_3_00      3       /* CCCR/FBR Version 3.00 */


#define     SDIO_SDIO_REV_1_00	    0	    /* SDIO Spec Version 1.00 */
#define     SDIO_SDIO_REV_1_10	    1	    /* SDIO Spec Version 1.10 */
#define     SDIO_SDIO_REV_1_20	    2	    /* SDIO Spec Version 1.20 */
#define     SDIO_SDIO_REV_2_00	    3	    /* SDIO Spec Version 2.00 */
#define     SDIO_SDIO_REV_3_00	    4	    /* SDIO Spec Version 3.00 */

#define     SDIO_CCCR_SD		    0x01

#define     SDIO_SD_REV_1_01	    0	    /* SD Physical Spec Version 1.01 */
#define     SDIO_SD_REV_1_10	    1	    /* SD Physical Spec Version 1.10 */
#define     SDIO_SD_REV_2_00	    2	    /* SD Physical Spec Version 2.00 */

#define     SDIO_CCCR_IOEx		    0x02
#define     SDIO_CCCR_IORx		    0x03

#define     SDIO_CCCR_IENx		    0x04	/* Function/Master Interrupt Enable */
#define     SDIO_CCCR_INTx		    0x05	/* Function Interrupt Pending */

#define     SDIO_CCCR_ABORT		    0x06	/* function abort/card reset */

#define     SDIO_CCCR_IF		    0x07	/* bus interface controls */

#define     SDIO_BUS_WIDTH_1BIT	    0x00
#define     SDIO_BUS_WIDTH_4BIT	    0x02

#define     SDIO_BUS_CD_DISABLE     0x80	/* disable pull-up on DAT3 (pin 1) */

#define     SDIO_CCCR_CAPS		    0x08

#define     SDIO_CCCR_CAP_SDC	    0x01	/* can do CMD52 while data transfer */
#define     SDIO_CCCR_CAP_SMB	    0x02	/* can do multi-block xfers (CMD53) */
#define     SDIO_CCCR_CAP_SRW	    0x04	/* supports read-wait protocol */
#define     SDIO_CCCR_CAP_SBS	    0x08	/* supports suspend/resume */
#define     SDIO_CCCR_CAP_S4MI	    0x10	/* interrupt during 4-bit CMD53 */
#define     SDIO_CCCR_CAP_E4MI	    0x20	/* enable ints during 4-bit CMD53 */
#define     SDIO_CCCR_CAP_LSC	    0x40	/* low speed card */
#define     SDIO_CCCR_CAP_4BLS	    0x80	/* 4 bit low speed card */

#define     SDIO_CCCR_CIS		    0x09	/* common CIS pointer (3 bytes) */

/* Following 4 regs are valid only if SBS is set */
#define     SDIO_CCCR_SUSPEND	0x0c
#define     SDIO_CCCR_SELx		0x0d
#define     SDIO_CCCR_EXECx		0x0e
#define     SDIO_CCCR_READYx	0x0f

#define     SDIO_CCCR_BLKSIZE	0x10

#define     SDIO_CCCR_POWER		0x12

#define     SDIO_POWER_SMPC	    0x01	/* Supports Master Power Control */
#define     SDIO_POWER_EMPC	    0x02	/* Enable Master Power Control */

#define     SDIO_CCCR_SPEED		0x13

#define     SDIO_SPEED_SHS		0x01	/* Supports High-Speed mode */
#define     SDIO_SPEED_EHS		0x02	/* Enable High-Speed mode */
#define     SDIO_SPEED_BSS_SHIFT    1
#define     SDIO_SPEED_BSS_MASK     (7<<SDIO_SPEED_BSS_SHIFT)
#define     SDIO_SPEED_SDR12        (0<<SDIO_SPEED_BSS_SHIFT)
#define     SDIO_SPEED_SDR25        (1<<SDIO_SPEED_BSS_SHIFT)
#define     SDIO_SPEED_SDR50        (2<<SDIO_SPEED_BSS_SHIFT)
#define     SDIO_SPEED_SDR104       (3<<SDIO_SPEED_BSS_SHIFT)
#define     SDIO_SPEED_DDR50        (4<<SDIO_SPEED_BSS_SHIFT)

#define MMC_TIMING_LEGACY	0
#define MMC_TIMING_MMC_HS	1
#define MMC_TIMING_SD_HS	2
#define MMC_TIMING_UHS_SDR12	MMC_TIMING_LEGACY
#define MMC_TIMING_UHS_SDR25	MMC_TIMING_SD_HS
#define MMC_TIMING_UHS_SDR50	3
#define MMC_TIMING_UHS_SDR104	4
#define MMC_TIMING_UHS_DDR50	5
#define MMC_TIMING_MMC_HS200	6

#define HIGHT_SPEED_MAX_DTR     50000000
#define UHS_SDR104_MAX_DTR      208000000
#define UHS_SDR50_MAX_DTR       100000000
#define UHS_DDR50_MAX_DTR       50000000
#define UHS_SDR25_MAX_DTR       UHS_DDR50_MAX_DTR
#define UHS_SDR12_MAX_DTR       25000000

/*
 * Function Basic Registers (FBR)
 */

#define     SDIO_FBR_BASE(f)	    ((f) * 0x100) /* base of function f's FBRs */

#define     SDIO_FBR_STD_IF		    0x00

#define     SDIO_FBR_SUPPORTS_CSA	0x40	/* supports Code Storage Area */
#define     SDIO_FBR_ENABLE_CSA	    0x80	/* enable Code Storage Area */

#define     SDIO_FBR_STD_IF_EXT	    0x01

#define     SDIO_FBR_POWER		    0x02

#define     SDIO_FBR_POWER_SPS	    0x01	/* Supports Power Selection */
#define     SDIO_FBR_POWER_EPS	    0x02	/* Enable (low) Power Selection */

#define     SDIO_FBR_CIS		    0x09	/* CIS pointer (3 bytes) */

#define     SDIO_FBR_CSA		    0x0C	/* CSA pointer (3 bytes) */

#define     SDIO_FBR_CSA_DATA	    0x0F

#define     SDIO_FBR_BLKSIZE	    0x10	/* block size (2 bytes) */

#define     SDIO_CIS_BASE		    0x1000 /* base of CIS Area */

/*
*R5 is reponse of cmd52 and cmd53
*/
#define R5_COM_CRC_ERROR	(1 << 15)	/* er, b */
#define R5_ILLEGAL_COMMAND	(1 << 14)	/* er, b */
#define R5_ERROR		    (1 << 11)	/* erx, c */
#define R5_FUNCTION_NUMBER	(1 << 9)	/* er, c */
#define R5_OUT_OF_RANGE		(1 << 8)	/* er, c */

#define R4_18V_PRESENT      (1<<24)
#define R4_MEMORY_PRESENT   (1<<27)
//#ifndef __max_min__
//#define __max_min__
//#define max(a, b) (((a) > (b)) ? (a) : (b))
//#define min(a, b) (((a) < (b)) ? (a) : (b))
//#endif
typedef struct _sdio_cccr
{
    unsigned int		sdio_vsn;
	unsigned int		multi_block:1,
				low_speed:1,
				wide_bus:1,
				high_power:1,
				high_speed:1;
}sdio_cccr;

/*
 * SDIO function CIS tuple (unknown to the core)
 */
struct sdio_func_tuple
{
	struct sdio_func_tuple *next;
	unsigned char code;
	unsigned char size;
//	unsigned char data[0];
	unsigned char data;
};

typedef int (tpl_parse_t)(UINT8, const unsigned char *, unsigned);

struct cis_tpl
{
	unsigned char code;
	unsigned char min_size;
	tpl_parse_t *parse;
};
#if 0
int cistpl_manfid(UINT8, const unsigned char *buf, unsigned size);
int cistpl_funce(UINT8, const unsigned char *buf, unsigned size);

static const struct cis_tpl cis_tpl_list[] =
{
	{	0x15,	3,	0/*cistpl_vers_1*/		     },
	{	0x20,	4,	cistpl_manfid                },
	{	0x21,	2,	0/*cistpl_funcid*/	         },
	{	0x22,	0,	cistpl_funce		         },
};
#endif
struct sdio_func
{
    unsigned int		      num;		/* function number */
	unsigned		  max_blksize;	    /* maximum block size */
	unsigned		  cur_blksize;	    /* current block size */
    unsigned char		    class;		/* standard interface class */
    unsigned int           vendor;      /* vendor id */
    unsigned int           device;      /* device id */
};

#define     FUNCTION_BLOCKSIZE  256

#define     MAXFUNCTION     8
#define     VENDORID        (0x02df)

#define SINGLE_BLOCK_SIZE_SHIFT 9
#define SINGLE_BLOCK_SIZE 		0x200

extern UINT32 SD_CTRL_BASE_ADDR;

int  sdio_init(void);
int  sdcard_init(void);
UINT32 sdcard_read_multiblks(UINT32 base, UINT32 buffer, UINT32 loop, UINT32 blk_cnt);
UINT32 sdcard_write_multiblks(UINT32 base, UINT32 buffer, UINT32 loop, UINT32 blk_cnt);
UINT32 sdcard_write_mass_storage(UINT32 buffer, UINT32 loop, UINT32 blk_cnt);
UINT32 sdcard_read_mass_storage(UINT32 buffer, UINT32 loop, UINT32 blk_cnt);
BOOL sdcard_is_ready(void);

#endif //_SDIO_H_
