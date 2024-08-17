@echo off
:top
cls
cd ext/raylib
rem del /Q libraylib.a
"../tcc/tcc.exe" -w -I. -c -Iexternal/glfw/include core.c -std=c99 -Wall -DPLATFORM_DESKTOP -DSTBI_NO_SIMD -DGRAPHICS_API_OPENGL_11
"../tcc/tcc.exe" -w -I. -c rglfw.c -std=c99 -Wall -DPLATFORM_DESKTOP -DSTBI_NO_SIMD
"../tcc/tcc.exe" -w -I. -c shapes.c -std=c99 -Wall -DPLATFORM_DESKTOP -DSTBI_NO_SIMD
"../tcc/tcc.exe" -w -I. -c textures.c -std=c99 -Wall -DPLATFORM_DESKTOP -DSTBI_NO_SIMD
"../tcc/tcc.exe" -w -I. -c text.c -std=c99 -Wall -DPLATFORM_DESKTOP -DSTBI_NO_SIMD
"../tcc/tcc.exe" -w -I. -c models.c -std=c99 -Wall -DPLATFORM_DESKTOP -DSTBI_NO_SIMD
"../tcc/tcc.exe" -w -I. -c raudio.c -std=c99 -Wall -DPLATFORM_DESKTOP -DSTBI_NO_SIMD -DMAL_NO_WASAPI
"../tcc/tcc.exe" -w -I. -c utils.c -std=c99 -Wall -DPLATFORM_DESKTOP -DSTBI_NO_SIMD
"../tcc/tcc.exe" -ar rcs libraylib.a core.o rglfw.o shapes.o textures.o text.o models.o raudio.o utils.o
cd..
cd..
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
-w ^
-o IBLayer3.exe

if %errorlevel% neq 0 (
	pause
	goto :top
)
IBLayer3.exe main.txt
if %errorlevel% neq 0 pause
goto :top