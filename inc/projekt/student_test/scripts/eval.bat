@echo off
::                             ==================
::                             Project evaluation
::                             ==================
::
::
:: Author:  Michal Kekely <ikekelym@fit.vutbr.cz>
:: Date:  22.3.2021

echo|set /p ="Results evaluation                      "

if not exist %WORK_DIR%\sim\output.txt (
    echo [Failed]
    echo Error: Simulation failed, see work\log\sim.log file
) else (
    FC %WORK_DIR%\sim\input.txt %WORK_DIR%\sim\output.txt >%LOG_DIR%\eval.log
    if !errorlevel! neq 0 (
        echo [Failed]
        echo Error: Unexpected simulation results
        echo ------------------------------------------------------------
        echo -- Comparation of expected outputs vs. actual outputs     --
        echo ------------------------------------------------------------
        type %LOG_DIR%\eval.log
    ) else (
        if not exist %WORK_DIR%\synth\uart.ngc (
            echo [Failed]
            echo Error: Synthesis failed, see work\log\synth.log file
        ) else (
            echo [Done]
            echo Results are OK
        )
    )
)

