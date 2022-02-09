fp_open_library()

fp_library_include_directories(
    "${FP_BASE}/bootloader/inc"
	"${FP_BASE}/common/inc"
    "${FP_BASE}/driver/usb/inc"
	"${FP_BASE}/driver/usb"
	"${FP_BASE}/os/inc"
)

fp_open_library_sources(
	udc_driver.c
	usb_descriptor.c
	usb_device.c
	usb_init.c
	usb1_device.c
	usb2_device.c
	mvUsbDevCh9.c
	mvUsbDevMain.c
	mvUsbDevRecv.c
	mvUsbDevSend.c
	mvUsbDevUtl.c
	mvUsbHsDevCncl.c
	mvUsbHsDevMain.c
	mvUsbHsDevUtl.c
	mvUsbLog.c
	mvUsbMemory.c
	mvUsbModem.c
	mvUsbNet.c
	mvUsbStorage.c
	UsbMgr.c
	UsbMgr_util.c
	rndis.c
	rndis_buff.c
	netif_pc.c
	
)
if (0)
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

endif()

