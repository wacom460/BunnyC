#include "imbored.h"

struct IBVecData {
	union{
		IBObj obj;
		IBStr str;
		IBTask task;
		IBOp op;
		bool boolean;
		IBExpects expects;
		IBNameInfoDB niDB;
		IBNameInfo ni;
		IBDictKey dictKey;
		IBDictKeyDef dictKeyDef;
		IBTypeInfo ti;
		IBVector vec;
	};
};

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
