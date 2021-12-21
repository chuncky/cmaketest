@echo off

set cur_dir=%~dp0

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