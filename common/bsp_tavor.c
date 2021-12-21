#include "common.h"
#include "bsp.h"

typedef enum
{
	CHIP_ID_UNKNOWN   = 0x00,
    CHIP_ID_CRANE     = 0x6731,
    CHIP_ID_CRANE_Z1  = 0xF06731,
    CHIP_ID_CRANE_Z2  = 0xF16731,
    CHIP_ID_CRANE_A0  = 0xA06731,
    CHIP_ID_CRANE_A1  = 0xA16731,
    CHIP_ID_CRANEG    = 0x3602,
    CHIP_ID_CRANEG_Z1 = 0x003602, 
    CHIP_ID_CRANEG_Z2 = 0x003602, 
    CHIP_ID_CRANEG_A0 = 0xA03603, 
    CHIP_ID_CRANEG_A1 = 0xA13603, /*to be confirmed*/
    CHIP_ID_CRANEG_AX = 0x3603,
    CHIP_ID_CRANEM    = 0x1603,   
    CHIP_ID_CRANEM_A0 = 0xA01603, 
    CHIP_ID_CRANEM_A1 = 0xA11603, /*to be confirmed*/

}ChipIDType;




static unsigned long ChipID = 0;
unsigned int GetChipID( void )
{
	if(ChipID == 0)
		ChipID = ((*(volatile UINT32*)0xD4282C00)&0xffff);
	return ChipID;

}

static unsigned long LongChipId = 0;
unsigned int GetLongChipID( void )
{
	if(LongChipId == 0)
		LongChipId = ((*(volatile UINT32*)0xD4282C00)&0xffffff);
	return LongChipId;
}

BOOL IsChipCrane(void)              {return (GetChipID()     == CHIP_ID_CRANE    );}
BOOL IsChipCrane_A0(void)           {return (GetLongChipID() == CHIP_ID_CRANE_A0 );}
BOOL IsChipCrane_A1(void)           {return (GetLongChipID() == CHIP_ID_CRANE_A1 );}
BOOL IsChipCraneG(void)             {return (( GetChipID() == CHIP_ID_CRANEG ) || ( GetChipID() == CHIP_ID_CRANEG_AX ));}
BOOL IsChipCraneG_Z1(void)          {return (GetLongChipID() == CHIP_ID_CRANEG_Z1);}
BOOL IsChipCraneG_Z2(void)          {return (GetLongChipID() == CHIP_ID_CRANEG_Z2);}
BOOL IsChipCraneG_A0(void)          {return (GetLongChipID() == CHIP_ID_CRANEG_A0);}
BOOL IsChipCraneG_A1(void)          {return (GetLongChipID() == CHIP_ID_CRANEG_A1);}
BOOL IsChipCraneG_AX(void)          {return (GetChipID() == CHIP_ID_CRANEG_AX);    }
BOOL IsChipCraneM(void)             {return (GetChipID()     == CHIP_ID_CRANEM   );}
BOOL IsChipCraneM_A0(void)          {return (GetLongChipID() == CHIP_ID_CRANEM_A0);}
BOOL IsChipCraneM_A1(void)          {return (GetLongChipID() == CHIP_ID_CRANEM_A1);}

BOOL IsChip_Crane_A0_or_Above(void)
{
    static BOOL retVal=0xff;

    if (retVal != 0xff)
        return retVal;
    switch (GetLongChipID())
    {
        case CHIP_ID_CRANE_Z1: 
        case CHIP_ID_CRANE_Z2: 
            retVal = FALSE;
            break;
        default: //This applies for Crane A0/A1 and later silicon.
            retVal = TRUE;
            break;
    }
    return retVal;
}

BOOL IsChip_CraneG_A0_or_Above(void)
{
    static BOOL retVal=0xff;

    if (retVal != 0xff)
        return retVal;
    switch (GetLongChipID())
    {
        /*case CHIP_ID_CRANEG_Z1: */
        case CHIP_ID_CRANEG_Z2: //same ID as Z1
            retVal = FALSE;
            break;
        default: //This applies for CraneG A0/A1 and later silicon.
            retVal = TRUE;
            break;
    }
    return retVal;
}


