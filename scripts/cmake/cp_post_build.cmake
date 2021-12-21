
set(DBG_ON "0")

if(${DBG_ON})
  MESSAGE(STATUS "CMAKE_BINARY_DIR         : ${CMAKE_BINARY_DIR}")
  MESSAGE(STATUS "CP_SOURCE_DIR            : ${CP_SOURCE_DIR}")
  MESSAGE(STATUS "CP_SDK_LIB_DIR           : ${CP_SDK_LIB_DIR}")
endif()

#
# delete the old
#
file(GLOB MDB_FILES
    ${CMAKE_BINARY_DIR}/*MDB.txt
)
#MESSAGE(STATUS "remove ${MDB_FILES}")
foreach(mdb_file ${MDB_FILES})
    file(REMOVE ${mdb_file})
endforeach()
#
# copy the new
#
file(GLOB MDB_FILES
    ${CP_SOURCE_DIR}/tavor/Arbel/bin/*MDB.txt
)
#MESSAGE(STATUS "copy ${MDB_FILES}")
foreach(mdb_file ${MDB_FILES})
    file(COPY ${mdb_file} DESTINATION ${CMAKE_BINARY_DIR})
endforeach()
file(COPY ${CP_SOURCE_DIR}/tavor/Arbel/CRANE_SDK_LIB/${CP_SDK_LIB_DIR}/hsiupdlibdev.i DESTINATION ${CMAKE_BINARY_DIR})
