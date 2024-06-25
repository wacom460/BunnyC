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

#define FGWHITE   IBSetColor(IBFgWHITE  )
#define FGRED     IBSetColor(IBFgRED    )
#define FGGREEN   IBSetColor(IBFgGREEN  )
#define FGBLUE    IBSetColor(IBFgBLUE   )
#define FGYELLOW  IBSetColor(IBFgYELLOW )
#define FGCYAN    IBSetColor(IBFgCYAN   )
#define FGMAGENTA IBSetColor(IBFgMAGENTA)
#define FGBROWN   IBSetColor(IBFgBROWN  )

#define DEBUGPRINTS

#ifdef DEBUGPRINTS
#define PLINE printf("[%d]", __LINE__)
#else
#define PLINE
#endif
#define OP_NAME_LEN 32
#define COMMENT_CHAR ('~')
#define IBComp1STR_MAX 64
#define ThingStructTypeHeaderVarType ("int")
#define ThingStructTypeHeaderVarName ("__thingTYPE")
#if defined(__TINYC__) || defined(__GNUC__)
#define __debugbreak()
#endif
#define DB __debugbreak();

#define IBASSERT(x, errMsg){\
	if(!(x)) {\
		PLINE;\
		FGRED;\
		printf("Assertion failed!!! -> %s\n%s", errMsg, #x);\
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

	OP_SpaceChar, OP_Comma, OP_CommaSpace, OP_Name, OP_String,
	OP_CPrintfFmtStr, OP_Char, OP_If, OP_Else, OP_For, OP_While,
	OP_Block, OP_c8, OP_u8, OP_u16, OP_u32, OP_u64, OP_i8, OP_i16,
	OP_i32, OP_i64, OP_f32, OP_d64, OP_Pointer, OP_DoublePointer,
	OP_TripplePointer, OP_IBComp1Flags, OP_dbgBreak, OP_dbgAssert,
	OP_dbgAssertWantArgs,OP_TaskType, OP_TaskStack, OP_NotEmpty,
	OP_TabChar,OP_UseNeedStr,OP_UseStrSysLib,OP_NameInfoDB,
	OP_NameInfo,OP_Expects,

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
void IBStrInitNTStr(IBStr* str, char* nullTerminated);
size_t IBStrGetLen(IBStr* str);
void IBStrAppendCh(IBStr* str, char ch, int count);
char* IBStrAppendCStr(IBStr* str, char *with);
void IBStrAppendFmt(IBStr* str, char* fmt, ...);
char* IBStrAppend(IBStr* str, IBStr* with);
typedef union IBVecData {
	void* ptr;
	struct Obj* obj;
	struct IBTask* task;
	Op* op;
	bool *boolean;
	struct Expects* expects;
	struct NameInfoDB* niDB;
	struct NameInfo* ni;
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
IBVecData* IBVectorPush(IBVector* vec);
void IBVectorCopyPush(IBVector* vec, void* elem);
void IBVectorCopyPushBool(IBVector* vec, bool val);
void IBVectorCopyPushOp(IBVector* vec, Op val);
void IBVectorCopyPushIBColor(IBVector* vec, IBColor col);
IBVecData* IBVectorTop(IBVector* vec);
IBVecData* IBVectorFront(IBVector* vec);
void IBVectorPop(IBVector* vec, void(*freeFunc)(void*));
void IBVectorPopFront(IBVector* vec, void(*freeFunc)(void*));
void IBVectorFreeSimple(IBVector* vec);
#define IBVectorFree(vec, freeFunc){\
	int i;\
	for(i = 0;i<(vec)->elemCount;i++){\
		freeFunc((void*)IBVectorGet((vec), i));\
	}\
	IBVectorFreeSimple((vec));\
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
typedef struct NameInfo {
	Op type;
	char* name;
} NameInfo;
typedef struct NameInfoDB {
	IBVector pairs;
} NameInfoDB;
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
typedef struct Expects {
	IBVector pfxs;/*Op P */
	IBVector nameOps;/*Op N */
	char* pfxErr;
	char* nameOpErr;
	int life;
	int lineNumInited;
} Expects;
void _ExpectsInit(int LINENUM, Expects* exp, int life,
	char* pfxErr, char* nameOpErr, char* fmt, ...);
/*special fmt chars :
* 'P': pfx
* 'N': nameOP
* 
* PUT AT END OF FMT STR: e.g. "PPPNNc"
* 'c': code block macro (adds OP_Op, OP_If, OP_VarType... etc)
*/
#define ExpectsInit(exp, life, pfxErr, nameOpErr, fmt, ...) \
	_ExpectsInit(__LINE__, exp, life, pfxErr, nameOpErr, fmt, __VA_ARGS__);
void ExpectsPrint(Expects* exp);
void ExpectsFree(Expects* exp);
typedef struct IBTask {
	Op type;
	IBVector expStack; /*Expects*/
	IBVector working;/*Obj*/
} IBTask;
void TaskInit(IBTask* t, Op type);
void TaskFree(IBTask* t);
typedef struct IBComp1 {
	int Line;
	int Column;
	Op Pfx;
	char Str[IBComp1STR_MAX];
	IBStr CHeaderStructs;/* .h */
	IBStr CHeaderFuncs;
	IBStr CFile;/* .c */

	IBVector ObjStack; /*Obj*/
	IBVector ModeStack; /*Op*/
	IBVector TaskStack; /*IBTask*/
	IBVector StrReadPtrsStack; /*bool*/
	IBVector ColorStack; /*IBColor*/
	IBVector CodeBlockStack; /*IBCodeBlock*/

	char* InputStr;
	Op Pointer;
	Op Privacy;
	Op NameOp;
	char Ch;
	char LastCh;
	bool Running;
	bool StringMode;
	bool StrAllowSpace;
	Op CommentMode;
	NameInfoDB NameTypeCtx;
} IBComp1;
void _Err(IBComp1* ibc, Op code, char *msg);
#define Err(code, msg){\
	IBComp1PushColor(ibc, IBFgRED);\
	PLINE;\
	_Err(ibc, code, msg);\
}
Obj* IBComp1GetObj(IBComp1* ibc);
void IBComp1PrintVecData(IBComp1* ibc, IBVecData* data, Op type);
void IBComp1VecPrint(IBComp1* ibc, IBVector* vec);
Obj* IBComp1FindStackObjUnderIndex(IBComp1* ibc, int index, Op type);
Obj* IBComp1FindStackObjUnderTop(IBComp1* ibc, Op type);
void IBComp1Init(IBComp1* ibc);
IBTask* IBComp1FindTaskUnderIndex(IBComp1* ibc, int index, Op type);
void IBComp1Free(IBComp1* ibc);
void IBComp1PushColor(IBComp1* ibc, IBColor col);
void IBComp1PopColor(IBComp1* ibc);
int IBComp1GetTabCount(IBComp1* ibc);
IBCodeBlock* IBComp1CodeBlocksTop(IBComp1* ibc);
void _IBComp1PushCodeBlock(IBComp1* ibc, IBCodeBlock** cbDP);
#define IBComp1PushCodeBlock(ibc, cbDP){\
	PLINE;\
	_IBComp1PushCodeBlock(ibc, cbDP);\
}
void _IBComp1PopCodeBlock(IBComp1* ibc, bool copyToParent, IBCodeBlock** cbDP);
#define IBComp1PopCodeBlock(ibc, copyToParent, cbDP){\
	PLINE;\
	_IBComp1PopCodeBlock(ibc, copyToParent, cbDP);\
}
void _IBComp1PushTask(IBComp1* ibc, Op taskOP, Expects** exectsDP, IBTask** taskDP);
#define IBComp1PushTask(ibc, taskOP, exectsDP, taskDP){\
	PLINE;\
	_IBComp1PushTask(ibc, taskOP, exectsDP, taskDP);\
}
void _IBComp1PopTask(IBComp1* ibc);
#define IBComp1PopTask(ibc){\
	PLINE;\
	_IBComp1PopTask(ibc);\
}
void _IBComp1PushObj(IBComp1* ibc, Obj** o);
#define IBComp1PushObj(ibc, objDP){\
	PLINE;\
	_IBComp1PushObj(ibc, objDP);\
}
#define GetObjCount (ibc->ObjStack.elemCount)
void _IBComp1PopObj(IBComp1* ibc, bool pushToWorking, Obj **objDP);
#define IBComp1PopObj(ibc, p2w, objDP){\
	PLINE;\
	_IBComp1PopObj(ibc, p2w, objDP);\
}
void _IBComp1Push(IBComp1* ibc, Op mode, bool strAllowSpace);
#define IBComp1Push(ibc, mode, strAllowSpace){\
	PLINE;\
	_IBComp1Push(ibc, mode, strAllowSpace);\
}
void _IBComp1Pop(IBComp1* ibc);
#define IBComp1Pop(ibc){\
	PLINE;\
	_IBComp1Pop(ibc);\
}

/*life:0 = infinite, -1 life each pfx*/
void IBComp1PushExpects(IBComp1* ibc, Expects** expDP);

void IBComp1PopExpects(IBComp1* ibc);

/*frees exps top and ow expDP with top ptr for user to init again */
void IBComp1ReplaceExpects(IBComp1* ibc, Expects** expDP);

bool IBComp1IsPfxExpected(IBComp1* ibc, Op pfx);
bool IBComp1IsNameOpExpected(IBComp1* ibc, Op nameOp);
/*NO NEWLINES AT END OF STR*/
void IBComp1Tick(IBComp1* ibc, FILE *f);
void IBComp1InputChar(IBComp1* ibc, char ch);
void IBComp1InputStr(IBComp1* ibc, char* str);
void IBComp1FinishTask(IBComp1* ibc);
Val IBComp1StrToVal(IBComp1* ibc, char* str, Op expectedType);
char* IBComp1GetCPrintfFmtForType(IBComp1* ibc, Op type);
void IBComp1Prefix(IBComp1* ibc);
void IBComp1Str(IBComp1* ibc);
void IBComp1StrPayload(IBComp1* ibc);
void IBComp1ExplainErr(IBComp1* ibc, Op code);
#define SetObjType(obj, tt){\
	PLINE;\
	DbgFmt("SetObjType: %s(%d) -> %s(%d)\n", GetOpName(obj->type), (int)obj->type, GetOpName(tt), (int)tt);\
	obj->type=tt;\
}
#define PopExpects(){\
	PLINE;\
	IBComp1PopExpects(ibc);\
}
IBTask* IBComp1GetTask(IBComp1* ibc);
Op IBComp1GetMode(IBComp1* ibc);

Expects* TaskGetExpTop(IBTask* t);
IBVector* TaskGetExpPfxsTop(IBTask *t);
IBVector* TaskGetExpNameOPsTop(IBTask* t);

#define SetTaskType(task, tt){\
	assert(task);\
	PLINE;\
	printf(" SetTaskType: %s -> %s\n", GetOpName(task->type), GetOpName(tt));\
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
char* IBComp1StringModeIgnoreChars = "";
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
	{"NoChange",OP_NoChange}
};
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
"\n";
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
IBVecData* IBVectorPush(IBVector* vec) {
	IBVecData* topPtr;
	if (vec->elemCount + 1 > vec->slotCount) {
		void* ra;
		ra = NULL;
		vec->slotCount++;
		vec->dataSize = vec->elemSize * vec->slotCount;
		assert(vec->data);
		ra = realloc(vec->data, vec->dataSize);
		assert(ra);
		if(ra)vec->data = ra;
	}
	topPtr = (IBVecData*)((char*)vec->data + vec->elemSize * vec->elemCount);
	vec->elemCount++;
	return topPtr;
}
void IBVectorCopyPush(IBVector* vec, void* elem) {
	memcpy(IBVectorPush(vec), elem, vec->elemSize);
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
void IBVectorPop(IBVector* vec, void(*freeFunc)(void*)){
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
void NameInfoInit(NameInfo* info){
	info->type=OP_NotSet;
	info->name=NULL;
}
void NameInfoFree(NameInfo* info) {
	free(info->name);
}
void NameInfoDBInit(NameInfoDB* db) {
	IBVectorInit(&db->pairs, sizeof(NameInfo), OP_NameInfo);
}
void NameInfoDBAdd(NameInfoDB* db, char* name, Op type) {
	NameInfo info;
	info.type = type;
	info.name = strdup(name);
	IBVectorCopyPush(&db->pairs, &info);
}
Op NameInfoDBFindType(NameInfoDB* db, char* name) {
	NameInfo* pair;
	int idx;
	idx = 0;
	while (pair = (NameInfo*)IBVectorIterNext(&db->pairs, &idx)) {
		if (!strcmp(pair->name, name))
			return pair->type;
	}
	return OP_NotFound;
}
void NameInfoDBFree(NameInfoDB* db) {
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
void _ExpectsInit(int LINENUM, Expects* exp, int life,
	char *pfxErr, char* nameOpErr, char *fmt, ...) {
	va_list args;
	Op pfx;
	Op nameOp;
	int i;
	IBVectorInit(&exp->pfxs, sizeof(Op), OP_Op);
	IBVectorInit(&exp->nameOps, sizeof(Op), OP_Op);
	exp->pfxErr=pfxErr;
	exp->nameOpErr=nameOpErr;
	exp->life=life;
	exp->lineNumInited=LINENUM;
	DbgFmt("Expect: { ","");
	va_start(args, fmt);
	for (i = 0; i < strlen(fmt); i++) {
		char ch = fmt[i];
		switch (ch) {
		case 'P': {
			pfx=va_arg(args, Op);
			IBVectorCopyPushOp(&exp->pfxs, pfx);
			DbgFmt("Pfx:%s(%d) ", GetPfxName(pfx), (int)pfx);
			break;
		}
		case 'N':{
			nameOp=va_arg(args, Op);
			IBVectorCopyPushOp(&exp->nameOps, nameOp);
			DbgFmt("NameOP:%s(%d) ", GetOpName(nameOp), (int)nameOp);
			break;
		}
		case 'c': {
			PLINE;
			DbgFmt("CodeBlockMacro ", "");
			IBVectorCopyPushOp(&exp->pfxs, OP_Op);
			IBVectorCopyPushOp(&exp->pfxs, OP_VarType);
			IBVectorCopyPushOp(&exp->pfxs, OP_String);
			IBVectorCopyPushOp(&exp->nameOps, OP_If);
			IBVectorCopyPushOp(&exp->nameOps, OP_Set);
			IBVectorCopyPushOp(&exp->nameOps, OP_Done);
			break;
		}
		}
	}
	DbgFmt("}\n","");
	va_end(args);
}
void ExpectsPrint(Expects* ap) {
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
void ExpectsFree(Expects* ap) {
	assert(ap);
	IBVectorFreeSimple(&ap->pfxs);
	IBVectorFreeSimple(&ap->nameOps);
}
void TaskInit(IBTask* t, Op type) {
	IBVectorInit(&t->working, sizeof(Obj), OP_Obj);
	IBVectorInit(&t->expStack, sizeof(Expects), OP_Expects);
	t->type = type;
}
void TaskFree(IBTask* t) {
	assert(t);
	IBVectorFree(&t->expStack, ExpectsFree);
	IBVectorFree(&t->working, ObjFree);
}
void _Err(IBComp1* ibc, Op code, char *msg){
	printf("Error at %u:%u \"%s\"(%d). %s\n",
		ibc->Line, ibc->Column, GetOpName(code), (int)code, msg);
	IBComp1ExplainErr(ibc, code);
	IBComp1PopColor(ibc);
#ifdef DEBUGPRINTS
	/*IBComp1VecPrint(ibc, &ibc->ObjStack);
	IBComp1VecPrint(ibc, &ibc->TaskStack);*/
#endif
	IBComp1PushColor(ibc, IBFgCYAN);
	printf("PRESS ENTER TO CONTINUE ANYWAY");
	IBComp1PopColor(ibc);
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
Obj* IBComp1GetObj(IBComp1* ibc) {
	return (Obj*)IBVectorTop(&ibc->ObjStack);
}
void IBComp1PrintVecData(IBComp1* ibc, IBVecData* data, Op type){
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
		IBComp1PrintVecData(ibc, obj->func.thingTask, OP_Task);
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
		Expects* exp = (Expects*)data;
		DbgFmt("EXPECTS -> [\n"
			"\t\tpfxErr: %s\n"
			"\t\tnameOpErr: %s\n"
			"\t\tlife: %d\n"
			"\t\tlineNumInited: %d\n"
			"\t\tpfxs:\n",
			exp->pfxErr, exp->nameOpErr, exp->life, exp->lineNumInited);
		IBComp1VecPrint(ibc, &exp->pfxs);
		DbgFmt("\t\tnameOps:\n", "");
		IBComp1VecPrint(ibc, &exp->nameOps);
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
		IBComp1VecPrint(ibc, &task->working);
		DbgFmt("\tExpects:\n", "");
		IBComp1VecPrint(ibc, &task->expStack);
		DbgFmt("]\n", "");
		break;
	}
	default:
		break;
	}
}
void IBComp1VecPrint(IBComp1* ibc, IBVector* vec) {
	int idx;
	IBVecData* data;

	assert(vec);
	switch (vec->type) {
	case OP_Op: {
		IBComp1PushColor(ibc, IBFgCYAN);
		DbgFmt("OPs","")
		break;
	}
	case OP_Obj: {
		IBComp1PushColor(ibc, IBFgYELLOW);
		DbgFmt("OBJs", "");
		break;
	}
	case OP_Expects: {
		IBComp1PushColor(ibc, IBFgRED);
		DbgFmt("EXPECTs", "");
		break;
	}
	case OP_Task: {
		IBComp1PushColor(ibc, IBFgBLUE);
		DbgFmt("TASKs", "");
		break;
	}
	default: {
		IBComp1PushColor(ibc, IBFgMAGENTA);
		DbgFmt("UNKNOWN!", "");
		break;
	}
	}
	DbgFmt(" vec -> [\n", "");
	data = NULL;
	idx = 0;
	while (data = IBVectorIterNext(vec, &idx)) {
		DbgFmt("\t(%d)", idx);
		IBComp1PrintVecData(ibc, data, vec->type);		
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
	IBComp1PopColor(ibc);
}
Obj* IBComp1FindStackObjUnderIndex(IBComp1* ibc, int index, Op type) {
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
Obj* IBComp1FindStackObjUnderTop(IBComp1* ibc, Op type){
	Obj* o;
	int i;
	if(ibc->ObjStack.elemCount < 2) return NULL;
	for (i = ibc->ObjStack.elemCount - 1; i >= 0;) {
		o = (Obj*)IBVectorGet(&ibc->ObjStack, i--);
		if (o->type == type) return o;
	}
	return NULL;
}
void IBComp1Init(IBComp1* ibc){
	Obj* o;
	Expects* exp;
	IBCodeBlock* cb;
	ibc->Running = true;
	ibc->Line = 1;
	ibc->Column = 1;
	ibc->Pfx = OP_Null;
	ibc->Str[0] = '\0';
	IBStrInit(&ibc->CHeaderStructs, 1);
	IBStrInit(&ibc->CHeaderFuncs, 1);
	IBStrAppendCStr(&ibc->CHeaderStructs, "#ifndef HEADER_H_\n#define HEADER_H_\n\n");
	IBStrInit(&ibc->CFile, 1);
	IBStrAppendCStr(&ibc->CFile, "#include \"header.h\"\n\n");
	ibc->Pointer = OP_NotSet;
	ibc->Privacy = OP_Public;
	ibc->NameOp = OP_Null;
	ibc->Ch = '\0';
	ibc->LastCh = '\0';
	ibc->StringMode = false;
	ibc->StrAllowSpace = false;
	ibc->CommentMode = OP_NotSet;
	ibc->InputStr = NULL;
	NameInfoDBInit(&ibc->NameTypeCtx);
	IBVectorInit(&ibc->ObjStack, sizeof(Obj), OP_Obj);
	IBVectorInit(&ibc->ModeStack, sizeof(Op), OP_Op);
	IBVectorInit(&ibc->StrReadPtrsStack, sizeof(bool), OP_Bool);
	IBVectorInit(&ibc->TaskStack, sizeof(IBTask), OP_Task);
	IBVectorInit(&ibc->ColorStack, sizeof(IBColor), OP_IBColor);
	IBVectorInit(&ibc->CodeBlockStack, sizeof(IBCodeBlock), OP_IBCodeBlock);
	cb=(IBCodeBlock*)IBVectorPush(&ibc->CodeBlockStack);
	IBCodeBlockInit(cb);
	IBComp1PushColor(ibc, IBFgWHITE);
	IBVectorCopyPushBool(&ibc->StrReadPtrsStack, false);
	IBComp1Push(ibc, OP_ModePrefixPass, false);
	IBComp1PushObj(ibc, &o);
	IBComp1PushTask(ibc, OP_RootTask, &exp, NULL);
	ExpectsInit(exp, 0, "", "", "PNNNNNNN", 
		OP_Op, OP_Use, OP_Imaginary, OP_Func, 
		OP_Thing, OP_Space, OP_Public, OP_Private);
}
IBTask* IBComp1FindTaskUnderIndex(IBComp1* ibc, int index, Op type){
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
void IBComp1Free(IBComp1* ibc) {
	IBTask* t;
	Obj* o;
	IBCodeBlock* cb;
	assert(ibc);
	if (ibc->InputStr) {
		IBComp1InputStr(ibc, ibc->InputStr);
		ibc->InputStr = NULL;
	}
	if(ibc->CodeBlockStack.elemCount != 1)Err(OP_Error, "dirty codeblock stack");
	cb=(IBCodeBlock*)IBVectorTop(&ibc->CodeBlockStack);
	assert(!(IBStrGetLen(&cb->variables) + 
		IBStrGetLen(&cb->code) + 
		IBStrGetLen(&cb->footer)));
	o=IBComp1GetObj(ibc);
	if (ibc->StringMode)
		Err(OP_Error, "Reached end of file without closing string");
	if(ibc->Str[0]) IBComp1StrPayload(ibc);
	t=IBComp1GetTask(ibc);
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
			IBComp1PopObj(ibc, true, NULL);
			IBComp1FinishTask(ibc);
			break;
		}
		}
	}
	IBStrAppendCStr(&ibc->CHeaderFuncs, "\n#endif\n");
#ifdef DEBUGPRINTS
	IBComp1PushColor(ibc, IBFgMAGENTA);
	DbgFmt("-> Compilation complete <-\n");
	IBComp1PopColor(ibc);
	IBComp1PushColor(ibc, IBFgWHITE);
	IBComp1PushColor(ibc, IBFgCYAN);
	DbgFmt(".h File: \n");
	IBComp1PopColor(ibc);
	DbgFmt("%s%s\n", ibc->CHeaderStructs.start, ibc->CHeaderFuncs.start);
	IBComp1PushColor(ibc, IBFgGREEN);
	DbgFmt(".c File: \n");
	IBComp1PopColor(ibc);
	DbgFmt("%s\n", ibc->CFile.start);
#else
	printf("%s%s\n%s\n",
		ibc->CHeaderStructs.start, ibc->CHeaderFuncs.start, ibc->CFile.start);
#endif
	IBComp1PopColor(ibc);
	IBVectorFree(&ibc->CodeBlockStack, IBCodeBlockFree);
	IBVectorFreeSimple(&ibc->ColorStack);
	IBVectorFree(&ibc->ObjStack, ObjFree);
	IBVectorFreeSimple(&ibc->ModeStack);
	IBVectorFreeSimple(&ibc->StrReadPtrsStack);
	IBVectorFree(&ibc->TaskStack, TaskFree);
	NameInfoDBFree(&ibc->NameTypeCtx);
	IBStrFree(&ibc->CHeaderStructs);
	IBStrFree(&ibc->CHeaderFuncs);
	IBStrFree(&ibc->CFile);
}
void IBComp1PushColor(IBComp1* ibc, IBColor col){
	IBVectorCopyPushIBColor(&ibc->ColorStack, col);
	IBSetColor(col);
}
void IBComp1PopColor(IBComp1* ibc){
	IBVectorPop(&ibc->ColorStack, NULL);
	assert(ibc->ColorStack.elemCount);
	IBSetColor(*(IBColor*)IBVectorTop(&ibc->ColorStack));
}
int IBComp1GetTabCount(IBComp1* ibc){
	return ibc->CodeBlockStack.elemCount - 1;
}
IBCodeBlock* IBComp1CodeBlocksTop(IBComp1* ibc){
	assert(ibc->CodeBlockStack.elemCount);
	return (IBCodeBlock*)IBVectorTop(&ibc->CodeBlockStack);
}
void _IBComp1PushCodeBlock(IBComp1* ibc, IBCodeBlock** cbDP){
	IBCodeBlock* cb;
	DbgFmt(" Push code block\n");
	cb=(IBCodeBlock*)IBVectorPush(&ibc->CodeBlockStack);
	IBCodeBlockInit(cb);
	if(cbDP) (*cbDP) = cb;
}
void _IBComp1PopCodeBlock(IBComp1* ibc, bool copyToParent, IBCodeBlock** cbDP){
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
	if(cbDP) (*cbDP) = IBComp1CodeBlocksTop(ibc);
}
void _IBComp1PushTask(IBComp1* ibc, Op taskOP, Expects** exectsDP, IBTask** taskDP) {
	IBTask* t;
	assert(exectsDP);
	DbgFmt(" Push task %s(%d)\n", GetOpName(taskOP), (int)taskOP);
	t = (IBTask*)IBVectorPush(&ibc->TaskStack);
	if(taskDP) (*taskDP) = t;
	TaskInit(t, taskOP);
	(*exectsDP) = (Expects*)IBVectorPush(&t->expStack);
}
void _IBComp1PopTask(IBComp1* ibc) {
	IBTask* t;
	assert(ibc);
	t=IBComp1GetTask(ibc);
	assert(t);
	DbgFmt(" Pop task %s(%d)\n", GetOpName(t->type),(int)t->type);
	IBVectorPop(&ibc->TaskStack, TaskFree);
}
void _IBComp1PushObj(IBComp1* ibc, Obj** o) {
	Obj *obj;
	DbgFmt(" Push obj: ","");
	if (ibc->ObjStack.elemCount) {
#ifdef DEBUGPRINTS
		ObjPrint(IBComp1GetObj(ibc));
		DbgFmt(" -> ", "");
#endif
	}
	obj = (Obj*)IBVectorPush(&ibc->ObjStack);
	ObjInit(obj);
#ifdef DEBUGPRINTS
	ObjPrint(obj);
	DbgFmt("\n","");
#endif
	if (o) { (*o) = IBComp1GetObj(ibc); }
}
void _IBComp1PopObj(IBComp1* ibc, bool pushToWorking, Obj** objDP) {
	Obj* o;
	IBTask* t;
	t=IBComp1GetTask(ibc);
	assert(t);
	o = IBComp1GetObj(ibc);
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
		newHome=(Obj*)IBVectorPush(&t->working);
		assert(newHome);
		memcpy(newHome, o, sizeof(Obj));
		if(ibc->ObjStack.elemCount < 1){
			Err(OP_Error, "no obj in stack");
		}else if (ibc->ObjStack.elemCount == 1) {
			ObjInit(o);
		}else if (ibc->ObjStack.elemCount > 1) {
			IBVectorPop(&ibc->ObjStack, NULL);
			o=IBComp1GetObj(ibc);
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
			o = IBComp1GetObj(ibc);
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
void _IBComp1Push(IBComp1* ibc, Op mode, bool strAllowSpace){
	Op cm;
	assert(ibc);
	cm=IBComp1GetMode(ibc);
	ibc->StrAllowSpace = strAllowSpace;
	IBVectorCopyPushOp(&ibc->ModeStack, mode);
	DbgFmt(" Push mode: %s(%d) -> %s(%d)\n", GetOpName(cm), (int)cm, GetOpName(mode), (int)mode);
}
void _IBComp1Pop(IBComp1* ibc) {
	Op type=OP_Null;
	Op mode;
	Op mode2;
	IBTask *t;
	Expects* exp;
	assert(ibc);
	t = IBComp1GetTask(ibc);
	assert(t);
	mode=IBComp1GetMode(ibc);
	IBVectorPop(&ibc->ModeStack, NULL);
	mode2 = IBComp1GetMode(ibc);
	if(t)type=t->type;
	DbgFmt(" Pop mode: %s(%d) -> %s(%d)\n", GetOpName(mode), (int)mode, GetOpName(mode2), (int)mode2);
	assert(t->expStack.elemCount);
#ifdef DEBUGPRINTS
	exp=TaskGetExpTop(t);
	ExpectsPrint(exp);
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
void IBComp1PushExpects(IBComp1* ibc, Expects **expDP){
	IBTask* t;
	Expects* exp;
	t = IBComp1GetTask(ibc);
	assert(t);
	assert(expDP);
	if(expDP){
		exp = (Expects*)IBVectorPush(&t->expStack);
		assert(exp);
		(*expDP) = exp;
	}	
}
void IBComp1PopExpects(IBComp1* ibc) {
	IBTask* t = IBComp1GetTask(ibc);
	Expects* exp;
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
		pfxsIb = &((Expects*)IBVectorTop(&t->expStack))->pfxs;
#ifdef DEBUGPRINTS
		idx = 0;
		while (oi = (Op*)IBVectorIterNext(pfxsIb,&idx)) {
			DbgFmt("%s ", GetPfxName(*oi));
		}
		DbgFmt("}\n","");
#endif
	}
}
void IBComp1ReplaceExpects(IBComp1* ibc, Expects** expDP){
	IBTask* t = IBComp1GetTask(ibc);
	Expects* exp;
	assert(t);
	exp = TaskGetExpTop(t);
	assert(exp);
#ifdef DEBUGPRINTS
	PLINE;
	DbgFmt(" Replace expects:\n", "");
	ExpectsPrint(exp);
#endif
	ExpectsFree(exp);
	*expDP = exp;
}
bool IBComp1IsPfxExpected(IBComp1* ibc, Op pfx) {
	Op* oi;
	int idx;
	IBTask* t;
	Expects* ap;
	t = NULL;
	ap = NULL;
	t = IBComp1GetTask(ibc);
	assert(t);
	assert(t->expStack.elemCount);
	idx = 0;
	oi = NULL;
	ap = TaskGetExpTop(t);
	if(ap) while (oi = (Op*)IBVectorIterNext(&ap->pfxs,&idx)) {
		assert(oi);
		if (oi && *oi == pfx)
			return true;
	}
	return false;
}
bool IBComp1IsNameOpExpected(IBComp1* ibc, Op nameOp){
	Op* oi;
	int idx;
	IBTask* t;
	Expects* exp;
	t = IBComp1GetTask(ibc);
	assert(t);
	assert(t->expStack.elemCount);
	idx = 0;
	oi = NULL;
	exp = TaskGetExpTop(t);
	while (oi = (Op*)IBVectorIterNext(&exp->nameOps, &idx)) {
		assert(oi);
		if (oi && *oi == nameOp)
			return true;
	}
	return false;
}
void IBComp1Tick(IBComp1* ibc, FILE* f){
	char ch;
	if (ibc->InputStr) {
		IBComp1InputStr(ibc, ibc->InputStr);
		ibc->InputStr = NULL;
	}
	else {
		assert(f);
		if ((ch = (char)fgetc(f)) != EOF) {
			if (ch != 0xffffffff)
				IBComp1InputChar(ibc, ch);
		}
		else {
			IBComp1InputChar(ibc, '\n');
			ibc->Running = false;
		}
	}
}
/*NO NEWLINES AT END OF STR*/
void IBComp1InputChar(IBComp1* ibc, char ch){
	Op m;
	IBTask* t;
	Obj* o;
	bool nl;
	ibc->Ch = ch;
	nl = false;
	m=IBComp1GetMode(ibc);
	t = IBComp1GetTask(ibc);
	o = IBComp1GetObj(ibc);
	if(ibc->CommentMode==OP_NotSet&&
		ibc->Ch==COMMENT_CHAR/*&&
		ibc->LastCh!=COMMENT_CHAR*/)
	{
		/*PLINE;
		DbgFmt(" LINE COMMENT ON\n","");*/
		ibc->CommentMode = OP_Comment;
		IBComp1Push(ibc, OP_ModeComment, false);
	}else if(ibc->CommentMode==OP_Comment&&
			ibc->LastCh==ibc->Ch &&
				!ibc->StringMode
				&&ibc->Ch==COMMENT_CHAR&&
				m==OP_ModeComment)
	{
		/*PLINE;
		DbgFmt(" MULTI COMMENT ON!!!!!!\n","");*/
		IBComp1Pop(ibc);
		IBComp1Push(ibc, OP_ModeMultiLineComment, false);
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
			DbgFmt("Char():Line end\n","");
		}
		if (ibc->CommentMode == OP_Comment) {
			IBComp1Pop(ibc);
			ibc->CommentMode = OP_NotSet;
		}
		switch (m) {
		case OP_ModeStrPass: {
			IBComp1StrPayload(ibc);
			break;
		}
		}
		switch (o->type) {
		case OP_CallWantArgs: {
			IBComp1PopObj(ibc, true, &o);
			break;
		}
		case OP_VarWantValue:
		case OP_VarComplete: {
			IBComp1PopObj(ibc, true, &o);
			break;
		}
		}
		switch(t->type){
			case OP_IfFinished: {
				IBCodeBlock* cb;
				Expects* exp;
				IBComp1PopObj(ibc, true, &o);
				SetTaskType(t, OP_IfBlockWantCode);
				IBComp1ReplaceExpects(ibc, &exp);
				ExpectsInit(exp, 0, "", "", "Nc", OP_Done);
				IBComp1PushCodeBlock(ibc, &cb);
				break;
			}
			case OP_ThingWantRepr: {
				Expects* exp;
				SetTaskType(t, OP_ThingWantContent);
				IBComp1ReplaceExpects(ibc, &exp);
				ExpectsInit(exp, 0, "", "", "PPNN", OP_Op, OP_VarType, OP_Func, OP_Done);
				break;
			}
			case OP_CPrintfHaveFmtStr: {
				IBComp1FinishTask(ibc);
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
				Op mod;
				SetObjType(o, OP_FuncSigComplete);
				PopExpects();
				mod = ObjGetMod(IBComp1GetObj(ibc));
				IBComp1PopObj(ibc, true, NULL);
				if (mod != OP_Imaginary) {
					Expects *exp;
					IBCodeBlock *cb;
					IBComp1PushExpects(ibc, &exp);
					ExpectsInit(exp, 0,"","",
						"PPPNN",
						OP_Op, OP_String, OP_VarType, OP_If, OP_Done);
					SetTaskType(t, OP_FuncWantCode);
					IBComp1PushCodeBlock(ibc, &cb);
				}
				else {
					IBComp1FinishTask(ibc);
				}
				break;
			}
		}
		break;
	}
	}
	m = IBComp1GetMode(ibc);
	ibc->Column++;
	if (!nl && ibc->CommentMode == OP_NotSet) {
		/*if(ibc->Ch == ' ') printf("-> SPACE (0x%x)\n",  ibc->Ch);
		else printf("-> %c (0x%x) %d:%d\n",
			ibc->Ch, ibc->Ch, ibc->Line, ibc->Column);*/

		switch (m) {
		case OP_ModeComment:
		case OP_ModeMultiLineComment:
			break;
		case OP_ModePrefixPass:
			IBComp1Prefix(ibc);
			break;
		case OP_ModeStrPass:
			IBComp1Str(ibc);
			break;
		default: Err(OP_Error, "unknown mode");
			break;
		}
	}
	if (nl) {
		if (IBComp1IsPfxExpected(ibc, OP_LineEnd)) PopExpects();
		ibc->Column = 0;
		ibc->Line++;
	}
	ibc->LastCh=ibc->Ch;
	if(m==OP_ModeMultiLineComment&&ibc->CommentMode==OP_NotSet){
		IBComp1Pop(ibc);
	}
}
void IBComp1InputStr(IBComp1* ibc, char* str)
{
	int i;
	for (i = 0; str[i] != '\0'; i++)
		IBComp1InputChar(ibc, str[i]);
}
Val IBComp1StrToVal(IBComp1* ibc, char* str, Op expectedType) {
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
char* IBComp1GetCPrintfFmtForType(IBComp1* ibc, Op type) {
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
void IBComp1FinishTask(IBComp1* ibc)	{
	IBVector* wObjs;
	IBCodeBlock* cb;
	int tabCount;
	IBTask* t;
	t = IBComp1GetTask(ibc);
	assert(t);
	DbgFmt("FinishTask: %s(%d)\n", GetOpName(t->type), (int)t->type);
	if(!ibc->TaskStack.elemCount)Err(OP_ErrNoTask, "task stack EMPTY!");
	wObjs = &t->working;
	assert(wObjs);
	if(!wObjs->elemCount)Err(OP_Error, "workingObjs EMPTY!");
	cb=IBComp1CodeBlocksTop(ibc);
	tabCount=IBComp1GetTabCount(ibc);
	switch (t->type) {
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
		IBStrAppendCh(&cb->header, '\t', tabCount - 1);
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
		IBStrAppendFmt(&cb->header, ") {\n");
		IBComp1VecPrint(ibc, wObjs);
		IBStrAppendCh(&cb->footer, '\t', tabCount - 1);
		IBStrAppendFmt(&cb->footer, "}\n");
		IBComp1PopCodeBlock(ibc, true, &cb);
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
		bool imaginary;
		Obj* funcObj;
		Obj* thingObj;

		thingObj = NULL;
		argc = 0;
		imaginary = false;
		IBStrInit(&cFuncModsTypeName, 1);
		IBStrInit(&cFuncArgsThing, 1);
		IBStrInit(&cFuncArgs, 1);
		IBStrInit(&cFuncArgsEnd, 1);
		IBStrInit(&cFuncCode, 1);
		idx = 0;
		funcObj = NULL;
		for (i = 0; i < wObjs->elemCount; ++i) {
			o = (Obj*)IBVectorGet(wObjs, i);
			switch (ObjGetType(o)) {
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
			case OP_FuncSigComplete: {
				if (ObjGetMod(o) == OP_Imaginary) {
					imaginary = true;
				}
			}
			case OP_FuncHasName:
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
						wo = IBComp1FindStackObjUnderTop(ibc, OP_Thing);
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
		if (imaginary) {
			IBStrAppendCStr(&cFuncArgsEnd, ");\n\n");
		}
		else {
			IBStr cbOut;
			IBCodeBlock* cb;
			IBStrAppendCStr(&cFuncArgsEnd, "){\n");
			IBStrInit(&cbOut, 1);
			cb=IBComp1CodeBlocksTop(ibc);
			IBCodeBlockFinish(cb, &cbOut);
			IBStrAppend(&cFuncCode, &cbOut);
			IBStrFree(&cbOut);
			IBComp1PopCodeBlock(ibc, false, &cb);
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
		if (!imaginary) {
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
									IBComp1GetCPrintfFmtForType(ibc, type));
								break;
							}
							case OP_String:
								assert(vo->var.type==OP_String);
							case OP_Value:{
								IBStrAppendCStr(&cb->code,
									IBComp1GetCPrintfFmtForType(ibc, vo->var.type));
								break;
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
	IBComp1PopTask(ibc);
}
void IBComp1Prefix(IBComp1* ibc){
	Obj* obj;
	IBTask* t;
	Expects* expTop;
	t = IBComp1GetTask(ibc);
	assert(t);
	expTop = TaskGetExpTop(t);
	/*for assigning func call ret val to var*/
	if (ibc->Pfx == OP_Value && ibc->Ch == '@' && !ibc->Str[0]) {
		Expects* exp;
		IBComp1PushExpects(ibc, &exp);
		ExpectsInit(exp, 1, "", "", "P", OP_Op);
	}
	ibc->Pfx = fromPfxCh(ibc->Ch);
	if(ibc->Pfx == OP_SpaceChar
		|| ibc->Pfx == OP_TabChar) return;
	if (ibc->Pfx == OP_Unknown) Err(OP_ErrUnknownPfx, "catastrophic err");
	obj=IBComp1GetObj(ibc);
	if (ibc->Pfx != OP_Unknown
		&& (!t || expTop->pfxs.elemCount)
		&& !IBComp1IsPfxExpected(ibc, ibc->Pfx)
	)
	{
		Err(OP_ErrUnexpectedNextPfx, "");
	}
	DbgFmt("PFX:%s(%d)\n", GetPfxName(ibc->Pfx), (int)ibc->Pfx);
	switch (ibc->Pfx) {
	case OP_String: { /* " */
		ibc->StringMode = true;
		IBComp1Push(ibc, OP_ModeStrPass, false);
		break;
	}
	case OP_VarType:
		IBVectorCopyPushBool(&ibc->StrReadPtrsStack, true);
	case OP_Value:
	case OP_Op:
	case OP_Name:
		/*getchar();*/
		IBComp1Push(ibc, OP_ModeStrPass, false);
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
void IBComp1Str(IBComp1* ibc){
	char chBuf[2];
	chBuf[0] = ibc->Ch;
	chBuf[1] = '\0';
	if (ibc->StringMode) {
		switch (ibc->Ch) {
		case '"': {
			ibc->StringMode = false;
			IBComp1StrPayload(ibc);
			return;
		}
		}
	}
	else {
		switch (ibc->Pfx) {
		case OP_Value: {
			switch (ibc->Ch) {
			case '@': {
				IBComp1Pop(ibc);
				IBComp1Prefix(ibc);
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
					IBComp1StrPayload(ibc);
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
	StrConcat(ibc->Str, IBComp1STR_MAX, chBuf);
}
IBTask* IBComp1GetTask(IBComp1* ibc){
	IBTask* ret= (IBTask*)IBVectorTop(&ibc->TaskStack);
	if (!ret)Err(OP_Error, "no task in stack");
	return ret;
}
Op IBComp1GetMode(IBComp1* ibc){
	Op ret=OP_Null;
	Op* p=(Op*)IBVectorTop(&ibc->ModeStack);
	if (p)ret = *p;
	return ret;
}
Expects* TaskGetExpTop(IBTask* t){
	Expects* ret;
	assert(t);
	ret = (Expects*)IBVectorTop(&t->expStack);
	assert(ret);
	return ret;
}
IBVector* TaskGetExpPfxsTop(IBTask* t){
	Expects* ret;
	assert(t);
	ret = (Expects*)IBVectorTop(&t->expStack);
	assert(ret);
	return &ret->pfxs;
}
IBVector* TaskGetExpNameOPsTop(IBTask* t){
	Expects* ret;
	assert(t);
	ret = (Expects*)IBVectorTop(&t->expStack);
	assert(ret);
	return &ret->nameOps;
}
void IBComp1StrPayload(IBComp1* ibc){
	Val strVal;
	IBTask *t;
	Obj* o;
	t=IBComp1GetTask(ibc);
	o=IBComp1GetObj(ibc);
	strVal.i32=atoi(ibc->Str);
	ibc->NameOp = GetOpFromName(ibc->Str);
	DbgFmt("StrPayload: %s\n", ibc->Str);
	switch (ibc->Pfx)
	{
	case OP_String: { /* " */
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
			IBComp1PushObj(ibc, &o);
			ObjSetType(o, OP_String);
			o->var.type=OP_String;
			ObjSetStr(o, ibc->Str);
			IBComp1PopObj(ibc, true, NULL);
			break;
		}
		case OP_IfBlockWantCode:
		case OP_FuncWantCode: { /*printf*/
			Expects *ap;
			Obj* o;
			IBComp1PushTask(ibc, OP_CPrintfHaveFmtStr, &ap, NULL);
			ExpectsInit(ap, 0, "expected fmt args or line end",
				"",	"PPPP", OP_Value, OP_Name, OP_String, OP_LineEnd);
			IBComp1PushObj(ibc, &o);
			ObjSetStr(o, ibc->Str);
			ObjSetType(o, OP_CPrintfFmtStr);
			IBComp1PopObj(ibc, true, NULL);
			break;
		}
		}
		break;
	}
	case OP_Value: { /*=*/
		switch (o->type) {
		case OP_IfNeedLVal: {
			o->ifO.lvVal = strVal;
			o->ifO.lvTYPE = OP_Value;
			o->ifO.lvDataType = OP_i32;
			SetObjType(o, OP_IfNeedMidOP);
			SetTaskType(t, OP_IfNeedMidOP);
			break;
		}
		case OP_IfNeedRVal: {
			Expects* exp;
			o->ifO.rvVal = strVal;
			o->ifO.rvTYPE = OP_Value;
			o->ifO.rvDataType = OP_i32;
			SetObjType(o, OP_IfFinished);
			SetTaskType(t, OP_IfFinished);
			IBComp1ReplaceExpects(ibc, &exp);
			ExpectsInit(exp, 0, "", "",
				"P",
				OP_LineEnd);
			break;
		}
		case OP_VarWantValue: {
			Obj* o;
			o = IBComp1GetObj(ibc);
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
				IBComp1PushObj(ibc, &o);
				o->val = strVal;
				ObjSetType(o, OP_Value);
				o->var.type = OP_i32;/*for now*/
				IBComp1PopObj(ibc, true, NULL);
				break;
			}
			case OP_FuncNeedRetVal: {
				Obj* o;
				int idx;
				idx = 0;
				while (o = (Obj*)IBVectorIterNext(&t->working,&idx)) {
					if (ObjGetType(o) == OP_FuncSigComplete) {
						DbgFmt("Finishing func got ret value\n","");
						o->func.retVal = IBComp1StrToVal(ibc, ibc->Str, o->func.retType);
						PopExpects();
						SetTaskType(t, OP_Func);
						IBComp1FinishTask(ibc);
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
		case OP_ThingWantRepr: {
			SetTaskType(t, OP_ThingWantContent);			
			PopExpects();
			break;
		}
		case OP_ThingWantContent: {
			Obj* o;
			Expects* exp;
			IBComp1PushObj(ibc, &o);
			o->var.type = ibc->NameOp;
			o->var.mod = ibc->Pointer;
			o->var.privacy=ibc->Privacy;
			o->var.valSet = false;
			SetObjType(o, OP_VarNeedName);
			IBComp1PushExpects(ibc, &exp);
			ExpectsInit(exp, 0, "expected variable name", "", "P", OP_Name);
			break;
		}
		case OP_IfBlockWantCode:
		case OP_FuncWantCode: {
			Obj* o;
			Expects* exp;
			IBComp1PushObj(ibc, &o);
			o->var.type = ibc->NameOp;
			o->var.mod = ibc->Pointer;
			o->var.valSet = false;
			SetObjType(o, OP_VarNeedName);
			IBComp1PushExpects(ibc, &exp);
			ExpectsInit(exp, 0, "expected variable name", "", "P", OP_Name);
			break;
		}
		}
		switch (o->type) {
		case OP_FuncNeedsRetValType: {
			if (t->type != OP_FuncHasName)Err(OP_Error, "func signature needs name");
			IBComp1GetObj(ibc)->func.retType = ibc->NameOp;
			IBComp1GetObj(ibc)->func.retTypeMod = ibc->Pointer;
			SetObjType(o, OP_FuncSigComplete);
			break;
		}
		case OP_FuncHasName: {
			Obj* o;
			Expects* exp;
			IBComp1PushObj(ibc, &o);
			SetObjType(o, OP_FuncArgNameless);
			o->arg.type = ibc->NameOp;
			o->arg.mod = ibc->Pointer;
			IBComp1PushExpects(ibc, &exp);
			ExpectsInit(exp, 0, "expected func arg name", "", "P", OP_Name);
			break;
		}
		}
		break;
	case OP_Name: { /* $ */
		switch(t->type){
		case OP_IfNeedLVal: {
			Expects* exp;
			OverwriteStr(&o->ifO.lvName, ibc->Str);
			o->ifO.lvTYPE = OP_Name;
			SetObjType(o, OP_IfNeedMidOP);
			SetTaskType(t, OP_IfNeedMidOP);
			IBComp1ReplaceExpects(ibc, &exp);
			ExpectsInit(exp, 0, "", "",
				"PN",
				OP_Op, OP_Equals);
			break;
		}
		case OP_IfNeedRVal: {
			Expects* exp;
			OverwriteStr(&o->ifO.rvName, ibc->Str);
			o->ifO.lvTYPE = OP_Name;
			SetObjType(o, OP_IfFinished);
			SetTaskType(t, OP_IfFinished);
			IBComp1ReplaceExpects(ibc, &exp);
			ExpectsInit(exp, 0, "", "",
				"P",
				OP_LineEnd);
			break;
		}
		case OP_ThingWantName: {
			Obj* o;
			Expects* exp;
			/*assert(GetObjCount == 1);
			assert(GetObjType == OP_NotSet);*/
			IBComp1PushObj(ibc, &o);
			assert(ibc->Str[0]!='\0');
			ObjSetName(o, ibc->Str);
			ObjSetType(o, OP_Thing);
			SetTaskType(t, OP_ThingWantRepr);
			PopExpects();
			/*IBComp1PushExpects(ibc, &exp);
			ExpectsInit(exp, 0, "", "",
				"PPN",
				OP_Op, OP_LineEnd, OP_Repr);*/
			/*SetTaskType(t, OP_ThingWantContent);
			IBComp1PushExpects(ibc, &exp);
			ExpectsInit(exp, 0, "expected vartype (%)", "expected @pub, @priv, or @thing", 
				"PPNNNN", 
				OP_Op, OP_VarType, OP_Func, OP_Public, OP_Private, OP_Done);*/
			break;
		}
		case OP_UseNeedStr: {
			Op lib;
			lib = ibc->NameOp;
			switch (lib) {
			case OP_UseStrSysLib: {
				DbgFmt("Inputting system lib code to compiler\n","");
				assert(!ibc->InputStr);
				ibc->InputStr = SysLibCodeStr;
				break;
			}
			default: {

				break;
			}
			}
			IBComp1PopTask(ibc);
			break;
		}
		case OP_SpaceNeedName: {
			Obj* o;
			IBComp1PushObj(ibc, &o);
			assert(ibc->Str[0]!='\0');
			ObjSetName(o, ibc->Str);
			ObjSetType(o, OP_Space);
			IBComp1PopObj(ibc, true, &o);
			SetTaskType(t, OP_SpaceHasName);
			IBComp1FinishTask(ibc);
			break;
		}
		case OP_CPrintfHaveFmtStr: {
			Obj* o;
			IBComp1PushObj(ibc, &o);
			ObjSetName(o, ibc->Str);
			ObjSetType(o, OP_Name);
			IBComp1PopObj(ibc, true, NULL);
			break;
		}
		}
		switch (o->type) {
		case OP_VarNeedName: {
			Expects* exp;
			ObjSetName(IBComp1GetObj(ibc), ibc->Str);
			NameInfoDBAdd(&ibc->NameTypeCtx, ibc->Str, IBComp1GetObj(ibc)->var.type);
			SetObjType(o, OP_VarWantValue);
			PopExpects();
			IBComp1PushExpects(ibc, &exp);
			ExpectsInit(exp, 0,
				"expected value or line end after var name",
				"",
				"PP",
				OP_Value, OP_LineEnd);
			break;
		}
		case OP_CallNeedName: { /* =@call */
			Expects* exp;
			SetObjType(o, OP_CallWantArgs);
			PopExpects();
			IBComp1PushExpects(ibc, &exp);
			ExpectsInit(exp, 0,
				"expected var type or line end after func name", "",
				"PPP",
				OP_Name, OP_Value, OP_LineEnd);
		}
		case OP_Func: {
			Expects* exp;
			SetObjType(o, OP_FuncHasName);
			SetTaskType(t, OP_FuncHasName);
			IBComp1PushExpects(ibc, &exp);
			ExpectsInit(exp, 0, "", "",
				"PPPN",
				OP_VarType, OP_Op, OP_LineEnd, OP_Return);
			ObjSetName(IBComp1GetObj(ibc), ibc->Str);
			break;
		}
		case OP_FuncArgNameless:
			SetObjType(o, OP_FuncArgComplete);
			PopExpects();
			ObjSetName(IBComp1GetObj(ibc), ibc->Str);
			NameInfoDBAdd(&ibc->NameTypeCtx, ibc->Str, IBComp1GetObj(ibc)->arg.type);
			IBComp1PopObj(ibc, true, NULL);
			break;
		}
		break;
	}
	case OP_Op: { /* @ */
		bool expected;
		expected = IBComp1IsNameOpExpected(ibc, ibc->NameOp);
		if(!expected)Err(OP_ErrUnexpectedNameOP, "unexpected nameOP");
		switch (ibc->NameOp) {
		/*case OP_IfNeedMidOP: {

			break;
		}*/
		case OP_Repr: {
			switch (t->type) {
			case OP_ThingWantRepr: {
				Expects* exp;
				//SetTaskType(t, OP_ThingWantContent);
				IBComp1PushExpects(ibc, &exp);
				ExpectsInit(exp, 0, "expected vartype (%)", "", "P", OP_VarType);
				break;
			}
			}
			break;
		}
		case OP_Space: {
			Expects* ap;
			IBComp1PushTask(ibc, OP_SpaceNeedName, &ap, NULL);
			ExpectsInit(ap, 0, "expected space name", "", "P", OP_Name);
			break;
		}
		case OP_Thing: {
			Expects* ap;
			IBComp1PushTask(ibc, OP_ThingWantName, &ap, NULL);
			ExpectsInit(ap, 0, "", "", "PNN", OP_Op, OP_Done);
			IBComp1PushExpects(ibc, &ap);
			ExpectsInit(ap, 0, "", "", "PPN", OP_Op, OP_LineEnd, OP_Repr);
			IBComp1PushExpects(ibc, &ap);
			ExpectsInit(ap, 0, "expected thing name", "", "P", OP_Name);
			break;
		}
		case OP_dbgAssert: {
			Expects* ap;
			IBComp1PushTask(ibc, OP_dbgAssertWantArgs, &ap, NULL);
			ExpectsInit(ap, 0, "expected string", "", "P", OP_String);
			break;
		}
		case OP_Call: {
			switch (o->type) {
			case OP_VarWantValue: {
				Obj* o;
				Expects* exp;
				IBComp1PushObj(ibc, &o);
				ObjSetType(o, OP_CallNeedName);
				IBComp1PushExpects(ibc, &exp);
				ExpectsInit(exp, 0, "expected function name", "", "P", OP_Name);
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
			Expects* exp;
			ObjSetMod(IBComp1GetObj(ibc), ibc->NameOp);
			IBComp1PushExpects(ibc, &exp);
			ExpectsInit(exp, 1, "", "", "PN", OP_Op, OP_Func);
			break;
		}
		case OP_Done:
			if (ibc->TaskStack.elemCount < 1) Err(OP_ErrNoTask, "");
			switch (t->type) {
			case OP_IfBlockWantCode: {
				IBComp1FinishTask(ibc);
				break;
			}
			case OP_ThingWantContent: {
				Obj* o;
				o = IBComp1GetObj(ibc);
				assert(o->type == OP_Thing);
				IBComp1PopObj(ibc, true, &o);
				assert(o->type == OP_NotSet);
				IBComp1FinishTask(ibc);
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
							Expects* exp;
							IBComp1PushExpects(ibc, &exp);
							if (o->func.retType == OP_c8 && o->func.retTypeMod == OP_Pointer) {
								ExpectsInit(exp, 0, "", "", "PP", OP_Name, OP_String);
							}
							else {
								ExpectsInit(exp, 0, "", "", "P", OP_Value);
							}
							SetTaskType(t, OP_FuncNeedRetVal);
						}
						else {
							SetTaskType(t, OP_Func);
							IBComp1FinishTask(ibc);
						}
					}
				}
				break;
			}
			}
			break;
		case OP_Return: {
			Op objT = o->type;
			switch (objT) {
			/*case OP_FuncArgComplete: {
				DbgFmt("what\n","");
				IBComp1PopObj(ibc, true, NULL);
				if (o->type != OP_FuncHasName) {
					Err(OP_Error, "expected FuncHasName");
					break;
				}
			}*/
			case OP_FuncHasName: {
				Expects* exp;
				SetObjType(o, OP_FuncNeedsRetValType);
				IBComp1PushExpects(ibc, &exp);
				ExpectsInit(exp, 0, "", "", "P", OP_VarType);
				break;
			}
			default:
				Err(OP_ErrUnexpectedOp, "");
				break;
			}
			break;
		}
		case OP_Func: { //func1
			Expects* ap;
			Obj* o;
			IBTask* t;
			t = IBComp1GetTask(ibc);
			assert(t);
			IBComp1PushObj(ibc, &o);
			o->func.thingTask = t->type == OP_ThingWantContent ? t : NULL;
			IBComp1PushTask(ibc, OP_FuncNeedName, &ap, NULL);
			ExpectsInit(ap, 0, "expected function name", "", "P", OP_Name);
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
			Expects* ap;
			IBComp1PushTask(ibc, OP_UseNeedStr, &ap, NULL);
			ExpectsInit(ap, 0, "expected @use name", "", "P", OP_Name);
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
				Expects* exp;
				o->ifO.midOp = ibc->NameOp;
				ObjSetType(o, OP_IfNeedRVal);
				SetTaskType(t, OP_IfNeedRVal);
				IBComp1ReplaceExpects(ibc, &exp);
				ExpectsInit(exp, 0, "", "", "PP", OP_Name, OP_Value);
				break;
			}
			}
			break;
		}
		case OP_If: {
			switch (t->type) {
			case OP_IfBlockWantCode:
			case OP_FuncWantCode: {
				IBTask* nt;
				Expects* nexp;
				Obj* o;
				IBComp1PushObj(ibc, &o);
				ObjSetType(o, OP_IfNeedLVal);
				IBComp1PushTask(ibc, OP_IfNeedLVal, &nexp, &nt);
				ExpectsInit(nexp, 0, "expected lval", "", "PP", OP_Value, OP_Name/*, OP_String*/);
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
	printf("Str payload complete\n");
#endif
	IBComp1Pop(ibc);
	if(ibc->StrReadPtrsStack.elemCount > 1){
		if (*(bool*)IBVectorTop(&ibc->StrReadPtrsStack))
			ibc->Pointer = OP_NotSet;
		IBVectorPop(&ibc->StrReadPtrsStack, NULL);
	}
}
void IBComp1ExplainErr(IBComp1* ibc, Op code) {
	switch (code) {
	case OP_FuncNeedRetVal: {
		printf("You forgot to return a value from the function");
		break;
	}
	case OP_ErrUnexpectedNameOP: {
		Expects* exp;
		IBTask* t;
		t=IBComp1GetTask(ibc);
		assert(t);
		exp = TaskGetExpTop(t);
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
		Expects *ap;
		IBTask *t;
		int idx;
		t =IBComp1GetTask(ibc);
		ap=TaskGetExpTop(t);
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
	ObjPrint(IBComp1GetObj(ibc));
	DbgFmt("\n","");
#endif
}
void IBSetColor(IBColor col) {
#ifdef _WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), col);
#endif
}
int main(int argc, char** argv) {
	IBDatabase db;
	FILE* f;
	if (argc < 2) {
		printf("Please specify a file\n");
		getchar();
		return -1;
	}
	g_DB = &db;
	IBDatabaseInit(g_DB);
	f = fopen(argv[1], "r");
	if (f){
		IBComp1 comp;
		IBComp1Init(&comp);
		while (comp.Running)
			IBComp1Tick(&comp, f);
		DbgFmt("Exiting\n","");
		//assert(comp.InputStr == NULL);
		IBComp1Free(&comp);
		getchar();
		fclose(f);
		return 0;
	}
	else{
		printf("Error opening file\n");
	}
	return 1;
}
#endif
