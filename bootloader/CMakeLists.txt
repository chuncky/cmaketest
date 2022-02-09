

list(APPEND GLOBLE_FEATURE_DEF  "TF_MBEDTLS_KEY_ALG_ID=1")
list(APPEND GLOBLE_FEATURE_DEF  "TF_MBEDTLS_HASH_ALG_ID=1")

fp_library_include_directories(
    "${FP_BASE}/bootloader/inc"
	"${FP_BASE}/common/inc"
)
fp_sources(
  main.c
  OBM_StartUp.s
  vectors.s
)

#add_subdirectory_ifdef(CONFIG_BUILD_EXAMPLE cmake_example)
#add_subdirectory(chip)
#add_subdirectory_ifdef(CONFIG_BUILD_MULTIMEDIA_SYSTEM media)
if (0)
if (DEFINED CONFIG_BUILD_MULTIMEDIA_SYSTEM)
else()
fp_sources(
  media/media_interface_dummy.c
  media/mci/mci_Dummy.cpp
  media/hal/src/mci_lcd_asr.c
)
endif()
endif()

add_subdirectory(src)

if (0)
add_subdirectory(log)
add_subdirectory(base)
add_subdirectory(os)
add_subdirectory(task)
add_subdirectory_ifdef(CONFIG_SUPPORT_UIDBG dbg)
add_subdirectory(modem)
add_subdirectory_ifdef(CONFIG_BUILD_HAL_TEST test)
add_subdirectory_ifdef(CONFIG_MMI_SUPPORT_SMS sms)
add_subdirectory(sys)
add_subdirectory(device)
add_subdirectory_ifdef(CONFIG_BUILD_ATA_TEST hwtest)
add_subdirectory_ifdef(CONFIG_MMI_SUPPORT_BLUETOOTH bluetooth)

add_subdirectory_ifdef(CONFIG_SUPPORT_QRCODE iot/qrcode)
add_subdirectory_ifdef(CONFIG_JRTC_VIDEO_CALL videocall)
add_subdirectory_ifdef(CONFIG_ENABLE_HAL_KAWAII_MQTT connectivity/netapp/mqttclient)
#add_subdirectory(sdk_adapter)
endif()
fp_generate_objliblist_file()

if (0)
install(DIRECTORY test/media/sample/ DESTINATION ${CMAKE_INSTALL_PREFIX}/hal/test/media/sample/)
install(DIRECTORY media/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/hal/media/include/)
install(DIRECTORY modem/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/hal/modem/include/)
install(DIRECTORY media/external_codecs/libs/ DESTINATION ${CMAKE_INSTALL_PREFIX}/hal/media/external_codecs/libs/)
install(FILES modem/libhal__modem__asr-ril.a DESTINATION ${CMAKE_INSTALL_PREFIX}/hal/modem/)
install(FILES dbg/libhal_libdbg.a DESTINATION ${CMAKE_INSTALL_PREFIX}/hal/dbg/)
if (DEFINED CONFIG_MMI_SUPPORT_SMS)
install(FILES sms/libmUI_sms_src_hal.a DESTINATION ${CMAKE_INSTALL_PREFIX}/hal/sms/)
endif()
endif()

