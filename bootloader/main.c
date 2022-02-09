#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "cpu.h"
#include "qspi.h"
#include "reg.h"
#include "system.h"
//#include "uart.h"
#include "ptable.h"
#include "loadtable.h"
//#include "lzop.h"
#include "sdio.h"
#include "pmic.h"
#include "ff.h"
#include "version_block.h"
#include "spi_nor.h"
#include "fip.h"
#include "asr_property.h"
#include "logo_table.h"
#include "asr_lzma.h"
#include "bsp.h"
#include "intc.h"
#include "timer_hw.h"
void FIQ_Routine(void)
{
    uart_printf("Kernel Panic");
    uart_printf("Early FIQ call");

    while (1) {
    }
}

void UNDEF_Routine(void)
{
    uart_printf("Kernel Panic");
    uart_printf("UNDEF_Routine call");

    while (1) {
    }
}

void PABT_Routine(void)
{
    uart_printf("Kernel Panic");
    uart_printf("PABT_Routine call");

    while (1) {
    }
}

void DABT_Routine(void)
{
    uart_printf("Kernel Panic");
    uart_printf("DABT_Routine call");

    while (1) {
    }
}

void IRQ_Routine(void *arg)
{
    (void)arg;

    uart_printf("in IRQ_Routine");
}


//unsigned int init_cpsr;

#define MODE32_svc          0x13
#define SPSR_T_ARM          0
#define SPSR_E_LITTLE       0
#define SPSR_FIQ_BIT        (1 << 0)
#define SPSR_IRQ_BIT        (1 << 1)
#define SPSR_ABT_BIT        (1 << 2)
#define DISABLE_ALL_EXCEPTIONS  (SPSR_FIQ_BIT | SPSR_IRQ_BIT | SPSR_ABT_BIT)

#define MODE_RW_SHIFT       0x4
#define MODE_RW_32      0x1
#define MODE32_SHIFT        0
#define MODE32_MASK     0x1f
#define SPSR_T_SHIFT        5
#define SPSR_T_MASK     0x1
#define SPSR_E_SHIFT        9
#define SPSR_E_MASK     0x1
#define SPSR_AIF_SHIFT      6
#define SPSR_AIF_MASK       0x7

#define SPSR_MODE32(mode, isa, endian, aif)     \
    (MODE_RW_32 << MODE_RW_SHIFT |          \
     ((mode) & MODE32_MASK) << MODE32_SHIFT |    \
     ((isa) & SPSR_T_MASK) << SPSR_T_SHIFT |     \
     ((endian) & SPSR_E_MASK) << SPSR_E_SHIFT |  \
     ((aif) & SPSR_AIF_MASK) << SPSR_AIF_SHIFT)

/******************************
MACRO NAME: ADD_VIBRATOR_IN_CODE
Despriction:   Add vibrator feedback after onkeycheck during boot up. 
		       Default: no vibrator feedback
		       
Owner:         Hengshan Yang
Time:          2019-10-16
******************************/
//#define ADD_VIBRATOR_IN_CODE

typedef struct {
#if 1 //yqian
    uint32_t  pc;
#else
    uintptr_t pc;
#endif
    uint32_t  spsr;
} transfer_parameter;

//void transfer_control(transfer_parameter *param) __attribute__ ((noreturn));
extern void transfer_control(transfer_parameter *param);
//extern int lzop_decompress_safe(lzo_bytep src,  lzo_bytep dest, lzo_uint32p dest_len, lzo_uint32p cpz_src_len);
//extern int lzop_decompress_safe_internal_flash(lzo_bytep src,  lzo_bytep dest, lzo_uint32p dest_len, lzo_uint32p cpz_src_len);
extern void cp_uart_init(void );

/***************************************
Function:	  Keypad_OTA_Flag_Check_B
Description:  Detect OTA_Sdcard update keypad combination flag
Parmeters:	  None

Return value: True: Have detected the update keypad combination flag
              False: Not detect the update keypad combination flag
***************************************/
extern BOOL Keypad_OTA_Flag_Check_B(void);

/***************************************
Function:	  Keypad_disable
Description:  Disable Keypad interrupt and disable direct keypad and Matrix keypad mode
Parmeters:	  None

Return value: NONE
***************************************/
extern void Keypad_disable(void);

//#ifdef ONKEY_CHECK
/* Timer0_0 is configured to free run @1MHz from BootROM. */
#define APBTIMER0_CNT_REG   0xD4014090
#define APBTIMER0_EN_REG    0xD4014000

/*============================================*/


typedef struct{
	char  *ImageName;
	UINT32 FlashAddress;
	UINT32 LoadAddress;
	UINT32 FlashSize;
	UINT32 ExecSize;
	UINT32 UncompressDstAddr;
	CompressedType_e CompressedType;
	_ptflash ImageInFlashType;
}CopyImageInfo_t;





#define LOGO_HEADER_LENGTH (0x10)	
//Lzma use buffer in "malloc mechanism"
#define COMPRESSED_ALGORITHM_BUFFER_SIZE (0x00010000)
UINT8 g_compressed_buffer[COMPRESSED_ALGORITHM_BUFFER_SIZE];







#define PMUTIMER_WDT_STATUS_REG 0xD4080070
BOOL IfWdtResetTriggered(void)
{
	return ((*(volatile unsigned long*)PMUTIMER_WDT_STATUS_REG) & 0x1);
}

void Timer0_enable(BOOL enable){
	if(enable){
		*(volatile unsigned long*)APBTIMER0_EN_REG = 0x1; //enable timer0_0 (in free run)
	}else{
	    *(volatile unsigned long*)APBTIMER0_EN_REG = 0x0; //stop Timer0_0 
	}
}

void OnkeyPowerOnCheck(void)
{
    unsigned int i, j=0;

    uart_printf("OnkeyPowerOnCheck\r\n");

	Timer0_enable(TRUE);
	for(i=0;i<10;i++) //Almost 2s from BootROM to here for ONKEY check
    {
        if (!PMIC_ONKEY_IS_DETECTED())
            j++;
        else
            j = 0;
        DelayInMilliSecond(100);
        if (j > 2)
        {
            uart_printf("ONKEY down");
            PM812_SW_PDOWN(); //Power down if ONKEY is not pressed long enough.
        }
    }

	Timer0_enable(FALSE);

}
//#endif



int set_mpu_reg_0(void )
{
    unsigned int acc_ctrl;
    uintptr_t   base = 0x00000000;
    size_t      size = 0x80000000;

    /* Grant read, write and excution privilege to the code area */
    acc_ctrl = MPU_REGION_ACCESS_CTRL_AP(0x3)       /* read, write */
               | MPU_REGION_ACCESS_CTRL_TEX(0x1);   /* non-cacheable */

    mpu_set_region(0, base, size, acc_ctrl);
    mpu_enable_region(0, 1);
    return 0;
}

int set_mpu_reg_1(void )
{
    unsigned int acc_ctrl;
    uintptr_t   base = 0x00000000;
    size_t      size = 0x80000000;

    /* Grant read, write and excution privilege to the code area */
    acc_ctrl = MPU_REGION_ACCESS_CTRL_AP(0x3)       /* read, write */
               | MPU_REGION_ACCESS_CTRL_TEX(0x1);   /* non-cacheable */

    mpu_set_region(1, base, size, acc_ctrl);
    mpu_enable_region(1, 1);
    return 0;
}

int set_mpu_reg_2(void )
{
    unsigned int acc_ctrl;
    uintptr_t   base = 0x80000000;
    size_t      size = 0x80000000;

    /* Grant read, write and excution privilege to the code area */
    acc_ctrl = MPU_REGION_ACCESS_CTRL_AP(0x3)       /* read, write */
               | MPU_REGION_ACCESS_CTRL_TEX(0x1);   /* non-cacheable */

    mpu_set_region(2, base, size, acc_ctrl);
    mpu_enable_region(2, 1);
    return 0;
}

int set_mpu_reg_3(void )
{
    unsigned int acc_ctrl;
    uintptr_t   base = 0x80000000;
    size_t      size = 0x4000000; //64MB

    /* Grant read, write and excution privilege to the code area */
    acc_ctrl = MPU_REGION_ACCESS_CTRL_AP(0x3)       /* read, write */
               | MPU_REGION_ACCESS_CTRL_TEX(0x1)   /* non-cacheable */
               | MPU_REGION_ACCESS_CTRL_B          /* bufferable */
               | MPU_REGION_ACCESS_CTRL_C;         /* cacheable */

    mpu_set_region(3, base, size, acc_ctrl);
    mpu_enable_region(3, 1);
    return 0;
}

int set_mpu_for_DSP(void )
{
    unsigned int acc_ctrl;
    uintptr_t   base = 0xd1000000;
    size_t      size = 0x00100000;

    /* Grant read, write and excution privilege to the code area */
    acc_ctrl = MPU_REGION_ACCESS_CTRL_AP(0x3)       /* read, write */
               | MPU_REGION_ACCESS_CTRL_TEX(0x1);   /* non-cacheable */

    mpu_set_region(1, base, size, acc_ctrl);
    mpu_enable_region(1, 1);
    return 0;
}

int set_mpu_reg_4(void)
{
    unsigned int acc_ctrl;
    uintptr_t   base = 0xc0000000;
    size_t      size = 0x40000000;// 1GB
    acc_ctrl =MPU_REGION_ACCESS_CTRL_XN
            | MPU_REGION_ACCESS_CTRL_AP(0x3)       /* read, write */
            | MPU_REGION_ACCESS_CTRL_TEX(0x0)   /* non-cacheable */
            | MPU_REGION_ACCESS_CTRL_B;         /* bufferable */

    mpu_set_region(4, base, size, acc_ctrl);
    mpu_enable_region(4, 1);

    return 0;
}

static int set_mpu_reg_5(uintptr_t base, size_t size)
{
    unsigned int acc_ctrl;

    /* Grant read, write and excution privilege to the code area */
    acc_ctrl = MPU_REGION_ACCESS_CTRL_AP(0x3)       /* read, write */
               | MPU_REGION_ACCESS_CTRL_TEX(0x1);   /* non-cacheable */

    mpu_set_region(5, base, size, acc_ctrl);
    mpu_enable_region(5, 1);

    return 0;
}

int enable_psram_cache(uintptr_t addr, size_t length)
{
    unsigned int acc_ctrl;
    uintptr_t   base = addr;
    size_t      size = length;

    /* Grant read, write and excution privilege to the code area */
    acc_ctrl = MPU_REGION_ACCESS_CTRL_AP(0x3)       /* read, write */
	           | MPU_REGION_ACCESS_CTRL_B          /* bufferable */
               | MPU_REGION_ACCESS_CTRL_C;         /* cacheable */


    mpu_set_region(0, base, size, acc_ctrl);
    mpu_enable_region(0, 1);
    return 0;
}
extern void mpu_value_check(void);
extern void   TransferControl(unsigned int);
extern void   TransferControlToLogo(unsigned int addr, unsigned int para);

#define QSPI_AMBA_BASE  0x80000000U
void boot_image(void)
{
    transfer_parameter param;

    param.pc = QSPI_AMBA_BASE + QSPI_FLASH_A1_OFFSET + 0x100000;
    param.spsr = SPSR_MODE32(MODE32_svc, SPSR_T_ARM, SPSR_E_LITTLE, DISABLE_ALL_EXCEPTIONS);

    writel(0, QSPI_IPB_BASE + QSPI_BFGENCR);
    writel(QSPI_AMBA_BASE + QSPI_FLASH_A1_OFFSET + 0x100000, QSPI_IPB_BASE + QSPI_SFAR);
    uart_printf("boot_image   --- 0");
    //display_binary((void*)param.pc, 128);

    uart_printf("boot_image   --- 1");

    set_mpu_reg_5(QSPI_AMBA_BASE + QSPI_FLASH_A1_OFFSET, 0x8000000);
    uart_printf("boot_image   --- 2");

    uart_printf("boot to %#X, cpsr should be %#X", param.pc, param.spsr);

#if 1 //yqian
    TransferControl(param.pc);
#else
    transfer_control(&param);
#endif    
}




int qspi_flash_init(void);

volatile unsigned long CpExecAddress;
volatile unsigned long CpCopySize;
volatile unsigned long DspExecAddress;
volatile unsigned long DspCopySize;
volatile unsigned long CpFlashAddress,Cp_2_FlashAddress;
volatile unsigned long DspFlashAddress;
volatile unsigned long rfCopySize;
volatile unsigned long rfFlashAddress;
volatile unsigned long rfLoadAddress;
volatile unsigned long DspImageSize;

#define FLASH_CP_START  CpFlashAddress
#define FLASH_DSP_START DspFlashAddress
#define PSRAM_DSP_START DspExecAddress
#define DSP_COPY_SIZE   DspCopySize

#define DISPLAY_SIZE  32

UINT32 CopyInternalFlashInPsram(UINT32 FlashAddr,UINT32 FlashSize,UINT32 PsramStartAddr, UINT32 PsramEndAddr)
{
	UINT32 RetAddr;
	INT32  res;
	//check
	#define ALIGN_SIZE 0x1000
	RetAddr = (PsramStartAddr + ALIGN_SIZE - 1) &~ (ALIGN_SIZE - 1);	//4K align
	if(FlashSize > PsramEndAddr - RetAddr){
		CP_LOGE("[ERROR] Flash Size is more than psram region size.\r\n");
		while(1);
	}
	//copy
	res = asr_norflash_read(FlashAddr, (uint8_t *)RetAddr, FlashSize);
	if(res != 0){
		CP_LOGE("[ERROR] Flash Read Error.\r\n");
		while(1);
	}
	return RetAddr;
}


int copy_cp_from_flash_to_psram(void)
{
    //COPY CP.bin
    unsigned int tmp;
    CP_LOGI("start copy CP bin from [0x%0.8x] to [0x%0.8x] ,size=[0x%0.8x]\r\n",
            FLASH_CP_START,
            CpExecAddress,
            CpCopySize);

    memcpy( (void *)CpExecAddress,
            (void *)FLASH_CP_START,
            CpCopySize);
    
    tmp = memcmp((void*)CpExecAddress,
                 (void*)FLASH_CP_START,
                 CpCopySize);

    CP_LOGI("after memcopy , do memcmp result= %s\r\n",(tmp==0)?"SUCCEED":"FAILED");
    return tmp;
}

//int copy_dsp_from_flash_to_psram(unsigned int * inLen)
//{
//	unsigned long magic;
//    //COPY DSP.bin
//    unsigned int ret = -1;
//    unsigned int outLen = -1;
//    uart_printf("[DSP] decompress from [0x%.08x] to [0x%0.8x]\r\n", FLASH_DSP_START ,PSRAM_DSP_START);
//    uart_printf("[DSP] expected released dsp size [0x%.08x]\r\n", DSP_COPY_SIZE);

//	magic = *(volatile unsigned long*)FLASH_DSP_START;

//	if (0x4f5a4c89 == magic) //"a lzop image"
//	{
//        if(!lzop_decompress_safe((unsigned char *)FLASH_DSP_START,(unsigned char *)PSRAM_DSP_START,&outLen,inLen)){
//            uart_printf("[DSP] released dsp size [0x%.08x] inLen=[0x%.08x]\r\n", outLen,*inLen);
//            if(outLen != DSP_COPY_SIZE){
//                uart_printf("[DSP] WARNING: decompressed length [0x%.08x] unmatch with expected [0x%.08x].\r\n",outLen,DSP_COPY_SIZE);
//            }
//            ret = 0;
//        }else{
//	        uart_printf("\r\n ** ERROR: DSP image decompress failed .\r\n");
//        }
//	}else{
//	    uart_printf("\r\n ** ERROR: expected a compressed DSP image.\r\n");
//    }
//    return ret;
//}

void test_asr_norflash_api(void)
{
	UINT32 MAGIC;

	asr_norflash_read(0x90000000,&MAGIC,sizeof(MAGIC));
	CP_LOGD("MAGIC:0x%.08x\r\n",MAGIC);
}

INT32 copy_compressed_logo_from_flash_to_psram(   CopyImageInfo_t *p_CopyLogoInfo,const UINT32 HeaderFileSize)
{
    INT32 ret;
    INT32 outLen = -1;
	BOOL SwitchFlag = FALSE;
	UINT32 UncompressSrcAddr;
	UINT32 UncompressDstAddr;
	UINT32 inLen;

//	test_asr_norflash_api();

	if(NO_COMPRESSED == p_CopyLogoInfo->CompressedType){
		//[3.1] logo.bin is not compressed
		CP_LOGI("memcopy: begin, dst:0x%x, src:0x%x, size:0x%x\r\n",
		         p_CopyLogoInfo->LoadAddress,
		         p_CopyLogoInfo->FlashAddress,
		         p_CopyLogoInfo->FlashSize);
		asr_norflash_read(p_CopyLogoInfo->FlashAddress,(UINT8 *)p_CopyLogoInfo->LoadAddress,p_CopyLogoInfo->FlashSize);
		ret = 0;
	}else if(LZMA == p_CopyLogoInfo->CompressedType){
		CP_LOGI("start decompress logo\r\n");
		
		/* load logo header info from flash into psram */
		CP_LOGD("[LOGO] load header info from flash to psram\r\n");
		asr_norflash_read(p_CopyLogoInfo->FlashAddress,(UINT8 *)p_CopyLogoInfo->LoadAddress,HeaderFileSize);
		
		/* decompress compressed logo data from flash into psram */
		CP_LOGD("decompress logo from flash to psram\r\n");
		inLen = p_CopyLogoInfo->FlashSize - HeaderFileSize;
		UncompressDstAddr = p_CopyLogoInfo->LoadAddress  + HeaderFileSize;
		
		if(p_CopyLogoInfo->ImageInFlashType == pt_flash_external){
			UncompressSrcAddr = p_CopyLogoInfo->FlashAddress + HeaderFileSize;
		}else if(p_CopyLogoInfo->ImageInFlashType == pt_flash_internal){
//			UINT32 LzmaPsramStartAddr = get_heap_guard_addr();
//			UINT32 LzmaPsramEndAddr   = get_heap_endmark_addr();
			UINT32 LzmaPsramStartAddr = get_rw_zi_start_addr();
			UINT32 LzmaPsramEndAddr   = get_rw_zi_end_addr();
			
			UncompressSrcAddr = CopyInternalFlashInPsram(p_CopyLogoInfo->FlashAddress + HeaderFileSize,inLen,LzmaPsramStartAddr,LzmaPsramEndAddr);
		}
		
		CP_LOGD("src_addr:[0x%.08x],src_len:[0x%.08x],dst_addr:[0x%.08x], %s in flash type:%d\r\n",UncompressSrcAddr,inLen,
			UncompressDstAddr,p_CopyLogoInfo->ImageName,p_CopyLogoInfo->ImageInFlashType);
		
		if(LZMA == p_CopyLogoInfo->CompressedType){
			if(IsLzmaOptimize() == FALSE){
				LzmaOptimizeSwitch(TRUE);
			}
			ret = LzmaUncompress((unsigned char *)UncompressDstAddr,(size_t *)&outLen, 
				(const unsigned char *)UncompressSrcAddr,(size_t *)&inLen);
			if(IsLzmaOptimize() == TRUE){
				LzmaOptimizeSwitch(FALSE);
			}
		}
        CP_LOGD("inLen=[0x%.08x], outLen=[0x%.08x]\r\n",inLen,outLen);
	}
	
    return ret;
}


int copy_rf_from_flash_to_psram(unsigned int rfFlashAddress,unsigned int rfCopySize,unsigned int rfLoadAddress)
{
    //COPY rf.bin
    unsigned int tmp;
    if(rfLoadAddress == INVALID_ADDRESS)
    {
        uart_printf("[RF ] skip rf.bin load as rfLoadAddress = INVALID_ADDRESS\r\n");
        return 0;
    }
    uart_printf("[RF ] copy from [0x%0.8x] to [0x%0.8x] size=[0x%0.8x]\r\n",
            rfFlashAddress,
            rfLoadAddress,
            rfCopySize);

	{
	    memcpy( (void *)rfLoadAddress,
	            (void *)rfFlashAddress,
	            rfCopySize);
	    tmp = memcmp((void*)rfLoadAddress,
	                 (void*)rfFlashAddress,
	                 rfCopySize);

	    uart_printf("[RF ] memcmp result %s\r\n",(tmp==0)?"SUCCEED":"FAILED");
    }
    return tmp;
}

//unsigned int try_seprated_rw_cpzladdr_convert(unsigned int input_address){
//	static int addr_offset;
//	static int addr_offset_inited = 0;

//	//if input_address inside external flash , return input_address
//	if(input_address <= SPINOR_BASEADDR)
//		return input_address;

//	//if input_address inside internal flash , caculate the offset for further use
//	if(addr_offset_inited == 0){
//		addr_offset = input_address - Cp_2_FlashAddress;//the first input base
//		uart_printf("[CP ] try_seprated_rw_cpzladdr_convert offset = [0x%0.8x]\r\n",addr_offset);
//		addr_offset_inited = 1;
//	}

//	/*
//	uart_printf("try_seprated_rw_cpzladdr_convert [0x%0.8x]->[0x%0.8x]\r\n"
//			,input_address
//			,(input_address - addr_offset));
//	*/

//	return (input_address - addr_offset);
//}

//void region_decompress_from_flash_to_psram(void)
//{
//    char region_compress_mark[8];
//    unsigned schedule_count = 0;
//    rw_region_item region_info;
//	unsigned decompress_result,outLen,inLen;
//    char * rw_cpz_struct_addr=(char*)get_rw_cpz_struct_addr();


//    //load region compressed struct
//    memcpy(region_compress_mark,
//            rw_cpz_struct_addr,
//            sizeof(region_compress_mark));

//    if(!strncmp(region_compress_mark, RW_REGION_MARK_PRE_STRING ,strlen(RW_REGION_MARK_PRE_STRING)-1)){
//        CP_LOGI("Region CPZ struct detected from LDT\r\n");
//        while(1){
//            //read NEXT region cpz info struct
//            memcpy(&region_info
//                    ,(void*)(rw_cpz_struct_addr + (sizeof(rw_region_item)*schedule_count++))
//                    ,sizeof(rw_region_item));

//            if(strncmp(region_info.RW_REGION_MARK, RW_REGION_MARK_PRE_STRING ,strlen(RW_REGION_MARK_PRE_STRING)-1))
//            { 
//                //expend endpoint of RW decompress working flow for compressed image
//                CP_LOGW("stop decompress as no further %s detected\r\n",RW_REGION_MARK_PRE_STRING);
//                break;
//            }else if(region_info.RW_REGION_COMPRESSED_ADDR == RW_REGION_COMPRESSED_ADDR_NONE ){
//                //expend endpoint of RW decompress working flow for uncompressed image
//                //
//                //no RW_REGION_COMPRESSED_ADDR detected,
//                //image did not compressed with /x/tavor/Arbel/build/external_region_compress.pl script
//                //the decompress would be done by /hop/BSP/src/Cinit1.c
//                CP_LOGI("skip region decompress as no RW_REGION_COMPRESSED_ADDR detected\r\n");
//                break;
//            }

//            //dump region info
//            CP_LOGI("decompress [%8s] from [%.08x] to [%.08x]\r\n",
//                    region_info.RW_REGION_NAME,
//                    try_seprated_rw_cpzladdr_convert(region_info.RW_REGION_COMPRESSED_ADDR),
//                    region_info.RW_REGION_EXEC_ADDR
//                    );

//	    if(region_info.RW_REGION_COMPRESSED_ADDR >= SPINOR_BASEADDR){
//		    decompress_result = lzop_decompress_safe_internal_flash(
//			    (unsigned char*)try_seprated_rw_cpzladdr_convert(region_info.RW_REGION_COMPRESSED_ADDR),
//			    (unsigned char*)region_info.RW_REGION_EXEC_ADDR,
//			    &outLen,&inLen);
//	    }else{
//		    decompress_result = lzop_decompress_safe(
//			    (unsigned char*)region_info.RW_REGION_COMPRESSED_ADDR,
//			    (unsigned char*)region_info.RW_REGION_EXEC_ADDR,
//			    &outLen,&inLen);
//	    }

//            if(decompress_result != 0){
//                CP_LOGE("\r\n ** ERROR: RW DECOMPRESS ERROR RESULT=[%d]",decompress_result);
//                while(1) {};
//            }
//        }
//    }
//    else
//    {
//        CP_LOGW("Region CPZ struct not detected from LDT\r\n");
//    }
//}

#define UpdaterExecAddress 0x7E400000//bind on updater build sct loader config
void ota_entry(void) {
	volatile unsigned long FotaParamEndAddress;
	char fota_flag[6];

	//detect whether fota partition supported
	if(!is_fota_param_available()){
		uart_printf("[OTA] no available fota_param , FOTA not supported\r\n");
		return;
	}

	//check OTA flag or
	if(ptable_is_flash_on_internal("fota_param")){
//		asr3601s_spinor_flashinit();
		asr3601s_spinor_do_read(get_fota_param_start_addr() /*START*/
				,fota_flag
				,sizeof(fota_flag));
	}else{
		memcpy(fota_flag
				,get_fota_param_vstart_addr() /*VSTART*/
				,sizeof(fota_flag));
	}
	
	//if(!strncmp(fota_flag,"ASROTA",6) || Keypad_OTA_Flag_Check_B()){
	if(!strncmp(fota_flag,"ASROTA",6) ){
#ifdef 	ADD_VIBRATOR_IN_CODE
		//[0] Before jump to updater, disable vibrator
		NingboVibratorDisable();
#endif		
		//[1] TODO:detect ADUPS flags
		uart_printf("[OTA] ASROTA prefix detected or Keypad Flag detect!\r\n");

		//[2] TODO:load updater_a or updater_b to psram
		memcpy( (void *)UpdaterExecAddress,
			(void *)get_updater_start_addr(),
			get_updater_copy_size());

		//[3] jump to execute updater
		uart_printf("\r\n >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
		uart_printf("\r\n >> BOOTLOADER DONE JUMP TO UPDATER"); 
		uart_printf("\r\n >> PC      : 0x%x",UpdaterExecAddress);
		uart_printf("\r\n >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
		TransferControl(UpdaterExecAddress);
	}else{
		CP_LOGW("NO FOTA FLAG\r\n");
	}
}




// This typedef describes the structure at which the CPU register's image is kept.
   typedef struct  {/** Total size - 31*4 == 124 bytes == 0x7C bytes. **/
	   UINT32 usrR1_R14[14];/**size=0x38**/// place to store relevant user\system registers (r1-r14)
	   UINT32 cpsr; 		/**size=0x4 **/// place to store the CPSR
	   UINT32 fiqR8_R14[7]; /**size=0x1C**/// place to store all FIQ registers	(r8-r14)
	   UINT32 abtSP_LR[2];	/**size=0x8 **/// place to store the abort stack pointer and link register
	   UINT32 undSP_LR[2];	/**size=0x8 **/// place to store the undefined stack pointer and link register
	   UINT32 irqSP_LR[2];	/**size=0x8 **/// place to store the interrupt stack pointer and link register
	   UINT32 svcSP_LR[2];	/**size=0x8 **/// place to store the supervisor stack pointer and link register
   }CommPM_CPUImageRegsS;

typedef struct { // this structure contains a 32 bit field for every R/W 
				  // register of CP15 that might be modified after init (in run time).
	UINT32 dCacheLockdown;	/* inst. cache lockdown - register 9 */
	UINT32 iCacheLockdown;	/* data  cache lockdown - register 9 */
	UINT32 traceProcessID;	/* trace process ID 	- register 13*/ 
	UINT32 testState;		/* Test state			- register 15*/
	UINT32 cacheDebugIndex; /* Cache debug index	- register 15*/
}CommPM_CP15BackupS;

typedef struct {
        CommPM_CPUImageRegsS cpuRegs; // This field must be first (the assembly code relies on it).
        CommPM_CP15BackupS   cp15Backup;
}CommPM_DTCM_DSS;


typedef struct { 
	UINT32 ImageMagic; 
	UINT32 ImageInitialEntryPoint;
	UINT32 ImageLoadAddress;
	UINT32 ImageHeaderSize;
#ifdef SUPPORT_COMPRESSED_LOGO
	UINT32 logo_compressed_magic;
#endif	
}ImageHeader;




CommPM_DTCM_DSS _logoContextRegs;
UINT32 reg_oldR0;
extern void saveContext(UINT32 R0, UINT32 R1);
extern void restoreContext(void);
#if 0
void printlog(UINT32 val)
{
	uart_printf("context: 0x%x\r\n", val);
    int i;
	UINT32 *pt;
	pt = (UINT32 *)&_logoContextRegs;
	for (i=0; i<sizeof(CommPM_DTCM_DSS); i++)
		uart_printf("0x%x\r\n", *pt++);
}
#endif




void LogoEntry(void)
{
	volatile UINT32 LogoFlashStartAddress;
	volatile UINT32 LogoFlashSize;
	ImageHeader logo_header_block_info;
	CompressedType_e CompressedType;
	CopyImageInfo_t  CopyLogoInfo;
	INT32 ret = -1;

	//[0] logo header table init.( will detect logo exist and valid in funtion internal)
	logo_header_table_init();
	
	//[1] get logo.bin flash info
	LogoFlashStartAddress = get_logo_flash_start_addr();
	LogoFlashSize = get_logo_flash_size();
	CP_LOGD("logo image in flash start addr: 0x%x ,Size: 0x%x\r\n",
				 LogoFlashStartAddress,
				 LogoFlashSize);


 	//[3] read logo header block info from flash into psram
	asr_norflash_read(LogoFlashStartAddress,(UINT8 *)&logo_header_block_info,sizeof(ImageHeader));
#ifdef SUPPORT_COMPRESSED_LOGO
	asr_norflash_read(LogoFlashStartAddress+logo_header_block_info.ImageHeaderSize,(UINT8 *)&logo_header_block_info.logo_compressed_magic,sizeof(logo_header_block_info.logo_compressed_magic));
	CP_LOGI("logo compressed magic: 0x%x\r\n",logo_header_block_info.logo_compressed_magic);
#endif
	CP_LOGI("logo magic:0x%x, entry point: 0x%x,load addr: 0x%x, header file size:0x%x\r\n",
		         logo_header_block_info.ImageMagic, 
		         logo_header_block_info.ImageInitialEntryPoint, 
		         logo_header_block_info.ImageLoadAddress,
		         logo_header_block_info.ImageHeaderSize
		    );	

	if(logo_header_block_info.ImageHeaderSize == 0){
		CP_LOGE("[LOGO] Error, logo header size: 0x%.08x is errror!\r\n",logo_header_block_info.ImageHeaderSize);
		while(1);
	}

	//[3] load logo.bin to psram
	CopyLogoInfo.ImageName        = "logo";
	CopyLogoInfo.CompressedType   = GetCompressedType(logo_header_block_info.logo_compressed_magic);
	CopyLogoInfo.FlashAddress     = LogoFlashStartAddress;
	CopyLogoInfo.LoadAddress      = logo_header_block_info.ImageLoadAddress;
	CopyLogoInfo.FlashSize        = LogoFlashSize;
	CopyLogoInfo.ImageInFlashType = ptable_get_flash_type(CopyLogoInfo.ImageName);


	ret = copy_compressed_logo_from_flash_to_psram(&CopyLogoInfo,logo_header_block_info.ImageHeaderSize);
	if(0 != ret){
		CP_LOGE("[LOGO] copy logo.bin err!\r\n");
		while(1);
	}
	
	//[4] jump to execute updater
	CP_LOGD(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n ");
	CP_LOGI("B33 JUMP TO LOGO at 0x%x\r\n", logo_header_block_info.ImageInitialEntryPoint); 
	CP_LOGD(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
	
	TransferControlToLogo(logo_header_block_info.ImageInitialEntryPoint, NULL);
}

#if 0
//add to detect ota flag
BOOL ota_flag_check(void) {
	volatile unsigned long FotaParamStartAddress;
	volatile unsigned long FotaParamEndAddress;
	char fota_flag[6];

	//detect whether fota partition supported
	FotaParamStartAddress = get_fota_param_start_addr();
	FotaParamEndAddress   = get_fota_param_end_addr();
	if(FotaParamStartAddress == FotaParamEndAddress){
		uart_printf("[ota_flag_check] FotaParamStartAddress == FotaParamEndAddress , FOTA not supported\r\n");
		return 0;
	}
	//check OTA flag or
	memcpy(fota_flag,FotaParamStartAddress,sizeof(fota_flag));

	if(!strncmp(fota_flag,"ASROTA",6)){
		uart_printf("[ota_flag_check] OTA FLAG DETECTED \r\n");
		return 1;
	}else{
		uart_printf("[ota_flag_check] NO OTA FLAG DETECTED \r\n");
		return 0;
	}
}
#endif
extern int fotalcd_init(void);

struct asrlcdd_screen_info{
    unsigned int width;
    unsigned int height;
};

//transfer_parameter param;
extern unsigned int Load$$CODE$$Base;
extern unsigned int Image$$CODE$$Base;
extern unsigned int Image$$CODE$$Length;

extern unsigned int Load$$DATA$$RW$$Base;
extern unsigned int Image$$DATA$$RW$$Base;
extern unsigned int Image$$DATA$$RW$$Length;

extern unsigned int Image$$DATA$$ZI$$Base;
extern unsigned int Image$$DATA$$ZI$$Length;

extern unsigned int Load$$ITCM$$Base;
extern unsigned int Image$$ITCM$$Base;
extern unsigned int Image$$ITCM$$Length;

struct asrlcdd_screen_info  fota_mciLcdFBInfo;
struct asrlcdd_screen_info  fota_mciLcdScreenInfo;
extern unsigned char *g_asrlcd_framebuffer;
extern void fotaLcdflush(UINT16* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy);

//Replace Text or image --> background color
void Fota_Clear_LCD(void)
{

	int i = 0;
	int k = 0;
	UINT16 *p_image_buf = (UINT16 *)g_asrlcd_framebuffer;

	//fatal_printf("%s,lcd(%dx%d)\r\n",__func__,fota_mciLcdFBInfo.height,fota_mciLcdScreenInfo.width);

	for(i = 0; i<fota_mciLcdFBInfo.height; i++) {
		for(k = 0; k <fota_mciLcdFBInfo.width/2; k++) {
			p_image_buf[i*fota_mciLcdFBInfo.width/2+k] = 0xFFFF;
		}
	}

}


void Fota_lcdshow(UINT8* buffer)
{
	uart_printf("%s,LCD: refresh: (%d*%d)!\n",__func__, fota_mciLcdFBInfo.width, fota_mciLcdFBInfo.height);
	fotaLcdflush((UINT16*)buffer,0,0,(fota_mciLcdFBInfo.width/2-1),(fota_mciLcdFBInfo.height-1));
//	OSATaskSleep(10);

}

#define REDCOLOR (0xf800)

#define GREENCOLOR (0x07E0)

#define BLUECOLOR (0x001F)

void Fota_displaycolor(UINT16 color)
{

	int i = 0;
	int k = 0;
	UINT16 *p_image_buf = (UINT16 *)g_asrlcd_framebuffer;

	//fatal_printf("%s,lcd(%dx%d)\r\n",__func__,fota_mciLcdFBInfo.height,fota_mciLcdScreenInfo.width);

	for(i = 0; i<fota_mciLcdFBInfo.height; i++) {
		for(k = 0; k <fota_mciLcdFBInfo.width/2; k++) {
			p_image_buf[i*fota_mciLcdFBInfo.width/2+k] = color;
		}
	}

}

void Fota_displayRGB(void)
{

	int i = 0;
	int k = 0;
	UINT16 *p_image_buf = (UINT16 *)g_asrlcd_framebuffer;
	UINT16  color;

	//fatal_printf("%s,lcd(%dx%d)\r\n",__func__,fota_mciLcdFBInfo.height,fota_mciLcdScreenInfo.width);

	for(i = 0; i<fota_mciLcdFBInfo.height; i++) {

		if(i<(fota_mciLcdFBInfo.height/3)){
			for(k = 0; k <fota_mciLcdFBInfo.width/2; k++) {
				p_image_buf[i*fota_mciLcdFBInfo.width/2+k] = REDCOLOR;
			}
		}
		else if (i<((fota_mciLcdFBInfo.height*2)/3))
		{
			for(k = 0; k <fota_mciLcdFBInfo.width/2; k++) {
				p_image_buf[i*fota_mciLcdFBInfo.width/2+k] = GREENCOLOR;
			}
		}
		else
		{
			for(k = 0; k <fota_mciLcdFBInfo.width/2; k++) {
				p_image_buf[i*fota_mciLcdFBInfo.width/2+k] = BLUECOLOR;
			}
		}
	}

}
extern void USBDevicePhase1Init(void);
extern void usb_device_init(void );
extern void timerPhase1Init(void);
extern void TickPhase1Init(void);
extern void INTCPhase2Init(void);
extern void timerPhase2Init(void);
extern void InitTimers(void);
extern void initTimerCount13MHz(void);
extern void TickPhase2Init(void);
extern void NUTickRegister(void);
extern void dumpTimerReg(TCR_NUMBER tcr_number);
void bootloader(void)
{
    unsigned int val;
    volatile UINT8 var;

    _ptentry *cpEntry, *cp_2_Entry;

	unsigned int src_addr;
	unsigned int dst_addr;
	unsigned int len;
	unsigned int i;
	unsigned int *psrc, *pdst;

	src_addr=(unsigned int)&(Load$$ITCM$$Base);
	dst_addr=(unsigned int)&(Image$$ITCM$$Base);
	len=(unsigned int)&(Image$$ITCM$$Length);
	len=(len+3)/4;
	psrc=(unsigned int *)src_addr;
	pdst=(unsigned int *)dst_addr;
	for(i=0;i<len;i++)
		*pdst++=*psrc++;
	
	src_addr=(unsigned int)&(Load$$CODE$$Base);
	dst_addr=(unsigned int)&(Image$$CODE$$Base);
	len=(unsigned int)&(Image$$CODE$$Length);
	len=(len+3)/4;
	psrc=(unsigned int *)src_addr;
	pdst=(unsigned int *)dst_addr;
	for(i=0;i<len;i++)
		*pdst++=*psrc++;

	src_addr=(unsigned int)&(Load$$DATA$$RW$$Base);
	dst_addr=(unsigned int)&(Image$$DATA$$RW$$Base);
	len=(unsigned int)&(Image$$DATA$$RW$$Length);
	len=(len+3)/4;
	psrc=(unsigned int *)src_addr;
	pdst=(unsigned int *)dst_addr;
	for(i=0;i<len;i++)
		*pdst++=*psrc++;


	dst_addr=(unsigned int)&(Image$$DATA$$ZI$$Base);
	len=(unsigned int)&(Image$$DATA$$ZI$$Length);
	len=(len+3)/4;
	pdst=(unsigned int *)dst_addr;
	for(i=0;i<len;i++)
		*pdst++=0;

    //uart init
	cp_uart_init();
    CP_LOGI("B33 START\r\n");
//	usticaInit();
    PMIC_Init();

	//Print VB_VERSION INFO
	extern Boot33VerBlockType boot33_vb;
	CP_LOGD("[BOOT33]VB_VERSION_DATE   			:[%s]\r\n",boot33_vb.vb.version_block.version_date);
	CP_LOGI("SECBOOT_SUPPORT:%s\r\n",boot33_vb.vb.version_block.secboot_support);
	CP_LOGD("[BOOT33]VB_COMPRESSED_LOGO_SUPPORT :[%s]\r\n",boot33_vb.vb.version_block.logo_compress_support);
	CP_LOGI("ver:%s\r\n",boot33_vb.vb.version_block.build_info_string);

	asr_property_set("b33.version",boot33_vb.vb.version_block.build_info_string);
	asr_property_set("b33.secboot",boot33_vb.vb.version_block.secboot_support);
	asr_property_set("b33.compressed_logo",boot33_vb.vb.version_block.logo_compress_support);
	asr_property_dump();

#if 1//disable to skip onkey-detect for local debug
    if (!IfWdtResetTriggered())
    {
        var = PM812_GET_POWER_UP_LOG();
        CP_LOGI("PowerOn: 0x%x\r\n", var);
        if (var&(0x1<<0)) /*onkey reason*/
            OnkeyPowerOnCheck();
        else if ((var&(0x1<<6))&&(!PMIC_CHARGER_IS_DETECTED()))
            PM812_SW_PDOWN();
		
#ifdef 	ADD_VIBRATOR_IN_CODE
        NingboVibratorEnable();
#endif
    }
#endif
//    CP_LOGD("Built by DS-5.\r\n");

    /* Enable MPU and using the cortex-r default memory map as the background region */
    val = sctlr_get();
    val |= SCTLR_BR | SCTLR_M;
    sctlr_set(val);

    CP_LOGD("boot33 MPU config\r\n");

    set_mpu_reg_0();
    set_mpu_reg_3();
    set_mpu_reg_4();
    set_mpu_for_DSP();
//	mpu_value_check();
//    PMIC_Init();
//	sdcard_init();
#ifdef AVOID_PTABLE
    CpFlashAddress = CP_BASE_ADDR;
#else
    //PTABLE INIT AND DUMP
    if (ptable_init())
    {
        CP_LOGD("bad partition table!\r\n");
        goto FAIL;
    }
//  ptable_dump();

	//KEY PARTITION INFO FETCH
    cpEntry = ptable_find_entry("cp");
    CpFlashAddress = cpEntry->vstart;
    if(cpEntry == NULL)CpFlashAddress = CP_BASE_ADDR;//default address as backup one

    cp_2_Entry = ptable_find_entry("cp_2");
    if(cpEntry != NULL)Cp_2_FlashAddress = cp_2_Entry->vstart;
#endif
    //QSPI FLASH INIT
    bbu_qspi_init();
	if(is_partition_internal_flash(PTABLE_FLASH_INTERNAL) == TRUE){
		asr3601s_spinor_flashinit();
	}

	INTCPhase1Init();
	timerPhase1Init();
	//USBDevicePhase1Init();
	TickPhase1Init();

    //LOADTABLE INIT AND DUMP
    loadtable_init(CpFlashAddress);
	malloc_init(g_compressed_buffer, COMPRESSED_ALGORITHM_BUFFER_SIZE);

	unsigned int  uiwidth;
	unsigned int  uiheigh;
	uart_printf("%s-01\r\n",__func__);
	sysSetLocalInterrupt(ENABLE_IRQ);
	INTCPhase2Init();
	uart_printf("%s-02\r\n",__func__);
	timerPhase2Init();
	uart_printf("%s-03\r\n",__func__);
	InitTimers();
	uart_printf("%s-04\r\n",__func__);
	initTimerCount13MHz();
	uart_printf("%s-05\r\n",__func__);
	NUTickRegister();
	TickPhase2Init();
	//usb_device_init();

#if 0	
	fotalcd_init();

	hal_getlcdinfo(&uiwidth,&uiheigh);
	uart_printf("%s,uisize(%d,%d)!\n",__func__,uiwidth,uiheigh);
	fota_mciLcdFBInfo.height=uiheigh;
	fota_mciLcdFBInfo.width=uiwidth*2;

	fota_mciLcdScreenInfo.height=fota_mciLcdFBInfo.height;
	fota_mciLcdScreenInfo.width=fota_mciLcdFBInfo.width/2;

	Fota_Clear_LCD();
	Fota_lcdshow(g_asrlcd_framebuffer);
	mdelay(1000);

	Fota_displaycolor(REDCOLOR);
	Fota_lcdshow(g_asrlcd_framebuffer);
	mdelay(1000);
	Fota_displaycolor(GREENCOLOR);
	Fota_lcdshow(g_asrlcd_framebuffer);
	mdelay(1000);
	Fota_displaycolor(BLUECOLOR);
	Fota_lcdshow(g_asrlcd_framebuffer);

	mdelay(1000);
    //ota_entry();
	//LogoEntry();
	Fota_displayRGB();
	Fota_lcdshow(g_asrlcd_framebuffer);
	mdelay(1000);	
	dump_loadtable();
#endif	
	while(1)
	{
		uart_printf("while continue 0x%x\r\n",timerCountRead(TS_TIMER_ID));
		//dumpTimerReg(TS_TIMER_ID);
		mdelay(1000);	

	}






#ifdef BOOT33_SECBOOT_SUPPORT
	#include "secboot.h"
	secboot_init();
	Timer0_enable(TRUE);
	/*
	currenttly do not support image inside internal 8M spi_nor flash
	below items under secboot check , also open config to customer
	refer to their owner fip image config inside of ABOOT config.json
	*/
	secboot_item_check(ITEM_CP);     //item CP_IMAGE_ID of fip .json
	secboot_item_check(ITEM_DSP);    //item DSP_IMAGE_ID of fip .json
	secboot_item_check(ITEM_APP);    //item APP_IMAGE_ID of fip .json
	secboot_item_check(ITEM_USER1);  //item USER1_IMAGE_ID of fip .json
	secboot_item_check(ITEM_USER2);  //item USER2_IMAGE_ID of fip .json
	secboot_item_check(ITEM_USER3);  //item USER3_IMAGE_ID of fip .json
	Timer0_enable(FALSE);
#endif

    //anti rollback check
    if(anti_rollback_check()){
        goto FAIL;
    }

   CpExecAddress = get_cp_exec_addr(); //CP Exec Addr


    // [3] transfer control
    CP_LOGD("********************************************\r\n");
    CP_LOGI("B33 DONE\r\n"); 
    CP_LOGD("** VERSION : %s\r\n ",boot33_vb.vb.version_block.version_date);
    CP_LOGI("PC:0x%x\r\n",CpExecAddress);
    CP_LOGD("********************************************\r\n");
    TransferControl(CpExecAddress);
FAIL:
    CP_LOGE("Bootloader FAILED");
    while(1);
}

