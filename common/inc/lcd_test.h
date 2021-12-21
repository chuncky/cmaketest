#ifndef _LCD_TEST_H_
#define _LCD_TEST_H_

#include "common.h"
typedef unsigned short      	UINT16;
typedef unsigned char       	UINT8;
typedef unsigned int   			UINT32;

// [ simware ] display a picture after ota
#define OTA_LCD_SUCESS_LOGO_WIDTH 38
#define OTA_LCD_SUCESS_LOGO_HEIGHT 57

#define ENGLISH_WIDTH (8)
#define ENGLISH_HEIGHT (16)

/**
 * Note: If battery value is not more than "BATTERY_LOW_THRESHOLD", not display logo.
 * Note: When battery value is less than 2900 mv, software can not run.
 */
#define BATTERY_LOW_THRESHOLD (3000)
#define BATTERY_HIGH_VALUE	  (3400)

/**
 * BackLight Brightness Level
 * Range [ 0 - 5]: 	0  --- off
 */
#define Normal_BackLight_Brightness_Level		(3)
#define LowerBattery_BackLight_Brightness_Level	(1)


#define CHINESE_WIDTH_16       (16)
#define CHINESE_HEIGHT_16      (16)

#define CHINESE_WIDTH_24       (24)
#define CHINESE_HEIGHT_24      (24)



/*
**************************************************************************************
*1. Description
*   (1) Customer can modify this MACRO according to their needs.
*   (2) MACRO description:
*       *CHINESE_WIDTH
*       *CHINESE_HEIGHT: default is 24X24
**************************************************************************************
*/
//Chinese font array description
#if ( defined LCD_RESOLUTION_240_320 ) || ( defined LCD_RESOLUTION_320_240 )
#define CHINESE_WIDTH       (CHINESE_WIDTH_24)
#define CHINESE_HEIGHT      (CHINESE_HEIGHT_24)
#elif ( defined LCD_RESOLUTION_128_160 ) || ( defined LCD_RESOLUTION_176_220 ) || ( defined LCD_RESOLUTION_128_64 ) || \
	  ( defined LCD_RESOLUTION_240_240 ) || ( defined LCD_RESOLUTION_128_128 )
#define CHINESE_WIDTH       (CHINESE_WIDTH_16)
#define CHINESE_HEIGHT      (CHINESE_HEIGHT_16)
#else
#define CHINESE_WIDTH       (CHINESE_WIDTH_24)
#define CHINESE_HEIGHT      (CHINESE_HEIGHT_24)
#endif


//-----------------------------------

 
 


#ifdef LCD_GLOBALS
#define LCD_EXT
#else
#define LCD_EXT extern
#endif

#define OTA_TEXT_MAX_NUM (4)

/* need sync with ota download process */
typedef enum{
	CHINESE,	/* 0 */
	ENGLISH		/* 1 */
}OTA_LANGUAGE_T;


LCD_EXT volatile OTA_LANGUAGE_T g_language_type;



typedef enum{
	OTA_DISPLAY_INFO_TYPE_TIPS,
	OTA_DISPLAY_INFO_TYPE_UPGRADE_PRE,
	OTA_DISPLAY_INFO_TYPE_UPDATING,
	OTA_DISPLAY_INFO_TYPE_UPGRADE_SUCCESS,
	OTA_DISPLAY_INFO_TYPE_UPGRADE_FAIL,
	OTA_DISPLAY_INFO_TYPE_ADDITION_INFO,
	OTA_DISPLAY_INFO_TYPE_BAR_TEXT_AND_NUM,
	OTA_DISPLAY_INFO_TYPE_BAR_TEXT,
	OTA_DISPLAY_INFO_TYPE_BAR_NUM,
}OTA_LCD_DISPLAY_INFO_TYPE;

/*
*********************************************************
1. ota english display info height
-------------------------------------------------------------
| ota status tips info1        | 0 - ENGLISH_HEIGHT * 4  |
-------------------------------------------------------------
| ota status tips info2        | 0 - ENGLISH_HEIGHT * 3  |
-------------------------------------------------------------
| ota status info              | 0 - ENGLISH_HEIGHT * 2   |
--------------------------------------------------------------
| ota status additional info       | 0 -   ENGLISH_HEIGHT   |
--------------------------------------------------------------
| ota progress bar text        | 0                      |  ---> LCD_HEIGHT / 2
-------------------------------------------------------------
| ota progress bar             | 20   |
-------------------------------------------------------------
*********************************************************
*/

/* 1.1  ota tips text */
#define OTA_TIPS_TEXT1 					("CAUTION:")
#define OTA_TIPS_TEXT1_HEIGHT_OFFSET 	(OTA_TIPS_TEXT2_HEIGHT_OFFSET - ENGLISH_HEIGHT)

#define OTA_TIPS_TEXT2 				("KEEP CHARGER!")
#define OTA_TIPS_TEXT2_HEIGHT_OFFSET 	(OTA_STATUS_HEIGHT_OFFSET - ENGLISH_HEIGHT)


/* 1.2 ota status text */
#define      OTA_STATUS_PRE_TEXT1 		("prepare to update")
#define      OTA_STATUS_UPDATING_TEXT2  ("updating...")
#define      OTA_STATUS_RES_TEXT3 		("update success!")
#define      OTA_STATUS_RES_TEXT4 		("update fail !")

#define OTA_STATUS_HEIGHT_OFFSET 	(OTA_STATUS_ADDITIONAL_HEIGHT_OFFSET - ENGLISH_HEIGHT)

/* 1.3 ota status additional info */
// 1.3.1 adups ota fail text
#define OTA_ERR_TEXT1 ("Package not found!")
#define OTA_ERR_TEXT2 ("patch fail,reboot!")

#define OTA_STATUS_ADDITIONAL_HEIGHT_OFFSET    (OTA_PROGRESS_BAR_TEXT_AND_NUM_HEIGHT_OFFSET - ENGLISH_HEIGHT)

/* 1.4 ota progress bar parameters */
	//1.4.1 progress bar text location
	//note: macro:OTA_PROGRESS_BAR_ENGLISH_TEXT will use in sprintf, so add brackets will make mistakes
#define OTA_PROGRESS_BAR_ENGLISH_TEXT 		"Writing:"	
#define OTA_PROGRESS_BAR_TEXT_AND_NUM_HEIGHT_OFFSET 			(0)	



	//1.4.3 progress bar location
#define BAR_HEIGHT_INTERVAL_DISTANCE (8)	
#define OTA_PROGRESS_BAR_HEIGHT_OFFSET (OTA_PROGRESS_BAR_TEXT_AND_NUM_HEIGHT_OFFSET + CHINESE_HEIGHT + BAR_HEIGHT_INTERVAL_DISTANCE)

	//1.4.4 progress bar width
#define OTA_PROGRESS_BAR_WIDTH ( CHINESE_HEIGHT + CHINESE_HEIGHT)

//=====================================================================
/*
*********************************************************
2. ota chinese display info height
--------------------------------------------------------------------------------------
| ota status tips info                       | 0 - ENGLISH_HEIGHT - CHINESE_HEIGHT   * 2 - CHINESE_INTERVAL 	 	
------------------------------------------------------
| ota status info                            | 0 - ENGLISH_HEIGHT - CHINESE_HEIGHT - CHINESE_INTERVAL  	   
--------------------------------------------------------------------------------------
| ota status additional info (english info)        | 0 - ENGLISH_HEIGHT  					 						
--------------------------------------------------------------------------------------
| ota progress bar text                      | 0    								---> LCD_HEIGHT / 2
--------------------------------------------------------------------------------------
| ota progress bar                           | 0 +  CHINESE_HEIGHT +  BAR_HEIGHT_INTERVAL_DISTANCE		 	 
--------------------------------------------------------------------------------------

*********************************************************
*/
#define CHINESE_INTERVAL (5)

/* 2.1  ota tips text */
#define OTA_TIPS_CHINESE_TEXT1 					("请勿断电")
#define OTA_TIPS_CHINESE_TEXT1_NUM			(4)
#define OTA_TIPS_CHINESE_TEXT1_HEIGHT_OFFSET 	(OTA_STATUS_CHINESE_HEIGHT_OFFSET - CHINESE_HEIGHT - CHINESE_INTERVAL)

/* 2.2 ota status text */
#define      OTA_STATUS_CHINESE_PRE_TEXT1 					("升级前，准备中")
#define 	 OTA_STATUS_CHINESE_PRE_TEXT1_NUM			(7)

#define      OTA_STATUS_CHINESE_UPGRADING_TEXT2  			("正在升级中")
#define      OTA_STATUS_CHINESE_UPGRADING_TEXT2_NUM		(5)

#define      OTA_STATUS_CHINESE_RES_TEXT3 					("升级成功")
#define      OTA_STATUS_CHINESE_RES_TEXT3_NUM			(4)		

#define      OTA_STATUS_CHINESE_RES_TEXT4 					("升级失败")
#define      OTA_STATUS_CHINESE_RES_TEXT4_NUM			(4)

#define      OTA_STATUS_CHINESE_HEIGHT_OFFSET 			    (OTA_STATUS_ADDITIONAL_HEIGHT_OFFSET - CHINESE_HEIGHT - CHINESE_INTERVAL)

/* 2.3 ota status additional info (if necessary) */
//2.3.1 adups ota fail text	
#define OTA_ERR_CHINESE_TEXT1 (OTA_ERR_TEXT1)
#define OTA_ERR_CHINESE_TEXT2 (OTA_ERR_TEXT2)

#define OTA_STATUS_ADDITIONAL_HEIGHT_OFFSET    (OTA_PROGRESS_BAR_CHINESE_TEXT_HEIGHT_OFFSET - ENGLISH_HEIGHT)

/* 2.4 ota progress bar parameters */
	//2.4.1 progress bar text location
#define OTA_PROGRESS_BAR_CHINESE_TEXT 			"更新:"
#define OTA_PROGRESS_BAR_CHINESE_TEXT_NUM	(3)
#define OTA_PROGRESS_BAR_CHINESE_TEXT_HEIGHT_OFFSET 		(0)	

	//2.4.2 progress bar num
		//''num'+'%'
#define LENGTH_CHINESE_UPGRADE_BAR_NUM1		(1+1)
#define LENGTH_CHINESE_UPGRADE_BAR_NUM2		(2+1)
#define LENGTH_CHINESE_UPGRADE_BAR_NUM3		(3+1)

		//'%' location
#define PER_CENT_OFFSET						(10)
#define OTA_PROGRESS_BAR_CHINESE_NUM_HEIGHT_OFFSET         (0)

#define LENGTH_CHINESE_UPGRADE_BAR_NUM		(11)


/* info color  */
/* some RGB565 color definitions */
/*
#define   BLACK     0x0000
#define   NAVY		0x000F
#define   DGREEN	0x03E0
#define   DCYAN     0x03EF
#define   DGREEN	0x7800
#define   PURPLE    0x780F
#define   OLIVE     0x7BE0
#define   LGRAY     0xC618
#define   DGRAY     0x7BEF
#define   BLUE      0x001F
#define   GREEN     0x07E0
#define   CYAN      0x07FF
#define   RED       0xF800
#define   MAGENTA   0xF81F
#define   YELLOW    0xFFE0
#define   WHITE     0xFFFF
*/

#define OTA_TIPS_COLOR							0xF800
#define OTA_OTHER_TEXT_INFO_COLOR				0x0000
#define OTA_PROGRESS_BAR_COLOR 					0x07E0

/* define calculate text location macro function */
//height
#define OTA_HEIGHT_CAL(screen_h,offset_h) (((screen_h) / 2) + (offset_h))

//width
/* note: text total pixel width = text_length * pixel_width */
/*
--------------------------------------------------------------
                        M
					|	|     |
                      text1			---        OTA_WIDTH_MIDDLE_CAL
                    text2           ---  OTA_WIDTH_MIDDLE_LEFT_CAL
                         text3      --- OTA_WIDTH_MIDDLE_RIGHT_CAL
---------------------------------------------------------------
*/

#define OTA_WIDTH_MIDDLE_CAL(screen_w,text_length) (((screen_w) - (text_length) * ENGLISH_WIDTH) / 2) 
#define OTA_LEFT_ALIGN (0)

#define OTA_CHINESE_WIDTH_MIDDLE_CAL(screen_w,text_length,width) (((screen_w) - (text_length) * (width)) / 2) 
#define OTA_CHINESE_WIDTH_MIDDLE_LEFT_CAL(screen_w,text_length,width) (((screen_w) / 2 - (text_length) * (width)))
#define OTA_CHINESE_WIDTH_MIDDLE_RIGHT_CAL(screen_w) ((screen_w) / 2)





typedef struct
{
	int hundreds;
	int tens;
	int units;
}Upgrade_Bar_Num_t;

typedef struct ota_lcd_display_info{	
	OTA_LANGUAGE_T 				ota_info_language_type;	/* text language type. ( chinese or english? ) */
	OTA_LCD_DISPLAY_INFO_TYPE   ota_info_content_type;	/* Reference struct --- "OTA_LCD_DISPLAY_INFO_TYPE" */
	UINT8          				text_num;				/* one text corresponding to one line */
	UINT8						TextWdith;				/* text width ( 24 or 32 ...) */
	UINT8						TextHeight;				
	UINT16 						TextColor;				/* RGB565 color */
	UINT16 						LcdWidth;
	UINT16						LcdHeight;
	UINT16                      *p_FrameBuffer;			/* point to framebuffer */
	char          				*p_text_buf[OTA_TEXT_MAX_NUM];
	UINT16		   				text_height_offset[OTA_TEXT_MAX_NUM];
}OTA_LCD_DISPLAY_INFO_T;




#ifdef SMALL_CODE_UPDATER
extern void init_lcd_vars(void);
extern OTA_LANGUAGE_T Get_LanguageType( void );

extern void OTA_LCD_ShowProcess(int per);

/**
 *@brief:  display info in lcd
 *@param   p_addtional_text: display addition info(only english info) in lcd(it has special region in lcd.  
 *		   ota_info_type   : user can input following:  
 *								@OTA_DISPLAY_INFO_TYPE_TIPS,
 *								@OTA_DISPLAY_INFO_TYPE_UPGRADE_PRE,
 *								@OTA_DISPLAY_INFO_TYPE_UPDATING,
 *								@OTA_DISPLAY_INFO_TYPE_UPGRADE_SUCCESS,
 *								@OTA_DISPLAY_INFO_TYPE_UPGRADE_FAIL, 		   
 *         ota_language	   : get val from flash
 *                              reference global var --- g_language_type 
 *@retval NULL
 */
extern void OTA_LCD_DisplayText(const char *p_addtional_text,const OTA_LCD_DISPLAY_INFO_TYPE ota_info_type,const OTA_LANGUAGE_T ota_language);
extern void OTA_LCD_ShowImage( void );
extern void OTA_Clear_LCD(void);
#endif

#ifdef SMALL_CODE_LOGO
extern void POWER_ON_LCD_ShowImage( void );
#endif


extern void lcd_get_panel_info( void );



#if ( defined LCD_USE_IN_CODE )	&& (defined SMALL_CODE_UPDATER) && ( defined TEST_OTA_DISPLAY_INFO )
#define DELAY_TIME2 1000 * 5
extern void test_logo_LcdShowProcess_Demo( void );
extern void test_OTA_LCD_DisplayText( void );
#endif


#endif
