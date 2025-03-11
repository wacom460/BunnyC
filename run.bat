@echo off
:top
del compiler.exe
cls
"ext/tcc/tcc" ^
-Isrc ^
-Iext/inc ^
-llibtcc ^
src/*.c ^
-o compiler.exe
".\compiler.exe" main.ib
pause >nul
goto :top