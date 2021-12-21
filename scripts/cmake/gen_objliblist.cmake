

########################################################
# 1. function call before build target
########################################################
#
# 1.1: delete objlibs, and clear the objlib list file
# 
# e.g.
# COMMAND  ${CMAKE_COMMAND}
#    "-DINPUT_OBJLIBLIST_FILE=${GUI_OBJLIBLIST_FILE_ORIG}"
#    "-DOUTPUT_OBJLIBLIST_FILE=${GUI_OBJLIBLIST_FILE}"
#    "-DPREFIX_STR_GROUP=${PREFIX_STR_GROUP}"
#    "-DCONCAT_STR_GROUP=${CONCAT_STR_GROUP}"
#    "-DDEL_OBJLIB=1"
#    -P ${FP_BASE}/scripts/cmake/gen_objliblist.cmake
#

########################################################
# 2. function call after build target
########################################################
#
# 2.1: 
#   generate objlib list file according to the objlibs after build,
# 
# e.g.
# COMMAND  ${CMAKE_COMMAND}
#    "-DINPUT_OBJLIBLIST_FILE=${GUI_OBJLIBLIST_FILE_ORIG}"
#    "-DOUTPUT_OBJLIBLIST_FILE=${GUI_OBJLIBLIST_FILE}"
#    "-DPREFIX_STR_GROUP=${PREFIX_STR_GROUP}"
#    "-DCONCAT_STR_GROUP=${CONCAT_STR_GROUP}"
#    -P ${FP_BASE}/scripts/cmake/gen_objliblist.cmake
#

#
# 2.2: check build is finished,
#      and generate objlib list file according to the objlibs after build,
#
# e.g.
# COMMAND  ${CMAKE_COMMAND}
#    "-DINPUT_OBJLIBLIST_FILE=${GUI_OBJLIBLIST_FILE_ORIG}"
#    "-DOUTPUT_OBJLIBLIST_FILE=${GUI_OBJLIBLIST_FILE}"
#    "-DPREFIX_STR_GROUP=${PREFIX_STR_GROUP}"
#    "-DCONCAT_STR_GROUP=${CONCAT_STR_GROUP}"
#    "-DBUILD_FINISH_FILE=${BUILD_FINISH_FILE}"
#    -P ${FP_BASE}/scripts/cmake/gen_objliblist.cmake
#

set(DBG_ON "0")

if(${DBG_ON})
    message(STATUS "Start to generate objliblist.txt")
    message(STATUS "INPUT_OBJLIBLIST_FILE    : ${INPUT_OBJLIBLIST_FILE}")
    message(STATUS "OUTPUT_OBJLIBLIST_FILE   : ${OUTPUT_OBJLIBLIST_FILE}")
    message(STATUS "PREFIX_STR_GROUP         : ${PREFIX_STR_GROUP}")
    message(STATUS "CONCAT_STR_GROUP         : ${CONCAT_STR_GROUP}")
    message(STATUS "BUILD_FINISH_FILE        : ${BUILD_FINISH_FILE}")
    message(STATUS "DEL_OBJLIB               : ${DEL_OBJLIB}")
    message(STATUS "PREPEND_STRING           : ${PREPEND_STRING}")
    message(STATUS "APPEND_STRING            : ${APPEND_STRING}")
endif()

if(DEFINED BUILD_FINISH_FILE)

    message(STATUS " ******************************************")
    message(STATUS " *** The target is building...          ***")
    message(STATUS " *** Please wait several minutes        ***")
    message(STATUS " *** Or you can stop it by CTRL+C       ***")
    message(STATUS " ******************************************")

    set(repeat 0)
    while(1)
        if(EXISTS ${BUILD_FINISH_FILE})
            file(STRINGS ${BUILD_FINISH_FILE} BUILD_RESULT)
            string(COMPARE EQUAL "${BUILD_RESULT}" "build_result=0" build_result_match)
            if(${build_result_match})
                message(STATUS "build success: ${BUILD_RESULT}, repeat: ${repeat}")
                break()
            else()
                math(EXPR repeat "${repeat}+1")
                if(${repeat} LESS 10)
                    continue()
                endif()
                message(STATUS "build failed: ${BUILD_RESULT}, repeat: ${repeat}")
                file(REMOVE ${OUTPUT_OBJLIBLIST_FILE})
                return()
            endif()
        endif()
        #
        # TODO: TIMEOUT or build flag
        #
        ###message(STATUS "waiting build finish")
    endwhile(1)
    message(STATUS "end to check build finish text")
endif()

file(STRINGS ${INPUT_OBJLIBLIST_FILE} OBJLIBLIST)

if(DEFINED PREPEND_STRING)
    string(PREPEND OBJLIBLIST "${PREPEND_STRING};")
endif()

if(DEFINED {APPEND_STRING)
    string(APPEND OBJLIBLIST ";${APPEND_STRING}")
endif()

if(${DBG_ON})
    message(STATUS "OBJLIBLIST: ${OBJLIBLIST}")
endif()

set(first_write 1)

FOREACH(objlib_file ${OBJLIBLIST})

    file(TO_CMAKE_PATH "${objlib_file}" cmake_objlib_file)
    if(${DBG_ON})
        message(STATUS "handle objlib file:${cmake_objlib_file}")
    endif()
    
    set(index 0)
    set(prefix_match 0)
    FOREACH(prefix ${PREFIX_STR_GROUP})
        if(${DBG_ON})
            message(STATUS "handle prefix:${prefix}")
        endif()
		
        string(LENGTH ${prefix} prefix_len)
        string(SUBSTRING ${cmake_objlib_file} 0 ${prefix_len} prefix_string)
        string(COMPARE EQUAL ${prefix_string} ${prefix} prefix_match)

        if(prefix_match)
        
            list(GET CONCAT_STR_GROUP ${index} concat_str)
            string(CONCAT full_path_objlib_file "${concat_str}" "${cmake_objlib_file}")
            
            if(${DBG_ON})
                message(STATUS "prefix match")
                message(STATUS "get concat string ${concat_str} at index ${index}")                        
                message(STATUS "full path obj:${full_path_objlib_file}")
            endif()
             #
             # exit the second foreach() loop
             #
            break()
        else()
            if(${DBG_ON})
                message(STATUS "prefix miss match")
            endif()
        endif()
		
        math(EXPR index "${index}+1")
		
    ENDFOREACH()
	
    if(NOT prefix_match)
        message(FATAL_ERROR "prefix miss match, please check input file and prefix string group")
    endif()
	
    if(EXISTS ${full_path_objlib_file})
        if(DEL_OBJLIB)
            file(REMOVE ${full_path_objlib_file})
            if(${DBG_ON})
                message(STATUS "deleted")
            endif()
        else()
            if(${first_write})
                file(REMOVE ${OUTPUT_OBJLIBLIST_FILE})
                set(first_write 0)
            endif()
            file(APPEND ${OUTPUT_OBJLIBLIST_FILE} ${full_path_objlib_file})
            file(APPEND ${OUTPUT_OBJLIBLIST_FILE} "\n")
            if(${DBG_ON})
                message(STATUS "objlib append to list file")
            endif()
            if(COPY_TO)
                file(COPY ${full_path_objlib_file} DESTINATION ${COPY_TO})
            endif()
        endif()
    endif()
  
ENDFOREACH()

