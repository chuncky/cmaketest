//#include "predefines.h"
//#include "timer_dec.h"
#include "lcd_predefine.h"
#include "plat_config_defs.h"
#include "lcd_reg.h"
#ifdef USE_MEDIACLK_INTERFACE
#include "../../chip/clock/media-clk/media_clk.h"
#endif
#include <ui_os_api.h>
#include "lcd_drv.h"


#define PMU_BASE_ADDR 0xd4282800
#define MULTIMEDIA_CLK_RES_CTRL0 0x4C
#define MULTIMEDIA_CLK_RES_CTRL1 0x50

//#define PMU_CTRL0_VALUE 0x7027F
//#define PMU_CTRL0_VALUE 0x7003F
#define PMU_CTRL0_VALUE 0x7007F
//#define PMU_CTRL1_VALUE 0x2
#define PMU_CTRL1_VALUE 0x12
//#define PMU_CTRL1_VALUE 0xC1C1033f

/*used for assert mode only, will close camera*/
void lcd_ass_poweron(void)
{
	uint32_t reg0 = 0;
	uint32_t reg1 = 0;
	uint32_t reg2 = 0;
	int sclk = LCD_SCLK_FREQ;

	LCDLOGD("lcd_ass_poweron ++\r\n");
	/*power on multimedia power domain*/
	reg0 = BIT_17;
	LCD_BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg0);
	LCDLOGD("lcd_ass_poweron: power on sleep 1!\r\n");
	reg0 |= BIT_18;
	LCD_BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg0);
	LCDLOGD("lcd_ass_poweron: power on sleep 2!\r\n");
	uudelay(5);
	reg0 |= BIT_16;
	LCD_BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg0);
	LCDLOGD("lcd_ass_poweron: power on iolation!\r\n");
	uudelay(5);

	/*enable all clks*/
	reg1 = BIT_1; /*LCD_CI_ACLK reset*/
	reg0 |= BIT_2; /*LCD_HCLK reset*/
	reg0 |= BIT_1; /*LCD_SCLK reset*/
	reg0 |= BIT_0; /*LCD_AXICLK reset*/
	LCD_BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg0);
	LCD_BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL1, reg1);
	switch(sclk){
	case 312000:
		reg0 |= BIT_8;
		reg2 = 0x40000001;
		break;
	case 156000:
		reg0 |= BIT_8;
		reg2 = 0x40000002;
		break;
	case 125000:
		reg0 |= BIT_11;
		reg0 |= BIT_9;
		reg2 = 0x432D0001;
		break;
	case 120000:
		reg0 |= BIT_8;
		reg2 = 0x43B10002;
		break;	
	case 104000:
#if 0
		reg0 |= BIT_11;
		reg0 |= BIT_10;
		reg0 |= BIT_9;
		reg2 = 0x40000001;
#else  /*work around for A0 timing issue*/
		reg0 |= BIT_8;
		reg2 = 0x40000003;
#endif
		break;
	case 52000:
		reg0 |= BIT_6; /*LCD_SCLK src = 52M*/
		reg0 |= BIT_8;
		reg2 = 0x40000001;
		break;
	default:
		LCDLOGE("lcd_ass_poweron: unsupport sclk (%d)", sclk);
	}
	LCD_BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg0);
	reg1 |= BIT_4; /*LCD_CI_ACLK enable*/
	reg0 |= BIT_5; /*LCD_HCLK enable*/
	reg0 |= BIT_4; /*LCD_SCLK enable*/
	reg0 |= BIT_3; /*LCD_AXICLK enable*/
	LCD_BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg0);
	LCD_BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL1, reg1);
	LCD_BU_REG_WRITE(LCD_BASE_ADDR + SCLK_DIV, reg2);//enable sclk

	LCDLOGI("lcd_ass_poweron 0x%x = 0x%x\r\n", PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0,
		reg0);
	LCDLOGI("lcd_ass_poweron 0x%x = 0x%x\r\n", PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL1,
		reg1);
}


void lcd_ass_poweroff(void)
{
	uint32_t reg0 = 0;
	uint32_t reg1 = 0;
	LCDLOGD("lcd_ass_poweroff ++\r\n");
	reg0 = LCD_BU_REG_READ(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0);
	reg1 = LCD_BU_REG_READ(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL1);
	reg1 &= ~BIT_4; /*LCD_CI_ACLK disable*/
	reg0 &= ~BIT_5; /*LCD_HCLK disable*/
	reg0 &= ~BIT_4; /*LCD_SCLK disable*/
	reg0 &= ~BIT_3; /*LCD_AXICLK disable*/
	LCD_BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg0);
	LCD_BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL1, reg1);
	reg1 &= ~BIT_1; /*LCD_CI_ACLK reset*/
	reg0 &= BIT_2; /*LCD_HCLK reset*/
	reg0 &= BIT_1; /*LCD_SCLK reset*/
	reg0 &= BIT_0; /*LCD_AXICLK reset*/
	LCD_BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg0);
	LCD_BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL1, reg1);

	reg0 &= ~BIT_16;
	LCD_BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg0);
	LCDLOGD("lcd_ass_poweroff: power off iolation!\r\n");
	uudelay(5);
	reg0 &= ~BIT_18;
	LCD_BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg0);
	LCDLOGD("lcd_ass_poweroff: power off sleep 2!\r\n");
	reg0 &= ~BIT_17;
	LCD_BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg0);
	LCDLOGD("lcd_ass_poweroff: power off sleep 1!\r\n");

	LCDLOGI("lcd_ass_poweroff 0x%x = 0x%x\r\n", PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0,
		reg0);
	LCDLOGI("lcd_ass_poweroff 0x%x = 0x%x\r\n", PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL1,
		reg1);
}

void lcd_hw_reset(int32_t work_mode)
{
	int reg;
	uint32_t reg2 = 0;
	int sclk = LCD_SCLK_FREQ;
	reg = LCD_BU_REG_READ(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0);
	LCDLOGE("lcd_hw_reset: 0x%x read 0x%x\r\n", PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg);
	reg = reg & ~(BIT_0 | BIT_1 | BIT_2);
	LCDLOGE("lcd_hw_reset: 0x%x write 0x%x\r\n", PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg);
	LCD_BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg);
	if(work_mode == LCD_WORK_MODE_INTERRUPT)
		UOS_Sleep(MS_TO_TICKS(5));
	else
		mdelay(5);
	reg = reg | (BIT_0 | BIT_1 | BIT_2);
	LCDLOGE("lcd_hw_reset: 0x%x write 0x%x\r\n", PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg);
	LCD_BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg);

switch(sclk){
	case 312000:
		reg2 = 0x40000001;
		break;
	case 156000:
		reg2 = 0x40000002;
		break;
	case 125000:
		reg2 = 0x432D0001;
		break;
	case 120000:
		reg2 = 0x43B10002;
		break;
	case 104000:
#if 0
		reg2 = 0x40000001;
#else  /*work around for A0 timing issue*/
		reg2 = 0x40000003;
#endif
		break;
	case 52000:
		reg2 = 0x40000001;
		break;
	default:
		LCDLOGE("lcd_ass_poweron: unsupport sclk (%d)", sclk);
	}

	LCD_BU_REG_WRITE(LCD_BASE_ADDR + SCLK_DIV, reg2);//enable sclk	
}


#ifdef USE_MEDIACLK_INTERFACE
void lcd_poweron(void)
{
	LCDLOGD("lcd_poweron +++\r\n");
//	media_clk_dump(LEVEL_DUMP_CLOCK_TIMES);
	media_power_on(MODULE_DISPLAY);
	media_clk_enable(MEDIA_CLK_LCD_AHB | MEDIA_CLK_LCD_CI, MODULE_DISPLAY);
	media_clk_set_rate(MEDIA_CLK_LCD_SCLK,LCD_SCLK_FREQ);

	media_clk_enable(MEDIA_CLK_LCD_SCLK|MEDIA_CLK_LCD_AXI, MODULE_DISPLAY);
	media_clk_dump(LEVEL_DUMP_CLOCK_ONLY);
	LCDLOGD("lcd_poweron ---\r\n");
}

void lcd_poweroff(void)
{
	LCDLOGD("lcd_poweroff +++\r\n");
//	media_clk_dump(LEVEL_DUMP_CLOCK_TIMES);
	media_clk_disable(MEDIA_CLK_LCD_SCLK, MODULE_DISPLAY);
	media_clk_disable(MEDIA_CLK_LCD_AHB|MEDIA_CLK_LCD_CI|MEDIA_CLK_LCD_AXI, MODULE_DISPLAY);
	media_power_off(MODULE_DISPLAY);
//	media_clk_dump(LEVEL_DUMP_CLOCK_TIMES);
	LCDLOGD("lcd_poweroff ---\r\n");
}
#endif
