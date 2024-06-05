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
#define OP_NAME_LEN 10

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
	FuncPre1, //have name
	FuncPre2, //have name and args
	FuncArg,
	FuncArgsEnd,
	LineEnd,
	Op,
	Done,//      @
	Return,//    ret
	NoChange,
	Struct,
	Variable,
	Comment,
	Public,
	Private,
	Imaginary, //for compiler features (extern)
	Void,

	Set,//                 set
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

	//compiler modes
	ModePrefixPass,
	ModeStrPass,
	ModeStrPayload,
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

struct Obj {
	Op type = Op::Null;
	Op privacy = Op::Public;
	char* name = nullptr;
	void setName(const char* name) {
		if (this->name) {
			free(this->name);
			this->name = nullptr;
		}
		this->name = _strdup(name);
	}
	Op op;
	Val val = {};
	std::vector<Obj>* children = nullptr;
};

struct OpNamePair {
	char name[OP_NAME_LEN];
	Op op;
};

struct Path
{
	Op op;
	int len;
	int (*func)(std::vector<Op>*);
	Op paths[10];
};

OpNamePair opNames[] = {
	{"null", Op::Null},
	{"no", Op::False},
	{"set", Op::Set},
	{"yes", Op::True},
	{"func", Op::Func},
	{"~", Op::Comment},
	{"%", Op::Variable},
	{"@", Op::Done},
	{"ret", Op::Return},
	{"if", Op::If},
	{"else", Op::Else},
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
};

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
	case '%': return Op::Variable;
	case '&': return Op::Pointer;
	case '\"': return Op::String;
	case '\'': return Op::Char;
	}
	return Op::Unknown;
}

class Compiler {
	Op expectNextPfx = Op::Null;
	Op pfx = Op::Null;
	Op strPayload = Op::Null;
	bool procOnNewL = true;
	char ch = '\0';
	std::stack<Op> opStack;
	std::stack<Op> modeStack;
	std::stack<Obj> objStack;
	std::string str;
	bool strAllowSpace = false;

public:
	Compiler()
	{
		modeStack.push(Op::ModePrefixPass);
		objStack.push({});
	}

	void push(Op mode, bool strAllowSpace = false){
		this->strAllowSpace = strAllowSpace;
		modeStack.push(mode);
	}
	void pop() {
		modeStack.pop();
	}
	//NO NEWLINES AT END OF STR
	void ExplainErr(Op code) {
		switch (code) {
		case Op::ErrUnexpectedNextPfx:
			printf("Unexpected next prefix. Expected OP %d", (int)expectNextPfx);
			break;
		default:
			printf("Unknown error");
		}		
	}
	void Err(Op code, const char* msg) {
		printf("%s OP: %d Error: ", msg, (int)code);
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
		pfx = fromPfxCh(ch);
		if (pfx != Op::Unknown 
			&& expectNextPfx != Op::Null 
			&& pfx != expectNextPfx)
				Err(Op::ErrUnexpectedNextPfx, "");
		switch (pfx) {
		case Op::Variable:
			if (expectNextPfx == pfx) {

			}
		case Op::Op:
		case Op::Name:
			/*if (expectNextPfx == pfx) {
				switch (obj.op) {
				case Op::Func:

					break;
				}
			}*/
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
		auto cs = str.c_str();
		printf("Str: %s\n", cs);
		switch (pfx)
		{
		case Op::Name:
			switch (obj.op){
			case Op::Func:
				obj.op = Op::FuncPre1;
			case Op::Variable:
				obj.setName(cs);
				//pop();
				break;
			}
			break;
		case Op::Op:
			Op nameOp = GetOpFromName(cs);
			switch (nameOp)
			{
			case Op::Func:
				obj.type = nameOp;
				obj.op = nameOp;
				expectNextPfx = Op::Name;
				//pop();
				break;
			case Op::Null:
				obj.type = Op::Variable;
				obj.op = nameOp;
				//pop();
				break;
			case Op::Public:
			case Op::Private:
				obj.privacy = nameOp;
				break;
			}
		}
		str.clear();
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