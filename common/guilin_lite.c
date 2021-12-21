/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                GuilinLite.c


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

#include "guilin_lite.h"
#include "pmic.h"

/*===========================================================================

                          INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      GuilinLiteRead                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function Read GuilinLite by PI2C interface.                      */
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
int GuilinLiteRead( GuilinLite_Reg_Type guilin_lite_reg_type, unsigned char reg, unsigned char *value )
{
    int res = 0;

    switch( guilin_lite_reg_type )
    {
        case GUILIN_LITE_BASE_Reg:
        {
            res = USTICAI2CReadDi_base(reg);
            GUILIN_LITE_UART_DEBUG( "[%s] SLAVE=[BASE], REG=[0x%.2x] , VAL=[0x%.2x]", __FUNCTION__,reg,res);
            break;
        }

        case GUILIN_LITE_POWER_Reg:
        {
            res = USTICAI2CReadDi_power(reg);
            GUILIN_LITE_UART_DEBUG( "[%s] SLAVE=[POWER], REG=[0x%.2x] , VAL=[0x%.2x]", __FUNCTION__,reg,res);
            break;
        }

        default:
        {
            GUILIN_LITE_UART_DEBUG( "[%s] UNKNOW TARGET REG", __FUNCTION__);
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
/*      GuilinLiteWrite                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function Write GuilinLite by PI2C interface.                     */
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
int GuilinLiteWrite( GuilinLite_Reg_Type guilin_lite_reg_type, unsigned char reg, unsigned char value )
{
    switch( guilin_lite_reg_type )
    {
        case GUILIN_LITE_BASE_Reg:
        {
			USTICAI2CWriteDi_base(reg, value);
            GUILIN_LITE_UART_DEBUG( "[%s] SLAVE=[BASE ], REG=[0x%.2x] , VAL=[0x%.2x]", __FUNCTION__,reg,value);
            break;
        }
        case GUILIN_LITE_POWER_Reg:
        {
            USTICAI2CWriteDi_power(reg, value);
            GUILIN_LITE_UART_DEBUG( "[%s] SLAVE=[POWER] , REG=[0x%.2x] , VAL=[0x%.2x]", __FUNCTION__,reg,value);
            break;
        }
        default:
        {
            GUILIN_LITE_UART_DEBUG( "[%s] UNKNOW TARGET REG", __FUNCTION__);
            break;
        }
    }

    return 0;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      GuilinLiteClkInit                                                    */
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
void GuilinLiteClkInit( void )
{

}

//VBUCK FUNC
int GuilinLite_VBUCK_Set_DVC_Enable(unsigned char reg, unsigned char enable){
		unsigned char tmp;

		//keep the ENABLE_BIT[6:0] as previous
		if(GUILIN_LITE_CONTAIN_VBUCK_DVC_EN_BIT(reg))
		{
			GuilinLiteRead( GUILIN_LITE_POWER_Reg, reg, &tmp );
			if(enable){
				tmp |= GUILIN_LITE_VBUCK_ENABLE_DVC_MASK;
			}else{
				tmp &= ~GUILIN_LITE_VBUCK_ENABLE_DVC_MASK;
			}
			return GuilinLiteWrite( GUILIN_LITE_POWER_Reg, reg, tmp );
		}
		else
		{
			GUILIN_LITE_UART_DEBUG("[%s] ERROR REG=[0x%.2x]",__FUNCTION__,reg);
			return 1;
		}

}



//VBUCK FUNC
int GuilinLite_VBUCK_Set_Enable(unsigned char reg, unsigned char enable){
		unsigned char tmp;

		//keep the ENABLE_BIT[6:0] as previous
		if(GUILIN_LITE_CONTAIN_VBUCK_EN_BIT(reg))
		{
			GuilinLiteRead( GUILIN_LITE_POWER_Reg, reg, &tmp );
			if(enable){
				tmp |= GUILIN_LITE_VBUCK_ENABLE_MASK;
			}else{
				tmp &= ~GUILIN_LITE_VBUCK_ENABLE_MASK;
			}
			return GuilinLiteWrite( GUILIN_LITE_POWER_Reg, reg, tmp );
		}
		else
		{
			GUILIN_LITE_UART_DEBUG("[%s] ERROR REG=[0x%.2x]",__FUNCTION__,reg);
			return 1;
		}

}

int GuilinLite_VBUCK_Set_Slpmode(unsigned char reg, unsigned char mode){
		unsigned char tmp;
		//keep other expect SLP_BIT[4:3]
		if(GUILIN_LITE_CONTAIN_VBUCK_SLEEP_MODE_BIT(reg))
		{
			GuilinLiteRead( GUILIN_LITE_POWER_Reg, reg, &tmp );
			tmp &= ~GUILIN_LITE_VBUCK_SLEEP_MODE_MASK;
			tmp |= (mode & GUILIN_LITE_VBUCK_SLEEP_MODE_MASK);
			return GuilinLiteWrite( GUILIN_LITE_POWER_Reg, reg, tmp );
		}
		else
		{
			GUILIN_LITE_UART_DEBUG("[%s] ERROR REG=[0x%.2x]",__FUNCTION__,reg);
			return 1;
		}
}


int GuilinLite_VBUCK_Set_VOUT(unsigned char reg, unsigned char value){
		unsigned char tmp;

		// keep the ENABLE_BIT as previous
		if(GUILIN_LITE_CONTAIN_VBUCK_ACTIVE_VOUT_BIT(reg))
		{
			GuilinLiteRead( GUILIN_LITE_POWER_Reg, reg, &tmp );
			tmp &= ~GUILIN_LITE_CONTAIN_VBUCK_ACTIVE_VOUT_MASK;
			tmp |= (value & GUILIN_LITE_CONTAIN_VBUCK_ACTIVE_VOUT_MASK);
		}
		//keep the DVC_ENABLE_BIT bit as previous
		else if(GUILIN_LITE_CONTAIN_VBUCK_SLEEP_VOUT_BIT(reg))
		{
			GuilinLiteRead( GUILIN_LITE_POWER_Reg, reg, &tmp );
			tmp &= ~GUILIN_LITE_CONTAIN_VBUCK_SLEEP_VOUT_MASK;
			tmp |= (value & GUILIN_LITE_CONTAIN_VBUCK_SLEEP_VOUT_MASK);
		}
		else if(GUILIN_LITE_CONTAIN_VBUCK_DVC_VOUT_BIT(reg))
		{
			GuilinLiteRead( GUILIN_LITE_POWER_Reg, reg, &tmp );
			tmp &= ~GUILIN_LITE_CONTAIN_VBUCK_DVC_VOUT_MASK;
			tmp |= (value & GUILIN_LITE_CONTAIN_VBUCK_DVC_VOUT_MASK);
		}else{
			GUILIN_LITE_UART_DEBUG("[%s] ERROR REG=[0x%.2x]",__FUNCTION__,reg);
			return 1;
		}
		return GuilinLiteWrite( GUILIN_LITE_POWER_Reg, reg, tmp );
}

//LDO FUNC
int GuilinLite_LDO_Set_Enable(unsigned char reg, unsigned char enable){
		unsigned char tmp;

		//keep the ENABLE_BIT[5:0] as previous
		if(GUILIN_LITE_CONTAIN_LDO_EN_BIT(reg))
		{
			GuilinLiteRead( GUILIN_LITE_POWER_Reg, reg, &tmp );
			if(enable){
				tmp |= GUILIN_LITE_LDO_ENABLE_MASK;
			}else{
				tmp &= ~GUILIN_LITE_LDO_ENABLE_MASK;
			}
			return GuilinLiteWrite( GUILIN_LITE_POWER_Reg, reg, tmp );
		}
		else
		{
			GUILIN_LITE_UART_DEBUG("[%s] ERROR REG=[0x%.2x]",__FUNCTION__,reg);
			return 1;
		}
}

int GuilinLite_LDO_Set_Slpmode(unsigned char reg, unsigned char mode){
		unsigned char tmp;

		//keep the SLP_MODE[3:0]
		if(GUILIN_LITE_CONTAIN_LDO_SLEEP_MODE_BIT(reg))
		{
			GuilinLiteRead( GUILIN_LITE_POWER_Reg, reg, &tmp );
			mode &= GUILIN_LITE_LDO_SLEEP_MODE_MASK;
			tmp &= ~GUILIN_LITE_LDO_SLEEP_MODE_MASK;
			tmp |= mode;
			return GuilinLiteWrite( GUILIN_LITE_POWER_Reg, reg, tmp );
		}
		else
		{
			GUILIN_LITE_UART_DEBUG("[%s] ERROR REG=[0x%.2x]",__FUNCTION__,reg);
			return 1;
		}
}

int GuilinLite_LDO_Set_VOUT(unsigned char reg, unsigned char value){
		unsigned char tmp;

		if(GUILIN_LITE_CONTAIN_LDO_ACTIVE_VOUT_BIT(reg))
		{
			GuilinLiteRead( GUILIN_LITE_POWER_Reg, reg, &tmp );
			tmp &= ~GUILIN_LITE_LDO_ACTIVE_VOUT_MASK;
			tmp |= (value & GUILIN_LITE_LDO_ACTIVE_VOUT_MASK);
		}
		else if(GUILIN_LITE_CONTAIN_LDO_SLEEP_VOUT_BIT(reg))
		{
			GuilinLiteRead( GUILIN_LITE_POWER_Reg, reg, &tmp );
			tmp &= ~GUILIN_LITE_LDO_SLEEP_VOUT_MASK;
			tmp |= (value & GUILIN_LITE_LDO_SLEEP_VOUT_MASK);
		}else{
			GUILIN_LITE_UART_DEBUG("[%s] ERROR REG=[0x%.2x]",__FUNCTION__,reg);
			return 1;
		}
		return GuilinLiteWrite( GUILIN_LITE_POWER_Reg, reg, tmp );
}

//ICAT EXPORTED FUNCTION - PMIC,GUILIN_LITE,SW_reset
int GuilinLite_SW_Reset(void){
	unsigned char tmp;

	//set discharge_time to 0
	GuilinLiteRead( GUILIN_LITE_BASE_Reg,  GUILIN_LITE_RESET_DISCHARGE_REG , &tmp);
	tmp &= ~GUILIN_LITE_RESET_DISCHARGE_MASK;
	GuilinLiteWrite( GUILIN_LITE_BASE_Reg, GUILIN_LITE_RESET_DISCHARGE_REG, tmp);

	//set fault_wu_en then set fault_wu
	GuilinLiteRead(  GUILIN_LITE_BASE_Reg, GUILIN_LITE_FAULT_WU_REG, &tmp);
	GuilinLiteWrite( GUILIN_LITE_BASE_Reg, GUILIN_LITE_FAULT_WU_REG, (tmp|GUILIN_LITE_FAULT_WU_ENABLE_BIT));
	GuilinLiteRead(  GUILIN_LITE_BASE_Reg, GUILIN_LITE_FAULT_WU_REG, &tmp);
	GuilinLiteWrite( GUILIN_LITE_BASE_Reg, GUILIN_LITE_FAULT_WU_REG, (tmp|GUILIN_LITE_FAULT_WU_BIT));

	//force a software powerdown
	GUILIN_LITE_UART_DEBUG( "PMIC Reset......");
	GuilinLiteRead(  GUILIN_LITE_BASE_Reg, GUILIN_LITE_RESET_REG ,&tmp );
	GuilinLiteWrite( GUILIN_LITE_BASE_Reg, GUILIN_LITE_RESET_REG ,(tmp | GUILIN_LITE_SW_PDOWN_BIT));

    return 0;
}

void GuilinLite_Ldo_6_set_2_8(void)
{
    GuilinLite_LDO_Set_VOUT(GUILIN_LITE_LDO6_ACTIVE_VOUT_REG, GUILIN_LITE_LDO6_ACTIVE_2V80);
}

void GuilinLite_Ldo_6_set(BOOL OnOff)
{
	GuilinLite_LDO_Set_Enable(GUILIN_LITE_LDO6_ENABLE_REG,OnOff);
}

void GuilinLite_Ldo_3_set_1_8(void)
{
	GuilinLite_LDO_Set_VOUT(GUILIN_LITE_LDO3_ACTIVE_VOUT_REG,GUILIN_LITE_LDO3_ACTIVE_1V80);
}

void GuilinLite_Ldo_3_set_3_0(void)
{
    GuilinLite_LDO_Set_VOUT(GUILIN_LITE_LDO3_ACTIVE_VOUT_REG, GUILIN_LITE_LDO3_ACTIVE_3V00);
}

void GuilinLite_Ldo_3_set(BOOL OnOff)
{
	GuilinLite_LDO_Set_Enable(GUILIN_LITE_LDO3_ENABLE_REG,OnOff);
}

void GuilinLite_Ldo_8_set_1_8(void)
{
	GuilinLite_LDO_Set_VOUT(GUILIN_LITE_LDO8_ACTIVE_VOUT_REG,GUILIN_LITE_LDO8_ACTIVE_1V80);
}

void GuilinLite_Ldo_8_set_3_0(void)
{
    GuilinLite_LDO_Set_VOUT(GUILIN_LITE_LDO8_ACTIVE_VOUT_REG, GUILIN_LITE_LDO8_ACTIVE_3V00);
}

void GuilinLite_Ldo_8_set(BOOL OnOff)
{
	GuilinLite_LDO_Set_Enable(GUILIN_LITE_LDO8_ENABLE_REG,OnOff);
}

void GuilinLite_Ldo_7_set(BOOL OnOff)
{
	GuilinLite_LDO_Set_Enable(GUILIN_LITE_LDO7_ENABLE_REG,OnOff);
}

void GuilinLite_Ldo_7_set_2_8(void)
{
    GuilinLite_LDO_Set_VOUT(GUILIN_LITE_LDO7_ACTIVE_VOUT_REG, GUILIN_LITE_LDO7_ACTIVE_2V80);
}
void GuilinLite_Ldo_7_set_1_8(void)
{
    GuilinLite_LDO_Set_VOUT(GUILIN_LITE_LDO7_ACTIVE_VOUT_REG, GUILIN_LITE_LDO7_ACTIVE_1V80);
}



