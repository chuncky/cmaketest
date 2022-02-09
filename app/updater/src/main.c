#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "qspi.h"
#include "reg.h"
#include "system.h"
//#include "uart.h"
#include "ptable.h"
#include "loadtable.h"
#include "sdio.h"
#include "pmic.h"
#include "ff.h"
#include "OTA_sdcard.h"
#include "version_block.h"
#include "mci_lcd.h"
#include "ningbo.h"
#include "spi_nor.h"
#include "lcd_test.h"

#define fatal_printf uart_printf

extern void cp_uart_init(void );



int qspi_flash_init(void);
#ifdef FOTA_ASRSPI
extern unsigned int Load$$ITCM$$Base;
extern unsigned int Image$$ITCM$$Base;
extern unsigned int Image$$ITCM$$Length;
#endif
extern unsigned int Load$$CODE$$Base;
extern unsigned int Image$$CODE$$Base;
extern unsigned int Image$$CODE$$Length;
extern unsigned int Load$$DATA$$RW$$Base;
extern unsigned int Image$$DATA$$RW$$Base;
extern unsigned int Image$$DATA$$RW$$Length;
extern unsigned int Image$$DATA$$ZI$$Base;
extern unsigned int Image$$DATA$$ZI$$Length;

FIL handle_lcd;
unsigned short picture_buf_0[153600/2];
unsigned short picture_buf_1[153600/2];

#ifdef WATCHLCDST7789_CODE_USE
unsigned short picture_buf_2[115200/2];
#else
unsigned short picture_buf_2[153600/2];
#endif

/********************* Dummy Function ******************************/
void CacheCleanMemory(void *pMem, UINT32 size)
{
	return;
}
/*****************************************************************/

int set_mpu_reg(void )
{
    unsigned int cnt = 5;
    unsigned int acc_ctrl;

    //region 5: 0x7E600000, 1M , noncache
    acc_ctrl = MPU_REGION_ACCESS_CTRL_AP(0x3)|MPU_REGION_ACCESS_CTRL_TEX(0x1);
    mpu_set_region(cnt, 0x7E600000, 0x00100000, acc_ctrl);
    mpu_enable_region(cnt, 1);

/* below is just a cache cfg sample
    //region 6: 0x7E700000, 1M , cache
	cnt++;
    acc_ctrl = MPU_REGION_ACCESS_CTRL_AP(0x3)|MPU_REGION_ACCESS_CTRL_TEX(0x0);
    mpu_set_region(cnt, 0x7E700000, 0x00100000, acc_ctrl);
    mpu_enable_region(cnt, 1);
*/
    return 0;
}
extern void mpu_value_check(void);

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




void updater_delay_ms(UINT32 ms)
{
	INT32 cnt = 0x29F9;  //uart_log cal as: 1ms
	volatile INT32 i = 0;
	
	for(;ms != 0; ms--){
		i = cnt;
		for(;i != 0; i--);
	}
}


#ifdef FOTA_ASRSPI	//1. function global var declaration and other function
/* 1. function or variable declaration */
extern void spi_rw_test_init(void);
extern void mrdy_srdy_gpio_init(void);
extern UINT8  ap2cpbuf[1024 * 2];

#pragma arm section zidata="TEST_NONCACHE_DATA"
//char test_noncache_buffer[128]={0};
//UINT32 dma_spi_recv_buf[512] = {0};
//UINT32 dma_spi_send_buf[512] = {0};
__align(32) UINT32 Mux2SPITxBuffer[512]={0};
__align(32) UINT32 SlaveRxbuffer[512]={0};
__align(32) UINT32 DummyTxbuffer[512]={0};
__align(32) UINT8 P1dmaDesc[(SSP_READ_DMA_DESC*sizeof(DMA_DESCRIPTOR) +16)];
__align(32) UINT8 P2dmaDesc[(SSP_READ_DMA_DESC*sizeof(DMA_DESCRIPTOR) +16)];
#pragma arm section
#endif


extern void OTA_LCD_DisplayText(const char *p_addtional_text,const OTA_LCD_DISPLAY_INFO_TYPE ota_info_type,const OTA_LANGUAGE_T ota_language);
extern MCI_ASR_LCD_INFO lcd_info_test;


#define VOLT_THRESHOID (3800)
#ifdef FOTA_ASRSPI
void dump_ITCM_addr(void)
{
	uart_printf("Load$$ITCM$$Base:0x%x\r\n",(unsigned int)&(Load$$ITCM$$Base));
	uart_printf("Image$$ITCM$$Base:0x%x\r\n",(unsigned int)&(Image$$ITCM$$Base));
	uart_printf("Image$$ITCM$$Length:0x%x\r\n",(unsigned int)&(Image$$ITCM$$Length));
}
#endif
void updater(void)
{
	volatile unsigned long FotaParamStartAddress;
	volatile unsigned long FotaParamEndAddress;
	volatile unsigned long FotaPkgStartAddress;
	volatile unsigned long FotaPkgEndAddress;


	
	volatile unsigned long CpFlashAddress;
	char fota_flag[32];
	volatile UINT32 volt;
	BOOL nStat = TRUE;

	unsigned int val;
	unsigned int src_addr;
	unsigned int dst_addr;
	unsigned int len;
	unsigned int i;
	unsigned int *psrc, *pdst;
#ifdef FOTA_ASRSPI
	src_addr=(unsigned int)&(Load$$ITCM$$Base);
	dst_addr=(unsigned int)&(Image$$ITCM$$Base);
	len=(unsigned int)&(Image$$ITCM$$Length);
	len=(len+3)/4;
	psrc=(unsigned int *)src_addr;
	pdst=(unsigned int *)dst_addr;
	for(i=0;i<len;i++)
		*pdst++=*psrc++;
#endif	
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
	uart_printf("[UPDATER] UART INIT DONE\r\n");

    /* Enable MPU and using the cortex-r default memory map as the background region */
    val = sctlr_get();
    val |= SCTLR_BR | SCTLR_M;
    sctlr_set(val);
    uart_printf("updater MPU config\r\n");
    set_mpu_reg();
	mpu_value_check();
//	uart_printf("test_noncache_buffer addr = %x",test_noncache_buffer);
	//Print VB_VERSION INFO
	extern UpdaterVerBlockType updater_vb;
	uart_printf("[UPDATER]VB_VERSION_DATE   :[%s]\r\n",updater_vb.vb.version_block.version_date);
	uart_printf("[UPDATER]VB_OEM_LCD_TYPE   :[%s]\r\n",updater_vb.vb.version_block.oem_lcd_type);
	uart_printf("[UPDATER]VB_FOTA_SOLUTION  :[%s]\r\n",updater_vb.vb.version_block.fota_solution);
	uart_printf("[UPDATER]build_info_string :[%s]\r\n",updater_vb.vb.version_block.build_info_string);

    //*(volatile unsigned long*)APBTIMER0_EN_REG = 0x1; //enable Timer0_0 (in free run)

	PMIC_Init();
#ifdef FOTA_ASRSPI	
//	dump_ITCM_addr();
#endif
	
#ifndef FOTA_ASRSPI
	/* NOTE: do not check battery in 2-chip solution, as battery not place in cp */
	/* make sure battery voltage is more than 3.8v */
	uart_printf("[UPDATER] Check battery voltage\r\n");	
	while(1){
		//check battery voltage
		volt = pm813_get_bat_vol();
		if(volt > VOLT_THRESHOID){
			uart_printf("[UPDATER] battery voltage: %d\r\n",volt);
			break;
		}
		//check usb is detected ?
		nStat = PM812_CHARGER_IS_DETECTED();
		if(!nStat){
            uart_printf("[UPDATER] Charger disconnect\r\n");
            updater_delay_ms(1);
            PMIC_PowerDown();
            break;
        }
		//delay to be in charging
		mdelay(1000);
	}
#endif	
	
	//detect OTA_Sd_Key_Flag; 
	/*Before "sdcard_init", detect OTA_sdcard keypad combination to avoid waitting "sdcard_init".*/
#if 0 
	if(Keypad_OTA_Flag_Check_U())
		strncpy(OTA_Sd_KeyFlag,OTA_FLAG_SDCARD_KEY_FLAG,strlen(OTA_FLAG_SDCARD_KEY_FLAG)+1);
#endif
//#if (defined FOTA_ASRSD || defined FOTA_ASRSPI)
#if (defined FOTA_ASRSD)

	sdcard_init();
#endif
#ifdef LCD_USE_IN_CODE
	//mci_Logo_LcdClose();	//Close before Logo Progress Bar
	
	mci_Logo_LcdInit();	
	OTA_Clear_LCD();
    //lcd_test();
    //test_logo_LcdShowProcess_Demo();	
#endif
	//QSPI FLASH INIT
	bbu_qspi_init();
	CpFlashAddress = CP_BASE_ADDR;

	uart_printf("dump ptable in updater\r\n");
	ptable_dump();
	
	loadtable_init(CpFlashAddress);
	
	
#ifdef FOTA_ASRSPI 
	spi_rw_test_init();
	mrdy_srdy_gpio_init();

	//OTA_SPI_UPDATE();
    //while(1);
#endif
#if 1	
	//detect FOTA-flag 
	FotaParamStartAddress = get_fota_param_start_addr();
	FotaParamEndAddress   = get_fota_param_end_addr();
	FotaPkgStartAddress=get_fota_pkg_start_addr();
	FotaPkgEndAddress=get_fota_pkg_end_addr();
	uart_printf("[UPDATER] fotapkgstartaddr=0x%x,FotaPkgEndAddress=0x%x\r\n",FotaPkgStartAddress,FotaPkgEndAddress);
	
#ifndef FOTA_ASRSPI 
	if(FotaPkgStartAddress>=SPINOR_BASEADDR){
		uart_printf("[UPDATER] fotapkg in the internal flash,init the internal flash.\r\n");
		asr3601s_spinor_flashinit();
	}
#endif

#ifdef LCD_USE_IN_CODE
	/* init some gloabl variables related lcd */
	init_lcd_vars();
#endif
				
	if(FotaParamStartAddress == FotaParamEndAddress){
		uart_printf("[UPDATER] FotaParamStartAddress == FotaParamEndAddress , should never be here\r\n");
		goto FAILED;
	}

	//check FOTA flag: "OTA_Flag"
	if(FotaParamStartAddress<SPINOR_BASEADDR){
		memcpy(fota_flag,FotaParamStartAddress,sizeof(fota_flag));
	}
#ifndef FOTA_ASRSPI 
	else
	{
		uart_printf("[UPDATER] FotaParamStartAddress=0x%x\r\n",FotaParamStartAddress);
		asr_norflash_read(FotaParamStartAddress,fota_flag,sizeof(fota_flag));
	}
#endif
	if(!strncmp(fota_flag,"ASROTA",strlen("ASROTA"))){
		//[1] detect ASROTA-F flags
		uart_printf("[FOTA] ASROTA detected\r\n");

#ifdef LCD_USE_IN_CODE
		//[2] If need, open blacklight and show lcd info text.
#ifdef FOTA_ADUPS
		OTA_LCD_DisplayText(NULL,OTA_DISPLAY_INFO_TYPE_UPGRADE_PRE,g_langulage_type);
#endif
		mci_LcdSetBrightness(5);
#endif

#ifdef FOTA_ADUPS
		extern unsigned char datastorein;
		if(!strncmp(fota_flag,"ASROTA-F-GS",strlen("ASROTA-F-GS"))){
			//[3] ADUPS update process
			datastorein = 0;
			AUDPSProcedure();
		}
		else if(!strncmp(fota_flag,"ASROTA-R-GS",strlen("ASROTA-R-GS"))){
			//[3] ADUPS update process
			datastorein = 1;
			AUDPSProcedure();
		}
		else if(!strncmp(fota_flag,"ASROTA-T-GS",strlen("ASROTA-T-GS"))){
			//[3] ADUPS update process
			sdcard_init();
			datastorein = 2;
			AUDPSProcedure();
		}

#endif

#ifdef FOTA_RED_ALI
	 if(!strncmp(fota_flag,"ASROTA-F-RS",strlen("ASROTA-F-RS"))){
			//[4] FOTA solution: Redstone
			if(rs_ua_execute())
				uart_printf("UPDATER FAILED\r\n");
			else
				uart_printf("UPDATER SUCCESS\r\n");

			clear_fota_magic();
#ifdef LCD_USE_IN_CODE
			mci_LcdSetBrightness(0);	
#endif 
			pm813_userdata_set_for_automation_test(RESET_FOTA_FLAG);
			PMIC_SW_RESET();
		}
		//[5] FOTA solution: ALI
		else if(!strncmp(fota_flag,"ASROTA-F-ALI",strlen("ASROTA-F-ALI"))){
		//ALIOTA:may be create a new ota_flag, and new if-else branch
#ifdef LCD_USE_IN_CODE
			//mci_LcdSetBrightness(5);	
#endif
			uart_printf("start ali ota.\r\n");
			if(ota_updater() == (-1)){
				uart_printf("ali ota err.\r\n");
				//while(1);
			}
			uart_printf("end ali ota.\r\n");
	
#ifdef LCD_USE_IN_CODE
			mci_LcdSetBrightness(0);	
#endif
			pm813_userdata_set_for_automation_test(RESET_FOTA_FLAG);
			PMIC_SW_RESET();
		}
#endif //FOTA_REDSTONE_AND_ALI

#ifdef FOTA_ASRDM
		if(!strncmp(fota_flag,"ASROTA-F-DM",strlen("ASROTA-F-DM"))){
			//[6 FOTA solution: ASRDM
			if(datamodule_fota())
				uart_printf("UPDATER FAILED\r\n");
			else
				uart_printf("UPDATER SUCCESS\r\n");
	
			clear_fota_magic();
#ifdef LCD_USE_IN_CODE
			mci_LcdSetBrightness(0);	
#endif 
			pm813_userdata_set_for_automation_test(RESET_FOTA_FLAG);
			PMIC_SW_RESET();
		}

#endif
		if(!strncmp(fota_flag,"ASROTA-S",8)){
#ifdef FOTA_ASRSD
				//ASR_OTA_Sdcard
				if(!strncmp(fota_flag,"ASROTA-S",8))	//OTA_Sdcard[1]  OTA_Flag
					ASR_OTA_Sdcard();
				else if(!strncmp(OTA_Sd_KeyFlag,OTA_FLAG_SDCARD_KEY_FLAG,strlen(OTA_FLAG_SDCARD_KEY_FLAG)))	//OTA_Sdcard[2]   Keypad Combination
					ASR_OTA_Key_Sdcard();
				
#ifdef LCD_USE_IN_CODE
				mci_LcdSetBrightness(0);	
#endif
				pm813_userdata_set_for_automation_test(RESET_FOTA_FLAG);
				PMIC_SW_RESET();
#endif
		}else if(!strncmp(fota_flag,"ASROTA2S",8)){
#ifdef FOTA_ASRSPI
			OTA_SPI_UPDATE();
#endif
		}
	}
	else{
		uart_printf("[FOTA] NO FOTA FLAG DETECTED \r\n");
		goto FAILED;
	}
#endif
FAILED:
    uart_printf("UPDATER FAILED\r\n");
    while(1);
}
