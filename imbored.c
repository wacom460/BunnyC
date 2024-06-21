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
#define BoolStr(b) (b ? "true" : "false")
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
	IBFgWHITE = 
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	IBFgRED = 
		FOREGROUND_RED,
	IBFgGREEN =
		FOREGROUND_GREEN,
	IBFgBLUE =
		FOREGROUND_BLUE,
	IBFgYELLOW =
		FOREGROUND_RED | FOREGROUND_GREEN,
	IBFgCYAN =
		FOREGROUND_BLUE | FOREGROUND_GREEN,
	IBFgMAGENTA =
		FOREGROUND_RED | FOREGROUND_BLUE,
	IBFgBROWN =
		FOREGROUND_RED | FOREGROUND_GREEN,
	IBFgIntensity =
		FOREGROUND_INTENSITY,

	//bg
	IBBgWHITE =
		BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
	IBBgRED =
		BACKGROUND_RED,
	IBBgGREEN =
		BACKGROUND_GREEN,
	IBBgBLUE =
		BACKGROUND_BLUE,
	IBBgYELLOW =
		BACKGROUND_RED | BACKGROUND_GREEN,
	IBBgCYAN =
		BACKGROUND_BLUE | BACKGROUND_GREEN,
	IBBgMAGENTA =
		BACKGROUND_RED | BACKGROUND_BLUE,
	IBBgBROWN =
		BACKGROUND_RED | BACKGROUND_GREEN,
	IBBgIntensity =
		BACKGROUND_INTENSITY
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

#define FGWHITE \
	IBSetColor(IBFgWHITE)
#define FGRED \
	IBSetColor(IBFgRED)
#define FGGREEN \
	IBSetColor(IBFgGREEN)
#define FGBLUE \
	IBSetColor(IBFgBLUE)
#define FGYELLOW \
	IBSetColor(IBFgYELLOW)
#define FGCYAN \
	IBSetColor(IBFgCYAN)
#define FGMAGENTA \
	IBSetColor(IBFgMAGENTA)
#define FGBROWN \
	IBSetColor(IBFgBROWN)

#define DEBUGPRINTS

#ifdef DEBUGPRINTS
#define PLINE printf("[%d]", __LINE__)
#else
#define PLINE
#endif
#define OP_NAME_LEN 32
#define COMMENT_CHAR ('~')
#define CompilerSTR_MAX 64
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
	OP_GreaterThanOrEquals, OP_ScopeOpen, OP_ScopeClose,
	OP_ParenthesisOpen, OP_ParenthesisClose, OP_BracketOpen,
	OP_BracketClose, OP_SingleQuote, OP_DoubleQuote,
	OP_CPrintfHaveFmtStr,OP_TaskStackEmpty,OP_RootTask,
	OP_Thing,OP_ThingWantName,OP_ThingWantContent,OP_ThingWantRepr,
	OP_SpaceNeedName,OP_SpaceHasName, OP_Obj, OP_Bool,
	OP_Task, OP_IBColor,OP_Repr,

	OP_SpaceChar, OP_Comma, OP_CommaSpace, OP_Name, OP_String,
	OP_CPrintfFmtStr, OP_Char, OP_If, OP_Else, OP_For, OP_While,
	OP_Block, OP_c8, OP_u8, OP_u16, OP_u32, OP_u64, OP_i8, OP_i16,
	OP_i32, OP_i64, OP_f32, OP_d64, OP_Pointer, OP_DoublePointer,
	OP_TripplePointer, OP_CompilerFlags, OP_dbgBreak, OP_dbgAssert,
	OP_dbgAssertWantArgs,OP_TaskType, OP_TaskStack, OP_NotEmpty,
	OP_TabChar,OP_UseNeedStr,OP_UseStrSysLib,OP_NameInfoDB,
	OP_NameInfo,OP_Expects,

	OP_NotFound, OP_Error, OP_ErrNOT_GOOD, OP_ErrUnexpectedNextPfx,
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
char *IBStrAppend(IBStr* str, char *with);
typedef union IBVecData {
	void* ptr;
	struct Obj* obj;
	struct Task* task;
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
typedef struct IB_Variable {
	Op type;
	IBStr name;
	Val val;
} IB_Variable;
IB_Variable* IB_VariableNew(Op type, IBStr name, Val val);
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
	struct Task* thingTask;
} FuncObj;
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
#define ExpectsInit(exp, life, pfxErr, nameOpErr, fmt, ...)\
	_ExpectsInit(__LINE__, exp, life, pfxErr, nameOpErr, fmt, __VA_ARGS__);
void ExpectsPrint(Expects* exp);
void ExpectsFree(Expects* exp);
typedef struct Task {
	Op type;
	IBVector expStack; /*Expects*/
	IBVector working;/*Obj*/
	IBStr code1;
	IBStr code2;
} Task;
void TaskInit(Task* t, Op type);
void TaskFree(Task* t);
typedef struct Compiler {
	int m_Line;
	int m_Column;
	Op m_Pfx;
	char m_Str[CompilerSTR_MAX];
	IBStr m_CHeaderStructs;/* .h */
	IBStr m_CHeaderFuncs;
	IBStr m_CFile;/* .c */

	IBVector m_ObjStack; /*Obj*/
	IBVector m_ModeStack; /*Op*/
	IBVector m_TaskStack; /*Task*/
	IBVector m_StrReadPtrsStack; /*bool*/
	IBVector m_ColorStack; /*IBColor*/

	char* inputStr;
	Op m_Pointer;
	Op m_Privacy;
	Op m_NameOp;
	char m_Ch;
	char m_LastCh;
	bool m_Running;
	bool m_StringMode;
	bool m_StrAllowSpace;
	Op m_CommentMode;
	NameInfoDB m_NameTypeCtx;
} Compiler;
void _Err(Compiler *compiler, Op code, char *msg);
#define Err(code, msg){\
	PLINE;\
	_Err(compiler, code, msg);\
}
Obj* CompilerGetObj(Compiler* compiler);
void CompilerPrintVecData(Compiler* compiler, IBVecData* data, Op type);
void CompilerVecPrint(Compiler* compiler, IBVector* vec);
Obj* CompilerFindStackObjUnderIndex(Compiler* compiler, int index, Op type);
Obj* CompilerFindStackObjUnderTop(Compiler* compiler, Op type);
void CompilerInit(Compiler* compiler);
Task* CompilerFindTaskUnderIndex(Compiler* compiler, int index, Op type);
void CompilerFree(Compiler* compiler);
void CompilerPushColor(Compiler* compiler, IBColor col);
void CompilerPopColor(Compiler* compiler);
void _CompilerPushTask(Compiler* compiler, Op taskOP, Expects** exectsDP, Task** taskDP);
#define CompilerPushTask(compiler, taskOP, exectsDP, taskDP){\
	PLINE;\
	_CompilerPushTask(compiler, taskOP, exectsDP, taskDP);\
}
void _CompilerPopTask(Compiler* compiler);
#define CompilerPopTask(compiler){\
	PLINE;\
	_CompilerPopTask(compiler);\
}
void _CompilerPushObj(Compiler* compiler, Obj** o);
#define CompilerPushObj(compiler, objDP){\
	PLINE;\
	_CompilerPushObj(compiler, objDP);\
}
#define GetObjCount (compiler->m_ObjStack.elemCount)
void _CompilerPopObj(Compiler* compiler, bool pushToWorking, Obj **objDP);
#define CompilerPopObj(compiler, p2w, objDP){\
	PLINE;\
	_CompilerPopObj(compiler, p2w, objDP);\
}
void _CompilerPush(Compiler* compiler, Op mode, bool strAllowSpace);
#define CompilerPush(compiler, mode, strAllowSpace){\
	PLINE;\
	_CompilerPush(compiler, mode, strAllowSpace);\
}
void _CompilerPop(Compiler* compiler);
#define CompilerPop(compiler){\
	PLINE;\
	_CompilerPop(compiler);\
}
/*life:0 = infinite, -1 life each pfx*/
void CompilerPushExpects(Compiler* compiler, Expects** expDP);
void CompilerPopExpects(Compiler* compiler);
bool CompilerIsPfxExpected(Compiler* compiler, Op pfx);
bool CompilerIsNameOpExpected(Compiler* compiler, Op nameOp);
/*NO NEWLINES AT END OF STR*/
void CompilerTick(Compiler* compiler, FILE *f);
void CompilerInputChar(Compiler* compiler, char ch);
void CompilerInputStr(Compiler* compiler, char* str);
void CompilerPopAndDoTask(Compiler* compiler);
Val CompilerStrToVal(Compiler* compiler, char* str, Op expectedType);
char* CompilerGetCPrintfFmtForType(Compiler* compiler, Op type);
void CompilerPrefix(Compiler* compiler);
void CompilerStr(Compiler* compiler);
void CompilerStrPayload(Compiler* compiler);
void CompilerExplainErr(Compiler* compiler, Op code);
#define SetObjType(type) ObjSetType(CompilerGetObj(compiler), type)
#define GetObjType (ObjGetType(CompilerGetObj(compiler)))
#define PushPfxs(pfxs, msg, life){\
	PLINE;\
	pushAllowedNextPfxs(pfxs, msg, life);\
}
#define PopExpects(){\
	PLINE;\
	CompilerPopExpects(compiler);\
}
Task* CompilerGetTask(Compiler *compiler);
Op CompilerGetMode(Compiler *compiler);

Expects* TaskGetExpTop(Task* t);
IBVector* TaskGetExpPfxsTop(Task *t);
IBVector* TaskGetExpNameOPsTop(Task* t);

//#define GetTask CompilerGetTask(compiler)
//#define GetTaskPfxs TaskGetExpPfxsTop(GetTask)
//#define GetTaskNameOPs TaskGetExpNameOPsTop(GetTask)
//#define GetTask _GetTask(compiler)
//#define GetExpectsStack (GetTask ? &GetTask->expStack : NULL)
//#define GetExpectsTop ((Expects*)IBVectorTop(GetExpectsStack))
//#define GetMode CompilerGetMode(compiler)
//#define GetTaskType (GetTask ? GetTask->type : OP_TaskStackEmpty)
#define SetTaskType(task, tt){\
	assert(task);\
	PLINE;\
	printf("SetTaskType: %s -> %s\n", GetOpName(task->type), GetOpName(tt));\
	task->type = tt;\
}
//#define SetTaskType(tt) {\
//	GetTask->type = tt;\
//}
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
char* CompilerStringModeIgnoreChars = "";
OpNamePair opNames[] = {
	{"null", OP_Null},{"no", OP_False},{"yes", OP_True},{"set", OP_Set},
	{"call", OP_Call},{"add", OP_SetAdd},{"exec", OP_Func},{"~", OP_Comment},
	{"%", OP_VarType},{"Value", OP_Value},{"@", OP_Done},{"ret", OP_Return},
	{"ext", OP_Imaginary},{"if", OP_If},{"else", OP_Else},{"use", OP_Use},
	{"build", OP_Build},{"space", OP_Space},{"+", OP_Add},{"-", OP_Subtract},
	{"*", OP_Multiply},{"/", OP_Divide},{"is", OP_Equals},{"neq", OP_NotEquals},
	{"lt", OP_LessThan},{"gt", OP_GreaterThan},{"lteq", OP_LessThanOrEquals},
	{"gteq", OP_GreaterThanOrEquals},{"{", OP_ScopeOpen},{"}", OP_ScopeClose},
	{"(", OP_ParenthesisOpen},{")", OP_ParenthesisClose},{"[", OP_BracketOpen},
	{"]", OP_BracketClose},{",", OP_Comma},{"$", OP_Name},{"for", OP_For},
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
	{"ErrNOT_GOOD", OP_ErrNOT_GOOD},{"FuncNeedName",OP_FuncNeedName},{"String", OP_String},
	{"VarComplete", OP_VarComplete},{"VarWantValue",OP_VarWantValue},{"LineEnd", OP_LineEnd},
	{"CPrintfHaveFmtStr",OP_CPrintfHaveFmtStr},{"FuncWantCode",OP_FuncWantCode},
	{"dbgBreak", OP_dbgBreak},{"dbgAssert", OP_dbgAssert}, { "CallNeedName",OP_CallNeedName },
	{"CallWantArgs", OP_CallWantArgs},{"CallComplete", OP_CallComplete},
	{"TaskStackEmpty", OP_TaskStackEmpty}, {"CPrintfFmtStr", OP_CPrintfFmtStr},
	{"SpaceChar",OP_SpaceChar},{"use",OP_Use},{"UseNeedStr",OP_UseNeedStr},
	{"sys", OP_UseStrSysLib},{"junt", OP_Thing},{"SpaceNeedName",OP_SpaceNeedName},
	{"RootTask", OP_RootTask},{"ErrUnknownPfx",OP_ErrUnknownPfx},
	{"ErrUnexpectedNameOP",OP_ErrUnexpectedNameOP},{"ThingWantName",OP_ThingWantName},
	{"ThingWantContent",OP_ThingWantContent},{"SpaceHasName",OP_SpaceHasName},
	{"ErrDirtyTaskStack",OP_ErrDirtyTaskStack},{"repr", OP_Repr},{"NotEmpty",OP_NotEmpty},
	{"ThingWantRepr",OP_ThingWantRepr},
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
};
OpNamePair dbgAssertsNP[] = {
	{"taskType", OP_TaskType},{ "taskStack", OP_TaskStack },{"notEmpty", OP_NotEmpty}
};
char* SysLibCodeStr =
"@space $sys\n"
"@pub\n"
"@ext @exec $malloc %i32 $size @ret %&?\n"
"@ext @exec $realloc %&? $ptr %i32 $newSize @ret %&?\n"
"@ext @exec $free %&? $ptr\n"
"@ext @exec $strdup %&c8 $str @ret %&c8\n"
"@ext @exec $strcat %&c8 $str1 %&c8 $str2 @ret %&c8\n"
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
char *IBStrAppend(IBStr* str, char *with) {
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
	o->arg.type = OP_Null;
	o->arg.mod = OP_NotSet;
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
void TaskInit(Task* t, Op type) {
	IBVectorInit(&t->working, sizeof(Obj), OP_Obj);
	IBVectorInit(&t->expStack, sizeof(Expects), OP_Expects);
	t->type = type;
	IBStrInit(&t->code1, 1);
	IBStrInit(&t->code2, 1);
}
void TaskFree(Task* t) {
	assert(t);
	IBVectorFree(&t->expStack, ExpectsFree);
	IBVectorFree(&t->working, ObjFree);
}
void _Err(Compiler *compiler, Op code, char *msg){
	FGRED;
	printf("Error at %u:%u \"%s\"(%d). %s\n",
		compiler->m_Line, compiler->m_Column, GetOpName(code), (int)code, msg);
	CompilerExplainErr(compiler, code);
#ifdef DEBUGPRINTS
	/*CompilerVecPrint(compiler, &compiler->m_ObjStack);
	CompilerVecPrint(compiler, &compiler->m_TaskStack);*/
#endif
	printf("\nPress enter to break.");
	getchar();
	__debugbreak();
	exit(-1);
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
void IBDatabaseInit(IBDatabase* db){

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
Obj* CompilerGetObj(Compiler* compiler) {
	return (Obj*)IBVectorTop(&compiler->m_ObjStack);
}
void CompilerPrintVecData(Compiler* compiler, IBVecData* data, Op type){
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
		CompilerPrintVecData(compiler, obj->func.thingTask, OP_Task);
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
			"\t\t]\n",
			obj->var.val.i32,
			BoolStr(obj->var.valSet),
			GetOpName(obj->var.type), (int)obj->var.type,
			GetOpName(obj->var.mod), (int)obj->var.mod,
			GetOpName(obj->var.privacy), (int)obj->var.privacy,
			GetOpName(obj->arg.type), (int)obj->arg.type,
			GetOpName(obj->arg.mod), (int)obj->arg.mod
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
		CompilerVecPrint(compiler, &exp->pfxs);
		DbgFmt("\t\tnameOps:\n", "");
		CompilerVecPrint(compiler, &exp->nameOps);
		DbgFmt("]\n", "");
		break;
	}
	case OP_Task: {
		Task* task = (Task*)data;
		DbgFmt("TASK -> [\n"
			"\tType: %s(%d)\n"
			"\tCode1:\n%s(%d)\n"
			"\tCode2:\n%s(%d)\n"
			"\tWorking objs:\n",
			GetOpName(task->type), (int)task->type);
		CompilerVecPrint(compiler, &task->working);
		DbgFmt("\tExpects:\n", "");
		CompilerVecPrint(compiler, &task->expStack);
		DbgFmt("]\n", "");
		break;
	}
	default:
		break;
	}
}
void CompilerVecPrint(Compiler* compiler, IBVector* vec) {
	int idx;
	IBVecData* data;

	assert(vec);
	switch (vec->type) {
	case OP_Op: {
		CompilerPushColor(compiler, IBFgCYAN);
		DbgFmt("OPs","")
		break;
	}
	case OP_Obj: {
		CompilerPushColor(compiler, IBFgYELLOW);
		DbgFmt("OBJs", "");
		break;
	}
	case OP_Expects: {
		CompilerPushColor(compiler, IBFgRED);
		DbgFmt("EXPECTs", "");
		break;
	}
	case OP_Task: {
		CompilerPushColor(compiler, IBFgBLUE);
		DbgFmt("TASKs", "");
		break;
	}
	default: {
		CompilerPushColor(compiler, IBFgMAGENTA);
		DbgFmt("UNKNOWN!", "");
		break;
	}
	}
	DbgFmt(" vec -> [\n", "");
	data = NULL;
	idx = 0;
	while (data = IBVectorIterNext(vec, &idx)) {
		DbgFmt("\t(%d)", idx);
		CompilerPrintVecData(compiler, data, vec->type);		
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
	CompilerPopColor(compiler);
}
Obj* CompilerFindStackObjUnderIndex(Compiler* compiler, int index, Op type) {
	int i;
	if(compiler->m_ObjStack.elemCount < 2)Err(OP_ErrNOT_GOOD, "Not enough objects on stack");
	if(index >= compiler->m_ObjStack.elemCount)Err(OP_ErrNOT_GOOD, "Index out of bounds");
	for (i = index - 1; i >= 0;) {
		Obj* o;
		o = (Obj*)IBVectorGet(&compiler->m_ObjStack, i--);
		if (o->type == type) return o;
	}
	return NULL;
}
Obj* CompilerFindStackObjUnderTop(Compiler* compiler, Op type){
	Obj* o;
	int i;
	if(compiler->m_ObjStack.elemCount < 2) return NULL;
	for (i = compiler->m_ObjStack.elemCount - 1; i >= 0;) {
		o = (Obj*)IBVectorGet(&compiler->m_ObjStack, i--);
		if (o->type == type) return o;
	}
	return NULL;
}
void CompilerInit(Compiler* compiler){
	Obj* o;
	Expects* exp;
	compiler->m_Running = true;
	compiler->m_Line = 1;
	compiler->m_Column = 1;
	compiler->m_Pfx = OP_Null;
	compiler->m_Str[0] = '\0';
	IBStrInit(&compiler->m_CHeaderStructs, 1);
	IBStrInit(&compiler->m_CHeaderFuncs, 1);
	IBStrAppend(&compiler->m_CHeaderStructs, "#ifndef HEADER_H_\n#define HEADER_H_\n\n");
	IBStrInit(&compiler->m_CFile, 1);
	IBStrAppend(&compiler->m_CFile, "#include \"header.h\"\n\n");
	compiler->m_Pointer = OP_NotSet;
	compiler->m_Privacy = OP_Public;
	compiler->m_NameOp = OP_Null;
	compiler->m_Ch = '\0';
	compiler->m_LastCh = '\0';
	compiler->m_StringMode = false;
	compiler->m_StrAllowSpace = false;
	compiler->m_CommentMode = OP_NotSet;
	compiler->inputStr = NULL;
	NameInfoDBInit(&compiler->m_NameTypeCtx);
	IBVectorInit(&compiler->m_ObjStack, sizeof(Obj), OP_Obj);
	IBVectorInit(&compiler->m_ModeStack, sizeof(Op), OP_Op);
	IBVectorInit(&compiler->m_StrReadPtrsStack, sizeof(bool), OP_Bool);
	IBVectorInit(&compiler->m_TaskStack, sizeof(Task), OP_Task);
	IBVectorInit(&compiler->m_ColorStack, sizeof(IBColor), OP_IBColor);
	CompilerPushColor(compiler, IBFgWHITE);
	IBVectorCopyPushBool(&compiler->m_StrReadPtrsStack, false);
	CompilerPush(compiler, OP_ModePrefixPass, false);
	CompilerPushObj(compiler, &o);
	CompilerPushTask(compiler, OP_RootTask, &exp, NULL);
	ExpectsInit(exp, 0, "", "", "PNNNNNNN", 
		OP_Op, OP_Use, OP_Imaginary, OP_Func, 
		OP_Thing, OP_Space, OP_Public, OP_Private);
}
Task* CompilerFindTaskUnderIndex(Compiler* compiler, int index, Op type){
	int i;
	if(compiler->m_TaskStack.elemCount < 2)Err(OP_ErrNOT_GOOD, "Not enough tasks on stack");
	if(index == -1) index = compiler->m_TaskStack.elemCount - 1;
	if(index >= compiler->m_TaskStack.elemCount)Err(OP_ErrNOT_GOOD, "Index out of bounds");
	for (i = index - 1; i >= 0;) {
		Task* t;
		t = (Task*)IBVectorGet(&compiler->m_TaskStack, i--);
		if (t->type == type) return t;
	}
	return NULL;
}
void CompilerFree(Compiler* compiler) {
	Task* t;
	assert(compiler);
	if (compiler->m_StringMode)
		Err(OP_ErrNOT_GOOD, "Reached end of file without closing string");
	if(compiler->m_Str[0]) CompilerStrPayload(compiler);
	t=CompilerGetTask(compiler);
	if(t->type != OP_RootTask)Err(OP_ErrDirtyTaskStack,
		"Reached end of file not at root task");
	if (compiler->m_TaskStack.elemCount) {
		switch (((Task*)IBVectorTop(&compiler->m_TaskStack))->type) {
		case OP_FuncNeedRetVal:
			Err(OP_ErrNOT_GOOD, "Reached end of file without closing function");
			break;
		case OP_FuncSigComplete:
		case OP_FuncHasName: {
			SetObjType(OP_FuncSigComplete);
			CompilerPopObj(compiler, true, NULL);
			CompilerPopAndDoTask(compiler);
			break;
		}
		}
	}
	IBStrAppend(&compiler->m_CHeaderFuncs, "\n#endif\n");
#ifdef DEBUGPRINTS
	DbgFmt("-> Compilation complete <-\nC Header:\n%s%s\n\nC File:\n%s\n\n",
		compiler->m_CHeaderStructs.start, compiler->m_CHeaderFuncs.start, compiler->m_CFile.start);
#else
	printf("%s%s\n%s\n",
		compiler->m_CHeaderStructs.start, compiler->m_CHeaderFuncs.start, compiler->m_CFile.start);
#endif
	IBVectorFreeSimple(&compiler->m_ColorStack);
	IBVectorFree(&compiler->m_ObjStack, ObjFree);
	IBVectorFreeSimple(&compiler->m_ModeStack);
	IBVectorFreeSimple(&compiler->m_StrReadPtrsStack);
	IBVectorFree(&compiler->m_TaskStack, TaskFree);
	NameInfoDBFree(&compiler->m_NameTypeCtx);
	IBStrFree(&compiler->m_CHeaderStructs);
	IBStrFree(&compiler->m_CHeaderFuncs);
	IBStrFree(&compiler->m_CFile);
}
void CompilerPushColor(Compiler* compiler, IBColor col){
	IBVectorCopyPushIBColor(&compiler->m_ColorStack, col);
	IBSetColor(col);
}
void CompilerPopColor(Compiler* compiler){
	IBVectorPop(&compiler->m_ColorStack, NULL);
	assert(compiler->m_ColorStack.elemCount);
	IBSetColor(*(IBColor*)IBVectorTop(&compiler->m_ColorStack));
}
void _CompilerPushTask(Compiler* compiler, Op taskOP, Expects** exectsDP, Task** taskDP) {
	Task* t;
	assert(exectsDP);
	DbgFmt(" Push task %s(%d)\n", GetOpName(taskOP), (int)taskOP);
	t = (Task*)IBVectorPush(&compiler->m_TaskStack);
	if(taskDP) (*taskDP) = t;
	TaskInit(t, taskOP);
	(*exectsDP) = (Expects*)IBVectorPush(&t->expStack);
}
void _CompilerPopTask(Compiler* compiler) {
	Task* t;
	assert(compiler);
	t=CompilerGetTask(compiler);
	assert(t);
	DbgFmt(" Pop task %s(%d)\n", GetOpName(t->type),(int)t->type);
	IBVectorPop(&compiler->m_TaskStack, TaskFree);
}
void _CompilerPushObj(Compiler* compiler, Obj** o) {
	Obj *obj;
	DbgFmt(" Push obj: ","");
	if (compiler->m_ObjStack.elemCount) {
#ifdef DEBUGPRINTS
		ObjPrint(CompilerGetObj(compiler));
		DbgFmt(" -> ", "");
#endif
	}
	obj = (Obj*)IBVectorPush(&compiler->m_ObjStack);
	ObjInit(obj);
#ifdef DEBUGPRINTS
	ObjPrint(obj);
	DbgFmt("\n","");
#endif
	if (o) { (*o) = CompilerGetObj(compiler); }
}
void _CompilerPopObj(Compiler* compiler, bool pushToWorking, Obj** objDP) {
	Obj* o;
	Task* t;
	t=CompilerGetTask(compiler);
	assert(t);
	o = CompilerGetObj(compiler);
	assert(o);
	if (pushToWorking){
		Obj* newHome;
		if (o->type == OP_NotSet)Err(OP_ErrNOT_GOOD, "");
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
		if(compiler->m_ObjStack.elemCount < 1){
			Err(OP_ErrNOT_GOOD, "no obj in stack");
		}else if (compiler->m_ObjStack.elemCount == 1) {
			ObjInit(o);
		}else if (compiler->m_ObjStack.elemCount > 1) {
			IBVectorPop(&compiler->m_ObjStack, NULL);
			o=CompilerGetObj(compiler);
		}
	}else{
#ifdef DEBUGPRINTS
		DbgFmt("Pop obj: ", "");
		ObjPrint(o);
#endif
		if (compiler->m_ObjStack.elemCount == 1) {
			ObjFree(o);
			ObjInit(o);
		}
		else if (compiler->m_ObjStack.elemCount > 1) {
			printf("ec: %d\n", compiler->m_ObjStack.elemCount);
			IBVectorPop(&compiler->m_ObjStack, ObjFree);
			o = CompilerGetObj(compiler);
		}
		DbgFmt(" -> ","");
		assert(compiler->m_ObjStack.elemCount);
#ifdef DEBUGPRINTS
		ObjPrint(o);
		DbgFmt("\n","");
#endif
	}
	if(objDP) (*objDP) = o;
}
void _CompilerPush(Compiler* compiler, Op mode, bool strAllowSpace){
	Op cm;
	assert(compiler);
	cm=CompilerGetMode(compiler);
	compiler->m_StrAllowSpace = strAllowSpace;
	IBVectorCopyPushOp(&compiler->m_ModeStack, mode);
	DbgFmt(" push: %s(%d) -> %s(%d)\n", GetOpName(cm), (int)cm, GetOpName(mode), (int)mode);
}
void _CompilerPop(Compiler* compiler) {
	Op type=OP_Null;
	Op mode;
	Task *t;
	Expects* exp;
	assert(compiler);
	t = CompilerGetTask(compiler);
	assert(t);
	mode=CompilerGetMode(compiler);
	IBVectorPop(&compiler->m_ModeStack, NULL);
	if(t)type=t->type;
	DbgFmt(" pop: to %s(%d) Task type:%s(%d)\n", GetOpName(mode), (int)mode, GetOpName(type), (int)type);
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
void CompilerPushExpects(Compiler* compiler, Expects **expDP){
	Task* t;
	Expects* exp;
	t = CompilerGetTask(compiler);
	assert(t);
	assert(expDP);
	if(expDP){
		exp = (Expects*)IBVectorPush(&t->expStack);
		assert(exp);
		(*expDP) = exp;
	}	
}
void CompilerPopExpects(Compiler* compiler) {
	Task* t = CompilerGetTask(compiler);
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
		//if (!GetExpectsStack->elemCount) Err(OP_ErrNOT_GOOD, "catastrophic failure");
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
bool CompilerIsPfxExpected(Compiler* compiler, Op pfx) {
	Op* oi;
	int idx;
	Task* t;
	Expects* ap;
	t = NULL;
	ap = NULL;
	t = CompilerGetTask(compiler);
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
bool CompilerIsNameOpExpected(Compiler* compiler, Op nameOp){
	Op* oi;
	int idx;
	Task* t;
	Expects* exp;
	t = CompilerGetTask(compiler);
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
void CompilerTick(Compiler* compiler, FILE* f){
	char ch;
	if (compiler->inputStr) {
		CompilerInputStr(compiler, compiler->inputStr);
		compiler->inputStr = NULL;
	}
	else {
		assert(f);
		if ((ch = (char)fgetc(f)) != EOF) {
			if (ch != 0xffffffff)
				CompilerInputChar(compiler, ch);
		}
		else compiler->m_Running = false;
	}
}
/*NO NEWLINES AT END OF STR*/
void CompilerInputChar(Compiler* compiler, char ch){
	Op m;
	Task* t;
	bool nl;
	compiler->m_Ch = ch;
	nl = false;
	m=CompilerGetMode(compiler);
	t = CompilerGetTask(compiler);
	if(compiler->m_CommentMode==OP_NotSet&&
		compiler->m_Ch==COMMENT_CHAR/*&&
		compiler->m_LastCh!=COMMENT_CHAR*/)
	{
		/*PLINE;
		DbgFmt(" LINE COMMENT ON\n","");*/
		compiler->m_CommentMode = OP_Comment;
		CompilerPush(compiler, OP_ModeComment, false);
	}else if(compiler->m_CommentMode==OP_Comment&&
			compiler->m_LastCh==compiler->m_Ch &&
				!compiler->m_StringMode
				&&compiler->m_Ch==COMMENT_CHAR&&
				m==OP_ModeComment)
	{
		/*PLINE;
		DbgFmt(" MULTI COMMENT ON!!!!!!\n","");*/
		CompilerPop(compiler);
		CompilerPush(compiler, OP_ModeMultiLineComment, false);
		compiler->m_CommentMode = OP_MultiLineComment;
		compiler->m_Ch='\0';
	}else if(compiler->m_CommentMode==OP_MultiLineComment&&
		compiler->m_LastCh==compiler->m_Ch &&
				!compiler->m_StringMode
				&&compiler->m_Ch==COMMENT_CHAR&&
				m==OP_ModeMultiLineComment) {
		/*PLINE;
		DbgFmt(" MULTI COMMENT OFF!\n","");*/
		compiler->m_CommentMode=OP_NotSet;
	}
	switch (compiler->m_Ch) {
	case '\0': return;
	case '\n': {
		nl = true;
		if (compiler->m_CommentMode == OP_NotSet) {
			DbgFmt("Char():Line end\n","");
		}
		if (compiler->m_CommentMode == OP_Comment) {
			CompilerPop(compiler);
			compiler->m_CommentMode = OP_NotSet;
		}
		switch (m) {
		case OP_ModeStrPass: {
			CompilerStrPayload(compiler);
			break;
		}
		}
		switch (GetObjType) {
		case OP_CallWantArgs: {
			CompilerPopObj(compiler, true, NULL);
			break;
		}
		case OP_VarWantValue:
		case OP_VarComplete: {
			CompilerPopObj(compiler, true, NULL);
			break;
		}
		}
		switch(t->type){
		case OP_ThingWantRepr: {
			SetTaskType(t, OP_ThingWantContent);
			break;
		}
			case OP_CPrintfHaveFmtStr: {
				CompilerPopAndDoTask(compiler);
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
				SetObjType(OP_FuncSigComplete);
				PopExpects();
				mod = ObjGetMod(CompilerGetObj(compiler));
				CompilerPopObj(compiler, true, NULL);
				if (mod != OP_Imaginary) {
					Expects *exp;
					CompilerPushExpects(compiler, &exp);
					ExpectsInit(exp, 0,"","",
						"PPPNN",
						OP_Op, OP_String, OP_VarType, OP_If, OP_Done);
					SetTaskType(t, OP_FuncWantCode);
				}
				else {
					CompilerPopAndDoTask(compiler);
				}
				break;
			}
		}
		break;
	}
	}
	m = CompilerGetMode(compiler);
	compiler->m_Column++;
	if (!nl && compiler->m_CommentMode == OP_NotSet) {
		/*if(compiler->m_Ch == ' ') printf("-> SPACE (0x%x)\n",  compiler->m_Ch);
		else printf("-> %c (0x%x) %d:%d\n",
			compiler->m_Ch, compiler->m_Ch, compiler->m_Line, compiler->m_Column);*/
		switch (m) {
		case OP_ModeComment:
		case OP_ModeMultiLineComment:
			break;
		case OP_ModePrefixPass:
			CompilerPrefix(compiler);
			break;
		case OP_ModeStrPass:
			CompilerStr(compiler);
			break;
		default: Err(OP_ErrNOT_GOOD, "unknown mode");
			break;
		}
	}
	if (nl) {
		if (CompilerIsPfxExpected(compiler, OP_LineEnd)) PopExpects();
		compiler->m_Column = 0;
		compiler->m_Line++;
	}
	compiler->m_LastCh=compiler->m_Ch;
	if(m==OP_ModeMultiLineComment&&compiler->m_CommentMode==OP_NotSet){
		CompilerPop(compiler);
	}
}
void CompilerInputStr(Compiler* compiler, char* str)
{
	int i;
	for (i = 0; str[i] != '\0'; i++)
		CompilerInputChar(compiler, str[i]);
}
Val CompilerStrToVal(Compiler* compiler, char* str, Op expectedType) {
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
char* CompilerGetCPrintfFmtForType(Compiler* compiler, Op type) {
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
	Err(OP_ErrNOT_GOOD, "GetPrintfFmtForType: unknown type");
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
void CompilerPopAndDoTask(Compiler* compiler)	{
	IBVector* wObjs;
	Task* t;
	bool subTask;
	t = CompilerGetTask(compiler);
	assert(t);
	DbgFmt("PopAndDoTask()\n","");
	if(!compiler->m_TaskStack.elemCount)Err(OP_ErrNoTask, "task stack EMPTY!");
	wObjs = &t->working;
	assert(wObjs);
	if(!wObjs->elemCount)Err(OP_ErrNOT_GOOD, "workingObjs EMPTY!");
	subTask = false;
	switch (t->type) {
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

				IBStrAppend(&header, "struct ");
				IBStrAppend(&header, o->name);
				IBStrAppend(&header, " {\n");
				/*IBStrAppend(&header, "\t");
				IBStrAppend(&header, ThingStructTypeHeaderVarType);
				IBStrAppend(&header, " ");
				IBStrAppend(&header, ThingStructTypeHeaderVarName);
				IBStrAppend(&header, ";\n");*/
				IBStrAppend(&footer, "} ");
				IBStrAppend(&footer, o->name);
				IBStrAppend(&footer, ";\n\n");

				break;
			}
			case OP_VarWantValue:
			case OP_VarComplete: {
				IBStrAppend(&body, "\t");
				IBStrAppend(&body, GetCEqu(o->var.type));
				IBStrAppend(&body, GetCEqu(o->var.mod));
				IBStrAppend(&body, " ");
				IBStrAppend(&body, o->name);
				IBStrAppend(&body, ";\n");
				//default values will be stored in db
				break;
			}
			}
		}
		IBStrAppend(&compiler->m_CHeaderStructs, header.start);
		IBStrAppend(&compiler->m_CHeaderStructs, body.start);
		IBStrAppend(&compiler->m_CHeaderStructs, footer.start);
		IBStrFree(&header);
		IBStrFree(&body);
		IBStrFree(&footer);
		break;
	}
	case OP_SpaceHasName: {

		break;
	}
	case OP_FuncWantCode: break;
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
				if (at == OP_Null)Err(OP_ErrNOT_GOOD, "arg type NULL");

				if (cFuncArgs.start[0] != '\0') {
					IBStrAppend(&cFuncArgs, ", ");
				}
				IBStrAppend(&cFuncArgs, GetCEqu(o->arg.type));
				IBStrAppend(&cFuncArgs, GetCEqu(o->arg.mod));
				IBStrAppend(&cFuncArgs, " ");
				if (!o->name)Err(OP_ErrNOT_GOOD, "arg name NULL");
				IBStrAppend(&cFuncArgs, o->name);
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
					/*StrConcat(cFuncModsTypeName, CODE_STR_MAX, GetCEqu(mod));
					StrConcat(cFuncModsTypeName, CODE_STR_MAX, " ");*/
					IBStrAppend(&cFuncModsTypeName, GetCEqu(mod));
					IBStrAppend(&cFuncModsTypeName, " ");
				}
				/*StrConcat(cFuncModsTypeName, CODE_STR_MAX, GetCEqu(o->func.retType));
				StrConcat(cFuncModsTypeName, CODE_STR_MAX, GetCEqu(o->func.retTypeMod));
				StrConcat(cFuncModsTypeName, CODE_STR_MAX, " ");*/
				IBStrAppend(&cFuncModsTypeName, GetCEqu(o->func.retType));
				IBStrAppend(&cFuncModsTypeName, GetCEqu(o->func.retTypeMod));
				IBStrAppend(&cFuncModsTypeName, " ");
				if (!o->name)Err(OP_ErrNOT_GOOD, "func name NULL");
				if (o->name) {
					if (o->func.thingTask)
					{
						Obj* wo;
						int idx;
						idx = 0;
						wo = CompilerFindStackObjUnderTop(compiler, OP_Thing);
						if (wo) {
							/*StrConcat(cFuncModsTypeName, CODE_STR_MAX, wo->name);
							StrConcat(cFuncModsTypeName, CODE_STR_MAX, "_");*/
							IBStrAppend(&cFuncModsTypeName, wo->name);
							IBStrAppend(&cFuncModsTypeName, "_");
							thingObj = wo;
						}
					}
					//StrConcat(cFuncModsTypeName, CODE_STR_MAX, o->name);
					IBStrAppend(&cFuncModsTypeName, o->name);
				}
				//StrConcat(cFuncModsTypeName, CODE_STR_MAX, "(");
				IBStrAppend(&cFuncModsTypeName, "(");
				if (thingObj) {
					/*StrConcat(cFuncArgsThing, CODE_STR_MAX, "struct ");
					StrConcat(cFuncArgsThing, CODE_STR_MAX, thingObj->name);
					StrConcat(cFuncArgsThing, CODE_STR_MAX, "* ptr");*/
					IBStrAppend(&cFuncArgsThing, "struct ");
					IBStrAppend(&cFuncArgsThing, thingObj->name);
					IBStrAppend(&cFuncArgsThing, "* ptr");
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
				/*StrConcat(cFuncCode, CODE_STR_MAX, "\t");
				StrConcat(cFuncCode, CODE_STR_MAX, GetCEqu(o->var.type));
				StrConcat(cFuncCode, CODE_STR_MAX, GetCEqu(o->var.mod));
				StrConcat(cFuncCode, CODE_STR_MAX, " ");*/
				IBStrAppend(&cFuncCode, "\t");
				IBStrAppend(&cFuncCode, GetCEqu(o->var.type));
				IBStrAppend(&cFuncCode, GetCEqu(o->var.mod));
				IBStrAppend(&cFuncCode, " ");
				if (!o->name)Err(OP_ErrNOT_GOOD, "var name NULL");
				//StrConcat(cFuncCode, CODE_STR_MAX, o->name);
				IBStrAppend(&cFuncCode, o->name);
				if (o->var.valSet) {
					//StrConcat(cFuncCode, CODE_STR_MAX, " = ");
					IBStrAppend(&cFuncCode, " = ");
					Val2Str(valBuf, 32, o->var.val, o->var.type);
					//StrConcat(cFuncCode, 32, valBuf);
					//sprintf(valBuf, "%d", o->var.val.i32);
					//StrConcat(cFuncCode, 32, valBuf);
					IBStrAppend(&cFuncCode, valBuf);

				}
				//StrConcat(cFuncCode, CODE_STR_MAX, ";\n");
				IBStrAppend(&cFuncCode, ";\n");
				break;
			}
			}
		}
		if (imaginary) {
			//StrConcat(cFuncArgsEnd, CODE_STR_MAX, ");\n\n");
			IBStrAppend(&cFuncArgsEnd, ");\n\n");
		}
		else {
			/*StrConcat(cFuncArgsEnd, CODE_STR_MAX, "){\n");
			StrConcat(cFuncCode, CODE_STR_MAX, GetTaskCodeP1);*/
			IBStrAppend(&cFuncArgsEnd, "){\n");
			IBStrAppend(&cFuncCode, t->code1.start);
			if(!funcObj)Err(OP_ErrNOT_GOOD, "funcObj NULL");
			if (funcObj->func.retType != OP_Void) {
				char valBuf[32];
				valBuf[0] = '\0';
				//StrConcat(cFuncCode, CODE_STR_MAX, "\treturn ");
				IBStrAppend(&cFuncCode, "\treturn ");
				Val2Str(valBuf, 32, funcObj->func.retVal, funcObj->func.retType);
				/*StrConcat(cFuncCode, 32, valBuf);
				StrConcat(cFuncCode, CODE_STR_MAX, ";\n");*/
				IBStrAppend(&cFuncCode, valBuf);
				IBStrAppend(&cFuncCode, ";\n");
			}
			//StrConcat(cFuncCode, CODE_STR_MAX, "}\n\n");
			IBStrAppend(&cFuncCode, "}\n\n");
		}
		if (strcmp(funcObj->name, "main"))
		{
			IBStrAppend(&compiler->m_CHeaderFuncs, cFuncModsTypeName.start);
			IBStrAppend(&compiler->m_CHeaderFuncs, cFuncArgsThing.start);
			if (argc && thingObj) IBStrAppend(&compiler->m_CHeaderFuncs, ", ");
			IBStrAppend(&compiler->m_CHeaderFuncs, cFuncArgs.start);
			IBStrAppend(&compiler->m_CHeaderFuncs, ");\n");
		}
		if (!imaginary) {
			IBStrAppend(&compiler->m_CFile, cFuncModsTypeName.start);
			IBStrAppend(&compiler->m_CFile, cFuncArgsThing.start);
			if (argc && thingObj) IBStrAppend(&compiler->m_CFile, ", ");
			IBStrAppend(&compiler->m_CFile, cFuncArgs.start);
			IBStrAppend(&compiler->m_CFile, cFuncArgsEnd.start);
			IBStrAppend(&compiler->m_CFile, cFuncCode.start);
		}
		break;
	}
	case OP_CPrintfHaveFmtStr: {
		bool firstPercent;
		Obj* fmtObj;
		int varIdx;
		int i;
		subTask = true;
		if (t && wObjs->elemCount) {
			fmtObj = (Obj*)wObjs->data;
			//StrConcat(GetTaskCode, CODE_STR_MAX, "\tprintf(\"");
			IBStrAppend(&t->code1, "\tprintf(\"");
			firstPercent = false;
			varIdx = 1;
			for (i = 0; i < (int)strlen(fmtObj->str); ++i) {
				char c;
				c = fmtObj->str[i];
				switch (c) {
				case '%':{
						if (!firstPercent) {
							//StrConcat(GetTaskCode, CODE_STR_MAX, "%");
							IBStrAppend(&t->code1, "%");
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
								Op type = NameInfoDBFindType(&compiler->m_NameTypeCtx, vo->name);
								IBStrAppend(&t->code1, 
									CompilerGetCPrintfFmtForType(compiler, type));
								break;
							}
							case OP_String:
								assert(vo->var.type==OP_String);
							case OP_Value:{
								IBStrAppend(&t->code1,
									CompilerGetCPrintfFmtForType(compiler, vo->var.type));
								break;
							}
							case OP_CPrintfFmtStr: break;
							default:{
								Err(OP_ErrNOT_GOOD, "unhandled printf arg type");
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
					//StrConcat(GetTaskCode, CODE_STR_MAX, chBuf);
					IBStrAppend(&t->code1, chBuf);
					break;
				}
				}
			}
			//StrConcat(GetTaskCode, CODE_STR_MAX, "\"");
			IBStrAppend(&t->code1, "\"");
			if (wObjs->elemCount > 1) {
				//StrConcat(GetTaskCode, CODE_STR_MAX, ", ");
				IBStrAppend(&t->code1, ", ");
			}
			for (i = 1; i < wObjs->elemCount; ++i) {
				Obj* o;
				o = (Obj*)IBVectorGet(wObjs, i);
				switch (ObjGetType(o)) {
				case OP_Name: {
					//StrConcat(GetTaskCode, CODE_STR_MAX, o->name);
					IBStrAppend(&t->code1, o->name);
					break;
				}
				case OP_String: {
					/*StrConcat(GetTaskCode, CODE_STR_MAX, "\"");
					StrConcat(GetTaskCode, CODE_STR_MAX, o->str);
					StrConcat(GetTaskCode, CODE_STR_MAX, "\"");*/
					IBStrAppend(&t->code1, "\"");
					IBStrAppend(&t->code1, o->str);
					IBStrAppend(&t->code1, "\"");
					break;
				}
				case OP_Value: {
					char valBuf[32];
					valBuf[0] = '\0';
					Val2Str(valBuf, 32, o->val, o->var.type);
					//StrConcat(GetTaskCode, 32, valBuf);
					IBStrAppend(&t->code1, valBuf);
					break;
				}
				}
				if (i < wObjs->elemCount - 1) {
					//StrConcat(GetTaskCode, CODE_STR_MAX, ", ");
					IBStrAppend(&t->code1, ", ");
				}
			}
			//StrConcat(GetTaskCode, CODE_STR_MAX, ");\n");
			IBStrAppend(&t->code1, ");\n");
		}
		break;
	}
	}
	if (subTask) {
		switch (t->type) {
		case OP_CPrintfHaveFmtStr: {
			if (compiler->m_TaskStack.elemCount - 2 >= 0) {
				/*char theCode[CODE_STR_MAX];
				theCode[0] = '\0';*/
				IBStr theCode;
				IBStrInit(&theCode, 1);
				IBStrAppend(&theCode, t->code1.start);
				CompilerPopTask(compiler);
				switch (t->type) {
				case OP_FuncWantCode: {
					IBStrAppend(&t->code1, theCode.start);
					break;
				}
				}
			}else Err(OP_ErrNOT_GOOD, "compiler->m_TaskStack.size() - 2 < 0");
			break;
		}
		}
	}
	else CompilerPopTask(compiler);
}
void CompilerPrefix(Compiler* compiler){
	Obj* obj;
	Task* t;
	Expects* expTop;
	t = CompilerGetTask(compiler);
	assert(t);
	expTop = TaskGetExpTop(t);
	/*for assigning func call ret val to var*/
	if (compiler->m_Pfx == OP_Value && compiler->m_Ch == '@' && !compiler->m_Str[0]) {
		Expects* exp;
		CompilerPushExpects(compiler, &exp);
		ExpectsInit(exp, 1, "", "", "P", OP_Op);
	}
	compiler->m_Pfx = fromPfxCh(compiler->m_Ch);
	if(compiler->m_Pfx == OP_SpaceChar
		|| compiler->m_Pfx == OP_TabChar) return;
	if (compiler->m_Pfx == OP_Unknown) Err(OP_ErrUnknownPfx, "catastrophic err");
	obj=CompilerGetObj(compiler);
	if (compiler->m_Pfx != OP_Unknown
		&& (!t || expTop->pfxs.elemCount)
		&& !CompilerIsPfxExpected(compiler, compiler->m_Pfx)
	)
	{
		Err(OP_ErrUnexpectedNextPfx, "");
	}
	DbgFmt("PFX:%s(%d)\n", GetPfxName(compiler->m_Pfx), (int)compiler->m_Pfx);
	switch (compiler->m_Pfx) {
	case OP_String: { /* " */
		compiler->m_StringMode = true;
		CompilerPush(compiler, OP_ModeStrPass, false);
		break;
	}
	case OP_VarType:
		IBVectorCopyPushBool(&compiler->m_StrReadPtrsStack, true);
	case OP_Value:
	case OP_Op:
	case OP_Name:
		/*getchar();*/
		CompilerPush(compiler, OP_ModeStrPass, false);
		break;
	case OP_SpaceChar:
		break;
	case OP_Comment:
		break;
	}
	if (compiler->m_Pfx == OP_Op) {
		if (expTop && expTop->life && --expTop->life <= 0) {
			IBVectorPop(&t->expStack, ExpectsFree);
		}
	}
}
void CompilerStr(Compiler* compiler){
	char chBuf[2];
	chBuf[0] = compiler->m_Ch;
	chBuf[1] = '\0';
	if (compiler->m_StringMode) {
		switch (compiler->m_Ch) {
		case '"': {
			compiler->m_StringMode = false;
			CompilerStrPayload(compiler);
			return;
		}
		}
	}
	else {
		switch (compiler->m_Pfx) {
		case OP_Value: {
			switch (compiler->m_Ch) {
			case '@': {
				CompilerPop(compiler);
				CompilerPrefix(compiler);
				return;
			}
			}
			break;
		}
		}
		switch (compiler->m_Ch) {
		case '\t': return;
		case ' ': {
			if (compiler->m_StrAllowSpace) break;
			else {
					CompilerStrPayload(compiler);
				return;
			}
		}
		case '&': {
			if (*(bool*)IBVectorTop(&compiler->m_StrReadPtrsStack)) {
				switch (compiler->m_Pointer) {
				case OP_NotSet:
					DbgFmt("Got pointer\n","");
					compiler->m_Pointer = OP_Pointer;
					break;
				case OP_Pointer:
					DbgFmt("Got double pointer\n","");
					compiler->m_Pointer = OP_DoublePointer;
					break;
				case OP_DoublePointer:
					DbgFmt("Got tripple pointer\n","");
					compiler->m_Pointer = OP_TripplePointer;
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
	StrConcat(compiler->m_Str, CompilerSTR_MAX, chBuf);
}
Task* CompilerGetTask(Compiler *compiler){
	Task* ret= (Task*)IBVectorTop(&compiler->m_TaskStack);
	if (!ret)Err(OP_ErrNOT_GOOD, "no task in stack");
	return ret;
}
Op CompilerGetMode(Compiler* compiler){
	Op ret=OP_Null;
	Op* p=(Op*)IBVectorTop(&compiler->m_ModeStack);
	if (p)ret = *p;
	return ret;
}
Expects* TaskGetExpTop(Task* t){
	Expects* ret;
	assert(t);
	ret = (Expects*)IBVectorTop(&t->expStack);
	assert(ret);
	return ret;
}
IBVector* TaskGetExpPfxsTop(Task* t){
	Expects* ret;
	assert(t);
	ret = (Expects*)IBVectorTop(&t->expStack);
	assert(ret);
	return &ret->pfxs;
}
IBVector* TaskGetExpNameOPsTop(Task* t){
	Expects* ret;
	assert(t);
	ret = (Expects*)IBVectorTop(&t->expStack);
	assert(ret);
	return &ret->nameOps;
}
void CompilerStrPayload(Compiler* compiler){
	Val strVal;
	Task *t;
	t=CompilerGetTask(compiler);
	strVal.i32=atoi(compiler->m_Str);
	compiler->m_NameOp = GetOpFromName(compiler->m_Str);
	DbgFmt("StrPayload: %s\n", compiler->m_Str);
	switch (compiler->m_Pfx)
	{
	case OP_String: { /* " */
		switch(t->type){
		case OP_dbgAssertWantArgs: {
			switch (GetOpFromNameList(compiler->m_Str, OP_dbgAssert)) {
			case OP_TaskType:{
				break;
			}
			}
			break;
		}
		case OP_CPrintfHaveFmtStr:{
			Obj *o;
			CompilerPushObj(compiler, &o);
			ObjSetType(o, OP_String);
			o->var.type=OP_String;
			ObjSetStr(o, compiler->m_Str);
			CompilerPopObj(compiler, true, NULL);
			break;
		}
		case OP_FuncWantCode: { /*printf*/
			Expects *ap;
			Obj* o;
			CompilerPushTask(compiler, OP_CPrintfHaveFmtStr, &ap, NULL);
			ExpectsInit(ap, 0, "expected fmt args or line end",
				"",	"PPPP", OP_Value, OP_Name, OP_String, OP_LineEnd);
			CompilerPushObj(compiler, &o);
			ObjSetStr(o, compiler->m_Str);
			ObjSetType(o, OP_CPrintfFmtStr);
			CompilerPopObj(compiler, true, NULL);
			break;
		}
		}
		break;
	}
	case OP_Value: { /*=*/
		switch (GetObjType) {
		case OP_VarWantValue: {
			Obj* o;
			o = CompilerGetObj(compiler);
			o->var.val = strVal;
			o->var.valSet = true;
			SetObjType(OP_VarComplete);
			PopExpects();
			break;
		}
		}
		if (compiler->m_TaskStack.elemCount) {
			switch (t->type) {
			case OP_CPrintfHaveFmtStr:{
				Obj *o;
				CompilerPushObj(compiler, &o);
				o->val = strVal;
				ObjSetType(o, OP_Value);
				o->var.type = OP_i32;/*for now*/
				CompilerPopObj(compiler, true, NULL);
				break;
			}
			case OP_FuncNeedRetVal: {
				Obj* o;
				int idx;
				idx = 0;
				while (o = (Obj*)IBVectorIterNext(&t->working,&idx)) {
					if (ObjGetType(o) == OP_FuncSigComplete) {
						DbgFmt("Finishing func got ret value\n","");
						o->func.retVal = CompilerStrToVal(compiler, compiler->m_Str, o->func.retType);
						PopExpects();
						SetTaskType(t, OP_Func);
						CompilerPopAndDoTask(compiler);
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
			CompilerPushObj(compiler, &o);
			o->var.type = compiler->m_NameOp;
			o->var.mod = compiler->m_Pointer;
			o->var.privacy=compiler->m_Privacy;
			o->var.valSet = false;
			SetObjType(OP_VarNeedName);
			CompilerPushExpects(compiler, &exp);
			ExpectsInit(exp, 0, "expected variable name", "", "P", OP_Name);
			break;
		}
		case OP_FuncWantCode: {
			Obj* o;
			Expects* exp;
			CompilerPushObj(compiler, &o);
			o->var.type = compiler->m_NameOp;
			o->var.mod = compiler->m_Pointer;
			o->var.valSet = false;
			SetObjType(OP_VarNeedName);
			CompilerPushExpects(compiler, &exp);
			ExpectsInit(exp, 0, "expected variable name", "", "P", OP_Name);
			break;
		}
		}
		switch (GetObjType) {
		case OP_FuncNeedsRetValType: {
			if (t->type != OP_FuncHasName)Err(OP_ErrNOT_GOOD, "func signature needs name");
			CompilerGetObj(compiler)->func.retType = compiler->m_NameOp;
			CompilerGetObj(compiler)->func.retTypeMod = compiler->m_Pointer;
			SetObjType(OP_FuncSigComplete);
			break;
		}
		case OP_FuncHasName: {
			Obj* o;
			Expects* exp;
			CompilerPushObj(compiler, &o);
			SetObjType(OP_FuncArgNameless);
			o->arg.type = compiler->m_NameOp;
			o->arg.mod = compiler->m_Pointer;
			CompilerPushExpects(compiler, &exp);
			ExpectsInit(exp, 0, "expected func arg name", "", "P", OP_Name);
			break;
		}
		}
		break;
	case OP_Name: { /* $ */
		switch(t->type){
		case OP_ThingWantName: {
			Obj* o;
			Expects* exp;
			/*assert(GetObjCount == 1);
			assert(GetObjType == OP_NotSet);*/
			CompilerPushObj(compiler, &o);
			assert(compiler->m_Str[0]!='\0');
			ObjSetName(o, compiler->m_Str);
			ObjSetType(o, OP_Thing);
			t->type = OP_ThingWantRepr;
			PopExpects();
			/*CompilerPushExpects(compiler, &exp);
			ExpectsInit(exp, 0, "", "",
				"PPN",
				OP_Op, OP_LineEnd, OP_Repr);*/
			/*t->type = OP_ThingWantContent;
			CompilerPushExpects(compiler, &exp);
			ExpectsInit(exp, 0, "expected vartype (%)", "expected @pub, @priv, or @junt", 
				"PPNNNN", 
				OP_Op, OP_VarType, OP_Func, OP_Public, OP_Private, OP_Done);*/
			break;
		}
		case OP_UseNeedStr: {
			Op lib;
			lib = compiler->m_NameOp;
			switch (lib) {
			case OP_UseStrSysLib: {
				DbgFmt("Inputting system lib code to compiler\n","");
				assert(!compiler->inputStr);
				compiler->inputStr = SysLibCodeStr;
				break;
			}
			default: {

				break;
			}
			}
			CompilerPopTask(compiler);
			break;
		}
		case OP_SpaceNeedName: {
			Obj* o;
			CompilerPushObj(compiler, &o);
			assert(compiler->m_Str[0]!='\0');
			ObjSetName(o, compiler->m_Str);
			ObjSetType(o, OP_Space);
			CompilerPopObj(compiler, true, &o);
			t->type = OP_SpaceHasName;
			CompilerPopAndDoTask(compiler);
			break;
		}
		case OP_CPrintfHaveFmtStr: {
			Obj* o;
			CompilerPushObj(compiler, &o);
			ObjSetName(o, compiler->m_Str);
			ObjSetType(o, OP_Name);
			CompilerPopObj(compiler, true, NULL);
			break;
		}
		}


		switch (GetObjType) {
		case OP_VarNeedName: {
			Expects* exp;
			ObjSetName(CompilerGetObj(compiler), compiler->m_Str);
			NameInfoDBAdd(&compiler->m_NameTypeCtx, compiler->m_Str, CompilerGetObj(compiler)->var.type);
			SetObjType(OP_VarWantValue);
			PopExpects();
			CompilerPushExpects(compiler, &exp);
			ExpectsInit(exp, 0,
				"expected value or line end after var name",
				"",
				"PP",
				OP_Value, OP_LineEnd);
			break;
		}
		case OP_CallNeedName: { /* =@call */
			Expects* exp;
			SetObjType(OP_CallWantArgs);
			PopExpects();
			CompilerPushExpects(compiler, &exp);
			ExpectsInit(exp, 0,
				"expected var type or line end after func name", "",
				"PPP",
				OP_Name, OP_Value, OP_LineEnd);
		}
		case OP_Func: {
			Expects* exp;
			SetObjType(OP_FuncHasName);
			SetTaskType(t, OP_FuncHasName);
			CompilerPushExpects(compiler, &exp);
			ExpectsInit(exp, 0, "", "",
				"PPPN",
				OP_VarType, OP_Op, OP_LineEnd, OP_Return);
			ObjSetName(CompilerGetObj(compiler), compiler->m_Str);
			break;
		}
		case OP_FuncArgNameless:
			SetObjType(OP_FuncArgComplete);
			PopExpects();
			ObjSetName(CompilerGetObj(compiler), compiler->m_Str);
			NameInfoDBAdd(&compiler->m_NameTypeCtx, compiler->m_Str, CompilerGetObj(compiler)->arg.type);
			CompilerPopObj(compiler, true, NULL);
			break;
		}
		break;
	}
	case OP_Op: { /* @ */
		bool expected;
		expected = CompilerIsNameOpExpected(compiler, compiler->m_NameOp);
		if(!expected)Err(OP_ErrUnexpectedNameOP, "unexpected nameOP");
		switch (compiler->m_NameOp) {
		case OP_Repr: {
			switch (t->type) {
			case OP_ThingWantRepr: {
				Expects* exp;
				//t->type = OP_ThingWantContent;
				CompilerPushExpects(compiler, &exp);
				ExpectsInit(exp, 0, "expected vartype (%)", "", "P", OP_VarType);
				break;
			}
			}
			break;
		}
		case OP_Space: {
			Expects* ap;
			CompilerPushTask(compiler, OP_SpaceNeedName, &ap, NULL);
			ExpectsInit(ap, 0, "expected space name", "", "P", OP_Name);
			break;
		}
		case OP_Thing: {
			Expects* ap;
			CompilerPushTask(compiler, OP_ThingWantName, &ap, NULL);
			ExpectsInit(ap, 0, "", "", "PNN", OP_Op, OP_Done);
			CompilerPushExpects(compiler, &ap);
			ExpectsInit(ap, 0, "", "", "PPN", OP_Op, OP_LineEnd, OP_Repr);
			CompilerPushExpects(compiler, &ap);
			ExpectsInit(ap, 0, "expected thing name", "", "P", OP_Name);
			break;
		}
		case OP_dbgAssert: {
			Expects* ap;
			CompilerPushTask(compiler, OP_dbgAssertWantArgs, &ap, NULL);
			ExpectsInit(ap, 0, "expected string", "", "P", OP_String);
			break;
		}
		case OP_Call: {
			switch (GetObjType) {
			case OP_VarWantValue: {
				Obj* o;
				Expects* exp;
				CompilerPushObj(compiler, &o);
				ObjSetType(o, OP_CallNeedName);
				CompilerPushExpects(compiler, &exp);
				ExpectsInit(exp, 0, "expected function name", "", "P", OP_Name);
			}
			}
			break;
		}
		case OP_dbgBreak: {
			/*__debugbreak();*/
			/*compiler->m_TaskStack;*/
			break;
		}
		case OP_Imaginary: {
			Expects* exp;
			ObjSetMod(CompilerGetObj(compiler), compiler->m_NameOp);
			CompilerPushExpects(compiler, &exp);
			ExpectsInit(exp, 1, "", "", "PN", OP_Op, OP_Func);
			break;
		}
		case OP_Done:
			if (!compiler->m_TaskStack.elemCount) Err(OP_ErrNoTask, "");
			switch (t->type) {
			case OP_ThingWantContent: {
				Obj* o;
				o = CompilerGetObj(compiler);
				assert(o->type == OP_Thing);
				CompilerPopObj(compiler, true, NULL);
				assert(GetObjType == OP_NotSet);
				CompilerPopAndDoTask(compiler);
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
							CompilerPushExpects(compiler, &exp);
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
							CompilerPopAndDoTask(compiler);
						}
					}
				}
				break;
			}
			}
			break;
		case OP_Return: {
			Op objT = GetObjType;
			switch (objT) {
			case OP_FuncArgComplete: {
				DbgFmt("what\n","");
				CompilerPopObj(compiler, true, NULL);
				if (GetObjType != OP_FuncHasName) {
					Err(OP_ErrNOT_GOOD, "expected FuncHasName");
					break;
				}
			}
			case OP_FuncHasName: {
				Expects* exp;
				SetObjType(OP_FuncNeedsRetValType);
				CompilerPushExpects(compiler, &exp);
				ExpectsInit(exp, 0, "", "", "P", OP_VarType);
				break;
			}
			default:
				Err(OP_ErrUnexpectedOp, "");
				break;
			}
			break;
		}
		case OP_Func: {
			Expects* ap;
			Obj* o;
			Task* t;
			t = CompilerGetTask(compiler);
			assert(t);
			CompilerPushObj(compiler, &o);
			o->func.thingTask = t->type == OP_ThingWantContent ? t : NULL;
			CompilerPushTask(compiler, OP_FuncNeedName, &ap, NULL);
			ExpectsInit(ap, 0, "expected function name", "", "P", OP_Name);
			o->type = compiler->m_NameOp;
			o->privacy = compiler->m_Privacy;
			o->func.retType = OP_Void;
			o->func.retTypeMod = OP_NotSet;
			break;
		}
		case OP_Public:
		case OP_Private:
			compiler->m_Privacy = compiler->m_NameOp;
			break;
		case OP_Use: {
			Expects* ap;
			CompilerPushTask(compiler, OP_UseNeedStr, &ap, NULL);
			ExpectsInit(ap, 0, "expected @use name", "", "P", OP_Name);
			break;
		}
		default:
			Err(OP_ErrUnknownOpStr, "");
		}
		break;
	}
	}
	compiler->m_Str[0] = '\0';
#ifdef DEBUGPRINTS
	printf("Str payload complete\n");
#endif
	CompilerPop(compiler);
	if(compiler->m_StrReadPtrsStack.elemCount > 1){
		if (*(bool*)IBVectorTop(&compiler->m_StrReadPtrsStack))
			compiler->m_Pointer = OP_NotSet;
		IBVectorPop(&compiler->m_StrReadPtrsStack, NULL);
	}
}
void CompilerExplainErr(Compiler* compiler, Op code) {
	switch (code) {
	case OP_FuncNeedRetVal: {
		printf("You forgot to return a value from the function");
		break;
	}
	case OP_ErrUnexpectedNameOP: {
		Expects* exp;
		Task* t;
		t=CompilerGetTask(compiler);
		assert(t);
		exp = TaskGetExpTop(t);
		printf("NameOP \"@%s\" wasn't expected.\nExpected:\n", compiler->m_Str);
		ExpectsPrint(exp);
		break;
	}
	case OP_ErrUnknownPfx:
		printf("This prefix \"%c\" is unknown!", compiler->m_Ch);
		break;
	case OP_ErrUnknownOpStr:
		printf("Unknown OP str @%s\n", compiler->m_Str);
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
		Task *t;
		int idx;
		t =CompilerGetTask(compiler);
		ap=TaskGetExpTop(t);
		if(ap && t){
			assert(ap->pfxs.elemCount);
			printf("Err: \"%s\" Unexpected next prefix %s. Pfx idx:%d\nEnforced at line %d. Allowed:",
				ap->pfxErr, GetPfxName(compiler->m_Pfx),
					ap->pfxs.elemCount - 1, ap->lineNumInited);
			idx = 0;
			while (oi = (Op*)IBVectorIterNext(&ap->pfxs,&idx)) {
				printf("%s(%d),", GetPfxName(*oi), (int)*oi);
			}
		}else{
			printf("No task. Only Op(@) prefix allowed. Pfx: %s. Allowed pfxs: %s\n", GetPfxName(compiler->m_Pfx),
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
	DbgFmt("OBJ:","");
	ObjPrint(CompilerGetObj(compiler));
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
		Compiler comp;
		CompilerInit(&comp);
		while (comp.m_Running)
			CompilerTick(&comp, f);
		DbgFmt("Exiting\n","");
		CompilerFree(&comp);
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
