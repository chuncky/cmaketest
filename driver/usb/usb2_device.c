/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/

/*******************************************************************************
 *
 * Title:        USB2.0 DEVICE - USB Device API
 *
 * Filename:     usb2_device.c
 *
 * Author:       Yael Shemla
 *
 * Description:  This file contains the USB Device Stack & API
 *
 * Notes:
 ******************************************************************************/

#if defined USB2ONLY      // following defines should be defined only once, if usb1 exist (i.e. !USB2ONLY) it will define it.
#define  _USB_DEVICE_TYPES_
#define _UDC_TYPES_
#endif
#include <stdint.h>
#include "usb_def.h"
#include "usb_device_types.h"   // for _usbDeviceDescriptor declaration
#undef  _USB_DEVICE_TYPES_
#include "udc_config.h"         // for _u2dcDefaultEndpointConfig definition
#undef _USB_TYPES_
#include "usb_device_def.h"     // for USB_DEVICE_EP0_GET_TRANSACTION_DIR
#include "usb_cable.h"

#include "udc_types.h"          // for UDC_EP_DIR_IN definition

#include "mvUsbCh9.h"           // for REQ_TYPE_STANDARD definition
#include "mvUsbDevApi.h"
#include "mvUsbDevPrv.h"        // for USB_DEV_STATE_STRUCT_PTR definition

//#include "prm.h"
#include "intc.h"               // for INTCConfigure declaration
#include "intc_list.h"          // for INTC_SRC_USB2CI_OTG definition
#include "gpio.h"               // for GPIO_DEBOUNCE_DISABLE definition
//#include "diag_API.h"
#include "bsp.h"
//#include "log.h"
#include "rndis.h"
#include "mvUsbLog.h"
#ifdef MV_USB2_MASS_STORAGE
#include "mvUsbStorage.h"
#endif
#include "usb_init.h"
#include "usb2_device.h"
#include "mvUsbModem.h"
#include "usb_descriptor.h"
//#define BU_REG_READ(x) (*(volatile unsigned long *)(x))
//#define BU_REG_WRITE(x,y) ((*(volatile unsigned long *)(x)) = y )

#define fatal_printf uart_printf
//#include "bsp_hisr.h"

#define USB_SPH_MISC_CR 		0xC0000004
#define PMUA_USB_CLK_RES_CTRL 	0xd428285c

OS_HISR _rxUsbCommHisrRef;
void usbCommDevHisr(void );

OS_HISR _rxUsbWakeupHisrRef;


 OS_HISR UsbCheckConnHisrRef;
 OS_HISR UsbSimUnplugHisrRef;

#define IPNET_MOD	"[IPNT]"

#define USB_POOL_SIZE  8*1024
#define DIAG_MAX_RX_MSG_SIZE 4096

#pragma arm section zidata="USB_POOL"
 __align(8) UINT8 USBPoolArray	[USB_POOL_SIZE];
#pragma arm section zidata

__align(8)  UINT8 _diag_rx_data_buffer[DIAG_MAX_RX_MSG_SIZE];
__align(4) UINT8 _sulog_rx_data_buffer[128];

#define GPIOG1_BASE	(0xD4019004)
#define GPIOG1_PLR	(GPIOG1_BASE + 0)
#define GPIOG1_PDR	(GPIOG1_BASE + 0xC)
#define GPIOG1_PSR	(GPIOG1_BASE + 0x18)
#define GPIOG1_PCR	(GPIOG1_BASE + 0x24)
#define GPIOG1_CDR	(GPIOG1_BASE + 0x60)


#define GPIOG1_RER	(GPIOG1_BASE + 0x30)
#define GPIOG1_EDR	(GPIOG1_BASE + 0x48)
#define GPIOG1_CPMASK (GPIOG1_BASE + 0xA8)
#define GPIO33_SHIFT	(33 - 32)
#define GPIO34_SHIFT	(34 - 32)
#define GPIO35_SHIFT	(35 - 32)
#define GPIO50_SHIFT	(50 - 32)
#define GPIO54_SHIFT	(54 - 32)


#define	GPIO33_MFPR	(0xD401E160)
#define	GPIO34_MFPR	(0xD401E164)
#define	GPIO35_MFPR	(0xD401E168)
#define	GPIO50_MFPR	(0xD401E1A4)
#define	GPIO54_MFPR	(0xD401E1B4)

#define SHIFT0(Val)  (Val)
#define SHIFT1(Val)  ((Val) << 1)
#define SHIFT2(Val)  ((Val) << 2)
#define SHIFT3(Val)  ((Val) << 3)
#define SHIFT4(Val)  ((Val) << 4)
#define SHIFT5(Val)  ((Val) << 5)
#define SHIFT6(Val)  ((Val) << 6)
#define SHIFT7(Val)  ((Val) << 7)
#define SHIFT8(Val)  ((Val) << 8)
#define SHIFT9(Val)  ((Val) << 9)
#define SHIFT10(Val) ((Val) << 10)
#define SHIFT11(Val) ((Val) << 11)
#define SHIFT12(Val) ((Val) << 12)
#define SHIFT13(Val) ((Val) << 13)
#define SHIFT14(Val) ((Val) << 14)
#define SHIFT15(Val) ((Val) << 15)
#define SHIFT16(Val) ((Val) << 16)
#define SHIFT17(Val) ((Val) << 17)
#define SHIFT18(Val) ((Val) << 18)
#define SHIFT19(Val) ((Val) << 19)
#define SHIFT20(Val) ((Val) << 20)
#define SHIFT21(Val) ((Val) << 21)
#define SHIFT22(Val) ((Val) << 22)
#define SHIFT23(Val) ((Val) << 23)
#define SHIFT24(Val) ((Val) << 24)
#define SHIFT25(Val) ((Val) << 25)
#define SHIFT26(Val) ((Val) << 26)
#define SHIFT27(Val) ((Val) << 27)
#define SHIFT28(Val) ((Val) << 28)
#define SHIFT29(Val) ((Val) << 29)
#define SHIFT30(Val) ((Val) << 30)
#define SHIFT31(Val) ((unsigned int)(Val) << 31)


static __align(8) UINT8 ep0_out_data_buffer[USB_EP0_MAX_RX_TRANSFER_SIZE];
static __align(8) UINT8 ep0_ctrl_setup_buff[UDC2_EP0_CTRL_TRANSFER_SETUP_PACKET_SIZE];
const UDC_EndpointConfigS* udcDefaultEndpointCfg = _u2dcDefaultEndpointConfig;

void usbCommDevInit( void );
static void USB2DriverInterruptHandler(INTC_InterruptInfo interruptInfo);
static void USB2DeviceDatabaseReset(BOOL fullInit);
//static void USB2ConfigureWakeup(void);
static void USB2DeviceHWInit(void);
static UINT8 USB2DeviceStartEp0 (void* handle);
static void USB2DeviceControlProcessIdle(void* handle, uint_8  type, boolean  setup, uint_8 direction, uint_8* buffer, uint_32 length, uint_8  error);
static void USB2DeviceEventNotify (void* handle, uint_8  type, boolean  setup, uint_8 direction, uint_8* buffer, uint_32 length, uint_8  error);
static void USB2DeviceTransactionCompleted (void* handle, uint_8  endpoint, boolean  setup, uint_8 direction, uint_8* buffer, uint_32 length, uint_8  error);
extern uint_8 _sulog_hw_mode_start(_usb_device_handle handle ,INT32 sulog_output_port);
extern uint_8 _sulog_hw_mode_init(_usb_device_handle handle ,INT32 sulog_output_port);
extern uint_8 _sulog_hw_mode_stop(_usb_device_handle handle ,INT32 sulog_output_port);
extern void SulogOffControlForUsbRemove(void);
void SetUsbPmLock(uint_8 val);
extern void craneCommPMPowerupUsbPhy(void);
extern void ustica_USB_turnOn(void);
extern void craneCommPMPowerdownUsbPhy(void);

extern void *USBMemPoolAlignMalloc(UINT32 size);
extern void CommPMEnableUsbWakeupSource(void);
//extern void DbgPrintf(char* fmt, ...);
extern void InitUSBMemoryPool	(void);
extern UINT32 is_dev_chnl_ready(UINT32 chnl);
extern void set_cp2ap_pm_gpio_high(void);
extern USBDeviceStatusNotifyFn  statusNotifyFn_Patch;
extern UDC_EndpointE            USBDeviceConvertUSB2UDCEndpoint(USBDevice_EndpointE endpoint);
USB_DEV_STATE_STRUCT_PTR        _usbDeviceHandle = NULL;
extern USB_DEV_STATE_STRUCT     ci2Device;
const UDC_EndpointConfigS _u2dcHsicDefaultEndpointConfig[UDC_TOTAL_ENDPOINTS] =
{
/*  UDC        Config  Int   Alt.     USB         Endpoint          Endpoint        Max.         Double     Endpoint
   Endpoint     Num.   Num.  Set.   Endpoint        Type            Direction      Packet        Buffer      Enable
                                      Num                                           Size         Enable                  */
/*ENDPOINT_0   { 0,     0,    0,  0          , UDC_EP_CONTROL  , 0xFF          , UDC_MPS_16  , UDC_DB_ENA, UDC_EP_ENA },   ENDPOINT_0*/
/*ENDPOINT_0*/ { 0,     0,    0,  0          ,(UDC_EndpointTypeE)0,(UDC_EndpointDirE)0,UDC_MPS_64,(UDC_EndpointDoubleBufE)0,(UDC_EndpointEnableE)0}, /*ENDPOINT_0*/
/*ENDPOINT_A*/ { 1,     0,    0,  1          , UDC_EP_BULK     , UDC_EP_DIR_IN , UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_A*/
/*ENDPOINT_B*/ { 1,     0,    0,  2          , UDC_EP_BULK     , UDC_EP_DIR_OUT, UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_B*/
/*ENDPOINT_C*/ { 1,     0,    0,  3          , UDC_EP_BULK     , UDC_EP_DIR_IN , UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_C*/
/*ENDPOINT_D*/ { 1,     0,    0,  4          , UDC_EP_BULK     , UDC_EP_DIR_OUT, UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_D*/
/*ENDPOINT_E*/ { 1,     0,    0,  5          , UDC_EP_BULK     , UDC_EP_DIR_IN , UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_E*/
/*ENDPOINT_F*/ { 1,     0,    0,  6          , UDC_EP_BULK     , UDC_EP_DIR_OUT, UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_F*/
/*ENDPOINT_G*/ { 1,     0,    0,  7          , UDC_EP_BULK     , UDC_EP_DIR_IN , UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_G*/
/*ENDPOINT_H*/ { 1,     0,    0,  8          , UDC_EP_BULK     , UDC_EP_DIR_OUT, UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_H*/
/*ENDPOINT_I*/ { 1,     0,    0,  9          , UDC_EP_BULK     , UDC_EP_DIR_IN , UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_I*/
/*ENDPOINT_J*/ { 1,     0,    0,  0xA        , UDC_EP_BULK     , UDC_EP_DIR_OUT, UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_J*/
/*ENDPOINT_K*/ { 1,     0,    0,  0xB        , UDC_EP_BULK     , UDC_EP_DIR_IN,  UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_K*/
/*ENDPOINT_L*/ { 1,     0,    0,  0xC        , UDC_EP_BULK     , UDC_EP_DIR_OUT, UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_L*/
/*ENDPOINT_M*/ { 1,     0,    0,  0xD        , UDC_EP_BULK     , UDC_EP_DIR_IN,  UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_M*/
/*ENDPOINT_N*/ { 1,     0,    0,  0xE        , UDC_EP_BULK     , UDC_EP_DIR_OUT, UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_N*/
/*ENDPOINT_P*/ { 0,     0,    0,  0          ,(UDC_EndpointTypeE)0,(UDC_EndpointDirE)0,0,(UDC_EndpointDoubleBufE)0,(UDC_EndpointEnableE)0}, /*ENDPOINT_P*/
/*ENDPOINT_Q*/ { 0,     0,    0,  0          ,(UDC_EndpointTypeE)0,(UDC_EndpointDirE)0,0,(UDC_EndpointDoubleBufE)0,(UDC_EndpointEnableE)0}, /*ENDPOINT_Q*/
/*ENDPOINT_R*/ { 0,     0,    0,  0          ,(UDC_EndpointTypeE)0,(UDC_EndpointDirE)0,0,(UDC_EndpointDoubleBufE)0,(UDC_EndpointEnableE)0}, /*ENDPOINT_R*/
/*ENDPOINT_S*/ { 0,     0,    0,  0          ,(UDC_EndpointTypeE)0,(UDC_EndpointDirE)0,0,(UDC_EndpointDoubleBufE)0,(UDC_EndpointEnableE)0}, /*ENDPOINT_S*/
/*ENDPOINT_T*/ { 0,     0,    0,  0          ,(UDC_EndpointTypeE)0,(UDC_EndpointDirE)0,0,(UDC_EndpointDoubleBufE)0,(UDC_EndpointEnableE)0}, /*ENDPOINT_T*/
/*ENDPOINT_U*/ { 0,     0,    0,  0          ,(UDC_EndpointTypeE)0,(UDC_EndpointDirE)0,0,(UDC_EndpointDoubleBufE)0,(UDC_EndpointEnableE)0}, /*ENDPOINT_U*/
/*ENDPOINT_V*/ { 0,     0,    0,  0          ,(UDC_EndpointTypeE)0,(UDC_EndpointDirE)0,0,(UDC_EndpointDoubleBufE)0,(UDC_EndpointEnableE)0}, /*ENDPOINT_V*/
/*ENDPOINT_W*/ { 0,     0,    0,  0          ,(UDC_EndpointTypeE)0,(UDC_EndpointDirE)0,0,(UDC_EndpointDoubleBufE)0,(UDC_EndpointEnableE)0}, /*ENDPOINT_W*/
/*ENDPOINT_X*/ { 0,     0,    0,  0          ,(UDC_EndpointTypeE)0,(UDC_EndpointDirE)0,0,(UDC_EndpointDoubleBufE)0,(UDC_EndpointEnableE)0}  /*ENDPOINT_X*/
};

#ifdef USB_REMOTEWAKEUP
extern BOOL SetRemoteWakeupFeature;
extern BOOL EnableRemoteWakeup;
#endif
/*******************************************************************************
 * Function:    usb_get_dtd_nums
 *******************************************************************************
 * Description:  USB get DTD numbers.
 *
 *
 * Parameters:
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
UINT16 usb_get_dtd_nums(void)
{
    USB_DEV_STATE_STRUCT_PTR usb_dev_ptr;

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)_usbDeviceHandle;
    return usb_dev_ptr->DTD_ENTRIES;
}

/*******************************************************************************
 * Function:    USB_uncached_memalloc
 *******************************************************************************
 * Description:  USB get noncached memory.
 *
 *
 * Parameters:
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
UINT32* USB_uncached_memalloc(UINT32 size, UINT32 phyAddr)
{

	UINT32* ptr = NULL;
	//uart_printf("USB_uncached_memalloc size is %d \r\n",size);
	ptr=(UINT32 *)USBMemPoolAlignMalloc(size);
	//uart_printf("ptr is 0x%lx \r\n", (UINT32 *) ptr);

	ASSERT(ptr != NULL);
	return ptr;

}

static uint_8 USB2DeviceStartEp0 (void* handle)
{
    UINT8 error = USB_OK;
    error = _usb_device_init_endpoint(handle, 0, 64, MV_USB_SEND,  MV_USB_CONTROL_ENDPOINT, 0);
    if (error != USB_OK)
    {
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ERROR, "%s: failed in _usb_device_init_endpoint SEND, error=%d\n", __FUNCTION__, (int)error);
    }
    else
    {
        error = _usb_device_init_endpoint(handle, 0, 64, MV_USB_RECV, MV_USB_CONTROL_ENDPOINT, 0);
        if (error != USB_OK)
        {
            MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ERROR, "%s: failed in _usb_device_init_endpoint RECV, error=%d\n", __FUNCTION__, (int)error);
        }
    }
    if (error == USB_OK)
        _usbDeviceEndpoint[0].endpointOpen = TRUE;
    else
        _usbDeviceEndpoint[0].endpointOpen = FALSE;

//    error = _usb_device_send_data (_usbDeviceHandle, 0 /* EP #0 */, _usbDeviceDescriptors.pDeviceDescriptor, _usbDeviceDescriptors.deviceDescriptorLength);

    return error;
}


/*******************************************************************************
 * Function:    USB2DeviceEndpointGetHWCfg
 *******************************************************************************
 * Description:  returns endpoint HW configuration
 *
 *
 * Parameters:
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
UDC_EndpointConfigS* USB2DeviceEndpointGetHWCfg(USBDevice_EndpointE endpoint)
{
    if( USB2_IS_FULL_SPEED())
    {
        return ((UDC_EndpointConfigS *)(&(_u1dcDefaultEndpointConfig[USBDeviceConvertUSB2UDCEndpoint(endpoint)])));
    }
    else
    {
        return ((UDC_EndpointConfigS *)(&(_u2dcDefaultEndpointConfig[USBDeviceConvertUSB2UDCEndpoint(endpoint)])));
    }
}
#ifdef EDEN_1928
#define INTC_SRC_USB2CI_OTG  INTC_SRC_USB_EDEN
#elif defined(NEZHA3_1826)
#define INTC_SRC_USB2CI_OTG  INTC_SRC_USB
#else
#define INTC_SRC_USB2CI_OTG  INTC_SRC_USB
#endif
void usbDeviceEndpoint_init(void)
{
  memset((void*)_usbDeviceEndpoint,0,sizeof(_usbDeviceEndpoint));
}

volatile UINT32 usb_ddr_lock = 0;
volatile UINT32 usb_remote_test = 0;
volatile UINT32 pm_resume_pending = 0;
extern volatile UINT32 pm_usb_busy;


/* NOTE: what's the BOARD_1/2 */
void GPIOED_LISR(UINT32 sourceNum)
{

}

void GPIO_InitWakeup(void)
{

}

uint_8 Get_GPIO_WakeupLevel(void)
{

	/*Fix coverity[missing_return]*/
	return 0;
}

uint_8 GetUsbDDRLock(void)
{
	//if(AC_IS_CPONLY)
		return pm_usb_busy;
	//else
		//return usb_ddr_lock;
}
void SetUsbDDRLock( uint_8 val)
{

	usb_ddr_lock = val;
}
void USB_GPIO_WK(void)
{
	MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_PMU_WK,"A2C\r\n");
	SetUsbDDRLock(1);
	SetUsbPmLock(1);
	CommPMEnableUsbWakeupSource();
	set_cp2ap_pm_gpio_high();
}
void SendUsbRemoteWakeup(void)
{
	//DbgPrintf("Remote Wakeup\r\n");

	MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_PMU_WK,"RW\r\n");

	SetUsbDDRLock(1);

	//open PHY clock and poll for complete
	*(volatile unsigned long *)(0xD4208184) &= ~(1 << 23);
	while(*(volatile unsigned long *)(0xD4208184) & (1 << 23));

	set_cp2ap_pm_gpio_high();

	//force resume
	//*(volatile unsigned int *)(0xD4208184) |=  (1 << 6);

	pm_usb_busy = 1;
	pm_resume_pending = 1;


}
UINT32 GetResumePending(void)
{
	return pm_resume_pending;
}
void ClearResumePending(void)
{
	pm_resume_pending = 0;
}
UINT8 GetUsbPmLock(void)
{
	//if(AC_IS_CPONLY)
		return pm_usb_busy;
	//else
	//	return (pm_usb_busy | 1 | pm_resume_pending);
}
uint_8 Check_AP2CP_Conflict(void)
{
	return  pm_usb_busy == 0; //((pm_usb_busy == 0) && (GetUSBGPIO() == 1));
}

#ifdef NEZHA3_1826
extern unsigned char Is_Nezha3Stepping_FromZ3(void);
extern void SetAXISD(unsigned char set_flag);
#endif

void SetUsbPmLock(UINT8 val)
{
	pm_usb_busy = val;

#if 0
	if(Is_Nezha3Stepping_FromZ3())
	{
		if(val)
		{
			SetAXISD(0);
		}
		else
		{
			SetAXISD(1);
		}
	}
#endif
}

UsbWkpCallback  usbWakeupHandler=NULL;
OSATimerRef usbtimerref;

extern volatile UINT32    usb_shutdown;
extern volatile UINT32 diagTxFail;
//extern void ustica_USB_shutdown(void);
extern void diag_output_to_usb_stop(UINT8 block);
extern void USB2MgrDeviceUnplugPlug_plus(PlatformUsbDescType desc);
extern void usb_uart_connect(uint8_t connect_status);

BOOL checkMassstorageEnabled(void)
{
    Usb_DriverS *pUsbDrvConfig = GetUsbDriverConfig();
    if(pUsbDrvConfig->mass_storage == MASS_STORAGE_ENABLE)
        return TRUE;
    else
        return FALSE;

}
extern volatile UINT32 SDNeedtoRemount;
volatile UINT32 USBEnuming = 0;
void checkUSBconnection(UINT32 tsT32k)
{
    STATUS osStatus;

	osStatus = OS_Activate_HISR(&UsbCheckConnHisrRef);
	ASSERT(osStatus == OS_SUCCESS);

}
void usbCheckConnectHisr(void)
{
    Usb_DriverS *pUsbDrvConfig = GetUsbDriverConfig();
    mvUsbDescriptorInfo *pUsbDesInfo = USB2GetDescriptorInfo();
	if(*(volatile unsigned long*)0xD428287C & 1 << 15) //usb connected
	{
		uart_printf("UC%d\r\n",checkMassstorageEnabled());
		SetUsbPmLock(1);
		USBEnuming = 1;
 //       OSATimerStart(usbEnumtimerref, 200*2, 0, enumwaitdone, 0);
		ustica_USB_turnOn();

		craneCommPMPowerupUsbPhy();
		usb_shutdown = 0;

#ifdef PLAT_USE_ALIOS
		usb_uart_connect(1);
#endif
	}
	else
	{
	    uart_printf("UD\r\n");
		usb_shutdown = 1;
		
		USBEnuming = 0;
		SetUsbPmLock(0);
		#ifdef LWIP_IPNETBUF_SUPPORT
        SetUsbEnumFlag(0);
		#endif
		SulogOffControlForUsbRemove();
		#ifdef LWIP_IPNETBUF_SUPPORT
		_usb_dci_vusb20_stop_transter();
		#endif
		//ustica_USB_shutdown();
		craneCommPMPowerdownUsbPhy();
		if(checkMassstorageEnabled())
		{
            uart_printf("SD\r\n");
            SDNeedtoRemount = 1;
        }
		#ifdef PROJ_ASR_APP_001
		pUsbDrvConfig->mass_storage = MASS_STORAGE_DISABLE;
		#endif
		USB2MgrUpdateDescriptor(pUsbDesInfo->ReConfigDesc, pUsbDesInfo->UsbMode);
		diag_output_to_usb_stop(1);
#ifdef PLAT_USE_ALIOS
		usb_uart_connect(0);
#endif
	}
	diagTxFail = 0;
    if(usbWakeupHandler)
        (*usbWakeupHandler)();//call hisr func
	OSATimerStop (usbtimerref);
}

void usbWakeUpHisr(void )
{
	//delay 100ms to read USB connect register
	OSATimerStart(usbtimerref, 4, 0, checkUSBconnection, 0);
}

void triggerUsbWakeUpHisr(void )
{
    ASSERT(OS_SUCCESS == OS_Activate_HISR(&_rxUsbWakeupHisrRef));
}

void USB_WAKEUP_CALLBACK_REGISTER(UsbWkpCallback handler)
{
    if (handler != NULL)
	    usbWakeupHandler = handler;
}
void USB2UnPlugHISR(void)
{
    ErrorLogPrintf("USB UnPlug HISR");
    _usb_dci_vusb20_stop_transter();
}

void USB2UnPlugHandler(void)
{
	STATUS osStatus;

	osStatus = OS_Activate_HISR(&UsbSimUnplugHisrRef);
	ASSERT(osStatus == OS_SUCCESS);
}

void USB2DevicePhase2Init(void)
{
    UINT8 error;
	UINT32 tmp;
    uart_printf("%s-01\r\n",__func__);
    _usbDeviceHandle = &ci2Device;

	//uart_printf("USB2DevicePhase2Init start\r\n");
    InitUSBMemoryPool();

#ifndef CRANE_MCU_DONGLE
    usbCommDevInit();
#endif

	OSATimerCreate(&usbtimerref);

#ifndef CRANE_MCU_DONGLE
	OS_Create_HISR(&_rxUsbCommHisrRef, "usbCommDev", usbCommDevHisr, 2);
#endif

	OS_Create_HISR(&_rxUsbWakeupHisrRef, "usbWakeUp", usbWakeUpHisr, 2);	
	OS_Create_HISR(&UsbCheckConnHisrRef, "UsbCConn", usbCheckConnectHisr, 2);
	OS_Create_HISR(&UsbSimUnplugHisrRef, "UsbUplug", USB2UnPlugHISR, 2);
	

    //log_printf("InitUSBMemoryPool\n");
#if defined MV_USB_TRACE_PRINT
    UsbDeviceSetupUARTforDebug();
#endif
    USB2DeviceHWInit();
   // log_printf("USB2DeviceHWInit\n");

    //INTCConfigure(INTC_SRC_USB2CI_OTG, INTC_IRQ, /* INTC_HIGH_LEVEL */  INTC_RISING_EDGE, GPIO_DEBOUNCE_DISABLE);   // YSS TO RETURN HERE CHECK IF INTC_HIGH_LEVEL IS RIGHT
	INTCConfigure(INTC_SRC_USB2CI_OTG, INTC_IRQ, INTC_HIGH_LEVEL);
	//log_printf("INTCConfigure\n");
    INTCBind(INTC_SRC_USB2CI_OTG, USB2DriverInterruptHandler);
   // log_printf("USB INTCBind\n ");

//    USB2_DEACTIVATE_HARDWARE();

    // Init the driver context struct NOW
    //------------------------------------------------------
    memset( &ci2Device, 0x00, sizeof(USB_DEV_STATE_STRUCT) );


    //------------------------------------------------
    // services should be registered before peripheral is "connected" to USB transport bus.
    // we put the non EP0 transaction complete service at the head of the service list
    // to reduce search time for this particular service
    //------------------------------------------------

    // register the "standard request" function
    //----------------------------------------
    error = _usb_device_register_service(_usbDeviceHandle, MV_USB_SERVICE_EP0, USB2DeviceControlProcessIdle);
    if (error != USB_OK)
    {
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ERROR, "%s: registration of control idle, error=%d\n", __FUNCTION__, (int)error);
    }

    // resgister the "reset", "suspend", "resume" function with the driver
    //---------------------------------------------------------------------
    error = _usb_device_register_service(_usbDeviceHandle, MV_USB_SERVICE_BUS_RESET, USB2DeviceEventNotify);
    if (error != USB_OK)
    {
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ERROR, "%s: failed in registration of reset event, error=%d\n", __FUNCTION__, (int)error);
    }
    error = _usb_device_register_service(_usbDeviceHandle, MV_USB_SERVICE_SUSPEND, USB2DeviceEventNotify);
    if (error != USB_OK)
    {
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ERROR, "%s: failed in registration of suspend event, error=%d\n", __FUNCTION__, (int)error);
    }
    error = _usb_device_register_service(_usbDeviceHandle, MV_USB_SERVICE_RESUME, USB2DeviceEventNotify);
    if (error != USB_OK)
    {
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ERROR, "%s: failed in registration of resume event, error=%d\n", __FUNCTION__, (int)error);
    }

	//log_printf("try to do usb device init.\n");
    // This call brings up the entire CI2 peripheral
    // including memory allocation, device reset,
    // and determining device base address and offsets
    //-------------------------------------------------
    error = _usb_device_init(0, (_usb_device_handle *)(&_usbDeviceHandle));
    if (error != USB_OK)
    {
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ERROR, "%s: device init failed, error=%d\n", __FUNCTION__, (int)error);
    }
    // This is a Self-Powered Device so set device status to show that.
    _usb_device_set_status(_usbDeviceHandle, MV_USB_STATUS_DEVICE, MV_USB_SELF_POWERED);
    // log_printf("USB2DevicePhase2Init end\n ");
	//if(AC_IS_CPONLY)
	{
		 //if power up when deattached, chip sleep could be predicted, so turn on wakeup immediately
		if (USB2_IS_ATTACHED() == FALSE)
		{
			CommPMEnableUsbWakeupSource();
		}
	}
    uart_printf("%s-end\r\n",__func__);
} /* End of < UDCDriverPhase2Init > */


static void USB2DriverInterruptHandler(INTC_InterruptInfo interruptInfo)
{
    _usb_dci_vusb20_isr(_usbDeviceHandle);
}

void USB2DriverInterruptloopHandler(void)
{
	 _usb_dci_vusb20_isr(_usbDeviceHandle);
}




void _usb_cancel_all_transfer(_usb_device_handle handle)
{
    USB_DEV_STATE_STRUCT_PTR     usb_dev_ptr;
    SERVICE_STRUCT_PTR           service_ptr, temp_ptr;
    int                          ep;

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    for(ep=0; ep<(usb_dev_ptr->MAX_ENDPOINTS); ep++)
    {
        /* Cancel all transfers on all endpoints */
        while(_usb_device_get_transfer_status(handle, ep, MV_USB_RECV) !=
                                                    MV_USB_STATUS_IDLE)
        {
//        	DbgPrintf("cancel %d recv\r\n",ep);
			//log_printf(LOG_CRIT IPNET_MOD"cancle end %d\r\n",ep);
            _usb_device_cancel_transfer(handle, ep, MV_USB_RECV);
        }
        while(_usb_device_get_transfer_status(handle, ep, MV_USB_SEND) !=
                                                    MV_USB_STATUS_IDLE)
        {
//        	DbgPrintf("cancel %d send\r\n",ep);
            _usb_device_cancel_transfer(handle, ep, MV_USB_SEND);
        }
    }
}

#define HSIC_POLL_CHNL	1
#define HSIC_POLL_BUF_LEN (1024*4)

UINT8 hsic_poll_tx_buf[HSIC_POLL_BUF_LEN];
UINT8 hsic_poll_rx_buf[HSIC_POLL_BUF_LEN];

UINT32 hsic_eeh_temp_buf[HSIC_POLL_BUF_LEN/4];


UINT32 hsic_poll_tx_done_flag=0;
UINT32 hsic_poll_tx_len=0;

UINT32 hsic_poll_rx_done_flag=0;
UINT32 hsic_poll_rx_len=0;


void hsic_poll_tx_done(UINT8 direction, UINT8* buffer, UINT32 length)
{
	hsic_poll_tx_done_flag=1;
	hsic_poll_tx_len=length;
}

void hsic_poll_rx_done(UINT8 direction, UINT8* buffer, UINT32 length)
{
	hsic_poll_rx_done_flag=1;
	hsic_poll_rx_len=length;
}

void hsic_poll_init(void)
{
	USBCDevice_ReturnCodeE usbStatus;
	BspCustomType type;

	type=bspGetBoardType();
	{
		_usb_cancel_all_transfer(_usbDeviceHandle);
	}

}

void hsic_delay(UINT32 val)
{
	volatile UINT32 cnt;
	cnt=val;
	while(cnt--)
		;

}


UINT32 hsic_read_poll(void *buf)
{
	USBCDevice_ReturnCodeE usbStatus;
	static UINT32 tmp_cnt_read=0;
	hsic_poll_rx_done_flag=0;
	tmp_cnt_read++;
	if(tmp_cnt_read<10)
		hsic_delay(10000);

//	DbgPrintf("hsic_read_poll\r\n");
	usbStatus = USBDeviceEndpointReceive((USBDevice_EndpointE)(2*HSIC_POLL_CHNL), hsic_poll_rx_buf, HSIC_POLL_BUF_LEN, TRUE);
	while(hsic_poll_rx_done_flag==0)
		_usb_dci_vusb20_isr(_usbDeviceHandle);
	memcpy(buf, hsic_poll_rx_buf, hsic_poll_rx_len);
//	DbgPrintf("hsic_read_poll Done %d\r\n",hsic_poll_rx_len);
	return hsic_poll_rx_len;
}

UINT32 hsic_write_poll(UINT8 *buf,UINT32 len)
{
	static UINT32 tmp_cnt_write=0;

	USBCDevice_ReturnCodeE usbStatus;
	hsic_poll_tx_done_flag=0;

	tmp_cnt_write++;
	if(tmp_cnt_write<10)
		hsic_delay(10000);

//	DbgPrintf("hsic_write_poll %d\r\n",len);
	usbStatus = USBDeviceEndpointTransmit((USBDevice_EndpointE)(HSIC_POLL_CHNL*2-1),(UINT8 *)buf,(UINT16)len, TRUE);
	while(hsic_poll_tx_done_flag==0)
		_usb_dci_vusb20_isr(_usbDeviceHandle);
//	DbgPrintf("hsic_write_poll %d Done\r\n",len);
	return len;
}


/*******************************************************************************
 * Function:    USB2DeviceDatabaseReset
 *******************************************************************************
 * Description: Initialize internal database
 *
 * Parameters:  fullInit    - full initialization (after powerup) or partial
 *                            (during operation)
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
static void USB2DeviceDatabaseReset(BOOL fullInit)
{
    UINT32          idx;

    /* Descriptors */
    if(fullInit)
    {
        _usbDeviceDescriptors.pDeviceDescriptor                     = NULL;
        _usbDeviceDescriptors.deviceDescriptorLength                = 0;
        _usbDeviceDescriptors.pConfigDescriptor                     = NULL;
        _usbDeviceDescriptors.configDescriptorLength                = 0;
   		_usbDeviceDescriptors.pQualifDescriptor						= NULL;
		_usbDeviceDescriptors.qualifDescriptorLength				= 0;
   		_usbDeviceDescriptors.pOtherSpeedDescriptor					= NULL;
		_usbDeviceDescriptors.otherSpeedDescriptorLength			= 0;
        _usbDeviceDescriptors.stringDescriptorTotal                 = 0;

        for(idx=0; idx<USB_DEVICE_TOTAL_STRINGS; idx++)
        {
            _usbDeviceDescriptors.pStringDescriptor[idx]            = NULL;
        }
/*
        #if defined MV_USB2_FULL_SPEED_MODE
        udcDefaultEndpointCfg = _u1dcDefaultEndpointConfig;
        #else
        udcDefaultEndpointCfg = _u2dcDefaultEndpointConfig;
        #endif
*/
    }

    /* USB device data base */
    _usbDeviceDatabase.status                                       = USB_DEVICE_STATUS_NOT_CONNECTED;
    if(fullInit)
    {
        _usbDeviceDatabase.statusNotifyFn                           = NULL;
        _usbDeviceDatabase.vendorClassRequestNotifyFn               = NULL;
        _usbDeviceDatabase.endpointZeroNotifyFn                     = NULL;
    }

    /* Endpoint 0 */
    _usbDeviceDatabase.ep0.state                                    = USB_DEVICE_EP0_STATE_IDLE;
    _usbDeviceDatabase.ep0.operation                                = USB_DEVICE_EP0_NO_OPERATION;
    _usbDeviceDatabase.ep0.ctrl_out_data_stage_buff                 = ep0_out_data_buffer;
    _usbDeviceDatabase.ep0.ctrl_setup_buff                          = ep0_ctrl_setup_buff;
    _usbDeviceDatabase.ep0.crtl_trasnfers_cnt                       = 0;
    _usbDeviceDatabase.ep0.ep0_int_cnt                              = 0;
    _usbDeviceDatabase.ep0.ep0_IN_cnt                               = 0;
    _usbDeviceDatabase.ep0.ep0_OUT_cnt                              = 0;

    /* General Endpoint */
    for(idx=0; idx<USB_DEVICE_TOTAL_ENDPOINTS; idx++)
    {
        _usbDeviceEndpoint[idx].endpointBusy                         = FALSE;
        if(fullInit)
        {
            _usbDeviceEndpoint[idx].endpointOpen                     = FALSE;
            _usbDeviceEndpoint[idx].udcEndpoint                      = UDC_INVALID_ENDPOINT;
            _usbDeviceEndpoint[idx].transactionCompletedNotifyFn     = NULL;
        }
    }
} /* End of < USB2DeviceDatabaseReset > */

#if defined(UDC_USE_WAKEUP)                  // YSS - RETURN HERE TO ENABLE WAKEUP ISSUE FOR TAVOR PV.
static UINT32 USB2DeviceWakeupClear(void)
{
    #define ACR1_USB_WK_STATUS 0x00040000
    volatile UINT32 *ACR1_addr = (volatile UINT32*)0x42900068;
    UINT32 ACR1_val=*ACR1_addr;
    *ACR1_addr = ACR1_val & (~ACR1_USB_WK_STATUS); //write 0 bit 18;
    *ACR1_addr = ACR1_val | ACR1_USB_WK_STATUS;//write 1 bit 18
    return ACR1_val&ACR1_USB_WK_STATUS;
}

/*******************************************************************************
 * Function:    UDCDriverWakeupHandler
 *******************************************************************************
 * Description: Reset the UDC hardware
 *
 * Parameters:  interruptInfo
 *
 * Outt:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
static void USB2DeviceWakeupDisable(void)
{
    //disable wake up event detection in the pad - prevents UDC from working correcly
   _pUdcPadRegisters->UMCE = 0xD;
    USB2DeviceWakeupClear();
    PMUEWakeupControl(0,PMUE_WAKE_URE|PMUE_WAKE_USB2);
    INTCDisable(INTC_SRC_USB_OTG_PAD2_WAKEUP);
}

static void USB2DeviceWakeupEnable(void)
{
   _pUdcPadRegisters->UPWER     = 0x00000004;
    USB2DeviceWakeupClear();
    PMUEWakeupControl(PMUE_WAKE_URE|PMUE_WAKE_USB2,PMUE_WAKE_URE|PMUE_WAKE_USB2);
    INTCEnable(INTC_SRC_USB_OTG_PAD2_WAKEUP);
}

static void USB2DeviceWakeupHandler(INTC_InterruptInfo interruptInfo)
{
    USB2DeviceWakeupDisable();
    UDC_CLOCK_CONTROL(PMU_ON);
#ifdef EDEN_1928
    INTCEnable(INTC_SRC_USB_EDEN);
#elif defined (NEZHA3_1826)
	INTCEnable(INTC_SRC_USB_NZ3);
#else
    INTCEnable(INTC_SRC_USB);
#endif
#if defined(UDC_USE_WAKEUP_LED)
    usbWakeIndication(USB_AWAKE);
#endif //UDC_USE_WAKEUP_LED
}

#endif //UDC_USE_WAKEUP

/*******************************************************************************
 * Function:    USB2DeviceEndpointMultiTransmit
 *******************************************************************************
 * Description: Transmit data through endpoint
 *
 * Parameters:  endpoint        - handle to udcEndpoint
 *              pTxBuffer       - buffer to transmit
 *              length          - length of buffer
 *              autoZLP         - whether to send ZLP automatically or not
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
USBCDevice_ReturnCodeE USB2DeviceEndpointMultiTransmit(USBDevice_EndpointE            endpoint,
                                                     UINT32                         numOfBuffers,
                                                     USBDevice_MultiTransmitListS   *multiList)
{
                                 /*
     * YSS - need to implement it if necessary,
     * since now DMA is build in USB2.0, no need for DIAG to prepare multilist for DMA descriptors
     */

    return USB_DEVICE_RC_OK;
} /* End of < USB2DeviceEndpointMultiTransmit > */


/*******************************************************************************
 * Function:    USB2DeviceEndpointReceive
 *******************************************************************************
 * Description: Prepare an endpoint for receive ("Priming endpoint")
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
USBCDevice_ReturnCodeE USB2DeviceEndpointReceive(USBDevice_EndpointE         endpoint,
                                               UINT8                        *pRxBuffer,
                                               UINT32                       length,
                                               BOOL                         expect_zlp)
{
    UINT8   error = USB_OK;
    //unsigned long           cpsrReg;
    USBDevice_EndpointDataS *pEndpoint = &_usbDeviceEndpoint[endpoint];
    UINT16     mv_usb_status;


    DIAG_RAM_REPORT_FORMATTED_INDEX_2P(usbErrorDebug, 0x30, 0xAA);

    /* check for endpoint  */
    if(!pEndpoint->endpointOpen)
        return USB_DEVICE_RC_ENDPOINT_NOT_OPENED;

    /* sanity check for length */
    USB_DEVICE_ASSERT(length != 0);

#if 0
    /* no re-entrancy allowed for same endpoint !!! */
    cpsrReg = disableInterrupts(); /* lock */

    if( pEndpoint->endpointBusy )
    {
        restoreInterrupts(cpsrReg); /* unlock */
        return USB_DEVICE_RC_ENDPOINT_BUSY;
    }

    pEndpoint->endpointBusy     = TRUE;
    restoreInterrupts(cpsrReg); /* unlock */
#endif

    _usb_device_get_status(_usbDeviceHandle, MV_USB_STATUS_DEVICE_STATE, &mv_usb_status);
    if (mv_usb_status == MV_USB_STATE_CONFIG)
    {
        error = _usb_device_recv_data (_usbDeviceHandle, endpoint,  pRxBuffer, length);
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_INFO, "%s: perform rcv preparation ep#=%d, expected len=%d, buff=0x%x\n", __FUNCTION__, endpoint, length, (int)pRxBuffer);

        if (error != USB_OK)
        {
            MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ERROR, "%s: failed in _usb_device_recv_data, error=%d\n", __FUNCTION__, (int)error);
        }
    }
    else     // save recive buffer, to prime ep after enum completes.
    {
        _usbDeviceEndpoint[endpoint].pRxBuffer    = pRxBuffer;
        _usbDeviceEndpoint[endpoint].pRxBuffSize  = length;
    }

    //pEndpoint->endpointBusy = FALSE;

    return (error == USB_OK ? USB_DEVICE_RC_OK : USB_DEVICE_RC_ERROR);
} /* End of < USB2DeviceEndpointReceive > */

/*******************************************************************************
 * Function:    USB2DeviceEndpointTransmit
 *******************************************************************************
 * Description: Transmit data through endpoint
 *
 * Parameters:  endpoint        - udc endpoint
 *              pTxBuffer       - buffer to transmit
 *              length          - length of buffer
 *              autoZLP         - whether to send ZLP automatically or not
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
extern volatile UINT32 prepareUSBdown;
extern volatile UINT32 usb_shutdown;
USBCDevice_ReturnCodeE USB2DeviceEndpointTransmit(USBDevice_EndpointE endpoint,  UINT8 *pTxBuffer, UINT32 txLength, BOOL autoZLP)
{
    UINT8 error;
    //unsigned long           cpsrReg;
    USBDevice_EndpointDataS *pEndpoint = &_usbDeviceEndpoint[endpoint];

    DIAG_RAM_REPORT_FORMATTED_INDEX_2P(usbErrorDebug, 0x30, 0xAA);

    // CACHED_TO_NON_CACHED needed when getting buffers from DIAG that uses SHMEM;
    //these buffers from Harbell& Boerne shared memeory (0xD0... this macro will transtale it to 0xBF...)
    pTxBuffer = CACHED_TO_NON_CACHED(pTxBuffer);

    /* check for USB cable connection status */
    if (USB2_IS_ATTACHED() == FALSE || prepareUSBdown || usb_shutdown)
    {
        return USB_DEVICE_RC_NOT_CONNECTED;
    }

    /* check for endpoint  */
    if(!pEndpoint->endpointOpen)
    {
        return USB_DEVICE_RC_ENDPOINT_NOT_OPENED;
    }

    if ( endpoint != USB_DEVICE_ENDPOINT_0)      // YS - is it realy needed? (prevents zlp from non-ep0)
    {
        /* sanity check for length */
        USB_DEVICE_ASSERT(txLength != 0);
    }

#if 0
    /* no re-entrance is allowed for same endpoint !!! */
    cpsrReg = disableInterrupts(); /* lock */

    if( pEndpoint->endpointBusy )
    {
        restoreInterrupts(cpsrReg); /* unlock */
        return USB_DEVICE_RC_ENDPOINT_BUSY;
    }

    pEndpoint->endpointBusy     = TRUE;
    //pEndpoint->transferState    = USB_DEVICE_TRANSFER_STATE_TX_STARTED;
    restoreInterrupts(cpsrReg); /* unlock */

	///////////////////////////
	{
		usb_is_busy |= 1<<endpoint;
	}
	///////////////////////////

#endif


    error = _usb_device_send_data (_usbDeviceHandle, endpoint, pTxBuffer, txLength);
    if (error != USB_OK)
    {/* error result */
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ERROR, "%s: failed in _usb_device_send_data, error=%d\n", __FUNCTION__, (int)error);
    }

    //pEndpoint->endpointBusy     = FALSE;

    return (error == USB_OK ? USB_DEVICE_RC_OK : USB_DEVICE_RC_ERROR);
}

void _usb_device_sulog_init(INT32 sulog_output_port){
	_sulog_hw_mode_init(_usbDeviceHandle, sulog_output_port);
}
void _usb_device_sulog_start(INT32 sulog_output_port){
	_sulog_hw_mode_start(_usbDeviceHandle, sulog_output_port);
}
void _usb_device_sulog_stop(INT32 sulog_output_port)
{
	_sulog_hw_mode_stop(_usbDeviceHandle, sulog_output_port);
}

USBCDevice_ReturnCodeE USB2DeviceEndpointCancelTransmit(USBDevice_EndpointE endpoint)
{
	unsigned long			cpsrReg;
	USBDevice_EndpointDataS *pEndpoint = &_usbDeviceEndpoint[endpoint];
	UINT8 error;

//	DbgPrintf("Cancel %d\r\n",endpoint);

	/* check for USB cable connection status */
	if (USB2_IS_ATTACHED() == FALSE)
	{
		return USB_DEVICE_RC_NOT_CONNECTED;
	}

	/* check for endpoint  */
	if(!pEndpoint->endpointOpen)
	{
		return USB_DEVICE_RC_ENDPOINT_NOT_OPENED;
	}

#if 0
    /* no re-entrance is allowed for same endpoint !!! */
    cpsrReg = disableInterrupts(); /* lock */

    if( pEndpoint->endpointBusy )
    {
        restoreInterrupts(cpsrReg); /* unlock */
        return USB_DEVICE_RC_ENDPOINT_BUSY;
    }

    pEndpoint->endpointBusy     = TRUE;
    //pEndpoint->transferState    = USB_DEVICE_TRANSFER_STATE_TX_STARTED;
    restoreInterrupts(cpsrReg); /* unlock */
#endif

	error = _usb_device_cancel_transfer(_usbDeviceHandle, endpoint, 1);

    pEndpoint->endpointBusy     = FALSE;

    return (error == USB_OK ? USB_DEVICE_RC_OK : USB_DEVICE_RC_ERROR);

}


USBCDevice_ReturnCodeE USB2DeviceEndpointCancelReceive(USBDevice_EndpointE endpoint)
{
	unsigned long			cpsrReg;
	USBDevice_EndpointDataS *pEndpoint = &_usbDeviceEndpoint[endpoint];
	UINT8 error;


	/* check for USB cable connection status */
	if (USB2_IS_ATTACHED() == FALSE)
	{
		return USB_DEVICE_RC_NOT_CONNECTED;
	}

	/* check for endpoint  */
	if(!pEndpoint->endpointOpen)
	{
		return USB_DEVICE_RC_ENDPOINT_NOT_OPENED;
	}

#if 0
    /* no re-entrance is allowed for same endpoint !!! */
    cpsrReg = disableInterrupts(); /* lock */

    if( pEndpoint->endpointBusy )
    {
        restoreInterrupts(cpsrReg); /* unlock */
        return USB_DEVICE_RC_ENDPOINT_BUSY;
    }

    pEndpoint->endpointBusy     = TRUE;
    //pEndpoint->transferState    = USB_DEVICE_TRANSFER_STATE_TX_STARTED;
    restoreInterrupts(cpsrReg); /* unlock */
#endif

	error = _usb_device_cancel_transfer(_usbDeviceHandle, endpoint, 0);

    pEndpoint->endpointBusy     = FALSE;

    return (error == USB_OK ? USB_DEVICE_RC_OK : USB_DEVICE_RC_ERROR);

}

/*******************************************************************************
 * Function:    USB2DeviceVendorClassResponse
 *******************************************************************************
 * Description: Response function for Vendor/Class request
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes: Can be called only as a respinse to VendorClassReq callback
 *
 ******************************************************************************/
void USB2DeviceVendorClassResponse(UINT8                             *pBuffer,
                                  UINT16                            bufferLength)
{
	/* no re-entrancy allowed for same endpoint !!! */
	UINT32 cpsrReg = disableInterrupts(); /* lock */

	_usbDeviceDatabase.ep0.ctrl_in_data_stage_buff              = pBuffer;
	_usbDeviceDatabase.ep0.ctrl_in_data_stage_total_length      = bufferLength;
	_usbDeviceDatabase.ep0.ctrl_in_data_stage_current_length    = 0;

	USB2DeviceEndpointTransmit( USB_DEVICE_ENDPOINT_0 , pBuffer, bufferLength, FALSE);
	if ( bufferLength > 0 )
	{
	    _usb_device_recv_data(_usbDeviceHandle, 0,0,0);
	}


    restoreInterrupts(cpsrReg); /* unlock */
} /* End of < USB2DeviceVendorClassResponse > */

/*******************************************************************************
 * Function:    USB2DeviceEndpointReceiveCompleted
 *******************************************************************************
 * Description: Prepare an endpoint for receive
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
USBCDevice_ReturnCodeE USB2DeviceEndpointReceiveCompleted(USBDevice_EndpointE  endpoint)
{
    USBDevice_EndpointDataS *pEndpoint = &_usbDeviceEndpoint[endpoint];
    UINT8 error;

    error = _usb_device_recv_data(_usbDeviceHandle, pEndpoint->udcEndpoint, pEndpoint->pRxBuffer, pEndpoint->pRxBuffSize);
    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_RX, "%s: perform rcv preparation ep#=%d\n", __FUNCTION__, endpoint);
    if (error != USB_OK)
    {/* error result */
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ERROR, "%s: failed in _usb_device_recv_data, error=%d\n", __FUNCTION__, (int)error);
        return USB_DEVICE_RC_ERROR;
    }

    return USB_DEVICE_RC_OK;
} /* End of < USB2DeviceEndpointReceiveCompleted > */

/*******************************************************************************
 * Function:    USB2DeviceEndpointReceiveCompletedExt
 *******************************************************************************
 * Description: Prepare an endpoint for receive
 *
 * Parameters:  USBDevice_EndpointE          endpoint
 *
 * Output:      UINT32 next_packet_expected_length
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
USBCDevice_ReturnCodeE USB2DeviceEndpointReceiveCompletedExt(USBDevice_EndpointE   endpoint, UINT8  *pRxBuffer,UINT32 next_packet_expected_length,BOOL expect_zlp)
{
    USBDevice_EndpointDataS *pEndpoint = &_usbDeviceEndpoint[endpoint];
    UINT8 error;

    error = _usb_device_recv_data(_usbDeviceHandle, pEndpoint->udcEndpoint, CACHED_TO_NON_CACHED(pRxBuffer), next_packet_expected_length);
    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_RX, "%s: perform rcv preparation ep#=%d, expected len=%d, buff=0x%x\n", __FUNCTION__, endpoint, next_packet_expected_length, (int)pRxBuffer);

    if (error != USB_OK)
    {/* error result */
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ERROR, "%s: failed in _usb_device_recv_data, error=%d\n", __FUNCTION__, (int)error);
        return USB_DEVICE_RC_ERROR;
    }

    return USB_DEVICE_RC_OK;
} /* End of < USB2DeviceEndpointReceiveCompleted > */


static volatile UINT32 ci2D0TurnOns = 0;
static volatile UINT32 ci2ACCR1TurnOns = 0;

static void USB2DeviceEnableAppSubSysClocks(void)
{
	/*Fix coverity[uninit_use]*/
    return;
}   //end usbEnableClocks()

#define	USB2_BASE	0xD4207000
//#define USB2_PLL_CTRL_REG0	(USB2_BASE)
//#define USB2_PLL_CTRL_REG1	(USB2_BASE+0x04)
#define USB2_TX_CTRL_REG0	(USB2_BASE+0x0C)
#define USB2_RX_CTRL_REG0	(USB2_BASE+0x14)
#define PHY_OTG_CTRL		(USB2_BASE+0x34)

static void USB2DeviceEnablePhy(void)
{
    *(volatile long *)0xd4207104= 0x10901003;
    *(volatile long *) 0xd4207004=0x5af0;
    *(volatile long *) 0xd4207008=0x3233;
    //*(volatile long *) 0xd4207010=0x0288;
    *(volatile long *) 0xd4207010=0x0188;
    //*(volatile long *) 0xd4207014=0x07c4;
    *(volatile long *) 0xd4207014=0x07b4;
    //*(volatile long *) 0xd4207018=0x0eff;
    *(volatile long *) 0xd4207018=0x0aff;
    *(volatile long *) 0xd4207020=0xaaa1;
    *(volatile long *) 0xd4207024=0x3892;
    *(volatile long *) 0xd4207028=0x0125;
    *(volatile long *) 0xd4207030=0x0110;
    *(volatile long *) 0xd4207034=0x5680;
    *(volatile long *) 0xd420703c=0x2586;
    *(volatile long *) 0xd4207040=0xe400;
    *(volatile long *) 0xd4207058=0x0;
    *(volatile long *) 0xd420705c=0x8;
    *(volatile long *) 0xd4207078=0x9011;
    *(volatile long *) 0xd420707c=0x0;
}

#ifdef NEZHA3_1826
/*
 *
 *	THE 16-bit MMP3 REGISTER DEFINES
 *
 */
#define USB2_PLL_CTRL_REG0		(USB2_BASE+0x00)
#define USB2_PLL_CTRL_REG1		(USB2_BASE+0x04)
#define USB2_CAL_CTRL			(USB2_BASE+0x08)
#define USB2_TX_CH_CTRL0		(USB2_BASE+0x0C)
#define USB2_TX_CH_CTRL1		(USB2_BASE+0x10)
#define USB2_RX_CH_CTRL0		(USB2_BASE+0x14)
#define USB2_RX_CH_CTRL1		(USB2_BASE+0x18)
#define USB2_DIG_CTRL0			(USB2_BASE+0x1C)
#define USB2_DIG_CTRL1			(USB2_BASE+0x20)
#define USB2_TEST_CTRL_STATUS0	(USB2_BASE+0x24)
#define USB2_TEST_CTRL_STATUS1	(USB2_BASE+0x28)
#define USB2_MON_REG			(USB2_BASE+0x2C)
#define USB2_PHY_RESERVE		(USB2_BASE+0x30)
#define USB2_PHY_OTG_CTRL		(USB2_BASE+0x34)
#define USB2_PHY_CHRG_DET_CTRL	(USB2_BASE+0x38)


/*
 *
 *	THE BIT DEFINES
 *
 */

/*		USB2_PLL_CTRL_REG0	0x00  	*/
#define USB2_PLL_REFDIV_BASE 			0
#define USB2_PLL_REFDIV_MASK 			SHIFT0(0x7F)
#define USB2_PLL_FBDIV_BASE				16
#define USB2_PLL_FBDIV_MASK				SHIFT16(0x1FF)
#define USB2_PLL_SELLPFR_BASE			28
#define USB2_PLL_SELLPFR_MASK			SHIFT28(0x3)
#define USB2_PLL_PLLREADY_BASE			31
#define USB2_PLL_PLLREADY_MASK			SHIFT31(0x1)

/*		USB2_PLL_CTRL_REG1	0x04  	*/
#define USB2_PLL_PUPLL_BASE 			0
#define USB2_PLL_PUPLL_MASK 			SHIFT0(0x1)
#define USB2_PLL_CTRLPUBYREG_BASE 		1
#define USB2_PLL_CTRLPUBYREG_MASK 		SHIFT1(0x1)

/*		USB2_PHY_OTG_CTRL	0x34  	*/
#define USB2_PLL_PUOTG_BASE 			4
#define USB2_PLL_PUOTG_MASK 			SHIFT4(0x1)
#define USB2_PLL_OTGCONTROLBYPIN_BASE 	5
#define USB2_PLL_OTGCONTROLBYPIN_MASK 	SHIFT5(0x1)

/*		USB2_TX_CH_CTRL0	0x0C  	*/
#define USB2_PLL_PUANA_BASE 			24
#define USB2_PLL_PUANA_MASK 			SHIFT24(0x1)
#define USB2_PLL_PUBYREG_BASE 			25
#define USB2_PLL_PUBYREG_MASK		 	SHIFT25(0x1)

/*		USB2_CAL_CTRL		0x08  	*/
#define USB2_PLL_IMPCALDONE_BASE 		23
#define USB2_PLL_IMPCALDONE_MASK 		SHIFT23(0x1)
#define USB2_PLL_PLLCALDONE_BASE		31
#define USB2_PLL_PLLCALDONE_MASK	 	SHIFT31(0x1)

/*		USB2_RX_CH_CTRL1		0x18  	*/
#define USB2_PLL_SQCALDONE_BASE			31
#define USB2_PLL_SQCALDONE_MASK	 		SHIFT31(0x1)
#define USB2_PLL_READY_MASK				SHIFT15(0x1)

UINT32 OSCR0IntervalInMicro_self(UINT32 Before, UINT32 After)
{
	UINT32 temp = (After - Before);

	return (temp / (30*1000));
}

void EnablePhy28_Nezha3(void)
{
	unsigned int uiReg, startTime, endTime;
	volatile  UINT32 temp1 = 0x0, temp2 = 0x0;

    /* Initialize the USB2 PHY */
	// Config PLL using 26Mhz ref clock. 25Mhz not supported on TTD2.
	// PLL_CTRL0(offset: 0x0), set SEL_LPFR(bit[29:28]) to 0x1; set FBDIV(bit[24:16]) to 0xF0; set REFDIV(bit[6:0]) to 0xD.

	BU_REG_WRITE( USB2_PLL_CTRL_REG0, BU_REG_READ(USB2_PLL_CTRL_REG0) & ~(USB2_PLL_SELLPFR_MASK |
																		  USB2_PLL_FBDIV_MASK |
																		  USB2_PLL_REFDIV_MASK) );
	BU_REG_WRITE( USB2_PLL_CTRL_REG0, BU_REG_READ(USB2_PLL_CTRL_REG0) |    (0x1<<USB2_PLL_SELLPFR_BASE |
	                                                                   		0xf0<<USB2_PLL_FBDIV_BASE |
	                                                                   		0xd<<USB2_PLL_REFDIV_BASE) );
	BU_REG_WRITE(USB2_DIG_CTRL0, BU_REG_READ(USB2_DIG_CTRL0) | (0x80)); //PLL_LOCK_BYPASS

	// USB2_PLL_CTRL_REG1
	BU_REG_WRITE( USB2_PLL_CTRL_REG1, BU_REG_READ(USB2_PLL_CTRL_REG1) | (0x1<<USB2_PLL_PUPLL_BASE |
		 									  				   			 0x1<<USB2_PLL_CTRLPUBYREG_BASE) );

	// USB2_PHY_OTG_CTRL
	BU_REG_WRITE( USB2_PHY_OTG_CTRL, BU_REG_READ(USB2_PHY_OTG_CTRL) | (0x1<<USB2_PLL_PUOTG_BASE) );
	BU_REG_WRITE( USB2_PHY_OTG_CTRL, BU_REG_READ(USB2_PHY_OTG_CTRL) & ~(USB2_PLL_OTGCONTROLBYPIN_MASK) );

	// USB2_TX_CH_CTRL0
	BU_REG_WRITE( USB2_TX_CH_CTRL0, BU_REG_READ(USB2_TX_CH_CTRL0) | (0x1<<USB2_PLL_PUANA_BASE |
		 									  				   		 0x1<<USB2_PLL_PUBYREG_BASE) );
	//Steel's recommendation:
	// USB2_RX_CTRL_REG0
    BU_REG_WRITE(USB2_RX_CH_CTRL0, BU_REG_READ(USB2_RX_CH_CTRL0) & (~0xF));
    BU_REG_WRITE(USB2_RX_CH_CTRL0, BU_REG_READ(USB2_RX_CH_CTRL0) | (0xa));                    // Set SQ_THRESH = 0xA

      //USB2_DIG_REG0
    BU_REG_WRITE( USB2_DIG_CTRL0, BU_REG_READ(USB2_DIG_CTRL0) & (~(0x7<<16)) );
    BU_REG_WRITE( USB2_DIG_CTRL0, BU_REG_READ(USB2_DIG_CTRL0) & (~(0x7<<12)) );
    BU_REG_WRITE( USB2_DIG_CTRL0, BU_REG_READ(USB2_DIG_CTRL0) & (~(0x3)) );
    BU_REG_WRITE( USB2_DIG_CTRL0, BU_REG_READ(USB2_DIG_CTRL0) | (0x7<<16) );
    BU_REG_WRITE( USB2_DIG_CTRL0, BU_REG_READ(USB2_DIG_CTRL0) | (0x4<<12) );
    BU_REG_WRITE( USB2_DIG_CTRL0, BU_REG_READ(USB2_DIG_CTRL0) | (0x2) );

	// Check for CALIBRATION complete. Timeout after 1 ms.
	startTime = timerCountRead(2);//GetOSCR0();
  	do
	{
		temp1 = BU_REG_READ(USB2_CAL_CTRL);
		temp2 = BU_REG_READ(USB2_RX_CH_CTRL1);

		// Check and wait for PLLCAL_DONE=1, IMPCAL_DONE=1, SQCAL_DONE=1 in calibration control registers.
    	if ( ( (temp1 & (USB2_PLL_IMPCALDONE_MASK | USB2_PLL_PLLCALDONE_MASK)) == (USB2_PLL_IMPCALDONE_MASK | USB2_PLL_PLLCALDONE_MASK) ) &&
    		 ( (temp2 & USB2_PLL_SQCALDONE_MASK) > 0 ) )
		{
	    	break;
		}
	} while( OSCR0IntervalInMicro_self(startTime, timerCountRead(2)) < 1000 );	// Timeout after 1 ms.
}
//#else

//Define OTG PHY for Nezha2/Eden (28nm)
#define USB2_PLL_CTRL0_28		0x0
#define USB2_PLL_CTRL1_28		0x4
#define USB2_CAL_CTRL_28		0x8
#define USB2_TX_CTRL0_28		0xC
#define USB2_TX_CTRL1_28		0x10
#define USB2_RX_CTRL0_28		0x14
#define USB2_RX_CTRL1_28		0x18
#define USB2_DIG_CTRL0_28		0x1C
#define USB2_DIG_CTRL1_28		0x20
#define USB2_TEST_CTRL0_28	0x24
#define USB2_TEST_CTRL1_28	0x28
#define USB2_PHYMON_28		0x2C
#define USB2_PHY_RSVD_28		0x30
#define USB2_OTG_CTRL_28		0x34
#define USB2_CHARGER_CTRL_28	0x38
#define USB2_CTRL0_28			0xC4
#define USB2_CTRL1_28			0xC8
#define USB2_CTRL2_28			0xD4
#define USB2_CTRL3_28			0xDC

#define USB2_PLL0_READY_SHIFT_28			31
#define USB2_PLL0_R_ROTATE_SHIFT_28		30
#define USB2_PLL0_SEL_LPFR_SHIFT_28		28
#define USB2_PLL0_FBDIV_SHIFT_28			16
#define USB2_PLL0_VDDL_SHIFT_28			14
#define USB2_PLL0_VDDM_SHIFT_28			12
#define USB2_PLL0_ICP_MASK_28			(0x7<<8)
#define USB2_PLL0_ICP_SHIFT_28			8
#define USB2_PLL0_REFDIV_SHIFT_28		0

#define USB2_PLL1_PU_BY_REG_SHIFT_28	1
#define USB2_PLL1_PU_PLL_SHIFT_28		0

#define USB2_CALIB_PLLCAL_DONE_SHIFT_28 31
#define USB2_CALIB_IMPCAL_DONE_SHIFT_28 23
#define USB2_CALIB_PLLCAL_START_SHIFT_28 22
#define USB2_CALIB_PLLCAL_SHIFT_28		20
#define USB2_CALIB_KVCO_SHIFT_28		16
#define USB2_CALIB_IMPCAL_START_SHIFT_28 13
#define USB2_CALIB_IMPCAL_VTH_SHIFT_28	8

#define USB2_TX0_PU_BY_REG_SHIFT_28		25
#define USB2_TX0_PU_ANA_SHIFT_28		24
#define USB2_TX0_AMP_SHIFT_28			20
#define USB2_TX0_LOWVDD_EN_SHIFT_28		6
#define USB2_TX0_HS_SR_SEL_SHIFT_28		4
#define USB2_TX0_HSDRV_EN_SHIFT_28		0

#define USB2_RX0_TESTMON_SHIFT_28		20
#define USB2_RX0_SQ_DLY_SEL_SHIFT_28	12
#define USB2_RX0_SQ_THRESH_SHIFT_28	    0

#define USB2_RX1_SQCAL_START_SHIFT_28	4
#define USB2_RX1_SQ_AMP_CAL_EN_SHIFT_28	3
#define USB2_RX1_SQ_AMP_CAL_SHIFT_28	0

#define USB2_DIG0_SQ_FILT_SHIFT_28		16
#define USB2_DIG0_SQ_BLK_SHIFT_28		12
#define USB2_DIG0_SYNC_NUM_SHIFT_28		0

#define USB2_MON_USB_MON_SHIFT_28		16
#define USB2_MON_REG_MON_SEL_SHIFT_28	0

#define USB2_OTG_CTRL_BY_PIN_SHIFT_28	5
#define USB2_OTG_PU_OTG_SHIFT_28		4
#define USB2_OTG_TESTMON_SHIFT_28		0

#define USB2_CHGDTC_CTRL_ENABLE_SWITCH_DM_SHIFT_28 13
#define USB2_CHGDTC_CTRL_ENABLE_SWITCH_DP_SHIFT_28 12
#define USB2_CHGDTC_CTRL_VSRC_CHARGE_SHIFT_28	10
#define USB2_CHGDTC_CTRL_VDAT_CHARGE_SHIFT_28	8
#define USB2_CHGDTC_CTRL_CDP_DM_AUTO_SWITCH_SHIFT_28 7
#define USB2_CHGDTC_CTRL_DP_DM_SWAP_SHIFT_28	6
#define USB2_CHGDTC_CTRL_PU_CHRG_DTC_SHIFT_28	5
#define USB2_CHGDTC_CTRL_PD_EN_SHIFT_28			4
#define USB2_CHGDTC_CTRL_DCP_EN_SHIFT_28		3
#define USB2_CHGDTC_CTRL_CDP_EN_SHIFT_28		2
#define USB2_CHGDTC_CTRL_TESTMON_CHRGDTC_SHIFT_28 0

#define USB2_USB_CTRL1_CHRG_DTC_OUT_SHIFT_28	4
#define USB2_USB_CTRL1_VBUSDTC_OUT_SHIFT_28		2

extern int uart_printf(const char* fmt, ...);
void PauseMsec(unsigned long i)
{
	unsigned long j = 0;
	unsigned long temp = 1024*1024*i;
	for(j=0;j<temp;j++);
}

void EnablePhy28(void)
{
	unsigned int uiDelay;

	{
		uart_printf("EnablePhy28, Wait until VBus is on......\r\n");
		while (1)
		{
			// Poll for bit 2 of the USB PHY USB_CTRL1 register
			if ((*(volatile unsigned long*)(USB2_BASE + USB2_CTRL1_28)) & (1<<USB2_USB_CTRL1_VBUSDTC_OUT_SHIFT_28))
				break;
		}
	}

	*(unsigned long*)(USB2_BASE + USB2_PLL_CTRL0_28) &= ~((0x3<<USB2_PLL0_SEL_LPFR_SHIFT_28) |
												  (0x1ff<<USB2_PLL0_FBDIV_SHIFT_28) |
												  //(0x3<<USB2_PLL0_VDDL_SHIFT_28) |
												  //(0x3<<USB2_PLL0_VDDM_SHIFT_28) |
												  (0xf<<USB2_PLL0_ICP_SHIFT_28) |
												  (0x7f<<USB2_PLL0_REFDIV_SHIFT_28) | 3<<14);

	*(unsigned long*)(USB2_BASE + USB2_PLL_CTRL0_28) |= (0x1<<USB2_PLL0_SEL_LPFR_SHIFT_28 |	// 26MHz/0xd = 2 -> SEL_LPFR = 1
										  0xf0<<USB2_PLL0_FBDIV_SHIFT_28 |
										  //0x1<<USB2_PLL0_VDDL_SHIFT_28 |
										  //0x1<<USB2_PLL0_VDDM_SHIFT_28 |
										  0x3<<USB2_PLL0_ICP_SHIFT_28 |
										  0xd<<USB2_PLL0_REFDIV_SHIFT_28 | 1<<14);	// 26MHz*0xf0/0xd = 480MHz

	// USB2_PLL_REG1: Power up PHY PLL
	*(unsigned long*)(USB2_BASE + USB2_PLL_CTRL1_28) |= (1<<USB2_PLL1_PU_BY_REG_SHIFT_28 |
												 1<<USB2_PLL1_PU_PLL_SHIFT_28);

	//USB2 Calibration Control Register USB2_CAL_CTRL_28
	//USB_LOG_INFO("USB2_CAL_CTRL_28 @0x%x = 0x%x",(USB2_BASE + USB2_CAL_CTRL_28), *(vpWORD)(USB2_BASE + USB2_CAL_CTRL_28));
	*(unsigned long*)(USB2_BASE + USB2_CAL_CTRL_28) &= ~((0x3<<USB2_CALIB_PLLCAL_SHIFT_28) |
												 (0x7<<USB2_CALIB_KVCO_SHIFT_28) |
												 (0x7<<USB2_CALIB_IMPCAL_VTH_SHIFT_28));

	*(unsigned long*)(USB2_BASE + USB2_CAL_CTRL_28) |= (0x1<<USB2_CALIB_PLLCAL_SHIFT_28	|		// Start with default value of 1
												0x3<<USB2_CALIB_KVCO_SHIFT_28 |			// Start with default value of 3
												0x4<<USB2_CALIB_IMPCAL_VTH_SHIFT_28); 	// Start with default value of 4

	//USB2 TX Control Register 0
	*(unsigned long*)(USB2_BASE + USB2_TX_CTRL0_28) &= ~(0x7<<USB2_TX0_AMP_SHIFT_28);
	*(unsigned long*)(USB2_BASE + USB2_TX_CTRL0_28) |= (1<<USB2_TX0_PU_BY_REG_SHIFT_28 |
												1<<USB2_TX0_PU_ANA_SHIFT_28 |
												0x3<<USB2_TX0_AMP_SHIFT_28);
    *(volatile unsigned long*)(0xd420700c) |= 0x30; //shanan advise

	//USB2 RX Channel Control Register 0
	*(unsigned long*)(USB2_BASE + USB2_RX_CTRL0_28) &= ~(0xf<<USB2_RX0_SQ_THRESH_SHIFT_28);
	*(unsigned long*)(USB2_BASE + USB2_RX_CTRL0_28) |= (0xa<<USB2_RX0_SQ_THRESH_SHIFT_28);

	//USB2 Digital Control Register 0
	//Program SQ_FILT, SQ_BLK and SYNC_NUM because the "bad cable" issue
	*(unsigned long*)(USB2_BASE + USB2_DIG_CTRL0_28) &= ~((0x7<<USB2_DIG0_SQ_FILT_SHIFT_28) |
												  (0x7<<USB2_DIG0_SQ_BLK_SHIFT_28) |
												  (0x3<<USB2_DIG0_SYNC_NUM_SHIFT_28));
	*(unsigned long*)(USB2_BASE + USB2_DIG_CTRL0_28) |= ((7<<USB2_DIG0_SQ_FILT_SHIFT_28) |
												 (4<<USB2_DIG0_SQ_BLK_SHIFT_28) |
												 (2<<USB2_DIG0_SYNC_NUM_SHIFT_28));
    *(volatile unsigned long*)(0xd420701c) |= 0x70000;//shanan advise
    *(volatile unsigned long*)(0xd420701c) |= 0x4000;//shanan advise
	*(volatile unsigned long*)(0xd420701c) &= 0xffffcfff;//shanan advise
	*(volatile unsigned long*)(0xd420701c) |= 0x0001;//shanan advise
	*(volatile unsigned long*)(0xd420701c) &= 0xfffffffe;//shanan advise

	*(volatile unsigned long*)(0xd420701c) |= 0x0080;//shanan advise
	*(volatile unsigned long*)(0xd4207000) &= 0xfffff0ff;//shanan advise
	*(volatile unsigned long*)(0xd4207000) |= 0x00000200;//shanan advise

	//USB2 OTG Control Register
	*(unsigned long*)(USB2_BASE + USB2_OTG_CTRL_28) &= ~(1<<USB2_OTG_CTRL_BY_PIN_SHIFT_28);		//Make sure this bit is cleared
	*(unsigned long*)(USB2_BASE + USB2_OTG_CTRL_28) |= 1<<USB2_OTG_PU_OTG_SHIFT_28;

	// If PLLCAL not done yet, do the calibration
	if ((*(unsigned long*)(USB2_BASE + USB2_CAL_CTRL_28) & (1u<<USB2_CALIB_PLLCAL_DONE_SHIFT_28)) == 0)
	{
		PauseMsec(1);
		*(unsigned long*)(USB2_BASE + USB2_CAL_CTRL_28) |= 1<<USB2_CALIB_PLLCAL_START_SHIFT_28; // Set PLLCAL_START bit to 1
		PauseMsec(1);
		// Make sure PHY PLL is ready
		uiDelay = 0;
	  	while((*(unsigned long*)(USB2_BASE + USB2_CAL_CTRL_28) & (1u<<USB2_CALIB_PLLCAL_DONE_SHIFT_28)) == 0)
		{
			PauseMsec(1);
			uiDelay++;
			if (uiDelay > 100)
			{
				break;
			}
		}
		*(unsigned long*)(USB2_BASE + USB2_CAL_CTRL_28) &= ~(1<<USB2_CALIB_PLLCAL_START_SHIFT_28); // Clear PLLCAL_START bit
	}
	// Impedance Calibration
	if (*(unsigned long*)(USB2_BASE + USB2_CAL_CTRL_28) & (1<<USB2_CALIB_IMPCAL_START_SHIFT_28))
	{
		// Make sure we have a 0->1 transition
		*(unsigned long*)(USB2_BASE + USB2_CAL_CTRL_28) &= ~(1<<USB2_CALIB_IMPCAL_START_SHIFT_28);
		PauseMsec(1);
	}
	*(unsigned long*)(USB2_BASE + USB2_CAL_CTRL_28) |= 1<<USB2_CALIB_IMPCAL_START_SHIFT_28; // Set IMPCAL_START bit to 1
	PauseMsec(1);
	uiDelay = 0;
  	while((*(unsigned long*)(USB2_BASE + USB2_CAL_CTRL_28) & (1<<USB2_CALIB_IMPCAL_DONE_SHIFT_28)) == 0)
	{
		PauseMsec(1);
		uiDelay++;
		if (uiDelay > 100)
		{
			break;
		}
	}
	*(unsigned long*)(USB2_BASE + USB2_CAL_CTRL_28) &= ~(1<<USB2_CALIB_IMPCAL_START_SHIFT_28); // Clear IMPCAL_START bit back to 0
}
#endif

/* USB PHY for Crane, same settings as NezhaC */
#define	USB2_PHYBASE	        (0xD4207000)

/*	PHY REGS */
#define USB2_PHY_REG00	        (USB2_PHYBASE+0x00)
#define USB2_PHY_REG01	        (USB2_PHYBASE+0x04)
#define USB2_PHY_REG02	        (USB2_PHYBASE+0x08)
#define USB2_PHY_REG03	        (USB2_PHYBASE+0x0C)
#define USB2_PHY_REG04	        (USB2_PHYBASE+0x10)
#define USB2_PHY_REG05	        (USB2_PHYBASE+0x14)
#define USB2_PHY_REG06	        (USB2_PHYBASE+0x18)
#define USB2_PHY_REG07	        (USB2_PHYBASE+0x1C)
#define USB2_PHY_REG08	        (USB2_PHYBASE+0x20)
#define USB2_PHY_REG09	        (USB2_PHYBASE+0x24)
#define USB2_PHY_REG0A	        (USB2_PHYBASE+0x28)
#define USB2_PHY_REG0B	        (USB2_PHYBASE+0x2C)
#define USB2_PHY_REG0C	        (USB2_PHYBASE+0x30)
#define USB2_PHY_REG0D	        (USB2_PHYBASE+0x34)
#define USB2_PHY_REG28	        (USB2_PHYBASE+0xA0)
#define USB2_PHY_REG29	        (USB2_PHYBASE+0xA4)




#define USB2_PLL_BIT_RDY		(0x1 << 0)
#define USB2_CFG_HS_SRCS_SEL	(0x1 << 0)
#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)
#define BIT_2 (1 << 2)
#define BIT_3 (1 << 3)
#define BIT_4 (1 << 4)
#define BIT_5 (1 << 5)
#define BIT_6 (1 << 6)
#define BIT_7 (1 << 7)
#define BIT_8 (1 << 8)
#define BIT_9 (1 << 9)
#define BIT_10 (1 << 10)
#define BIT_11 (1 << 11)
#define BIT_12 (1 << 12)
#define BIT_13 (1 << 13)
#define BIT_14 (1 << 14)
#define BIT_15 (1 << 15)
#define BIT_16 (1 << 16)
#define BIT_17 (1 << 17)
#define BIT_18 (1 << 18)
#define BIT_19 (1 << 19)
#define BIT_20 (1 << 20)
#define BIT_21 (1 << 21)
#define BIT_22 (1 << 22)
#define BIT_23 (1 << 23)
#define BIT_24 (1 << 24)
#define BIT_25 (1 << 25)
#define BIT_26 (1 << 26)
#define BIT_27 (1 << 27)
#define BIT_28 (1 << 28)
#define BIT_29 (1 << 29)
#define BIT_30 (1 << 30)
#define BIT_31 ((unsigned)1 << 31)

UINT32 phyregStore1[25] = {0};
UINT32 phyregStore2[25] = {0};
UINT32 phyregStore3 = 0;
UINT32 phyregStorefinish = 0;

void phyreg_dump_before_reenum(void)
{
    UINT32 i = 0;
	UINT32 tmp = 0;
    if(*(volatile unsigned long*)0xd4207038 & 0xfe)
    {
        for(i=0;i<25;i++)
        {
            tmp = *(volatile unsigned long*)0xd4207010;
            tmp = tmp & ~(0xff<<8);
            tmp = tmp | (i << 8);
            *(volatile unsigned long*)0xd4207010 = tmp; 
            phyregStore1[i] = *(volatile unsigned long*)0xd4207010;
            phyregStore2[i] = *(volatile unsigned long*)0xd42070cc;
        }  
        phyregStore3 = *(volatile unsigned long*)0xd4207038;
        *(volatile unsigned long*)0xd4207038 = 0xff;   
        phyregStorefinish = 1;
    }
}

void phyreg_dump_print(void)
{
    UINT32 i = 0;
    if(phyregStorefinish)
    {
        fatal_printf("0xd4207038:0x%lx\r\n",phyregStore3);
        for(i=0;i<25;i++)
        {
            fatal_printf("0xd4207010:0x%lx, 0xd42070cc:0x%lx\r\n",phyregStore1[i],phyregStore2[i]);
        }
        phyregStorefinish = 0; 
    }
}
void phyreg_dump(void)
{
    static UINT32 i = 0;
	UINT32 tmp = 0;
    if(*(volatile unsigned long*)0xd4207038 & 0xfe)
    {
        fatal_printf("0xd4207004:0x%lx,0xd420700c:0x%lx,0xd4207018:0x%lx\r\n", *(volatile unsigned long*)0xd4207004,*(volatile unsigned long*)0xd420700c,*(volatile unsigned long*)0xd4207018);
        for(i=0;i<25;i++)
        {
            tmp = *(volatile unsigned long*)0xd4207010;
            tmp = tmp & ~(0xff<<8);
            tmp = tmp | (i << 8);
            *(volatile unsigned long*)0xd4207010 = tmp;
            uart_printf("0xd4207010:0x%lx, 0xd42070cc:0x%lx\r\n",*(volatile unsigned long*)0xd4207010,*(volatile unsigned long*)0xd42070cc);
        }  
        
        fatal_printf("phyerr:0x%lx\r\n",*(volatile unsigned long*)0xd4207038);

        *(volatile unsigned long*)0xd4207038 = 0xff;      
    }

}

UINT8 usb_phy_mode_fuse_get(void)
{
    UINT32 geu_fuse_val;
    *((volatile UINT32 *)0xd4282868) = 0x09;
    geu_fuse_val = *(volatile UINT32 *)0xd420140c;
    *((volatile UINT32 *)0xd4282868) = 0x0;

    /*Bank0[92](USB PHY mode): 
    1 -- serial mode; 0 -- parallel mode */
    return (geu_fuse_val>>12)&0x1;
}

static void USB2DeviceEnablePHY_Crane(void)
{
    volatile UINT32 j = 0, k = 0;
    UINT32 pll_wait_us = 200;
    UINT32 reg = 0;

    *(volatile unsigned long *)PMUA_USB_CLK_RES_CTRL |= BIT_3; //PMUA_USB_CLK_RES_CTRL_USB_AXICLK_EN;
    *(volatile unsigned long *)PMUA_USB_CLK_RES_CTRL |= BIT_0; //PMUA_USB_CLK_RES_CTRL_USB_AXI_RST;

    /* Make some delay */
    for(j=0; j<100; j++)
    {
        k++;
    }

    //WAIT FOR PHY PLL RDY
    while (((BU_REG_READ(USB2_PHY_REG01) & USB2_PLL_BIT_RDY) != USB2_PLL_BIT_RDY)
            && (pll_wait_us--))
    {
        /* Make some delay */
        for(j=0; j<100; j++)
        {
            k++;
        }
    }
    if(CHIP_IS_CRANE)
    {
        *(volatile unsigned long *)USB2_PHY_REG28 |= 0x3<<12; //set 0xd42070a0[13:12]=0b11
    }
    //Release usb2 phy internal reset and enable clock gating
    BU_REG_WRITE(USB2_PHY_REG01, 0x60ef);
    BU_REG_WRITE(USB2_PHY_REG0D, 0x1C);
    if(CHIP_IS_CRANEG | CHIP_IS_CRANEM)
    {
        if(usb_phy_mode_fuse_get()) 
        {
            *(volatile unsigned long*)0xd4207018 |= 0x1; //serial mode
            fatal_printf("0xd4207018:0x%lx\r\n",*(volatile unsigned long*)0xd4207018);
        }
        else   
        {   
            *(volatile unsigned long*)0xd4207018 &= ~0x1; //parallel mode
            fatal_printf("0xd4207018:0x%lx\r\n",*(volatile unsigned long*)0xd4207018);
        }
        reg = *(volatile unsigned long*)USB2_PHY_REG29;
        *(volatile unsigned long*)USB2_PHY_REG29 = (reg & ~(0x1f)) | 0x1B;
        fatal_printf("USB2_PHY_REG29:0x%lx\r\n",*(volatile unsigned long*)USB2_PHY_REG29);
    }
    else
    {
        *(volatile unsigned long*)(0xD420708C) |= 0x7<<12; //added on 20200508 to optimize usb connction
 	    *(volatile unsigned long*)(0xD4207088) &= ~(0x1<<6); //added on 20200623 to optimize usb connction
 	    fatal_printf("USB2DeviceEnablePHY_Crane optimize\r\n");
 	}

}

static void USB2DeviceHWInit(void)
{
    UINT32          regVal;
    static          UINT8 firstEntrance = 0;

    if ( firstEntrance )
        return;

#if 1 //qianying
    USB2DeviceEnablePHY_Crane();
#else
#ifndef PHS_SW_DEMO_TTC
    // Enable Firewall (RDH bit)
    regVal = USB2_REG_READ(BPMU_ASCR_ADDR);
	regVal = regVal | 0x80000000; // if we write   "regVal |= (UINT32)(1<<31);" we get warning while compilation.
    USB2_REG_WRITE(BPMU_ASCR_ADDR, regVal) ;
#else

	//Hongji add clock setting 201011

	USB2_REG_WRITE(PMUA_USB_CLK_RES_CTRL,0x08);
	USB2_REG_WRITE(PMUA_USB_CLK_RES_CTRL,0x09);

//End 20081006
//PU_REF=1;
#if !defined(EDEN_1928) && !defined(NEZHA3_1826)
	regVal=1<<20;
	USB2_REG_WRITE(USB_SPH_MISC_CR,regVal);
#endif
#endif
    // Enable Application SubSystem Clock for USB2 CI OTG
    USB2DeviceEnableAppSubSysClocks();

#endif
    firstEntrance = 1;
}

void USB2DeviceEnableUsbInterrupt(void)
{
    INTCEnable(INTC_SRC_USB2CI_OTG);
}
void USB2DeviceDisableUsbInterrupt(void)
{
    INTCDisable(INTC_SRC_USB2CI_OTG);
}

static UINT8 USB2DeviceInitEps(void* handle)
{
    UINT32 epMaxPacketSize;
    UDC_EndpointE       udcEndpoint;
    UINT8 error = USB_OK, epNum, epDir, epType = MV_USB_BULK_ENDPOINT, epFlag;

    if( USB2_IS_FULL_SPEED())
    {
        udcDefaultEndpointCfg = _u1dcDefaultEndpointConfig;
    }
    else
    {
        udcDefaultEndpointCfg = _u2dcDefaultEndpointConfig;
    }

    for(udcEndpoint = UDC_ENDPOINT_A; udcEndpoint < USB_DEVICE_TOTAL_ENDPOINTS; udcEndpoint++)
    {
        if( _usbDeviceEndpoint[udcEndpoint].endpointOpen)
        {
            epNum = udcDefaultEndpointCfg[udcEndpoint].usbEndpointNum;
            epDir = udcDefaultEndpointCfg[udcEndpoint].usbEndpointDir;
            epMaxPacketSize = udcDefaultEndpointCfg[udcEndpoint].usbMaxPacketSize;

            epFlag = ( _usbDeviceEndpoint[udcEndpoint].zlp ? !MV_USB_DEVICE_DONT_ZERO_TERMINATE : MV_USB_DEVICE_DONT_ZERO_TERMINATE);

            error = _usb_device_init_endpoint (_usbDeviceHandle, epNum, epMaxPacketSize, epDir , epType, epFlag);
            if (error != USB_OK)
            {
                MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ERROR, "%s: failed in _usb_device_init_endpoint, error=%d\n", __FUNCTION__, (int)error);
                break;
            }
        }
    }
    return error;
}

static UINT8 USB2DeviceStopEps(void* handle)
{
    UDC_EndpointE udcEndpoint;
    UINT8  error = USB_OK, epNum, epDir;
    int   lockKey;

    lockKey = USB_lock();

	/*Fix coverity[overrun-local]*/

	for(udcEndpoint = UDC_ENDPOINT_A; udcEndpoint < USB_DEVICE_TOTAL_ENDPOINTS; udcEndpoint++)
	{
        if( _usbDeviceEndpoint[udcEndpoint].endpointOpen)
        {
            epNum = udcDefaultEndpointCfg[udcEndpoint].usbEndpointNum;
            epDir = udcDefaultEndpointCfg[udcEndpoint].usbEndpointDir;

            error = _usb_device_cancel_transfer (_usbDeviceHandle, epNum, epDir);
            _usbDeviceEndpoint[epNum].endpointBusy  = FALSE;
            if (error != USB_OK)
            {
                MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ERROR, "%s: failed in _usb_device_cancel_transfer, error=%d\n", __FUNCTION__, (int)error);
                break;
            }
    /*        error = _usb_device_deinit_endpoint(_usbDeviceHandle, epNum, epDir);
            if (error != USB_OK)
            {
                USB_DEBUG(ERR_DEINIT_EP ,error)
                break;
            }
            error = _usb_device_unregister_service(_usbDeviceHandle, epType);
            if (error != USB_OK)
            {
                USB_DEBUG(ERR_UNREG_SVC ,error)
                break;
            }      */
        }
    }
    USB_unlock(lockKey);
    return error;
}

#define USB_SETUP_BMREQUEST_INDEX                0
#define USB_SETUP_BREQUEST_INDEX                 1
#define USB_SETUP_WVALUE_INDEX                   2
#define USB_SETUP_DESCRIPTOR_INDEX_WVALUE_INDEX  2
#define USB_SETUP_DESCRIPTOR_TYPE_WVALUE_INDEX   3
#define USB_SETUP_WINDEX_INDEX                   4
#define USB_SETUP_WLENGTH_INDEX                  6

#define USB_SETUP_REQUEST_DIRECTION_MASK          0x80
#define USB_SETUP_REQUEST_DIRECTION_TO_HOST       0x80
#define USB_SETUP_REQUEST_DIRECTION_TO_DEVICE     0x00

#define USB_SETUP_REQUEST_TYPE_MASK               0x60
#define USB_SETUP_REQUEST_TYPE_STANDARD           0x00
#define USB_SETUP_REQUEST_TYPE_CLASS              0x20
#define USB_SETUP_REQUEST_TYPE_VENDOR             0x40
#define USB_SETUP_REQUEST_TYPE_RESERVED           0x60

#define USB_SETUP_REQUEST_RECIPIENT_MASK          0x1F
#define USB_SETUP_REQUEST_RECIPIENT_DEVICE        0x00
#define USB_SETUP_REQUEST_RECIPIENT_INTERFACE     0x01
#define USB_SETUP_REQUEST_RECIPIENT_ENDPOINT      0x02
#define USB_SETUP_REQUEST_RECIPIENT_OTHER         0x03

#define USB_COMM_DEVREQ_SEND_ENCAP_CMD      0x00
#define USB_COMM_DEVREQ_GET_ENCAP_RESP      0x01

#define USB_COMM_DEVREQ_SET_COMM_FEATURE    0x02
#define USB_COMM_DEVREQ_GET_COMM_FEATURE    0x03
#define USB_COMM_DEVREQ_CLEAR_COMM_FEATURE  0x04
#define   USB_COMM_FEATURE_CODE_RESERVED         0x00
#define   USB_COMM_FEATURE_CODE_ABSTRACT_STATE   0x01
#define   USB_COMM_FEATURE_CODE_COUNTRY_SETTING  0x02

#define USB_COMM_DEVREQ_SET_LINE_CODING     0x20
#define USB_COMM_DEVREQ_GET_LINE_CODING     0x21
#define   USB_COMM_DEVREQ_GET_LINE_CODING_REPLY_LEN 7

#define USB_COMM_DEVREQ_SET_LINE_CTRL_STATE 0x22
#define   USB_COMM_DEVREQ_SET_LINE_CTRL_STATE_HD_RTS_BIT 0x02
#define   USB_COMM_DEVREQ_SET_LINE_CTRL_STATE_DTR_BIT    0x01

#define USB_COMM_DEVREQ_SEND_BREAK          0x23

#define USB_COMM_DEVREQ_GET_COMM_FEATURE_REPLY_LEN    0x02

#define USB_COMM_LINE_CODE_STOPBITS_IDX 4
#define USB_COMM_LINE_CODE_PARITY_IDX   5
#define USB_COMM_LINE_CODE_DATABITS_IDX 6

#define USB_COMM_LINE_CODE_NO_PARITY      0
#define USB_COMM_LINE_CODE_ODD_PARITY     1
#define USB_COMM_LINE_CODE_EVEN_PARITY    2
#define USB_COMM_LINE_CODE_MARK_PARITY    3
#define USB_COMM_LINE_CODE_SPACE_PARITY   4

#define USB_COMM_LINE_CODE_ONE_STOP_BIT       0
#define USB_COMM_LINE_CODE_1POINT5_STOP_BITS  1
#define USB_COMM_LINE_CODE_TWO_STOP_BITS      2


#define USB_COMM_LINE_CODING_BAUD_RATE ((UINT32)115200)


#define DevFail(...)

static SETUP_STRUCT setupPacket;  /* yss - leave this variable static or global, it's on purpose, to save former request and use when setup=0  */
static USB_SetupCmdS vendorSetupPacket;

#ifndef CRANE_MCU_DONGLE
static __align(8) UINT8 usbCommDevLineCoding[USB_COMM_DEVREQ_GET_LINE_CODING_REPLY_LEN] ;
static __align(8) UINT8 usbCommDevCommFeature[USB_COMM_DEVREQ_GET_COMM_FEATURE_REPLY_LEN] ;
static __align(8) UINT8 usbSetLineCtr[10]={0xa1,0x20,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00};
static __align(8) UINT8 usbGetCommDevLineCoding[USB_COMM_DEVREQ_GET_LINE_CODING_REPLY_LEN] ;

typedef struct UsbSetupPacketFTag
{
  UINT8                    bmRequestType;
  UINT8                    bRequest;
  UINT16                   wValue;
  UINT16                   wIndex;
  UINT16                   wLength;
} UsbSetupPacketF;

 void usbCommDevInit( void )
{

  usbCommDevLineCoding[0] = (USB_COMM_LINE_CODING_BAUD_RATE >>  0) & 0xff;
  usbCommDevLineCoding[1] = (USB_COMM_LINE_CODING_BAUD_RATE >>  8) & 0xff;
  usbCommDevLineCoding[2] = (USB_COMM_LINE_CODING_BAUD_RATE >> 16) & 0xff;
  usbCommDevLineCoding[3] = (USB_COMM_LINE_CODING_BAUD_RATE >> 24) & 0xff;

  usbCommDevLineCoding[USB_COMM_LINE_CODE_STOPBITS_IDX] = USB_COMM_LINE_CODE_ONE_STOP_BIT;
  usbCommDevLineCoding[USB_COMM_LINE_CODE_PARITY_IDX]   = USB_COMM_LINE_CODE_NO_PARITY;
  usbCommDevLineCoding[USB_COMM_LINE_CODE_DATABITS_IDX] = 8; /* [5,6,7,8,16] */

  usbCommDevCommFeature[0] = 0x00; /* The endpoints on this interface will continue
                                   * to offer/accept data and multiplexing is disabled */
  usbCommDevCommFeature[1] = 0x00; /* reserved bits set to zero */
}


void *comm_handle=NULL;
//duqiu 120913
char usb_crtl_cmd_pipe[64]={0};
UINT32 ap_req_chnl=0;

volatile UINT32 hsic_pm_test_flag=0;
void usbCommDevHisr(void )
{
 char crtl_pipe_len=0;

    /* Host-> Device */
    if( vendorSetupPacket.bmRequestType == (USB_SETUP_REQUEST_DIRECTION_TO_DEVICE |
                                 USB_SETUP_REQUEST_TYPE_CLASS |
                                 USB_SETUP_REQUEST_RECIPIENT_INTERFACE ))
    {
      switch(vendorSetupPacket.bRequest)
      {
        case USB_COMM_DEVREQ_SEND_ENCAP_CMD:
          /* Not yet supported. */
          DevFail("USB_COMM_DEVREQ_SEND_ENCAP_CMD");
          //handled = TRUE;
          break;

        case USB_COMM_DEVREQ_SET_COMM_FEATURE:
          switch( vendorSetupPacket.wValue )
          {
            case USB_COMM_FEATURE_CODE_RESERVED:
              /* No-op */
              //handled = TRUE;
              break;

            case USB_COMM_FEATURE_CODE_ABSTRACT_STATE:
              /* Control Transfer with no Data phase, allow Status phase */
              //usbDclControlTransferStatusStage(USB_ENDPOINT_ZERO);
               _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;
               _usb_device_send_data(comm_handle, 0, 0, 0);
              //handled = TRUE;
              break;

            case USB_COMM_FEATURE_CODE_COUNTRY_SETTING:
              /* Only valid for devices that have a Country Selection
               * Functional Descriptor. We don't have one.
               * Fall through to error case. */
            default:
              //DevParam( packet.bRequest, packet.wValue, packet.wLength );
              /* Control Transfer failed, cannot NAK control transfers so Stall */
              _usb_device_stall_endpoint(comm_handle, 0, 0);
              break;
          }
          break;

        case USB_COMM_DEVREQ_CLEAR_COMM_FEATURE:
          /* Sets feature to default state */
          switch(vendorSetupPacket.wValue)
          {
            case USB_COMM_FEATURE_CODE_RESERVED:
              /* No-op */
             // handled = TRUE;
              break;

            case USB_COMM_FEATURE_CODE_ABSTRACT_STATE:
              /* Control Transfer with no Data phase, allow Status phase */
              //usbDclControlTransferStatusStage(USB_ENDPOINT_ZERO);
               _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;
              _usb_device_send_data(comm_handle, 0, 0, 0);
              //handled = TRUE;
              break;

            case USB_COMM_FEATURE_CODE_COUNTRY_SETTING:
              /* Only valid for devices that have a Country Selection
               * Functional Descriptor. We don't have one.
               * Fall through to error case. */
            default:
              //DevParam( packet.bRequest, packet.wValue, packet.wLength );
              /* Control Transfer failed, cannot NAK control transfers so Stall */
              //usbStackStallEndpointAddress(USB_ENDPOINT_ZERO);
               _usb_device_stall_endpoint(comm_handle, 0, 0);
              break;
          }
          break;

        case USB_COMM_DEVREQ_SET_LINE_CODING:
          /* Baud rate, start/stop bits */

          /* All we need to do is to know it's coming.  USB data arrives of
           * its own accord. */
          //usbCommExpectingLineCodingPayload = TRUE;

		//uart_printf("USB_COMM_DEVREQ_SET_LINE_CODING\r\n");
		 _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_DATA_OUT;
          _usb_device_recv_data(comm_handle, 0, &usbGetCommDevLineCoding[0], 7);




          _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;
          //memcpy(usbCommDevLineCoding,usbGetCommDevLineCoding,7);
          _usb_device_send_data(comm_handle, 0, 0, 0);
         // uart_printf("Send Ack\r\n");

          /* Control Transfer WITH Rx/OUT Data phase.  Need to hold off
           * allowing the Status phase until we've seen the requested Data
           * arrive, otherwise another device request may be sent before
           * we've dealt with this one.
           *
           * The call to usbDclControlTransferStatusStage(0); is done on
           * receipt of the data. */

          /* some devices require endpoint zero events to be enabled to
           * receive data */
         // usbDclEnableEndpointZeroEvents();
          //handled = TRUE;
          break;

        case USB_COMM_DEVREQ_SET_LINE_CTRL_STATE:


          /* Control Transfer with no Data phase, allow Status phase */
         _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;
         _usb_device_send_data(comm_handle, 0, 0, 0);
          //uart_printf("set line ctrl state over \r\n");
		  //uart_printf("USB_COMM_DEVREQ_SET_LINE_CTRL_STATE \r\n");

          break;

        case USB_COMM_DEVREQ_SEND_BREAK:
          /* Simulates 'Break' event.  This event serves no useful purpose
           * (it is ignored in the conventional UART serial port interface)
           * so we ignore it here.
           * For info, wValue contains duration in msec (but FFFF sets Break
           * State until it's repeated with Value set to 0000). No data
           * payload.
           *
           * Also it is not yet supported. */
         _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;
         _usb_device_send_data(comm_handle, 0, 0, 0);
          break;

        default:
          _usb_device_stall_endpoint(comm_handle, 0, 0);
          break;
      }
    }
    else
    /* Request for data xfr Device-> Host */
    if( vendorSetupPacket.bmRequestType == (USB_SETUP_REQUEST_DIRECTION_TO_HOST |
                                 USB_SETUP_REQUEST_TYPE_CLASS |
                                 USB_SETUP_REQUEST_RECIPIENT_INTERFACE ))
    {
      switch(vendorSetupPacket.bRequest)
      {
        case USB_COMM_DEVREQ_GET_LINE_CODING:
          if( vendorSetupPacket.wLength == USB_COMM_DEVREQ_GET_LINE_CODING_REPLY_LEN)
          {
            //uart_printf("USB_COMM_DEVREQ_GET_LINE_CODING\r\n");
             _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_DATA_IN;
            _usb_device_send_data (comm_handle, 0, usbCommDevLineCoding, 7);
              _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;
            _usb_device_recv_data(comm_handle, 0, 0, 0);
             //uart_printf("wait ack\r\n");
          }
          break;

        case USB_COMM_DEVREQ_GET_ENCAP_RESP:
          /* Not yet supported. */
          //handled = TRUE;
          DevFail("USB_COMM_FEATURE_GET_ENCAP_RESP");

          break;
        case USB_COMM_DEVREQ_GET_COMM_FEATURE:
          //usbCommDevReqGetCommFeature();
          _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_DATA_IN;
		  _usb_device_send_data (comm_handle, 0, &usbCommDevCommFeature[0], 2);

          /* Control Transfer with Tx/IN Data phase, allow Status phase
           * to happen when host ACK's the transmission */
          //usbDclControlTransferStatusStage(USB_ENDPOINT_ZERO);
          _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;
          _usb_device_recv_data(comm_handle, 0, 0, 0);
          //handled = TRUE;
          break;

        default:
          //DevParam( packet.bRequest, packet.wValue, packet.wLength );
          //usbStackStallEndpointAddress(USB_ENDPOINT_ZERO);
          _usb_device_stall_endpoint(comm_handle, 0, 0);
          break;
      }
    }
    else
    {
	  //if(AC_IS_CPONLY)
      if(1)
	  {
      DevFail("Class Device Request has invalid bmRequestType\r\n");
      _usb_device_stall_endpoint(comm_handle, 0, 0);
	  }
	  else
	  {
	     if( vendorSetupPacket.bmRequestType == (USB_SETUP_REQUEST_DIRECTION_TO_DEVICE |
	                                 USB_SETUP_REQUEST_TYPE_VENDOR |
	                                 USB_SETUP_REQUEST_RECIPIENT_ENDPOINT ))
	     {
			crtl_pipe_len = (char)(0xff&vendorSetupPacket.wLength);

			 _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_DATA_OUT;
				  _usb_device_recv_data(comm_handle, 0, (UINT8*)&usb_crtl_cmd_pipe[0], crtl_pipe_len);
				 _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;
				  //memcpy(usbCommDevLineCoding,usbGetCommDevLineCoding,7);
				  _usb_device_send_data(comm_handle, 0, 0, 0);


			ap_req_chnl=usb_crtl_cmd_pipe[3]&0x0F;


		 }
	     else if(vendorSetupPacket.bmRequestType == (USB_SETUP_REQUEST_DIRECTION_TO_HOST |
	                                 USB_SETUP_REQUEST_TYPE_VENDOR |
	                                 USB_SETUP_REQUEST_RECIPIENT_ENDPOINT ))
	     {
			 crtl_pipe_len =(char)(0xff&vendorSetupPacket.wLength);

			if(is_dev_chnl_ready(ap_req_chnl+1)==1)
			{
				usb_crtl_cmd_pipe[2]=0x11;
			}
			else
				usb_crtl_cmd_pipe[2]=0x10;

			 _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_DATA_IN;
				   _usb_device_send_data (comm_handle, 0,  (UINT8*)&usb_crtl_cmd_pipe[0], crtl_pipe_len);
				   /* Control Transfer with Tx/IN Data phase, allow Status phase
					* to happen when host ACK's the transmission */
						_usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;
				   _usb_device_recv_data(comm_handle, 0, 0, 0);


	     }
		 else
		 {

		  DevFail("Class Device Request has invalid bmRequestType\r\n");
	      _usb_device_stall_endpoint(comm_handle, 0, 0);
	     }
	  }
    }

}
#endif

volatile usbIntDebug_t usbIntDebugLog[16];
extern volatile unsigned long usbIntDebugLog_index;
UINT32 EEHandlerFlag = 0;
extern volatile UINT32 usb_enumerating;

static void USB2DeviceControlProcessIdle(void* handle, uint_8  type, boolean  setup, uint_8 direction, uint_8* buffer, uint_32 length, uint_8  error)
{
    static boolean waitForStatusAfterInStage = FALSE;
    if(USB_IS_ENUMERATED() == 0)
        usb_enumerating = 1;

    if ( setup )
    {
        _usb_device_read_setup_data(handle, MV_USB_SERVICE_EP0, (UINT8*)&setupPacket);
       // uart_dump(&setupPacket,8);
        _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_SETUP;
        _usbDeviceDatabase.ep0.ctrl_setup_buff = (UINT8*)&setupPacket;
        _usbDeviceDatabase.ep0.crtl_trasnfers_cnt++;
        _usbDeviceDatabase.ep0.ep0_int_cnt++;

        if ( length < USB2_EP0_MAX_PACKET_SIZE )
        {
            _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;
        }

        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP0, "SETUP, Length=%d Error=#%d direction=%d Value(hex): %02.2x %02.2x %02.2x %02.2x %02.2x %02.2x %02.2x %02.2x\n",
            length, error, direction, ((UINT8*)&setupPacket)[0],((UINT8*)&setupPacket)[1],((UINT8*)&setupPacket)[2],((UINT8*)&setupPacket)[3],((UINT8*)&setupPacket)[4],((UINT8*)&setupPacket)[5],((UINT8*)&setupPacket)[6],((UINT8*)&setupPacket)[7]);
    }
    else
    {
        if (length>=7)
        {
            MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP0, "NON-SETUP, Length=%d Error=#%d Direction=%d, Value(hex): %02.2x %02.2x %02.2x %02.2x %02.2x %02.2x %02.2x\n",
            length, error, direction, (buffer)[0],(buffer)[1],(buffer)[2],(buffer)[3],(buffer)[4],(buffer)[5],(buffer)[6]);
        }
        else
        {
            MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP0, "NON-SETUP, Length=%d Error=#%d Direction=%d\n", length, error, direction);
        }


    }

    if(setup)
    {
        USB_TRACE("REQUESTTYPE:%x, REQUEST:%x, value:%x, INDEX:%x, LENGTH:%x, setup:%d", setupPacket.REQUESTTYPE, setupPacket.REQUEST, setupPacket.VALUE, setupPacket.INDEX, setupPacket.LENGTH, setup);
    }

	if(EEHandlerFlag == 0){
	usbIntDebugLog[usbIntDebugLog_index].request_type = setupPacket.REQUESTTYPE;
	usbIntDebugLog[usbIntDebugLog_index].request = setupPacket.REQUEST;
		}

    switch (setupPacket.REQUESTTYPE & 0x60)
    {
        case REQ_TYPE_STANDARD:
        {
            switch (setupPacket.REQUEST)
            {
                case REQ_GET_STATUS:
                {
                    mvUsbCh9GetStatus(handle, setup, &setupPacket);
                    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP0, "STD_GETSTAT\n");
                    break;
                }

                case REQ_CLEAR_FEATURE:
                {
                    mvUsbCh9ClearFeature(handle, setup, &setupPacket);
                    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP0, "STD_CLRADD\n");
                    break;
                }

                case REQ_SET_FEATURE:
                {
                    mvUsbCh9SetFeature(handle, setup, &setupPacket);
                    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP0, "STD_SETFEA\n");
                    break;
                }

                case REQ_SET_ADDRESS:
                {
                    mvUsbCh9SetAddress(handle, setup, &setupPacket);
                    USB2DeviceInitEps(handle);

                    mvUsbModemClearDummyFlag();
                    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP0, "STD_SETADR\n");
                    break;
                }

                case REQ_GET_DESCRIPTOR:
                {
                    mvUsbCh9GetDescriptior(handle, setup, &setupPacket);
                    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP0, "STD_GETDSC\n");
                    break;
                }

                case REQ_SET_DESCRIPTOR:
                {
                    mvUsbCh9SetDescriptior(handle, setup, &setupPacket);
                    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP0, "STD_SETDSC\n");
                    break;
                }

                case REQ_GET_CONFIGURATION:
                {
                    mvUsbCh9GetConfig(handle, setup, &setupPacket);
                    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP0, "STD_GETCFG\n");
                    break;
                }

                case REQ_SET_CONFIGURATION:
                {
                    mvUsbCh9SetConfig(handle, setup, &setupPacket);
                    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP0, "STD_SETCFG\n");
                    break;
                }

                case REQ_GET_INTERFACE:
                {
                    mvUsbCh9GetInterface(handle, setup, &setupPacket);
                    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP0, "STD_GETIF\n");
                    break;
                }

                case REQ_SET_INTERFACE:
                {
                    mvUsbCh9SetInterface(handle, setup, &setupPacket);
                    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP0, "STD_SETIF\n");
                    break;
                }

                default:
                {
					fatal_printf("st1:%x,%x,%x,%x,%x,%d", setupPacket.REQUESTTYPE, setupPacket.REQUEST, setupPacket.VALUE, setupPacket.INDEX, setupPacket.LENGTH, setup);
                    _usb_device_stall_endpoint(handle, 0, 0);
                    break;
                }
            } /* Endswitch */

            break;
        }

        case REQ_TYPE_CLASS:  /* class specific request */
        {
            switch (setupPacket.REQUEST)
            {
                case REQ_SET_COMM_FEATURE:
                {
                    mvUsbCh9SetCommFeature(handle, setup, &setupPacket);
                    break;
                }

                case REQ_GET_COMM_FEATURE:
                {
                    mvUsbCh9GetCommFeature(handle, setup, &setupPacket);
                    break;
                }

                case REQ_SET_CONTROL_LINE_STATE:
                {
                    mvUsbCh9SetCtrlLineST(handle, setup, &setupPacket);
                    break;
                }

                case REQ_CLEAR_COMM_FEATURE:
                {
                    mvUsbCh9ClearCommFeature(handle, setup, &setupPacket);
                    break;
                }

                case REQ_SET_LINE_CODING:
                {
                    mvUsbCh9SetLineCode(handle, setup, &setupPacket);
                    break;
                }

                case REQ_GET_LINE_CODING:
                {
                    mvUsbCh9GetLineCode(handle, setup, &setupPacket);
                    break;
                }

#ifdef MV_USB2_MASS_STORAGE
                case REQ_GET_MAX_LUN:
                {
                    mvUsbCh9GetMaxLun(handle, setup, &setupPacket);
                    break;
                }

                case REQ_BOT_MSC_RESET:
                {
                    mvUsbCh9BotMscReset(handle, setup, &setupPacket);
                    break;
                }
#endif

                case REQ_SEND_ENCAPSULATED_COMMAND:
                {
                    mvUsbCh9EncapsulatedCmd(handle, setup, &setupPacket);
                    break;
                }

                case REQ_GET_ENCAPSULATED_RESPONSE:
                {
                    mvUsbCh9GetEncapsulatedRsp(handle, setup, &setupPacket);
                    break;
                }

                case REQ_SET_ETHERNET_MULTICAST_FILTERS:
                {
                    mvUsbCh9SetEthernetMulticastFilters( handle, setup, &setupPacket );
                    break;
                }
                case REQ_SET_ETHERNET_PACKET_FILTER:
                {
                    mvUsbCh9SetEthernetPacketFilter(handle, setup, &setupPacket);
                    break;
                }

//#ifdef MV_USB_MBIM
                case REQ_GET_NTB_PARAMETERS:
                {
                    mvUsbCh9GetNTBParameters(handle, setup, &setupPacket);
                    break;
                }

                case REQ_GET_NTB_INPUT_SIZE:
                {
                    mvUsbCh9GetNTBInputSize(handle, setup, &setupPacket);
                    break;
                }

                case REQ_SET_NTB_INPUT_SIZE:
                {
                    mvUsbCh9SetNTBInputSize(handle, setup, &setupPacket);
                    break;
                }

                case REQ_GET_NTB_FORMAT:
                {
                    mvUsbCh9GetNTBFormat(handle, setup, &setupPacket);
                    break;
                }

                case REQ_SET_NTB_FORMAT:
                {
                    mvUsbCh9SetNTBFormat(handle, setup, &setupPacket);
                    break;
                }

                case REQ_GET_CRC_MODE:
                {
                    mvUsbCh9GetCRCMode(handle, setup, &setupPacket);
                    break;
                }

                case REQ_SET_CRC_MODE:
                {
                    mvUsbCh9SetCRCMode(handle, setup, &setupPacket);
                    break;
                }
//#endif /* MV_USB_MBIM.  */

                case REQ_SET_NET_ADDRESS:
                {
                    mvUsbCh9SetNetAddress( handle, setup, &setupPacket );
                    break;
                }

                case REQ_GET_NET_ADDRESS:
                {
                    mvUsbCh9GetNetAddress( handle, setup, &setupPacket );
                    break;
                }

                case REQ_CLASS_SPECIFIC_RESERVED_5:
                {
                    if(setup)
                        mvUsbCh9SendEp0Data(handle, 0, 0);
                    break;
                }

                default:
                {

					fatal_printf("st2:%x,%x,%x,%x,%x,%d", setupPacket.REQUESTTYPE, setupPacket.REQUEST, setupPacket.VALUE, setupPacket.INDEX, setupPacket.LENGTH, setup);
                    _usb_device_stall_endpoint(handle, 0, 0);
                    break;
                }
            }

            break;
        }

        case REQ_TYPE_VENDOR:
        {
            /* vendor specific request can be handled here*/
            //if(!ifAcatUsb)
            {
                //Add 0x22 for USB serial to finish enumeration process
                if(setupPacket.REQUEST==0x22)
                {
                    _usb_device_send_data(handle, 0, 0, 0);
                }
            }

            if ( direction == UDC_EP_DIR_OUT )
            {
                if (USB_DEVICE_EP0_GET_TRANSACTION_DIR(setupPacket.REQUESTTYPE) == USBCDevice_DataStageDirectionH2D) /* H2D direction */
                {
                    //prepare fields
                    if ( setup )
                    {
                        _usbDeviceDatabase.ep0.ctrl_out_data_stage_total_length = setupPacket.LENGTH;

                        if ( setupPacket.LENGTH == 0 )
                        {
                            if(_usbDeviceDatabase.vendorClassRequestNotifyFn)
                            {
                                USB_DEVICE_ASSERT(_usbDeviceDatabase.vendorClassRequestNotifyFn ); //must not be NULL
                                memcpy((char*)&vendorSetupPacket,(char *)&setupPacket, sizeof(SETUP_STRUCT));
                                vendorSetupPacket.p_data = NULL;
                                _usbDeviceDatabase.vendorClassRequestNotifyFn(&vendorSetupPacket);
                                MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP0, "H2D - SETUP - vendorClassRequestNotifyFn called with setupPacket\n");
                            }
                            else
                            {
                                ErrorLogPrintf("vendorClassRequestNotifyFn is null, send zero packet.");
                                _usb_device_send_data(handle, 0, 0, 0);
                            }
                        }
                        else // setupPacket.LENGTH > 0) /* host only transmits setup packet, notify user so user can reply with ZLP */
                        {
                            USB_DEVICE_ASSERT(setupPacket.LENGTH <= USB_EP0_MAX_RX_TRANSFER_SIZE );    /* if assert need to enlarge ep0_out_data_buffer size*/
                            _usb_device_recv_data(_usbDeviceHandle, 0, _usbDeviceDatabase.ep0.ctrl_out_data_stage_buff, setupPacket.LENGTH);     /* Ack with ZLP. */
                            MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP0, "H2D - SETUP - prepare recv OUT data (H2D) len = %d\n", setupPacket.LENGTH);
                        }
                    }
                    else
                    {     /* we arive here for Ack on OUT stage (that its length is > 0 */
                        if ( _usbDeviceDatabase.ep0.ctrl_out_data_stage_total_length == length)
                        {
                            memcpy((char *)&vendorSetupPacket,(char *)&setupPacket, sizeof(SETUP_STRUCT));

                            if ( length == 0)
                            {
                                vendorSetupPacket.p_data = NULL;
                            }
                            else
                            {
                                vendorSetupPacket.p_data = _usbDeviceDatabase.ep0.ctrl_out_data_stage_buff;
                            }

                            if(_usbDeviceDatabase.vendorClassRequestNotifyFn!= NULL)
                            {
                                USB_DEVICE_ASSERT(_usbDeviceDatabase.vendorClassRequestNotifyFn);
                                _usbDeviceDatabase.vendorClassRequestNotifyFn(&vendorSetupPacket);
                                MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP0, "H2D - NON-setup - vendorClassRequestNotifyFn called with setup+DATA length=%d\n", length);
                            }
                            else
                            {
                                ErrorLogPrintf("vendorClassRequestNotifyFn is null, send zero packet.");
                                _usb_device_send_data(handle, 0, 0, 0);
                            }
                        }
                        else
                        {

							fatal_printf("st3:%x,%x,%x,%x,%x,%d", setupPacket.REQUESTTYPE, setupPacket.REQUEST, setupPacket.VALUE, setupPacket.INDEX, setupPacket.LENGTH, setup);
                            MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP0, "H2D - Non-setup - PROBLEM!!!!!\n");
                        }
                    }
                }
                else   /* D2H direction */
                {
                    if ( setup )
                    {
                        if(_usbDeviceDatabase.vendorClassRequestNotifyFn != NULL)
                        {
                            USB_DEVICE_ASSERT(_usbDeviceDatabase.vendorClassRequestNotifyFn ); //must not be NULL
                            memcpy((char*)&vendorSetupPacket,(char *)&setupPacket, sizeof(SETUP_STRUCT));
                            vendorSetupPacket.p_data = NULL;

                            //must notify upper layer SW & expect transmit
                            _usbDeviceDatabase.vendorClassRequestNotifyFn((USB_SetupCmdS*)&setupPacket);
                            MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP0, "D2H - SETUP - call vendorClassRequestNotifyFn with setup packet\n");

                            if ( setupPacket.LENGTH )
                            {
                                waitForStatusAfterInStage = TRUE;  // Host should Ack after IN stage completes.
                            }
                        }
                        else
                        {
                            ErrorLogPrintf("vendorClassRequestNotifyFn is null, send zero packet.");
                            _usb_device_send_data(handle, 0, 0, 0);
                        }
                    }
                    else  /* IN stage */
                    {
                        // Note: preparation for ZLP Ack for IN stage which its length is > 0  is done in TransmitEndpoint API.
                        USB_DEVICE_ASSERT( waitForStatusAfterInStage );
                        waitForStatusAfterInStage = FALSE;
                        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP0, "D2H - NON-setup - Host ACK after IN stage\n");
						fatal_printf("\r\n NON \r\n");
                    }
                }
            }
            else
            {
                MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP0, "ACK for length=%d\n", length);
            }

            break;
        }

        default:
        {
            if ( setup )
            {
				fatal_printf("st4:%x,%x,%x,%x,%x,%d", setupPacket.REQUESTTYPE, setupPacket.REQUEST, setupPacket.VALUE, setupPacket.INDEX, setupPacket.LENGTH, setup);
                _usb_device_stall_endpoint(handle, 0, 0);
            }
            break;
        }
    } /* Endswitch */

    if ( !setup )
    {
        mvUsbProcessNonSetupData(handle, direction, buffer, length);
    }

    return;
}

/*******************************************************************************
 * Function:    USB2DeviceEndpointAbort
 *******************************************************************************
 * Description: Abort any endpoint activity
 *
 * Parameters:  endpoint        - handle to udcEndpoint
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
USBCDevice_ReturnCodeE USB2DeviceEndpointAbort(USBDevice_EndpointE endpoint)
{
    if( _usbDeviceEndpoint[endpoint].endpointOpen)
        _usb_device_cancel_transfer(_usbDeviceHandle, _usbDeviceEndpoint[endpoint].udcEndpoint, _u2dcDefaultEndpointConfig[_usbDeviceEndpoint[endpoint].udcEndpoint].usbEndpointDir);

    _usbDeviceEndpoint[endpoint].endpointBusy = FALSE;

    return USB_DEVICE_RC_OK;
}

/*******************************************************************************
 * Function:    USB1DeviceAbortAllEndpoints
 *******************************************************************************
 * Description: Abort all endpoints
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
static void USB2DeviceAbortAllEndpoints(void)
{
    USBDevice_EndpointE ep;

    for(ep=USB_DEVICE_ENDPOINT_0; ep<USB_DEVICE_TOTAL_ENDPOINTS; ep++)
    {
        USB2DeviceEndpointAbort(ep);
    }
}

/*******************************************************************************
 * Function:    USB2DeviceDeactivateHardware
 *******************************************************************************
 * Description: Turn OFF UDC hardware (not to be use for suspend)
 *
 * Parameters:  none
 *
 * Outt:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
static void USB2DeviceDeactivateHardware(void)
{
    UINT32 cpsrReg;
    volatile UINT32 j,k=0;


    cpsrReg = disableInterrupts();
#if defined(UDC_USE_WAKEUP)
    USB2DeviceWakeupDisable();
#endif
    USB2DeviceStopEps (_usbDeviceHandle);
    restoreInterrupts(cpsrReg);
    USB2_DEACTIVATE_HARDWARE();

    /* clear the currently selected config value */
    _usb_device_set_status(_usbDeviceHandle, MV_USB_STATUS_CURRENT_CONFIG, 0);
    _usb_device_set_status(_usbDeviceHandle, MV_USB_STATUS_DEVICE_STATE, MV_USB_STATE_UNKNOWN);

	/* Make some delay */
	for(j=0;j<100;j++)
	{
	    k++;
	}

}

static void USB2DeviceStatusChange(USBDevice_StatusChangeE newStatusChange)
{
    BOOL  needToNotify    = FALSE;

    switch (newStatusChange)
    {
        case USB_DEVICE_ST_CHANGE_CABLE_OUT:
            USB2DeviceDeactivateHardware();
            USB2DeviceDatabaseReset(FALSE); // reset endpoint management states etc
            _usbDeviceDatabase.status   = USB_DEVICE_STATUS_NOT_CONNECTED;
            needToNotify                = TRUE;
            break;

        case USB_DEVICE_ST_CHANGE_CABLE_IN:
            _usbDeviceDatabase.status   = USB_DEVICE_STATUS_CONNECTED;
            USB2_ACTIVATE_HARDWARE(); //actually enable interrupts and UDC
            break;

        case USB_DEVICE_ST_CHANGE_RESET:
            USB2DeviceDatabaseReset(FALSE);
			if ( USB2_IS_HI_SPEED())
			{
	        	udcDefaultEndpointCfg = _u2dcDefaultEndpointConfig;
			}
			else
			{
	        	udcDefaultEndpointCfg = _u1dcDefaultEndpointConfig;
			}
            _usbDeviceDatabase.status   = USB_DEVICE_STATUS_RESET;
            needToNotify                = TRUE;

            /* cancel all pending requests and stop Hardware  */
            _usb_device_cancel_transfer(_usbDeviceHandle, MV_USB_SERVICE_EP0, MV_USB_SEND);
            _usb_device_cancel_transfer(_usbDeviceHandle, MV_USB_SERVICE_EP0, MV_USB_RECV);

            USB2DeviceStopEps(_usbDeviceHandle);

             /* Restart Control Endpoint #0 */
            USB2DeviceStartEp0(_usbDeviceHandle);


            /* Restart message queue of usb net */

            mvUsbNetResetMsgQ();
#ifdef USB_REMOTEWAKEUP
			EnableRemoteWakeup = FALSE;
			SetRemoteWakeupFeature = FALSE;
			mvUsbDatalistfree();
#endif
            _usb_device_set_status(_usbDeviceHandle, MV_USB_STATUS_CURRENT_CONFIG, 0);


           /*
            USB2DeviceAbortAllEndpoints();
           */
            break;

        case USB_DEVICE_ST_CHANGE_GET_DEVICE_DESC:
            /* When using COMPOSITE device, GET_DEVICE_DESC can be received few times
             * due to the PC enumeration process.
             * Therefore, GET_DEVICE_DESC changes the current status only if previous state was RESET
             */
            if(_usbDeviceDatabase.status == USB_DEVICE_STATUS_RESET)
            {

                _usbDeviceDatabase.status   = USB_DEVICE_STATUS_ENUM_IN_PROCESS;
                needToNotify            = TRUE;
            }
            else
            {/* do nothing - ignore this */
                return;
            }
            break;

        case USB_DEVICE_ST_CHANGE_CONFIG_CHANGE:            /* Connection turned ON  */

            _usbDeviceDatabase.status   = USB_DEVICE_STATUS_CONNECTED;
            needToNotify                = TRUE;
            break;

        case USB_DEVICE_ST_CHANGE_SUSPEND:
            break;
        case USB_DEVICE_ST_CHANGE_RESUME:
            break;
    }



    if(needToNotify &&  _usbDeviceDatabase.statusNotifyFn)
    {
        _usbDeviceDatabase.statusNotifyFn(_usbDeviceDatabase.status);

        if(statusNotifyFn_Patch)
            (statusNotifyFn_Patch)(_usbDeviceDatabase.status);
    }
} /* End of < USB2DeviceStatusChange > */

static void USB2DeviceEventNotify (void* handle, uint_8  type, boolean  setup, uint_8 direction, uint_8* buffer, uint_32 length, uint_8  error)
{
    switch(type)
    {
        case MV_USB_SERVICE_BUS_RESET:
            USB2DeviceStatusChange(USB_DEVICE_ST_CHANGE_RESET);
            break;

        case MV_USB_SERVICE_SUSPEND:
            USB2DeviceStatusChange(USB_DEVICE_ST_CHANGE_SUSPEND);
            break;

        case MV_USB_SERVICE_RESUME:
            USB2DeviceStatusChange(USB_DEVICE_ST_CHANGE_RESUME);
            break;
    }
}

/*******************************************************************************
 * Function:    USB2DeviceCableDetectionNotify
 *******************************************************************************
 * Description: EP0 interrupt handler
 *
 * Parameters:  cableStatus     - cable status
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
void USB2DeviceCableDetectionNotify(USBCable_StatusE cableStatus)
{
    if(cableStatus == USB_CABLE_OUT)
    {/*USB_CABLE_OUT - USB OFF */
        USB2DeviceStatusChange(USB_DEVICE_ST_CHANGE_CABLE_OUT);
    }
    else
    {/*USB_CABLE_IN - USB ON */
       USB2DeviceStatusChange(USB_DEVICE_ST_CHANGE_CABLE_IN);
    }
} /* End of < USB2DeviceCableDetectionNotify > */

/*******************************************************************************
 * Function:    USB2DeviceEndpointStall
 *******************************************************************************
 * Description: stall endpoint
 *
 * Parameters:  endpoint        - handle to udcEndpoint
 *
 * Output:      none
 *
 * Returns:     USB_DEVICE_RC_OK if stall is entered ( USB_DEVICE_RC_ERROR otherwise )
 *
 * Notes: can only be called from task,  blocked.
 ******************************************************************************/
USBCDevice_ReturnCodeE  USB2DeviceEndpointStall(USBDevice_EndpointE endpoint)
{
    _usb_device_stall_endpoint( _usbDeviceHandle, endpoint, udcDefaultEndpointCfg[endpoint].usbEndpointDir );

    // yss - return here - see if requires to perform clear or check afterwards and send USB_DEVICE_RC_ERROR / OK accordingly

    return USB_DEVICE_RC_OK;
}

/*******************************************************************************
 * Function:    USB2DeviceEndpointClose
 *******************************************************************************
 * Description: Open endpoint for usage
 *
 * Parameters:  endpoint        - endpoint number
 *              usageType       - usage type (DMA / Interrupt)
 *              dmaChannel      - in case of DMA this is channel number
 *
 * Output:      none
 *
 * Returns:     USBCDevice_ReturnCodeE - return code
 *
 * Notes:
 ******************************************************************************/
USBCDevice_ReturnCodeE USB2DeviceEndpointClose(USBDevice_EndpointE endpoint)
{
    UINT8 error = USB_OK;
    UINT32 cpsr;
    UDC_EndpointE     udcEndpoint = USBDeviceConvertUSB2UDCEndpoint(endpoint);
    UDC_EndpointDirE  endpointDir = udcDefaultEndpointCfg[endpoint].usbEndpointDir;;

    /* check if already open */
    if( !_usbDeviceEndpoint[endpoint].endpointOpen )
    {
        return USB_DEVICE_RC_ENDPOINT_NOT_OPENED;
    }

    cpsr = disableInterrupts();

    error = _usb_device_cancel_transfer (_usbDeviceHandle, udcEndpoint, endpointDir);
    if (error != USB_OK)
    {
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ERROR, "%s: failed in _usb_device_cancel_transfer, error=%d\n", __FUNCTION__, (int)error);
    }

    restoreInterrupts(cpsr);

    _usbDeviceEndpoint[endpoint].endpointOpen                   = FALSE;
    _usbDeviceEndpoint[endpoint].transactionCompletedNotifyFn   = NULL;

    _usb_device_unregister_service(	_usbDeviceHandle,udcEndpoint);


    return USB_DEVICE_RC_OK;
} /* End of < USB2DeviceEndpointClose > */

static void USB2DeviceTransactionCompleted (void* handle, uint_8  endpoint, boolean  setup, uint_8 direction, uint_8* buffer, uint_32 length, uint_8  error)
{
	/*for future debug use*/
	MV_USB_DEBUG_TRACE(((direction==1)? MV_USB_DEBUG_FLAG_TX : MV_USB_DEBUG_FLAG_RX), "%s: ep#=%d, is-setup=%d, dir=%d, buff=0x%x, len=%d, err=%d\n", __FUNCTION__, (int)endpoint, (int)setup, (int)direction, (int)buffer, length, (int)error);

    if ( error != USB_OK )
    {
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ERROR, "%s: failed endpoint #: %d, error=%d\n", __FUNCTION__, endpoint, (int)error);
    }
    buffer = CACHED_TO_NON_CACHED(buffer);    // yss - since recieve path uses DMA (in contrast to exist in USB1.1) we must take care of the buffer before return to user.
    _usbDeviceEndpoint[endpoint].transactionCompletedNotifyFn( (USBDevice_EndpointE)endpoint, buffer, length, TRUE);     // YSS - RETURN HERE, CHECK/CHANGE REGARDING LAST PARAMETER end_of_transmit
    _usbDeviceEndpoint[endpoint].endpointBusy = FALSE;
}


/*******************************************************************************
 * Function:    USB2DeviceEndpointOpen
 *******************************************************************************
 * Description: Open endpoint for usage
 *
 * Parameters:  endpoint            - endpoint number
 *              usageType           - usage type (DMA / Interrupt)
 *              dmaChannel          - in case of DMA this is channel number
 *              *pDescBuffer        - pointer to descriptor buffer
 *              descBufferLength    - descriptor buffer length
 *              transactionCompletedNotifyFn - transaction completed notify function
 *
 * Output:      none
 *
 * Returns:     USBCDevice_ReturnCodeE - return code
 *
 * Notes:
 ******************************************************************************/
USBCDevice_ReturnCodeE USB2DeviceEndpointOpen(USBDevice_EndpointE                       endpoint,
                                            USBDevice_UsageTypeE                      usageType,
                                            UINT8                                     dmaChannel,
                                            UINT8                                     *pBuffer,
                                            UINT16                                    bufferLength,
                                            USBDeviceTransactionCompletedNotifyFn     transactionCompletedNotifyFn)
{
    UINT8 error = USB_OK;
    int            lockKey;
    UDC_EndpointE  udcEndpoint;

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_INFO, "%s: ep#=%d type=%d buff=0x%x\n", __FUNCTION__, (int)endpoint, (int)usageType, (int)pBuffer);
    if(_usbDeviceEndpoint[endpoint].endpointOpen)
    {
        return USB_DEVICE_RC_ENDPOINT_IN_USE;
    }

    /* convert USB endpoint into UDC endpoint */
    _usbDeviceEndpoint[endpoint].udcEndpoint = USBDeviceConvertUSB2UDCEndpoint(endpoint);

    /* if endpoint not found this means it is not in configuration */
    if(_usbDeviceEndpoint[endpoint].udcEndpoint >= UDC_TOTAL_ENDPOINTS)
    {
        return USB_DEVICE_RC_ENDPOINT_NOT_IN_CONFIG;
    }
    udcEndpoint = _usbDeviceEndpoint[endpoint].udcEndpoint;

    lockKey = USB_lock();

    error = _usb_device_register_service(_usbDeviceHandle, udcEndpoint, USB2DeviceTransactionCompleted);
    if (error != USB_OK)
    {
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ERROR, "%s: failed in _usb_device_register_service, error=%d ep#=%d\n", __FUNCTION__, (int)error, (int)endpoint);
        return USB_DEVICE_RC_OPEN_ERROR;
    }
    error = _usb_device_init_endpoint(_usbDeviceHandle,
                                udcEndpoint,
                                udcDefaultEndpointCfg[udcEndpoint].usbMaxPacketSize,
                                udcDefaultEndpointCfg[udcEndpoint].usbEndpointDir,
                                udcDefaultEndpointCfg[udcEndpoint].usbEndpointType,
                                MV_USB_DEVICE_DONT_ZERO_TERMINATE);
	if(pBuffer==NULL)
	{
		_usbDeviceEndpoint[endpoint].pRxBuffer    = NULL;
        _usbDeviceEndpoint[endpoint].pRxBuffSize  = 0;
	}
    if (error != USB_OK)
    {
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ERROR, "%s: failed in _usb_device_init_endpoint, error=%d ep#=%d\n", __FUNCTION__, (int)error, (int)endpoint);
    }
    if ( pBuffer != NULL )
    {
        error = _usb_device_recv_data ( _usbDeviceHandle, udcEndpoint,  pBuffer, bufferLength);
        if (error != USB_OK)
        {
            MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ERROR, "%s: failed in _usb_device_recv_data, error=%d\n", __FUNCTION__, (int)error);
        }
        else
        {
            _usbDeviceEndpoint[endpoint].pRxBuffer    = pBuffer;
            _usbDeviceEndpoint[endpoint].pRxBuffSize  = bufferLength;
        }
    }

    _usbDeviceEndpoint[endpoint].endpointOpen                   = TRUE;
    _usbDeviceEndpoint[endpoint].transactionCompletedNotifyFn   = transactionCompletedNotifyFn;
    _usbDeviceEndpoint[endpoint].zlp = ( (usageType & USB_DEVICE_NEED_ZLP) == USB_DEVICE_NEED_ZLP ? TRUE : FALSE);

    USB_unlock(lockKey);
    return USB_DEVICE_RC_OK;
} /* End of < USB2DeviceEndpointOpen > */



/*******************************************************************************
 * Function:    USB2DeviceIsControllerEnabled
 *******************************************************************************
 * Description:  returns USB Device controller state
 *
 *
 * Parameters:
 *
 * Output:      none
 *
 * Returns:     return TRUE if USB Device controller is active, FALSE otherwise
 *
 * Notes:
 ******************************************************************************/
BOOL USB2DeviceIsControllerEnabled (void)
{
    USB_DEV_STATE_STRUCT_PTR         usb_dev_ptr;
    VUSB20_REG_STRUCT_PTR            dev_ptr;

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)_usbDeviceHandle;
    dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

    /* Check if Stop bit is on in the controller */
    if (dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD & (EHCI_CMD_RUN_STOP))
        return TRUE;
    else
       return FALSE;
}

// this function is called rigth after set configuration (which was different from the current configuration)
void USB2DevicePerformAfterEnum(void)
{
    UDC_EndpointE       udcEndpoint;
    UINT8 epNum;

    UINT8 epDir;

    UINT8* pRxBuffer;
    UINT32 bufferLen;


    //ErrorLogPrintf("Set configuration");


    for(udcEndpoint = UDC_ENDPOINT_A; udcEndpoint < USB_DEVICE_TOTAL_ENDPOINTS; udcEndpoint++)
    {
        if( _usbDeviceEndpoint[udcEndpoint].endpointOpen)
        {
            epNum = udcDefaultEndpointCfg[udcEndpoint].usbEndpointNum;

            epDir = udcDefaultEndpointCfg[udcEndpoint].usbEndpointDir;

            pRxBuffer = _usbDeviceEndpoint[udcEndpoint].pRxBuffer;
            bufferLen = _usbDeviceEndpoint[udcEndpoint].pRxBuffSize;
            if ( (pRxBuffer != NULL) && (bufferLen != 0)&&(epNum!=0))
            {
            	//uart_printf("rx ep %d\r\n",epNum);

            	if (_usb_device_get_dtd_cnt(epNum, epDir, NULL) == 0 )
            	{
                    USB2DeviceEndpointReceive((USBDevice_EndpointE)epNum, pRxBuffer,  bufferLen,  0);
                }
            }


            _usb_dci_vusb20_enable_endpoint(_usbDeviceHandle, epNum, epDir);

        }
    }
    USB2DeviceStatusChange(USB_DEVICE_ST_CHANGE_CONFIG_CHANGE);

    USB_TRACE("Set configuration");
}


void USB2DeviceEnableEndpoint(void)
{
    UDC_EndpointE       udcEndpoint;
    UINT8 epNum;
    UINT8 epDir;

    for(udcEndpoint = UDC_ENDPOINT_A; udcEndpoint < USB_DEVICE_TOTAL_ENDPOINTS; udcEndpoint++)
    {
        if( _usbDeviceEndpoint[udcEndpoint].endpointOpen)
        {
            epNum = udcDefaultEndpointCfg[udcEndpoint].usbEndpointNum;

            epDir = udcDefaultEndpointCfg[udcEndpoint].usbEndpointDir;

            _usb_dci_vusb20_enable_endpoint(_usbDeviceHandle, epNum, epDir);

        }
    }
}

void USB2DevicePhase1Init(void)
{
     /*  initialize all USB APIs to USB2.0 interface
     * ---------------------------------------------------*/
    USBDevicePhase2Init             = USB2DevicePhase2Init;
    USBDeviceEndpointGetHWCfg       = USB2DeviceEndpointGetHWCfg;
    USBDeviceIsControllerEnabled    = USB2DeviceIsControllerEnabled;
    USBDeviceEndpointStall          = USB2DeviceEndpointStall;
    USBDeviceEndpointOpen           = USB2DeviceEndpointOpen;
	USBDeviceEndpointClose			= USB2DeviceEndpointClose;
	USBDeviceEndpointAbort			= USB2DeviceEndpointAbort;
	USBDeviceVendorClassResponse	= USB2DeviceVendorClassResponse;
	USBDeviceEndpointReceive		= USB2DeviceEndpointReceive;
	USBDeviceEndpointReceiveCompletedExt = USB2DeviceEndpointReceiveCompletedExt;
	USBDeviceEndpointReceiveCompleted =	USB2DeviceEndpointReceiveCompleted;
	USBDeviceEndpointTransmit 	    = USB2DeviceEndpointTransmit;
	USBDeviceEndpointCancelTransmit = USB2DeviceEndpointCancelTransmit;
    USBDeviceEndpointCancelReceive = USB2DeviceEndpointCancelReceive;
    USBDeviceEndpointMultiTransmit  = USB2DeviceEndpointMultiTransmit;
    USBDeviceCableDetectionNotify   = USB2DeviceCableDetectionNotify;

#if defined (_TAVOR_BOERNE_)
	PRMManage( PRM_SRVC_USB_20_CLIENT_OTG0_PV, PRM_RSRC_ALLOC);
#endif
    USB2DeviceDatabaseReset(TRUE);
}


int USB2GetDTDEntry(void)
{
    USB_DEV_STATE_STRUCT_PTR             usb_dev_ptr;

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)_usbDeviceHandle;

    ASSERT(usb_dev_ptr != NULL)
    return usb_dev_ptr->DTD_ENTRIES;
}
