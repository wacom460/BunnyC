#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
//not actually a compiler
//ascii only, maybe utf8 later...
//c transpiler for now
//no order of operations, sequential ONLY

//can compile only ONE file for simplicity
//compiler options inside source code, preferably using code

//Example code:
/*
@struct $Entity
	%c8 name
	%f32 x y

@done

@func $FuncDoThing i32 var1 i64 var2 &&void funcPtr @returns i32

@done 0

@&c8 $AddTwoStrs &c8 str1 &c8 str2
	%c8 ret = str1 + str2
@done %ret
*/

#define OBJ_NAME_LEN 64

//multiple uses
enum class Type {
	Null,
	Done,
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

	Error,
	ErrNOT_GOOD,

	Mode,
	ModePrefixPass,
	ModeCharPath,

	//keep at bottom
	Prefix,
	PfxName = '$',
	//PfxStruct = '#'
	PfxType = '@',
	PfxPointer = '&',
	PfxVariable = '%'
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
	Type type;
	char name[OBJ_NAME_LEN];
	//union {
		Type retType;
		Val val;
		Type opCode;
	//};
};

struct State {
};

struct Path {
	std::vector<Type> path;
};

class Compiler {
	bool parenthesisOpen, 
		 bracketOpen = false,
		 scopeOpen = false;
	Type type, lastType,
		 privacy = Type::Public,
		 prefix, mode = Type::ModePrefixPass,
		 op = Type::Null, lastOp = Type::Null;
	Obj obj; // working obj mem, add to objs when done

	std::vector<Obj> objs;
public:
	void ReadChar(char ch)
	{
		//prefix pass

		switch ((Type)ch)
		{
			case Type::Null:
				break;
			case Type::ErrNOT_GOOD:
				exit((int)op);
			case Type::PfxType:
				
				break;
		}
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