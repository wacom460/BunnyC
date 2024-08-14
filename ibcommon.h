#ifndef IBCOMMON_H_
#define IBCOMMON_H_
#include "ibop.h"
#include "ibmisc.h"

#define CLAMP_IMP\
	return val < min ? min : val > max ? max : val;
#define CLAMP_FUNC(type, name)\
	type name(type val, type min, type max)
CLAMP_FUNC(int, ClampInt);
CLAMP_FUNC(long long int, ClampSizeT);
typedef struct IBStr {
	char* start;
	char* end; /*ptr of null terminator '\0'*/
} IBStr;
void IBStrInit(IBStr* str);
void IBStrInitWithCStr(IBStr* str, char* cstr);

//you cannot use IBStrFree on a IBStr init'd with this
void IBStrInitExt(IBStr* str, char* cstr);

void IBStrFree(IBStr* str);
void IBStrClear(IBStr* str);
void IBStrReplaceWithCStr(IBStr* str, char* cstr);
void IBStrInitNTStr(IBStr* str, char* nullTerminated);
bool IBStrContainsAnyOfChars(IBStr* str, char* chars);
long long int IBStrGetLen(IBStr* str);
void IBStrAppendCh(IBStr* str, char ch, int count);
char* IBStrAppendCStr(IBStr* str, char* with);
void IBStrAppendFmt(IBStr* str, char* fmt, ...);
char* IBStrAppend(IBStr* str, IBStr* with);
int IBStrStripFront(IBStr* str, char ch);
typedef union IBVecData {
	void* ptr;
	struct IBObj* obj;
	struct IBTask* task;
	IBOp* op;
	bool* boolean;
	struct IBExpects* expects;
	struct IBNameInfoDB* niDB;
	struct IBNameInfo* ni;
	struct IBDictionary* dict;
	struct IBDictKey* dictKey;
	struct IBDictKeyDef* dictKeyDef;
} IBVecData;
typedef struct IBVector {
	long long int elemSize;
	IBOp type;
	int elemCount;
	int slotCount;
	long long int dataSize;
	//do not expect pointers to stay valid, realloc is called on change
	IBVecData* data;/*DATA BLOCK*/
} IBVector;
void IBVectorInit(IBVector* vec, long long int elemSize, IBOp type);
IBVecData* IBVectorGet(IBVector* vec, int idx);
void* IBVectorIterNext(IBVector* vec, int* idx);
void _IBVectorPush(IBVector* vec, IBVecData** dataDP);
#define IBVectorPush(vec, dataDP){\
	/*int c=(vec)->elemCount - 1;*/\
	_IBVectorPush((vec), dataDP);\
	/*PLINE;\
	DbgFmt(" VectorPush: %s ", #vec); \
	IBPushColor(IBFgCYAN); \
	DbgFmt("[%d] -> [%d]\n", c, (vec)->elemCount - 1);\
	IBPopColor();\*/ \
}
void IBVectorCopyPush(IBVector* vec, void* elem);
void IBVectorCopyPushBool(IBVector* vec, bool val);
void IBVectorCopyPushOp(IBVector* vec, IBOp val);
IBVecData* IBVectorTop(IBVector* vec);
IBVecData* IBVectorFront(IBVector* vec);
#define IBVectorPop(vec, freeFunc){\
	int c=(vec)->elemCount - 1;\
	_IBVectorPop((vec), freeFunc);\
	/*PLINE;\
	DbgFmt(" VectorPop: %s ", #vec); \
	IBPushColor(IBFgCYAN); \
	DbgFmt("[%d] -> [%d]\n", c, (vec)->elemCount - 1);\
	IBPopColor();\*/ \
}
void _IBVectorPop(IBVector* vec, void(*freeFunc)(void*));
void IBVectorPopFront(IBVector* vec, void(*freeFunc)(void*));
void IBVectorFreeSimple(IBVector* vec);
#define IBVectorFree(vec, freeFunc){\
	int i;\
	for(i = 0;i<(vec)->elemCount;i++){\
		freeFunc((void*)IBVectorGet((vec), i));\
	}\
	IBVectorFreeSimple((vec));\
}


#endif