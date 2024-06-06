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

#define OBJ_NAME_LEN 64
#define OP_NAME_LEN 32

//multiple uses
enum class Op {
	Null,False,True,Unknown,NotSet,Any,Use,Build,Space,

	Func,
	__FuncBuildingStart__,
	FuncHasName, //have name
	FuncNeedsRetValType,FuncArgsVarNeedsName,FuncArgNameless,
	FuncArgComplete,
	//FuncArgsEnd,
	FuncNeedVarsAndCode,
	__FuncBuildingEnd__,
	FuncNameAndArgsAndRetComplete,

	LineEnd,Op,Value,Done,Return,NoChange,Struct,VarType,
	Comment,Public,Private,Imaginary,Void,/*StackFloorObj,*/
	Set,SetAdd,Colon,Dot,Add,Subtract,Multiply,Divide,
	AddEq,SubEq,MultEq,DivEq,Equals,NotEquals,LessThan,
	GreaterThan,LessThanOrEquals,GreaterThanOrEquals,
	ScopeOpen, ScopeClose,
	ParenthesisOpen, ParenthesisClose,
	BracketOpen, BracketClose,

	Comma,Name,String,Char,If,Else,For,While,Block,

	c8, //char
	u8, u16, u32, u64, //unsigned
	i8, i16, i32, i64, //signed
	f32, d64, //float, double

	Pointer,DoublePointer,CompilerFlags,

	Error,
	ErrNOT_GOOD,
	ErrUnexpectedNextPfx,
	ErrExpectedVariablePfx,

	//compiler modes
	ModePrefixPass,ModeStrPass,ModeComment,ModeMultiLineComment,
	ModeString
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
	{"&", Op::Pointer},
	{"ErrUnexpectedNextPfx", Op::ErrUnexpectedNextPfx},
	{"ModePrefixPass", Op::ModePrefixPass},
	{"ModeStrPass", Op::ModeStrPass},
	{"ModeComment", Op::ModeComment},
	{"ModeMultiLineComment", Op::ModeMultiLineComment},
	{"ModeString", Op::ModeString},
	{"FuncHasName", Op::FuncHasName},
	{"NotSet", Op::NotSet},
	//{"StackFloorObj", Op::StackFloorObj},
	{"Return", Op::Return},
	{"FuncArgComplete",Op::FuncArgComplete},
	{"FuncNeedsRetValType",Op::FuncNeedsRetValType},
};
OpNamePair pfxNames[] = {
	{"Op (@)", Op::Op},
	{"Comment (~)", Op::Comment},
	{"Name($)", Op::Name},
	{"VarType (%)", Op::VarType},
	{"Pointer (&)", Op::Pointer},
	{"LineEnd (\\n)", Op::LineEnd},
	{"Return (@ret)", Op::Return},
};
const char* GetOpName(Op op) {
	for (auto& opN : opNames)
		if (op == opN.op) return opN.name;
	return "OP NOT FOUND";
}
const char* GetPfxName(Op op) {
	for (auto& opN : pfxNames)
		if (op == opN.op) return opN.name;
	return "PFX NOT FOUND";
}
Op GetOpFromName(const char* name) {
	for (auto& op : opNames)
		if (!strcmp(op.name, name)) return op.op;
	return Op::Error;
}
static Op fromPfxCh(char ch) {
	/*if (ch == '\n') {
		printf("hi\n");
	}*/
	switch (ch) {
	case '\n': {
		printf("hi\n");
		return Op::LineEnd;
	}
	case '@': return Op::Op;
	case '~': return Op::Comment;
	case '$': return Op::Name;
	case '%': return Op::VarType;
	case '&': return Op::Pointer;
	case '\"': return Op::String;
	case '\'': return Op::Char;
	case '=': return Op::Value;
	}
	return Op::Unknown;
}
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
void owStr(char** str, const char* with) {
	if (*str) free(*str);
	*str = _strdup(with);
}
struct Obj {
	Op type = Op::NotSet;
	Op modifier = Op::NotSet;
	Op privacy = Op::NotSet;
	char* name = nullptr;
	char* str = nullptr;
	union {
		Op op;
		Op argType;
	};
	Val val = {};
	/*std::vector<Obj>* children = nullptr;*/	
	void setName(const char* name) { owStr(&this->name, name); }
	void setStr(const char* Str) { owStr(&this->str, str); }
	void print() {
		unsigned __int64 u64 = val.u64;
		printf("OBJ[Type: %s(%d), Name: %s, Str: %s OP: %s(%d) Val as i64:%I64u]\n",
			GetOpName(type), (int)type, name, str, GetOpName(op), (int)op, u64);
	}
};
class Compiler {
	Op pfx = Op::Null;
	Op strPayload = Op::Null;
	bool procOnNewL = true;
	//bool opPfxAllowed = true;
	char ch = '\0';
	std::stack<bool> opAllowedStack;
	std::stack<Op> modeStack;
	std::stack<Obj> objStack;
	std::vector<Op> allowedNextPfxs;
	std::vector<Obj> workingObjs;
	std::string str;
	bool strAllowSpace = false;
	//bool returnChar = false;
	unsigned int line = 0;
	unsigned int column = 0;
public:
	Compiler(){
		opAllowedStack.push(true);
		push(Op::ModePrefixPass);
		pushObj({});
	}
	Obj& pushObj(Obj obj) {
		if (!objStack.empty()) {
			printf("Obj bef push:");
			objStack.top().print();
		}
		objStack.push(obj);
		printf("Obj aft push:");
		objStack.top().print();
		return objStack.top();
	}
	Obj& popObj(bool pushToWorking) {
		if (pushToWorking) workingObjs.push_back(objStack.top());
		printf("Obj before pop:");
		objStack.top().print();
		//assert(objStack.top().type != Op::StackFloorObj);
		objStack.pop();
		if (objStack.empty()) pushObj({});
		printf("Obj after pop:");
		objStack.top().print();
		return objStack.top();
	}
	void push(Op mode, bool strAllowSpace = false){
		this->strAllowSpace = strAllowSpace;
		modeStack.push(mode);
		printf("push: to %s\n", GetOpName(modeStack.top()));
	}
	Op pop() {
		modeStack.pop();
		printf("pop: to %s\n", GetOpName(modeStack.top()));
		return modeStack.top();
	}
	void setAllowedNextPfxs(std::vector<Op> allowedNextPfxs) {
		this->allowedNextPfxs = allowedNextPfxs;
		printf("set allowed next pfxs to: ");
		for (auto& p : this->allowedNextPfxs) printf("%s,", GetPfxName(p));
		printf("\n");
	}
	bool isPfxExpected(Op pfx) {
		//if (pfx == Op::Any)return true;
		if (allowedNextPfxs.empty()) return true;
		if (pfx == Op::Op) return opAllowedStack.top();
		for (auto& p : allowedNextPfxs) if (p == pfx) return true;
		return false;
	}
	//NO NEWLINES AT END OF STR
	void ExplainErr(Op code) {
		//auto& expectNextPfx = nextPfxStack.top();
		switch (code) {
		case Op::ErrUnexpectedNextPfx:
			printf("Unexpected next prefix %s. Allowed:", GetPfxName(pfx));
			for (auto& p : allowedNextPfxs)
				printf("%s,", GetPfxName(p));
			break;
		case Op::ErrExpectedVariablePfx:
			printf("Expected a variable type to be next.");
			break;
		default:
			printf("Unknown error");
		}
		printf("\nOBJ MEM:");
		objStack.top().print();
	}
	void Err(Op code, const char* msg) {
		printf("Compiler died: \"%s\" At %u:%u OP: \"%s\"(%d) Error: ", msg, line, column, GetOpName(code), (int)code);
		ExplainErr(code);
		printf("\n");
		exit(-1);
	}
	void Char(char ch){
		column++;
		this->ch = ch;
		//printf("-> %c\n", this->ch);
		auto m = modeStack.top();
		bool nl = false;
		switch (this->ch) {
			case '\0': return;
			case '\n': {
				nl = true;
				//printf("got newl\n");
				/*if (m != Op::ModePrefixPass)
				{
					m = pop();
				}*/
				break;
			}
		}
		switch (m){
			case Op::ModePrefixPass:
				Prefix();
				break;
			case Op::ModeStrPass:
				Str();
				break;
		}
		if (nl) {
			column = 0;
			line++;
		}
	}
	void Prefix(){
		auto& obj = objStack.top();
		pfx = fromPfxCh(ch);		
		if (pfx != Op::Unknown 
			&& !allowedNextPfxs.empty()
			&& !isPfxExpected(pfx))
			Err(Op::ErrUnexpectedNextPfx, "");
		printf("Got pfx %s\n", GetPfxName(pfx));
		switch (pfx) {
		case Op::LineEnd:
			setAllowedNextPfxs({});
			break;
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
			return;
		case ' ':
			if (strAllowSpace) break;
			else {
				StrPayload();
				return;
			}
		case '&':
			if (pfx == Op::VarType) {
				//printf("ptr");
			}
		}
		str.append(1, ch);
	}
	void StrPayload(){
		auto cs = str.c_str();
		Op nameOp = GetOpFromName(cs);
		printf("Str: %s\n", cs);
		switch (pfx)
		{
		case Op::Value:
			break;
		case Op::VarType:
			switch (objStack.top().type) {
			case Op::FuncNeedsRetValType: {
				opAllowedStack.pop();
				setAllowedNextPfxs({ Op::LineEnd });//Op::Op is implicit allowing @ret next
				objStack.top().type = Op::FuncNameAndArgsAndRetComplete;
				popObj(true);
				break;
			}
			case Op::FuncHasName:
				pushObj({});
				objStack.top().type = Op::FuncArgNameless;
				setAllowedNextPfxs({Op::Name});
				break;
			}
			break;
		case Op::Name://just dont use fallthru here...
			switch (objStack.top().type){
			case Op::Func:
				objStack.top().type = Op::FuncHasName;
				setAllowedNextPfxs({Op::VarType, Op::LineEnd });
				objStack.top().setName(cs);
				break;
			case Op::FuncArgNameless:
				objStack.top().type = Op::FuncArgComplete;
				setAllowedNextPfxs({ Op::VarType, Op::LineEnd });
				objStack.top().setName(cs);
				popObj(true);
				break;
			case Op::VarType:
				objStack.top().setName(cs);
				break;
			}
			switch (objStack.top().type){
			case Op::FuncArgComplete:
				
				break;
			}
			break;
		case Op::Op: 
			switch (nameOp) {
			case Op::LineEnd:
				printf("LINEEND\n");
				break;
			case Op::Done:
				switch (objStack.top().type) {
				case Op::Func:
					break;
				}
				break;
			case Op::Return: {
				auto& t = objStack.top().type;
				switch (t) {
				case Op::FuncHasName:
					opAllowedStack.push(false);
					objStack.top().type = Op::FuncNeedsRetValType;
					setAllowedNextPfxs({ Op::VarType });
					break;
				}
				break;
			}
			case Op::Func:
				objStack.top().type = nameOp;
				setAllowedNextPfxs({Op::Name});
				break;
			case Op::Null:
				objStack.top().type = Op::VarType;
				objStack.top().val.op = nameOp;
				break;
			case Op::Public:
			case Op::Private:
				objStack.top().privacy = nameOp;
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
		while (!feof(f)) {
			//c.Char(fgetc(f));
			char ch;
			fread(&ch, 1, 1, f);
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