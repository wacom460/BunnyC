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
#include <assert.h>
#include <stdarg.h>
#define bool char
#define true 1
#define false 0
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

#define DEBUGPRINTS

#ifdef DEBUGPRINTS
#define PLINE printf("[%d]", __LINE__)
#else
#define PLINE
#endif
#define OP_NAME_LEN 32
#define COMMENT_CHAR ('~')
#define CODE_STR_MAX 512
#define CompilerSTR_MAX 64
#define ThingStructTypeHeaderVarType ("int")
#define ThingStructTypeHeaderVarName ("__thingTYPE")
#if defined(__TINYC__) || defined(__GNUC__)
#define __debugbreak()
#endif
#define DB __debugbreak();

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
	OP_Thing,OP_ThingWantName,OP_ThingWantContent,OP_SpaceNeedName,
	OP_SpaceHasName,

	OP_SpaceChar, OP_Comma, OP_CommaSpace, OP_Name, OP_String,
	OP_CPrintfFmtStr, OP_Char, OP_If, OP_Else, OP_For, OP_While,
	OP_Block, OP_c8, OP_u8, OP_u16, OP_u32, OP_u64, OP_i8, OP_i16,
	OP_i32, OP_i64, OP_f32, OP_d64, OP_Pointer, OP_DoublePointer,
	OP_TripplePointer, OP_CompilerFlags, OP_dbgBreak, OP_dbgAssert,
	OP_dbgAssertWantArgs,OP_TaskType, OP_TaskStack, OP_NotEmpty,
	OP_TabChar,OP_UseNeedStr,OP_UseStrSysLib,

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
	unsigned char *boolean;
	struct Expects* expects;
	struct NameInfoDB* niDB;
	struct NameInfo* ni;
} IBVecData;
typedef struct IBVector {
	size_t elemSize;
	int elemCount;
	int slotCount;
	int protectedSlotCount;/*cant pop past this, if 0 then unaffecting*/
	size_t dataSize;
	IBVecData* data;/*DATA BLOCK*/
} IBVector;
void IBVectorInit(IBVector* vec, size_t elemSize);
IBVecData* IBVectorGet(IBVector* vec, int idx);
void* IBVectorIterNext(IBVector* vec, int* idx);
IBVecData* IBVectorPush(IBVector* vec);
void IBVectorCopyPush(IBVector* vec, void* elem);
void IBVectorCopyPushBool(IBVector* vec, bool val);
void IBVectorCopyPushOp(IBVector* vec, Op val);
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
	struct Obj* thingHost;
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
	char code1[CODE_STR_MAX];
	char code2[CODE_STR_MAX];
} Task;
void TaskInit(Task* t, Op type);
void TaskFree(Task* t);
typedef struct Compiler {
	int m_Line;
	int m_Column;
	Op m_Pfx;
	char m_Str[CompilerSTR_MAX];
	/*char m_cOutput[CODE_STR_MAX];*/
	IBStr m_CHeaderStructs;/* .h */
	IBStr m_CHeaderFuncs;
	IBStr m_CFile;/* .c */

	IBVector m_ObjStack; /*Obj*/
	IBVector m_ModeStack; /*Op*/
	IBVector m_TaskStack; /*Task*/
	IBVector m_StrReadPtrsStack; /*bool*/

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
void CompilerInit(Compiler* compiler);
void CompilerFree(Compiler* compiler);
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
Task* _GetTask(Compiler *compiler){
	Task* ret;
	ret = NULL;
	if (compiler->m_TaskStack.elemCount) {
		ret = (Task*)IBVectorTop(&compiler->m_TaskStack);
		assert(ret);
	}
	return ret;
}
#define GetTask _GetTask(compiler)
#define GetExpectsStack (GetTask ? &GetTask->expStack : NULL)
#define GetExpectsTop ((Expects*)IBVectorTop(GetExpectsStack))
#define GetMode *((Op*)IBVectorTop(&compiler->m_ModeStack))
#define GetTaskType (GetTask ? GetTask->type : OP_TaskStackEmpty)
#define GetTaskCode   (GetTask->code1)
#define GetTaskCodeP1 (GetTask->code2)
#define SetTaskType(tt) {\
	GetTask->type = tt;\
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
void owStr(char** str, char* with);

#ifndef IB_HEADER
char* CompilerStringModeIgnoreChars = "";
OpNamePair opNames[] = {
	{"null", OP_Null},{"no", OP_False},{"yes", OP_True},{"set", OP_Set},
	{"call", OP_Call},{"add", OP_SetAdd},{"func", OP_Func},{"~", OP_Comment},
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
	{"sys", OP_UseStrSysLib},{"thing", OP_Thing},{"SpaceNeedName",OP_SpaceNeedName},
	{"RootTask", OP_RootTask},{"ErrUnknownPfx",OP_ErrUnknownPfx},
	{"ErrUnexpectedNameOP",OP_ErrUnexpectedNameOP},{"ThingWantName",OP_ThingWantName},
	{"ThingWantContent",OP_ThingWantContent},{"SpaceHasName",OP_SpaceHasName},
	{"ErrDirtyTaskStack",OP_ErrDirtyTaskStack}
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
	{"extern", OP_Imaginary},
};
OpNamePair dbgAssertsNP[] = {
	{"taskType", OP_TaskType},{ "taskStack", OP_TaskStack },{"notEmpty", OP_NotEmpty}
};
char* SysLibCodeStr =
"@space $sys\n"
"@ext @func $malloc %i32 $size @ret %&?\n"
"@ext @func $realloc %&? $ptr %i32 $newSize @ret %&?\n"
"@ext @func $free %&? $ptr\n"
"@ext @func $strdup %&c8 $str @ret %&c8\n"
"@ext @func $strcat %&c8 $str1 %&c8 $str2 @ret %&c8\n"
"\n";

CLAMP_FUNC(int, ClampInt) CLAMP_IMP
CLAMP_FUNC(size_t, ClampSizeT) CLAMP_IMP
void IBStrInit(IBStr* str, size_t reserve){
	assert(reserve > 0);
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
	owStr(&str->start, nullTerminated);
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
	len = IBStrGetLen(str);
	/* assert(len < 8192); */
	withLen = strlen(with);
	if(!withLen) return str->start;
	assert(withLen > 0);
	assert(str->start);
	ra = realloc(str->start, len + withLen + 1);
	assert(ra);
	if (ra) {
		str->start = (char*)ra;
		strcat(str->start, with);
		str->end = str->start + len + withLen;
		return str->start;
	}else {
		assert(0);
		exit(-1);
	}
	return NULL;
}
void IBVectorInit(IBVector* vec, size_t elemSize) {
	void* m;
	vec->elemSize = elemSize;
	vec->elemCount = 0;
	vec->slotCount = 8;
	vec->protectedSlotCount = 0;
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
	if (!vec) return NULL;
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
		vec->data = ra;
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
IBVecData* IBVectorTop(IBVector* vec) {
	assert(vec);
	if (vec->elemCount <= 0) {
		__debugbreak();
		return NULL;
	}
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
	if (vec->protectedSlotCount && vec->elemCount <= vec->protectedSlotCount) {
		assert(0);
		exit(-1);
	}
	if(vec->elemCount <= 0) return;
	if(freeFunc) freeFunc((void*)IBVectorGet(vec, vec->elemCount - 1));
	vec->elemCount--;
	vec->slotCount=vec->elemCount;
	if(vec->slotCount<1)vec->slotCount=1;
	vec->dataSize = vec->elemSize * vec->slotCount;
	if(vec->elemCount){
		assert(vec->data);
		ra = realloc(vec->data, vec->dataSize);
		if (ra) vec->data = ra;
		assert(vec->data);
	}
}
void IBVectorPopFront(IBVector* vec, void(*freeFunc)(void*)){
	size_t newSize;
	void *ra;
	if(vec->elemCount < 1) return;
	if(vec->elemCount == 1){
		vec->elemCount--;
		return;
	}
	newSize=(vec->dataSize * vec->elemCount) - vec->dataSize;
	assert(newSize>=vec->dataSize);
	ra = malloc(newSize);
	assert(ra);
	if(ra){
		memcpy(ra, IBVectorGet(vec, 1), newSize);
		free(vec->data);
		vec->data = ra;
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
	IBVectorInit(&db->pairs, sizeof(NameInfo));
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
	IBVectorInit(&exp->pfxs, sizeof(Op));
	IBVectorInit(&exp->nameOps, sizeof(Op));
	exp->pfxErr=pfxErr;
	exp->nameOpErr=nameOpErr;
	exp->life=life;
	exp->lineNumInited=LINENUM;
#ifdef DEBUGPRINTS
	printf("Expect: { ");
#endif
	va_start(args, fmt);
	for (i = 0; i < strlen(fmt); i++) {
		char ch = fmt[i];
		switch (ch) {
		case 'P': {
			pfx=va_arg(args, Op);
			IBVectorCopyPushOp(&exp->pfxs, pfx);
#ifdef DEBUGPRINTS
			printf("Pfx:%s(%d) ", GetPfxName(pfx), (int)pfx);
#endif
			break;
		}
		case 'N':{
			nameOp=va_arg(args, Op);
			IBVectorCopyPushOp(&exp->nameOps, nameOp);
#ifdef DEBUGPRINTS
			printf("NameOP:%s(%d) ", GetOpName(nameOp), (int)nameOp);
#endif
			break;
		}
		}
	}
#ifdef DEBUGPRINTS
	printf("}\n");
#endif
	va_end(args);
}
void ExpectsPrint(Expects* ap) {
	Op* oi;
	int idx;
	idx = 0;
	printf(
#ifdef DEBUGPRINTS
		"[LN:%d] "
#endif
		"Prefix { ", ap->lineNumInited);
	while (oi = (Op*)IBVectorIterNext(&ap->pfxs, &idx))
		printf("%s(%d) ", GetPfxName(*oi), (int)*oi);
	printf("}\nNameOP { ");
	idx = 0;
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
	IBVectorInit(&t->working, sizeof(Obj));
	IBVectorInit(&t->expStack, sizeof(Expects));
	t->type = type;
	t->code1[0] = '\0';
	t->code2[0] = '\0';
}
void TaskFree(Task* t) {
	assert(t);
	IBVectorFree(&t->expStack, ExpectsFree);
	IBVectorFree(&t->working, ObjFree);
}
void _Err(Compiler *compiler, Op code, char *msg){
	printf("Error at %u:%u \"%s\"(%d). %s\n",
		compiler->m_Line, compiler->m_Column, GetOpName(code), (int)code, msg);
	CompilerExplainErr(compiler, code);
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
void owStr(char** str, char* with) {
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
	compiler->m_Privacy = OP_NotSet;
	compiler->m_NameOp = OP_Null;
	compiler->m_Ch = '\0';
	compiler->m_LastCh = '\0';
	compiler->m_StringMode = false;
	compiler->m_StrAllowSpace = false;
	compiler->m_CommentMode = OP_NotSet;
	compiler->inputStr = NULL;
	NameInfoDBInit(&compiler->m_NameTypeCtx);
	IBVectorInit(&compiler->m_ObjStack, sizeof(Obj));
	IBVectorInit(&compiler->m_ModeStack, sizeof(Op));
	IBVectorInit(&compiler->m_StrReadPtrsStack, sizeof(bool));
	IBVectorInit(&compiler->m_TaskStack, sizeof(Task));
	IBVectorCopyPushBool(&compiler->m_StrReadPtrsStack, false);
	CompilerPush(compiler, OP_ModePrefixPass, false);
	CompilerPushObj(compiler, &o);
	CompilerPushTask(compiler, OP_RootTask, &exp, NULL);
	ExpectsInit(exp, 0, "", "", "PNNNNN", OP_Op, OP_Use, OP_Imaginary, OP_Func, OP_Thing, OP_Space);
}
void CompilerFree(Compiler* compiler) {
	assert(compiler);
	if (compiler->m_StringMode)
		Err(OP_ErrNOT_GOOD, "Reached end of file without closing string");
	if(compiler->m_Str[0]) CompilerStrPayload(compiler);
	if(GetTaskType != OP_RootTask)Err(OP_ErrDirtyTaskStack,
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
	printf("-> Compilation complete <-\nC Header:\n%s%s\n\nC File:\n%s\n\n",
		compiler->m_CHeaderStructs.start, compiler->m_CHeaderFuncs.start, compiler->m_CFile.start);
	IBVectorFree(&compiler->m_ObjStack, ObjFree);
	IBVectorFreeSimple(&compiler->m_ModeStack);
	IBVectorFreeSimple(&compiler->m_StrReadPtrsStack);
	IBVectorFree(&compiler->m_TaskStack, TaskFree);
	NameInfoDBFree(&compiler->m_NameTypeCtx);
	IBStrFree(&compiler->m_CHeaderStructs);
	IBStrFree(&compiler->m_CHeaderFuncs);
	IBStrFree(&compiler->m_CFile);
}
void _CompilerPushTask(Compiler* compiler, Op taskOP, Expects** exectsDP, Task** taskDP) {
	Task* t;
	assert(exectsDP);
#ifdef DEBUGPRINTS
	printf(" Push task %s(%d)\n", GetOpName(taskOP),(int)taskOP);
#endif
	t = (Task*)IBVectorPush(&compiler->m_TaskStack);
	if(taskDP) (*taskDP) = t;
	TaskInit(t, taskOP);
	(*exectsDP) = (Expects*)IBVectorPush(&t->expStack);
}
void _CompilerPopTask(Compiler* compiler) {
	assert(compiler);
	assert(GetTask);
#ifdef DEBUGPRINTS
	printf(" Pop task %s(%d)\n", GetOpName(GetTaskType),(int)GetTaskType);
#endif
	IBVectorPop(&compiler->m_TaskStack, TaskFree);
}
void _CompilerPushObj(Compiler* compiler, Obj** o) {
	Obj *obj;
#ifdef DEBUGPRINTS
	printf(" Push obj: ");
#endif
	if (compiler->m_ObjStack.elemCount) {
#ifdef DEBUGPRINTS
		ObjPrint(CompilerGetObj(compiler));
		printf(" -> ");
#endif
	}
	obj = (Obj*)IBVectorPush(&compiler->m_ObjStack);
	ObjInit(obj);
#ifdef DEBUGPRINTS
	ObjPrint(obj);
	printf("\n");
#endif
	if (o) { (*o) = CompilerGetObj(compiler); }
}
void _CompilerPopObj(Compiler* compiler, bool pushToWorking, Obj** objDP) {
	Obj* o;
	assert(GetTask);
	o = CompilerGetObj(compiler);
	assert(o);
	if (pushToWorking){
		Obj* newObjMem;
		Task* t;
		if (GetObjType == OP_NotSet)Err(OP_ErrNOT_GOOD, "");
		t = GetTask;
		assert(t);
		assert(o);
#ifdef DEBUGPRINTS
		printf(" To working: ");
#endif
#ifdef DEBUGPRINTS
		ObjPrint(o);
		printf("\n");
#endif
		newObjMem=(Obj*)IBVectorPush(&t->working);
		assert(newObjMem);
		ObjCopy(newObjMem, o);
	}
#ifdef DEBUGPRINTS
	printf("Pop obj: ");
	ObjPrint(o);
#endif
	if (compiler->m_ObjStack.elemCount == 1) {
		ObjFree(o);
		ObjInit(o);
	}
	else {
		IBVectorPop(&compiler->m_ObjStack, ObjFree);
		o = CompilerGetObj(compiler);
	}
#ifdef DEBUGPRINTS
	printf(" -> ");
#endif
	assert(compiler->m_ObjStack.elemCount);
#ifdef DEBUGPRINTS
	ObjPrint(o);
	printf("\n");
#endif
	if(objDP) (*objDP) = o;
}
void _CompilerPush(Compiler* compiler, Op mode, bool strAllowSpace){
	compiler->m_StrAllowSpace = strAllowSpace;
	IBVectorCopyPushOp(&compiler->m_ModeStack, mode);
#ifdef DEBUGPRINTS
	printf(" push: to %s(%d)\n", GetOpName(GetMode), (int)GetMode);
#endif
}
void _CompilerPop(Compiler* compiler) {
	Op type=OP_Null;
	Task *t;
	t = GetTask;
	IBVectorPop(&compiler->m_ModeStack, NULL);
	if(t)type=t->type;
#ifdef DEBUGPRINTS
	printf(" pop: to %s(%d) Task type:%s(%d)\n", GetOpName(GetMode), (int)GetMode, GetOpName(type), (int)type);
#endif
	assert(t->expStack.elemCount);
#ifdef DEBUGPRINTS
	ExpectsPrint(GetExpectsTop);
#endif
}
Op ObjGetType(Obj* obj) { return obj->type; }
void _ObjSetType(Obj* obj, Op type) {
#ifdef DEBUGPRINTS
	printf(" obj type: %s(%d) -> %s(%d)\n",
		GetOpName(obj->type), (int)obj->type, GetOpName(type), (int)type);
#endif
	obj->type = type;
}
Op ObjGetMod(Obj* obj) { return obj->modifier; }
void ObjSetMod(Obj* obj, Op mod) {
#ifdef DEBUGPRINTS
	printf("obj mod: %s(%d) -> %s(%d)\n",
		GetOpName(obj->modifier), (int)obj->modifier, GetOpName(mod), (int)mod);
#endif
	obj->modifier = mod;
}
void _ObjSetName(Obj* obj, char* name) {
	assert(obj);
#ifdef DEBUGPRINTS
	printf(" obj name: %s -> %s\n", obj->name, name);
#endif
	owStr(&obj->name, name);
}
void ObjSetStr(Obj* obj, char* Str) {
#ifdef DEBUGPRINTS
	printf("obj str: %s -> %s\n", obj->str, Str);
#endif
	owStr(&obj->str, Str);
}
void ObjCopy(Obj* dst, Obj* src) {
	assert(dst && src);
	*dst=*src;
	dst->name = NULL;
	dst->str = NULL;
	if(src->name) owStr(&dst->name, src->name);
	if(src->str) owStr(&dst->str, src->str);
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
		/*if(u64)*/printf("Val:%d", obj->val.i32);
		printf("]");
	}
}
void CompilerPushExpects(Compiler* compiler, Expects **expDP){
	Task* t;
	Expects* exp;
	t = GetTask;
	assert(t);
	assert(expDP);
	if(expDP){
		assert(*expDP);
		exp = (Expects*)IBVectorPush(&t->expStack);
		assert(exp);
		(*expDP) = exp;
	}	
}
void CompilerPopExpects(Compiler* compiler) {
	IBVector* pfxsIb = &GetExpectsTop->pfxs;
	if (pfxsIb->elemCount) {
		Op* oi;
		int idx;

#ifdef DEBUGPRINTS
		printf(" Expects POP: { ");
		idx = 0;
		while (oi = (Op*)IBVectorIterNext(pfxsIb, &idx)) {
			printf("%s ", GetPfxName(*oi));
		}
		printf("} -> { ");
#endif
		IBVectorPop(GetExpectsStack, ExpectsFree);
		if (!GetExpectsStack->elemCount) Err(OP_ErrNOT_GOOD, "catastrophic failure");
		pfxsIb = &GetExpectsTop->pfxs;
#ifdef DEBUGPRINTS
		idx = 0;
		while (oi = (Op*)IBVectorIterNext(pfxsIb,&idx)) {
			printf("%s ", GetPfxName(*oi));
		}
		printf("}\n");
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
	t = GetTask;
	assert(t);
	assert(GetExpectsStack->elemCount);
	idx = 0;
	oi = NULL;
	ap = GetExpectsTop;
	if(ap) while (oi = (Op*)IBVectorIterNext(&ap->pfxs,&idx)) {
		assert(oi);
		if (oi && *oi == pfx)
			return true;
	}
	return false;
}
bool CompilerIsNameOpExpected(Compiler* compiler, Op nameOp)
{
	Op* oi;
	int idx;
	Task* t;
	Expects* exp;
	t = GetTask;
	assert(t);
	assert(GetExpectsStack->elemCount);
	idx = 0;
	oi = NULL;
	exp = GetExpectsTop;
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
	bool nl;
	compiler->m_Ch = ch;
	nl = false;
	m=GetMode;
	if(compiler->m_CommentMode==OP_NotSet&&
		compiler->m_Ch==COMMENT_CHAR/*&&
		compiler->m_LastCh!=COMMENT_CHAR*/)
	{
		/*PLINE;
		printf(" LINE COMMENT ON\n");*/
		compiler->m_CommentMode = OP_Comment;
		CompilerPush(compiler, OP_ModeComment, false);
	}else if(compiler->m_CommentMode==OP_Comment&&
			compiler->m_LastCh==compiler->m_Ch &&
				!compiler->m_StringMode
				&&compiler->m_Ch==COMMENT_CHAR&&
				m==OP_ModeComment)
	{
		/*PLINE;
		printf(" MULTI COMMENT ON!!!!!!\n");*/
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
		printf(" MULTI COMMENT OFF!\n");*/
		compiler->m_CommentMode=OP_NotSet;
	}
	switch (compiler->m_Ch) {
	case '\0': return;
	case '\n': {
		nl = true;
		if (compiler->m_CommentMode == OP_NotSet) {
#ifdef DEBUGPRINTS
			printf("Char():Line end\n");
#endif
		}
		if (compiler->m_CommentMode == OP_Comment) {
			CompilerPop(compiler);
			compiler->m_CommentMode = OP_NotSet;
		}
		switch (GetMode) {
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
		switch(GetTaskType){
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
					ExpectsInit(exp, 0,
						"expected operator, print statement, or variable declaration",
						"",
						"PPPN",
						OP_Op, OP_String, OP_VarType, OP_Done);
					SetTaskType(OP_FuncWantCode);
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
	m = GetMode;
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
	t = GetTask;
	assert(t);
#ifdef DEBUGPRINTS
	printf("PopAndDoTask()\n");
#endif
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
		char cFuncModsTypeName[CODE_STR_MAX];
		char cFuncArgsThing[CODE_STR_MAX];
		char cFuncArgs[CODE_STR_MAX];
		char cFuncArgsEnd[CODE_STR_MAX];
		char cFuncCode[CODE_STR_MAX];
		bool imaginary;
		Obj* funcObj;

		argc = 0;
		imaginary = false;
		cFuncModsTypeName[0] = '\0';
		cFuncArgs[0] = '\0';
		cFuncArgsThing[0] = '\0';
		cFuncArgsEnd[0] = '\0';
		cFuncCode[0] = '\0';
		funcObj = NULL;
		for (i = 0; i < wObjs->elemCount; ++i) {
			o = (Obj*)IBVectorGet(wObjs, i);
			switch (ObjGetType(o)) {
			case OP_FuncArgComplete: {/*multiple allowed*/
				Op at;
				at = o->arg.type;
				argc++;
				if (at == OP_Null)Err(OP_ErrNOT_GOOD, "arg type NULL");

				if (cFuncArgs[0] != '\0') {
					StrConcat(cFuncArgs, CODE_STR_MAX, ", ");
				}
				StrConcat(cFuncArgs, CODE_STR_MAX, GetCEqu(o->arg.type));
				StrConcat(cFuncArgs, CODE_STR_MAX, GetCEqu(o->arg.mod));
				StrConcat(cFuncArgs, CODE_STR_MAX, " ");
				if (!o->name)Err(OP_ErrNOT_GOOD, "arg name NULL");
				StrConcat(cFuncArgs, CODE_STR_MAX, o->name);
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
					StrConcat(cFuncModsTypeName, CODE_STR_MAX, GetCEqu(mod));
					StrConcat(cFuncModsTypeName, CODE_STR_MAX, " ");
				}
				StrConcat(cFuncModsTypeName, CODE_STR_MAX, GetCEqu(o->func.retType));
				StrConcat(cFuncModsTypeName, CODE_STR_MAX, GetCEqu(o->func.retTypeMod));
				StrConcat(cFuncModsTypeName, CODE_STR_MAX, " ");
				if (!o->name)Err(OP_ErrNOT_GOOD, "func name NULL");
				if (o->name) {
					if (o->func.thingHost) {
						//StrConcat(cFuncModsTypeName, CODE_STR_MAX, "ThingFUNC_");
						StrConcat(cFuncModsTypeName, CODE_STR_MAX, o->func.thingHost->name);
						StrConcat(cFuncModsTypeName, CODE_STR_MAX, "_");
					}
					StrConcat(cFuncModsTypeName, CODE_STR_MAX, o->name);
				}
				StrConcat(cFuncModsTypeName, CODE_STR_MAX, "(");
				if (o->func.thingHost) {
					StrConcat(cFuncArgsThing, CODE_STR_MAX, "struct ");
					StrConcat(cFuncArgsThing, CODE_STR_MAX, o->func.thingHost->name);
					StrConcat(cFuncArgsThing, CODE_STR_MAX, "* ptr");
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
				StrConcat(cFuncCode, CODE_STR_MAX, "\t");
				StrConcat(cFuncCode, CODE_STR_MAX, GetCEqu(o->var.type));
				StrConcat(cFuncCode, CODE_STR_MAX, GetCEqu(o->var.mod));
				StrConcat(cFuncCode, CODE_STR_MAX, " ");
				if (!o->name)Err(OP_ErrNOT_GOOD, "var name NULL");
				StrConcat(cFuncCode, CODE_STR_MAX, o->name);
				if (o->var.valSet) {
					StrConcat(cFuncCode, CODE_STR_MAX, " = ");
					sprintf(valBuf, "%d", o->var.val.i32);
					StrConcat(cFuncCode, 32, valBuf);
				}
				StrConcat(cFuncCode, CODE_STR_MAX, ";\n");
				break;
			}
			}
		}
		if (imaginary) {
			StrConcat(cFuncArgsEnd, CODE_STR_MAX, ");\n\n");
		}
		else {
			StrConcat(cFuncArgsEnd, CODE_STR_MAX, "){\n");
			StrConcat(cFuncCode, CODE_STR_MAX, GetTaskCodeP1);
			if(!funcObj)Err(OP_ErrNOT_GOOD, "funcObj NULL");
			if (funcObj->func.retType != OP_Void) {
				char valBuf[32];
				valBuf[0] = '\0';
				StrConcat(cFuncCode, CODE_STR_MAX, "\treturn ");
				Val2Str(valBuf, 32, funcObj->func.retVal, funcObj->func.retType);
				StrConcat(cFuncCode, 32, valBuf);
				StrConcat(cFuncCode, CODE_STR_MAX, ";\n");
			}
			StrConcat(cFuncCode, CODE_STR_MAX, "}\n\n");
		}
		if (strcmp(funcObj->name, "main"))
		{
			IBStrAppend(&compiler->m_CHeaderFuncs, cFuncModsTypeName);
			IBStrAppend(&compiler->m_CHeaderFuncs, cFuncArgsThing);
			if (argc && funcObj->func.thingHost) IBStrAppend(&compiler->m_CHeaderFuncs, ", ");
			IBStrAppend(&compiler->m_CHeaderFuncs, cFuncArgs);
			IBStrAppend(&compiler->m_CHeaderFuncs, ");\n");
		}
		if (!imaginary) {
			IBStrAppend(&compiler->m_CFile, cFuncModsTypeName);
			IBStrAppend(&compiler->m_CFile, cFuncArgsThing);
			if (argc && funcObj->func.thingHost) IBStrAppend(&compiler->m_CFile, ", ");
			IBStrAppend(&compiler->m_CFile, cFuncArgs);
			IBStrAppend(&compiler->m_CFile, cFuncArgsEnd);
			IBStrAppend(&compiler->m_CFile, cFuncCode);
		}
		break;
	}
	case OP_CPrintfHaveFmtStr: {
		bool firstPercent;
		Obj* fmtObj;
		int varIdx;
		int i;
		subTask = true;
		if (GetTask && wObjs->elemCount) {
			fmtObj = (Obj*)wObjs->data;
			StrConcat(GetTaskCode, CODE_STR_MAX, "\tprintf(\"");
			firstPercent = false;
			varIdx = 1;
			for (i = 0; i < (int)strlen(fmtObj->str); ++i) {
				char c;
				c = fmtObj->str[i];
				switch (c) {
				case '%':{
						if (!firstPercent) {
							StrConcat(GetTaskCode, CODE_STR_MAX, "%");
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
								StrConcat(GetTaskCode, CODE_STR_MAX,
									CompilerGetCPrintfFmtForType(compiler, type));
								break;
							}
							case OP_String:
								assert(vo->var.type==OP_String);
							case OP_Value:{
								StrConcat(GetTaskCode, CODE_STR_MAX,
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
					StrConcat(GetTaskCode, CODE_STR_MAX, chBuf);
					break;
				}
				}
			}
			StrConcat(GetTaskCode, CODE_STR_MAX, "\"");
			if (wObjs->elemCount > 1) {
				StrConcat(GetTaskCode, CODE_STR_MAX, ", ");
			}
			for (i = 1; i < wObjs->elemCount; ++i) {
				Obj* o;
				o = (Obj*)IBVectorGet(wObjs, i);
				switch (ObjGetType(o)) {
				case OP_Name: {
					StrConcat(GetTaskCode, CODE_STR_MAX, o->name);
					break;
				}
				case OP_String: {
					StrConcat(GetTaskCode, CODE_STR_MAX, "\"");
					StrConcat(GetTaskCode, CODE_STR_MAX, o->str);
					StrConcat(GetTaskCode, CODE_STR_MAX, "\"");
					break;
				}
				case OP_Value: {
					char valBuf[32];
					valBuf[0] = '\0';
					Val2Str(valBuf, 32, o->val, o->var.type);
					StrConcat(GetTaskCode, 32, valBuf);
				}
				}
				if (i < wObjs->elemCount - 1) {
					StrConcat(GetTaskCode, CODE_STR_MAX, ", ");
				}
			}
			StrConcat(GetTaskCode, CODE_STR_MAX, ");\n");
		}
		break;
	}
	}
	if (subTask) {
		switch (GetTaskType) {
		case OP_CPrintfHaveFmtStr: {
			if (compiler->m_TaskStack.elemCount - 2 >= 0) {
				char theCode[CODE_STR_MAX];
				theCode[0] = '\0';
				strcpy(theCode, GetTaskCode);
				CompilerPopTask(compiler);
				switch (GetTaskType) {
				case OP_FuncWantCode: {
					StrConcat(GetTaskCodeP1, CODE_STR_MAX, theCode);
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
		&& (!GetTask || GetExpectsTop->pfxs.elemCount)
		&& !CompilerIsPfxExpected(compiler, compiler->m_Pfx)
	)
	{
		Err(OP_ErrUnexpectedNextPfx, "");
	}
#ifdef DEBUGPRINTS
	printf("PFX:%s(%d)\n", GetPfxName(compiler->m_Pfx), (int)compiler->m_Pfx);
#endif
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
		Expects* aps=NULL;
		Task* t;
		t = GetTask;
		if(t) aps = GetExpectsTop;
		if (aps && aps->life && --aps->life <= 0) {
			IBVectorPop(GetExpectsStack, ExpectsFree);
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
#ifdef DEBUGPRINTS
					printf("Got pointer\n");
#endif
					compiler->m_Pointer = OP_Pointer;
					break;
				case OP_Pointer:
#ifdef DEBUGPRINTS
					printf("Got double pointer\n");
#endif
					compiler->m_Pointer = OP_DoublePointer;
					break;
				case OP_DoublePointer:
#ifdef DEBUGPRINTS
					printf("Got tripple pointer\n");
#endif
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
void CompilerStrPayload(Compiler* compiler){
	Val strVal;
	Task *t;
	t=NULL;
	t=GetTask;
	strVal.i32=atoi(compiler->m_Str);
	compiler->m_NameOp = GetOpFromName(compiler->m_Str);
#ifdef DEBUGPRINTS
	printf("StrPayload: %s\n", compiler->m_Str);
#endif
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
			o=CompilerGetObj(compiler);
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
			switch (GetTaskType) {
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
#ifdef DEBUGPRINTS
						printf("Finishing func got ret value\n");
#endif
						o->func.retVal = strVal;
						PopExpects();
						SetTaskType(OP_Func);
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
		switch (GetTaskType) {
		case OP_ThingWantContent: {
			Obj* o;
			Expects* exp;
			CompilerPushObj(compiler, &o);
			o->var.type = compiler->m_NameOp;
			o->var.mod = compiler->m_Pointer;
			o->var.privacy=compiler->m_Privacy;
			compiler->m_Privacy = OP_NotSet;
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
			if (GetTaskType != OP_FuncHasName)Err(OP_ErrNOT_GOOD, "func signature needs name");
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
			assert(GetObjCount == 1);
			assert(GetObjType == OP_NotSet);
			CompilerPushObj(compiler, &o);
			assert(compiler->m_Str[0]!='\0');
			ObjSetName(o, compiler->m_Str);
			ObjSetType(o, OP_Thing);
			t->type = OP_ThingWantContent;
			CompilerPushExpects(compiler, &exp);
			ExpectsInit(exp, 0, "expected vartype (%)", "expected @pub, @priv, or @func", 
				"PPNNNN", 
				OP_Op, OP_VarType, OP_Func, OP_Public, OP_Private, OP_Done);
			break;
		}
		case OP_UseNeedStr: {
			Op lib;
			lib = compiler->m_NameOp;
			switch (lib) {
			case OP_UseStrSysLib: {
#ifdef DEBUGPRINTS
				printf("Inputting system lib code to compiler\n");
#endif
				/*CompilerInputStr(compiler, SysLibCodeStr);*/
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
			SetTaskType(OP_FuncHasName);
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
		case OP_Space: {
			Expects* ap;
			CompilerPushTask(compiler, OP_SpaceNeedName, &ap, NULL);
			ExpectsInit(ap, 0, "expected space name", "", "P", OP_Name);
			break;
		}
		case OP_Thing: {
			Expects* ap;
			CompilerPushTask(compiler, OP_ThingWantName, &ap, NULL);
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
			switch (GetTaskType) {
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
#ifdef DEBUGPRINTS
				PLINE;
				printf(" Finishing function\n");
#endif
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
							SetTaskType(OP_FuncNeedRetVal);
						}
						else {
							SetTaskType(OP_Func);
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
#ifdef DEBUGPRINTS
				printf("what\n");
#endif
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
			t = GetTask;
			CompilerPushObj(compiler, &o);
			if (t->type == OP_ThingWantContent) {
				bool thingFound;
				Obj* wo;
				int idx;
				thingFound = false;
				idx = 0;
				while (wo = (Obj*)IBVectorIterNext(&compiler->m_ObjStack, &idx)) {
					if (wo->type == OP_Thing) {
						thingFound = true;
						o->func.thingHost = wo;
					}
				}
				assert(thingFound);
			}
			else {
				o->func.thingHost = NULL;
			}
			CompilerPushTask(compiler, OP_FuncNeedName, &ap, NULL);
			ExpectsInit(ap, 0, "expected function name", "", "P", OP_Name);
			o->type = compiler->m_NameOp;
			o->privacy = compiler->m_Privacy;
			compiler->m_Privacy = OP_NotSet;
			CompilerGetObj(compiler)->func.retType = OP_Void;
			CompilerGetObj(compiler)->func.retTypeMod = OP_NotSet;
			break;
		}
		case OP_Public:
		case OP_Private:
			//assert(compiler->m_Privacy == OP_NotSet);//wasnt reset
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
	if(compiler->m_StrReadPtrsStack.elemCount > 1)
	{
		if (*(bool*)IBVectorTop(&compiler->m_StrReadPtrsStack)) {
			compiler->m_Pointer = OP_NotSet;
		}
		IBVectorPop(&compiler->m_StrReadPtrsStack, NULL);
	}
}
void CompilerExplainErr(Compiler* compiler, Op code) {
	switch (code) {
	case OP_ErrUnexpectedNameOP: {
		Expects* exp;
		Task* t;
		t=GetTask;
		assert(t);
		exp = GetExpectsTop;
		printf("NameOP \"@%s\" wasn't expected.\nExpected:\n", compiler->m_Str);
		ExpectsPrint(exp);
		break;
	}
	case OP_ErrUnknownPfx:
		printf("This prefix \"%c\" is unknown!", compiler->m_Pfx);
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
		t =GetTask;
		ap=GetExpectsTop;
		if(ap && t){
			assert(ap->pfxs.elemCount);
			printf("Err: \"%s\" Unexpected next prefix %s. Pfx idx:%d\nEnforced at line %d. Allowed:",
				ap->pfxErr, GetPfxName(compiler->m_Pfx),
					ap->pfxs.elemCount - 1, ap->lineNumInited);
			idx = 0;
			while (oi = (Op*)IBVectorIterNext(&GetExpectsTop->pfxs,&idx)) {
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
	printf("OBJ:");
	ObjPrint(CompilerGetObj(compiler));
	printf("\n");
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
#ifdef DEBUGPRINTS
		printf("Exiting\n");
#endif
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
