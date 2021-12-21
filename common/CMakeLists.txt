fp_include_directories(
  inc
)
fp_sources(
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
