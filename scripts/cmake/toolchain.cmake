##############################
#     toolchain.cmake
##############################

#CMAKE_SYSTEM_NAME may be set explicitly when first configuring a new build tree in order to enable cross compiling.
#In this case the CMAKE_SYSTEM_VERSION variable must also be set explicitly
#refer to https://cmake.org/Wiki/CMake_Cross_Compiling
if(${CMAKE_ARM_COMPILER_AC6})
else()

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR arm)
endif()

file(TO_CMAKE_PATH "$ENV{CC_PATH}" TOOL_ROOT_DIR)
#set(CMAKE_ASM_COMPILER    ${TOOL_ROOT_DIR}/armasm.exe)
#set(CMAKE_C_COMPILER         ${TOOL_ROOT_DIR}/armcc.exe)
#set(CMAKE_CXX_COMPILER    ${TOOL_ROOT_DIR}/armcc.exe)
#set(CMAKE_LINKER                   ${TOOL_ROOT_DIR}/armlink.exe)
#set(CMAKE_AR                           ${TOOL_ROOT_DIR}/armar.exe)
set(CMAKE_OBJCOPY                ${TOOL_ROOT_DIR}/fromelf.exe)

set(CMAKE_ASM_OUTPUT_EXTENSION .o)
set(CMAKE_C_OUTPUT_EXTENSION .o)
set(CMAKE_CXX_OUTPUT_EXTENSION .o)

set(CMAKE_EXECUTABLE_SUFFIX_ASM .elf)
set(CMAKE_EXECUTABLE_SUFFIX_C .elf)
set(CMAKE_EXECUTABLE_SUFFIX_CXX .elf)

set(CMAKE_STATIC_LIBRARY_PREFIX lib)
set(CMAKE_STATIC_LIBRARY_SUFFIX .a)

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories:NEVER:ONLY
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER)

set(CMAKE_C_USE_RESPONSE_FILE_FOR_OBJECTS 0)
set(CMAKE_C_USE_RESPONSE_FILE_FOR_INCLUDES 0)

if (${CONFIG_CMAKE_DEBUG})
  MESSAGE(STATUS "TOOL_ROOT_DIR		: ${TOOL_ROOT_DIR}")
  MESSAGE(STATUS "CMAKE_ASM_COMPILER		: ${CMAKE_ASM_COMPILER}")
  MESSAGE(STATUS "CMAKE_C_COMPILER		: ${CMAKE_C_COMPILER}")
  MESSAGE(STATUS "CMAKE_CXX_COMPILER		: ${CMAKE_CXX_COMPILER}")
  MESSAGE(STATUS "CMAKE_AR			: ${CMAKE_AR}")
  MESSAGE(STATUS "CMAKE_LINKER			: ${CMAKE_LINKER}")
  MESSAGE(STATUS "CMAKE_RANLIB			: ${CMAKE_RANLIB}")
  MESSAGE(STATUS "CMAKE_OBJCOPY		: ${CMAKE_OBJCOPY}")

  ###include(CMakePrintSystemInformation)
endif()
