#include "ibmain.h"
#include "imbored.h"
#include <stdio.h>
#include "ib_ide.h"
#include "ibcolor.h"

void IBcompFrontend(int argc, char** argv, int* rv) {
	if (argc < 2) {
		printf("Please specify a file\n");
		*rv = -1;
		return;
	}
	FILE* f = fopen(argv[1], "r");
	if (f) {
		IBLayer3 comp;
		IBLayer3Init(&comp);
		while (comp.Running)
			IBLayer3Tick(&comp, f);
		DbgFmt("Exiting\n", "");
		//assert(comp.InputStr == NULL);
		IBLayer3Free(&comp);
		fclose(f);
		*rv = 0;
	}
	else {
		printf("Error opening file\n");
	}
}

void IBideFrontend(int argc, char** argv, int* rv) {
	IBIdeStart();
	*rv = 0;
}

int main(int argc, char** argv) {
	int rv = 1;
	IBVectorInit(&g_ColorStack, sizeof(IBColor), OP_IBColor);
	IBPushColor(IBFgWHITE);
	IBDatabase db;
	g_DB = &db;
	IBDatabaseInit(g_DB);

	//IBcompFrontend(argc, argv, &rv);
	IBideFrontend(argc, argv, &rv);

	IBVectorFreeSimple(&g_ColorStack);
	return rv;
}
