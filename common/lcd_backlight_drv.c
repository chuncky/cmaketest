#include "backlight_drv.h"
//#include "cgpio.h"
#include "mfpr_api.h"

#define BACKLIGHT_GPIO 77
#define BACKLIGHT_GPIO_FWP 36

enum{
	BACKLIGHT_STATUS_ON,
	BACKLIGHT_STATUS_OFF,
	BACKLIGHT_STATUS_MAX
};

extern BOOL PMIC_IS_PM812(void);
extern BOOL PMIC_IS_PM813(void);
extern void NingboLcdBackLightCtrl(UINT8 level);

int g_backlight_status = BACKLIGHT_STATUS_OFF ;
int g_backlight_brightness = 7;

typedef struct 
{
	UINT32 reg;
	UINT32 padding[2];
}GPIO_Single_Register;

typedef volatile struct
{
	volatile GPIO_Single_Register PLR;//0x0
	volatile GPIO_Single_Register PDR;//0xc
	volatile GPIO_Single_Register PSR;//0X18
	volatile GPIO_Single_Register PCR;//0X24
	volatile GPIO_Single_Register RER;//0X30
	volatile GPIO_Single_Register FER;//0X3C
	volatile GPIO_Single_Register EDR;//0X48
	volatile GPIO_Single_Register SDR;//0X54
	volatile GPIO_Single_Register CDR;//0X60
	volatile GPIO_Single_Register SRER;//0X6C
	volatile GPIO_Single_Register CRER;//0X78
	volatile GPIO_Single_Register SFER;//0X84
	volatile GPIO_Single_Register CFER;//0X90
	volatile GPIO_Single_Register AP_MASK;//0X9C
	volatile GPIO_Single_Register CP_MASK;//0XA8
}GPIORegisters;

typedef enum
{
	VSPI_GPIO_0=0,
	VSPI_GPIO_32=32,
	VSPI_GPIO_33=33,
	VSPI_GPIO_34,
	VSPI_GPIO_35,
	VSPI_GPIO_36,
	VSPI_GPIO_64=64,
	VSPI_GPIO_96=96,
	VSPI_GPIO_121=121,
	VSPI_GPIO_MAX=127,
	VSPI_GPIO_NULL=128
}VSpiGpio;
#define GPIO_REGISTER_BASE				0xD4019000
#define GPIO_REGISTER_GROUPS				4

#define GPIO_GROUP_BASE1					0
#define GPIO_GROUP_BASE2					4
#define GPIO_GROUP_BASE3					8
#define GPIO_GROUP_BASE4					0x100

static UINT32 GPIORegisterBase[GPIO_REGISTER_GROUPS] = {	GPIO_GROUP_BASE1,
															GPIO_GROUP_BASE2,
															GPIO_GROUP_BASE3,
															GPIO_GROUP_BASE4};
#define GPIO_SHIFT(gpio) (1 << (gpio%32))


static UINT32 *GetBaseAddr(UINT32 portHandle)
{
	UINT32 base_addr;
	
	if(portHandle < VSPI_GPIO_32)
		base_addr =  GPIO_REGISTER_BASE + GPIORegisterBase[0];
	else if(portHandle < VSPI_GPIO_64)
		base_addr =  GPIO_REGISTER_BASE + GPIORegisterBase[1];
	else if(portHandle < VSPI_GPIO_96)
		base_addr =  GPIO_REGISTER_BASE + GPIORegisterBase[2];
	else if(portHandle <= VSPI_GPIO_MAX)
		base_addr =  GPIO_REGISTER_BASE + GPIORegisterBase[3];
	else
		base_addr =  0;

	return (UINT32 *)base_addr;
}

void gpio_set_output(unsigned int port)
{
	GPIORegisters *GPIOReg;
	GPIOReg = (GPIORegisters *)GetBaseAddr(port);
	GPIOReg->SDR.reg =	GPIO_SHIFT(port);
}
void gpio_set_value(unsigned int port, int value)
{
	//set high
	GPIORegisters *GPIOReg;
	GPIOReg = (GPIORegisters *)GetBaseAddr(port);
	if(value == 0) {
		GPIOReg->PCR.reg = GPIO_SHIFT(port);
	}
	else {
		GPIOReg->PSR.reg = GPIO_SHIFT(port);
	}
}
static void backlight_on(int on)
{
	//GPIOReturnCode ret;
    
	unsigned int value[2];
	
	if( PMIC_IS_PM802())
	{
		if(BACKLIGHT_STATUS_ON == on){
			//ret = GpioSetDirection(BACKLIGHT_GPIO, GPIO_OUT_PIN);
			//LCD_ASSERT(ret == GPIORC_OK);
			//ret = GpioSetLevel(BACKLIGHT_GPIO, GPIO_HIGH); /*backlight*/
			//LCD_ASSERT(ret == GPIORC_OK);*/
			value[0] = MFP_REG(GPIO_36| MFP_AF0 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW & ~MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE);
			value[1] = MFP_EOC;
			mfp_config(value);
			//set output
			gpio_set_output(BACKLIGHT_GPIO_FWP);
			gpio_set_value(BACKLIGHT_GPIO_FWP,1);

			
		} else {
			//ret = GpioSetLevel(BACKLIGHT_GPIO, GPIO_LOW); /*backlight*/
			//LCD_ASSERT(ret == GPIORC_OK);
			value[0] = MFP_REG(GPIO_36| MFP_AF0 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW & ~MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE);
			value[1] = MFP_EOC;
			mfp_config(value);
			//set output
			gpio_set_output(BACKLIGHT_GPIO_FWP);
			gpio_set_value(BACKLIGHT_GPIO_FWP,0);
		}
	}
	else if( PMIC_IS_PM812())
	{
		if(BACKLIGHT_STATUS_ON == on){
			//ret = GpioSetDirection(BACKLIGHT_GPIO, GPIO_OUT_PIN);
			//LCD_ASSERT(ret == GPIORC_OK);
			//ret = GpioSetLevel(BACKLIGHT_GPIO, GPIO_HIGH); /*backlight*/
			//LCD_ASSERT(ret == GPIORC_OK);*/
			value[0] = MFP_REG(GPIO_77| MFP_AF0 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW & ~MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE);
			value[1] = MFP_EOC;
			mfp_config(value);
			//set output
			gpio_set_output(BACKLIGHT_GPIO);
			gpio_set_value(BACKLIGHT_GPIO,1);

			
		} else {
			//ret = GpioSetLevel(BACKLIGHT_GPIO, GPIO_LOW); /*backlight*/
			//LCD_ASSERT(ret == GPIORC_OK);
			value[0] = MFP_REG(GPIO_77| MFP_AF0 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW & ~MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE);
			value[1] = MFP_EOC;
			mfp_config(value);
			//set output
			gpio_set_output(BACKLIGHT_GPIO);
			gpio_set_value(BACKLIGHT_GPIO,0);
		}
	}
}

void backlight_set_brightness(uint32_t brightness)
{
	LCDLOGD("backlight_set_brightness: %d\r\n", brightness);
	if(0 == brightness){
		if(BACKLIGHT_STATUS_OFF != g_backlight_status){
			backlight_on(BACKLIGHT_STATUS_OFF);

			g_backlight_status = BACKLIGHT_STATUS_OFF;
		}
	} else {
		if(BACKLIGHT_STATUS_OFF == g_backlight_status){
			//change vldo for brightness
			g_backlight_brightness = brightness;
			backlight_on(BACKLIGHT_STATUS_ON);
			g_backlight_status = BACKLIGHT_STATUS_ON;			
		} else {
			if(brightness != g_backlight_brightness){
				//change vldo for brightness
				g_backlight_brightness = brightness;
			}
		}
	}

	if(PMIC_IS_PM813()||PMIC_IS_PM813S())
	{
        NingboLcdBackLightCtrl(brightness);
	}
}
