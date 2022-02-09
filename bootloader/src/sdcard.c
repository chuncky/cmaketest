/***************************************************************************
* Filename: sdcard.c
*
* Authors:
*
* Description:
* SD card driver
* SDIO init and supply basic APIs
*
* Last Updated:
*
* Notes:
******************************************************************************/
#include "sdio.h"
//#include "intc.h"               // for INTCConfigure declaration
//#include "intc_list.h"          // for INTC_SRC_USB2CI_OTG definition

//#include "bsp_hisr.h"

//#include "utilities.h"
//#include "pmic.h"
#include "ff.h"
#include "fat32_utils.h"
#define fatal_printf uart_printf
#ifdef  MV_USB2_MASS_STORAGE
#include "mvUsbStorage.h"
#endif
//add by yazhouren@asrmicro.com
#define PLAT_CRANE 1
#define FAT32_FILE_SYSTEM
///define for boot33
extern unsigned char pmic_id;
OSA_STATUS OSAFlagWait( OSFlagRef Ref, UINT32 mask, UINT32 operation, UINT32 *flags, UINT32 timeout )
{
    return OS_SUCCESS;
}
//#define ASSERT(x) {if((x)==0){uart_printf("%s:%d\r\n", __FILE__, __LINE__);while(1);}}
int sdcard_fatfs_mount_status_g;
int sdcard_fat_is_ok(void)
{
    return sdcard_fatfs_mount_status_g;
}

void CacheCleanMemory(void *pMem, UINT32 size)
{
	return;
}



#ifdef FAT32_FILE_SYSTEM
//#include "crossplatformsw.h"
#endif
//#include "cgpio.h"

//#include "usb_macro.h"
//#include "platform.h"
//#include "mvUsbStorage.h"
#define SDCARD_DEBUG 0
#define SDCARD_LISR_DBG 0

#define SDCARD_INIT_UNDO 	0
#define	SDCARD_INIT_SUCCESS 1
#define SDCARD_INIT_FAILED	2


 //0 for invalid card
 // 1 for standard card
 // 2 for high capacity card
static char card_type = 0;
static char hs_switch = 0;
#define CARD_TYPE_INVALID   0
#define CARD_TYPE_STANDARD  1
#define CARD_TYPE_HIGH      2

//card status
static char card_status = 1;
#define CARD_STATUS_INSERT  0
#define CARD_STATUS_REMOVE  1

static char card_mode;
#define CARD_MODE_INT		0
#define CARD_MODE_POLL		1

__align(4)  unsigned char SDStack[2048];
__align(4)  unsigned char SDCard_InsertRemove_Stack[2048];

volatile static UINT16 SD_RESPONSE[8];         //response registers
OSSemaRef	SDSysRef = NULL;
//OSSemaRef   SDCMDRef = NULL;
OSSemaRef   SDDATARef = NULL;

OSTaskRef   SDTaskRef = NULL;
OSTaskRef	SDCard_InsertRemove_TaskRef = NULL;
OSFlagRef   SDFlgRef  = NULL;
OSFlagRef	SD_CMDReset_FlgRef = NULL;

static OS_HISR                      SdcardHISR;   //HISR control block
static OS_HISR						SdcardInsertHISR;
static OS_HISR						SdcardRemoveHISR;

//hisr for command and data transfer
//static NU_HISR						SdCMDHISR;
static OS_HISR						SDDATAHISR;

static UINT8         SD_CMD_COMP_status = 0;
static UINT8         SD_DATA_COMP_status = 0;
static UINT8         SD_DATA_START_status = 0;
static UINT8         sdcard_initialized = 0;
static UINT8		  sdcard_reinitialized = 0;
static UINT8 		  SD_CMD_BufferReady = 0;
static UINT16         SD_ERR_INTSTATUS = 0;

static UINT32         SD_RCA = 0;    //RCA of MMC card
static UINT32         SDCARD_CTRL_BASE_ADDR = 0;
static UINT32 		  SD_BLK_NUM = 0;

static UINT8 		  SD_Read_status = 0;
static UINT8 		  SD_Write_status = 0;

static UINT8         SD_Trans_Status = 0;
volatile UINT8 CMD8_timout_cnt = 0;
static UINT8         SD_Infinite_Err_Flag = 0;
static UINT8         SD_Decrease_Clk_Flag = 0;

static UINT32 gSDRecordTime;
static int ee_sd_dump_status_g = 1;


OSA_STATUS OSASemaphoreRelease_sdcard(OSASemaRef semaRef)
{
    if (ee_sd_dump_status_g)
		return OS_SUCCESS;
	else 
		//return OSASemaphoreRelease(semaRef);
		return OS_SUCCESS;
}
OSA_STATUS OSASemaphoreAcquire_sdcard(OSASemaRef semaRef, UINT32 timeout)
{
    if (ee_sd_dump_status_g)
		return OS_SUCCESS;
	else
		//return OSASemaphoreAcquire(semaRef, timeout);
		return OS_SUCCESS;
}

#define BU_REG_READ(x) (*(volatile unsigned long *)(x))
#define BU_REG_WRITE(x,y) ((*(volatile unsigned long *)(x)) = y )

void AIB_Secure_Write(unsigned int address, unsigned int value)
{
	BU_REG_WRITE(0xd4015050, 0xBABA);	
	BU_REG_WRITE(0xd4015054, 0xEB10);
	BU_REG_WRITE(address, value);
}

UINT32 AIB_Secure_Read(unsigned int address)
{
	UINT32 val;
	BU_REG_WRITE(0xd4015050, 0xBABA);	
	BU_REG_WRITE(0xd4015054, 0xEB10);
	val = BU_REG_READ(address);
	return val;
}

//add by yazhouren@asrmicro.com
#define AIB_MMC1_IO_REG      0xD401e82c
 static void AIB_MMC1_IO_Set_3_3V(void)
 {
	 volatile unsigned int val = 0;

	 val = AIB_Secure_Read(AIB_MMC1_IO_REG); //AIB_MMC1_IO_REG
	 val &= ~(0x4); //0--> 3.3V enable, 1-->1.8V enable
	 AIB_Secure_Write(AIB_MMC1_IO_REG, val);
 }

static void AIB_MMC1_IO_Set_1_8V(void)
 {
	 volatile unsigned int val = 0;
 
	 val = AIB_Secure_Read(AIB_MMC1_IO_REG); //AIB_MMC1_IO_REG
	 val |= (0x4); //0--> 3.3V enable, 1-->1.8V enable
	 AIB_Secure_Write(AIB_MMC1_IO_REG, val);
 }

 static BOOL PlatformSDCardEnable(void)
 {
	 return TRUE;
 }
 static void PMIC_Set_SDIO_1_8V(void)
 {
     if (PMIC_IS_PM812())
     {
	    PM812_Ldo_12_set_1_8();
	 }
	 else if(PMIC_IS_PM813()||PMIC_IS_PM813S())
	 {
        Ningbo_Ldo_9_set_1_8();
	 }
	 else //802
	 {
        Guilin_Ldo_6_set_1_8();
	 }
	 AIB_MMC1_IO_Set_1_8V();
 }

 static void PMIC_Set_SDIO_2_8V(void)
 {
	 AIB_MMC1_IO_Set_3_3V();
     if (PMIC_IS_PM812())
     {
	    PM812_Ldo_12_set_2_8();
	 }
	 else if(PMIC_IS_PM813()||PMIC_IS_PM813S())
	 {
        Ningbo_Ldo_9_set_2_8();
	 }
	 else //802
	 {
        Guilin_Ldo_6_set_2_8();
	 }
 }

 /*******************************************************************************
 * Function: eeGetSystemAssertFlag
 ********************************************************************************
 * Description: Get system assert flag.
 *
 * Parameters:
 *
 * Return value:
 *
 * Notes:
 *******************************************************************************/
 BOOL eeGetSystemAssertFlag(void)
 {
	 //return ee_sd_dump_status_g;
	 return eeSDDump_status_get();
 }



 /* SDR104 tuning pattern */
static const UINT8 sd_tuning_pattern[] =
{
	0xff, 0x0f, 0xff, 0x00,   0xff, 0xcc, 0xc3, 0xcc,   0xc3, 0x3c, 0xcc, 0xff,   0xfe, 0xff, 0xfe, 0xef,
	0xff, 0xdf, 0xff, 0xdd,   0xff, 0xfb, 0xff, 0xfb,   0xbf, 0xff, 0x7f, 0xff,   0x77, 0xf7, 0xbd, 0xef,
	0xff, 0xf0, 0xff, 0xf0,   0x0f, 0xfc, 0xcc, 0x3c,   0xcc, 0x33, 0xcc, 0xcf,   0xff, 0xef, 0xff, 0xee,
	0xff, 0xfd, 0xff, 0xfd,   0xdf, 0xff, 0xbf, 0xff,   0xbb, 0xff, 0xf7, 0xff,   0xf7, 0x7f, 0x7b, 0xde,
};

//extern void USB2MgrDeviceReset(void);
extern void Create_CurDir(void);
extern BOOL DumpFileToSDEnable(void);
//get fat32 file system init status
//extern UINT8 FSInit_Status_Get(void);
extern void sdcard_check_free_space(UINT32 MinSize, BOOL DumpBin);
extern void Fat32_Init(void);
//extern void FSInit_Status_Init(void);
extern BOOL eeGetSystemAssertFlag(void);

static void SdCard_Wait(UINT32 count)
{
    UINT32 i = 0;
    for(i=0; i<count; i++)
    {;}
}

static void SdCard_MsWait(UINT32 count)
{
    UINT32 i = 0;
    UINT32 j = 0;
    for(i=0; i<count; i++)
    {
        for(j=0; j<40000; j++)
        {
            ;
        }
    }
}


static UINT8 sdcard_readb(UINT32 base, UINT32 reg)
{
    volatile UINT8* ptr8;
    UINT8 tmp;
    ptr8 = (volatile UINT8*)(base + reg);
    tmp = *ptr8;
    return tmp;
}

static void sdcard_writeb(UINT32 base, UINT32 reg, UINT8 data)
{
    volatile UINT8* ptr8;
    ptr8 = (volatile UINT8*)(base + reg);
    *ptr8 = data;
}

static UINT32 sdcard_readl(UINT32 base, UINT32 reg)
{
    volatile UINT32* ptr32;
    UINT32 tmp;
    ptr32 = (volatile UINT32*)(base + reg);
    tmp = *ptr32;
    return tmp;
}

static void sdcard_writel(UINT32 base, UINT32 reg, UINT32 data)
{
    volatile UINT32* ptr32;
    ptr32 = (volatile UINT32*)(base + reg);
    *ptr32 = data;
}

static UINT16 sdcard_readw(UINT32 base, UINT32 reg)
{
    volatile UINT16* ptr16;
    UINT16 tmp;
    ptr16 = (volatile UINT16*)(base + reg);
    tmp = *ptr16;
    return tmp;
}

static void sdcard_writew(UINT32 base, UINT32 reg, UINT16 data)
{
    volatile UINT16* ptr16;
    ptr16 = (volatile UINT16*)(base + reg);
    *ptr16 = data;
}

static void SdCard_IRQ_Enable(UINT32 base_addr, UINT16 mask)
{
	volatile UINT16* ptr16;

	ptr16 = (UINT16*) (base_addr + SD_NORM_INTR_STS_EBLE_offset);
	*ptr16 |= mask ;

	ptr16 = (UINT16*) (base_addr + SD_NORM_INTR_STS_INTR_EBLE_offset);
	*ptr16 |= mask ;
}

/**
 *  @brief: enable clock
 *  @base_addr: base address
 *  @clock: dividsion 0--48MHZ; 1--24MHZ; 2--12MHZ; 4--6MHZ;8--3MHZ
 */
static void SdCard_Enable_Clock(UINT32 base_addr, UINT32 CLOCK)
{
    volatile UINT16* ptr16;
    UINT32      tick_count;
#ifdef PLAT_CRANE
	{
        CLOCK = ((CLOCK&0x300)>>8)|((CLOCK&0xFF)<<2);
        ptr16 = (UINT16*) (base_addr + SD_CLOCK_CTRL_offset);
        if((*ptr16>>6) == (CLOCK&0x3FF))
        {
          //  return;
        }

        /*turn on sd clock and set timeouts*/
        ptr16 = (UINT16*) (base_addr + SD_CLOCK_CTRL_offset);
        *ptr16 = (((CLOCK & 0X3FF)) << 6 | 0x01);
    }
#else
    if(NezhaChipIS_B0() == 1)
    {
        CLOCK = ((CLOCK&0x300)>>8)|((CLOCK&0xFF)<<2);
        ptr16 = (UINT16*) (base_addr + SD_CLOCK_CTRL_offset);
        if((*ptr16>>6) == (CLOCK&0x3FF))
        {
          //  return;
        }

        /*turn on sd clock and set timeouts*/
        ptr16 = (UINT16*) (base_addr + SD_CLOCK_CTRL_offset);
        *ptr16 = (((CLOCK & 0X3FF)) << 6 | 0x01);
    }
    else
    {
        ptr16 = (UINT16*) (base_addr + SD_CLOCK_CTRL_offset);
        if((*ptr16>>8) == (CLOCK&0xFF))
        {
           // return;
        }

        /*turn on sd clock and set timeouts*/
        ptr16 = (UINT16*) (base_addr + SD_CLOCK_CTRL_offset);
        *ptr16 = (((CLOCK & 0XFF)) << 8 | 0x01);
    }
#endif

    /*Loop wait for sd clock and timeout are set*/
    tick_count = 100;
    while( !(*ptr16 & 0x0002 == 0x2) )
    {
        if(tick_count == 0)
        {
            uart_printf("clock timeout \r\n");
        }
        SdCard_MsWait(1);
        tick_count--;
    }

    *ptr16 |= 0x0004;

    ptr16 = (UINT16*) (base_addr + SD_CLOCK_CTRL_offset);

    //uart_printf("SDCARD:SD_CLOCK_CTRL_offset=%x", *ptr16);

}

static UINT32 SdCard_SW_ResetData(UINT32 base_addr)
{
	volatile UINT16* ptr16;
	UINT32 timeout = 10000;
	UINT16 val;

#if SDCARD_DEBUG
	uart_printf("SdCard_SW_ResetData\r\n");
#endif

	ptr16 = (UINT16*) (base_addr + SD_SW_RESET_CTRL_offset);
	val = *ptr16;

	val |= SW_RST_DAT;

	*ptr16 = val;

	do
    {
		val = *ptr16;
		if (!(val & (SW_RST_DAT)))
		{
			break;
		}

	} while (timeout--);

	if (timeout)
	{
		return MMC_ERR_NONE;
	}

	uart_printf("SdCard_SW_ResetData timeout\r\n");
	return MMC_ERR_CMD_TOUT;
}

static UINT32 SdCard_SW_ResetCmd(UINT32 base_addr)
{
	volatile UINT16* ptr16;
	UINT32 timeout = 10000;
	UINT16 val;

#if SDCARD_DEBUG
	uart_printf("SdCard_SW_ResetCmd\r\n");
#endif

	ptr16 = (UINT16*) (base_addr + SD_SW_RESET_CTRL_offset);
	val = *ptr16;

	val |= SW_RST_CMD;

	*ptr16 = val;

	do
    {
		val = *ptr16;
		if (!(val & (SW_RST_CMD)))
		{
			break;
		}

	} while (timeout--);

	if (timeout)
	{
		//uart_printf("SdCard_SW_ResetCmd end\r\n");
		return MMC_ERR_NONE;
	}

	uart_printf("SdCard_SW_ResetCmd timeout\r\n");
	return MMC_ERR_CMD_TOUT;
}


static UINT32 SdCard_SW_Reset(UINT32 base_addr)
{
	volatile UINT16* ptr16;
	UINT32 timeout = 1000;
	UINT16 val;

#if SDCARD_DEBUG
	uart_printf("Software reset for card\r\n");
#endif

	ptr16 = (UINT16*) (base_addr + SD_SW_RESET_CTRL_offset);
	val = *ptr16;

	val |= SW_RST_CMD | SW_RST_DAT;

	*ptr16 = val;

	do
    {
		val = *ptr16;
		if (!(val & (SW_RST_DAT | SW_RST_CMD | SW_RST_ALL)))
			break;
		SdCard_Wait(1000);
	} while (timeout--);

	if (timeout)
		return MMC_ERR_NONE;

#if SDCARD_DEBUG
	uart_printf("Software reset timeout\r\n");
#endif

	return MMC_ERR_CMD_TOUT;
}

/**
 *  @brief: send cmd without data
 *
 *  @cmd: command code
 *  @argv:  argument
 *  @base_addr: base address
 *
 *  @return:    0 is success, otherwise is failure
 */
static UINT32 SdCard_Cmd_Nodata(UINT16 cmd, UINT32 argv, UINT32 base_addr)
{
	volatile UINT32* ptr32;
	volatile UINT16* ptr16;
	UINT32 tick_count;
    UINT32 mask;
	int timeout = 0;
	UINT16 isr_sts;
	OSA_STATUS status;
	/*Set wait*/
	SD_CMD_COMP_status = 0;

    timeout = 1000000;
    mask = SDHCI_CMD_INHIBIT;
    while((sdcard_readl(base_addr, SD_PRESENT_STAT_0_offset))&mask)
    {
        if(timeout == 0)
        {
            uart_printf("Controller never released inhabit bit\r\n");
            return MMC_ERR_CMD_TOUT;
        }
        timeout--;
    }

	/*put to execute*/
	ptr32 = (UINT32*) (base_addr + SD_ARG_LOW_offset);
	*ptr32 = argv;

    ptr16 = (UINT16*) (base_addr + SD_CMD_offset);
    *ptr16 = cmd;

	/*wait for complete*/
	tick_count = 40000;

	if(sdcard_get_mode() == CARD_MODE_INT)
	{
		while (!SD_CMD_COMP_status)
		{
	    	//SdCard_Wait(2000);
	    	SdCard_Wait(500);//change by yazhouren@asrmicro.com
			tick_count--;
			if (tick_count == 0 )
			{
				uart_printf("Wait for command %d time out!\r\n",cmd>>8);
				return MMC_ERR_CMD_TOUT;
			}
		}

	}
	else if(sdcard_get_mode() == CARD_MODE_POLL)
	{
		isr_sts = sdcard_readw(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_offset);
		while (!(isr_sts & CMD_COMP))
		{
	    	//SdCard_Wait(2000);
	    	SdCard_Wait(500);	//change by hengshanyang, Owner:yazhouren
			tick_count--;
			if(tick_count == 0 )
			{
				uart_printf("Wait for command %d time out!\r\n",cmd>>8);
				return MMC_ERR_CMD_TOUT;
			}
			isr_sts = sdcard_readw(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_offset);
		}
		//clear cmd complete interrupt
		sdcard_writew(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_offset, CMD_COMP);
	}

	/*Get [0--7] Response registers*/
	ptr16 = (UINT16*) (base_addr + SD_RESP_0_offset);
	for (tick_count = 0; tick_count < 8; tick_count++)
	{
		SD_RESPONSE[tick_count] = *ptr16;
		ptr16++;
	}

	return MMC_ERR_NONE;
}

/**
 *  @brief: send cmd with data
 *
 *  @cmd: command code
 *  @argv:  argument
 *  @base_addr: base address
 *
 *  @return:    0 is success, otherwise is failure
 */
static UINT32 SdCard_Read_NoDMA(UINT16 cmd, UINT32 argv, UINT16 blk_size, UINT16 blk_cnt, UINT32 buffer)
{
	volatile UINT32* ptr32;
	volatile UINT16* ptr16;
	UINT32 tick_count,base_addr;
    UINT32 mask;
	int timeout = 0;
	UINT16 isr_sts;
	OSA_STATUS status;

	base_addr = SDCARD_CTRL_BASE_ADDR;
	/*Set wait*/
	SD_CMD_COMP_status = 0;
	SD_CMD_BufferReady = 0;

    timeout = 1000000;
    mask = SDHCI_CMD_INHIBIT;
    while((sdcard_readl(base_addr, SD_PRESENT_STAT_0_offset))&mask)
    {
        if(timeout == 0)
        {
            uart_printf("Controller never released inhabit bit\r\n");
            return MMC_ERR_CMD_TOUT;
        }
        timeout--;
    }

	/*set block size and block count*/
	ptr32 = (UINT32*) (base_addr + SD_BLOCK_SIZE_offset);
	*ptr32 = blk_size | (blk_cnt << 16);
	/*put to execute*/
	ptr32 = (UINT32*) (base_addr + SD_ARG_LOW_offset);
	*ptr32 = argv;

	sdcard_writel(base_addr, SD_TRANSFER_MODE_offset, (0x10|(cmd<<16)));

	/*wait for complete*/
#if 0
	tick_count = 40000;

	if(sdcard_get_mode() == CARD_MODE_INT)
	{
		while (!SD_CMD_COMP_status)
		{
	    	SdCard_Wait(2000);
			tick_count--;
			if (tick_count == 0 )
			{
				uart_printf("Wait for command %d time out!\r\n",cmd>>8);
				return MMC_ERR_CMD_TOUT;
			}
		}

	}
	else if(sdcard_get_mode() == CARD_MODE_POLL)
	{
		isr_sts = sdcard_readw(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_offset);
		while (!(isr_sts & CMD_COMP))
		{
	    	SdCard_Wait(2000);
			tick_count--;
			if(tick_count == 0 )
			{
				uart_printf("Wait for command %d time out!\r\n",cmd>>8);
				return MMC_ERR_CMD_TOUT;
			}
			isr_sts = sdcard_readw(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_offset);
		}
		//clear cmd complete interrupt
		sdcard_writew(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_offset, CMD_COMP);
	}

	/*Get [0--7] Response registers*/
	ptr16 = (UINT16*) (base_addr + SD_RESP_0_offset);
	for (tick_count = 0; tick_count < 8; tick_count++)
	{
		SD_RESPONSE[tick_count] = *ptr16;
		ptr16++;
	}
#endif
	//wait for buffer ready
	tick_count = 80000;
	if(sdcard_get_mode() == CARD_MODE_INT)
	{
		while (!SD_CMD_BufferReady)
		{
	    	SdCard_Wait(2000);
			tick_count--;
			if (tick_count == 0 )
			{
				uart_printf("Wait for CMD%d Buffer ready time out!\r\n",cmd>>8);
				return MMC_ERR_CMD_TOUT;
			}
		}

	}
	else if(sdcard_get_mode() == CARD_MODE_POLL)
	{
		isr_sts = sdcard_readw(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_offset);
		while (!(isr_sts & RX_RDY))
		{
	    	SdCard_Wait(2000);
			tick_count--;
			if(tick_count == 0 )
			{
				uart_printf("Wait for CMD%d Buffer ready time out!\r\n",cmd>>8);
				return MMC_ERR_CMD_TOUT;
			}
			isr_sts = sdcard_readw(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_offset);
		}
		//clear cmd complete interrupt
		sdcard_writew(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_offset, CMD_COMP);
	}

	UINT16 count;
	count = 0;
	ptr32 = (UINT32 *)buffer;
	while(count < blk_size)
	{
		*ptr32 = sdcard_readl(base_addr, SD_BUF_PORT_0_offset);
		ptr32++;
		count = count+4;
	}

	return MMC_ERR_NONE;
}
int sdcard_clk_on(void)
{
	volatile UINT32* ptr32;

	/*Turn on SD host 0 axi clock and pheripherial clock*/
	ptr32 = (UINT32*) (SD0_HOST_PMU_AXI_CLOCK) ;
#ifdef PLAT_CRANE
	*ptr32 = 0x95b;//change by yazhouren@asrmicro.com
#else
	*ptr32 = 0x59;
#endif
	/*Turn on SD host 1 axi clock and pheripherial clock*/
	ptr32 = (UINT32*) (SD1_HOST_PMU_AXI_CLOCK) ;
#ifdef PLAT_CRANE
	*ptr32 = 0x952;//change by yazhouren@asrmicro.com
#else
	*ptr32 = 0x52;
#endif

	uart_printf("SDCARD:SD1_HOST_PMU_AXI_CLOCK=%x\r\n", *(UINT32*) (SD0_HOST_PMU_AXI_CLOCK));
	return 0;
}

int sdcard_clk_off(UINT32 base)
{
	volatile UINT32* ptr32;
	UINT16 clk;

	/*Turn off SD host 1 axi clock and pheripherial clock*/
	ptr32 = (UINT32*) (SD1_HOST_PMU_AXI_CLOCK) ;
	*ptr32 = 0x0;
	clk = sdcard_readw(base, SD_CLOCK_CTRL_offset);

    //disable sd card clock
	clk = clk & 0xfffb;
	sdcard_writel(base, SD_CLOCK_CTRL_offset, clk);

	uart_printf("clk off:0x%x", sdcard_readl(base, SD_CLOCK_CTRL_offset));
}

/**
 *  @brief This function set voltage.
 *
 *  @return MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static int sdcard_set_power()
{
    volatile UINT8*  ptr8;

    ptr8 = (UINT8*) (SDCARD_CTRL_BASE_ADDR + SDHCI_POWER_CONTROL);
    if(0x0F == *ptr8)
    {
        return 0;
    }
    else
    {
        uart_printf("==sdcard set power==\r\n");
        *ptr8 = 0x0E;  //3.3v
        *ptr8 |= SDHCI_POWER_ON;
        SdCard_MsWait(20);
        uart_printf("sdcard set power %x\r\n",*((UINT8*) (SDCARD_CTRL_BASE_ADDR + SDHCI_POWER_CONTROL)));
        ptr8 = (UINT8*) (SDCARD_CTRL_BASE_ADDR + SD_HOST_CTRL_offset);
        *ptr8 = 0x00;
        uart_printf("sdcard set ctrl %x\r\n",*((UINT8*) (SDCARD_CTRL_BASE_ADDR + SD_HOST_CTRL_offset)));
    }

    return 0;

}

#define UNSTUFF_BITS(resp,start,size)					\
	({								\
		const int __size = size;				\
		const UINT32 __mask = (__size < 32 ? 1 << __size : 0) - 1;	\
		const int __off = 3 - ((start) / 32);			\
		const int __shft = (start) & 31;			\
		UINT32 __res;						\
									\
		__res = resp[__off] >> __shft;				\
		if (__size + __shft > 32)				\
			__res |= resp[__off-1] << ((32 - __shft) % 32);	\
		__res & __mask;						\
	})

static void SD_GetResponse136(UINT32* resp)
{
	resp[0] = SD_RESPONSE[5] >> 8 | SD_RESPONSE[6] << 8 | SD_RESPONSE[7] << 24;
	resp[1] = SD_RESPONSE[3] >> 8 | SD_RESPONSE[4] << 8 | SD_RESPONSE[5] << 24;
	resp[2] = SD_RESPONSE[1] >> 8 | SD_RESPONSE[2] << 8 | SD_RESPONSE[3] << 24;
	resp[3] = SD_RESPONSE[0] << 8 | SD_RESPONSE[1] << 24;
}

static UINT32 SD_Decode_CSD(UINT32 base_addr)
{
	UINT32 c_size,c_size_mult,csd_struct,capacity,read_len;
	UINT32 resp[4];

	SD_GetResponse136(resp);

	csd_struct  = UNSTUFF_BITS(resp, 126, 2);
	switch(csd_struct){
	case 0:
		c_size = UNSTUFF_BITS(resp,62,12);
		c_size_mult = UNSTUFF_BITS(resp,47,3);
		read_len = UNSTUFF_BITS(resp,80,4);
		capacity = (1+c_size) << (c_size_mult + 2);
		capacity = capacity << read_len;

		SD_BLK_NUM = capacity/0x200;
		uart_printf("c_size:0x%x,c_size_mult:0x%x,read_len:0x%x",c_size,c_size_mult,read_len);
		uart_printf("standard card, capacity:0x%x.,blk_num:0x%x\n",capacity,SD_BLK_NUM);
		break;
	case 1:
		c_size = UNSTUFF_BITS(resp, 48, 22);
		capacity = (1+c_size) << 10;
		//this should be block number, the actual capacity need to mult with 0x200
		SD_BLK_NUM = capacity;
		uart_printf("high capacity card, capacity:0x%x\n",capacity);
		break;
	default:
			uart_printf("unrecognised CSD structure version:0x%x",csd_struct);
		}

	return MMC_ERR_NONE;
}


extern void PMIC_ShutDown_SdLDO(void);
static int sdcard_cmd_init()
{
    UINT32          status = 0;
    UINT16             cmd = 0;
	UINT32            argv = 0;
	UINT32             ocr = 0;
    UINT32           count = 0;
    UINT32       base_addr = SD1_HOST_CTRL_ADDR;
	UINT32 		 res;
	card_type = CARD_TYPE_INVALID;
    /*
    cmd0 set card to idle, cmd8 check the version,
    cmd0 -> cmd8 -> r7 ->cmd5 ->r4 ->cmd3 ->r6 ->cmd7 ->r1b
    */

	/*Invoke CMD0 */
    uart_printf("Send CMD0\n");
	//SdCard_MsWait(50);
	status = SdCard_Cmd_Nodata(0x0000, 0x00000000, base_addr);
	if (status != MMC_ERR_NONE)
	{
		uart_printf("Error Invoke CMD0");
	}
    else
    {
        uart_printf("CMD0 Done!\n");
    }

    /*Invoke CMD8 */
    uart_printf("Send CMD8\n");
	//SdCard_MsWait(50);
#if 1
	cmd = 0x081a;
    /*set ocr value to the voltage, 0xAA is check pattern*/
	argv = ((MMC_OCR_AVAIL & 0xFF8000) != 0) << 8 | 0xAA;
	status = SdCard_Cmd_Nodata(cmd , argv, base_addr);
	if (status != MMC_ERR_NONE)
	{
		uart_printf("Error Invoke CMD8, version1.0 or No SD inserted!\n");
		return MMC_ERR_CMD_TOUT;//add by yazhouren@asrmicro.com
	}
    else
	{
	    UINT8 result = SD_RESPONSE[0]&0xFF;
        if(result != 0xAA)
        {
            uart_printf("cmd8 error\n");
            return MMC_ERR_INVALID;
        }

        /*cmd8's response is r7, r7 equal resp0 in this program,
                resp0[15:8]=voltage, resp0[7:0]=check pattern*/
        uart_printf("Response CMD8: %x\n", SD_RESPONSE[0]);
		uart_printf("voltage: %x \r\n", SD_RESPONSE[1]);
        uart_printf("CMD8 Done!\n");
	}
#endif
    CMD8_timout_cnt = 0;
 	//send ACMD41
	count = 400;
	//SdCard_MsWait(50);
	do{
		cmd = 0x371a;
		argv = 0x0;
		status = SdCard_Cmd_Nodata(cmd , argv, base_addr);

		if(status != MMC_ERR_NONE)
		{
			uart_printf("Error Invoke CMD55");
			return MMC_ERR_CMD_TOUT;
		}

		//send command with HCS=1(BIT30) and S18R=1(BIT24)
		cmd = 0x2902;
		//argv = 0x40ff8000;
		argv = 0x41300000;
		status = SdCard_Cmd_Nodata(cmd,argv,base_addr);
		if(status != MMC_ERR_NONE)
		{
			uart_printf("Error Invoke CMD41\n");
			return MMC_ERR_CMD_TOUT;
		}

		res = SD_RESPONSE[1];

		count --;
		//uart_printf("ACMD41 send\n");
		SdCard_MsWait(10);
	}
	while((count > 0) && ((res & 0x8000) == 0x0));

	//check BIT31
	if((res & 0x8000) == 0)
	{
		uart_printf("sd card init failed!response:0x%x,0x%x\n",SD_RESPONSE[0],SD_RESPONSE[1]);

	}
	//check BIT30, CCS
	if(res & 0x4000)
	{
		card_type = CARD_TYPE_HIGH;
		uart_printf("high capacity card\n");
	}
	else
	{
		card_type = CARD_TYPE_STANDARD;
		uart_printf("standard card\n");
	}

    //only enable UHS for Samsung platform
    #if 0 //add by yazhouren@asrmicro.com
    if((PlatformGetProjectType() == NEZHA_SMIFI_V2) || (PlatformGetProjectType() == NEZHA_SMNAND_V2))
    {
	    if(res & 0x100)
	    {
		    hs_switch = 1;
		    uart_printf("This card can switch to HS mode");
	    }
	}
	#endif
	uart_printf("ACMD41,response:0x%x,0x%x\n",SD_RESPONSE[0],SD_RESPONSE[1]);

	if(hs_switch == 1)
	{
		//send cmd11
		argv = sdcard_readl(base_addr, SD_PRESENT_STAT_0_offset);
		uart_printf("before send cmd11, present state:0x%x",argv);
		uart_printf("Send CMD11");
		cmd = 0x0b3a;
		argv = 0;
		status = SdCard_Cmd_Nodata(cmd,argv,base_addr);
		if(status != MMC_ERR_CMD_TOUT)
		{
			uart_printf("CMD11 response OK");
			SdCard_MsWait(10);
			argv = sdcard_readl(base_addr, SD_PRESENT_STAT_0_offset);
			uart_printf("before disable clock, present state:0x%x",argv);
			//disable SD clock
			cmd = sdcard_readw(base_addr, SD_CLOCK_CTRL_offset);
			cmd = cmd & 0xFFFB;
			sdcard_writew(base_addr, SD_CLOCK_CTRL_offset, cmd);
			SdCard_MsWait(2);

			//modify Host control 2, 1.8v signaling enable
			cmd = sdcard_readw(base_addr, 0x3E);
			uart_printf("before change, host control2:0x%x",cmd);
			cmd = cmd | 0x8;
			sdcard_writew(base_addr, 0x3E, cmd);

			//modify host control 1 register
			cmd = sdcard_readw(base_addr, SD_HOST_CTRL_offset);
			cmd = cmd & 0xff;
			cmd = cmd | 0xb00;
			sdcard_writew(base_addr, SD_HOST_CTRL_offset, cmd);
			uart_printf("host control1:0x%x",cmd);

			//change the voltage to 1.8v
			PMIC_Set_SDIO_1_8V();
			SdCard_MsWait(10);
			cmd = sdcard_readw(base_addr, 0x3E);
			uart_printf("change voltage, host control2:0x%x",cmd);
			//modify SD FIFO param, enable  force clock on
			argv = sdcard_readl(base_addr, 0x104);
			uart_printf("1 SD FIFO param:0x%x",argv);

			argv = argv | 0x1800;
			sdcard_writel(base_addr, 0x104, argv);


			argv = sdcard_readl(base_addr, SD_PRESENT_STAT_0_offset);
			while((argv & 0xf00000) != 0xf00000)
			{
				argv = sdcard_readl(base_addr, SD_PRESENT_STAT_0_offset);
				uart_printf("present state:0x%x",argv);
			}
			uart_printf("Enable Clock,present state:0x%x",argv);

            SdCard_MsWait(5);

			//disable clock force sd
			argv = sdcard_readl(base_addr, 0x104);
			uart_printf("2 SD FIFO param:0x%x",argv);

			argv = argv & 0xFFFFE7FF;
			sdcard_writel(base_addr, 0x104, argv);
			uart_printf("disable force on, SD FIFO param:0x%x", sdcard_readl(base_addr, 0x104));

			//enable sd clock
			SdCard_Enable_Clock(base_addr,0x300);
			SdCard_MsWait(5);

		}
		else
		{
			uart_printf("Error Invoke CMD11\n");
		}
	}
	//clear cmd8 timeout count
	CMD8_timout_cnt = 0;
	//SdCard_MsWait(20);

	uart_printf("Send CMD2\n");
	cmd = 0x0209;
	argv = 0x0;
	status = SdCard_Cmd_Nodata(cmd,argv,base_addr);
	if(status != MMC_ERR_NONE)
	{
		uart_printf("Error Invoke CMD2");
        return MMC_ERR_CMD_TOUT;
	}
	//SdCard_MsWait(50);
	/*Invoke CMD3, cmd3 get the RCA*/
    //uart_printf("Send CMD3");
	cmd = 0x031a;
	argv = 0x0;
	status = SdCard_Cmd_Nodata(cmd,argv, base_addr);
	if (status != MMC_ERR_NONE)
	{
		uart_printf("Error Invoke CMD3");
        return MMC_ERR_CMD_TOUT;
	}
    else
	{
        /*read RCA, cmd3's response is R6, R6[39:24] is RCA, R6[23:8] is card status
        in this program, resp1 is RCA*/
    	uart_printf("CMD3 Response: %x, %x\n", SD_RESPONSE[0], SD_RESPONSE[1]);
    	SD_RCA = SD_RESPONSE[1] & 0xffff;

    	uart_printf("Card RCA: %x", SD_RCA);
        //uart_printf("CMD3 Done!");
	}

	//SdCard_MsWait(50);
	//send command 9 to read CSD
	status = SdCard_Cmd_Nodata(0x0901, SD_RCA<<16, base_addr);

	if (status != MMC_ERR_NONE)
	{
		uart_printf("Error Invoke CMD9\r\n");
	} else {
		uart_printf("CMD9 Done!\r\n");
	}

	/*decode card CSD*/
	SD_Decode_CSD(base_addr);

	/*Invoke CMD7, cmd7 is select card, its argument[31:16] is RCA,other bits are stuff*/
	argv = SD_RCA << 16;
	//argv = 0x10000;
	status = SdCard_Cmd_Nodata(0x071b, argv, base_addr);
	if (status != MMC_ERR_NONE)
	{
		uart_printf("Error Invoke CMD7\n");
        return MMC_ERR_CMD_TOUT;
	}
    else
    {
        /*cmd7's response is R1b, R1b is identical to R1 with an optional busy
        signal transmitted on the data line, R1[39:8] is card status, in this program,
        resp0 and resp1 is represent these bits*/
        //uart_printf("CMD7 Response: %x, %x", SD_RESPONSE[0], SD_RESPONSE[1]);
		//uart_printf("CMD7 Done!");
	}
	//SdCard_MsWait(50);

	//uart_printf("card initialization and identification passed,enter into transfer state!\n");
    return 0;
}

static int sdcard_enable_highspeed(void)
{
	UINT16 cmd = 0;
	UINT16 value;
	UINT32 base_addr = SD1_HOST_CTRL_ADDR;
	UINT32 status = 0, argv;

	cmd = 0x063a;
	argv = 0x80fffff1;

	status = SdCard_Cmd_Nodata(cmd, argv, base_addr);

	if (status != MMC_ERR_NONE)
	{
		uart_printf("Error Invoke CMD6");
        return MMC_ERR_CMD_TOUT;
	}

	SdCard_MsWait(100);

	value = sdcard_readw(base_addr, SD_HOST_CTRL_offset);
	uart_printf("before set highspeed,host control:0x%x\n",value);
	value |=  (1<<2);
	sdcard_writew(base_addr, SD_HOST_CTRL_offset, value);
	uart_printf("after set highspeed,host control:0x%x\n",value);

	uart_printf("sd card set highspeed success!\n");
	return 0;
}

static int sdcard_set_width(UINT32 bus_width)
{
	UINT32 base_addr = SD1_HOST_CTRL_ADDR;
	UINT16 value;
	UINT16 cmd = 0;
	UINT32 argv = 0;
	UINT32 status = 0;

	value = sdcard_readw(base_addr, SD_HOST_CTRL_offset);
	uart_printf("original host control:0x%x\n",value);
	value = (value & 0xfffd) | (bus_width<<1);
	uart_printf("modified host control:0x%x\n",value);
	sdcard_writew(base_addr, SD_HOST_CTRL_offset, value);
	SdCard_MsWait(100);

	cmd = 0x371a;
	argv = SD_RCA << 16;

	status = SdCard_Cmd_Nodata(cmd, argv, base_addr);
	if (status != MMC_ERR_NONE)
	{
		uart_printf("Error Invoke CMD55");
        return MMC_ERR_CMD_TOUT;
	}

	cmd = 0x061a;
	argv = bus_width<<1;

	status = SdCard_Cmd_Nodata(cmd, argv, base_addr);
	if (status != MMC_ERR_NONE)
	{
		uart_printf("Error Invoke ACMD6");
        return MMC_ERR_CMD_TOUT;
	}

	uart_printf("sd card set buswidth success!\n");
	return 0;
}

static void sdcard_config_pin()
{
    
    volatile unsigned long *r;
    #if 0 //add by yazhouren@asrmicro.com
	PlatformBoardType board_type = NEZHA_MAX_BOARD_TYPE;
	board_type = PlatformGetBoardType();
    switch(board_type)
    {
        case NEZHA_MMIFI_V4_BOARD:
        {
            Sdcard_clear_gpio49_bit(); //added by notion 20140319
            break;
        }

        default:
        {
            break;
        }
    }
	#endif
    //uart_printf("==pin config start==");

    sdcard_card_detect_config();

#ifdef PLAT_CRANE
    //r = (volatile unsigned long*)(0xD401E2C0);  //GPIO70
    //*r = 0x10C0;

/*
    r = (volatile unsigned long*)(0xD401E0AC);  //card detect
    uart_printf("The default MMC_CD value:0x%x\n",*r);
    *r = 0x10A0;
	uart_printf("The modified MMC_CD value:0x%x\n",*r);
*/
    r = (volatile unsigned long*)(0xD401E0A8);  //clk
    *r = 0x10C0;

    r = (volatile unsigned long*)(0xD401E0A4);  //cmd
    *r = 0x10C0;
//	*r = 0x10C0; //config as gpio17

    r = (volatile unsigned long*)(0xD401E0A0);  //data0
    *r = 0x10C0;

    r = (volatile unsigned long*)(0xD401E09C);  //data1
    *r = 0x10C0;

    r = (volatile unsigned long*)(0xD401E098);  //data2
    *r = 0x10C0;

    r = (volatile unsigned long*)(0xD401E094);  //data3
    *r = 0x10C0;


    //config GPIO70 as output
    //r = (volatile unsigned long*)(0xD4019008 + 0x0C);
    //*r |= 0x40;

    //r = (volatile unsigned long*)(0xD4019008 + 0x18);
    //*r |= 0x40;   //GPIO70 to high level

#endif

    //uart_printf("==pin config end==");

}

static sdcard_card_detect_config()
{
    volatile unsigned long *r;
    //uart_printf("==card detect pin config==");
    //0xac, function0, card detect
    r = (volatile unsigned long*)(0xD401E0ac);  //card detect
    *r = (*r & 0xFFFF0000) | 0xD0C0;
    uart_printf("card detect:0x%x\r\n",*r);
}
void sdcard_burstsize_config()
{
    volatile UINT16* ptr16;
    UINT32       base_addr = SD1_HOST_CTRL_ADDR;

    /*set sd_clk delay */
	ptr16 = (UINT16*) (base_addr + SD_CLOCK_AND_BURST_SIZE_SETUP_offset);
	*ptr16 = 0x03C5;
    uart_printf("burst set %x", *((UINT16*) (base_addr + SD_CLOCK_AND_BURST_SIZE_SETUP_offset)));
}

/**
 *  @brief: This function enable or disable card interrupt.
 *
 *  @enable: 1 is enable, 0 is disable
 */
void sd_card_int(UNSIGNED_INT enable)
{
    UINT16 isr;

    isr = sdcard_readw(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_EBLE_offset);
    if(enable)
        isr |= CARD_INT;
    else
        isr &= ~CARD_INT;
    sdcard_writew(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_EBLE_offset, isr);
    sdcard_writew(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_INTR_EBLE_offset, isr);
}

void sd_dumpregs(UINT32 base)
{
    UINT16 isr_sts = 0;

	uart_printf("SD card error: 0x%x", sdcard_readl(base, SD_PRESENT_STAT_0_offset));

	uart_printf(": ============== REGISTER DUMP ==============\r\n");
	uart_printf(": DMA addr: 0x%08x | Version:  0x%08x\r\n",
		sdcard_readl(base, SD_SYSADDR_LOW_offset),
		sdcard_readw(base, SDHCI_HOST_VERSION));
	uart_printf(": Blk size: 0x%08x | Blk cnt:  0x%x\r\n",
		sdcard_readw(base, SD_BLOCK_SIZE_offset),
		sdcard_readw(base, SD_BLOCK_COUNT_offset));
	uart_printf(": Argument: 0x%08x | Trn mode: 0x%08x\r\n",
		sdcard_readl(base, SD_ARG_LOW_offset),
		sdcard_readw(base, SD_TRANSFER_MODE_offset));
	uart_printf(": Present:  0x%08x | Host ctl: 0x%08x\r\n",
		sdcard_readl(base, SD_PRESENT_STAT_0_offset),
		sdcard_readw(base, SD_HOST_CTRL_offset));
	uart_printf(": Host control2: 0x%08x", sdcard_readw(base,0x3e));
	uart_printf(": Power:    0x%08x | Blk gap:  0x%08x\r\n",
		sdcard_readb(base, SDHCI_POWER_CONTROL),
		sdcard_readw(base, SD_BGAP_CTRL_offset));
	uart_printf(": Wake-up:  0x%08x | Clock:    0x%08x\r\n",
		sdcard_readb(base, SDHCI_WAKE_UP_CONTROL),
		sdcard_readw(base, SD_CLOCK_CTRL_offset));
	uart_printf(": Timeout:  0x%08x | Int stat: 0x%08x\r\n",
		sdcard_readb(base, SD_SW_RESET_CTRL_offset),
		sdcard_readl(base, SD_NORM_INTR_STS_offset));
	uart_printf(": Int enab: 0x%08x | Sig enab: 0x%08x\r\n",
		sdcard_readl(base, SD_NORM_INTR_STS_EBLE_offset),
		sdcard_readl(base, SD_NORM_INTR_STS_INTR_EBLE_offset));
	uart_printf(": AC12 err: 0x%08x | Slot int: 0x%08x\r\n",
		sdcard_readw(base, SDHCI_ACMD12_ERR),
		sdcard_readw(base, SDHCI_SLOT_INT_STATUS));
	uart_printf(": Caps:     0x%08x | Max curr: 0x%08x\r\n",
		sdcard_readl(base, SDHCI_CAPABILITIES),
		sdcard_readl(base, SDHCI_MAX_CURRENT));
	uart_printf(": Command:  0x%08x\r\n",
		sdcard_readw(base, SD_CMD_offset));
#ifndef PLAT_CRANE
	uart_printf(": Fifo_prm: 0x%08x | Burst set: 0x%08x\r\n",
		sdcard_readw(base, SD_FIFO_PARA_offset),
		sdcard_readw(base, SD_CLOCK_AND_BURST_SIZE_SETUP_offset));
#endif
//	uart_printf("SPI mode:0x%x\n",sdcard_readw(base, 0xe4));
	uart_printf(": ===========================================\r\n");

	if(eeGetSystemAssertFlag())
	{
        isr_sts = sdcard_readw(base, SD_NORM_INTR_STS_offset);
        if(isr_sts & ERR_INT )
        {
            /*FSInit_Status_Init();*/
            sdcard_initialized = SDCARD_INIT_UNDO;
            uart_printf("Force fs/SD to be uninitialized!");
			sdcard_writel(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_EBLE_offset, 0x0);
			sdcard_writel(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_INTR_EBLE_offset, 0x0);
        }
	}
}

#define SD_INSERT_CARD_FLAG_BIT 0x1
#define SD_REMOVE_CARD_FLAG_BIT 0x2

#define SD_RESET_CMD_FLAG_BIT 0x1

#ifdef FAT32_FILE_SYSTEM
FATFS fatfs_fs[2];
#endif
void sdcard_reinit(void)
{
    UINT32 base_addr = SD1_HOST_CTRL_ADDR;
	UINT32 ret;
	sdcard_reinitialized = SDCARD_INIT_UNDO;
    UINT16 value;

	if(sdcard_initialized == SDCARD_INIT_UNDO)
	{
	    uart_printf("SD device haven't init before\n");
		return;
    }
	
    PMIC_Set_SDIO_2_8V();
	
    sdcard_initialized = SDCARD_INIT_UNDO;
	uart_printf("Insert Card, re-init command\n");
    sdcard_set_power();

	SdCard_Enable_Clock(base_addr, 0x300);
    SdCard_MsWait(50);

    sdcard_set_mode(CARD_MODE_INT);

/*
    //set buswidth as 1 bit
	value = sdcard_readw(base_addr, SD_HOST_CTRL_offset);
	uart_printf("original host control:0x%x\n",value);
	value = value & 0xfffd ;
	uart_printf("modified host control:0x%x\n",value);
	sdcard_writew(base_addr, SD_HOST_CTRL_offset, value);
	SdCard_MsWait(100);
*/
	SdCard_SW_Reset(base_addr);
	sd_dumpregs(base_addr);

    CMD8_timout_cnt = 0;

	ret = sdcard_cmd_init();

	if(ret)
		goto RE_INIT_ERR;

	SdCard_Enable_Clock(base_addr, 0x2);
	sdcard_writeb(base_addr, SD_SW_RESET_CTRL_offset, 0x0E);

	//0 for 1bit and 1 for 4bit bus width
	ret = sdcard_set_width(1);
	if(ret)
		goto RE_INIT_ERR;

    UINT32 TX_CFG;
    //keep the hold time for default speed mode
    TX_CFG = sdcard_readl(base_addr, 0x118);
    uart_printf("1 TX_CFG:0x%x",TX_CFG);
    TX_CFG = TX_CFG | 0x40000000;
    sdcard_writel(base_addr, 0x118, TX_CFG);
    uart_printf("2 TX_CFG:0x%x",sdcard_readl(base_addr, 0x118));

	uart_printf("==sdcard re-init OK==\r\n");

	sdcard_initialized = SDCARD_INIT_SUCCESS;
	sdcard_reinitialized = SDCARD_INIT_SUCCESS;
	card_status = CARD_STATUS_INSERT;

#ifdef FAT32_FILE_SYSTEM
	SD_Infinite_Err_Flag = 0;
	////////Fat32_Init();
	FRESULT res = f_mount(&fatfs_fs[0], DUMP_VOL, 1);
	extern int sdcard_fatfs_mount_status_g;
	if (res == FR_OK) {
		sdcard_fatfs_mount_status_g = 1;
		fatal_printf("SDCARD is mounted sucessfully\n");
	}
#endif
		return;

RE_INIT_ERR:
	uart_printf("==sdcard re-init failed==\r\n");
	sdcard_initialized = SDCARD_INIT_FAILED;
	sdcard_reinitialized = SDCARD_INIT_FAILED;
	return;
}

#if 0
void SD_ResetCMD_Task()
{
    OSA_STATUS status;
    UINT32 flag_value,cmd_value;
    UINT32 flag_mask = SD_RESET_CMD_FLAG_BIT;
    SD_Decrease_Clk_Flag = 0;
	while(1)
	{
		//wait for flag
		uart_printf("SD_ResetCMD_Task wait\n");
		status = OSAFlagWait(SD_CMDReset_FlgRef, flag_mask,OSA_FLAG_OR_CLEAR,&flag_value,OSA_SUSPEND);
		ASSERT(status == OS_SUCCESS);
		if(flag_value & SD_RESET_CMD_FLAG_BIT)
		{
			cmd_value = sdcard_readl(SD1_HOST_CTRL_ADDR, SD_TRANSFER_MODE_offset);
			//just for command8
            uart_printf("cmd_value:0x%x, err_status:0x%x\n",cmd_value,SD_ERR_INTSTATUS);

			if(sdcard_initialized != SDCARD_INIT_SUCCESS)
				CMD8_timout_cnt++;
		    else
		        SD_Infinite_Err_Flag = 1;

			if(CMD8_timout_cnt >= 3)
			{
				CMD8_timout_cnt = 0;
				SD_Infinite_Err_Flag = 1;
			}
			else if(SD_Infinite_Err_Flag == 1)
			{
			    if(SD_Decrease_Clk_Flag == 0)
			    {
			        SD_Decrease_Clk_Flag = 1;

			        uart_printf("decrease clk here");

			        SdCard_Enable_Clock(SD1_HOST_CTRL_ADDR, 3);
			        SdCard_SW_Reset(SD1_HOST_CTRL_ADDR);
			        SD_Infinite_Err_Flag = 0;
			    }else
			    {
			        //sd_dumpregs(SD1_HOST_CTRL_ADDR);
				    SdCard_SW_Reset(SD1_HOST_CTRL_ADDR);
				    //send command again
				    sdcard_writel(SD1_HOST_CTRL_ADDR, SD_TRANSFER_MODE_offset, cmd_value);
			    }
			}

		}
	}
}

void SD_CARD_Insert_Remove_Task()
{
    OSA_STATUS status;
    UINT32 flag_value,cmd_value;
    UINT32 flag_mask = SD_INSERT_CARD_FLAG_BIT | SD_REMOVE_CARD_FLAG_BIT;

	while(1)
	{
		//wait for flag
		uart_printf("SD_InsertRemove_Task wait\n");
		status = OSAFlagWait(SDFlgRef, flag_mask,OSA_FLAG_OR_CLEAR,&flag_value,OSA_SUSPEND);
		ASSERT(status == OS_SUCCESS);

		if(flag_value & SD_INSERT_CARD_FLAG_BIT)
		{
			sdcard_reinit();

            if(sdcard_initialized == SDCARD_INIT_SUCCESS)
            {
                uart_printf("handle insert here!\n");
                card_status = CARD_STATUS_INSERT;
                /*USB2MgrDeviceUnplugPlug_SD();*/ //yazhouren@asrmicro.com
            }
		}

		if(flag_value & SD_REMOVE_CARD_FLAG_BIT)
		{
			uart_printf("handle remove here!\n");
			card_status = CARD_STATUS_REMOVE;
			/*USB2MgrDeviceUnplugPlug_SD();*/ //yazhouren@asrmicro.com
		}
	}
}

static void sdcard_cmderr_hisr(void)
{
	//set flag
		OS_STATUS os_status;
		uart_printf("cmderr_hisr\n");
		os_status = OSAFlagSet (SD_CMDReset_FlgRef, SD_RESET_CMD_FLAG_BIT, OSA_FLAG_OR);
		ASSERT(os_status==OS_SUCCESS);
}

static void sdcard_insert_hisr(void)
{
	//set flag
		OS_STATUS os_status;
		//uart_printf("sdcard_insert_hisr\n");
		os_status = OSAFlagSet (SDFlgRef, SD_INSERT_CARD_FLAG_BIT, OSA_FLAG_OR);
		ASSERT(os_status==OS_SUCCESS);
}

static void sdcard_remove_hisr(void)
{
	//set flag
		OS_STATUS os_status;
		//uart_printf("sdcard_insert_hisr\n");
		os_status = OSAFlagSet (SDFlgRef, SD_REMOVE_CARD_FLAG_BIT, OSA_FLAG_OR);
		ASSERT(os_status==OS_SUCCESS);
}

static void sdcard_DATA_hisr(void)
{
	OSA_STATUS status;
	UINT32 count;
	OSASemaphorePoll(SDDATARef, &count);

	if(count == 0)
	    OSASemaphoreRelease_sdcard(SDDATARef);
//	status = OSASemaphoreRelease_sdcard(SDDATARef);
//	ASSERT(status == OS_SUCCESS);
}

static void sdcard_task_init()
{
	OS_STATUS   status;
	status = OSASemaphoreCreate (&SDSysRef, 1, OSA_FIFO);
	ASSERT(status == OS_SUCCESS);

//	status = OSASemaphoreCreate(&SDCMDRef, 0, OSA_FIFO);
//	ASSERT(status == OS_SUCCESS);

	status = OSASemaphoreCreate(&SDDATARef, 0, OSA_FIFO);
	ASSERT(status == OS_SUCCESS);

	status = OSAFlagCreate(&SDFlgRef);
    ASSERT(status == OS_SUCCESS);

	status = OSAFlagCreate(&SD_CMDReset_FlgRef);

	//create task for reset command when command timeout happens
    status = OSATaskCreate(&SDTaskRef, SDStack, 2048, 42, "SD_Reset_TASK", SD_ResetCMD_Task, NULL);
    ASSERT(status == OS_SUCCESS);

	//create task to handle card insert and remove interrupt
    status = OSATaskCreate(&SDCard_InsertRemove_TaskRef, SDCard_InsertRemove_Stack, 2048, 90, "SD_Card_TASK", SD_CARD_Insert_Remove_Task, NULL);
    ASSERT(status == OS_SUCCESS);

	//create HISR
    OS_Create_HISR(&SdcardHISR, "sdcardhisr", sdcard_cmderr_hisr, HISR_PRIORITY_2);

	OS_Create_HISR(&SdcardInsertHISR, "sdcardinserthisr", sdcard_insert_hisr,HISR_PRIORITY_2);
	OS_Create_HISR(&SdcardRemoveHISR, "sdcardremovehisr", sdcard_remove_hisr,HISR_PRIORITY_2);

//	OS_Create_HISR(&SdCMDHISR, "sdcardcmdhisr", sdcard_CMD_hisr,HISR_PRIORITY_2);
	OS_Create_HISR(&SDDATAHISR, "sdcarddatahisr", sdcard_DATA_hisr,HISR_PRIORITY_2);
}
#endif
/**
 *  @brief: This function handle LISR.
 *
 */
//just for lisr interval debug
#if SDCARD_LISR_DBG
static int lisr_start[0x40] = {0};
static int lisr_end[0x40] = {0};
static int lisr_loop = 0;
#endif
void dump_sd_lisr()
{
#if SDCARD_LISR_DBG
	int loop;
	for(loop = 0; loop < 0x40; loop++)
	{
		uart_printf("loop:0x%x,start:0x%x,interval:0x%x\n",loop,lisr_start[loop],(lisr_end[loop] - lisr_start[loop]));
	}
#endif
}

//extern unsigned long sd_dbg_tick;
#if 0
static void sdcard_isr(INTC_InterruptInfo sourceInfo)
{
	UINT16 isr_sts = 0;
    UINT16 err_sts = 0;
    int    cardint = 0,status;

#if SDCARD_LISR_DBG
	lisr_start[lisr_loop] = rti_get_current_tick();
#endif
	if (SDCARD_CTRL_BASE_ADDR == 0)
	{
		return;
	}

    isr_sts = sdcard_readw(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_offset);
    if (isr_sts == 0 || isr_sts == 0xFFFF)
    {
#if SDCARD_LISR_DBG
        uart_printf("Err isr sts:%x\r\n", isr_sts);
#endif
		return;
	}

    sdcard_writew(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_offset, isr_sts);

    if (isr_sts & CARD_INS)
    {
        uart_printf("CARD Insert interrupt: %x\r\n", isr_sts);
		OS_Activate_HISR(&SdcardInsertHISR);

    }

	if(isr_sts & CARD_REM)
	{
		uart_printf("CARD Remove interrupt: %x\r\n", isr_sts);
		OS_Activate_HISR(&SdcardRemoveHISR);
	}

    if (isr_sts & (CMD_COMP))
    {
        SD_CMD_COMP_status = 1;
    }

    if (isr_sts & (XFER_COMP))
    {
        SD_DATA_COMP_status = 1;
        //uart_printf("DATA");
        if(SD_DATA_START_status == 1)
        {
			OS_Activate_HISR(&SDDATAHISR);
			SD_DATA_START_status = 0;
        }
    }

	if(isr_sts & (RX_RDY))
	{
		uart_printf("Buffer ready,isr_sts:0x%x",isr_sts);
		SD_CMD_BufferReady = 1;
	}

    if (isr_sts & CARD_INT)
    {
        cardint = 1;
        uart_printf("Card interrupt\n");
        /* For debug*/
        sd_card_int(0);
    }

	if (isr_sts & DMA_INT)
	{
		sdcard_writel(SDCARD_CTRL_BASE_ADDR, SD_SYSADDR_LOW_offset,sdcard_readl(SDCARD_CTRL_BASE_ADDR,SD_SYSADDR_LOW_offset));
		//uart_printf("DMA interrupt\n");
	}

	if (isr_sts & ERR_INT )
	{
        err_sts = sdcard_readw(SDCARD_CTRL_BASE_ADDR, SD_ERR_INTR_STS_offset);
        sdcard_writew(SDCARD_CTRL_BASE_ADDR, SD_ERR_INTR_STS_offset, err_sts);
        SD_ERR_INTSTATUS = err_sts;
        #if 0
        if(FSInit_Status_Get() != 1)
    	    uart_printf("isr_sts:0x%x,err_sts:0x%x",isr_sts,err_sts);
        #endif
        if (err_sts & (CMD_TO_ERR | CMD_IDX_ERR | CMD_END_BIT_ERR | CMD_CRC_ERR))
        {
    		if (err_sts & CMD_TO_ERR)
    		{
    			/*if(FSInit_Status_Get() != 1)
    			    uart_printf("Command timeout error");*/
				status = OS_Activate_HISR(&SdcardHISR);
    	    }
    		if (err_sts & CMD_IDX_ERR)
    		{
    			SD_Infinite_Err_Flag++;
                //uart_printf("Command index error!\r\n");
            }
    		if (err_sts & CMD_END_BIT_ERR)
    		{
    			SD_Infinite_Err_Flag++;
                //uart_printf("Command end bit error!\r\n");
    		}
    		if (err_sts & CMD_CRC_ERR)
    		{
    			SD_Infinite_Err_Flag++;
                //uart_printf("Command CRC error!\r\n");
    		}
        }else{
			SD_Infinite_Err_Flag++;
        }

		if((SD_Infinite_Err_Flag == 1) && (sdcard_initialized == SDCARD_INIT_SUCCESS))
		{
		    if(SD_Decrease_Clk_Flag == 0)
		        status = OS_Activate_HISR(&SdcardHISR);
		    else
		    {
		        /*if(FSInit_Status_Get() != 1)
		            uart_printf("disable interrupt here!");*/

			    OS_Activate_HISR(&SDDATAHISR);
			    sdcard_writel(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_EBLE_offset, 0x0);
			    sdcard_writel(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_INTR_EBLE_offset, 0x0);
		    }
		}
	}

#if SDCARD_DEBUG
    uart_printf("leave sdcard_isr\r\n");
#endif

#if SDCARD_LISR_DBG
	lisr_end[lisr_loop] = rti_get_current_tick();
	lisr_loop++;

	if(lisr_loop == 0x40)
		lisr_loop = 0;
#endif
}
#endif



UINT32 sdcard_read_multiblks(UINT32 base, UINT32 buffer, UINT32 loop, UINT32 blk_cnt);
UINT32 sdcard_write_multiblks(UINT32 base, UINT32 buffer, UINT32 loop, UINT32 blk_cnt);
UINT32 sdcard_read_singleblk(UINT32 base, UINT32 buffer, UINT32 loop);
UINT32 sdcard_write_singleblk(UINT32 base, UINT32 buffer, UINT32 loop);



#define TEST_SIZE 0x2000 //0x400000 //512*1024*8

//0 for un-init, 1 for init success, 2 for init failed
//0 for un-init, 1 for standard, 2 for high capacity
UINT32 sdcard_get_status(void)
{
	if(sdcard_initialized == SDCARD_INIT_SUCCESS)
		return 1;
	else
		return 0;
}

UINT32 sdcard_get_blknum(void)
{
	return SD_BLK_NUM;
}

UINT32 sdcard_get_mode(void)
{
	return card_mode;
}

UINT32 sdcard_set_mode(UINT32 mode)
{
	card_mode = mode;
}

BOOL sdcard_transmit_status(void)
{
    if(SD_Trans_Status == 1)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
static void SdCard_Check_TransStatus(UINT32 base);

int eeSDDump_status_get(void)
{
    return ee_sd_dump_status_g;
}

UINT32 sdcard_ee_dump_folder_index = 0;
#if 0
void eeSDDumpMakeDir(void)
{
	FAT32FILEINFO fileinfo_ptr;
	unsigned int handle;
	int ret = 0;
	UINT32 m_ind;
	char str[128] = "\0";
    static int flag = 0;
	
	if (flag != 0)
		return;
	flag = 1;
	
	sprintf(str, "%s\\%s*",DUMP_VOL ,DUMP_DIR_PRENAME);
	handle = FAT_findfirst(str, &fileinfo_ptr);
    
	if (handle != 0) {
		sdcard_ee_dump_folder_index = m_atoi_dec((char *)fileinfo_ptr.info.m_lpszFullName + m_strlen(DUMP_DIR_PRENAME));
		if(sdcard_ee_dump_folder_index == 0xFFFFFFFF)
			sdcard_ee_dump_folder_index =0;
		while(!ret){
			ret = FAT_findnext(handle, &fileinfo_ptr);
			if (ret == 0) {
				m_ind = m_atoi_dec((char *)fileinfo_ptr.info.m_lpszFullName + m_strlen(DUMP_DIR_PRENAME));
				if(m_ind == 0xFFFFFFFF) continue;
				sdcard_ee_dump_folder_index = MAX(m_ind,sdcard_ee_dump_folder_index);
			}
		}
		FAT_findfileclose(handle);
	}else{
		// TODO: tmp workaround to make an Empty dir here , or findfirst would failed, have to fix further
		//sprintf(str, "%s\\%sN",DUMP_VOL ,DUMP_DIR_PRENAME);
		//FAT_MakeDir(str, NULL);
	}

	//mkdir
	sprintf(str, "%s\\%s%d",DUMP_VOL ,DUMP_DIR_PRENAME,++sdcard_ee_dump_folder_index);
	FAT_MakeDir(str, NULL);
	uart_printf("eeSDDumpMakeDir end\r\n");
}
#endif
void eeSDDumpPrepare(void)
{
#ifdef FAT32_FILE_SYSTEM
	UINT32 isr_sts;
    UINT32 base = SD1_HOST_CTRL_ADDR;
	UINT32 timeout = 40000;
    ee_sd_dump_status_g = 1;
    uart_printf("EE LOG: %s Enter\n", __func__);
    if (sdcard_is_ready())
    {
    	//set sd card as polling mode
        sdcard_set_mode(1);
		//set sd card with lower clock
		SdCard_Enable_Clock(base, 3);
		//Enable SD card interrupt
		SdCard_IRQ_Enable(base, 0xF1FF);
		//enable error interrupt
	    sdcard_writew(base, SD_ERR_INTR_STS_EBLE_offset, 0x3ff);
    	sdcard_writew(base, (SD_ERR_INTR_STS_EBLE_offset + 0x4), 0x3ff);

		if((SD_Read_status == 1)||(SD_Write_status != 0))
		{
			uart_printf("SD transfer hasn't completed,%d,%d!!!\n",SD_Read_status,SD_Write_status);
            if(SD_Write_status != 1)
                SD_Trans_Status = 1;

			//wait for data transfer end interrupt
			if(SD_Write_status != 2)
			{
				isr_sts = sdcard_readw(base, SD_NORM_INTR_STS_offset);
				while ( !(isr_sts & XFER_COMP))
				{
					if(isr_sts & DMA_INT)
					{
						sdcard_writew(base, SD_NORM_INTR_STS_offset, DMA_INT);
						sdcard_writel(base, SD_SYSADDR_LOW_offset,sdcard_readl(base,SD_SYSADDR_LOW_offset));
						//uart_printf("DMA interrupt\n");
						timeout = 40000;
					}
	    			SdCard_Wait(2000);
					timeout--;
					if (timeout == 0 )
					{
						if(isr_sts & ERR_INT )
						{
      		  				isr_sts = sdcard_readw(base, SD_ERR_INTR_STS_offset);
       						sdcard_writew(base, SD_ERR_INTR_STS_offset, isr_sts);
    	        			uart_printf("err_sts:0x%x\n",isr_sts);
							SdCard_SW_Reset(base);
							timeout = 40000;
						}else
						{
							uart_printf("POLL mode,Wait for data transfer time out!\n");
							sd_dumpregs(base);
							//modify by qgwang, 20131015
							break;
						}
					}
					isr_sts = sdcard_readw(base, SD_NORM_INTR_STS_offset);
				}
				//clear data complete interrupt
				sdcard_writew(base, SD_NORM_INTR_STS_offset, XFER_COMP);
			}

			//wait for card states change
			SdCard_Check_TransStatus(base);
		}
/*
   		if(DumpFileToSDEnable())
    	{
    	    if(FAT32_IsFormat() == 1)
    	    {
    	        sdcard_check_free_space(0x6400000, TRUE);
       		    Create_CurDir();
       		}
       		else
       		{
       		    uart_printf("Non-fat32 file system, don't create current dir!");
   		    }

    	}

    	diag_comm_create_assert_sdl();
*/

    }
    else if(sdcard_is_insert())
    {
        uart_printf("Non-fat32 file system, don't save dump bin!\n");
    }

    uart_printf("EE LOG: %s Exit\n", __func__);
#endif

}

#if 1
void sdcard_test(UINT32 base)
{
	UINT32 ret = 1;
	UINT32 start, end;
	int loop = 0;
	unsigned char * buf_src;
	unsigned char * buf_dest;
	buf_src = (unsigned char *)malloc(TEST_SIZE);
	buf_dest = (unsigned char *)malloc(TEST_SIZE);
	int loop_big;
	uart_printf("read/write check here");

for(loop_big = 0; loop_big < 0x40; loop_big++)
{
	memset(buf_src, 0x55+loop_big, TEST_SIZE);
	memset(buf_dest, 0x2, TEST_SIZE);


	ret = sdcard_write_singleblk(base, buf_src, loop_big*16);
	ASSERT(ret == 0);
	
//	SdCard_MsWait(20);

	ret = sdcard_read_singleblk(base, buf_dest, loop_big*16);
	ASSERT(ret == 0);

	ret = memcmp(buf_src, buf_dest, 512);
	if(ret != 0)
	{
		uart_printf("r/w check error:0x%x,loop:%d\n",ret,loop_big);
		return ret;
	}

	//uart_printf("loop %d, r/w check right!\n",loop_big);
}
	uart_printf("read/write check here passed!");

	//return;

for(loop_big = 0; loop_big < 0x10; loop_big ++)
{
	start = (UINT32)buf_src;
	for(loop = 0; loop < 8; loop ++)
	{
		memset((void *)start,((loop+loop_big+10)&0xff),512);
		start = start + 512;
	}
//	memset(buf_src, 0xa5, TEST_SIZE);
	memset(buf_dest, 0x0, TEST_SIZE);
	//uart_printf("src:0x%x,dest:0x%x\n",buf_src,buf_dest);


	for(loop = 0; loop < 0x2000; )
	{
		ret = sdcard_write_multiblks(base,(buf_src + loop*512),(loop+0x2000*loop_big),16);
		if(ret)
			return ret;
		loop = loop + 16;
	}

	for(loop = 0; loop < 0x2000; )
	{
		ret = sdcard_read_multiblks(base,(buf_dest + loop*512),(loop+0x2000*loop_big),16);
		if(ret)
			return ret;
		loop = loop + 16;
	}
	ret = memcmp(buf_src, buf_dest, TEST_SIZE);
	if(ret != 0)
	{
		uart_printf("1 loop 0x%x r/w check error:0x%x\n",loop_big,ret);
		return ret;
	}

	memset(buf_dest,0x0,512*8);
	ret = sdcard_write_multiblks(base, buf_src, loop_big, 0x8);
	if(ret)
		return ret;

	ret = sdcard_write_multiblks(base, buf_src, loop_big, 0x8);
	if(ret)
		return ret;

	ret = sdcard_read_multiblks(base, buf_dest, loop_big, 0x8);
	if(ret)
		return ret;

	ret = memcmp(buf_src, buf_dest, 512*8);
	if(ret != 0)
	{
		uart_printf("2 loop 0x%x r/w check error:0x%x\n",loop_big,ret);
		return ret;
	}

	uart_printf("loop 0x%x r/w check right!\n",loop_big);
}

	return;
}
#endif
#if 0
static void sd_isr(INTC_InterruptInfo sourceInfo)
{
	UINT16 slot_int;
	slot_int = sdcard_readw(SD1_HOST_CTRL_ADDR, SDHCI_SLOT_INT_STATUS);

#ifdef PLAT_CRANE
	if((slot_int & 0x1) == 1)
		sdcard_isr(sourceInfo);

	if((slot_int & 0x2) == 2) {
		//sdio_isr(sourceInfo);
		uart_printf("ERR----------ERR---------------ERR SDIO\n");

		uart_printf("ERR----------ERR---------------ERR SDIO\n");
		uart_printf("ERR----------ERR---------------ERR SDIO\n");
		uart_printf("ERR----------ERR---------------ERR SDIO\n");
	}
#else
	if((slot_int & 0x3) == 1)
		sdcard_isr(sourceInfo);
	else if((slot_int & 0x3) == 0)
		sdio_isr(sourceInfo);
#endif
}
#endif
static void SdCard_Switch_Function(UINT32 base_addr)
{
	UINT32 status;
//send CMD6, check function
	UINT16 cmd = 0x63a;
	UINT32 argv = 0xffff;
	UINT8 data_block[64],count;
	uart_printf("Send CMD6");

    memset(data_block, 0, 64);
	status = SdCard_Read_NoDMA(cmd, argv, 64, 1, data_block);

	if (status != MMC_ERR_NONE)
	{
		uart_printf("Error Invoke CMD6\r\n");
	} else {
		uart_printf("CMD6 Done!\r\n");
	}

	for(count = 0; count < 64; )
	{
		uart_printf("data:0x%x,0x%x,0x%x,0x%x",data_block[count],data_block[count+1],data_block[count+2],data_block[count+3]);
		count = count + 4;
	}
//send CMD6, switch function

	cmd = 0x63a;
	argv = 0x80000003;

	status = SdCard_Cmd_Nodata(cmd, argv, base_addr);

	if (status != MMC_ERR_NONE)
	{
		uart_printf("Error Invoke CMD6\r\n");
	} else {
		uart_printf("CMD6 Done!\r\n");
		//modify Host control 2, SDR25 mode
		cmd = sdcard_readw(base_addr, 0x3E);
		uart_printf("before set SDR mode, host control2:0x%x",cmd);
		cmd = cmd | 0x3;
		sdcard_writew(base_addr, 0x3E, cmd);
		uart_printf("SDR104, host control2:0x%x",cmd);
	}
}

/**
 *  @brief: This function tunes the RX windows with the given clock.
 *  @return the delay value
 */
static void SdCard_Tune_Clock(UINT32 base_addr)
{
	UINT8 delay, match;
	UINT16 count;
	UINT32 rx_cfg;
	UINT8 data_block[64];
	match = 1;

	memset(data_block, 0, 64);

	rx_cfg = sdcard_readl(base_addr, 0x114);
	uart_printf("The default rx_cfg value:0x%x",rx_cfg);
//	count = sdcard_readw(base_addr, 0x3E);
//	uart_printf("before tune, host control2:0x%x",count);
//	count = count | 0x40;
//	sdcard_writew(base_addr, 0x3E, count);
//	uart_printf("Enable execute tune, host control2:0x%x",count);
    SdCard_MsWait(5);

	for(delay = 0; match == 1; delay++)
	{
		rx_cfg = rx_cfg & 0xffff0000;
		rx_cfg = rx_cfg | (delay<<8) | 0x6;
		uart_printf("The rx_cfg value:0x%x",rx_cfg);
		sdcard_writel(base_addr, 0x114, rx_cfg);

		SdCard_Read_NoDMA(0x133a, 0, 64, 1, data_block);

		if(memcmp(data_block, sd_tuning_pattern, 64) != 0)
		{
			match = 0;
			for(count = 0; count < 64; )
			{
				uart_printf("data:0x%x,0x%x,0x%x,0x%x",data_block[count],data_block[count+1],data_block[count+2],data_block[count+3]);
				count = count + 4;
			}
		}
		uart_printf("match:0x%x",match);
	}
    SdCard_MsWait(5);

	uart_printf("The max delay value:0x%x",delay);
	delay = delay>>2;
	//set the delay value
	rx_cfg = sdcard_readl(base_addr, 0x114);
	rx_cfg = rx_cfg & 0xffff0000;
	rx_cfg = rx_cfg | (delay<<8) | 0x6;
	sdcard_writel(base_addr, 0x114, rx_cfg);
}

/* One GPIO pin is applied to enable external LDO for SDCARD power supply on EVB with PM802. */
#define GPIO_SDCARD_PWR 75 

void sdcard_poweron(void)
{
	if (PMIC_IS_PM812())
	{
    	PM812_Ldo_12_set_2_8();
    	PM812_Ldo_12_set(TRUE);//SDIO
    	PM812_Ldo_13_set_2_8();
    	PM812_Ldo_13_set(TRUE);//SDCARD
    }
    else if(PMIC_IS_PM813()||PMIC_IS_PM813S())
    {
        Ningbo_Ldo_9_set_2_8();
        Ningbo_Ldo_9_set(TRUE);//SDIO
        Ningbo_Ldo_4_set_2_8();
        Ningbo_Ldo_4_set(TRUE);//SDCARD
    }
	#if 0
    else //802
    {
        Guilin_Ldo_6_set_2_8();
        Guilin_Ldo_6_set(TRUE);//SDIO

        GpioSetDirection(GPIO_SDCARD_PWR, GPIO_OUT_PIN);
        GpioSetLevel(GPIO_SDCARD_PWR, 1); //enable SDCARD power.
    }
	#endif
}

void sdcard_poweroff(void)
{
    //INTCDisable(INTC_SRC_MMC);
	if (PMIC_IS_PM812())
	{
    	PM812_Ldo_12_set(FALSE);//SDIO
    	PM812_Ldo_13_set(FALSE);//SDCARD
    }
    else if(PMIC_IS_PM813()||PMIC_IS_PM813S())
    {
        Ningbo_Ldo_9_set(FALSE);//SDIO
        Ningbo_Ldo_4_set(FALSE);//SDCARD
    }
	#if 0
    else //802
    {
        Guilin_Ldo_6_set(FALSE);//SDIO
        GpioSetLevel(GPIO_SDCARD_PWR, 0); //disable SDCARD power.
    }
	#endif
}

/**
 *  @brief: This function initialize SDIO bus driver.
 *
 *  @return MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
int sdcard_init(void)
{
    UINT32 base_addr = SD1_HOST_CTRL_ADDR;
    UINT16   int_sta;
    int         ret = 0;
    int       fnnum = 0;
	
    if(!PlatformSDCardEnable())
    {
        return 0;
    }
    fatal_printf("===sd card init===\r\n");
	//enable sd card vcc yazhouren@asrmicrom.com
	AIB_MMC1_IO_Set_3_3V();

    sdcard_poweron();
	//return 0;

	sdcard_initialized = SDCARD_INIT_UNDO;
    SDCARD_CTRL_BASE_ADDR = base_addr;

    //config i2c and sdio pinmux
    sdcard_config_pin();

    sdcard_clk_on();

    SdCard_MsWait(10);

    //config sdio power
    sdcard_set_power();
    //config sdio clock, modify by qigang
    SdCard_Enable_Clock(base_addr, 0x300);
    //SdCard_Enable_Clock(base_addr, 0);

    //config burst size
    sdcard_burstsize_config();

    /*init sdio's irq Int status register*/
	//enable all interrupt, modify by qigang
    SdCard_IRQ_Enable(base_addr, 0xF1FF);

	sdcard_writew(base_addr, SD_ERR_INTR_STS_EBLE_offset, 0x3ff);
	sdcard_writew(base_addr, (SD_ERR_INTR_STS_EBLE_offset + 0x4), 0x3ff);

	//sd_dumpregs(SD1_HOST_CTRL_ADDR);
    #if 0
	sdcard_task_init();

    /*configure source of INT, and set ISR*/
    INTCConfigure(INTC_SRC_MMC, INTC_IRQ, INTC_HIGH_LEVEL);
    INTCBind(INTC_SRC_MMC, sd_isr);
    /*INT source enable*/
    INTCEnable(INTC_SRC_MMC);
 
	sdcard_set_mode(CARD_MODE_INT);
    #endif
	sdcard_set_mode(CARD_MODE_POLL);
    SdCard_MsWait(10);

	SdCard_SW_Reset(base_addr);

	fatal_printf("===sd card cmd init begin===\r\n");
    //sd_dumpregs(SD1_HOST_CTRL_ADDR);
	#ifdef SDCARD_INSERT_DEC
    UINT16 card_insert = sdcard_readw(base_addr, 0x26);
	if(!(card_insert & 0x5)) {
		fatal_printf("sdcard don't insert:%d!\r\n", card_insert);
		goto SD_Handle_Err;
	}
	#endif
    ret = sdcard_cmd_init();
    if(ret)
        goto SD_Handle_Err;

    //high speed, not completed yet
	//clear interrupt status
	int_sta = sdcard_readw(base_addr, SD_NORM_INTR_STS_offset);
	if(int_sta)
	{
		uart_printf("before enable highspeed, interrupt status:0x%x\n",int_sta);
		sdcard_writew(base_addr,SD_NORM_INTR_STS_offset,int_sta);
	}
	sdcard_writeb(SD1_HOST_CTRL_ADDR, SD_SW_RESET_CTRL_offset, 0x0E);

//0 for 1bit and 1 for 4bit bus width
	ret = sdcard_set_width(1);
	if(ret)
		goto SD_Handle_Err;

	if(hs_switch == 1)
	{
	   // SdCard_MsWait(10);
		SdCard_Switch_Function(base_addr);
	}else
	{
		UINT32 TX_CFG;
		//keep the hold time for default speed mode
		TX_CFG = sdcard_readl(base_addr, 0x118);
		uart_printf("1 TX_CFG:0x%x\n",TX_CFG);
		TX_CFG = TX_CFG | 0x40000000;
		sdcard_writel(base_addr, 0x118, TX_CFG);
		uart_printf("2 TX_CFG:0x%x\n",sdcard_readl(base_addr, 0x118));
	}

	SdCard_Enable_Clock(base_addr, 0x3);

	if(hs_switch == 1)
	{
	    //SdCard_MsWait(10);
    	SdCard_Tune_Clock(base_addr);
    	SdCard_MsWait(10);
    }

    //modify host2 register to stop clock when no data transfer
    int_sta = sdcard_readw(base_addr, 0x3E);
    uart_printf("the default value of host2 is 0x%x\n",int_sta);
    int_sta = int_sta | 0x4000;
    sdcard_writew(base_addr, 0x3E, int_sta);

    fatal_printf("==sdcard init OK==\r\n");

	sdcard_initialized = SDCARD_INIT_SUCCESS;
	card_status = CARD_STATUS_INSERT;
    /* Init FAT32 system only when SD card is ready.*/
#ifdef FAT32_FILE_SYSTEM
	SD_Infinite_Err_Flag = 0;
	////////Fat32_Init();
	eeSDDumpPrepare();
	FRESULT res = f_mount(&fatfs_fs[0], DUMP_VOL, 1);
	extern int sdcard_fatfs_mount_status_g;
	if (res == FR_OK) {
		sdcard_fatfs_mount_status_g = 1;
		fatal_printf("SDCARD is mounted successfully\n");
	}
	else {
		fatal_printf("SDCARD mount err:%d\n", res);
	}
#endif

    return ret;

SD_Handle_Err:
    fatal_printf("sdcard init error!!!\n");

    sdcard_poweroff();
    
    //only disable SD CLK and LDO for Samsung platform
    //if((PlatformGetProjectType() == NEZHA_SMIFI_V2) || (PlatformGetProjectType() == NEZHA_SMNAND_V2))
    //{
    //    sdcard_clk_off(base_addr);
    //    PMIC_ShutDown_SdLDO();
    //}
    CMD8_timout_cnt = 0;
    sdcard_initialized = SDCARD_INIT_FAILED;

    return ret;
}

UINT32 sdcard_read_multiblks(UINT32 base, UINT32 buffer, UINT32 loop, UINT32 blk_cnt)
{
	UINT32 timeout,mask;
	OSA_STATUS status;
	UINT16 isr_sts;
	gSDRecordTime = /*OSAGetTicks()*/0;

#ifdef MV_USB2_MASS_STORAGE
    mvUsbStorageNotifyUsbActive();
#endif

	status = OSASemaphoreAcquire_sdcard(SDSysRef, OS_SUSPEND);
    ASSERT(status == OS_SUCCESS);
	SD_Read_status = 1;

	SD_CMD_COMP_status = 0;
	SD_DATA_COMP_status = 0;
	SD_DATA_START_status = 1;
	CacheInvalidateMemory(buffer, 0x200*blk_cnt);
	//sdio_set_dmaaddr(base, dma_addr);
	sdcard_writel(base, SD_SYSADDR_LOW_offset, buffer);

	//sdio_set_blkarg(base, SDIO_CMD18_BLKARG);
	sdcard_writel(base, SD_BLOCK_SIZE_offset, 0x7200 | (blk_cnt << 16));

	timeout = 1000000;
    mask = SDHCI_CMD_INHIBIT;
    while((sdcard_readl(base, SD_PRESENT_STAT_0_offset))&mask)
    {
        if(timeout == 0)
        {
            uart_printf("Controller never released inhabit bit\r\n");
		    status = OSASemaphoreRelease_sdcard(SDSysRef);
    		ASSERT(status == OS_SUCCESS);
			SD_Read_status = 0;
            return MMC_ERR_CMD_TOUT;
        }
        timeout--;
    }
	//set card read offset
	//sdio_set_argument(base, (loop * SDIO_TRANS_SIZE));
	if(card_type == CARD_TYPE_HIGH)
		sdcard_writel(base, SD_ARG_LOW_offset, loop);
	else if(card_type == CARD_TYPE_STANDARD)
		sdcard_writel(base, SD_ARG_LOW_offset, loop*0x200);
	//sdcard_writel(base, SD_ARG_LOW_offset, loop);
	//send command 18
	sdcard_writel(base, SD_TRANSFER_MODE_offset, 0x123a0037);


	/*wait for complete*/
	timeout = 40000;
	if(sdcard_get_mode() == CARD_MODE_INT)
	{
		while (!SD_CMD_COMP_status)
		{
	    	SdCard_Wait(2000);
			timeout--;
			if(timeout == 0 )
			{
				uart_printf("INT mode,Wait for command18 time out!\r\n");
				uart_printf("INT mode,Wait for command18 time out!\r\n");
				if(SD_Decrease_Clk_Flag == 0)
			    {
				    sd_dumpregs(base);
				}
                status = OSASemaphoreRelease_sdcard(SDSysRef);
    			ASSERT(status == OS_SUCCESS);
				SD_Read_status = 0;

				return MMC_ERR_CMD_TOUT;
			}

			if(SD_Infinite_Err_Flag == 1)
			{
			    status = OSASemaphoreRelease_sdcard(SDSysRef);
    			ASSERT(status == OS_SUCCESS);
				SD_Read_status = 0;
				return MMC_ERR_CMD_TOUT;
			}

		}

	}else if(sdcard_get_mode() == CARD_MODE_POLL)
	{
		isr_sts = sdcard_readw(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_offset);
		while (!(isr_sts & CMD_COMP))
		{
	    	SdCard_Wait(2000);
			timeout--;
			if (timeout == 0 )
			{
				fatal_printf("POLL mode,Wait for command18 time out!\r\n");
				uart_printf("blk num:0x%x",blk_cnt);
				sd_dumpregs(base);
			    status = OSASemaphoreRelease_sdcard(SDSysRef);
    			ASSERT(status == OS_SUCCESS);
				return MMC_ERR_CMD_TOUT;
			}
			isr_sts = sdcard_readw(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_offset);
		}

		//clear cmd complete interrupt
		sdcard_writew(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_offset, CMD_COMP);
	}

	timeout = 40000;

	if(sdcard_get_mode() == CARD_MODE_INT)
	{
		status = OSASemaphoreAcquire_sdcard(SDDATARef, OS_SUSPEND);
    	ASSERT(status == OS_SUCCESS);
		if(SD_DATA_COMP_status != 1)
		{
		    status = OSASemaphoreRelease_sdcard(SDSysRef);
    		ASSERT(status == OS_SUCCESS);
			return MMC_ERR_DATA_TOUT;
		}
/*
		while (!SD_DATA_COMP_status)
		{
	   		SdCard_Wait(2000);
			timeout--;
			if (timeout == 0 )
			{
				uart_printf("INT mode,CMD18,Wait for data transfer time out!\r\n");
				sd_dumpregs(base);
				status = OSASemaphoreRelease_sdcard(SDSysRef);
    			ASSERT(status == OS_SUCCESS);
				return MMC_ERR_DATA_TOUT;
			}
		}
*/
	}else if(sdcard_get_mode() == CARD_MODE_POLL)
	{
		isr_sts = sdcard_readw(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_offset);
		while ( !(isr_sts & XFER_COMP))
		{
			if(isr_sts & DMA_INT)
			{
				sdcard_writew(base, SD_NORM_INTR_STS_offset, DMA_INT);
				sdcard_writel(base, SD_SYSADDR_LOW_offset,sdcard_readl(base,SD_SYSADDR_LOW_offset));
				uart_printf("CMD18 DMA interrupt");
				timeout = 40000;
			}
	    	SdCard_Wait(2000);
			timeout--;
			if (timeout == 0 )
			{
				uart_printf("POLL mode,CMD18, Wait for data transfer time out!\r\n");
				sd_dumpregs(base);
				status = OSASemaphoreRelease_sdcard(SDSysRef);
    			ASSERT(status == OS_SUCCESS);
				return MMC_ERR_DATA_TOUT;
				//timeout = 40000;
			}
			isr_sts = sdcard_readw(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_offset);
		}
		//clear data complete interrupt
		sdcard_writew(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_offset, XFER_COMP);
	}

	SdCard_SW_Reset(base);
	//SdCard_Wait(200000);
	SD_Read_status = 0;
	status = OSASemaphoreRelease_sdcard(SDSysRef);
    ASSERT(status == OS_SUCCESS);

	return 0;
}

UINT32 sdcard_read_singleblk(UINT32 base, UINT32 buffer, UINT32 loop)
{
	UINT32 timeout,mask;
	OSA_STATUS status;
	UINT16 isr_sts;
	gSDRecordTime = /*OSAGetTicks()*/0;

#ifdef MV_USB2_MASS_STORAGE
    mvUsbStorageNotifyUsbActive();
#endif
	status = OSASemaphoreAcquire_sdcard(SDSysRef, OS_SUSPEND);
    ASSERT(status == OS_SUCCESS);
	SD_Read_status = 1;

	SD_CMD_COMP_status = 0;
	SD_DATA_COMP_status = 0;
	SD_DATA_START_status = 1;
	CacheInvalidateMemory(buffer, 0x200);
	//sdio_set_dmaaddr(base, dma_addr);
	sdcard_writel(base, SD_SYSADDR_LOW_offset, buffer);

	sdcard_writel(base, SD_BLOCK_SIZE_offset, 0x7200 | (1 << 16));
	timeout = 1000000;
    mask = SDHCI_CMD_INHIBIT;
    while((sdcard_readl(base, SD_PRESENT_STAT_0_offset))&mask)
    {
        if(timeout == 0)
        {
            uart_printf("Controller never released inhabit bit\r\n");
			status = OSASemaphoreRelease_sdcard(SDSysRef);
    		ASSERT(status == OS_SUCCESS);
			SD_Read_status = 0;
            return MMC_ERR_CMD_TOUT;
        }
        timeout--;
    }
	//set card read offset
	//sdio_set_argument(base, (loop * SDIO_TRANS_SIZE));
	if(card_type == CARD_TYPE_HIGH)
		sdcard_writel(base, SD_ARG_LOW_offset, loop);
	else if(card_type == CARD_TYPE_STANDARD)
		sdcard_writel(base, SD_ARG_LOW_offset, loop*0x200);
	//sdcard_writel(base, SD_ARG_LOW_offset, loop);
	//send command 17
	sdcard_writel(base, SD_TRANSFER_MODE_offset, 0x113a0011);
	/*wait for complete*/
	timeout = 40000;
	if(sdcard_get_mode() == CARD_MODE_INT)
	{
		while (!SD_CMD_COMP_status)
		{
	    	SdCard_Wait(2000);
			timeout--;
			if(timeout == 0 )
			{
				uart_printf("INT mode,Wait for command17 time out!\r\n");
				uart_printf("INT mode,Wait for command17 time out!\r\n");
			    if(SD_Decrease_Clk_Flag == 0)
			    {
				    sd_dumpregs(base);
				}
				status = OSASemaphoreRelease_sdcard(SDSysRef);
    			ASSERT(status == OS_SUCCESS);
				SD_Read_status = 0;

				return MMC_ERR_CMD_TOUT;
			}

			if(SD_Infinite_Err_Flag == 1)
			{
				status = OSASemaphoreRelease_sdcard(SDSysRef);
    			ASSERT(status == OS_SUCCESS);
				SD_Read_status = 0;
				return MMC_ERR_CMD_TOUT;
			}

		}

	}else if(sdcard_get_mode() == CARD_MODE_POLL)
	{
		isr_sts = sdcard_readw(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_offset);
		while (!(isr_sts & CMD_COMP))
		{
	    	SdCard_Wait(2000);
			timeout--;
			if (timeout == 0 )
			{
				fatal_printf("POLL mode,Wait for command17 time out!\r\n");
				//uart_printf("blk num:0x%x",blk_cnt);
				sd_dumpregs(base);
				status = OSASemaphoreRelease_sdcard(SDSysRef);
    			ASSERT(status == OS_SUCCESS);
				return MMC_ERR_CMD_TOUT;
			}
			isr_sts = sdcard_readw(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_offset);
		}
		//clear cmd complete interrupt
		sdcard_writew(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_offset, CMD_COMP);
	}

	timeout = 40000;

	if(sdcard_get_mode() == CARD_MODE_INT)
	{
		status = OSASemaphoreAcquire_sdcard(SDDATARef, OS_SUSPEND);
    	ASSERT(status == OS_SUCCESS);
		if(SD_DATA_COMP_status != 1)
		{
			status = OSASemaphoreRelease_sdcard(SDSysRef);
    		ASSERT(status == OS_SUCCESS);
			return MMC_ERR_DATA_TOUT;
		}

	}else if(sdcard_get_mode() == CARD_MODE_POLL)
	{
		isr_sts = sdcard_readw(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_offset);
		while ( !(isr_sts & XFER_COMP))
		{
			if(isr_sts & DMA_INT)
			{
				sdcard_writew(base, SD_NORM_INTR_STS_offset, DMA_INT);
				sdcard_writel(base, SD_SYSADDR_LOW_offset,sdcard_readl(base,SD_SYSADDR_LOW_offset));
				uart_printf("CMD17 DMA interrupt");
				timeout = 40000;
			}
	    	SdCard_Wait(2000);
			timeout--;
			if (timeout == 0 )
			{
				uart_printf("POLL mode,CMD17, Wait for data transfer time out!\r\n");
				sd_dumpregs(base);
				status = OSASemaphoreRelease_sdcard(SDSysRef);
    			ASSERT(status == OS_SUCCESS);
				return MMC_ERR_DATA_TOUT;
				//timeout = 40000;
			}
			isr_sts = sdcard_readw(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_offset);
		}
		//clear data complete interrupt
		sdcard_writew(SDCARD_CTRL_BASE_ADDR, SD_NORM_INTR_STS_offset, XFER_COMP);
	}

	SdCard_SW_Reset(base);
	//SdCard_Wait(200000);
	SD_Read_status = 0;
	status = OSASemaphoreRelease_sdcard(SDSysRef);
    ASSERT(status == OS_SUCCESS);
	return 0;
}

static void SdCard_Check_TransStatus(UINT32 base)
{
	UINT16 cmd;
	UINT32 argv;

	//response type R1
	cmd = 0x0d1a;
	argv = SD_RCA << 16;
	SdCard_Cmd_Nodata(cmd, argv, base);
	//check card status, transfer and data ready
	while((((SD_RESPONSE[0]>>9) & 0xf) != 4) || ((SD_RESPONSE[0] & 0x100) == 0))
	{
		//uart_printf("Response 0:0x%x",SD_RESPONSE[0]);
		SdCard_Cmd_Nodata(cmd, argv, base);

		if(SD_Infinite_Err_Flag == 1)
			return;
	}
}

UINT32 sdcard_write_multiblks(UINT32 base, UINT32 buffer, UINT32 loop, UINT32 blk_cnt)
{
	UINT32 timeout,mask;
	OSA_STATUS status;
	UINT16 isr_sts;
	gSDRecordTime = /*OSAGetTicks()*/0;

#ifdef MV_USB2_MASS_STORAGE
    mvUsbStorageNotifyUsbActive();
#endif

	status = OSASemaphoreAcquire_sdcard(SDSysRef, OS_SUSPEND);
    ASSERT(status == OS_SUCCESS);
    SD_Write_status = 1;

	SD_CMD_COMP_status = 0;
	SD_DATA_COMP_status = 0;
	SD_DATA_START_status = 1;
	CacheCleanMemory(buffer, 0x200*blk_cnt);

	//sdio_set_dmaaddr(base, dma_addr);
	sdcard_writel(base, SD_SYSADDR_LOW_offset, buffer);

	//sdio_set_blkarg(base, SDIO_CMD18_BLKARG);
	sdcard_writel(base, SD_BLOCK_SIZE_offset, 0x7200 | (blk_cnt << 16));

	timeout = 1000000;
    mask = SDHCI_CMD_INHIBIT;
    while((sdcard_readl(base, SD_PRESENT_STAT_0_offset))&mask)
    {
        if(timeout == 0)
        {
            uart_printf("Controller never released inhabit bit\r\n");
			status = OSASemaphoreRelease_sdcard(SDSysRef);
    		ASSERT(status == OS_SUCCESS);
			SD_Write_status = 0;
            return MMC_ERR_CMD_TOUT;
        }
        timeout--;
    }

	//set card read offset
	if(card_type == CARD_TYPE_HIGH)
		sdcard_writel(base, SD_ARG_LOW_offset, loop);
	else if(card_type == CARD_TYPE_STANDARD)
		sdcard_writel(base, SD_ARG_LOW_offset, loop*0x200);

	sdcard_writel(base, SD_TRANSFER_MODE_offset, 0x193a0027);

	/*wait for complete*/
	timeout = 40000;

	if(sdcard_get_mode() == CARD_MODE_INT)
	{
		while (!SD_CMD_COMP_status)
		{
	    	SdCard_Wait(2000);
			timeout--;
			if(timeout == 0 )
			{
				uart_printf("INT mode, Wait for command25 time out!\r\n");
			    if(SD_Decrease_Clk_Flag == 0)
			    {
				    sd_dumpregs(base);
				}

				status = OSASemaphoreRelease_sdcard(SDSysRef);
    			ASSERT(status == OS_SUCCESS);
				SD_Write_status = 0;

				return MMC_ERR_CMD_TOUT;
			}

			if(SD_Infinite_Err_Flag == 1)
			{
				status = OSASemaphoreRelease_sdcard(SDSysRef);
    			ASSERT(status == OS_SUCCESS);
				SD_Read_status = 0;
				return MMC_ERR_CMD_TOUT;
			}
		}

	}else if(sdcard_get_mode() == CARD_MODE_POLL)
	{
		isr_sts = sdcard_readw(base, SD_NORM_INTR_STS_offset);
		while (!(isr_sts & CMD_COMP))
		{
	    	SdCard_Wait(2000);
			timeout--;
			if (timeout == 0 )
			{
				fatal_printf("POLL mode, Wait for command25 time out!\r\n");
				sd_dumpregs(base);
				status = OSASemaphoreRelease_sdcard(SDSysRef);
    			ASSERT(status == OS_SUCCESS);
				return MMC_ERR_CMD_TOUT;
			}
			isr_sts = sdcard_readw(base, SD_NORM_INTR_STS_offset);
		}

		//clear cmd complete interrupt
		sdcard_writew(base, SD_NORM_INTR_STS_offset, CMD_COMP);
	}

	timeout = 80000;
	if(sdcard_get_mode() == CARD_MODE_INT)
	{
		status = OSASemaphoreAcquire_sdcard(SDDATARef, OS_SUSPEND);
    	ASSERT(status == OS_SUCCESS);

		if(SD_DATA_COMP_status != 1)
		{
			status = OSASemaphoreRelease_sdcard(SDSysRef);
    		ASSERT(status == OS_SUCCESS);
			return MMC_ERR_DATA_TOUT;
		}
/*
		while (!SD_DATA_COMP_status)
		{
	   		SdCard_Wait(2000);
			timeout--;
			if (timeout == 0 )
			{
				uart_printf("INT mode,CMD25,Wait for data transfer time out!\r\n");
				sd_dumpregs(base);
				status = OSASemaphoreRelease_sdcard(SDSysRef);
    			ASSERT(status == OS_SUCCESS);
				return MMC_ERR_DATA_TOUT;
			}
		}
*/
	}else if(sdcard_get_mode() == CARD_MODE_POLL)
	{
		isr_sts = sdcard_readw(base, SD_NORM_INTR_STS_offset);
		while ( !(isr_sts & XFER_COMP))
		{
			if(isr_sts & DMA_INT)
			{
				sdcard_writew(base, SD_NORM_INTR_STS_offset, DMA_INT);
				sdcard_writel(base, SD_SYSADDR_LOW_offset,sdcard_readl(base,SD_SYSADDR_LOW_offset));
				//uart_printf("CMD25 DMA interrupt");
				timeout = 40000;

			}
	    	SdCard_Wait(2000);
			timeout--;
			if (timeout == 0 )
			{
				if(isr_sts & ERR_INT )
				{
      		  		isr_sts = sdcard_readw(base, SD_ERR_INTR_STS_offset);
       				sdcard_writew(base, SD_ERR_INTR_STS_offset, isr_sts);
    	        	uart_printf("err_sts:0x%x",isr_sts);
					SdCard_SW_Reset(base);
					timeout = 40000;
				}else
				{
					status = OSASemaphoreRelease_sdcard(SDSysRef);
    				ASSERT(status == OS_SUCCESS);
					uart_printf("POLL mode,CMD25,Wait for data transfer time out!");
					sd_dumpregs(base);
					return MMC_ERR_DATA_TOUT;
				}
			}
			isr_sts = sdcard_readw(base, SD_NORM_INTR_STS_offset);
		}
		//clear data complete interrupt
		sdcard_writew(base, SD_NORM_INTR_STS_offset, XFER_COMP);
	}

	SD_Write_status = 2;

	SdCard_Check_TransStatus(base);
	//SdCard_Wait(200000);
	SD_Write_status = 0;
	status = OSASemaphoreRelease_sdcard(SDSysRef);
    ASSERT(status == OS_SUCCESS);

	return MMC_ERR_NONE;

}

UINT32 sdcard_write_singleblk(UINT32 base, UINT32 buffer, UINT32 loop)
{
	UINT32 timeout,mask;
	OSA_STATUS status;
	UINT16 isr_sts;
	gSDRecordTime = /*OSAGetTicks()*/0;

#ifdef MV_USB2_MASS_STORAGE
    mvUsbStorageNotifyUsbActive();
#endif

	status = OSASemaphoreAcquire_sdcard(SDSysRef, OS_SUSPEND);
    ASSERT(status == OS_SUCCESS);
    SD_Write_status = 1;

	SD_CMD_COMP_status = 0;
	SD_DATA_COMP_status = 0;
	SD_DATA_START_status = 1;
	CacheCleanMemory(buffer, 0x200);

	//sdio_set_dmaaddr(base, dma_addr);
	sdcard_writel(base, SD_SYSADDR_LOW_offset, buffer);

	//sdio_set_blkarg(base, SDIO_CMD18_BLKARG);
	sdcard_writel(base, SD_BLOCK_SIZE_offset, 0x7200 | (1 << 16));

	timeout = 1000000;
    mask = SDHCI_CMD_INHIBIT;
    while((sdcard_readl(base, SD_PRESENT_STAT_0_offset))&mask)
    {
        if(timeout == 0)
        {
            uart_printf("Controller never released inhabit bit\r\n");
			status = OSASemaphoreRelease_sdcard(SDSysRef);
    		ASSERT(status == OS_SUCCESS);
			SD_Write_status = 0;
            return MMC_ERR_CMD_TOUT;
        }
        timeout--;
    }

	//set card read offset
	if(card_type == CARD_TYPE_HIGH)
		sdcard_writel(base, SD_ARG_LOW_offset, loop);
	else if(card_type == CARD_TYPE_STANDARD)
		sdcard_writel(base, SD_ARG_LOW_offset, loop*0x200);

	//send CMD24
	sdcard_writel(base, SD_TRANSFER_MODE_offset, 0x183a0001);

	/*wait for complete*/
	timeout = 40000;

	if(sdcard_get_mode() == CARD_MODE_INT)
	{
		while (!SD_CMD_COMP_status)
		{
	    	SdCard_Wait(2000);
			timeout--;
			if(timeout == 0 )
			{
				uart_printf("INT mode, Wait for command24 time out!\r\n");
				if(SD_Decrease_Clk_Flag == 0)
			    {
				    sd_dumpregs(base);
				}

				status = OSASemaphoreRelease_sdcard(SDSysRef);
    			ASSERT(status == OS_SUCCESS);
				SD_Write_status = 0;

				return MMC_ERR_CMD_TOUT;
			}

			if(SD_Infinite_Err_Flag == 1)
			{
				status = OSASemaphoreRelease_sdcard(SDSysRef);
    			ASSERT(status == OS_SUCCESS);
				SD_Read_status = 0;
				return MMC_ERR_CMD_TOUT;
			}
		}

	}else if(sdcard_get_mode() == CARD_MODE_POLL)
	{
		isr_sts = sdcard_readw(base, SD_NORM_INTR_STS_offset);
		while (!(isr_sts & CMD_COMP))
		{
	    	SdCard_Wait(2000);
			timeout--;
			if (timeout == 0 )
			{
				fatal_printf("POLL mode, Wait for command24 time out!\r\n");
				sd_dumpregs(base);
				status = OSASemaphoreRelease_sdcard(SDSysRef);
    			ASSERT(status == OS_SUCCESS);
				return MMC_ERR_CMD_TOUT;
			}
			isr_sts = sdcard_readw(base, SD_NORM_INTR_STS_offset);
		}

		//clear cmd complete interrupt
		sdcard_writew(base, SD_NORM_INTR_STS_offset, CMD_COMP);
	}

	timeout = 80000;
	if(sdcard_get_mode() == CARD_MODE_INT)
	{
		status = OSASemaphoreAcquire_sdcard(SDDATARef, OS_SUSPEND);
    	ASSERT(status == OS_SUCCESS);

		if(SD_DATA_COMP_status != 1)
		{
			status = OSASemaphoreRelease_sdcard(SDSysRef);
    		ASSERT(status == OS_SUCCESS);
			return MMC_ERR_DATA_TOUT;
		}
	}else if(sdcard_get_mode() == CARD_MODE_POLL)
	{
		isr_sts = sdcard_readw(base, SD_NORM_INTR_STS_offset);
		while ( !(isr_sts & XFER_COMP))
		{
			if(isr_sts & DMA_INT)
			{
				sdcard_writew(base, SD_NORM_INTR_STS_offset, DMA_INT);
				sdcard_writel(base, SD_SYSADDR_LOW_offset,sdcard_readl(base,SD_SYSADDR_LOW_offset));
				uart_printf("CMD24 DMA interrupt");
				timeout = 40000;

			}
	    	SdCard_Wait(2000);
			timeout--;
			if (timeout == 0 )
			{
				if(isr_sts & ERR_INT )
				{
      		  		isr_sts = sdcard_readw(base, SD_ERR_INTR_STS_offset);
       				sdcard_writew(base, SD_ERR_INTR_STS_offset, isr_sts);
    	        	uart_printf("err_sts:0x%x",isr_sts);
					SdCard_SW_Reset(base);
					timeout = 40000;
				}else
				{
					status = OSASemaphoreRelease_sdcard(SDSysRef);
    				ASSERT(status == OS_SUCCESS);
					uart_printf("POLL mode,CMD25,Wait for data transfer time out!");
					sd_dumpregs(base);
					return MMC_ERR_DATA_TOUT;
				}
			}
			isr_sts = sdcard_readw(base, SD_NORM_INTR_STS_offset);
		}
		//clear data complete interrupt
		sdcard_writew(base, SD_NORM_INTR_STS_offset, XFER_COMP);
	}

	SD_Write_status = 2;

	SdCard_Check_TransStatus(base);
	//SdCard_Wait(200000);
	SD_Write_status = 0;
	status = OSASemaphoreRelease_sdcard(SDSysRef);
    ASSERT(status == OS_SUCCESS);

	return MMC_ERR_NONE;

}

UINT32 sdcard_write_mass_storage(UINT32 buffer, UINT32 loop, UINT32 blk_cnt)
{
	UINT32 ret = 0;
	if((sdcard_initialized != SDCARD_INIT_SUCCESS) || (SD_Infinite_Err_Flag == 1) || (card_status == CARD_STATUS_REMOVE))
	{
		return 0x10;
	}
	//uart_printf("write,loop:0x%x,blk_cnt:0x%x",loop,blk_cnt);
	if(blk_cnt == 1)
		ret = sdcard_write_singleblk(SD1_HOST_CTRL_ADDR, buffer, loop);
	else if(blk_cnt > 1)
    	ret = sdcard_write_multiblks(SD1_HOST_CTRL_ADDR, buffer, loop, blk_cnt);

	return ret;
}


UINT32 sdcard_read_mass_storage(UINT32 buffer, UINT32 loop, UINT32 blk_cnt)
{
	UINT32 ret = 0;
	if((sdcard_initialized != SDCARD_INIT_SUCCESS) || (SD_Infinite_Err_Flag == 1) ||(card_status == CARD_STATUS_REMOVE))
	{
		return 0x10;
	}

	//uart_printf("read,loop:0x%x,blk_cnt:0x%x",loop,blk_cnt);
	if(blk_cnt == 1)
		ret = sdcard_read_singleblk(SD1_HOST_CTRL_ADDR, buffer, loop);
	else if(blk_cnt > 1)
    	ret = sdcard_read_multiblks(SD1_HOST_CTRL_ADDR, buffer, loop, blk_cnt);

	return ret;
}

UINT8 sdcard_get_errflag(void)
{
	return SD_Infinite_Err_Flag;
}
BOOL sdcard_is_ready(void)
{
    if(PlatformSDCardEnable()/*&&(FSInit_Status_Get()==1)*/&&(card_status != CARD_STATUS_REMOVE))
    {
        if((SD_Decrease_Clk_Flag != 0) && (sdcard_get_errflag() == 1) &&(!eeGetSystemAssertFlag()))
            return FALSE;
        else
            return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL sdcard_is_insert(void)
{
    if(PlatformSDCardEnable()&&(sdcard_get_status()==1)&&(card_status != CARD_STATUS_REMOVE))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*extern BOOL diagCommSdLogEnable(void);
BOOL SDWork(void)
{
	if(TRUE == diagCommSdLogEnable())
	{
        return TRUE;
	}
	else
		return FALSE;
}*/

