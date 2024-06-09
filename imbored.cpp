#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <vector>
#include <stack>
#include <string>

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
	Op type = OP_NotSet;
	const char* name = NULL;
} NameInfo;
typedef struct NameInfoDB {
	std::vector<NameInfo> pairs = {};
	void add(const char* name, Op type);
	Op findType(const char* name);
} NameInfoDB;
typedef struct FuncObj {
	Val retVal = {};
	Op retType = OP_NotSet;
	Op retTypeMod = OP_NotSet;
} FuncObj;
typedef struct ArgObj {
	Op type = OP_Null;
	Op mod = OP_NotSet;
} ArgObj;
typedef struct VarObj {
	Val val = {};
	bool valSet = false;
	Op type = OP_NotSet;
	Op mod = OP_NotSet;
} VarObj;
const char* GetOpName(Op op);
typedef struct Obj {
private:
	Op type = OP_NotSet;
	Op modifier = OP_NotSet;
public:
	Op privacy = OP_NoChange;
	char* name = NULL;
	char* str = NULL;
	//union {
		FuncObj func;
		VarObj var;
		ArgObj arg = {};
	//};
	Val val = {};
	const Op getType();
	void setType(Op type);
	Op getMod();
	void setMod(Op mod);
	void setName(const char* name);
	void setStr(const char* Str);
	void print();
} Obj;
typedef struct AllowedPfxs {
	std::vector<Op> pfxs = {};
	const char* err = NULL;
	int life=0;
} AllowedPfxs;
typedef struct Task {
	Op type = OP_NotSet;
	std::vector<Obj> working = {};
	std::string code = {};
	std::string codePart1 = {};
	std::string codePart2 = {};
} Task;
typedef struct  Compiler {
	int m_Line = 1, m_Column = 1;
	Op m_Pfx = OP_Null;
	std::string m_Str = {}, m_cOutput = {};
	std::stack<Obj> m_ObjStack = {};
	std::stack<AllowedPfxs> m_AllowedNextPfxsStack = {};
	std::stack<Op> m_ModeStack;
	std::stack<Task> m_TaskStack;
	std::stack<bool> m_StrReadPtrsStack;
	Op m_Pointer = OP_NotSet;
	Op m_NameOp = OP_Null;
	char m_Ch = '\0';
	bool m_StringMode = false;
	bool m_StrAllowSpace = false;
	Op m_CommentMode = OP_NotSet;
	int m_MultiLineOffCount = 0;
	NameInfoDB m_NameTypeCtx = {};

	Obj& GetObj();
	Compiler();
	~Compiler();
	void pushTask(Op task);
	void popTask();
	Obj& pushObj(Obj obj);
	Obj& popObj(bool pushToWorking = true);
	void push(Op mode, bool strAllowSpace = false);
	Op pop();
	//life:0 = infinite, -1 life each pfx
	void pushAllowedNextPfxs(std::vector<Op> allowedNextPfxs, const char* err, int life);
	void popAllowedNextPfxs();
	bool isPfxExpected(Op pfx);
	//NO NEWLINES AT END OF STR
	void Char(char ch);
	void PopAndDoTask();
	const char* GetCPrintfFmtForType(Op type);
	void Prefix();
	void Str();
	void StrPayload();
	void ExplainErr(Op code);
} Compiler;
#define Err(code, msg){\
	PRINT_LINE_INFO();\
	printf(":%s At %u:%u \"%s\"(%d)\nExplanation: ", msg, m_Line, m_Column, GetOpName(code), (int)code);\
	ExplainErr(code);\
	printf("\n");\
	__debugbreak();\
}
#define SetObjType(type){\
	PRINT_LINE_INFO();\
	GetObj().setType(type);\
}
#define GetObjType (GetObj().getType())
#define PushPfxs(pfxs, msg, life){\
	PRINT_LINE_INFO();\
	pushAllowedNextPfxs(pfxs, msg, life);\
}
#define PopPfxs(){\
	PRINT_LINE_INFO();\
	popAllowedNextPfxs();\
}
#define GetTask (m_TaskStack.top())
#define GetTaskType   (GetTask.type)
#define GetTaskCode   (GetTask.code)
#define GetTaskCodeP1 (GetTask.codePart1)
#define GetTaskCodeP2 (GetTask.codePart2)
#define SetTaskType(tt) {\
	PRINT_LINE_INFO();\
	printf("SetTaskType: %s(%d) -> %s(%d)\n", GetOpName(GetTask.type), (int)GetTask.type, GetOpName(tt), (int)tt);\
	GetTask.type = tt;\
}
#define GetTaskWorkingObjs (GetTask.working)
#define SwitchTaskStackStart if (!m_TaskStack.empty()) {\
	switch (m_TaskStack.top().type) {
#define SwitchTaskStackEnd }\
	}
struct OpNamePair {
	char name[OP_NAME_LEN];
	Op op;
};
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
	for (auto& opN : cEquivelents)
		if (op == opN.op) return opN.name;
	return "?";
}
const char* GetOpName(Op op) {
	for (auto& opN : opNames)
		if (op == opN.op) return opN.name;
	return "?";
}
const char* GetPfxName(Op op) {
	for (auto& opN : pfxNames)
		if (op == opN.op) return opN.name;
	return "?";
}
Op GetOpFromName(const char* name) {
	for (auto& op : opNames)
		if (!strcmp(op.name, name)) return op.op;
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
Obj& Compiler::GetObj() {
	return m_ObjStack.top();
}
Compiler::Compiler(){
	m_StrReadPtrsStack.push(false);
	//PushPfxs({OP_Op}, "");
	m_AllowedNextPfxsStack.push({ { OP_Op }, "" });
	push(OP_ModePrefixPass);
	pushObj({});
}
Compiler::~Compiler() {
	if (m_StringMode)Err(OP_ErrNOT_GOOD, "Reached end of file without closing string");
	if (!m_Str.empty()) StrPayload();
	SwitchTaskStackStart
		case OP_FuncNeedRetVal:
			Err(OP_ErrNOT_GOOD, "Reached end of file without closing function");
			break;
		case OP_FuncSigComplete:
		case OP_FuncHasName: {
			SetObjType(OP_FuncSigComplete);
			popObj(true);
			PopAndDoTask();
			break;
		}
	SwitchTaskStackEnd
	printf("-> Compilation complete <-\nResulting C code:\n\n");
	printf("%s", m_cOutput.c_str());
}
void Compiler::pushTask(Op task) {
	printf("Push task %s(%d)\n", GetOpName(task),(int)task);
	m_TaskStack.push({ task, {} });
}
void Compiler::popTask() {
	printf("Pop task %s(%d)\n", GetOpName(m_TaskStack.top().type),(int)m_TaskStack.top().type);
	m_TaskStack.pop();
}
Obj& Compiler::pushObj(Obj obj) {
	printf("Push obj: ");
	if (!m_ObjStack.empty()) {
		GetObj().print();
		printf(" -> ");
	}
	m_ObjStack.push(obj);
	GetObj().print();
	printf("\n");
	return GetObj();
}
Obj& Compiler::popObj(bool pushToWorking) {
	if (pushToWorking){
		if (GetObjType == OP_NotSet)Err(OP_ErrNOT_GOOD, "");
		printf("To working: ");
		GetObj().print();
		printf("\n");
		GetTaskWorkingObjs.push_back(GetObj());
	}
	printf("Pop obj: ");
	GetObj().print();
	if (m_ObjStack.size() == 1)GetObj() = {};
	else m_ObjStack.pop();
	printf(" -> ");
	GetObj().print();
	printf("\n");
	return GetObj();
}
void Compiler::push(Op mode, bool strAllowSpace){
	this->m_StrAllowSpace = strAllowSpace;
	m_ModeStack.push(mode);
	printf("push: to %s\n", GetOpName(m_ModeStack.top()));
}
Op Compiler::pop() {
	m_ModeStack.pop();
	printf("pop: to %s\n", GetOpName(m_ModeStack.top()));
	return m_ModeStack.top();
}
const Op Obj::getType() { return type; }
void Obj::setType(Op type) {
	printf(" obj type: %s(%d) -> %s(%d)\n", GetOpName(this->type), (int)this->type, GetOpName(type), (int)type);
	this->type = type;
}
Op Obj::getMod() { return modifier; }
void Obj::setMod(Op mod) {
	printf("obj mod: %s(%d) -> %s(%d)\n", GetOpName(this->modifier), (int)this->modifier, GetOpName(mod), (int)mod);
	modifier = mod;
}
void Obj::setName(const char* name) {
	printf("obj name: %s -> %s\n", this->name, name);
	owStr(&this->name, name);
}
void Obj::setStr(const char* Str) {
	printf("obj str: %s -> %s\n", this->str, Str);
	owStr(&this->str, Str);
}
void Obj::print() {
	unsigned __int64 u64 = val.u64;
	printf("[");
	if (type != OP_NotSet) {
		printf("Type:%s(%d),", GetOpName(type), (int)type);
	}
	if(name)printf("Name:%s,", name);
	if (str)printf("Str:%s,", str);
	if (modifier != OP_NotSet) {
		printf("Mod:%s,", GetOpName(modifier));
	}
	/*if(u64)*/printf("Val:%I64u", u64);
	printf("]");
}
void Compiler::pushAllowedNextPfxs(std::vector<Op> allowedNextPfxs, const char* err, int life) {
	if (!m_AllowedNextPfxsStack.top().pfxs.empty()) {
		printf(" allowed pfxs PUSH: { ");
		for (auto& p : m_AllowedNextPfxsStack.top().pfxs) printf("%s ", GetPfxName(p));
		printf("} -> { ");
		m_AllowedNextPfxsStack.push({ allowedNextPfxs, err, life });
		for (auto& p : m_AllowedNextPfxsStack.top().pfxs) printf("%s ", GetPfxName(p));
		printf("}\n");
	}
	else Err(OP_ErrNOT_GOOD, "pfx stack vec cannot be empty");
}
void Compiler::popAllowedNextPfxs() {
	if (!m_AllowedNextPfxsStack.top().pfxs.empty()) {
		printf(" allowed pfxs POP: { ");
		for (auto& p : m_AllowedNextPfxsStack.top().pfxs) printf("%s ", GetPfxName(p));
		printf("} -> { ");
		m_AllowedNextPfxsStack.pop();
		if (m_AllowedNextPfxsStack.empty()) Err(OP_ErrNOT_GOOD, "catastrophic failure");
		for (auto& p : m_AllowedNextPfxsStack.top().pfxs) printf("%s ", GetPfxName(p));
		printf("}\n");
	}
}
bool Compiler::isPfxExpected(Op pfx) {
	for (auto& p : m_AllowedNextPfxsStack.top().pfxs) if (p == pfx) return true;
	return false;
}
//NO NEWLINES AT END OF STR
void Compiler::Char(char ch){
	this->m_Ch = ch;
	bool nl = false;
	switch (this->m_Ch) {
	case COMMENT_CHAR: {
		if (m_StringMode) break;
		switch (m_CommentMode) {
		case OP_NotSet: {
			m_CommentMode = OP_Comment;
			push(OP_ModeComment);
			break;
		}
		case OP_Comment: {
			pop();
			push(OP_ModeMultiLineComment);
			m_CommentMode = OP_MultiLineComment;
			break;
		}
		case OP_MultiLineComment: {
			switch (m_MultiLineOffCount++) {
			case 0: break;
			case 1: {
				pop();
				m_MultiLineOffCount = 0;
				m_CommentMode = OP_NotSet;
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
		if(m_CommentMode==OP_NotSet)printf("Char():Line end\n");
		if (m_CommentMode == OP_Comment) {
			pop();
			m_CommentMode = OP_NotSet;
		}
		switch (m_ModeStack.top()) {
		case OP_ModeStrPass: {
			StrPayload();
			break; 
		}
		}
		switch (GetObjType) {
		case OP_CallWantArgs: {
			popObj(true);
			break;
		}
		case OP_VarWantValue: 
		case OP_VarComplete: {
			popObj(true);
			break;
		}
		}
		SwitchTaskStackStart
			case OP_CPrintfHaveFmtStr: {
				PopAndDoTask();
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
				Op mod = GetObj().getMod();
				popObj(true);
				if (mod != OP_Imaginary) {
					auto allowed = { OP_Op,OP_String, OP_VarType };
					PushPfxs(allowed, "expected operator, print statement, or variable declaration", 0);
					m_TaskStack.top().type = OP_FuncWantCode;
				}
				else {
					PopAndDoTask();
				}
				break;
			}
		SwitchTaskStackEnd
		break;
	}
	}
	if (m_MultiLineOffCount == 1 && m_Ch != COMMENT_CHAR) {
		m_MultiLineOffCount = 0;
	}
	auto m = m_ModeStack.top();
	m_Column++;
	if (!nl && m_CommentMode == OP_NotSet) {
		if(this->m_Ch == ' ') printf("-> SPACE (0x%x)\n",  this->m_Ch);
		else printf("-> %c (0x%x) %d:%d\n", this->m_Ch, this->m_Ch, m_Line, m_Column);
		switch (m) {
		case OP_ModePrefixPass:
			Prefix();
			break;
		case OP_ModeStrPass:
			Str();
			break;
		}
	}
	if (nl) {
		if (isPfxExpected(OP_LineEnd)) PopPfxs();
		m_Column = 0;
		m_Line++;
	}
}
const char* Compiler::GetCPrintfFmtForType(Op type) {
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
void Compiler::PopAndDoTask()	{
	printf("PopAndDoTask()\n");
	if(m_TaskStack.empty())Err(OP_ErrNoTask, "task stack EMPTY!");
	if(GetTaskWorkingObjs.empty())Err(OP_ErrNOT_GOOD, "workingObjs EMPTY!");
	bool subTask = false;
	switch (m_TaskStack.top().type) {
	case OP_FuncWantCode: break;
	case OP_FuncSigComplete:
	case OP_FuncHasName:
	case OP_Func: {
		std::string cFuncModsTypeName, cFuncArgs, cFuncCode;
		bool imaginary = false;
		Obj* funcObj=NULL;
		for (int i = 0; i < GetTaskWorkingObjs.size(); ++i) {
			auto& o = GetTaskWorkingObjs[i];
			switch (o.getType()) {
			case OP_FuncArgComplete: {//multiple allowed
				auto at = o.arg.type;
				if (at == OP_Null)Err(OP_ErrNOT_GOOD, "arg type NULL");
				if(!cFuncArgs.empty())cFuncArgs += ", ";
				cFuncArgs += GetCEqu(o.arg.type);
				cFuncArgs += GetCEqu(o.arg.mod);
				cFuncArgs += " ";
				if (!o.name)Err(OP_ErrNOT_GOOD, "arg name NULL");
				cFuncArgs += std::string(o.name);			
				break;
			}
			case OP_FuncSigComplete: {
				if (o.getMod() == OP_Imaginary) {
					//printf("img\n");
					imaginary = true;
				}
			}
			case OP_FuncHasName:
			case OP_CompletedFunction: {//should only happen once
				funcObj = &o;
				auto mod = o.getMod();
				if (mod != OP_NotSet) {
					cFuncModsTypeName += GetCEqu(mod);
					cFuncModsTypeName += " ";
				}
				cFuncModsTypeName += GetCEqu(o.func.retType);
				cFuncModsTypeName += GetCEqu(o.func.retTypeMod);
				cFuncModsTypeName += " ";
				if (!o.name)Err(OP_ErrNOT_GOOD, "func name NULL");
				cFuncModsTypeName += std::string(o.name);
				cFuncModsTypeName += "(";
				break;
			}
			}
		}
		for (auto& o : GetTaskWorkingObjs) {
			switch (o.getType()) {
			case OP_VarComplete: {
				cFuncCode += "\t";	
				cFuncCode += GetCEqu(o.var.type);
				cFuncCode += GetCEqu(o.var.mod);
				cFuncCode += " ";
				if(!o.name)Err(OP_ErrNOT_GOOD, "var name NULL");
				cFuncCode += o.name;
				cFuncCode += "=";
				cFuncCode += std::to_string(o.var.val.i64);
				cFuncCode += ";\n";
				break;
			}
			}
		}
		if (imaginary) {
			cFuncArgs += ");\n\n";
			//cFuncCode += "}";
		}
		else {
			cFuncArgs += "){\n";
			cFuncCode += GetTaskCodeP1;
			if(!funcObj)Err(OP_ErrNOT_GOOD, "funcObj NULL");
			if (funcObj->func.retType != OP_Void) {
				cFuncCode += "\treturn ";
				switch (funcObj->func.retType) {
				case OP_u8:  { cFuncCode += std::to_string(funcObj->func.retVal.u8);  break; }
				case OP_c8:  { cFuncCode += std::to_string(funcObj->func.retVal.c8);  break; }
				case OP_i16: { cFuncCode += std::to_string(funcObj->func.retVal.i16); break; }
				case OP_u16: { cFuncCode += std::to_string(funcObj->func.retVal.u16); break; }
				case OP_i32: { cFuncCode += std::to_string(funcObj->func.retVal.i32); break; }
				case OP_i64: { cFuncCode += std::to_string(funcObj->func.retVal.i64); break; }
				case OP_u32: { cFuncCode += std::to_string(funcObj->func.retVal.u32); break; }
				case OP_u64: { cFuncCode += std::to_string(funcObj->func.retVal.u64); break; }
				case OP_f32: { cFuncCode += std::to_string(funcObj->func.retVal.f32); break; }
				case OP_d64: { cFuncCode += std::to_string(funcObj->func.retVal.d64); break; }
				}
				cFuncCode+=";\n";
			}
			cFuncCode += "}\n\n";
		}
		//printf("%s\n", std::string(cFuncModsTypeName+cFuncArgs+cFuncCode).c_str());
		m_cOutput += std::string(cFuncModsTypeName + cFuncArgs + cFuncCode);

		//GetTaskWorkingObjs.clear();
		break;
	}
	case OP_CPrintfHaveFmtStr: {
		subTask = true;
		Obj& fmtObj = GetTaskWorkingObjs.front();
		GetTaskCode += "\tprintf(\"";
		bool firstPercent = false;
		int varIdx = 1;
		for (int i = 0; i < strlen(fmtObj.str); ++i) {
			auto c = fmtObj.str[i];
			switch (c) {
			case '%':{
					if (!firstPercent) {
						GetTaskCode += "%";
						firstPercent = true;
					}
					else {
						auto& vo = GetTaskWorkingObjs[varIdx];
						switch (vo.getType()) {
						case OP_Name:{
							auto type = m_NameTypeCtx.findType(vo.name);
							GetTaskCode += GetCPrintfFmtForType(type);
							break;
						}
						case OP_Value:{
							GetTaskCode += GetCPrintfFmtForType(vo.var.type);
							break;
						}
						}
						firstPercent = false;
						varIdx++;
					}
					break;
				}
			default: {
				GetTaskCode += c;
				break;
			}
			}
		}
		GetTaskCode += "\"";
		if (GetTaskWorkingObjs.size() > 1) GetTaskCode += ", ";
		for (int i = 1; i < GetTaskWorkingObjs.size(); ++i) {
			Obj& o = GetTaskWorkingObjs[i];
			switch (o.getType()) {
			case OP_Name: {
				GetTaskCode += o.name;
				break;
			}
			case OP_String: {
				GetTaskCode += "\"";
				GetTaskCode += std::string(o.str);
				GetTaskCode += "\"";
				break;
			}
			case OP_Value: {
				GetTaskCode += std::to_string(o.val.i32);//for now
			}
			}
			if(i < GetTaskWorkingObjs.size()-1) GetTaskCode += ", ";
		}
		GetTaskCode += ");\n";
		break;
	}
	}
	if (subTask) {
		switch (GetTaskType) {
		case OP_CPrintfHaveFmtStr: {
			if (m_TaskStack.size() - 2 >= 0) {
				std::string theCode = GetTaskCode;
				popTask();
				switch (GetTaskType) {
				case OP_FuncWantCode: {
					GetTask.codePart1 += theCode;
					break;
				}
				}
			}else Err(OP_ErrNOT_GOOD, "m_TaskStack.size() - 2 < 0");
			break;
		}
		}
	}
	else popTask();
}
void Compiler::Prefix(){
	//for assigning func call ret val to var
	if (m_Pfx == OP_Value && m_Ch == '@' && m_Str.empty()) {
		PushPfxs({ OP_Op }, "", 1);
	}
	m_Pfx = fromPfxCh(m_Ch);
	auto& obj = GetObj();
	if (m_Pfx != OP_Unknown 
		&& !(m_AllowedNextPfxsStack.top().pfxs.empty())
		&& !isPfxExpected(m_Pfx))
		Err(OP_ErrUnexpectedNextPfx, "");
	printf("PFX:%s(%d)\n", GetPfxName(m_Pfx), (int)m_Pfx);
	switch (m_Pfx) {
	case OP_String: { //"
		m_StringMode = true;
		push(OP_ModeStrPass);
		break;
	}
	case OP_VarType:
		m_StrReadPtrsStack.push(true);
	case OP_Value:
	case OP_Op:
	case OP_Name:
		//getchar();
		push(OP_ModeStrPass);
		break;
	case OP_Comment:
		break;
	}
	if (m_Pfx == OP_Op) {
		auto& aps = m_AllowedNextPfxsStack.top();
		if (aps.life && --aps.life <= 0) m_AllowedNextPfxsStack.pop();
	}
}
void Compiler::Str(){
	if (m_StringMode) {
		switch (m_Ch) {
		case '"': {
			m_StringMode = false;
			StrPayload();
			return;
		}
		}
		m_Str.append(1, m_Ch);
		return;
	}
	switch (m_Pfx) {
	case OP_Value: {
		switch (m_Ch) {
		case '@': {
			pop();
			Prefix();
			return;
		}
		}
		break;
	}
	}
	switch (m_Ch) {
	case '\t': return;
	case ' ': {
		if (m_StrAllowSpace) break;
		else return StrPayload();
	}
	case '&': {
		if (m_StrReadPtrsStack.top()) {
			switch (m_Pointer) {
			case OP_NotSet:
				printf("got pointer\n");
				m_Pointer = OP_Pointer;
				break;
			case OP_Pointer:
				printf("got double pointer\n");
				m_Pointer = OP_DoublePointer;
				break;
			case OP_DoublePointer:
				printf("got tripple pointer\n");
				m_Pointer = OP_TripplePointer;
				break;
			case OP_TripplePointer:
				Err(OP_ErrQuadriplePointersNOT_ALLOWED, "");
				break;
			}
			return;
		}
	}
	}
	m_Str.append(1, m_Ch);
}
void Compiler::StrPayload(){
	printf("Doing Str payload\n");
	auto cs = m_Str.c_str();
	Val strVal = {};
	strVal.i32=atoi(cs);
	m_NameOp = GetOpFromName(cs);
	printf("Str: %s\n", cs);
	switch (m_Pfx)
	{
	case OP_String: { //"
		SwitchTaskStackStart
		case OP_FuncWantCode: { //printf
			pushTask(OP_CPrintfHaveFmtStr);
			pushObj({});
			GetObj().setStr(cs);
			GetObj().setType(OP_CPrintfFmtStr);
			popObj(true);
			auto allowed = { OP_Value, OP_Name, OP_String, OP_LineEnd };
			PushPfxs(allowed, "expected fmt args or line end", 0);
			break;
		}
		SwitchTaskStackEnd
		break;
	}
	case OP_Value: { //=
		switch (GetObjType) {
		case OP_VarWantValue: {
			GetObj().var.val = strVal;
			SetObjType(OP_VarComplete);
			PopPfxs();
			break;
		}
		}
		if (!m_TaskStack.empty()) {
			switch (m_TaskStack.top().type) {
			case OP_CPrintfHaveFmtStr:{
				pushObj({});
				//GetObj().setStr(cs);
				GetObj().val = strVal;
				GetObj().setType(OP_Value);
				GetObj().var.type = OP_i32;//for now
				popObj(true);
				//PopPfxs();
				break;
			}
			case OP_FuncNeedRetVal:
				for (auto& obj : GetTaskWorkingObjs) {
					if (obj.getType() == OP_FuncSigComplete) {
						printf("Finishing func got ret value\n");
						obj.func.retVal = strVal;
						//SetObjType(OP_CompletedFunction);
						PopPfxs();
						SetTaskType(OP_Func);
						PopAndDoTask();
						break;
					}
				}
				break;
			}
		}
		break;
	}
	case OP_VarType: //%
		switch (m_TaskStack.top().type) {
		case OP_FuncWantCode: {
			pushObj({});
			GetObj().var.type = m_NameOp;
			GetObj().var.mod = OP_NotSet;
			SetObjType(OP_VarNeedName);
			PushPfxs({ OP_Name }, "Expected variable name after variable type", 0);
			break;
		}
		}
		switch (GetObjType) {
		case OP_FuncNeedsRetValType: {
			if (GetTaskType != OP_FuncHasName)Err(OP_ErrNOT_GOOD, "func signature needs name");
			GetObj().func.retType = m_NameOp;
			GetObj().func.retTypeMod = m_Pointer;
			SetObjType(OP_FuncSigComplete);
			/*if (GetObj().getMod() == OP_Imaginary) {
				SetTaskType(OP_FuncSigComplete);
			}
			else {
				SetTaskType(OP_FuncWantCode);
			}*/
			//popObj(true);
			/*auto allowed = { OP_Op,OP_String, OP_VarType };
			PopPfxs();
			PopPfxs();
			PushPfxs(allowed, "expected operator, print statement, or variable declaration");*/
			break;
		}
		case OP_FuncHasName:
			pushObj({});
			SetObjType(OP_FuncArgNameless);
			GetObj().arg.type= m_NameOp;
			GetObj().arg.mod = m_Pointer;
			//PopPfxs();
			PushPfxs({OP_Name}, "Expected func arg name", 0);
			break;
		}
		break;
	case OP_Name: { //$
		SwitchTaskStackStart
		case OP_CPrintfHaveFmtStr: {
			auto&o=pushObj({});
			o.setName(cs);
			o.setType(OP_Name);
			popObj(true);
			break;
		}
		SwitchTaskStackEnd
		switch (GetObjType) {
		case OP_CallNeedName: { //=@call
			SetObjType(OP_CallWantArgs);
			PopPfxs();
			auto allowed = { OP_Name, OP_Value, OP_LineEnd };
			PushPfxs(allowed, "expected var type or line end after func name", 0);

		}
		case OP_Func: {
			SetObjType(OP_FuncHasName);
			SetTaskType(OP_FuncHasName);
			PopPfxs();
			auto allowed = { OP_VarType,OP_Op,OP_LineEnd/*means allowed pfx will be cleared on newline*/ };
			PushPfxs(allowed, "", 0);
			GetObj().setName(cs);
			break;
		}
		case OP_FuncArgNameless:
			SetObjType(OP_FuncArgComplete);
			PopPfxs();
			GetObj().setName(cs);
			m_NameTypeCtx.add(cs, GetObj().arg.type);
			popObj(true);
			break;
		case OP_VarNeedName:
			GetObj().setName(cs);
			m_NameTypeCtx.add(cs, GetObj().var.type);
			SetObjType(OP_VarWantValue);
			PopPfxs();
			auto allowed = { OP_Value, OP_LineEnd };
			PushPfxs(allowed, "expected value or line end after var name", 0);
			break;
		}
		break;
	}
	case OP_Op: //@
		switch (m_NameOp) {
		case OP_Call:{
			switch (GetObjType) {
			case OP_VarWantValue: {
				/*auto& tst = m_TaskStack.top();
				auto& ost = m_ObjStack.top();
				auto& pfxs=m_AllowedNextPfxsStack.top().pfxs;*/
				auto&o=pushObj({});
				o.setType(OP_CallNeedName);
				PushPfxs({OP_Name}, "expected function name", 0);
			}
			}
			break;
		}
		case OP_dbgBreak: {
			__debugbreak();
			m_TaskStack;
			break;
		}
		case OP_Imaginary:
			GetObj().setMod(m_NameOp);
			//setAllowedNextPfxs({});
			//PopPfxs();
			break;
		case OP_Done:
			if (m_TaskStack.empty()) Err(OP_ErrNoTask, "");
			switch (GetTaskType) {
			case OP_Func:
			case OP_FuncHasName:
			case OP_FuncWantCode:
				PRINT_LINE_INFO();
				printf(" Finishing function\n");
				for (auto& obj : GetTaskWorkingObjs) {
					//TODO: could cache func obj index later
					if (obj.getType() == OP_FuncSigComplete) {
						if (obj.func.retType != OP_Void) {
							PushPfxs({OP_Value},"", 0);
							SetTaskType(OP_FuncNeedRetVal);
						}else {
							SetTaskType(OP_Func);
							PopAndDoTask();
							PopPfxs();
						}
					}
				}
				break;
			}
			break;
		case OP_Return: {
			auto t = GetObjType;
			switch (t) {
			case OP_FuncArgComplete: {
				printf("what\n");
				popObj(true);
				if (GetObjType != OP_FuncHasName) {
					Err(OP_ErrNOT_GOOD, "expected FuncHasName");
					break;
				}
			}
			case OP_FuncHasName:
				SetObjType(OP_FuncNeedsRetValType);
				//PopPfxs();
				PushPfxs({ OP_VarType },"", 0);
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
			SetObjType(m_NameOp);
			GetObj().func.retType = OP_Void;
			GetObj().func.retTypeMod = OP_NotSet;
			PushPfxs({OP_Name}, "",0);
			pushTask(OP_FuncNeedName);
			break;
		case OP_Public:
		case OP_Private:
			GetObj().privacy = m_NameOp;
			break;
		default:
			Err(OP_ErrUnknownOpStr, "");
		}
	}
	m_Str.clear();
	printf("Str payload complete\n");
	pop();
	if(m_StrReadPtrsStack.size() > 1)
	{
		if (m_StrReadPtrsStack.top()) m_Pointer = OP_NotSet;
		m_StrReadPtrsStack.pop();
	}
}
void Compiler::ExplainErr(Op code) {
	switch (code) {
	case OP_ErrUnknownOpStr:
		printf("Unknown OP str @%s\n", m_Str.c_str());
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
		printf("%s Unexpected next prefix %s. Pfx stack idx:%zd Allowed:", m_AllowedNextPfxsStack.top().err, GetPfxName(m_Pfx), m_AllowedNextPfxsStack.size() - 1);
		for (auto& p : m_AllowedNextPfxsStack.top().pfxs)
			printf("%s,", GetPfxName(p));
		break;
	case OP_ErrExpectedVariablePfx:
		printf("Expected a variable type to be next.");
		break;
	default:
		printf("Err msg unimplemented for %s", GetOpName(code));
	}
	printf("\nOBJ:");
	GetObj().print();
	printf("\n");
}
int main(int argc, char** argv) {
	FILE* f;
	const char* fname = /*argv[1]*/"main.txt";
	if (!fopen_s(&f, fname, "r")){
		Compiler c;
		while (!feof(f)) {
			char ch;
			ch = fgetc(f);
			if (ch == 0xffffffff)break;
			//fread(&ch, 1, 1, f);
			//printf("%c\n", ch);
			c.Char(ch);
		}
		fclose(f);
		return 0;
	}
	else{
		printf("Error\n");
	}
	return 1;
}
void NameInfoDB::add(const char* name, Op type){
	pairs.push_back({ type, _strdup(name) });
}
Op NameInfoDB::findType(const char* name){
	for (auto& p : pairs)
		if (!strcmp(p.name, name))
			return p.type;
	return OP_NotFound;
}
