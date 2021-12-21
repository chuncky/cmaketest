#include "lcd_reg.h"
#ifdef USE_MEDIACLK_INTERFACE
#include "media_clk.h"
#endif

/** 
************************************************************
* Note: Macro "CONFIG_BOARD_CRANE_EVB_Z2" to control GPIO config
************************************************************
**/
#define CONFIG_BOARD_CRANE_EVB_Z2 1

extern void PM812_Ldo_8_set_2_8(void);
extern void PM812_Ldo_8_set(int OnOff);
extern BOOL PMIC_IS_PM812(void);

extern void Ningbo_Ldo_1_set_2_8(void);
extern void Ningbo_Ldo_1_set(BOOL OnOff);

extern void timerDelay(UINT32 Usec) ;

#define PMU_BASE_ADDR 0xd4282800
#define MULTIMEDIA_CLK_RES_CTRL0 0x4C
#define MULTIMEDIA_CLK_RES_CTRL1 0x50

#define PMU_CTRL0_VALUE 0x7007F
#define PMU_CTRL1_VALUE 0x12

void lcd_env_prepare(void)
{
	LCDLOGD("lcd_env_prepare ++\r\n");
#ifndef TEST_MCU_PANEL /*spi panel*/
	(*(volatile unsigned long *)(0xd401e2DC)) = 0xd0c1;//GPIO77, LCD_BL

	(*(volatile unsigned long *)(0xd401e12c)) = 0xd0c2;//GPIO20, lcd_spi_clk,F2
	(*(volatile unsigned long *)(0xd401e134)) = 0xd0c2;//GPIO22, lcd_spi_cs0, F2
	(*(volatile unsigned long *)(0xd401e138)) = 0xd0c2;//gpio23, lcd_spi_din
	(*(volatile unsigned long *)(0xd401e13c)) = 0xd0c2;//GPIO24, lcd_spi_dout/SDA
	(*(volatile unsigned long *)(0xd401e140)) = 0xd0c2;//gpio25, VSYNC/TE
	(*(volatile unsigned long *)(0xd401e144)) = 0xd0c2;//GPIO26, SMPN_RSTB/RESET

#if (defined HW_PLATFORM_4_LINE_SUPPORT)	
	/* support 4-line*/
	(*(volatile unsigned long *)(0xd401e130)) = 0xd0c0;//GPIO21 spi_rs
#else
	/* support 3-line*/
	(*(volatile unsigned long *)(0xd401e130)) = 0xd0c2;//GPIO21??
	(*(volatile unsigned long *)(0xd401e148)) = 0xd0c2;//GPIO27, lcd_spi_dout_1 for 2 lane
#endif



#else /*mcu panel*/
#ifndef CONFIG_BOARD_CRANE_EVB_Z2                 // z1 board
	(*(volatile unsigned int *)(0xd401e12c)) = 0xd0c6; /*DB0*/
	(*(volatile unsigned int *)(0xd401e12c)) = 0xd0c6;
	(*(volatile unsigned int *)(0xd401e130)) = 0xd0c6; /*CS1*/
	(*(volatile unsigned int *)(0xd401e134)) = 0xd0c6; /*CS0*/
	(*(volatile unsigned int *)(0xd401e138)) = 0xd0c6; /*RDB*/
	(*(volatile unsigned int *)(0xd401e13c)) = 0xd0c6; /*WRB*/
	(*(volatile unsigned int *)(0xd401e144)) = 0xd0c6; /*RSTB*/

	(*(volatile unsigned int *)(0xd401e14c)) = 0xd0c5; /*A0*/

	(*(volatile unsigned int *)(0xd401e158)) = 0xd0c3; /*DB1*/
	(*(volatile unsigned int *)(0xd401e15c)) = 0xd0c3; /*DB2*/

	(*(volatile unsigned int *)(0xd401e1b0)) = 0xd0c7; /*DB3*/
	(*(volatile unsigned int *)(0xd401e1b4)) = 0xd0c7; /*DB4*/

	(*(volatile unsigned int *)(0xd401e140)) = 0xd0c2; /*TE*/

	(*(volatile unsigned int *)(0xd401e1e0)) = 0xd0c1; /*DB5*/
	(*(volatile unsigned int *)(0xd401e1e4)) = 0xd0c1; /*DB6*/
	(*(volatile unsigned int *)(0xd401e1e8)) = 0xd0c1; /*DB7*/
#else                                                                      // z2 board
	(*(volatile unsigned int *)(0xd401e12c)) = 0xd0c6; /*DB0*/
	(*(volatile unsigned int *)(0xd401e12c)) = 0xd0c6;
	(*(volatile unsigned int *)(0xd401e108)) = 0xd0c7; /*CS1*/
	(*(volatile unsigned int *)(0xd401e134)) = 0xd0c6; /*CS0*/
	(*(volatile unsigned int *)(0xd401e138)) = 0xd0c6; /*RDB*/
	(*(volatile unsigned int *)(0xd401e13c)) = 0xd0c6; /*WRB*/
	(*(volatile unsigned int *)(0xd401e144)) = 0xd0c6; /*RSTB*/

	(*(volatile unsigned int *)(0xd401e0c8)) = 0xd0c5; /*A0*/

	(*(volatile unsigned int *)(0xd401e158)) = 0xd0c3; /*DB1*/
	(*(volatile unsigned int *)(0xd401e15c)) = 0xd0c3; /*DB2*/

	(*(volatile unsigned int *)(0xd401e32c)) = 0xd0c3; /*DB3*/
	(*(volatile unsigned int *)(0xd401e0d0)) = 0xd0c3; /*DB4*/

	(*(volatile unsigned int *)(0xd401e140)) = 0xd0c6; /*TE*/

	(*(volatile unsigned int *)(0xd401e130)) = 0xd0c6; /*DB5*/
	(*(volatile unsigned int *)(0xd401e148)) = 0xd0c6; /*DB6*/
	(*(volatile unsigned int *)(0xd401e14c)) = 0xd0c6; /*DB7*/
#endif
#endif

    if (PMIC_IS_PM812())
    {
	    PM812_Ldo_8_set_2_8();
	    PM812_Ldo_8_set(1);
    }
    else
    {
        Ningbo_Ldo_1_set_2_8();
        Ningbo_Ldo_1_set(1);

		#ifdef WATCHLCDST7789_CODE_USE
		Ningbo_Ldo_8_set_2_8();
		Ningbo_Ldo_8_set(1);

		Ningbo_Ldo_11_set_1_8();
		Ningbo_Ldo_11_set(1);
		#endif
    }
	LCDLOGD("lcd_env_prepare --\r\n");	
}

/*used for assert mode only, will close camera*/
void lcd_ass_poweron(void)
{
	uint32_t reg0 = 0;
	uint32_t reg1 = 0;
	uint32_t reg2 = 0;
	int sclk = LCD_SCLK_FREQ;

	LCDLOGD("lcd_ass_poweron ++\r\n");
	//lcd_env_prepare();
	/*power on multimedia power domain*/
	reg0 = BIT_17;
	BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg0);
	LCDLOGD("lcd_ass_poweron: power on sleep 1!\r\n");
	reg0 |= BIT_18;
	BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg0);
	LCDLOGD("lcd_ass_poweron: power on sleep 2!\r\n");
	uudelay(5);
	reg0 |= BIT_16;
	BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg0);
	LCDLOGD("lcd_ass_poweron: power on iolation!\r\n");
	uudelay(5);

	/*enable all clks*/
	reg1 = BIT_1; /*LCD_CI_ACLK reset*/
	reg0 |= BIT_2; /*LCD_HCLK reset*/
	reg0 |= BIT_1; /*LCD_SCLK reset*/
	reg0 |= BIT_0; /*LCD_AXICLK reset*/
	BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg0);
	BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL1, reg1);
	switch(sclk){
	case 312000:
		reg0 |= BIT_8;
		reg2 = 0x40000001;
		break;
	case 125000:
		reg0 |= BIT_11;
		reg0 |= BIT_9;
		reg2 = 0x432D0001;
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
	BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg0);
	reg1 |= BIT_4; /*LCD_CI_ACLK enable*/
	reg0 |= BIT_5; /*LCD_HCLK enable*/
	reg0 |= BIT_4; /*LCD_SCLK enable*/
	reg0 |= BIT_3; /*LCD_AXICLK enable*/
	BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg0);
	BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL1, reg1);
	BU_REG_WRITE(LCD_BASE_ADDR + SCLK_DIV, reg2);//enable sclk

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
	reg0 = BU_REG_READ(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0);
	reg1 = BU_REG_READ(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL1);
	reg1 &= ~BIT_4; /*LCD_CI_ACLK disable*/
	reg0 &= ~BIT_5; /*LCD_HCLK disable*/
	reg0 &= ~BIT_4; /*LCD_SCLK disable*/
	reg0 &= ~BIT_3; /*LCD_AXICLK disable*/
	BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg0);
	BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL1, reg1);
	reg1 &= ~BIT_1; /*LCD_CI_ACLK reset*/
	reg0 &= BIT_2; /*LCD_HCLK reset*/
	reg0 &= BIT_1; /*LCD_SCLK reset*/
	reg0 &= BIT_0; /*LCD_AXICLK reset*/
	BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg0);
	BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL1, reg1);

	reg0 &= ~BIT_16;
	BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg0);
	LCDLOGD("lcd_ass_poweroff: power off iolation!\r\n");
	uudelay(5);
	reg0 &= ~BIT_18;
	BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg0);
	LCDLOGD("lcd_ass_poweroff: power off sleep 2!\r\n");
	reg0 &= ~BIT_17;
	BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg0);
	LCDLOGD("lcd_ass_poweroff: power off sleep 1!\r\n");

	LCDLOGI("lcd_ass_poweroff 0x%x = 0x%x\r\n", PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0,
		reg0);
	LCDLOGI("lcd_ass_poweroff 0x%x = 0x%x\r\n", PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL1,
		reg1);
}

void lcd_hw_reset(void)
{
	int reg;
	reg = BU_REG_READ(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0);
	LCDLOGE("lcd_hw_reset: 0x%x read 0x%x\r\n", PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg);
	reg = reg & ~(BIT_0 | BIT_1 | BIT_2);
	LCDLOGE("lcd_hw_reset: 0x%x write 0x%x\r\n", PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg);
	BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg);
    mdelay(5);
	//OSATaskSleep(MS_TO_TICKS(5));
	reg = reg | (BIT_0 | BIT_1 | BIT_2);
	LCDLOGE("lcd_hw_reset: 0x%x write 0x%x\r\n", PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg);
	BU_REG_WRITE(PMU_BASE_ADDR + MULTIMEDIA_CLK_RES_CTRL0, reg);
}

#ifdef USE_MEDIACLK_INTERFACE
void lcd_poweron(void)
{
	LCDLOGD("lcd_poweron +++\r\n");
	//lcd_env_prepare();
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
	LCDLOGD("lcd_poweroff ++\r\n");
//	media_clk_dump(LEVEL_DUMP_CLOCK_TIMES);
	media_clk_disable(MEDIA_CLK_LCD_AHB|MEDIA_CLK_LCD_CI|MEDIA_CLK_LCD_SCLK|MEDIA_CLK_LCD_AXI, MODULE_DISPLAY);
	media_power_off(MODULE_DISPLAY);
//	media_clk_dump(LEVEL_DUMP_CLOCK_TIMES);
	LCDLOGD("lcd_poweroff --\r\n");
}

#endif
