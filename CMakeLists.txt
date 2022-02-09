cmake_minimum_required(VERSION 3.10)

if($ENV{CXX} STREQUAL "armclang.exe")
	set(CMAKE_ARM_COMPILER_AC6 TRUE)
endif()

if(${CMAKE_ARM_COMPILER_AC6})
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR cortex-r5)
endif()

project(feature_phone LANGUAGES ASM C CXX)

include($ENV{FP_BASE}/scripts/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)

if (${CONFIG_CMAKE_DEBUG})
  MESSAGE(STATUS "CMAKE_MAKE_PROGRAM       : ${CMAKE_MAKE_PROGRAM}")
  MESSAGE(STATUS "CMAKE_C_COMPILER_ID      : ${CMAKE_C_COMPILER_ID}")
  MESSAGE(STATUS "CMAKE_PROJECT_NAME       : ${CMAKE_PROJECT_NAME}")
  
  MESSAGE(STATUS "CMAKE_HOME_DIRECTORY     : ${CMAKE_HOME_DIRECTORY}")
  MESSAGE(STATUS "CMAKE_SOURCE_DIR         : ${CMAKE_SOURCE_DIR}")
  MESSAGE(STATUS "CMAKE_BINARY_DIR         : ${CMAKE_BINARY_DIR}")
  MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR : ${CMAKE_CURRENT_SOURCE_DIR}")
  MESSAGE(STATUS "CMAKE_CURRENT_BINARY_DIR : ${CMAKE_CURRENT_BINARY_DIR}")
  MESSAGE(STATUS "CMAKE_PARENT_LIST_FILE   : ${CMAKE_PARENT_LIST_FILE}")
  MESSAGE(STATUS "CMAKE_CURRENT_LIST_DIR   : ${CMAKE_CURRENT_LIST_DIR}")
  MESSAGE(STATUS "CMAKE_CURRENT_LIST_FILE  : ${CMAKE_CURRENT_LIST_FILE}")
  MESSAGE(STATUS "PROJECT_SOURCE_DIR       : ${PROJECT_SOURCE_DIR}")
  MESSAGE(STATUS "PROJECT_BINARY_DIR       : ${PROJECT_BINARY_DIR}")
  MESSAGE(STATUS "<PROJECT-NAME>_SOURCE_DIR: ${${CMAKE_PROJECT_NAME}_SOURCE_DIR}")
  MESSAGE(STATUS "<PROJECT-NAME>_BINARY_DIR: ${${CMAKE_PROJECT_NAME}_BINARY_DIR}")
  MESSAGE(STATUS "CMAKE_CACHEFILE_DIR      : ${CMAKE_CACHEFILE_DIR}")
endif()


##############################
#     CMakeLists.txt
##############################


MESSAGE(STATUS "CMAKE_CURRENT_LIST_DIR   : ${CMAKE_CURRENT_LIST_DIR}")
MESSAGE(STATUS "PROJECT_BINARY_DIR       : ${PROJECT_BINARY_DIR}")
MESSAGE(STATUS "CMAKE_COMMAND       : ${CMAKE_COMMAND}")
if(${CMAKE_ARM_COMPILER_AC6})
MESSAGE(STATUS "CMAKE_ARM_COMPILER_AC6       : ${CMAKE_ARM_COMPILER_AC6}")
endif()
set(  FP_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(        FP_BASE "${FP_SOURCE_DIR}")

set( HAL_SOURCE_DIR "${FP_SOURCE_DIR}/hal")
set( BOOTLOADER_SOURCE_DIR "${FP_SOURCE_DIR}/bootloader")

set( SCRIPTS_DIR "${FP_SOURCE_DIR}/scripts")
set( TOOL_DIR "${FP_SOURCE_DIR}/tool")
set(CMAKE_INSTALL_PREFIX "${CONFIG_RELEASE_INSTALL_DIRECTORY}")


set(EMPTY_FILE ${CMAKE_CURRENT_BINARY_DIR}/empty_file.c)
file(WRITE ${EMPTY_FILE})

set(FEEDBACK_FILE ${CMAKE_BINARY_DIR}/feedback.txt)
if (NOT EXISTS ${FEEDBACK_FILE})
  if (EXISTS ${SCRIPTS_DIR}/boards/${BOARD}/feedback.txt)
    file(COPY ${SCRIPTS_DIR}/boards/${BOARD}/feedback.txt DESTINATION ${CMAKE_BINARY_DIR})
  else()
    file(WRITE ${FEEDBACK_FILE})
  endif()
endif()

set(ARCH_FLAGS "--cpu=Cortex-R5")


#
# the main target
#
set(FP_TARGET "boot33")
add_executable(${FP_TARGET})
target_sources(${FP_TARGET} PRIVATE
    ${EMPTY_FILE}
)

#
# bootloader target
#
fp_declare_project(bootloader)

#
# pre build
#

list(APPEND GLOBLE_FEATURE_DEF  "SUPPORT_COMPRESSED_LOGO")

if (DEFINED CONFIG_RELEASE_WITH_NO_ASR_UI)
    list(APPEND GLOBLE_FEATURE_DEF "CONFIG_RELEASE_WITH_NO_ASR_UI=1")
endif()

if (DEFINED CONFIG_ASR_SIM_SINGLE)
    list(APPEND GLOBLE_FEATURE_DEF  "CONFIG_ASR_SIM_SINGLE=1")
endif()

if (DEFINED CONFIG_BUILD_CAMERA_TARGET)
	list(APPEND GLOBLE_FEATURE_DEF  "BUILD_CAMERA_HAL=1")
endif()

if (DEFINED CONFIG_SUPPORT_MM_AMRWB)
	list(APPEND GLOBLE_FEATURE_DEF  "AMRWB_SUPPORT=1")
endif()


if (DEFINED CONFIG_SUPPORT_MM_ADPCM)
    list(APPEND GLOBLE_FEATURE_DEF  "CONFIG_ADPCM_CODEC_SUPPORT=1") 
endif()

if (DEFINED CONFIG_SUPPORT_MM_AVI)
    list(APPEND GLOBLE_FEATURE_DEF  "AVI_SUPPORT=1") 
endif()

if (DEFINED CONFIG_DISPLAY_240x320)
	list(APPEND GLOBLE_FEATURE_DEF  "CONFIG_DISPLAY_240x320=1")
	list(APPEND GLOBLE_FEATURE_DEF  "LCDSIZE_240_320=1")
  if (DEFINED CONFIG_BOARD_CRANEM_EVB)
	list(APPEND GLOBLE_FEATURE_DEF  "LCDDEPTH_16")
  else()
	list(APPEND GLOBLE_FEATURE_DEF  "LCDDEPTH_32")
  endif()
endif()

if (DEFINED CONFIG_DISPLAY_320x240)
	list(APPEND GLOBLE_FEATURE_DEF  "CONFIG_DISPLAY_320x240=1")
	list(APPEND GLOBLE_FEATURE_DEF  "LCDSIZE_320_240=1")
	list(APPEND GLOBLE_FEATURE_DEF  "LCDDEPTH_32")
endif()
if (DEFINED CONFIG_DISPLAY_128x160)
	list(APPEND GLOBLE_FEATURE_DEF  "CONFIG_DISPLAY_128x160=1")
	list(APPEND GLOBLE_FEATURE_DEF  "LCDSIZE_128_160=1")
	list(APPEND GLOBLE_FEATURE_DEF  "LCDDEPTH_32")
	### simu 128x160 on DKB's 240x320 lcd. only for demo.
	list(APPEND GLOBLE_FEATURE_DEF  "SIMU_128_160_ON_LCD240x320=1")
endif()
if (DEFINED CONFIG_DISPLAY_128x64)
	list(APPEND GLOBLE_FEATURE_DEF  "CONFIG_DISPLAY_128x64=1")
	list(APPEND GLOBLE_FEATURE_DEF  "LCDSIZE_128_64=1")
	list(APPEND GLOBLE_FEATURE_DEF  "LCDDEPTH_32")
	### simu 128x64 on DKB's 240x320 lcd. only for demo.
	list(APPEND GLOBLE_FEATURE_DEF  "SIMU_128_64_ON_LCD240x320=1")
endif()
if (DEFINED CONFIG_DISPLAY_176x220)
	list(APPEND GLOBLE_FEATURE_DEF  "CONFIG_DISPLAY_176x220=1")
	list(APPEND GLOBLE_FEATURE_DEF  "LCDSIZE_176_220=1")
	list(APPEND GLOBLE_FEATURE_DEF  "LCDDEPTH_32")
	### simu 176x220 on DKB's 240x320 lcd. only for demo.
	list(APPEND GLOBLE_FEATURE_DEF  "SIMU_176_220_ON_LCD240x320=1")
endif()
if (DEFINED CONFIG_DISPLAY_128x128)
	list(APPEND GLOBLE_FEATURE_DEF  "CONFIG_DISPLAY_128x128=1")
	list(APPEND GLOBLE_FEATURE_DEF  "LCDSIZE_128_128=1")
	list(APPEND GLOBLE_FEATURE_DEF  "LCDDEPTH_16")
	### simu 128x128 on DKB's 240x320 lcd. only for demo.
	list(APPEND GLOBLE_FEATURE_DEF  "SIMU_128_128_ON_LCD240x320=1")
endif()
if (DEFINED CONFIG_DISPLAY_240x240)
	list(APPEND GLOBLE_FEATURE_DEF  "CONFIG_DISPLAY_240x240=1")
	list(APPEND GLOBLE_FEATURE_DEF  "LCDSIZE_240_240=1")
	list(APPEND GLOBLE_FEATURE_DEF  "LCDDEPTH_32")
	### simu 240x240 on DKB's 240x320 lcd. only for demo.
	#list(APPEND GLOBLE_FEATURE_DEF  "SIMU_240_240_ON_LCD240x320=1")
endif()

if (DEFINED CONFIG_FWP)
else ()
#add it:for set external SD music as callcoming/sms ring.
list(APPEND GLOBLE_FEATURE_DEF "ENABLE_SETTINGS_RING_FROM_FILES"   )
endif()

if (DEFINED CONFIG_MINIPHONE)
list(APPEND GLOBLE_FEATURE_DEF  "CONFIG_MINIPHONE=1")
endif()

if (DEFINED CONFIG_SUPPORT_QRCODE)
list(APPEND GLOBLE_FEATURE_DEF "CONFIG_SUPPORT_QRCODE=1")
endif()

if (DEFINED CONFIG_SUPPORT_QRCODE_ZXING)
list(APPEND GLOBLE_FEATURE_DEF "CONFIG_SUPPORT_QRCODE_ZXING=1")
endif()

#
#list some global MACRO from app_target_defs.h
#
#"__MMI_T9__" "__MMI_T9_V7__"
	list(APPEND GLOBLE_FEATURE_DEF
	"__ADP_MULTI_SIM__"
	"__MULTI_SIM_SUPPORT__"
	"__MOD_SMSAL__"
	"__MOD_CSM__"
	"__MOD_PHB__"
	"__MOD_RAC__"
	"PHB_REJECTCALL_NO_RECORD_AND_MENU"
	"__SMS_SUPPORT_SANMU_DRAFTBOX__"
	"__SUPPORT_SMSCLASS__"
	"__PHB_SUPPORT_ASR_NUMBERS__"
	"__PRAYERTIMES_SUPPORT__"
	"ENABLE_NVRAM"
	"__SMS_HAS_RECENT_SEND_LIST__"
	"ENABLE_MCI"
	"__FS_MOVE_SUPPORT__"
	"__PHB_0x81_SUPPORT__"
	"__SUPPORT_NO_SIM_LOAD_ME_SMS__"
	"__THREADX__"
	"__TARGET_ASR3601__"
	"USE_PLATFORM_TIMER"
	"CSD_NEW_API"
	"__PLAT_USE_UCS2_ENCODING__"
	"__DATA_STATUS_REPORT_SUPPORT__"
	"MV_USB2_MASS_STORAGE"
)

if (DEFINED CONFIG_DYNAMIC_SWITCH_DATA_SIM_SUPPORT)
	list(APPEND GLOBLE_FEATURE_DEF  
    "__DYNAMIC_SWITCH_DATA_SIM_SUPPORT__"
)
endif()

if (DEFINED CONFIG_MSYS_RIL)
	list(APPEND GLOBLE_FEATURE_DEF "ENABLE_MSYS_RIL_SUPPORT")
endif()

if (DEFINED CONFIG_MSYS_MINI_AT)
	list(APPEND GLOBLE_FEATURE_DEF "ENABLE_MSYS_MINI_AT_SUPPORT")
endif()

if (DEFINED CONFIG_FMRADIO_PLAYER_SUPPORT)
    list(APPEND GLOBLE_FEATURE_DEF "CONFIG_FMRADIO_PLAYER_SUPPORT")
endif()

#if (DEFINED CONFIG_BUILD_CAMERA_TARGET)
#    list(APPEND GLOBLE_FEATURE_DEF "BUILD_CAMERA_MODUAL")
#endif()

if ((DEFINED CONFIG_BOARD_CUSTOM_B) OR (DEFINED CONFIG_BOARD_CUSTOM_V))
	list(APPEND GLOBLE_FEATURE_DEF  "FM_RADIO_WITH_EXTERNAL_ANT")
endif()


	list(APPEND GLOBLE_FEATURE_DEF  "CP_ERASE")
	list(APPEND GLOBLE_FEATURE_DEF  "ONKEY_CHECK")
		list(APPEND GLOBLE_FEATURE_DEF  "SUPPORT_COMPRESSED_LOGO")




#
# list globel include directories here
#
fp_include_directories(
    "${FP_SOURCE_DIR}/inc"

    "${FP_SOURCE_DIR}/inc/gui"
    "${FP_SOURCE_DIR}/inc/gui/media"
    "${FP_SOURCE_DIR}/inc/gui/message"
    "${FP_SOURCE_DIR}/inc/gui/nvram"
    "${FP_SOURCE_DIR}/inc/gui/asr"
    "${FP_SOURCE_DIR}/inc/media"
    "${FP_SOURCE_DIR}/inc/gui"
    "${FP_SOURCE_DIR}/inc/nvram"
    "${FP_SOURCE_DIR}/inc/cus"
    "${FP_SOURCE_DIR}/common/inc"	
    "${PROJECT_BINARY_DIR}/include/generated"
)

#
# list globel definitions 
#
fp_compile_definitions(
    "$<$<COMPILE_LANGUAGE:C>:__C_DEF__>"
    "$<$<COMPILE_LANGUAGE:C>:ENV_XSCALE>"
    "$<$<COMPILE_LANGUAGE:C>:PLAT_USE_THREADX>"
    ${GLOBLE_FEATURE_DEF}
)

#
# target C/ASM flags:
#

fp_compile_options(
    ${ARCH_FLAGS}
    "-g"
# del compl warn msg of the warn No:174 2830 3042 3043 161 2020-10-22 by guanzz
    "--apcs=/interwork"
    "$<$<COMPILE_LANGUAGE:C>:-O3>"
    "$<$<COMPILE_LANGUAGE:CXX>:-O3>"
	"$<$<COMPILE_LANGUAGE:C>:--gnu>"
    "$<$<COMPILE_LANGUAGE:CXX>:--gnu>"
    "$<$<COMPILE_LANGUAGE:C>:-Ospace>"
    "$<$<COMPILE_LANGUAGE:CXX>:-Ospace>"
    "$<$<COMPILE_LANGUAGE:C>:--feedback=${FEEDBACK_FILE}>"
    "$<$<COMPILE_LANGUAGE:CXX>:--feedback=${FEEDBACK_FILE}>"
    "$<$<COMPILE_LANGUAGE:ASM>:--cpreproc>"
    "$<$<COMPILE_LANGUAGE:ASM>:--cpreproc_opts=-D,__ASSEMBLY__>"	
    "$<$<COMPILE_LANGUAGE:ASM>:-g>"
    "$<$<COMPILE_LANGUAGE:ASM>:--apcs=/interwork>"
)


fp_compile_options(
    "$<$<COMPILE_LANGUAGE:C>:--split_sections>"
    "$<$<COMPILE_LANGUAGE:CXX>:--split_sections>"
)

#SET(CMAKE_ASM_ARMASM_FLAGS "--cpu Cortex-R5 --no_unaligned_access -g  --apcs /inter " )

#add_compile_options(--cpu Cortex-R5 --no_unaligned_access -g  --apcs /inter )

MESSAGE(STATUS "ASM_FLAGS       : ${ASM_FLAGS}")
MESSAGE(STATUS "CMAKE_ASM_COMPILER       : ${CMAKE_ASM_COMPILER}")

#
# add modules
#
#add_subdirectory(hal)
#add_subdirectory(cus)
add_subdirectory(os)
add_subdirectory(driver)
add_subdirectory(common)
add_subdirectory(bootloader)



set(fp_lnk
#  ${LINKERFLAGPREFIX},-Map=${PROJECT_BINARY_DIR}/${KERNEL_MAP_NAME}
#  -u_OffsetAbsSyms 
#  -u_ConfigAbsSyms
#  ${E_KERNEL_ENTRY}
#  ${LINKERFLAGPREFIX},--start-group 
#  ${LINKERFLAGPREFIX},--whole-archive
  ${FP_LIBS_PROPERTY}
#  ${LINKERFLAGPREFIX},--no-whole-archive
#  kernel
#  ${OFFSETS_O_PATH} 
#  ${LINKERFLAGPREFIX},--end-group
#  ${LIB_INCLUDE_DIR}
#  -L${PROJECT_BINARY_DIR}
#  ${TOOLCHAIN_LIBS}
)



#
# build bootloader
#

set(BOOTLOADER_OBJLIBLIST_FILE "${CMAKE_CURRENT_BINARY_DIR}/bootloader_objliblist.txt")
set(BOOTLOADER_OBJLIBLIST_FILE_ORIG "${BOOTLOADER_SOURCE_DIR}/bootloader_objliblist.txt")
set(BOOTLOADER_OBJLIBLIST_FILE_ALL_IN_ONE "${CMAKE_CURRENT_BINARY_DIR}/bootloader_all_in_one.txt")

MESSAGE(STATUS "BOOTLOADER_OBJLIBLIST_FILE       : ${BOOTLOADER_OBJLIBLIST_FILE}")
MESSAGE(STATUS "BOOTLOADER_OBJLIBLIST_FILE_ORIG       : ${BOOTLOADER_OBJLIBLIST_FILE_ORIG}")

    set(BOOTLOADER_TARGET "bootloader")
    set(BOOTLOADERCLEAN_TARGET "${BOOTLOADER_TARGET}clean")
 set(BOOTLOADER_LIB "${CMAKE_BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${BOOTLOADER_TARGET}${CMAKE_STATIC_LIBRARY_SUFFIX}")

    set(BOOTLOADER_PREFIX_STR_GROUP "./")
    set(BOOTLOADER_CONCAT_STR_GROUP "${CMAKE_BINARY_DIR}/bootloader/")

    set(BOOTLOADER_ALL_IN_ONE_TEXT
        #
        # export OBM_StartUp.c
        #    
        "${CMAKE_BINARY_DIR}/CMakeFiles/${FP_CURRENT_CATCHALL_LIB}.dir/bootloader/OBM_StartUp.o\n"
		"${CMAKE_BINARY_DIR}/CMakeFiles/${FP_CURRENT_CATCHALL_LIB}.dir/bootloader/vectors.o\n"
        "${BOOTLOADER_LIB}\n"
)

file(WRITE ${BOOTLOADER_OBJLIBLIST_FILE_ALL_IN_ONE} ${BOOTLOADER_ALL_IN_ONE_TEXT})

    add_custom_target(${BOOTLOADER_TARGET} ALL
        COMMAND ${CMAKE_COMMAND} -E echo "buiding bootloader target..."
        # armar.exe --create -cr libhal.a libstandalone.a libhelloexample.a
        # COMMAND ${CMAKE_AR} --create -cr ${HAL_LIB} $<TARGET_FILE:standalone> $<TARGET_FILE:helloexample>

        COMMAND  ${CMAKE_COMMAND}
                "-DINPUT_OBJLIBLIST_FILE=${BOOTLOADER_OBJLIBLIST_FILE_ORIG}"
                "-DOUTPUT_OBJLIBLIST_FILE=${BOOTLOADER_OBJLIBLIST_FILE}"
                "-DPREFIX_STR_GROUP=${BOOTLOADER_PREFIX_STR_GROUP}"
                "-DCONCAT_STR_GROUP=${BOOTLOADER_CONCAT_STR_GROUP}"
                -P ${FP_BASE}/scripts/cmake/gen_objliblist.cmake
        COMMAND ${CMAKE_AR} --create -cr ${BOOTLOADER_LIB} --via ${BOOTLOADER_OBJLIBLIST_FILE}
    )
    get_target_property(libs ${FP_CURRENT_INTERFACE} associated_libs)
    add_dependencies(${BOOTLOADER_TARGET} ${libs} ${FP_CURRENT_CATCHALL_LIB})

    add_dependencies(${FP_TARGET} ${BOOTLOADER_TARGET})


#
# set link flag
#
# armlink.exe -o ..\bin\Arbel_PMD2NONE_40M.axf
#             --via ..\obj_PMD2NONE\Arbel_PMD2NONE_targ_objliblist.txt 
#             --elf --scatter \csw\platform\dev_plat\build\Seagull_40MB_GB15_A0_Nezha3.sct
#             --map --symbols --info sizes,totals --list ..\bin\Arbel_PMD2NONE_40M.map --keep init.o(Header) --keep init.o(Vectors)


	list(APPEND
        pre_link_commands
        COMMAND ${CMAKE_COMMAND} -E echo "pre link..."
    )
    if (DEFINED CONFIG_LINK_CUSTOM_SCATTER_FILE)
        set(FP_SCATTER_FILE ${FP_BASE}/${CONFIG_SCATTER_FILE})
    else()
        set(SCAT_FILE "bootloader/bootloader_scatter.scat")
        set(FP_SCATTER_FILE ${FP_BASE}/${SCAT_FILE})
    endif()
	
	add_custom_command(
    TARGET ${FP_TARGET}
    PRE_LINK
    ${pre_link_commands}
	)



set(link_commands_flags
		"--via ${BOOTLOADER_OBJLIBLIST_FILE_ALL_IN_ONE} --elf --scatter ${FP_SCATTER_FILE}  --map --symbols --info sizes,totals --list --entry=Entry_point --keep version_block.o\(IMGVERBLOCK\)"
)


set_target_properties(${FP_TARGET} PROPERTIES LINK_FLAGS
    ${link_commands_flags}
)

set_property(TARGET ${FP_TARGET} APPEND PROPERTY LINK_DEPENDS
    ${BOOTLOADER_OBJLIBLIST_FILE}
)

#set_property(TARGET ${FP_TARGET} APPEND PROPERTY LINK_DEPENDS
#    ${FP_SCATTER_FILE}
#)

#
# post build
#
list_append_ifdef(
    CONFIG_BUILD_OUTPUT_BIN
    post_build_commands
	COMMAND ${CMAKE_COMMAND} -E echo "post_build_commands..."
    COMMAND ${CMAKE_OBJCOPY} --bin --output ${FP_TARGET}.bin ${FP_TARGET}.elf
)

list_append_ifdef(
    CONFIG_OUTPUT_DISASSEMBLY
    post_build_commands
    COMMAND ${CMAKE_OBJCOPY} --output ${FP_TARGET}.asm -acst ${FP_TARGET}.elf
)


add_custom_command(
    TARGET ${FP_TARGET}
    POST_BUILD
    ${post_build_commands}
)


set(FP_LINK "fp_link")
add_executable(${FP_LINK})
target_sources(${FP_LINK} PRIVATE
    ${EMPTY_FILE}
)

add_custom_command(
    TARGET ${FP_LINK}
    PRE_LINK
    ${pre_link_commands}
)
set_target_properties(${FP_LINK} PROPERTIES LINK_FLAGS
    ${link_commands_flags}
)
add_custom_command(
    TARGET ${FP_LINK}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rename ${FP_LINK}.elf ${FP_TARGET}.elf
    COMMAND ${CMAKE_COMMAND} -E rename ${FP_LINK}.map ${FP_TARGET}.map
    ${post_build_commands}
)


#
# clean extra files
#
set_property(DIRECTORY PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${FP_TARGET}.bin"
    "${CMAKE_CURRENT_BINARY_DIR}/${FP_TARGET}.map"
    "${CMAKE_CURRENT_BINARY_DIR}/${FP_TARGET}.asm"
)

#
# clean all
#
if (NOT DEFINED CONFIG_CREATE_HAL_TARGET)
    list(APPEND
        cleanall_commands
        COMMAND ${CMAKE_COMMAND} -E echo "cleaning hal..."
        COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}" --target clean
    )
else(DEFINED CONFIG_CREATE_HAL_TARGET)
    list_append_ifdef(
        CONFIG_CREATE_HAL_TARGET
        cleanall_commands
        COMMAND ${CMAKE_COMMAND} -E echo "cleaning hal..."
        COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}" --target ${HALCLEAN_TARGET}
    )
endif()

set(CLEANALL_TARGET "cleanall")
add_custom_target(${CLEANALL_TARGET}
    ${cleanall_commands}
)

#
# don't make the "install" target 
# depend on the "all" target
#
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)

#
# install hal
#
string(REPLACE ${HAL_SOURCE_DIR}
    ${CMAKE_INSTALL_PREFIX}/hal dest_path
    ${HAL_SOURCE_DIR})
install(DIRECTORY ${HAL_SOURCE_DIR}/ DESTINATION ${dest_path}
    FILES_MATCHING PATTERN "CMakeLists.txt"
                   PATTERN "*.cmake"
                   PATTERN "Kconfig"
                   PATTERN "*.sct"
                   PATTERN "*_objliblist.txt")
#
# install gui
# move to gui module's CMakeLists.txt
#

#
# install cus
#
string(REPLACE ${FP_SOURCE_DIR}/cus
    ${CMAKE_INSTALL_PREFIX}/cus dest_path
    ${FP_SOURCE_DIR}/cus)
install(CODE "MESSAGE(\"Sample install message \")")
if (DEFINED CONFIG_ENABLE_HAL_RELEASE_FOR_CP)
install(FILES ${FP_SOURCE_DIR}/cus/CMakeLists.txt ${FP_SOURCE_DIR}/cus/Kconfig DESTINATION ${dest_path})
install(DIRECTORY ${FP_SOURCE_DIR}/cus/evb_mHal			DESTINATION ${dest_path} PATTERN ".git" EXCLUDE)
else()
install(DIRECTORY ${FP_SOURCE_DIR}/cus/ DESTINATION ${dest_path})
endif()

install(FILES autobuild.bat cmake.bat CMakeLists.txt env.bat Kconfig DESTINATION ${CMAKE_INSTALL_PREFIX})

if (NOT DEFINED CONFIG_ENABLE_HAL_RELEASE_FOR_CP)
install(FILES cmd.exe DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()


#
# install common
#
string(REPLACE ${FP_SOURCE_DIR}/comnon
    ${CMAKE_INSTALL_PREFIX}/comnon dest_path
    ${FP_SOURCE_DIR}/comnon)
install(CODE "MESSAGE(\"Sample install message \")")
if (DEFINED CONFIG_ENABLE_HAL_RELEASE_FOR_CP)
install(FILES ${FP_SOURCE_DIR}/comnon/CMakeLists.txt ${FP_SOURCE_DIR}/comnon/Kconfig DESTINATION ${dest_path})
install(DIRECTORY ${FP_SOURCE_DIR}/comnon/evb_mHal			DESTINATION ${dest_path} PATTERN ".git" EXCLUDE)
else()
install(DIRECTORY ${FP_SOURCE_DIR}/comnon/ DESTINATION ${dest_path})
endif()

if(DEFINED CONFIG_INSTALL_CP)
#
# install cp
#
string(REPLACE ${FP_SOURCE_DIR}
    ${CMAKE_INSTALL_PREFIX} dest_path
    ${CP_SOURCE_DIR})
install(DIRECTORY ${CP_SOURCE_DIR}/ DESTINATION ${dest_path}
    PATTERN ".git" EXCLUDE)
endif()

if(DEFINED CONFIG_INSTALL_TOOL)
#
# install tool
#
string(REPLACE ${FP_SOURCE_DIR}
    ${CMAKE_INSTALL_PREFIX} dest_path
    ${TOOL_DIR})
if (DEFINED CONFIG_ENABLE_HAL_RELEASE_FOR_CP)
install(DIRECTORY ${TOOL_DIR}/MinGW 					DESTINATION ${dest_path} PATTERN ".git" EXCLUDE)
install(DIRECTORY ${TOOL_DIR}/cmake-3.14.3-win64-x64 	DESTINATION ${dest_path} PATTERN ".git" EXCLUDE)
install(DIRECTORY ${TOOL_DIR}/python-3.6.5-embed-amd64 	DESTINATION ${dest_path} PATTERN ".git" EXCLUDE)
install(DIRECTORY ${TOOL_DIR}/downloadtool 				DESTINATION ${dest_path} PATTERN ".git" EXCLUDE)
install(DIRECTORY ${TOOL_DIR}/coreutils-5.3.0-bin		DESTINATION ${dest_path} PATTERN ".git" EXCLUDE)
install(FILES ${TOOL_DIR}/install-x64-evb.bat ${TOOL_DIR}/GeneratorRD.py ${TOOL_DIR}/RDGenerator_dos.exe DESTINATION ${dest_path})
else()
install(DIRECTORY ${TOOL_DIR}/ DESTINATION ${dest_path} PATTERN ".git" EXCLUDE)
endif()
endif()

if(DEFINED CONFIG_INSTALL_SCRIPTS)
#
# install scripts
#
string(REPLACE ${FP_SOURCE_DIR}
    ${CMAKE_INSTALL_PREFIX} dest_path
    ${SCRIPTS_DIR})
install(DIRECTORY ${SCRIPTS_DIR}/ DESTINATION ${dest_path})

# after install scripts, modify config file if need
install(SCRIPT "${SCRIPTS_DIR}/cmake/post_scripts_install.cmake")
endif()
