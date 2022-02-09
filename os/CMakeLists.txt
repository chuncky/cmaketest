fp_open_library()

fp_library_include_directories(
    "${FP_BASE}/bootloader/inc"
	"${FP_BASE}/common/inc"
    "${FP_BASE}/driver/lcd/inc"
	"${FP_BASE}/os/inc"
)

fp_open_library_sources(
	os_hal.c 
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
