#include "ib_ide.h"
#include <raylib.h>

void IBIdeStart() {

	InitWindow(800, 600, "imboredIDE");
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);
		EndDrawing();
	}
}