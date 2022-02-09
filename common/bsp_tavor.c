#include "common.h"
#include "bsp.h"
#include "os_hal.h"
#include "mvUsbNet.h"
#include "rndis.h"
#include "teldef.h"
#include "os_hal.h"
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

/* ECM Transmit message */
#define ECM_TRANSMIT_MSG		                    0x4D434554

const char board_config[8] = "\2EVB_3";	/* current board configuration */
const char ac_link_config[8] = "\4NONE"; /* current AP/CP link type */

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

void CommPMEnableUsbWakeupSource(void)
{
    

}

void SulogOffControlForUsbRemove(void)
{

}


BspCustomType bspGetBoardType()
{
	char* ptr;

	ptr = (char*)&board_config[0];
	return (BspCustomType) *ptr;
}

volatile UINT32 diagTxFail =0;

void diag_output_to_usb_stop(UINT8 block)
{




}

void set_cp2ap_pm_gpio_high(void)
{

}

/************************************************************************************
*                       Gobal Variables
*************************************************************************************/
/* Network disconnected indication message */
UINT8 NetworkDisconnectIndication[8] =
{
    0xA1,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
};

/* Network Connected indication message */
UINT8 NetworkConnectIndication[8] =
{
    0xA1,
    0x00,
    0x01,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
};

/* Connection speed change message */
UINT8 ConnectionSpeedChange[16] =
{
    0xA1,
    0x2A,
    0x00,
    0x00,
    0x00,
    0x00,
    0x08,
    0x00,
    0x00,
    0x00,
    0x64,
    0x19,
    0x00,
    0x00,
    0x64,
    0x19
};

/*******************************************************************************
* Function    : Ecm_strtoascii
*
* Description :
*   Convert the string to ascii
*
******************************************************************************/
UINT8 Ecm_strtoascii(UINT8 str)
{
    if(str<=9)
        return(str+0x30);
    else if(str<=0xf)
        return (str+ 0x41 - 0xa);
    else
    {
        uart_printf("%s, input string is error!");
        ASSERT(0);
		return 0;
    }
}

/*******************************************************************************
* Function    : Ecm_getstrECMMACADDR
*
* Description :
*   Generate the string mac address used for usb protocal
*
* Parameter   :
*   Inputs    :
*       N/A.
*   Outputs   :
*       N/A.
* Returns     :
*       N/A.
*
* Notes:
*       N/A.
******************************************************************************/
void Ecm_getstrECMMACADDR(UINT8 * strMac, UINT32 len)
{
    UINT8 *mac = mvUsbNetGetMacAddress();
    UINT8 index, mac_index;
    uart_printf("Enter Ecm_getstrECMMACADDR");

    mac_index = 0;
    for(index = 2; (index < len)&&(mac_index < 6); )
    {
        strMac[index] = Ecm_strtoascii(mac[mac_index] >> 4);
        strMac[index+1] = 0x0;
        strMac[index+2] = Ecm_strtoascii(mac[mac_index]&0xf);
        strMac[index+3] = 0x0;

        index = index + 4;
        mac_index++;
    }
}

/*******************************************************************************
* Function  : EcmBitrate
*
* Descritption:
*   Report the bitrate of ECM.
*
* Parameter :
*   Inputs  : N/A.
*   Ouputs  : N/A.
*
* Returns   :
*   N/A.
*
* Notes:
*   N/A.
******************************************************************************/
UINT32 EcmBitrate(void)
{
    if( USB2_IS_FULL_SPEED())
    {
        return 19 *  64 * 1 * 1000 * 8;
    }
    else
    {
        return 13 * 512 * 8 * 1000 * 8;
    }
}




/*******************************************************************************
* Function    : Ecm_NetworkIndication
*
* Description :
*   ECM Network indication message
*
* Parameter   :
*   Inputs    :
*       N/A.
*   Outputs   :
*       N/A.
* Returns     :
*       N/A.
*
* Notes:
*       N/A.
******************************************************************************/
void Ecm_NetworkIndication(void)
{
    mvUsbNetType type = mvUsbGetNetType();

    if(type != USB_NET_ECM)
    {
        return;
    }

	mvUsbNetTransmitToUsb((USBDevice_EndpointE)USB_NET_CRTL_ENDPOINT,
	                NetworkDisconnectIndication, 8);

    mvUsbNetTransmitToUsb((USBDevice_EndpointE)USB_NET_CRTL_ENDPOINT,
                    NetworkConnectIndication, 8);

    mvUsbNetTransmitToUsb((USBDevice_EndpointE)USB_NET_CRTL_ENDPOINT,
                    ConnectionSpeedChange, 16);

    uart_printf("Ecm Network Ind");
}

BOOL SysIsAssert(void)
{
    return 0;
}

INT8 pc_netif_status(UINT8  status)
{
    return 0;
}

/***********************************************************************
* Function: ECM_remove_hdr
*
* Description:
*   There is no header in ECM.
*
* Parameters:
*   Inputs:
*       date : pointer to message
*   Outputs:
*	    lenth: number of bytes in message
*
* Return value:
*   Always it is 0.
*
* Notes:
*   N/A.
***********************************************************************/
BOOL Ecm_remove_hdr(UINT8 **data, UINT32 *msgLen, UINT32 *dataLen)
{
	return TRUE;
}

/***********************************************************************
* Function: ECM_add_hdr
*
* Description:
*   There is no header in ECM.
*
* Parameters:
*   Inputs:
*       date : pointer to message
*	    lenth: number of bytes in message
*
* Return value:
*   N/A.
*
* Notes:
*   N/A.
***********************************************************************/
void Ecm_add_hdr (UINT8 *data, UINT32 length)
{
    rndis_packet_msg_type *header = NULL;

    ASSERT(data != NULL);

    header = (rndis_packet_msg_type *)data;
    memset(header, 0x00, sizeof(rndis_packet_msg_type));

    header->MessageType   = ECM_TRANSMIT_MSG;
}

/*******************************************************************************
* Function    : Ecm_send_packet
*
* Description :
*    Perform the specific interface logic to tx a trace
*  on the external interface USB without not using DMA
*
* Parameter   :
*   Inputs    :
*       N/A.
*   Outputs   :
*       N/A.
* Returns     :
*       N/A.
*
* Notes:
*       N/A.
******************************************************************************/
void Ecm_send_packet(UINT8* data, UINT32 len)
{
    mvUsbNetParams *ecm_param_ptr = mvUsbGetNetParams();

    if((data != NULL)&&(len != 0))
    {
        /* Add Header information.      */
        if( ecm_param_ptr->state == USB_NET_UNINITED )
	    {
            ecm_free(data);
            return;
	    }

        Ecm_add_hdr(data, len);

        data += RNDIS_HEADER_LEN;
        len  -= RNDIS_HEADER_LEN;

        /* Transmit packet data to USB.*/
        mvUsbNetLwipSendPacket(data, len);
    }
    else
    {
        uart_printf("%s: data:%x, len:%x", __FUNCTION__, data, len);
    }
}




/*******************************************************************************
* Function    : ecm_free
*
* Description :
*   Free the buffer.
*
* Parameter   :
*   Inputs    :
*       buf => The address of buffer.
*   Outputs   :
*       N/A.
* Returns     :
*       N/A.
*
* Notes:
*       N/A.
******************************************************************************/
void ecm_free(void *buf)
{
    mac_pkt *ptr = NULL;
    pmsg *msg = NULL;
    rndis_packet_msg_type *header = NULL;

    header = (rndis_packet_msg_type *)((UINT8 *)(buf) - RNDIS_HEADER_LEN);

    if(header->MessageType == ECM_TRANSMIT_MSG)
    {
        msg = (pmsg*)((UINT8*)header - PMSG_SIZE);

        if(msg && msg->free)
        {
            msg->free(msg);
        }
    }
    else
    {
        header = (rndis_packet_msg_type *)buf;

        if((header->MessageType == REMOTE_NDIS_PACKET_MSG)&&
           (header->DataOffset == 36))
        {
            rndis_free( buf );
        }
        else
        {
            uart_printf("ecm_free unknown message!!!");
        }
    }
}





/*******************************************************************************
* Function    : EcmParamInit
*
* Description :
*   Inilization of EcmParam struct.
*
* Parameter   :
*   Inputs    :
*       N/A.
*   Outputs   :
*       N/A.
* Returns     :
*       N/A.
*
* Notes:
*       N/A.
******************************************************************************/
void EcmParamInit(void)
{
    mvUsbNetParams *ecm_param_ptr = mvUsbGetNetParams();
    uart_printf("+++hml+++ %s : %d",__func__, __LINE__);
    if(ecm_param_ptr->state != USB_NET_ECM_INIT)
    {
    	memset( (void *)ecm_param_ptr, 0, sizeof(mvUsbNetParams) );         /* Clear some structs.                      */
    	ecm_param_ptr->state         = USB_NET_ECM_INIT;
    	ecm_param_ptr->speed         = EcmBitrate( )/ 100;

        //mvUsbNetResetRingBuffer();
        uart_printf("Ecm Paras Init");
    }

    if(!mvUsbNetIfEnable())
    {
        Ecm_NetworkIndication();

        pc_netif_status(TRUE);

        uart_printf("PC Netif up");
    }
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Ecm_indicate_status_msg                                          */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function send message to PC to indicate the current status.  */
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
void Ecm_indicate_status_msg(UINT8 *buf)
{
    UINT32 MsgType;
    mvUsbNetType type = mvUsbGetNetType();
    mvUsbNetParams *ecm_param_ptr = mvUsbGetNetParams();

    ASSERT(buf != NULL);

	MsgType = buf[1] << 8 | buf[2] << 16 | buf[3] << 24;

    ASSERT(MsgType == USBNET_IND_ID);

    if(type != USB_NET_ECM)
    {
        uart_printf("%s: wrong net type", __FUNCTION__, type);
        return;
    }

	if (ecm_param_ptr->state == USB_NET_UNINITED)
	{
	    uart_printf("%s: ECM is not initialized", __FUNCTION__);
		return;
	}

    switch(buf[0])
    {
        case 0:
        {
			 //Disconnect
            ecm_param_ptr->media_state = NDIS_MEDIA_STATE_DISCONNECTED;

            mvUsbNetTransmitToUsb((USBDevice_EndpointE)USB_NET_CRTL_ENDPOINT,
	                NetworkDisconnectIndication, 8);

            break;
        }

        case 1:
        {
			//Connect
            ecm_param_ptr->media_state = NDIS_MEDIA_STATE_CONNECTED;

            mvUsbNetTransmitToUsb((USBDevice_EndpointE)USB_NET_CRTL_ENDPOINT,
                    NetworkConnectIndication, 8);


            break;
        }

        default:
        {
            ASSERT(0);
            break;
        }
    }

	mvUsbNetTransmitToUsb((USBDevice_EndpointE)USB_NET_CRTL_ENDPOINT,
                    ConnectionSpeedChange, 16);
}












/* Timer0_0 is configured to free run @1MHz from BootROM. */
#define APBTIMER0_CNT_REG   0xD4014090
#define APBTIMER0_EN_REG    0xD4014000

unsigned long GetTimer0CNT(void){
	return *(volatile unsigned long*)APBTIMER0_CNT_REG;
}

void Timer0_Switch(unsigned char OnOff)
{
	if(OnOff)        
		*(volatile unsigned long *)APBTIMER0_EN_REG |= 0x1;  //enable Timer0_0 (in free run)    
	else        
		*(volatile unsigned long *)APBTIMER0_EN_REG &= ~0x1; //disable Timer0_0 (in free run)
}

/*----------------------------------------------------------*/

unsigned long Timer0IntervalInMilli(unsigned long Before, unsigned long After)
{
    unsigned long temp = (After - Before);
    return (temp / (1000));
}


void DelayInMilliSecond(unsigned int ms)
{
    unsigned long startTime, endTime;

    startTime = GetTimer0CNT(); 
    do
    {
        endTime = GetTimer0CNT();    
    }
    while(Timer0IntervalInMilli(startTime, endTime) < ms);
}
const char usbmode = USB_NORMAL_MODE;

char GetUsbMode(void)
{
    char *mode = (char*)(&usbmode);
    return (*mode);
}

void UsbDriverGetSetting(void )
{



}


UINT8 gsATPMode[NUM_OF_TEL_ATP]={MODEM_CONTROL_MODE};

UINT8 Get_sATP_Mode(UINT8 sATP)
{
    if (sATP< NUM_OF_TEL_ATP)
        return gsATPMode[sATP];
    return 0xFF;

}

UINT8 ATCmdSvrRdy;
UINT8 gsATP2CID[];

BOOL getATRdyPhase1(void)
{
    return ATCmdSvrRdy;
}
BOOL openAtChannel(TelAtParserID channel)
{
    return TRUE;
}

void usb_uart_rx_data_in(uint8_t* data, uint32_t size)
{

}

void sendData2SACReq(unsigned char cid, char *buf, int len)
{


}

void USBMemPoolAlignFree(void* alignAddress)
{

    rtdm_memory_free(alignAddress);


}



/**
 *  @brief  system AIC - Set CP15 Interrupt Type
 *
 *  @param[in]  nIntState   Interrupt state. ( \ref ENABLE_IRQ / \ref ENABLE_FIQ / \ref ENABLE_FIQ_IRQ /
 *                                             \ref DISABLE_IRQ / \ref DISABLE_FIQ / \ref DISABLE_FIQ_IRQ)
 *
 *  @return   0
 */
INT32 sysSetLocalInterrupt(INT32 nIntState)
{
   INT32 temp;

   switch (nIntState)
   {
      case ENABLE_IRQ:
      case ENABLE_FIQ:
      case ENABLE_FIQ_IRQ:
           __asm
           {
               MRS    temp, CPSR
               AND    temp, temp, nIntState
               MSR    CPSR_cxsf, temp
           }
           break;

      case DISABLE_IRQ:
      case DISABLE_FIQ:
      case DISABLE_FIQ_IRQ:
           __asm
           {
               MRS    temp, CPSR
               ORR    temp, temp, nIntState
               MSR    CPSR_cxsf, temp
           }
           break;

      default:
           ;
   }
   return 0;
}

void XIRQClockOnOff(CLK_OnOff OnOff)           // PRM_SRVC_TIMER2_13M_GB
{                                                  // RSRC_TIMER2_13M_GB (0x1 << 16)
		UINT32 value;  
		value = REG_READ(ACER);
		if(OnOff == CLK_ON)
		{
			TURN_BIT_OFF(value, 1 << 2);
			TURN_BIT_ON (value, 1 << 1);
		}
		else
		{
			TURN_BIT_OFF(value, 1 << 1);
			TURN_BIT_ON (value, 1 << 2);
		}
		REG_WRITE(ACER, value);
}

#define  ARB_TIMEOUT1_EN_CLR 0xD42A0080
#define  ARB_TIMEOUT2_EN_CLR 0xD42A0088

#define  ARB_TIMEOUT1_STATUS 0xD42A0090
#define  ARB_TIMEOUT2_STATUS 0xD42A0098

#define  ARB_TIMEOUT1_ADDR   0xD42A00A0
#define  ARB_TIMEOUT2_ADDR   0xD42A00A8


UINT32 TIMEOUT1_STATUS;
UINT32 TIMEOUT1_ADDR;
void read_arb_regs(void )
{

	TIMEOUT1_STATUS  = REG_READ(ARB_TIMEOUT1_STATUS);
	uart_printf("read_arb_regs ARB_TIMEOUT1_STATUS=0x%x\r\n", TIMEOUT1_STATUS);
	if(TIMEOUT1_STATUS!=0x13)  //[4:3]=0x2: CR5 access; [0]=1: write
	   uart_printf("read_arb_regs Error: CR5 timeout1_status, ref_data=0x13, rd_data=0x%x\r\n", TIMEOUT1_STATUS);

	//read retry timeout address
	TIMEOUT1_ADDR  = REG_READ(ARB_TIMEOUT1_ADDR);
	uart_printf("read_arb_regs ARB_TIMEOUT1_ADDR=0x%x\r\n", TIMEOUT1_ADDR);
}

