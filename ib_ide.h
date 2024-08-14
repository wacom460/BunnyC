#ifndef IB_IDE_H_
#define IB_IDE_H_
#include "ibcommon.h"
#include "ibmisc.h"

#define IBHELLO_WORLD_SAMPLE_CODE \
"blk $main i32 $argc c8^ $argv -> i32\n" \
"    \"Hello, %%\" \"world\"\n" \
"_ 0\n" \
;

typedef struct IBIdeFile {
	IBStr name;
	IBStr path;
	IBStr data;
	bool modified;
} IBIdeFile;

void IBIdeFileInit(IBIdeFile* ideF, char* name);
void IBIdeFileFree(IBIdeFile* ideF);

typedef struct IBIdeProject {
	IBStr name;
	IBVector files; //IBIdeFile
} IBIdeProject;

void IBIdeProjectInit(IBIdeProject* proj);
void IBIdeProjectFree(IBIdeProject* proj);

void IBIdeStart();
#endif