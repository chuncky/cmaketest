@echo off

set root_dir=%~dp0..\
set extract_strings_from_xlsx_tool=%root_dir%tool\extract_strings_from_xlsx.exe
set xlsx_file=%root_dir%gui\targets\resource\strings_xls\asr-240x320\asr-204x320-fp_strings.xlsx
set output_dir=asr-240x320
call %extract_strings_from_xlsx_tool% %xlsx_file% %output_dir% %root_dir% -t evb
