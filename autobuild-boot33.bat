@rem
@rem useage:
@rem autobuild.bat or autobuild.bat -b board_name -f conf_file -t [daily_build/gen_rel/bld_rel]
@rem

@rem
@rem copy from env.bat
@rem

@echo off

set cur_dir=%~dp0
echo %cur_dir%

set PATH_PREV=%PATH%

@rem set CMAKE_PATH=%cur_dir%tool\cmake-3.11.1-win64-x64\bin
set CMAKE_PATH=%cur_dir%tool\cmake-3.14.3-win64-x64\bin
set CC_PATH=C:\Program Files\DS-5 v5.26.2\sw\ARMCompiler5.06u4\bin
set MAKE_PATH=%cur_dir%tool\MinGW\bin
set PYTHON_PATH=%cur_dir%tool\python-3.6.5-embed-amd64
set MSYS2_ROOT=C:\tools\msys64
set COREUTILS_PATH=%cur_dir%tool\coreutils-5.3.0-bin\bin
set PATH=%PYTHON_PATH%;%PATH%;%CMAKE_PATH%;%CC_PATH%;%MAKE_PATH%;%COREUTILS_PATH%
echo %PATH%

@rem
@rem ENV variables that cmake must use
@rem
set FP_BASE=%~dp0
set ASM=armasm.exe
set CC=armcc.exe
set CXX=armcc.exe

@rem
@rem copy from cmake.bat
@rem
@echo off
setlocal EnableDelayedExpansion

set "cur_dir=%~dp0"
set "build_dir=build\"
set "rel_build_dir=rel\build"

set parm_num=0
echo %*
for %%a in (%*) do set /a parm_num+=1
if %parm_num% EQU 0 (
  goto paramSetting
)
echo %parm_num%

set "option="
for %%a in (%*) do (
  echo %%a
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

if defined option-t (
  echo Option -t given: "%option-t%"
  set "build_type=!option-t!"
) else (
  echo Option -t not given
  set "build_type=daily_build"
)
echo build_type:!build_type!

if defined option-s (
  echo Option -s given: "%option-s%"
  set "cpsdk_dir=!option-s!"
) else (
  echo Option -s not given
  set "cpsdk_dir=cp"
)
echo cpsdk_dir:!cpsdk_dir!

echo %cur_dir%%build_dir%%board_name%

if not exist %cur_dir%%build_dir%%board_name% (md %cur_dir%%build_dir%%board_name%)
  
cd %cur_dir%%build_dir%%board_name% 

xcopy /Y %cur_dir%\scripts\build.bat %cur_dir%%build_dir%%board_name%\build.bat*

if /i "%build_type%"=="no_ui" (
xcopy /Y %cur_dir%\scripts\build_no_ui.bat %cur_dir%%build_dir%%board_name%\build.bat*
)

if exist %cur_dir%%build_dir%%board_name%\CMakeCache.txt (del CMakeCache.txt)

cmake -G "MinGW Makefiles"  -DCPSDK=%cpsdk_dir% -DBOARD=%board_name% -DCONF_FILE=%conf_file% ..\.. -Wdev

echo build_type:%build_type%
if /i "%build_type%"=="daily_build" goto daily_build
if /i "%build_type%"=="no_ui" 		goto daily_build
if /i "%build_type%"=="gen_rel"     goto gen_rel
if /i "%build_type%"=="bld_rel"     goto bld_rel
if /i "%build_type%"=="framework"   goto framework
echo build type not found!
goto byebye

:daily_build
echo daily_build
build.bat
goto byebye
  
:gen_rel
echo gen_rel
make && make install
cd %cur_dir%%build_dir%
rd /q/s -rf %rel_build_dir% >nul 2>nul
cd %cur_dir%
goto byebye
 
:bld_rel
echo bld_rel
make
goto byebye

:framework
echo framework
make hal
goto byebye

:byebye
echo byebye
set PATH=%PATH_PREV%

endlocal