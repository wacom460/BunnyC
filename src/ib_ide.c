#include "ib_ide.h"
#include <raylib.h>
#include <assert.h>

void IBIdeFileInfoInit(IBIdeFileInfo* info)
{
	IBVectorInit(&info->lineInfo, sizeof(IBIdeLineInfo), OP_IBIdeLineInfo);
}

void IBIdeFileInfoFree(IBIdeFileInfo* info)
{
	IBVectorFreeSimple(&info->lineInfo);
}

void IBIdeFileInfoRegenerate(IBIdeFileInfo* info, IBIdeFile* ideF)
{
	IBVectorClear(&info->lineInfo, NULL);
}

void IBIdeFileInit(IBIdeFile* ideF, char* name)
{
	IBStrInitWithCStr(&ideF->name, name);
	IBStrInit(&ideF->path);
	IBStrInit(&ideF->data);
	ideF->modified = false;
	IBIdeFileInfoInit(&ideF->info);
}

void IBIdeFileFree(IBIdeFile* ideF)
{
	IBStrFree(&ideF->name);
	IBStrFree(&ideF->path);
	IBStrFree(&ideF->data);
}

void IBIdeProjectInit(IBIdeProject* proj, char* name) {
	IBIdeFile* newFile = NULL;
	IBStrInitWithCStr(&proj->name, name);
	IBVectorInit(&proj->files, sizeof(IBIdeFile), OP_IBIdeFile);
	IBVectorPush(&proj->files, &newFile);
	assert(newFile);
	IBIdeFileInit(newFile, "new."IB_FILE_EXT);
}

void IBIdeProjectFree(IBIdeProject* proj)
{
	IBIdeFile* file = NULL;
	int idx = 0;
	while (file = IBVectorIterNext(&proj->files, &idx))
		IBIdeFileFree(file);
	IBVectorFreeSimple(&proj->files);
	IBStrFree(&proj->name);
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