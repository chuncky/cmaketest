#=========================================================================
# File Name      : test.mak
# Description    : Main make file for the softutil/fatfs package.
#
# Usage          : make [-s] -f test.mak OPT_FILE=<path>/<opt_file>
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

PACKAGE_NAME     = test
PACKAGE_BASE     = framework
PACKAGE_DEP_FILE = test_dep.mak
PACKAGE_PATH     = $(BUILD_ROOT)/$(PACKAGE_BASE)/$(PACKAGE_NAME)

# The path locations of source and include file directories.
PACKAGE_SRC_PATH    = $(PACKAGE_PATH)
PACKAGE_INC_PATHS   = $(PACKAGE_PATH)/media/mdi/inc    \
					  $(PACKAGE_PATH)/media/mdi/sstreams    \
                      $(BUILD_ROOT)/$(PACKAGE_BASE)/inc  \
					  $(BUILD_ROOT)/$(PACKAGE_BASE)/inc/cp
					  
 

               
# Package source files, paths not required
PACKAGE_SRC_FILES +=  media/mdi/src/mdi_testbench.c lcd/lcd_test.c


# These are the tool flags specific to the FDI package only.
# The environment, target, and group also set flags.
PACKAGE_ASMFLAGS =
PACKAGE_CFLAGS   =
PACKAGE_CPPFLAGS =
PACKAGE_DFLAGS   =
PACKAGE_ARFLAGS  =

# The package dependency file
#PACKAGE_DEP_FILE = test_dep.mak

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









