/*
**************************************************************************************
*       * DISPLAY_CHINESE_INFO: control whether chineses are displayed during FOTA.
*                                   if not defined, not display chineses
**************************************************************************************
*/
#define DISPLAY_CHINESE_INFO

#include "lcdd_logo_asr.h"
#include "lcd_test.h"
#include "lcd_test_data.h"






typedef enum 
{
	BG_RED,
	BG_BLCAK,
	BG_GREEN,
	BG_BLUE,
	BG_WHITE,
	BG_END
} LCD_BGCOLOR_INDEX;


/**
  *g_lcd_framebuffer  --- lcd framebuffer
  *240                --- lcd_width
  *320                --- lcd_height
  *16                 --- bits per pixel 
  *8                  --- bits per byte 
 */
static unsigned char g_lcd_framebuffer[240*320*16/8];

static struct asrlcdd_screen_info lcd_info_test;

static Upgrade_Bar_Num_t s_UpgradeBarNum;

static __inline void uudelay(int us);
static __inline void mdelay(int ms);
static void ParseNum(const INT32 NUM,Upgrade_Bar_Num_t *p_upgrade_bar_num);
static void s_ota_LcdShowProcess(int per);

static void lcd_fill_english_into_framebuffer(const OTA_LCD_DISPLAY_INFO_T          	 *p_lcd_display_info);

static void fill_lcd_chinese_info_into_framebuffer(OTA_LCD_DISPLAY_INFO_T *p_ota_display_info,
									 const OTA_LCD_DISPLAY_INFO_TYPE  lcd_display_info_type);

static void fill_lcd_english_info_into_framebuffer( OTA_LCD_DISPLAY_INFO_T             	*p_lcd_display_info,
											          const OTA_LCD_DISPLAY_INFO_TYPE  lcd_display_info_type);

static char* s_get_lcd_FrameBufferPointer(void)
{
	return (char*)g_lcd_framebuffer;
}

static __inline void uudelay(int us)
{
	volatile uint32_t i;
	for(i=0; i<us*100;i++)
		i = i+1;
}

static __inline void mdelay(int ms)
{
	volatile uint32_t i;
	for(i=0; i<ms;i++)
		uudelay(10);
}





/*****************************************************************************/
// Description:  Set 8 pixels from(x,y) to(x+7,y).
// Note:
/*****************************************************************************/
static void test_Logo_LCD_Set8Pixels (uint16_t cx, uint16_t cy, uint8_t bit_mask,
	uint16_t lcd_width,uint16_t * framebuffer,const uint16_t COLOR)
{
    uint16_t *buffer_ptr = 0;
    uint16_t i;

    buffer_ptr = framebuffer;
    for (i=0; i<8; i++)
        if ( (bit_mask & (0x01<<i)) !=0x00)
            *(buffer_ptr+cy*lcd_width+cx+i) = COLOR;
}


static void SetChinesePixels(uint16_t cx, uint16_t cy, uint8_t *bit_mask,
	uint16_t lcd_width,uint16_t * framebuffer,const uint16_t COLOR)
{
    uint16_t *buffer_ptr = 0;
    uint16_t i,j;
	uint16_t one_chinese_line_bytes = CHINESE_WIDTH/8;
	
	//static uint8_t index = 0;
	//uart_printf("[logo] bit_mask_%d:0x%x\r\n",index++,*bit_mask);
	
    buffer_ptr = framebuffer;
    for (i=0; i<one_chinese_line_bytes; i++){	/* one line Bytes */
		for(j=0;j<8;j++)	/* 8 bits  */
        	if ( (*(bit_mask+i) & (0x80u>>j)) !=0x00)
            	*(buffer_ptr+cy*lcd_width+cx+i*8+j) = COLOR;		
    }
}

#if (defined LCD_USE_IN_CODE) && (defined SMALL_CODE_UPDATER) && (defined DISPLAY_CHINESE_INFO)
static void SetPixels_DisplayChinese(OTA_LCD_DISPLAY_INFO_T *p_ota_display_info)
{
	volatile uint8_t text_num;
    uint16_t  i,j;
	uint16_t OneChineseLineBytes;
	uint16_t OneChineseBytes;
	uint16_t TextLoactionX;
	uint16_t TextLoactionY;
	int32_t hundreds,tens,units;	
	//INIT
	OneChineseLineBytes = p_ota_display_info->TextWdith/8;
	OneChineseBytes     = p_ota_display_info->TextWdith*p_ota_display_info->TextHeight/8;
	text_num            = p_ota_display_info->text_num;

	if(p_ota_display_info->ota_info_content_type == OTA_DISPLAY_INFO_TYPE_BAR_TEXT){
		TextLoactionX = OTA_CHINESE_WIDTH_MIDDLE_LEFT_CAL(p_ota_display_info->LcdWidth,text_num,p_ota_display_info->TextWdith);
	}else if(p_ota_display_info->ota_info_content_type == OTA_DISPLAY_INFO_TYPE_BAR_NUM){
		TextLoactionX = OTA_CHINESE_WIDTH_MIDDLE_RIGHT_CAL(p_ota_display_info->LcdWidth);
	}else{
		TextLoactionX = OTA_CHINESE_WIDTH_MIDDLE_CAL(p_ota_display_info->LcdWidth,text_num,p_ota_display_info->TextWdith);
	}
	TextLoactionY = OTA_HEIGHT_CAL(p_ota_display_info->LcdHeight,p_ota_display_info->text_height_offset[0]);
	
	if(NULL == p_ota_display_info->p_FrameBuffer){
		CP_LOGE("[ERROR] func:%s,line:%d,p_ota_display_info->p_FrameBuffer ERR\r\n",__func__,__LINE__);
		while(1);
	}

	CP_LOGD("DBG: [Chinese]Enter %s,OTA_LCD_DISPLAY_INFO_TYPE:%d,text_num:%d,x:%d,y:%d\r\n",
		__func__,p_ota_display_info->ota_info_content_type,text_num,TextLoactionX,TextLoactionY);
	
	switch(p_ota_display_info->ota_info_content_type)
	{
		case OTA_DISPLAY_INFO_TYPE_TIPS:
		{	
			for(i=0;i<text_num;i++){  				/*num of chinese */
				for(j=0;j<p_ota_display_info->TextHeight;j++)	/*height of one chinese */
					SetChinesePixels(TextLoactionX + i*p_ota_display_info->TextWdith, TextLoactionY+j, 
						&dataptr_OTA_TIPS_CHINESE_TEXT1[i][0][j*OneChineseLineBytes], p_ota_display_info->LcdWidth,p_ota_display_info->p_FrameBuffer,p_ota_display_info->TextColor);
			}			
		}
		break;
		case OTA_DISPLAY_INFO_TYPE_UPGRADE_PRE:
		{	
			for(i=0;i<text_num;i++){  				/*num of chinese */
				for(j=0;j<p_ota_display_info->TextHeight;j++)	/*height of one chinese */
					SetChinesePixels(TextLoactionX + i*p_ota_display_info->TextWdith, TextLoactionY+j, 
						&dataptr_OTA_STATUS_CHINESE_PRE_TEXT1[i][0][j*OneChineseLineBytes], p_ota_display_info->LcdWidth,p_ota_display_info->p_FrameBuffer,p_ota_display_info->TextColor);
			}			
		}
		break;
		case OTA_DISPLAY_INFO_TYPE_UPDATING:
		{	
			for(i=0;i<text_num;i++){  				/*num of chinese */
				for(j=0;j<p_ota_display_info->TextHeight;j++)	/*height of one chinese */
					SetChinesePixels(TextLoactionX + i*p_ota_display_info->TextWdith, TextLoactionY+j, 
						&dataptr_OTA_STATUS_CHINESE_UPGRADING_TEXT2[i][0][j*OneChineseLineBytes], p_ota_display_info->LcdWidth,p_ota_display_info->p_FrameBuffer,p_ota_display_info->TextColor);
			}			
		}
		break;
		case OTA_DISPLAY_INFO_TYPE_UPGRADE_SUCCESS:
		{	
			for(i=0;i<text_num;i++){  				/*num of chinese */
				for(j=0;j<p_ota_display_info->TextHeight;j++)	/*height of one chinese */
					SetChinesePixels(TextLoactionX + i*p_ota_display_info->TextWdith, TextLoactionY+j, 
						&dataptr_OTA_STATUS_CHINESE_RES_TEXT3[i][0][j*OneChineseLineBytes], p_ota_display_info->LcdWidth,p_ota_display_info->p_FrameBuffer,p_ota_display_info->TextColor);
			}			
		}
		break;
		case OTA_DISPLAY_INFO_TYPE_UPGRADE_FAIL:
		{	
			for(i=0;i<text_num;i++){  				/*num of chinese */
				for(j=0;j<p_ota_display_info->TextHeight;j++)	/*height of one chinese */
					SetChinesePixels(TextLoactionX + i*p_ota_display_info->TextWdith, TextLoactionY+j, 
						&dataptr_OTA_STATUS_CHINESE_RES_TEXT4[i][0][j*OneChineseLineBytes], p_ota_display_info->LcdWidth,p_ota_display_info->p_FrameBuffer,p_ota_display_info->TextColor);
			}			
		}
		break;
		case OTA_DISPLAY_INFO_TYPE_BAR_TEXT:
		{	
			for(i=0;i<text_num;i++){  				/*num of chinese */
				for(j=0;j<p_ota_display_info->TextHeight;j++)	/*height of one chinese */
					SetChinesePixels(TextLoactionX + i*p_ota_display_info->TextWdith, TextLoactionY+j, 
						&dataptr_OTA_PROGRESS_BAR_CHINESE_TEXT[i][0][j*OneChineseLineBytes], p_ota_display_info->LcdWidth,p_ota_display_info->p_FrameBuffer,p_ota_display_info->TextColor);
			}			
		}
		break;

		case OTA_DISPLAY_INFO_TYPE_BAR_NUM:
		{	
			hundreds = s_UpgradeBarNum.hundreds;
			tens     = s_UpgradeBarNum.tens;
			units    = s_UpgradeBarNum.units;

			if(hundreds > 0){
				for(j=0;j<p_ota_display_info->TextHeight;j++)	/*height of one chinese */
					SetChinesePixels(TextLoactionX, TextLoactionY+j, 
						(&dataptr_progress_bar_num[hundreds][0][j*OneChineseLineBytes]), p_ota_display_info->LcdWidth,p_ota_display_info->p_FrameBuffer,p_ota_display_info->TextColor);
				TextLoactionX += p_ota_display_info->TextWdith;
			}
			if(tens > 0 || text_num == LENGTH_CHINESE_UPGRADE_BAR_NUM3){
				for(j=0;j<p_ota_display_info->TextHeight;j++)	/*height of one chinese */
					SetChinesePixels(TextLoactionX, TextLoactionY+j, 
						(&dataptr_progress_bar_num[tens][0][j*OneChineseLineBytes]), p_ota_display_info->LcdWidth,p_ota_display_info->p_FrameBuffer,p_ota_display_info->TextColor);
				TextLoactionX += p_ota_display_info->TextWdith;
			}
			for(j=0;j<p_ota_display_info->TextHeight;j++)	/*height of one chinese */
				SetChinesePixels(TextLoactionX, TextLoactionY+j, 
					(&dataptr_progress_bar_num[units][0][j*OneChineseLineBytes]), p_ota_display_info->LcdWidth,p_ota_display_info->p_FrameBuffer,p_ota_display_info->TextColor);
			TextLoactionX += p_ota_display_info->TextWdith;
			for(j=0;j<p_ota_display_info->TextHeight;j++)	/*height of one chinese */
				SetChinesePixels(TextLoactionX, TextLoactionY+j, 
					(&dataptr_progress_bar_num[PER_CENT_OFFSET][0][j*OneChineseLineBytes]), p_ota_display_info->LcdWidth,p_ota_display_info->p_FrameBuffer,p_ota_display_info->TextColor);			
		}	
		break;
		default:
			CP_LOGE("[chinese] display chinese info type error.\r\n");
			while(1);
	}			
}
#endif

void LcdDisplayCleanMemoryCache(void *pMem, unsigned int size)
{
	CacheCleanMemory(pMem, size);
}


static void ParseNum(const INT32 NUM,Upgrade_Bar_Num_t *p_upgrade_bar_num)
{
	p_upgrade_bar_num->hundreds = NUM / 100;
	p_upgrade_bar_num->tens	 = (NUM - p_upgrade_bar_num->hundreds * 100) / 10;
	p_upgrade_bar_num->units	 = NUM % 10;

	CP_LOGD("hundreds:%d,tens:%d,units:%d\r\n",p_upgrade_bar_num->hundreds,p_upgrade_bar_num->tens,p_upgrade_bar_num->units);
}


static void s_ota_LcdShowProcess(int per)
{
#if defined LCD_USE_IN_CODE	&& (defined SMALL_CODE_UPDATER)
	UINT16 progress_bar_start_h;
	UINT16 chinese_len;
	char   OTA_PROGRESS_BAR_ENGLISH_TEXT_buf[100];
	INT32 hundreds,tens,units;
	UINT32 i = 0;
	UINT32 j = 0;
	OTA_LANGUAGE_T language_type = Get_LanguageType();

	OTA_LCD_DISPLAY_INFO_T ota_display_info;

	/* [1] INIT */
	ota_display_info.LcdWidth = lcd_info_test.width;
	ota_display_info.LcdHeight = lcd_info_test.height;
	
	ota_display_info.p_FrameBuffer = (UINT16 *)s_get_lcd_FrameBufferPointer();
	memset((char*)(ota_display_info.p_FrameBuffer), 0xff, ota_display_info.LcdWidth*ota_display_info.LcdHeight*2);
	
	progress_bar_start_h = OTA_HEIGHT_CAL(ota_display_info.LcdHeight, OTA_PROGRESS_BAR_HEIGHT_OFFSET);
	CP_LOGD("DBG: ShowProcess start\r\n");
	CP_LOGD("DBG: s_ota_LcdShowProcess: panel weight %d, height %d \r\n",ota_display_info.LcdWidth, ota_display_info.LcdHeight);

	/* [2] prepare the "ota_tips" data */
	//set language type info
	ota_display_info.ota_info_language_type = language_type;
	
#if (defined DISPLAY_CHINESE_INFO)
	if(ota_display_info.ota_info_language_type == CHINESE){
		//set tips info
		fill_lcd_chinese_info_into_framebuffer(&ota_display_info,OTA_DISPLAY_INFO_TYPE_TIPS);

		//set status info
		if(100 == per){
			fill_lcd_chinese_info_into_framebuffer(&ota_display_info,OTA_DISPLAY_INFO_TYPE_UPGRADE_SUCCESS);
		}else{
			fill_lcd_chinese_info_into_framebuffer(&ota_display_info,OTA_DISPLAY_INFO_TYPE_UPDATING);
		}

		//set progress bar text
		fill_lcd_chinese_info_into_framebuffer(&ota_display_info,OTA_DISPLAY_INFO_TYPE_BAR_TEXT);

		// set progress bar num
		if(per > 100 || per < 0){
			CP_LOGE("[ERROR] per: 0x%x is not right\r\n",per);
			while(1);
		}
		ParseNum(per,&s_UpgradeBarNum);
		fill_lcd_chinese_info_into_framebuffer(&ota_display_info,OTA_DISPLAY_INFO_TYPE_BAR_NUM);
	}
#endif	
	if(ota_display_info.ota_info_language_type == ENGLISH){
		fill_lcd_english_info_into_framebuffer(&ota_display_info, OTA_DISPLAY_INFO_TYPE_TIPS);
		if(100 == per){			
			fill_lcd_english_info_into_framebuffer(&ota_display_info, OTA_DISPLAY_INFO_TYPE_UPGRADE_SUCCESS);
		}else{
			fill_lcd_english_info_into_framebuffer(&ota_display_info, OTA_DISPLAY_INFO_TYPE_UPDATING);
		}
		
		sprintf(OTA_PROGRESS_BAR_ENGLISH_TEXT_buf,OTA_PROGRESS_BAR_ENGLISH_TEXT" %d%%",per);
		ota_display_info.p_text_buf[0] = OTA_PROGRESS_BAR_ENGLISH_TEXT_buf;
		fill_lcd_english_info_into_framebuffer(&ota_display_info, OTA_DISPLAY_INFO_TYPE_BAR_TEXT_AND_NUM);
		CP_LOGD("OTA_PROGRESS_BAR_ENGLISH_TEXT length:%d\r\n",strlen(OTA_PROGRESS_BAR_ENGLISH_TEXT_buf));
	}
	
	//3.2 prepare the "progress bar" data 
	for(i = progress_bar_start_h; i < progress_bar_start_h + OTA_PROGRESS_BAR_WIDTH; i++){
		for(j = 0; j < ota_display_info.LcdWidth * per / 100; j++){
			*(ota_display_info.p_FrameBuffer + i*ota_display_info.LcdWidth + j) = OTA_PROGRESS_BAR_COLOR;
		}
	}
	LcdDisplayCleanMemoryCache((void*)ota_display_info.p_FrameBuffer,(ota_display_info.LcdWidth*ota_display_info.LcdHeight*2));
	/* [4] display the above data in lcd */	
	ASRLCDD_Logo_Blit16_sync(ota_display_info.p_FrameBuffer, ota_display_info.LcdWidth, ota_display_info.LcdHeight,
		0, 0, ota_display_info.LcdWidth* 2);
    CP_LOGD("%s end\n",__func__);
#endif	
}


static void lcd_fill_english_into_framebuffer(const OTA_LCD_DISPLAY_INFO_T *p_lcd_display_info)
{
	char      ch;
	uint8_t  *data_ptr;
	uint16_t  i;
	uint16_t  j;
	uint16_t  offset;
	uint16_t OTA_x;
	uint16_t OTA_y;
	char *p_text = NULL;
	uint16_t text_engliash_num;
	
	/* get the panel parameters and memset framebuffer */
	const uint16_t LCD_WIDTH = p_lcd_display_info->LcdWidth;
	const uint16_t LCD_HEIGHT = p_lcd_display_info->LcdHeight;
	
	for(i = 0; i < p_lcd_display_info->text_num; i++){
		//INIT
		p_text = p_lcd_display_info->p_text_buf[i];
		if(p_text == NULL){
			CP_LOGE("[ERROR] p_text error\r\n");
			while(1);
		}
		text_engliash_num = strlen(p_lcd_display_info->p_text_buf[i]);

		//center or left align ?
		if(text_engliash_num <= LCD_WIDTH / ENGLISH_WIDTH)
			OTA_x = OTA_WIDTH_MIDDLE_CAL(LCD_WIDTH, text_engliash_num);
		else
			OTA_x = OTA_LEFT_ALIGN;
		OTA_y = OTA_HEIGHT_CAL(LCD_HEIGHT,p_lcd_display_info->text_height_offset[i]);
		
		for(;text_engliash_num != 0;text_engliash_num--){
	        ch = *p_text++;
			
	        if ( (ch <= 0x20) || (ch & 0x80))
	            offset = 0;  // non-printable character, use the default char(' ')
	        else
	            offset = ( (uint16_t) ( (uint8_t) ch - 0x20)) * 16;

	        data_ptr = (uint8_t *)&__ASS_LCD_ASCII_8X16[offset];

	        for (j = 0; j < 16; j++){
	            test_Logo_LCD_Set8Pixels(OTA_x, OTA_y+j, *data_ptr++, LCD_WIDTH,p_lcd_display_info->p_FrameBuffer,p_lcd_display_info->TextColor);
	        }
	        OTA_x+=8;
	        if ( (OTA_x + 8) > LCD_WIDTH) // Next line.
	        {
	            OTA_y += 16;
	            OTA_x  = 0;
	        }

	        if (OTA_y > (LCD_HEIGHT-16))
	            break;
	    }
	}
}

static void fill_lcd_chinese_info_into_framebuffer(OTA_LCD_DISPLAY_INFO_T *p_ota_display_info,
									 const OTA_LCD_DISPLAY_INFO_TYPE  lcd_display_info_type)
{
	p_ota_display_info->TextHeight  = CHINESE_HEIGHT;
	p_ota_display_info->TextWdith	= CHINESE_WIDTH;
	
	p_ota_display_info->ota_info_content_type = lcd_display_info_type;

	switch(p_ota_display_info->ota_info_content_type)
	{
		case OTA_DISPLAY_INFO_TYPE_TIPS:
			p_ota_display_info->text_num = OTA_TIPS_CHINESE_TEXT1_NUM;
			p_ota_display_info->text_height_offset[0] = OTA_TIPS_CHINESE_TEXT1_HEIGHT_OFFSET;
			p_ota_display_info->TextColor = OTA_TIPS_COLOR;
		break;

		case OTA_DISPLAY_INFO_TYPE_UPGRADE_PRE:
			p_ota_display_info->text_num = OTA_STATUS_CHINESE_PRE_TEXT1_NUM;
			p_ota_display_info->text_height_offset[0] = OTA_STATUS_CHINESE_HEIGHT_OFFSET;
			p_ota_display_info->TextColor = OTA_OTHER_TEXT_INFO_COLOR;
		break;

		case OTA_DISPLAY_INFO_TYPE_UPDATING:
			p_ota_display_info->text_num = OTA_STATUS_CHINESE_UPGRADING_TEXT2_NUM;
			p_ota_display_info->text_height_offset[0] = OTA_STATUS_CHINESE_HEIGHT_OFFSET;
			p_ota_display_info->TextColor = OTA_OTHER_TEXT_INFO_COLOR;
		break;

		case OTA_DISPLAY_INFO_TYPE_UPGRADE_SUCCESS:
			p_ota_display_info->text_num = OTA_STATUS_CHINESE_RES_TEXT3_NUM;
			p_ota_display_info->text_height_offset[0] = OTA_STATUS_CHINESE_HEIGHT_OFFSET;
			p_ota_display_info->TextColor = OTA_OTHER_TEXT_INFO_COLOR;
		break;

		case OTA_DISPLAY_INFO_TYPE_UPGRADE_FAIL:
			p_ota_display_info->text_num = OTA_STATUS_CHINESE_RES_TEXT4_NUM;
			p_ota_display_info->text_height_offset[0] = OTA_STATUS_CHINESE_HEIGHT_OFFSET;
			p_ota_display_info->TextColor = OTA_OTHER_TEXT_INFO_COLOR;
		break;

		case OTA_DISPLAY_INFO_TYPE_BAR_TEXT:
			p_ota_display_info->text_num = OTA_PROGRESS_BAR_CHINESE_TEXT_NUM;
			p_ota_display_info->text_height_offset[0] = OTA_PROGRESS_BAR_CHINESE_TEXT_HEIGHT_OFFSET;
			p_ota_display_info->TextColor = OTA_OTHER_TEXT_INFO_COLOR;
		break;
		
		case OTA_DISPLAY_INFO_TYPE_BAR_NUM:
			if(s_UpgradeBarNum.hundreds > 0){
				p_ota_display_info->text_num = LENGTH_CHINESE_UPGRADE_BAR_NUM3;
			}else if(s_UpgradeBarNum.tens > 0){
				p_ota_display_info->text_num = LENGTH_CHINESE_UPGRADE_BAR_NUM2;
			}else{
				p_ota_display_info->text_num = LENGTH_CHINESE_UPGRADE_BAR_NUM1;
			}
			p_ota_display_info->text_height_offset[0] = OTA_PROGRESS_BAR_CHINESE_NUM_HEIGHT_OFFSET;
			p_ota_display_info->TextColor = OTA_OTHER_TEXT_INFO_COLOR;
		break;

		default:
			CP_LOGE("[ERROR] parameter err, ota_info_content_type:%d\r\n",p_ota_display_info->ota_info_content_type);
			while(1);
	}

	SetPixels_DisplayChinese(p_ota_display_info);
}

static void fill_lcd_english_info_into_framebuffer( OTA_LCD_DISPLAY_INFO_T             	*p_lcd_display_info,
											          const OTA_LCD_DISPLAY_INFO_TYPE  lcd_display_info_type)
{
	UINT16 lcd_width;
	UINT16 lcd_height;

	lcd_width = p_lcd_display_info->LcdWidth;
	lcd_height = p_lcd_display_info->LcdHeight;
	p_lcd_display_info->ota_info_content_type = lcd_display_info_type;

	switch(p_lcd_display_info->ota_info_content_type)
	{
		case OTA_DISPLAY_INFO_TYPE_TIPS:
			if(p_lcd_display_info->ota_info_language_type == CHINESE){
				return;
			}
			p_lcd_display_info->p_text_buf[0]         = OTA_TIPS_TEXT1;
			p_lcd_display_info->text_height_offset[0] = OTA_TIPS_TEXT1_HEIGHT_OFFSET;
			
			p_lcd_display_info->p_text_buf[1]         = OTA_TIPS_TEXT2;
			p_lcd_display_info->text_height_offset[1] = OTA_TIPS_TEXT2_HEIGHT_OFFSET;
			p_lcd_display_info->text_num = 2;
			p_lcd_display_info->TextColor = OTA_TIPS_COLOR;
			break;
		case OTA_DISPLAY_INFO_TYPE_UPGRADE_PRE:
			p_lcd_display_info->p_text_buf[0]         = OTA_STATUS_PRE_TEXT1;
			p_lcd_display_info->text_height_offset[0] = OTA_STATUS_HEIGHT_OFFSET;
			p_lcd_display_info->text_num = 1;
			p_lcd_display_info->TextColor = OTA_OTHER_TEXT_INFO_COLOR;
			break;
		case OTA_DISPLAY_INFO_TYPE_UPDATING:
			p_lcd_display_info->p_text_buf[0]         = OTA_STATUS_UPDATING_TEXT2;
			p_lcd_display_info->text_height_offset[0] = OTA_STATUS_HEIGHT_OFFSET;
			p_lcd_display_info->text_num = 1;				
			p_lcd_display_info->TextColor = OTA_OTHER_TEXT_INFO_COLOR;
			break;
		case OTA_DISPLAY_INFO_TYPE_UPGRADE_SUCCESS:
			p_lcd_display_info->p_text_buf[0]         = OTA_STATUS_RES_TEXT3;
			p_lcd_display_info->text_height_offset[0] = OTA_STATUS_HEIGHT_OFFSET;
			p_lcd_display_info->text_num = 1;
			p_lcd_display_info->TextColor = OTA_OTHER_TEXT_INFO_COLOR;
			break;
		case OTA_DISPLAY_INFO_TYPE_UPGRADE_FAIL:
			p_lcd_display_info->p_text_buf[0]         = OTA_STATUS_RES_TEXT4;
			p_lcd_display_info->text_height_offset[0] = OTA_STATUS_HEIGHT_OFFSET;
			p_lcd_display_info->text_num = 1;
			p_lcd_display_info->TextColor = OTA_OTHER_TEXT_INFO_COLOR;
			break;
		case OTA_DISPLAY_INFO_TYPE_ADDITION_INFO:
			p_lcd_display_info->text_height_offset[0] = OTA_STATUS_ADDITIONAL_HEIGHT_OFFSET;
			p_lcd_display_info->text_num = 1;
			p_lcd_display_info->TextColor = OTA_OTHER_TEXT_INFO_COLOR;
			break;
		case OTA_DISPLAY_INFO_TYPE_BAR_TEXT_AND_NUM:
			p_lcd_display_info->text_height_offset[0] = OTA_PROGRESS_BAR_TEXT_AND_NUM_HEIGHT_OFFSET;
			p_lcd_display_info->text_num = 1;
			p_lcd_display_info->TextColor = OTA_OTHER_TEXT_INFO_COLOR;
			break;
		default:
			CP_LOGE("file:%s,func:%s,line:%d, invalid ota display info type!\r\n",__FILE__,__func__,__LINE__);
			while(1);
	}

	lcd_fill_english_into_framebuffer(p_lcd_display_info);
}

/************************************************* OPEN API **************************************************/
// ----------------- 1. updater ----------------- */
#ifdef SMALL_CODE_UPDATER
void OTA_LCD_ShowProcess(int per)
{
#if (defined LCD_USE_IN_CODE)
	s_ota_LcdShowProcess(per);
	//mdelay(3000);	  //Balance delay to see the "Progress Bar" and considering "OTA time"
#endif	
}

void OTA_LCD_DisplayText(const char *p_addtional_text,const OTA_LCD_DISPLAY_INFO_TYPE ota_info_type,const OTA_LANGUAGE_T ota_language)
{
#if (defined LCD_USE_IN_CODE)
	OTA_LCD_DISPLAY_INFO_T ota_display_info;	 

	/* INIT */
	ota_display_info.LcdWidth = lcd_info_test.width;
	ota_display_info.LcdHeight = lcd_info_test.height;
	ota_display_info.p_FrameBuffer = (uint16_t *)s_get_lcd_FrameBufferPointer();
	memset((char*)(ota_display_info.p_FrameBuffer), 0xff, ota_display_info.LcdWidth*ota_display_info.LcdHeight*2);
	ota_display_info.ota_info_language_type = ota_language;
	CP_LOGD("[DBG] panel weight %d, height %d\r\n",ota_display_info.LcdWidth, ota_display_info.LcdHeight);


#if (defined DISPLAY_CHINESE_INFO)
	/*  3.1 write chinese tips data into framebuffer */
	if(ota_display_info.ota_info_language_type == CHINESE){
		//display tips info
		fill_lcd_chinese_info_into_framebuffer(&ota_display_info,OTA_DISPLAY_INFO_TYPE_TIPS);

		//display user info
		fill_lcd_chinese_info_into_framebuffer(&ota_display_info,ota_info_type);
	}
#endif
	if(ota_display_info.ota_info_language_type == ENGLISH){
		/*  3.2 write english tips data into framebuffer */
		// display tips info
		fill_lcd_english_info_into_framebuffer(&ota_display_info, OTA_DISPLAY_INFO_TYPE_TIPS);

		// display user info
		fill_lcd_english_info_into_framebuffer(&ota_display_info, ota_info_type);	 
	}
	/* [4] display additional content */
	if(p_addtional_text != NULL){
		if(ota_display_info.ota_info_language_type == ENGLISH){
		 ota_display_info.p_text_buf[0] = p_addtional_text;
		 fill_lcd_english_info_into_framebuffer(&ota_display_info, OTA_DISPLAY_INFO_TYPE_ADDITION_INFO);
		}
	}else{
		CP_LOGD("p_addtional_text is NULL\r\n");
	}

	LcdDisplayCleanMemoryCache((void*)ota_display_info.p_FrameBuffer,(ota_display_info.LcdWidth*ota_display_info.LcdHeight*2));
	
	ASRLCDD_Logo_Blit16_sync(ota_display_info.p_FrameBuffer, ota_display_info.LcdWidth, ota_display_info.LcdHeight, 0, 0, ota_display_info.LcdWidth * 2);
	mdelay(1000);
#endif
}

void OTA_LCD_ShowImage( void )
{
#if (defined LCD_USE_IN_CODE) && (defined FOTA_ASRSD)
	UINT16 *p_image_buf = OTA_LCD_Success_Logo;

	CP_LOGI("%s start\n",__func__);
	CP_LOGI("panel weight %d, height %d \n",OTA_LCD_SUCESS_LOGO_WIDTH, OTA_LCD_SUCESS_LOGO_HEIGHT);  

	ASRLCDD_Logo_Blit16_sync(p_image_buf,OTA_LCD_SUCESS_LOGO_WIDTH,OTA_LCD_SUCESS_LOGO_HEIGHT, 0, 0, OTA_LCD_SUCESS_LOGO_WIDTH * 2);
	mdelay(1000*50);

	CP_LOGI("%s end\n",__func__);
#endif
}

void OTA_Clear_LCD(void)
{
//#ifdef LCD_USE_IN_CODE
//	struct asrlcdd_rect dest_rect;
//	dest_rect.startX = 0;
//	dest_rect.startY = 0;
//	dest_rect.width = lcd_info_test.width;
//	dest_rect.height = lcd_info_test.height;
//	ASRLCDD_Logo_Fill_Rect(&dest_rect,0xFFFFFF);
//#endif
}
#endif //SMALL_CODE_UPDATER

// ----------------- 2. logo ----------------- */
#ifdef SMALL_CODE_LOGO
void POWER_ON_LCD_ShowImage( void )
{
#ifdef LCD_USE_IN_CODE
	UINT16 *p_image_buf = (UINT16 *)Power_On_Logo;
	CP_LOGI("SHOW LOGO START\r\n");

	CP_LOGI("Number of elements:0x%x\r\n",sizeof(Power_On_Logo)/sizeof(UINT16));
	ASRLCDD_Logo_Blit16_sync((UINT16 *)p_image_buf,lcd_info_test.width, lcd_info_test.height, 0, 0, lcd_info_test.width * 2);

	//  mdelay(50);

	CP_LOGI("SHOW LOGO END\r\n");
#endif
}
#endif /* SMALL_CODE_LOGO */
											  



													  

// ----------------- 3. utility ----------------- */
void lcd_get_panel_info( void )
{
#ifdef LCD_USE_IN_CODE
	ASRLCDD_GetUIInfo(&lcd_info_test.width, &lcd_info_test.height);
	CP_LOGI("panel weight:%d,height:%d\r\n",lcd_info_test.width, lcd_info_test.height);
#endif
}

// ----------------- 4. debug ----------------- */
#if ( defined LCD_USE_IN_CODE )	&& (defined SMALL_CODE_UPDATER) && ( defined TEST_OTA_DISPLAY_INFO )
void test_logo_LcdShowProcess_Demo( void )
{
	UINT8 i;

	for(i = 0; i < 11; i++){
		s_ota_LcdShowProcess(i * 10);
		mdelay(1000*5);
	}
}

void test_OTA_LCD_DisplayText( void )
{
	char OTA_LCD_info[10][40] = {	
									"Start Updating!",					//0   ----   ota status info
									"Start Erasing Flash",				//1   ----   ota status info
									"Start Writing Flash",				//2   ----   ota status info
									"Update Success!",					//3   ----   ota status info
									"OTA_Sd update Failed!",			//4   ----   ota status info
									"Update file Error!",				//5   ----   ota status info
									"Write End"							//6   ----   ota status info
								};
	OTA_LANGUAGE_T language_type = Get_LanguageType();

	/*	adpus*/
	CP_LOGI("adups display info\r\n");							
	OTA_LCD_DisplayText(OTA_ERR_TEXT1,OTA_DISPLAY_INFO_TYPE_UPGRADE_FAIL,language_type);
	mdelay(DELAY_TIME2);
	OTA_LCD_DisplayText(OTA_ERR_TEXT2,OTA_DISPLAY_INFO_TYPE_UPGRADE_FAIL,language_type);
	mdelay(DELAY_TIME2);
	OTA_LCD_DisplayText(NULL,OTA_DISPLAY_INFO_TYPE_UPGRADE_PRE,language_type);
	mdelay(DELAY_TIME2);
	OTA_LCD_DisplayText(NULL,OTA_DISPLAY_INFO_TYPE_UPGRADE_PRE,language_type);
	mdelay(DELAY_TIME2);
	OTA_LCD_DisplayText(NULL,OTA_DISPLAY_INFO_TYPE_UPGRADE_PRE,language_type);
	mdelay(DELAY_TIME2);
	
	/* ASRSD */
	CP_LOGI("ASRSD display info\r\n");							
	OTA_LCD_DisplayText(NULL,OTA_DISPLAY_INFO_TYPE_UPGRADE_PRE,language_type);
	mdelay(DELAY_TIME2);
	OTA_LCD_DisplayText(OTA_LCD_info[5],OTA_DISPLAY_INFO_TYPE_UPGRADE_FAIL,language_type);
	mdelay(DELAY_TIME2);
	OTA_LCD_DisplayText(NULL,OTA_DISPLAY_INFO_TYPE_UPGRADE_PRE,language_type);
	mdelay(DELAY_TIME2);
}
#endif /* #if ( defined LCD_USE_IN_CODE )	&& (defined SMALL_CODE_UPDATER) && ( defined TEST_OTA_DISPLAY_INFO ) */	


//void dumpLCDReg()
//{
//	UINT32 lcd_base_addr = 0xD420A000;
//	UINT32 i=0;

//	uart_printf("0xD428284C = 0x%lx\r\n",*(volatile unsigned long *)(0xd4282800 + 0x4C));
//	uart_printf("0xD4282850 = 0x%lx\r\n",*(volatile unsigned long *)(0xd4282800 + 0X50));
//	uart_printf("0xD420A118 = 0x%lx\r\n",*(volatile unsigned long *)(0xD420A118));
//	uart_printf("0xd420a09c = 0x%lx\r\n",*(volatile unsigned long *)(0xd420a09c));
//	for(i=0x34; i<=0x48;i+=4)
//	{
//		uart_printf("(0x%lx) = 0x%lx\r\n",lcd_base_addr+i,*(volatile unsigned long *)(lcd_base_addr+i));
//	}

//	for(i=0x70; i<=0x90;i+=4)
//	{
//		uart_printf("(0x%lx) = 0x%lx\r\n",lcd_base_addr+i,*(volatile unsigned long *)(lcd_base_addr+i));
//	}
//	
//	for(i=0xC0; i<=0xC8;i+=4)
//	{
//		uart_printf("(0x%lx) = 0x%lx\r\n",lcd_base_addr+i,*(volatile unsigned long *)(lcd_base_addr+i));
//	}

//	for(i=0xE0; i<=0x108;i+=4)
//	{
//		uart_printf("(0x%lx) = 0x%lx\r\n",lcd_base_addr+i,*(volatile unsigned long *)(lcd_base_addr+i));
//	}

//	for(i=0x124; i<=0x164;i+=4)
//	{
//		uart_printf("(0x%lx) = 0x%lx\r\n",lcd_base_addr+i,*(volatile unsigned long *)(lcd_base_addr+i));
//	}

//	for(i=0x174; i<=0x188;i+=4)
//	{
//		uart_printf("(0x%lx) = 0x%lx\r\n",lcd_base_addr+i,*(volatile unsigned long *)(lcd_base_addr+i));
//	}

//	for(i=0x190; i<=0x19C;i+=4)
//	{
//		uart_printf("(0x%lx) = 0x%lx\r\n",lcd_base_addr+i,*(volatile unsigned long *)(lcd_base_addr+i));
//	}

//	for(i=0x1A8; i<=0x1DC;i+=4)
//	{
//		uart_printf("(0x%lx) = 0x%lx\r\n",lcd_base_addr+i,*(volatile unsigned long *)(lcd_base_addr+i));
//	}

//	for(i=0x1E8; i<=0x1F8;i+=4)
//	{
//		uart_printf("(0x%lx) = 0x%lx\r\n",lcd_base_addr+i,*(volatile unsigned long *)(lcd_base_addr+i));
//	}

//	for(i=0x200; i<=0x20C;i+=4)
//	{
//		uart_printf("(0x%lx) = 0x%lx\r\n",lcd_base_addr+i,*(volatile unsigned long *)(lcd_base_addr+i));
//	}
//}

