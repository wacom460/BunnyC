@echo off
:top
del program.exe
cls
"./ext/tcc/tcc.exe" ^
-Isrc ^
-Iext/inc ^
-llibtcc ^
src/*.c ^
-o program.exe
program.exe main.ib
pause >nul
goto :top