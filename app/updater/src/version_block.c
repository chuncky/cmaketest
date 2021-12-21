#include "version_block.h"

//for basic updater build version
#define VB_VERSION_DATE        "20201002"

#define INFO_STRING			   VB_VERSION_DATE##"_ver1"

//for LCD TYPE INFO
//Note: The following will only choose one, Default is "NO_LCD"
#ifdef LCD_ADAPT
#define VB_OEM_LCD_TYPE        "LCD_ADAPT"
#endif

#ifdef WATCHLCDST7789_CODE_USE
#define VB_OEM_LCD_TYPE        "WATCHLCDST7789"
#endif

#ifdef LCD_ST7789VER1
#define VB_OEM_LCD_TYPE        "LCD_ST7789VER1"
#endif


#ifdef LCD_GC9306VER1
#define VB_OEM_LCD_TYPE        "LCD_GC9306VER1"
#endif

#ifdef LCD_GC9306VER2
#define VB_OEM_LCD_TYPE        "LCD_GC9306VER2"
#endif

#ifdef LCD_ST7789VER2
#define VB_OEM_LCD_TYPE        "LCD_ST7789VER2"
#endif


#ifndef LCD_USE_IN_CODE			//default
#define VB_OEM_LCD_TYPE "NO_LCD"
#endif


//for FOTA SOLUTION
#ifdef FOTA_ADUPS
#define VB_FOTA_SOLUTION       "FOTA_ADUPS"
#endif

#if 0
#ifdef FOTA_REDSTONE
#define VB_FOTA_SOLUTION       "FOTA_REDSTONE"
#endif
#endif

#ifdef FOTA_RED_ALI
#define VB_FOTA_SOLUTION       "FOTA_RED_ALI"
#endif
#ifdef FOTA_ASRSD
#define VB_FOTA_SOLUTION       "FOTA_ASRSD"
#endif

#ifdef FOTA_ASRSPI
#define VB_FOTA_SOLUTION       "FOTA_ASRSPI"
#endif

#ifdef FOTA_ASRDM
#define VB_FOTA_SOLUTION       "FOTA_ASRDM"
#endif

#pragma arm section rodata="IMGVERBLOCK"
const UpdaterVerBlockType updater_vb =
{
	//+++++++++++++++++++++++++++++
	//AREA_1: VERSION_INFO
	{
		VB_VERSION_DATE,
		VB_OEM_LCD_TYPE,
		VB_FOTA_SOLUTION,
		INFO_STRING
	}
};
#pragma arm section code
