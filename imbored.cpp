#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <vector>
#include <stack>
#include <string>

//not actually a compiler
//ascii only, maybe utf8 later...
//transpile to C
//no order of operations, sequential ONLY
//compiler options inside source code, preferably using code
//in number order breakpoints, if hit in the wrong order or missing then failure

#define PRINT_LINE_INFO() printf("LINE:%d", __LINE__)
#define OBJ_NAME_LEN 64
#define OP_NAME_LEN 32
#define IB_DEBUG_EXTRA1 1
#define COMMENT_CHAR ('~')

enum class Op { //multiple uses
	Null,False,True,Unknown,NotSet,Any,Use,Build,Space,

	Func,FuncHasName,FuncNeedName,FuncNeedsRetValType,
	FuncArgsVarNeedsName,FuncArgNameless,FuncArgComplete,
	FuncWantCode,FuncSigComplete,FuncNeedRetVal,FuncArg,
	CompletedFunction,

	VarNeedName,VarWantValue,VarComplete,

	Op,Value,Done,Return,NoChange,Struct,VarType,LineEnd,
	Comment,MultiLineComment,Public,Private,Imaginary,Void,
	Set,SetAdd,Call,Colon,Dot,Add,Subtract,Multiply,Divide,
	AddEq,SubEq,MultEq,DivEq,Equals,NotEquals,LessThan,
	GreaterThan,LessThanOrEquals,GreaterThanOrEquals,
	ScopeOpen, ScopeClose,ParenthesisOpen, ParenthesisClose,
	BracketOpen,BracketClose,SingleQuote,DoubleQuote,
	CPrintfHaveFmtStr,

	Comma,CommaSpace,Name,String,CPrintfFmtStr,Char,If,Else,For,While,Block,
	c8,u8,u16,u32,u64,i8,i16,i32,i64,f32,d64,
	Pointer,DoublePointer,TripplePointer,CompilerFlags,

	NotFound,Error,ErrNOT_GOOD,ErrUnexpectedNextPfx,
	ErrExpectedVariablePfx,ErrNoTask,ErrUnexpectedOp,
	ErrQuadriplePointersNOT_ALLOWED,ErrUnknownOpStr,

	ModePrefixPass,ModeStrPass,ModeComment,ModeMultiLineComment,
};
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
struct NameInfo {
	Op type = Op::NotSet;
	const char* name = nullptr;
};
struct NameInfoDB {
	std::vector<NameInfo> pairs = {};
	void add(const char* name, Op type);
	Op findType(const char* name);
};
struct FuncObj {
	Val retVal = {};
	Op retType = Op::NotSet;
	Op retTypeMod = Op::NotSet;
};
struct ArgObj {
	Op type = Op::Null;
	Op mod = Op::NotSet;
};
struct VarObj {
	Val val = {};
	bool valSet = false;
	Op type = Op::NotSet;
	Op mod = Op::NotSet;
};
const char* GetOpName(Op op);
struct Obj {
private:
	Op type = Op::NotSet;
	Op modifier = Op::NotSet;
public:
	Op privacy = Op::NoChange;
	char* name = nullptr;
	char* str = nullptr;
	union {
		FuncObj func;
		VarObj var;
		ArgObj arg = {};
	};
	Val val = {};
	const Op getType();
	void setType(Op type);
	Op getMod();
	void setMod(Op mod);
	void setName(const char* name);
	void setStr(const char* Str);
	void print();
};
struct AllowedPfxs {
	std::vector<Op> pfxs = {};
	const char* err = NULL;
};
struct Task {
	Op type = Op::NotSet;
	std::vector<Obj> working = {};
	std::string code = {};
	std::string codePart1 = {};
	std::string codePart2 = {};
};
class Compiler {
public:
	int m_Line = 0, m_Column = 0;
	Op m_Pfx = Op::Null;
	std::string m_Str = {}, m_cOutput = {};
	std::stack<Obj> m_ObjStack = {};
	std::stack<AllowedPfxs> m_AllowedNextPfxsStack = {};
	Op m_Pointer = Op::NotSet;
	Op m_NameOp = Op::Null;
	char m_Ch = '\0';
	std::stack<bool> m_StrReadPtrsStack;
	bool m_StringMode = false;
	std::stack<Op> m_ModeStack;
	std::stack<Task> m_TaskStack;
	bool m_StrAllowSpace = false;
	Op m_CommentMode = Op::NotSet;
	int m_MultiLineOffCount = 0;
	NameInfoDB m_NameTypeCtx = {};

	Compiler();
	~Compiler();
	void pushTask(Op task);
	void popTask();
	Obj& pushObj(Obj obj);
	Obj& popObj(bool pushToWorking = true);
	void push(Op mode, bool strAllowSpace = false);
	Op pop();
	void pushAllowedNextPfxs(std::vector<Op> allowedNextPfxs, const char* err = nullptr);
	void popAllowedNextPfxs();
	bool isPfxExpected(Op pfx);
	//NO NEWLINES AT END OF STR
	void Char(char ch);
	void PopAndDoTask();
	const char* GetPrintfFmtForType(Op type);
	void Prefix();
	void Str();
	void StrPayload();
	void ExplainErr(Op code);
};
#define Err(code, msg){\
	PRINT_LINE_INFO();\
	printf(":%s At %u:%u \"%s\"(%d)\nExplanation: ", msg, m_Line, m_Column, GetOpName(code), (int)code);\
	ExplainErr(code);\
	printf("\n");\
	__debugbreak();\
}
#define SetObjType(type){\
	PRINT_LINE_INFO();\
	GetObj.setType(type);\
}
#define GetObj (m_ObjStack.top())
#define GetObjType (GetObj.getType())
#define PushPfxs(pfxs, msg){\
	PRINT_LINE_INFO();\
	pushAllowedNextPfxs(pfxs, msg);\
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
	{"null", Op::Null},{"no", Op::False},{"yes", Op::True},{"set", Op::Set},
	{"call", Op::Call},{"add", Op::SetAdd},{"func", Op::Func},{"~", Op::Comment},
	{"%", Op::VarType},{"=", Op::Value},{"@", Op::Done},{"ret", Op::Return},
	{"ext", Op::Imaginary},{"if", Op::If},{"else", Op::Else},{"use", Op::Use},
	{"build", Op::Build},{"space", Op::Space},{"+", Op::Add},{"-", Op::Subtract},
	{"*", Op::Multiply},{"/", Op::Divide},{"is", Op::Equals},{"neq", Op::NotEquals},
	{"lt", Op::LessThan},{"gt", Op::GreaterThan},{"lteq", Op::LessThanOrEquals},
	{"gteq", Op::GreaterThanOrEquals},{"{", Op::ScopeOpen},{"}", Op::ScopeClose},
	{"(", Op::ParenthesisOpen},{")", Op::ParenthesisClose},{"[", Op::BracketOpen},
	{"]", Op::BracketClose},{",", Op::Comma},{"$", Op::Name},{"for", Op::For},
	{"loop", Op::While},{"block", Op::Block},{"struct", Op::Struct},{"priv", Op::Private},
	{"pub", Op::Public},{"void", Op::Void},{"c8", Op::c8},{"u8", Op::u8},{"u16", Op::u16},
	{"u32", Op::u32},{"u64", Op::u64},{"i8", Op::i8},{"i16", Op::i16},{"i32", Op::i32},
	{"i64", Op::i64},{"f32", Op::f32},{"d64", Op::d64},{"pointer", Op::Pointer},
	{"double pointer", Op::DoublePointer},{"tripple pointer", Op::TripplePointer},
	{"ErrUnexpectedNextPfx", Op::ErrUnexpectedNextPfx},{"ModePrefixPass", Op::ModePrefixPass},
	{"ModeStrPass", Op::ModeStrPass},{"ModeComment", Op::ModeComment},{"NotSet", Op::NotSet},
	{"ModeMultiLineComment", Op::ModeMultiLineComment},{"FuncHasName", Op::FuncHasName},
	{"Return", Op::Return},{"FuncArgNameless", Op::FuncArgNameless},
	{"FuncArgComplete",Op::FuncArgComplete},{"FuncNeedsRetValType",Op::FuncNeedsRetValType},
	{"FuncSignatureComplete", Op::FuncSigComplete},{"VarNeedName", Op::VarNeedName},
	{"ErrExpectedVariablePfx",Op::ErrExpectedVariablePfx},{"VarComplete", Op::VarComplete},
	{"ErrNoTask", Op::ErrNoTask},{"FuncNeedRetVal",Op::FuncNeedRetVal},
	{"CompletedFunction",Op::CompletedFunction},{"ErrUnknownOpStr",Op::ErrUnknownOpStr},
	{"ErrNOT_GOOD", Op::ErrNOT_GOOD},{"FuncNeedName",Op::FuncNeedName},{"String", Op::String},
	{"VarComplete", Op::VarComplete},{"VarWantValue",Op::VarWantValue},{"LineEnd", Op::LineEnd},
	{"CPrintfHaveFmtStr",Op::CPrintfHaveFmtStr},{"FuncWantCode",Op::FuncWantCode},
};
OpNamePair pfxNames[] = {
	{"NULL", Op::Null},{"Value (=)", Op::Value},{"Op (@)", Op::Op},
	{"Comment (~)", Op::Comment},{"Name($)", Op::Name},
	{"VarType (%)", Op::VarType},{"Pointer (&)", Op::Pointer},
	{"Return (@ret)", Op::Return},{"Op::Unknown", Op::Unknown},
	{"String (\")", Op::String},{"LineEnd", Op::LineEnd},
};
OpNamePair cEquivelents[] = {
	{"void", Op::Void},{"return", Op::Return},
	{"int", Op::i32},{"unsigned int", Op::u32},
	{"long long", Op::i64},{"unsigned long long", Op::u64},
	{"short", Op::i16},{"char", Op::i8},{"char", Op::c8},
	{"unsigned short", Op::u16},{"unsigned char", Op::u8},
	{"*", Op::Pointer},{"**", Op::DoublePointer},
	{"***", Op::TripplePointer},{", ", Op::CommaSpace},
	{"(", Op::ParenthesisOpen},{")", Op::ParenthesisClose},
	{"{", Op::ScopeOpen},{"}", Op::ScopeClose},
	{"", Op::NotSet},
	{"extern", Op::Imaginary},
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
	return Op::Error;
}
Op fromPfxCh(char ch) {
	switch (ch) {
	case '@': return Op::Op;
	case '$': return Op::Name;
	case '%': return Op::VarType;
	case '&': return Op::Pointer;
	case '\"': return Op::String;
	case '\'': return Op::Char;
	case '=': return Op::Value;
	default: return Op::Unknown;
	}
}
void owStr(char** str, const char* with) {
	if (*str) free(*str);
	*str = _strdup(with);
}
Compiler::Compiler(){
	m_StrReadPtrsStack.push(false);
	PushPfxs({Op::Op}, "");
	push(Op::ModePrefixPass);
	pushObj({});
}
Compiler::~Compiler() {
	if (m_StringMode)Err(Op::ErrNOT_GOOD, "Reached end of file without closing string");
	if (!m_Str.empty()) StrPayload();
	SwitchTaskStackStart
		case Op::FuncSigComplete:
		case Op::FuncHasName: {
			SetObjType(Op::FuncSigComplete);
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
#if IB_DEBUG_EXTRA1
	printf("Push obj: ");
	if (!m_ObjStack.empty()) {
		GetObj.print();
		printf(" -> ");
	}
#endif
	m_ObjStack.push(obj);
#if IB_DEBUG_EXTRA1
	GetObj.print();
	printf("\n");
#endif
	return GetObj;
}
Obj& Compiler::popObj(bool pushToWorking) {
	if (pushToWorking){
		if (GetObjType == Op::NotSet)Err(Op::ErrNOT_GOOD, "");
		printf("To working: ");
		GetObj.print();
		printf("\n");
		GetTaskWorkingObjs.push_back(GetObj);
	}
#if IB_DEBUG_EXTRA1
	printf("Pop obj: ");
	GetObj.print();
#endif
	if (m_ObjStack.size() == 1)GetObj = {};
#if IB_DEBUG_EXTRA1
	printf(" -> ");
	GetObj.print();
	printf("\n");
#endif
	return GetObj;
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
	if (type != Op::NotSet) {
		printf("Type:%s(%d),", GetOpName(type), (int)type);
	}
	if(name)printf("Name:%s,", name);
	if (str)printf("Str:%s,", str);
	if (modifier != Op::NotSet) {
		printf("Mod:%s,", GetOpName(modifier));
	}
	/*if(u64)*/printf("Val:%I64u", u64);
	printf("]");
}
void Compiler::pushAllowedNextPfxs(std::vector<Op> allowedNextPfxs, const char* err) {
	m_AllowedNextPfxsStack.push({ allowedNextPfxs, err });
	if (!m_AllowedNextPfxsStack.top().pfxs.empty()) {
		printf(" push allowed next pfxs");
		for (auto& p : m_AllowedNextPfxsStack.top().pfxs) printf(", %s", GetPfxName(p));
		printf("\n");
	}
	else Err(Op::ErrNOT_GOOD, "pfx stack vec cannot be empty");
}
void Compiler::popAllowedNextPfxs() {
	m_AllowedNextPfxsStack.pop();
	if (!m_AllowedNextPfxsStack.top().pfxs.empty()) {
		printf(" allowed next pfxs after POP");
		for (auto& p : m_AllowedNextPfxsStack.top().pfxs) printf(", %s", GetPfxName(p));
		printf("\n");
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
		case Op::NotSet: {
			m_CommentMode = Op::Comment;
			push(Op::ModeComment);
			break;
		}
		case Op::Comment: {
			pop();
			push(Op::ModeMultiLineComment);
			m_CommentMode = Op::MultiLineComment;
			break;
		}
		case Op::MultiLineComment: {
			switch (m_MultiLineOffCount++) {
			case 0: break;
			case 1: {
				pop();
				m_MultiLineOffCount = 0;
				m_CommentMode = Op::NotSet;
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
		if(m_CommentMode==Op::NotSet)printf("Char():Line end\n");
		if (m_CommentMode == Op::Comment) {
			pop();
			m_CommentMode = Op::NotSet;
		}
		switch (m_ModeStack.top()) {
		case Op::ModeStrPass: {
			StrPayload();
			break; 
		}
		}
		switch (GetObjType) {
		case Op::VarWantValue: 
		case Op::VarComplete: {
			popObj(true);
			PopPfxs();
			break;
		}
		}
		SwitchTaskStackStart
			case Op::CPrintfHaveFmtStr: {
				PopAndDoTask();
				break;
			}
			case Op::FuncArgsVarNeedsName: {
				Err(Op::FuncArgsVarNeedsName,"");
				break;
			}
			case Op::FuncNeedRetVal: {
				Err(Op::FuncNeedRetVal, "");
				break;
			}
			//case Op::FuncSignatureComplete:
			case Op::FuncHasName: {
				SetObjType(Op::FuncSigComplete);
				PopPfxs();
				PushPfxs({ Op::Op,Op::String }, "");
				popObj(true);
				m_TaskStack.top().type = Op::FuncWantCode;
				break;
			}
		SwitchTaskStackEnd
		break;
	}
	}
	auto m = m_ModeStack.top();
	m_Column++;
	if (!nl && m_CommentMode == Op::NotSet) {
		if(this->m_Ch == ' ') printf("-> SPACE (0x%x)\n",  this->m_Ch);
		else printf("-> %c (0x%x)\n", this->m_Ch, this->m_Ch);
		switch (m) {
		case Op::ModePrefixPass:
			Prefix();
			break;
		case Op::ModeStrPass:
			Str();
			break;
		}
	}
	if (nl) {
		if (isPfxExpected(Op::LineEnd)) PopPfxs();
		m_Column = 0;
		m_Line++;
	}
}
const char* Compiler::GetPrintfFmtForType(Op type) {
	switch (type) {
	case Op::String: return "s";
	case Op::i32:    return "d";
	case Op::f32:    return "f";
	case Op::u32:    return "u";
	case Op::Char:   return "c";
	}
	return "???";
	//Err(Op::ErrNOT_GOOD, "GetPrintfFmtForType: unknown type");
}
void Compiler::PopAndDoTask()	{
	if(m_TaskStack.empty())Err(Op::ErrNoTask, "task stack EMPTY!");
	if(GetTaskWorkingObjs.empty())Err(Op::ErrNOT_GOOD, "workingObjs EMPTY!");
	switch (m_TaskStack.top().type) {
	case Op::FuncWantCode: {

		break;
	}
	case Op::FuncSigComplete:
	case Op::FuncHasName:
	case Op::Func: {
		std::string cFuncModsTypeName, cFuncArgs, cFuncCode;
		bool imaginary = false;
		Obj* funcObj=nullptr;
		for (int i = 0; i < GetTaskWorkingObjs.size(); ++i) {
			auto& o = GetTaskWorkingObjs[i];
			switch (o.getType()) {
			case Op::FuncArgComplete: {//multiple allowed
				auto at = o.arg.type;
				if (at == Op::Null)Err(Op::ErrNOT_GOOD, "arg type NULL");
				if(!cFuncArgs.empty())cFuncArgs += ", ";
				cFuncArgs += GetCEqu(o.arg.type);
				cFuncArgs += GetCEqu(o.arg.mod);
				cFuncArgs += " ";
				if (!o.name)Err(Op::ErrNOT_GOOD, "arg name NULL");
				cFuncArgs += std::string(o.name);			
				break;
			}
			case Op::FuncSigComplete: {
				if (o.getMod() == Op::Imaginary) {
					//printf("img\n");
					imaginary = true;
				}
			}
			case Op::FuncHasName:
			case Op::CompletedFunction: {//should only happen once
				funcObj = &o;
				auto mod = o.getMod();
				if (mod != Op::NotSet) {
					cFuncModsTypeName += GetCEqu(mod);
					cFuncModsTypeName += " ";
				}
				cFuncModsTypeName += GetCEqu(o.func.retType);
				cFuncModsTypeName += GetCEqu(o.func.retTypeMod);
				cFuncModsTypeName += " ";
				if (!o.name)Err(Op::ErrNOT_GOOD, "func name NULL");
				cFuncModsTypeName += std::string(o.name);
				cFuncModsTypeName += "(";
				break;
			}
			}
		}
		for (auto& o : GetTaskWorkingObjs) {
			switch (o.getType()) {
			case Op::VarComplete: {
				cFuncCode += "\t";	
				cFuncCode += GetCEqu(o.var.type);
				cFuncCode += GetCEqu(o.var.mod);
				cFuncCode += " ";
				if(!o.name)Err(Op::ErrNOT_GOOD, "var name NULL");
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
			if(!funcObj)Err(Op::ErrNOT_GOOD, "funcObj NULL");
			if (funcObj->func.retType != Op::Void) {
				cFuncCode += "\treturn ";
				switch (funcObj->func.retType) {
				case Op::u8:  { cFuncCode += std::to_string(funcObj->func.retVal.u8);  break; }
				case Op::c8:  { cFuncCode += std::to_string(funcObj->func.retVal.c8);  break; }
				case Op::i16: { cFuncCode += std::to_string(funcObj->func.retVal.i16); break; }
				case Op::u16: { cFuncCode += std::to_string(funcObj->func.retVal.u16); break; }
				case Op::i32: { cFuncCode += std::to_string(funcObj->func.retVal.i32); break; }
				case Op::i64: { cFuncCode += std::to_string(funcObj->func.retVal.i64); break; }
				case Op::u32: { cFuncCode += std::to_string(funcObj->func.retVal.u32); break; }
				case Op::u64: { cFuncCode += std::to_string(funcObj->func.retVal.u64); break; }
				case Op::f32: { cFuncCode += std::to_string(funcObj->func.retVal.f32); break; }
				case Op::d64: { cFuncCode += std::to_string(funcObj->func.retVal.d64); break; }
				}
				cFuncCode+=";\n";
			}
			cFuncCode += "}\n\n";
		}
		//printf("%s\n", std::string(cFuncModsTypeName+cFuncArgs+cFuncCode).c_str());
		m_cOutput += std::string(cFuncModsTypeName + cFuncArgs + cFuncCode);
		break;
	}
	case Op::CPrintfHaveFmtStr: {
		Obj& fmtObj = GetTaskWorkingObjs.front();
		GetTaskCode += "printf(\"";
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
						switch (GetTaskWorkingObjs[varIdx].getType()) {
						case Op::Name:{
							//get type of variable
							//func args
							//func block vars

							//break;
						}
						case Op::Value:{
								GetTaskCode += GetPrintfFmtForType(GetTaskWorkingObjs[varIdx].var.type);
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
			case Op::Name: {
				GetTaskCode += o.name;
				break;
			}
			case Op::String: {
				GetTaskCode += "\"";
				GetTaskCode += std::string(o.str);
				GetTaskCode += "\"";
				break;
			}
			case Op::Value: {
				GetTaskCode += std::to_string(o.val.i32);//for now
			}
			}
			if(i < GetTaskWorkingObjs.size()-1) GetTaskCode += ", ";
		}
		GetTaskCode += ");\n";
		break;
	}
	}
	popTask();
	GetTaskWorkingObjs.clear();
}
void Compiler::Prefix(){
	m_Pfx = fromPfxCh(m_Ch);
	auto& obj = GetObj;
	if (m_Pfx != Op::Unknown 
		&& !(m_AllowedNextPfxsStack.top().pfxs.empty())
		&& !isPfxExpected(m_Pfx))
		Err(Op::ErrUnexpectedNextPfx, "");
	printf("PFX:%s(%d)\n", GetPfxName(m_Pfx), (int)m_Pfx);
	switch (m_Pfx) {
	case Op::String: { //"
		m_StringMode = true;
		push(Op::ModeStrPass);
		break;
	}
	case Op::VarType:
		m_StrReadPtrsStack.push(true);
	case Op::Value:
	case Op::Op:
	case Op::Name:
		push(Op::ModeStrPass);
		break;
	case Op::Comment:
		break;
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
	switch (m_Ch) {
	case '\t': return;
	case ' ': {
		if (m_StrAllowSpace) break;
		else return StrPayload();
	}
	case '&': {
		if (m_StrReadPtrsStack.top()) {
			switch (m_Pointer) {
			case Op::NotSet:
				printf("got pointer\n");
				m_Pointer = Op::Pointer;
				break;
			case Op::Pointer:
				printf("got double pointer\n");
				m_Pointer = Op::DoublePointer;
				break;
			case Op::DoublePointer:
				printf("got tripple pointer\n");
				m_Pointer = Op::TripplePointer;
				break;
			case Op::TripplePointer:
				Err(Op::ErrQuadriplePointersNOT_ALLOWED, "");
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
	case Op::String: { //"
		SwitchTaskStackStart
		case Op::FuncWantCode: { //printf
			pushTask(Op::CPrintfHaveFmtStr);
			pushObj({});
			GetObj.setStr(cs);
			GetObj.setType(Op::CPrintfFmtStr);
			popObj(true);
			auto allowed = { Op::Value, Op::Name, Op::String, Op::LineEnd };
			PushPfxs(allowed, "expected fmt args or line end");
			break;
		}
		SwitchTaskStackEnd
		break;
	}
	case Op::Value: { //=
		switch (GetObjType) {
		case Op::VarWantValue: {
			GetObj.var.val = strVal;
			SetObjType(Op::VarComplete);
			break;
		}
		}
		if (!m_TaskStack.empty()) {
			switch (m_TaskStack.top().type) {
			case Op::CPrintfHaveFmtStr:{
				pushObj({});
				//GetObj.setStr(cs);
				GetObj.val = strVal;
				GetObj.setType(Op::Value);
				GetObj.var.type = Op::i32;//for now
				popObj(true);
				//PopPfxs();
				break;
			}
			case Op::FuncNeedRetVal:
				for (auto& obj : GetTaskWorkingObjs) {
					if (obj.getType() == Op::FuncSigComplete) {
						printf("Finishing func got ret value\n");
						obj.func.retVal = strVal;
						//SetObjType(Op::CompletedFunction);
						PopPfxs();
						SetTaskType(Op::Func);
						PopAndDoTask();
						break;
					}
				}
				break;
			}
		}
		break;
	}
	case Op::VarType: //%
		switch (m_TaskStack.top().type) {
		case Op::FuncWantCode: {
			pushObj({});
			GetObj.var.type = m_NameOp;
			GetObj.var.mod = Op::NotSet;
			SetObjType(Op::VarNeedName);
			PushPfxs({ Op::Name }, "Expected variable name after variable type");
			break;
		}
		}
		switch (GetObjType) {
		case Op::FuncNeedsRetValType: {
			if (GetTaskType != Op::FuncHasName)Err(Op::ErrNOT_GOOD, "func signature needs name");
			GetObj.func.retType = m_NameOp;
			GetObj.func.retTypeMod = m_Pointer;
			SetObjType(Op::FuncSigComplete);
			if (GetObj.getMod() == Op::Imaginary) {
				SetTaskType(Op::FuncSigComplete);
			}
			else {
				SetTaskType(Op::FuncWantCode);
			}
			popObj(true);
			break;
		}
		case Op::FuncHasName:
			pushObj({});
			SetObjType(Op::FuncArgNameless);
			GetObj.arg.type= m_NameOp;
			GetObj.arg.mod = m_Pointer;
			PushPfxs({Op::Name}, "");
			break;
		}
		break;
	case Op::Name: //$
		switch (GetObjType){
		case Op::Func:
			SetObjType(Op::FuncHasName);
			SetTaskType(Op::FuncHasName);
			PopPfxs();
			PushPfxs({Op::VarType,Op::LineEnd/*means allowed pfx will be cleared on newline*/}, "");
			GetObj.setName(cs);
			break;
		case Op::FuncArgNameless:
			SetObjType(Op::FuncArgComplete);
			PopPfxs();
			GetObj.setName(cs);
			m_NameTypeCtx.add(cs, GetObj.arg.type);
			popObj(true);
			break;
		case Op::VarNeedName:
			GetObj.setName(cs);
			m_NameTypeCtx.add(cs, GetObj.var.type);
			SetObjType(Op::VarWantValue);
			//PushPfxs({ Op::Value });
			PopPfxs();
			break;
		}
		break;
	case Op::Op: //@
		switch (m_NameOp) {
		case Op::Imaginary:
			GetObj.setMod(m_NameOp);
			//setAllowedNextPfxs({});
			PopPfxs();
			break;
		case Op::Done:
			if (m_TaskStack.empty()) Err(Op::ErrNoTask, "");
			switch (GetTaskType) {
			case Op::Func:
			case Op::FuncHasName:
			case Op::FuncWantCode:
				printf("Finishing function\n");
				for (auto& obj : GetTaskWorkingObjs) {
					//TODO: could cache func obj index later
					if (obj.getType() == Op::FuncSigComplete) {
						if (obj.func.retType != Op::Void) {
							PushPfxs({Op::Value},"");
							SetTaskType(Op::FuncNeedRetVal);
						}else {
							SetTaskType(Op::Func);
							PopAndDoTask();
							PopPfxs();
						}
					}
				}
				break;
			}
			break;
		case Op::Return: {
			auto t = GetObjType;
			switch (t) {
			case Op::FuncHasName:
				SetObjType(Op::FuncNeedsRetValType);
				PushPfxs({ Op::VarType },"");
				break;
			default:
				Err(Op::ErrUnexpectedOp, "");
				break;
			}
			break;
		}
		case Op::Func:
			if (GetObjType != Op::NotSet)Err(Op::ErrNOT_GOOD, "");
			SetObjType(m_NameOp);
			GetObj.func.retType = Op::Void;
			GetObj.func.retTypeMod = Op::NotSet;
			PushPfxs({Op::Name}, "");
			pushTask(Op::FuncNeedName);
			break;
		case Op::Public:
		case Op::Private:
			GetObj.privacy = m_NameOp;
			break;
		default:
			Err(Op::ErrUnknownOpStr, "");
		}
	}
	m_Str.clear();
	printf("Str payload complete\n");
	pop();
	if(m_StrReadPtrsStack.size() > 1)
	{
		if (m_StrReadPtrsStack.top()) m_Pointer = Op::NotSet;
		m_StrReadPtrsStack.pop();
	}
}
void Compiler::ExplainErr(Op code) {
	switch (code) {
	case Op::ErrUnknownOpStr:
		printf("Unknown OP str @%s\n", m_Str.c_str());
		break;
	case Op::ErrQuadriplePointersNOT_ALLOWED:
		printf("Why?");
		break;
	case Op::ErrUnexpectedOp:
		printf("Unexpected OP");
		break;
	case Op::ErrNoTask:
		printf("No working task to call done (@@) for");
		break;
	case Op::ErrUnexpectedNextPfx:
		printf("%s Unexpected next prefix %s. Pfx stack idx:%zd Allowed:", m_AllowedNextPfxsStack.top().err, GetPfxName(m_Pfx), m_AllowedNextPfxsStack.size() - 1);
		for (auto& p : m_AllowedNextPfxsStack.top().pfxs)
			printf("%s,", GetPfxName(p));
		break;
	case Op::ErrExpectedVariablePfx:
		printf("Expected a variable type to be next.");
		break;
	default:
		printf("Err msg unimplemented for %s", GetOpName(code));
	}
	printf("\nOBJ:");
	GetObj.print();
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
	return Op::NotFound;
}
