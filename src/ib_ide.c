#include "ib_ide.h"
#include <raylib.h>
#include <assert.h>
#include <string.h>

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
	ideF->savedToDisk = false;
	IBIdeFileInfoInit(&ideF->info);
	IBLayer3Init(&ideF->ibc);
}

void IBIdeFileFree(IBIdeFile* ideF)
{
	IBLayer3Free(&ideF->ibc);
	IBStrFree(&ideF->name);
	IBStrFree(&ideF->path);
	IBStrFree(&ideF->data);
}

void IBIdeFileCompile(IBIdeFile* ideF)
{
	IBLayer3Free(&ideF->ibc);
	IBLayer3Init(&ideF->ibc);
	for (int i = 0; i < IBStrGetLen(&ideF->data); i++) {
		if (ideF->ibc.InputStr) {
			IBLayer3InputStr(&ideF->ibc, ideF->ibc.InputStr);
			ideF->ibc.InputStr = NULL;
		}
		IBLayer3InputChar(&ideF->ibc, ideF->data.start[i]);
	}
}

void IBIdeProjectInit(IBIdeProject* proj, char* name) {
	IBIdeFile* newFile = NULL;
	IBStrInitWithCStr(&proj->name, name);
	IBVectorInit(&proj->files, sizeof(IBIdeFile), OP_IBIdeFile);
	IBVectorPush(&proj->files, &newFile);
	IBIdeFileInit(newFile, "new."IB_FILE_EXT);
	IBStrAppendCStr(&newFile->data, IBHELLO_WORLD_SAMPLE_CODE);
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

void IBIdeFrame() {

}

IBIde g_Ide;

void IBIdeStart() {
	memset(&g_Ide, 0, sizeof(IBIde));
	IBIdeProjectInit(&g_Ide.proj, "new project");
	IBIdeFile* file = IBVectorGet(&g_Ide.proj.files, 0);
	assert(file);
	//GenImageFontAtlas
	InitWindow(800, 600, "imboredIDE");
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	SetExitKey(0);
	SetTargetFPS(20);
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground((Color) { 35, 20, 130, 80 });
		IBIdeFrame();
		EndDrawing();
	}
}