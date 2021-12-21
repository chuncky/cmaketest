@echo off

set  uirespkg_root_dir=%~dp0..\..\
set  uirespkg_release_ori_dir=%cd%
set  uirespkg_release_bat_dir=%~dp0
set  uirespkg_release_1_dir=%uirespkg_release_bat_dir%..\..\gui\mgapollo\respkg\build\obj
set  uirespkg_release_2_dir=%uirespkg_release_bat_dir%..\..\gui\targets\resource\fonts
set  uirespkg_release_3_dir=%uirespkg_release_bat_dir%..\..\gui\mgapollo\respkg\src
set  uirespkg_release_pyhon_path=%uirespkg_root_dir%tool\python-3.6.5-embed-amd64
set  uirespkg_release_font=%1

echo release uirespkg ....
echo %1
echo %uirespkg_release_font%

set  font_file=0
if "%uirespkg_release_font%"=="__MMI_FONT_CN__" (
	set font_file=YH2312F.ttf
)
if "%uirespkg_release_font%"=="__MMI_FONT_TW__" (
	set font_file=YH18030F.ttf
)
if "%uirespkg_release_font%"=="__MMI_FONT_LF__" (
	set font_file=SansSerifLatinFullSetF.ttf
)
if "%uirespkg_release_font%"=="__MMI_FONT_ARBF__" (
	set font_file=SansSerifArabicF.ttf
)
if "%uirespkg_release_font%"=="__MMI_FONT_THAI__" (
	set font_file=SansSerifThaiF.ttf
)
echo %font_file%

if exist mkres (
	if exist %uirespkg_release_1_dir% (
		xcopy /Y %uirespkg_release_1_dir% mkres
		rmdir /S /Q %uirespkg_release_1_dir%
	)
	if exist %uirespkg_release_2_dir% (
		xcopy /Y %uirespkg_release_2_dir% mkres
	)
	if exist %uirespkg_release_3_dir%\autoconf.h (
		xcopy /Y %uirespkg_release_3_dir%\autoconf.h mkres
		del /Q %uirespkg_release_3_dir%\autoconf.h
	)
	if %font_file% EQU 0 (
		%uirespkg_release_pyhon_path%\python %uirespkg_release_bat_dir%makerespkg.py --first mkres\mkres.bin -o uirespkg.bin
	) else (
		%uirespkg_release_pyhon_path%\python %uirespkg_release_bat_dir%makerespkg.py --first mkres\mkres.bin --second mkres\%font_file% -o uirespkg.bin
	)
)

echo release uirespkg done
