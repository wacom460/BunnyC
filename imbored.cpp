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
#define IB_DEBUG_EXTRA1 1
#define COMMENT_CHAR ('~')

//multiple uses
enum class Op {
	Null,False,True,Unknown,NotSet,Any,Use,Build,Space,

	Func,
	__FuncBuildingStart__,
	FuncHasName, //have name
	FuncNeedName,
	FuncNeedsRetValType,FuncArgsVarNeedsName,FuncArgNameless,
	FuncArgComplete,
	FuncNeedVarsAndCode,
	FuncSignatureComplete,
	FuncNeedReturnValue,
	CompletedFunction,
	__FuncBuildingEnd__,

	__VariableBuildingStart__,
	VarNeedName,//have var type need name
	VarComplete, //have type and name
	__VariableBuildingEnd__,

	/*LineEnd,*/Op,Value,Done,Return,NoChange,Struct,VarType,
	Comment,MultiLineComment,Public,Private,Imaginary,Void,/*StackFloorObj,*/
	Set,SetAdd,Colon,Dot,Add,Subtract,Multiply,Divide,
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
	ModeString,

	__DO_NOT_INSERT_AFTER_THIS__,
	__DO__NOT__DO__IT__,
	__CustomOpsStart__,
	__CustomTypesStart__ = __DO__NOT__DO__IT__ + 100,
	__CustomTypesEnd__ = __CustomTypesStart__ - 1
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
	{"imaginary", Op::Imaginary},
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
	{"ModeString", Op::ModeString},
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
	{"ErrUnknownOpStr",Op::ErrUnknownOpStr}
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
struct FuncObj {
	Val returnValue = {};
	Op retType = Op::NotSet;
	Op retTypeMod = Op::NotSet;
};
struct ArgObj {
	Op type = Op::Null;
	Op mod = Op::NotSet;
};
struct Obj {
private:
	Op type = Op::NotSet;
	Op modifier = Op::NotSet;
public:
	const Op getType() { return type; }
	void setType(Op type) {
		printf("obj type: %s(%d) -> %s(%d)\n", GetOpName(this->type), (int)this->type, GetOpName(type), (int)type);
		this->type = type;
	}
	Op getMod() { return modifier; }
	void setMod(Op mod) { 
		printf("obj mod: %s(%d) -> %s(%d)\n", GetOpName(this->modifier), (int)this->modifier, GetOpName(mod), (int)mod);
		modifier = mod; 
	}
	Op privacy = Op::NoChange;
	char* name = nullptr;
	char* str = nullptr;
	union {
		FuncObj func;
		ArgObj arg = {};
	};
	Val val = {};
	void setName(const char* name) { 
		printf("obj name: %s -> %s\n", this->name, name);
		owStr(&this->name, name); 
	}
	void setStr(const char* Str) { 
		printf("obj str: %s -> %s\n", this->str, Str);
		owStr(&this->str, Str); 
	}
	void print() {
		unsigned __int64 u64 = val.u64;
		printf("[Type:%s(%d),Name:%s,Str:%s,Mod:%s,Val:%I64u]\n",
			GetOpName(type), (int)type, name, str, GetOpName(modifier), u64);
	}
};
class Compiler {
	Op pfx = Op::Null;
	Op pointer = Op::NotSet;
	Op nameOp = Op::Null;
	char ch = '\0';
	std::stack<bool> opAllowedStack, strReadPtrsStack;
	std::stack<Op> modeStack, taskStack;
	std::stack<Obj> objStack;
	std::stack<std::vector<Op>> allowedNextPfxsStack;
	std::vector<Obj> workingObjs;
	std::string str;
	bool strAllowSpace = false;
	unsigned int line = 0;
	unsigned int column = 0;
	Op commentMode = Op::NotSet;
	int multiLineOffCount = 0;
public:
	Compiler(){
		opAllowedStack.push(true);
		strReadPtrsStack.push(false);
		allowedNextPfxsStack.push({});
		push(Op::ModePrefixPass);
		pushObj({});
	}
	~Compiler() {
		if (!str.empty()) StrPayload();
		printf("-> Compilation complete <-\n");
	}
	void pushTask(Op task) {
		taskStack.push(task);
	}
	void popTask() {
		taskStack.pop();
	}
	Obj& pushObj(Obj obj) {
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
	Obj& popObj(bool pushToWorking) {
		if (pushToWorking)
		{
			assert(objStack.top().getType() != Op::NotSet);
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
	void pushAllowedNextPfxs(std::vector<Op> allowedNextPfxs) {
		//allowedNextPfxsStack.push(allowedNextPfxs); //soon(TM)
		allowedNextPfxsStack.top() = allowedNextPfxs;
		if (!allowedNextPfxsStack.top().empty()) {
			printf("set allowed next pfxs to");
			for (auto& p : allowedNextPfxsStack.top()) printf(", %s", GetPfxName(p));
			printf("\n");
		}else printf("push allowed next pfxs to EMPTY\n");
	}
	void popAllowedNextPfxs() {
		allowedNextPfxsStack.pop();
	}
	bool isPfxExpected(Op pfx) {
		if (allowedNextPfxsStack.top().empty()) return true;
		if (pfx == Op::Op) return opAllowedStack.top();
		for (auto& p : allowedNextPfxsStack.top()) if (p == pfx) return true;
		return false;
	}
	//NO NEWLINES AT END OF STR
	void ExplainErr(Op code) {
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
			printf("Unexpected next prefix %s. Allowed:", GetPfxName(pfx));
			for (auto& p : allowedNextPfxsStack.top())
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
	void Err(Op code, const char* msg="") {
		printf("ERR:%s At %u:%u \"%s\"(%d)\nExplanation: ", msg, line, column, GetOpName(code), (int)code);
		ExplainErr(code);
		printf("\n");
		exit(-1);
	}
	void Char(char ch){
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
					Err(Op::FuncArgsVarNeedsName);
					break;
				}
				case Op::FuncNeedReturnValue: {
					Err(Op::FuncNeedReturnValue);
					break;
				}
				case Op::FuncSignatureComplete:
				case Op::FuncHasName:
				case Op::Func:
					popObj(true);
					taskStack.top() = Op::FuncNeedVarsAndCode;
					break;
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
	void PopAndDoTask()	{
		assert(!taskStack.empty());
		switch (taskStack.top()) {
		case Op::FuncNeedVarsAndCode: {

			break;
		}
		//case Op::FuncHasName:
		case Op::Func: {
			std::string cFuncModsTypeName, cFuncArgs, cFuncCode;
			bool imaginary = false;
			for (int i = 0; i < workingObjs.size(); ++i) {
				auto& o = workingObjs[i];
				switch (o.getType()) {
				case Op::FuncArgComplete: {//multiple allowed
					auto at = o.arg.type;
					assert(at != Op::Null);
					if(!cFuncArgs.empty())cFuncArgs += ", ";
					cFuncArgs += GetCEqu(o.arg.type);
					cFuncArgs += GetCEqu(o.arg.mod);
					cFuncArgs += " ";
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
					auto mod = o.getMod();
					if (mod != Op::NotSet) {
						cFuncModsTypeName += GetCEqu(mod);
						cFuncModsTypeName += " ";
					}
					cFuncModsTypeName += GetCEqu(o.func.retType);
					cFuncModsTypeName += GetCEqu(o.func.retTypeMod);
					cFuncModsTypeName += " ";
					assert(o.name != nullptr);
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
				cFuncCode += "}";
			}
			printf("%s\n", std::string(cFuncModsTypeName+cFuncArgs+cFuncCode).c_str());
			break;
		}
		}
		popTask();
		workingObjs.clear();
	}
	void Prefix(){
		pfx = fromPfxCh(ch);
		//if (pfx == Op::LineEnd) return;
		auto& obj = objStack.top();
		if (pfx != Op::Unknown 
			&& !allowedNextPfxsStack.top().empty()
			&& !isPfxExpected(pfx))
			Err(Op::ErrUnexpectedNextPfx, "");
		printf("PFX:%s\n", GetPfxName(pfx));
		switch (pfx) {
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
	void Str(){
		switch (ch) {
		case '\t': return;
		case ' ': {
			if (strAllowSpace) break;
			else return StrPayload();
		}
		case '&': {
			if (strReadPtrsStack.top()) {
				switch (pointer) {
				case Op::NotSet:
					printf("got pointer\n");
					pointer = Op::Pointer;
					break;
				case Op::Pointer:
					printf("got double pointer\n");
					pointer = Op::DoublePointer;
					break;
				case Op::DoublePointer:
					printf("got tripple pointer\n");
					pointer = Op::TripplePointer;
					break;
				case Op::TripplePointer:
					Err(Op::ErrQuadriplePointersNOT_ALLOWED);
					break;
				}
				return;
			}
		}
		}
		str.append(1, ch);
	}
	void StrPayload(){
		printf("Doing Str payload\n");
		auto cs = str.c_str();
		Val strVal = {};
		strVal.i64=strtoll(cs, nullptr, 10);
		nameOp = GetOpFromName(cs);
		printf("Str: %s\n", cs);
		switch (pfx)
		{
		case Op::Value: //=
			if (!taskStack.empty()) {
				switch (taskStack.top()) {
				case Op::Func:
					for (auto& obj : workingObjs) {
						if (obj.getType() == Op::FuncNeedReturnValue) {
							printf("Finishing func got ret value\n");
							obj.func.returnValue = strVal;
							obj.setType(Op::CompletedFunction);
							PopAndDoTask();
							popObj(true);
							break;
						}
					}
					break;
				}
			}			
			break;
		case Op::VarType: //%
			switch (objStack.top().getType()) {
			case Op::NotSet:
				objStack.top().setType(Op::VarNeedName);
				break;
			case Op::FuncNeedsRetValType: {
				opAllowedStack.pop();
				//pushAllowedNextPfxs({});

				objStack.top().func.retType = nameOp;
				objStack.top().func.retTypeMod = pointer;
				objStack.top().setType(Op::FuncSignatureComplete);
				popObj(true);
				assert(taskStack.top() == Op::FuncHasName);
				taskStack.top() = Op::Func;
				break;
			}
			case Op::FuncHasName:
				pushObj({});
				objStack.top().setType(Op::FuncArgNameless);
				objStack.top().arg.type= nameOp;
				objStack.top().arg.mod = pointer;
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
			switch (nameOp) {
			case Op::Imaginary:
				objStack.top().setMod(nameOp);
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
						//TODO: could cache func obj later but idk how
						if (obj.getType() == Op::FuncSignatureComplete) {
							if (obj.func.retType != Op::Void) {
								pushAllowedNextPfxs({Op::Value});
							}else {
								//setAllowedNextPfxs({});
								popAllowedNextPfxs();
							}
							obj.setType(Op::FuncNeedReturnValue);
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
					Err(Op::ErrUnexpectedOp);
					break;
				}
				break;
			}
			case Op::Func:
				assert(objStack.top().getType() == Op::NotSet);
				objStack.top().setType(nameOp);
				objStack.top().func.retType = Op::Void;
				objStack.top().func.retTypeMod = Op::NotSet;
				pushAllowedNextPfxs({Op::Name});
				pushTask(Op::FuncNeedName);
				break;
			case Op::Public:
			case Op::Private:
				objStack.top().privacy = nameOp;
				break;
			default:
				Err(Op::ErrUnknownOpStr);
			}
		}
		str.clear();
		printf("Str payload complete\n");
		pop();
		if(strReadPtrsStack.size() > 1)
		{
			if (strReadPtrsStack.top()) pointer = Op::NotSet;
			strReadPtrsStack.pop();
		}
	}
};
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