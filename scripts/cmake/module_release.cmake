macro (module_release)
file(RELATIVE_PATH module_dir ${CMAKE_HOME_DIRECTORY} ${CMAKE_CURRENT_SOURCE_DIR})
set(module_install_dir ${CMAKE_INSTALL_PREFIX}/${module_dir})
###MESSAGE(STATUS "module_install_dir: ${module_install_dir}\n")

set(library_src_files ${library_c_files} ${library_cpp_files})
set(export_src_files ${export_c_files} ${export_cpp_files})

###MESSAGE(STATUS "library_c_files : ${library_c_files}")
###MESSAGE(STATUS "library_cpp_files : ${library_cpp_files}")
###MESSAGE(STATUS "library_src_files : ${library_src_files}")

set(file_exists "FALSE")
foreach(src ${library_src_files})
  if(IS_ABSOLUTE ${src})
    set(path ${src})
  else()
    set(path ${CMAKE_CURRENT_SOURCE_DIR}/${src})
  endif()

  if(IS_DIRECTORY ${path})
    message(FATAL_ERROR "library_src_files was called on a directory")
  endif()

  if(EXISTS ${path})
    set(file_exists "TRUE")
    break()
  else()
###    MESSAGE(STATUS "${path} NOT EXISTS")
  endif()

endforeach()
if("${file_exists}" STREQUAL "FALSE")
  set(library_src_files "")
endif()

if (library_src_files)
    #
    # create a library target
    #
    if("${module_lib_name}" STREQUAL "")
        fp_library()
        fp_library_get_current_dir_lib_name(module_lib_name)
    else()
        fp_library_named(${module_lib_name})
    endif()

    MESSAGE(STATUS "module_lib_name: ${module_lib_name}")

    fp_library_sources(
        ${library_src_files}
    )

    if(module_incs)
        fp_library_include_directories("${module_incs}")
    endif()

    if(module_defs)	
        fp_library_compile_definitions("${module_defs}")
    endif()

    if(module_opts)
        fp_library_compile_options("${module_opts}")
    endif()

    #
    # install library target
    #
    install(TARGETS ${module_lib_name} DESTINATION ${module_install_dir})

else()

    if("${module_lib_name}" STREQUAL "")
      fp_library_get_current_dir_lib_name(module_lib_name)
    endif()

    MESSAGE(STATUS "current_dir_lib_name: ${module_lib_name}")

    set(module_lib_full_name "${CMAKE_STATIC_LIBRARY_PREFIX}${module_lib_name}${CMAKE_STATIC_LIBRARY_SUFFIX}")

    list(APPEND
        copy_commands
        COMMAND ${CMAKE_COMMAND} -E echo "start to copy ${module_lib_full_name} to ${CMAKE_CURRENT_BINARY_DIR}/${module_lib_full_name}"
    )
    if(EXISTS ${module_install_dir}/${module_lib_full_name})
        list(APPEND
            copy_commands
            COMMAND ${CMAKE_COMMAND} -E copy ${module_install_dir}/${module_lib_full_name} ${CMAKE_CURRENT_BINARY_DIR}/${module_lib_full_name}
        )
    else()
        list(APPEND
            copy_commands
            COMMAND ${CMAKE_COMMAND} -E echo "${module_lib_full_name} not exists"
        )
    endif()
    add_custom_target(${module_lib_name}    
        ${copy_commands}
    )

    get_target_property(libs ${FP_CURRENT_INTERFACE} associated_libs)
    list(APPEND libs ${module_lib_name})
    #MESSAGE(STATUS "after append : ${libs}" "\n\r")
    set_target_properties(${FP_CURRENT_INTERFACE} PROPERTIES 
        associated_libs "${libs}"
    )

endif()

###MESSAGE(STATUS "export_src_files:${export_src_files}\n")
if (export_src_files)
    #
    #
    #
    fp_sources(
        ${export_src_files}
    )

    if(module_incs)
        fp_include_directories(${module_incs})
    endif()

    if(module_defs)
        fp_compile_definitions(${module_defs})
    endif()

    if(module_opts)
        fp_compile_options(${module_opts})
    endif()

endif()
endmacro()