/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Ningbo.c


GENERAL DESCRIPTION

    This file is for ASR I2C package.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2017 by ASR, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who        what, where, why
--------   ------     ----------------------------------------------------------
03/07/2018   Qianying    Created module
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "pmic.h"
#include "ningbo.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================

            EXTERN DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

                          INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      NingboRead                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function Read Ningbo by PI2C interface.                      */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
int NingboRead( Ningbo_Reg_Type ningbo_reg_type, unsigned char reg, unsigned char *value )
{
    int res = 0;

    switch( ningbo_reg_type )
    {
        case NINGBO_BASE_Reg:
        {
            res = USTICAI2CReadDi_base(reg);
            NINGBO_UART_DEBUG( "[NINGBO_BASE_Reg] SLAVE=[BASE], REG=[0x%.2x] , VAL=[0x%.2x]\r\n", reg,res);
            break;
        }

        case NINGBO_POWER_Reg:
        {
            res = USTICAI2CReadDi_power(reg);
            NINGBO_UART_DEBUG( "[NINGBO_POWER_Reg] SLAVE=[POWER], REG=[0x%.2x] , VAL=[0x%.2x]\r\n", reg,res);
            break;
        }

        case NINGBO_GPADC_Reg:
        {
            res = USTICAI2CReadDi_GPADC(reg);
            NINGBO_UART_DEBUG( "[NINGBO_GPADC_Reg] SLAVE=[GPADC], REG=[0x%.2x] , VAL=[0x%.2x]\r\n", reg,res);
            break;
        }

        default:
        {
            NINGBO_UART_DEBUG( "Err: UNKNOWN TARGET REG");
            break;
        }
    }

    *value = res;

    return 0;
}


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      NingboWrite                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function Write Ningbo by PI2C interface.                     */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
int NingboWrite( Ningbo_Reg_Type ningbo_reg_type, unsigned char reg, unsigned char value )
{
    switch( ningbo_reg_type )
    {
        case NINGBO_BASE_Reg:
        {
			USTICAI2CWriteDi_base(reg, value);
            NINGBO_UART_DEBUG( "[NINGBO_BASE_Reg] SLAVE=[BASE ], REG=[0x%.2x] , VAL=[0x%.2x]\r\n", reg,value);
            break;
        }
        case NINGBO_POWER_Reg:
        {
            USTICAI2CWriteDi_power(reg, value);
            NINGBO_UART_DEBUG( "[NINGBO_POWER_Reg] SLAVE=[POWER] , REG=[0x%.2x] , VAL=[0x%.2x]\r\n", reg,value);
            break;
        }
		case NINGBO_GPADC_Reg:
		{
            USTICAI2CWriteDi_GPADC(reg, value);
            NINGBO_UART_DEBUG( "[NINGBO_GPADC_Reg] SLAVE=[GPADC], REG=[0x%.2x] , VAL=[0x%.2x]\r\n", reg,value);
			break;
		}
        default:
        {
            NINGBO_UART_DEBUG( "Err: UNKNOWN TARGET REG");
            break;
        }
    }

    return 0;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      NingboDisableWDT                                                 */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function Disable Watchdog of Ustia.                          */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      meas_val                            The 12bit ADC value          */
/*                                                                       */
/*************************************************************************/
//BIT_0 = 0	:	WD_ENABLE
//BIT_0 = 1	:	WD_DISABLE
void Ningbo_WDT_ENABLE( BOOL enable)
{
    unsigned char var = 0;
    //set fault_wu_en then set fault_wu
    Ningbo_FAULT_WU_EN_ENABLE();
    Ningbo_FAULT_WU_ENABLE();

    NingboRead( NINGBO_BASE_Reg, NINGBO_WD_REG, &var );
    enable?(var &= ~NINGBO_WD_DIS):(var |= NINGBO_WD_DIS);
    NingboWrite( NINGBO_BASE_Reg, NINGBO_WD_REG, var );
}

void NingboDisableWDT( void )
{
    Ningbo_WDT_ENABLE(FALSE);
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      NingboClkInit                                                    */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function initialize the Ustia clock.                         */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      meas_val                            The 12bit ADC value          */
/*                                                                       */
/*************************************************************************/
void NingboClkInit( void )
{
    unsigned char var = 0, var1 = 0;

	//switch clk to XO
    NingboRead( NINGBO_BASE_Reg, NINGBO_RTC_CONTROL_REG, &var );//BP_0xF1[2]
	var |= NINGBO_RTC_USE_XO;
	NingboWrite( NINGBO_BASE_Reg, NINGBO_RTC_CONTROL_REG, var );

    NingboRead( NINGBO_BASE_Reg, NINGBO_RTC_CTRL_REG, &var );//BP_0xD0[7]
	var |= NINGBO_RTC_USE_XO_BIT;
	NingboWrite( NINGBO_BASE_Reg, NINGBO_RTC_CTRL_REG, var );

	//set clock mux
	NingboRead( NINGBO_BASE_Reg, NINGBO_CLK_32K_SEL_REG, &var );//BP,A=0xE4,Val=0x47
	var |= NINGBO_CLK_32K_SEL;
	NingboWrite( NINGBO_BASE_Reg, NINGBO_CLK_32K_SEL_REG, var );

    NingboRead( NINGBO_BASE_Reg, NINGBO_CRYSTAL_CAP_SET_REG, &var );
    var &= ~0xE0;
    var |= NINGBO_CRYSTAL_CAP_20PF;
    NingboWrite( NINGBO_BASE_Reg, NINGBO_CRYSTAL_CAP_SET_REG, var );

    NingboRead( NINGBO_BASE_Reg, NINGBO_POWERDOWN_LOG_REG, &var );
    NingboRead( NINGBO_BASE_Reg, NINGBO_POWERDOWN_LOG_REG2, &var1 );
    CP_LOGI("LastPowerOff: 0x%x @0xE5, 0x%x @0xE6\r\n", var, var1);

    /* Set 15s for power-down-by-long-ONKEY-pressing period, this register is 
    restricted to change only one bit each time. */
    NingboWrite( NINGBO_BASE_Reg, NINGBO_RTC_MISC_3_REG, 0xc0 );
    NingboWrite( NINGBO_BASE_Reg, NINGBO_RTC_MISC_3_REG, 0xe0 );
    NingboWrite( NINGBO_BASE_Reg, NINGBO_RTC_MISC_3_REG, 0xf0 );

    /* Power down classD to avoid buzzing under some scenario. */
	NingboRead( NINGBO_BASE_Reg, 0x31, &var );//BP_0x31[6]
	var |= 0x1<<6;
	NingboWrite( NINGBO_BASE_Reg, 0x31, var );
    
}

void Ningbo_VBUCK1_Set_FPWM( void )
{
    unsigned char var = 0;

	//set fpwm mode for buck1, power page, @0x25[3]=1
    NingboRead( NINGBO_POWER_Reg, NINGBO_VBUCK1_FSM_REG4, &var );
	var |= 0x1<<3;
	NingboWrite( NINGBO_POWER_Reg, NINGBO_VBUCK1_FSM_REG4, var );

}

//VBUCK FUNC
int Ningbo_VBUCK_Set_DVC_Enable(unsigned char reg, unsigned char enable){
		unsigned char tmp;

		//keep the ENABLE_BIT[6:0] as previous
		if(NINGBO_CONTAIN_VBUCK_DVC_EN_BIT(reg))
		{
			NingboRead( NINGBO_POWER_Reg, reg, &tmp );
			if(enable){
				tmp |= NINGBO_VBUCK_ENABLE_DVC_MASK;
			}else{
				tmp &= ~NINGBO_VBUCK_ENABLE_DVC_MASK;
			}
			return NingboWrite( NINGBO_POWER_Reg, reg, tmp );
		}
		else
		{
			NINGBO_UART_DEBUG("[Ningbo_VBUCK_Set_DVC_Enable] ERROR REG=[0x%.2x]\r\n",reg);
			return 1;
		}

}



//VBUCK FUNC
int Ningbo_VBUCK_Set_Enable(unsigned char reg, unsigned char enable){
		unsigned char tmp;

		//keep the ENABLE_BIT[6:0] as previous
		if(NINGBO_CONTAIN_VBUCK_EN_BIT(reg))
		{
			NingboRead( NINGBO_POWER_Reg, reg, &tmp );
			if(enable){
				tmp |= NINGBO_VBUCK_ENABLE_MASK;
			}else{
				tmp &= ~NINGBO_VBUCK_ENABLE_MASK;
			}
			return NingboWrite( NINGBO_POWER_Reg, reg, tmp );
		}
		else
		{
			NINGBO_UART_DEBUG("[%s] ERROR REG=[0x%.2x]",__FUNCTION__,reg);
			return 1;
		}

}

int Ningbo_VBUCK_Set_Slpmode(unsigned char reg, unsigned char mode){
		unsigned char tmp;
		//keep other expect SLP_BIT[4:3]
		if(NINGBO_CONTAIN_VBUCK_SLEEP_MODE_BIT(reg))
		{
			NingboRead( NINGBO_POWER_Reg, reg, &tmp );
			tmp &= ~NINGBO_VBUCK_SLEEP_MODE_MASK;
			tmp |= (mode & NINGBO_VBUCK_SLEEP_MODE_MASK);
			return NingboWrite( NINGBO_POWER_Reg, reg, tmp );
		}
		else
		{
			NINGBO_UART_DEBUG("[%s] ERROR REG=[0x%.2x]",__FUNCTION__,reg);
			return 1;
		}
}


int Ningbo_VBUCK_Set_VOUT(unsigned char reg, unsigned char value){
		unsigned char tmp;

		// keep the ENABLE_BIT as previous
		if(NINGBO_CONTAIN_VBUCK_ACTIVE_VOUT_BIT(reg))
		{
			NingboRead( NINGBO_POWER_Reg, reg, &tmp );
			tmp &= ~NINGBO_CONTAIN_VBUCK_ACTIVE_VOUT_MASK;
			tmp |= (value & NINGBO_CONTAIN_VBUCK_ACTIVE_VOUT_MASK);
		}
		//keep the DVC_ENABLE_BIT bit as previous
		else if(NINGBO_CONTAIN_VBUCK_SLEEP_VOUT_BIT(reg))
		{
			NingboRead( NINGBO_POWER_Reg, reg, &tmp );
			tmp &= ~NINGBO_CONTAIN_VBUCK_SLEEP_VOUT_MASK;
			tmp |= (value & NINGBO_CONTAIN_VBUCK_SLEEP_VOUT_MASK);
		}
		else if(NINGBO_CONTAIN_VBUCK_DVC_VOUT_BIT(reg))
		{
			NingboRead( NINGBO_POWER_Reg, reg, &tmp );
			tmp &= ~NINGBO_CONTAIN_VBUCK_DVC_VOUT_MASK;
			tmp |= (value & NINGBO_CONTAIN_VBUCK_DVC_VOUT_MASK);
		}else{
			NINGBO_UART_DEBUG("[%s] ERROR REG=[0x%.2x]",__FUNCTION__,reg);
			return 1;
		}
		return NingboWrite( NINGBO_POWER_Reg, reg, tmp );
}

//LDO FUNC
int Ningbo_LDO_Set_Enable(unsigned char reg, unsigned char enable){
		unsigned char tmp;

		//keep the ENABLE_BIT[5:0] as previous
		if(NINGBO_CONTAIN_LDO_EN_BIT(reg))
		{
			NingboRead( NINGBO_POWER_Reg, reg, &tmp );
			if(enable){
				tmp |= NINGBO_LDO_ENABLE_MASK;
			}else{
				tmp &= ~NINGBO_LDO_ENABLE_MASK;
			}
			return NingboWrite( NINGBO_POWER_Reg, reg, tmp );
		}
		else
		{
			NINGBO_UART_DEBUG("[%s] ERROR REG=[0x%.2x]",__FUNCTION__,reg);
			return 1;
		}
}

int Ningbo_LDO_Set_Slpmode(unsigned char reg, unsigned char mode){
		unsigned char tmp;

		//keep the SLP_MODE[3:0]
		if(NINGBO_CONTAIN_LDO_SLEEP_MODE_BIT(reg))
		{
			NingboRead( NINGBO_POWER_Reg, reg, &tmp );
			mode &= NINGBO_LDO_SLEEP_MODE_MASK;
			tmp &= ~NINGBO_LDO_SLEEP_MODE_MASK;
			tmp |= mode;
			return NingboWrite( NINGBO_POWER_Reg, reg, tmp );
		}
		else
		{
			NINGBO_UART_DEBUG("[%s] ERROR REG=[0x%.2x]",__FUNCTION__,reg);
			return 1;
		}
}

int Ningbo_LDO_Set_VOUT(unsigned char reg, unsigned char value){
		unsigned char tmp;

		if(NINGBO_CONTAIN_LDO_ACTIVE_VOUT_BIT(reg))
		{
			NingboRead( NINGBO_POWER_Reg, reg, &tmp );
			tmp &= ~NINGBO_LDO_ACTIVE_VOUT_MASK;
			tmp |= (value & NINGBO_LDO_ACTIVE_VOUT_MASK);
		}
		else if(NINGBO_CONTAIN_LDO_SLEEP_VOUT_BIT(reg))
		{
			NingboRead( NINGBO_POWER_Reg, reg, &tmp );
			tmp &= ~NINGBO_LDO_SLEEP_VOUT_MASK;
			tmp |= (value & NINGBO_LDO_SLEEP_VOUT_MASK);
		}else{
			NINGBO_UART_DEBUG("[%s] ERROR REG=[0x%.2x]",__FUNCTION__,reg);
			return 1;
		}
		return NingboWrite( NINGBO_POWER_Reg, reg, tmp );
}

void Ningbo_FAULT_WU_EN_ENABLE(void)
{
	unsigned char tmp;
	//set fault_wu_en
	NingboRead(  NINGBO_BASE_Reg, NINGBO_FAULT_WU_REG, &tmp);
	NingboWrite( NINGBO_BASE_Reg, NINGBO_FAULT_WU_REG, (tmp|NINGBO_FAULT_WU_ENABLE_BIT));
}

void Ningbo_FAULT_WU_ENABLE(void)
{
	unsigned char tmp;
	//then set fault_wu
	NingboRead(  NINGBO_BASE_Reg, NINGBO_FAULT_WU_REG, &tmp);
	NingboWrite( NINGBO_BASE_Reg, NINGBO_FAULT_WU_REG, (tmp|NINGBO_FAULT_WU_BIT));
}

void Ningbo_WD_KICK(void)
{
	unsigned char tmp;
	NingboRead( NINGBO_BASE_Reg, NINGBO_RESET_REG, &tmp);
	NingboWrite( NINGBO_BASE_Reg, NINGBO_RESET_REG, (tmp|NINGBO_WD_REST_BIT));
}

//ICAT EXPORTED FUNCTION - PMIC,NINGBO,SW_reset
int Ningbo_SW_Reset(void){
	unsigned char tmp;

	//set discharge_time to 0
	NingboRead( NINGBO_BASE_Reg,  NINGBO_RESET_DISCHARGE_REG , &tmp);
	tmp &= ~NINGBO_RESET_DISCHARGE_MASK;
	NingboWrite( NINGBO_BASE_Reg, NINGBO_RESET_DISCHARGE_REG, tmp);

	//set fault_wu_en then set fault_wu
	Ningbo_FAULT_WU_EN_ENABLE();
	Ningbo_FAULT_WU_ENABLE();

	//force a software powerdown
	NINGBO_UART_DEBUG( "PMIC Reset......");
	NingboRead(  NINGBO_BASE_Reg, NINGBO_RESET_REG ,&tmp );
	NingboWrite( NINGBO_BASE_Reg, NINGBO_RESET_REG ,(tmp | NINGBO_SW_REST_BIT));

    return 0;
}

//ICAT EXPORTED FUNCTION - PMIC,NINGBO,Dump_REG
void Ningbo_Dump_PMIC_Register(void)
{
    unsigned char var,count;

    NINGBO_UART_DEBUG( "====================================NINGBO BASE PAGE============================\r\n");
	for(count=0x01; count<=0xf7; count++){NingboRead( NINGBO_BASE_Reg, count, &var );NINGBO_UART_DEBUG( "[BASE	]ADDR=[0x%.2x] VAL=[0x%.2x]\r\n",count,var); }
    NINGBO_UART_DEBUG( "====================================NINGBO POWER PAGE============================");
	for(count=0x01; count<=0xf7; count++){NingboRead( NINGBO_POWER_Reg, count, &var );NINGBO_UART_DEBUG( "[POWER	]ADDR=[0x%.2x] VAL=[0x%.2x]\r\n",count,var); }
    NINGBO_UART_DEBUG( "====================================NINGBO GPADC PAGE============================");
	for(count=0x01; count<=0xf7; count++){NingboRead( NINGBO_GPADC_Reg, count, &var );NINGBO_UART_DEBUG( "[GPADC	]ADDR=[0x%.2x] VAL=[0x%.2x]\r\n",count,var); }

}

void Ningbo_Aditional_Workaround(void)
{
    unsigned char var;
    // No SC and RC1 modify
    NingboRead( NINGBO_POWER_Reg, 0x24, &var );
    var |= 0x10;
    NingboWrite( NINGBO_POWER_Reg, 0x24, var );

/* [1] set pwr_hold bit
   PAGE	: BASE_PAGE
   ADDR	: 0x0D
   VAL	: BIT_7 = 1	*/

   NingboRead(NINGBO_BASE_Reg, NINGBO_PWR_HOLD_REG, &var);
   NingboWrite(NINGBO_BASE_Reg, NINGBO_PWR_HOLD_REG, (var|NINGBO_PWR_HOLD_BIT)); //This is only required for DVC enablement.
}

void Ningbo_VBUCK1_CFG(UINT8 value)
{
    if ((value < NINGBO_VBUCK_0V8000)||(value > NINGBO_VBUCK_1V2000))
    {
        uart_printf("Wrong buck1 value input!\r\n");
        return;
    }
    Ningbo_VBUCK_Set_VOUT(NINGBO_VBUCK1_ACTIVE_VOUT_REG,value);
}

void Ningbo_Ldo_9_set_1_8(void)
{
	Ningbo_LDO_Set_VOUT(NINGBO_LDO9_ACTIVE_VOUT_REG,NINGBO_LDO9_ACTIVE_1V80);
}

void Ningbo_Ldo_9_set_2_8(void)
{
    Ningbo_LDO_Set_VOUT(NINGBO_LDO9_ACTIVE_VOUT_REG, NINGBO_LDO9_ACTIVE_2V80);
}

void Ningbo_Ldo_9_set(BOOL OnOff)
{
	Ningbo_LDO_Set_Enable(NINGBO_LDO9_ENABLE_REG,OnOff);
}

void Ningbo_Ldo_4_set_1_8(void)
{
	Ningbo_LDO_Set_VOUT(NINGBO_LDO4_ACTIVE_VOUT_REG,NINGBO_LDO4_ACTIVE_1V80);
}

void Ningbo_Ldo_4_set_2_8(void)
{
    Ningbo_LDO_Set_VOUT(NINGBO_LDO4_ACTIVE_VOUT_REG, NINGBO_LDO4_ACTIVE_2V80);
}

void Ningbo_Ldo_4_set(BOOL OnOff)
{
	Ningbo_LDO_Set_Enable(NINGBO_LDO4_ENABLE_REG,OnOff);
}

void Ningbo_Ldo_3_set_1_8(void)
{
	Ningbo_LDO_Set_VOUT(NINGBO_LDO3_ACTIVE_VOUT_REG,NINGBO_LDO3_ACTIVE_1V80);
}

void Ningbo_Ldo_3_set_3_0(void)
{
    Ningbo_LDO_Set_VOUT(NINGBO_LDO3_ACTIVE_VOUT_REG, NINGBO_LDO3_ACTIVE_3V00);
}

void Ningbo_Ldo_3_set(BOOL OnOff)
{
	Ningbo_LDO_Set_Enable(NINGBO_LDO3_ENABLE_REG,OnOff);
}

void Ningbo_Ldo_8_set_1_8(void)
{
	Ningbo_LDO_Set_VOUT(NINGBO_LDO8_ACTIVE_VOUT_REG,NINGBO_LDO8_ACTIVE_1V80);
}

void Ningbo_Ldo_8_set_3_0(void)
{
    Ningbo_LDO_Set_VOUT(NINGBO_LDO8_ACTIVE_VOUT_REG, NINGBO_LDO8_ACTIVE_3V00);
}

void Ningbo_Ldo_8_set(BOOL OnOff)
{
	Ningbo_LDO_Set_Enable(NINGBO_LDO8_ENABLE_REG,OnOff);
}

void Ningbo_Ldo_1_set_2_8(void)
{
    Ningbo_LDO_Set_VOUT(NINGBO_LDO1_ACTIVE_VOUT_REG, NINGBO_LDO1_ACTIVE_2V80);
}

void Ningbo_Ldo_1_set(BOOL OnOff)
{
	Ningbo_LDO_Set_Enable(NINGBO_LDO1_ENABLE_REG,OnOff);
}

void Ningbo_Ldo_10_set_2_8(void)
{
    Ningbo_LDO_Set_VOUT(NINGBO_LDO10_ACTIVE_VOUT_REG, NINGBO_LDO10_ACTIVE_2V80);
}

void Ningbo_Ldo_10_set(BOOL OnOff)
{
	Ningbo_LDO_Set_Enable(NINGBO_LDO10_ENABLE_REG,OnOff);
}

void Ningbo_Ldo_11_set_2_8(void)
{
    Ningbo_LDO_Set_VOUT(NINGBO_LDO11_ACTIVE_VOUT_REG, NINGBO_LDO11_ACTIVE_2V80);
}

void Ningbo_Ldo_11_set(BOOL OnOff)
{
	Ningbo_LDO_Set_Enable(NINGBO_LDO11_ENABLE_REG,OnOff);
}

void Ningbo_Ldo_12_set_2_8(void)
{
    Ningbo_LDO_Set_VOUT(NINGBO_LDO12_ACTIVE_VOUT_REG, NINGBO_LDO12_ACTIVE_2V80);
}

void Ningbo_Ldo_12_set(BOOL OnOff)
{
	Ningbo_LDO_Set_Enable(NINGBO_LDO12_ENABLE_REG,OnOff);
}

void Ningbo_Ldo_13_set_1_8(void)
{
    Ningbo_LDO_Set_VOUT(NINGBO_LDO13_ACTIVE_VOUT_REG, NINGBO_LDO13_ACTIVE_1V80);
}

void Ningbo_Ldo_13_set(BOOL OnOff)
{
	Ningbo_LDO_Set_Enable(NINGBO_LDO13_ENABLE_REG,OnOff);
}

//porting from hal/I2C/src/ningbo.c
void NingboVibratorEnable(void)
{
    unsigned char var;

    NingboWrite(NINGBO_BASE_Reg, 0x43, 0x7f); 

    NingboRead(NINGBO_BASE_Reg, 0x4C, &var);
    NingboWrite(NINGBO_BASE_Reg, 0x4C, (var|(0x1<<3))); 
}
void NingboVibratorDisable(void)
{
    unsigned char var;

    NingboRead(NINGBO_BASE_Reg, 0x4C, &var);
    NingboWrite(NINGBO_BASE_Reg, 0x4C, (var&~(0x1<<3))); 
}

void NingboLcdBackLightCtrl(UINT8 level) //should be 0~5
{
    unsigned char var;
    
    if (level > 5) level=5;
    NingboRead(NINGBO_BASE_Reg, 0x4C, &var);
    if (level == 0) //switch off
    {
        NingboWrite(NINGBO_BASE_Reg, 0x4C, (var&~(0x1<<2))); 
        return;
    }
    NingboWrite(NINGBO_BASE_Reg, 0x42, 0x7f); 
    NingboWrite(NINGBO_BASE_Reg, 0x48, level * 6); /*31/5, register 0x48 has 5 bits*/
    if ((var&(0x1<<2))==0)
        NingboWrite(NINGBO_BASE_Reg, 0x4C, (var|(0x1<<2))); 

    return;
}

