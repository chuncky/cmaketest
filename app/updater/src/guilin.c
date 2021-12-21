/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Guilin.c


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

#include "guilin.h"
#include "pmic.h"

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
/*      GuilinRead                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function Read Guilin by PI2C interface.                      */
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
int GuilinRead( Guilin_Reg_Type guilin_reg_type, unsigned char reg, unsigned char *value )
{
    int res = 0;

    switch( guilin_reg_type )
    {
        case GUILIN_BASE_Reg:
        {
            res = USTICAI2CReadDi_base(reg);
            GUILIN_UART_DEBUG( "[%s] SLAVE=[BASE], REG=[0x%.2x] , VAL=[0x%.2x]", __FUNCTION__,reg,res);
            break;
        }

        case GUILIN_POWER_Reg:
        {
            res = USTICAI2CReadDi_power(reg);
            GUILIN_UART_DEBUG( "[%s] SLAVE=[POWER], REG=[0x%.2x] , VAL=[0x%.2x]", __FUNCTION__,reg,res);
            break;
        }

        case GUILIN_GPADC_Reg:
        {
            res = USTICAI2CReadDi_GPADC(reg);
            GUILIN_UART_DEBUG( "[%s] SLAVE=[GPADC], REG=[0x%.2x] , VAL=[0x%.2x]", __FUNCTION__,reg,res);
            break;
        }

        default:
        {
            GUILIN_UART_DEBUG( "[%s] UNKNOW TARGET REG", __FUNCTION__);
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
/*      GuilinWrite                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function Write Guilin by PI2C interface.                     */
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
int GuilinWrite( Guilin_Reg_Type guilin_reg_type, unsigned char reg, unsigned char value )
{
    switch( guilin_reg_type )
    {
        case GUILIN_BASE_Reg:
        {
			USTICAI2CWriteDi_base(reg, value);
            GUILIN_UART_DEBUG( "[%s] SLAVE=[BASE ], REG=[0x%.2x] , VAL=[0x%.2x]", __FUNCTION__,reg,value);
            break;
        }
        case GUILIN_POWER_Reg:
        {
            USTICAI2CWriteDi_power(reg, value);
            GUILIN_UART_DEBUG( "[%s] SLAVE=[POWER] , REG=[0x%.2x] , VAL=[0x%.2x]", __FUNCTION__,reg,value);
            break;
        }
		case GUILIN_GPADC_Reg:
		{
            USTICAI2CWriteDi_GPADC(reg, value);
            GUILIN_UART_DEBUG( "[%s] SLAVE=[GPADC], REG=[0x%.2x] , VAL=[0x%.2x]", __FUNCTION__,reg,value);
			break;
		}
        default:
        {
            GUILIN_UART_DEBUG( "[%s] UNKNOW TARGET REG", __FUNCTION__);
            break;
        }
    }

    return 0;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      GuilinClkInit                                                    */
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
void GuilinClkInit( void )
{
    unsigned char var = 0;

	//switch clk to XO
    GuilinRead( GUILIN_BASE_Reg, GUILIN_RTC_CONTROL_REG, &var );//BP_0xF1[2]
	var |= GUILIN_RTC_USE_XO;
	GuilinWrite( GUILIN_BASE_Reg, GUILIN_RTC_CONTROL_REG, var );

    GuilinRead( GUILIN_BASE_Reg, GUILIN_RTC_CTRL_REG, &var );//BP_0xD0[7]
	var |= GUILIN_RTC_USE_XO_BIT;
	GuilinWrite( GUILIN_BASE_Reg, GUILIN_RTC_CTRL_REG, var );

	//set clock mux
	GuilinRead( GUILIN_BASE_Reg, GUILIN_CLK_32K_SEL_REG, &var );//BP,A=0xE4,Val=0x47
	var |= GUILIN_CLK_32K_SEL;
	GuilinWrite( GUILIN_BASE_Reg, GUILIN_CLK_32K_SEL_REG, var );

#if 1
	// NOTICE: for A0/A1 version , the CAP were set outside the chip
	//         if software config the CAP reg , the 32K clock would
	//         not start-up, So, the CAP should noly set after B0
	//
        GuilinRead( GUILIN_BASE_Reg, GUILIN_CRYSTAL_CAP_SET_REG, &var );
        var &= ~0xE0;
        var |= GUILIN_CRYSTAL_CAP_20PF;
        GuilinWrite( GUILIN_BASE_Reg, GUILIN_CRYSTAL_CAP_SET_REG, var );
#endif
}
void Guilin_VBUCK1_Set_FPWM( void )
{
    unsigned char var = 0;

	//set fpwm mode for buck1, power page, @0x25[3]=1
    GuilinRead( GUILIN_POWER_Reg, GUILIN_VBUCK1_FSM_REG4, &var );
	var |= 0x1<<3;
	GuilinWrite( GUILIN_POWER_Reg, GUILIN_VBUCK1_FSM_REG4, var );
}

void Guilin_VBUCK4_Set_FPWM( void )
{
    unsigned char var = 0;

	//set fpwm mode for buck4, power page, @0x55[3]=1
    GuilinRead( GUILIN_POWER_Reg, GUILIN_VBUCK4_FSM_REG4, &var );
	var |= 0x1<<3;
	GuilinWrite( GUILIN_POWER_Reg, GUILIN_VBUCK4_FSM_REG4, var );
}

//VBUCK FUNC
int Guilin_VBUCK_Set_DVC_Enable(unsigned char reg, unsigned char enable){
		unsigned char tmp;

		//keep the ENABLE_BIT[6:0] as previous
		if(GUILIN_CONTAIN_VBUCK_DVC_EN_BIT(reg))
		{
			GuilinRead( GUILIN_POWER_Reg, reg, &tmp );
			if(enable){
				tmp |= GUILIN_VBUCK_ENABLE_DVC_MASK;
			}else{
				tmp &= ~GUILIN_VBUCK_ENABLE_DVC_MASK;
			}
			return GuilinWrite( GUILIN_POWER_Reg, reg, tmp );
		}
		else
		{
			GUILIN_UART_DEBUG("[%s] ERROR REG=[0x%.2x]",__FUNCTION__,reg);
			return 1;
		}

}



//VBUCK FUNC
int Guilin_VBUCK_Set_Enable(unsigned char reg, unsigned char enable){
		unsigned char tmp;

		//keep the ENABLE_BIT[6:0] as previous
		if(GUILIN_CONTAIN_VBUCK_EN_BIT(reg))
		{
			GuilinRead( GUILIN_POWER_Reg, reg, &tmp );
			if(enable){
				tmp |= GUILIN_VBUCK_ENABLE_MASK;
			}else{
				tmp &= ~GUILIN_VBUCK_ENABLE_MASK;
			}
			return GuilinWrite( GUILIN_POWER_Reg, reg, tmp );
		}
		else
		{
			GUILIN_UART_DEBUG("[%s] ERROR REG=[0x%.2x]",__FUNCTION__,reg);
			return 1;
		}

}

int Guilin_VBUCK_Set_Slpmode(unsigned char reg, unsigned char mode){
		unsigned char tmp;
		//keep other expect SLP_BIT[4:3]
		if(GUILIN_CONTAIN_VBUCK_SLEEP_MODE_BIT(reg))
		{
			GuilinRead( GUILIN_POWER_Reg, reg, &tmp );
			tmp &= ~GUILIN_VBUCK_SLEEP_MODE_MASK;
			tmp |= (mode & GUILIN_VBUCK_SLEEP_MODE_MASK);
			return GuilinWrite( GUILIN_POWER_Reg, reg, tmp );
		}
		else
		{
			GUILIN_UART_DEBUG("[%s] ERROR REG=[0x%.2x]",__FUNCTION__,reg);
			return 1;
		}
}


int Guilin_VBUCK_Set_VOUT(unsigned char reg, unsigned char value){
		unsigned char tmp;

		// keep the ENABLE_BIT as previous
		if(GUILIN_CONTAIN_VBUCK_ACTIVE_VOUT_BIT(reg))
		{
			GuilinRead( GUILIN_POWER_Reg, reg, &tmp );
			tmp &= ~GUILIN_CONTAIN_VBUCK_ACTIVE_VOUT_MASK;
			tmp |= (value & GUILIN_CONTAIN_VBUCK_ACTIVE_VOUT_MASK);
		}
		//keep the DVC_ENABLE_BIT bit as previous
		else if(GUILIN_CONTAIN_VBUCK_SLEEP_VOUT_BIT(reg))
		{
			GuilinRead( GUILIN_POWER_Reg, reg, &tmp );
			tmp &= ~GUILIN_CONTAIN_VBUCK_SLEEP_VOUT_MASK;
			tmp |= (value & GUILIN_CONTAIN_VBUCK_SLEEP_VOUT_MASK);
		}
		else if(GUILIN_CONTAIN_VBUCK_DVC_VOUT_BIT(reg))
		{
			GuilinRead( GUILIN_POWER_Reg, reg, &tmp );
			tmp &= ~GUILIN_CONTAIN_VBUCK_DVC_VOUT_MASK;
			tmp |= (value & GUILIN_CONTAIN_VBUCK_DVC_VOUT_MASK);
		}else{
			GUILIN_UART_DEBUG("[%s] ERROR REG=[0x%.2x]",__FUNCTION__,reg);
			return 1;
		}
		return GuilinWrite( GUILIN_POWER_Reg, reg, tmp );
}

//LDO FUNC
int Guilin_LDO_Set_Enable(unsigned char reg, unsigned char enable){
		unsigned char tmp;

		//keep the ENABLE_BIT[5:0] as previous
		if(GUILIN_CONTAIN_LDO_EN_BIT(reg))
		{
			GuilinRead( GUILIN_POWER_Reg, reg, &tmp );
			if(enable){
				tmp |= GUILIN_LDO_ENABLE_MASK;
			}else{
				tmp &= ~GUILIN_LDO_ENABLE_MASK;
			}
			return GuilinWrite( GUILIN_POWER_Reg, reg, tmp );
		}
		else
		{
			GUILIN_UART_DEBUG("[%s] ERROR REG=[0x%.2x]",__FUNCTION__,reg);
			return 1;
		}
}

int Guilin_LDO_Set_Slpmode(unsigned char reg, unsigned char mode){
		unsigned char tmp;

		//keep the SLP_MODE[3:0]
		if(GUILIN_CONTAIN_LDO_SLEEP_MODE_BIT(reg))
		{
			GuilinRead( GUILIN_POWER_Reg, reg, &tmp );
			mode &= GUILIN_LDO_SLEEP_MODE_MASK;
			tmp &= ~GUILIN_LDO_SLEEP_MODE_MASK;
			tmp |= mode;
			return GuilinWrite( GUILIN_POWER_Reg, reg, tmp );
		}
		else
		{
			GUILIN_UART_DEBUG("[%s] ERROR REG=[0x%.2x]",__FUNCTION__,reg);
			return 1;
		}
}

int Guilin_LDO_Set_VOUT(unsigned char reg, unsigned char value){
		unsigned char tmp;

		if(GUILIN_CONTAIN_LDO_ACTIVE_VOUT_BIT(reg))
		{
			GuilinRead( GUILIN_POWER_Reg, reg, &tmp );
			tmp &= ~GUILIN_LDO_ACTIVE_VOUT_MASK;
			tmp |= (value & GUILIN_LDO_ACTIVE_VOUT_MASK);
		}
		else if(GUILIN_CONTAIN_LDO_SLEEP_VOUT_BIT(reg))
		{
			GuilinRead( GUILIN_POWER_Reg, reg, &tmp );
			tmp &= ~GUILIN_LDO_SLEEP_VOUT_MASK;
			tmp |= (value & GUILIN_LDO_SLEEP_VOUT_MASK);
		}else{
			GUILIN_UART_DEBUG("[%s] ERROR REG=[0x%.2x]",__FUNCTION__,reg);
			return 1;
		}
		return GuilinWrite( GUILIN_POWER_Reg, reg, tmp );
}

//ICAT EXPORTED FUNCTION - PMIC,GUILIN,SW_reset
int Guilin_SW_Reset(void){
	unsigned char tmp;

	//set discharge_time to 0
	GuilinRead( GUILIN_BASE_Reg,  GUILIN_RESET_DISCHARGE_REG , &tmp);
	tmp &= ~GUILIN_RESET_DISCHARGE_MASK;
	GuilinWrite( GUILIN_BASE_Reg, GUILIN_RESET_DISCHARGE_REG, tmp);

	//set fault_wu_en then set fault_wu
	GuilinRead(  GUILIN_BASE_Reg, GUILIN_FAULT_WU_REG, &tmp);
	GuilinWrite( GUILIN_BASE_Reg, GUILIN_FAULT_WU_REG, (tmp|GUILIN_FAULT_WU_ENABLE_BIT));
	GuilinRead(  GUILIN_BASE_Reg, GUILIN_FAULT_WU_REG, &tmp);
	GuilinWrite( GUILIN_BASE_Reg, GUILIN_FAULT_WU_REG, (tmp|GUILIN_FAULT_WU_BIT));

	//force a software powerdown
	GUILIN_UART_DEBUG( "PMIC Reset......");
	GuilinRead(  GUILIN_BASE_Reg, GUILIN_RESET_REG ,&tmp );
	GuilinWrite( GUILIN_BASE_Reg, GUILIN_RESET_REG ,(tmp | GUILIN_SW_REST_BIT));

    return 0;
}

//ICAT EXPORTED FUNCTION - PMIC,GUILIN,Dump_REG
void Guilin_Dump_PMIC_Register(void)
{
    unsigned char var,count;

    GUILIN_UART_DEBUG( "====================================GUILIN BASE PAGE============================");
	for(count=0x01; count<=0xf7; count++){GuilinRead( GUILIN_BASE_Reg, count, &var );GUILIN_UART_DEBUG( "[BASE	]ADDR=[0x%.2x] VAL=[0x%.2x]",count,var); }
    GUILIN_UART_DEBUG( "====================================GUILIN POWER PAGE============================");
	for(count=0x01; count<=0xf7; count++){GuilinRead( GUILIN_POWER_Reg, count, &var );GUILIN_UART_DEBUG( "[POWER	]ADDR=[0x%.2x] VAL=[0x%.2x]",count,var); }
    GUILIN_UART_DEBUG( "====================================GUILIN GPADC PAGE============================");
	for(count=0x01; count<=0xf7; count++){GuilinRead( GUILIN_GPADC_Reg, count, &var );GUILIN_UART_DEBUG( "[GPADC	]ADDR=[0x%.2x] VAL=[0x%.2x]",count,var); }

}

void Guilin_Aditional_Workaround(void)
{
/* [1] set pwr_hold bit
   PAGE	: BASE_PAGE
   ADDR	: 0x0D
   VAL	: BIT_7 = 1	*/

   unsigned char var;
   GuilinRead(GUILIN_BASE_Reg, GUILIN_PWR_HOLD_REG, &var);
   GuilinWrite(GUILIN_BASE_Reg, GUILIN_PWR_HOLD_REG, (var|GUILIN_PWR_HOLD_BIT));
}

void Guilin_VBUCK1_CFG(UINT8 value)
{
    if ((value < GUILIN_VBUCK_0V8000)||(value > GUILIN_VBUCK_1V2000))
    {
        uart_printf("Wrong buck1 value input!\r\n");
        return;
    }
    Guilin_VBUCK_Set_VOUT(GUILIN_VBUCK1_ACTIVE_VOUT_REG,value);
}

void Guilin_Ldo_1_set_2_8(void)
{
    Guilin_LDO_Set_VOUT(GUILIN_LDO1_ACTIVE_VOUT_REG, GUILIN_LDO1_ACTIVE_2V80);
}

void Guilin_Ldo_1_set(BOOL OnOff)
{
	Guilin_LDO_Set_Enable(GUILIN_LDO1_ENABLE_REG,OnOff);
}

void Guilin_Ldo_3_set_1_8(void)
{
	Guilin_LDO_Set_VOUT(GUILIN_LDO3_ACTIVE_VOUT_REG,GUILIN_LDO3_ACTIVE_1V80);
}

void Guilin_Ldo_3_set_3_0(void)
{
    Guilin_LDO_Set_VOUT(GUILIN_LDO3_ACTIVE_VOUT_REG, GUILIN_LDO3_ACTIVE_3V00);
}

void Guilin_Ldo_3_set(BOOL OnOff)
{
	Guilin_LDO_Set_Enable(GUILIN_LDO3_ENABLE_REG,OnOff);
}

void Guilin_Ldo_4_set_1_8(void)
{
	Guilin_LDO_Set_VOUT(GUILIN_LDO4_ACTIVE_VOUT_REG,GUILIN_LDO4_ACTIVE_1V80);
}

void Guilin_Ldo_4_set_3_0(void)
{
    Guilin_LDO_Set_VOUT(GUILIN_LDO4_ACTIVE_VOUT_REG, GUILIN_LDO4_ACTIVE_3V00);
}

void Guilin_Ldo_4_set(BOOL OnOff)
{
	Guilin_LDO_Set_Enable(GUILIN_LDO4_ENABLE_REG,OnOff);
}

void Guilin_Ldo_6_set_1_8(void)
{
	Guilin_LDO_Set_VOUT(GUILIN_LDO6_ACTIVE_VOUT_REG,GUILIN_LDO6_ACTIVE_1V80);
}

void Guilin_Ldo_6_set_2_8(void)
{
    Guilin_LDO_Set_VOUT(GUILIN_LDO6_ACTIVE_VOUT_REG, GUILIN_LDO6_ACTIVE_2V80);
}

void Guilin_Ldo_6_set(BOOL OnOff)
{
	Guilin_LDO_Set_Enable(GUILIN_LDO6_ENABLE_REG,OnOff);
}

