#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>
#include <string>

//not actually a compiler
//ascii only, maybe utf8 later...
//transpile to C
//no order of operations, sequential ONLY

//compiler options inside source code, preferably using code

//Example code:
//note that the tabbing does not matter.
//one line is one statement
/*
~single line comment

~~multi
line
comment~~

~~
hi
~~multi 
line comment
inception~~
hi
~~

@space clib ~C Library binding
@imaginary @func
$malloc u32 size @returns &void~~whats up~~
$free &void ptr
$realloc &void ptr u32 size @returns &void
@@

@space sys
@public~public sets all future code to public if on its own line like this
@func

@@

//------------------
@use sys
@space game

@build
	@lib SDL2
@@

@space @global @func
$GlobalFunc @returns &c8
@@ "Hello World"
@@

@space %c8
	@func
		$len

	@@
@@

@struct~define multiple structs
	$Entity
		%c8 name
		%f32 x,y

		@func
		$setName &c8 name
			%len = @call name.len
		@@

		$move f32 x,y 
			@comment deadzone
			@if x geq 0
			
			@@
		@@
	@@

	@extends Entity
	$Actor

	@@
	@@

@@

@func $Main i32 argCount &c8[] args @ret i32
	@if argCount neq 0 @ret 1
@@ 0

@func $FuncDoThing i32 var1 i64 var2 &&void funcPtr @ret i32
	
@@ 0

@func $AddTwoStrs &c8 str1 &c8 str2 @returns &c8
	%c8 ret = str1 + str2
@@ %ret
*/

#define OBJ_NAME_LEN 64
#define OP_NAME_LEN 10

enum class Pfx {
	Null = 0,
	Name = '$',
	Op = '@',
	Dot = '.',
	Pointer = '&',
	Variable = '%',
	String = '\"',
	Char = '\'',
	Comment = '~',
};

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
	Done,//      @
	Return,//    ret

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

//Obj HandleFunc(std::vector<Op>* ops) {
//	Obj ret;
//	for(auto& op : *ops) {
//		switch (op) {
//		case Op::FuncArg:
//		case Op::FuncArgsEnd:
//			break;
//		}
//	}
//	return ret;
//}
//
//Path path[] = {
//	{Op::Func, 2, NULL, {Op::FuncArg, Op::FuncArgsEnd}},
//	{Op::Func, 3, NULL, {Op::FuncArg, Op::FuncArg, Op::FuncArgsEnd}},
//	{Op::Func, 4, NULL, {Op::FuncArg, Op::FuncArg, Op::FuncArg, Op::FuncArgsEnd}},
//	{Op::Func, 5, NULL, {Op::FuncArg, Op::FuncArg, Op::FuncArg, Op::FuncArg, Op::FuncArgsEnd}},
//	{Op::Func, 6, NULL, {Op::FuncArg, Op::FuncArg, Op::FuncArg, Op::FuncArg, Op::FuncArg, Op::FuncArgsEnd}},
//};

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
	{"<", Op::LessThan},
	{">", Op::GreaterThan},
	{"<=", Op::LessThanOrEquals},
	{">=", Op::GreaterThanOrEquals},
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
		if (strcmp(op.name, name) == 0)
			return op.op;
	return Op::Error;
}

class Compiler {
	Op mode = Op::ModePrefixPass, lastMode = Op::ModePrefixPass,
		op = Op::Null, lastOp = Op::Null;
	std::vector<Op> validNextOps;
	Pfx expectNextPfx = Pfx::Null;
		Op strPayload = Op::Null;
		bool procOnNewL = true;
	Obj obj = {}; // working obj mem, add to objs when done
	char ch = '\0';
	std::vector<Obj> objs;
	std::vector<Op> opStack;
	std::string str;
	bool strAllowSpace = false;
public:
	void set(Op mode, bool strAllowSpace = false){
		this->lastMode = this->mode;
		this->mode = mode;
		this->strAllowSpace = strAllowSpace;
	}
	void Char(char ch){
		this->ch = ch;
		switch (mode){
			case Op::ModePrefixPass: return Prefix();
			case Op::ModeStrPass: return Str();
			case Op::ModeStrPayload: return StrPayload();
		}
	}
	void Prefix(){
		switch (expectNextPfx) {
		case Pfx::Null:
			break;
		case Pfx::Name:
			switch (obj.op)
			{
			case Op::Func:
				break;
			}
			break;
		case Pfx::Op:
			break;
		}
		auto pfx = (Pfx)ch;
		switch (pfx) {
		case Pfx::Variable:
			
		case Pfx::Op:
		case Pfx::Name:
			//strPayload = obj.op;
			set(Op::ModeStrPass);
			break;
		case Pfx::Comment:

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
		switch (obj.op)
		{
		case Op::Variable:
		case Op::Name:
			switch (obj.op)
			{
				case Op::Func:
					obj.setName(str.c_str());
					obj.op = Op::FuncPre1;
					validNextOps.clear();
					validNextOps.push_back(Op::FuncArg);
					validNextOps.push_back(Op::FuncArgsEnd);
					break;
			}
			Op nameOp = GetOpFromName(str.c_str());
			switch (nameOp)
			{
			/*case Op::Name:
				if (obj.name)
				{
					free(obj.name);
					obj.name = nullptr;
				}
				obj.name = strdup(str.c_str());
				break;*/
			case Op::Func:
				obj.op = nameOp;
				expectNextPfx = Pfx::Name;
				mode = Op::ModePrefixPass;
				break;
			}
			//objs.push_back(obj);
			break;
		}
		str.clear();
		mode = Op::ModePrefixPass;
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