@echo off
del IBLayer3.exe
"./ext/tcc/tcc.exe" ^
src/imbored.c ^
src/ib_ide.c ^
src/ibmain.c ^
src/imp.c ^
-Isrc ^
-Iext/inc ^
-Iext/tcc/libtcc ^
-Iext/raylib ^
-L. ^
-Lext/raylib ^
-lmsvcrt ^
-lraylib ^
-lopengl32 ^
-lgdi32 ^
-lwinmm ^
-lkernel32 ^
-lshell32 ^
-luser32 ^
-llibtcc ^
-g ^
-w ^
-o IBLayer3.exe
if %errorlevel% neq 0 pause