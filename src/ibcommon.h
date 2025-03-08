#ifndef IBCOMMON_H_
#define IBCOMMON_H_
#include "ibop.h"

#define IBMAGIC (1011933)
#define IB_ASSERTMAGICP(o)\
	IBassert((o)->initMagic==IBMAGIC)
#define IB_ASSERTMAGIC(o)\
	IBassert((o).initMagic==IBMAGIC)
#define IB_DEFMAGIC \
	unsigned int initMagic
#define IB_SETMAGICP(o)\
	(o)->initMagic=IBMAGIC
#define IB_SETMAGIC(o)\
	(o).initMagic=IBMAGIC
#define IB_FILE_EXT "ib"
#ifndef bool
#define bool char
#endif
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
#define IB_TRUESTR "true"
#define IBFALSESTR "false"

#ifdef __TINYC__
int memcpy_s(void* dest, long long destsz, void* src, long long count);
long long strnlen(char* s, long long maxlen);
#endif

#if defined(__TINYC__) || defined(__GNUC__)
#define __debugbreak() exit(-1)
#endif
#define DB __debugbreak();

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
long long int IBStrLen(IBStr* str);
void IBStrAppendCh(IBStr* str, char ch, int count);
char* IBStrAppendCStr(IBStr* str, char* with);
void IBStrAppendFmt(IBStr* str, char* fmt, ...);
char* IBStrAppend(IBStr* str, IBStr* with);
int IBStrStripFront(IBStr* str, char ch);

struct IBVecData;

#define IBVEC_PUSHINFO_MAX (32)
#define IBVEC_DEFAULT_SLOTCOUNT 16
#define IBVEC_WARNINGS 1

typedef struct IBVecPushInfo {
	struct IBVecData* ptr;
	char* filePath;
	int lineNum;
} IBVecPushInfo;
typedef struct IBVector {
	long long int elemSize;
	IBOp type;
	int elemCount;
	int slotCount;
	int initialSlotCount;
	long long dataSize;
	int reallocCount;
	char doNotShrink;
	IB_DEFMAGIC;
	//do not expect pointers to stay valid, realloc is called on change
	struct IBVecData* data;/*DATA BLOCK*/
	IBVecPushInfo PushInfo[IBVEC_PUSHINFO_MAX];
} IBVector;
void IBVectorInit(IBVector* vec, int elemSize, IBOp type, int count);
struct IBVecData* IBVectorGet(IBVector* vec, int idx);
void* _IBVectorIterNext(IBVector* vec, int* idx, int lineNum);

#define IBDBGFILELINEPARAMS	,char*file,int ln
#define IBDBGFPL2 ,file,ln
#define IBDBGFLPI1 ,__FILE__,__LINE__

#define IBVectorIterNext(vec,idx) _IBVectorIterNext(vec,idx,__LINE__)
void _IBVectorPush(IBVector* vec, struct IBVecData** dataDP IBDBGFILELINEPARAMS);
#define IBVectorPush(vec, dataDP){\
	/*int c=(vec)->elemCount - 1;*/\
	_IBVectorPush((vec), (struct IBVecData**)dataDP IBDBGFLPI1);\
	/*PLINE;\
	DbgFmt(" VectorPush: %s ", #vec); \
	IBPushColor(IBFgCYAN); \
	DbgFmt("[%d] -> [%d]\n", c, (vec)->elemCount - 1);\
	IBPopColor();\*/ \
}
void _IBVectorCopyPush(IBVector* vec, void* elem IBDBGFILELINEPARAMS);
#define IBVectorCopyPush(vec,elem)\
	_IBVectorCopyPush(vec,elem IBDBGFLPI1)
void _IBVectorCopyPushBool(IBVector* vec, bool val IBDBGFILELINEPARAMS);
#define IBVectorCopyPushBool(vec,val)\
	_IBVectorCopyPushBool(vec,val IBDBGFLPI1)
void _IBVectorCopyPushOp(IBVector* vec, IBOp val IBDBGFILELINEPARAMS);
#define IBVectorCopyPushOp(vec,val)\
	_IBVectorCopyPushOp(vec,val IBDBGFLPI1)
struct IBVecData* IBVectorTop(IBVector* vec);
struct IBVecData* IBVectorFront(IBVector* vec);
#define IBVectorPop(vec, freeFunc)\
	_IBVectorPop((vec), (void(*)(void*))(freeFunc))
#define IBVectorClear(vec, freeFunc){\
	while((vec)->elemCount){\
		IBVectorPop((vec), (void(*)(void*))(freeFunc));\
	}\
}

void _IBVectorPop(IBVector* vec, void(*freeFunc)(void*));
void _IBVectorPopFront(IBVector* vec, void(*freeFunc)(void*));
#define IBVectorPopFront(vec,freeFunc)\
	_IBVectorPopFront(vec,(void(*)(void*))(freeFunc))
void IBVectorFreeSimple(IBVector* vec);
void _IBVectorReinitPushInfo(IBVector* vec);
#define IBVectorFree(vec, freeFunc){\
	int i;\
	for(i = 0;i<(vec)->elemCount;i++){\
		(freeFunc)((void*)IBVectorGet((vec), i));\
	}\
	IBVectorFreeSimple((vec));\
}

#define Assert(x) \
if(!(x)) {         \
    printf("FAIL: " \
        "at %s:%d -> %s==0!\n", \
        __FILE__, __LINE__, #x); \
    DB;               \
}

#endif