#ifndef IBCOMMON_H_
#define IBCOMMON_H_
#include "ibop.h"
#include "ibmisc.h"

#if defined(__TINYC__) || defined(__GNUC__)
#define __debugbreak()
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
#define IBVEC_PUSHINFO_MAX (100)
#define IBVEC_DEFAULT_SLOTCOUNT 16
#define IBVEC_WARNINGS 1
typedef struct IBVecPushInfo {
	char* filePath;
	int lineNum;
} IBVecPushInfo;
typedef struct IBVector {
	long long int elemSize;
	IBOp type;
	int elemCount;
	int slotCount;
	int initialSlotCount;
	int dataSize;
	int reallocCount;
	char doNotShrink;
	IB_DEFMAGIC;
	//do not expect pointers to stay valid, realloc is called on change
	IBVecData* data;/*DATA BLOCK*/
	IBVecPushInfo PushInfo[IBVEC_PUSHINFO_MAX];
} IBVector;
void IBVectorInit(IBVector* vec, int elemSize, IBOp type, int count);
IBVecData* IBVectorGet(IBVector* vec, int idx);
void* _IBVectorIterNext(IBVector* vec, int* idx, int lineNum);

#define IBDBGFILELINEPARAMS	,char*file,int ln
#define IBDBGFPL2 ,file,ln
#define IBDBGFLPI1 ,__FILE__,__LINE__

#define IBVectorIterNext(vec,idx) _IBVectorIterNext(vec,idx,__LINE__)
void _IBVectorPush(IBVector* vec, IBVecData** dataDP IBDBGFILELINEPARAMS);
#define IBVectorPush(vec, dataDP){\
	/*int c=(vec)->elemCount - 1;*/\
	_IBVectorPush((vec), dataDP IBDBGFLPI1);\
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
IBVecData* IBVectorTop(IBVector* vec);
IBVecData* IBVectorFront(IBVector* vec);
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
#define IBVectorFree(vec, freeFunc){\
	int i;\
	for(i = 0;i<(vec)->elemCount;i++){\
		(freeFunc)((void*)IBVectorGet((vec), i));\
	}\
	IBVectorFreeSimple((vec));\
}

// ZII :3

#define Assert(x) \
if(!(x)) {         \
    printf("FAIL: " \
        "at %s:%d -> %s==0!\n", \
        __FILE__, __LINE__, #x); \
    DB;               \
}

#define LE "\n"
#define SPC " "
#define SET(x, y)\
	((x) |= (y))
#define SETIF(e, x, y)\
    if(e) SET(x, y);
#define UNSET(x, y)\
	((x) &= ~(y))
#define UNSETIF(e,x,y)\
    if(e)UNSET(x,y);
#define TOGGLE(x, y)\
	((x) ^= (y))
#define TOGGLEIF(e,x,y)\
    if(e)TOGGLE(x,y);
#define ISSET(x, y)\
	((x) & (y))
#define ISNTSET(x,y)\
	!ISSET(x,y)
#define ARRITEMLEN(x)\
    (sizeof((x)[0]))
#define ARRLEN(x)\
    (sizeof((x))\
	/ ARRITEMLEN((x)))
#define LOADED_BIT\
	(1 << 0)
#define LB LOADED_BIT
#define LOADED(x)\
    ((x)->flags & LB)
#define FLAGBIT(x)\
	(1 << (x))
#define FB(x)\
	FLAGBIT(x)

#define DEFSAVELOAD(TYPE)                   \
    void TYPE##Save(TYPE* ptr, char* loc);  \
    bool TYPE##Load(TYPE* ptr, char* loc);

#define IMPSAVELOAD(TYPE, VER)               \
    void TYPE##Save(TYPE* ptr, char* loc) {  \
        FILE* file = fopen(loc, "wb");       \
        Assert(file);                        \
        if (file) {                          \
            fwrite(ptr, sizeof(char),        \
            	sizeof(TYPE), file);         \
            fclose(file);                    \
        }                                    \
    }                                        \
    bool TYPE##Load(TYPE* ptr, char* loc){   \
        Assert(ptr->ver<=VER);               \
        FILE* file = fopen(loc, "rb");       \
        if (file) {                          \
            memset(ptr,0,sizeof(TYPE));      \
            fseek(file, 0, SEEK_END);        \
            long size=ftell(file);           \
            Assert(size<=sizeof(TYPE));      \
            fseek(file, 0, SEEK_SET);        \
            fread(ptr,                       \
            	sizeof(TYPE), 1, file);      \
            fclose(file);                    \
            return true;\
        }                                    \
        return false;\
    }

#define FORARR(arr)\
    for(int i=0;i<ARRLEN(arr);++i)

#define FORARRREV(arr)\
    for(int i=ARRLEN(arr)-1;i>0;--i)

#define FORARRLB(arr) \
    for(int i=0;(i<ARRLEN(arr)) \
        && LOADED(&((arr)[i])); \
        ++i)

#define FORARRNLB(arr)\
    for(int i=0;(i<ARRLEN(arr))\
        && (!LOADED(&((arr)[i])));\
        ++i)


#endif