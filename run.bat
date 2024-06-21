@echo off
:top
cls
"./tcc/tcc.exe" -I./tcc/libtcc/ -L./ -llibtcc -run ./imbored.c main.txt
pause
goto top