/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/

/*******************************************************************************
 *                      M O D U L E     B O D Y
 *******************************************************************************
 *  COPYRIGHT (C) 2003, 2004 Intel Corporation.
 *
 *  This software as well as the software described in it is furnished under
 *  license and may only be used or copied in accordance with the terms of the
 *  license. The information in this file is furnished for informational use
 *  only, is subject to change without notice, and should not be construed as
 *  a commitment by Intel Corporation. Intel Corporation assumes no
 *  responsibility or liability for any errors or inaccuracies that may appear
 *  in this document or any software that may be provided in association with
 *  this document.
 *  Except as permitted by such license, no part of this document may be
 *  reproduced, stored in a retrieval system, or transmitted in any form or by
 *  any means without the express written consent of Intel Corporation.
 *
 *******************************************************************************
 *
 * Title:        UDC DRIVER - USB Device Controller header file
 *
 * Filename:     udc2_hw.h
 *
 * Author:
 *
 * Description:
 *
 * Notes:
 ******************************************************************************/

#if !defined(_U2CI_HW_H_)
#define      _U2CI_HW_H_

#include "mvUsbCore.h"       // for e.g. EHCI_CMD_RUN_STOP definition
#include "udc_def.h"        // for IS_BIT_ON macro definition

//#define UDC_MAX_UDC_ENDPOINTS       UDC_TOTAL_ENDPOINTS  /* including EP0 */
//#define UDC_MAX_UDC_ENDPOINTS       24  /* including EP0 */
//#define UDC_USB_MAX_ENDPOINTS       16  /* including EP0 */
#define USB2_EP0_MAX_PACKET_SIZE     (64)
//Hongji 201011

#define USB_REGISTERS_BASE          (0xd4208000)

#define USB_PHY_BASE                (0x5550A000)


/*******************************************/
/* USB Core Registers                  */
/*******************************************/

#define USB2_ID_REG                  (USB_REGISTERS_BASE + 0x00)
#define USB2_GENERAL_REG             (USB_REGISTERS_BASE + 0x04)
#define USB2_HOST_REG                (USB_REGISTERS_BASE + 0x08)
#define USB2_DEVICE_REG              (USB_REGISTERS_BASE + 0x0C)
#define USB2_TX_BUF_REG              (USB_REGISTERS_BASE + 0x10)
#define USB2_RX_BUF_REG              (USB_REGISTERS_BASE + 0x14)
#define USB2_TTTX_BUF_REG            (USB_REGISTERS_BASE + 0x18)
#define USB2_TTRX_BUF_REG            (USB_REGISTERS_BASE + 0x1C)
#define USB2_CAP_LENGTH_REG          (USB_REGISTERS_BASE + 0x100)
#define USB2_CAP_HCI_VERSION_REG     (USB_REGISTERS_BASE + 0x102)
#define USB2_CAP_HCS_PARAMS_REG      (USB_REGISTERS_BASE + 0x104)
#define USB2_CAP_HCC_PARAMS_REG      (USB_REGISTERS_BASE + 0x108)
#define USB2_CAP_DCI_VERSION_REG     (USB_REGISTERS_BASE + 0x120)
#define USB2_CAP_DCC_PARAMS_REG      (USB_REGISTERS_BASE + 0x124)
#define USB2_CMD_REG                (USB_REGISTERS_BASE + 0x140)
#define USB2_CMD_RUN_BIT             0
#define USB2_CMD_RUN_MASK            (1 << USB2_CMD_RUN_BIT)
#define USB2_CMD_RESET_BIT           1
#define USB2_CMD_RESET_MASK          (1 << USB2_CMD_RESET_BIT)
#define USB2_STATUS_REG              (USB_REGISTERS_BASE + 0x144)
#define USB2_INTR_REG                (USB_REGISTERS_BASE + 0x148)
#define USB2_FRAME_INDEX_REG         (USB_REGISTERS_BASE + 0x14C)
#define USB2_PERIODIC_LIST_BASE_REG  (USB_REGISTERS_BASE + 0x154)
#define USB2_DEV_ADDR_REG            (USB_REGISTERS_BASE + 0x154)
#define USB2_ASYNC_LIST_ADDR_REG     (USB_REGISTERS_BASE + 0x158)
#define USB2_ENDPOINT_LIST_ADDR_REG  (USB_REGISTERS_BASE + 0x158)
#define USB2_BURST_SIZE_REG          (USB_REGISTERS_BASE + 0x160)
#define USB2_TX_FILL_TUNING_REG      (USB_REGISTERS_BASE + 0x164)
#define USB2_TX_TT_FILL_TUNING_REG   (USB_REGISTERS_BASE + 0x168)
#define USB2_CONFIG_FLAG_REG         (USB_REGISTERS_BASE + 0x180)
#define USB2_PORTSC0_REG             (USB_REGISTERS_BASE + 0x184)
#define USB2_OTGSC_REG               (USB_REGISTERS_BASE + 0x1A4)
#define USB2_MODE_REG                (USB_REGISTERS_BASE + 0x1A8)
#define USB2_MODE_OFFSET             0
#define USB2_MODE_MASK               (3 << USB2_MODE_OFFSET)
#define USB2_MODE_HOST               (3 << USB2_MODE_OFFSET)
#define USB2_MODE_DEVICE             (2 << USB2_MODE_OFFSET)
#define USB2_ENDPT_NAK_REG           (USB_REGISTERS_BASE + 0x178)
#define USB2_ENDPT_NAK_EN_REG        (USB_REGISTERS_BASE + 0x17C)
#define USB2_ENDPT_SETUP_STAT_REG    (USB_REGISTERS_BASE + 0x1AC)
#define USB2_ENDPT_PRIME_REG         (USB_REGISTERS_BASE + 0x1B0)
#define USB2_ENDPT_FLUSH_REG         (USB_REGISTERS_BASE + 0x1B4)
#define USB2_ENDPT_STATUS_REG        (USB_REGISTERS_BASE + 0x1B8)
#define USB2_ENDPT_COMPLETE_REG      (USB_REGISTERS_BASE + 0x1BC)
#define USB2_ENDPT_CTRL_REG(ep)      (USB_REGISTERS_BASE + 0x1C0 + (ep*4))

/*******************************************/
/* USB PHY (UTMI) Registers           */
/*******************************************/

//Hongji modify 201011
#define USB_NEWPHY_BASE 0xd4207000
#define U2P_CTRL        (USB_NEWPHY_BASE+4)
#define U2PPLL_ADDR     (USB_NEWPHY_BASE+0x08)      // USB2 PHY PLL Control register
#define U2PTX_ADDR      (USB_NEWPHY_BASE+0x0c)      // USB2 PHY TX Control register
#define U2PRX_ADDR      (USB_NEWPHY_BASE+0x10)      // USB2 PHY RX Control register
#define U2PIVREF_ADDR   (USB_NEWPHY_BASE+0x14)      // USB2 PHY IVREF Control register
#define U2PT0_ADDR      (USB_NEWPHY_BASE+0x18)      // USB2 PHY Test 0 Control register
#define U2PT1_ADDR      (USB_NEWPHY_BASE+0x1c)      // USB2 PHY Test 1 Control register
#define U2PT2_ADDR      (USB_NEWPHY_BASE+0x20)      // USB2 PHY Test 2 Control register
//#define U2PT3_ADDR      0x5550A01C      // USB2 PHY Test 3 Control register
//#define U2PT4_ADDR      0x5550A020      // USB2 PHY Test 4 Control register
//#define U2PT5_ADDR      0x5550A024      // USB2 PHY Test 5 Control register
#define U2PT3_ADDR      (USB_NEWPHY_BASE+0X34)      // USB2 PHY Test 3 Control register
#define U2PT4_ADDR      (USB_NEWPHY_BASE+0x38)      // USB2 PHY Test 4 Control register
#define U2PT5_ADDR      (USB_NEWPHY_BASE+0x3c)

/*
 *
 *	THE BASE ADDRESSES
 *
 */

#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)
#define BIT_2 (1 << 2)
#define BIT_3 (1 << 3)
#define BIT_4 (1 << 4)
#define BIT_5 (1 << 5)
#define BIT_6 (1 << 6)
#define BIT_7 (1 << 7)
#define BIT_8 (1 << 8)
#define BIT_9 (1 << 9)
#define BIT_10 (1 << 10)
#define BIT_11 (1 << 11)
#define BIT_12 (1 << 12)
#define BIT_13 (1 << 13)
#define BIT_14 (1 << 14)
#define BIT_15 (1 << 15)
#define BIT_16 (1 << 16)
#define BIT_17 (1 << 17)
#define BIT_18 (1 << 18)
#define BIT_19 (1 << 19)
#define BIT_20 (1 << 20)
#define BIT_21 (1 << 21)
#define BIT_22 (1 << 22)
#define BIT_23 (1 << 23)
#define BIT_24 (1 << 24)
#define BIT_25 (1 << 25)
#define BIT_26 (1 << 26)
#define BIT_27 (1 << 27)
#define BIT_28 (1 << 28)
#define BIT_29 (1 << 29)
#define BIT_30 (1 << 30)
#define BIT_31 ((unsigned)1 << 31)

#define SHIFT0(Val)  (Val)
#define SHIFT1(Val)  ((Val) << 1)
#define SHIFT2(Val)  ((Val) << 2)
#define SHIFT3(Val)  ((Val) << 3)
#define SHIFT4(Val)  ((Val) << 4)
#define SHIFT5(Val)  ((Val) << 5)
#define SHIFT6(Val)  ((Val) << 6)
#define SHIFT7(Val)  ((Val) << 7)
#define SHIFT8(Val)  ((Val) << 8)
#define SHIFT9(Val)  ((Val) << 9)
#define SHIFT10(Val) ((Val) << 10)
#define SHIFT11(Val) ((Val) << 11)
#define SHIFT12(Val) ((Val) << 12)
#define SHIFT13(Val) ((Val) << 13)
#define SHIFT14(Val) ((Val) << 14)
#define SHIFT15(Val) ((Val) << 15)
#define SHIFT16(Val) ((Val) << 16)
#define SHIFT17(Val) ((Val) << 17)
#define SHIFT18(Val) ((Val) << 18)
#define SHIFT19(Val) ((Val) << 19)
#define SHIFT20(Val) ((Val) << 20)
#define SHIFT21(Val) ((Val) << 21)
#define SHIFT22(Val) ((Val) << 22)
#define SHIFT23(Val) ((Val) << 23)
#define SHIFT24(Val) ((Val) << 24)
#define SHIFT25(Val) ((Val) << 25)
#define SHIFT26(Val) ((Val) << 26)
#define SHIFT27(Val) ((Val) << 27)
#define SHIFT28(Val) ((Val) << 28)
#define SHIFT29(Val) ((Val) << 29)
#define SHIFT30(Val) ((Val) << 30)
#define SHIFT31(Val) ((unsigned int)(Val) << 31)

#if 0
#define	USB2_BASE	0xD4207000

/*
 *
 *	THE 16-bit WUKONG REGISTER DEFINES
 *
 */
#define USB2_PLL_CTRL_REG0	(USB2_BASE+0x04)
#define USB2_PLL_CTRL_REG1	(USB2_BASE+0x08)
#define USB2_TX_CTRL_REG0	(USB2_BASE+0x10)
#define USB2_TX_CTRL_REG1	(USB2_BASE+0x14)
#define USB2_TX_CTRL_REG2	(USB2_BASE+0x18)
#define USB2_RX_CTRL_REG0	(USB2_BASE+0x20)
#define USB2_RX_CTRL_REG1	(USB2_BASE+0x24)
#define USB2_RX_CTRL_REG2	(USB2_BASE+0x28)
#define USB2_ANA_REG0		(USB2_BASE+0x30)
#define USB2_ANA_REG1		(USB2_BASE+0x34)
#define USB2_ANA_REG2		(USB2_BASE+0x38)
#define USB2_DIG_REG0		(USB2_BASE+0x3C)
#define USB2_DIG_REG1		(USB2_BASE+0x40)
#define USB2_DIG_REG2		(USB2_BASE+0x44)
#define USB2_DIG_REG3		(USB2_BASE+0x48)
#define USB2_TEST_REG0		(USB2_BASE+0x4C)
#define USB2_TEST_REG1		(USB2_BASE+0x50)
#define USB2_TEST_REG2		(USB2_BASE+0x54)
#define USB2_CHARGER_REG0	(USB2_BASE+0x58)
#define USB2_OTG_REG0		(USB2_BASE+0x5C)
#define USB2_PHY_MON0		(USB2_BASE+0x60)
#define USB2_RESETVE_REG0	(USB2_BASE+0x64)

#define USB2_ICID_REG0		(USB2_BASE+0x78)
#define USB2_ICID_REG1		(USB2_BASE+0x7C)

/*
 *
 *	THE BIT DEFINES
 *
 */

/*		USB2_PLL_CTRL_REG0	0x04  	*/
#define USB2_PLL_FBDIV_BASE				0
#define USB2_PLL_FBDIV_MASK				SHIFT0(0xFF)
#define USB2_PLL_REFDIV_BASE			8
#define USB2_PLL_REFDIV_MASK			SHIFT8(0xF)
#define USB2_PLL_VDD12_BASE				12
#define USB2_PLL_VDD12_MASK				SHIFT12(0x3)
#define USB2_PLL_VDD18_BASE				14
#define USB2_PLL_VDD18_MASK				SHIFT14(0x3)

/*		USB2_PLL_CTRL_REG1	0x08  	*/
#define USB2_PLL_CAL12_BASE				0
#define USB2_PLL_CAL12_MASK				SHIFT0(0x3)
#define USB2_PLL_VCOCAL_START_BASE		2
#define USB2_PLL_VCOCAL_START_MASK      SHIFT2(0x1)
#define USB2_PLL_CLK_BLK_EN_BASE		3
#define USB2_PLL_CLK_BLK_EN_MASK        SHIFT3(0x1)
#define USB2_PLL_KVCO_BASE				4
#define USB2_PLL_KVCO_MASK				SHIFT4(0x7)
#define USB2_PLL_KVCO_EXT_BASE			7
#define USB2_PLL_KVCO_EXT_MASK          SHIFT7(0x1)
#define USB2_PLL_ICP_BASE				8
#define USB2_PLL_ICP_MASK				SHIFT8(0x7)
#define USB2_PLL_LOCKDET_ISEL_BASE		11
#define USB2_PLL_LOCKDET_ISEL_MASK		SHIFT11(0x1)
#define USB2_PLL_LOCK_BYPASS_BASE		12
#define USB2_PLL_LOCK_BYPASS_MASK		SHIFT12(0x1)
#define USB2_PLL_PU_PLL_BASE			13
#define USB2_PLL_PU_PLL_MASK			SHIFT13(0x1)
#define USB2_PLL_CONTROL_BY_PIN_BASE	14
#define USB2_PLL_CONTROL_BY_PIN_MASK	SHIFT14(0x1)
#define USB2_PLL_READY_BASE				15
#define USB2_PLL_READY_MASK				SHIFT15(0x1)

/*		USB2_TX_CTRL_REG0	0x10  	*/
#define USB2_TX_EXT_FS_RCAL_BASE		0
#define USB2_TX_EXT_FS_RCAL_MASK		SHIFT0(0xF)
#define USB2_TX_HS_RCAL_BASE			4
#define USB2_TX_HS_RCAL_MASK			SHIFT4(0xF)
#define USB2_TX_IMPCAL_VTH_BASE			8
#define USB2_TX_IMPCAL_VTH_MASK			SHIFT8(0x7)
#define USB2_TX_EXT_FS_RCAL_EN_BASE		11
#define USB2_TX_EXT_FS_RCAL_EN_MASK		SHIFT11(0x1)
#define USB2_TX_EXT_HS_RCAL_EN_BASE		12
#define USB2_TX_EXT_HS_RCAL_EN_MASK		SHIFT12(0x1)
#define USB2_TX_RCAL_START_BASE			13
#define USB2_TX_RCAL_START_MASK			SHIFT13(0x1)
#define USB2_TX_DATA_BLOCK_EN_BASE		14
#define USB2_TX_DATA_BLOCK_EN_MASK		SHIFT14(0x1)

/*		USB2_TX_CTRL_REG1	0x14	*/
#define USB2_TX_CK60_PHSEL_BASE			0
#define USB2_TX_CK60_PHSEL_MASK			SHIFT0(0xF)
#define USB2_TX_AMP_BASE				4
#define USB2_TX_AMP_MASK				SHIFT4(0x7)
#define USB2_TX_LOW_VDD_EN_BASE			7
#define USB2_TX_LOWVDD_MASK				SHIFT7(0x1)
#define USB2_TX_VDD12_BASE				8
#define USB2_TX_VDD12_MASK				SHIFT8(0x3)
#define USB2_TX_VDD15_BASE				10
#define USB2_TX_VDD15_MASK				SHIFT10(0x3)

/*		USB2_TX_CTRL_REG2	0x18 	*/
#define USB2_TX_DRV_SLEWRATE_BASE       10
#define USB2_TX_DRV_SLEWRATE_MASK       SHIFT10(0x3)

/*		USB2_RX_CTRL_REG0	0x20  	*/
#define USB2_RX_INTP_BASE				0
#define USB2_RX_INTP_MASK				SHIFT0(0x3)
#define USB2_RX_LPF_COEF_BASE			2
#define USB2_RX_LPF_COEF_MAKS			SHIFT2(0x3)
#define USB2_RX_SQ_THRESH_BASE			4
#define USB2_RX_SQ_THRESH_MASK			SHIFT4(0xF)
#define USB2_RX_DISCON_THRESH_BASE		8
#define USB2_RX_DISCON_THRESH_MASK		SHIFT8(0x3)
#define USB2_RX_SQ_LENGTH_BASE			10
#define USB2_RX_SQ_LENGTH_MASK			SHIFT10(0x3)
#define USB2_RX_ACQ_LENGTH_BASE			12
#define USB2_RX_ACQ_LENGTH_MASK			SHIFT12(0x3)
#define USB2_RX_USQ_LENGTH_BASE			14
#define USB2_RX_USQ_LENGTH_MASK			SHIFT14(0x1)
#define USB2_RX_PHASE_FREEZE_DLY_BASE	15
#define USB2_RX_PHASE_FREEZE_DLY_MASK	SHIFT15(0x1)

/*		USB2_RX_CTRL_REG1	0x24	*/
#define USB2_RX_S2T03_DLY_SEL_BASE		0
#define USB2_RX_S2T03_DLY_SEL_MASK		SHIFT0(0x3)
#define USB2_RX_CDR_FASTLOCK_EN_BASE	2
#define USB2_RX_CDR_FASTLOCK_EN_MASK	SHIFT2(0x1)
#define USB2_RX_CDR_COEF_SEL_BASE		3
#define USB2_RX_CDR_COEF_SEL_MASK		SHIFT3(0x1)
#define USB2_RX_EDGE_DET_SEL_BASE		4
#define USB2_RX_EDGE_DET_SEL_MASK		SHIFT4(0x3)
#define USB2_RX_DATA_BLOCK_LENGTH_BASE	6
#define USB2_RX_DATA_BLOCK_LENGTH_MASK	SHIFT6(0x3)
#define USB2_RX_CAP_SEL_BASE			8
#define USB2_RX_CAP_SEL_MASK			SHIFT8(0x7)
#define USB2_RX_EDGE_DET_EN_BASE		11
#define USB2_RX_EDGE_DET_EN_MASK		SHIFT11(0x1)
#define USB2_RX_DATA_BLOCK_EN_BASE		12
#define USB2_RX_DATA_BLOCK_EN_MASK		SHIFT12(0x1)
#define USB2_RX_EARLY_VOS_ON_EN_BASE	13
#define USB2_RX_EARLY_VOS_ON_EN_MASK	SHIFT13(0x1)

/*		USB2_RX_CTRL_REG2	0x28 	*/
#define USB2_RX_VDD12_BASE				0
#define USB2_RX_VDD12_MASK				SHIFT0(0x3)
#define USB2_RX_VDD18_BASE				2
#define USB2_RX_VDD18_MASK				SHIFT2(0x3)
#define USB2_RX_SQ_ALWAYS_ON_BASE		4
#define USB2_RX_SQ_ALWAYS_ON_MASK		SHIFT4(0x1)
#define USB2_RX_SQ_BUFFER_EN_BASE		5
#define USB2_RX_SQ_BUFFER_EN_MASK		SHIFT5(0x1)
#define USB2_RX_SAMPLER_CTRL_BASE		6
#define USB2_RX_SAMPLER_CTRL_MASK		SHIFT6(0x1)
#define USB2_RX_SQ_CM_BASE				7
#define USB2_RX_SQ_CM_MASK				SHIFT7(0x1)
#define USB2_RX_USQ_FILTER_BASE			8
#define USB2_RX_USQ_FILTER_MASK			SHIFT8(0x1)

/*		USB2_ANA					*/
/*		USB2_ANA_REG0	0x30  		*/
#define USB2_ANA_IPTAT_SEL_BASE			0
#define USB2_ANA_VDD_DIG_TOP_SEL_BASE	3
#define USB2_ANA_TOPVDD18_BASE			4
#define USB2_ANA_DIG_SEL_BASE			6
#define USB2_ANA_BG_VSEL_BASE			8

/*		USB2_ANA_REG1	0x34  		*/
#define USB2_ANA_PU_BASE				14

/*		USB2_OTG_REG0		0x5C 	*/
#define USB2_OTG_PU_BASE				3

/*		USB2_TEST_REG0		0x4C 	*/
#define USB2_TEST_PATTERN_BASE			0
#define USB2_TEST_MODE_BASE				8
#define USB2_TEST_BYPASS_BASE			11
#define USB2_TEST_LENGTH_BASE			12
#define USB2_TEST_LPBK_EN_BASE			14
#define USB2_TEST_DIG_LPBK_BASE			15
/*		USB2_TEST_REG1		0x50 	*/
#define USB2_TEST_XCVR_SELECT_BASE		0
#define USB2_TEST_OP_MODE				2
#define USB2_TEST_TERM_SELECT_BASE		4
#define USB2_TEST_TX_BITSTUFF_EN_BASE	5
#define USB2_TEST_SUSPENDM_BASE			6
#define USB2_TEST_UTMI_SEL_BASE			7
#define USB2_TEST_SKIP_BASE				8
#define USB2_TEST_RESET_BASE			12
#define USB2_TEST_EN_BASE				13
#define USB2_TEST_FLAG_BASE				14
#define USB2_TEST_DONE_BASE				15

/* -------------------- */

#define UTMI_BASE 		0xD4207100

#define	UTMI_CTRL					(UTMI_BASE+0x0004)	/* 32 bit	USB Control*/

#define UTMI_CTRL_PU_CLK                    BIT_23
#define UTMI_CTRL_PU_REF                    BIT_20
/*		Bit(s) UTMI_CTRL_RSRV_25_15 reserved */
#define	UTMI_CTRL_REG_DP_PULLDOWN			BIT_14			/* REG_DP_PULLDOWN */
#define	UTMI_CTRL_REG_DM_PULLDOWN			BIT_13			/* REG_DM_PULLDOWN */
#define	UTMI_CTRL_REG_ARC_DPDM_MODE			BIT_12			/* REG_ARC_DPDM_MODE */
/*		Bit(s) UTMI_CTRL_RSRV_11_2 reserved */
#define	UTMI_CTRL_PU_PLL					BIT_1			/* PU_PLL */
#define	UTMI_CTRL_PU						BIT_0			/* PU */
#endif




#define U2PRS_ADDR      0x5550A02C      // USB2 PHY Reserve register
#define U2PMN_ADDR      0x5550A030      // USB2 PHY Monitor register

#define AXI_CG_ADDR     0x5550A100
#define MMC_CG_ADDR     0x5550A104
#define U2O_CG_ADDR     0x5550A108      // U2O Clock Gate Register
#define U2H_CG_ADDR     0x5550A10C
#define BPMU_ASCR_ADDR  0x40F40000
#define BCCU_ACSR_ADDR  BPMU_ASCR_ADDR
#define BCCU_ACCR1_ADDR 0x41340020
#define D0CKEN_C        0x41340024      // D0 mode Clock Enable Register C
#define U2P480_ADDR     0x42404078      // USB2 PHY 480Mhz Control (U2P480)


/*******************************************/
/* USB Registers bit mask                  */
/*******************************************/
#define D0CKEN_C_OTG_CLK_ENABLE_MASK    0x00000440          /* mask to enable OTG AXI bus & Peripheral clocks (bits 6 & 10) */
#define USB_U2PT0_REG_ARC_DPDM_MODE     0x10000000
#define USB_U2PTX_REG_RCAL_START        0x00001000
#define USB_U2PRX_REG_BIT_SQ_THRESH     0x000000F0          /* SQ_THRESH 4:7, should be set to 0xF*/
#define USB_U2PPLL_PLL_READY            0x00800000
#define USB_U2PPLL_REG_BITS_ICP_MASK    0x00006000          /* BITS_ICP 12:14, should be set to 6 */
#define USB_U2PPLL_REG_KVCO_MASK        0x00020000          /* REG_KVCO 17:15, should be set to 4 */
#define USB_U2PPLL_REG_BIT_KVCO_EXT     0x00400000
#define USB_U2PIVREF_REG_RXVDD18        0x60000000          /* RXVDD18 30:29 */
#define USB_U2PIVREF_REG_BG_VSEL        0x00000300          /* BG_VSEL 9:8 */
#define USB_PORTSC0_REG_BG_BIT_PSPD     0x0C000000          /* PSPD 27:26 */

#define USB_ACCR1_PU_OTG_ENABLE         0x00001000
#define USB_ACCR1_PU_PLL_ENABLE         0x00000800
#define USB_ACCR1_PU_ENABLE             0x00000400
#define BPMU_ASCR_ADDR_RDH_BIT          0x80000000          /* RDH bit 31 */


#define USB2_REG_WRITE(rEG,dATA)        ((*(volatile UINT32 *)(rEG)) = dATA)
#define USB2_REG_WRITE8(rEG,dATA)       ((*(volatile UINT8 *)(rEG)) = dATA & 0xff )
#define USB2_REG_READ(rEG)              (*(volatile UINT32 *)(rEG))


#define USB2_ACTIVATE_HARDWARE()    (TURN_BIT_ON(((USB_DEV_STATE_STRUCT_PTR)_usbDeviceHandle)->DEV_PTR->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD, EHCI_CMD_RUN_STOP) )
#define USB2_DEACTIVATE_HARDWARE()  (TURN_BIT_OFF(((USB_DEV_STATE_STRUCT_PTR)_usbDeviceHandle)->DEV_PTR->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD, EHCI_CMD_RUN_STOP) )
#define USB2_IS_UDC_ACTIVATED()     (IS_BIT_ON(((USB_DEV_STATE_STRUCT_PTR)_usbDeviceHandle)->DEV_PTR->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD, EHCI_CMD_RUN_STOP))
#define USB2_IS_ATTACHED()          (IS_BIT_ON(((USB_DEV_STATE_STRUCT_PTR)_usbDeviceHandle)->DEV_PTR->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX, EHCI_PORTSCX_CURRENT_CONNECT_STATUS))

#define USB2_IS_HI_SPEED()        	((USB2_REG_READ(USB2_PORTSC0_REG) & USB_PORTSC0_REG_BG_BIT_PSPD)>>26 == MV_USB_SPEED_HIGH)
#define USB2_IS_FULL_SPEED()        ((USB2_REG_READ(USB2_PORTSC0_REG) & USB_PORTSC0_REG_BG_BIT_PSPD)>>26 == MV_USB_SPEED_FULL)


#endif /*_U2CI_HW_H_*/
