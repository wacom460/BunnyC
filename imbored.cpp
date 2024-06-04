#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>
#include <string>

//not actually a compiler
//ascii only, maybe utf8 later...
//c transpiler for now
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
	
	Use,//       use
	Build,//     build
	Space,//     space
	Func,//      func
	Done,//      @
	Return,//    ret

	Struct,
	Variable,
	Comment,
	Public,
	Private,
	Imaginary, //for compiler features (extern)
	Void,

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
	ModeNamePass, //name of op
	ModeComment,
	ModeMultiLineComment,
	ModeString
};

typedef union Val {
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
	union {
		char* name;
		Op op;
		Val val = {};
	};
	std::vector<Obj>* children = nullptr;
};

struct OpNamePair {
	char name[OP_NAME_LEN];
	Op op;
};

OpNamePair opNames[] = {
	{"null", Op::Null},
	{"no", Op::False},
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
	{"=", Op::Equals},
	{"!+", Op::NotEquals},
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
	{"", Op::Block},
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

Op GetOpFromName(char* name){
	for (auto& op : opNames)
		if (strcmp(op.name, name) == 0)
			return op.op;
	return Op::Error;
}

class Compiler {
	Op mode = Op::ModePrefixPass,
		 op = Op::Null, lastOp = Op::Null;
	Obj obj = {}; // working obj mem, add to objs when done
	char ch = '\0';
	std::vector<Obj> objs;
public:
	void Char(char ch){
		this->ch = ch;
		switch (ch)
		{
		//case '\n':
		default:break;
		}
		switch ((Pfx)ch)
		{
			case Pfx::Op: mode = Op::ModeNamePass; break;
			case Pfx::Name: mode = Op::ModeNamePass; break;
			case Pfx::Pointer: mode = Op::ModeNamePass; break;
			case Pfx::Variable: mode = Op::ModeNamePass; break;
		}
		switch (mode){
			case Op::ModePrefixPass: return Prefix();
			case Op::ModeNamePass: return Name();
		}
	}
	void Prefix(){
		
	}
	void Name(){
		
	}
};

int main(int argc, char** argv) {
	//declar sys libs here using Compiler::ReadChar
	FILE* f;
	if (!fopen_s(&f, argv[1], "r")){
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