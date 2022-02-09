fp_open_library()

fp_library_include_directories(
    "${FP_BASE}/bootloader/inc"
	"${FP_BASE}/common/inc"
    "${FP_BASE}/driver/lcd/inc"
	"${FP_BASE}/os/inc"
	"${FP_BASE}/driver/usb"
	"${FP_BASE}/driver/usb/inc"
)

fp_open_library_sources(
	ssp_host.c 
	asr_nor_flash.c 
	spi_nor.c 
	minilzo.c 
	FreqChange.c 
	LzmaDec.c 
	system.c 
	tinyalloc.c
	asr_property.c 
	bsp_tavor.c 
	updater_table.c 
	logo_table.c 
	asr_lzma.c
	pmic.c
	guilin.c
	ningbo.c 
	guilin_lite.c
	intc_xirq.c
	gpio.c
	utilities.c
	commpm.c
	sdcard.c
	ff.c
	diskio.c
	timer.c
	tick_manager.c
	
)

if (0)
if (DEFINED CONFIG_MINIPHONE)
fp_sources(
	nvram_user_config_miniphone.c
)
else()
fp_sources(
  	nvram_user_config.c
)
endif()
endif()
