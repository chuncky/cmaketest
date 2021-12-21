/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/

#ifndef _MCI_LCD_H_
#define _MCI_LCD_H_

#include "uhaltype.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GUI_LCD_MUTEX

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// 
/// @defgroup mciLcdApi MCI LCD Screen
/// @{
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// ============================================================================
// MCI_LCD_ID_T
// ----------------------------------------------------------------------------
/// Enum lcd id
// ============================================================================
typedef enum {
	MCI_LCD_ID_MAIN,
	MCI_LCD_ID_SUB,
}MCI_LCD_ID_T;

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

// ============================================================================
// Functions
// ============================================================================

void dump_lcd_framebuffer(void);

// ============================================================================
// mci_LcdInit
// ----------------------------------------------------------------------------
/// Initialize LCM and fill the LCD screen with the background color
/// @param background : color to use to fill the screen
/// @return none
// ============================================================================
VOID mci_LcdInit(UINT32 background ,int panel_is_ready);
int mci_Logo_LcdInit(UINT32 background);
VOID mci_ee_LcdInit(UINT32 background);

// ============================================================================
// mci_LcdPowerOn
// ----------------------------------------------------------------------------
/// Turns the LCM power on and off<br/>
/// @param on : TRUE to turn on the LCM power, FALSE to turn if off
/// @return none
// ============================================================================
VOID mci_LcdPowerOn(BOOL on);

// ============================================================================
// mci_LcdScreenOn
// ----------------------------------------------------------------------------
/// Turns the LCD screen on and off<br/>
/// @param on : TRUE to turn on the screen, FALSE to turn it off
/// @return none
// ============================================================================
VOID mci_LcdScreenOn(BOOL on);

// ============================================================================
// mci_LcdSetFBInfo
// ----------------------------------------------------------------------------
/// set framebuffer information<br/>
/// @param stride : the stride of framebuffer, unit is byte.
/// @param hight : the height of framebuffer, unit is line.
/// @return none
// ============================================================================
VOID mci_LcdSetFBInfo(int stride, int height);

// ============================================================================
// mci_LcdSetFBBitdepth
// ----------------------------------------------------------------------------
/// set framebuffer bitdepth<br/>
/// @param bitdepth : the bitdepth of framebuffer, unit is bit.
/// @return none
// ============================================================================
VOID mci_LcdSetFBBitdepth(int bitdepth);

// ============================================================================
// mci_LcdSetUIInfo
// ----------------------------------------------------------------------------
/// set ui information<br/>
/// @param width : the width of ui, unit is pixel.
/// @param hight : the height of ui, unit is line.
/// @return none
// ============================================================================
VOID mci_LcdSetUIInfo(int width, int height);

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
// mci_LcdBlockWrite
// ----------------------------------------------------------------------------
/// Copy the selected area of the frame buffer to the LCD<br/>
/// @param  startx : top left corner x value
/// @param  starty : top left corner y value
/// @param  endx : bottom right corner x value
/// @param  endy : bottom right corner y value
/// @return none
// ============================================================================
VOID mci_LcdBlockWrite_sync(UINT16* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy);
VOID mci_Logo_LcdBlockWrite_sync(UINT8* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy);
VOID mci_ee_LcdBlockWrite_sync(UINT8* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy);

// ============================================================================
// mci_LcdGetDimension
// ----------------------------------------------------------------------------
/// Returns the width and height of the LCD module
/// @param out_LCD_width : lcd screen width
/// @param out_LCD_height : lcd screen height
/// @return none
// ============================================================================
VOID mci_LcdGetDimension(UINT16 *out_LCD_width,UINT16 *out_LCD_height);

// ============================================================================
// mci_LcdSleep
// ----------------------------------------------------------------------------
/// Set the LCM in sleep mode
/// @return none
// ============================================================================
VOID mci_LcdSleep(VOID);

// ============================================================================
// mci_LcdWakeup
// ----------------------------------------------------------------------------
/// Wake up the LCM
/// @return none
// ============================================================================
VOID mci_LcdWakeup(VOID);

// ============================================================================
// mci_LcdClearScreen
// ----------------------------------------------------------------------------
/// Fill the screen with the selected color
/// @param background : Color to use to fill the buffer in 565 RGB format
/// @return none
// ============================================================================
VOID mci_LcdClearScreen(UINT16 background);

// ============================================================================
// mci_LcdSetBrightness
// ----------------------------------------------------------------------------
/// Set brightness level
/// @param level : brightness level 0 (dim) - 7 (bright)
/// @return none
// ============================================================================
VOID mci_LcdSetBrightness(UINT8 level);

// ============================================================================
// mci_LcdSetContrast
// ----------------------------------------------------------------------------
/// set Contrast value<br/>
/// @param contrast : contrast value
/// @return none
// ============================================================================
VOID mci_LcdSetContrast(UINT8 contrast);

BOOL mci_LcdIsActive(void);

// ============================================================================
// mci_Read_Panel_ID
// ----------------------------------------------------------------------------
/// read panel id
/// @ASRLCDD_Open must be called before called mci_Read_Panel_ID;
// ============================================================================

VOID mci_Read_Panel_ID(void);

#ifdef LCD_DUAL_PANEL_SUPPORT
VOID mci_SubLcdSetFBInfo(int stride, int height);
VOID mci_SubLcdSetFBBitdepth(int bitdepth);
VOID mci_SetActiveLcd(MCI_LCD_ID_T lcd_id);
#endif
/// @}

#ifdef __cplusplus
}
#endif

#endif //_MCI_LCD_H_

