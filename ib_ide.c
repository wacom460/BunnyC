#include "ib_ide.h"
#include <raylib.h>


void IBIdeFileInit(IBIdeFile* ideF)
{
	IBStrInit(&ideF->name);
	IBStrInit(&ideF->path);
	IBStrInit(&ideF->data);
}

void IBIdeFileFree(IBIdeFile* ideF)
{	
	IBStrFree(&ideF->name);
	IBStrFree(&ideF->path);
	IBStrFree(&ideF->data);
}

void IBIdeProjectInit(IBIdeProject* proj)
{
}

void IBIdeProjectFree(IBIdeProject* proj)
{
}

void IBIdeStart() {
	InitWindow(800, 600, "imboredIDE");
	/*Camera3D cam;
	cam.fovy = 90;
	cam.position = (Vector3){ 10,10,0 };
	cam.target = (Vector3){ 0,0,0 };
	cam.type = CAMERA_FIRST_PERSON;
	cam.up = (Vector3){ 0,1,0 };
	DisableCursor();*/
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground((Color) { 100, 20, 0, 80 });
		SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
		DrawLineEx((Vector2) { 10, 10 }, (Vector2) { 100, 100 }, 3, RED);
		DrawRectangleLinesEx((Rectangle) { 10, 10, 200, 100 }, 5, BLUE);
		/*UpdateCamera(&cam);
		BeginMode3D(cam);
		DrawCube((Vector3) { 0, 0, 0 }, 2, 2, 3, BLACK);
		EndMode3D();
		DrawText("hi", 100, 100, 20, WHITE);*/
		EndDrawing();
	}
}