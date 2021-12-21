#=========================================================================
# File Name      : media.mak
# Description    : Main make file for the softutil/media package.
#
# Usage          : make [-s] -f media.mak OPT_FILE=<path>/<opt_file>
#
# Notes          : The options file defines macro values defined
#                  by the environment, target, and groups. It
#                  must be included for proper package building.
#
# Copyright (c) 2001 Intel Corporation. All Rights Reserved
#=========================================================================

# Package build options
include $(OPT_FILE)

# Package Makefile information, note: package.mak --> cpppackage.mak
GEN_PACK_MAKEFILE = $(BUILD_ROOT)/env/$(HOST)/build/cpppackage.mak

# Define Package ---------------------------------------

PACKAGE_NAME     = media
PACKAGE_BASE     = framework
PACKAGE_DEP_FILE = media_dep.mak
PACKAGE_PATH     = $(BUILD_ROOT)/$(PACKAGE_BASE)/$(PACKAGE_NAME)

# The path locations of source and include file directories.
PACKAGE_SRC_PATH    = $(PACKAGE_PATH)

PACKAGE_INC_PATHS   = $(BUILD_ROOT)/$(PACKAGE_BASE)/inc  \
					  $(BUILD_ROOT)/$(PACKAGE_BASE)/inc/cp   \
					  $(BUILD_ROOT)/$(PACKAGE_BASE)/inc/gui   \
					  $(BUILD_ROOT)/$(PACKAGE_BASE)/inc/gui/media   \
					  $(BUILD_ROOT)/$(PACKAGE_BASE)/inc/gui/asr   \
					  $(BUILD_ROOT)/$(PACKAGE_BASE)/inc/gui/message \
					  $(PACKAGE_PATH)/framework/player  \
					  $(PACKAGE_PATH)/framework/utils  \
					  $(PACKAGE_PATH)/framework/extractor \
					  $(PACKAGE_PATH)/framework/recorder \
					  $(BUILD_ROOT)/$(PACKAGE_BASE)/chip/isp/ \
					  $(BUILD_ROOT)/$(PACKAGE_BASE)/chip/isp/cam-hal \
					  $(BUILD_ROOT)/$(PACKAGE_BASE)/chip/isp/online_drv \
					  $(BUILD_ROOT)/$(PACKAGE_BASE)/chip/isp/offline_drv

FORCE_INC_PATHS = $(PACKAGE_PATH)/framework
              
# Package source files, paths not required
PACKAGE_SRC_FILES =  mci/mci_audio.cpp mci/mci_recorder.cpp mci/mci_Video.cpp 
#hal/src/mci_lcd_asr.cpp

# These are the tool flags specific to the FDI package only.
# The environment, target, and group also set flags.
# add macro:_CBA_HEADER to diff "Header file with path"
PACKAGE_ASMFLAGS =
PACKAGE_CFLAGS   = -D_CBA_HEADER -DCRANE_UOS
PACKAGE_CPPFLAGS =
PACKAGE_DFLAGS   =
PACKAGE_ARFLAGS  =

# The package dependency file
PACKAGE_DEP_FILE = media_dep.mak

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









