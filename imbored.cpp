#include <stdio.h>
#include <stdlib.h>
#include <vector>
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
@use system
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

enum class Pfx {
	Name = '$',
	Type = '@',
	Pointer = '&',
	Variable = '%',
};

//multiple uses
enum class Op {
	Null = 0,
	False = 0,
	True = 1,
	Done,
	Return,
	If,
	Else,
	//operators
	Cast,
	Add,
	Imaginary, //for compiler features
	Subtract,
	Multiply,
	Divide,
	Equals,
	NotEquals,
	LessThan,
	GreaterThan,
	LessThanOrEquals,
	GreaterThanOrEquals,
	ScopeOpen, ScopeClose,
	ParenthesisOpen, ParenthesisClose,
	BracketOpen, BracketClose,
	DataType,
	Comma,
	Name, //func name, var name
	String,
	Char,
	For,
	While,
	Block,//block of variables and code
	Struct,
	Private, Public, Void,
	c8, //char
	u8, u16, u32, u64, //unsigned
	i8, i16, i32, i64, //signed
	f32, d64, //float, double

	CompilerFlags,

	Error,
	ErrNOT_GOOD,

	//Mode
	ModePrefixPass,
	ModeNamePass,
	ModeComment,
	ModeMultiLineComment,
	ModeString
};

struct Val {
	union {
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
	};
};
struct Obj {
	Op type;
	char name[OBJ_NAME_LEN];
	//union {
		Op retType;
		Val val;
		Op opCode;
	//};
};

struct State {
};

struct Path {
	Op type;
	Op path[5];
};

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
		case '\n':

		}
		switch ((Op)ch)
		{

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
	if (!fopen_s(&f, argv[1], "r"))
	{
		Compiler c;
		//c.ReadChar()
		fclose(f);
		return 0;
	}
	else
	{
		printf("Error\n");
	}
	return 1;
}