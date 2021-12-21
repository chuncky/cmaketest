#ifndef __MFPR_API_H__
#define __MFPR_API_H__

//#include "plat_types.h"
/*
 * MFP configuration is represented by a 32-bit unsigned integer
 */
#define MFP(_off, _pull, _drv, _slp, _edge, _sleep, _afn) ( \
	/* bits 31..16 - MFP Register Offset */	(((_off) & 0xffff) << 16) | \
	/* bits 15..13 - Run Mode Pull State */	(((_pull) & 0x7) << 13) | \
	/* bit  12..11 - Driver Strength */		(((_drv) & 0x3) << 11) | \
	/* bits 10     - pad driver */				(((_slp) & 0x1) << 10) | \
	/* bit  09..07 - sleep mode */			(((_sleep) & 0xe) << 6) | \
	/* bits 06..04 - Edge Detection */		(((_edge) & 0x7) << 4) | \
	/* bits 03     - sleep mode */			(((_sleep) & 0x1) << 3) | \
	/* bits 02..00 - Alt-fun select */			((_afn) & 0x7))

/*
 * to facilitate the definition, the following macros are provided
 *
 * 				    offset, pull,pF, drv,dF, edge,eF ,afn,aF
 */
#define MFP_OFFSET_MASK		MFP(0xffff,    0,    0,   0,   0,   0,   0)
#define MFP_REG(x)			MFP(x,         0,    0,   0,   0,   0,   0)
#define MFP_REG_GET_OFFSET(x)	((x & MFP_OFFSET_MASK) >> 16)

#define MFP_AF0				MFP(0x0000,    0,    0,   0,   0,   0,   0)
#define MFP_AF1				MFP(0x0000,    0,    0,   0,   0,   0,   1)
#define MFP_AF2				MFP(0x0000,    0,    0,   0,   0,   0,   2)
#define MFP_AF3				MFP(0x0000,    0,    0,   0,   0,   0,   3)
#define MFP_AF4				MFP(0x0000,    0,    0,   0,   0,   0,   4)
#define MFP_AF5				MFP(0x0000,    0,    0,   0,   0,   0,   5)
#define MFP_AF6				MFP(0x0000,    0,    0,   0,   0,   0,   6)
#define MFP_AF7				MFP(0x0000,    0,    0,   0,   0,   0,   7)
#define MFP_AF_MASK			MFP(0x0000,    0,    0,   0,   0,   0,   7)

#define MFP_SLEEP_CTRL2		MFP(0x0000,    0,    0,   0,   0,   1,   0)
#define MFP_SLEEP_DIR		MFP(0x0000,    0,    0,   0,   0,   2,   0)
#define MFP_SLEEP_DATA		MFP(0x0000,    0,    0,   0,   0,   4,   0)
#define MFP_SLEEP_CTRL		MFP(0x0000,    0,    0,   0,   0,   8,   0)
#define MFP_SLEEP_MASK		MFP(0x0000,    0,    0,   0,   0, 0xf,   0)

#define MFP_SLEEP_NONE		MFP(0x0000,    0,    0,   0,   0,   0,   0)
#define MFP_SLEEP_FLOAT		MFP(0x0000,    0,    0,   0,   0, 0xb,   0)
#define MFP_SLEEP_OUTPUT_HIGH	MFP(0x0000,    0,    0,   0,   0, 0xd,   0)
#define MFP_SLEEP_OUTPUT_LOW	MFP(0x0000,    0,    0,   0,   0,   9,   0)

#define MFP_LPM_EDGE_NONE	MFP(0x0000,    0,    0,   0,   4,   0,   0)
#define MFP_LPM_EDGE_RISE	MFP(0x0000,    0,    0,   0,   1,   0,   0)
#define MFP_LPM_EDGE_FALL	MFP(0x0000,    0,    0,   0,   2,   0,   0)
#define MFP_LPM_EDGE_BOTH	MFP(0x0000,    0,    0,   0,   3,   0,   0)
#define MFP_LPM_EDGE_MASK	MFP(0x0000,    0,    0,   0,   7,   0,   0)

#define MFP_SLP_DI			MFP(0x0000,    0,    0,   1,   0,   0,   0)

#define MFP_DRIVE_VERY_SLOW	MFP(0x0000,    0,    0,   0,   0,   0,   0)
#define MFP_DRIVE_SLOW		MFP(0x0000,    0,    1,   0,   0,   0,   0)
#define MFP_DRIVE_MEDIUM	MFP(0x0000,    0,    2,   0,   0,   0,   0)
#define MFP_DRIVE_FAST		MFP(0x0000,    0,    3,   0,   0,   0,   0)
#define MFP_DRIVE_MASK		MFP(0x0000,    0,    3,   0,   0,   0,   0)

#define MFP_PULL_NONE		MFP(0x0000,    0,    0,   0,   0,   0,   0)
#define MFP_PULL_LOW		MFP(0x0000,    5,    0,   0,   0,   0,   0)
#define MFP_PULL_HIGH		MFP(0x0000,    6,    0,   0,   0,   0,   0)
#define MFP_PULL_BOTH		MFP(0x0000,    7,    0,   0,   0,   0,   0)
#define MFP_PULL_FLOAT		MFP(0x0000,    4,    0,   0,   0,   0,   0)
#define MFP_PULL_MASK		MFP(0x0000,    7,    0,   0,   0,   0,   0)

#define MFP_VALUE_MASK		(MFP_PULL_MASK | MFP_DRIVE_MASK | MFP_SLP_DI \
				| MFP_LPM_EDGE_MASK | MFP_SLEEP_MASK \
				| MFP_AF_MASK)
#define MFP_EOC			0xffffffff	/* indicates end-of-conf */

/*
 * Pin names and MFPR addresses
 */
#define MFPR_OFFSET(x)					((x)*4)
#define GPIO_00							MFPR_OFFSET(55)
#define GPIO_01							MFPR_OFFSET(56)
#define GPIO_02							MFPR_OFFSET(57)
#define GPIO_03							MFPR_OFFSET(58)
#define GPIO_04							MFPR_OFFSET(59)
#define GPIO_05							MFPR_OFFSET(60)
#define GPIO_06							MFPR_OFFSET(61)
#define GPIO_07							MFPR_OFFSET(62)
#define GPIO_08							MFPR_OFFSET(63)
#define GPIO_09							MFPR_OFFSET(64)
#define GPIO_10							MFPR_OFFSET(65)
#define GPIO_11							MFPR_OFFSET(66)
#define GPIO_12							MFPR_OFFSET(67)
#define GPIO_13							MFPR_OFFSET(68)
#define GPIO_14							MFPR_OFFSET(69)
#define GPIO_15							MFPR_OFFSET(70)
#define GPIO_16							MFPR_OFFSET(71)
#define GPIO_17							MFPR_OFFSET(72)
#define GPIO_18							MFPR_OFFSET(73)
#define GPIO_19							MFPR_OFFSET(74)
#define GPIO_20							MFPR_OFFSET(75)
#define GPIO_21							MFPR_OFFSET(76)
#define GPIO_22							MFPR_OFFSET(77)
#define GPIO_23							MFPR_OFFSET(78)
#define GPIO_24							MFPR_OFFSET(79)
#define GPIO_25							MFPR_OFFSET(80)
#define GPIO_26							MFPR_OFFSET(81)
#define GPIO_27							MFPR_OFFSET(82)
#define GPIO_28							MFPR_OFFSET(83)
#define GPIO_29							MFPR_OFFSET(84)
#define GPIO_30							MFPR_OFFSET(85)
#define GPIO_31							MFPR_OFFSET(86)
#define GPIO_32							MFPR_OFFSET(87)
#define GPIO_33							MFPR_OFFSET(88)
#define GPIO_34							MFPR_OFFSET(89)
#define GPIO_35							MFPR_OFFSET(90)
#define GPIO_36							MFPR_OFFSET(91)
#define GPIO_49							MFPR_OFFSET(104)
#define GPIO_50							MFPR_OFFSET(105)
#define GPIO_51							MFPR_OFFSET(106)
#define GPIO_52							MFPR_OFFSET(107)
#define GPIO_53							MFPR_OFFSET(108)
#define GPIO_54							MFPR_OFFSET(109)
#define GPIO_55							MFPR_OFFSET(160)
#define GPIO_56							MFPR_OFFSET(161)
#define GPIO_57							MFPR_OFFSET(162)
#define GPIO_58							MFPR_OFFSET(163)
#define GPIO_59							MFPR_OFFSET(164)
#define GPIO_60							MFPR_OFFSET(193)
#define GPIO_61							MFPR_OFFSET(194)
#define GPIO_62							MFPR_OFFSET(195)
#define GPIO_63							MFPR_OFFSET(196)
#define GPIO_64							MFPR_OFFSET(197)
#define GPIO_65							MFPR_OFFSET(198)
#define GPIO_66							MFPR_OFFSET(199)
#define GPIO_71							MFPR_OFFSET(110)
#define GPIO_72							MFPR_OFFSET(111)
#define GPIO_73							MFPR_OFFSET(112)
#define GPIO_74							MFPR_OFFSET(113)
#define GPIO_75							MFPR_OFFSET(114)
#define GPIO_76							MFPR_OFFSET(115)
#define GPIO_77							MFPR_OFFSET(116)
#define GPIO_78							MFPR_OFFSET(117)
#define GPIO_81							MFPR_OFFSET(120)
#define GPIO_82							MFPR_OFFSET(121)
#define GPIO_83							MFPR_OFFSET(122)
#define GPIO_84							MFPR_OFFSET(123)
#define GPIO_85							MFPR_OFFSET(124)
#define GPIO_86							MFPR_OFFSET(125)
#define GPIO_121						MFPR_OFFSET(203)
#define GPIO_124						MFPR_OFFSET(52)
#define GPIO_TDS_DIO0_DVC_0				MFPR_OFFSET(173)
#define GPIO_TDS_DIO1_DVC_1				MFPR_OFFSET(174)
#define GPIO_69_TDS_DIO2				MFPR_OFFSET(175)
#define GPIO_70_TDS_DIO3				MFPR_OFFSET(176)
#define GPIO_TDS_DIO4_QSPI_DAT3			MFPR_OFFSET(177)
#define GPIO_TDS_DIO5_QSPI_DAT2			MFPR_OFFSET(178)
#define GPIO_TDS_DIO6_QSPI_DAT1			MFPR_OFFSET(179)
#define GPIO_TDS_DIO7_QSPI_DAT0			MFPR_OFFSET(180)
#define GPIO_TDS_CLK_QSPI_CLK			MFPR_OFFSET(181)
#define GPIO_TDS_DIO8_QSPI_CS1			MFPR_OFFSET(182)
#define GPIO_TDS_DIO9_QSPI_CS2			MFPR_OFFSET(183)
#define GPIO_TDS_DIO10_QSPI_DQM			MFPR_OFFSET(184)
#define GPIO_79_TDS_DIO11				MFPR_OFFSET(185)
#define GPIO_80_TDS_DIO12				MFPR_OFFSET(186)
#define GPIO_TDS_DIO13_SDIO_DAT3		MFPR_OFFSET(187)
#define GPIO_TDS_DIO14_SDIO_DAT2		MFPR_OFFSET(188)
#define GPIO_TDS_DIO15_SDIO_DAT1		MFPR_OFFSET(189)
#define GPIO_TDS_DIO16_SDIO_DAT0		MFPR_OFFSET(190)
#define GPIO_TDS_DIO17_SDIO_CMD			MFPR_OFFSET(191)
#define GPIO_TDS_DIO18_SDIO_CLK			MFPR_OFFSET(192)
#define GPIO_USIM2_UCLK					MFPR_OFFSET(152)
#define GPIO_USIM2_UIO					MFPR_OFFSET(153)
#define GPIO_USIM2_URSTn				MFPR_OFFSET(154)
#define GPIO_SLAVE_RESET_OUT_N			MFPR_OFFSET(50)
#define GPIO_USIM_UCLK					MFPR_OFFSET(200)
#define GPIO_USIM_UIO					MFPR_OFFSET(201)
#define GPIO_USIM_URSTn					MFPR_OFFSET(202)
#define GPIO_VCXO_OUT					MFPR_OFFSET(54)
#define GPIO_VCXO_REQ					MFPR_OFFSET(53)
#define GPIO_PRI_TDI					MFPR_OFFSET(45)
#define GPIO_PRI_TMS					MFPR_OFFSET(46)
#define GPIO_PRI_TCK					MFPR_OFFSET(47)
#define GPIO_PRI_TDO					MFPR_OFFSET(48)
#define GPIO_MMC1_CD					MFPR_OFFSET(43)
#define GPIO_MMC1_CLK					MFPR_OFFSET(42)
#define GPIO_MMC1_CMD					MFPR_OFFSET(41)
#define GPIO_MMC1_DAT0					MFPR_OFFSET(40)
#define GPIO_MMC1_DAT1					MFPR_OFFSET(39)
#define GPIO_MMC1_DAT2					MFPR_OFFSET(38)
#define GPIO_MMC1_DAT3					MFPR_OFFSET(37)


/* Functions */
//void mfp_init(void);
// pls call mfp config function.
//void mfp_config(unsigned int  *mfp_cfgs);
//void mfp_dump(unsigned int mfpr_offset);
void mfp_config(unsigned int * mfp_cfgs);
void gpio_set_output(unsigned int port);
void gpio_set_value(unsigned int port, int value);

#endif
