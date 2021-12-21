#=========================================================================
# File Name      : media.mak
# Description    : Main make file for the framework/media package.
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
PACKAGE_NAME     = device
PACKAGE_BASE     = framework
PACKAGE_DEP_FILE = device_dep.mak
PACKAGE_PATH     = $(BUILD_ROOT)/$(PACKAGE_BASE)/$(PACKAGE_NAME)

# The path locations of source file directories.

PACKAGE_SRC_PATH   = $(PACKAGE_PATH)
						
# The path locations of include file directories.	
PACKAGE_INC_PATHS   = 	\
						$(BUILD_ROOT)\$(PACKAGE_BASE)\inc   \
						$(BUILD_ROOT)\$(PACKAGE_BASE)\inc\gui \
						$(BUILD_ROOT)\$(PACKAGE_BASE)\inc\cp \
						$(BUILD_ROOT)/$(PACKAGE_BASE)/chip/isp/online_drv \
						$(BUILD_ROOT)/$(PACKAGE_BASE)/chip/isp/offline_drv \
						$(BUILD_ROOT)/$(PACKAGE_BASE)/chip/isp/cam-hal \
						$(BUILD_ROOT)/$(PACKAGE_BASE)/chip/isp \
						$(BUILD_ROOT)/$(PACKAGE_BASE)/chip/lcd \
						$(BUILD_ROOT)/$(PACKAGE_BASE)/chip/clock/media-clk \
						$(BUILD_ROOT)/$(PACKAGE_BASE)/chip/isp\online_drv/pipeline \
						$(BUILD_ROOT)/$(PACKAGE_BASE)/device\lcd \
						$(BUILD_ROOT)/$(PACKAGE_BASE)/device
						
						
# Package source files, paths not required
PACKAGE_SRC_FILES = camera/cam_sensor_spi_gc032a.c \
	                 camera/cam_sensor_mipi_gc030a.c \
	                 cam_sensor.c
PACKAGE_SRC_FILES += lcd/lcd_st7789v_spi.c lcd/lcd_st7789v_mcu.c lcd/lcd_gc9305_spi.c lcd/lcd_gc9305_mcu.c lcd/lcd_dummy_spi.c lcd/lcd_dummy_mcu.c lcd/panel_list.c \
                     lcd/lcd_mcu_common.c lcd/lcd_spi_common.c lcd/backlight_drv.c
# These are the tool flags specific to the FDI package only.
# The environment, target, and group also set flags.
#PACKAGE_CFLAGS is C MACRO
PACKAGE_ASMFLAGS =
PACKAGE_CFLAGS   =
PACKAGE_CPPFLAGS =
PACKAGE_DFLAGS   =
PACKAGE_ARFLAGS  =

# The package dependency file
PACKAGE_DEP_FILE = device_dep.mak

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









