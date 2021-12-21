#ifndef _MEDIA_CLK_H_
#define _MEDIA_CLK_H_
#include "common.h"

#define MEDIA_CLK_OFFLINE        BIT(0)
#define MEDIA_CLK_ISP2X          BIT(1)
#define MEDIA_CLK_PHY            BIT(2)
#define MEDIA_CLK_INTERFACE      BIT(3)
#define MEDIA_CLK_OFFLINE_AXI    BIT(4)
#define MEDIA_CLK_LCD_CI         BIT(5)
#define MEDIA_CLK_LCD_AHB        BIT(6)
#define MEDIA_CLK_POWER          BIT(7)
#define MEDIA_CLK_LCD_SCLK		 BIT(8)
#define MEDIA_CLK_LCD_AXI		 BIT(9)
#define MEDIA_CLK_JPEG_FUNC		 BIT(10)

#ifndef NULL
#define NULL ((void*)0)
#endif

#define OS_WAIT_FORERVER (0xFFFFFFFFUL)
#define OS_WAIT_100MS (0xFFUL)

enum media_clk_module_id{
	MODULE_CAMERA = 0,
	MODULE_DISPLAY,
	MODULE_VIDEO,
	MODULE_MAX,
};

enum media_clk_dump_level{
	LEVEL_DUMP_CLOCK_ONLY = 0,
	LEVEL_DUMP_CLOCK_TIMES,
};

enum media_clk_sub_module_id{
	SUB_MODULE_OFFLINE = 0,
	SUB_MODULE_ISP2X,
	SUB_MODULE_PHY,
	SUB_MODULE_INTERFACE,
	SUB_MODULE_OFFLINE_AXI,
	SUB_MODULE_LCD_CI,
	SUB_MODULE_LCD_AHB,
	SUB_MODULE_POWER,  // multimedia power
	SUB_MODULE_LCD_SCLK,
	SUB_MODULE_LCD_AXI,
	SUB_MODULE_JPEG_FUNC,
	SUB_MODULE_MAX
};

int media_clk_set_rate(int clk_type, int clk_rate);
int media_clk_enable(int clk_type, int module_id);
int media_clk_disable(int clk_type, int module_id);
int media_clk_reset(uint32_t clk_type);
int media_power_on(int module_id);
int media_power_off(int module_id);
int media_clk_dump(enum media_clk_dump_level dump_level);
void media_clk_set_bit(unsigned int  reg, unsigned int  val);
void media_clk_clr_bit(unsigned int  reg, unsigned int  val);

#endif
