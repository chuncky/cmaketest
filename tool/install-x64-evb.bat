@echo off
setlocal EnableDelayedExpansion

set CUR_DIR=%~dp0
set FP_BASE=%CUR_DIR%..\
set BUILD_DIR=%FP_BASE%build\crane_evb_z2\
set LOG_FILE=%BUILD_DIR%arelease.log

@rem echo %CUR_DIR%
@rem echo %FP_BASE%
@rem echo %BUILD_DIR%
@rem echo %LOG_FILE%

reg Query "HKLM\Hardware\Description\System\CentralProcessor\0" | find /i "x86" > NUL && set OS=32BIT || set OS=64BIT

if %OS%==32BIT (
    set "matchstr=x86"
) else (
    set "matchstr=x64"
)

@echo off
for /f "delims=" %%j in ('dir /ad/b %FP_BASE%tool\downloadtool') do (echo %%j | findstr %matchstr% >NUL && set "download_tool=%FP_BASE%tool\downloadtool\%%j")
echo %download_tool%

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
if defined option-c (
@rem  echo Option -c given: "%option-c%"
  set "cus_board=!option-c!"
) else (
@rem  echo Option -c not given
  set "cus_board=evb"
)
echo cus_board:!cus_board!

if defined option-s (
@rem  echo Option -s given: "%option-s%"
  set "cus_sdk=!option-s!"
) else (
@rem  echo Option -s not given
  set "cus_sdk=cp"
)
echo cus_sdk:!cus_sdk!

set "DSPRF_DIR=%FP_BASE%cus\evb\images"

set "CPSDK_DIR=%FP_BASE%%cus_sdk%"
set "images_dir=%download_tool%\images"

for /f  %%i in ('dir /s/b %CPSDK_DIR%\startup\logo\release\ ^| findstr bin') do set LOG_BIN=%%i
echo %LOG_BIN%

if "!cus_board!" equ "evb" (
    if exist %BUILD_DIR%crane_evb.bin  copy %BUILD_DIR%crane_evb.bin %images_dir%\cp.bin >> %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\boot33.bin  copy %CPSDK_DIR%\tavor\Arbel\build\boot33.bin %images_dir% >> %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\apn.bin  copy %CPSDK_DIR%\tavor\Arbel\build\apn.bin %images_dir% > %LOG_FILE%
	if exist %LOG_BIN%  copy %LOG_BIN% %images_dir%\logo.bin >> %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\updater_lcd_adapt_adups.bin  copy %CPSDK_DIR%\tavor\Arbel\build\updater_lcd_adapt_adups.bin %images_dir%\updater.bin > %LOG_FILE%
    if exist %DSPRF_DIR%\dsp.bin  copy %DSPRF_DIR%\dsp.bin %images_dir% >> %LOG_FILE%
    if exist %DSPRF_DIR%\rf.bin   copy %DSPRF_DIR%\rf.bin  %images_dir% >> %LOG_FILE%
    if exist %BUILD_DIR%uirespkg.bin  copy %BUILD_DIR%uirespkg.bin %images_dir%\uirespkg.bin >> %LOG_FILE%
)
if "!cus_board!" equ "evb_g" (
    if exist %BUILD_DIR%crane_evb.bin  copy %BUILD_DIR%crane_evb.bin %images_dir%\cp.bin >> %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\boot33.bin  copy %CPSDK_DIR%\tavor\Arbel\build\boot33.bin %images_dir% >> %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\apn.bin  copy %CPSDK_DIR%\tavor\Arbel\build\apn.bin %images_dir% > %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\logo_lcd_adapt.bin  copy %CPSDK_DIR%\tavor\Arbel\build\logo_lcd_adapt.bin %images_dir%\logo.bin >> %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\updater_lcd_adapt_adups.bin  copy %CPSDK_DIR%\tavor\Arbel\build\updater_lcd_adapt_adups.bin %images_dir%\updater.bin > %LOG_FILE%
    if exist %FP_BASE%cus\%cus_board%\images\dsp.bin  copy %FP_BASE%cus\%cus_board%\images\dsp.bin %images_dir% >> %LOG_FILE%
    if exist %FP_BASE%cus\%cus_board%\images\rf.bin   copy %FP_BASE%cus\%cus_board%\images\rf.bin  %images_dir% >> %LOG_FILE%
    if exist %BUILD_DIR%uirespkg.bin  copy %BUILD_DIR%uirespkg.bin %images_dir%\uirespkg.bin >> %LOG_FILE%
)
if "!cus_board!" equ "evb_g_a0" (
    if exist %BUILD_DIR%crane_evb.bin  copy %BUILD_DIR%crane_evb.bin %images_dir%\cp.bin >> %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\boot33.bin  copy %CPSDK_DIR%\tavor\Arbel\build\boot33.bin %images_dir% >> %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\apn.bin  copy %CPSDK_DIR%\tavor\Arbel\build\apn.bin %images_dir% > %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\logo_lcd_adapt.bin  copy %CPSDK_DIR%\tavor\Arbel\build\logo_lcd_adapt.bin %images_dir%\logo.bin >> %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\updater_lcd_adapt_adups.bin  copy %CPSDK_DIR%\tavor\Arbel\build\updater_lcd_adapt_adups.bin %images_dir%\updater.bin > %LOG_FILE%
    if exist %FP_BASE%cus\evb_g\images\dsp.bin  copy %FP_BASE%cus\evb_g\images\dsp.bin %images_dir% >> %LOG_FILE%
    if exist %FP_BASE%cus\%cus_board%\images\rf.bin   copy %FP_BASE%cus\%cus_board%\images\rf.bin  %images_dir% >> %LOG_FILE%
    if exist %BUILD_DIR%uirespkg.bin  copy %BUILD_DIR%uirespkg.bin %images_dir%\uirespkg.bin >> %LOG_FILE%
)
if "!cus_board!" equ "evb_sdk009" (
    if exist %BUILD_DIR%crane_evb.bin  copy %BUILD_DIR%crane_evb.bin %images_dir%\cp.bin >> %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\boot33.bin  copy %CPSDK_DIR%\tavor\Arbel\build\boot33.bin %images_dir% >> %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\apn.bin  copy %CPSDK_DIR%\tavor\Arbel\build\apn.bin %images_dir% > %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\logo_lcd_adapt.bin  copy %CPSDK_DIR%\tavor\Arbel\build\logo_lcd_adapt.bin %images_dir%\logo.bin >> %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\updater_lcd_adapt_adups.bin  copy %CPSDK_DIR%\tavor\Arbel\build\updater_lcd_adapt_adups.bin %images_dir%\updater.bin > %LOG_FILE%
    if exist %FP_BASE%cus\%cus_board%\images\dsp.bin  copy %FP_BASE%cus\%cus_board%\images\dsp.bin %images_dir% >> %LOG_FILE%
    if exist %FP_BASE%cus\%cus_board%\images\rf.bin   copy %FP_BASE%cus\%cus_board%\images\rf.bin  %images_dir% >> %LOG_FILE%
    if exist %BUILD_DIR%uirespkg.bin  copy %BUILD_DIR%uirespkg.bin %images_dir%\uirespkg.bin >> %LOG_FILE%
)
if "!cus_board!" equ "cus_b" (
    if exist %BUILD_DIR%crane_evb.bin  copy %BUILD_DIR%crane_evb.bin %images_dir%\cp.bin >> %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\boot33.bin  copy %CPSDK_DIR%\tavor\Arbel\build\boot33.bin %images_dir% >> %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\apn.bin  copy %CPSDK_DIR%\tavor\Arbel\build\apn.bin %images_dir% >> %LOG_FILE%
    if exist %FP_BASE%cus\evb\images\dsp.bin  copy %FP_BASE%cus\evb\images\dsp.bin %images_dir% >> %LOG_FILE%
    if exist %FP_BASE%cus\%cus_board%\images\updater.bin  copy %FP_BASE%cus\%cus_board%\images\updater.bin %images_dir% >> %LOG_FILE%
    if exist %FP_BASE%cus\%cus_board%\images\logo.bin  copy %FP_BASE%cus\%cus_board%\images\logo.bin %images_dir% >> %LOG_FILE%
    if exist %FP_BASE%cus\%cus_board%\images\rf.bin  copy %FP_BASE%cus\%cus_board%\images\rf.bin %images_dir% >> %LOG_FILE%
    if exist %BUILD_DIR%uirespkg.bin  copy %BUILD_DIR%uirespkg.bin %images_dir%\uirespkg.bin >> %LOG_FILE%
)
if "!cus_board!" equ "cus_b" if "!cus_sdk!" equ "cp009" (
    if exist %FP_BASE%cus\evb_sdk009\images\dsp.bin  copy %FP_BASE%cus\evb_sdk009\images\dsp.bin %images_dir% >> %LOG_FILE%
)
if "!cus_board!" equ "cus_v" (
    if exist %BUILD_DIR%crane_evb.bin  copy %BUILD_DIR%crane_evb.bin %images_dir%\cp.bin >> %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\boot33.bin  copy %CPSDK_DIR%\tavor\Arbel\build\boot33.bin %images_dir% >> %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\apn.bin  copy %CPSDK_DIR%\tavor\Arbel\build\apn.bin %images_dir% >> %LOG_FILE%
    if exist %FP_BASE%cus\evb\images\dsp.bin  copy %FP_BASE%cus\evb\images\dsp.bin %images_dir% >> %LOG_FILE%
    if exist %FP_BASE%cus\%cus_board%\images\updater.bin  copy %FP_BASE%cus\%cus_board%\images\updater.bin %images_dir% >> %LOG_FILE%
    if exist %FP_BASE%cus\%cus_board%\images\logo.bin  copy %FP_BASE%cus\%cus_board%\images\logo.bin %images_dir% >> %LOG_FILE%
    if exist %FP_BASE%cus\%cus_board%\images\rf.bin  copy %FP_BASE%cus\%cus_board%\images\rf.bin %images_dir% >> %LOG_FILE%
    if exist %BUILD_DIR%uirespkg.bin  copy %BUILD_DIR%uirespkg.bin %images_dir%\uirespkg.bin >> %LOG_FILE%
)
if "!cus_board!" equ "evb_m" (
    if exist %BUILD_DIR%crane_evb.bin  copy %BUILD_DIR%crane_evb.bin %images_dir%\cp.bin >> %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\boot33.bin  copy %CPSDK_DIR%\tavor\Arbel\build\boot33.bin %images_dir% >> %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\apn.bin  copy %CPSDK_DIR%\tavor\Arbel\build\apn.bin %images_dir% > %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\updater_lcd_adapt_adups.bin  copy %CPSDK_DIR%\tavor\Arbel\build\updater_lcd_adapt_adups.bin %images_dir%\updater.bin > %LOG_FILE%
    if exist %FP_BASE%cus\%cus_board%\images\logo.bin  copy %FP_BASE%cus\%cus_board%\images\logo.bin %images_dir% >> %LOG_FILE%
    if exist %FP_BASE%cus\%cus_board%\images\dsp.bin  copy %FP_BASE%cus\%cus_board%\images\dsp.bin %images_dir% >> %LOG_FILE%
    if exist %FP_BASE%cus\%cus_board%\images\rf.bin   copy %FP_BASE%cus\%cus_board%\images\rf.bin  %images_dir% >> %LOG_FILE%
    if exist %BUILD_DIR%uirespkg.bin  copy %BUILD_DIR%uirespkg.bin %images_dir%\uirespkg.bin >> %LOG_FILE%
)
if "!cus_board!" equ "cus_v" if "!cus_sdk!" equ "cp009" (
    if exist %FP_BASE%cus\evb_sdk009\images\dsp.bin  copy %FP_BASE%cus\evb_sdk009\images\dsp.bin %images_dir% >> %LOG_FILE%
)
if "!cus_board!" equ "cus_x" (
    if exist %BUILD_DIR%crane_evb.bin  copy %BUILD_DIR%crane_evb.bin %images_dir%\cp.bin >> %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\boot33.bin  copy %CPSDK_DIR%\tavor\Arbel\build\boot33.bin %images_dir% >> %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\apn.bin  copy %CPSDK_DIR%\tavor\Arbel\build\apn.bin %images_dir% >> %LOG_FILE%
    if exist %FP_BASE%cus\evb_g\images\dsp.bin  copy %FP_BASE%cus\evb_g\images\dsp.bin  %images_dir% >> %LOG_FILE%
    if exist %FP_BASE%cus\%cus_board%\images\updater.bin  copy %FP_BASE%cus\%cus_board%\images\updater.bin %images_dir% >> %LOG_FILE%
    if exist %FP_BASE%cus\%cus_board%\images\logo.bin  copy %FP_BASE%cus\%cus_board%\images\logo.bin %images_dir% >> %LOG_FILE%
    if exist %FP_BASE%cus\%cus_board%\images\rf.bin  copy %FP_BASE%cus\%cus_board%\images\rf.bin %images_dir% >> %LOG_FILE%
    if exist %BUILD_DIR%uirespkg.bin  copy %BUILD_DIR%uirespkg.bin %images_dir%\uirespkg.bin >> %LOG_FILE%
)
if "!cus_board!" equ "evb_fwp" (
    if exist %BUILD_DIR%crane_evb.bin  copy %BUILD_DIR%crane_evb.bin %images_dir%\cp.bin >> %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\boot33.bin  copy %CPSDK_DIR%\tavor\Arbel\build\boot33.bin %images_dir% >> %LOG_FILE%
    if exist %CPSDK_DIR%\tavor\Arbel\build\apn.bin  copy %CPSDK_DIR%\tavor\Arbel\build\apn.bin %images_dir% >> %LOG_FILE%
    if exist %DSPRF_DIR%\dsp.bin  copy %DSPRF_DIR%\dsp.bin  %images_dir% >> %LOG_FILE%
    if exist %FP_BASE%cus\%cus_board%\images\updater.bin  copy %FP_BASE%cus\%cus_board%\images\updater.bin %images_dir% >> %LOG_FILE%
    if exist %FP_BASE%cus\%cus_board%\images\logo.bin  copy %FP_BASE%cus\%cus_board%\images\logo.bin %images_dir% >> %LOG_FILE%
    if exist %DSPRF_DIR%\rf.bin   copy %DSPRF_DIR%\rf.bin  %images_dir% >> %LOG_FILE%
    if exist %BUILD_DIR%uirespkg.bin  copy %BUILD_DIR%uirespkg.bin %images_dir%\uirespkg.bin >> %LOG_FILE%
)


python GeneratorRD.py %FP_BASE%cus\!cus_board! >> %LOG_FILE%
copy %FP_BASE%cus\!cus_board!\images\ReliableData.bin %images_dir% >> %LOG_FILE%

set "product_type=CRANE_A0_16MB"
set "product=ASR_CRANE_EVB"
REM if "!cus_sdk!" equ "cpLWG" (
    if "!cus_board!" equ "cus_x" (
        set "product_type=CRANEGM_A0_16+8MB"
    ) else if "!cus_board!" equ "evb_g_a0" (
        set "product_type=CRANEGM_A0_16+8MB"
    )  else if "!cus_board!" equ "evb_g" (
        set "product_type=CRANEG_Z2_32+8MB"
    )  else if "!cus_board!" equ "evb_m" (
        set "product_type=CRANEGM_A0_16MB"
    ) else (
        set "product_type=CRANE_A0_16MB"
    )
REM )
set "zip_file=!product!_!product_type!.zip"

echo "create zip !zip_file!..."
set "release_tool=%download_tool%\arelease"
call %release_tool% -c %download_tool%\config\ -I %images_dir%\ -g -p !product! -v !product_type! %zip_file% >> %LOG_FILE%
if exist %CUR_DIR%%zip_file% move %CUR_DIR%%zip_file% %BUILD_DIR%

if "!cus_board!" equ "evb" if "!cus_sdk!" equ "cp" (
    set "zip_dcxo_file=!product!_!product_type!_DCXO.zip"
    set "cus_board=evb_dcxo"
    echo "create zip !zip_dcxo_file!..."

    python GeneratorRD.py %FP_BASE%cus\!cus_board! >> %LOG_FILE%
    copy %FP_BASE%cus\!cus_board!\images\ReliableData.bin %images_dir% >> %LOG_FILE%

    call %release_tool% -c %download_tool%\config\ -I %images_dir%\ -g -p !product! -v !product_type! !zip_dcxo_file! >> %LOG_FILE%
    if exist %CUR_DIR%!zip_dcxo_file! move %CUR_DIR%!zip_dcxo_file! %BUILD_DIR%
)

if "!cus_board!" equ "evb_sdk009" if "!cus_sdk!" equ "cp009" (
    set "zip_dcxo_file=!product!_!product_type!_DCXO.zip"
    set "cus_board=evb_sdk009_dcxo"
    echo "create zip !zip_dcxo_file!..."

    python GeneratorRD.py %FP_BASE%cus\!cus_board! >> %LOG_FILE%
    copy %FP_BASE%cus\!cus_board!\images\ReliableData.bin %images_dir% >> %LOG_FILE%

    call %release_tool% -c %download_tool%\config\ -I %images_dir%\ -g -p !product! -v !product_type! !zip_dcxo_file! >> %LOG_FILE%
    if exist %CUR_DIR%!zip_dcxo_file! move %CUR_DIR%!zip_dcxo_file! %BUILD_DIR%
)

if "!cus_board!" equ "evb_g" (
    set "zip_dcxo_file=!product!_!product_type!_DCXO.zip"
    set "cus_board=evb_g_dcxo"
    echo "create zip !zip_dcxo_file!..."

    python GeneratorRD.py %FP_BASE%cus\!cus_board! >> %LOG_FILE%
    copy %FP_BASE%cus\!cus_board!\images\ReliableData.bin %images_dir% >> %LOG_FILE%

    call %release_tool% -c %download_tool%\config\ -I %images_dir%\ -g -p !product! -v !product_type! !zip_dcxo_file! >> %LOG_FILE%
    if exist %CUR_DIR%!zip_dcxo_file! move %CUR_DIR%!zip_dcxo_file! %BUILD_DIR%
)
