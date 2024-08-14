@echo off
:top
cls
del ibtcc.exe
cd raylib
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
"./tcc/tcc.exe" imbored.c ib_ide.c -I./tcc/libtcc/ -L./ -Lraylib -Iraylib/src -lmsvcrt -lraylib -lopengl32 -lgdi32 -lwinmm -lkernel32 -lshell32 -luser32 -llibtcc -w -o ibtcc.exe
ibtcc.exe main.txt
pause
goto top