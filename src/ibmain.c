#include "imbored.h"

void removeExt(char* filename) {
	char* dot = strrchr(filename, '.');
	if(dot != NULL && dot != filename) {
		*dot = '\0';
	}
}

char* path2Filename(char* path) {
	char* last_slash = strrchr(path, '/');
	if(last_slash != NULL) {
		return last_slash + 1;
	}
	return path;
}

void IBcompFrontend(int argc, char** argv, int* rv) {
	if (argc < 2) {
		printf("Please specify a file\n");
		*rv = -1;
		return;
	}
	//printf("%d",(int)sizeof(IBVecData));
	FILE* f = fopen(argv[1], "r");
	if (f) {
		IBLayer3 comp;
		IBLayer3Init(&comp);		
		char* ext = strdup(argv[1]);
		removeExt(ext);
		IBStrAppendCStr(&comp.ibFileNameStr, path2Filename(ext));
		free(ext);
		while (comp.Running)
			IBLayer3Tick(&comp, f);
		DbgPuts("Exiting\n");
		//IBassert(comp.InputStr == NULL);
		IBLayer3Free(&comp);
		fclose(f);
		*rv = 0;
	}
	else {
		printf("Error opening file %s\n", argv[1]);
	}
}

int
main(argc, argv)
int argc;
char**argv;
{
	//IBDictTest();
	int rv = 1;
	IBVectorInit(&g_ColorStack, sizeof(IBColor), OP_IBColor, 512);
	g_ColorStack.doNotShrink=1;
	IBPushColor(IBFgWHITE);
	IBDatabase db;
	g_DB = &db;
	IBDatabaseInit(g_DB);

	IBcompFrontend(argc, argv, &rv);

	IBVectorFreeSimple(&g_ColorStack);

	IBDatabaseFree(g_DB);
	return rv;
}
