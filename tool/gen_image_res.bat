@echo off

set res_dir_name=asr-240x320
set root_dir=%~dp0..\
set gen_image_res_tool=%root_dir%tool\gen_image_res.exe
set gui_dir=%root_dir%gui
call %gen_image_res_tool% %gui_dir% %res_dir_name%

set res_dir_name=asr-176x220
set root_dir=%~dp0..\
set gen_image_res_tool=%root_dir%tool\gen_image_res.exe
set gui_dir=%root_dir%gui
call %gen_image_res_tool% %gui_dir% %res_dir_name%

set res_dir_name=asr-128x160
set root_dir=%~dp0..\
set gen_image_res_tool=%root_dir%tool\gen_image_res.exe
set gui_dir=%root_dir%gui
call %gen_image_res_tool% %gui_dir% %res_dir_name%

set res_dir_name=asr-320x240-FWP
set root_dir=%~dp0..\
set gen_image_res_tool=%root_dir%tool\gen_image_res.exe
set gui_dir=%root_dir%gui
call %gen_image_res_tool% %gui_dir% %res_dir_name%

set res_dir_name=asr-128x64-FWP
set root_dir=%~dp0..\
set gen_image_res_tool=%root_dir%tool\gen_image_res.exe
set gui_dir=%root_dir%gui
call %gen_image_res_tool% %gui_dir% %res_dir_name%

set res_dir_name=asr-320x240-FP
set root_dir=%~dp0..\
set gen_image_res_tool=%root_dir%tool\gen_image_res.exe
set gui_dir=%root_dir%gui
call %gen_image_res_tool% %gui_dir% %res_dir_name%
