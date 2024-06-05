#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>
#include <stack>
#include <string>

//not actually a compiler
//ascii only, maybe utf8 later...
//transpile to C
//no order of operations, sequential ONLY

//compiler options inside source code, preferably using code

#define OBJ_NAME_LEN 64
#define OP_NAME_LEN 32

//multiple uses
enum class Op {
	Null,//      nul
	False,//     no
	True,//      yes
	Unknown,// ?????
	Use,//       use
	Build,//     build
	Space,//     space
	Func,//      func
	__FuncBuildingStart__,
	FuncHasName, //have name
	FuncArgsVarNeedsName,
	FuncArgNameless,
	FuncArgComplete,
	FuncArgsEnd,
	__FuncBuildingEnd__,
	FuncComplete,
	LineEnd,
	Op,
	Value,//     =
	Done,//      @
	Return,//    ret
	NoChange,
	Struct,
	VarType,
	Comment,
	Public,
	Private,
	Imaginary, //satisfied at link time (extern)
	Void,

	Set,//                 set
	SetAdd,//			   add
	Colon,//               :
	Dot,//                 .
	Add,//                 +
	Subtract,//            -
	Multiply,//            *
	Divide,//              /
	AddEq,//               +=
	SubEq,//               -=
	MultEq,//              *=
	DivEq,//               /*
	Equals,//              eq
	NotEquals,//           neq
	LessThan,//            lt
	GreaterThan,//         gt
	LessThanOrEquals,//    lteq
	GreaterThanOrEquals,// gteq

	ScopeOpen, ScopeClose,
	ParenthesisOpen, ParenthesisClose,
	BracketOpen, BracketClose,
	
	Comma,
	Name, //func name, var name
	String,
	Char,

	If,
	Else,
	For,
	While,

	Block,//block of variables and code
	c8, //char
	u8, u16, u32, u64, //unsigned
	i8, i16, i32, i64, //signed
	f32, d64, //float, double
	Pointer,

	CompilerFlags,
	

	Error,
	ErrNOT_GOOD,
	ErrUnexpectedNextPfx,
	ErrExpectedVariablePfx,

	//compiler modes
	ModePrefixPass,
	ModeStrPass,
	ModeComment,
	ModeMultiLineComment,
	ModeString
};

typedef union Val {
	void (*vrFunc)(void*);
	int (*irFunc)(void*);
	Op op;
	unsigned char u8;
	unsigned short u16;
	unsigned int u32;
	unsigned long long u64;
	char i8;
	char ch;
	short i16;
	int i32;
	long long i64;
	float f32;
	double d64;
	void* ptr;
	char* str;
} Val;

void owStr(char** str, const char* with) {
	if (*str) free(*str);
	*str = _strdup(with);
}

struct Obj {
	Op type = Op::Null;
	Op privacy = Op::Public;
	char* name = nullptr;
	char* str = nullptr;
	void setName(const char* name) {
		owStr(&this->name, name);
	}
	void setStr(const char* Str) {
		owStr(&this->str, str);
	}
	Op op;
	Val val = {};
	/*std::vector<Obj>* children = nullptr;*/
};

struct OpNamePair {
	char name[OP_NAME_LEN];
	Op op;
};

OpNamePair opNames[] = {
	{"null", Op::Null},
	{"no", Op::False},
	{"set", Op::Set},
	{"add", Op::SetAdd},
	{"yes", Op::True},
	{"func", Op::Func},
	{"~", Op::Comment},
	{"%", Op::VarType},
	{"=", Op::Value},
	{"@", Op::Done},
	{"ret", Op::Return},
	{"if", Op::If},
	{"else", Op::Else},
	{"use", Op::Use},
	{"build", Op::Build},
	{"space", Op::Space},
	//{"cast", Op::Cast},
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
	{"private", Op::Private},
	{"public", Op::Public},
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
	{"&", Op::Pointer},
	{"ErrUnexpectedNextPfx", Op::ErrUnexpectedNextPfx},
	{"ModePrefixPass", Op::ModePrefixPass},
	{"ModeStrPass", Op::ModeStrPass},
	{"ModeComment", Op::ModeComment},
	{"ModeMultiLineComment", Op::ModeMultiLineComment},
	{"ModeString", Op::ModeString}
};

const char* GetOpName(Op op) {
	for (auto& opN : opNames)
		if (op == opN.op) return opN.name;
	return "OP NOT FOUND";
}

Op GetOpFromName(const char* name){
	for (auto& op : opNames)
		if (!strcmp(op.name, name)) return op.op;
	return Op::Error;
}

static Op fromPfxCh(char ch) {
	switch (ch) {
	case '@': return Op::Op;
	case '~': return Op::Comment;
	case '$': return Op::Name;
	case '%': return Op::VarType;
	case '&': return Op::Pointer;
	case '\"': return Op::String;
	case '\'': return Op::Char;
	case '=': return Op::Value;
	case '\n': return Op::LineEnd;
	}
	return Op::Unknown;
}

class Compiler {
	//Op expectNextPfx = Op::Null;
	Op pfx = Op::Null;
	Op strPayload = Op::Null;
	bool procOnNewL = true;
	char ch = '\0';
	std::stack<Op> opStack, modeStack;
	std::stack<Obj> funcStack, objStack; //functions, more later maybe
	std::vector<Op> allowedNextPfxs;
	std::string str;
	bool strAllowSpace = false;

public:
	Compiler()
	{
		push(Op::ModePrefixPass);
		pushObj({});
	}
	Obj& pushObj(Obj obj) {
		objStack.push(obj);
		return objStack.top();
	}
	void popObj() {
		objStack.pop();
	}
	void push(Op mode, bool strAllowSpace = false){
		this->strAllowSpace = strAllowSpace;
		modeStack.push(mode);
		printf("push: to %s\n", GetOpName(modeStack.top()));
	}
	void pop() {
		modeStack.pop();
		printf("pop: to %s\n", GetOpName(modeStack.top()));
	}
	void pushFunc(Obj obj) {
		funcStack.push(obj);
	}
	void popFunc() {
		funcStack.pop();
	}
	void setAllowedNextPfxs(std::vector<Op> allowedNextPfxs) {
		this->allowedNextPfxs = allowedNextPfxs;
	}
	bool isPfxExpected(Op pfx) {
		for (auto& p : allowedNextPfxs) 
			if (p == pfx) return true;
		return false;
	}
	//NO NEWLINES AT END OF STR
	void ExplainErr(Op code) {
		//auto& expectNextPfx = nextPfxStack.top();
		switch (code) {
		case Op::ErrUnexpectedNextPfx:
			printf("Unexpected next prefix. Allowed:");
			for (auto& p : allowedNextPfxs)
				printf("%s,", GetOpName(p));
			break;
		case Op::ErrExpectedVariablePfx:
			printf("Expected a variable type to be next.");
			break;
		default:
			printf("Unknown error");
		}		
	}
	void Err(Op code, const char* msg) {
		printf("Compiler died: \"%s\" OP: \"%s\"(%d) Error: ", msg, GetOpName(code), (int)code);
		ExplainErr(code);
		printf("\n");
		exit(-1);
	}
	void Char(char ch){
		this->ch = ch;
		switch (modeStack.top()){
			case Op::ModePrefixPass: return Prefix();
			case Op::ModeStrPass: return Str();
		}
	}
	void Prefix(){
		auto& obj = objStack.top();
		//auto& expectNextPfx = nextPfxStack.top();
		pfx = fromPfxCh(ch);
		
		if (pfx != Op::Unknown 
			&& !allowedNextPfxs.empty()
			&& !isPfxExpected(pfx))
			Err(Op::ErrUnexpectedNextPfx, "");
		printf("Got pfx %s\n", GetOpName(pfx));
		switch (pfx) {
		case Op::VarType:
		case Op::Op:
		case Op::Name:
			push(Op::ModeStrPass);
			break;
		case Op::Comment:
			break;
		}
	}
	void Str(){
		switch (ch) {
		case '\t':
		case '\r':
		case '\0':
			return;
		case '\n':
		case ' ':
			if (strAllowSpace) break;
			else {
				StrPayload();
				return;
			}
		}
		str.append(1, ch);
	}
	void StrPayload(){
		auto& obj = objStack.top();
		//auto& expectNextPfx = nextPfxStack.top();
		auto cs = str.c_str();
		Op nameOp = GetOpFromName(cs);
		printf("Str: %s\n", cs);
		switch (pfx)
		{
		case Op::Value:

			break;
		case Op::VarType:
			switch (obj.type) {
			case Op::FuncHasName:
				obj = pushObj({});
				obj.type = Op::FuncArgComplete;
				setAllowedNextPfxs({Op::Name});
				break;
			}
			break;
		case Op::Name:
			//just dont use fallthru here...
			switch (obj.type){
			case Op::Func:
				obj.type = Op::FuncHasName;
				setAllowedNextPfxs({Op::VarType});
				obj.setName(cs);
				break;
			case Op::FuncArgNameless:
				obj.type = Op::FuncArgComplete;
				setAllowedNextPfxs({ Op::VarType });
				obj.setName(cs);
				popObj();
				break;
			case Op::VarType:
				obj.setName(cs);
				break;
			}
			break;
		case Op::Op:
			switch (nameOp)
			{
			case Op::Done:
				switch (obj.type) {
				case Op::Func:
					break;
				}
				break;
			case Op::Func:
				obj.type = nameOp;
				setAllowedNextPfxs({Op::Name});
				break;
			case Op::Null:
				obj.type = Op::VarType;
				obj.val.op = nameOp;
				break;
			case Op::Public:
			case Op::Private:
				obj.privacy = nameOp;
				break;
			}
		}
		str.clear();
		printf("Str payload complete\n");
		pop();
	}
};

int main(int argc, char** argv) {
	//declar sys libs here using Compiler::ReadChar
	FILE* f;
	const char* fname = /*argv[1]*/"main.txt";
	if (!fopen_s(&f, fname, "r")){
		Compiler c;
		while(!feof(f)) c.Char(fgetc(f));
		fclose(f);
		return 0;
	}
	else{
		printf("Error\n");
	}
	return 1;
}
