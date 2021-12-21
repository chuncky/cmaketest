TARGET_	= updater

#Default: DS-5 is added in Patch
CC 	    = armcc.exe
LINK 	= armlink.exe
FROMELF	= fromelf.exe

src_dir = ../src
inc_dir = ../inc
scatter_dir = ..
lib_dir = ../lib

obj_dir = obj
dir_external_append_pad_crc = ..

#common start
common_src_dir = ../../common/src
common_inc_dir = ../../common/inc
obj_common_dir = obj_common
#common end

cflag=-I$(inc_dir) --cpu Cortex-R4  --no_unaligned_access -g -O2 --apcs /inter --gnu --thumb -D__MICROLIB  --feedback=feedback --split_sections
#common start
cflag += -I$(common_inc_dir)
#common end

#*************************SCATTER_FILE*****************
ifneq (,$(findstring ASRSPI,${FOTA_TYPE}))
SCATTER_FILE = $(scatter_dir)/scatter_ota_spi.scat
else
SCATTER_FILE = $(scatter_dir)/scatter.scat
endif
#*****************************************************

#*************************SMALL_CODE*****************
ifneq (,$(findstring UPDATER,${SMALL_CODE}))
	cflag += -DSMALL_CODE_UPDATER
endif
#*****************************************************

#
#FUNCTION_MACRO
#NOTE:   Default : NOT DEFINED
#        MACRO-1: SIMU_128_160_ON_LCD240x320 --- simulate 128x240 in 240x320 LCD
#
#cflag += -DSIMU_128_160_ON_LCD240x320



#********************** LCD_TYPE *********************
#1. LCD_ADAPT
ifneq (,$(findstring LCD_ADAPT,${LCD_TYPE}))
	cflag += -DLCD_USE_IN_CODE -DLCD_ADAPT
	cflag += -DLCD_ADAPT_PANEL
	lcd_type = _lcd_adapt
endif

#2. WATCHLCDST7789
ifneq (,$(findstring WATCHLCDST7789,${LCD_TYPE}))
	cflag += -DLCD_USE_IN_CODE -DWATCHLCDST7789_CODE_USE
	lcd_type = _watchlcdst7789
endif

#3. ST7789VER1
ifneq (,$(findstring ST7789VER1,${LCD_TYPE}))
	cflag += -DLCD_USE_IN_CODE -DLCD_ST7789VER1
	lcd_type = _st7789ver1
endif

#4. GC9306VER1
ifneq (,$(findstring GC9306VER1,${LCD_TYPE}))
	cflag += -DLCD_USE_IN_CODE -DLCD_GC9306VER1
	lcd_type = _gc9306ver1
endif

#5. GC9306VER2
ifneq (,$(findstring GC9306VER2,${LCD_TYPE}))
	cflag += -DLCD_USE_IN_CODE -DLCD_GC9306VER2
	lcd_type = _gc9306ver2
endif

#6. ST7789VER2
ifneq (,$(findstring ST7789VER2,${LCD_TYPE}))
	cflag += -DLCD_USE_IN_CODE -DLCD_ST7789VER2
	lcd_type = _st7789ver2
endif

#6. NO_LCD
ifneq (,$(findstring NO_LCD,${LCD_TYPE}))
	cflag +=
	lcd_type = _no_lcd
endif

#********************** FOTA_SOLUTION *********************
#1. ASRSD
ifneq (,$(findstring ASRSD,${FOTA_TYPE}))
cflag += -DFOTA_ASRSD
fota_type = _asrsd
endif

#2. ADUPS
ifneq (,$(findstring ADUPS,${FOTA_TYPE}))
cflag += -DFOTA_ADUPS
fota_type = _adups
libs = $(lib_dir)/abup_wosun.lib
endif

#3. REDSTONE
#ifneq (,$(findstring REDSTONE,${FOTA_TYPE}))
#cflag += -DFOTA_REDSTONE
#fota_type = _redstone
#libs = $(lib_dir)/librsua.a $(lib_dir)/librsuasdk.a
#endif

#4. REDSTONE_AND_ALI
ifneq (,$(findstring REDSTONE_AND_ALI,${FOTA_TYPE}))
cflag += -DFOTA_RED_ALI
fota_type = _redstone_and_ali
##REDSTONE's lib
libs += $(lib_dir)/librsua.a $(lib_dir)/librsuasdk.a
##ALI's lib
libs += $(lib_dir)/ota_updater.a
endif

#5. ASRSPI
ifneq (,$(findstring ASRSPI,${FOTA_TYPE}))
cflag += -DFOTA_ASRSPI
fota_type = _asrspi
endif

#6. ASRDM
ifneq (,$(findstring ASRDM,${FOTA_TYPE}))
cflag += -DFOTA_ASRDM
fota_type = _asrdm
endif



TARGET = ${TARGET_}${lcd_type}${fota_type}


srcs = $(src_dir)/main.c \
   $(src_dir)/qspi_core.c \
   $(src_dir)/qspi_dma.c \
   $(src_dir)/qspi_host.c \
   $(src_dir)/qspi_nor.c \
   $(src_dir)/system.c \
   $(src_dir)/uart.c \
   $(src_dir)/loadtable.c \
   $(src_dir)/ptable.c \
   $(src_dir)/pmic.c \
   $(src_dir)/ningbo.c \
   $(src_dir)/guilin.c \
   $(src_dir)/version_block.c \
   $(src_dir)/keypad_drv.c
   #$(src_dir)/sdcard.c \
   #$(src_dir)/diskio.c \
   #$(src_dir)/ff.c \
   #$(src_dir)/ff_dump_mem.c \
   #$(src_dir)/ffsystem.c \
   #$(src_dir)/ffunicode.c  

#common start
#add ../common/src
#Note: create a new MACRO: "srcs_common"
srcs_common = $(common_src_dir)/lcd_backlight_drv.c \
		$(common_src_dir)/lcd_drv.c \
		$(common_src_dir)/lcd_fns.c \
		$(common_src_dir)/lcd_gif_decoder.c \
		$(common_src_dir)/lcd_hal.c \
		$(common_src_dir)/lcd_panel_drv.c \
		$(common_src_dir)/lcd_spi_drv.c	\
		$(common_src_dir)/lcd_st7789v_spi.c \
		$(common_src_dir)/lcd_gc9306_spi.c \
		$(common_src_dir)/lcd_test.c  \
		$(common_src_dir)/lcdd_asr.c \
		$(common_src_dir)/mci_lcd_asr.c \
		$(common_src_dir)/media_clk.c  \
		$(common_src_dir)/panel_list.c \
		$(common_src_dir)/ssp_host.c \
   		$(common_src_dir)/asr_nor_flash.c \
		$(common_src_dir)/spi_nor.c \
		$(common_src_dir)/lcd_var.c \
		$(common_src_dir)/bsp_tavor.c

#add source *.c according to MACRO

#--------------------------------ADD LCD Files according MACRO-------------------------------------
#4. GC9306VER1  
ifneq (,$(findstring GC9306VER1,${LCD_TYPE}))	#add lcd *.c files
srcs_common += $(common_src_dir)/lcd_gc9306_spi_1_lane.c
endif

#5. GC9306VER2 
ifneq (,$(findstring GC9306VER2,${LCD_TYPE}))	#add lcd *.c files
srcs_common += $(common_src_dir)/lcd_gc9306_spi_4line_1data.c
endif
#common end 

#--------------------------------ADD FOTA files according MACRO-------------------------------------
#1. ASRSD
ifneq (,$(findstring ASRSD,${FOTA_TYPE}))
srcs += $(src_dir)/diskio.c \
	$(src_dir)/ff.c \
	$(src_dir)/ff_dump_mem.c \
	$(src_dir)/ffsystem.c \
	$(src_dir)/ffunicode.c \
	$(src_dir)/sdcard.c \
	$(src_dir)/OTA_sdcard.c
endif

#2. ADUPS
ifneq (,$(findstring ADUPS,${FOTA_TYPE}))
srcs += $(src_dir)/adups_bl_flash.c $(src_dir)/adups_bl_main.c \
   $(src_dir)/sdcard.c \
   $(src_dir)/diskio.c \
   $(src_dir)/ff.c \
   $(src_dir)/ff_dump_mem.c \
   $(src_dir)/ffsystem.c \
   $(src_dir)/ffunicode.c  
endif

#3. REDSTONE
ifneq (,$(findstring REDSTONE,${FOTA_TYPE}))
srcs +=  $(src_dir)/rs_ua_flash.c $(src_dir)/rs_ua_fs.c $(src_dir)/rs_ua_porting.c 
endif

#4. REDSTONE_AND_ALI
ifneq (,$(findstring REDSTONE_AND_ALI,${FOTA_TYPE}))
##REDSTONE
srcs +=  $(src_dir)/rs_ua_flash.c $(src_dir)/rs_ua_fs.c $(src_dir)/rs_ua_porting.c
##ALI
srcs +=  $(src_dir)/demo.c $(src_dir)/flash.c $(src_dir)/partitions.c
endif

#5. ASRSPI
ifneq (,$(findstring ASRSPI,${FOTA_TYPE}))
srcs_common += $(common_src_dir)/ssp.c

srcs_common += $(common_src_dir)/minilzo.c \
			   $(common_src_dir)/lzop.c \
			   $(common_src_dir)/lzop_buf.c

srcs_common += $(common_src_dir)/LzmaDec.c \
			   $(common_src_dir)/tinyalloc.c \
			   $(common_src_dir)/FreqChange.c
		       
srcs        += $(src_dir)/ota_spi.c
##Note: if add debug func:ota_spi_d_print_flash_iamge_to_sd, need add the following files related sd card file system.
#srcs += $(src_dir)/diskio.c \
#		$(src_dir)/ff.c \
#		$(src_dir)/ff_dump_mem.c \
#		$(src_dir)/ffsystem.c \
#		$(src_dir)/ffunicode.c \
#		$(src_dir)/sdcard.c
endif

#6. ASRDM
ifneq (,$(findstring ASRDM,${FOTA_TYPE}))
	       
srcs        += 	$(src_dir)/bspatch.c   \
				$(src_dir)/tinyalloc.c  \
				$(src_dir)/LzmaDecode.c
endif



dep_files = $(wildcard $(inc_dir)/*.h)
objs = $(patsubst %.c, ${obj_dir}/%.o, ${notdir ${srcs}})

#common start
dep_files += $(wildcard $(common_inc_dir)/*.h)
objs_common = $(patsubst %.c, ${obj_common_dir}/%.o, ${notdir ${srcs_common}})
#else
#objs_common =
#common end

objs_sum = $(objs)

#common start
objs_sum += $(objs_common)
#common end

${TARGET}.axf:$(objs_sum)
	$(CC) -I$(inc_dir) --cpu Cortex-R4  --no_unaligned_access -g -O2 --apcs /inter --gnu -c $(src_dir)/StartUp.s -o $(obj_dir)/StartUp.o
	$(LINK) $(objs_sum) $(obj_dir)/StartUp.o ${libs} --scatter $(SCATTER_FILE) --library_type=microlib --library_type=standardlib --output $@ --map --symbols --info sizes,totals --list ${TARGET}.map --keep version_block.o(IMGVERBLOCK)
	$(FROMELF) --output ${TARGET}.bin --bin ${TARGET}.axf
	perl $(dir_external_append_pad_crc)/external_append_pad_crc.pl ${TARGET}.bin

#1. build srcs\*.c ---> $(obj_dir)/*.o
${obj_dir}/%.o:$(src_dir)/%.c $(dep_files)
	if not exist $(obj_dir) mkdir $(obj_dir)
	$(CC) $(cflag) -c $< -o $@

#common start
#2. build $(common_src_dir)/*.c ---> $(obj_common_dir)/*.o
${obj_common_dir}/%.o: $(common_src_dir)/%.c $(dep_files)
	if not exist "$(obj_common_dir)" mkdir $(obj_common_dir)
	$(CC) $(cflag) -c $< -o $@
#common end
clean:
	del updater*.bin
	del updater*.map
	del updater*.axf
	del buildlog.txt
	rd /s /q  $(obj_dir)
	rd /s /q $(obj_common_dir)
