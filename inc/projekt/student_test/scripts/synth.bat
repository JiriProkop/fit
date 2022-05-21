@echo off
SETLOCAL ENABLEDELAYEDEXPANSION
::                    ====================================
::                    Synthesis and compilation of project
::                    ====================================
::
::
:: Author:  Lukas Kekely <ikekely@fit.vutbr.cz>
:: Date:  1.12.2021

echo|set /p ="Project synthesis                       "

md %WORK_DIR%\synth >NUL 2>&1
xcopy /s %SYNTH_DIR% %WORK_DIR%\synth >NUL

cd %WORK_DIR%\synth
md xst
md xst\projnav.tmp
xst -intstyle ise -ifn "uart.xst" -ofn "uart.syr" >..\..\%LOG_DIR%\synth.log 2>&1
cd ..\..

if not exist %WORK_DIR%\synth\uart.ngc echo [Failed] & exit /b 1
echo [Done]
