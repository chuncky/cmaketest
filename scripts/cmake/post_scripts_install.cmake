MESSAGE(STATUS "hello, this post install script\n")

function(modify_config_file config_file)

  if(NOT EXISTS ${config_file})
    MESSAGE(STATUS "${config_file} not exist\n")
    return()
  endif()

  file(STRINGS ${config_file} config_entries)
  file(APPEND ${config_file} "\n")
  foreach(modify_item ${ARGN})

    #
    # TODO:
    # 1. if already exist
    # 2. if exist, but different value
    # 3. exist more than one item
    #
    #foreach(config_item ${config_entries})

    #endforeach()

    file(APPEND ${config_file} ${modify_item})
    file(APPEND ${config_file} "\n")
    MESSAGE(STATUS "modify ${config_file} ${modify_item}\n")

  endforeach()

endfunction()

#
# TODO:
# use ${BOARD} better, but cannot pass in now
#
set(CONFIG_FILE "${CMAKE_INSTALL_PREFIX}/scripts/boards/crane_evb_z2/crane_evb_z2_defconfig")
modify_config_file(${CONFIG_FILE} "CONFIG_BUILD_FROM_SOURCE_CODE=n")
