#ifndef IMBORED_H_
#define IMBORED_H_

#define IBDEBUGPRINTS

#ifdef _WIN32
#include <Windows.h>
#define atoll _atoi64
#endif
#define bool char
#define true 1
#define false 0
#define IB_TRUESTR "true"
#define IBFALSESTR "false"
#define IB_FILEEXT "3"
#define IB_IllegalDbObjNameChars " \t\n,.:~!@#$%^&*=/()[]{}<>?|\\`'\""
#define BoolStr(b) (b ? IB_TRUESTR : IBFALSESTR)
#define BoolStrChar(b) (b ? "1" : "0")
#ifdef _MSC_VER
#define strdup _strdup
#endif

#ifdef _WIN32
typedef enum IBColor {
	//fg
	IBFgWHITE = FOREGROUND_RED
	| FOREGROUND_GREEN
	| FOREGROUND_BLUE,
	IBFgRED = FOREGROUND_RED,
	IBFgGREEN = FOREGROUND_GREEN,
	IBFgBLUE = FOREGROUND_BLUE,
	IBFgYELLOW = FOREGROUND_RED | FOREGROUND_GREEN,
	IBFgCYAN = FOREGROUND_BLUE | FOREGROUND_GREEN,
	IBFgMAGENTA = FOREGROUND_RED | FOREGROUND_BLUE,
	IBFgBROWN = FOREGROUND_RED | FOREGROUND_GREEN,
	IBFgIntensity = FOREGROUND_INTENSITY,

	//bg
	IBBgWHITE = BACKGROUND_RED
	| BACKGROUND_GREEN
	| BACKGROUND_BLUE,
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

#define CASE_BLOCKWANTCODE \
case OP_LoopBlockWantCode: \
case OP_CaseWantCode:      \
case OP_BlockWantCode:     \
case OP_IfBlockWantCode:   \
case OP_FuncWantCode:

#define CASE_UNIMP                       \
default: {                               \
	Err(OP_Error, "Unimplemented case"); \
	break;                               \
}

#ifdef IBDEBUGPRINTS
void _PrintLine(int l);
#define PLINE _PrintLine(__LINE__)
#else
#define PLINE
#endif
#define CASE_0THRU9 case '0': case '1': case '2': case '3': \
case '4': case '5': case '6': case '7': case '8': case '9':
#define CASE_aTHRUz case 'a': case 'b': case 'c': case 'd': \
case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': \
case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': \
case 'q': case 'r': case 's': case 't': case 'u': case 'v': \
case 'w': case 'x': case 'y': case 'z':
#define CASE_ATHRUZ case 'A': case 'B': case 'C': case 'D': \
case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': \
case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': \
case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': \
case 'W': case 'X': case 'Y': case 'Z':

#define OP_NAME_LEN 32
#define COMMENT_CHAR ('~')
#define IBLayer3STR_MAX 64
#define ThingStructTypeHeaderVarType ("int")
#define ThingStructTypeHeaderVarName ("__thingTYPE")
#if defined(__TINYC__) || defined(__GNUC__)
#define __debugbreak()
#endif
#define DB __debugbreak();

#define assert0(x) { \
	if(!(x)) { \
		printf("[%d]Assertion failed!!! %s\n", __LINE__, #x); \
		DB; \
		exit(-1); \
	} \
}

#define IBASSERT(x, errMsg){\
	if(!(x)) {\
		PLINE;\
		IBPushColor(IBFgRED);\
		printf("Assertion failed!!! -> %s\n%s", \
			errMsg, #x);\
		IBPopColor();\
		DB;\
		exit(-1);\
	}\
}
#define assert(x) IBASSERT(x, "")

#ifdef IBDEBUGPRINTS
#define DbgFmt(x, ...){\
	printf(x, __VA_ARGS__);\
}
#else
#define DbgFmt(x, ...)
#endif

#define CASE_UNIMP_A default: { \
	DB; \
	exit(-1); \
	break; \
}

#define _IB_OPS_ \
X(Null) \
X(False) \
X(True) \
X(OK) \
X(AlreadyExists) \
X(Unknown) \
X(NotSet) \
X(Any) \
X(Use) \
X(Build) \
X(Call) \
X(Case) \
X(Output) \
X(CallWantArgs) \
X(Space) \
X(Enum) \
X(Flags) \
X(Func) \
X(FuncHasName) \
X(FuncNeedName) \
X(FuncNeedsRetValType) \
X(FuncArgsVarNeedsName) \
X(FuncArgNameless) \
X(FuncArgComplete) \
X(FuncSigComplete) \
X(FuncNeedRetVal) \
X(FuncArg) \
X(CompletedFunction) \
X(VarNeedName) \
X(VarWantValue) \
X(VarComplete) \
X(BlockReturnNeedValue) \
X(ArgNeedValue) \
X(Ref) \
X(Deref) \
X(DoubleDeref) \
X(TrippleDeref) \
X(Arg) \
X(Op) \
X(Value) \
X(Return) \
X(NoChange) \
X(Struct) \
X(VarType) \
X(LineEnd) \
X(Comment) \
X(MultiLineComment) \
X(Public) \
X(Private) \
X(Imaginary) \
X(Void) \
X(Colon) \
X(Dot) \
X(Add) \
X(Subtract) \
X(Multiply) \
X(Divide) \
X(AddEq) \
X(SubEq) \
X(MultEq) \
X(DivEq) \
X(Equals) \
X(NotEquals) \
X(LessThan) \
X(GreaterThan) \
X(LessThanOrEquals) \
X(GreaterThanOrEquals) \
X(CPrintfHaveFmtStr) \
X(BracketOpen) \
X(BracketClose) \
X(ParenthesisOpen) \
X(ParenthesisClose) \
X(CurlyBraceOpen) \
X(CurlyBraceClose) \
X(TaskStackEmpty) \
X(RootTask) \
X(StructWantName) \
X(StructWantContent) \
X(StructWantRepr) \
X(SpaceNeedName) \
X(SpaceHasName) \
X(Placeholder) \
X(StructInit) \
X(NameOps) \
X(Obj) \
X(Bool) \
X(Task) \
X(IBColor) \
X(Repr) \
X(Table) \
X(IfNeedLVal) \
X(IfNeedMidOP) \
X(IfNeedRVal) \
X(IfFinished) \
X(IBCodeBlock) \
X(StructInitNeedName) \
X(Exclaim) \
X(Caret) \
X(Underscore) \
X(YouCantUseThatHere) \
X(TableNeedExpr) \
X(TableCaseNeedExpr) \
X(TableWantCase) \
X(CaseWantCode) \
X(EnumNeedName) \
X(EnumWantContent) \
X(IfBlockWantCode) \
X(BlockWantCode) \
X(LoopBlockWantCode) \
X(FuncWantCode) \
X(SpaceChar) \
X(Comma) \
X(CommaSpace) \
X(Name) \
X(As) \
X(ExprToName) \
X(String) \
X(TableCase) \
X(CPrintfFmtStr) \
X(Char) \
X(If) \
X(Else) \
X(For) \
X(Loop) \
X(Block) \
X(Fall) \
X(Number) \
X(c8) \
X(CString) \
X(u8) \
X(u16) \
X(u32) \
X(u64) \
X(i8) \
X(i16) \
X(i32) \
X(i64) \
X(f32) \
X(d64) \
X(Float) \
X(Double) \
X(Pointer) \
X(Break) \
X(DoublePointer) \
X(TripplePointer) \
X(IBLayer3Flags) \
X(NeedExpression) \
X(dbgBreak) \
X(dbgAssert) \
X(dbgAssertWantArgs) \
X(CallFunc) \
X(TaskType) \
X(TaskStack) \
X(NotEmpty) \
X(TabChar) \
X(NeedName) \
X(UseNeedStr) \
X(UseStrSysLib) \
X(NameInfoDB) \
X(NameInfo) \
X(Expects) \
X(CodeBlockCallFunc) \
X(ElseIf) \
X(EmptyStr) \
X(BuildingIf) \
X(SubtaskArgs) \
X(FloatingPoint) \
X(Modulo) \
X(Int) \
/*X(SetNeedName) \
X(SetNeedVal) \ */ \
X(NotFound) \
X(Error) \
X(ErrUnexpectedNextPfx) \
X(ErrExpectedVariablePfx) \
X(ErrNoTask) \
X(ErrUnexpectedOp) \
X(ErrQuadriplePointersNOT_ALLOWED) \
X(ErrUnknownOpStr) \
X(ErrProtectedSlot) \
X(ErrUnknownPfx) \
X(ErrUnexpectedNameOP) \
X(ErrDirtyTaskStack) \
X(ModePrefixPass) \
X(ModeStrPass) \
X(ModeComment) \
X(ModeMultiLineComment) \
X(ActOnName) \
X(ActOnNameEquals) \
X(RootObj) \
X(DBObj) \
X(PfxlessValue) \
X(IBDictKeyDef) \
X(EnumName) \
X(IBDictKey) \
X(Letter_azAZ) \
X(DataTypes) \
X(IBExpression) \
X(ForNeedStartValName) \
X(ForNeedStartInitVal) \
X(ForNeedMiddleExpr) \
X(ForNeedEndExpr) \
X(None) \

#define X(x) OP_##x,
typedef enum Op { /* multiple uses */
	_IB_OPS_
} Op;
#undef X

#define CASE_VALTYPES case OP_u8: \
	case OP_i8: case OP_c8: case OP_u16: case OP_i16: \
	case OP_u32: case OP_i32: case OP_u64: case OP_i64:

#define CLAMP_IMP\
	return val < min ? min : val > max ? max : val;
#define CLAMP_FUNC(type, name)\
	type name(type val, type min, type max)
CLAMP_FUNC(int, ClampInt);
CLAMP_FUNC(size_t, ClampSizeT);
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
size_t IBStrGetLen(IBStr* str);
void IBStrAppendCh(IBStr* str, char ch, int count);
char* IBStrAppendCStr(IBStr* str, char* with);
void IBStrAppendFmt(IBStr* str, char* fmt, ...);
char* IBStrAppend(IBStr* str, IBStr* with);
int IBStrStripFront(IBStr* str, char ch);
typedef union IBVecData {
	void* ptr;
	struct Obj* obj;
	struct IBTask* task;
	Op* op;
	bool* boolean;
	struct IBExpects* expects;
	struct IBNameInfoDB* niDB;
	struct IBNameInfo* ni;
	struct IBDictionary* dict;
	struct IBDictKey* dictKey;
	struct IBDictKeyDef* dictKeyDef;
} IBVecData;
typedef struct IBVector {
	size_t elemSize;
	Op type;
	int elemCount;
	int slotCount;
	size_t dataSize;
	//do not expect pointers to stay valid, realloc is called on change
	IBVecData* data;/*DATA BLOCK*/
} IBVector;
void IBVectorInit(IBVector* vec, size_t elemSize, Op type);
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
#define IBDStr "s"
#define IBDNum "d"
#define IBDInPtr "i"
#define IBDOutPtr "o"
#define IBDCount "c"
#define IBDInStr "z"
#define IBDInNum "x"
#define IBDOutStr "j"
#define IBDOutNum "k"
#define IBDOutKey "g"
typedef enum {
	IBDictDataType_Unknown = 0,
	IBDictDataType_RootKey,
	IBDictDataType_VoidPtr,
	IBDictDataType_Int,
	IBDictDataType_String,
} IBDictDataType;
//char* IBDictDataTypeToString(IBDictDataType type);
#define IBDICTKEY_MAXDATASIZE 256
#define IBDICTKEY_KEYSIZE 16
typedef struct {
	IBDictDataType type;
	IBVector children;
	union {
		//char* str;
		int num;
		char data[IBDICTKEY_KEYSIZE];
	} key;
	union {
		char data[IBDICTKEY_MAXDATASIZE];
		int num;
	} val;
} IBDictKey;
typedef struct {
	IBDictDataType type;
	IBDictKey* key;
	union {
		char* str;
		int num;
	};
} IBDictKeyDef;
void IBDictKeyInit(IBDictKey* key, IBDictKeyDef def);
void IBDictKeyInitRoot(IBDictKey* key);
void IBDictKeyFree(IBDictKey* key);
//recursive
IBDictKey* IBDictKeyFind(IBDictKey* rootKey, IBVector* keyDefStack);
IBDictKey* IBDictKeyFindChild(IBDictKey* key, IBDictKeyDef def);
//childDepth must be ptr to an int set to 0
void IBDictKeyPrint(IBDictKey* key, int* childDepth);

//typedef struct IBDictionary {
//	IBDictKey rootKey;
//} IBDictionary;
//void IBDictionaryInit(IBDictionary* dict);
//void IBDictionaryFree(IBDictionary* dict);
//IBDictKey* IBDictFind(IBDictionary* dict, IBVector* keyStack);

/*
* IBDictManip fmt charOPs:
*
* KEYS:
* s - string
* d - int
*
* READ/WRITE:
// i and o apply to previous charOPs
* i - in read ptr (count required)
* o - out write ptr (count required)
* c - count
*
* z - in char* (null terminated)
* x - in int
* j - out char* (null terminated)
* k - out int*
* g - out IBDictKey*
*
* EXAMPLES:
* IBDictManip(dict, "ddsx", 0, 0, "id", 1); //write 1 to "0.0.id"
*
* int i;
* IBDictManip(dict, "ddsk", 0, 0, "id", &i); //read 1 from "0.0.id"
*
*
	IBDictionary dict;
	IBDictionaryInit(&dict);

	//folder.file.100.info.date = 19910420
	IBDictKey* k = IBDictManip(&dict,
		IBDStr    IBDStr  IBDNum IBDStr  IBDStr  IBDInNum,
		"folder", "file", 100,   "info", "date", 19910420
	);
*/

IBDictKey* IBDictManip(IBDictKey* rootKey, char* fmt, ...);

//key = IBDictGet(dict, "0.0.id");
IBDictKey* IBDictGet(IBDictKey* rootKey, char* keyPath);
void IBDictTest();
/* GLOBAL COLOR STACK */
extern IBVector g_ColorStack; /*IBColor*/

char* StrConcat(char* dest, int count, char* src);
char StrStartsWith(char* str, char* with);
typedef union {
	bool boolean;
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
typedef struct IBNameInfo {
	Op type;
	char* name;
} IBNameInfo;
typedef struct IBNameInfoDB {
	IBVector pairs;//IBNameInfo
} IBNameInfoDB;
void IBNameInfoInit(IBNameInfo* info);
void IBNameInfoFree(IBNameInfo* info);
void IBNameInfoDBInit(IBNameInfoDB* db);
void IBNameInfoDBFree(IBNameInfoDB* db);
Op IBNameInfoDBAdd(struct IBLayer3* ibc, IBNameInfoDB* db, char* name, Op type, IBNameInfo** niDP);
Op IBNameInfoDBFindType(IBNameInfoDB* db, char* name);
IBNameInfoDB* IBNameInfoDBFind(IBNameInfoDB* db, char* name);
typedef struct IBCodeBlock {
	IBStr header;
	IBStr variables;
	IBStr varsInit;
	IBStr code;
	IBStr footer;
	IBNameInfoDB localVariables;
} IBCodeBlock;
void IBCodeBlockInit(IBCodeBlock* block);
void IBCodeBlockFinish(IBCodeBlock* block, IBStr* output);
void IBCodeBlockFree(IBCodeBlock* block);
typedef struct IB_DBObj {
	IBStr fileName;
	int fileLine;
	int fileColumn;
	Op type;
	IBStr name;
	IBVector children;/*IB_DBObj*/
} IB_DBObj;
IB_DBObj* IB_DBObjNew(IBStr* fileName, int fileLine, int fileColumn,
	Op objType, IBStr* objName);
void IB_DBObjFree(IB_DBObj* obj);
typedef struct IBDatabase {
	IB_DBObj* root;
} IBDatabase;
extern IBDatabase* g_DB;
void IBDatabaseInit(IBDatabase* db);
void IBDatabaseFree(IBDatabase* db);
IB_DBObj* IBDatabaseFind(IBDatabase* db, IBStr location);
typedef struct FuncObj {
	Op retTYPE;
	char* retStr;
	Val retVal;
	char* retValStr;
	Op retValType;
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
typedef struct TableObj {
	bool fallthru;
} TableObj;
typedef struct EnumObj {
	bool flags;
} EnumObj;
typedef struct ForObj {
	char* startName;
	Val start, end;
	Val step;
} ForObj;
char* GetOpName(Op op);
typedef struct Obj {
	Op type;
	Op modifier;
	Op privacy;
	char* name;
	char* str;
	union {
		FuncObj func;
		VarObj var;
		ArgObj arg;
		IfObj ifO;
		TableObj table;
		EnumObj enumO;
		ForObj forO;
	};
	Val val;
	Op valType;
} Obj;
void _ObjSetType(Obj* obj, Op type);
#define ObjSetType(obj, type){\
	PLINE;\
	_ObjSetType(obj, type);\
}
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
void Val2Str(char* dest, int destSz, Val v, Op type);
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
*
* PUT AT END OF FMT STR: e.g. "PPPNNc" "PPPe"
* 'c': code block macro (adds OP_Op, OP_If, OP_VarType... etc)
* 'e': expression macro
*/
#define ExpectsInit(exp, fmt, ...) \
	_ExpectsInit(__LINE__, exp, fmt, __VA_ARGS__);
void ExpectsPrint(IBExpects* exp);
void ExpectsFree(IBExpects* exp);
typedef struct TaskNeedExpression {
	Op finalVartype;
} TaskNeedExpression;
typedef struct IBTask {
	Op type;
	IBCodeBlock code;
	IBVector expStack; /*IBExpects*/
	IBVector working;/*Obj*/
	IBVector subTasks;/*IBTask*/
	TaskNeedExpression exprData;
} IBTask;
void TaskInit(IBTask* t, Op type);
void TaskFree(IBTask* t);
/*IBSharedState: shared state between
	multiple IBLayer3 instances*/
	//TODO: make race condition safe
typedef struct IBSharedState {
	//cant define output settings in more than one file...
	bool outputSettingsDefined;
} IBSharedState;
typedef struct IBExpression {
	IBCodeBlock cb;
} IBExpression;
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
	IBVector ExpressionStack; /*IBExpression*/

	char* InputStr;
	IBStr CurSpace;
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
	//IBNameInfoDB NameTypeCtx;
} IBLayer3;
#define Err(code, msg) { \
	int l = ibc->InputStr ? ibc->LineIS : ibc->Line; \
	int c = ibc->InputStr ? ibc->ColumnIS : ibc->Column; \
	IBPushColor(IBFgRED); \
	PLINE; \
	if (ibc->InputStr) printf("ERROR IN InputStr!!!\n"); \
	printf("Error at %u:%u \"%s\"(%d). %s\n", \
		l, c, GetOpName(code), (int)code, msg); \
	IBLayer3ExplainErr(ibc, code); \
	IBPopColor(); \
	DB \
	exit(-1); \
}
#define ErrF(code, fmt, ...) { \
	char str[512]; \
	sprintf_s(str, sizeof(str), fmt, __VA_ARGS__); \
	Err(code, str); \
}
void IBLayer3Init(IBLayer3* ibc);
void IBLayer3Free(IBLayer3* ibc);
Obj* IBLayer3GetObj(IBLayer3* ibc);
IBNameInfo* IBLayer3SearchNameInfo(IBLayer3* ibc, char* name);
void IBLayer3PrintVecData(IBVecData* data, Op type);
void IBLayer3VecPrint(IBVector* vec);
Obj* IBLayer3FindStackObjUnderTop(IBLayer3* ibc, Op type);
Obj* IBLayer3FindStackObjUnderIndex(IBLayer3* ibc, int index, Op type);
IBTask* IBLayer3FindTaskUnderIndex(IBLayer3* ibc, int index, Op type, int limit);
int IBLayer3GetTabCount(IBLayer3* ibc);
void IBLayer3Done(IBLayer3* ibc);
Obj* IBLayer3FindWorkingObj(IBLayer3* ibc, Op type);
IBCodeBlock* IBLayer3CodeBlocksTop(IBLayer3* ibc);
void _IBLayer3PushCodeBlock(IBLayer3* ibc, IBCodeBlock** cbDP);
#define IBLayer3PushCodeBlock(ibc, cbDP){\
	PLINE;\
	_IBLayer3PushCodeBlock(ibc, cbDP);\
}
void _IBLayer3PopCodeBlock(IBLayer3* ibc,
	bool copyToParent, IBCodeBlock** cbDP);
#define IBLayer3PopCodeBlock(ibc, copyToParent, cbDP){\
	PLINE;\
	_IBLayer3PopCodeBlock(ibc, copyToParent, cbDP);\
}
void _IBLayer3PushTask(IBLayer3* ibc, Op taskOP,
	IBExpects** exectsDP, IBTask** taskDP);
#define IBLayer3PushTask(ibc, taskOP, exectsDP, taskDP){\
	PLINE;\
	_IBLayer3PushTask(ibc, taskOP, exectsDP, taskDP);\
}
void _IBLayer3PopTask(IBLayer3* ibc, IBTask** taskDP, bool popToParent);
#define IBLayer3PopTask(ibc, taskDP, popToParent){\
	PLINE;\
	_IBLayer3PopTask(ibc, taskDP, popToParent);\
}
void _IBLayer3PushObj(IBLayer3* ibc, Obj** o);
#define IBLayer3PushObj(ibc, objDP){\
	PLINE;\
	_IBLayer3PushObj(ibc, objDP);\
}
#define GetObjCount (ibc->ObjStack.elemCount)
void _IBLayer3PopObj(IBLayer3* ibc, bool pushToWorking, Obj** objDP);
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
void IBLayer3Tick(IBLayer3* ibc, struct FILE* f);
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
	PLINE; \
	DbgFmt(" SetObjType: %s(%d) -> %s(%d)\n", \
		GetOpName(obj->type), (int)obj->type, \
		GetOpName(tt), (int)tt); \
	obj->type=tt; \
}
#define PopExpects(){\
	PLINE;\
	IBLayer3PopExpects(ibc);\
}
IBTask* IBLayer3GetTask(IBLayer3* ibc);
Op IBLayer3GetMode(IBLayer3* ibc);

IBExpects* IBTaskGetExpTop(IBTask* t);
IBVector* IBTaskGetExpPfxsTop(IBTask* t);
IBVector* IBTaskGetExpNameOPsTop(IBTask* t);
Op IBStrToBool(IBLayer3* ibc, char* str);
Op IBJudgeTypeOfStrValue(IBLayer3* ibc, char* str);

#define SetTaskType(task, tt){\
	assert(task);\
	PLINE;\
	DbgFmt(" SetTaskType: %s(%d) -> %s(%d)\n", \
		GetOpName(task->type), \
		(int)task->type, \
		GetOpName(tt), (int)tt);\
	task->type = tt;\
}

typedef struct OpNamePair {
	char name[OP_NAME_LEN];
	Op op;
} OpNamePair;
char* GetCEqu(Op op);
char* GetOpName(Op op);
char* GetPfxName(Op op);
//Op GetOpFromName(char* name);
Op GetOpFromNameList(char* name, Op list);
Op fromPfxCh(char ch);
void OverwriteStr(char** str, char* with);

#endif