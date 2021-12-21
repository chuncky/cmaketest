#ifndef	_KPC_H_
#define	_KPC_H_
//#include "gbl_types.h"
#include "common.h"
//#include "utils.h"

#define MASK_(__n,__w)  (((1<<(__w))-1)<<(__n))

/*
 *
 *   THE BASE ADDRESSES
 *
 */
#define	KPC_BASE	0xD4012000

typedef struct s_KDCreg {
	UINT32 kpc_pc;
	UINT32 reserved0;
	UINT32 kpc_dk;
	UINT32 reserved1;
	UINT32 kpc_rec;
	UINT32 reserved2;
	UINT32 kpc_mk;
	UINT32 reserved3;
	UINT32 kpc_as;
	UINT32 reserved4;
	UINT32 kpc_asmkp0;
	UINT32 reserved5;
	UINT32 kpc_asmkp1;
	UINT32 reserved6;
	UINT32 kpc_asmkp2;
	UINT32 reserved7;
	UINT32 kpc_asmkp3;
	UINT32 reserved8;
	UINT32 kpc_kdi;
} KDCReg;

volatile KDCReg *kpc_reg = (KDCReg *)KPC_BASE;

/*
 *
 *   THE REGISTER DEFINES
 *
 */
#define	KPC_PC		(KPC_BASE+0x0000)	/* 32 bit	KPC Keypad Control Register */
#define	KPC_DK		(KPC_BASE+0x0008)	/* 32 bit	KPC Keypad Direct Key Register */
#define	KPC_REC		(KPC_BASE+0x0010)	/* 32 bit	KPC Keypad Rotary Encoder Count Register */
#define	KPC_MK		(KPC_BASE+0x0018)	/* 32 bit	KPC Keypad Matrix Key Register */
#define	KPC_AS		(KPC_BASE+0x0020)	/* 32 bit	KPC Keypad Automatic Scan Register */
#define	KPC_ASMKP0	(KPC_BASE+0x0028)	/* 32 bit	KPC Keypad Automatic Scan Multiple Keypress
										 * Register 0
										 */
#define	KPC_ASMKP1	(KPC_BASE+0x0030)	/* 32 bit	KPC Keypad Automatic Scan Multiple Keypress
										 * Register 1
										 */
#define	KPC_ASMKP2	(KPC_BASE+0x0038)	/* 32 bit	KPC Keypad Automatic Scan Multiple Keypress
										 * Register 2
										 */
#define	KPC_ASMKP3	(KPC_BASE+0x0040)	/* 32 bit	KPC Keypad Automatic Scan Multiple Keypress
										 * Register 3
										 */
#define	KPC_KDI		(KPC_BASE+0x0048)	/* 32 bit	KPC Keypad Key Debounce Interval Register */

/*
 *
 *   THE BIT DEFINES
 *
 */
/*	KPC_PC		0x0000	KPC Keypad Control Register */
#define	KPC_PC_AS					BIT_30			/* Automatic Scan */
#define	KPC_PC_ASACT				BIT_29			/* Automatic Scan on Activity */
#define	KPC_PC_MKRN_MSK				MASK_(26,3)		/* Matrix Keypad Row Number */
#define	KPC_PC_MKRN_BASE			26
#define	KPC_PC_MKCN_MSK				MASK_(23,3)		/* Keypad Column Number */
#define	KPC_PC_MKCN_BASE			23
#define	KPC_PC_MI					BIT_22			/* Matrix Keypad Interrupt */
#define	KPC_PC_IMKP					BIT_21			/* Ignore Multiple Keypress */
#define	KPC_PC_MS7					BIT_20			/* Manual Matrix Scan line 7 */
#define	KPC_PC_MS6					BIT_19			/* Manual Matrix Scan line 6 */
#define	KPC_PC_MS5					BIT_18			/* Manual Matrix Scan line 5 */
#define	KPC_PC_MS4					BIT_17			/* Manual Matrix Scan line 4 */
#define	KPC_PC_MS3					BIT_16			/* Manual Matrix Scan line 3 */
#define	KPC_PC_MS2					BIT_15			/* Manual Matrix Scan line 2 */
#define	KPC_PC_MS1					BIT_14			/* Manual Matrix Scan line 1 */
#define	KPC_PC_MS0					BIT_13			/* Manual Matrix Scan line 0 */
#define	KPC_PC_ME					BIT_12			/* Matrix Keypad Enable */
#define	KPC_PC_MIE					BIT_11			/* Matrix Interrupt Enable */
#define	KPC_PC_DK_DEB_SEL			BIT_9			/* Direct Keypad Debounce Select */
#define	KPC_PC_DN_MSK				MASK_(6,3)		/* Direct Key Number */
#define	KPC_PC_DN_BASE				6
#define	KPC_PC_DI					BIT_5			/* Direct Keypad Interrupt */
#define	KPC_PC_RE_ZERO_DEB			BIT_4			/* Rotary Encoder Zero Debounce Interval */
#define	KPC_PC_REE1					BIT_3			/* Rotary Encoder 1 Enable */
#define	KPC_PC_REE0					BIT_2			/* Rotary Encoder 0 Enable */
#define	KPC_PC_DE					BIT_1			/* Direct Keypad Enable */
#define	KPC_PC_DIE					BIT_0			/* Direct Keypad Interrupt Enable */

/*	KPC_DK		0x0008	KPC Keypad Direct Key Register */
#define	KPC_DK_DKP			BIT_31	/* Direct Key Pressed */
#define	KPC_DK_DK7			BIT_7	/* Direct Key 7 input */
#define	KPC_DK_DK6			BIT_6	/* Direct Key 6 input */
#define	KPC_DK_DK5			BIT_5	/* Direct Key 5 input */
#define	KPC_DK_DK4			BIT_4	/* Direct Key 4 input */
#define	KPC_DK_RB1_DK3		BIT_3	/* Rotary Encoder B / Direct Key 3 input */
#define	KPC_DK_RA1_DK2		BIT_2	/* Rotary Encoder A / Direct Key 2 input */
#define	KPC_DK_RB0_DK1		BIT_1	/* Rotary Encoder B / Direct Key 1 input */
#define	KPC_DK_RA0_DK0		BIT_0	/* Rotary Encoder A / Direct Key 0 input */

/*	KPC_REC		0x0010	KPC Keypad Rotary Encoder Count Register */
#define	KPC_REC_OF1					BIT_31			/* Overflow bit for rotary encoder 1 */
#define	KPC_REC_UF1					BIT_30			/* Underflow bit for rotary encoder 1 */
#define	KPC_REC_RE_COUNT1_MSK		MASK_(16,8)	/* Count value for rotary encoder 1 */
#define	KPC_REC_RE_COUNT1_BASE		16
#define	KPC_REC_OF0					BIT_15			/* Overflow bit for rotary encoder 0 */
#define	KPC_REC_UF0					BIT_14			/* Underflow bit for rotary encoder 0 */
#define	KPC_REC_RE_COUNT0_MSK		MASK_(0,8)	/* Count value for rotary encoder 0 */
#define	KPC_REC_RE_COUNT0_BASE		0

/*	KPC_MK		0x0018	KPC Keypad Matrix Key Register */
#define	KPC_MK_MKP		BIT_31	/* MKP */
#define	KPC_MK_MR7		BIT_7	/* Matrix Row 7 */
#define	KPC_MK_MR6		BIT_6	/* Matrix Row 6 */
#define	KPC_MK_MR5		BIT_5	/* Matrix Row 5 */
#define	KPC_MK_MR4		BIT_4	/* Matrix Row 4 */
#define	KPC_MK_MR3		BIT_3	/* Matrix Row 3 */
#define	KPC_MK_MR2		BIT_2	/* Matrix Row 2 */
#define	KPC_MK_MR1		BIT_1	/* Matrix Row 1 */
#define	KPC_MK_MR0		BIT_0	/* Matrix Row 0 */

/*	KPC_AS		0x0020	KPC Keypad Automatic Scan Register */
#define	KPC_AS_SO				BIT_31			/* Scan On */
#define	KPC_AS_MUKP_MSK			MASK_(26,5)	/* Multiple Keys Pressed */
#define	KPC_AS_MUKP_BASE		26
#define	KPC_AS_RP_MSK			MASK_(4,4)		/* Row Pressed */
#define	KPC_AS_RP_BASE			4
#define	KPC_AS_CP_MSK			MASK_(0,4)		/* Column Pressed */
#define	KPC_AS_CP_BASE			0

/*	KPC_ASMKP0	0x0028	KPC Keypad Automatic Scan Multiple Keypress Register 0 */
#define	KPC_ASMKP0_SO				BIT_31			/* Scan On */
#define	KPC_ASMKP0_MKC1_MSK			MASK_(16,8)	/* Matrix Keypad Column 1 reading */
#define	KPC_ASMKP0_MKC1_BASE		16
#define	KPC_ASMKP0_MKC0_MSK			MASK_(0,8)	/* Matrix Keypad Column 0 reading */
#define	KPC_ASMKP0_MKC0_BASE		0

/*	KPC_ASMKP1	0x0030	KPC Keypad Automatic Scan Multiple Keypress Register 1 */
#define	KPC_ASMKP1_SO				BIT_31			/* Scan On */
#define	KPC_ASMKP1_MKC3_MSK			MASK_(16,8)	/* Matrix Keypad Column 3 Reading */
#define	KPC_ASMKP1_MKC3_BASE		16
#define	KPC_ASMKP1_MKC2_MSK			MASK_(0,8)	/* Matrix Keypad Column 2 Reading */
#define	KPC_ASMKP1_MKC2_BASE		0

/*	KPC_ASMKP2	0x0038	KPC Keypad Automatic Scan Multiple Keypress Register 2 */
#define	KPC_ASMKP2_SO				BIT_31			/* Scan On */
#define	KPC_ASMKP2_MKC5_MSK			MASK_(16,8)	/* Matrix Keypad Column 5 reading */
#define	KPC_ASMKP2_MKC5_BASE		16
#define	KPC_ASMKP2_MKC4_MSK			MASK_(0,8)	/* Matrix Keypad Column 4 reading */
#define	KPC_ASMKP2_MKC4_BASE		0

/*	KPC_ASMKP3	0x0040	KPC Keypad Automatic Scan Multiple Keypress Register 3 */
#define	KPC_ASMKP3_SO				BIT_31			/* Scan On */
#define	KPC_ASMKP3_MKC7_MSK			MASK_(16,8)	/* Matrix Keypad Column 7 reading */
#define	KPC_ASMKP3_MKC7_BASE		16
#define	KPC_ASMKP3_MKC6_MSK			MASK_(0,8)	/* Matrix Keypad Column 6 reading */
#define	KPC_ASMKP3_MKC6_BASE		0

/*	KPC_KDI		0x0048	KPC Keypad Key Debounce Interval Register */
#define	KPC_KDI_DIRECT_KEY_DEBOUNCE_INTERVAL_MSK		MASK_(8,8)	/* Direct Key Debounce Interval */
#define	KPC_KDI_DIRECT_KEY_DEBOUNCE_INTERVAL_BASE		8
#define	KPC_KDI_MATRIX_KEY_DEBOUNCE_INTERVAL_MSK		MASK_(0,8)	/* Matrix Key Debounce Interval */
#define	KPC_KDI_MATRIX_KEY_DEBOUNCE_INTERVAL_BASE		0


typedef struct _COS_EVENT {
    UINT32 nEventId;
    UINT32 nParam1;
    UINT32 nParam2;
    UINT32 nParam3;
} COS_EVENT;

#define EV_DM_BASE                   (50)
#define EV_KEY_DOWN                              (EV_DM_BASE+40)
#define EV_KEY_PRESS                             (EV_DM_BASE+41)
#define EV_KEY_UP                                (EV_DM_BASE+42)

typedef enum keypad_kp_mkin{
	KP_MKI0 = 0,
	KP_MKI1,
	KP_MKI2,
	KP_MKI3,
	KP_MKI4,
	KP_MKI5,
	KP_MKI6,
	KP_MKI7,
	KP_MKI_NO_VALID
}KEYPAD_KP_MKIN_E;
typedef enum keypad_kp_mkout{
	KP_MKO0 = 0,
	KP_MKO1,
	KP_MKO2,
	KP_MKO3,
	KP_MKO4,
	KP_MKO5,
	KP_MKO6,
	KP_MKO7,
	KP_MKO_NO_VALID
}KEYPAD_KP_MKOUT_E;
#define KEYPAD_MATRIX_LENGTH 8


typedef INT32 (*ValEventFunc)(COS_EVENT);
void ValKeypadRegister(ValEventFunc EventFuncP);
void ValKeypadMultikeyEn(BOOL En);

INT32 ValKeypadInit(BOOL MultiKey, UINT8 KeyHoldDuration, KEYPAD_KP_MKOUT_E row[KEYPAD_MATRIX_LENGTH],
	                   KEYPAD_KP_MKIN_E col[KEYPAD_MATRIX_LENGTH]);



#endif	/* _KPC_H_ */
