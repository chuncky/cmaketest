#ifndef __PONKEY__
#define __PONKEY__

typedef void (*OnkeyIntCallback)(void);

typedef enum {
	ONKEY_ON,
	ONKEY_OFF,
}ONKEY_STATUS;

typedef enum{
	LONGKEY_1=PM812_LONGKEY_1,
	LONGKEY_2=PM812_LONGKEY_2,
}LONGKEY_COUNT;

void PM812_ONKEY_BIND_INTC(OnkeyIntCallback callback);
void PM812_ONKEY_INTC_ENABLE(BOOL onoff);
ONKEY_STATUS PM812_GET_ONKEY_STATUS(void);
void PM812_LONKEY_PRESS_TIME_SET(UINT8 sec);
void PM812_LONKEY_EN(LONGKEY_COUNT num,BOOL enable);
#endif
