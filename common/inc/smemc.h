/******************************************************************************
 *
 *	(C)Copyright 2005 - 2007 Marvell.
 *
 *
 ******************************************************************************/

#ifndef	__INC_SMEMC_H
#define	__INC_SMEMC_H

#define SMC_BASE 			0xD4283800

#define SMC_MSCx 			(SMC_BASE | 0x20)
#define SMC_SXCNFGx			(SMC_BASE | 0x30)
#define SMC_MEMCLKCFGx		(SMC_BASE | 0x68)
#define SMC_CSDFICFGx		(SMC_BASE | 0x90)
#define SMC_CSDFICFG2		(SMC_BASE | 0x98)
#define SMC_CLK_RET_DEL		(SMC_BASE | 0xB0)
#define SMC_ADV_RET_DEL		(SMC_BASE | 0xB4)
#define SMC_CSADRMAPx		(SMC_BASE | 0xC0)
#define SMC_CSADRMAP2		(SMC_BASE | 0xC8)
#define SMC_TCVCFGREF		(SMC_BASE | 0xD0)
#define SMC_SMC_WE_Apx		(SMC_BASE | 0xE0)
#define SMC_OE_Apx			(SMC_BASE | 0xF0)
#define SMC_ADV_Apx			(SMC_BASE | 0x100)

#endif

