#ifndef FREQCHANGE_H
#define FREQCHANGE_H

#include "common.h"

//#include "gbl_types.h"
//#include "acipc.h"

typedef enum
{
	DDR_FREQUENCY_156,
	DDR_FREQUENCY_312,
	DDR_FREQUENCY_400,
	DDR_FREQUENCY_416 = DDR_FREQUENCY_400,
	DDR_FREQUENCY_528,
	DDR_FREQUENCY_624,
	DDR_FREQUENCY_667_800,
	DDR_FREQUENCY_INVALID
} DDR_FREQUENCY;


#define BU_REG_READ(x) (*(volatile unsigned long *)(x))
#define BU_REG_WRITE(x,y) ((*(volatile unsigned long *)(x)) = y )

/***************freq_change*/

#define PMUA_DM_CC_CP    0xd4282808
#define PMUA_CP_IMR      0xd4282870
#define PMUA_CC_CP       0xd4282800
#define PMUA_CC_AP       0xd4282804
#define PMUA_CP_ISR      0xd4282878
#define PMU_FCCR_T       0xd4050008
#define PMUA_CP_STATUS   0xd42828c4
#define PMUA_AP_ISR      0xd42828a0

#define PMU_PLL2CR       0xd4050034

#define PP_MAX_NUM 8
#define PP_MAX_NUM_NZ3 18

#if defined(NEZHA3_1826)

#define CPMU_DVC_CP       (0xD0250000+0x0080)
#define CPMU_DVC_DP       (0xD0250000+0x0084)
#define PMU_DVC_AP		  (0xd4050000+0x2020)
#define PMU_DVC_APSUB	  (0xd4050000+0x202C)
#define PMU_DVC_CHIP	  (0xd4050000+0x2030)
#define	CP_DVC_LV_MASK    ((CHIP_IS_HELAN4_A0 || CHIP_IS_NEZHA3_A0) ? 0xf : 0x7)//Helan4 or not
#define	CP_DVC_ACTIVE_OFF ((CHIP_IS_HELAN4_A0 || CHIP_IS_NEZHA3_A0) ? 8 : 4)    //Helan4 or not
#define	CP_DVC_CHG_REQ    ((CHIP_IS_HELAN4_A0 || CHIP_IS_NEZHA3_A0) ? 15 : 7)   //Helan4 or not

#endif

#define PMU_DVC_CTRL     0xd4052000
#define PMU_DVC_STBL_0_1 0xd4052004
#define PMU_DVC_STBL_1_2 0xd4052008
#define PMU_DVC_STBL_2_3 0xd405200C
#define PMU_DVC_CP       0xd4052024
#define PMU_DVC_DSP      0xd4052028
#define PMU_DVC_IMR      0xd4052050
#define PMU_DVC_ISR      0xd4052054
#define PMU_DVC_STATUS   0xd4052040
#define PMU_DVC_EX_STR   0xd405205C
#define PMU_CCGR_T					(0xd4050000+0x0024)
#define PMUA_DM_CC_AP				(0xD4282800+0x000C)
#define PMUA_DFC_AP					(0xD4282800+0x0180)
#define	PMUA_DFC_CP        			(0xD4282800+0x0184)
#define	PMUA_DFC_STATUS        		(0xD4282800+0x0188)
#define	PMUA_DFC_LEVEL0        		(0xD4282800+0x0190)
#define	PMUA_DFC_LEVEL1        		(0xD4282800+0x0194)
#define	PMUA_DFC_LEVEL2        		(0xD4282800+0x0198)
#define	PMUA_DFC_LEVEL3        		(0xD4282800+0x019C)
#define	PMUA_DFC_LEVEL4        		(0xD4282800+0x01A0)
#define	PMUA_DFC_LEVEL5        		(0xD4282800+0x01A4)
#define	PMUA_DFC_LEVEL6        		(0xD4282800+0x01A8)
#define	PMUA_DFC_LEVEL7        		(0xD4282800+0x01AC)

#define MAX_PPNUM 4 /* ULC */
#define MAX_DFCLVL_NUM 5
#define DFC_MAGIC_FLAG 0x4446434C

#define MAX_CPDVC_NUM_V1 5
#define DVC_MAGIC_V1 0x44564301

#if defined(NEZHA3_1826)

#define	CPMU_DFC_CP        			(0xD0250000+0x0090)
#define	CPMU_DFC_DP        			(0xD0250000+0x0094)
#define	CPMU_DFC_STATUS        		(0xD0250000+0x0098)

#endif

#ifdef EDEN_1928

#define CPMU_DDRDFC_CTRL   0xD025003C
#define CPMU_DVC           0xD025009C

#define DFC_INT            (1u<<31)
#define DFC_INT_MSK        (1<<30)
#define DFC_INT_CLR        (1<<29)
#define DFC_INT_PROGRESS   (1<<28)
#define DFC_HW_EN          (1<<5)

#endif

#if defined(NEZHA3_1826)
#define HWDFC_ENABLE		1
#else
#define HWDFC_ENABLE		0
#endif

//ICAT EXPORTED ENUM
typedef enum {
	_HWDFC_TEST_DISABEL=0,
	_HWDFC_TEST_ACTIVE_LPM_CORE,
	_HWDFC_TEST_ACTIVE_LPM,
	_HWDFC_TEST_ACTIVE,
	_HWDFC_TEST_CORE,	
	_HWDFC_TEST_NEW_API,	
} HWDFC_TEST_TYPE;
typedef enum 
{
    cpu_PLL1_156MHZ   = 0x0,   //PPL1:624,DIV:4; 
    cpu_PLL1_208MHZ   = 0x1,   //PPL1:624,DIV:3;  PPL1:832,DIV:4; 
    cpu_PLL1_312MHZ   = 0x2,   //PPL1:624,DIV:2; 
    cpu_PLL1_416MHZ   = 0x3,   //PPL1:832,DIV:2; 
    cpu_PLL1_624MHz   = 0x4,    
    cpu_PLL1_832MHz   = 0x5,
    cpu_PLL1_1248MHz  = 0x6,
    cpu_PLL2          = 0x7,
    cpu_PLL2P         = 0x8,
    cpu_PLL_NO_SET    = 0xFF,
}CPU_PLL_T;
//ICAT EXPORTED ENUM
typedef enum {
	_HWDFC_DISABEL=0,
	_HWDFC_ENABLE,
} HWDFC_TYPE;
typedef enum
{
	core_PLL1_416MHz = 0x0,
	core_PLL1_624MHz = 0x1,
	core_PLL2 = 0x2,
	core_PLL1_312MHz = 0x3,
	core_PLL_NO_SET = 0xFF,
} CORE_PLL_FREQ_T;


typedef enum
{
	ACLK_PLL1_416MHz = 0x0,
	ACLK_PLL1_624MHz = 0x1,
	ACLK_PLL2 = 0x2,
	ACLK_PLL1_312MHz = 0x3,
} ACLK_PLL_FREQ_T;

typedef enum
{
    core_PLL1_832MHz = 0x0,
    core_PLL1_1248MHz = 0x1,
	core_PLL2OUT = 0x2,
    core_PLL2P = 0x3,
} CORE_PLL_FREQ_Nezha2_T;

typedef enum
{
	DCLK_PLL1_416MHz = 0x0,
	DCLK_PLL1_624MHz = 0x1,
	DCLK_PLL2 = 0x2,
	DCLK_PLL2_DIV_2_5 = 0x3,
} DCLK_PLL_FREQ_T;

//ICAT EXPORTED ENUM
typedef enum {
	CFG_PP_0 = 0,
	CFG_PP_1,
	CFG_PP_2,
	CFG_PP_3,
	CFG_PP_4,
	CFG_PP_5,
	CFG_PP_6,
	CFG_PP_7,
	CFG_PP_AUTO = 0xFF, // CQ00023417
} PP_TYPE;

// ICAT EXPORTED ENUM - add by zhangwl for PP chang - CQ00023417
typedef enum {
	CPIDLE_0_5 = 5,
	CPIDLE_1_0 = 10,
	CPIDLE_1_5 = 15,
	CPIDLE_2_0 = 20,
	CPIDLE_2_5 = 25,
	CPIDLE_3_0 = 30,
	CPIDLE_3_5 = 35,
	CPIDLE_4_0 = 40,
	CPIDLE_4_5 = 45,
	CPIDLE_5_0 = 50,
	CPIDLE_5_5 = 55,
	CPIDLE_6_0 = 60,
	CPIDLE_6_5 = 65,
	CPIDLE_7_0 = 70,
	CPIDLE_7_5 = 75,
	CPIDLE_8_0 = 80,
	CPIDLE_8_5 = 85,
	CPIDLE_9_0 = 90,
	CPIDLE_9_5 = 95,
	CPIDLE_DEFAULT = 0xFF,
} PP_CPIDLE_T;

//ICAT EXPORTED ENUM
typedef enum {
	LTE_DMA_78M  = 0x7F,
	LTE_DMA_156M = 0x5F,
	LTE_DMA_208M = 0x4B,
	LTE_DMA_312M = 0x4F,
} LTE_DMA_CLOCK;

//ICAT EXPORTED ENUM
typedef enum
{
	VOLTAGE_0_65    = 0x04,
	VOLTAGE_0_70    = 0x08,
	VOLTAGE_0_75    = 0x0C,
	VOLTAGE_0_775   = 0x0E,
	VOLTAGE_0_7875  = 0x0F,
	VOLTAGE_0_80    = 0x10,
	VOLTAGE_0_8125  = 0x11,
	VOLTAGE_0_825   = 0x12,
	VOLTAGE_0_8375  = 0x13,
	VOLTAGE_0_85    = 0x14,
	VOLTAGE_0_8625  = 0x15,
	VOLTAGE_0_875   = 0x16,
	VOLTAGE_0_8875  = 0x17,
	VOLTAGE_0_90    = 0x18,
	VOLTAGE_0_9125  = 0x19,
	VOLTAGE_0_925   = 0x1A,
	VOLTAGE_0_9375  = 0x1B,
	VOLTAGE_0_95    = 0x1C,
	VOLTAGE_0_9625  = 0x1D,
	VOLTAGE_0_975   = 0x1E,
	VOLTAGE_0_9875  = 0x1F,
	VOLTAGE_1_00    = 0x20,
	VOLTAGE_1_0125  = 0x21,
	VOLTAGE_1_025   = 0x22,
	VOLTAGE_1_0375  = 0x23,
	VOLTAGE_1_05    = 0x24,
	VOLTAGE_1_0625  = 0x25,
	VOLTAGE_1_075   = 0x26,
	VOLTAGE_1_0875  = 0x27,
	VOLTAGE_1_10    = 0x28,
	VOLTAGE_1_1125  = 0x29,
	VOLTAGE_1_125   = 0x2A,
	VOLTAGE_1_1375  = 0x2B,
	VOLTAGE_1_15    = 0x2C,
	VOLTAGE_1_1625  = 0x2D,
	VOLTAGE_1_175   = 0x2E,
	VOLTAGE_1_1875  = 0x2F,
	VOLTAGE_1_20    = 0x30,
	VOLTAGE_1_2125  = 0x31,
	VOLTAGE_1_225   = 0x32,
	VOLTAGE_1_2375  = 0x33,
	VOLTAGE_1_25    = 0x34,
	VOLTAGE_1_2625  = 0x35,
	VOLTAGE_1_275   = 0x36,
	VOLTAGE_1_2875  = 0x37,
	VOLTAGE_1_30    = 0x38,
	VOLTAGE_1_35    = 0x3C,
	VOLTAGE_DEFAULT = 0xFF,
} PP_VOLTAGE_T;

//ICAT EXPORTED ENUM
typedef enum
{
	MIPS_5   = 5,
	MIPS_1_0 = 10,
	MIPS_1_5 = 15,
	MIPS_2_0 = 20,
	MIPS_2_5 = 25,
	MIPS_3_0 = 30,
	MIPS_3_5 = 35,
	MIPS_4_0 = 40,
	MIPS_4_5 = 45,
	MIPS_5_0 = 50,
	MIPS_5_5 = 55,
	MIPS_6_0 = 60,
	MIPS_6_5 = 65,
	MIPS_7_0 = 70,
	MIPS_7_5 = 75,
	MIPS_8_0 = 80,
	MIPS_8_5 = 85,
    MIPS_9_0 = 90,
    MIPS_9_5 = 95,
	MIPS_MAX = 0xFF,
} Mips_Threshold;

//ICAT EXPORTED STRUCT
typedef struct {
	PP_TYPE ProductPoint;
	Mips_Threshold  HighWaterMark; //add by zhangwl for PP chang - CQ00023417
	Mips_Threshold  LowWaterMark;  //add by zhangwl for PP chang - CQ00023417
	LTE_DMA_CLOCK LTEDmaClock;
	PP_VOLTAGE_T ActiveVoltage;
	PP_VOLTAGE_T SleepVoltage;
} LTE_CONFIG_S;

typedef struct
{
	CORE_PLL_FREQ_T core_PLL_use;
	ACLK_PLL_FREQ_T aclk_PLL_use;
	DCLK_PLL_FREQ_T dclk_PLL_use;
	UINT16 PLL2_value;
	UINT8  PCLK_div;
	UINT8  XPCLK_div;
	UINT8  PDCLK_div;
	UINT8  ACLK_div;
	UINT8  DCLK_div;
	UINT8  DCLK_SYNC;
	PP_VOLTAGE_T PP_Voltage;
} ProductPoint_T;

typedef struct
{
	CORE_PLL_FREQ_Nezha2_T core_PLL_use;
	ACLK_PLL_FREQ_T aclk_PLL_use;
	DCLK_PLL_FREQ_T dclk_PLL_use;
	UINT16 PLL2_value;
	UINT8  PCLK_div;
	UINT8  XPCLK_div;
	UINT8  PDCLK_div;
	UINT8  ACLK_div;
	UINT8  DCLK_div;
	UINT8  DCLK_SYNC;
	PP_VOLTAGE_T PP_Voltage;
} ProductPoint_Nezha2_T;

#ifdef NEZHA3_1826

typedef enum
{
    CP_PLL1_416MHz = 0x0,
    CP_PLL1_624MHz = 0x1,
    CP_PLL1_832MHz = 0x2,
	CP_PLL2OUT = 0x3,
} CORE_PLL_FREQ_Nezha3_T;

typedef enum
{
	CP_ACLK_312MHz = 0x0,
	CP_ACLK_416MHz = 0x1,
} ACLK_PLL_FREQ_Nezha3_T;

typedef struct
{
	CORE_PLL_FREQ_Nezha3_T core_PLL_use;
	ACLK_PLL_FREQ_Nezha3_T cpss_aclk_PLL_use;
	ACLK_PLL_FREQ_Nezha3_T lte_aclk_PLL_use;
	DCLK_PLL_FREQ_T dclk_PLL_use;
	UINT16 PLL2_value;
	UINT8  PCLK_div;
	UINT8  bus_clk_div;
	UINT8  mem_clk_div;
	UINT8  CPSS_ACLK_div;
	UINT8  LTE_ACLK_div;
	UINT8  DCLK_div;
	PP_VOLTAGE_T PP_Voltage;
} ProductPoint_Nezha3_T;

typedef enum
{
	LTE_ACLK_624MHz = 0x0,
	LTE_ACLK_416MHz = 0x1,
} LTE_ACLK_PLL_FREQ_Nezha3_Z3_T;

typedef enum
{
	CPSS_ACLK_312MHz = 0x0,
	CPSS_ACLK_416MHz = 0x1,
} CPSS_ACLK_PLL_FREQ_Nezha3_Z3_T;

typedef struct
{
	CORE_PLL_FREQ_Nezha3_T core_PLL_use;
	CPSS_ACLK_PLL_FREQ_Nezha3_Z3_T cpss_aclk_PLL_use;
	LTE_ACLK_PLL_FREQ_Nezha3_Z3_T lte_aclk_PLL_use;
	DCLK_PLL_FREQ_T dclk_PLL_use;
	UINT16 PLL2_value;
	UINT8  PCLK_div;
	UINT8  bus_clk_div;
	UINT8  mem_clk_div;
	UINT8  CPSS_ACLK_div;
	UINT8  LTE_ACLK_div;
	UINT8  DCLK_div;
	PP_VOLTAGE_T PP_Voltage;
} ProductPoint_Nezha3_Z3_T;
#endif

typedef enum
{
	FC_OK,
	FC_PP_NOT_EXIST,
	FC_ERROR,
	FC_NOT_SUPPORT,
} FC_RTN_CODE_T;

typedef enum
{
	CP_DVC_LV0  =  0,
	CP_DVC_LV1  =  1,
	CP_DVC_LV2  =  2,
	CP_DVC_LV3  =  3,
    CP_DVC_LV4  =  4,
    CP_DVC_LV5  =  5,
    CP_DVC_LV6  =  6,
    CP_DVC_LV7  =  7,
    CP_DVC_LV8  =  8,
    CP_DVC_LV9  =  9,
    CP_DVC_LV10 = 10,
    CP_DVC_LV11 = 11,
    CP_DVC_LV12 = 12,
    CP_DVC_LV13 = 13,
    CP_DVC_LV14 = 14,
    CP_DVC_LV15 = 15,
	
	CP_DVC_NO_SET = 0xFF,
} CP_DVC_LEVEL ;

typedef enum
{
	DSP_DVC_LV0  = 0,
	DSP_DVC_LV1  = 1,
	DSP_DVC_LV2  = 2,
	DSP_DVC_LV3  = 3,
	DSP_DVC_LV4  =  4,
    DSP_DVC_LV5  =  5,
    DSP_DVC_LV6  =  6,
    DSP_DVC_LV7  =  7,
    DSP_DVC_LV8  =  8,
    DSP_DVC_LV9  =  9,
    DSP_DVC_LV10 = 10,
    DSP_DVC_LV11 = 11,
    DSP_DVC_LV12 = 12,
    DSP_DVC_LV13 = 13,
    DSP_DVC_LV14 = 14,
    DSP_DVC_LV15 = 15,
    
	DSP_DVC_NO_SET = 0xFF,
} DSP_DVC_LEVEL ;

typedef enum
{
    SVC_VL0 = 0,
    SVC_VL1,
    SVC_VL2,
    SVC_VL3,
}  CRN_SVC_e;

#define REGS_GEU_BASE                     0xD4201000
#define EFUSE_DATA_BASE			REGS_GEU_BASE						/* efuse bank base address */

#define REG_GEU_GEU_STATUS                                        ASR_ADDR(REGS_GEU_BASE, 0X000)/*GEU Status Register*/
#define REG_GEU_GEU_CONFIG                                       ASR_ADDR(REGS_GEU_BASE, 0X004)/*GEU Configuration Register*/
#define REG_GEU_GEU_INIT_KEY_VALUE                          ASR_ADDR(REGS_GEU_BASE,0X008)/*GEU Initial Key Value Register*/
#define REG_GEU_GEU_INPUT_DATA_ENC_DEC                 ASR_ADDR(REGS_GEU_BASE, 0X028)/*Input Data for Encryption/Decryption Register*/
#define REG_GEU_GEU_FUSE_PROG_VAL1                        ASR_ADDR(REGS_GEU_BASE, 0X038)/*Fuse Programming Value[127:0] in Software Fuse Programming Mode Register*/
#define REG_GEU_GEU_FUSE_PROG_VAL2                        ASR_ADDR(REGS_GEU_BASE, 0X048)/*Fuse Programming Value[255:128] in Software Fuse Programming Mode Register*/
#define REG_GEU_GEU_OUT_DATA_AFTER_ENC_DEC         ASR_ADDR(REGS_GEU_BASE, 0X058)/*Output Data after Encryption/Decryption Register*/
#define REG_GEU_GEU_SRAM_SPACE_ROUND_KEY_TABLE ASR_ADDR(REGS_GEU_BASE, 0X068)/*SRAM Space for Round Key Table Register*/
#define REG_GEU_ROM_SPACE_SBOX_TABLE                    ASR_ADDR(REGS_GEU_BASE, 0X168)/*ROM Space for SBOX Table Register*/
#define REG_GEU_GEU_ROM_SPACE_INV_SBOX_TABLE     ASR_ADDR(REGS_GEU_BASE, 0X268)/*ROM Space for Inverse SBOX Table Register*/
#define REG_GEU_GEU_OTP_DATA1                                 ASR_ADDR(REGS_GEU_BASE, 0X368)/*One Time Programmable Data Register 1*/
#define REG_GEU_GEU_OTP_DATA2                                 ASR_ADDR(REGS_GEU_BASE, 0X36C)/*One Time Programmable Data Register 2*/
#define REG_GEU_GEU_OTP_DATA3                                 ASR_ADDR(REGS_GEU_BASE, 0X370)/*One Time Programmable Data Register 3*/
#define REG_GEU_GEU_OTP_DATA4                                 ASR_ADDR(REGS_GEU_BASE, 0X374)/*One Time Programmable Data Register 4*/
#define REG_GEU_PRNG_CTRL_REG                                 ASR_ADDR(REGS_GEU_BASE, 0X3a8)/*PRNG Control Register*/
#define REG_GEU_GEU_FUSE_VAL_APCFG1                       ASR_ADDR(REGS_GEU_BASE, 0X404)/*Fuse Value for Application Subsystem Config Register 1*/
#define REG_GEU_GEU_FUSE_VAL_APCFG2                       ASR_ADDR(REGS_GEU_BASE, 0X408)/*Fuse Value for Application Subsystem Config Register 2*/
#define REG_GEU_GEU_FUSE_VAL_APCFG3                       ASR_ADDR(REGS_GEU_BASE, 0X40C)/*Fuse Value for Application Subsystem Config Register 3*/
#define REG_GEU_GEU_FUSE_BANK0_127_96                   ASR_ADDR(REGS_GEU_BASE, 0X410)/*Fuse Bank0 127 96 Register*/
#define REG_GEU_GEU_FUSE_BANK0_239_208                   ASR_ADDR(REGS_GEU_BASE, 0X414)/*Fuse Bank0 239 208 Register*/
#define REG_GEU_GEU_FUSE_BANK0_207_192                   ASR_ADDR(REGS_GEU_BASE, 0X418)/*Fuse Bank0 207 192 Register*/
#define REG_GEU_GEU_FUSE_BANK3_223_192                   ASR_ADDR(REGS_GEU_BASE, 0X41c)/*Fuse Bank3 223 192 Register*/
#define REG_GEU_GEU_FUSE_BANK3_255_224                   ASR_ADDR(REGS_GEU_BASE, 0X420)/*Fuse Bank3 255 224 Register*/
#define REG_GEU_GEU_FUSE_VAL_ROOT_KEY1                  ASR_ADDR(REGS_GEU_BASE, 0X424)/*Fuse Value for Root Key Register 1*/
#define REG_GEU_GEU_FUSE_VAL_ROOT_KEY2                  ASR_ADDR(REGS_GEU_BASE, 0X428)/*Fuse Value for Root Key Register 2*/
#define REG_GEU_GEU_FUSE_VAL_ROOT_KEY3                  ASR_ADDR(REGS_GEU_BASE, 0X42C)/*Fuse Value for Root Key Register 3*/
#define REG_GEU_GEU_FUSE_VAL_ROOT_KEY4                  ASR_ADDR(REGS_GEU_BASE, 0X430)/*Fuse Value for Root Key Register 4*/
#define REG_GEU_GEU_FUSE_VAL_ROOT_KEY5                  ASR_ADDR(REGS_GEU_BASE, 0X434)/*Fuse Value for Root Key Register 5*/
#define REG_GEU_GEU_FUSE_VAL_ROOT_KEY6                  ASR_ADDR(REGS_GEU_BASE, 0X438)/*Fuse Value for Root Key Register 6*/
#define REG_GEU_GEU_FUSE_VAL_ROOT_KEY7                  ASR_ADDR(REGS_GEU_BASE, 0X43C)/*Fuse Value for Root Key Register 7*/
#define REG_GEU_GEU_FUSE_VAL_ROOT_KEY8                  ASR_ADDR(REGS_GEU_BASE, 0X440)/*Fuse Value for Root Key Register 8*/
#define REG_GEU_GEU_FUSE_VAL_OEM_HASH_1_8            ASR_ADDR(REGS_GEU_BASE, 0X444)/*Fuse Value for OEM Hash Key Registers 1- 8*/
#define REG_GEU_GEU_FUSE_STATUS                                ASR_ADDR(REGS_GEU_BASE, 0X484)/*Fuse Operation Status Register*/
#define REG_GEU_GEU_HW_RANDOM_NUM_GEN                 ASR_ADDR(REGS_GEU_BASE, 0X488)/*Hardware Random Number Generator Register*/
#define REG_GEU_GEU_FUSE_BANK0_191_160                   ASR_ADDR(REGS_GEU_BASE, 0X48C)/*Fuse Bank0 191 160 Register*/
#define REG_GEU_BLOCK3_RESERVED_0                            ASR_ADDR(REGS_GEU_BASE, 0X490)/*Block 3 Reserved Fuse Bit Register 0*/
#define REG_GEU_BLOCK3_RESERVED_1                            ASR_ADDR(REGS_GEU_BASE, 0X494)/*Block 3 Reserved Fuse Bit Register 1*/
#define REG_GEU_BLOCK3_RESERVED_2                            ASR_ADDR(REGS_GEU_BASE, 0X498)/*Block 3 Reserved Fuse Bit Register 2*/
#define REG_GEU_BLOCK3_RESERVED_3                            ASR_ADDR(REGS_GEU_BASE, 0X49c)/*Block 3 Reserved Fuse Bit Register 3*/
#define REG_GEU_BLOCK3_RESERVED_4                            ASR_ADDR(REGS_GEU_BASE, 0X4A0)/*Block 3 Reserved Fuse Bit Register 4*/
#define REG_GEU_BLOCK3_RESERVED_5                            ASR_ADDR(REGS_GEU_BASE, 0X4A4)/*Block 3 Reserved Fuse Bit Register 5*/
#define REG_GEU_GEU_FUSE_BANK0_159_128                   ASR_ADDR(REGS_GEU_BASE, 0X4A8)/*Fuse Bank0 159 128 Register*/
#define REG_GEU_GEU_FUSE_BANK0_255_240                   ASR_ADDR(REGS_GEU_BASE, 0X4C8)/*Fuse Bank0 255 240 Register*/

#define EFUSE_UUID_BASE		REG_GEU_GEU_FUSE_VAL_APCFG1			      /* bank 0,chip UUID */
#define EFUSE_HUK_BASE			REG_GEU_GEU_FUSE_VAL_ROOT_KEY1		             /* bank 1,Hardware Unique Key */
#define EFUSE_SSK_BASE			REG_GEU_GEU_FUSE_VAL_OEM_HASH_1_8		       /* bank 2,Secret Symmetric Key */
#define EFUSE_BLOCK3_BASE         REG_GEU_BLOCK3_RESERVED_0                            /* bank3,Block 3 Reserved Fuse Bit Register 0*/

#define GEU_CONFIG				REG_GEU_GEU_CONFIG

#define   REG_PMU_PMU_AES_CLK_RES_CTRL         0xD4282868
#define	FUSE_BANK_SIZE			(256/32)

#define REG_MAIN_CCGR   0xD4050024
#define SE_CLOCK_REG			REG_PMU_PMU_AES_CLK_RES_CTRL			//PMUap:  PMU_AES_CLK_RES_CTRL

/* bits definitions for register REG_GEU_GEU_CONFIG */
#define BIT_STICKY_CONTROL_BIT                            ( BIT(30) )
#define BIT_FUSE_POWER_BYPASS                             ( BIT(28) )
#define BITS_FUSE_CLOCK_DIVIDER(_X_)                      ( (_X_) << 25 & (BIT(25)|BIT(26)|BIT(27)) )
#define BIT_FUSE_CLOCK_DISABLE                            ( BIT(23) )
#define BIT_FUSE_SOFTWARE_RESET                           ( BIT(22) )
#define BITS_FUSE_BLOCK_NUMBER(_X_)                       ( (_X_) << 18 & (BIT(18)|BIT(19)|BIT(20)|BIT(21)) )
#define BIT_HIGH_VOLT_ENABLE                              ( BIT(17) )
#define BIT_BURN_FUSE_ENABLE                              ( BIT(16) )
#define BIT_FUSE_LOCK                                     ( BIT(15) )
#define BIT_ENABLE_SOFT_FUSE_PROG                         ( BIT(14) )
#define BIT_SEL_FUSE_B                                    ( BIT(13) )
#define BIT_GE_FUSE_READY205                              ( BIT(1) )
#define BIT_FUSE_BURN_DONE                                ( BIT(0) )

#define STICKY_CONTROL_BIT                  BIT_STICKY_CONTROL_BIT
#define FUSE_POWER_BYPASS                   BIT_FUSE_POWER_BYPASS
#define FUSE_CLOCK_DIVIDER(_X_)             BITS_FUSE_CLOCK_DIVIDER(_X_)
#define FUSE_CLOCK_DISABLE                  BIT_FUSE_CLOCK_DISABLE
#define FUSE_SOFTWARE_RESET                 BIT_FUSE_SOFTWARE_RESET
#define FUSE_BLOCK_NUMBER(_X_)              BITS_FUSE_BLOCK_NUMBER(_X_)
#define HIGH_VOLT_ENABLE                    BIT_HIGH_VOLT_ENABLE
#define BURN_FUSE_ENABLE                    BIT_BURN_FUSE_ENABLE
#define FUSE_LOCK                           BIT_FUSE_LOCK
#define ENABLE_SOFT_FUSE_PROG               BIT_ENABLE_SOFT_FUSE_PROG
#define SEL_FUSE_B                       	BIT_SEL_FUSE_B

/* bits definitions for register REG_GEU_FUSE_STATUS */
#define FUSE_READY                          BIT_GE_FUSE_READY205
#define FUSE_BURN_DONE                      BIT_FUSE_BURN_DONE

#define ASR_ADDR(a,b)   ((a)+(b))
#define  REGS_DRO_CONTROLIS_BASE                                     0xD4013200
#define  REGS_APBC_BASE    0xD4015000

#define REG_APBC_APBC_DRO_CLK_RST                         ASR_ADDR(REGS_APBC_BASE, 0X58)/*Clock/Reset Control Register forDRO*/

/* registers definitions for DRO_CONTROLIS */
#define REG_DRO_CONTROLIS_DRO_CTRL                        ASR_ADDR(REGS_DRO_CONTROLIS_BASE, 0X00)/*DRO control register*/
#define REG_DRO_CONTROLIS_DRO_CNT_TIME                    ASR_ADDR(REGS_DRO_CONTROLIS_BASE, 0X4)/*DRO runtime length register*/
#define REG_DRO_CONTROLIS_DRO_LVT                         ASR_ADDR(REGS_DRO_CONTROLIS_BASE, 0X8)/*DRO LVT Value  Register*/
#define REG_DRO_CONTROLIS_DRO_HVT                         ASR_ADDR(REGS_DRO_CONTROLIS_BASE, 0XC)/*DRO HVT Value  Register*/
#define REG_DRO_CONTROLIS_DRO_SVT                         ASR_ADDR(REGS_DRO_CONTROLIS_BASE, 0X10)/*DRO SVT Value  Register*/

#define NUM_PROFILES_CRN          8
#define NUM_SVC_LVL_CRN           4

void set_cpu_dvc(CP_DVC_LEVEL cp_dvc_val);
void set_cpu_freq(CORE_PLL_FREQ_T cp_freq_val);
void cpu_freq_init(CORE_PLL_FREQ_T cp_freq_val);
void set_dsp_dvc(DSP_DVC_LEVEL dsp_dvc_val);
BOOL set_ddr_freq(DDR_FREQUENCY freq);
void HWDFC_disable(void);
void HWDFC_enable(void);
extern int fatal_printf(const char * fmt,...);

#ifdef EDNE_1928
void set_ddr_hwdfc(UINT8 cp_hwdfc_idx);
void _set_ddr_hwdfc(UINT8 cp_hwdfc_idx);
void _set_cpu_freq(CPU_PLL_T cpu_freq_val);
#endif

#ifdef NEZHA3_1826
void Nezha3_PP_change(UINT8 ProductPoint);
void Helan4_PP_change(UINT8 ProductPoint); //Reuse Nezha3-Z3 PP, but not touch PLL2
#endif

void CpCoreFreqChangeTo416(void);

#endif
