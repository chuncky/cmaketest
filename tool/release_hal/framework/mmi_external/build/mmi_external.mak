#------------------------------------------------------------
# (C) Copyright [2006-2008] Marvell International Ltd.
# All Rights Reserved
#------------------------------------------------------------

#=========================================================================
# File Name      : LCD.mak
# Description    : Main make file for the framework\mmi_external package.
#
# Usage          : make [-s] -f LCD.mak OPT_FILE=<path>/<opt_file>
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

PACKAGE_NAME     = mmi_external
PACKAGE_BASE     = framework
PACKAGE_PATH     = $(BUILD_ROOT)/$(PACKAGE_BASE)/$(PACKAGE_NAME)

# The path locations of source and include file directories.
PACKAGE_SRC_PATH    = $(PACKAGE_PATH)/src
PACKAGE_INC_PATHS   = $(PACKAGE_PATH)/src $(PACKAGE_PATH)/inc \
										  $(BUILD_ROOT)/hop/telephony/atcmdsrv/inc  \
									      $(BUILD_ROOT)/hop/telephony/atparser/inc	\
									      $(BUILD_ROOT)/pcac/duster/inc	\
										  ${BUILD_ROOT}/hop/mmi_mat/inc 


PACKAGE_SRC_FILES = at_cmds_mmi.c

# These are the tool flags specific to the LCD package only.
# The environment, target, and group also set flags.
PACKAGE_ASMFLAGS =
PACKAGE_CFLAGS  =
PACKAGE_CPPFLAGS =
PACKAGE_DFLAGS  =

PACKAGE_ARFLAGS =

# The package dependency file
PACKAGE_DEP_FILE = mmi_external_dep.mak
# Define Package Variants -------------------------------

# Include the Standard Package Make File ---------------
include $(GEN_PACK_MAKEFILE)

# Include the Make Dependency File ---------------------
# This must be the last line in the file
include $(PACKAGE_DEP_FILE)
