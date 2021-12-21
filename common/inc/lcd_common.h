#ifndef _LCD_COMMON_H_
#define _LCD_COMMON_H_

enum{
	LCD_INTER_FLAG_WBDONE = 0x1,
	LCD_INTER_FLAG_FRAMEDONE = 0x2,
	LCD_INTER_FLAG_SPIDONE = 0x4,
	LCD_INTER_FLAG_MCUDONE = 0x8,
	LCD_INTER_FLAG_LIMIT,
};

#define COMMAND_TIMEOUT 3

#define FRAME_TIMEOUT 20

#endif /*_LCD_COMMON_H_*/