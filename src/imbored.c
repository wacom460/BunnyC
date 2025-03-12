#include "imbored.h"

#ifdef _WIN32
#define atoll _atoi64
#endif

#ifdef IBDEBUGPRINTS
void _PrintLine(char* f, int l)
{
	if (f) {
		char* rf = f;
		for(size_t i = strlen(f); i >= 0; i--) {
			char c = f[i];
			if (c == '\\' || c == '/') break;
			rf = f + i;
		}
		f = rf;
	}
	else f = "";
	IBPushColor(IBFgRED);
	printf("[");
	IBPushColor(IBBgGREEN);
	printf("%s:%d", f, l);
	IBPushColor(IBFgRED);
	printf("]");
	IBPopColor();
	IBPopColor();
	IBPopColor();
}
#endif

#define X(a) {#a, OP_##a},
IBOpNamePair opNamesAR[] = {
	_IB_OPS_
};
#undef X
/*#define OP(op) {#op, OP_##op},
#undef OP*/
IBOpNamePair PairNameOps[] = {
	{"null", OP_Null},{IBFALSESTR, OP_False},{IB_TRUESTR, OP_True},
	{"blk", OP_Func},{"~", OP_Comment},{"%", OP_VarType},
	{"return", OP_Return},{"ext", OP_Imaginary},
	{"if", OP_If},{"else", OP_Else},{"use", OP_Use},
	{"build", OP_Build},{"space", OP_Space},{"priv", OP_Private},
	{"eq", OP_Equals},{"neq", OP_NotEquals},{"lt", OP_LessThan},
	{"gt", OP_GreaterThan},{"lteq", OP_LessThanOrEquals},
	{"gteq", OP_GreaterThanOrEquals},{",", OP_Comma},
	{"for", OP_For},{"loop", OP_Loop},{"i64", OP_i64},{"f32", OP_f32},
	{"d64", OP_d64},{"pub", OP_Public},{"void", OP_Void},{"c8", OP_c8},
	{"u8", OP_u8},{"u16", OP_u16},{"u32", OP_u32},{"u64", OP_u64},
	{"i8", OP_i8},{"i16", OP_i16},{"bool", OP_Bool},
	{"i32", OP_i32},{"use",OP_Use},{"sys", OP_UseStrSysLib},
	{"struct", OP_Struct},{"repr", OP_Repr},{"elif", OP_ElseIf},
	{"", OP_EmptyStr},{"table", OP_Table},{"-", OP_Subtract},
	{"case", OP_Case},{"fall", OP_Fall},{"break", OP_Break},
	{"as", OP_As},{">", OP_GreaterThan},{"output", OP_Output},
	{"enum", OP_Enum},{"flags", OP_Flags},{"nts", OP_String},
	{"arguments",OP_RunArguments},{"include", OP_CInclude},
	{"!", OP_Exclaim},{"methods", OP_Methods},{"=", OP_Equals}
};
IBOpNamePair PairDataTypeOPs[] = {
	{"i8", OP_i8},{"i16", OP_i16},{"i32", OP_i32},{"i64", OP_i64},
	{"u8", OP_u8},{"u16", OP_u16},{"u32", OP_u32},{"u64", OP_u64},
	{"f32", OP_f32},{"d64", OP_d64},{"bool", OP_Bool},{"c8", OP_c8},
	{"nts", OP_String},{"void", OP_Void},
};
IBOpNamePair pfxNames[] = {
	{"NULL", OP_Null},{"Value(=)", OP_Value},{"Op(@)", OP_Op},
	{"Comment(~)", OP_Comment},{"Name($)", OP_Name},
	{"VarType(%)", OP_VarType},{"Pointer(&)", OP_Pointer},
	{"Return(@ret)", OP_Return},{"OP_Unknown", OP_Unknown},
	{"String(\")", OP_String},{"LineEnd(\\n)", OP_LineEnd},
	{"Tab character", OP_TabChar},{"Exclaim(!)", OP_Exclaim},
	{"Caret(^)", OP_Caret},{"Underscore(_)", OP_Underscore},
	{"ParenthesisOpen( ( )", OP_ParenthesisOpen},
	{"ParenthesisClose( ) )", OP_ParenthesisClose},
	{"CurlyBraceOpen( { )", OP_CurlyBraceOpen},
	{"CurlyBraceClose( } )", OP_CurlyBraceClose},
	{"BracketOpen( [ )", OP_BracketOpen},
	{"BracketClose( ] )", OP_BracketClose},
	{"Dot(.)", OP_Dot},{"LessThan(<)", OP_LessThan},{"Equals(=)", OP_Equals},
	{"GreaterThan(>)", OP_GreaterThan},
	{"Comma(,)", OP_Comma}, {"Subtract(-)", OP_Subtract},
	{"Add(+)", OP_Add},{"Divide(/)", OP_Divide},
	{"Multiply(*)", OP_Multiply},{"PfxlessValue(=)", OP_PfxlessValue},
	{"Letter_azAZ", OP_Letter_azAZ},{"SingleQuote(\')", OP_SingleQuote},
	{"Or(|)",OP_Or},
};
IBOpNamePair cEquivelents[] = {
	{"void", OP_Void},{"return", OP_Return},
	{"int", OP_i32},{"unsigned int", OP_u32},{"char", OP_Bool},
	{"long long", OP_i64},{"unsigned long long", OP_u64},
	{"short", OP_i16},{"char", OP_i8},{"char", OP_c8},
	{"unsigned short", OP_u16},{"unsigned char", OP_u8},
	{"*", OP_Pointer},{"**", OP_DoublePointer},
	{"***", OP_TripplePointer},{", ", OP_CommaSpace},
	{"(", OP_ParenthesisOpen},{")", OP_ParenthesisClose},
	{"{", OP_CurlyBraceOpen},{"}", OP_CurlyBraceClose},
	{"", OP_NotSet},
	{"extern", OP_Imaginary},{"float", OP_f32},{"double", OP_d64},
	{"==", OP_Equals},{"!=", OP_NotEquals},{"<", OP_LessThan},
	{">", OP_GreaterThan},{"<=", OP_LessThanOrEquals},
	{">=", OP_GreaterThanOrEquals},{"!=", OP_NotEquals},
	{"+", OP_Add},{"-", OP_Subtract},{"*", OP_Multiply},
	{"+=", OP_AddEq},{"-=", OP_SubEq},{"*=", OP_MultEq},
	{"/=", OP_DivEq},{"/", OP_Divide},{"%", OP_Modulo},
	{"*", OP_Deref},{"&", OP_Ref},{"**", OP_DoubleDeref},
	{"***", OP_TrippleDeref},{"char*", OP_String},{"|",OP_Or},
};
IBOpNamePair dbgAssertsNP[] = {
	{"taskType", OP_TaskType},
	{"taskStack", OP_TaskStack},
	{"notEmpty", OP_NotEmpty}
};

char* SysLibCodeStr =
"space $sys" LE
"pub" LE
"ext blk $malloc i32 $size -> void^" LE
"ext blk $realloc void^ $ptr i32 $newSize -> void^" LE
"ext blk $free void^ $ptr" LE
"ext blk $strdup c8^ $str -> c8^" LE
"ext blk $strcat c8^ $str1 c8^ $str2 -> c8^" LE
;

CLAMP_FUNC(int, ClampInt) { CLAMP_IMP }
CLAMP_FUNC(long long int, ClampSizeT) { CLAMP_IMP }

IBVector g_ColorStack;

char* IBGetCEqu(IBOp op)
{
	int sz;
	int i;
	IBassert(op != OP_Unknown);
	if (op == OP_None)return "";
	sz = sizeof(cEquivelents) / sizeof(cEquivelents[0]);
	for (i = 0; i < sz; i++) {
		if (op == cEquivelents[i].op) return cEquivelents[i].name;
	}
	return "[GetCEqu UNKNOWN!!!!]";
}

char* IBGetOpName(IBOp op)
{
	int sz;
	int i;
	sz=sizeof(opNamesAR) / sizeof(opNamesAR[0]);
	for (i = 0; i < sz; i++) {
		if (op == opNamesAR[i].op) return opNamesAR[i].name;
	}
	IBassert(0);
	return "?";
}

char* IBGetPfxName(IBOp op)
{
	int sz;
	int i;
	sz=sizeof(pfxNames) / sizeof(pfxNames[0]);
	for (i = 0; i < sz; i++) {
		if (op == pfxNames[i].op) return pfxNames[i].name;
	}
	//IBassert(0);
	return "?";
}

IBOp IBGetOpFromNameList(char* name, IBOp list)
{
#define IBListM(_OP, _PAIRS) case _OP: { \
	int sz; \
	int i; \
	sz = sizeof(_PAIRS) / sizeof(_PAIRS[0]); \
	for (i = 0; i < sz; i++) \
		if (!strcmp(_PAIRS[i].name, name)) return _PAIRS[i].op; \
	break; \
}
	switch (list) {
		IBListM(OP_Op, opNamesAR)
		IBListM(OP_DataTypes, PairDataTypeOPs)
		IBListM(OP_NameOps, PairNameOps)
		IBListM(OP_dbgAssert, dbgAssertsNP)
	}
	return OP_Unknown;
#undef IBListM
}

IBOp IBOPFromPfxCh(char ch)
{
	switch (ch) {
	IBCASE_aTHRUz
	IBCASE_ATHRUZ return OP_Letter_azAZ;
	case IBCOMMENT_CHAR: return OP_Comment;
	case '|': return OP_Or;
	case '\t': return OP_TabChar;
	case ' ': return OP_SpaceChar;
	case '@': return OP_Op;
	case '$': return OP_Name;
	case '%': return OP_VarType;
	case '\"': return OP_String;
	case '\'': return OP_SingleQuote;
	IBCASE_0THRU9 return OP_PfxlessValue;
	case '=': return OP_Value;
	case '&': return OP_Ref;
	case '+': return OP_Add;
	case '-': return OP_Subtract;
	case '/': return OP_Divide;
	case '*': return OP_Multiply;
	case '<': return OP_LessThan;
	case '>': return OP_GreaterThan;
	case '!': return OP_Exclaim;
	case '^': return OP_Caret; /* "code portals" */
	case '_': return OP_Underscore;
	case '(': return OP_ParenthesisOpen;
	case ')': return OP_ParenthesisClose;
	case '{': return OP_CurlyBraceOpen;
	case '}': return OP_CurlyBraceClose;
	case '[': return OP_BracketOpen;
	case ']': return OP_BracketClose;
	case ',': return OP_Comma;
	case '.': return OP_Dot;
	default: return OP_Unknown;
	}
}

void IBPushColor(IBColor col)
{
	IBVectorCopyPushIBColor(&g_ColorStack, col);
	IBSetColor(col);
}

void IBPopColor()
{
	IBColor* col;
	_IBVectorPop(&g_ColorStack, NULL);
	//IBassert(g_ColorStack.elemCount);
	col = (IBColor*)IBVectorTop(&g_ColorStack);
	if (col) IBSetColor(*col);
	else IBSetColor(IBFgWHITE);
}

void IBSetColor(IBColor col)
{
#ifdef _WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), col);
#endif
}
