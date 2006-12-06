@echo off
md clamwin-update
copy ..\update.mar clamwin-update
copy ..\updater.exe clamwin-update
copy ..\updater.ini clamwin-update
:: Usage:
:: updater.exe dir_where_is_update_mar pid_to_wait_or_zero [callback_workdir callback_executable callback_args...]
:: e.g: start /wait clamwin-update\updater.exe clamwin-update 0 c:\windows regedit.exe
start /wait clamwin-update\updater.exe clamwin-update 0
echo "Done - Press a key to delete update directory"
pause
del /s /q clamwin-update
rd clamwin-update
