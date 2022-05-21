@echo off
SETLOCAL ENABLEDELAYEDEXPANSION
:: Main project testing script
::
:: Author: Michal Kekely <ikekelym@fit.vutbr.cz>
:: Date: 31.3.2021

set LOGIN=xproko47

set WORK_DIR=work
set LOG_DIR=%WORK_DIR%\log
set SIM_DIR=sim
set SYNTH_DIR=synth

if [%1] == [] (
    call scripts/unpack.bat
    if !errorlevel! neq 0 exit /b 1
    call scripts/simulate.bat
    call scripts/synth.bat
    call scripts/eval.bat
)

if [%1] == [clean] (
    rd /s /q work >NUL 2>NUL
)
pause
