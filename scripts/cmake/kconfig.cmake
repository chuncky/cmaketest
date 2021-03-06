# Folders needed for conf/mconf files (kconfig has no method of redirecting all output files).
# conf/mconf needs to be run from a different directory because of: GH-3408
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/kconfig/include/generated)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/kconfig/include/config)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/include/generated)

set_ifndef(KCONFIG_ROOT ${FP_BASE}/Kconfig)

set(BOARD_DEFCONFIG ${BOARD_DIR}/${BOARD}_defconfig)
set(DOTCONFIG       ${FP_BASE}/build/.config)

if(CONF_FILE)
string(REPLACE " " ";" CONF_FILE_AS_LIST "${CONF_FILE}")
endif()

set(ENV{srctree}            ${FP_BASE})
set(ENV{KERNELVERSION}      ${PROJECT_VERSION})
set(ENV{KCONFIG_CONFIG}     ${DOTCONFIG})
set(ENV{KCONFIG_AUTOHEADER} ${AUTOCONF_H})

# Set environment variables so that Kconfig can prune Kconfig source
# files for other architectures
set(ENV{ENV_VAR_ARCH}      ${ARCH})
set(ENV{ENV_VAR_BOARD_DIR} ${BOARD_DIR})

add_custom_target(
  menuconfig
  ${CMAKE_COMMAND} -E env
  srctree=${FP_BASE}
  KERNELVERSION=${PROJECT_VERSION}
  KCONFIG_CONFIG=${DOTCONFIG}
  ENV_VAR_ARCH=$ENV{ENV_VAR_ARCH}
  ENV_VAR_BOARD_DIR=$ENV{ENV_VAR_BOARD_DIR}
  ${PYTHON_EXECUTABLE} ${FP_BASE}/scripts/kconfig/menuconfig.py ${KCONFIG_ROOT}
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/kconfig
  USES_TERMINAL
  )

# Bring in extra configuration files dropped in by the user or anyone else;
# make sure they are set at the end so we can override any other setting
file(GLOB config_files ${APPLICATION_BINARY_DIR}/*.conf)
list(SORT config_files)
set(
  merge_config_files
  ${BOARD_DEFCONFIG}
  ${CONF_FILE_AS_LIST}
  ${OVERLAY_CONFIG}
  ${config_files}
)

# Create a list of absolute paths to the .config sources from
# merge_config_files, which is a mix of absolute and relative paths.
set(merge_config_files_with_absolute_paths "")
foreach(f ${merge_config_files})
  if(IS_ABSOLUTE ${f})
    set(path ${f})
  else()
    set(path ${BOARD_DIR}/${f})
  endif()

  list(APPEND merge_config_files_with_absolute_paths ${path})
endforeach()

foreach(f ${merge_config_files_with_absolute_paths})
  if(NOT EXISTS ${f} OR IS_DIRECTORY ${f})
    message(FATAL_ERROR "File not found: ${f}")
  endif()
endforeach()

# Calculate a checksum of merge_config_files to determine if we need
# to re-generate .config
set(merge_config_files_checksum "")
foreach(f ${merge_config_files_with_absolute_paths})
  file(MD5 ${f} checksum)
  set(merge_config_files_checksum "${merge_config_files_checksum}${checksum}")
endforeach()

# Create a new .config if it does not exists, or if the checksum of
# the dependencies has changed
set(merge_config_files_checksum_file ${PROJECT_BINARY_DIR}/.cmake.dotconfig.checksum)
set(CREATE_NEW_DOTCONFIG "")
if(NOT EXISTS ${DOTCONFIG})
  set(CREATE_NEW_DOTCONFIG 1)
else()
  # Read out what the checksum was previously
  file(READ
    ${merge_config_files_checksum_file}
    merge_config_files_checksum_prev
    )
  set(CREATE_NEW_DOTCONFIG 1)
  if(
      ${merge_config_files_checksum} STREQUAL
      ${merge_config_files_checksum_prev}
      )
    set(CREATE_NEW_DOTCONFIG 0)
  endif()
endif()

if(CREATE_NEW_DOTCONFIG)
  set(merge_fragments ${merge_config_files})
else()
  set(merge_fragments ${DOTCONFIG})
endif()

execute_process(
  COMMAND
  ${PYTHON_EXECUTABLE}
  ${FP_BASE}/scripts/kconfig/kconfig.py
  ${KCONFIG_ROOT}
  ${DOTCONFIG}
  ${PROJECT_BINARY_DIR}/include/generated/autoconf.h
  ${merge_fragments}
  WORKING_DIRECTORY ${BOARD_DIR}
  # The working directory is set to the app dir such that the user
  # can use relative paths in CONF_FILE, e.g. CONF_FILE=nrf5.conf
  RESULT_VARIABLE ret
  )
if(NOT "${ret}" STREQUAL "0")
  message(FATAL_ERROR "command failed with return code: ${ret}")
endif()

if(CREATE_NEW_DOTCONFIG)
  file(WRITE
    ${merge_config_files_checksum_file}
    ${merge_config_files_checksum}
    )
endif()

# Force CMAKE configure when the configuration files changes.
foreach(merge_config_input ${merge_config_files} ${DOTCONFIG})
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${merge_config_input})
endforeach()

add_custom_target(config-sanitycheck DEPENDS ${DOTCONFIG})

# Parse the lines prefixed with CONFIG_ in the .config file from Kconfig
import_kconfig(${DOTCONFIG})
