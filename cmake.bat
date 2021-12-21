@rem
@rem useage:
@rem cmake.bat or cmake.bat -b board_name -f conf_file
@rem

@echo off
setlocal EnableDelayedExpansion

set "cur_dir=%~dp0"
set "build_dir=build\"

set parm_num=0
for %%a in (%*) do set /a parm_num+=1
if %parm_num% EQU 0 (
  goto paramSetting
)

set "option="
for %%a in (%*) do (
  if not defined option (
    set arg=%%a
    if "!arg:~0,1!" equ "-" set "option=!arg!"
  ) else (
    set "option!option!=%%a"
    set "option="
  )
)

SET option

:paramSetting
if defined option-b (
  echo Option -b given: "%option-b%"
  set "board_name=!option-b!"
) else (
  echo Option -b not given
  set "board_name=crane_evb"
)
echo board_name:!board_name!

if defined option-f (
  echo Option -f given: "%option-f%"
  set "conf_file=!option-f!"
) else (
  echo Option -f not given
  set "conf_file=!board_name!_defconfig"
)
echo conf_file:!conf_file!

if defined option-s (
  echo Option -s given: "%option-s%"
  set "cpsdk_dir=!option-s!"
) else (
  echo Option -s not given
  set "cpsdk_dir=cp"
)
echo cpsdk_dir:!cpsdk_dir!

if not exist %cur_dir%%build_dir%%board_name% (md %cur_dir%%build_dir%%board_name%)

cd %cur_dir%%build_dir%%board_name%

xcopy /Y %cur_dir%\scripts\build.bat %cur_dir%%build_dir%%board_name%\build.bat*

if exist %cur_dir%%build_dir%%board_name%\CMakeCache.txt (del CMakeCache.txt)

cmake -G "MinGW Makefiles" -DCPSDK=%cpsdk_dir% -DBOARD=%board_name% -DCONF_FILE=%conf_file% ..\.. -Wdev

pushd .
endlocal
popd