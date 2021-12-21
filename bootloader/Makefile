TARGET_	= boot33

#Default: DS-5 is added in Patch
CC 	    = armcc.exe
LINK 	= armlink.exe
FROMELF	= fromelf.exe

src_dir = src
inc_dir = inc
obj_dir = obj
feedback_file = feedback.txt
#common start
src_common_dir = ../common/src
inc_common_dir = ../common/inc
obj_common_dir = obj_common
#common end
cflag=-I$(inc_dir) --cpu Cortex-R5  --no_unaligned_access -g -O2 --apcs /inter --gnu --feedback $(feedback_file)
#common start
cflag += -I$(inc_common_dir)
#common end

############### Define Macro in Boot33 ########################
#Macro used in boot33 can add variable: "DEFS"

#1. Macro Used if CP is Erased
DEFS = -DCP_ERASE

#2. Macro Used to check ONKEY
#DEFS += -DONKEY_CHECK

#3. Macro Used to support compressed logo bin
DEFS += -DSUPPORT_COMPRESSED_LOGO

#Last, append variable DEFS into cflag
cflag += $(DEFS)
#####################################################


#Boot33 Naming Rule
TARGET = ${TARGET_}${lcd_type}

srcs = $(src_dir)/loadtable.c \
	$(src_dir)/main.c \
	$(src_dir)/ptable.c \
	$(src_dir)/qspi_core.c \
	$(src_dir)/qspi_dma.c \
	$(src_dir)/qspi_host.c \
	$(src_dir)/qspi_nor.c \
	$(src_dir)/uart.c \
	$(src_dir)/pmic.c \
	$(src_dir)/ningbo.c \
	$(src_dir)/guilin.c \
	$(src_dir)/version_block.c

############ BOOT33 SECBOOT START ##############
# enable below part for BOOT33 SECBOOT support

#cflag += -DMBEDTLS_CONFIG_FILE=\"mbedtls_config.h\" \
	-DTF_MBEDTLS_KEY_ALG_ID=1 \
	-DTF_MBEDTLS_HASH_ALG_ID=1 \
	-DBOOT33_SECBOOT_SUPPORT
#srcs += $(src_dir)/fip.c \
		$(src_dir)/secboot.c \
		$(src_dir)/asn1parse.c \
		$(src_dir)/oid.c \
		$(src_dir)/sha256.c \
		$(src_dir)/md.c \
		$(src_dir)/x509.c \
		$(src_dir)/pkparse.c \
		$(src_dir)/pk.c \
		$(src_dir)/pk_wrap.c \
		$(src_dir)/rsa.c \
		$(src_dir)/rsa_internal.c \
		$(src_dir)/md_wrap.c \
		$(src_dir)/bignum.c \
		$(src_dir)/platform_util.c \
		$(src_dir)/mbedtls_common.c \
		$(src_dir)/mbedtls_crypto.c \
		$(src_dir)/mbedtls_x509_parser.c

############ BOOT33 SECBOOT END ##############

#common start
srcs_common = $(common_src_dir)/ssp_host.c \
			  $(common_src_dir)/asr_nor_flash.c \
			  $(common_src_dir)/spi_nor.c \
			  $(common_src_dir)/minilzo.c \
			  $(common_src_dir)/FreqChange.c \
			  $(common_src_dir)/LzmaDec.c \
			  $(common_src_dir)/system.c \
			  $(common_src_dir)/tinyalloc.c \
			  $(common_src_dir)/asr_property.c \
			  $(common_src_dir)/bsp_tavor.c \
			  $(common_src_dir)/updater_table.c \
			  $(common_src_dir)/logo_table.c \
			  $(common_src_dir)/asr_lzma.c

ifneq (,$(findstring SDCARD_USE,${SDCARD}))
	cflag += -DSDCARD_USE_IN_CODE
#add sdcard source code
	srcs += $(src_dir)/sdcard.c  \
	$(src_dir)/diskio.c \
	$(src_dir)/ff.c \
	$(src_dir)/ff_dump_mem.c \
	$(src_dir)/ffsystem.c \
	$(src_dir)/ffunicode.c
endif
dep_files = $(wildcard $(inc_dir)/*.h)
objs = $(patsubst %.c, ${obj_dir}/%.o, ${notdir ${srcs}})

dep_files += $(wildcard $(inc_common_dir)/*.h)
objs_common = $(patsubst %.c, ${obj_common_dir}/%.o, ${notdir ${srcs_common}})

objs_sum = $(objs) $(objs_common)

${TARGET}.axf:$(objs_sum)
	if not exist ${feedback_file} @echo "ERROR : make feedback firstly" && exit 1
	$(CC) $(cflag) -c $(src_dir)/OBM_StartUp.s -o $(obj_dir)/OBM_StartUp.o
	$(LINK) $(objs_sum) $(obj_dir)/OBM_StartUp.o --scatter scatter.scat --output $@ --map --symbols --info sizes,totals --list ${TARGET}.map --keep version_block.o\(IMGVERBLOCK\)
	$(FROMELF) --output ${TARGET}.bin --bin ${TARGET}.axf

${obj_common_dir}/%.o: $(src_common_dir)/%.c $(dep_files) 
	if not exist $(obj_common_dir) mkdir $(obj_common_dir)
	$(CC) $(cflag) -c $< -o $@

${obj_dir}/%.o:${src_dir}/%.c $(dep_files)
	if not exist $(obj_dir) mkdir $(obj_dir)
	$(CC) $(cflag) -c $< -o $@

feedback:$(objs_sum)
	$(CC) $(cflag) -c $(src_dir)/OBM_StartUp.s -o $(obj_dir)/OBM_StartUp.o
	$(LINK) $(objs_sum) $(obj_dir)/OBM_StartUp.o --scatter scatter.scat --output $(feedback_file).axf --keep version_block.o\(IMGVERBLOCK\) --feedback=${feedback_file}
	rd /s /q $(obj_dir)
	del $(feedback_file).axf

clean:
	rd /s /q $(obj_dir)
	del $(feedback_file)*
	rd /s /q $(obj_common_dir)
	del $(TARGET)*.*
