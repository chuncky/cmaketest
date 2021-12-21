/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/
#ifndef __GUI_CP_INTERNEL_HEADER_FILE__
#error "Please don't include this file directly, please include cp_include.h"
#endif

#ifndef __CP_BSP_API_H__
#define __CP_BSP_API_H__

#ifdef __cplusplus
extern "C" {
#endif

/*========================== CHIP ID definition ==================*/

typedef enum
{
	CHIP_ID_UNKNOWN   = 0x00,
    CHIP_ID_CRANE     = 0x6731,
    CHIP_ID_CRANE_Z1  = 0xF06731,
    CHIP_ID_CRANE_Z2  = 0xF16731,
    CHIP_ID_CRANE_A0  = 0xA06731,
}ChipIDType;

unsigned int GetChipID(void);
unsigned int GetLongChipID(void);
unsigned long GetPlatformType(void);

extern BOOL IsChipCrane(void);
extern BOOL IsChipCrane_A0(void);
extern BOOL IsChipCrane_A1(void);
extern BOOL IsChipCraneG(void);
extern BOOL IsChipCraneG_Z1(void);
extern BOOL IsChipCraneG_Z2(void);
extern BOOL IsChipCraneG_A0(void);
extern BOOL IsChipCraneG_A1(void);
extern BOOL IsChipCraneM(void);
extern BOOL IsChipCraneM_Z1(void);
extern BOOL IsChipCraneM_Z2(void);
extern BOOL IsChipCraneM_A0(void);
extern BOOL IsChipCraneM_A1(void);

extern BOOL IsChip_Crane_A0_or_Above(void);
extern BOOL IsChip_CraneG_A0_or_Above(void);
extern BOOL IsChip_CraneM_A0_or_Above(void);

#define LONG_CHIP_ID GetLongChipID()

#define CHIP_IS_CRANE_Z1	(GetLongChipID() == CHIP_ID_CRANE_Z1)
#define CHIP_IS_CRANE_Z2	(GetLongChipID() == CHIP_ID_CRANE_Z2)

#define CHIP_IS_CRANE		(IsChipCrane())
#define CHIP_IS_CRANE_A0	(IsChipCrane_A0())
#define CHIP_IS_CRANE_A1	(IsChipCrane_A1())

#define CHIP_IS_CRANEG		(IsChipCraneG())
#define CHIP_IS_CRANEG_Z1	(IsChipCraneG_Z1())
#define CHIP_IS_CRANEG_Z2	(IsChipCraneG_Z2())
#define CHIP_IS_CRANEG_A0	(IsChipCraneG_A0())
#define CHIP_IS_CRANEG_A1	(IsChipCraneG_A1())

#define CHIP_IS_CRANEM		(IsChipCraneM())
#define CHIP_IS_CRANEM_Z1	(IsChipCraneM_Z1())
#define CHIP_IS_CRANEM_Z2	(IsChipCraneM_Z2())
#define CHIP_IS_CRANEM_A0	(IsChipCraneM_A0())
#define CHIP_IS_CRANEM_A1	(IsChipCraneM_A1())

#define PLATFORM_IS_ASIC    (GetPlatformType() == 0x0)
#define PLATFORM_IS_FPGA    (GetPlatformType() == 0x1)
#define PLATFORM_IS_ZEBU    (GetPlatformType() == 0x2) /* include Z1 */


// USB client activation (with cable detection)
void bspUsbActivate(void);
// USB detect phase1 charge of PMIC interrupt binding. must be could after PMICphase1.
void bspUsbDetectPhase1( void );

#ifdef __cplusplus
}
#endif

#endif /* __CP_BSP_API_H__ */
