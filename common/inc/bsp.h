#ifndef _BSP_H_
#define _BSP_H_

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

