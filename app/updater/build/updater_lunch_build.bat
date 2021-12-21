@echo off

::设置DOC窗口的尺寸，COLS---列数； LINES--行数
MODE con: COLS=200 LINES=6000

::启动变量延迟
setlocal ENABLEDELAYEDEXPANSION

::NOTE
::logo_script_list.csv中，可以添加客户定制的编译脚本
set updater_lunch_file=updater_script_list.csv
echo UPDATER BUILD SCRIPT TO CHOOSE
echo ----+-------------------------------------------------------------+---------------------------------------------------------------------
echo NUM + SCRIPT                                                      + DESCRIPTION
echo ----+-------------------------------------------------------------+---------------------------------------------------------------------

for %%i in (%updater_lunch_file%) do (
	for /f "tokens=1-3 delims=," %%a in (%%i) do (
		set "lunch_a= %%a "
		set "lunch_b=+ %%b                                                                                    "
		set "lunch_c=+ %%c"
		echo !lunch_a:~0,3! !lunch_b:~0,61! !lunch_c!
	)
)

echo ----------------------------------------------------------------------------------------------------------------------------------------


::根据用户输入的数字，选择对应的脚本
set /p updater_lunch_choose_num="> input your target build script NUM: "
for %%j in (%updater_lunch_file%) do (
	for /f "tokens=1-3 delims=," %%a in (%%j) do (
		set "user_input_num=%updater_lunch_choose_num%"
		if "%%a" == "%updater_lunch_choose_num%" (
			set UPDATER_LUNCH_TARGET_SCRIPT=%%b
			set UPDATER_LUNCH_DESCRIPTION=%%c 
		)
	)
)

::打印用户使用的编译脚本详细信息
echo USER CHOOSE, please review!
::如果用户输入数字错误，提示如下信息
if "%UPDATER_LUNCH_TARGET_SCRIPT%"=="" (
	echo ERROR: ** The input number:%updater_lunch_choose_num% not exist in %updater_lunch_file%.
	goto EOF
)
echo TARGET_SCRIPT     :%UPDATER_LUNCH_TARGET_SCRIPT%
echo DESCRIPTION       :%UPDATER_LUNCH_DESCRIPTION%


::开始编译
echo GOING TO CALL TARGET SCRIPT
goto REMOVE_DELAY
for /L %%a in (2,-1,0) do (
	set /p a=..<nul
	@ping 127.0.0.1 > nul
)
:REMOVE_DELAY

call m_tee %UPDATER_LUNCH_TARGET_SCRIPT%

:EOF
















