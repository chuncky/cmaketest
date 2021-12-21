#include "mci_lcd.h"
#include "lcdd_asr.h"
#include "pmic.h"

static MCI_LCD_BPFUN_T     g_mciLcdBypassFunction=NULL;
static MCI_ASR_LCD_INFO  g_lcd_panel_info; 
#define raw_uart_log uart_printf
// ============================================================================
// mci_LcdInit
// ----------------------------------------------------------------------------
// ============================================================================
#if 0 //ryz
VOID mci_LcdInit(UINT32 background)
{
    static BOOL visitFlag = 0;
	struct panel_spec* panel_info;


	if (!visitFlag)
    {   
		raw_uart_log("mci_LcdInit ++: %d\r\n",background);
		memset(&g_lcd_panel_info, 0, sizeof(g_lcd_panel_info));
		ASRLCDD_Phase1Init();
    	ASRLCDD_Phase2Init();
    	
		panel_info = ASRLCDD_GetPanelInfo();
		g_lcd_panel_info.format = ASR_PANEL_COLOR_FORMAT_RGB565;
		g_lcd_panel_info.bitsPerPixel= 16;
		g_lcd_panel_info.screen_width= panel_info->width;
		g_lcd_panel_info.screen_height = panel_info->height;
		g_lcd_panel_info.fb_width= panel_info->width * g_lcd_panel_info.bitsPerPixel/8;
		g_lcd_panel_info.fb_height= panel_info->height;
		g_lcd_panel_info.mode= panel_info->type;
		g_lcd_panel_info.backlightLevel = 5;
        g_mciLcdBypassFunction=NULL;
		mci_LcdSetBrightness(5);
		//mci_LcdClearScreen(background);
        visitFlag = 1;
		raw_uart_log("mci_LcdInit--\r\n");
    }
}
#endif
extern MCI_ASR_LCD_INFO lcd_info_test;
extern	store_lcd_panel_id(const UINT32 panel_id);
extern UINT32 get_lcd_panel_id(void);
VOID mci_Logo_LcdInit(void)
{
    static BOOL visitFlag = 0;
	struct panel_spec* panel_info;
	int ret;
	
//	raw_uart_log("mci_Logo_LcdInit ++\r\n");
#if 1
	if (!visitFlag)
    {   
    	LCDLOGD("mci_Logo_LcdInit\r\n");
		memset(&g_lcd_panel_info, 0, sizeof(g_lcd_panel_info));
    	ret = ASRLCDD_Logo_Open();
		if(ret != 0){
			raw_uart_log("ASRLCDD_Logo_Open fail\r\n");
			return;
		}
		
        panel_info = ASRLCDD_GetPanelInfo();
		g_lcd_panel_info.format = ASR_PANEL_COLOR_FORMAT_RGB565;
		g_lcd_panel_info.bitsPerPixel= 16;
		g_lcd_panel_info.screen_width= panel_info->width;
		g_lcd_panel_info.screen_height = panel_info->height;
		g_lcd_panel_info.fb_width= panel_info->width * g_lcd_panel_info.bitsPerPixel/8;
		g_lcd_panel_info.fb_height= panel_info->height;
		g_lcd_panel_info.mode= panel_info->type;
		g_lcd_panel_info.backlightLevel = 3;
        g_mciLcdBypassFunction=NULL;
		//mci_LcdSetBrightness(5);
        //  visitFlag = 1;	
    }
#endif

	mci_LcdGetPanelInfo(&lcd_info_test);
	mci_LcdSetFBInfo(lcd_info_test.screen_width*lcd_info_test.bitsPerPixel/8,lcd_info_test.screen_height);

#ifdef SMALL_CODE_LOGO
	//Note: If lcd init success, set flag;
	if (PMIC_IS_PM813()||PMIC_IS_PM813S())
	{
    	pm813_userdata_set_app_lcd_ever_configed();
    	CP_LOGI("set init flag,value:0x%x\r\n",pm813_userdata_read_app_lcd_ever_configed());
	}
	else if (PMIC_IS_PM802())
	{
        pm802_userdata_set_app_lcd_ever_configed();
        CP_LOGI("set init flag,value:0x%x\r\n",pm802_userdata_read_app_lcd_ever_configed());
	}

	store_lcd_panel_id(panel_info->panel_id);
	CP_LOGI("lcd_panel_id:0x%x\r\n",get_lcd_panel_id());	
#endif	
}

void mci_Logo_LcdClose(void)
{
	int ret;
	raw_uart_log("mci_LcdClose++ \r\n");

	ASRLCDD_Logo_Close();
	
	raw_uart_log("mci_LcdClose--\r\n");
	return ;
}

VOID mci_LcdSetFBInfo(int stride, int height)
{
	LCDLOGD("DBG: mci_LcdSetFBInfo (%d, %d)\r\n", stride, height);
	g_lcd_panel_info.fb_width= stride;
	g_lcd_panel_info.fb_height= height;
}



// ============================================================================
// mci_LcdScreenOn
// ----------------------------------------------------------------------------
VOID mci_LcdScreenOn(BOOL on)
{
    raw_uart_log("mci_LcdScreenOn %d ++\r\n", on);
    if ( on == TRUE )
    {
        raw_uart_log("mci_LcdScreenOn: Turn On with brightness %d\r\n",g_lcd_panel_info.backlightLevel);
        ASRLCDD_WakeUp();
        ASRLCDD_SetBrightness(g_lcd_panel_info.backlightLevel);
    } 
    else 
    {
        raw_uart_log("mci_LcdScreenOn: Turn Off\r\n");
        ASRLCDD_SetBrightness(0);
        ASRLCDD_Sleep();
    }
}

#if 0
// ============================================================================
// mci_LcdlayerMerge
// ----------------------------------------------------------------------------
VOID mci_LcdLayerMerge(UINT16* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy)
{
    LCDD_FBW_T frameBufferWin;
    UINT32 attempts = 0;

    if (!buffer)
    {
        raw_uart_log("mci_LcdLayerMerge buffer == 0");
    }
    else
    {
        raw_uart_log("mci_LcdLayerMerge %d %d %d %d",startx, starty, endx, endy);
        frameBufferWin.fb.buffer = buffer;
        frameBufferWin.fb.width = g_lcd_panel_info.width;
        frameBufferWin.fb.height = g_lcd_panel_info.height;

        frameBufferWin.roi.x=startx;
        frameBufferWin.roi.y=starty;
        frameBufferWin.roi.width=endx-startx+1;
        frameBufferWin.roi.height=endy-starty+1;

        while(ASRLCDD_LayerMerge(&frameBufferWin,startx,starty,TRUE) != LCDD_ERR_NO)
        {
            attempts++;
            if (attempts%1024)
            {
                raw_uart_log("mci_LcdLayerMerge Access Denied :%d!", attempts);
            }
            UOS_SleepTicks(64);
        }
    }
}
#endif

// ============================================================================
// mci_LcdBlockWrite
// ----------------------------------------------------------------------------
#if 0 //ryz
VOID mci_LcdBlockWrite(UINT16* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy)
{
    struct asrlcdd_framebuffer_window frameBufferWin;
    UINT32 attempts = 0;
	UINT32 offset = 0;

    if (g_mciLcdBypassFunction)
    {
        raw_uart_log("mci_LcdBlockWrite bypassed %d %d %d %d\r\n",startx, starty, endx, endy);
        g_mciLcdBypassFunction(buffer,startx, starty, endx, endy);
    }
    else
    {
		
		raw_uart_log("mci_LcdBlockWrite : from %d/%d(x/y) to %d/%d!\r\n", startx,starty,endx,endy);
        frameBufferWin.frame_info.pbuffer = buffer;
        frameBufferWin.frame_info.stride = g_lcd_panel_info.fb_width;
        frameBufferWin.frame_info.height = g_lcd_panel_info.fb_height;
		frameBufferWin.frame_info.bitdepth = g_lcd_panel_info.bitsPerPixel;

        frameBufferWin.roi_info.startX=startx;
        frameBufferWin.roi_info.startY=starty;
        frameBufferWin.roi_info.width=endx-startx+1;
        frameBufferWin.roi_info.height=endy-starty+1;
		#if 1
		offset = frameBufferWin.frame_info.stride * starty + startx *frameBufferWin.frame_info.bitdepth/8;
		CacheCleanMemory( (void *)((char*)buffer + offset), frameBufferWin.frame_info.stride*frameBufferWin.roi_info.height);
		#else
		CacheCleanMemory( (void *)buffer, g_mciLcdFBInfo.width*g_mciLcdFBInfo.height*frameBufferWin.frame_info.bitdepth/8);
		#endif
		
        while(ASRLCDD_Blit16(&frameBufferWin,startx,starty)!=0)
        {
            attempts++;
            if (attempts%1024)
            {
                raw_uart_log("mci_LcdBlockWrite Access Denied :%d!\r\n", attempts);
				return;
            }
        }

		
		raw_uart_log("mci_LcdBlockWrite end\r\n");
	
    }
}

// ============================================================================
// mci_LcdBlockWrite_sync
// ----------------------------------------------------------------------------
VOID mci_LcdBlockWrite_sync(UINT16* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy)
{
	struct asrlcdd_framebuffer_window frameBufferWin;
	UINT32 attempts = 0;
	UINT32 offset = 0;
	
	if (g_mciLcdBypassFunction)
	{
		raw_uart_log("mci_LcdBlockWrite_sync bypassed %d %d %d %d\r\n",startx, starty, endx, endy);
		g_mciLcdBypassFunction(buffer,startx, starty, endx, endy);
	}
	else
	{
		
		raw_uart_log("mci_LcdBlockWrite_sync : from %d/%d(x/y) to %d/%d!\r\n", startx,starty,endx,endy);

		frameBufferWin.frame_info.pbuffer = buffer;
		frameBufferWin.frame_info.stride = g_lcd_panel_info.fb_width;
		frameBufferWin.frame_info.height = g_lcd_panel_info.fb_height;
		frameBufferWin.frame_info.bitdepth = g_lcd_panel_info.bitsPerPixel;
	
		frameBufferWin.roi_info.startX=startx;
		frameBufferWin.roi_info.startY=starty;
		frameBufferWin.roi_info.width=endx-startx+1;
		frameBufferWin.roi_info.height=endy-starty+1;

	#if 1
		offset = frameBufferWin.frame_info.stride * starty + startx *frameBufferWin.frame_info.bitdepth/8;
		CacheCleanMemory( (void *)((char*)buffer + offset), frameBufferWin.frame_info.stride*frameBufferWin.roi_info.height);
	#else
		CacheCleanMemory( (void *)buffer, g_mciLcdFBInfo.width*g_mciLcdFBInfo.height*2);
	#endif
			
		while(ASRLCDD_Blit16_sync(&frameBufferWin,startx,starty)!=0)
		{
			attempts++;
			if (attempts%1024)
			{
				raw_uart_log("mci_LcdBlockWrite_sync Access Denied :%d!\r\n", attempts);
				return;
			}
		}

		
		raw_uart_log("mci_LcdBlockWrite_sync end\r\n");
	}

	
}
#endif
VOID mci_Logo_LcdBlockWrite_sync(UINT16* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy)
{
	struct asrlcdd_framebuffer_window frameBufferWin;
	UINT32 attempts = 0;
	UINT32 offset = 0;
	LCDLOGD("DBG: mci_Logo_LcdBlockWrite_sync ++\r\n");
	
	if (g_mciLcdBypassFunction)
	{
		raw_uart_log("mci_Logo_LcdBlockWrite_sync bypassed %d %d %d %d\r\n",startx, starty, endx, endy);
		g_mciLcdBypassFunction(buffer,startx, starty, endx, endy);
	}
	else
	{
		//raw_uart_log("mci_LcdBlockWrite %d %d %d %d\r\n",startx, starty, endx, endy);
		frameBufferWin.frame_info.pbuffer = buffer;
		frameBufferWin.frame_info.stride =  g_lcd_panel_info.fb_width;
		frameBufferWin.frame_info.height = g_lcd_panel_info.fb_height;
		frameBufferWin.frame_info.bitdepth = g_lcd_panel_info.bitsPerPixel;
	
		frameBufferWin.roi_info.startX=startx;
		frameBufferWin.roi_info.startY=starty;
		frameBufferWin.roi_info.width=endx-startx+1;
		frameBufferWin.roi_info.height=endy-starty+1;
#if 1
			offset = frameBufferWin.frame_info.stride * starty + startx *frameBufferWin.frame_info.bitdepth/8;
			CacheCleanMemory( (void *)((char*)buffer + offset), frameBufferWin.frame_info.stride*frameBufferWin.roi_info.height);
#else
			CacheCleanMemory( (void *)buffer, g_mciLcdFBInfo.width*g_mciLcdFBInfo.height*2);
#endif
			
		while(ASRLCDD_Ass_Blit16_sync(&frameBufferWin,startx,starty)!=0)
		{
			attempts++;
			if (attempts%1024)
			{
				raw_uart_log("mci_Logo_LcdBlockWrite_sync Access Denied :%d!\r\n", attempts);
				return;
			}
		}
		
	}

	LCDLOGI("INFO: mci_Logo_LcdBlockWrite_sync end\r\n");
}


VOID mci_LcdLayerMerge(UINT16* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy)
{
    raw_uart_log("mci_LcdLayerMerge ++\r\n");
}

// ============================================================================
// mci_LcdGetDimension
// ----------------------------------------------------------------------------
VOID mci_LcdGetDimension(UINT16 *out_LCD_width,UINT16 *out_LCD_height)
{
    raw_uart_log("mci_LcdGetDimension ++");
    *out_LCD_width=g_lcd_panel_info.screen_width;
    *out_LCD_height=g_lcd_panel_info.screen_height;
}
#if 0
// ============================================================================
// mci_LcdSleep
// ----------------------------------------------------------------------------
VOID mci_LcdSleep(VOID)
{
    raw_uart_log("mci_LcdSleep ++\r\n");
    ASRLCDD_SetBrightness(0);
    ASRLCDD_Sleep();
}

// ============================================================================
// mci_LcdWakeup
// ----------------------------------------------------------------------------
VOID mci_LcdWakeup(VOID)
{
    raw_uart_log("mci_LcdWakeup ++\r\n");
    ASRLCDD_WakeUp();
    ASRLCDD_SetBrightness(g_mciLcdBrightnessLevel);
}
#endif
// ============================================================================
// mci_LcdPartialOn
// ----------------------------------------------------------------------------
VOID mci_LcdPartialOn(UINT16 startLine,UINT16 endLine)
{
    raw_uart_log("mci_LcdPartialOn %d %d ++\r\n",startLine, endLine);
}

// ============================================================================
// mci_LcdPartialOff
// ----------------------------------------------------------------------------
VOID mci_LcdPartialOff(VOID)
{
    raw_uart_log("mci_LcdPartialOff ++\r\n");
}

// ============================================================================
// mci_LcdClearScreen
// ----------------------------------------------------------------------------
VOID mci_LcdClearScreen(UINT16 background)
{
    struct asrlcdd_roi_info roi;

    raw_uart_log("mci_LcdClearScreen %d ++\r\n",background);
    roi.startX=0;
    roi.startY=0;
    roi.width=g_lcd_panel_info.screen_width;
    roi.height=g_lcd_panel_info.screen_height;

    ASRLCDD_FillRect16(&roi, background);

	raw_uart_log("mci_LcdClearScreen --\r\n");
}

// ============================================================================
// mci_LcdGetParam
// ----------------------------------------------------------------------------
UINT8 mci_LcdGetParam(MCI_LCD_PARAM_T param_id)
{
    raw_uart_log("mci_LcdGetParam %d ++\r\n");
    switch(param_id)
    {
        // TODO implement other parameters
        case MCI_LCD_BIAS:
        case MCI_LCD_CONTRAST:
        case MCI_LCD_LINE_RATE:
        case MCI_LCD_TEMP_COMP:
            return 0;
            break;
        case MCI_LCD_BRIGHTNESS:
            return g_lcd_panel_info.backlightLevel;
            break;
        default:
            break;
    }
    return 0;
}

void mci_LcdGetPanelInfo(MCI_ASR_LCD_INFO *panel_info)
{
	memcpy(panel_info,&g_lcd_panel_info,sizeof(MCI_ASR_LCD_INFO));
}
// ============================================================================
// mci_LcdSetBrightness
// ----------------------------------------------------------------------------
VOID mci_LcdSetBrightness(UINT8 level)
{   
    CP_LOGD("mci_LcdSetBrightness %d++\r\n", level);
    g_lcd_panel_info.backlightLevel = level;
    ASRLCDD_SetBrightness(level);
}
VOID mci_LcdSetBrightnessLevel(UINT8 level)
{   
    raw_uart_log("mci_LcdSetBrightnessLevel %d++\r\n", level);
    g_lcd_panel_info.backlightLevel = level;
}
VOID mci_LcdSetBrightnessExt(UINT8 level)
{   
    raw_uart_log("mci_LcdSetBrightnessExt %d++\r\n", level);
    ASRLCDD_SetBrightness(level);
}
// ============================================================================
// mci_LcdSetBias
// ----------------------------------------------------------------------------
VOID mci_LcdSetBias(UINT8 bias)
{
    raw_uart_log("mci_LcdSetBias %d++\r\n", bias);
}

// ============================================================================
// mci_LcdSetContrast
// ----------------------------------------------------------------------------
VOID mci_LcdSetContrast(UINT8 contrast)
{
    raw_uart_log("mci_LcdSetContrast %d++\r\n",contrast);
}

// ============================================================================
// mci_LcdSetLineRate
// ----------------------------------------------------------------------------
VOID mci_LcdSetLineRate(UINT8 linerate)
{
    raw_uart_log("mci_LcdSetLineRate %d++\r\n",linerate);
}

// ============================================================================
// mci_LcdSetTempComp
// ----------------------------------------------------------------------------
VOID mci_LcdSetTempComp(UINT8 compensate)
{
    raw_uart_log("mci_LcdSetTempComp %d++\r\n",compensate);
}
