@echo off

set cur_dir=%~dp0
set cp_dir=%cur_dir%..\cp

subst X: /d
:unmount_X
if exist X: (
  echo unmount X...
  ping 192.0.2.2 -n 1 -w 1000 > nul
  goto unmount_X
)

subst X: %cp_dir%
:mount_X
if not exist X: (
  echo mount X...
  ping 192.0.2.2 -n 1 -w 1000 > nul
  goto mount_X
)