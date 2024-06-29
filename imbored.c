/* THIS FILE IS STRICT ANSI C89 ONLY */
/*define IB_HEADER to use in TCC and get definitions only*/
/*
#define IB_HEADER
#include "imbored.c" //access the compiler and structures
*/
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#ifdef _WIN32
#include <Windows.h>
#define atoll _atoi64
#endif
#define bool char
#define true 1
#define false 0
#define BoolStr(b) (b ? "YES" : "NO")
#ifdef _MSC_VER
#define strdup _strdup
#endif
/*
not actually a compiler
ascii only, maybe utf8 later...
transpile to ANSI C89
no order of operations, sequential ONLY
compiler options inside source code, preferably using code
in number order breakpoints, if hit in the wrong order or missing then failure
*/

//console colors
#ifdef _WIN32
typedef enum IBColor {
	//fg
	IBFgWHITE =  FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	IBFgRED = FOREGROUND_RED,
	IBFgGREEN = FOREGROUND_GREEN,
	IBFgBLUE = FOREGROUND_BLUE,
	IBFgYELLOW = FOREGROUND_RED | FOREGROUND_GREEN,
	IBFgCYAN = FOREGROUND_BLUE | FOREGROUND_GREEN,
	IBFgMAGENTA = FOREGROUND_RED | FOREGROUND_BLUE,
	IBFgBROWN = FOREGROUND_RED | FOREGROUND_GREEN,
	IBFgIntensity = FOREGROUND_INTENSITY,

	//bg
	IBBgWHITE = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
	IBBgRED = BACKGROUND_RED,
	IBBgGREEN = BACKGROUND_GREEN,
	IBBgBLUE = BACKGROUND_BLUE,
	IBBgYELLOW = BACKGROUND_RED | BACKGROUND_GREEN,
	IBBgCYAN = BACKGROUND_BLUE | BACKGROUND_GREEN,
	IBBgMAGENTA = BACKGROUND_RED | BACKGROUND_BLUE,
	IBBgBROWN = BACKGROUND_RED | BACKGROUND_GREEN,
	IBBgIntensity = BACKGROUND_INTENSITY
}IBColor;
#else//unix soon
typedef enum IBColor {
	//fg
	IBFgWHITE,
	IBFgRED,
	IBFgGREEN,
	IBFgBLUE,
	IBFgYELLOW,
	IBFgCYAN,
	IBFgMAGENTA,
	IBFgBROWN,
	IBFgIntensity,

	//bg
	IBBgWHITE,
	IBBgRED,
	IBBgGREEN,
	IBBgBLUE,
	IBBgYELLOW,
	IBBgCYAN,
	IBBgMAGENTA,
	IBBgBROWN,
	IBBgIntensity
}IBColor;
#endif

void IBSetColor(IBColor col);
void IBPushColor(IBColor col);
void IBPopColor();

#define DEBUGPRINTS

#ifdef DEBUGPRINTS
void _PrintLine(int l) {
	IBPushColor(IBFgRED);
	printf("[");
	IBPushColor(IBBgGREEN);
	printf("%d", l);
	IBPushColor(IBFgRED);
	printf("]");
	IBPopColor();
	IBPopColor();
	IBPopColor();
}
#define PLINE _PrintLine(__LINE__)
#else
#define PLINE
#endif
#define OP_NAME_LEN 32
#define COMMENT_CHAR ('~')
#define IBLayer3STR_MAX 64
#define ThingStructTypeHeaderVarType ("int")
#define ThingStructTypeHeaderVarName ("__thingTYPE")
#if defined(__TINYC__) || defined(__GNUC__)
#define __debugbreak()
#endif
#define DB __debugbreak();

#define IBASSERT(x, errMsg){\
	if(!(x)) {\
		PLINE;\
		IBPushColor(IBFgRED);\
		printf("Assertion failed!!! -> %s\n%s", errMsg, #x);\
		IBPopColor();\
		DB;\
		exit(-1);\
	}\
}
#define assert(x) IBASSERT(x, "")

#ifdef DEBUGPRINTS
#define DbgFmt(x, ...){\
	printf(x, __VA_ARGS__);\
}
#else
#define DbgFmt(x, ...)
#endif

struct IBDatabase* g_DB;

typedef enum Op { /* multiple uses */
	OP_Null, OP_False, OP_True, OP_Unknown, OP_NotSet, OP_Any, OP_Use,
	OP_Build, OP_Space,

	OP_Func, OP_FuncHasName, OP_FuncNeedName, OP_FuncNeedsRetValType,
	OP_FuncArgsVarNeedsName, OP_FuncArgNameless, OP_FuncArgComplete,
	OP_FuncWantCode, OP_FuncSigComplete, OP_FuncNeedRetVal, OP_FuncArg,
	OP_CompletedFunction,

	OP_VarNeedName, OP_VarWantValue, OP_VarComplete,
	OP_CallNeedName, OP_CallWantArgs, OP_CallComplete,
	OP_BlockReturnNeedValue, OP_ArgNeedValue, OP_Arg,

	OP_Op, OP_Value, OP_Done, OP_Return, OP_NoChange, OP_Struct,
	OP_VarType,	OP_LineEnd,	OP_Comment, OP_MultiLineComment,
	OP_Public, OP_Private, OP_Imaginary, OP_Void, OP_Set, OP_SetAdd,
	OP_Call, OP_Colon, OP_Dot, 	OP_Add, OP_Subtract, OP_Multiply,
	OP_Divide, OP_AddEq, OP_SubEq, OP_MultEq, OP_DivEq, OP_Equals,
	OP_NotEquals, OP_LessThan, OP_GreaterThan, OP_LessThanOrEquals,
	OP_GreaterThanOrEquals,OP_CPrintfHaveFmtStr,OP_ParenthesisOpen,
	OP_ParenthesisClose, OP_ScopeOpen, OP_ScopeClose,OP_TaskStackEmpty,
	OP_RootTask,OP_Thing,OP_ThingWantName,OP_ThingWantContent,
	OP_ThingWantRepr,OP_SpaceNeedName,OP_SpaceHasName, OP_Obj, OP_Bool,
	OP_Task, OP_IBColor,OP_Repr,OP_IfNeedLVal,OP_IfNeedMidOP,OP_IfNeedRVal,
	OP_IfFinished, OP_IfBlockWantCode, OP_IBCodeBlock, 
	OP_YouCantUseThatHere, OP_BlockWantCode,

	OP_SpaceChar, OP_Comma, OP_CommaSpace, OP_Name, OP_String,
	OP_CPrintfFmtStr, OP_Char, OP_If, OP_Else, OP_For, OP_While,
	OP_Block, OP_c8, OP_u8, OP_u16, OP_u32, OP_u64, OP_i8, OP_i16,
	OP_i32, OP_i64, OP_f32, OP_d64, OP_Pointer, OP_DoublePointer,
	OP_TripplePointer, OP_IBLayer3Flags, OP_dbgBreak, OP_dbgAssert,
	OP_dbgAssertWantArgs,OP_TaskType, OP_TaskStack, OP_NotEmpty,
	OP_TabChar,OP_UseNeedStr,OP_UseStrSysLib,OP_NameInfoDB,
	OP_NameInfo,OP_Expects,OP_ElseIf,OP_EmptyStr,

	OP_NotFound, OP_Error, OP_ErrUnexpectedNextPfx,
	OP_ErrExpectedVariablePfx, OP_ErrNoTask, OP_ErrUnexpectedOp,
	OP_ErrQuadriplePointersNOT_ALLOWED, OP_ErrUnknownOpStr,
	OP_ErrProtectedSlot,OP_ErrUnknownPfx,OP_ErrUnexpectedNameOP,
	OP_ErrDirtyTaskStack,

	OP_ModePrefixPass, OP_ModeStrPass, OP_ModeComment, OP_ModeMultiLineComment,
} Op;
#define CLAMP_IMP {\
	return val < min ? min : val > max ? max : val;\
}
#define CLAMP_FUNC(type, name) type name(type val, type min, type max)
CLAMP_FUNC(int, ClampInt);
CLAMP_FUNC(size_t, ClampSizeT);
typedef struct IBStr {
	char *start;
	char* end; /*ptr of null terminator '\0'*/
} IBStr;
void IBStrInit(IBStr* str, size_t reserve);
void IBStrFree(IBStr* str);
void IBStrClear(IBStr* str);
void IBStrInitNTStr(IBStr* str, char* nullTerminated);
size_t IBStrGetLen(IBStr* str);
void IBStrAppendCh(IBStr* str, char ch, int count);
char* IBStrAppendCStr(IBStr* str, char *with);
void IBStrAppendFmt(IBStr* str, char* fmt, ...);
char* IBStrAppend(IBStr* str, IBStr* with);
int IBStrStripFront(IBStr* str, char ch);
typedef union IBVecData {
	void* ptr;
	struct Obj* obj;
	struct IBTask* task;
	Op* op;
	bool *boolean;
	struct IBExpects* expects;
	struct IBNameInfoDB* niDB;
	struct IBNameInfo* ni;
} IBVecData;
typedef struct IBVector {
	size_t elemSize;
	Op type;
	int elemCount;
	int slotCount;
	size_t dataSize;
	IBVecData* data;/*DATA BLOCK*/
} IBVector;
void IBVectorInit(IBVector* vec, size_t elemSize, Op type);
IBVecData* IBVectorGet(IBVector* vec, int idx);
void* IBVectorIterNext(IBVector* vec, int* idx);
void _IBVectorPush(IBVector* vec, IBVecData** dataDP);
#define IBVectorPush(vec, dataDP){\
	int c=(vec)->elemCount - 1;\
	_IBVectorPush((vec), dataDP);\
	/*PLINE;\
	DbgFmt(" VectorPush: %s ", #vec); \
	IBPushColor(IBFgCYAN); \
	DbgFmt("[%d] -> [%d]\n", c, (vec)->elemCount - 1);\
	IBPopColor();\*/ \
}
void IBVectorCopyPush(IBVector* vec, void* elem);
void IBVectorCopyPushBool(IBVector* vec, bool val);
void IBVectorCopyPushOp(IBVector* vec, Op val);
void IBVectorCopyPushIBColor(IBVector* vec, IBColor col);
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
/* GLOBAL COLOR STACK */
IBVector g_ColorStack; /*IBColor*/
void IBPushColor(IBColor col) {
	IBVectorCopyPushIBColor(&g_ColorStack, col);
	IBSetColor(col);
}
void IBPopColor() {
	IBColor* col;
	_IBVectorPop(&g_ColorStack, NULL);
	//assert(g_ColorStack.elemCount);
	col = (IBColor*)IBVectorTop(&g_ColorStack);
	if(col) IBSetColor(*col);
	else IBSetColor(IBFgWHITE);
}

char* StrConcat(char* dest, int count, char* src);
typedef union {
	unsigned char u8;
	unsigned short u16;
	unsigned int u32;
	unsigned long long u64;
	char i8;
	char c8;
	short i16;
	int i32;
	long long i64;
	float f32;
	double d64;
} Val;
typedef struct IBCodeBlock {
	IBStr header;
	IBStr variables;
	IBStr code;
	IBStr footer;
} IBCodeBlock;
void IBCodeBlockInit(IBCodeBlock* block);
void IBCodeBlockFinish(IBCodeBlock* block, IBStr* output);
void IBCodeBlockFree(IBCodeBlock* block);
//typedef struct IB_Variable {
//	Op type;
//	IBStr name;
//	Val val;
//} IB_Variable;
//IB_Variable* IB_VariableNew(Op type, IBStr name, Val val);
typedef struct IB_Func {
	Op retType;
	Op retTypeMod;
	IBVector args;/*IB_Variable*/
} IB_Func;
typedef struct IB_DBObj {
	Op type;
	IBStr name;
	IBVector children;/*IB_DBObj*/
} IB_DBObj;
IB_DBObj* IB_DBObjNew(Op type, IBStr name);
void IB_DBObjFree(IB_DBObj* obj);
typedef struct IBDatabase {
	IB_DBObj* root;
} IBDatabase;
void IBDatabaseInit(IBDatabase* db);
void IBDatabaseFree(IBDatabase* db);
IB_DBObj* IBDatabaseFind(IBDatabase* db, IBStr location);
typedef struct IBNameInfo {
	Op type;
	char* name;
} IBNameInfo;
typedef struct IBNameInfoDB {
	IBVector pairs;
} IBNameInfoDB;
typedef struct FuncObj {
	Val retVal;
	Op retType;
	Op retTypeMod;
	struct IBTask* thingTask;
} FuncObj;
typedef struct IfObj {
	Op lvTYPE;
	char* lvName;
	Val lvVal;
	Op lvDataType;
	Op lvMod;
	Op midOp;
	Op rvTYPE;
	char* rvName;
	Val rvVal;
	Op rvDataType;
	Op rvMod;
} IfObj;
typedef struct ArgObj {
	Op type;
	Op mod;
} ArgObj;
typedef struct VarObj {
	Val val;
	bool valSet;
	Op type;
	Op mod;
	Op privacy;
} VarObj;
char* GetOpName(Op op);
typedef struct Obj {
	Op type;
	Op modifier;
	Op privacy;
	char* name;
	char* str;
	FuncObj func;
	VarObj var;
	ArgObj arg;
	IfObj ifO;
	Val val;
	Op valType;
} Obj;
Op ObjGetType(Obj* obj);
void _ObjSetType(Obj* obj, Op type);
#define ObjSetType(obj, type){\
	PLINE;\
	_ObjSetType(obj, type);\
}
Op ObjGetMod(Obj* obj);
void ObjSetMod(Obj* obj, Op mod);
void _ObjSetName(Obj* obj, char* name);
#define ObjSetName(obj, name){\
	PLINE;\
	_ObjSetName(obj,name);\
}
void ObjSetStr(Obj* obj, char* Str);
void ObjCopy(Obj* dst, Obj* src);
void ObjPrint(Obj* obj);
void ObjInit(Obj* o);
void ObjFree(Obj* o);
void Val2Str(char *dest, int destSz, Val v, Op type);
typedef struct IBExpects {
	IBVector pfxs;/*Op P */
	IBVector nameOps;/*Op N */
	char* pfxErr;
	char* nameOpErr;
	int life;
	int lineNumInited;
} IBExpects;
void _ExpectsInit(int LINENUM, IBExpects* exp, char* fmt, ...);
/*special fmt chars :
* 'P': pfx
* 'N': nameOP
* '0': life
* '1': pfxErr
* '2': nameOpErr
* PUT AT END OF FMT STR: e.g. "PPPNNc"
* 'c': code block macro (adds OP_Op, OP_If, OP_VarType... etc)
*/
#define ExpectsInit(exp, fmt, ...) \
	_ExpectsInit(__LINE__, exp, fmt, __VA_ARGS__);
void ExpectsPrint(IBExpects* exp);
void ExpectsFree(IBExpects* exp);
typedef struct IBTask {
	Op type;
	IBVector expStack; /*IBExpects*/
	IBVector working;/*Obj*/
} IBTask;
void TaskInit(IBTask* t, Op type);
void TaskFree(IBTask* t);
typedef struct IBLayer3 {
	int Line;
	int Column;
	int LineIS; //LINE inputstr
	int ColumnIS;//COLUMN inputstr
	Op Pfx;
	char Str[IBLayer3STR_MAX];
	IBStr CHeaderStructs;/* .h */
	IBStr CHeaderFuncs;
	IBStr CFile;/* .c */
	IBStr CurrentLineStr;

	IBVector ObjStack; /*Obj*/
	IBVector ModeStack; /*Op*/
	IBVector TaskStack; /*IBTask*/
	IBVector StrReadPtrsStack; /*bool*/
	IBVector CodeBlockStack; /*IBCodeBlock*/

	char* InputStr;
	char* SpaceNameStr;
	Op Pointer;
	Op Privacy;
	Op NameOp;
	Op LastNameOp;
	char Ch;
	char LastCh;
	bool Imaginary;
	bool Running;
	bool StringMode;
	bool StrAllowSpace;
	Op CommentMode;
	IBNameInfoDB NameTypeCtx;
} IBLayer3;
void _Err(IBLayer3* ibc, Op code, char *msg);
#define Err(code, msg){\
	IBPushColor(IBFgRED);\
	PLINE;\
	_Err(ibc, code, msg);\
}
void IBLayer3Init(IBLayer3* ibc);
void IBLayer3Free(IBLayer3* ibc);
Obj* IBLayer3GetObj(IBLayer3* ibc);
void IBLayer3PrintVecData(IBLayer3* ibc, IBVecData* data, Op type);
void IBLayer3VecPrint(IBLayer3* ibc, IBVector* vec);
Obj* IBLayer3FindStackObjUnderTop(IBLayer3* ibc, Op type);
Obj* IBLayer3FindStackObjUnderIndex(IBLayer3* ibc, int index, Op type);
IBTask* IBLayer3FindTaskUnderIndex(IBLayer3* ibc, int index, Op type);
int IBLayer3GetTabCount(IBLayer3* ibc);
IBCodeBlock* IBLayer3CodeBlocksTop(IBLayer3* ibc);
void _IBLayer3PushCodeBlock(IBLayer3* ibc, IBCodeBlock** cbDP);
#define IBLayer3PushCodeBlock(ibc, cbDP){\
	PLINE;\
	_IBLayer3PushCodeBlock(ibc, cbDP);\
}
void _IBLayer3PopCodeBlock(IBLayer3* ibc, bool copyToParent, IBCodeBlock** cbDP);
#define IBLayer3PopCodeBlock(ibc, copyToParent, cbDP){\
	PLINE;\
	_IBLayer3PopCodeBlock(ibc, copyToParent, cbDP);\
}
void _IBLayer3PushTask(IBLayer3* ibc, Op taskOP, IBExpects** exectsDP, IBTask** taskDP);
#define IBLayer3PushTask(ibc, taskOP, exectsDP, taskDP){\
	PLINE;\
	_IBLayer3PushTask(ibc, taskOP, exectsDP, taskDP);\
}
void _IBLayer3PopTask(IBLayer3* ibc, IBTask** taskDP);
#define IBLayer3PopTask(ibc, taskDP){\
	PLINE;\
	_IBLayer3PopTask(ibc, taskDP);\
}
void _IBLayer3PushObj(IBLayer3* ibc, Obj** o);
#define IBLayer3PushObj(ibc, objDP){\
	PLINE;\
	_IBLayer3PushObj(ibc, objDP);\
}
#define GetObjCount (ibc->ObjStack.elemCount)
void _IBLayer3PopObj(IBLayer3* ibc, bool pushToWorking, Obj **objDP);
#define IBLayer3PopObj(ibc, p2w, objDP){\
	PLINE;\
	_IBLayer3PopObj(ibc, p2w, objDP);\
}
void _IBLayer3Push(IBLayer3* ibc, Op mode, bool strAllowSpace);
#define IBLayer3Push(ibc, mode, strAllowSpace){\
	PLINE;\
	_IBLayer3Push(ibc, mode, strAllowSpace);\
}
void _IBLayer3Pop(IBLayer3* ibc);
#define IBLayer3Pop(ibc){\
	PLINE;\
	_IBLayer3Pop(ibc);\
}

/*life:0 = infinite, -1 life each pfx*/
void IBLayer3PushExpects(IBLayer3* ibc, IBExpects** expDP);

void IBLayer3PopExpects(IBLayer3* ibc);

/*frees exps top and ow expDP with top ptr for user to init again */
void IBLayer3ReplaceExpects(IBLayer3* ibc, IBExpects** expDP);

bool IBLayer3IsPfxExpected(IBLayer3* ibc, Op pfx);
bool IBLayer3IsNameOpExpected(IBLayer3* ibc, Op nameOp);
/*NO NEWLINES AT END OF STR*/
void IBLayer3Tick(IBLayer3* ibc, FILE *f);
void IBLayer3InputChar(IBLayer3* ibc, char ch);
void IBLayer3InputStr(IBLayer3* ibc, char* str);
void _IBLayer3FinishTask(IBLayer3* ibc);
#define IBLayer3FinishTask(ibc){\
	PLINE;\
	_IBLayer3FinishTask(ibc);\
}
Val IBLayer3StrToVal(IBLayer3* ibc, char* str, Op expectedType);
char* IBLayer3GetCPrintfFmtForType(IBLayer3* ibc, Op type);
void IBLayer3Prefix(IBLayer3* ibc);
void IBLayer3Str(IBLayer3* ibc);
void IBLayer3StrPayload(IBLayer3* ibc);
void IBLayer3ExplainErr(IBLayer3* ibc, Op code);
#define SetObjType(obj, tt){\
	PLINE;\
	DbgFmt(" SetObjType: %s(%d) -> %s(%d)\n", GetOpName(obj->type), (int)obj->type, GetOpName(tt), (int)tt);\
	obj->type=tt;\
}
#define PopExpects(){\
	PLINE;\
	IBLayer3PopExpects(ibc);\
}
IBTask* IBLayer3GetTask(IBLayer3* ibc);
Op IBLayer3GetMode(IBLayer3* ibc);

IBExpects* IBTaskGetExpTop(IBTask* t);
IBVector* IBTaskGetExpPfxsTop(IBTask *t);
IBVector* IBTaskGetExpNameOPsTop(IBTask* t);

#define SetTaskType(task, tt){\
	assert(task);\
	PLINE;\
	DbgFmt(" SetTaskType: %s(%d) -> %s(%d)\n", GetOpName(task->type), (int)task->type, GetOpName(tt), (int)tt);\
	task->type = tt;\
}

typedef struct OpNamePair {
	char name[OP_NAME_LEN];
	Op op;
} OpNamePair;
char* GetCEqu(Op op);
char* GetOpName(Op op);
char* GetPfxName(Op op);
Op GetOpFromName(char* name);
Op GetOpFromNameList(char* name, Op list);
Op fromPfxCh(char ch);
void OverwriteStr(char** str, char* with);

#ifndef IB_HEADER
char* IBLayer3StringModeIgnoreChars = "";
#define OP(op) {#op, ##op},
OpNamePair opNames[] = {
	{"null", OP_Null},{"no", OP_False},{"yes", OP_True},{"set", OP_Set},
	{"call", OP_Call},{"add", OP_SetAdd},{"func", OP_Func},{"~", OP_Comment},
	{"%", OP_VarType},{"Value", OP_Value},{"@", OP_Done},{"ret", OP_Return},
	{"ext", OP_Imaginary},{"if", OP_If},{"else", OP_Else},{"use", OP_Use},
	{"build", OP_Build},{"space", OP_Space},{"+", OP_Add},{"-", OP_Subtract},
	{"*", OP_Multiply},{"/", OP_Divide},{"eq", OP_Equals},{"neq", OP_NotEquals},
	{"lt", OP_LessThan},{"gt", OP_GreaterThan},{"lteq", OP_LessThanOrEquals},
	{"gteq", OP_GreaterThanOrEquals},{",", OP_Comma},{"$", OP_Name},{"for", OP_For},
	{"loop", OP_While},{"block", OP_Block},{"struct", OP_Struct},{"priv", OP_Private},
	{"pub", OP_Public},{"?", OP_Void},{"c8", OP_c8},{"u8", OP_u8},{"u16", OP_u16},
	{"u32", OP_u32},{"u64", OP_u64},{"i8", OP_i8},{"i16", OP_i16},{"i32", OP_i32},
	{"i64", OP_i64},{"f32", OP_f32},{"d64", OP_d64},{"pointer", OP_Pointer},
	{"double pointer", OP_DoublePointer},{"tripple pointer", OP_TripplePointer},
	{"ErrUnexpectedNextPfx", OP_ErrUnexpectedNextPfx},{"ModePrefixPass", OP_ModePrefixPass},
	{"ModeStrPass", OP_ModeStrPass},{"ModeComment", OP_ModeComment},{"NotSet", OP_NotSet},
	{"ModeMultiLineComment", OP_ModeMultiLineComment},{"FuncHasName", OP_FuncHasName},
	{"Return", OP_Return},{"FuncArgNameless", OP_FuncArgNameless},
	{"FuncArgComplete",OP_FuncArgComplete},{"FuncNeedsRetValType",OP_FuncNeedsRetValType},
	{"FuncSignatureComplete", OP_FuncSigComplete},{"VarNeedName", OP_VarNeedName},
	{"ErrExpectedVariablePfx",OP_ErrExpectedVariablePfx},{"VarComplete", OP_VarComplete},
	{"ErrNoTask", OP_ErrNoTask},{"FuncNeedRetVal",OP_FuncNeedRetVal},
	{"CompletedFunction",OP_CompletedFunction},{"ErrUnknownOpStr",OP_ErrUnknownOpStr},
	{"Error", OP_Error},{"FuncNeedName",OP_FuncNeedName},{"String", OP_String},
	{"VarComplete", OP_VarComplete},{"VarWantValue",OP_VarWantValue},{"LineEnd", OP_LineEnd},
	{"CPrintfHaveFmtStr",OP_CPrintfHaveFmtStr},{"FuncWantCode",OP_FuncWantCode},
	{"dbgBreak", OP_dbgBreak},{"dbgAssert", OP_dbgAssert}, { "CallNeedName",OP_CallNeedName },
	{"CallWantArgs", OP_CallWantArgs},{"CallComplete", OP_CallComplete},
	{"TaskStackEmpty", OP_TaskStackEmpty}, {"CPrintfFmtStr", OP_CPrintfFmtStr},
	{"SpaceChar",OP_SpaceChar},{"use",OP_Use},{"UseNeedStr",OP_UseNeedStr},
	{"sys", OP_UseStrSysLib},{"thing", OP_Thing},{"SpaceNeedName",OP_SpaceNeedName},
	{"RootTask", OP_RootTask},{"ErrUnknownPfx",OP_ErrUnknownPfx},
	{"ErrUnexpectedNameOP",OP_ErrUnexpectedNameOP},{"ThingWantName",OP_ThingWantName},
	{"ThingWantContent",OP_ThingWantContent},{"SpaceHasName",OP_SpaceHasName},
	{"ErrDirtyTaskStack",OP_ErrDirtyTaskStack},{"repr", OP_Repr},{"NotEmpty",OP_NotEmpty},
	{"ThingWantRepr",OP_ThingWantRepr},{"IfNeedLVal",OP_IfNeedLVal},
	{"IfNeedMidOP",OP_IfNeedMidOP},{"IfNeedRVal",OP_IfNeedRVal},
	{"IfFinished",OP_IfFinished},{"IfBlockWantCode",OP_IfBlockWantCode},
	{"NoChange",OP_NoChange},{"elif", OP_ElseIf},{"", OP_EmptyStr},
	OP(OP_BlockWantCode) OP(OP_YouCantUseThatHere) OP(OP_Arg)
};
#undef OP
OpNamePair pfxNames[] = {
	{"NULL", OP_Null},{"Value(=)", OP_Value},{"Op(@)", OP_Op},
	{"Comment(~)", OP_Comment},{"Name($)", OP_Name},
	{"VarType(%)", OP_VarType},{"Pointer(&)", OP_Pointer},
	{"Return(@ret)", OP_Return},{"OP_Unknown", OP_Unknown},
	{"String(\")", OP_String},{"LineEnd(\\n)", OP_LineEnd},
	{"Tab character", OP_TabChar},
};
OpNamePair cEquivelents[] = {
	{"void", OP_Void},{"return", OP_Return},
	{"int", OP_i32},{"unsigned int", OP_u32},
	{"long long", OP_i64},{"unsigned long long", OP_u64},
	{"short", OP_i16},{"char", OP_i8},{"char", OP_c8},
	{"unsigned short", OP_u16},{"unsigned char", OP_u8},
	{"*", OP_Pointer},{"**", OP_DoublePointer},
	{"***", OP_TripplePointer},{", ", OP_CommaSpace},
	{"(", OP_ParenthesisOpen},{")", OP_ParenthesisClose},
	{"{", OP_ScopeOpen},{"}", OP_ScopeClose},
	{"", OP_NotSet},
	{"extern", OP_Imaginary},{"float", OP_f32},{"double", OP_d64},
	{"==", OP_Equals},{"!=", OP_NotEquals},{"<", OP_LessThan},
	{">", OP_GreaterThan},{"<=", OP_LessThanOrEquals},
	{">=", OP_GreaterThanOrEquals},{"!=", OP_NotEquals},
};
OpNamePair dbgAssertsNP[] = {
	{"taskType", OP_TaskType},{ "taskStack", OP_TaskStack },{"notEmpty", OP_NotEmpty}
};
char* SysLibCodeStr =
"@space $sys\n"
"@pub\n"
"@ext @func $malloc %i32 $size @ret %&?\n"
"@ext @func $realloc %&? $ptr %i32 $newSize @ret %&?\n"
"@ext @func $free %&? $ptr\n"
"@ext @func $strdup %&c8 $str @ret %&c8\n"
"@ext @func $strcat %&c8 $str1 %&c8 $str2 @ret %&c8\n"
;
CLAMP_FUNC(int, ClampInt) CLAMP_IMP
CLAMP_FUNC(size_t, ClampSizeT) CLAMP_IMP
void IBStrInit(IBStr* str, size_t reserve){
	IBASSERT(reserve > 0, "Reserve must be > 0");
	assert(str);
	str->start = (char*)malloc(reserve);
	assert(str->start);
	str->end = str->start;
	if(str->start) (*str->start) = '\0';
}
void IBStrFree(IBStr* str){
	free(str->start);
}
void IBStrClear(IBStr* str){
	free(str->start);
	str->start = NULL;
	str->start = malloc(1);
	*str->start = '\0';
	str->end = str->start;
}
void IBStrInitNTStr(IBStr* str, char* nullTerminated){
	assert(nullTerminated);
	assert(str);
	OverwriteStr(&str->start, nullTerminated);
	str->end = str->start + strlen(nullTerminated);
}
size_t IBStrGetLen(IBStr* str) {
	size_t len;
	assert(str);
	assert(str->end >= str->start);
	len=str->end - str->start;
	return len;
}
void IBStrAppendCh(IBStr* str, char ch, int count){
	char astr[2];
	assert(str);
	assert(count > 0);
	astr[0] = ch;
	astr[1] = '\0';
	while(count--) IBStrAppendCStr(str, astr);
}
char *IBStrAppendCStr(IBStr* str, char *with) {
	void* ra;
	size_t len;
	size_t withLen;
	assert(str);
	withLen = strlen(with);
	if(!withLen) return str->start;
	assert(withLen > 0);
	assert(str->start);
	len = IBStrGetLen(str);
	ra = realloc(str->start, len + withLen + 1);
	assert(ra);
	if (ra) {
		str->start = (char*)ra;
		memcpy(str->start + len, with, withLen);
		*(str->start + len + withLen) = '\0';
		str->end = str->start + len + withLen;
		return str->start;
	}else {
		assert(0);
		exit(-1);
	}
	return NULL;
}
void IBStrAppendFmt(IBStr* str, char* fmt, ...){
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);
	IBStrAppendCStr(str, buf);
}
char* IBStrAppend(IBStr* str, IBStr* with){
	void* ra;
	size_t len;
	size_t withLen;
	assert(str);
	withLen = IBStrGetLen(with);
	if (!withLen) return str->start;
	assert(withLen > 0);
	assert(str->start);
	len = IBStrGetLen(str);
	ra = realloc(str->start, len + withLen + 1);
	assert(ra);
	if (ra) {
		str->start = (char*)ra;
		memcpy(str->start + len, with->start, withLen);
		*(str->start + len + withLen) = '\0';
		str->end = str->start + len + withLen;
		return str->start;
	}
	else {
		assert(0);
		exit(-1);
	}
	return NULL;
}
int IBStrStripFront(IBStr* str, char ch){
	int slen=IBStrGetLen(str);
	int in = 0;
	char* rep = NULL;
	char ch2 = '\0';
	while(true) {
		ch2 = str->start[in];
		if (ch2 == ch) in++;
		else break;
	}
	if(!in) return 0;
	rep=strdup(str->start+in);
	free(str->start);
	str->start=rep;
	str->end=str->start+(slen - in);
	assert(*(str->end) == '\0');
	return in;
}
void IBVectorInit(IBVector* vec, size_t elemSize, Op type) {
	void* m;
	vec->elemSize = elemSize;
	vec->type = type;
	vec->elemCount = 0;
	vec->slotCount = 1;
	vec->dataSize = vec->elemSize * vec->slotCount;
	vec->data = NULL;
	m=malloc(vec->dataSize);
	assert(m);
	vec->data = m;
	assert(vec->data);
	memset(vec->data, 0, vec->dataSize);
}
IBVecData* IBVectorGet(IBVector* vec, int idx) {
	if (idx >= vec->elemCount) return NULL;
	return (IBVecData*)((char*)vec->data + vec->elemSize * idx);
}
void* IBVectorIterNext(IBVector* vec, int* idx) {
	assert(idx);
	assert(vec);
	if (!vec || !idx) return NULL;
	if ((*idx) >= vec->elemCount) return NULL;
	return (char*)vec->data + vec->elemSize * (*idx)++;
}
void _IBVectorPush(IBVector* vec, IBVecData** dataDP) {
	IBVecData* topPtr;
	if (vec->elemCount + 1 > vec->slotCount) {
		void* ra;
		ra = NULL;
		vec->slotCount++;
		vec->dataSize = vec->elemSize * vec->slotCount;
		//DbgFmt("vec->dataSize: %d\n", vec->dataSize);
		assert(vec->data);
		ra = realloc(vec->data, vec->dataSize);
		//assert(ra);
		if(ra) vec->data = ra;
	}
	topPtr = (IBVecData*)((char*)vec->data + vec->elemSize * vec->elemCount);
	vec->elemCount++;
	if(dataDP) *dataDP = topPtr;
}
void IBVectorCopyPush(IBVector* vec, void* elem) {
	IBVecData* top;
	_IBVectorPush(vec, &top);
	memcpy(top, elem, vec->elemSize);
}
void IBVectorCopyPushBool(IBVector* vec, bool val) {
	IBVectorCopyPush(vec, &val);
}
void IBVectorCopyPushOp(IBVector* vec, Op val) {
	IBVectorCopyPush(vec, &val);
}
void IBVectorCopyPushIBColor(IBVector* vec, IBColor col){
	IBVectorCopyPush(vec, &col);
}
IBVecData* IBVectorTop(IBVector* vec) {
	assert(vec);
	if (vec->elemCount <= 0) return NULL;
	return (IBVecData*)((char*)vec->data + ((vec->elemCount - 1) * vec->elemSize));
}
IBVecData* IBVectorFront(IBVector* vec) {
	assert(vec);
	if (vec->elemCount <= 0) return NULL;
	assert(vec->data);
	return vec->data;
}
void _IBVectorPop(IBVector* vec, void(*freeFunc)(void*)){
	void* ra;
	assert(vec);
	if(vec->elemCount <= 0) return;
	if(freeFunc) freeFunc((void*)IBVectorGet(vec, vec->elemCount - 1));
	vec->elemCount--;
	vec->slotCount=vec->elemCount;
	if(vec->slotCount<1)vec->slotCount=1;
	vec->dataSize = vec->elemSize * vec->slotCount;
	if(vec->elemCount){
		assert(vec->data);
		ra = realloc(vec->data, vec->dataSize);
		assert(ra);
		if (ra) vec->data = ra;
		assert(vec->data);
	}
}
void IBVectorPopFront(IBVector* vec, void(*freeFunc)(void*)){
	size_t newSize;
	void *ra;
	if(vec->elemCount < 1) return;
	vec->elemCount--;
	vec->slotCount = vec->elemCount;
	if (vec->slotCount < 1)vec->slotCount = 1;
	if(vec->elemCount > 1){
		newSize = (vec->dataSize * vec->elemCount) - vec->dataSize;
		assert(newSize >= vec->dataSize);
		ra = malloc(newSize);
		assert(ra);
		if (ra) {
			memcpy(ra, IBVectorGet(vec, 1), newSize);
			free(vec->data);
			vec->data = ra;
		}
	}
}
void IBVectorFreeSimple(IBVector* vec) {
	free(vec->data);
}
char* StrConcat(char* dest, int count, char* src) {
	return strcat(dest, src);
}
void IBCodeBlockInit(IBCodeBlock* block){
	IBStrInit(&block->header, 1);
	IBStrInit(&block->variables, 1);
	IBStrInit(&block->code, 1);
	IBStrInit(&block->footer, 1);
}
void IBCodeBlockFinish(IBCodeBlock* block, IBStr* output){
	IBStrAppend(output, &block->header);
	IBStrAppend(output, &block->variables);
	IBStrAppend(output, &block->code);
	IBStrAppend(output, &block->footer);
}
void IBCodeBlockFree(IBCodeBlock* block){
	IBStrFree(&block->header);
	IBStrFree(&block->variables);
	IBStrFree(&block->code);
	IBStrFree(&block->footer);
}
void NameInfoInit(IBNameInfo* info){
	info->type=OP_NotSet;
	info->name=NULL;
}
void NameInfoFree(IBNameInfo* info) {
	free(info->name);
}
void NameInfoDBInit(IBNameInfoDB* db) {
	IBVectorInit(&db->pairs, sizeof(IBNameInfo), OP_NameInfo);
}
void NameInfoDBAdd(IBNameInfoDB* db, char* name, Op type) {
	IBNameInfo info;
	info.type = type;
	info.name = strdup(name);
	IBVectorCopyPush(&db->pairs, &info);
}
Op NameInfoDBFindType(IBNameInfoDB* db, char* name) {
	IBNameInfo* pair;
	int idx;
	idx = 0;
	while (pair = (IBNameInfo*)IBVectorIterNext(&db->pairs, &idx)) {
		if (!strcmp(pair->name, name))
			return pair->type;
	}
	return OP_NotFound;
}
void NameInfoDBFree(IBNameInfoDB* db) {
	assert(db);
	IBVectorFree(&db->pairs, NameInfoFree);
}
void ObjInit(Obj* o) {
	o->type=OP_NotSet;
	o->modifier=OP_NotSet;
	o->privacy=OP_NoChange;
	o->name=NULL;
	o->str=NULL;
	o->val.i32 = 0;
	o->valType = OP_Unknown;
	memset(&o->func, 0, sizeof(FuncObj));
	memset(&o->var, 0, sizeof(VarObj));
	memset(&o->ifO, 0, sizeof(IfObj));
	o->arg.type = OP_Null;
	o->arg.mod = OP_NotSet;
	o->ifO.lvName=NULL;
	o->ifO.rvName = NULL;
}
void ObjFree(Obj* o) {
	assert(o);
	if (o->name) free(o->name);
	if (o->str) free(o->str);
}
void _ExpectsInit(int LINENUM, IBExpects* exp, char *fmt, ...) {
	va_list args;
	Op pfx;
	Op nameOp;
	int i;
	IBVectorInit(&exp->pfxs, sizeof(Op), OP_Op);
	IBVectorInit(&exp->nameOps, sizeof(Op), OP_Op);
	exp->pfxErr="";
	exp->nameOpErr="";
	exp->life=0;
	exp->lineNumInited=LINENUM;
	//DbgFmt("Expect: { ","");
	va_start(args, fmt);
	for (i = 0; i < strlen(fmt); i++) {
		char ch = fmt[i];
		switch (ch) {
		case '0': {
			exp->life=va_arg(args, int);
			//DbgFmt("Life:%d ", exp->life);
			break;
		}
		case '1': {
			exp->pfxErr=va_arg(args, char*);
			//DbgFmt("PfxErr:%s ", exp->pfxErr);
			break;
		}
		case '2': {
			exp->nameOpErr=va_arg(args, char*);
			//DbgFmt("NameOpErr:%s ", exp->nameOpErr);
			break;
		}
		case 'P': {
			pfx=va_arg(args, Op);
			IBVectorCopyPushOp(&exp->pfxs, pfx);
			//DbgFmt("Pfx:%s(%d) ", GetPfxName(pfx), (int)pfx);
			break;
		}
		case 'N':{
			nameOp=va_arg(args, Op);
			IBVectorCopyPushOp(&exp->nameOps, nameOp);
			//DbgFmt("NameOP:%s(%d) ", GetOpName(nameOp), (int)nameOp);
			break;
		}
		case 'c': {
			PLINE;
			//DbgFmt("CodeBlockMacro ", "");
			IBVectorCopyPushOp(&exp->pfxs, OP_Op);
			IBVectorCopyPushOp(&exp->pfxs, OP_VarType);
			IBVectorCopyPushOp(&exp->pfxs, OP_String);
			IBVectorCopyPushOp(&exp->nameOps, OP_Call);
			IBVectorCopyPushOp(&exp->nameOps, OP_If);
			IBVectorCopyPushOp(&exp->nameOps, OP_Set);
			IBVectorCopyPushOp(&exp->nameOps, OP_Done);
			IBVectorCopyPushOp(&exp->nameOps, OP_Return);
			break;
		}
		}
	}
	//DbgFmt("}\n","");
	va_end(args);
}
void ExpectsPrint(IBExpects* ap) {
	Op* oi;
	int idx;
	idx = 0;
	assert(ap);
	oi=NULL;
	printf(
#ifdef DEBUGPRINTS
		"[LN:%d] "
#endif
		"Prefix { ", ap->lineNumInited);
	while (oi = (Op*)IBVectorIterNext(&ap->pfxs, &idx))
		printf("%s(%d) ", GetPfxName(*oi), (int)*oi);
	printf("}\nNameOP { ");
	idx = 0;
	oi=NULL;
	while (oi = (Op*)IBVectorIterNext(&ap->nameOps, &idx))
		printf("@%s(%d) ", GetOpName(*oi), (int)*oi);
	printf("}\n");
}
void ExpectsFree(IBExpects* ap) {
	assert(ap);
	IBVectorFreeSimple(&ap->pfxs);
	IBVectorFreeSimple(&ap->nameOps);
}
void TaskInit(IBTask* t, Op type) {
	IBVectorInit(&t->working, sizeof(Obj), OP_Obj);
	IBVectorInit(&t->expStack, sizeof(IBExpects), OP_Expects);
	t->type = type;
}
void TaskFree(IBTask* t) {
	assert(t);
	IBVectorFree(&t->expStack, ExpectsFree);
	IBVectorFree(&t->working, ObjFree);
}
void _Err(IBLayer3* ibc, Op code, char *msg){
	int l = ibc->InputStr ? ibc->LineIS : ibc->Line;
	int c = ibc->InputStr ? ibc->ColumnIS : ibc->Column;
	if (ibc->InputStr) printf("ERROR IN InputStr!!!\n");
	printf("Error at %u:%u \"%s\"(%d). %s\n",
		l, c, GetOpName(code), (int)code, msg);
	IBLayer3ExplainErr(ibc, code);
	IBPopColor();
#ifdef DEBUGPRINTS
	/*IBLayer3VecPrint(ibc, &ibc->ObjStack);
	IBLayer3VecPrint(ibc, &ibc->TaskStack);*/
#endif
	IBPushColor(IBFgCYAN);
	printf("PRESS ENTER TO CONTINUE ANYWAY");
	IBPopColor();
	getchar();
#if _DEBUG
	//__debugbreak();
#else
	exit(-1);
#endif
}
char* GetCEqu(Op op) {
	int sz;
	int i;
	sz=sizeof(cEquivelents) / sizeof(cEquivelents[0]);
	for (i = 0; i < sz; i++) {
		if (op == cEquivelents[i].op) return cEquivelents[i].name;
	}
	return "?";
}
IB_DBObj* IB_DBObjNew(Op type, IBStr name){
	return NULL;
}
void IB_DBObjFree(IB_DBObj* obj){
	IBStrFree(&obj->name);
	IBVectorFree(&obj->children, IB_DBObjFree);
	free(obj);
}
void IBDatabaseInit(IBDatabase* db){
}
void IBDatabaseFree(IBDatabase* db){
}
IB_DBObj* IBDatabaseFind(IBDatabase* db, IBStr location){
	return NULL;
}
char* GetOpName(Op op) {
	int sz;
	int i;
	sz=sizeof(opNames) / sizeof(opNames[0]);
	for (i = 0; i < sz; i++) {
		if (op == opNames[i].op) return opNames[i].name;
	}
	return "?";
}
char* GetPfxName(Op op) {
	int sz;
	int i;
	sz=sizeof(pfxNames) / sizeof(pfxNames[0]);
	for (i = 0; i < sz; i++) {
		if (op == pfxNames[i].op) return pfxNames[i].name;
	}
	return "?";
}
Op GetOpFromName(char* name) {
	int sz;
	int i;
	sz=sizeof(opNames) / sizeof(opNames[0]);
	for (i = 0; i < sz; i++) {
		if (!strcmp(opNames[i].name, name)) return opNames[i].op;
	}
	return OP_Error;
}
Op GetOpFromNameList(char* name, Op list) {
	switch (list) {
	case OP_dbgAssert: {
		int sz;
		int i;
		sz = sizeof(dbgAssertsNP) / sizeof(dbgAssertsNP[0]);
		for (i = 0; i < sz; i++) {
			if (!strcmp(dbgAssertsNP[i].name, name)) return dbgAssertsNP[i].op;
		}
		break;
	}
	}
	return OP_Unknown;
}
Op fromPfxCh(char ch) {
	switch (ch) {
	case '\t': return OP_TabChar;
	case ' ': return OP_SpaceChar;
	case '@': return OP_Op;
	case '$': return OP_Name;
	case '%': return OP_VarType;
	case '&': return OP_Pointer;
	case '\"': return OP_String;
	case '\'': return OP_Char;
	case '=': return OP_Value;
	default: return OP_Unknown;
	}
}
void OverwriteStr(char** str, char* with) {
	assert(str);
	assert(with);
	if (!with){
		if(*str)free(*str);
		*str=NULL;
		return;
	}
	if (*str) free(*str);
	*str = strdup(with);
	assert(*str);
}
Obj* IBLayer3GetObj(IBLayer3* ibc) {
	return (Obj*)IBVectorTop(&ibc->ObjStack);
}
void IBLayer3PrintVecData(IBLayer3* ibc, IBVecData* data, Op type){
	if (!data)return;
	switch (type) {
	case OP_Op: {
		Op op = *(Op*)data;
		DbgFmt("%s(%d)\n", GetOpName(op), (int)op);
		break;
	}
	case OP_Obj: {
		Obj* obj = (Obj*)data;
		DbgFmt("OBJ -> [\n"
			"\t\tType: %s(%d)\n"
			"\t\tModifier: %s(%d)\n"
			"\t\tPrivacy: %s(%d)\n"
			"\t\tName: %s\n"
			"\t\tStr: %s\n"
			"\t\tVal as i32: %d\n"
			"\t\tFunc: -> [\n"
			"\t\t\tRet val as i32: %d\n"
			"\t\t\tRet type: %s(%d)\n"
			"\t\t\tRet type mod: %s(%d)\n"
			"\t\t\tThing task: -> [\n",
			GetOpName(obj->type), (int)obj->type,
			GetOpName(obj->modifier), (int)obj->modifier,
			GetOpName(obj->privacy), (int)obj->privacy,
			obj->name, obj->str,
			obj->val.i32,
			obj->func.retVal.i32, 
			GetOpName(obj->func.retType), (int)obj->func.retType,
			GetOpName(obj->func.retTypeMod), (int)obj->func.retTypeMod
		);
		IBLayer3PrintVecData(ibc, obj->func.thingTask, OP_Task);
		DbgFmt(
			"\t\t]\n"
			"\t\tVar -> [\n"
			"\t\t\tVal as i32: %d\n"
			"\t\t\tVal set: %s\n"
			"\t\t\tType: %s(%d)\n"
			"\t\t\tModifier: %s(%d)\n"
			"\t\t\tPrivacy: %s(%d)\n"
			"\t\t]\n"
			"\t\tArg -> [\n"
			"\t\t\tType: %s(%d)\n"
			"\t\t\tMod: %s(%d)\n"
			"\t\tIfObj -> [\n"
			"\t\t\tlvTYPE: %s(%d)\n"
			"\t\t\tlvName: %s\n"
			"\t\t\tlvVal: %d\n"
			"\t\t\tlvDataType: %s(%d)\n"
			"\t\t\tlvMod: %s(%d)\n"
			"\t\t\tmidOP: %s(%d)\n"
			"\t\t\trvTYPE: %s(%d)\n"
			"\t\t\trvName: %s\n"
			"\t\t\trvVal: %d\n"
			"\t\t\trvDataType: %s(%d)\n"
			"\t\t\trvMod: %s(%d)\n"
			"\t\t]\n",
			obj->var.val.i32,
			BoolStr(obj->var.valSet),
			GetOpName(obj->var.type), (int)obj->var.type,
			GetOpName(obj->var.mod), (int)obj->var.mod,
			GetOpName(obj->var.privacy), (int)obj->var.privacy,
			GetOpName(obj->arg.type), (int)obj->arg.type,
			GetOpName(obj->arg.mod), (int)obj->arg.mod,
			GetOpName(obj->ifO.lvTYPE), (int)obj->ifO.lvTYPE,
			obj->ifO.lvName, 
			obj->ifO.lvVal.i32,
			GetOpName(obj->ifO.lvDataType), (int)obj->ifO.lvDataType,
			GetOpName(obj->ifO.lvMod), (int)obj->ifO.lvMod,
			GetOpName(obj->ifO.midOp), (int)obj->ifO.midOp,
			GetOpName(obj->ifO.rvTYPE), (int)obj->ifO.rvTYPE,
			obj->ifO.rvName,
			obj->ifO.rvVal.i32,
			GetOpName(obj->ifO.rvDataType), (int)obj->ifO.rvDataType,
			GetOpName(obj->ifO.rvMod), (int)obj->ifO.rvMod
		);
		DbgFmt("\t]\n", "");
		break;
	}
	case OP_Expects: {
		IBExpects* exp = (IBExpects*)data;
		DbgFmt("EXPECTS -> [\n"
			"\t\tpfxErr: %s\n"
			"\t\tnameOpErr: %s\n"
			"\t\tlife: %d\n"
			"\t\tlineNumInited: %d\n"
			"\t\tpfxs:\n",
			exp->pfxErr, exp->nameOpErr, exp->life, exp->lineNumInited);
		IBLayer3VecPrint(ibc, &exp->pfxs);
		DbgFmt("\t\tnameOps:\n", "");
		IBLayer3VecPrint(ibc, &exp->nameOps);
		DbgFmt("]\n", "");
		break;
	}
	case OP_Task: {
		IBTask* task = (IBTask*)data;
		DbgFmt("TASK -> [\n"
			"\tType: %s(%d)\n"
			"\tCode1:\n%s(%d)\n"
			"\tCode2:\n%s(%d)\n"
			"\tWorking objs:\n",
			GetOpName(task->type), (int)task->type);
		IBLayer3VecPrint(ibc, &task->working);
		DbgFmt("\tExpects:\n", "");
		IBLayer3VecPrint(ibc, &task->expStack);
		DbgFmt("]\n", "");
		break;
	}
	default:
		break;
	}
}
void IBLayer3VecPrint(IBLayer3* ibc, IBVector* vec) {
	int idx;
	IBVecData* data;

	assert(vec);
	switch (vec->type) {
	case OP_Op: {
		IBPushColor(IBFgCYAN);
		DbgFmt("OPs","")
		break;
	}
	case OP_Obj: {
		IBPushColor(IBFgYELLOW);
		DbgFmt("OBJs", "");
		break;
	}
	case OP_Expects: {
		IBPushColor(IBFgRED);
		DbgFmt("EXPECTs", "");
		break;
	}
	case OP_Task: {
		IBPushColor(IBFgBLUE);
		DbgFmt("TASKs", "");
		break;
	}
	default: {
		IBPushColor(IBFgMAGENTA);
		DbgFmt("UNKNOWN!", "");
		break;
	}
	}
	DbgFmt(" vec -> [\n", "");
	data = NULL;
	idx = 0;
	while (data = IBVectorIterNext(vec, &idx)) {
		DbgFmt("\t(%d)", idx);
		IBLayer3PrintVecData(ibc, data, vec->type);		
	}
	switch (vec->type) {
	case OP_Obj:
	case OP_Expects:
	case OP_Task:
	case OP_Op: {
		DbgFmt("]\n","");
		break;
	}
	}
	IBPopColor();
}
Obj* IBLayer3FindStackObjUnderIndex(IBLayer3* ibc, int index, Op type) {
	int i;
	if(ibc->ObjStack.elemCount < 2)Err(OP_Error, "Not enough objects on stack");
	if(index >= ibc->ObjStack.elemCount)Err(OP_Error, "Index out of bounds");
	for (i = index - 1; i >= 0;) {
		Obj* o;
		o = (Obj*)IBVectorGet(&ibc->ObjStack, i--);
		if (o->type == type) return o;
	}
	return NULL;
}
Obj* IBLayer3FindStackObjUnderTop(IBLayer3* ibc, Op type){
	Obj* o;
	int i;
	if(ibc->ObjStack.elemCount < 2) return NULL;
	for (i = ibc->ObjStack.elemCount - 1; i >= 0;) {
		o = (Obj*)IBVectorGet(&ibc->ObjStack, i--);
		if (o->type == type) return o;
	}
	return NULL;
}
void IBLayer3Init(IBLayer3* ibc){
	Obj* o;
	IBExpects* exp;
	IBCodeBlock* cb;
	ibc->Running = true;
	ibc->Imaginary = false;
	ibc->Line = 1;
	ibc->Column = 1;
	ibc->LineIS = 1;
	ibc->ColumnIS = 1;
	ibc->Pfx = OP_Null;
	ibc->Str[0] = '\0';
	IBStrInit(&ibc->CHeaderStructs, 1);
	IBStrInit(&ibc->CHeaderFuncs, 1);
	IBStrInit(&ibc->CurrentLineStr, 1);
	IBStrAppendCStr(&ibc->CHeaderStructs, "#ifndef HEADER_H_\n#define HEADER_H_\n\n");
	IBStrInit(&ibc->CFile, 1);
	IBStrAppendCStr(&ibc->CFile, "#include \"header.h\"\n\n");
	ibc->Pointer = OP_NotSet;
	ibc->Privacy = OP_Public;
	ibc->NameOp = OP_Null;
	ibc->LastNameOp = OP_Null;
	ibc->Ch = '\0';
	ibc->LastCh = '\0';
	ibc->StringMode = false;
	ibc->StrAllowSpace = false;
	ibc->CommentMode = OP_NotSet;
	ibc->InputStr = NULL;
	ibc->SpaceNameStr = NULL;
	OverwriteStr(&ibc->SpaceNameStr, "global");
	NameInfoDBInit(&ibc->NameTypeCtx);
	IBVectorInit(&ibc->ObjStack, sizeof(Obj), OP_Obj);
	IBVectorInit(&ibc->ModeStack, sizeof(Op), OP_Op);
	IBVectorInit(&ibc->StrReadPtrsStack, sizeof(bool), OP_Bool);
	IBVectorInit(&ibc->TaskStack, sizeof(IBTask), OP_Task);
	IBVectorInit(&ibc->CodeBlockStack, sizeof(IBCodeBlock), OP_IBCodeBlock);
	IBVectorPush(&ibc->CodeBlockStack, &cb)
	IBCodeBlockInit(cb);
	IBVectorCopyPushBool(&ibc->StrReadPtrsStack, false);
	IBLayer3Push(ibc, OP_ModePrefixPass, false);
	IBLayer3PushObj(ibc, &o);
	IBLayer3PushTask(ibc, OP_RootTask, &exp, NULL);
	ExpectsInit(exp, "PNNNNNNN", 
		OP_Op, OP_Use, OP_Imaginary, OP_Func, 
		OP_Thing, OP_Space, OP_Public, OP_Private);
}
IBTask* IBLayer3FindTaskUnderIndex(IBLayer3* ibc, int index, Op type){
	int i;
	if(ibc->TaskStack.elemCount < 2)Err(OP_Error, "Not enough tasks on stack");
	if(index == -1) index = ibc->TaskStack.elemCount - 1;
	if(index >= ibc->TaskStack.elemCount)Err(OP_Error, "Index out of bounds");
	for (i = index - 1; i >= 0;) {
		IBTask* t;
		t = (IBTask*)IBVectorGet(&ibc->TaskStack, i--);
		if (t->type == type) return t;
	}
	return NULL;
}
void IBLayer3Free(IBLayer3* ibc) {
	IBTask* t;
	Obj* o;
	IBCodeBlock* cb;
	assert(ibc);
	if (ibc->InputStr) {
		IBLayer3InputStr(ibc, ibc->InputStr);
		ibc->InputStr = NULL;
	}
	if(ibc->CodeBlockStack.elemCount != 1)Err(OP_Error, "dirty codeblock stack");
	cb=(IBCodeBlock*)IBVectorTop(&ibc->CodeBlockStack);
	assert(!(IBStrGetLen(&cb->variables) + 
		IBStrGetLen(&cb->code) + 
		IBStrGetLen(&cb->footer)));
	o=IBLayer3GetObj(ibc);
	if (ibc->StringMode)
		Err(OP_Error, "Reached end of file without closing string");
	if(ibc->Str[0]) IBLayer3StrPayload(ibc);
	t=IBLayer3GetTask(ibc);
	if(t->type != OP_RootTask)Err(OP_ErrDirtyTaskStack,
		"Reached end of file not at root task");
	if (ibc->TaskStack.elemCount) {
		switch (((IBTask*)IBVectorTop(&ibc->TaskStack))->type) {
		case OP_FuncNeedRetVal:
			Err(OP_Error, "Reached end of file without closing function");
			break;
		case OP_FuncSigComplete:
		case OP_FuncHasName: {
			SetObjType(o, OP_FuncSigComplete);
			IBLayer3PopObj(ibc, true, NULL);
			IBLayer3FinishTask(ibc);
			break;
		}
		}
	}
	IBStrAppendCStr(&ibc->CHeaderFuncs, "\n#endif\n");
#ifdef DEBUGPRINTS
	IBPushColor(IBFgMAGENTA);
	DbgFmt("-> Compilation complete <-\n","");
	IBPopColor();
	IBPushColor(IBFgWHITE);
	IBPushColor(IBFgCYAN);
	DbgFmt(".h File: \n","");
	IBPopColor();
	DbgFmt("%s%s", ibc->CHeaderStructs.start, ibc->CHeaderFuncs.start);
	IBPushColor(IBFgGREEN);
	DbgFmt(".c File: \n","");
	IBPopColor();
	DbgFmt("%s", ibc->CFile.start);
#else
	printf("%s%s\n%s\n",
		ibc->CHeaderStructs.start, ibc->CHeaderFuncs.start, ibc->CFile.start);
#endif
	IBPopColor();
	if (ibc->SpaceNameStr != NULL) {
		free(ibc->SpaceNameStr);
		ibc->SpaceNameStr = NULL;
	}
	IBVectorFree(&ibc->CodeBlockStack, IBCodeBlockFree);
	IBVectorFree(&ibc->ObjStack, ObjFree);
	IBVectorFreeSimple(&ibc->ModeStack);
	IBVectorFreeSimple(&ibc->StrReadPtrsStack);
	IBVectorFree(&ibc->TaskStack, TaskFree);
	NameInfoDBFree(&ibc->NameTypeCtx);
	IBStrFree(&ibc->CHeaderStructs);
	IBStrFree(&ibc->CHeaderFuncs);
	IBStrFree(&ibc->CFile);
}
int IBLayer3GetTabCount(IBLayer3* ibc){
	return ibc->CodeBlockStack.elemCount - 1;
}
IBCodeBlock* IBLayer3CodeBlocksTop(IBLayer3* ibc){
	assert(ibc->CodeBlockStack.elemCount);
	return (IBCodeBlock*)IBVectorTop(&ibc->CodeBlockStack);
}
void _IBLayer3PushCodeBlock(IBLayer3* ibc, IBCodeBlock** cbDP){
	IBCodeBlock* cb;
	DbgFmt(" Push code block\n","");
	IBVectorPush(&ibc->CodeBlockStack, &cb);
	IBCodeBlockInit(cb);
	if(cbDP) (*cbDP) = cb;
}
void _IBLayer3PopCodeBlock(IBLayer3* ibc, bool copyToParent, IBCodeBlock** cbDP){
	assert(ibc->CodeBlockStack.elemCount > 1);
	DbgFmt(" Pop code block. Copy to parent: %s\n", BoolStr(copyToParent));
	if (copyToParent) {
		if (ibc->CodeBlockStack.elemCount >= 2) {
			IBCodeBlockFinish(IBVectorTop(&ibc->CodeBlockStack),
				&((IBCodeBlock*)IBVectorGet(&ibc->CodeBlockStack,
					ibc->CodeBlockStack.elemCount - 2))->code);
		} else Err(OP_Error, "COMPILER FAILURE. No parent code block!");
	}
	IBVectorPop(&ibc->CodeBlockStack, IBCodeBlockFree);
	if(cbDP) (*cbDP) = IBLayer3CodeBlocksTop(ibc);
}
void _IBLayer3PushTask(IBLayer3* ibc, Op taskOP, IBExpects** exectsDP, IBTask** taskDP) {
	IBTask* t = IBLayer3GetTask(ibc);
	DbgFmt(" Push task ", "");
	if (t)
		DbgFmt("%s(%d) -> ", GetOpName(t->type), (int)t->type);
	DbgFmt(" %s(%d)\n", GetOpName(taskOP), (int)taskOP);
	IBVectorPush(&ibc->TaskStack, &t);
	if(taskDP) (*taskDP) = t;
	TaskInit(t, taskOP);
	IBVectorPush(&t->expStack, exectsDP);
	if (!exectsDP) {
		IBExpects* exp=IBTaskGetExpTop(t);
		ExpectsInit(exp, "P", OP_Null);
	}
}
void _IBLayer3PopTask(IBLayer3* ibc, IBTask** taskDP) {
	IBTask* t=NULL;
	IBTask* t2=NULL;
	assert(ibc);
	t=IBLayer3GetTask(ibc);
	assert(t);
	DbgFmt(" Pop task %s(%d) ", GetOpName(t->type), (int)t->type);
	IBVectorPop(&ibc->TaskStack, TaskFree);
	t2 = IBLayer3GetTask(ibc);
	assert(t2);
	DbgFmt("-> %s(%d)\n", GetOpName(t2->type), (int)t2->type);
	if(taskDP) (*taskDP) = t2;
}
void _IBLayer3PushObj(IBLayer3* ibc, Obj** o) {
	Obj *obj=IBLayer3GetObj(ibc);
	//assert(obj);
	DbgFmt(" Push obj: ","");
	if (obj && ibc->ObjStack.elemCount) {
#ifdef DEBUGPRINTS
		ObjPrint(obj);
		DbgFmt(" -> ", "");
#endif
	}
	IBVectorPush(&ibc->ObjStack, &obj);
	ObjInit(obj);
#ifdef DEBUGPRINTS
	ObjPrint(obj);
	DbgFmt("\n","");
#endif
	if (o) { (*o) = obj; }
}
void _IBLayer3PopObj(IBLayer3* ibc, bool pushToWorking, Obj** objDP) {
	Obj* o;
	IBTask* t;
	t=IBLayer3GetTask(ibc);
	assert(t);
	o = IBLayer3GetObj(ibc);
	assert(o);
	if (pushToWorking){
		Obj* newHome;
		if (o->type == OP_NotSet)Err(OP_Error, "");
#ifdef DEBUGPRINTS
		DbgFmt(" To working: ","");
#endif
#ifdef DEBUGPRINTS
		ObjPrint(o);
		DbgFmt("\n", "");
#endif
		IBVectorPush(&t->working, &newHome);
		assert(newHome);
		memcpy(newHome, o, sizeof(Obj));
		if(ibc->ObjStack.elemCount < 1){
			Err(OP_Error, "no obj in stack");
		}else if (ibc->ObjStack.elemCount == 1) {
			ObjInit(o);
		}else if (ibc->ObjStack.elemCount > 1) {
			IBVectorPop(&ibc->ObjStack, NULL);
			o=IBLayer3GetObj(ibc);
		}
	}else{
#ifdef DEBUGPRINTS
		DbgFmt("Pop obj: ", "");
		ObjPrint(o);
#endif
		if (ibc->ObjStack.elemCount == 1) {
			ObjFree(o);
			ObjInit(o);
		}
		else if (ibc->ObjStack.elemCount > 1) {
			printf("ec: %d\n", ibc->ObjStack.elemCount);
			IBVectorPop(&ibc->ObjStack, ObjFree);
			o = IBLayer3GetObj(ibc);
		}
		DbgFmt(" -> ","");
		assert(ibc->ObjStack.elemCount);
#ifdef DEBUGPRINTS
		ObjPrint(o);
		DbgFmt("\n","");
#endif
	}
	if(objDP) (*objDP) = o;
}
void _IBLayer3Push(IBLayer3* ibc, Op mode, bool strAllowSpace){
	Op cm;
	assert(ibc);
	cm=IBLayer3GetMode(ibc);
	ibc->StrAllowSpace = strAllowSpace;
	IBVectorCopyPushOp(&ibc->ModeStack, mode);
	DbgFmt(" Push mode: %s(%d) -> %s(%d)\n", GetOpName(cm), (int)cm, GetOpName(mode), (int)mode);
}
void _IBLayer3Pop(IBLayer3* ibc) {
	Op type=OP_Null;
	Op mode;
	Op mode2;
	IBTask *t;
	IBExpects* exp;
	assert(ibc);
	t = IBLayer3GetTask(ibc);
	assert(t);
	mode=IBLayer3GetMode(ibc);
	IBVectorPop(&ibc->ModeStack, NULL);
	mode2 = IBLayer3GetMode(ibc);
	if(t)type=t->type;
	DbgFmt(" Pop mode: %s(%d) -> %s(%d)\n", GetOpName(mode), (int)mode, GetOpName(mode2), (int)mode2);
	assert(t->expStack.elemCount);
#ifdef DEBUGPRINTS
	exp=IBTaskGetExpTop(t);
	//ExpectsPrint(exp);
#endif
}
Op ObjGetType(Obj* obj) { return obj->type; }
void _ObjSetType(Obj* obj, Op type) {
	DbgFmt(" obj type: %s(%d) -> %s(%d)\n",
		GetOpName(obj->type), (int)obj->type, GetOpName(type), (int)type);
	obj->type = type;
}
Op ObjGetMod(Obj* obj) { return obj->modifier; }
void ObjSetMod(Obj* obj, Op mod) {
	DbgFmt("obj mod: %s(%d) -> %s(%d)\n",
		GetOpName(obj->modifier), (int)obj->modifier, GetOpName(mod), (int)mod);
	obj->modifier = mod;
}
void _ObjSetName(Obj* obj, char* name) {
	assert(obj);
	DbgFmt(" obj name: %s -> %s\n", obj->name, name);
	OverwriteStr(&obj->name, name);
}
void ObjSetStr(Obj* obj, char* Str) {
	DbgFmt("obj str: %s -> %s\n", obj->str, Str);
	OverwriteStr(&obj->str, Str);
}
void ObjCopy(Obj* dst, Obj* src) {
	assert(dst && src);
	memcpy(dst, src, sizeof(Obj));
	dst->name = NULL;
	dst->str = NULL;
	if(src->name) OverwriteStr(&dst->name, src->name);
	if(src->str) OverwriteStr(&dst->str, src->str);
}
void ObjPrint(Obj* obj) {
	assert(obj);
	if(obj){
		printf("[");
		if (obj->type != OP_NotSet) {
			printf("Type:%s(%d),", GetOpName(obj->type), (int)obj->type);
		}
		if(obj->name)printf("Name:%s,", obj->name);
		if (obj->str)printf("Str:%s,", obj->str);
		if (obj->modifier != OP_NotSet) {
			printf("Mod:%s,", GetOpName(obj->modifier));
		}
		printf("Val:%d", obj->val.i32);
		printf("]");
	}
}
void IBLayer3PushExpects(IBLayer3* ibc, IBExpects **expDP){
	IBTask* t;
	IBExpects* exp;
	t = IBLayer3GetTask(ibc);
	assert(t);
	assert(expDP);
	if(expDP){
		IBVectorPush(&t->expStack, &exp);
		assert(exp);
		(*expDP) = exp;
	}	
}
void IBLayer3PopExpects(IBLayer3* ibc) {
	IBTask* t = IBLayer3GetTask(ibc);
	IBExpects* exp;
	assert(t);
	exp = IBVectorTop(&t->expStack);
	assert(exp);
	IBVector* pfxsIb = &exp->pfxs;
	if (pfxsIb->elemCount) {
		Op* oi;
		int idx;

#ifdef DEBUGPRINTS
		DbgFmt(" Expects POP: { ", "");
		idx = 0;
		while (oi = (Op*)IBVectorIterNext(pfxsIb, &idx)) {
			DbgFmt("%s ", GetPfxName(*oi));
		}
		DbgFmt("} -> { ","");
#endif
		IBVectorPop(&t->expStack, ExpectsFree);
		//if (!GetExpectsStack->elemCount) Err(OP_Error, "catastrophic failure");
		pfxsIb = &((IBExpects*)IBVectorTop(&t->expStack))->pfxs;
#ifdef DEBUGPRINTS
		idx = 0;
		while (oi = (Op*)IBVectorIterNext(pfxsIb,&idx)) {
			DbgFmt("%s ", GetPfxName(*oi));
		}
		DbgFmt("}\n","");
#endif
	}
}
void IBLayer3ReplaceExpects(IBLayer3* ibc, IBExpects** expDP){
	IBTask* t = IBLayer3GetTask(ibc);
	IBExpects* exp;
	assert(t);
	exp = IBTaskGetExpTop(t);
	assert(exp);
#ifdef DEBUGPRINTS
	PLINE;
	/*DbgFmt(" Replace expects:\n", "");
	ExpectsPrint(exp);*/
#endif
	ExpectsFree(exp);
	*expDP = exp;
}
bool IBLayer3IsPfxExpected(IBLayer3* ibc, Op pfx) {
	Op* oi;
	int idx;
	IBTask* t;
	IBExpects* ap;
	t = NULL;
	ap = NULL;
	t = IBLayer3GetTask(ibc);
	assert(t);
	assert(t->expStack.elemCount);
	idx = 0;
	oi = NULL;
	ap = IBTaskGetExpTop(t);
	if(ap) while (oi = (Op*)IBVectorIterNext(&ap->pfxs,&idx)) {
		assert(oi);
		if (oi && *oi == pfx)
			return true;
	}
	return false;
}
bool IBLayer3IsNameOpExpected(IBLayer3* ibc, Op nameOp){
	Op* oi;
	int idx;
	IBTask* t;
	IBExpects* exp;
	t = IBLayer3GetTask(ibc);
	assert(t);
	assert(t->expStack.elemCount);
	idx = 0;
	oi = NULL;
	exp = IBTaskGetExpTop(t);
	while (oi = (Op*)IBVectorIterNext(&exp->nameOps, &idx)) {
		assert(oi);
		if (oi && *oi == nameOp)
			return true;
	}
	return false;
}
void IBLayer3Tick(IBLayer3* ibc, FILE* f){
	char ch;
	if (ibc->InputStr) {
		IBLayer3InputStr(ibc, ibc->InputStr);
		ibc->InputStr = NULL;
	}
	else {
		assert(f);
		if ((ch = (char)fgetc(f)) != EOF) {
			if (ch != 0xffffffff)
				IBLayer3InputChar(ibc, ch);
		}
		else {
			IBLayer3InputChar(ibc, '\n');
			ibc->Running = false;
		}
	}
}
/*NO NEWLINES AT END OF STR*/
void IBLayer3InputChar(IBLayer3* ibc, char ch){
	Op m;
	IBTask* t;
	Obj* o;
	bool nl;
	ibc->Ch = ch;
	if (ibc->CommentMode == OP_NotSet && ibc->Ch != COMMENT_CHAR) 
		IBStrAppendCh(&ibc->CurrentLineStr, ibc->Ch, 1);
	nl = false;
	m=IBLayer3GetMode(ibc);
	t = IBLayer3GetTask(ibc);
	o = IBLayer3GetObj(ibc);
	if(ibc->CommentMode==OP_NotSet&&
		ibc->Ch==COMMENT_CHAR/*&&
		ibc->LastCh!=COMMENT_CHAR*/)
	{
		/*PLINE;
		DbgFmt(" LINE COMMENT ON\n","");*/
		ibc->CommentMode = OP_Comment;
		IBLayer3Push(ibc, OP_ModeComment, false);
	}else if(ibc->CommentMode==OP_Comment&&
			ibc->LastCh==ibc->Ch &&
				!ibc->StringMode
				&&ibc->Ch==COMMENT_CHAR&&
				m==OP_ModeComment)
	{
		/*PLINE;
		DbgFmt(" MULTI COMMENT ON!!!!!!\n","");*/
		IBLayer3Pop(ibc);
		IBLayer3Push(ibc, OP_ModeMultiLineComment, false);
		ibc->CommentMode = OP_MultiLineComment;
		ibc->Ch='\0';
	}else if(ibc->CommentMode==OP_MultiLineComment&&
		ibc->LastCh==ibc->Ch &&
				!ibc->StringMode
				&&ibc->Ch==COMMENT_CHAR&&
				m==OP_ModeMultiLineComment) {
		/*PLINE;
		DbgFmt(" MULTI COMMENT OFF!\n","");*/
		ibc->CommentMode=OP_NotSet;
	}
	switch (ibc->Ch) {
	case '\0': return;
	case '\n': {
		nl = true;
		if (ibc->CommentMode == OP_NotSet) {
			int l = ibc->InputStr ? ibc->LineIS : ibc->Line;
			int c = ibc->InputStr ? ibc->ColumnIS : ibc->Column;
			int stripped = 0;
			IBPushColor(IBBgCYAN);
			DbgFmt("[LINE END AT %d:%d]", l, c);
			IBPopColor();
			IBPushColor(IBFgYELLOW);
			stripped=IBStrStripFront(&ibc->CurrentLineStr, '\t');
			DbgFmt(" %s", ibc->CurrentLineStr.start);
			IBPopColor();
			IBStrClear(&ibc->CurrentLineStr);
		}
		if (ibc->CommentMode == OP_Comment) {
			IBLayer3Pop(ibc);
			ibc->CommentMode = OP_NotSet;
		}

		switch (m) {
		case OP_ModeStrPass: {
			IBLayer3StrPayload(ibc);
			break;
		}
		}

		switch (o->type) {
		case OP_CallWantArgs: {
			IBLayer3PopObj(ibc, true, &o);
			break;
		}
		case OP_VarWantValue:
		case OP_VarComplete: {
			IBLayer3PopObj(ibc, true, &o);
			break;
		}
		}

		switch(t->type){
		case OP_CallWantArgs: {
			IBLayer3FinishTask(ibc);
			break;
		}
		case OP_IfFinished: {
			IBCodeBlock* cb;
			IBExpects* exp;
			IBLayer3PopObj(ibc, true, &o);
			SetTaskType(t, OP_IfBlockWantCode);
			IBLayer3ReplaceExpects(ibc, &exp);
			ExpectsInit(exp, "NNNc", OP_Done, OP_ElseIf, OP_Else);
			IBLayer3PushCodeBlock(ibc, &cb);
			break;
		}
		case OP_ThingWantRepr: {
			IBExpects* exp;
			SetTaskType(t, OP_ThingWantContent);
			IBLayer3ReplaceExpects(ibc, &exp);
			ExpectsInit(exp, "PPNN", OP_Op, OP_VarType, OP_Func, OP_Done);
			break;
		}
		case OP_CPrintfHaveFmtStr: {
			IBLayer3FinishTask(ibc);
			break;
		}
		case OP_FuncArgsVarNeedsName: {
			Err(OP_FuncArgsVarNeedsName,"");
			break;
		}
		case OP_FuncNeedRetVal: {
			Err(OP_FuncNeedRetVal, "");
			break;
		}
		case OP_FuncSigComplete:
		case OP_FuncHasName: {
			o=IBLayer3GetObj(ibc);
			assert(o);
			SetObjType(o, OP_FuncSigComplete);
			PopExpects();
			IBLayer3PopObj(ibc, true, &o);
			if(!ibc->Imaginary){
				IBExpects *exp;
				IBCodeBlock *cb;
				IBLayer3PushExpects(ibc, &exp);
				ExpectsInit(exp, "c", OP_Null);
				SetTaskType(t, OP_FuncWantCode);
				IBLayer3PushCodeBlock(ibc, &cb);
			}
			else {
				IBLayer3FinishTask(ibc);
			}
			break;
		}
		}
		ibc->Imaginary = false;
		ibc->Pfx = OP_Null;
		break;
	}
	}
	m = IBLayer3GetMode(ibc);
	if (!nl && ibc->CommentMode == OP_NotSet) {
//#ifdef DEBUGPRINTS
//		{
//			int l = ibc->InputStr ? ibc->LineIS : ibc->Line;
//			int c = ibc->InputStr ? ibc->ColumnIS : ibc->Column;
//			if (ibc->Ch == ' ') printf("-> SPACE (0x%x)\n", ibc->Ch);
//			else printf("-> %c (0x%x) %d:%d\n",
//				ibc->Ch, ibc->Ch, l, c);
//		}
//#endif
		switch (m) {
		case OP_ModeComment:
		case OP_ModeMultiLineComment:
			break;
		case OP_ModePrefixPass:
			IBLayer3Prefix(ibc);
			break;
		case OP_ModeStrPass:
			IBLayer3Str(ibc);
			break;
		default: Err(OP_Error, "unknown mode");
			break;
		}
	}
	if (!ibc->InputStr) {
		ibc->Column++;
	}
	else {
		ibc->ColumnIS++;
	}
	if (nl) {
		if (IBLayer3IsPfxExpected(ibc, OP_LineEnd)) PopExpects();
		if (!ibc->InputStr) {
			ibc->Column = 1;
			ibc->Line++;
		}
		else {
			ibc->ColumnIS = 1;
			ibc->LineIS++;
		}
	}
	ibc->LastCh=ibc->Ch;
	if(m==OP_ModeMultiLineComment&&ibc->CommentMode==OP_NotSet){
		IBLayer3Pop(ibc);
	}
}
void IBLayer3InputStr(IBLayer3* ibc, char* str)
{
	int i;
	for (i = 0; str[i] != '\0'; i++)
		IBLayer3InputChar(ibc, str[i]);
}
Val IBLayer3StrToVal(IBLayer3* ibc, char* str, Op expectedType) {
	Val ret;
	switch (expectedType) {
	case OP_c8:
	case OP_i16:
	case OP_u32:
	case OP_u8:
	case OP_u16:
	case OP_i32: { ret.i32 = atoi(str); break; }
	case OP_i64:
	case OP_u64: { ret.u64 = atoll(str); break; }
	case OP_f32:
	case OP_d64: { ret.d64 = atof(str); break; }
	}
	return ret;
}
char* IBLayer3GetCPrintfFmtForType(IBLayer3* ibc, Op type) {
	switch (type) {
	case OP_String: return "s";
	case OP_i32:    return "d";
	case OP_i64:    return "lld";
	case OP_u64:    return "llu";
	case OP_d64:    return "f";
	case OP_f32:    return "f";
	case OP_u32:    return "u";
	case OP_Char:   return "c";
	}
	Err(OP_Error, "GetPrintfFmtForType: unknown type");
	return "???";
}
void Val2Str(char *dest, int destSz, Val v, Op type) {

	switch (type) {
	case OP_u8:  { sprintf(dest, "%u",   v.u8);  break; }
	case OP_c8:  { sprintf(dest, "%c",   v.c8);  break; }
	case OP_i16: { sprintf(dest, "%d",   v.i16); break; }
	case OP_u16: { sprintf(dest, "%u",   v.u16); break; }
	case OP_i32: { sprintf(dest, "%d",   v.i32); break; }
	case OP_i64: { sprintf(dest, "%lld", v.i64); break; }
	case OP_u32: { sprintf(dest, "%u",   v.u32); break; }
	case OP_u64: { sprintf(dest, "%llu", v.u64); break; }
	case OP_f32: { sprintf(dest, "%f",   v.f32); break; }
	case OP_d64: { sprintf(dest, "%f",   v.d64); break; }
	}
}
void _IBLayer3FinishTask(IBLayer3* ibc)	{
	IBVector* wObjs;
	IBCodeBlock* cb;
	int tabCount;
	IBTask* t;
	t = IBLayer3GetTask(ibc);
	assert(t);
	DbgFmt(" FinishTask: %s(%d)\n", GetOpName(t->type), (int)t->type);
	if(!ibc->TaskStack.elemCount)Err(OP_ErrNoTask, "task stack EMPTY!");
	wObjs = &t->working;
	assert(wObjs);
	if (!wObjs->elemCount) {/*Err(OP_Error, "workingObjs EMPTY!");*/
		DbgFmt("Warning: working objs for this task is empty!", "");
	}
	cb=IBLayer3CodeBlocksTop(ibc);
	tabCount=IBLayer3GetTabCount(ibc);
	switch (t->type) {
	case OP_CallWantArgs: {
		Obj* o = IBVectorGet(wObjs, 0);
		int idx = 0;
		IBStrAppendCh(&cb->code, '\t', tabCount);
		IBStrAppendFmt(&cb->code, "%s", o->str);
		IBStrAppendCStr(&cb->code, "(");
		while (o = (Obj*)IBVectorIterNext(wObjs, &idx)) {
			if (o->type != OP_Arg) continue;
			IBStrAppendFmt(&cb->code, "%s", o->str);
			if (idx <= wObjs->elemCount - 1) {
				IBStrAppendCStr(&cb->code, ", ");
			}
		}
		IBStrAppendCStr(&cb->code, ");\n");
		break;
	}
	case OP_BlockReturnNeedValue: {
		Obj* o;
		IBLayer3PopObj(ibc, true, &o);
		o = IBVectorTop(wObjs);
		IBStrAppendCh(&cb->code, '\t', tabCount);
		IBStrAppendFmt(&cb->code, "return ");
		assert(o->type == OP_BlockReturnNeedValue);
		switch (o->valType) {
		case OP_Value: {
			char buf[64];
			buf[0] = '\0';
			Val2Str(buf, 64, o->val, OP_i32);//for now
			IBStrAppendFmt(&cb->code, "%s", buf);
			break;
		}
		case OP_String: {
			IBStrAppendFmt(&cb->code, "\"%s\"", o->str);
			break;
		}
		case OP_Name: {
			IBStrAppendFmt(&cb->code, "%s", o->str);
			break;
		}
		}
		IBStrAppendCStr(&cb->code, ";\n");
		break;
	}
	case OP_IfBlockWantCode: {
		IfObj* ifO=NULL;
		Obj* o;
		int idx = 0;
		while (o = (Obj*)IBVectorIterNext(wObjs, &idx)) {
			if (o->type == OP_IfFinished) {
				ifO = &o->ifO;
				break;
			}
		}
		assert(ifO);
		if (ibc->LastNameOp != OP_ElseIf) {
			IBStrAppendCh(&cb->header, '\t', tabCount - 1);
			ibc->LastNameOp = OP_Null;
		}
		IBStrAppendFmt(&cb->header, "if (");
		switch (ifO->lvTYPE) {
		case OP_Value: {
			char buf[64];
			buf[0]='\0';
			Val2Str(buf, 64, ifO->lvVal, ifO->lvDataType);
			IBStrAppendFmt(&cb->header, "%s ", buf);
			break;
		}
		case OP_Name:
			IBStrAppendFmt(&cb->header, "%s ", ifO->lvName);
			break;
		}
		IBStrAppendFmt(&cb->header, "%s ", GetCEqu(ifO->midOp));
		switch (ifO->rvTYPE) {
		case OP_Value: {
			char buf[64];
			buf[0] = '\0';
			Val2Str(buf, 64, ifO->rvVal, ifO->rvDataType);
			IBStrAppendFmt(&cb->header, "%s", buf);
			break;
		}
		case OP_Name:
			IBStrAppendFmt(&cb->header, "%s", ifO->rvName);
			break;
		}
		IBStrAppendFmt(&cb->header, ") ");
	}
	case OP_BlockWantCode: {
		IBStrAppendFmt(&cb->header, "{\n");
		IBStrAppendCh(&cb->footer, '\t', tabCount - 1);
		IBStrAppendFmt(&cb->footer, "}\n");
		IBLayer3PopCodeBlock(ibc, true, &cb);
		break;
	}
	case OP_ThingWantContent: {
		IBStr header;
		IBStr body;
		IBStr footer;
		IBStr hFile;
		IBStr cFile;
		Obj* o;
		int idx;

		IBStrInit(&header, 1);
		IBStrInit(&body, 1);
		IBStrInit(&footer, 1);
		IBStrInit(&hFile, 1);
		IBStrInit(&cFile, 1);
		idx = 0;
		while (o = (Obj*)IBVectorIterNext(wObjs, &idx)) {
			switch (o->type) {
			case OP_Thing: {
				assert(o->name);
				assert(*o->name);
				
				//TODO:
				//assert that this name is unique
				//blindly trusting for now

				IBStrAppendCStr(&header, "struct ");
				IBStrAppendCStr(&header, o->name);
				IBStrAppendCStr(&header, " {\n");
				IBStrAppendCStr(&footer, "} ");
				IBStrAppendCStr(&footer, o->name);
				IBStrAppendCStr(&footer, ";\n\n");

				break;
			}
			case OP_VarWantValue:
			case OP_VarComplete: {
				IBStrAppendCStr(&body, "\t");
				IBStrAppendCStr(&body, GetCEqu(o->var.type));
				IBStrAppendCStr(&body, GetCEqu(o->var.mod));
				IBStrAppendCStr(&body, " ");
				IBStrAppendCStr(&body, o->name);
				IBStrAppendCStr(&body, ";\n");
				//default values will be stored in db
				break;
			}
			}
		}
		IBStrAppend(&ibc->CHeaderStructs, &header);
		IBStrAppend(&ibc->CHeaderStructs, &body);
		IBStrAppend(&ibc->CHeaderStructs, &footer);
		IBStrFree(&header);
		IBStrFree(&body);
		IBStrFree(&footer);
		break;
	}
	case OP_SpaceHasName: {
		Obj* o;
		int idx = 0;
		while (o = (Obj*)IBVectorIterNext(wObjs, &idx))
			if (o->type == OP_Space) break;

		break;
	}
	case OP_FuncWantCode:
	case OP_FuncSigComplete:
	case OP_FuncHasName:
	case OP_Func: {
		Obj* o;
		int idx;
		int i;
		int argc;
		IBStr cFuncModsTypeName;
		IBStr cFuncArgsThing;
		IBStr cFuncArgs;
		IBStr cFuncArgsEnd;
		IBStr cFuncCode;
		Obj* funcObj;
		Obj* thingObj;

		thingObj = NULL;
		argc = 0;
		IBStrInit(&cFuncModsTypeName, 1);
		IBStrInit(&cFuncArgsThing, 1);
		IBStrInit(&cFuncArgs, 1);
		IBStrInit(&cFuncArgsEnd, 1);
		IBStrInit(&cFuncCode, 1);
		idx = 0;
		funcObj = NULL;
		for (i = 0; i < wObjs->elemCount; ++i) {
			o = (Obj*)IBVectorGet(wObjs, i);
			switch (o->type) {
			case OP_FuncArgComplete: {/*multiple allowed*/
				Op at;
				at = o->arg.type;
				argc++;
				if (at == OP_Null)Err(OP_Error, "arg type NULL");

				if (cFuncArgs.start[0] != '\0') {
					IBStrAppendCStr(&cFuncArgs, ", ");
				}
				IBStrAppendCStr(&cFuncArgs, GetCEqu(o->arg.type));
				IBStrAppendCStr(&cFuncArgs, GetCEqu(o->arg.mod));
				IBStrAppendCStr(&cFuncArgs, " ");
				if (!o->name)Err(OP_Error, "arg name NULL");
				IBStrAppendCStr(&cFuncArgs, o->name);
				break;
			}
			case OP_Func:
			case OP_FuncHasName:
			case OP_FuncSigComplete:
			case OP_CompletedFunction: {/*should only happen once*/
				Op mod;
				funcObj = o;
				mod = ObjGetMod(o);
				if (mod != OP_NotSet) {
					IBStrAppendCStr(&cFuncModsTypeName, GetCEqu(mod));
					IBStrAppendCStr(&cFuncModsTypeName, " ");
				}
				IBStrAppendCStr(&cFuncModsTypeName, GetCEqu(o->func.retType));
				IBStrAppendCStr(&cFuncModsTypeName, GetCEqu(o->func.retTypeMod));
				IBStrAppendCStr(&cFuncModsTypeName, " ");
				if (!o->name)Err(OP_Error, "func name NULL");
				if (o->name) {
					if (o->func.thingTask)
					{
						Obj* wo;
						int idx;
						idx = 0;
						wo = IBLayer3FindStackObjUnderTop(ibc, OP_Thing);
						if (wo) {
							IBStrAppendCStr(&cFuncModsTypeName, wo->name);
							IBStrAppendCStr(&cFuncModsTypeName, "_");
							thingObj = wo;
						}
					}
					IBStrAppendCStr(&cFuncModsTypeName, o->name);
				}
				IBStrAppendCStr(&cFuncModsTypeName, "(");
				if (thingObj) {
					IBStrAppendCStr(&cFuncArgsThing, "struct ");
					IBStrAppendCStr(&cFuncArgsThing, thingObj->name);
					IBStrAppendCStr(&cFuncArgsThing, "* ptr");
				}
				break;
			}
			}
		}
		idx = 0;
		while (o= (Obj*)IBVectorIterNext(wObjs,&idx)) {
			switch (ObjGetType(o)) {
			case OP_VarComplete: {
				char valBuf[32];
				valBuf[0] = '\0';
				IBStrAppendCStr(&cFuncCode, "\t");
				IBStrAppendCStr(&cFuncCode, GetCEqu(o->var.type));
				IBStrAppendCStr(&cFuncCode, GetCEqu(o->var.mod));
				IBStrAppendCStr(&cFuncCode, " ");
				if (!o->name)Err(OP_Error, "var name NULL");
				IBStrAppendCStr(&cFuncCode, o->name);
				if (o->var.valSet) {
					IBStrAppendCStr(&cFuncCode, " = ");
					Val2Str(valBuf, 32, o->var.val, o->var.type);
					IBStrAppendCStr(&cFuncCode, valBuf);
				}
				IBStrAppendCStr(&cFuncCode, ";\n");
				break;
			}
			}
		}
		if (ibc->Imaginary) {
			//DbgFmt("[@ext @func]","");
			IBStrAppendCStr(&cFuncArgsEnd, ");\n\n");
		}
		else {
			//DbgFmt("[@func]", "");
			IBStr cbOut;
			IBCodeBlock* cb;
			IBStrAppendCStr(&cFuncArgsEnd, ") {\n");
			IBStrInit(&cbOut, 1);
			cb=IBLayer3CodeBlocksTop(ibc);
			IBCodeBlockFinish(cb, &cbOut);
			IBStrAppend(&cFuncCode, &cbOut);
			IBStrFree(&cbOut);
			IBLayer3PopCodeBlock(ibc, false, &cb);
			assert(ibc->CodeBlockStack.elemCount == 1);
			if (!funcObj) {
				Err(OP_Error, "funcObj NULL");
			} else if (funcObj->func.retType != OP_Void) {
				char valBuf[32];
				valBuf[0] = '\0';
				IBStrAppendCStr(&cFuncCode, "\treturn ");
				Val2Str(valBuf, 32, funcObj->func.retVal, funcObj->func.retType);
				IBStrAppendCStr(&cFuncCode, valBuf);
				IBStrAppendCStr(&cFuncCode, ";\n");
			}
			IBStrAppendCStr(&cFuncCode, "}\n\n");
		}
		if (funcObj && strcmp(funcObj->name, "main"))
		{
			IBStrAppendCStr(&ibc->CHeaderFuncs, cFuncModsTypeName.start);
			IBStrAppendCStr(&ibc->CHeaderFuncs, cFuncArgsThing.start);
			if (argc && thingObj) IBStrAppendCStr(&ibc->CHeaderFuncs, ", ");
			IBStrAppendCStr(&ibc->CHeaderFuncs, cFuncArgs.start);
			IBStrAppendCStr(&ibc->CHeaderFuncs, ");\n");
		}
		if (!ibc->Imaginary) {
			IBStrAppendCStr(&ibc->CFile, cFuncModsTypeName.start);
			IBStrAppendCStr(&ibc->CFile, cFuncArgsThing.start);
			if (argc && thingObj) IBStrAppendCStr(&ibc->CFile, ", ");
			IBStrAppendCStr(&ibc->CFile, cFuncArgs.start);
			IBStrAppendCStr(&ibc->CFile, cFuncArgsEnd.start);
			IBStrAppendCStr(&ibc->CFile, cFuncCode.start);
		}
		break;
	}
	case OP_CPrintfHaveFmtStr: {
		bool firstPercent;
		Obj* fmtObj;
		int varIdx;
		int i;
		if (t && wObjs->elemCount) {
			fmtObj = (Obj*)wObjs->data;
			IBStrAppendCh(&cb->code, '\t', tabCount);
			IBStrAppendCStr(&cb->code, "printf(\"");
			firstPercent = false;
			varIdx = 1;
			for (i = 0; i < (int)strlen(fmtObj->str); ++i) {
				char c;
				c = fmtObj->str[i];
				switch (c) {
				case '%':{
						if (!firstPercent) {
							IBStrAppendCStr(&cb->code, "%");
							firstPercent = true;
						}
						else {
							Obj* vo;
							Op voT;
							vo = (Obj*)IBVectorGet(wObjs, varIdx);
							/*printf("cfmt vidx:%d\n",varIdx);*/
							assert(vo);
							voT = ObjGetType(vo);
							/*if(voT==OP_String)DB*/
							switch (voT) {
							case OP_Name:{
								Op type = NameInfoDBFindType(&ibc->NameTypeCtx, vo->name);
								IBStrAppendCStr(&cb->code,
									IBLayer3GetCPrintfFmtForType(ibc, type));
								break;
							}
							case OP_String:
								assert(vo->var.type==OP_String);
							case OP_Value:{
								IBStrAppendCStr(&cb->code,
									IBLayer3GetCPrintfFmtForType(ibc, vo->var.type));
								break;
							}
							case OP_Arg: {
								IBLayer3VecPrint(ibc, wObjs);
								assert(0);
							}
							case OP_CPrintfFmtStr: break;
							default:{
								Err(OP_Error, "unhandled printf arg type");
							}

							}
							firstPercent = false;
							varIdx++;
						}
						break;
					}
				default: {
					char chBuf[2];
					chBuf[0] = c;
					chBuf[1] = '\0';
					IBStrAppendCStr(&cb->code, chBuf);
					break;
				}
				}
			}
			IBStrAppendCStr(&cb->code, "\"");
			if (wObjs->elemCount > 1) {
				IBStrAppendCStr(&cb->code, ", ");
			}
			for (i = 1; i < wObjs->elemCount; ++i) {
				Obj* o;
				o = (Obj*)IBVectorGet(wObjs, i);
				switch (ObjGetType(o)) {
				case OP_Name: {
					IBStrAppendCStr(&cb->code, o->name);
					break;
				}
				case OP_String: {
					IBStrAppendCStr(&cb->code, "\"");
					IBStrAppendCStr(&cb->code, o->str);
					IBStrAppendCStr(&cb->code, "\"");
					break;
				}
				case OP_Value: {
					char valBuf[32];
					valBuf[0] = '\0';
					Val2Str(valBuf, 32, o->val, o->var.type);
					IBStrAppendCStr(&cb->code, valBuf);
					break;
				}
				}
				if (i < wObjs->elemCount - 1) {
					IBStrAppendCStr(&cb->code, ", ");
				}
			}
			IBStrAppendCStr(&cb->code, ");\n");
		}
		break;
	}
	}
	IBLayer3PopTask(ibc, &t);
}
void IBLayer3Prefix(IBLayer3* ibc){
	Obj* obj;
	IBTask* t;
	IBExpects* expTop;
	t = IBLayer3GetTask(ibc);
	assert(t);
	expTop = IBTaskGetExpTop(t);
	/*for assigning func call ret val to var*/
	if (ibc->Pfx == OP_Value && ibc->Ch == '@' && !ibc->Str[0]) {
		IBExpects* exp;
		IBLayer3PushExpects(ibc, &exp);
		ExpectsInit(exp, "P0", OP_Op, 1);
	}
	ibc->Pfx = fromPfxCh(ibc->Ch);
	if(ibc->Pfx == OP_SpaceChar
		|| ibc->Pfx == OP_TabChar) return;
	if (ibc->Pfx == OP_Unknown) Err(OP_ErrUnknownPfx, "catastrophic err");
	obj=IBLayer3GetObj(ibc);
	if (ibc->Pfx != OP_Unknown
		&& (!t || expTop->pfxs.elemCount)
		&& !IBLayer3IsPfxExpected(ibc, ibc->Pfx))
	{
		Err(OP_ErrUnexpectedNextPfx, "");
	}
	IBPushColor(IBBgMAGENTA);
	DbgFmt("PFX","");
	IBPopColor();
	DbgFmt(": ", "");
	IBPushColor(IBBgBROWN);
	DbgFmt("%s(%d)", GetPfxName(ibc->Pfx), (int)ibc->Pfx);
	IBPopColor();
	DbgFmt("\n", "");
	switch (ibc->Pfx) {
	case OP_String: { /* " */
		ibc->StringMode = true;
		IBLayer3Push(ibc, OP_ModeStrPass, false);
		break;
	}
	case OP_VarType:
		IBVectorCopyPushBool(&ibc->StrReadPtrsStack, true);
	case OP_Value:
	case OP_Op:
	case OP_Name:
		/*getchar();*/
		IBLayer3Push(ibc, OP_ModeStrPass, false);
		break;
	case OP_SpaceChar:
		break;
	case OP_Comment:
		break;
	}
	if (ibc->Pfx == OP_Op) {
		if (expTop && expTop->life && --expTop->life <= 0) {
			IBVectorPop(&t->expStack, ExpectsFree);
		}
	}
}
void IBLayer3Str(IBLayer3* ibc){
	char chBuf[2];
	chBuf[0] = ibc->Ch;
	chBuf[1] = '\0';
	if (ibc->StringMode) {
		switch (ibc->Ch) {
		case '"': {
			ibc->StringMode = false;
			IBLayer3StrPayload(ibc);
			return;
		}
		}
	}
	else {
		switch (ibc->Pfx) {
		case OP_Value: {
			switch (ibc->Ch) {
			case '@': {
				IBLayer3Pop(ibc);
				IBLayer3Prefix(ibc);
				return;
			}
			}
			break;
		}
		}
		switch (ibc->Ch) {
		case '\t': return;
		case ' ': {
			if (ibc->StrAllowSpace) break;
			else {
					IBLayer3StrPayload(ibc);
				return;
			}
		}
		case '&': {
			if (*(bool*)IBVectorTop(&ibc->StrReadPtrsStack)) {
				switch (ibc->Pointer) {
				case OP_NotSet:
					DbgFmt("Got pointer\n","");
					ibc->Pointer = OP_Pointer;
					break;
				case OP_Pointer:
					DbgFmt("Got double pointer\n","");
					ibc->Pointer = OP_DoublePointer;
					break;
				case OP_DoublePointer:
					DbgFmt("Got tripple pointer\n","");
					ibc->Pointer = OP_TripplePointer;
					break;
				case OP_TripplePointer:
					Err(OP_ErrQuadriplePointersNOT_ALLOWED, "");
					break;
				}
				return;
			}
		}
		}
	}
	StrConcat(ibc->Str, IBLayer3STR_MAX, chBuf);
}
IBTask* IBLayer3GetTask(IBLayer3* ibc){
	IBTask* ret= (IBTask*)IBVectorTop(&ibc->TaskStack);
	//if (!ret)Err(OP_Error, "no task in stack");
	if (ret) {
		assert(ret->type >= 0);
	}
	return ret;
}
Op IBLayer3GetMode(IBLayer3* ibc){
	Op ret=OP_Null;
	Op* p=(Op*)IBVectorTop(&ibc->ModeStack);
	if (p)ret = *p;
	return ret;
}
IBExpects* IBTaskGetExpTop(IBTask* t){
	IBExpects* ret;
	assert(t);
	ret = (IBExpects*)IBVectorTop(&t->expStack);
	assert(ret);
	return ret;
}
IBVector* IBTaskGetExpPfxsTop(IBTask* t){
	IBExpects* ret;
	assert(t);
	ret = (IBExpects*)IBVectorTop(&t->expStack);
	assert(ret);
	return &ret->pfxs;
}
IBVector* IBTaskGetExpNameOPsTop(IBTask* t){
	IBExpects* ret;
	assert(t);
	ret = (IBExpects*)IBVectorTop(&t->expStack);
	assert(ret);
	return &ret->nameOps;
}
void IBLayer3StrPayload(IBLayer3* ibc){
	Val strVal;
	IBTask *t;
	int tabCount = IBLayer3GetTabCount(ibc);
	IBCodeBlock* cb = IBLayer3CodeBlocksTop(ibc);
	Obj* o;
	t=IBLayer3GetTask(ibc);
	o=IBLayer3GetObj(ibc);
	strVal.i32=atoi(ibc->Str);
	//if(ibc->Pfx==OP_Op) ibc->LastNameOp = ibc->NameOp;
	ibc->NameOp = GetOpFromName(ibc->Str);
	IBPushColor(IBFgGREEN);
	DbgFmt("StrPayload: ", "");
	IBPushColor(IBBgWHITE);
	DbgFmt("%s", ibc->Str);
	IBPopColor();
	IBPopColor();
	DbgFmt("\n", "");
	switch (ibc->Pfx) {
	case OP_String: { /* " */
		switch (o->type) {
		case OP_BlockReturnNeedValue: {
			ObjSetStr(o, ibc->Str);
			o->valType = OP_String;
			IBLayer3FinishTask(ibc);
			break;
		}
		}
		switch(t->type){
		case OP_dbgAssertWantArgs: {
			switch (GetOpFromNameList(ibc->Str, OP_dbgAssert)) {
			case OP_TaskType:{
				break;
			}
			}
			break;
		}
		case OP_CPrintfHaveFmtStr:{
			Obj *o;
			IBLayer3PushObj(ibc, &o);
			ObjSetType(o, OP_String);
			o->var.type=OP_String;
			ObjSetStr(o, ibc->Str);
			IBLayer3PopObj(ibc, true, NULL);
			break;
		}
		case OP_BlockWantCode:
		case OP_IfBlockWantCode:
		case OP_FuncWantCode: { /*printf*/
			IBExpects *ap;
			Obj* o;
			IBLayer3PushTask(ibc, OP_CPrintfHaveFmtStr, &ap, NULL);
			ExpectsInit(ap, "1PPPP", "expected fmt args or line end", 
				OP_Value, OP_Name, OP_String, OP_LineEnd);
			IBLayer3PushObj(ibc, &o);
			ObjSetStr(o, ibc->Str);
			ObjSetType(o, OP_CPrintfFmtStr);
			IBLayer3PopObj(ibc, true, NULL);
			break;
		}
		}
		break;
	}
	case OP_Value: { /*=*/
		switch (t->type) {
		}
		switch (o->type) {
		case OP_ArgNeedValue: {
			if (t->type == OP_CallWantArgs) {
				o->val = strVal;
				ObjSetType(o, OP_Arg);
				o->valType = OP_Value;
				IBLayer3PopObj(ibc, true, &o);
				IBLayer3PushObj(ibc, &o);
				SetObjType(o, OP_ArgNeedValue);
			}
			break;
		}
		case OP_BlockReturnNeedValue: {
			o->val = strVal;
			o->valType = OP_Value;
			IBLayer3FinishTask(ibc);
			break;
		}
		case OP_IfNeedLVal: {
			o->ifO.lvVal = strVal;
			o->ifO.lvTYPE = OP_Value;
			o->ifO.lvDataType = OP_i32;
			SetObjType(o, OP_IfNeedMidOP);
			SetTaskType(t, OP_IfNeedMidOP);
			break;
		}
		case OP_IfNeedRVal: {
			IBExpects* exp;
			o->ifO.rvVal = strVal;
			o->ifO.rvTYPE = OP_Value;
			o->ifO.rvDataType = OP_i32;
			SetObjType(o, OP_IfFinished);
			SetTaskType(t, OP_IfFinished);
			IBLayer3ReplaceExpects(ibc, &exp);
			ExpectsInit(exp, "P", OP_LineEnd);
			break;
		}
		case OP_VarWantValue: {
			Obj* o;
			o = IBLayer3GetObj(ibc);
			o->var.val = strVal;
			o->var.valSet = true;
			SetObjType(o, OP_VarComplete);
			PopExpects();
			break;
		}
		}
		if (ibc->TaskStack.elemCount) {
			switch (t->type) {
			case OP_CPrintfHaveFmtStr:{
				Obj *o;
				IBLayer3PushObj(ibc, &o);
				o->val = strVal;
				ObjSetType(o, OP_Value);
				o->var.type = OP_i32;/*for now*/
				IBLayer3PopObj(ibc, true, NULL);
				break;
			}
			case OP_FuncNeedRetVal: {
				Obj* o;
				int idx;
				idx = 0;
				while (o = (Obj*)IBVectorIterNext(&t->working,&idx)) {
					if (ObjGetType(o) == OP_FuncSigComplete) {
						DbgFmt("Finishing func got ret value\n","");
						o->func.retVal = IBLayer3StrToVal(ibc, ibc->Str, o->func.retType);
						PopExpects();
						SetTaskType(t, OP_Func);
						IBLayer3FinishTask(ibc);
						break;
					}
				}
				break;
			}
			}
		}
		break;
	}
	case OP_VarType: /* % */
		switch (t->type) {
		/*case OP_CallWantArgs: {
			Obj* o;
			IBExpects* exp;
			IBLayer3PushObj(ibc, &o);
			SetObjType(o, OP_ArgNeedValue);
			IBLayer3PushExpects(ibc, &exp);
			ExpectsInit(exp, "P", OP_Name);
			break;
		}*/
		case OP_ThingWantRepr: {
			SetTaskType(t, OP_ThingWantContent);			
			PopExpects();
			break;
		}
		case OP_ThingWantContent: {
			Obj* o;
			IBExpects* exp;
			IBLayer3PushObj(ibc, &o);
			o->var.type = ibc->NameOp;
			o->var.mod = ibc->Pointer;
			o->var.privacy=ibc->Privacy;
			o->var.valSet = false;
			SetObjType(o, OP_VarNeedName);
			IBLayer3PushExpects(ibc, &exp);
			ExpectsInit(exp, "1P", "expected variable name", OP_Name);
			break;
		}
		case OP_BlockWantCode:
		case OP_IfBlockWantCode:
		case OP_FuncWantCode: {
			Obj* o;
			IBExpects* exp;
			IBLayer3PushObj(ibc, &o);
			o->var.type = ibc->NameOp;
			o->var.mod = ibc->Pointer;
			o->var.valSet = false;
			SetObjType(o, OP_VarNeedName);
			IBLayer3PushExpects(ibc, &exp);
			ExpectsInit(exp, "1P", "expected variable name", OP_Name);
			break;
		}
		}
		switch (o->type) {
		case OP_FuncNeedsRetValType: {
			if (t->type != OP_FuncHasName)Err(OP_Error, "func signature needs name");
			IBLayer3GetObj(ibc)->func.retType = ibc->NameOp;
			IBLayer3GetObj(ibc)->func.retTypeMod = ibc->Pointer;
			SetObjType(o, OP_FuncSigComplete);
			break;
		}
		case OP_FuncHasName: {
			Obj* o;
			IBExpects* exp;
			IBLayer3PushObj(ibc, &o);
			SetObjType(o, OP_FuncArgNameless);
			o->arg.type = ibc->NameOp;
			o->arg.mod = ibc->Pointer;
			IBLayer3PushExpects(ibc, &exp);
			ExpectsInit(exp, "1P", "expected func arg name", OP_Name);
			break;
		}
		}
		break;
	case OP_Name: { /* $ */
		switch (o->type) {
		case OP_ArgNeedValue: {
			if (t->type == OP_CallWantArgs) {
				ObjSetStr(o, ibc->Str);
				ObjSetType(o, OP_Arg);
				o->valType = OP_Name;
				IBLayer3PopObj(ibc, true, &o);
				IBLayer3PushObj(ibc, &o);
				SetObjType(o, OP_ArgNeedValue);
				//IBLayer3PopExpects(ibc);
			}
			break;
		}
		case OP_CallNeedName: {
			IBExpects* exp;
			ObjSetStr(o, ibc->Str);
			ObjSetType(o, OP_Call);
			IBLayer3PopObj(ibc, true, &o);
			IBLayer3PushObj(ibc, &o);
			SetObjType(o, OP_ArgNeedValue);
			SetTaskType(t, OP_CallWantArgs);
			IBLayer3ReplaceExpects(ibc, &exp);
			ExpectsInit(exp, "PPPP", OP_Name, OP_Value, OP_String, OP_LineEnd);
			break;
		}
		case OP_BlockReturnNeedValue: {
			ObjSetStr(o, ibc->Str);
			o->valType = OP_Name;
			IBLayer3FinishTask(ibc);
			break;
		}
		}
		switch(t->type){
		case OP_IfNeedLVal: {
			IBExpects* exp;
			OverwriteStr(&o->ifO.lvName, ibc->Str);
			o->ifO.lvTYPE = OP_Name;
			SetObjType(o, OP_IfNeedMidOP);
			SetTaskType(t, OP_IfNeedMidOP);
			IBLayer3ReplaceExpects(ibc, &exp);
			ExpectsInit(exp, "PNNNNN",
				OP_Op, OP_Equals, OP_LessThan, 
				OP_GreaterThan, OP_LessThanOrEquals, 
				OP_GreaterThanOrEquals);
			break;
		}
		case OP_IfNeedRVal: {
			IBExpects* exp;
			OverwriteStr(&o->ifO.rvName, ibc->Str);
			o->ifO.lvTYPE = OP_Name;
			SetObjType(o, OP_IfFinished);
			SetTaskType(t, OP_IfFinished);
			IBLayer3ReplaceExpects(ibc, &exp);
			ExpectsInit(exp, "P", OP_LineEnd);
			break;
		}
		case OP_ThingWantName: {
			Obj* o;
			IBExpects* exp;
			IBLayer3PushObj(ibc, &o);
			assert(ibc->Str[0]!='\0');
			ObjSetName(o, ibc->Str);
			ObjSetType(o, OP_Thing);
			SetTaskType(t, OP_ThingWantRepr);
			PopExpects();
			break;
		}
		case OP_UseNeedStr: {
			Op lib;
			lib = ibc->NameOp;
			switch (lib) {
			case OP_UseStrSysLib: {
				IBPushColor(IBFgIntensity | IBFgYELLOW | IBBgBROWN);
				DbgFmt("Inputting system lib code to compiler\n","");
				IBPopColor();
				IBStrAppendCStr(&ibc->CHeaderFuncs, "/* System Lib Header */\n");
				assert(!ibc->InputStr);
				ibc->InputStr = SysLibCodeStr;
				ibc->Line++;
				ibc->Column = 1;
				break;
			}
			default: {

				break;
			}
			}
			IBLayer3PopTask(ibc,NULL);
			break;
		}
		case OP_SpaceNeedName: {
			Obj* o;
			IBLayer3PushObj(ibc, &o);
			assert(ibc->Str[0]!='\0');
			ObjSetName(o, ibc->Str);
			ObjSetType(o, OP_Space);
			IBLayer3PopObj(ibc, true, &o);
			SetTaskType(t, OP_SpaceHasName);
			IBLayer3FinishTask(ibc);
			break;
		}
		case OP_CPrintfHaveFmtStr: {
			Obj* o;
			IBLayer3PushObj(ibc, &o);
			ObjSetName(o, ibc->Str);
			ObjSetType(o, OP_Name);
			IBLayer3PopObj(ibc, true, NULL);
			break;
		}
		}
		switch (o->type) {
		case OP_VarNeedName: {
			IBExpects* exp;
			ObjSetName(IBLayer3GetObj(ibc), ibc->Str);
			NameInfoDBAdd(&ibc->NameTypeCtx, ibc->Str, IBLayer3GetObj(ibc)->var.type);
			SetObjType(o, OP_VarWantValue);
			PopExpects();
			IBLayer3PushExpects(ibc, &exp);
			ExpectsInit(exp, "1PP",
				"expected value or line end after var name",
				OP_Value, OP_LineEnd);
			break;
		}
		case OP_CallNeedName: { /* =@call */
			IBExpects* exp;
			SetObjType(o, OP_CallWantArgs);
			PopExpects();
			IBLayer3PushExpects(ibc, &exp);
			ExpectsInit(exp, "1PPP",
				"expected var type or line end after func name",
				OP_Name, OP_Value, OP_LineEnd);
		}
		case OP_Func: {
			IBExpects* exp;
			SetObjType(o, OP_FuncHasName);
			SetTaskType(t, OP_FuncHasName);
			IBLayer3PushExpects(ibc, &exp);
			ExpectsInit(exp, "PPPN",
				OP_VarType, OP_Op, OP_LineEnd, OP_Return);
			ObjSetName(IBLayer3GetObj(ibc), ibc->Str);
			break;
		}
		case OP_FuncArgNameless:
			SetObjType(o, OP_FuncArgComplete);
			PopExpects();
			ObjSetName(IBLayer3GetObj(ibc), ibc->Str);
			NameInfoDBAdd(&ibc->NameTypeCtx, ibc->Str, IBLayer3GetObj(ibc)->arg.type);
			IBLayer3PopObj(ibc, true, NULL);
			break;
		}
		break;
	}
	case OP_Op: { /* @ */
		bool expected;
		expected = IBLayer3IsNameOpExpected(ibc, ibc->NameOp);
		if(!expected)Err(OP_ErrUnexpectedNameOP, "unexpected nameOP");
		switch (ibc->NameOp) {
		/*case OP_IfNeedMidOP: {

			break;
		}*/
		case OP_Repr: {
			switch (t->type) {
			case OP_ThingWantRepr: {
				IBExpects* exp;
				//SetTaskType(t, OP_ThingWantContent);
				IBLayer3PushExpects(ibc, &exp);
				ExpectsInit(exp, "1P", "expected vartype (%)", OP_VarType);
				break;
			}
			}
			break;
		}
		case OP_Space: {
			IBExpects* ap;
			IBLayer3PushTask(ibc, OP_SpaceNeedName, &ap, NULL);
			ExpectsInit(ap, "1P", "expected space name", OP_Name);
			break;
		}
		case OP_Thing: {
			IBExpects* ap;
			IBLayer3PushTask(ibc, OP_ThingWantName, &ap, NULL);
			ExpectsInit(ap, "PNN", OP_Op, OP_Done);
			IBLayer3PushExpects(ibc, &ap);
			ExpectsInit(ap, "PPN", OP_Op, OP_LineEnd, OP_Repr);
			IBLayer3PushExpects(ibc, &ap);
			ExpectsInit(ap, "1P", "expected thing name", OP_Name);
			break;
		}
		case OP_dbgAssert: {
			IBExpects* ap;
			IBLayer3PushTask(ibc, OP_dbgAssertWantArgs, &ap, NULL);
			ExpectsInit(ap, "1P", "expected string", OP_String);
			break;
		}
		case OP_Call: {
			switch (t->type) {
			case OP_BlockWantCode:
			case OP_IfBlockWantCode:
			case OP_FuncWantCode: {
				IBTask* t;
				IBExpects* exp;
				Obj* o;
				IBLayer3PushObj(ibc, &o);
				ObjSetType(o, OP_CallNeedName);
				IBLayer3PushTask(ibc, OP_CallNeedName, &exp, &t);
				ExpectsInit(exp, "P", OP_Name);
				break;
			}
			}
			switch (o->type) {
			case OP_VarWantValue: {
				Obj* o;
				IBExpects* exp;
				IBLayer3PushObj(ibc, &o);
				ObjSetType(o, OP_CallNeedName);
				IBLayer3PushExpects(ibc, &exp);
				ExpectsInit(exp, "1P", "expected function name", OP_Name);
			}
			}
			break;
		}
		case OP_dbgBreak: {
			/*__debugbreak();*/
			/*ibc->TaskStack;*/
			break;
		}
		case OP_Imaginary: {
			ibc->Imaginary = true;
			IBPushColor(IBFgMAGENTA);
			DbgFmt("[GOT IMAGINARY]","");
			IBPopColor();
			DbgFmt("\n","");
			break;
		}
		case OP_Done:
			if (ibc->TaskStack.elemCount < 1) Err(OP_ErrNoTask, "");
			switch (t->type) {
			case OP_ThingWantContent: {
				Obj* o;
				o = IBLayer3GetObj(ibc);
				assert(o->type == OP_Thing);
				IBLayer3PopObj(ibc, true, &o);
				assert(o->type == OP_NotSet);
			}
			case OP_BlockWantCode:
			case OP_IfBlockWantCode: {
				IBLayer3FinishTask(ibc);
				break;
			}
			case OP_Func:
			case OP_FuncHasName:
			case OP_FuncWantCode: {
				Obj* o;
				IBVector* wo;
				int idx;
				assert(t);
				PLINE;
				DbgFmt(" Finishing function\n","");
				idx = 0;
				/*t = NULL;*/
				o = NULL;
				wo = &t->working;
				while (o = (Obj*)IBVectorIterNext(wo, &idx)) {
					/*TODO: could cache func obj index later*/
					if (ObjGetType(o) == OP_FuncSigComplete) {
						if (o->func.retType != OP_Void) {
							IBExpects* exp;
							IBLayer3PushExpects(ibc, &exp);
							if (o->func.retType == OP_c8 && o->func.retTypeMod == OP_Pointer) {
								ExpectsInit(exp, "PP", OP_Name, OP_String);
							}
							else {
								ExpectsInit(exp, "P", OP_Value);
							}
							SetTaskType(t, OP_FuncNeedRetVal);
						}
						else {
							SetTaskType(t, OP_Func);
							IBLayer3FinishTask(ibc);
						}
					}
				}
				break;
			}
			}
			break;
		case OP_Return: {
			switch (t->type) {
			case OP_BlockWantCode:
			case OP_IfBlockWantCode:
			case OP_FuncWantCode: {
				IBTask* t;
				IBExpects* exp;
				Obj* o;
				IBLayer3PushObj(ibc, &o);
				ObjSetType(o, OP_BlockReturnNeedValue);
				IBLayer3PushTask(ibc, OP_BlockReturnNeedValue, &exp, &t);
				ExpectsInit(exp, "PPP", OP_Value, OP_String, OP_Name);
				break;
			}
			}
			switch (o->type) {
			case OP_FuncHasName: {
				IBExpects* exp;
				SetObjType(o, OP_FuncNeedsRetValType);
				IBLayer3PushExpects(ibc, &exp);
				ExpectsInit(exp, "P", OP_VarType);
				break;
			}
			/*default:
				Err(OP_ErrUnexpectedOp, "");
				break;*/
			}
			break;
		}
		case OP_Func: { //func1
			IBExpects* ap;
			Obj* o;
			IBTask* t;
			t = IBLayer3GetTask(ibc);
			assert(t);
			IBLayer3PushObj(ibc, &o);
			o->func.thingTask = t->type == OP_ThingWantContent ? t : NULL;
			IBLayer3PushTask(ibc, OP_FuncNeedName, &ap, NULL);
			ExpectsInit(ap, "1P", "expected function name", OP_Name);
			o->type = ibc->NameOp;
			o->privacy = ibc->Privacy;
			o->func.retType = OP_Void;
			o->func.retTypeMod = OP_NotSet;
			break;
		}
		case OP_Public:
		case OP_Private:
			ibc->Privacy = ibc->NameOp;
			break;
		case OP_Use: {
			IBExpects* ap;
			IBLayer3PushTask(ibc, OP_UseNeedStr, &ap, NULL);
			ExpectsInit(ap, "1P", "expected @use $name", OP_Name);
			break;
		}
		case OP_LessThanOrEquals: //@lteq
		case OP_GreaterThanOrEquals: //@gteq
		case OP_LessThan: //@lt
		case OP_GreaterThan: //@gt
		case OP_NotEquals: //@neq
		case OP_Equals: { //@eq
			switch (t->type) {
			case OP_IfNeedMidOP: {
				IBExpects* exp;
				o->ifO.midOp = ibc->NameOp;
				ObjSetType(o, OP_IfNeedRVal);
				SetTaskType(t, OP_IfNeedRVal);
				IBLayer3ReplaceExpects(ibc, &exp);
				ExpectsInit(exp, "PP", OP_Name, OP_Value);
				break;
			}
			}
			break;
		}
		case OP_Else: {
			switch (t->type) {
			case OP_IfBlockWantCode: {
				IBTask* t;
				IBExpects* exp;
				IBLayer3FinishTask(ibc);
				IBLayer3PushTask(ibc, OP_BlockWantCode, &exp, &t);
				ExpectsInit(exp, "c", OP_Null);
				cb = IBLayer3CodeBlocksTop(ibc);
				IBStrAppendCh(&cb->code, '\t', tabCount - 1);
				IBStrAppendCStr(&cb->code, "else ");
				IBLayer3PushCodeBlock(ibc, &cb);
				break;
			}
			}
			break;
		}
		case OP_ElseIf: {
			switch (t->type) {
			case OP_IfBlockWantCode: {
				IBLayer3FinishTask(ibc);
				ibc->LastNameOp = ibc->NameOp;
				IBLayer3PushTask(ibc, OP_ElseIf, NULL, &t);
				break;
			}
			default: {
				Err(OP_YouCantUseThatHere, "You may only use @elif in @if blocks!!!");
				break;
			}
			}
		}
		case OP_If: {
			switch (t->type) {
			case OP_ElseIf: {
				cb = IBLayer3CodeBlocksTop(ibc);
				IBStrAppendCh(&cb->code, '\t', tabCount - 1);
				IBStrAppendCStr(&cb->code, "else ");
				IBLayer3PopTask(ibc, &t);
			}
			case OP_BlockWantCode:
			case OP_IfBlockWantCode:
			case OP_FuncWantCode: {
				IBTask* nt;
				IBExpects* nexp;
				Obj* o;
				IBLayer3PushObj(ibc, &o);
				ObjSetType(o, OP_IfNeedLVal);
				IBLayer3PushTask(ibc, OP_IfNeedLVal, &nexp, &nt);
				ExpectsInit(nexp, "1PP", "expected lval", OP_Value, OP_Name/*, OP_String*/);
				break;
			}
			default: {
				Err(OP_Error, "Unimplemented If task context");
				break;
			}
			}
			break;
		}
		default:
			Err(OP_ErrUnknownOpStr, "");
		}
		break;
	}
	}
	ibc->Str[0] = '\0';
#ifdef DEBUGPRINTS
	IBPushColor(IBFgMAGENTA);
	printf("Str payload complete");
	IBPopColor();
	printf("\n");
#endif
	IBLayer3Pop(ibc);
	if(ibc->StrReadPtrsStack.elemCount > 1){
		if (*(bool*)IBVectorTop(&ibc->StrReadPtrsStack))
			ibc->Pointer = OP_NotSet;
		IBVectorPop(&ibc->StrReadPtrsStack, NULL);
	}
//#ifdef _DEBUG
//#define IBOPSTEP
//#endif
#ifdef IBOPSTEP
	{
		int l = ibc->InputStr ? ibc->LineIS : ibc->Line;
		int c = ibc->InputStr ? ibc->ColumnIS : ibc->Column;
		IBPushColor(IBFgRED);
		DbgFmt("[");
		IBPushColor(IBBgGREEN);
		DbgFmt("%d:%d", l, c);
		IBPopColor();
		DbgFmt("]");
		IBPopColor();
		IBPushColor(IBBgYELLOW);
		DbgFmt("Press enter to ", "");
		IBPushColor(IBBgRED | IBFgYELLOW);
		DbgFmt("step", "");
		IBPopColor();
		IBPopColor();
		DbgFmt("\n", "");
		getchar();
	}
#endif
}
void IBLayer3ExplainErr(IBLayer3* ibc, Op code) {
	switch (code) {
	case OP_YouCantUseThatHere: {
		printf("You can't use that here");
		break;
	}
	case OP_FuncNeedRetVal: {
		printf("You forgot to return a value from the function");
		break;
	}
	case OP_ErrUnexpectedNameOP: {
		IBExpects* exp;
		IBTask* t;
		t=IBLayer3GetTask(ibc);
		assert(t);
		exp = IBTaskGetExpTop(t);
		printf("NameOP \"@%s\" wasn't expected.\nExpected:\n", ibc->Str);
		ExpectsPrint(exp);
		break;
	}
	case OP_ErrUnknownPfx:
		printf("This prefix \"%c\" is unknown!", ibc->Ch);
		break;
	case OP_ErrUnknownOpStr:
		printf("Unknown OP str @%s\n", ibc->Str);
		break;
	case OP_ErrQuadriplePointersNOT_ALLOWED:
		printf("quadruple pointer is not allowed");
		break;
	case OP_ErrUnexpectedOp:
		printf("Unexpected OP");
		break;
	case OP_ErrNoTask:
		printf("No working task to call done (@@) for");
		break;
	case OP_ErrUnexpectedNextPfx: {
		Op* oi;
		IBExpects *ap;
		IBTask *t;
		int idx;
		t =IBLayer3GetTask(ibc);
		ap=IBTaskGetExpTop(t);
		if(ap && t){
			assert(ap->pfxs.elemCount);
			printf("Err: \"%s\" Unexpected next prefix %s. Pfx idx:%d\nEnforced at line %d. Allowed:",
				ap->pfxErr, GetPfxName(ibc->Pfx),
					ap->pfxs.elemCount - 1, ap->lineNumInited);
			idx = 0;
			while (oi = (Op*)IBVectorIterNext(&ap->pfxs,&idx)) {
				printf("%s(%d),", GetPfxName(*oi), (int)*oi);
			}
		}else{
			printf("No task. Only Op(@) prefix allowed. Pfx: %s. Allowed pfxs: %s\n", GetPfxName(ibc->Pfx),
					GetPfxName(OP_Op));
		}
		break;
	}
	case OP_ErrExpectedVariablePfx:
		printf("Expected a variable type to be next.");
		break;
	default:
		printf("Err msg unimplemented for %s", GetOpName(code));
	}
#ifdef DEBUGPRINTS
	DbgFmt("\nOBJ:","");
	ObjPrint(IBLayer3GetObj(ibc));
	//DbgFmt("\n","");
#endif
	printf("\n");
}
void IBSetColor(IBColor col) {
#ifdef _WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), col);
#endif
}
int main(int argc, char** argv) {
	IBDatabase db;
	int rv;
	FILE* f;
	if (argc < 2) {
		printf("Please specify a file\n");
		getchar();
		return -1;
	}
	rv = 1;
	IBVectorInit(&g_ColorStack, sizeof(IBColor), OP_IBColor);
	IBPushColor(IBFgWHITE);
	g_DB = &db;
	IBDatabaseInit(g_DB);
	f = fopen(argv[1], "r");
	if (f){
		IBLayer3 comp;
		IBLayer3Init(&comp);
		while (comp.Running)
			IBLayer3Tick(&comp, f);
		DbgFmt("Exiting\n","");
		//assert(comp.InputStr == NULL);
		IBLayer3Free(&comp);
		getchar();
		fclose(f);
		rv = 0;
	}
	else{
		printf("Error opening file\n");
	}
	
	IBVectorFreeSimple(&g_ColorStack);
	return rv;
}
#endif
