#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>

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
#define COMPILER_STR_MAX 256

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
	OP_CPrintfHaveFmtStr,

	OP_SpaceChar, OP_Comma, OP_CommaSpace, OP_Name, OP_String,
	OP_CPrintfFmtStr, OP_Char, OP_If, OP_Else, OP_For, OP_While,
	OP_Block, OP_c8, OP_u8, OP_u16, OP_u32, OP_u64, OP_i8, OP_i16,
	OP_i32, OP_i64, OP_f32, OP_d64, OP_Pointer, OP_DoublePointer,
	OP_TripplePointer, OP_CompilerFlags, OP_dbgBreak,

	OP_NotFound, OP_Error, OP_ErrNOT_GOOD, OP_ErrUnexpectedNextPfx,
	OP_ErrExpectedVariablePfx, OP_ErrNoTask, OP_ErrUnexpectedOp,
	OP_ErrQuadriplePointersNOT_ALLOWED, OP_ErrUnknownOpStr,

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
typedef struct IBVector {
	size_t elemSize;
	int elemCount;
	int slotCount;
	size_t dataSize;
	size_t iterIdx;
	union {
		void* data;
		struct AllowedPfxs *apfxs;
		struct Task *task;
	};
} IBVector;
void IBVectorInit(IBVector* vec, size_t elemSize) {
	vec->elemSize = elemSize;
	vec->elemCount = 0;
	vec->slotCount = 1;
	vec->dataSize = vec->elemSize * vec->slotCount;
	vec->iterIdx = 0;
	vec->data = malloc(vec->dataSize);
	memset(vec->data, 0, vec->dataSize);
}
void* IBVectorGet(IBVector* vec, int idx) {
	if (idx >= vec->elemCount) return NULL;
	return (char*)vec->data + vec->elemSize * idx;
}
void* IBVectorIterNext(IBVector* vec) {
	if (vec->iterIdx >= vec->elemCount) {
		vec->iterIdx = 0;
		return NULL;
	}
	return (char*)vec->data + vec->elemSize * vec->iterIdx++;
}
void IBVectorCopyPush(IBVector* vec, void* elem) {
	assert(vec->elemCount <= vec->slotCount);
	if (vec->elemCount >= vec->slotCount) {
		vec->slotCount++;
		vec->dataSize = vec->elemSize * vec->slotCount;
		vec->data = realloc(vec->data, vec->dataSize);
	}
	memcpy((char*)vec->data + vec->elemSize * vec->elemCount, elem, vec->elemSize);
	vec->elemCount++;
}
void IBVectorCopyPushBool(IBVector* vec, bool val) {
	IBVectorCopyPush(vec, &val);
}
void IBVectorCopyPushOp(IBVector* vec, Op val) {
	IBVectorCopyPush(vec, &val);
}
void* IBVectorTop(IBVector* vec) {
	if (vec->elemCount <= 0) return NULL;
	return (char*)vec->data + vec->elemSize * (vec->elemCount - 1);
}
void* IBVectorFront(IBVector* vec) {
	if (vec->elemCount <= 0) return NULL;
	return (char*)vec->data;
}
void IBVectorPop(IBVector* vec) {
	if(vec->elemCount <= 0) return;
	vec->elemCount--;
	vec->slotCount = ClampSizeT(vec->slotCount, 1, vec->elemCount);
	vec->dataSize = vec->elemSize * vec->slotCount;
	realloc(vec->data, vec->dataSize);
}
void IBVectorFree(IBVector* vec) {
	free(vec->data);
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
void NameInfoDBAdd(NameInfoDB* db, const char* name, Op type);
Op NameInfoDBFindType(NameInfoDB* db, const char* name);
void NameInfoDBFree(NameInfoDB* db) {
	IBVectorFree(&db->pairs);
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
	//if(o->name) free(o->name);
	o->name=NULL;
	//if(o->str) free(o->str);
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
typedef struct Task {
	Op type;
	IBVector working;//Obj
	char code1[CODE_STR_MAX];
	char code2[CODE_STR_MAX];
} Task;
void TaskInit(Task* t, Op type) {
	IBVectorInit(&t->working, sizeof(Obj));
	t->type = type;
	t->code1[0] = '\0';
	t->code2[0] = '\0';
}
typedef struct  Compiler {
	int m_Line, m_Column;
	Op m_Pfx;
	char m_Str[COMPILER_STR_MAX];
	char m_cOutput[CODE_STR_MAX];

	IBVector m_ObjStack; //Obj
	IBVector m_AllowedNextPfxsStack; //AllowedPfxs
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
Obj* Compiler_GetObj(Compiler* compiler);
void Compiler_Init(Compiler* compiler);
void Compiler_Free(Compiler* compiler);
void Compiler_pushTask(Compiler* compiler, Op task);
void Compiler_popTask(Compiler* compiler);
Obj* Compiler_pushObj(Compiler* compiler);
Obj* Compiler_popObj(Compiler* compiler, bool pushToWorking);
void Compiler_push(Compiler* compiler, Op mode, bool strAllowSpace);
Op Compiler_pop(Compiler* compiler);
//life:0 = infinite, -1 life each pfx
//void pushAllowedNextPfxs(std::vector<Op> allowedNextPfxs, const char* err, int life);
void Compiler_pushAllowedPfxs(Compiler* compiler, int life, const char* err, int count, ...);
void Compiler_popAllowedNextPfxs(Compiler* compiler);
bool Compiler_isPfxExpected(Compiler* compiler, Op pfx);
//NO NEWLINES AT END OF STR
void Compiler_Char(Compiler* compiler, char ch);
void Compiler_PopAndDoTask(Compiler* compiler);
const char* Compiler_GetCPrintfFmtForType(Compiler* compiler, Op type);
void Compiler_Prefix(Compiler* compiler);
void Compiler_Str(Compiler* compiler);
void Compiler_StrPayload(Compiler* compiler);
void Compiler_ExplainErr(Compiler* compiler, Op code);
#define Err(code, msg){\
	PRINT_LINE_INFO();\
	printf(":%s At %u:%u \"%s\"(%d)\nExplanation: ", msg, compiler->m_Line, compiler->m_Column, GetOpName(code), (int)code);\
	Compiler_ExplainErr(compiler, code);\
	printf("\n");\
	__debugbreak();\
}
#define SetObjType(type){\
	PRINT_LINE_INFO();\
	ObjSetType(Compiler_GetObj(compiler), type);\
}
#define GetObjType (ObjGetType(Compiler_GetObj(compiler)))
#define PushPfxs(pfxs, msg, life){\
	PRINT_LINE_INFO();\
	pushAllowedNextPfxs(pfxs, msg, life);\
}
#define PopPfxs(){\
	PRINT_LINE_INFO();\
	Compiler_popAllowedNextPfxs(compiler);\
}
#define GetAllowedPfxsTop ((AllowedPfxs*)IBVectorTop(&compiler->m_AllowedNextPfxsStack))
#define GetTask ((Task*)IBVectorTop(&compiler->m_TaskStack))
#define GetMode *((Op*)IBVectorTop(&compiler->m_ModeStack))
#define GetTaskType   (GetTask->type)
#define GetTaskCode   (GetTask->code1)
#define GetTaskCodeP1 (GetTask->code2)
#define SetTaskType(tt) {\
	PRINT_LINE_INFO();\
	printf("SetTaskType: %s(%d) -> %s(%d)\n", GetOpName(GetTaskType), (int)GetTaskType, GetOpName(tt), (int)tt);\
	GetTask->type = tt;\
}
#define GetTaskWorkingObjs (GetTask->working)
#define SwitchTaskStackStart if (!compiler->m_TaskStack.elemCount) {\
	switch (GetTaskType) {
#define SwitchTaskStackEnd }\
	}
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
	{"CallWantArgs", OP_CallWantArgs},{"CallComplete", OP_CallComplete}
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
Obj* Compiler_GetObj(Compiler* compiler) {
	return (Obj*)IBVectorTop(&compiler->m_ObjStack);
}
void Compiler_Init(Compiler* compiler){
	AllowedPfxs ap;
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
	IBVectorInit(&compiler->m_AllowedNextPfxsStack, sizeof(AllowedPfxs));
	AllowedPfxsInit(&ap, 1, OP_Op);
	IBVectorCopyPush(&compiler->m_AllowedNextPfxsStack, &ap);
	
	//IBVectorInit(&compiler->m_NameTypeCtx.pairs, sizeof(NameInfo));
	NameInfoDBInit(&compiler->m_NameTypeCtx);
	IBVectorInit(&compiler->m_ObjStack, sizeof(Obj));
	IBVectorInit(&compiler->m_ModeStack, sizeof(Op));
	IBVectorInit(&compiler->m_StrReadPtrsStack, sizeof(bool));
	IBVectorInit(&compiler->m_TaskStack, sizeof(Task));
	IBVectorCopyPushBool(&compiler->m_StrReadPtrsStack, false);
	Compiler_push(compiler, OP_ModePrefixPass, false);
	Compiler_pushObj(compiler);
}
void Compiler_Free(Compiler* compiler) {
	if (compiler->m_StringMode)Err(OP_ErrNOT_GOOD, "Reached end of file without closing string");
	if(compiler->m_Str[0]) Compiler_StrPayload(compiler);
	if (compiler->m_TaskStack.elemCount) {
		switch (((Task*)IBVectorTop(&compiler->m_TaskStack))->type) {
		case OP_FuncNeedRetVal:
			Err(OP_ErrNOT_GOOD, "Reached end of file without closing function");
			break;
		case OP_FuncSigComplete:
		case OP_FuncHasName: {
			SetObjType(OP_FuncSigComplete);
			Compiler_popObj(compiler, true);
			Compiler_PopAndDoTask(compiler);
			break;
		}
		}
	
	}
	printf("-> Compilation complete <-\nResulting C code:\n\n");
	//printf("%s", compiler->m_cOutput.c_str());
	printf("%s", compiler->m_cOutput);
}
void Compiler_pushTask(Compiler* compiler, Op task) {
	Task t;
	printf("Push task %s(%d)\n", GetOpName(task),(int)task);
	TaskInit(&t, task);
	IBVectorCopyPush(&compiler->m_TaskStack, &t);
}
void Compiler_popTask(Compiler* compiler) {
	printf("Pop task %s(%d)\n", GetOpName(GetTaskType),(int)GetTaskType);
	IBVectorPop(&compiler->m_TaskStack);
}
Obj* Compiler_pushObj(Compiler* compiler) {
	Obj obj;
	ObjInit(&obj);
	printf("Push obj: ");
	if (compiler->m_ObjStack.elemCount) {
		ObjPrint(Compiler_GetObj(compiler));
		printf(" -> ");
	}
	IBVectorCopyPush(&compiler->m_ObjStack, &obj);
	ObjPrint(Compiler_GetObj(compiler));
	printf("\n");
	return Compiler_GetObj(compiler);
}
Obj* Compiler_popObj(Compiler* compiler, bool pushToWorking) {
	if (pushToWorking){
		if (GetObjType == OP_NotSet)Err(OP_ErrNOT_GOOD, "");
		printf("To working: ");
		ObjPrint(Compiler_GetObj(compiler));
		printf("\n");
		//GetTaskWorkingObjs.push_back(Compiler_GetObj(compiler));
		IBVectorCopyPush(&GetTaskWorkingObjs, Compiler_GetObj(compiler));
	}
	printf("Pop obj: ");
	ObjPrint(Compiler_GetObj(compiler));
	if (compiler->m_ObjStack.elemCount == 1) {
		//Compiler_GetObj(compiler) = {};
		ObjInit(Compiler_GetObj(compiler));
	}
	else {
		//compiler->m_ObjStack.Compiler_pop(compiler);
		IBVectorPop(&compiler->m_ObjStack);
	}
	printf(" -> ");
	ObjPrint(Compiler_GetObj(compiler));
	printf("\n");
	return Compiler_GetObj(compiler);
}
void Compiler_push(Compiler* compiler, Op mode, bool strAllowSpace){
	compiler->m_StrAllowSpace = strAllowSpace;
	//compiler->m_ModeStack.push(mode);
	IBVectorCopyPushOp(&compiler->m_ModeStack, mode);
	printf("push: to %s\n", GetOpName(GetMode));
}
Op Compiler_pop(Compiler* compiler) {
	IBVectorPop(&compiler->m_ModeStack);
	printf("pop: to %s\n", GetOpName(GetMode));
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
	unsigned __int64 u64 = obj->val.u64;
	printf("[");
	if (obj->type != OP_NotSet) {
		printf("Type:%s(%d),", GetOpName(obj->type), (int)obj->type);
	}
	if(obj->name)printf("Name:%s,", obj->name);
	if (obj->str)printf("Str:%s,", obj->str);
	if (obj->modifier != OP_NotSet) {
		printf("Mod:%s,", GetOpName(obj->modifier));
	}
	/*if(u64)*/printf("Val:%I64u", u64);
	printf("]");
}
//void Compiler_pushAllowedNextPfxs(std::vector<Op> allowedNextPfxs, const char* err, int life) {
//	if (!compiler->m_AllowedNextPfxsStack.top().pfxs.empty()) {
//		printf(" allowed pfxs PUSH: { ");
//		for (auto& p : compiler->m_AllowedNextPfxsStack.top().pfxs) printf("%s ", GetPfxName(p));
//		printf("} -> { ");
//		compiler->m_AllowedNextPfxsStack.push({ allowedNextPfxs, err, life });
//		for (auto& p : compiler->m_AllowedNextPfxsStack.top().pfxs) printf("%s ", GetPfxName(p));
//		printf("}\n");
//	}
//	else Err(OP_ErrNOT_GOOD, "pfx stack vec cannot be empty");
//}
void Compiler_pushAllowedPfxs(Compiler* compiler, int life, const char* err, int count, ...)
{
	va_list args;
	va_start(args, count);
	while (count--) {
		AllowedPfxs ap;
		//memset(&ap, 0, sizeof(AllowedPfxs));
		//ap.pfx = va_arg(args, Op);
		IBVectorInit(&ap.pfxs, sizeof(Op));
	}

}
void Compiler_popAllowedNextPfxs(Compiler* compiler) {
	if (GetAllowedPfxsTop->pfxs.elemCount) {
		Op* oi;
		printf(" allowed pfxs POP: { ");
		while (oi = (Op*)IBVectorIterNext(&GetAllowedPfxsTop->pfxs)) {
			printf("%s ", GetPfxName(*oi));
		}
		printf("} -> { ");
		//compiler->m_AllowedNextPfxsStack.Compiler_pop(compiler);
		IBVectorPop(&compiler->m_AllowedNextPfxsStack);
		if (!compiler->m_AllowedNextPfxsStack.elemCount) Err(OP_ErrNOT_GOOD, "catastrophic failure");
		while (oi = (Op*)IBVectorIterNext(&GetAllowedPfxsTop->pfxs)) {
			printf("%s ", GetPfxName(*oi));
		}
		printf("}\n");
	}
}
bool Compiler_isPfxExpected(Compiler* compiler, Op pfx) {
	Op* oi;
	while (oi = (Op*)IBVectorIterNext(&GetAllowedPfxsTop->pfxs)) {
		if (*oi == pfx) return true;
	}
	return false;
}
//NO NEWLINES AT END OF STR
void Compiler_Char(Compiler* compiler, char ch){
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
			Compiler_push(compiler, OP_ModeComment, false);
			break;
		}
		case OP_Comment: {
			Compiler_pop(compiler);
			Compiler_push(compiler, OP_ModeMultiLineComment, false);
			compiler->m_CommentMode = OP_MultiLineComment;
			break;
		}
		case OP_MultiLineComment: {
			switch (compiler->m_MultiLineOffCount++) {
			case 0: break;
			case 1: {
				Compiler_pop(compiler);
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
			Compiler_pop(compiler);
			compiler->m_CommentMode = OP_NotSet;
		}
		switch (GetMode) {
		case OP_ModeStrPass: {
			Compiler_StrPayload(compiler);
			break; 
		}
		}
		switch (GetObjType) {
		case OP_CallWantArgs: {
			Compiler_popObj(compiler, true);
			break;
		}
		case OP_VarWantValue: 
		case OP_VarComplete: {
			Compiler_popObj(compiler, true);
			break;
		}
		}
		SwitchTaskStackStart
			case OP_CPrintfHaveFmtStr: {
				Compiler_PopAndDoTask(compiler);
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
				Op mod = ObjGetMod(Compiler_GetObj(compiler));
				Compiler_popObj(compiler, true);
				if (mod != OP_Imaginary) {
					Compiler_pushAllowedPfxs(compiler, 0, "expected operator, print statement, or variable declaration",
						3,
						OP_Op, OP_String, OP_VarType);
					SetTaskType(OP_FuncWantCode);
				}
				else {
					Compiler_PopAndDoTask(compiler);
				}
				break;
			}
		SwitchTaskStackEnd
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
			Compiler_Prefix(compiler);
			break;
		case OP_ModeStrPass:
			Compiler_Str(compiler);
			break;
		}
	}
	if (nl) {
		if (Compiler_isPfxExpected(compiler, OP_LineEnd)) PopPfxs();
		compiler->m_Column = 0;
		compiler->m_Line++;
	}
}
const char* Compiler_GetCPrintfFmtForType(Compiler* compiler, Op type) {
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
void Compiler_PopAndDoTask(Compiler* compiler)	{
	printf("PopAndDoTask()\n");
	if(!compiler->m_TaskStack.elemCount)Err(OP_ErrNoTask, "task stack EMPTY!");
	if(!GetTaskWorkingObjs.elemCount)Err(OP_ErrNOT_GOOD, "workingObjs EMPTY!");
	bool subTask = false;
	switch (GetTaskType) {
	case OP_FuncWantCode: break;
	case OP_FuncSigComplete:
	case OP_FuncHasName:
	case OP_Func: {
		char cFuncModsTypeName[CODE_STR_MAX];
		cFuncModsTypeName[0] = '\0';
		char cFuncArgs[CODE_STR_MAX];
		cFuncArgs[0] = '\0';
		char cFuncCode[CODE_STR_MAX];
		cFuncCode[0] = '\0';
		bool imaginary = false;
		Obj* funcObj=NULL;
		for (int i = 0; i < GetTaskWorkingObjs.elemCount; ++i) {
			Obj* o = (Obj*)IBVectorGet(&GetTaskWorkingObjs, i);
			switch (ObjGetType(o)) {
			case OP_FuncArgComplete: {//multiple allowed
				auto at = o->arg.type;
				if (at == OP_Null)Err(OP_ErrNOT_GOOD, "arg type NULL");
				if (cFuncArgs[0] != '\0') {
					strcat_s(cFuncArgs, CODE_STR_MAX, ", ");
				}
				strcat_s(cFuncArgs, CODE_STR_MAX, GetCEqu(o->arg.type));
				strcat_s(cFuncArgs, CODE_STR_MAX, GetCEqu(o->arg.mod));
				strcat_s(cFuncArgs, CODE_STR_MAX, " ");
				if (!o->name)Err(OP_ErrNOT_GOOD, "arg name NULL");
				strcat_s(cFuncArgs, CODE_STR_MAX, o->name);
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
					strcat_s(cFuncModsTypeName, CODE_STR_MAX, GetCEqu(mod));
					strcat_s(cFuncModsTypeName, CODE_STR_MAX, " ");
				}
				strcat_s(cFuncModsTypeName, CODE_STR_MAX, GetCEqu(o->func.retType));
				strcat_s(cFuncModsTypeName, CODE_STR_MAX, GetCEqu(o->func.retTypeMod));
				strcat_s(cFuncModsTypeName, CODE_STR_MAX, " ");
				if (!o->name)Err(OP_ErrNOT_GOOD, "func name NULL");
				strcat_s(cFuncModsTypeName, CODE_STR_MAX, o->name);
				strcat_s(cFuncModsTypeName, CODE_STR_MAX, "(");
				break;
			}
			}
		}
		Obj* o;
		while (o= (Obj*)IBVectorIterNext(&GetTaskWorkingObjs)) {
			switch (ObjGetType(o)) {
			case OP_VarComplete: {
				char valBuf[32];
				valBuf[0] = '\0';
				strcat_s(cFuncCode, CODE_STR_MAX, "\t");
				strcat_s(cFuncCode, CODE_STR_MAX, GetCEqu(o->var.type));
				strcat_s(cFuncCode, CODE_STR_MAX, GetCEqu(o->var.mod));
				strcat_s(cFuncCode, CODE_STR_MAX, " ");
				if(!o->name)Err(OP_ErrNOT_GOOD, "var name NULL");
				strcat_s(cFuncCode, CODE_STR_MAX, o->name);
				strcat_s(cFuncCode, CODE_STR_MAX, "=");
				snprintf(valBuf, 32, "%I64u", o->var.val.i64);
				strcat_s(cFuncCode, 32, valBuf);
				strcat_s(cFuncCode, CODE_STR_MAX, ";\n");
				break;
			}
			}
		}
		if (imaginary) {
			strcat_s(cFuncArgs, CODE_STR_MAX, ");\n\n");
		}
		else {
			strcat_s(cFuncArgs, CODE_STR_MAX, "){\n");
			strcat_s(cFuncCode, CODE_STR_MAX, GetTaskCodeP1);
			if(!funcObj)Err(OP_ErrNOT_GOOD, "funcObj NULL");
			if (funcObj->func.retType != OP_Void) {
				strcat_s(cFuncCode, CODE_STR_MAX, "\treturn ");
				char valBuf[32];
				valBuf[0] = '\0';
				Val2Str(valBuf, 32, funcObj->func.retVal, funcObj->func.retType);
				strcat_s(cFuncCode, 32, valBuf);
				strcat_s(cFuncCode, CODE_STR_MAX, ";\n");
			}
			strcat_s(cFuncCode, CODE_STR_MAX, "}\n\n");
		}
		compiler->m_cOutput[0]='\0';
		strcat_s(compiler->m_cOutput, CODE_STR_MAX, cFuncModsTypeName);
		strcat_s(compiler->m_cOutput, CODE_STR_MAX, cFuncArgs);
		strcat_s(compiler->m_cOutput, CODE_STR_MAX, cFuncCode);
		break;
	}
	case OP_CPrintfHaveFmtStr: {
		subTask = true;
		//Obj& fmtObj = GetTaskWorkingObjs.front();
		Obj* fmtObj = (Obj*)IBVectorFront(&GetTaskWorkingObjs);
		strcat_s(GetTaskCode, CODE_STR_MAX, "\tprintf(\"");
		bool firstPercent = false;
		int varIdx = 1;
		for (int i = 0; i < strlen(fmtObj->str); ++i) {
			auto c = fmtObj->str[i];
			switch (c) {
			case '%':{
					if (!firstPercent) {
						strcat_s(GetTaskCode, CODE_STR_MAX, "%");
						firstPercent = true;
					}
					else {
						//auto& vo = GetTaskWorkingObjs[varIdx];
						Obj* vo = (Obj*)IBVectorGet(&GetTaskWorkingObjs, varIdx);
						switch (ObjGetType(vo)) {
						case OP_Name:{
							//auto type = compiler->m_NameTypeCtx.findType(vo->name);
							Op type = NameInfoDBFindType(&compiler->m_NameTypeCtx, vo->name);
							strcat_s(GetTaskCode, CODE_STR_MAX, Compiler_GetCPrintfFmtForType(compiler, type));
							break;
						}
						case OP_Value:{
							strcat_s(GetTaskCode, CODE_STR_MAX, Compiler_GetCPrintfFmtForType(compiler, vo->var.type));
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
				strcat_s(GetTaskCode, CODE_STR_MAX, chBuf);
				break;
			}
			}
		}
		strcat_s(GetTaskCode, CODE_STR_MAX, "\"");
		if (GetTaskWorkingObjs.elemCount > 1) {
			strcat_s(GetTaskCode, CODE_STR_MAX, ", ");
		}
		for (int i = 1; i < GetTaskWorkingObjs.elemCount; ++i) {
			//Obj& o = GetTaskWorkingObjs[i];
			Obj* o = (Obj*)IBVectorGet(&GetTaskWorkingObjs, i);
			switch (ObjGetType(o)) {
			case OP_Name: {
				strcat_s(GetTaskCode, CODE_STR_MAX, o->name);
				break;
			}
			case OP_String: {
				strcat_s(GetTaskCode, CODE_STR_MAX, "\"");
				strcat_s(GetTaskCode, CODE_STR_MAX, o->str);
				strcat_s(GetTaskCode, CODE_STR_MAX, "\"");
				break;
			}
			case OP_Value: {
				//GetTaskCode += std::to_string(o.val.i32);//for now
				char valBuf[32];
				valBuf[0] = '\0';
				Val2Str(valBuf, 32, o->val, o->var.type);
				strcat_s(GetTaskCode, 32, valBuf);
			}
			}
			if (i < GetTaskWorkingObjs.elemCount - 1) {
				strcat_s(GetTaskCode, CODE_STR_MAX, ", ");
			}
		}
		strcat_s(GetTaskCode, CODE_STR_MAX, ");\n");
		break;
	}
	}
	if (subTask) {
		switch (GetTaskType) {
		case OP_CPrintfHaveFmtStr: {
			if (compiler->m_TaskStack.elemCount - 2 >= 0) {
				char theCode[CODE_STR_MAX];
				theCode[0] = '\0';
				strcpy_s(theCode, CODE_STR_MAX, GetTaskCode);
				Compiler_popTask(compiler);
				switch (GetTaskType) {
				case OP_FuncWantCode: {
					strcat_s(GetTaskCodeP1, CODE_STR_MAX, theCode);
					break;
				}
				}
			}else Err(OP_ErrNOT_GOOD, "compiler->m_TaskStack.size() - 2 < 0");
			break;
		}
		}
	}
	else Compiler_popTask(compiler);
}
void Compiler_Prefix(Compiler* compiler){
	//for assigning func call ret val to var
	if (compiler->m_Pfx == OP_Value && compiler->m_Ch == '@' && !compiler->m_Str[0]) {
		//PushPfxs({ OP_Op }, "", 1);
		Compiler_pushAllowedPfxs(compiler, 1, "", 1, OP_Op);
	}
	compiler->m_Pfx = fromPfxCh(compiler->m_Ch);
	Obj* obj = Compiler_GetObj(compiler);
	if (compiler->m_Pfx != OP_Unknown 
		&& GetAllowedPfxsTop->pfxs.elemCount
		&& !Compiler_isPfxExpected(compiler, compiler->m_Pfx))
		Err(OP_ErrUnexpectedNextPfx, "");
	printf("PFX:%s(%d)\n", GetPfxName(compiler->m_Pfx), (int)compiler->m_Pfx);
	switch (compiler->m_Pfx) {
	case OP_String: { //"
		compiler->m_StringMode = true;
		Compiler_push(compiler, OP_ModeStrPass, false);
		break;
	}
	case OP_VarType:
		//compiler->m_StrReadPtrsStack.push(true);
		IBVectorCopyPushBool(&compiler->m_StrReadPtrsStack, true);
	case OP_Value:
	case OP_Op:
	case OP_Name:
		//getchar();
		Compiler_push(compiler, OP_ModeStrPass, false);
		break;
	case OP_Comment:
		break;
	}
	if (compiler->m_Pfx == OP_Op) {
		//auto& aps = compiler->m_AllowedNextPfxsStack.top();
		AllowedPfxs* aps = GetAllowedPfxsTop;
		if (aps->life && --aps->life <= 0) {
			//compiler->m_AllowedNextPfxsStack.Compiler_pop(compiler);
			IBVectorPop(&compiler->m_AllowedNextPfxsStack);
		}
	}
}
void Compiler_Str(Compiler* compiler){
	char chBuf[2];
	chBuf[0] = compiler->m_Ch;
	chBuf[1] = '\0';
	if (compiler->m_StringMode) {
		switch (compiler->m_Ch) {
		case '"': {
			compiler->m_StringMode = false;
			Compiler_StrPayload(compiler);
			return;
		}
		}
	}
	else {
		switch (compiler->m_Pfx) {
		case OP_Value: {
			switch (compiler->m_Ch) {
			case '@': {
				Compiler_pop(compiler);
				Compiler_Prefix(compiler);
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
			else return Compiler_StrPayload(compiler);
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
	strcat_s(compiler->m_Str, COMPILER_STR_MAX, chBuf);
}
void Compiler_StrPayload(Compiler* compiler){
	printf("Doing Str payload\n");
	Val strVal;
	strVal.i32=atoi(compiler->m_Str);
	compiler->m_NameOp = GetOpFromName(compiler->m_Str);
	printf("Str: %s\n", compiler->m_Str);
	switch (compiler->m_Pfx)
	{
	case OP_String: { //"
		SwitchTaskStackStart
		case OP_FuncWantCode: { //printf
			Compiler_pushTask(compiler, OP_CPrintfHaveFmtStr);
			Obj*o=Compiler_pushObj(compiler);
			ObjSetStr(o, compiler->m_Str);
			ObjSetType(o, OP_CPrintfFmtStr);
			Compiler_popObj(compiler, true);
			Compiler_pushAllowedPfxs(compiler, 0, "expected fmt args or line end", 4, OP_Value, OP_Name, OP_String, OP_LineEnd);
			break;
		}
		SwitchTaskStackEnd
		break;
	}
	case OP_Value: { //=
		switch (GetObjType) {
		case OP_VarWantValue: {
			Compiler_GetObj(compiler)->var.val = strVal;
			SetObjType(OP_VarComplete);
			PopPfxs();
			break;
		}
		}
		if (compiler->m_TaskStack.elemCount) {
			switch (GetTaskType) {
			case OP_CPrintfHaveFmtStr:{
				Compiler_pushObj(compiler);
				//Compiler_GetObj(compiler)->setStr(cs);
				Compiler_GetObj(compiler)->val = strVal;
				ObjSetType(Compiler_GetObj(compiler), OP_Value);
				Compiler_GetObj(compiler)->var.type = OP_i32;//for now
				Compiler_popObj(compiler, true);
				//PopPfxs();
				break;
			}
			case OP_FuncNeedRetVal: {
				Obj* o;
				while (o = (Obj*)IBVectorIterNext(&GetTaskWorkingObjs)) {
					if (ObjGetType(o) == OP_FuncSigComplete) {
						printf("Finishing func got ret value\n");
						o->func.retVal = strVal;
						//SetObjType(OP_CompletedFunction);
						PopPfxs();
						SetTaskType(OP_Func);
						Compiler_PopAndDoTask(compiler);
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
			Compiler_pushObj(compiler);
			Compiler_GetObj(compiler)->var.type = compiler->m_NameOp;
			Compiler_GetObj(compiler)->var.mod = OP_NotSet;
			SetObjType(OP_VarNeedName);
			Compiler_pushAllowedPfxs(compiler, 0,"Expected variable name after variable type", 1, OP_Name);
			break;
		}
		}
		switch (GetObjType) {
		case OP_FuncNeedsRetValType: {
			if (GetTaskType != OP_FuncHasName)Err(OP_ErrNOT_GOOD, "func signature needs name");
			Compiler_GetObj(compiler)->func.retType = compiler->m_NameOp;
			Compiler_GetObj(compiler)->func.retTypeMod = compiler->m_Pointer;
			SetObjType(OP_FuncSigComplete);
			/*if (Compiler_GetObj(compiler)->getMod() == OP_Imaginary) {
				SetTaskType(OP_FuncSigComplete);
			}
			else {
				SetTaskType(OP_FuncWantCode);
			}*/
			//Compiler_popObj(compiler, true);
			/*auto allowed = { OP_Op,OP_String, OP_VarType };
			PopPfxs();
			PopPfxs();
			PushPfxs(allowed, "expected operator, print statement, or variable declaration");*/
			break;
		}
		case OP_FuncHasName:
			Obj*o=Compiler_pushObj(compiler);
			SetObjType(OP_FuncArgNameless);
			o->arg.type= compiler->m_NameOp;
			o->arg.mod = compiler->m_Pointer;
			//PopPfxs();
			//PushPfxs({OP_Name}, "Expected func arg name", 0);
			Compiler_pushAllowedPfxs(compiler, 0,"Expected func arg name", 1, OP_Name);
			break;
		}
		break;
	case OP_Name: { //$
		SwitchTaskStackStart
		case OP_CPrintfHaveFmtStr: {
			Obj*o=Compiler_pushObj(compiler);
			ObjSetName(o, compiler->m_Str);
			ObjSetType(o, OP_Name);
			Compiler_popObj(compiler, true);
			break;
		}
		SwitchTaskStackEnd
		switch (GetObjType) {
		case OP_CallNeedName: { //=@call
			SetObjType(OP_CallWantArgs);
			PopPfxs();
			Compiler_pushAllowedPfxs(compiler, 0,"expected var type or line end after func name", 3, OP_Name, OP_Value, OP_LineEnd);

		}
		case OP_Func: {
			SetObjType(OP_FuncHasName);
			SetTaskType(OP_FuncHasName);
			PopPfxs();
			//auto allowed = { OP_VarType,OP_Op,OP_LineEnd };
			//PushPfxs(allowed, "", 0);
			Compiler_pushAllowedPfxs(compiler, 0,"", 3, OP_VarType, OP_Op, OP_LineEnd/*means allowed pfx will be cleared on newline*/);
			ObjSetName(Compiler_GetObj(compiler), compiler->m_Str);
			break;
		}
		case OP_FuncArgNameless:
			SetObjType(OP_FuncArgComplete);
			PopPfxs();
			ObjSetName(Compiler_GetObj(compiler), compiler->m_Str);
			//compiler->m_NameTypeCtx.add(compiler->m_Str, Compiler_GetObj(compiler)->arg.type);
			NameInfoDBAdd(&compiler->m_NameTypeCtx, compiler->m_Str, Compiler_GetObj(compiler)->arg.type);
			Compiler_popObj(compiler, true);
			break;
		case OP_VarNeedName:
			ObjSetName(Compiler_GetObj(compiler), compiler->m_Str);
			NameInfoDBAdd(&compiler->m_NameTypeCtx, compiler->m_Str, Compiler_GetObj(compiler)->var.type);
			SetObjType(OP_VarWantValue);
			PopPfxs();
			/*auto allowed = { OP_Value, OP_LineEnd };
			PushPfxs(allowed, "expected value or line end after var name", 0);*/
			Compiler_pushAllowedPfxs(compiler, 0,"expected value or line end after var name", 2, OP_Value, OP_LineEnd);
			break;
		}
		break;
	}
	case OP_Op: //@
		switch (compiler->m_NameOp) {
		case OP_Call:{
			switch (GetObjType) {
			case OP_VarWantValue: {
				/*auto& tst = compiler->m_TaskStack.top();
				auto& ost = compiler->m_ObjStack.top();
				auto& pfxs=compiler->m_AllowedNextPfxsStack.top().pfxs;*/
				Obj*o=Compiler_pushObj(compiler);
				ObjSetType(o, OP_CallNeedName);
				//PushPfxs({OP_Name}, "expected function name", 0);
				Compiler_pushAllowedPfxs(compiler, 0,"expected function name", 1, OP_Name);
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
			ObjSetMod(Compiler_GetObj(compiler), compiler->m_NameOp);
			//setAllowedNextPfxs({});
			//PopPfxs();
			break;
		case OP_Done:
			if (!compiler->m_TaskStack.elemCount) Err(OP_ErrNoTask, "");
			switch (GetTaskType) {
			case OP_Func:
			case OP_FuncHasName:
			case OP_FuncWantCode: {
				Obj* o;
				PRINT_LINE_INFO();
				printf(" Finishing function\n");
				while (o = (Obj*)IBVectorIterNext(&GetTaskWorkingObjs)) {
					//TODO: could cache func obj index later
					if (ObjGetType(o) == OP_FuncSigComplete) {
						if (o->func.retType != OP_Void) {
							//PushPfxs({OP_Value},"", 0);
							Compiler_pushAllowedPfxs(compiler, 0,"", 1, OP_Value);
							SetTaskType(OP_FuncNeedRetVal);
						}
						else {
							SetTaskType(OP_Func);
							Compiler_PopAndDoTask(compiler);
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
				Compiler_popObj(compiler, true);
				if (GetObjType != OP_FuncHasName) {
					Err(OP_ErrNOT_GOOD, "expected FuncHasName");
					break;
				}
			}
			case OP_FuncHasName:
				SetObjType(OP_FuncNeedsRetValType);
				//PopPfxs();
				//PushPfxs({ OP_VarType },"", 0);
				Compiler_pushAllowedPfxs(compiler, 0,"", 1, OP_VarType);
				break;
			default:
				Err(OP_ErrUnexpectedOp, "");
				break;
			}
			break;
		}
		case OP_Func:
			//if (GetObjType != OP_NotSet)Err(OP_ErrNOT_GOOD, "");
			//pushObj({});
			SetObjType(compiler->m_NameOp);
			Compiler_GetObj(compiler)->func.retType = OP_Void;
			Compiler_GetObj(compiler)->func.retTypeMod = OP_NotSet;
			//PushPfxs({OP_Name}, "",0);
			Compiler_pushAllowedPfxs(compiler, 0,"", 1, OP_Name);
			Compiler_pushTask(compiler, OP_FuncNeedName);
			break;
		case OP_Public:
		case OP_Private:
			Compiler_GetObj(compiler)->privacy = compiler->m_NameOp;
			break;
		default:
			Err(OP_ErrUnknownOpStr, "");
		}
	}
	//compiler->m_Str.clear();
	compiler->m_Str[0] = '\0';
	printf("Str payload complete\n");
	Compiler_pop(compiler);
	if(compiler->m_StrReadPtrsStack.elemCount > 1)
	{
		if (*(bool*)IBVectorTop(&compiler->m_StrReadPtrsStack)) compiler->m_Pointer = OP_NotSet;
		IBVectorPop(&compiler->m_StrReadPtrsStack);
	}
}
void Compiler_ExplainErr(Compiler* compiler, Op code) {
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
	case OP_ErrUnexpectedNextPfx:
		printf("%s Unexpected next prefix %s. Pfx stack idx:%zd Allowed:", GetAllowedPfxsTop->err, GetPfxName(compiler->m_Pfx), compiler->m_AllowedNextPfxsStack.elemCount - 1);
		Op* oi;
		while (oi = (Op*)IBVectorIterNext(&GetAllowedPfxsTop->pfxs)) {
			printf("%s,", GetPfxName(*oi));
		}
		break;
	case OP_ErrExpectedVariablePfx:
		printf("Expected a variable type to be next.");
		break;
	default:
		printf("Err msg unimplemented for %s", GetOpName(code));
	}
	printf("\nOBJ:");
	ObjPrint(Compiler_GetObj(compiler));
	printf("\n");
}
int main(int argc, char** argv) {
	FILE* f;
	const char* fname = /*argv[1]*/"main.txt";
	if (!fopen_s(&f, fname, "r")){
		Compiler c;
		Compiler_Init(&c);
		while (!feof(f)) {
			char ch = fgetc(f);
			if (ch == 0xffffffff) break;
			Compiler_Char(&c, ch);
		}
		fclose(f);
		return 0;
	}
	else{
		printf("Error\n");
	}
	return 1;
}
void NameInfoDBAdd(NameInfoDB *db, const char* name, Op type){
	NameInfo info;
	info.type = type;
	info.name = _strdup(name);
	IBVectorCopyPush(&db->pairs, &info);
}
Op NameInfoDBFindType(NameInfoDB* db, const char* name){
	NameInfo* pair;
	while (pair = (NameInfo*)IBVectorIterNext(&db->pairs)) {
		if (!strcmp(pair->name, name))
			return pair->type;
	}
	return OP_NotFound;
}
