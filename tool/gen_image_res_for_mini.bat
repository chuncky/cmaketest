@echo off

set res_dir_name=asr-240x320
set root_dir=%~dp0..\
set gen_image_res_tool=%root_dir%tool\gen_image_res.exe
set gui_dir=%root_dir%gui
call %gen_image_res_tool% %gui_dir% %res_dir_name% -t mini