::Note: must excute build_updater.bat in DS-5 env. 
::devide by LCD_TYPE and FOTA_TYPE
::naming rule: updater_[lcdtype]_[fota_type].bin

@echo off

::define some variables
set DIR_RELEASE=..\release
set DIR_README_UPDATER=..

::create release directory and updater_axf_map directory in updater directory
::1. create release directory in updater directory
if not exist %DIR_RELEASE% md %DIR_RELEASE%
::2. create updater_axf_map directory in updater\release directory
if not exist %DIR_RELEASE%\updater_axf_map md %DIR_RELEASE%\updater_axf_map

del /s /q /f %DIR_RELEASE%\*
::first copy updater_readme.txt
copy /Y %DIR_README_UPDATER%\readme_updater.txt %DIR_RELEASE%\

::*********************************************
set SMALL_CODE=UPDATER
::*********************************************

::NOTE
::open comment(goto START and goto SUCCESS) to control build specilize updater*.bin
::goto START

::**********************************2. WATCHLCD***************************************************
::START
::2.1 USE_FOTA:REDSTONE_AND_ALI
make clean
set LCD_TYPE=WATCHLCDST7789
set FOTA_TYPE=REDSTONE_AND_ALI
make 
copy /Y updater*.bin %DIR_RELEASE%\
copy /Y updater*.axf %DIR_RELEASE%\updater_axf_map\
copy /Y updater*.map %DIR_RELEASE%\updater_axf_map\

if not exist %DIR_RELEASE%\updater_watchlcdst7789_redstone_and_ali.bin goto ERROR
::goto SUCCESS

::START
::2.2 ADUPS
make clean
set LCD_TYPE=WATCHLCDST7789
set FOTA_TYPE=ADUPS
make 
copy /Y updater*.bin %DIR_RELEASE%\
copy /Y updater*.axf %DIR_RELEASE%\updater_axf_map\
copy /Y updater*.map %DIR_RELEASE%\updater_axf_map\

if not exist %DIR_RELEASE%\updater_watchlcdst7789_adups.bin goto ERROR
goto SUCCESS


:ERROR
::make clean
echo ****************************************************************
echo ***********************  ERROR  ********************************
echo ****************************************************************
goto END

:SUCCESS
make clean
echo ****************************************************************
echo ***********************  SUCCESS  ******************************
echo ****************************************************************

:END
