/*
** @file mci_lcd.h
**     
*/

#ifndef _MCI_LCD_H_
#define _MCI_LCD_H_

#include "lcd_predefine.h"
#ifdef __cplusplus
extern "C" {
#endif

#define LCD_TEST_ENABLE

// ============================================================================
// lcd_func_type
// ----------------------------------------------------------------------------
/// Enum describing the ID of the various parameters related to LCD
// ============================================================================
typedef enum {
    MCI_LCD_BIAS,
    MCI_LCD_CONTRAST,
    MCI_LCD_LINE_RATE,
    MCI_LCD_TEMP_COMP,
    MCI_LCD_BRIGHTNESS,
    MCI_LCD_PARAM_NUM
} MCI_LCD_PARAM_T;


typedef enum{
	ASR_PANEL_MODE_MCU = 0,
	ASR_PANEL_MODE_SPI = 1,
	ASR_PANEL_MODE_MAX
}ASR_PANEL_MODE;

typedef enum{
	ASR_PANEL_COLOR_FORMAT_RGB565,
	ASR_PANEL_COLOR_FORMAT_RGB1555,
	ASR_PANEL_COLOR_FORMAT_RGB888_PACKED,
	ASR_PANEL_COLOR_FORMAT_RGB888_UNPACKED,
	ASR_PANEL_COLOR_FORMAT_RGBA888, /*ARGB*/
	ASR_PANEL_COLOR_FORMAT_YUV422_PACKED,
	ASR_PANEL_COLOR_FORMAT_YUV422_PLANAR,
	ASR_PANEL_COLOR_FORMAT_YUV420_PLANAR,
	ASR_PANEL_COLOR_FORMAT_RGB888A,/*RGBA*/
	ASR_PANEL_COLOR_FORMAT_YUV420_SEMI,
	ASR_PANEL_COLOR_FORMAT_MAX
}ASR_PANEL_COLOR_FORMAT;



typedef struct ASR_LCD_INFO
{
	unsigned short screen_width;  //short
	unsigned short screen_height; // short
	unsigned short fb_width;  //short
	unsigned short fb_height; // short
	ASR_PANEL_COLOR_FORMAT format;
	ASR_PANEL_MODE mode;
	unsigned char bitsPerPixel; // short
	unsigned char backlightLevel; // short
}MCI_ASR_LCD_INFO;


// ============================================================================
// MCI_LCD_BPFUN_T
// ----------------------------------------------------------------------------
/// Type of the bypass function
/// buffer : buffer accessed by the BlockWrite_DI function
/// startx, starty, endx, endy defines the area refreshed by the BlockWrite_DI
// ============================================================================
typedef VOID (*MCI_LCD_BPFUN_T)(UINT16* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy);
// ============================================================================
// Functions
// ============================================================================

// ============================================================================
// mci_LcdInit
// ----------------------------------------------------------------------------
/// Initialize LCM and fill the LCD screen with the background color
/// @param background : color to use to fill the screen
/// @return none
// ============================================================================
VOID mci_LcdInit(UINT32 background);


// ============================================================================
// mci_Logo_LcdInit
// ----------------------------------------------------------------------------
/// Initialize LCM for logo info display, mainly used to init to display system assert information
/// @return none
// ============================================================================
VOID mci_Logo_LcdInit(void);


// ============================================================================
// mci_Logo_LcdClose
// ----------------------------------------------------------------------------
/// un-initialize panel and  power off LCD, please call the API before mci_Logo_LcdInit
/// @return none
// ============================================================================
VOID mci_Logo_LcdClose(void);


// ============================================================================
// mci_LcdSetFBInfo
// ----------------------------------------------------------------------------
/// Set the stride and height of LCD frame buffer
/// @param stride : stride of frame buffer
/// @param height : height of frame buffer 
/// @return none
// ============================================================================
VOID mci_LcdSetFBInfo(int stride, int height);


// ============================================================================
// mci_LcdScreenOn
// ----------------------------------------------------------------------------
/// Turns the LCD screen on and off<br/>
/// @param on : TRUE to turn on the screen, FALSE to turn it off
/// @return none
// ============================================================================
VOID mci_LcdScreenOn(BOOL on);

// ============================================================================
// mci_LcdBlockWrite
// ----------------------------------------------------------------------------
/// Copy the selected area of the frame buffer to the LCD<br/>
/// @param  startx : top left corner x value
/// @param  starty : top left corner y value
/// @param  endx : bottom right corner x value
/// @param  endy : bottom right corner y value
/// @return none
// ============================================================================
VOID mci_LcdBlockWrite(UINT16* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy);

// ============================================================================
// mci_LcdBlockWrite_sync
// ----------------------------------------------------------------------------
/// Copy the selected area of the frame buffer to the LCD directly<br/>
/// @param  startx : top left corner x value
/// @param  starty : top left corner y value
/// @param  endx : bottom right corner x value
/// @param  endy : bottom right corner y value
/// @return none
// ============================================================================
VOID mci_LcdBlockWrite_sync(UINT16* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy);

// ============================================================================
// mci_LcdBlockWrite_sync
// ----------------------------------------------------------------------------
/// Copy the selected area of the frame buffer to the LCD directly, used to display system assert information<br/>
/// @param  startx : top left corner x value
/// @param  starty : top left corner y value
/// @param  endx : bottom right corner x value
/// @param  endy : bottom right corner y value
/// @return none
// ============================================================================
VOID mci_Logo_LcdBlockWrite_sync(UINT16* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy);

// ============================================================================
// mci_LcdGetDimension
// ----------------------------------------------------------------------------
/// Returns the width and height of the LCD module
/// @param out_LCD_width : lcd screen width
/// @param out_LCD_height : lcd screen height
/// @return none
// ============================================================================
VOID mci_LcdGetDimension(UINT16 *out_LCD_width,UINT16 *out_LCD_height);
#if 0
// ============================================================================
// mci_LcdClearScreen
// ----------------------------------------------------------------------------
/// Fill the screen with the selected color
/// @param background : Color to use to fill the buffer in 565 RGB format
/// @return none
// ============================================================================
VOID mci_LcdClearScreen(UINT16 background);
#endif
// ============================================================================
// mci_LcdSetBrightness
// ----------------------------------------------------------------------------
/// Set brightness level
/// @param level : brightness level 0 (dim) - 7 (bright)
/// @return none
// ============================================================================
VOID mci_LcdSetBrightness(UINT8 level);
void mci_LcdGetPanelInfo(MCI_ASR_LCD_INFO *panel_info);


#ifdef __cplusplus
}
#endif

#endif //_MCI_LCD_H_

