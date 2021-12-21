#=========================================================================
# File Name      : fatfs.mak
# Description    : Main make file for the softutil/fatfs package.
#
# Usage          : make [-s] -f fatfs.mak OPT_FILE=<path>/<opt_file>
#
# Notes          : The options file defines macro values defined
#                  by the environment, target, and groups. It
#                  must be included for proper package building.
#
# Copyright (c) 2001 Intel Corporation. All Rights Reserved
#=========================================================================

# Package build options
include $(OPT_FILE)

# Package Makefile information
GEN_PACK_MAKEFILE = $(BUILD_ROOT)/env/$(HOST)/build/package.mak

# Define Package ---------------------------------------

PACKAGE_NAME     = chip
PACKAGE_BASE     = framework
PACKAGE_DEP_FILE = chip_dep.mak
PACKAGE_PATH     = $(BUILD_ROOT)/$(PACKAGE_BASE)/$(PACKAGE_NAME)

# The path locations of source and include file directories.
#$(BUILD_ROOT)/$(PACKAGE_BASE)/media/hal
PACKAGE_SRC_PATH    = $(PACKAGE_PATH)/isp/cam-hal $(PACKAGE_PATH)/isp/online_drv $(BUILD_ROOT)/$(PACKAGE_BASE)/media/hal/src \
                      $(PACKAGE_PATH)/lcd $(PACKAGE_PATH)/charger $(PACKAGE_PATH)/isp/offline_drv/cpp $(PACKAGE_PATH)/isp/online_drv/pipeline \
					  $(PACKAGE_PATH)/isp
PACKAGE_INC_PATHS   = $(PACKAGE_PATH)/src    \
                      ${BUILD_ROOT}/hal/gpio/inc \
                      ${BUILD_ROOT}/hal/i2c/inc \
                      ${BUILD_ROOT}\os\nu_xscale\inc \
		      ${BUILD_ROOT}\hal\usb_device\inc \
                      ${BUILD_ROOT}\os\osa\inc \
		      	$(BUILD_ROOT)/$(PACKAGE_BASE)\inc   \
				$(BUILD_ROOT)/$(PACKAGE_BASE)\inc\cus   \
			$(BUILD_ROOT)/$(PACKAGE_BASE)\inc\cp \
			$(BUILD_ROOT)/$(PACKAGE_BASE)\inc\gui \
			$(BUILD_ROOT)/$(PACKAGE_BASE)\inc\gui\asr \
			$(PACKAGE_PATH)/clock/media-clk \
			$(BUILD_ROOT)/$(PACKAGE_BASE)\media\external_codecs\jpglib\include \
			$(PACKAGE_PATH)/isp/online_drv	  \
			$(PACKAGE_PATH)/isp/online_drv/pipeline	  \
			$(PACKAGE_PATH)/isp/offline_drv	  \
                        $(PACKAGE_PATH)/isp/cam-hal \
			$(PACKAGE_PATH)/isp \
			$(BUILD_ROOT)/$(PACKAGE_BASE)/device/camera \
            $(BUILD_ROOT)/$(PACKAGE_BASE)/device/lcd \
			$(BUILD_ROOT)/$(PACKAGE_BASE)/chip/lcd
			

               
# Package source files, paths not required
PACKAGE_SRC_FILES +=  \
                      camera_hal_drv_ops.c \
	              camera_hal_core.c \
               	camera_hal_gui_interface.c \
	              camera_jpeg_encoder.c \
				  cam_hal_drv_interface.c \
				  afbc_drv.c cam_offline.c \
				  jpeg_dma.c jpeg_enc_drv.c \
				  scaler_rotation.c \
				  cam_fe.c cam_pipeline.c \
				  cam_pipeline_reg.c cam_tuning.c \
				  other_call.c

ifeq (,$(findstring MMI_VIDEO_CALL_DISABLE,${VARIANT_LIST}))
PACKAGE_SRC_FILES +=  camera_hal_video_call.c
endif
           
PACKAGE_SRC_FILES += lcdd_asr.c lcd_drv.c lcd_fns.c lcd_hal.c \
					panel_drv.c spi_drv.c lcd_gif_decoder.c mcu_drv.c lcdd_esd.c\
					mci_lcd_asr.c

ifneq (,$(findstring SUPPORT_CHARGER,${VARIANT_LIST}))
PACKAGE_INC_PATHS +=$(BUILD_ROOT)/$(PACKAGE_BASE)/chip/charger
PACKAGE_SRC_FILES += power_charger.c power_mmi.c power_nvm.c
endif					


# These are the tool flags specific to the FDI package only.
# The environment, target, and group also set flags.
PACKAGE_ASMFLAGS =
PACKAGE_CFLAGS   =
PACKAGE_CPPFLAGS =
PACKAGE_DFLAGS   =
PACKAGE_ARFLAGS  =

# The package dependency file
PACKAGE_DEP_FILE = chip_dep.mak

# Define Package Variants -------------------------------

# look for the variants in the VARIANT_LIST and override
# setting from the previous section. The variable
# FDI_VARIANT_1 is meant to be overwritten
# with actual variant names. More variants can be added
# as required.



# Include the Standard Package Make File ---------------
include $(GEN_PACK_MAKEFILE)

# Include the Make Dependency File ---------------------
# This must be the last line in the file
include $(PACKAGE_DEP_FILE)









