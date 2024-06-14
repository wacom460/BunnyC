@echo off
"./tcc/tcc.exe" -b -I./tcc/libtcc/ -L./ -llibtcc -run ./imbored.c main.txt
pause