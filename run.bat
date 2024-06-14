@echo off
"./tcc/tcc.exe" -I./tcc/libtcc/ -L./ -llibtcc -run ./imbored.c main.txt
pause