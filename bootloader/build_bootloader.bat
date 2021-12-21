::Note
::MUST excute build_all.bat in DS-5 env. 

::Turn off the echo from this line
@echo off

::create release directory
if not exist release md release

del /s /q /f release\*
::open comment(goto START and goto SUCCESS) to control build specilize boot33*.bin
::goto START
make clean
make feedback
make 
copy /Y boot33*.* release\
::check file exist in release?
if not exist release\boot33.bin goto ERROR
goto SUCCESS

:ERROR
::make clean
echo ****************************************************************
echo ***********************  ERROR  ********************************
echo ****************************************************************
goto END

:SUCCESS
make clean
echo ****************************************************************
echo ***********************  SUCCESS  ******************************
echo ****************************************************************
:END
