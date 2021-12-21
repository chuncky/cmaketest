#ifndef __PWDT__
#define __PWDT__

typedef enum {
	PMIC_WDT_8s,
	PMIC_WDT_16s,
	PMIC_WDT_32s,
	PMIC_WDT_64s,
	PMIC_WDT_256s
}PMIC_WD_TIMEOUT;


void PMIC_WD_TIMER_SET(PMIC_WD_TIMEOUT timeout);
void PMIC_WD_KICK(void);
void PMIC_WD_ENABLE(BOOL enable);

#endif
