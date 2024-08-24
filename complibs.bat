@echo off
cd ext/raylib
del libraylib.a core.o rglfw.o shapes.o textures.o text.o models.o raudio.o utils.o
"../tcc/tcc.exe" -w -I. -c -Iexternal/glfw/include core.c -std=c99 -Wall -DPLATFORM_DESKTOP -DSTBI_NO_SIMD -DGRAPHICS_API_OPENGL_11
"../tcc/tcc.exe" -w -I. -c rglfw.c -std=c99 -Wall -DPLATFORM_DESKTOP -DSTBI_NO_SIMD
"../tcc/tcc.exe" -w -I. -c shapes.c -std=c99 -Wall -DPLATFORM_DESKTOP -DSTBI_NO_SIMD
"../tcc/tcc.exe" -w -I. -c textures.c -std=c99 -Wall -DPLATFORM_DESKTOP -DSTBI_NO_SIMD
"../tcc/tcc.exe" -w -I. -c text.c -std=c99 -Wall -DPLATFORM_DESKTOP -DSTBI_NO_SIMD
"../tcc/tcc.exe" -w -I. -c models.c -std=c99 -Wall -DPLATFORM_DESKTOP -DSTBI_NO_SIMD
"../tcc/tcc.exe" -w -I. -c raudio.c -std=c99 -Wall -DPLATFORM_DESKTOP -DSTBI_NO_SIMD -DMAL_NO_WASAPI
"../tcc/tcc.exe" -w -I. -c utils.c -std=c99 -Wall -DPLATFORM_DESKTOP -DSTBI_NO_SIMD
"../tcc/tcc.exe" -ar rcs libraylib.a core.o rglfw.o shapes.o textures.o text.o models.o raudio.o utils.o
pause