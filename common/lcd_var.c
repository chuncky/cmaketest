#if defined SMALL_CODE_UPDATER
#define LCD_GLOBALS


/***************************************************
*
*create a new file:     ---	lcd_var.c, in order to define global variables and declare these global variables in lcd_test.h
*file1: lcd_var.c   ---  	define global variables
*file2: lcd_test.h  ---		declare these global variables
*
***************************************************/

#include "lcd_test.h"
#include "types.h"

extern get_fota_param_start_addr();

void init_lcd_vars(void)
{
	UINT32 language_type;
	const UINT32 ota_language_type_offset = 16;
	UINT32 language_type_start_address_in_flash;

/* Case1: FOTA_ASRSD */
#if defined ( FOTA_ASRSD )
	g_language_type = CHINESE;
#endif

/* Case2: FOTA_ADUPS or FOTA_RED_ALI */
#if defined ( FOTA_ADUPS ) || defined ( FOTA_RED_ALI )
	// get language type from fota_param partition.
	CP_LOGI("fota_param addr:0x%x\r\n",get_fota_param_start_addr());
	language_type_start_address_in_flash = get_fota_param_start_addr() + ota_language_type_offset;
	asr_norflash_read(language_type_start_address_in_flash, (uint8_t *)&language_type, sizeof(language_type));

	g_language_type = (OTA_LANGUAGE_T)language_type;
#endif

// for debug
#if defined ( TEST_OTA_DISPLAY_INFO )
	#if 0
	g_language_type = ENGLISH;
	#else
	g_language_type = CHINESE;
	#endif
#endif

	CP_LOGI("[OTA] language type:%s\r\n",language_type == CHINESE ? "CHINESE" : "ENGLISH");
}

OTA_LANGUAGE_T Get_LanguageType( void )
{
	return g_language_type;
}

#endif /* #if defined SMALL_CODE_UPDATER */
