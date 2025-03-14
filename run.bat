@echo off
:top
cls
"ext/tcc/tcc" ^
-Isrc ^
-llibtcc ^
-run ^
src/build.c ^
main.ib
pause >nul
goto :top