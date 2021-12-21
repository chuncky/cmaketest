if (DEFINED CONFIG_MINIPHONE)
list(APPEND GLOBLE_FEATURE_DEF  "CONFIG_MINIPHONE=1")
endif()
if(0)
fp_sources(
  hal_init.c
)
endif()


#add_subdirectory_ifdef(CONFIG_BUILD_EXAMPLE cmake_example)
#add_subdirectory_ifdef(CONFIG_MMI_SUPPORT_BLUETOOTH bluetooth)
#add_subdirectory(task)

add_subdirectory(lcd)

fp_generate_objliblist_file()

if(0)

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