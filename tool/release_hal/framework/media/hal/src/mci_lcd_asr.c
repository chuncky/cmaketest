#include "mci_lcd.h"
#include "lcdd_asr.h"
#include "ui_log_api.h"
#include "fs_api.h"
#include "ui_os_api.h"

enum {
	MCI_LCD_STATUS_UNINIT = 0,
	MCI_LCD_STATUS_SLEEP,
	MCI_LCD_STATUS_NORMAL
};

PRIVATE struct asrlcdd_screen_info  g_mciLcdScreenInfo;
PRIVATE struct asrlcdd_screen_info  g_mciLcdFBInfo;
PRIVATE UINT32 g_mciLcdFBBitdepth = 16;

PUBLIC UINT8 g_mciLcdBrightnessLevel=0xff;
PUBLIC BOOL g_mciLcdBackLightOn=FALSE;
extern unsigned char g_asrlcd_framebuffer[320*320*4];


int g_mciActiveLcd = MCI_LCD_ID_MAIN;
int g_mciLcdStatus = MCI_LCD_STATUS_UNINIT;
volatile int g_switch_lcd_mutex_id = INVALID_MUTEX_ID;

#ifdef LCD_DUAL_PANEL_SUPPORT
PRIVATE struct asrlcdd_screen_info  g_mciSubLcdScreenInfo;
PRIVATE struct asrlcdd_screen_info  g_mciSubLcdFBInfo;
PRIVATE UINT32 g_mciSubLcdFBBitdepth = 16;
PUBLIC UINT8 g_mciSubLcdBrightnessLevel=0xff;

#ifdef CONFIG_UI_SUB_LCD
extern BOOL sub_lcd_flag;
#endif

#endif

static int to_framewindow(UINT16* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy,
								struct asrlcdd_framebuffer_window *frameBufferWin, char* name, int lcd_id)
{
	struct asrlcdd_screen_info *fb_info = NULL;
	struct asrlcdd_screen_info *screen_info = NULL;
	uint32_t bitdepth;

	fb_info = &g_mciLcdFBInfo;
	screen_info = &g_mciLcdScreenInfo;
	bitdepth = g_mciLcdFBBitdepth;

#ifdef LCD_DUAL_PANEL_SUPPORT
	if(lcd_id == MCI_LCD_ID_SUB){
		fb_info = &g_mciSubLcdFBInfo;
		screen_info = &g_mciSubLcdScreenInfo;
		bitdepth = g_mciSubLcdFBBitdepth;
	}
#endif
	
    //raw_uart_log("%s: %d %d %d %d\r\n",name, startx, starty, endx, endy);
    frameBufferWin->frame_info.pbuffer = buffer;
    frameBufferWin->frame_info.stride = fb_info->width;//g_mciLcdScreenInfo.width * 2;//(endx-startx+1) * 2;//
    frameBufferWin->frame_info.height = fb_info->height;//g_mciLcdScreenInfo.height;//endy-starty+1;//
	frameBufferWin->frame_info.bitdepth = bitdepth;

    frameBufferWin->roi_info.startX=startx;
    frameBufferWin->roi_info.startY=starty;
    frameBufferWin->roi_info.width=endx-startx+1;
    frameBufferWin->roi_info.height=endy-starty+1;

//		raw_uart_log("%s1: (%d,%d,%d,%d)\r\n",name, frameBufferWin->roi_info.startX, frameBufferWin->roi_info.startY,
//			frameBufferWin->roi_info.width, frameBufferWin->roi_info.height);

    frameBufferWin->roi_info.startX = (frameBufferWin->roi_info.startX >> 3) << 3;
    frameBufferWin->roi_info.startY = (frameBufferWin->roi_info.startY >> 2) << 2;

    frameBufferWin->roi_info.width = endx - frameBufferWin->roi_info.startX + 1;
    frameBufferWin->roi_info.height = endy - frameBufferWin->roi_info.startY + 1;
    frameBufferWin->roi_info.width = (frameBufferWin->roi_info.width + 7)/8*8;
    frameBufferWin->roi_info.height = (frameBufferWin->roi_info.height + 3)/4*4;

//		raw_uart_log("%s2: (%d,%d,%d,%d)\r\n",name, frameBufferWin->roi_info.startX, frameBufferWin->roi_info.startY,
//			frameBufferWin->roi_info.width, frameBufferWin->roi_info.height);

	if(frameBufferWin->roi_info.startX >= screen_info->width){
		raw_uart_log("%s: Invalid refresh area (startx = %d, width = %d)\r\n",
			name, frameBufferWin->roi_info.startX, screen_info->width);
		return -1;
	}

	if(frameBufferWin->roi_info.startY >= screen_info->height){
		raw_uart_log("%s: Invalid refresh area (startY = %d, height = %d)\r\n",
			name, frameBufferWin->roi_info.startY, screen_info->height);
		return -1;
	}

    if(frameBufferWin->roi_info.width > (screen_info->width - frameBufferWin->roi_info.startX)){
            frameBufferWin->roi_info.width = screen_info->width -frameBufferWin->roi_info.startX;
    }

    if(frameBufferWin->roi_info.height > (screen_info->height - frameBufferWin->roi_info.startY)){
            frameBufferWin->roi_info.height = screen_info->height - frameBufferWin->roi_info.startY;
    }
	return 0;
}

static VOID mci_MainLcdScreenOn(BOOL on)
{
    raw_uart_log("mci_MainLcdScreenOn (on:%d) ++\r\n", on);
    if ( on == TRUE )
    {
        raw_uart_log("mci_MainLcdScreenOn: Turn On with brightness %d\r\n",g_mciLcdBrightnessLevel);
    	ASRLCDD_SetBrightness(g_mciLcdBrightnessLevel);
    } else {
        raw_uart_log("mci_MainLcdScreenOn: Turn Off\r\n");
       	ASRLCDD_SetBrightness(0);
    }
}

static VOID mci_MainLcdClearScreen(UINT16 background)
{
    struct asrlcdd_roi_info roi;

    raw_uart_log("mci_MainLcdClearScreen %d ++\r\n",background);
    roi.startX=0;
    roi.startY=0;
    roi.width=g_mciLcdScreenInfo.width;
    roi.height=g_mciLcdScreenInfo.height;

    ASRLCDD_FillRect16(&roi, background);
}

static VOID mci_MainLcdBlockWrite(UINT16* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy)
{
    struct asrlcdd_framebuffer_window frameBufferWin;
    UINT32 attempts = 0;
	int ret;
//    raw_uart_log("mci_MainLcdBlockWrite (%d, %d, %d, %d)++\r\n", startx, starty, endx, endy);

//	dump_lcd_framebuffer();

	ret = to_framewindow(buffer, startx, starty, endx, endy, &frameBufferWin, __func__, MCI_LCD_ID_MAIN);
	if(0 != ret)
		return;

    while(ASRLCDD_Blit16(&frameBufferWin,frameBufferWin.roi_info.startX,frameBufferWin.roi_info.startY)!=0)
    {
        attempts++;
        if (attempts%1024)
        {
            raw_uart_log("mci_MainLcdBlockWrite Access Denied :%d!\r\n", attempts);
			return;
        }
        //UOS_SleepTicks(64);
    }
}

static VOID mci_MainLcdSetBrightness(UINT8 level)
{   
    raw_uart_log("mci_MainLcdSetBrightness %d++\r\n", level);

	g_mciLcdBrightnessLevel = level;
    ASRLCDD_SetBrightness(g_mciLcdBrightnessLevel);
}

static VOID mci_MainLcdSetBrightnessExt(UINT8 level)
{   
    raw_uart_log("mci_MainLcdSetBrightnessExt %d++\r\n", level);
    ASRLCDD_SetBrightness(level);
}

static VOID mci_MainLcdTurnOffBacklight(void)
{
    raw_uart_log("mci_MainLcdTurnOffBacklight++\r\n");
    ASRLCDD_SetBrightness(0);
    //backlight_set_brightness(0);
}

static VOID mci_MainLcdWakeup(VOID)
{
#if 0
    BOOL orig_status = g_mciLcdBackLightOn;
    if(g_mciLcdBackLightOn == FALSE)
        ASRLCDD_WakeUp();
    else{
        ASRLCDD_SetBrightness(g_mciLcdBrightnessLevel);
        g_mciLcdBackLightOn = TRUE;
    }

    raw_uart_log("mci_LcdWakeup, backlight flag:[%d,%d]\n", orig_status, g_mciLcdBackLightOn);
#else
	raw_uart_log("mci_MainLcdWakeup ++\r\n");
	ASRLCDD_WakeUp();
//	mci_LcdSetBrightness(g_mciLcdBrightnessLevel);
	g_mciLcdStatus = MCI_LCD_STATUS_NORMAL;
#endif
}

static VOID mci_MainLcdSleep(VOID)
{
#if 0
    BOOL orig_status = g_mciLcdBackLightOn;
	ASRLCDD_SetBrightness(0);
    /*
    * the variable g_mciLcdBackLightOn MUST be
    * set AFTER calling ASRLCDD_SetBrightness()
    */
    g_mciLcdBackLightOn = FALSE;
    raw_uart_log("mci_LcdSleep, backlight flag:[%d,%d]\n", orig_status, g_mciLcdBackLightOn);
#else
	raw_uart_log("mci_MainLcdSleep ++\r\n");
	mci_MainLcdTurnOffBacklight();
	ASRLCDD_Sleep();
	g_mciLcdStatus = MCI_LCD_STATUS_SLEEP;
#endif
}

#ifdef LCD_DUAL_PANEL_SUPPORT
VOID mci_SubLcdSetFBInfo(int stride, int height)
{
	raw_uart_log("mci_SubLcdSetFBInfo (%d, %d)\r\n", stride, height);
	g_mciSubLcdFBInfo.width = stride;
	g_mciSubLcdFBInfo.height = height;
}

VOID mci_SubLcdSetFBBitdepth(int bitdepth)
{
	raw_uart_log("mci_SubLcdSetFBBitdepth (%d)\r\n", bitdepth);
	g_mciSubLcdFBBitdepth = bitdepth;
}

static VOID mci_SubLcdScreenOn(BOOL on)
{
    raw_uart_log("mci_SubLcdScreenOn (on:%d) ++\r\n", on);
    if ( on == TRUE )
    {
        raw_uart_log("mci_SubLcdScreenOn: Turn On with brightness %d\r\n",g_mciSubLcdBrightnessLevel);
        	ASRLCDD_SetSubBrightness(g_mciSubLcdBrightnessLevel);
    } else {
        raw_uart_log("mci_SubLcdScreenOn: Turn Off\r\n");
			ASRLCDD_SetSubBrightness(0);
    }
}

static VOID mci_SubLcdBlockWrite(UINT16* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy)
{
    struct asrlcdd_framebuffer_window frameBufferWin;
    UINT32 attempts = 0;
	int ret;
//	raw_uart_log("mci_SubLcdBlockWrite++ %d %d %d %d\r\n",startx, starty, endx, endy);

	ret = to_framewindow(buffer, startx, starty, endx, endy, &frameBufferWin, __func__, MCI_LCD_ID_SUB);
	if(0 != ret)
		return;
	
    while(ASRLCDD_SubBlit16(&frameBufferWin,frameBufferWin.roi_info.startX,frameBufferWin.roi_info.startY)!=0)
    {
        attempts++;
        if (attempts%1024)
        {
            raw_uart_log("mci_SubLcdBlockWrite Access Denied :%d!\r\n", attempts);
			return;
        }
        //UOS_SleepTicks(64);
    }
}

static VOID mci_SubLcdClearScreen(UINT16 background)
{
    struct asrlcdd_roi_info roi;

    raw_uart_log("mci_SubLcdClearScreen %d ++\r\n",background);
    roi.startX=0;
    roi.startY=0;
    roi.width=g_mciSubLcdScreenInfo.width;
    roi.height=g_mciSubLcdScreenInfo.height;

    ASRLCDD_SubFillRect16(&roi, background);
}

static VOID mci_SubLcdSetBrightness(UINT8 level)
{   
	raw_uart_log("mci_SubLcdSetBrightness %d!\r\n", level);
    g_mciSubLcdBrightnessLevel = level;
    ASRLCDD_SetSubBrightness(g_mciSubLcdBrightnessLevel);
}

static VOID mci_SubLcdTurnOffBacklight(void)
{
    raw_uart_log("mci_SubLcdTurnOffBacklight++\r\n");
    ASRLCDD_SetSubBrightness(0);
}

static VOID mci_SubLcdSleep(VOID)
{
    raw_uart_log("mci_SubLcdSleep\n");
	mci_SubLcdTurnOffBacklight();
	ASRLCDD_SubSleep();
	g_mciLcdStatus = MCI_LCD_STATUS_SLEEP;
}

static VOID mci_SubLcdWakeup(VOID)
{
    raw_uart_log("mci_SubLcdWakeup\n");
    ASRLCDD_SubWakeUp();
	mci_SubLcdSetBrightness(g_mciSubLcdBrightnessLevel);
	g_mciLcdStatus = MCI_LCD_STATUS_NORMAL;
}

VOID mci_SetActiveLcd(MCI_LCD_ID_T lcd_id)
{
    raw_uart_log("mci_SetActiveLcd++ (%d ->  %d)\r\n", g_mciActiveLcd, lcd_id);

	if(g_switch_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_TakeMutex(g_switch_lcd_mutex_id);

	if(lcd_id != g_mciActiveLcd){
		g_mciActiveLcd = lcd_id;
		if(MCI_LCD_STATUS_NORMAL == g_mciLcdStatus){
			if(lcd_id == MCI_LCD_ID_MAIN){
				raw_uart_log("mci_SetActiveLcd: switch from sub panel to main panel\r\n");
				mci_SubLcdSetBrightness(0);
				mci_SubLcdSleep();
				mci_MainLcdWakeup();
		        sub_lcd_flag = 0;
				mci_MainLcdSetBrightness(3);
			} else {
				raw_uart_log("mci_SetActiveLcd: switch from main panel to sub panel\r\n");
				mci_MainLcdSetBrightness(0);
				mci_MainLcdSleep();
				mci_SubLcdWakeup();
			    sub_lcd_flag = 1;
				mci_SubLcdSetBrightness(3);
			}
		}
	}

	if(g_switch_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_ReleaseMutex(g_switch_lcd_mutex_id);

	raw_uart_log("mci_SetActiveLcd-- (%d ->  %d)\r\n", g_mciActiveLcd, lcd_id);
}
#endif


// ============================================================================
// mci_LcdInit
// ----------------------------------------------------------------------------
// ============================================================================
VOID mci_LcdInit(UINT32 background ,int panel_is_ready)
{
    static BOOL visitFlag = 0;
	raw_uart_log("mci_LcdInit ++\r\n");

	if (!visitFlag)
    {
    	raw_uart_log("mci_LcdInit\r\n");
        ASRLCDD_Open(panel_is_ready);
        ASRLCDD_GetScreenInfo(&g_mciLcdScreenInfo);
#ifdef LCD_DUAL_PANEL_SUPPORT
		ASRLCDD_GetSubScreenInfo(&g_mciSubLcdScreenInfo);		
		g_switch_lcd_mutex_id = UOS_NewMutex("switch_lcd");
#endif
#if 0
        /* fix URT bug# 82329 */
        if(((background & (~0xffff)) == 0) && (startup_is_silent_reset() == 0))
            mci_LcdClearScreen(background & 0xffff);
#endif
        visitFlag = 1;
		g_mciLcdStatus = MCI_LCD_STATUS_SLEEP;
    }
}

void dump_lcd_framebuffer(void)
{
	static int frame_count = 0;
	char ui_frame[32] = "d:/ui_f";
	INT32 fd;
	raw_uart_log("dump_lcd_framebuffer +++ \r\n");

	raw_uart_log("mci_LcdBlockWrite width = %d , height = %d \r\n",g_mciLcdFBInfo.width,g_mciLcdFBInfo.height);
	sprintf(ui_frame, "d:/ui_f%d.raw", frame_count++);
	fd = FS_Open(ui_frame,	FS_O_CREAT | FS_O_TRUNC | FS_O_WRONLY, 0);
	FS_Write(fd,(UINT8 *)g_asrlcd_framebuffer,g_mciLcdFBInfo.width*g_mciLcdFBInfo.height);
	FS_Close(fd);
	raw_uart_log("dump_lcd_framebuffer --- \r\n");
}

#ifdef ATA_TEST
int mci_LcdInit_ATA(UINT32 background, int panel_is_ready)
{
    static BOOL visitFlag = 0;
    static int ret = 0;
    raw_uart_log("mci_LcdInit_ATA ++\r\n");

    if (!visitFlag)
    {
        raw_uart_log("mci_LcdInit_ATA\r\n");
        ret = ASRLCDD_Open(panel_is_ready);
        ASRLCDD_GetScreenInfo(&g_mciLcdScreenInfo);
#ifdef LCD_DUAL_PANEL_SUPPORT
		ASRLCDD_GetSubScreenInfo(&g_mciSubLcdScreenInfo);
#endif
#if 0
		/* fix URT bug# 82329 */
        if((background & (~0xffff)) == 0)
            mci_LcdClearScreen(background & 0xffff);
#endif
        visitFlag = 1;
		g_mciLcdStatus = MCI_LCD_STATUS_SLEEP;
	}

    return ret;
}
#endif
int mci_Logo_LcdInit(UINT32 background)
{
	int ret = 0;
    static BOOL visitFlag = 0;
	raw_uart_log("mci_Logo_LcdInit ++\r\n");

	if (!visitFlag)
    {
    	raw_uart_log("mci_Logo_LcdInit\r\n");
        ret = ASRLCDD_Logo_Open();
        ASRLCDD_GetScreenInfo(&g_mciLcdScreenInfo);
#ifdef LCD_DUAL_PANEL_SUPPORT
		ASRLCDD_GetSubScreenInfo(&g_mciSubLcdScreenInfo);
#endif
        visitFlag = 1;
		g_mciLcdStatus = MCI_LCD_STATUS_SLEEP;
	}
//	while(1);
	return ret;
}
#ifdef ATA_TEST
int mci_Logo_LcdInit_ATA(UINT32 background)
{
    static BOOL visitFlag = 0;
    static int ret = 0;
    raw_uart_log("mci_Logo_LcdInit_ATA ++\r\n");

    if (!visitFlag)
    {
        raw_uart_log("mci_Logo_LcdInit_ATA\r\n");
        ret = ASRLCDD_Logo_Open();
        ASRLCDD_GetScreenInfo(&g_mciLcdScreenInfo);
#ifdef LCD_DUAL_PANEL_SUPPORT
		ASRLCDD_GetSubScreenInfo(&g_mciSubLcdScreenInfo);
#endif
        visitFlag = 1;
		g_mciLcdStatus = MCI_LCD_STATUS_SLEEP;
    }

    return ret;
}
#endif
VOID mci_ee_LcdInit(UINT32 background)
{
    static BOOL visitFlag = 0;
	raw_uart_log("mci_ee_LcdInit ++\r\n");

	if (!visitFlag)
	{
		raw_uart_log("mci_ee_LcdInit\r\n");
		ASRLCDD_Assert_Open();
		ASRLCDD_GetScreenInfo(&g_mciLcdScreenInfo);
#ifdef LCD_DUAL_PANEL_SUPPORT
		ASRLCDD_GetSubScreenInfo(&g_mciSubLcdScreenInfo);
#endif
		visitFlag = 1;
		g_mciLcdStatus = MCI_LCD_STATUS_SLEEP;
	}
}

VOID mci_LcdUnInit(VOID)
{
#ifdef LCD_DUAL_PANEL_SUPPORT
	UOS_FreeMutex(g_switch_lcd_mutex_id);
	g_switch_lcd_mutex_id = INVALID_MUTEX_ID;
#endif
}

VOID mci_LcdSetFBInfo(int stride, int height)
{
	raw_uart_log("mci_LcdSetFBInfo (%d, %d)\r\n", stride, height);
	g_mciLcdFBInfo.width = stride;
	g_mciLcdFBInfo.height = height;
}

VOID mci_LcdSetFBBitdepth(int bitdepth)
{
	raw_uart_log("mci_LcdSetFBBitdepth (%d)\r\n", bitdepth);
	g_mciLcdFBBitdepth = bitdepth;
}

VOID mci_LcdSetUIInfo(int width, int height)
{
	PRIVATE struct asrlcdd_screen_info ui_info;

	raw_uart_log("mci_LcdSetUIInfo (%d,%d)\r\n", width, height);
	ui_info.width = width;
	ui_info.height = height;
	ASRLCDD_SetUIInfo(&ui_info);
}

// ============================================================================
// mci_LcdPowerOn
// ----------------------------------------------------------------------------
VOID mci_LcdPowerOn(BOOL on)
{
    raw_uart_log("mci_LcdPowerOn %d ++\r\n", on);
    // TODO
    // Currently not implemtented
    // Should use a pmd function
}

// ============================================================================
// mci_LcdScreenOn
// ----------------------------------------------------------------------------
VOID mci_LcdScreenOn(BOOL on)
{
    raw_uart_log("mci_LcdScreenOn (on:%d) ++\r\n", on);
	if(g_switch_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_TakeMutex(g_switch_lcd_mutex_id);

	if(MCI_LCD_ID_MAIN == g_mciActiveLcd){
		mci_MainLcdScreenOn(on);
#ifdef LCD_DUAL_PANEL_SUPPORT
	} else {
		mci_SubLcdScreenOn(on);
#endif
	}

	if(g_switch_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_ReleaseMutex(g_switch_lcd_mutex_id);
}

// ============================================================================
// mci_LcdBlockWrite
// ----------------------------------------------------------------------------
VOID mci_LcdBlockWrite(UINT16* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy)
{
//    raw_uart_log("mci_LcdBlockWrite (%d, %d, %d, %d)++\r\n", startx, starty, endx, endy);

	if(g_switch_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_TakeMutex(g_switch_lcd_mutex_id);

	if(MCI_LCD_ID_MAIN == g_mciActiveLcd){
		mci_MainLcdBlockWrite(buffer, startx, starty, endx, endy);
#ifdef LCD_DUAL_PANEL_SUPPORT
	} else {
		mci_SubLcdBlockWrite(buffer, startx, starty, endx, endy);
#endif
	}

	if(g_switch_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_ReleaseMutex(g_switch_lcd_mutex_id);
}

// ============================================================================
// mci_LcdBlockWrite_sync
// ----------------------------------------------------------------------------
VOID mci_LcdBlockWrite_sync(UINT16* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy)
{
	struct asrlcdd_framebuffer_window frameBufferWin;
	UINT32 attempts = 0;
	int ret;

	raw_uart_log("mci_LcdBlockWrite_sync ++\r\n");

	ret = to_framewindow(buffer, startx, starty, endx, endy, &frameBufferWin, __func__, MCI_LCD_ID_MAIN);
	if(0 != ret)
		return;
	
    while(ASRLCDD_Blit16_sync(&frameBufferWin,frameBufferWin.roi_info.startX,frameBufferWin.roi_info.startY)!=0)
    {
        attempts++;
        if (attempts%1024)
        {
            raw_uart_log("mci_LcdBlockWrite_sync Access Denied :%d!\r\n", attempts);
			return;
        }
        //UOS_SleepTicks(64);
    }
}

VOID mci_Logo_LcdBlockWrite_sync(UINT8* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy)
{
	struct asrlcdd_framebuffer_window frameBufferWin;
	UINT32 attempts = 0;
	int ret;

	//raw_uart_log("mci_Logo_LcdBlockWrite_sync ++\r\n");

	ret = to_framewindow(buffer, startx, starty, endx, endy, &frameBufferWin, __func__, MCI_LCD_ID_MAIN);
	if(0 != ret)
		return;
	
    while(ASRLCDD_Logo_Blit16_sync(&frameBufferWin,frameBufferWin.roi_info.startX,frameBufferWin.roi_info.startY)!=0)
    {
        attempts++;
        if (attempts%1024)
        {
            raw_uart_log("mci_LcdBlockWrite_sync Access Denied :%d!\r\n", attempts);
			return;
        }
        //UOS_SleepTicks(64);
    }
}

VOID mci_ee_LcdBlockWrite_sync(UINT8* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy)
{
	struct asrlcdd_framebuffer_window frameBufferWin;
	UINT32 attempts = 0;
	int ret;

	//raw_uart_log("mci_ee_LcdBlockWrite_sync ++\r\n");

	ret = to_framewindow(buffer, startx, starty, endx, endy, &frameBufferWin, __func__, MCI_LCD_ID_MAIN);
	if(0 != ret)
		return;
	
    while(ASRLCDD_Assert_Blit16_sync(&frameBufferWin,frameBufferWin.roi_info.startX,frameBufferWin.roi_info.startY)!=0)
    {
        attempts++;
        if (attempts%1024)
        {
            raw_uart_log("mci_LcdBlockWrite_sync Access Denied :%d!\r\n", attempts);
			return;
        }
        //UOS_SleepTicks(64);
    }
}

// ============================================================================
// mci_LcdGetDimension
// ----------------------------------------------------------------------------
VOID mci_LcdGetDimension(UINT16 *out_LCD_width,UINT16 *out_LCD_height)
{
    raw_uart_log("mci_LcdGetDimension ++");
    *out_LCD_width=g_mciLcdScreenInfo.width;
    *out_LCD_height=g_mciLcdScreenInfo.height;
}

// ============================================================================
// mci_LcdSleep
// ----------------------------------------------------------------------------
VOID mci_LcdSleep(VOID)
{
	raw_uart_log("mci_LcdSleep ++\r\n");

	if(g_switch_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_TakeMutex(g_switch_lcd_mutex_id);

	if(MCI_LCD_ID_MAIN == g_mciActiveLcd){
		mci_MainLcdSleep();
#ifdef LCD_DUAL_PANEL_SUPPORT
	} else {
		mci_SubLcdSleep();
#endif
	}

	if(g_switch_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_ReleaseMutex(g_switch_lcd_mutex_id);
}

// ============================================================================
// mci_LcdWakeup
// ----------------------------------------------------------------------------
VOID mci_LcdWakeup(VOID)
{
	raw_uart_log("mci_LcdWakeup ++\r\n");
	if(g_switch_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_TakeMutex(g_switch_lcd_mutex_id);

	if(MCI_LCD_ID_MAIN == g_mciActiveLcd){
		mci_MainLcdWakeup();
#ifdef LCD_DUAL_PANEL_SUPPORT
	} else {
		mci_SubLcdWakeup();
#endif
	}

	if(g_switch_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_ReleaseMutex(g_switch_lcd_mutex_id);
}

// ============================================================================
// mci_LcdClearScreen
// ----------------------------------------------------------------------------
VOID mci_LcdClearScreen(UINT16 background)
{
    raw_uart_log("mci_LcdClearScreen %d ++\r\n",background);
	if(g_switch_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_TakeMutex(g_switch_lcd_mutex_id);

	if(MCI_LCD_ID_MAIN == g_mciActiveLcd){
		mci_MainLcdClearScreen(background);
#ifdef LCD_DUAL_PANEL_SUPPORT
	} else {
		mci_SubLcdClearScreen(background);
#endif
	}

	if(g_switch_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_ReleaseMutex(g_switch_lcd_mutex_id);
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
            return g_mciLcdBrightnessLevel;
            break;
        default:
            break;
    }
    return 0;
}

// ============================================================================
// mci_LcdSetBrightness
// ----------------------------------------------------------------------------
VOID mci_LcdSetBrightness(UINT8 level)
{   
    raw_uart_log("mci_LcdSetBrightness %d++\r\n", level);

	if(g_switch_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_TakeMutex(g_switch_lcd_mutex_id);

	if(MCI_LCD_ID_MAIN == g_mciActiveLcd){
		mci_MainLcdSetBrightness(level);
#ifdef LCD_DUAL_PANEL_SUPPORT
	} else {
		mci_SubLcdSetBrightness(level);
#endif
	}

	if(g_switch_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_ReleaseMutex(g_switch_lcd_mutex_id);
}

VOID mci_LcdSetBrightnessExt(UINT8 level)
{   
    raw_uart_log("mci_LcdSetBrightnessExt %d++\r\n", level);
	if(g_switch_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_TakeMutex(g_switch_lcd_mutex_id);

	if(MCI_LCD_ID_MAIN == g_mciActiveLcd){
		mci_MainLcdSetBrightnessExt(level);
#ifdef LCD_DUAL_PANEL_SUPPORT
	} else {
		mci_SubLcdSetBrightness(level);
#endif
	}

	if(g_switch_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_ReleaseMutex(g_switch_lcd_mutex_id);
}

VOID mci_LcdTurnOffBacklight(void)
{
    raw_uart_log("mci_LcdTurnOffBacklight++\r\n");
	if(g_switch_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_TakeMutex(g_switch_lcd_mutex_id);

	if(MCI_LCD_ID_MAIN == g_mciActiveLcd){
		mci_MainLcdTurnOffBacklight();
#ifdef LCD_DUAL_PANEL_SUPPORT
	} else {
		mci_SubLcdSetBrightness(0);
#endif
	}

	if(g_switch_lcd_mutex_id != INVALID_MUTEX_ID)
		UOS_ReleaseMutex(g_switch_lcd_mutex_id);

}

// ============================================================================
// mci_LcdSetContrast
// ----------------------------------------------------------------------------
VOID mci_LcdSetContrast(UINT8 contrast)
{
    raw_uart_log("mci_LcdSetContrast %d++\r\n",contrast);
}

BOOL mci_LcdIsActive(void)
{
    return ASRLCDD_LcdIsActive();
}

VOID mci_Read_Panel_ID(void)
{
	ASRLCDD_Read_Panel_ID();
}
