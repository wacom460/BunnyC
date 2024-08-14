@echo off
rem :top
cls
cd ext/raylib
del libraylib.a
"../tcc/tcc.exe" -w -Isrc -c -Isrc/external/glfw/include src/core.c -std=c99 -Wall -DPLATFORM_DESKTOP -DSTBI_NO_SIMD -DGRAPHICS_API_OPENGL_11
"../tcc/tcc.exe" -w -Isrc -c src/rglfw.c -std=c99 -Wall -DPLATFORM_DESKTOP -DSTBI_NO_SIMD
"../tcc/tcc.exe" -w -Isrc -c src/shapes.c -std=c99 -Wall -DPLATFORM_DESKTOP -DSTBI_NO_SIMD
"../tcc/tcc.exe" -w -Isrc -c src/textures.c -std=c99 -Wall -DPLATFORM_DESKTOP -DSTBI_NO_SIMD
"../tcc/tcc.exe" -w -Isrc -c src/text.c -std=c99 -Wall -DPLATFORM_DESKTOP -DSTBI_NO_SIMD
"../tcc/tcc.exe" -w -Isrc -c src/models.c -std=c99 -Wall -DPLATFORM_DESKTOP -DSTBI_NO_SIMD
"../tcc/tcc.exe" -w -Isrc -c src/raudio.c -std=c99 -Wall -DPLATFORM_DESKTOP -DSTBI_NO_SIMD -DMAL_NO_WASAPI
"../tcc/tcc.exe" -w -Isrc -c src/utils.c -std=c99 -Wall -DPLATFORM_DESKTOP -DSTBI_NO_SIMD
"../tcc/tcc.exe" -ar rcs libraylib.a core.o rglfw.o shapes.o textures.o text.o models.o raudio.o utils.o
cd..
cd..
del IBLayer3.exe
"./ext/tcc/tcc.exe" src/imbored.c src/ib_ide.c -Isrc -Iext/tcc/libtcc/ -L./ -Lext/raylib -Iext/raylib/src -lmsvcrt -lraylib -lopengl32 -lgdi32 -lwinmm -lkernel32 -lshell32 -luser32 -llibtcc -w -o IBLayer3.exe
IBLayer3.exe main.txt
pause
rem goto top