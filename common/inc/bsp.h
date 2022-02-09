#ifndef _BSP_H_
#define _BSP_H_



typedef enum
{
	ASR_EVB,
	CUSTOM_NOKIA,
	CUSTOM_EVB3,
	TIGX_MIFI,
	CUSTOM_3,
	CUSTOM_4,
	CUSTOM_FF_P1,
	CUSTOM_1920,
	CUSTOM_7,
	CUSTOM_SAARN,
	CUSTOM_9,
	CUSTOM_10,
	CUSTOM_11,
	CUSTOM_12,
	CUSTOM_SIM2,
	BOARD_1,
	BOARD_2,
	BOARD_3,
	BOARD_4,
	BOARD_5,
	BOARD_6,
	BOARD_7,
	BOARD_8,
	BOARD_9,
	BOARD_10,
	BOARD_11,
	BOARD_12,

}BspCustomType;


/* The parameters for sysSetLocalInterrupt() use */
#define ENABLE_IRQ        0x7F  /*!< Enable I-bit of CP15  */
#define ENABLE_FIQ        0xBF  /*!< Enable F-bit of CP15  */
#define ENABLE_FIQ_IRQ    0x3F  /*!< Enable I-bit and F-bit of CP15  */
#define DISABLE_IRQ       0x80  /*!< Disable I-bit of CP15  */
#define DISABLE_FIQ       0x40  /*!< Disable F-bit of CP15  */
#define DISABLE_FIQ_IRQ   0xC0  /*!< Disable I-bit and F-bit of CP15  */

INT32 sysSetLocalInterrupt(INT32 nIntState);


typedef enum
{
	CLK_OFF =0,
	CLK_ON

}CLK_OnOff;


// The base address of the APB Communication Subsystem Clock Control Registers 
#define APB_CP_CLOCK_CTRL_BASE    0xD404B000//0xD403B000////peichange//
#define TCER                    (APB_CP_CLOCK_CTRL_BASE + 0x0014)   //TCU Clock Control Register
#define ACER                    (APB_CP_CLOCK_CTRL_BASE + 0x0018)   //AIRQ Clock Control Register
#define UCER                    (APB_CP_CLOCK_CTRL_BASE + 0x001C)   //UART Clock Control Register
#define USCER1                  (APB_CP_CLOCK_CTRL_BASE + 0x0020)   //USIM 1 Clock Control Register
#define USCER2                  (APB_CP_CLOCK_CTRL_BASE + 0x0024)   //USIM 2 Clock Control Register
#define ICER                    (APB_CP_CLOCK_CTRL_BASE + 0x0028)   //TWSI Clock Control Register
#define ECCER                   (APB_CP_CLOCK_CTRL_BASE + 0x002C)   //APB E-Cipher Clock Control Register
#define TICER                   (APB_CP_CLOCK_CTRL_BASE + 0x0030)   //APB Timers Clock Control Register
#define GCER                    (APB_CP_CLOCK_CTRL_BASE + 0x0034)   //GSSP Clock Control Register
#define AICER                   (APB_CP_CLOCK_CTRL_BASE + 0x0038)   //IPC/RIPC Clock Control Register
#define SCCR                    (APB_CP_CLOCK_CTRL_BASE + 0x003C)   //Slow Clock Clock Control Register






unsigned int GetChipID(void);
unsigned int GetLongChipID(void);


extern BOOL IsChipCrane(void);
extern BOOL IsChipCrane_A0(void);
extern BOOL IsChipCrane_A1(void);
extern BOOL IsChipCraneG(void);
extern BOOL IsChipCraneG_Z1(void);
extern BOOL IsChipCraneG_Z2(void);
extern BOOL IsChipCraneG_A0(void);
extern BOOL IsChipCraneG_A1(void);
extern BOOL IsChipCraneG_AX(void);
extern BOOL IsChipCraneM(void);
extern BOOL IsChipCraneM_Z1(void);
extern BOOL IsChipCraneM_Z2(void);
extern BOOL IsChipCraneM_A0(void);
extern BOOL IsChipCraneM_A1(void);


#define CHIP_IS_CRANE		(IsChipCrane())
#define CHIP_IS_CRANE_A0	(IsChipCrane_A0())
#define CHIP_IS_CRANE_A1	(IsChipCrane_A1())

#define CHIP_IS_CRANEG		(IsChipCraneG())
#define CHIP_IS_CRANEG_Z1	(IsChipCraneG_Z1())
#define CHIP_IS_CRANEG_Z2	(IsChipCraneG_Z2())
#define CHIP_IS_CRANEG_A0	(IsChipCraneG_A0())
#define CHIP_IS_CRANEG_A1	(IsChipCraneG_A1())
#define CHIP_IS_CRANEG_AX	(IsChipCraneG_AX())

#define CHIP_IS_CRANEM		(IsChipCraneM())
#define CHIP_IS_CRANEM_A0	(IsChipCraneM_A0())
#define CHIP_IS_CRANEM_A1	(IsChipCraneM_A1())
#endif	/* _BSP_H_ */

