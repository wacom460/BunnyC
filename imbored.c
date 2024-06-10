#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#define bool char
#define true 1
#define false 0

//not actually a compiler
//ascii only, maybe utf8 later...
//transpile to C99
//no order of operations, sequential ONLY
//compiler options inside source code, preferably using code
//in number order breakpoints, if hit in the wrong order or missing then failure

#define PRINT_LINE_INFO() printf("LINE:%d", __LINE__)
#define OBJ_NAME_LEN 64
#define OP_NAME_LEN 32
#define COMMENT_CHAR ('~')
#define CODE_STR_MAX 1024
#define CompilerSTR_MAX 256
#ifdef __TINYC__
#define __debugbreak() assert(0)
#endif

typedef enum Op { //multiple uses
	OP_Null, OP_False, OP_True, OP_Unknown, OP_NotSet, OP_Any, OP_Use, OP_Build, OP_Space,

	OP_Func, OP_FuncHasName, OP_FuncNeedName, OP_FuncNeedsRetValType,
	OP_FuncArgsVarNeedsName, OP_FuncArgNameless, OP_FuncArgComplete,
	OP_FuncWantCode, OP_FuncSigComplete, OP_FuncNeedRetVal, OP_FuncArg,
	OP_CompletedFunction,

	OP_VarNeedName, OP_VarWantValue, OP_VarComplete,
	OP_CallNeedName, OP_CallWantArgs, OP_CallComplete,	

	OP_Op, OP_Value, OP_Done, OP_Return, OP_NoChange, OP_Struct, OP_VarType, OP_LineEnd,
	OP_Comment, OP_MultiLineComment, OP_Public, OP_Private, OP_Imaginary, OP_Void,
	OP_Set, OP_SetAdd, OP_Call, OP_Colon, OP_Dot, OP_Add, OP_Subtract, OP_Multiply, OP_Divide,
	OP_AddEq, OP_SubEq, OP_MultEq, OP_DivEq, OP_Equals, OP_NotEquals, OP_LessThan,
	OP_GreaterThan, OP_LessThanOrEquals, OP_GreaterThanOrEquals,
	OP_ScopeOpen, OP_ScopeClose, OP_ParenthesisOpen, OP_ParenthesisClose,
	OP_BracketOpen, OP_BracketClose, OP_SingleQuote, OP_DoubleQuote,
	OP_CPrintfHaveFmtStr,OP_TaskStackEmpty,OP_RootTask,

	OP_SpaceChar, OP_Comma, OP_CommaSpace, OP_Name, OP_String,
	OP_CPrintfFmtStr, OP_Char, OP_If, OP_Else, OP_For, OP_While,
	OP_Block, OP_c8, OP_u8, OP_u16, OP_u32, OP_u64, OP_i8, OP_i16,
	OP_i32, OP_i64, OP_f32, OP_d64, OP_Pointer, OP_DoublePointer,
	OP_TripplePointer, OP_CompilerFlags, OP_dbgBreak,

	OP_NotFound, OP_Error, OP_ErrNOT_GOOD, OP_ErrUnexpectedNextPfx,
	OP_ErrExpectedVariablePfx, OP_ErrNoTask, OP_ErrUnexpectedOp,
	OP_ErrQuadriplePointersNOT_ALLOWED, OP_ErrUnknownOpStr,
	OP_ErrProtectedSlot,

	OP_ModePrefixPass, OP_ModeStrPass, OP_ModeComment, OP_ModeMultiLineComment,
} Op;
int ClampInt(int val, int min, int max) {
	if (val < min) return min;
	if (val > max) return max;
	return val;
}
size_t ClampSizeT(size_t val, size_t min, size_t max) {
	if (val < min) return min;
	if (val > max) return max;
	return val;
}
typedef union IBVecData {
	union IBVecData* data;
	struct Obj* obj;
	struct Task* task;
	Op* op;
	int* boolean;
	struct AllowedPfxs* apfxs;
	struct NameInfoDB* niDB;
} IBVecData;
typedef struct IBLLNode {
	struct IBLLNode* prev;
	struct IBLLNode* next;
	IBVecData;//data ptr
} IBLLNode;
typedef struct IBVector {
	size_t elemSize;
	int elemCount;
	int slotCount;
	int protectedSlotCount;//cant pop past this, if 0 then unaffecting
	size_t dataSize;
	IBLLNode* start;
	IBLLNode* end;
	IBVecData;//DATA BLOCK
} IBVector;
void IBVectorInit(IBVector* vec, size_t elemSize) {
	vec->elemSize = elemSize;
	vec->elemCount = 0;
	vec->slotCount = 1;
	vec->protectedSlotCount = 0;
	vec->dataSize = vec->elemSize * vec->slotCount;
	vec->data = malloc(vec->dataSize);
	assert(vec->data);
	memset(vec->data, 0, vec->dataSize);
	vec->start = malloc(vec->slotCount * sizeof(IBLLNode));
	vec->end = vec->start;
	assert(vec->start);
	memset(vec->start, 0, vec->slotCount * sizeof(IBLLNode));
}
IBVecData* IBVectorGet(IBVector* vec, int idx) {
	if (idx >= vec->elemCount) return NULL;
	return (IBVecData*)((char*)vec->data + vec->elemSize * idx);
}
void* IBVectorIterNext(IBVector* vec, int* idx) {
	if (!vec) return NULL;
	if ((*idx) >= vec->elemCount) return NULL;
	return (char*)vec->data + vec->elemSize * (*idx)++;
}
IBVecData* IBVectorPush(IBVector* vec) {
	IBVecData* topPtr;
	assert(vec->elemCount <= vec->slotCount);
	if (vec->elemCount >= vec->slotCount) {
		void* ra;
		vec->slotCount++;
		vec->dataSize = vec->elemSize * vec->slotCount;
		assert(vec->data);
		ra = realloc(vec->data, vec->dataSize);
		assert(ra);
		vec->data = ra;
		ra = realloc(vec->start, vec->slotCount * sizeof(IBLLNode));
		assert(ra);
		vec->start = ra;
	}
	topPtr = (IBVecData*)((char*)vec->data + vec->elemSize * vec->elemCount);
	if (vec->elemCount > 0) {
		vec->start[vec->elemCount - 1].next = &vec->start[vec->elemCount];
		vec->start[vec->elemCount].prev = &vec->start[vec->elemCount - 1];
	}
	else vec->start[vec->elemCount].prev = NULL;
	vec->start[vec->elemCount].next = NULL;
	vec->start[vec->elemCount].data = topPtr;
	vec->end = &vec->start[vec->elemCount];
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
	if (vec->elemCount <= 0) return NULL;
	return (IBVecData*)((char*)vec->data + (vec->elemCount - 1) * vec->elemSize);
}
IBVecData* IBVectorFront(IBVector* vec) {
	if (vec->elemCount <= 0) return NULL;
	return vec->data;
}
void IBVectorPop(IBVector* vec) {
	if (vec->protectedSlotCount && vec->elemCount <= vec->protectedSlotCount) {
		assert(0);
		return;
	}
	void* ra;
	if(vec->elemCount <= 0) return;
	vec->elemCount--;
	vec->slotCount = ClampInt(vec->slotCount, 1, vec->elemCount);
	vec->dataSize = vec->elemSize * vec->slotCount;
	assert(vec->data);
	ra = realloc(vec->data, vec->dataSize);
	if (ra) vec->data = ra;
	assert(vec->data);
	vec->end = &vec->start[vec->elemCount - 1];
	vec->end->next = NULL;
}
void IBVectorFreeSimple(IBVector* vec) {
	free(vec->start);
	free(vec->data);
}
#define IBVectorFree(vec, freeFunc){\
	int i;\
	i = 0;\
	for(;i<(vec)->elemCount;i++){\
		freeFunc((void*)IBVectorGet((vec), i));\
	}\
	IBVectorFreeSimple((vec));\
}

char* StrConcat(char* dest, int count, const char* src) {
	return strcat(dest, src);
}
typedef union Val {
	unsigned char u8;
	unsigned short u16;
	unsigned int u32;
	unsigned __int64 u64;
	char i8;
	char c8;
	short i16;
	int i32;
	__int64 i64;
	float f32;
	double d64;
} Val;
typedef struct NameInfo {
	Op type;
	const char* name;
} NameInfo;
void NameInfoInit(NameInfo* info){
	info->type=OP_NotSet;
	info->name=NULL;
}
typedef struct NameInfoDB {
	IBVector pairs;	
} NameInfoDB;
void NameInfoDBInit(NameInfoDB* db) {
	IBVectorInit(&db->pairs, sizeof(NameInfo));
}
void NameInfoDBAdd(NameInfoDB* db, const char* name, Op type) {
	NameInfo info;
	info.type = type;
	info.name = _strdup(name);
	IBVectorCopyPush(&db->pairs, &info);
}
Op NameInfoDBFindType(NameInfoDB* db, const char* name) {
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
	IBVectorFreeSimple(&db->pairs);
}
typedef struct FuncObj {
	Val retVal;
	Op retType;
	Op retTypeMod;
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
} VarObj;
const char* GetOpName(Op op);
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
const Op ObjGetType(Obj* obj);
void ObjSetType(Obj* obj, Op type);
Op ObjGetMod(Obj* obj);
void ObjSetMod(Obj* obj, Op mod);
void ObjSetName(Obj* obj, const char* name);
void ObjSetStr(Obj* obj, const char* Str);
void ObjPrint(Obj* obj);
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
typedef struct AllowedPfxs {
	IBVector pfxs;//Op
	const char* err;
	int life;
} AllowedPfxs;
void AllowedPfxsInit(AllowedPfxs* ap, int count, ...) {
	va_list args;
	Op o;
	IBVectorInit(&ap->pfxs, sizeof(Op));
	ap->err=NULL;
	ap->life=0;
	va_start(args, count);
	while (count--) {
		o = va_arg(args, Op);
		IBVectorCopyPushOp(&ap->pfxs, o);
	}
	va_end(args);
}
void AllowedPfxsFree(AllowedPfxs* ap) {
	IBVectorFreeSimple(&ap->pfxs);
}
typedef struct Task {
	Op type;
	IBVector apfxsStack; //AllowedPfxs
	IBVector working;//Obj
	char code1[CODE_STR_MAX];
	char code2[CODE_STR_MAX];
} Task;
void TaskInit(Task* t, Op type) {
	AllowedPfxs* ap;
	IBVectorInit(&t->working, sizeof(Obj));
	IBVectorInit(&t->apfxsStack, sizeof(AllowedPfxs));
	t->apfxsStack.protectedSlotCount = 1;
	ap = (AllowedPfxs*)IBVectorPush(&t->apfxsStack);
	AllowedPfxsInit(ap, 1, OP_Op);
	//IBVectorPop(&t->apfxsStack);//should fail
	t->type = type;
	t->code1[0] = '\0';
	t->code2[0] = '\0';
}
void TaskFree(Task* t) {
	IBVectorFree(&t->apfxsStack, AllowedPfxsFree);
}
typedef struct Compiler {
	int m_Line;
	int m_Column;
	Op m_Pfx;
	char m_Str[CompilerSTR_MAX];
	char m_cOutput[CODE_STR_MAX];

	IBVector m_ObjStack; //Obj
	IBVector m_ModeStack; //Op
	IBVector m_TaskStack; //Task
	IBVector m_StrReadPtrsStack; //bool

	Op m_Pointer;
	Op m_NameOp;
	char m_Ch;
	bool m_StringMode;
	bool m_StrAllowSpace;
	Op m_CommentMode;
	int m_MultiLineOffCount;
	NameInfoDB m_NameTypeCtx;	
} Compiler;
Obj* CompilerGetObj(Compiler* compiler);
void CompilerInit(Compiler* compiler);
void CompilerFree(Compiler* compiler);
void CompilerPushTask(Compiler* compiler, Op task);
void CompilerPopTask(Compiler* compiler);
Obj* CompilerPushObj(Compiler* compiler);
Obj* CompilerPopObj(Compiler* compiler, bool pushToWorking);
void CompilerPush(Compiler* compiler, Op mode, bool strAllowSpace);
Op CompilerPop(Compiler* compiler);
//life:0 = infinite, -1 life each pfx
//void pushAllowedNextPfxs(std::vector<Op> allowedNextPfxs, const char* err, int life);
void CompilerPushAllowedPfxs(Compiler* compiler, int life, const char* err, int count, ...);
void CompilerPopAllowedNextPfxs(Compiler* compiler);
bool CompilerIsPfxExpected(Compiler* compiler, Op pfx);
//NO NEWLINES AT END OF STR
void CompilerChar(Compiler* compiler, char ch);
void CompilerPopAndDoTask(Compiler* compiler);
const char* CompilerGetCPrintfFmtForType(Compiler* compiler, Op type);
void CompilerPrefix(Compiler* compiler);
void CompilerStr(Compiler* compiler);
void CompilerStrPayload(Compiler* compiler);
void CompilerExplainErr(Compiler* compiler, Op code);
#define Err(code, msg){\
	PRINT_LINE_INFO();\
	printf(":%s At %u:%u \"%s\"(%d)\nExplanation: ", msg, compiler->m_Line, compiler->m_Column, GetOpName(code), (int)code);\
	CompilerExplainErr(compiler, code);\
	printf("\n");\
	__debugbreak();\
}
#define SetObjType(type){\
	PRINT_LINE_INFO();\
	ObjSetType(CompilerGetObj(compiler), type);\
}
#define GetObjType (ObjGetType(CompilerGetObj(compiler)))
#define PushPfxs(pfxs, msg, life){\
	PRINT_LINE_INFO();\
	pushAllowedNextPfxs(pfxs, msg, life);\
}
#define PopPfxs(){\
	PRINT_LINE_INFO();\
	CompilerPopAllowedNextPfxs(compiler);\
}
#define GetTask ((Task*)IBVectorTop(&compiler->m_TaskStack))
#define GetAPfxsStack (&GetTask->apfxsStack)
#define GetAllowedPfxsTop ((AllowedPfxs*)IBVectorTop(GetAPfxsStack))
#define GetMode *((Op*)IBVectorTop(&compiler->m_ModeStack))
#define GetTaskType   ((compiler->m_TaskStack.elemCount) ? GetTask->type : OP_TaskStackEmpty)
#define GetTaskCode   (GetTask->code1)
#define GetTaskCodeP1 (GetTask->code2)
#define SetTaskType(tt) {\
	PRINT_LINE_INFO();\
	printf(" SetTaskType: %s(%d) -> %s(%d)\n", GetOpName(GetTaskType), (int)GetTaskType, GetOpName(tt), (int)tt);\
	GetTask->type = tt;\
}
#define GetTaskWorkingObjs (GetTask ? (&GetTask->working) : NULL)
typedef struct OpNamePair {
	char name[OP_NAME_LEN];
	Op op;
} OpNamePair;
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
	{"pub", OP_Public},{"void", OP_Void},{"c8", OP_c8},{"u8", OP_u8},{"u16", OP_u16},
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
	{"dbgBreak", OP_dbgBreak},{"CallNeedName",OP_CallNeedName},
	{"CallWantArgs", OP_CallWantArgs},{"CallComplete", OP_CallComplete},
	{"TaskStackEmpty", OP_TaskStackEmpty}, {"CPrintfFmtStr", OP_CPrintfFmtStr},
};
OpNamePair pfxNames[] = {
	{"NULL", OP_Null},{"Value(=)", OP_Value},{"Op(@)", OP_Op},
	{"Comment(~)", OP_Comment},{"Name($)", OP_Name},
	{"VarType(%)", OP_VarType},{"Pointer(&)", OP_Pointer},
	{"Return(@ret)", OP_Return},{"OP_Unknown", OP_Unknown},
	{"String(\")", OP_String},{"LineEnd(\\n)", OP_LineEnd},
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
const char* GetCEqu(Op op) {
	int sz=sizeof(cEquivelents)/sizeof(cEquivelents[0]);
	for (int i = 0; i < sz; i++) {
		if (op == cEquivelents[i].op) return cEquivelents[i].name;
	}
	return "?";
}
const char* GetOpName(Op op) {
	int sz = sizeof(opNames) / sizeof(opNames[0]);
	for (int i = 0; i < sz; i++) {
		if (op == opNames[i].op) return opNames[i].name;
	}
	return "?";
}
const char* GetPfxName(Op op) {
	int sz = sizeof(pfxNames) / sizeof(pfxNames[0]);
	for (int i = 0; i < sz; i++) {
		if (op == pfxNames[i].op) return pfxNames[i].name;
	}
	return "?";
}
Op GetOpFromName(const char* name) {
	int sz = sizeof(opNames) / sizeof(opNames[0]);
	for (int i = 0; i < sz; i++) {
		if (!strcmp(opNames[i].name, name)) return opNames[i].op;
	}
	return OP_Error;
}
Op fromPfxCh(char ch) {
	switch (ch) {
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
void owStr(char** str, const char* with) {
	if (*str) free(*str);
	*str = _strdup(with);
}
Obj* CompilerGetObj(Compiler* compiler) {
	return (Obj*)IBVectorTop(&compiler->m_ObjStack);
}
void CompilerInit(Compiler* compiler){
	Obj* o;
	Task* rootTask;
	compiler->m_Line = 1;
	compiler->m_Column = 1;
	compiler->m_Pfx = OP_Null;
	compiler->m_Str[0] = '\0';
	compiler->m_cOutput[0] = '\0';
	compiler->m_Pointer = OP_NotSet;
	compiler->m_NameOp = OP_Null;
	compiler->m_Ch = '\0';
	compiler->m_StringMode = false;
	compiler->m_StrAllowSpace = false;
	compiler->m_CommentMode = OP_NotSet;
	compiler->m_MultiLineOffCount = 0;
	NameInfoDBInit(&compiler->m_NameTypeCtx);
	IBVectorInit(&compiler->m_ObjStack, sizeof(Obj));
	IBVectorInit(&compiler->m_ModeStack, sizeof(Op));
	IBVectorInit(&compiler->m_StrReadPtrsStack, sizeof(bool));
	IBVectorInit(&compiler->m_TaskStack, sizeof(Task));
	compiler->m_TaskStack.protectedSlotCount = 1;
	rootTask = (Task*)IBVectorPush(&compiler->m_TaskStack);
	TaskInit(rootTask, OP_RootTask);
	IBVectorCopyPushBool(&compiler->m_StrReadPtrsStack, false);
	CompilerPush(compiler, OP_ModePrefixPass, false);
	o=CompilerPushObj(compiler);
}
void CompilerFree(Compiler* compiler) {
	if (compiler->m_StringMode)Err(OP_ErrNOT_GOOD, "Reached end of file without closing string");
	if(compiler->m_Str[0]) CompilerStrPayload(compiler);
	if (compiler->m_TaskStack.elemCount) {
		switch (((Task*)IBVectorTop(&compiler->m_TaskStack))->type) {
		case OP_FuncNeedRetVal:
			Err(OP_ErrNOT_GOOD, "Reached end of file without closing function");
			break;
		case OP_FuncSigComplete:
		case OP_FuncHasName: {
			SetObjType(OP_FuncSigComplete);
			CompilerPopObj(compiler, true);
			CompilerPopAndDoTask(compiler);
			break;
		}
		}
	
	}
	printf("-> Compilation complete <-\nResulting C code:\n\n");
	printf("%s", compiler->m_cOutput);
}
void CompilerPushTask(Compiler* compiler, Op task) {
	Task t;
	printf("Push task %s(%d)\n", GetOpName(task),(int)task);
	TaskInit(&t, task);
	IBVectorCopyPush(&compiler->m_TaskStack, &t);
}
void CompilerPopTask(Compiler* compiler) {
	printf("Pop task %s(%d)\n", GetOpName(GetTaskType),(int)GetTaskType);
	IBVectorPop(&compiler->m_TaskStack);
}
Obj* CompilerPushObj(Compiler* compiler) {
	Obj obj;
	ObjInit(&obj);
	printf("Push obj: ");
	if (compiler->m_ObjStack.elemCount) {
		ObjPrint(CompilerGetObj(compiler));
		printf(" -> ");
	}
	IBVectorCopyPush(&compiler->m_ObjStack, &obj);
	ObjPrint(CompilerGetObj(compiler));
	printf("\n");
	return CompilerGetObj(compiler);
}
Obj* CompilerPopObj(Compiler* compiler, bool pushToWorking) {
	if (pushToWorking){
		if (GetObjType == OP_NotSet)Err(OP_ErrNOT_GOOD, "");
		printf("To working: ");
		ObjPrint(CompilerGetObj(compiler));
		printf("\n");
		IBVectorCopyPush(GetTaskWorkingObjs, CompilerGetObj(compiler));
	}
	printf("Pop obj: ");
	ObjPrint(CompilerGetObj(compiler));
	if (compiler->m_ObjStack.elemCount == 1) {
		Obj* o;
		o=CompilerGetObj(compiler);
		if (o->name)free(o->name);
		if (o->str)free(o->str);
		ObjInit(CompilerGetObj(compiler));
	}
	else {
		IBVectorPop(&compiler->m_ObjStack);
	}
	printf(" -> ");
	ObjPrint(CompilerGetObj(compiler));
	printf("\n");
	return CompilerGetObj(compiler);
}
void CompilerPush(Compiler* compiler, Op mode, bool strAllowSpace){
	compiler->m_StrAllowSpace = strAllowSpace;
	IBVectorCopyPushOp(&compiler->m_ModeStack, mode);
	printf("push: to %s(%d)\n", GetOpName(GetMode), (int)GetMode);
}
Op CompilerPop(Compiler* compiler) {
	IBVectorPop(&compiler->m_ModeStack);
	printf("pop: to %s(%d)\n", GetOpName(GetMode), (int)GetMode);
	return GetMode;
}
const Op ObjGetType(Obj* obj) { return obj->type; }
void ObjSetType(Obj* obj, Op type) {
	printf(" obj type: %s(%d) -> %s(%d)\n", GetOpName(obj->type), (int)obj->type, GetOpName(type), (int)type);
	obj->type = type;
}
Op ObjGetMod(Obj* obj) { return obj->modifier; }
void ObjSetMod(Obj* obj, Op mod) {
	printf("obj mod: %s(%d) -> %s(%d)\n", GetOpName(obj->modifier), (int)obj->modifier, GetOpName(mod), (int)mod);
	obj->modifier = mod;
}
void ObjSetName(Obj* obj, const char* name) {
	printf("obj name: %s -> %s\n", obj->name, name);
	owStr(&obj->name, name);
}
void ObjSetStr(Obj* obj, const char* Str) {
	printf("obj str: %s -> %s\n", obj->str, Str);
	owStr(&obj->str, Str);
}
void ObjPrint(Obj* obj) {
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
void CompilerPushAllowedPfxs(Compiler* compiler, int life, const char* err, int count, ...)
{
	Op o;
	Op* oi;
	va_list args;
	AllowedPfxs *ap;
	int idx;
	va_start(args, count);
	printf(" apfxs PUSH: { ");
	idx = 0;
	assert(GetAllowedPfxsTop->pfxs.elemCount);
	while (oi = (Op*)IBVectorIterNext(&GetAllowedPfxsTop->pfxs, &idx))
		printf("%s ", GetPfxName(*oi));
	printf("} -> { ");
	ap = (AllowedPfxs*)IBVectorPush(GetAPfxsStack);
	AllowedPfxsInit(ap, 0);
	while (count--) {
		o = va_arg(args, Op);
		IBVectorCopyPushOp(&ap->pfxs, o);
		printf("%s ", GetPfxName(o));
	}
	printf("}\n");
}
void CompilerPopAllowedNextPfxs(Compiler* compiler) {
	IBVector* pfxsIb = &GetAllowedPfxsTop->pfxs;
	if (pfxsIb->elemCount) {
		Op* oi;
		int idx;

		printf(" allowed pfxs POP: { ");
		idx = 0;
		while (oi = (Op*)IBVectorIterNext(pfxsIb, &idx)) {
			printf("%s ", GetPfxName(*oi));
		}
		printf("} -> { ");
		IBVectorPop(GetAPfxsStack);
		if (!GetAPfxsStack->elemCount) Err(OP_ErrNOT_GOOD, "catastrophic failure");
		pfxsIb = &GetAllowedPfxsTop->pfxs;
		idx = 0;
		while (oi = (Op*)IBVectorIterNext(pfxsIb,&idx)) {
			printf("%s ", GetPfxName(*oi));
		}
		printf("}\n");
	}
}
bool CompilerIsPfxExpected(Compiler* compiler, Op pfx) {
	Op* oi;
	int idx;
	idx = 0;
	while (oi = (Op*)IBVectorIterNext(&GetAllowedPfxsTop->pfxs,&idx)) {
		if (*oi == pfx) return true;
	}
	return false;
}
//NO NEWLINES AT END OF STR
void CompilerChar(Compiler* compiler, char ch){
	Op m;
	bool nl;
	compiler->m_Ch = ch;
	nl = false;
	switch (compiler->m_Ch) {
	case COMMENT_CHAR: {
		if (compiler->m_StringMode) break;
		switch (compiler->m_CommentMode) {
		case OP_NotSet: {
			compiler->m_CommentMode = OP_Comment;
			CompilerPush(compiler, OP_ModeComment, false);
			break;
		}
		case OP_Comment: {
			CompilerPop(compiler);
			CompilerPush(compiler, OP_ModeMultiLineComment, false);
			compiler->m_CommentMode = OP_MultiLineComment;
			break;
		}
		case OP_MultiLineComment: {
			switch (compiler->m_MultiLineOffCount++) {
			case 0: break;
			case 1: {
				CompilerPop(compiler);
				compiler->m_MultiLineOffCount = 0;
				compiler->m_CommentMode = OP_NotSet;
				break;
			}
			}
			break;
		}
		}
		break;
	}
	case '\0': return;
	case '\n': {
		nl = true;
		if(compiler->m_CommentMode==OP_NotSet)printf("Char():Line end\n");
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
			CompilerPopObj(compiler, true);
			break;
		}
		case OP_VarWantValue: 
		case OP_VarComplete: {
			CompilerPopObj(compiler, true);
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
			//case OP_FuncSignatureComplete:
			case OP_FuncSigComplete:
			case OP_FuncHasName: {
				SetObjType(OP_FuncSigComplete);
				PopPfxs();
				//PopPfxs();
				Op mod = ObjGetMod(CompilerGetObj(compiler));
				CompilerPopObj(compiler, true);
				if (mod != OP_Imaginary) {
					CompilerPushAllowedPfxs(compiler, 0, "expected operator, print statement, or variable declaration",
						3,
						OP_Op, OP_String, OP_VarType);
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
	if (compiler->m_MultiLineOffCount == 1 && compiler->m_Ch != COMMENT_CHAR) {
		compiler->m_MultiLineOffCount = 0;
	}
	m = GetMode;
	compiler->m_Column++;
	if (!nl && compiler->m_CommentMode == OP_NotSet) {
		if(compiler->m_Ch == ' ') printf("-> SPACE (0x%x)\n",  compiler->m_Ch);
		else printf("-> %c (0x%x) %d:%d\n", compiler->m_Ch, compiler->m_Ch, compiler->m_Line, compiler->m_Column);
		switch (m) {
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
		if (CompilerIsPfxExpected(compiler, OP_LineEnd)) PopPfxs();
		compiler->m_Column = 0;
		compiler->m_Line++;
	}
}
const char* CompilerGetCPrintfFmtForType(Compiler* compiler, Op type) {
	switch (type) {
	case OP_String: return "s";
	case OP_i32:    return "d";
	case OP_i64:    return "lld";
	case OP_u64:    return "llu";
	case OP_f32:    return "f";
	case OP_u32:    return "u";
	case OP_Char:   return "c";
	}
	Err(OP_ErrNOT_GOOD, "GetPrintfFmtForType: unknown type");
	return "???";
}
void Val2Str(char *dest, int destSz, Val v, Op type) {

	switch (type) {
	case OP_u8:  { snprintf(dest, destSz, "%u",   v.u8);  break; }
	case OP_c8:  { snprintf(dest, destSz, "%c",   v.c8);  break; }
	case OP_i16: { snprintf(dest, destSz, "%d",   v.i16); break; }
	case OP_u16: { snprintf(dest, destSz, "%u",   v.u16); break; }
	case OP_i32: { snprintf(dest, destSz, "%d",   v.i32); break; }
	case OP_i64: { snprintf(dest, destSz, "%lld", v.i64); break; }
	case OP_u32: { snprintf(dest, destSz, "%u",   v.u32); break; }
	case OP_u64: { snprintf(dest, destSz, "%llu", v.u64); break; }
	case OP_f32: { snprintf(dest, destSz, "%f",   v.f32); break; }
	case OP_d64: { snprintf(dest, destSz, "%f",   v.d64); break; }
	}
}
void CompilerPopAndDoTask(Compiler* compiler)	{
	IBVector* wObjs;
	printf("PopAndDoTask()\n");
	if(!compiler->m_TaskStack.elemCount)Err(OP_ErrNoTask, "task stack EMPTY!");
	wObjs = GetTaskWorkingObjs;
	assert(wObjs);
	if(!wObjs->elemCount)Err(OP_ErrNOT_GOOD, "workingObjs EMPTY!");
	bool subTask = false;
	switch (GetTaskType) {
	case OP_FuncWantCode: break;
	case OP_FuncSigComplete:
	case OP_FuncHasName:
	case OP_Func: {
		Obj* o;
		int idx;
		char cFuncModsTypeName[CODE_STR_MAX];
		char cFuncArgs[CODE_STR_MAX];
		char cFuncCode[CODE_STR_MAX];
		bool imaginary;
		Obj* funcObj;

		imaginary = false;
		cFuncModsTypeName[0] = '\0';
		cFuncArgs[0] = '\0';
		cFuncCode[0] = '\0';
		funcObj = NULL;
		for (int i = 0; i < GetTaskWorkingObjs->elemCount; ++i) {
			Obj* o = (Obj*)IBVectorGet(GetTaskWorkingObjs, i);
			switch (ObjGetType(o)) {
			case OP_FuncArgComplete: {//multiple allowed
				auto at = o->arg.type;
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
			case OP_CompletedFunction: {//should only happen once
				funcObj = o;
				Op mod = ObjGetMod(o);
				if (mod != OP_NotSet) {
					StrConcat(cFuncModsTypeName, CODE_STR_MAX, GetCEqu(mod));
					StrConcat(cFuncModsTypeName, CODE_STR_MAX, " ");
				}
				StrConcat(cFuncModsTypeName, CODE_STR_MAX, GetCEqu(o->func.retType));
				StrConcat(cFuncModsTypeName, CODE_STR_MAX, GetCEqu(o->func.retTypeMod));
				StrConcat(cFuncModsTypeName, CODE_STR_MAX, " ");
				if (!o->name)Err(OP_ErrNOT_GOOD, "func name NULL");
				StrConcat(cFuncModsTypeName, CODE_STR_MAX, o->name);
				StrConcat(cFuncModsTypeName, CODE_STR_MAX, "(");
				break;
			}
			}
		}
		idx = 0;
		while (o= (Obj*)IBVectorIterNext(GetTaskWorkingObjs,&idx)) {
			switch (ObjGetType(o)) {
			case OP_VarComplete: {
				char valBuf[32];
				valBuf[0] = '\0';
				StrConcat(cFuncCode, CODE_STR_MAX, "\t");
				StrConcat(cFuncCode, CODE_STR_MAX, GetCEqu(o->var.type));
				StrConcat(cFuncCode, CODE_STR_MAX, GetCEqu(o->var.mod));
				StrConcat(cFuncCode, CODE_STR_MAX, " ");
				if(!o->name)Err(OP_ErrNOT_GOOD, "var name NULL");
				StrConcat(cFuncCode, CODE_STR_MAX, o->name);
				StrConcat(cFuncCode, CODE_STR_MAX, "=");
				snprintf(valBuf, 32, "%I64u", o->var.val.i64);
				StrConcat(cFuncCode, 32, valBuf);
				StrConcat(cFuncCode, CODE_STR_MAX, ";\n");
				break;
			}
			}
		}
		if (imaginary) {
			StrConcat(cFuncArgs, CODE_STR_MAX, ");\n\n");
		}
		else {
			StrConcat(cFuncArgs, CODE_STR_MAX, "){\n");
			StrConcat(cFuncCode, CODE_STR_MAX, GetTaskCodeP1);
			if(!funcObj)Err(OP_ErrNOT_GOOD, "funcObj NULL");
			if (funcObj->func.retType != OP_Void) {
				StrConcat(cFuncCode, CODE_STR_MAX, "\treturn ");
				char valBuf[32];
				valBuf[0] = '\0';
				Val2Str(valBuf, 32, funcObj->func.retVal, funcObj->func.retType);
				StrConcat(cFuncCode, 32, valBuf);
				StrConcat(cFuncCode, CODE_STR_MAX, ";\n");
			}
			StrConcat(cFuncCode, CODE_STR_MAX, "}\n\n");
		}
		compiler->m_cOutput[0]='\0';
		StrConcat(compiler->m_cOutput, CODE_STR_MAX, cFuncModsTypeName);
		StrConcat(compiler->m_cOutput, CODE_STR_MAX, cFuncArgs);
		StrConcat(compiler->m_cOutput, CODE_STR_MAX, cFuncCode);
		break;
	}
	case OP_CPrintfHaveFmtStr: {
		subTask = true;
		Obj* fmtObj = GetTaskWorkingObjs->start->obj;
		StrConcat(GetTaskCode, CODE_STR_MAX, "\tprintf(\"");
		bool firstPercent = false;
		int varIdx = 1;
		for (int i = 0; i < strlen(fmtObj->str); ++i) {
			auto c = fmtObj->str[i];
			switch (c) {
			case '%':{
					if (!firstPercent) {
						StrConcat(GetTaskCode, CODE_STR_MAX, "%");
						firstPercent = true;
					}
					else {
						Obj* vo = (Obj*)IBVectorGet(GetTaskWorkingObjs, varIdx);
						switch (ObjGetType(vo)) {
						case OP_Name:{
							Op type = NameInfoDBFindType(&compiler->m_NameTypeCtx, vo->name);
							StrConcat(GetTaskCode, CODE_STR_MAX, CompilerGetCPrintfFmtForType(compiler, type));
							break;
						}
						case OP_Value:{
							StrConcat(GetTaskCode, CODE_STR_MAX, CompilerGetCPrintfFmtForType(compiler, vo->var.type));
							break;
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
		if (GetTaskWorkingObjs->elemCount > 1) {
			StrConcat(GetTaskCode, CODE_STR_MAX, ", ");
		}
		for (int i = 1; i < GetTaskWorkingObjs->elemCount; ++i) {
			Obj* o = (Obj*)IBVectorGet(GetTaskWorkingObjs, i);
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
			if (i < GetTaskWorkingObjs->elemCount - 1) {
				StrConcat(GetTaskCode, CODE_STR_MAX, ", ");
			}
		}
		StrConcat(GetTaskCode, CODE_STR_MAX, ");\n");
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
	//for assigning func call ret val to var
	if (compiler->m_Pfx == OP_Value && compiler->m_Ch == '@' && !compiler->m_Str[0]) {
		CompilerPushAllowedPfxs(compiler, 1, "", 1, OP_Op);
	}
	compiler->m_Pfx = fromPfxCh(compiler->m_Ch);
	obj=CompilerGetObj(compiler);
	if (compiler->m_Pfx != OP_Unknown 
		&& GetAllowedPfxsTop->pfxs.elemCount
		&& !CompilerIsPfxExpected(compiler, compiler->m_Pfx))
		Err(OP_ErrUnexpectedNextPfx, "");
	printf("PFX:%s(%d)\n", GetPfxName(compiler->m_Pfx), (int)compiler->m_Pfx);
	switch (compiler->m_Pfx) {
	case OP_String: { //"
		compiler->m_StringMode = true;
		CompilerPush(compiler, OP_ModeStrPass, false);
		break;
	}
	case OP_VarType:
		IBVectorCopyPushBool(&compiler->m_StrReadPtrsStack, true);
	case OP_Value:
	case OP_Op:
	case OP_Name:
		//getchar();
		CompilerPush(compiler, OP_ModeStrPass, false);
		break;
	case OP_Comment:
		break;
	}
	if (compiler->m_Pfx == OP_Op) {
		AllowedPfxs* aps = GetAllowedPfxsTop;
		if (aps->life && --aps->life <= 0) {
			IBVectorPop(GetAPfxsStack);
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
					printf("got pointer\n");
					compiler->m_Pointer = OP_Pointer;
					break;
				case OP_Pointer:
					printf("got double pointer\n");
					compiler->m_Pointer = OP_DoublePointer;
					break;
				case OP_DoublePointer:
					printf("got tripple pointer\n");
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
	printf("Doing Str payload\n");
	Val strVal;
	strVal.i32=atoi(compiler->m_Str);
	compiler->m_NameOp = GetOpFromName(compiler->m_Str);
	printf("Str: %s\n", compiler->m_Str);
	switch (compiler->m_Pfx)
	{
	case OP_String: { //"
		switch(GetTaskType){
		case OP_FuncWantCode: { //printf
			CompilerPushTask(compiler, OP_CPrintfHaveFmtStr);
			Obj*o=CompilerPushObj(compiler);
			ObjSetStr(o, compiler->m_Str);
			ObjSetType(o, OP_CPrintfFmtStr);
			CompilerPopObj(compiler, true);
			CompilerPushAllowedPfxs(compiler, 0, "expected fmt args or line end", 4, OP_Value, OP_Name, OP_String, OP_LineEnd);
			break;
		}
		}
		break;
	}
	case OP_Value: { //=
		switch (GetObjType) {
		case OP_VarWantValue: {
			CompilerGetObj(compiler)->var.val = strVal;
			SetObjType(OP_VarComplete);
			PopPfxs();
			break;
		}
		}
		if (compiler->m_TaskStack.elemCount) {
			switch (GetTaskType) {
			case OP_CPrintfHaveFmtStr:{
				CompilerPushObj(compiler);
				CompilerGetObj(compiler)->val = strVal;
				ObjSetType(CompilerGetObj(compiler), OP_Value);
				CompilerGetObj(compiler)->var.type = OP_i32;//for now
				CompilerPopObj(compiler, true);
				break;
			}
			case OP_FuncNeedRetVal: {
				Obj* o;
				int idx;
				idx = 0;
				while (o = (Obj*)IBVectorIterNext(GetTaskWorkingObjs,&idx)) {
					if (ObjGetType(o) == OP_FuncSigComplete) {
						printf("Finishing func got ret value\n");
						o->func.retVal = strVal;
						PopPfxs();
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
	case OP_VarType: //%
		switch (GetTaskType) {
		case OP_FuncWantCode: {
			CompilerPushObj(compiler);
			CompilerGetObj(compiler)->var.type = compiler->m_NameOp;
			CompilerGetObj(compiler)->var.mod = OP_NotSet;
			SetObjType(OP_VarNeedName);
			CompilerPushAllowedPfxs(compiler, 0,"Expected variable name after variable type", 1, OP_Name);
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
			o=CompilerPushObj(compiler);
			SetObjType(OP_FuncArgNameless);
			o->arg.type = compiler->m_NameOp;
			o->arg.mod = compiler->m_Pointer;
			CompilerPushAllowedPfxs(compiler, 0, "Expected func arg name", 1, OP_Name);
			break;
		}
		}
		break;
	case OP_Name: { //$
		switch(GetTaskType){
		case OP_CPrintfHaveFmtStr: {
			Obj*o=CompilerPushObj(compiler);
			ObjSetName(o, compiler->m_Str);
			ObjSetType(o, OP_Name);
			CompilerPopObj(compiler, true);
			break;
		}
		}
		switch (GetObjType) {
		case OP_CallNeedName: { //=@call
			SetObjType(OP_CallWantArgs);
			PopPfxs();
			CompilerPushAllowedPfxs(compiler, 0,"expected var type or line end after func name", 3, OP_Name, OP_Value, OP_LineEnd);

		}
		case OP_Func: {
			SetObjType(OP_FuncHasName);
			SetTaskType(OP_FuncHasName);
			PopPfxs();
			CompilerPushAllowedPfxs(compiler, 0,"", 3, OP_VarType, OP_Op, OP_LineEnd/*means allowed pfx will be cleared on newline*/);
			ObjSetName(CompilerGetObj(compiler), compiler->m_Str);
			break;
		}
		case OP_FuncArgNameless:
			SetObjType(OP_FuncArgComplete);
			PopPfxs();
			ObjSetName(CompilerGetObj(compiler), compiler->m_Str);
			NameInfoDBAdd(&compiler->m_NameTypeCtx, compiler->m_Str, CompilerGetObj(compiler)->arg.type);
			CompilerPopObj(compiler, true);
			break;
		case OP_VarNeedName:
			ObjSetName(CompilerGetObj(compiler), compiler->m_Str);
			NameInfoDBAdd(&compiler->m_NameTypeCtx, compiler->m_Str, CompilerGetObj(compiler)->var.type);
			SetObjType(OP_VarWantValue);
			PopPfxs();
			CompilerPushAllowedPfxs(compiler, 0,"expected value or line end after var name", 2, OP_Value, OP_LineEnd);
			break;
		}
		break;
	}
	case OP_Op: //@
		switch (compiler->m_NameOp) {
		case OP_Call:{
			switch (GetObjType) {
			case OP_VarWantValue: {
				Obj*o=CompilerPushObj(compiler);
				ObjSetType(o, OP_CallNeedName);
				CompilerPushAllowedPfxs(compiler, 0,"expected function name", 1, OP_Name);
			}
			}
			break;
		}
		case OP_dbgBreak: {
			__debugbreak();
			compiler->m_TaskStack;
			break;
		}
		case OP_Imaginary:
			ObjSetMod(CompilerGetObj(compiler), compiler->m_NameOp);
			break;
		case OP_Done:
			if (!compiler->m_TaskStack.elemCount) Err(OP_ErrNoTask, "");
			switch (GetTaskType) {
			case OP_Func:
			case OP_FuncHasName:
			case OP_FuncWantCode: {
				Obj* o;
				int idx;
				PRINT_LINE_INFO();
				printf(" Finishing function\n");
				idx = 0;
				while (o = (Obj*)IBVectorIterNext(GetTaskWorkingObjs,&idx)) {
					//TODO: could cache func obj index later
					if (ObjGetType(o) == OP_FuncSigComplete) {
						if (o->func.retType != OP_Void) {
							CompilerPushAllowedPfxs(compiler, 0,"", 1, OP_Value);
							SetTaskType(OP_FuncNeedRetVal);
						}
						else {
							SetTaskType(OP_Func);
							CompilerPopAndDoTask(compiler);
							PopPfxs();
						}
					}
				}
				break;
			}
			}
			break;
		case OP_Return: {
			auto t = GetObjType;
			switch (t) {
			case OP_FuncArgComplete: {
				printf("what\n");
				CompilerPopObj(compiler, true);
				if (GetObjType != OP_FuncHasName) {
					Err(OP_ErrNOT_GOOD, "expected FuncHasName");
					break;
				}
			}
			case OP_FuncHasName:
				SetObjType(OP_FuncNeedsRetValType);
				CompilerPushAllowedPfxs(compiler, 0,"", 1, OP_VarType);
				break;
			default:
				Err(OP_ErrUnexpectedOp, "");
				break;
			}
			break;
		}
		case OP_Func:
			SetObjType(compiler->m_NameOp);
			CompilerGetObj(compiler)->func.retType = OP_Void;
			CompilerGetObj(compiler)->func.retTypeMod = OP_NotSet;
			CompilerPushAllowedPfxs(compiler, 0,"", 1, OP_Name);
			CompilerPushTask(compiler, OP_FuncNeedName);
			break;
		case OP_Public:
		case OP_Private:
			CompilerGetObj(compiler)->privacy = compiler->m_NameOp;
			break;
		default:
			Err(OP_ErrUnknownOpStr, "");
		}
	}
	compiler->m_Str[0] = '\0';
	printf("Str payload complete\n");
	CompilerPop(compiler);
	if(compiler->m_StrReadPtrsStack.elemCount > 1)
	{
		if (*(bool*)IBVectorTop(&compiler->m_StrReadPtrsStack)) compiler->m_Pointer = OP_NotSet;
		IBVectorPop(&compiler->m_StrReadPtrsStack);
	}
}
void CompilerExplainErr(Compiler* compiler, Op code) {
	switch (code) {
	case OP_ErrUnknownOpStr:
		printf("Unknown OP str @%s\n", compiler->m_Str);
		break;
	case OP_ErrQuadriplePointersNOT_ALLOWED:
		printf("Why?");
		break;
	case OP_ErrUnexpectedOp:
		printf("Unexpected OP");
		break;
	case OP_ErrNoTask:
		printf("No working task to call done (@@) for");
		break;
	case OP_ErrUnexpectedNextPfx: {
		Op* oi;
		int idx;
		printf("%s Unexpected next prefix %s. Pfx stack idx:%d Allowed:", 
			GetAllowedPfxsTop->err, GetPfxName(compiler->m_Pfx), 
				GetAPfxsStack->elemCount - 1);
		idx = 0;
		while (oi = (Op*)IBVectorIterNext(&GetAllowedPfxsTop->pfxs,&idx)) {
			printf("%s,", GetPfxName(*oi));
		}
		break;
	}
	case OP_ErrExpectedVariablePfx:
		printf("Expected a variable type to be next.");
		break;
	default:
		printf("Err msg unimplemented for %s", GetOpName(code));
	}
	printf("\nOBJ:");
	ObjPrint(CompilerGetObj(compiler));
	printf("\n");
}
int main(int argc, char** argv) {
	FILE* f;
	const char* fname = /*argv[1]*/"main.txt";
	f = fopen(fname, "r");
	if (f){
		Compiler *comp = (Compiler*)malloc(sizeof(Compiler));
		char ch;
		CompilerInit(comp);
		while ((ch = fgetc(f)) != EOF) {
			if (ch == 0xffffffff) break;
			CompilerChar(comp, ch);
		}
		printf("Exiting\n");
		CompilerFree(comp);
		free(comp);
		fclose(f);
		return 0;
	}
	else{
		printf("Error opening file\n");
	}
	return 1;
}