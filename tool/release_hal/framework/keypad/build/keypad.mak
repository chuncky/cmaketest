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

PACKAGE_NAME     = keypad
PACKAGE_BASE     = framework
PACKAGE_DEP_FILE = keypad_dep.mak
PACKAGE_PATH     = $(BUILD_ROOT)/$(PACKAGE_BASE)/$(PACKAGE_NAME)

# The path locations of source and include file directories.
PACKAGE_SRC_PATH    = $(PACKAGE_PATH)/src
PACKAGE_INC_PATHS   = $(PACKAGE_PATH)/src    \
                      $(PACKAGE_PATH)/inc    \
                      ${BUILD_ROOT}\os\nu_xscale\inc \
		      ${BUILD_ROOT}\hal\usb_device\inc \
                      ${BUILD_ROOT}\os\osa\inc \
		      $(BUILD_ROOT)/$(PACKAGE_BASE)/chip/inc \
		      $(BUILD_ROOT)/framework/inc
 

               
# Package source files, paths not required
PACKAGE_SRC_FILES +=  keypad_drv.c


# These are the tool flags specific to the FDI package only.
# The environment, target, and group also set flags.
PACKAGE_ASMFLAGS =
PACKAGE_CFLAGS   =
PACKAGE_CPPFLAGS =
PACKAGE_DFLAGS   =
PACKAGE_ARFLAGS  =

# The package dependency file
#PACKAGE_DEP_FILE = fatfs_dep.mak

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









