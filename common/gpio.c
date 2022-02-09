#include "common.h"
#include "gpio.h"

/*bit definition for GPIO functions*/
#define GPIO_BIT_ENABLE_RED	(1 << 4)
#define GPIO_BIT_ENABLE_FED	(1 << 5)
#define GPIO_BIT_CLEAR_ED		(1 << 6)
#define GPIO_BIT_SLEEP_DIR	(1 << 7)

#define GPIO_BIT_DRIVE_0		(1 << 11)
#define GPIO_BIT_DRIVE_1		(1 << 12)
#define GPIO_BIT_PULLDN		(1 << 13)
#define GPIO_BIT_PULLUP		(1 << 14)
#define GPIO_BIT_PULL_SEL		(1 << 15)

typedef enum
{
    GPIO_NO_EDGE = 0,
    GPIO_RISE_EDGE,
    GPIO_FALL_EDGE,
    GPIO_TWO_EDGE,
}GPIOTransitionType;


typedef struct 
{
	UINT32 reg;
	UINT32 padding[2];
}GPIO_Single_Register;
typedef enum
{
    GPIO_IN_PIN = 0,
    GPIO_OUT_PIN = 1
}GPIOPinDirection;
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




/*
MFPR Fragments
*/
#define  GPIO_MFPR_BASE1 0xD401E0DC
#define  GPIO_MFPR_BASE2 0xD401E1A0
#define  GPIO_MFPR_BASE3 0xD401E304
#define  GPIO_MFPR_BASE4 0xD401E1B8
#define  GPIO_MFPR_BASE5 0xD401E1E0
#define  GPIO_MFPR_BASE6 0xD401E0C8
#define  GPIO_MFPR_BASE7 0xD401E2EC
#define  GPIO_MFPR_BASE8 0xD401E2E4
                                
#define  GPIO_MFPR_BASE9 0xD401E2BC
#define  GPIO_MFPR_BASE10 0xD401E32C
#define  GPIO_MFPR_BASE11 0xD401E094
                                
#define  GPIO_MFPR_BASE12 0xD401E2F0
#define  GPIO_MFPR_BASE13 0xD401E0B4
#define  GPIO_MFPR_BASE14 0xD401E2DC
                                
#define INVALID_GPIO_MFPR_ADDR	0xFFFFFFFF
#define CHIP_IS_CRANEM		(IsChipCraneM())




UINT32 GPIO_MFPR_ADDR(UINT32 n)  	
{
    return (CHIP_IS_CRANEM? \
                (  n <= 36  ?   GPIO_MFPR_BASE1 + (n << 2): \
                   ((n <= 43) &&(n >=37)) ?   GPIO_MFPR_BASE11 + ((n-37) << 2): \
                   ((n <= 54) &&(n >=49)) ?   GPIO_MFPR_BASE2 + ((n-49) << 2): \
                   ((n <= 66)&&(n >=60) )  ?   GPIO_MFPR_BASE3 + ((n-60) << 2): \
                   ((n <= 76)&&(n>=71))  ?   GPIO_MFPR_BASE4 + ((n-71)<<2): \
                   ((n <= 86)&&(n>=81) ) ?   GPIO_MFPR_BASE5 + ((n-81) << 2): \
                   ((n <= 126)&&(n >= 122))?   GPIO_MFPR_BASE6+((n-122) << 2) :     \
                   (n == 48 )?   GPIO_MFPR_BASE7 :      \
                   ((n <= 59)&&(n >=55) )  ?   GPIO_MFPR_BASE12 + ((n-55) << 2): \
                   ((n <= 120)&&(n >=117) )  ?   GPIO_MFPR_BASE13 + ((n-117) << 2): \
                   (n == 121 )?   GPIO_MFPR_BASE10 :        \
                   ((n <= 80)&&(n >=79)) ? GPIO_MFPR_BASE8 + ((n-79)<<2) : \
                   ((n <= 70)&&(n >=69)) ? GPIO_MFPR_BASE9 + ((n-69)<<2) : \
                   ((n <= 78)&&(n>=77))  ?   GPIO_MFPR_BASE14 + ((n-77)<<2): \
                   INVALID_GPIO_MFPR_ADDR): \
                (  n <= 36  ?   GPIO_MFPR_BASE1 + (n << 2): \
                   ((n <= 43) &&(n >=37)) ?   GPIO_MFPR_BASE11 + ((n-37) << 2): \
                   ((n <= 54) &&(n >=49)) ?   GPIO_MFPR_BASE2 + ((n-49) << 2): \
                   ((n <= 66)&&(n >=60) )  ?   GPIO_MFPR_BASE3 + ((n-60) << 2): \
                   ((n <= 78)&&(n>=71))  ?   GPIO_MFPR_BASE4 + ((n-71)<<2): \
                   ((n <= 86)&&(n>=81) ) ?   GPIO_MFPR_BASE5 + ((n-81) << 2): \
                   ((n <= 126)&&(n >= 122))?   GPIO_MFPR_BASE6+((n-122) << 2) :     \
                   (n == 48 )?   GPIO_MFPR_BASE7 :      \
                   ((n <= 59)&&(n >=55) )  ?   GPIO_MFPR_BASE12 + ((n-55) << 2): \
                   ((n <= 120)&&(n >=117) )  ?   GPIO_MFPR_BASE13 + ((n-117) << 2): \
                   (n == 121 )?   GPIO_MFPR_BASE10 :        \
                   ((n <= 80)&&(n >=79)) ? GPIO_MFPR_BASE8 + ((n-79)<<2) : \
                   ((n <= 70)&&(n >=69)) ? GPIO_MFPR_BASE9 + ((n-69)<<2) : \
                   INVALID_GPIO_MFPR_ADDR));
}                                                                   



static UINT32 GPIORegisterBase[GPIO_REGISTER_GROUPS] = {	GPIO_GROUP_BASE1,
															GPIO_GROUP_BASE2,
															GPIO_GROUP_BASE3,
															GPIO_GROUP_BASE4};
#define GPIO_SHIFT(gpio) (1 << (gpio%32))

static GPIO_Port_Handler GPIOPortHandler[GPIO_MAX_PORT_HANDLERS];


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

void cgpio_set_output(unsigned int port)
{
	GPIORegisters *GPIOReg;
	
	GPIOReg = (GPIORegisters *)GetBaseAddr(port);
	GPIOReg->SDR.reg =	GPIO_SHIFT(port);
}

void cgpio_set_value(unsigned int port, int value)
{
	GPIORegisters *GPIOReg;
	
	GPIOReg = (GPIORegisters *)GetBaseAddr(port);
	if(value == 0) {
		GPIOReg->PCR.reg = GPIO_SHIFT(port);
	}
	else {
		GPIOReg->PSR.reg = GPIO_SHIFT(port);
	}
}


/***********************************************************************
* Function: GpioBindInterruptCallback                          		   *
************************************************************************
* Description: Bind a callback function to a GPIO's interrupt	       *
*                                                                      *
* Parameters:	portHandle - the port handle                           *
*                                                                      *
* Return value: GPIORC_OK
				GPIO_NULL_POINTER 									   *
*				GPIORC_INVALID_PORT_HANDLE			                   *
* Notes:                                                               *
* 																	   *
**************************************************************************/

GPIOReturnCode GpioBindInterruptCallback(UINT32 portHandle, GPIOCallback func)
{

	GPIO_CHECK_VALID_HANDLE(portHandle);
	
	GPIO_CHECK_VALID_CALLBACK(func);

	GPIOPortHandler[portHandle].ISRhandler = func;

	return GPIORC_OK;
}

/***********************************************************************
* Function: GpioGetDirection                                           *
************************************************************************
* Description: Get the direction of a given port                  	   *
*                                                                      *
* Parameters:	portHandle - the port handle                           *
*                                                                      *
* Return value: GPIO_IN_PIN
				GPIO_OUT_PIN										   *
*				GPIORC_INVALID_PORT_HANDLE						       *
*
*                                                                      *
* Notes:                                                               *
* 																	   *
**************************************************************************/
GPIOPinDirection GpioGetDirection(UINT32 portHandle)
{
	GPIORegisters *GPIOReg;
	UINT32 bit;
	
//	GPIO_CHECK_VALID_HANDLE(portHandle);

	GPIOReg = (GPIORegisters *)GetBaseAddr(portHandle);
	
	bit = GPIOReg->PDR.reg & GPIO_SHIFT(portHandle);

	return bit? GPIO_OUT_PIN : GPIO_IN_PIN;
}


/***********************************************************************
* Function: GpioSetDirection                                           *
************************************************************************
* Description: Controls the direction of a given port                  *
*                                                                      *
* Parameters:	portHandle - the port handle                           *
*				dir - the required direction						   *
*                                                                      *
* Return value: GPIORC_OK		                                       *
*				GPIORC_INVALID_PORT_HANDLE						       *
*               GPIORC_INCORRECT_DIRECTION
				GPIORC_PIN_USED_IN_PORT							   *
*                                                                      *
* Notes:                                                               *
* 																	   *
*																	   *
* 																	   *
* If the port is with interrupt, it can not be changed to output with *
* this API																*
*************************************************************************/
GPIOReturnCode GpioSetDirection(UINT32 portHandle, GPIOPinDirection dir)
{
	GPIORegisters *GPIOReg;
	volatile UINT32 *mfpr_addr;
	UINT32 regval;
	
	
	GPIO_CHECK_VALID_HANDLE(portHandle);
	
	GPIO_CHECK_VALID_DIRECTION(dir);

	mfpr_addr = (volatile UINT32 *)GPIO_MFPR_ADDR(portHandle);
	if((UINT32)mfpr_addr == INVALID_GPIO_MFPR_ADDR)
	{
	    CP_LOGW("INVALID_GPIO_MFPR_ADDR: %d in func: %s\r\n",portHandle, __func__);
		return GPIORC_INVALID_PORT_HANDLE;
    }
#if 0 //remove this restriction, since this senario does exist.   
	if (dir == GPIO_OUT_PIN &&
		GPIOPortHandler[portHandle].IntcEnabled == TRUE)
	{
			return GPIORC_PIN_USED_IN_PORT;
	}
#endif
	GPIOReg = (GPIORegisters *)GetBaseAddr(portHandle);
	ASSERT(GPIOReg != 0);
	
	switch (dir) {
		case GPIO_OUT_PIN:

			GPIOReg->SDR.reg =	GPIO_SHIFT(portHandle);
			*(mfpr_addr) &= ~(GPIO_BIT_SLEEP_DIR);
			
			break;
		case GPIO_IN_PIN:

			GPIOReg->CDR.reg =	GPIO_SHIFT(portHandle);
			*(mfpr_addr) |= GPIO_BIT_SLEEP_DIR;
			
			break;
	}

	//medium drive slew rate
	*(mfpr_addr) |= GPIO_BIT_DRIVE_1;
	
	return GPIORC_OK;

}
/***********************************************************************
* Function: GpioSetLevel                                              *
************************************************************************
* Description: Writes a value to a port					               *
*                                                                      *
* Parameters:	portHandle - the port handle                           *
*				value - the value to be written						   *
*                                                                      *
* Return value: GPIORC_OK		                                       *
*				GPIORC_INVALID_PORT_HANDLE
*				GPIORC_INCORRECT_INIT_VALUE
*				GPIORC_WRITE_TO_INPUT								   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
GPIOReturnCode GpioSetLevel(UINT32 portHandle, UINT32 value)
{
	GPIORegisters *GPIOReg;
	
	GPIO_CHECK_VALID_HANDLE(portHandle);
	
	GPIO_CHECK_VALID_VALUE(value);

	if(GpioGetDirection(portHandle) != GPIO_OUT_PIN)
		return GPIORC_WRITE_TO_INPUT;

	GPIOReg = (GPIORegisters *)GetBaseAddr(portHandle);

	switch (value) {
		case 1:
			GPIOReg->PSR.reg = GPIO_SHIFT(portHandle);
			break;
		case 0:
			GPIOReg->PCR.reg = GPIO_SHIFT(portHandle);
			break;
		}
	
	return GPIORC_OK;


}

/***********************************************************************
* Function: GpioEnableEdgeDetection                                    *
************************************************************************
* Description: Enable edge detection function of a port		           *
*                                                                      *
* Parameters:	portHandle - the port handle                           *
*				edge - the edge detection type						   *
*                                                                      *
* Return value: GPIORC_OK
				GPIORC_INVALID_PORT_HANDLE
				GPIORC_INCORRECT_TRANSITION_TYPE					   *
*				GPIORC_PIN_NOT_FREE
*				
*			                                            			   *
* Notes:                                                               *
***********************************************************************/
GPIOReturnCode GpioEnableEdgeDetection(UINT32 portHandle, 
														GPIOTransitionType edge)
{
	GPIORegisters *GPIOReg;
	
	GPIO_CHECK_VALID_HANDLE(portHandle);
	GPIO_CHECK_VALID_TRANSITION(edge);

	GPIOReg = (GPIORegisters *)GetBaseAddr(portHandle);
	ASSERT(GPIOReg != 0);
	
	if(GpioGetDirection(portHandle) != GPIO_IN_PIN)
			return GPIORC_PIN_NOT_FREE;
	
	if(edge == GPIO_RISE_EDGE || edge == GPIO_TWO_EDGE)
	{
		GPIOReg->SRER.reg = GPIO_SHIFT(portHandle);
		GPIOPortHandler[portHandle].IntcEnabled = TRUE;
	}
	if(edge == GPIO_FALL_EDGE || edge == GPIO_TWO_EDGE)
	{
		GPIOReg->SFER.reg = GPIO_SHIFT(portHandle);		
		GPIOPortHandler[portHandle].IntcEnabled = TRUE;
	}
	GPIOReg->CP_MASK.reg |= GPIO_SHIFT(portHandle);
	
	return GPIORC_OK;
	
}
















/***********************************************************************
* Function: GpioDisableEdgeDetection                                   *
************************************************************************
* Description: Disable edge detection function of a port		       *
*                                                                      *
* Parameters:	portHandle - the port handle                           *
*				edge - the edge detection type						   *
*                                                                      *
* Return value: GPIORC_OK
				GPIORC_INVALID_PORT_HANDLE
				GPIORC_INCORRECT_TRANSITION_TYPE					   *
*				
*				
*			                                            			   *
* Notes:                                                               *
***********************************************************************/

GPIOReturnCode GpioDisableEdgeDetection(UINT32 portHandle, 
														GPIOTransitionType edge)
{
	GPIORegisters *GPIOReg;
	volatile UINT32 regVal;
	
	GPIO_CHECK_VALID_HANDLE(portHandle);

	GPIO_CHECK_VALID_TRANSITION(edge);

	GPIOReg = (GPIORegisters *)GetBaseAddr(portHandle);
	ASSERT(GPIOReg != 0);
	
	if(edge == GPIO_RISE_EDGE || edge == GPIO_TWO_EDGE)
	{
		GPIOReg->CRER.reg = GPIO_SHIFT(portHandle);
	}
	if(edge == GPIO_FALL_EDGE || edge == GPIO_TWO_EDGE)
	{
		GPIOReg->CFER.reg = GPIO_SHIFT(portHandle);
	}
	
	if((GPIOReg->RER.reg & GPIO_SHIFT(portHandle)==0) && (GPIOReg->FER.reg & GPIO_SHIFT(portHandle)==0))
	{
		regVal = GPIOReg->CP_MASK.reg;
		regVal &= ~(GPIO_SHIFT(portHandle));
		GPIOReg->CP_MASK.reg = regVal;
		GPIOPortHandler[portHandle].IntcEnabled= FALSE;
	}
	
	return GPIORC_OK;
}