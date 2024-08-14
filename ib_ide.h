#ifndef IB_IDE_H_
#define IB_IDE_H_
#include "ibcommon.h"
#define IBIDEFILE_NAME_MAX 100
typedef struct IBIdeFile {
	char name[IBIDEFILE_NAME_MAX];
	char* data;
}IBIdeFile;

void IBIdeStart();
#endif