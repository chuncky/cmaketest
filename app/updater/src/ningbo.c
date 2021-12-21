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

#define     NINGBO_RTC_USER_DATA_4_REG          0xF7 
#define     NINGBO2_RTC_USER_DATA_15_REG        0xCF            

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
extern void uudelay(int us);
extern void mdelay(int ms);


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
    unsigned char var = 0;

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

void Ningbo_Ldo_8_set_2_8(void)
{
	Ningbo_LDO_Set_VOUT(NINGBO_LDO8_ACTIVE_VOUT_REG,NINGBO_LDO8_ACTIVE_2V80);
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

void Ningbo_Ldo_11_set_1_8(void)
{
    Ningbo_LDO_Set_VOUT(NINGBO_LDO11_ACTIVE_VOUT_REG, NINGBO_LDO11_ACTIVE_1V80);
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
void pm813_userdata_set_for_automation_test(UINT8 flag)
{
    unsigned char var = 0;

	if(PMIC_IS_PM813())
    {    
        NingboRead( NINGBO_BASE_Reg, NINGBO_RTC_USER_DATA_4_REG, &var );
        var &= ~0xF;    
        var |= (flag&0xF); //only use low 4 bits
        NingboWrite( NINGBO_BASE_Reg, NINGBO_RTC_USER_DATA_4_REG, var );
    }
    else if(PMIC_IS_PM813S())
    {    
        NingboRead( NINGBO_BASE_Reg, NINGBO2_RTC_USER_DATA_15_REG, &var );
        var &= ~0xF;    
        var |= (flag&0xF); //only use low 4 bits
        NingboWrite( NINGBO_BASE_Reg, NINGBO2_RTC_USER_DATA_15_REG, var );
    }
}

void Ningbo_MEAS_ENABLE(NINGBO_MEAS_EN meas_en)
{
    UINT8 tmp;
    
	tmp = PMIC_READ_REG_GPADC(NINGBO_MEAS_TO_ENABLE_REG(meas_en));
	tmp |= NINGBO_MEAS_TO_ENABLE_BIT(meas_en);
	PMIC_WRITE_REG_GPADC(NINGBO_MEAS_TO_ENABLE_REG(meas_en),tmp);
}

UINT16 PM813_GPADC_READ_VOL_MEAS(UINT8 meaReg)
{
	UINT16 meas_val;
	UINT8 reg_value[2];

	/* Read two registers, the alignment will be done as follows:
	Register 1 - bits 7:0 => 8 MSB bits <11:4> of measurement value
	Register 2 - bits 3:0 => 4 LSB bits <3:0> of measurement value
	*/

	reg_value[0] = PMIC_READ_REG_GPADC(meaReg+0);
	reg_value[1] = PMIC_READ_REG_GPADC(meaReg+1);

	meas_val = (reg_value[0] << 4) | (reg_value[1] & 0x0F);

	return meas_val;
}


UINT32 Ningbo_Get_Meas_Factor(NINGBO_MEAS_EN meas_en)
{
    switch (meas_en)
    {
        case NINGBO_TINT_MEAS_EN:
        case NINGBO_BATID_MEAS_EN:
        case NINGBO_BATTEMP_MEAS_EN:
        case NINGBO_GPADC0_MEAS_EN:
        case NINGBO_GPADC1_MEAS_EN:
            return 1;
        case NINGBO_BUCK1_MEAS_EN:
        case NINGBO_BUCK2_MEAS_EN:
        case NINGBO_AVDD_MEAS_EN:
        case NINGBO_DVDD_MEAS_EN:
            return 2;
        case NINGBO_VCHG_MEAS_EN:
        case NINGBO_VBAT_MEAS_EN:
        case NINGBO_VPWR_MEAS_EN:
        case NINGBO_VSUP_MEAS_EN:
            return 5;
        default:
            return 3;
            /* includes: 
        	NINGBO_BUCK3_MEAS_EN,
        	NINGBO_LDO1_MEAS_EN,
        	NINGBO_LDO2_MEAS_EN,
        	NINGBO_LDO3_MEAS_EN,
        	NINGBO_LDO4_MEAS_EN,
        	NINGBO_LDO5_MEAS_EN,
        	NINGBO_LDO6_MEAS_EN,
        	NINGBO_LDO7_MEAS_EN,
        	NINGBO_LDO8_MEAS_EN,
        	NINGBO_LDO9_MEAS_EN,
        	NINGBO_LDO10_MEAS_EN,
        	NINGBO_LDO11_MEAS_EN,
        	NINGBO_LDO12_MEAS_EN,
        	NINGBO_LDO13_MEAS_EN,
        	NINGBO_VRTC_MEAS_EN,
        	*/
    }
}

void PM813_GPADC_NONSTOP_TRIG_DISABLE(void)
{
    UINT8 tmp;
	tmp = PMIC_READ_REG_GPADC(NINGBO_GPADC_MODE_CONTROL_REG);
	tmp &= ~(NINGBO_NON_STOP|NINGBO_GPADC_EN); 
	PMIC_WRITE_REG_GPADC(NINGBO_GPADC_MODE_CONTROL_REG,tmp);
}

void Ningbo_MEAS_DISABLE(NINGBO_MEAS_EN meas_en)
{
    UINT8 tmp;
    
	tmp = PMIC_READ_REG_GPADC(NINGBO_MEAS_TO_ENABLE_REG(meas_en));
	tmp &= ~(NINGBO_MEAS_TO_ENABLE_BIT(meas_en));
	PMIC_WRITE_REG_GPADC(NINGBO_MEAS_TO_ENABLE_REG(meas_en),tmp);
}



UINT32 PM813_BATTERY_GET_VBAT_MEAS_VOL_MV(void)
{
    UINT32 meas_val;

	//enable BAT MEAS
    Ningbo_MEAS_ENABLE(NINGBO_VBAT_MEAS_EN);

	/*trigger meas, use non-stop instead of single-trigger, 
	since the AVG value can be correct only after the first four times of read of single-trigger. */
	PM813_GPADC_NONSTOP_TRIG_ENABLE();

	//get VBAT meas 
    meas_val = PM813_GPADC_READ_VOL_MEAS(NINGBO_VINLDO_AVE_REG);
    //Voltage=hex2dec(CODE)/4096*1.3*128/129 (unit: V)(CODE is the adc output, 12bit)
    meas_val = VOLT_CONVERT_12BIT_MV(meas_val)*Ningbo_Get_Meas_Factor(NINGBO_VBAT_MEAS_EN); 

    //disable non-stop to save power
    PM813_GPADC_NONSTOP_TRIG_DISABLE();
    
	//disable BAT MEAS
//    Ningbo_MEAS_DISABLE(NINGBO_VBAT_MEAS_EN);//sync with 813S to workaround

	return meas_val;
}

void PM813_GPADC_NONSTOP_TRIG_ENABLE(void)
{
    UINT8 tmp;
	tmp = PMIC_READ_REG_GPADC(NINGBO_GPADC_MODE_CONTROL_REG);
	tmp |= (NINGBO_NON_STOP|NINGBO_GPADC_EN); 
	PMIC_WRITE_REG_GPADC(NINGBO_GPADC_MODE_CONTROL_REG,tmp);
}

BOOL PMIC_CHARGER_IS_DETECTED(void)
{
	if (PMIC_IS_PM812())
	    return ((PMIC_READ_REG_BASE(PM812_STATUS_1_REG) & PM812_EXTON_STATUS_BIT) ? TRUE : FALSE);
	else
        return ((PMIC_READ_REG_BASE(NINGBO_STATUS_REG1) & NINGBO_VBUS_STATUS_BIT) ? TRUE : FALSE);
}


UINT32 pm813_get_bat_vol(void)     
{
    UINT32  vol = PM813_BATTERY_GET_VBAT_MEAS_VOL_MV();

    if(PMIC_CHARGER_IS_DETECTED() && pm813_get_chargerError())
    {
        pm813_charger_restore();
    }
    return vol;
}

CHGFSM_OUT pm813_get_charger_FSM_state(void)
{
    unsigned char var;

//    mdelay(5);
    NingboRead(NINGBO_BASE_Reg, NINGBO_READONLY_DATA_4_REG, &var);
//    uart_printf("@BP_0x5B = 0x%x\r\n", var);
    return (var>>NINGBO_CHG_FSM_OUT_SHIFT);
}

BOOL pm813_get_chargerError(void)     
{       
    return (pm813_get_charger_FSM_state()<=2);
}

void pm813_charger_switch(BOOL OnOff) //1 -- on, 0 -- off
{
    unsigned char var;

    NingboRead(NINGBO_BASE_Reg, NINGBO_CHG_CTRL_MISC3_REG, &var);
    var &= ~0x0F;
    if (!OnOff)
        var |= 0x0E; //force termination
    NingboWrite(NINGBO_BASE_Reg, NINGBO_CHG_CTRL_MISC3_REG, var); 
}


void pm813_charger_restore(void)
{
    pm813_charger_switch(0); //charger off
    mdelay(1000);
    pm813_charger_switch(1); //charger on
}

void pm813_charger_force_check(BOOL OnOff) 
{
    unsigned char var;

    NingboRead(NINGBO_BASE_Reg, NINGBO_CHG_CTRL_MISC3_REG, &var);
    var &= ~0x0F;
    if (OnOff)
        var |= 0x09; //force check
    NingboWrite(NINGBO_BASE_Reg, NINGBO_CHG_CTRL_MISC3_REG, var); 
}


UINT32 pm813_get_battery_voltage_withoutCharger(void)     
{
    UINT32 batVol;

    pm813_charger_force_check(1); //force check
    batVol = pm813_get_bat_vol();
    pm813_charger_force_check(0); //back normal

    return batVol;
}
