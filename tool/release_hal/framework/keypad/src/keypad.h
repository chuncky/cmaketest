#ifndef _KEYPAD_H_
#define	_KEYPAD_H_
//#include "key_defs.h"

#define COL_TOTAL	7
#define ROW_TOTAL	7

#define	MANUAL		0
#define	AUTO		1

#define	SINGLE_KEY	0
#define	MULTI_KEY	1

typedef enum{
	DIRECT_KEY = 1,
	MATRIX_KEY,
	ROTARY_0,
	ROTARY_1
}Keypad_Type;


#define SCROLL_UP	(1)
#define SCROLL_DOWN	(-1)
#define SCROLL_INV	(0)

#define	KPAD_PRIORITY	15


#define KEYPAD_ROW_NUM		(0x5)			/* */
#define KEYPAD_COL_NUM		(0x5)			/* */

#define KEYPAD_ROW_MASK		(0x0F)			/* */
#define KEYPAD_COL_MASK		(0x0F)			/* */

#define KEDPAD_POLLING_TIMEOUT	100 		/* 100 * 20 ms = 2 seconds */
#define KEDPAD_DEBOUNCE_TIMERINTERVAL	20 		/* 20 ms */
#define KEDPAD_POLLING_COUNT	2 		/* poll twice to check valid key */
#define KEDPAD_VALID_COUNT	2 		/* 2 as valid */

typedef struct s_Keyrec {
	int kcnt;
	int keys[25];

} KeyRec;

void Keypad_ISR(void);

#endif
