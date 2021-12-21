@echo off

set  uirespkg_gen_bin_ori_dir=%cd%
set  uirespkg_gen_bin_bat_dir=%~dp0
set  uirespkg_gen_bin_top_dir=%uirespkg_gen_bin_bat_dir%..\..\gui\mgapollo\respkg\build

echo generate uirespkg ....
echo %1 %2
REM call %uirespkg_gen_bin_bat_dir%respkg_gen_code.bat %1
cd   %uirespkg_gen_bin_top_dir% && call env.bat && make clean && make UI_RESPKG_CT_ERES=%1 && cd %uirespkg_gen_bin_ori_dir%
call %uirespkg_gen_bin_bat_dir%respkg_release.bat %2
echo generate uirespkg done
