#include "ib_ide.h"
#include <raylib.h>
#include <assert.h>
#include <string.h>
#include <raylib-nuklear.h>

static struct nk_context* ctx;
IBIde g_Ide;

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
	if (nk_begin(ctx, "Start", nk_rect(100, 100, 220, 220),
		NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_CLOSABLE | NK_WINDOW_SCALABLE)) {
		nk_layout_row_static(ctx, 50, 150, 1);
		if (nk_button_label(ctx, "New project")) {
		}
	}
}

void IBIdeStart() {
	Font font = LoadFont("font.ttf");
	ctx = InitNuklearEx(font, 16);
	memset(&g_Ide, 0, sizeof(IBIde));
	g_Ide.open = true;
	IBIdeProjectInit(&g_Ide.proj, "new project");
	IBIdeFile* file = IBVectorGet(&g_Ide.proj.files, 0);
	assert(file);
	InitWindow(800, 600, "imboredIDE");
	SetWindowState(FLAG_WINDOW_RESIZABLE/* | FLAG_VSYNC_HINT*/);
	SetTargetFPS(60);
	SetExitKey(0);
	while (g_Ide.open) {
		UpdateNuklear(ctx);
		IBIdeFrame();
		nk_end(ctx);
		BeginDrawing();
		ClearBackground((Color) { 35, 20, 130, 80 });
		DrawNuklear(ctx);
		EndDrawing();
		if(WindowShouldClose()) g_Ide.open = false;
	}
}