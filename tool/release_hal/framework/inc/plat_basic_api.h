
#ifndef __PLAT_BASIC_API_H__
#define __PLAT_BASIC_API_H__

#include "plat_types.h"
#include "plat_assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/** import from cp beg*/

typedef  enum
{
    TCR_1,                              // non drowsy TCR
    TCR_2,                              // non drowsy TCR
    TCR_3,                              // non drowsy TCR

    TCR_4, MAX_HW_TCR_NUMBER = TCR_4, TCR_EXT = TCR_4,
    TCR_5,
    TCR_6,
    
    MAX_TCR_NUMBER

}TCR_NUMBER;
unsigned timerCountRead (TCR_NUMBER tcr_number);
#define TS_TIMER_ID TCR_2

// Returns the original CPSR
// exp: old_level = enableInterrupts();
// old_level MUST be local automatic variable !!
unsigned long disableInterrupts(void);

// Returns the original CPSR
// exp: old_level = enableInterrupts();
// old_level MUST be local automatic variable !!
void restoreInterrupts(unsigned long ir);

#define UI_INTERRUPT_SAVE_AREA		unsigned long cpsr
#define UI_DISABLE_IRQ_FIQ			cpsr = disableInterrupts()
#define UI_RESTORE_IRQ_FIQ			restoreInterrupts(cpsr)

#define UI_disableInterrupts		disableInterrupts
#define UI_restoreInterrupts		restoreInterrupts

/** import from cp end*/

void ui_delay_ms(unsigned int ms);
void ui_delay_us(unsigned int us);

void system_power_down(void);
void system_reboot(void);

/** HW platform internel definitions 
 ** e.g. 
 **     IS_HW_PLATFORM(BOARD_ID_CRANE_EVB_V100)   
 **     IS_HW_PLATFORM(BOARD_ID_CRANE_PHONE_V100) 
 **/
const char* bspGetBoardName(void);
int plat_get_cus_board(void);

#define IS_HW_PLATFORM(id)	((plat_get_cus_board()) == (unsigned int)(id))

#define BOARD_ID_CRANE_EVB_V100		0x00
#define BOARD_ID_CRANE_PHONE_V100	0x01

/*end*/

#ifdef __cplusplus
}
#endif

#endif
