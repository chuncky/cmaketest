
if (0)
fp_sources(
  ui_os_entry.c
  hal_task.c
  task_init.c
)
endif()

fp_open_library()

fp_library_include_directories(
    "${FP_BASE}/bootloader/inc"
)

fp_open_library_sources(
	loadtable.c 
	ptable.c 
	qspi_core.c 
	qspi_dma.c 
	qspi_host.c 
	qspi_nor.c 
	uart.c 
	version_block.c
	
)
