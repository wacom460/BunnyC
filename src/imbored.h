#ifndef IMBORED_H_
#define IMBORED_H_
#include "ibcommon.h"
#include "ibmisc.h"
#include <libtcc.h>



#define IBDEBUGPRINTS

#ifdef _MSC_VER
#define strdup _strdup
#endif

#define IB_IllegalDbObjNameChars " \t\n,.:~!@#$%^&*=/()[]{}<>?|\\`'\""
#define IBBoolStr(b) (b ? IB_TRUESTR : IBFALSESTR)
#define IBBoolStrChar(b) (b ? "1" : "0")

#ifdef IBDEBUGPRINTS
void _PrintLine(int l);
#define PLINE _PrintLine(__LINE__)
#else
#define PLINE
#endif

#define IBCASE_BLOCKWANTCODE \
case OP_LoopBlockWantCode: \
case OP_CaseWantCode:      \
case OP_BlockWantCode:     \
case OP_IfBlockWantCode:   \
case OP_FuncWantCode:

#define IBCASE_UNIMP                       \
default: {                               \
	Err(OP_Error, "Unimplemented case"); \
	break;                               \
}

#define IBCASE_UNIMP_A default: { \
	DB; \
	break; \
}

#define IB_STARTS_WITH_SELFDOT(o)\
	((strnlen((o), 5)>=5)&&(!strncmp((o),"self.",5)))
#define IB_SELFDOTLESS_NTSP(o) ((o)+5)

#define IBCASE_0THRU9 case '0': case '1': case '2': case '3': \
case '4': case '5': case '6': case '7': case '8': case '9':

#define IBCASE_aTHRUz case 'a': case 'b': case 'c': case 'd': \
case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': \
case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': \
case 'q': case 'r': case 's': case 't': case 'u': case 'v': \
case 'w': case 'x': case 'y': case 'z':

#define IBCASE_ATHRUZ case 'A': case 'B': case 'C': case 'D': \
case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': \
case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': \
case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': \
case 'W': case 'X': case 'Y': case 'Z':

#define IBCASE_AaTHRUZz IBCASE_aTHRUz IBCASE_ATHRUZ
#define IBCASE_0AaTHRU9Zz IBCASE_AaTHRUZz IBCASE_0THRU9

#define IBCASE_NUMTYPES case OP_u8: case OP_c8: case OP_i8: \
	case OP_u16: case OP_i16: case OP_u32: case OP_i32: \
	case OP_f32: case OP_u64: case OP_i64: case OP_d64:

#define IBOP_NAME_LEN 32
#define IBCOMMENT_CHAR ('~') // ~( ~( )~ )~
#define IBCOMMENT_CHAR_OPEN ('(')
#define IBCOMMENT_CHAR_CLOSE (')')
#define IBLayer3STR_MAX 64

#define IB_WARN_ON_REALLOC 0

#if IB_WARN_ON_REALLOC
#define IBREALLOCWARNING\
	DbgFmt("[%s:%d] %s realloc called!\n",__FILE__,__LINE__,__func__);
#else
#define IBREALLOCWARNING
#endif

#define IBREALLOC(nptr,ptr,sz)\
	nptr = realloc(ptr,sz);\
	IBREALLOCWARNING


#define IBASSERT0(x) { \
	if(!(x)) { \
		printf("[%d]Assertion failed!!! %s\n", __LINE__, #x); \
		DB; \
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
	}\
}

#ifdef IBDEBUGPRINTS
#define DbgFmt(x, ...){\
	printf(x, __VA_ARGS__);\
}
#else
#define DbgFmt(x, ...)
#endif

#define DbgPuts(x) printf("%s",x);

#include "ibop.h"

#define CASE_VALTYPES case OP_u8: \
	case OP_i8: case OP_c8: case OP_u16: case OP_i16: \
	case OP_u32: case OP_i32: case OP_u64: case OP_i64:

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
typedef enum IBDictDataType {
	IBDictDataType_Unknown = 0,
	IBDictDataType_RootKey,
	IBDictDataType_VoidPtr,
	IBDictDataType_Int,
	IBDictDataType_String,
} IBDictDataType;
//char* IBDictDataTypeToString(IBDictDataType type);
#define IBDICTKEY_MAXDATASIZE 256
#define IBDICTKEY_KEYSIZE 16
typedef struct IBDictKey {
	IBDictDataType type;
	IBVector children;
	union key {
		//char* str;
		int num;
		char data[IBDICTKEY_KEYSIZE];
	} key;
	union val {
		char data[IBDICTKEY_MAXDATASIZE];
		int num;
	} val;
} IBDictKey;
typedef struct IBDictKeyDef {
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
typedef union IBVal {
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
} IBVal;

typedef struct {
	//IBOp infoType; //OP_Builtin,OP_Custom
	IBOp type;//OP_Enum,OP_Struct,OP_StructVar,OP_Func,OP_i32,OP_c8 etc..
	IBStr name;
	IBVector members;
	struct {
		char isFlags;
	} Enum;
	struct {
		int val;
	} EnumValue;
	struct {
		int flags;
		char placeholder;
	} Struct;
	struct {
		IBOp privacy;
		IBOp type;
	} StructVar;
	struct {
		char isMethod;/*methods are sub functions of types*/
		//todo: store func args info
	} Function;
	IB_DEFMAGIC;
} IBTypeInfo;
void IBTypeInfoInit(IBTypeInfo* ti, IBOp type, char* name);
void IBTypeInfoFree(IBTypeInfo* ti);
void IBTypeInfoFindMember(IBTypeInfo* ti, char* name, IBTypeInfo** outDP);
typedef struct IBNameInfo {
	IBOp type;
	IBTypeInfo*ti;
	IBOp cast;
	char* name;
} IBNameInfo;
typedef struct IBNameInfoDB {
	IBVector pairs;//IBNameInfo
} IBNameInfoDB;
void IBNameInfoInit(IBNameInfo* info);
void IBNameInfoFree(IBNameInfo* info);
void IBNameInfoDBInit(IBNameInfoDB* db);
void IBNameInfoDBFree(IBNameInfoDB* db);
IBOp IBNameInfoDBAdd(struct IBLayer3* ibc, IBNameInfoDB* db, char* name, IBOp type, IBNameInfo** niDP);
IBOp IBNameInfoDBFindType(IBNameInfoDB* db, char* name);
IBNameInfoDB* _IBNameInfoDBFind(IBNameInfoDB* db, char* name, int lineNum);
#define IBNameInfoDBFind(db,name) _IBNameInfoDBFind(db,name,__LINE__)
typedef struct IBCodeBlock {
	IBStr header;
	IBStr variables;
	IBStr varsInit;
	IBStr codeLeft;
	IBStr code;
	IBStr codeRight;
	IBStr footer;
	IBNameInfoDB localVariables;
	IB_DEFMAGIC;
} IBCodeBlock;
void IBCodeBlockInit(IBCodeBlock* block);
void IBCodeBlockFinish(IBCodeBlock* block, IBStr* output);
void IBCodeBlockFree(IBCodeBlock* block);
typedef struct IB_DBObj {
	IBStr fileName;
	int fileLine;
	int fileColumn;
	IBOp type;
	IBStr name;
	IBVector children;/*IB_DBObj*/
} IB_DBObj;
IB_DBObj* IB_DBObjNew(IBStr* fileName, int fileLine, int fileColumn,
	IBOp objType, IBStr* objName);
void IB_DBObjFree(IB_DBObj* obj);
typedef struct IBDatabase {
	IB_DBObj* root;
} IBDatabase;
extern IBDatabase* g_DB;
void IBDatabaseInit(IBDatabase* db);
void IBDatabaseFree(IBDatabase* db);
IB_DBObj* IBDatabaseFind(IBDatabase* db, IBStr location);
char* IBGetOpName(IBOp op);
typedef struct IBObj {
	IBOp type;
	IBOp modifier;
	IBOp valType;
	IBOp privacy;
	char* name;
	char* str;
	IBVal val;
	struct _IBFuncObj {
		IBOp retTYPE;
		IBOp retValType;
		IBOp retValVarcast;
		IBOp retTypeMod;
		char* retStr;
		char* retValStr;
		IBVal retVal;
		struct IBTask* thingTask;//stupid
	} func;
	struct _IBIfObj {
		IBOp lvTYPE;
		IBOp lvDataType;
		IBOp lvMod;
		IBOp midOp;
		IBOp rvTYPE;
		IBOp rvDataType;
		IBOp rvMod;
		IBVal lvVal;
		IBVal rvVal;
		char* lvName;
		char* rvName;
	} ifO;
	struct _IBArgObj {
		IBOp type;
		IBOp mod;
		IBVector arrIndexExprs;//IBStr
	} arg;
	struct _IBVarObj {
		IBOp type;
		IBTypeInfo*ti;
		IBOp mod;
		IBOp privacy;
		bool valSet;
		IBVal val;
		char* valStrLiteral;
	} var;
	struct _IBForObj {
		char* startName;
		IBVal start, end;
		IBVal step;
	} forO;
	struct _IBEnumObj {
		bool flags;
	} enumO;
	struct _IBTableObj {
		bool fallthru;
	} table;
} IBObj;
void _ObjSetType(IBObj* obj, IBOp type);
#define ObjSetType(obj, type){\
	PLINE;\
	_ObjSetType(obj, type);\
}
void ObjSetMod(IBObj* obj, IBOp mod);
void _ObjSetName(IBObj* obj, char* name);
#define ObjSetName(obj, name){\
	PLINE;\
	_ObjSetName(obj,name);\
}
void ObjSetStr(IBObj* obj, char* Str);
void ObjCopy(IBObj* dst, IBObj* src);
void ObjPrint(IBObj* obj);
void ObjInit(IBObj* o);
void ObjFree(IBObj* o);
void Val2Str(char* dest, int destSz, IBVal v, IBOp type);
typedef struct IBExpects {
	IBVector pfxs;/*IBOp P */
	IBVector nameOps;/*IBOp N */
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
typedef struct IBTaskNeedExpression {
	IBOp finalVartype;
} IBTaskNeedExpression;
typedef struct IBTask {
	IBOp type;
	IBCodeBlock code;
	IBVector expStack; /*IBExpects*/
	IBVector working;/*IBObj*/
	IBVector subTasks;/*IBTask*/
	IBTaskNeedExpression exprData;
} IBTask;
void TaskInit(IBTask* t, IBOp type);
void TaskFree(IBTask* t);
void TaskFindWorkingObj(IBTask*t,IBOp type, IBObj**outDP);
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
	IBOp Pfx;
	IBOp Pointer;
	IBOp Privacy;
	IBOp NameOp;
	IBOp LastNameOp;
	IBOp Varcast;
	IBOp CommentMode;
	int Line;
	int Column;
	int LineIS; //LINE inputstr
	int ColumnIS;//COLUMN inputstr
	IBStr CIncludesStr;
	IBStr CHeader_Structs;
	IBStr CHeader_Funcs;
	IBStr CCode;
	IBStr FinalOutput;
	IBStr CurrentLineStr;
	IBStr RunArguments;
	//IBStr ArrayIndexExprStr;
	IBVector ArrayIndexExprsVec;//IBStr
	IBVector TypeRegistry;//IBTypeInfo

	IBVector ObjStack; /*IBObj*/
	IBVector ModeStack; /*IBOp*/
	IBVector TaskStack; /*IBTask*/
	IBVector StrReadPtrsStack; /*bool*/
	IBVector CodeBlockStack; /*IBCodeBlock*/
	IBVector ExpressionStack; /*IBExpression*/
	IBNameInfoDB GlobalVariables;

	char* InputStr;
	IBStr CurSpace;
	char Ch;
	char LastCh;
	char DefiningStruct;
	char DefiningMethods;

	//[DOT PATH]
	//obj.thing.whatever ->
	//obj
	//thing
	//whatever
	char DotPathOn;
	IBVector DotPathVec;//IBStr

	char*_methodsStructName;
	//char DefiningEnum;
	bool Imaginary;
	bool Running;
	bool StringMode;
	bool StrAllowSpace;
	bool IncludeCStdioHeader;
	bool IncludeCStdlibHeader;
	TCCState* TCC;
	char Str[IBLayer3STR_MAX];
} IBLayer3;
#define Err(code, msg) { \
	int l = ibc->InputStr ? ibc->LineIS : ibc->Line; \
	int c = ibc->InputStr ? ibc->ColumnIS : ibc->Column; \
	IBPushColor(IBFgRED); \
	PLINE; \
	if (ibc->InputStr) printf("ERROR IN InputStr!!!\n"); \
	printf("Error at %u:%u \"%s\"(%d). %s\n", \
		l, c, IBGetOpName(code), (int)code, msg); \
	IBLayer3ExplainErr(ibc, code); \
	IBPopColor(); \
	DB; \
}
#define ErrF(code, fmt, ...) { \
	char str[512]; \
	sprintf_s(str, sizeof(str), fmt, __VA_ARGS__); \
	Err(code, str); \
}
void IBLayer3Init(IBLayer3* ibc);
void IBLayer3Free(IBLayer3* ibc);
void IBLayer3RegisterCustomType(IBLayer3*ibc,char*name, 
	IBOp type,/*OP_Enum,OP_Struct,OP_FuncPtr*/
	IBTypeInfo**outDP);
void IBLayer3FindType(IBLayer3*ibc,char*name,IBTypeInfo**outDP);
void IBLayer3CompileTCC(IBLayer3* ibc);
IBObj* IBLayer3GetObj(IBLayer3* ibc);
IBNameInfo* _IBLayer3SearchNameInfo(IBLayer3* ibc, char* name, int ln);
#define IBLayer3SearchNameInfo(ibc,name)\
	_IBLayer3SearchNameInfo(ibc,name,__LINE__)
void IBLayer3PrintVecData(struct IBVecData* data, IBOp type);
void IBLayer3VecPrint(IBVector* vec);
IBObj* IBLayer3FindStackObjRev(IBLayer3*ibc,IBOp type);
IBObj* IBLayer3FindStackObjUnderTop(IBLayer3* ibc, IBOp type);
IBObj* IBLayer3FindStackObjUnderIndex(IBLayer3* ibc, int index, IBOp type);
IBTask* IBLayer3FindTaskUnderIndex(IBLayer3* ibc, int index, IBOp type, int limit);
int IBLayer3GetTabCount(IBLayer3* ibc);
void IBLayer3Done(IBLayer3* ibc);
IBObj* IBLayer3FindWorkingObj(IBLayer3* ibc, IBOp type);
IBObj* IBLayer3FindWorkingObjRev(IBLayer3* ibc, IBOp type);
IBObj* IBLayer3FindWorkingObjUnderIndex(IBLayer3* ibc, int index, IBOp type);
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
void _IBLayer3PushTask(IBLayer3* ibc, IBOp taskOP,
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
void _IBLayer3PushObj(IBLayer3* ibc, IBObj** o);
#define IBLayer3PushObj(ibc, objDP){\
	PLINE;\
	_IBLayer3PushObj(ibc, objDP);\
}
#define GetObjCount (ibc->ObjStack.elemCount)
void _IBLayer3PopObj(IBLayer3* ibc, bool pushToWorking, IBObj** objDP);
#define IBLayer3PopObj(ibc, p2w, objDP){\
	PLINE;\
	_IBLayer3PopObj(ibc, p2w, objDP);\
}
void _IBLayer3Push(IBLayer3* ibc, IBOp mode, bool strAllowSpace);
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

bool IBLayer3IsPfxExpected(IBLayer3* ibc, IBOp pfx);
bool IBLayer3IsNameOpExpected(IBLayer3* ibc, IBOp nameOp);
/*NO NEWLINES AT END OF STR*/
void IBLayer3Tick(IBLayer3* ibc, struct FILE* f);
void IBLayer3InputChar(IBLayer3* ibc, char ch);
void IBLayer3InputStr(IBLayer3* ibc, char* str);
void _IBLayer3FinishTask(IBLayer3* ibc);
#define IBLayer3FinishTask(ibc){\
	PLINE;\
	_IBLayer3FinishTask(ibc);\
}
IBVal IBLayer3StrToVal(IBLayer3* ibc, char* str, IBOp expectedType);
char* IBLayer3GetCPrintfFmtForType(IBLayer3* ibc, IBOp type);
void IBLayer3Prefix(IBLayer3* ibc);
void IBLayer3Str(IBLayer3* ibc);
void IBLayer3StrPayload(IBLayer3* ibc);
void IBLayer3ExplainErr(IBLayer3* ibc, IBOp code);
#define SetObjType(obj, tt){\
	PLINE; \
	DbgFmt(" SetObjType: %s(%d) -> %s(%d)\n", \
		IBGetOpName(obj->type), (int)obj->type, \
		IBGetOpName(tt), (int)tt); \
	obj->type=tt; \
}
#define PopExpects(){\
	PLINE;\
	IBLayer3PopExpects(ibc);\
}
IBTask* IBLayer3GetTask(IBLayer3* ibc);
IBOp IBLayer3GetMode(IBLayer3* ibc);

IBExpects* IBTaskGetExpTop(IBTask* t);
IBVector* IBTaskGetExpPfxsTop(IBTask* t);
IBVector* IBTaskGetExpNameOPsTop(IBTask* t);
IBOp IBStrToBool(IBLayer3* ibc, char* str);
IBOp IBJudgeTypeOfStrValue(IBLayer3* ibc, char* str);

#define SetTaskType(task, tt){\
	assert(task);\
	PLINE;\
	DbgFmt(" SetTaskType: %s(%d) -> %s(%d)\n", \
		IBGetOpName(task->type), \
		(int)task->type, \
		IBGetOpName(tt), (int)tt);\
	task->type = tt;\
}

typedef struct IBOpNamePair {
	char name[IBOP_NAME_LEN];
	IBOp op;
} IBOpNamePair;
char* IBGetCEqu(IBOp op);
char* IBGetOpName(IBOp op);
char* IBGetPfxName(IBOp op);
IBOp IBGetOpFromNameList(char* name, IBOp list);
IBOp IBOPFromPfxCh(char ch);
void IBOverwriteStr(char** str, char* with);

#endif
