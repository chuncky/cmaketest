
set(DBG_ON "0")

if(${DBG_ON})
    message(STATUS "Start to install objlib")
    message(STATUS "INPUT_OBJLIB_FILE    : ${INPUT_OBJLIB_FILE}")
    message(STATUS "INSTALL_DIR          : ${INSTALL_DIR}")
endif()

file(STRINGS ${INPUT_OBJLIB_FILE} OBJLIBLIST)

FOREACH(objlib_file ${OBJLIBLIST})
    if(DEL_OBJLIB)
        get_filename_component(name ${objlib_file} NAME)
        file(REMOVE ${INSTALL_DIR}/${name})
        if(${DBG_ON})
            message(STATUS "delete ${INSTALL_DIR}/${name} first")
        endif()
    endif()
    file(COPY ${objlib_file} DESTINATION ${INSTALL_DIR})
ENDFOREACH()