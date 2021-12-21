#include "adups_typedef.h"
#include "adups_bl_main.h"
#include "common.h"


#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include "pmic.h"
#include "ningbo.h"
#include "mci_lcd.h"
#include "lcd_test.h"


#define ADUPS_FOTA_ENABLE_DIFF_ERROR 1
#define ADUPS_FOTA_ENABLE_DIFF_DEBUG 1


static adups_int32 adups_bl_debug_switch = 0;
adups_uint8 adups_delta_num=0;
adups_uint8 adups_curr_delta=1;
multi_head_info_patch PackageInfo = {0};

extern int fotapatch_procedure(void);
extern void frame_memory_finalize(void);
extern ADUPS_BOOL adups_init_mem_pool(adups_uint8* buffer, adups_uint32 len);

#define LOG_DEBUG NULL

adups_uint8 adups_debug_buffer[128];

void adups_bl_debug_print(void* ctx, const adups_char* fmt, ...) 
{
	va_list args;
	memset(adups_debug_buffer, 0x0, sizeof(adups_debug_buffer));
	
	va_start(args, fmt);
	vsnprintf(adups_debug_buffer, 128, fmt, args);
	uart_printf("%s\r\n", adups_debug_buffer);
	va_end(args);
}

ADUPS_BOOL adups_get_pre_patch_flag(void)
{
	return ADUPS_FALSE;
}

ADUPS_BOOL adups_get_pre_check_flag(void)
{
	return ADUPS_TRUE;
}

extern adups_uint32 Image$$FOTA_BUFFER$$Base;
extern adups_uint32 Image$$FOTA_BUFFER_ENDMARK$$Base;
#define FOTA_BUFFER_ADDR_START	((adups_uint32)&(Image$$FOTA_BUFFER$$Base))
#define FOTA_BUFFER_ADDR_END	((adups_uint32)&(Image$$FOTA_BUFFER_ENDMARK$$Base))

adups_uint8* adups_bl_get_working_buffer(void)
{
	uart_printf("[ADUPS] adups_bl_get_working_buffer =%x\r\n", FOTA_BUFFER_ADDR_START);
        return FOTA_BUFFER_ADDR_START;
}

adups_uint64 adups_bl_get_working_buffer_len(void)
{
	uart_printf("[ADUPS] adups_bl_get_working_buffer_len = 0x%x\r\n", (FOTA_BUFFER_ADDR_END-FOTA_BUFFER_ADDR_START));
        return (adups_uint64)(FOTA_BUFFER_ADDR_END-FOTA_BUFFER_ADDR_START);
}

adups_uint32 adups_bl_get_diff_param_size(void)
{
	return 64*1024;
}

adups_uint32 adups_bl_get_curr_write_address(void)
{
	return 0;
}


adups_uint32 adups_bl_get_app_base(void)
{
	return PackageInfo.multi_bin_address;
}

adups_uint32 adups_bl_get_delta_base(void)
{
	return PackageInfo.multi_bin_offset;
}

void WacthDogRestart()
{
	pm813_userdata_set_for_automation_test(RESET_FOTA_FLAG);
	PMIC_SW_RESET();
}

void ClearFotaMagic(void){
	char FotaMagic[8] = {0};

	adups_bl_debug_print(LOG_DEBUG, "ClearFotaMagic\r\n");
	//adups_bl_write_block(FotaMagic, get_fota_param_start_addr(), sizeof(FotaMagic));
	clear_fota_magic();

	return;
}

#ifdef LCD_USE_IN_CODE
extern void OTA_Clear_LCD(void);
#endif
void adups_patch_ratio(adups_uint16 ratio)
{
	//xiaoke TODO: may update LCD progress bar here
	adups_bl_debug_print(LOG_DEBUG, "+FOTA: patch ratio: %d\r",ratio);

	//+++++++++++++++++++++++++++++++++++++++++++
	//Add LCD Info here
	#ifdef LCD_USE_IN_CODE
		//LCD show update progress
		static adups_uint16 last_current = 500;	//init a "special value" to make sure only display when ration is 0.
		static adups_uint8 clear_lcd = 0;
		//[1] Before show progress, clear the LCD info. Note: only do it once
		if(clear_lcd == 0){
			//clear lcd
			OTA_Clear_LCD();
			clear_lcd = 1;
		}
		//[2] show progress bar
		if(ratio != last_current){
			OTA_LCD_ShowProcess(ratio);
			last_current = ratio;
		}
	#endif
	//+++++++++++++++++++++++++++++++++++++++++++
}

extern void OTA_LCD_DisplayText(const char *p_addtional_text,const OTA_LCD_DISPLAY_INFO_TYPE ota_info_type,const OTA_LANGUAGE_T ota_language);
extern MCI_ASR_LCD_INFO lcd_info_test;
extern void updater_delay_ms(UINT32 ms);
extern unsigned long GetTimer0CNT(void);
#define ADUPS_DELAY_TO_SEE_DISPLAY_INFO (1000)

void AUDPSProcedure(void)
{
	adups_int16 status = ADUPS_FUDIFFNET_ERROR_NONE;
	adups_uint8 i;
	adups_bl_debug_print(LOG_DEBUG, "AUDPSProcedure  4.0\r\n");
	//adups_uint8 *tt=pvPortMalloc(100);

	

	if(!adups_IsPackgeFound())
	{
		status=ADUPS_FUDIFFNET_ERROR_UPDATE_ERROR_END;
		#ifdef ADUPS_FOTA_ENABLE_DIFF_ERROR
		adups_bl_debug_print(LOG_DEBUG, "adups Package: not found\r\n");
		OTA_LCD_DisplayText(OTA_ERR_TEXT1,OTA_DISPLAY_INFO_TYPE_UPGRADE_FAIL,g_langulage_type);
		/* delay 2s to make sure user and tester to see the text in lcd screen */
		updater_delay_ms(ADUPS_DELAY_TO_SEE_DISPLAY_INFO + 1000);
		
		//do pmic reset here
		ClearFotaMagic();
		pm813_userdata_set_for_automation_test(RESET_FOTA_FLAG);
		PMIC_SW_RESET();
		#endif
		return;
	}	

	adups_get_package_number(&adups_delta_num);
	#ifdef ADUPS_FOTA_ENABLE_DIFF_DEBUG
	adups_bl_debug_print(LOG_DEBUG, "package_number=%d\r\n",adups_delta_num);
	#endif



   /*Init memory buffer*/   
   
   if (NULL == adups_bl_get_working_buffer()) 
  {
  	#ifdef ADUPS_FOTA_ENABLE_DIFF_ERROR
      // adups_bl_debug_print(LOG_DEBUG, "Can not alloc UPI working buffer\n");
	#endif
       status=ADUPS_FUDIFFNET_ERROR_INSUFFICIENT_WORKING_BUF;
   }


   if(status == ADUPS_FUDIFFNET_ERROR_NONE)
   {
	

	while(adups_curr_delta <= adups_delta_num)
	{
		adups_init_mem_pool(adups_bl_get_working_buffer(), adups_bl_get_working_buffer_len());
		memset(&PackageInfo,0,sizeof(multi_head_info_patch));
		#ifdef ADUPS_FOTA_ENABLE_DIFF_DEBUG
		adups_bl_debug_print(LOG_DEBUG, "adups_curr_delta=%d\n\r",adups_curr_delta);
		#endif
		status=adups_get_package_info(&PackageInfo,adups_curr_delta);
		if(status!=ADUPS_FUDIFFNET_ERROR_NONE)
			break;
		#ifdef ADUPS_FOTA_ENABLE_DIFF_DEBUG
		adups_bl_debug_print(LOG_DEBUG, "PackageInfo add=%x,m=%d,offset=%d\n\r",PackageInfo.multi_bin_address,PackageInfo.multi_bin_method,PackageInfo.multi_bin_offset);   
		#endif


		 status = fotapatch_procedure();

		
		if(status != ADUPS_FUDIFFNET_ERROR_NONE)
		{
			#ifdef ADUPS_FOTA_ENABLE_DIFF_DEBUG
			adups_bl_debug_print(LOG_DEBUG, "patch fail\r\n");


			#endif
			break;
		}
		else
		{
			#ifdef ADUPS_FOTA_ENABLE_DIFF_DEBUG
			adups_bl_debug_print(LOG_DEBUG, "patch sucess\r\n"); 

			#endif
			adups_curr_delta++;
		}
		frame_memory_finalize();
	}

      	

   }


	if(status == ADUPS_FUDIFFNET_ERROR_NONE || status == 8)
	{
		adups_bl_erase_delata();
		adups_bl_erase_backup_region();
		#if 1//def ADUPS_FOTA_ENABLE_DIFF_DEBUG
		adups_bl_debug_print(NULL, "Update done, reboot now ...\n\r");
		/* delay to display OTA SUCESS info*/
		//+++++++++++++++++++++++++++++++++++
		OTA_LCD_ShowProcess(100);
		
		/* delay 2s to make sure user and tester to see the 100% bar in lcd screen */
		updater_delay_ms(ADUPS_DELAY_TO_SEE_DISPLAY_INFO+ 1000);						
		//+++++++++++++++++++++++++++++++++++
		//do pmic reset here
		ClearFotaMagic();
#ifdef LCD_USE_IN_CODE
		mci_LcdSetBrightness(0);	
#endif
		#endif
	}
	else if(status > ADUPS_FUDIFFNET_ERROR_NONE)
	{
		adups_bl_debug_print(NULL, "Update failed, reboot now ...\n\r");
		OTA_LCD_DisplayText(OTA_ERR_TEXT2,OTA_DISPLAY_INFO_TYPE_UPGRADE_FAIL,g_langulage_type);
		/* delay 2s to make sure user and tester to see the text in lcd screen */
		updater_delay_ms(ADUPS_DELAY_TO_SEE_DISPLAY_INFO + 1000);
		adups_bl_erase_backup_region();	
	    ClearFotaMagic();
	}
	else
	{

		#if 1//def ADUPS_FOTA_ENABLE_DIFF_DEBUG
		adups_bl_debug_print(NULL, "***Something wrong during update, status=%d\n\r", status);
		#endif


		

	}

		pm813_userdata_set_for_automation_test(RESET_FOTA_FLAG);
		PMIC_SW_RESET();	
}


