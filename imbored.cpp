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

//multiple uses
enum class Op {
	Null,False,True,Unknown,NotSet,Any,Use,Build,Space,

	Func,
	__FuncBuildingStart__,
	FuncHasName,FuncNeedName,FuncNeedsRetValType,FuncArgsVarNeedsName,
	FuncArgNameless,FuncArgComplete,FuncNeedVarsAndCode,
	FuncSignatureComplete,FuncNeedReturnValue,CompletedFunction,
	__FuncBuildingEnd__,

	__VariableBuildingStart__,
	VarNeedName,VarComplete,
	__VariableBuildingEnd__,

	Op,Value,Done,Return,NoChange,Struct,VarType,
	Comment,MultiLineComment,Public,Private,Imaginary,Void,
	Set,SetAdd,Call,Colon,Dot,Add,Subtract,Multiply,Divide,
	AddEq,SubEq,MultEq,DivEq,Equals,NotEquals,LessThan,
	GreaterThan,LessThanOrEquals,GreaterThanOrEquals,
	ScopeOpen, ScopeClose,
	ParenthesisOpen, ParenthesisClose,
	BracketOpen, BracketClose,

	Comma,CommaSpace,Name,String,Char,If,Else,For,While,Block,
	c8,	u8, u16, u32, u64, i8, i16, i32, i64,f32, d64,
	Pointer,DoublePointer,TripplePointer,CompilerFlags,

	Error, ErrNOT_GOOD, ErrUnexpectedNextPfx,
	ErrExpectedVariablePfx, ErrNoTask,ErrUnexpectedOp,
	ErrQuadriplePointersNOT_ALLOWED, ErrUnknownOpStr,

	ModePrefixPass,ModeStrPass,ModeComment,ModeMultiLineComment,
};
typedef union Val {
	void (*vrFunc)(void*);
	int (*irFunc)(void*);
	Op op;
	unsigned char u8;
	unsigned short u16;
	unsigned int u32;
	unsigned __int64 u64;
	char i8;
	char ch;
	short i16;
	int i32;
	__int64 i64;
	float f32;
	double d64;
	void* ptr;
	char* str;
} Val;
struct FuncObj {
	Val retVal = {};
	Op retType = Op::NotSet;
	Op retTypeMod = Op::NotSet;
};
struct ArgObj {
	Op type = Op::Null;
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
	char* err = NULL;
};
class Compiler {
public:
	int line = 0, column = 0;
	Op m_Pfx = Op::Null;
	std::string str = {};
	std::stack<Obj> objStack = {};
	std::stack<AllowedPfxs> allowedNextPfxsStack = {};
	Op m_Pointer = Op::NotSet;
	Op m_NameOp = Op::Null;
	char ch = '\0';
	std::stack<bool> opAllowedStack, strReadPtrsStack;
	std::stack<Op> modeStack, taskStack;
	std::vector<Obj> workingObjs;
	bool strAllowSpace = false;
	Op commentMode = Op::NotSet;
	int multiLineOffCount = 0;
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
	void Prefix();
	void Str();
	void StrPayload();
	void ExplainErr(Op code);
};
#define Err(code, msg){\
	PRINT_LINE_INFO();\
	printf(":%s At %u:%u \"%s\"(%d)\nExplanation: ", msg, line, column, GetOpName(code), (int)code);\
	ExplainErr(code);\
	printf("\n");\
	__debugbreak();\
}
struct OpNamePair {
	char name[OP_NAME_LEN];
	Op op;
};
OpNamePair opNames[] = {
	{"null", Op::Null},
	{"no", Op::False},
	{"yes", Op::True},
	{"set", Op::Set},
	{"call", Op::Call},
	{"add", Op::SetAdd},
	{"func", Op::Func},
	{"~", Op::Comment},
	{"%", Op::VarType},
	{"=", Op::Value},
	{"@", Op::Done},
	{"ret", Op::Return},
	{"ext", Op::Imaginary},
	{"if", Op::If},
	{"else", Op::Else},
	{"use", Op::Use},
	{"build", Op::Build},
	{"space", Op::Space},
	{"+", Op::Add},
	{"-", Op::Subtract},
	{"*", Op::Multiply},
	{"/", Op::Divide},
	{"is", Op::Equals},
	{"neq", Op::NotEquals},
	{"lt", Op::LessThan},
	{"gt", Op::GreaterThan},
	{"lteq", Op::LessThanOrEquals},
	{"gteq", Op::GreaterThanOrEquals},
	{"{", Op::ScopeOpen},
	{"}", Op::ScopeClose},
	{"(", Op::ParenthesisOpen},
	{")", Op::ParenthesisClose},
	{"[", Op::BracketOpen},
	{"]", Op::BracketClose},
	{",", Op::Comma},
	{"$", Op::Name},
	{"for", Op::For},
	{"loop", Op::While},
	{"block", Op::Block}, //maybe use for templates
	{"struct", Op::Struct},
	{"priv", Op::Private},
	{"pub", Op::Public},
	{"void", Op::Void},
	{"c8", Op::c8},
	{"u8", Op::u8},
	{"u16", Op::u16},
	{"u32", Op::u32},
	{"u64", Op::u64},
	{"i8", Op::i8},
	{"i16", Op::i16},
	{"i32", Op::i32},
	{"i64", Op::i64},
	{"f32", Op::f32},
	{"d64", Op::d64},
	{"pointer", Op::Pointer},
	{"double pointer", Op::DoublePointer},
	{"tripple pointer", Op::TripplePointer},
	{"ErrUnexpectedNextPfx", Op::ErrUnexpectedNextPfx},
	{"ModePrefixPass", Op::ModePrefixPass},
	{"ModeStrPass", Op::ModeStrPass},
	{"ModeComment", Op::ModeComment},
	{"ModeMultiLineComment", Op::ModeMultiLineComment},
	{"FuncHasName", Op::FuncHasName},
	{"NotSet", Op::NotSet},
	{"Return", Op::Return},
	{"FuncArgNameless", Op::FuncArgNameless},
	{"FuncArgComplete",Op::FuncArgComplete},
	{"FuncNeedsRetValType",Op::FuncNeedsRetValType},
	{"FuncSignatureComplete", Op::FuncSignatureComplete},
	{"VarNeedName", Op::VarNeedName},
	{"VarComplete", Op::VarComplete},
	{"ErrNoTask", Op::ErrNoTask},
	{"ErrExpectedVariablePfx",Op::ErrExpectedVariablePfx},
	{"FuncNeedReturnValue",Op::FuncNeedReturnValue},
	{"CompletedFunction",Op::CompletedFunction},
	{"ErrUnknownOpStr",Op::ErrUnknownOpStr},
	{"ErrNOT_GOOD", Op::ErrNOT_GOOD},
};
OpNamePair pfxNames[] = {
	{"NULL", Op::Null},
	{"Value (=)", Op::Value},
	{"Op (@)", Op::Op},
	{"Comment (~)", Op::Comment},
	{"Name($)", Op::Name},
	{"VarType (%)", Op::VarType},
	{"Pointer (&)", Op::Pointer},
	//{"LineEnd (\\n)", Op::LineEnd},
	{"Return (@ret)", Op::Return},
};
OpNamePair cEquivelents[] = {
	{"void", Op::Void},
	{"return", Op::Return},
	{"int", Op::i32},
	{"short", Op::i16},
	{"char", Op::i8},
	{"char", Op::c8},
	{"*", Op::Pointer},
	{"**", Op::DoublePointer},
	{"***", Op::TripplePointer},
	{", ", Op::CommaSpace},
	{"(", Op::ParenthesisOpen},
	{")", Op::ParenthesisClose},
	{"{", Op::ScopeOpen},
	{"}", Op::ScopeClose},
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
	opAllowedStack.push(true);
	strReadPtrsStack.push(false);
	allowedNextPfxsStack.push({});
	push(Op::ModePrefixPass);
	pushObj({});
}Compiler::~Compiler() {
	if (!str.empty()) StrPayload();
	printf("-> Compilation complete <-\n");
}
void Compiler::pushTask(Op task) {
	printf("Push task %s(%d)\n", GetOpName(task),(int)task);
	taskStack.push(task);
}
void Compiler::popTask() {
	printf("Pop task %s(%d)\n", GetOpName(taskStack.top()),(int)taskStack.top());
	taskStack.pop();
}
Obj& Compiler::pushObj(Obj obj) {
#if IB_DEBUG_EXTRA1
	if (!objStack.empty()) {
		printf("Obj bef push:");
		objStack.top().print();
	}
#endif
	objStack.push(obj);
#if IB_DEBUG_EXTRA1
	printf("Obj aft push:");
	objStack.top().print();
#endif
	return objStack.top();
}
Obj& Compiler::popObj(bool pushToWorking) {
	if (pushToWorking)
	{
		if (objStack.top().getType() == Op::NotSet)Err(Op::ErrNOT_GOOD, "");
		printf("pushed to working: ");
		objStack.top().print();
		workingObjs.push_back(objStack.top());
	}
#if IB_DEBUG_EXTRA1
	printf("Obj before pop:");
	objStack.top().print();
#endif
	objStack.pop();
	if (objStack.empty()) {
		printf("objstack EMPTY. adding empty obj\n");
		pushObj({});
	}
#if IB_DEBUG_EXTRA1
	printf("Obj after pop:");
	objStack.top().print();
#endif
	return objStack.top();
}
void Compiler::push(Op mode, bool strAllowSpace){
	this->strAllowSpace = strAllowSpace;
	modeStack.push(mode);
	printf("push: to %s\n", GetOpName(modeStack.top()));
}
Op Compiler::pop() {
	modeStack.pop();
	printf("pop: to %s\n", GetOpName(modeStack.top()));
	return modeStack.top();
}
const Op Obj::getType() { return type; }
void Obj::setType(Op type) {
	printf("obj type: %s(%d) -> %s(%d)\n", GetOpName(this->type), (int)this->type, GetOpName(type), (int)type);
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
	printf("[Type:%s(%d),Name:%s,Str:%s,Mod:%s,Val:%I64u]\n",
		GetOpName(type), (int)type, name, str, GetOpName(modifier), u64);
}
void Compiler::pushAllowedNextPfxs(std::vector<Op> allowedNextPfxs, const char* err) {
	//allowedNextPfxsStack.push(allowedNextPfxs); //soon(TM)
	allowedNextPfxsStack.top().pfxs = allowedNextPfxs;
	if(err) owStr(&allowedNextPfxsStack.top().err, err);
	if (!allowedNextPfxsStack.top().pfxs.empty()) {
		printf("set allowed next pfxs to");
		for (auto& p : allowedNextPfxsStack.top().pfxs) printf(", %s", GetPfxName(p));
		printf("\n");
	}else printf("push allowed next pfxs to EMPTY\n");
}
void Compiler::popAllowedNextPfxs() {
	if (allowedNextPfxsStack.top().err)free(allowedNextPfxsStack.top().err);
	allowedNextPfxsStack.pop();
	if (allowedNextPfxsStack.empty())allowedNextPfxsStack.push({});
}
bool Compiler::isPfxExpected(Op pfx) {
	if (allowedNextPfxsStack.top().pfxs.empty()) return true;
	if (pfx == Op::Op) return opAllowedStack.top();
	for (auto& p : allowedNextPfxsStack.top().pfxs) if (p == pfx) return true;
	return false;
}
//NO NEWLINES AT END OF STR
void Compiler::Char(char ch){
	this->ch = ch;
	bool nl = false;
	switch (this->ch) {
	case COMMENT_CHAR: {
		switch (commentMode) {
		case Op::NotSet: {
			commentMode = Op::Comment;
			push(Op::ModeComment);
			break;
		}
		case Op::Comment: {
			pop();
			push(Op::ModeMultiLineComment);
			commentMode = Op::MultiLineComment;
			break;
		}
		case Op::MultiLineComment: {
			switch (multiLineOffCount++) {
			case 0: break;
			case 1: {
				pop();
				multiLineOffCount = 0;
				commentMode = Op::NotSet;
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
		if(commentMode==Op::NotSet)printf("Char():Line end\n");
		if (commentMode == Op::Comment) {
			pop();
			commentMode = Op::NotSet;
		}
		switch (modeStack.top()) {
		case Op::ModePrefixPass:
			break;
		case Op::ModeStrPass: {
			StrPayload();
			//pushAllowedNextPfxs({});
			break; }
		}
		if (!taskStack.empty()) {
			switch (taskStack.top()) {
			case Op::FuncArgsVarNeedsName: {
				Err(Op::FuncArgsVarNeedsName,"");
				break;
			}
			case Op::FuncNeedReturnValue: {
				Err(Op::FuncNeedReturnValue, "");
				break;
			}
			case Op::FuncSignatureComplete:
			case Op::FuncHasName: {
				//popObj(true);
				PopAndDoTask();
				break;
			}
			}
		}
		break;
	}
	}
	auto m = modeStack.top();
	column++;
	if (!nl && commentMode == Op::NotSet) {
		if(this->ch == ' ') printf("-> SPACE (0x%x)\n",  this->ch);
		else printf("-> %c (0x%x)\n", this->ch, this->ch);
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
		column = 0;
		line++;
	}
}
void Compiler::PopAndDoTask()	{
	if(taskStack.empty())Err(Op::ErrNoTask, "task stack EMPTY!");
	switch (taskStack.top()) {
	case Op::FuncNeedVarsAndCode: {

		break;
	}
	case Op::FuncSignatureComplete:
	case Op::FuncHasName:
	case Op::Func: {
		std::string cFuncModsTypeName, cFuncArgs, cFuncCode;
		bool imaginary = false;
		Obj* funcObj=nullptr;
		for (int i = 0; i < workingObjs.size(); ++i) {
			auto& o = workingObjs[i];
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
			case Op::FuncSignatureComplete: {
				if (o.getMod() == Op::Imaginary) {
					//printf("img\n");
					imaginary = true;
				}
			}
			case Op::FuncHasName:
			case Op::CompletedFunction://should only happen once
				funcObj = &o;
				auto mod = o.getMod();
				if (mod != Op::NotSet) {
					cFuncModsTypeName += GetCEqu(mod);
					cFuncModsTypeName += " ";
				}
				cFuncModsTypeName += GetCEqu(o.func.retType);
				cFuncModsTypeName += GetCEqu(o.func.retTypeMod);
				cFuncModsTypeName += " ";
				if(!o.name)Err(Op::ErrNOT_GOOD, "func name NULL");
				cFuncModsTypeName += std::string(o.name);
				cFuncModsTypeName += "(";
				//printf("AAA");
				break;
			}
		}
		if (imaginary) {
			cFuncArgs += ");\n";
			//cFuncCode += "}";
		}
		else {
			cFuncArgs += "){\n";
			if (funcObj->func.retType != Op::Void) {
				cFuncCode += ("\treturn " + std::to_string(funcObj->func.retVal.i32) + ";\n");//TODO:FIX!!!!!!!!!!!!!
			}
			cFuncCode += "}";
		}
		printf("%s\n", std::string(cFuncModsTypeName+cFuncArgs+cFuncCode).c_str());
		break;
	}
	}
	popTask();
	workingObjs.clear();
}
void Compiler::Prefix(){
	m_Pfx = fromPfxCh(ch);
	//if (pfx == Op::LineEnd) return;
	auto& obj = objStack.top();
	if (m_Pfx != Op::Unknown 
		&& !(allowedNextPfxsStack.top().pfxs.empty())
		&& !isPfxExpected(m_Pfx))
		Err(Op::ErrUnexpectedNextPfx, "");
	printf("PFX:%s\n", GetPfxName(m_Pfx));
	switch (m_Pfx) {
	case Op::VarType:
		strReadPtrsStack.push(true);
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
	switch (ch) {
	case '\t': return;
	case ' ': {
		if (strAllowSpace) break;
		else return StrPayload();
	}
	case '&': {
		if (strReadPtrsStack.top()) {
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
	str.append(1, ch);
}
void Compiler::StrPayload(){
	printf("Doing Str payload\n");
	auto cs = str.c_str();
	Val strVal = {};
	strVal.i64=strtoll(cs, nullptr, 10);
	m_NameOp = GetOpFromName(cs);
	printf("Str: %s\n", cs);
	switch (m_Pfx)
	{
	case Op::Value: { //=
		if (!taskStack.empty()) {
			switch (taskStack.top()) {
			case Op::FuncNeedReturnValue:
				for (auto& obj : workingObjs) {
					if (obj.getType() == Op::FuncSignatureComplete) {
						printf("Finishing func got ret value\n");
						obj.func.retVal = strVal;
						//obj.setType(Op::CompletedFunction);
						taskStack.top() = Op::Func;
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
		switch (taskStack.top()) {
		case Op::FuncNeedVarsAndCode: {
			pushObj({});
			objStack.top().arg.type = m_NameOp;
			objStack.top().setType(Op::VarNeedName);
			pushAllowedNextPfxs({ Op::Name }, "Expected variable name after variable type");
			break;
		}
		}
		switch (objStack.top().getType()) {
		case Op::NotSet:
			objStack.top().setType(Op::VarNeedName);
			break;
		case Op::FuncNeedsRetValType: {
			opAllowedStack.pop();
			objStack.top().func.retType = m_NameOp;
			objStack.top().func.retTypeMod = m_Pointer;
			objStack.top().setType(Op::FuncSignatureComplete);
			if(taskStack.top() != Op::FuncHasName)Err(Op::ErrNOT_GOOD, "func signature needs name");
			if (objStack.top().getMod() == Op::Imaginary) {
				taskStack.top() = Op::FuncSignatureComplete;
			}
			else {
				taskStack.top() = Op::FuncNeedVarsAndCode;
			}
			popObj(true);
			break;
		}
		case Op::FuncHasName:
			pushObj({});
			objStack.top().setType(Op::FuncArgNameless);
			objStack.top().arg.type= m_NameOp;
			objStack.top().arg.mod = m_Pointer;
			pushAllowedNextPfxs({Op::Name});
			break;
		}
		break;
	case Op::Name: //$
		switch (objStack.top().getType()){
		case Op::Func:
			objStack.top().setType(Op::FuncHasName);
			taskStack.top() = Op::FuncHasName;
			pushAllowedNextPfxs({Op::VarType });
			objStack.top().setName(cs);
			break;
		case Op::FuncArgNameless:
			objStack.top().setType(Op::FuncArgComplete);
			pushAllowedNextPfxs({ Op::VarType });
			objStack.top().setName(cs);
			popObj(true);
			break;
		case Op::VarNeedName:
			objStack.top().setName(cs);
			objStack.top().setType(Op::VarComplete);
			popObj(true);
			break;
		}
		break;
	case Op::Op: //@
		switch (m_NameOp) {
		case Op::Imaginary:
			objStack.top().setMod(m_NameOp);
			//setAllowedNextPfxs({});
			popAllowedNextPfxs();
			break;
		case Op::Done:
			if (taskStack.empty()) Err(Op::ErrNoTask, "");
			switch (taskStack.top()) {
			case Op::Func:
			//case Op::CompletedFunction:
			case Op::FuncNeedVarsAndCode:
				printf("Finishing function\n");
				for (auto& obj : workingObjs) {
					//TODO: could cache func obj index later
					if (obj.getType() == Op::FuncSignatureComplete) {
						if (obj.func.retType != Op::Void) {
							pushAllowedNextPfxs({Op::Value});
							taskStack.top() = Op::FuncNeedReturnValue;
						}else {
							obj.setType(Op::CompletedFunction);
							popAllowedNextPfxs();
						}
					}
				}
				break;
			}
			break;
		case Op::Return: {
			auto t = objStack.top().getType();
			switch (t) {
			case Op::FuncHasName:
				opAllowedStack.push(false);
				objStack.top().setType(Op::FuncNeedsRetValType);
				pushAllowedNextPfxs({ Op::VarType });
				break;
			default:
				Err(Op::ErrUnexpectedOp, "");
				break;
			}
			break;
		}
		case Op::Func:
			if (objStack.top().getType() != Op::NotSet)Err(Op::ErrNOT_GOOD, "");
			objStack.top().setType(m_NameOp);
			objStack.top().func.retType = Op::Void;
			objStack.top().func.retTypeMod = Op::NotSet;
			pushAllowedNextPfxs({Op::Name});
			pushTask(Op::FuncNeedName);
			break;
		case Op::Public:
		case Op::Private:
			objStack.top().privacy = m_NameOp;
			break;
		default:
			Err(Op::ErrUnknownOpStr, "");
		}
	}
	str.clear();
	printf("Str payload complete\n");
	pop();
	if(strReadPtrsStack.size() > 1)
	{
		if (strReadPtrsStack.top()) m_Pointer = Op::NotSet;
		strReadPtrsStack.pop();
	}
}
void Compiler::ExplainErr(Op code) {
	switch (code) {
	case Op::ErrUnknownOpStr:
		printf("Unknown OP str @%s\n", str.c_str());
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
		printf("Unexpected next prefix %s. Allowed:", GetPfxName(m_Pfx));
		for (auto& p : allowedNextPfxsStack.top().pfxs)
			printf("%s,", GetPfxName(p));
		break;
	case Op::ErrExpectedVariablePfx:
		printf("Expected a variable type to be next.");
		break;
	default:
		printf("Err msg unimplemented for %s", GetOpName(code));
	}
	printf("\nOBJ MEM:");
	objStack.top().print();
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