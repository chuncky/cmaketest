fp_open_library()

fp_library_include_directories(
    "${FP_BASE}/bootloader/inc"
	"${FP_BASE}/common/inc"
    "${FP_BASE}/driver/lcd/inc"
)

fp_open_library_sources(
	bsp_common.c
	hal_board.c
	hal_disp.c
	halsoc_lcd_spi.c
	halsoc_lcdc.c
)

if (DEFINED CONFIG_LCD_MIPI_PANEL)

	fp_open_library_sources(
		halsoc_lcd_mipi.c
		halsoc_lcd_mipi_dphy.c
		halsoc_lcd_mipi_dsi.c
	)

if (DEFINED CONFIG_LCD_RM69330_MIPI)
    fp_open_library_sources(rm69330_mipi.c)
    fp_compile_definitions("USE_LCD_PANEL_RM69330_MIPI" "USE_CRANE_LCD_MIPI")
endif()

if (DEFINED CONFIG_LCD_ST7796S_MIPI)
    fp_open_library_sources(st7796s_mipi.c)
    fp_compile_definitions("USE_LCD_PANEL_ST7796S_MIPI" "USE_CRANE_LCD_MIPI")
endif()
endif()

if (DEFINED CONIFG_LCD_PANEL_ST7789V_SPI_PHONE)
    fp_open_library_sources(st7789v_spi.c)
    fp_compile_definitions("USE_LCD_PANEL_ST7789V_SPI_PHONE" "LCD_SPI")
endif()



