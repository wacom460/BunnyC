#pragma once

#define IBDEBUGPRINTS

#include "common.h"
#include "color.h"

#include <libtcc.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifdef _MSC_VER
#define strdup _strdup
#endif

#define IBBoolStr(b) (b ? IB_TRUESTR : IBFALSESTR)
#define IBBoolStrChar(b) (b ? "1" : "0")

extern char* SysLibCodeStr;

#ifdef IBDEBUGPRINTS
void _PrintLine(char* f, int l);
#define PLINE _PrintLine(__FILE__, __LINE__)
#define PLINE_FMT(fmt, ...) PLINE; printf(fmt, __VA_ARGS__)
#else
#define PLINE
#define PLINE_FMT
#endif

#define IBRUN_MAXARGS 10

#define IBCASE_BLOCKWANTCODE \
	case OP_LoopBlockWantCode: \
	case OP_CaseWantCode: \
	case OP_BlockWantCode: \
	case OP_IfBlockWantCode: \
	case OP_FuncWantCode:

#define IBCASE_UNIMPLEMENTED \
	default: \
	{ \
		Err(OP_Error, "Unimplemented case"); \
		break; \
	}

#define IBCASE_UNIMP_A \
	default: \
	{ \
		DB; \
		break; \
	}

#define IB_STARTS_WITH_SELFDOT(o) \
	((strnlen((o), 5) >= 5) && (!strncmp((o),"self.", 5)))

#define IB_SELFDOTLESS_NTSP(o) ((o) + 5)

#define IBCASE_0THRU9 \
	case '0': case '1': case '2': case '3': \
	case '4': case '5': case '6': case '7': case '8': case '9':

#define IBCASE_aTHRUz \
	case 'a': case 'b': case 'c': case 'd': \
	case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': \
	case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': \
	case 'q': case 'r': case 's': case 't': case 'u': case 'v': \
	case 'w': case 'x': case 'y': case 'z':

#define IBCASE_ATHRUZ \
	case 'A': case 'B': case 'C': case 'D': \
	case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': \
	case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': \
	case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': \
	case 'W': case 'X': case 'Y': case 'Z':

#define IBCASE_AaTHRUZz \
	IBCASE_aTHRUz IBCASE_ATHRUZ

#define IBCASE_0AaTHRU9Zz \
	IBCASE_AaTHRUZz IBCASE_0THRU9

#define IBCASE_NUMTYPES \
	case OP_u8: case OP_c8: case OP_i8: \
	case OP_u16: case OP_i16: case OP_u32: case OP_i32: \
	case OP_f32: case OP_u64: case OP_i64: case OP_d64:

#define IBOP_NAME_LEN 32
#define IBLayer3STR_MAX 64

#define IB_WARN_ON_REALLOC 0

#if IB_WARN_ON_REALLOC
#define IBREALLOCWARNING \
	DbgFmt("[%s:%d] %s realloc called!\n", __FILE__, __LINE__, __func__);
#else
#define IBREALLOCWARNING
#endif

#define IBREALLOC(nptr, ptr, sz) \
	nptr = realloc(ptr, sz); \
	IBREALLOCWARNING

#define IBASSERT0(x) \
	{ \
		if(!(x)) \
		{ \
			printf("[%d]Assertion failed!!! %s\n", __LINE__, #x); \
			DB; \
		} \
	}

#define IBASSERT(x, errMsg) \
	{ \
		if(!(x)) \
		{ \
			PLINE; \
			IBPushColor(IBFgRED); \
			printf("Assertion failed!!! -> %s\n%s", \
				errMsg, #x); \
			IBPopColor(); \
			DB; \
		} \
	}

#define IBassert(x) \
	IBASSERT(x, "")

#ifdef IBDEBUGPRINTS
#define DbgFmt(x, ...) \
	{ \
		printf(x, __VA_ARGS__); \
	}
#define DbgPuts(x) \
	printf("%s",x);
#else
#define DbgFmt(x, ...)
#define DbgPuts(x)
#endif

#include "op.h"

#define CASE_VALTYPES \
	case OP_u8: case OP_i8: case OP_c8: case OP_u16: \
	case OP_i16: case OP_u32: case OP_i32: case OP_u64: \
	case OP_i64:

extern IBVector g_ColorStack; /*IBColor*/

char* StrConcat(char* dest, int count, char* src);
char StrStartsWith(char* str, char* with);

typedef union IBVal
{
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

typedef struct IBTypeInfo
{
	STRUCT_DATA_TYPE_IDENT;
	//IBOp infoType; //OP_Builtin,OP_Custom
	IBOp type;//OP_Enum,OP_Struct,OP_StructVar,OP_Func,OP_i32,OP_c8 etc..
	struct IBTypeInfo* memberVarType;
	IBStr name;
	IBVector members;//IBTypeInfo
	struct
	{
		char isFlags;
	} Enum;
	struct
	{
		int val;
	} EnumValue;
	struct
	{
		int flags;
		char placeholder;
	} Struct;
	struct
	{
		IBOp type;
	} StructVar;
	struct
	{
		char isMethod;/*methods are sub functions of types*/
		//todo: store func args info
	} Function;
	IB_DEFMAGIC;
} IBTypeInfo;

void IBTypeInfoInit(IBTypeInfo* ti, IBOp type, char* name);
void IBTypeInfoFree(IBTypeInfo* ti);
void IBTypeInfoFindMember(IBTypeInfo* ti, char* name, IBTypeInfo** outDP);

typedef struct IBNameInfo
{
	STRUCT_DATA_TYPE_IDENT;
	IBOp type;
	IBTypeInfo*ti;
	IBOp cast;
	char* name;
	IBVector members; //IBNameInfo
} IBNameInfo;

void IBNameInfoInit(IBNameInfo* info);
void IBNameInfoFree(IBNameInfo* info);
IBOp IBNameInfoAddMember(struct IBLayer3* ibc, IBNameInfo* ni, char* name, IBOp type, IBNameInfo** niDP);
IBOp IBNameInfoFindType(IBNameInfo* ni, char* name);

IBNameInfo* _IBNameInfoFindMember(IBNameInfo* ni, char* name, int lineNum);
#define IBNameInfoFindMember(ni, name) \
	_IBNameInfoFindMember(ni, name, __LINE__)

typedef struct IBCodeBlock
{
	STRUCT_DATA_TYPE_IDENT;
	IBStr header;
	IBStr variables;
	IBStr varsInit;
	IBStr codeLeft;
	IBStr code;
	IBStr codeRight;
	IBStr footer;
	IBNameInfo localVariables;
	IB_DEFMAGIC;
} IBCodeBlock;

void IBCodeBlockInit(IBCodeBlock* block);
void IBCodeBlockFinish(IBCodeBlock* block, IBStr* output);
void IBCodeBlockFree(IBCodeBlock* block);

char* IBGetOpName(IBOp op);

typedef struct IBObj
{
	STRUCT_DATA_TYPE_IDENT;
	IBOp type;
	IBOp modifier;
	IBOp valType;
	char* name;
	char* str;
	IBVal val;
	struct _IBFuncObj
	{
		IBOp retTYPE;
		IBOp retValType;
		IBOp retValVarcast;
		IBOp retTypeMod;
		char* retStr;
		char* retValStr;
		IBVal retVal;
		struct IBTask* thingTask;//stupid
	} func;
	struct _IBIfObj
	{
		IBOp lvTYPE;
		IBOp lvMod;
		IBOp midOp;
		IBOp rvTYPE;
		IBOp rvMod;
		IBVal lvVal;
		IBVal rvVal;
		char* lvName;
		char* rvName;
	} ifO;
	struct _IBArgObj
	{
		IBOp type;
		IBOp mod;
		IBVector arrIndexExprs;//IBStr
	} arg;
	struct _IBVarObj
	{
		IBOp type;
		IBTypeInfo*ti;
		IBOp mod;
		bool valSet;
		IBVal val;
		char* valStrLiteral;
	} var;
	struct _IBForObj
	{
		char* startName;
		IBVal start, end;
		IBVal step;
	} forO;
	struct _IBEnumObj
	{
		bool flags;
	} enumO;
	struct _IBTableObj
	{
		bool fallthru;
	} table;
} IBObj;

void _IBObjSetType(IBObj* obj, IBOp type);
#define IBObjSetType(obj, type) \
{ \
	PLINE; \
	_IBObjSetType(obj, type); \
}

void IBObjSetMod(IBObj* obj, IBOp mod);

void _IBObjSetName(IBObj* obj, char* name);
#define IBObjSetName(obj, name) \
{ \
	PLINE; \
	_IBObjSetName(obj,name); \
}

void IBObjSetStr(IBObj* obj, char* Str);
void IBObjCopy(IBObj* dst, IBObj* src);
void ObjPrint(IBObj* obj);
void ObjInit(IBObj* o);
void ObjFree(IBObj* o);
void Val2Str(char* dest, int destSz, IBVal v, IBOp type);

typedef struct IBExpects
{	
	STRUCT_DATA_TYPE_IDENT;
	IBVector pfxs; /* IBOp P */
	IBVector nameOps; /* IBOp N */
	char* pfxErr;
	char* nameOpErr;
	int life;
	int lineNumInited;
} IBExpects;

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
void _IBExpectsInit(int LINENUM, IBExpects* exp, char* fmt, ...);
#define IBExpectsInit(exp, fmt, ...) \
	_IBExpectsInit(__LINE__, exp, fmt, __VA_ARGS__);

void IBExpectsPrint(IBExpects* exp);
void IBExpectsFree(IBExpects* exp);

typedef struct IBTaskNeedExpression
{
	IBOp finalVartype;
} IBTaskNeedExpression;

typedef struct IBTask
{
	STRUCT_DATA_TYPE_IDENT;
	IBOp type;
	IBCodeBlock code;
	IBVector expStack; /*IBExpects*/
	IBVector working;/*IBObj*/
	IBVector subTasks;/*IBTask*/
	IBTaskNeedExpression exprData;
} IBTask;

void TaskInit(IBTask* t, IBOp type);
void TaskFree(IBTask* t);
void TaskFindWorkingObj(IBTask* t, IBOp type, IBObj** outDP);

typedef struct IBExpression
{
	IBCodeBlock cb;
} IBExpression;

typedef struct IBLayer3
{
	IBOp Pfx;
	IBOp Pointer;
	IBOp NameOp;
	IBOp LastNameOp;
	IBOp Varcast;
	int Line;
	int Column;
	int LineIS; //LINE inputstr
	int ColumnIS;//COLUMN inputstr
	IBStr ibFileNameStr; //the MAIN in MAIN.ib
	IBStr CIncludesStr;
	IBStr CHeader_Structs;
	IBStr CHeader_Funcs;
	IBStr CCode;
	IBStr HFileStr;
	IBStr CFileStr;
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

	char* InputStr;
	char Ch;
	char LastCh;
	char DefiningStruct;
	IBTypeInfo* DefiningStructTypeInfo;
	IBTypeInfo* DefiningEnumTypeInfo;
	char DefiningMethods;

	char DotPathOn;
	IBVector DotPathVec; //IBStr

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

void IBLayer3PFX_DOT(IBLayer3* ibc);
void IBLayer3PFX_SINGLEQUOTE(IBLayer3* ibc);
void IBLayer3PFX_EXCLAIM(IBLayer3* ibc);
void IBLayer3PFX_STRING(IBLayer3* ibc);
void IBLayer3PFX_VARTYPE(IBLayer3* ibc);
void IBLayer3PFX_NAME(IBLayer3* ibc);
void IBLayer3PFX_OP(IBLayer3* ibc);
void IBLayer3PFX_VALUE(IBLayer3* ibc, IBOp valType, IBVal strVal);
void IBLayer3PFX_LESSTHAN(IBLayer3* ibc);
void IBLayer3PFX_GREATERTHAN(IBLayer3* ibc);

#define Err(code, msg) \
{ \
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

#define ErrF(code, fmt, ...) \
{ \
	char str[512]; \
	sprintf_s(str, sizeof(str), fmt, __VA_ARGS__); \
	Err(code, str); \
}

void IBLayer3Init(IBLayer3* ibc);
void IBLayer3Free(IBLayer3* ibc);
void IBLayer3RegisterCustomType(IBLayer3* ibc, char* name, 
	IBOp type,/* OP_Enum, OP_Struct, OP_FuncPtr */
	IBTypeInfo** outDP);
void IBLayer3FindType(IBLayer3* ibc, char* name, IBTypeInfo** outDP);
void IBLayer3CompileTCC(IBLayer3* ibc);
IBObj* IBLayer3GetObj(IBLayer3* ibc);
IBNameInfo* IBLayer3TryFindNameInfoInStructVar(IBLayer3* ibc, IBNameInfo* ni);

IBNameInfo* _IBLayer3SearchNameInfo(IBLayer3* ibc, char* name, int ln);
#define IBLayer3SearchNameInfo(ibc, name) \
	_IBLayer3SearchNameInfo(ibc, name, __LINE__)

IBObj* IBLayer3FindStackObjRev(IBLayer3* ibc,IBOp type);
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
#define IBLayer3PushCodeBlock(ibc, cbDP) \
{ \
	PLINE; \
	_IBLayer3PushCodeBlock(ibc, cbDP); \
}

void _IBLayer3PopCodeBlock(IBLayer3* ibc,
	bool copyToParent, IBCodeBlock** cbDP);
#define IBLayer3PopCodeBlock(ibc, copyToParent, cbDP) \
{ \
	PLINE; \
	_IBLayer3PopCodeBlock(ibc, copyToParent, cbDP); \
}

void _IBLayer3PushTask(IBLayer3* ibc, IBOp taskOP,
	IBExpects** exectsDP, IBTask** taskDP);
#define IBLayer3PushTask(ibc, taskOP, exectsDP, taskDP) \
{ \
	PLINE; \
	_IBLayer3PushTask(ibc, taskOP, exectsDP, taskDP); \
}

void _IBLayer3PopTask(IBLayer3* ibc, IBTask** taskDP, bool popToParent);
#define IBLayer3PopTask(ibc, taskDP, popToParent) \
{ \
	PLINE; \
	_IBLayer3PopTask(ibc, taskDP, popToParent); \
}

void _IBLayer3PushObj(IBLayer3* ibc, IBObj** o);
#define IBLayer3PushObj(ibc, objDP) \
{ \
	PLINE; \
	_IBLayer3PushObj(ibc, objDP); \
}

#define GetObjCount (ibc->ObjStack.elemCount)
void _IBLayer3PopObj(IBLayer3* ibc, bool pushToWorking, IBObj** objDP);
#define IBLayer3PopObj(ibc, p2w, objDP) \
{ \
	PLINE; \
	_IBLayer3PopObj(ibc, p2w, objDP); \
}

void _IBLayer3Push(IBLayer3* ibc, IBOp mode, bool strAllowSpace);
#define IBLayer3Push(ibc, mode, strAllowSpace) \
{ \
	PLINE; \
	_IBLayer3Push(ibc, mode, strAllowSpace); \
}

void _IBLayer3Pop(IBLayer3* ibc);
#define IBLayer3Pop(ibc) \
{ \
	PLINE; \
	_IBLayer3Pop(ibc); \
}

void IBStrSplitBy(IBStr* str, char ch, IBVector* toVec);

/*life:0 = infinite, -1 life each pfx*/
void IBLayer3PushExpects(IBLayer3* ibc, IBExpects** expDP);

void IBLayer3PopExpects(IBLayer3* ibc);

/*frees exps top and ow expDP with top ptr for user to init again */
void IBLayer3ReplaceExpects(IBLayer3* ibc, IBExpects** expDP);

bool IBLayer3IsPfxExpected(IBLayer3* ibc, IBOp pfx);
bool IBLayer3IsNameOpExpected(IBLayer3* ibc, IBOp nameOp);
/*NO NEWLINES AT END OF STR*/
void IBLayer3Tick(IBLayer3* ibc, FILE* f);
void IBLayer3InputChar(IBLayer3* ibc, char ch);
void IBLayer3InputStr(IBLayer3* ibc, char* str);
void _IBLayer3FinishTask(IBLayer3* ibc);
#define IBLayer3FinishTask(ibc) \
{ \
	PLINE; \
	_IBLayer3FinishTask(ibc); \
}
IBVal IBLayer3StrToVal(IBLayer3* ibc, char* str, IBOp expectedType);
char* IBLayer3GetCPrintfFmtForType(IBLayer3* ibc, IBOp type);
void IBLayer3Prefix(IBLayer3* ibc);
void IBLayer3Str(IBLayer3* ibc);
void IBLayer3StrPayload(IBLayer3* ibc);
void IBLayer3ExplainErr(IBLayer3* ibc, IBOp code);
#define SetObjType(obj, tt) \
{ \
	PLINE; \
	DbgFmt(" SetObjType: %s(%d) -> %s(%d)\n", \
		IBGetOpName(obj->type), (int) obj->type, \
		IBGetOpName(tt), (int) tt); \
	obj->type = tt; \
}
#define PopExpects() \
{ \
	PLINE; \
	IBLayer3PopExpects(ibc); \
}
IBTask* IBLayer3GetTask(IBLayer3* ibc);
IBOp IBLayer3GetMode(IBLayer3* ibc);

IBExpects* IBTaskGetExpTop(IBTask* t);
IBVector* IBTaskGetExpPfxsTop(IBTask* t);
IBVector* IBTaskGetExpNameOPsTop(IBTask* t);
IBOp IBStrToBool(IBLayer3* ibc, char* str);
IBOp IBJudgeTypeOfStrValue(IBLayer3* ibc, char* str);

#define SetTaskType(task, tt) \
{ \
	IBassert(task); \
	PLINE; \
	DbgFmt(" SetTaskType: %s(%d) -> %s(%d)\n", \
		IBGetOpName(task->type), \
		(int)task->type, \
		IBGetOpName(tt), (int) tt); \
	task->type = tt; \
}

typedef struct IBOpNamePair
{
	char name[IBOP_NAME_LEN];
	IBOp op;
} IBOpNamePair;

char* IBGetCEqu(IBOp op);
char* IBGetOpName(IBOp op);
char* IBGetPfxName(IBOp op);
IBOp IBGetOpFromNameList(char* name, IBOp list);
IBOp IBOPFromPfxCh(char ch);
void IBOverwriteStr(char** str, char* with);

struct IBVecData
{
	union
	{
		IBObj obj;
		IBStr str;
		IBTask task;
		IBOp op;
		bool boolean;
		IBExpects expects;
		IBNameInfo nameInfo;
		IBTypeInfo typeInfo;
		IBVector vec;
		IBCodeBlock codeBlock;
		IBExpression expression;
	};
};
