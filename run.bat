@echo off
:top
cls
"ext/tcc/tcc" ^
-Isrc ^
-Iext/inc ^
-llibtcc ^
src/*.c ^
-o program.exe
program main.ib
pause >nul
del program.exe
goto :top