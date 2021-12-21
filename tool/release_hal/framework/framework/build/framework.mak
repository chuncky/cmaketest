#------------------------------------------------------------
# (C) Copyright [2006-2008] Marvell International Ltd.
# All Rights Reserved
#------------------------------------------------------------

#=========================================================================
# File Name      : framework.mak
# Description    : Main make file for the framework/framework group.
#
# Usage          : make [-s] -f framework.mak OPT_FILE=<path>/<opt_file>
#
# Notes          : The options file defines macro values defined
#                  by the environment, target, and groups. It
#                  must be included for proper group building.
#
#=========================================================================

# Group build options
include ${OPT_FILE}

# Group Makefile information
GEN_GROUP_MAKEFILE = ${BUILD_ROOT}/env/${HOST}/build/group.mak

# Define Group ---------------------------------------

GROUP_NAME     = framework
GROUP_BASE     = framework
GROUP_DEP_FILE = framework_dep.mak

GROUP_PATH = ${BUILD_ROOT}/$(GROUP_BASE)/$(GROUP_NAME)

# The relative path locations of local source and include file directories.
LOCAL_SRC_PATH    = $(GROUP_PATH)/src $(GROUP_PATH)
LOCAL_INC_PATHS   = $(GROUP_PATH)/src \
		${BUILD_ROOT}/os/nu_xscale/inc		\
		${BUILD_ROOT}/os/osa/inc		\
		${BUILD_ROOT}/hal/core/inc		\
		${BUILD_ROOT}/hal/GPIO/inc		\
		${BUILD_ROOT}/hal/RTC/inc		\
		${BUILD_ROOT}/hal/UART/inc		\
		$(PACKAGE_PATH)/src/include
#LOCAL_SRC_FILES = hal_init.c
#ifneq (,$(findstring MMI_INTERFACE,${VARIANT_LIST}))
#GROUP_INC_PATHS +=  ${BUILD_ROOT}/framework/LCD/inc
#endif
# Define Default Group Variant -----------------------

# Group source files, paths not required

# local group build flags for source files
# contained in this group directory
LOCAL_CFLAGS  =
LOCAL_DFLAGS  =

# These are the tool flags specific to the softutil group only.
# The environment, target, and group also set flags.
GROUP_CFLAGS  = -DBUILD_CAMERA_HAL -DCONFIG_SUPPORT_UILOG
GROUP_DFLAGS  =
GROUP_ARFLAGS =
ifneq (,$(findstring MMI_INTERFACE,${VARIANT_LIST}))
# Define COMMON members of this group
GROUP_PACKAGE_LIST  =  framework/keypad  \
                       framework/chip \
					   framework/device \
					   framework/media \
					   framework\test \
					   framework\os \
					   framework\cus

# GROUP_PACKAGE_LIST +=  framework/LCD
else
  ifneq (,$(findstring KEYPAD_SUPPORT,${VARIANT_LIST}))
  GROUP_PACKAGE_LIST  =  framework/keypad
  endif
endif

GROUP_PACKAGE_LIST +=  framework/mmi_external

# package and group variants required for this target variant
# syntax is <PACKAGE>_<VARIANT> or <GROUP>_<VARIANT>.
GROUP_VARIANT_LIST =

# Define Group Variants -------------------------------


#========================================================================

GROUP_GROUP_LIST   =

#========================================================================
#          PACKAGES
#========================================================================

# look for the variants in the VARIANT_LIST and override
# setting from the previous section. The variable


ifneq (,$(findstring FLAVOR_COM,${VARIANT_LIST}))
#VARIANT_LIST += SOFTUTIL_NO_FDI
endif

#LOCAL_SRC_FILES += fstdio_wrap.c FatSysWrapper.c

# GROUP_TRACE_LIST contains package and groups in which diagnostics/tracing
# will be compiled. Not compiling diagnostics/tracing in some modules
# will reduce code size and increase code performance.
GROUP_TRACE_LIST   =  $(GROUP_PACKAGE_LIST)

GROUP_TRACE_TYPE   = STRING_TRACING

# Include the Standard Group Make File ---------------
include ${GEN_GROUP_MAKEFILE}

# Include the Make Dependency File ---------------------
# This must be the last line in the file
include ${GROUP_DEP_FILE}
