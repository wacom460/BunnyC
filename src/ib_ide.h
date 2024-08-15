#ifndef IB_IDE_H_
#define IB_IDE_H_
#include "ibcommon.h"
#include "ibmisc.h"
#include "imbored.h"

#define IBHELLO_WORLD_SAMPLE_CODE \
"blk $main i32 $argc nts^ $argv -> i32\n" \
"    \"Hello, %%\" \"world\"\n" \
"_ 0\n"

typedef struct IBIdeCursorPos {
	int row;
	int column;
}IBIdeCursorPos;

typedef struct IBIdeLineInfo {
	int startIdx; //idx of line start character
	int endIdx; // idx of '\n'
}IBIdeLineInfo;

typedef struct IBIdeFileInfo {
	
	
	//index into file data of where each rendered line is
	IBVector lineInfo; //IBIdeLineInfo


}IBIdeFileInfo;

void IBIdeFileInfoInit(IBIdeFileInfo* info);
void IBIdeFileInfoFree(IBIdeFileInfo* info);
void IBIdeFileInfoRegenerate(IBIdeFileInfo* info, struct IBIdeFile* ideF);

typedef struct IBIdeFileRegion {
	int startIdx;
	int length;
	char* caption;
}IBIdeFileRegion;

//for later
#define IBIDE_FILE_CHUNK_SIZE 1024
typedef struct IBIdeFileChunk {
	int startPos;//means: 1*IBIDE_FILE_CHUNK_SIZE
	char data[IBIDE_FILE_CHUNK_SIZE];
}IBIdeFileChunk;

typedef struct IBIdeFile {
	IBStr name;
	IBStr path;
	
	//for now load whole files into memory
	IBStr data;
	//later
	//IBVector chunks; //IBIdeFileChunk

	bool modified;
	bool savedToDisk;
	IBIdeFileInfo info;

	IBLayer3 ibc;
} IBIdeFile;

void IBIdeFileInit(IBIdeFile* ideF, char* name);
void IBIdeFileFree(IBIdeFile* ideF);
void IBIdeFileCompile(IBIdeFile* ideF);

typedef struct IBIdeProject {
	IBStr name;
	IBVector files; //IBIdeFile
} IBIdeProject;

void IBIdeProjectInit(IBIdeProject* proj, char* name);
void IBIdeProjectFree(IBIdeProject* proj);

typedef struct IBIde {
	IBIdeProject proj;
} IBIde;

extern IBIde g_Ide;

void IBIdeStart();
#endif