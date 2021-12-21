@echo off
setlocal enableextensions enabledelayedexpansion


set parm_num=0
for %%a in (%*) do set /a parm_num+=1
if %parm_num% EQU 0 (
  set TARGET_NAME=crane_evb
) else (
  if %parm_num% EQU 1 (
    set TARGET_NAME=%1%
  ) else (
    echo ****************************
    echo usage: build.bat [target_name]
    echo build.bat
    echo build.bat crane_evb
    echo build.bat cleanall
    echo build.bat hal
    echo build.bat halclean
    echo build.bat gui
    echo build.bat guiclean
    echo build.bat cp
    echo build.bat cpclean
    echo ****************************
    pause
    exit 1
  )
)

set BUILD_GUI=0


set totalcores=Unknown
for /F %%A in ('wmic cpu get numberofcores') do @if %%A GTR 0 set totalcores=%%A
set /a N=2*totalcores
echo NumberOfCores=%totalcores%, N=%N%

set TEE_CMD=wtee.exe
@rem set TEE_CMD=tee.exe

  if not x%TARGET_NAME:crane_evb=%==x%TARGET_NAME% (
    @rem build.bat
    @rem build.bat crane_evb
    (cmake --build . --target bootloader -j%N% & call echo %%^^ERRORLEVEL%% ^>myError.txt) 2>&1 | %TEE_CMD% bootloader_build.log
    for /f %%A in (myError.txt) do (
      echo hal build returned %%A
      if %%A NEQ 0 goto byebye
    )

    make fp_link 2>&1 | %TEE_CMD% fp_link_build.log
  ) else (
      @rem build.bat hal
      @rem build.bat halclean
      @rem build.bat gui
      @rem build.bat guiclean
      @rem build.bat cleanall
      @rem build.bat fp_link
      (cmake --build . --target %TARGET_NAME% -j%N% & call echo %%^^ERRORLEVEL%% ^>myError.txt) 2>&1 | %TEE_CMD% %TARGET_NAME%_build.log
      for /f %%A in (myError.txt) do (
        echo %TARGET_NAME% build returned %%A
        if %%A NEQ 0 goto byebye
      )
  )


:byebye
