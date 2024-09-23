#include "imbored.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "ib_ide.h"
#include "ibcolor.h"

#define assert(x) IBASSERT(x, "")

#ifdef _WIN32
//#include <Windows.h>//included in ibcolor.h
#define atoll _atoi64
#endif

#ifdef IBDEBUGPRINTS
void _PrintLine(int l) {
	IBPushColor(IBFgRED);
	printf("[");
	IBPushColor(IBBgGREEN);
	printf("%d", l);
	IBPushColor(IBFgRED);
	printf("]");
	IBPopColor();
	IBPopColor();
	IBPopColor();
}
#endif

IBDatabase* g_DB;
char* IBLayer3StringModeIgnoreChars = "";
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
	{"!", OP_Exclaim},{"methods", OP_Methods},
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
	{"Dot(.)", OP_Dot},{"LessThan(<)", OP_LessThan},
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
"space $sys\n"
"pub\n"
"ext blk $malloc i32 $size -> void^\n"
"ext blk $realloc void^ $ptr i32 $newSize -> void^\n"
"ext blk $free void^ $ptr\n"
"ext blk $strdup c8^ $str -> c8^\n"
"ext blk $strcat c8^ $str1 c8^ $str2 -> c8^\n"
;
CLAMP_FUNC(int, ClampInt) { CLAMP_IMP }
CLAMP_FUNC(long long int, ClampSizeT) { CLAMP_IMP }
void IBStrInit(IBStr* str) {
	IBASSERT0(str);
	str->start = (char*)malloc(1);
	IBASSERT0(str->start);
	str->end = str->start;
	if(str->start) (*str->start) = '\0';
}
void IBStrInitWithCStr(IBStr* str, char* cstr){
	IBStrInit(str);
	IBStrAppendCStr(str, cstr);
}
void IBStrInitExt(IBStr* str, char* cstr){
	str->start = cstr;
	str->end = cstr + strlen(cstr);
}
void IBStrFree(IBStr* str){
	free(str->start);
}
void IBStrClear(IBStr* str){
	if(str->start) free(str->start);
	str->start = NULL;
	str->start = malloc(1);
	IBASSERT0(str->start);
	if (str->start) {
		*(str->start) = '\0';
		str->end = str->start;
	}
}
void IBStrReplaceWithCStr(IBStr* str, char* cstr){
	IBStrClear(str);
	IBStrAppendCStr(str, cstr);
}
void IBStrInitNTStr(IBStr* str, char* nullTerminated){
	IBASSERT0(nullTerminated);
	IBASSERT0(str);
	IBOverwriteStr(&str->start, nullTerminated);
	str->end = str->start + strlen(nullTerminated);
}
bool IBStrContainsAnyOfChars(IBStr* str, char* chars) {
	char* p;
	for(p=str->start; p<str->end; p++)
		if(strchr(chars, *p)) return true;
	return false;
}
long long int IBStrLen(IBStr* str) {
	size_t len;
	IBASSERT0(str);
	IBASSERT0(str->end);
	IBASSERT0((*(str->end)) == '\0');
	IBASSERT0(str->end >= str->start);
	len=str->end - str->start;
	return len;
}
void IBStrAppendCh(IBStr* str, char ch, int count){
	char astr[2];
	if (count < 1) return;
	IBASSERT0(str);
	astr[0] = ch;
	astr[1] = '\0';
	while(count--)
		IBStrAppendCStr(str, astr);
}
char* IBStrAppendCStr(IBStr* str, char *with) {
	void* ra;
	size_t len;
	size_t withLen;
	IBASSERT0(str);
	withLen = strlen(with);
	if(!withLen) return str->start;
	IBASSERT0(withLen > 0);
	IBASSERT0(str->start);
	len = IBStrLen(str);
	IBREALLOC(ra, str->start, len + withLen + 1);
	IBASSERT0(ra);
	if (ra) {
		str->start = (char*)ra;
		memcpy(str->start + len, with, withLen);
		*(str->start + len + withLen) = '\0';
		str->end = str->start + len + withLen;
		return str->start;
	}
	return NULL;
}
void IBStrAppendFmt(IBStr* str, char* fmt, ...) {
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);
	IBStrAppendCStr(str, buf);
}
char* IBStrAppend(IBStr* str, IBStr* with){
	void* ra;
	size_t len;
	size_t withLen;
	IBASSERT0(str);
	withLen = IBStrLen(with);
	if (!withLen) return str->start;
	IBASSERT0(withLen > 0);
	IBASSERT0(str->start);
	len = IBStrLen(str);
	IBREALLOC(ra, str->start, len + withLen + 1);
	IBASSERT0(ra);
	if (ra) {
		str->start = (char*)ra;
		memcpy(str->start + len, with->start, withLen);
		*(str->start + len + withLen) = '\0';
		str->end = str->start + len + withLen;
		return str->start;
	}
	return NULL;
}
int IBStrStripFront(IBStr* str, char ch){
	int slen=IBStrLen(str);
	int in = 0;
	char* rep = NULL;
	char ch2 = '\0';
	while(true) {
		ch2 = str->start[in];
		if (ch2 == ch) in++;
		else break;
	}
	if(!in) return 0;
	rep=strdup(str->start+in);
	free(str->start);
	str->start=rep;
	str->end=str->start+(slen - in);
	IBASSERT0(str->end);
	if (str->end) {
		char ec =
			*(str->end);
		IBASSERT0(ec == '\0');
	}
	return in;
}
void IBVectorInit(IBVector* vec, int elemSize, IBOp type, int count) {
	void* m;
	IBASSERT0(count>0);
	memset(vec,0,sizeof*vec);
	vec->initMagic=IBMAGIC;
	vec->elemSize = elemSize;
	vec->type = type;
	vec->elemCount = 0;
	vec->initialSlotCount=count;
	vec->doNotShrink=count > 1;
	vec->slotCount = count;
	vec->dataSize = vec->elemSize * vec->slotCount;
	vec->data = NULL;
	m=malloc(vec->dataSize);
	IBASSERT0(m);
	vec->data = m;
	IBASSERT0(vec->data);
	memset(vec->data, 0, vec->dataSize);
}
struct IBVecData* IBVectorGet(IBVector* vec, int idx) {
	IBASSERT0(vec);
	IB_ASSERTMAGICP(vec);
	IBASSERT0(vec->elemCount >= 0);
	IBASSERT0(vec->slotCount >= 0);
	IBASSERT0(idx>=0);
	IBASSERT0(idx < vec->elemCount);
	if (vec->elemCount < 1
		|| idx >= vec->elemCount) return NULL;
	return (struct IBVecData*)((char*)vec->data + vec->elemSize * idx);
}
void* _IBVectorIterNext(IBVector* vec, int* idx, int lineNum) {
	//DbgFmt("[%d]"__FUNCTION__,lineNum);
	IBASSERT0(idx);
	IBASSERT0(vec);
	IB_ASSERTMAGICP(vec);
	IBASSERT0((*idx) >= 0);
	IBASSERT0(vec->elemCount <= vec->slotCount);
	IBASSERT0(vec->elemCount + vec->slotCount + vec->dataSize >= 0);
	//DbgFmt("(%p,%d(%p),%d)\n", vec, *idx, idx, lineNum);
	if (!vec || !idx) return NULL;
	if ((*idx) >= vec->elemCount) return NULL;
	return (void*)((char*)vec->data + (vec->elemSize * ((*idx)++)));
}
void _IBVectorPush(IBVector* vec, struct IBVecData** dataDP IBDBGFILELINEPARAMS) {
	struct IBVecData* topPtr;
	IBASSERT0(vec);
	IB_ASSERTMAGICP(vec);
	IBASSERT0(vec->elemSize);
	IBASSERT0(vec->type)
	if (vec->elemCount + 1 > vec->slotCount) {
		void*ra=0;
		vec->slotCount++;
		vec->dataSize = vec->elemSize * vec->slotCount;
		//DbgFmt("vec->dataSize: %d\n", vec->dataSize);
		IBASSERT0(vec->data);
		IBREALLOC(ra,vec->data, vec->dataSize);
		IBASSERT0(ra);
		if(ra) vec->data = ra;
		vec->reallocCount++;
	}
	topPtr = (struct IBVecData*)((char*)vec->data + (vec->elemSize * vec->elemCount));
	memset(topPtr, 0, vec->elemSize);
	int pil = ClampInt(vec->elemCount, 0, IBVEC_PUSHINFO_MAX - 1);
	vec->PushInfo[pil].lineNum=ln;
	vec->PushInfo[pil].filePath=file;
	_IBVectorReinitPushInfo(vec);
	vec->elemCount++;
	if(dataDP) *dataDP = topPtr;
}
void _IBVectorCopyPush(IBVector* vec, void* elem IBDBGFILELINEPARAMS) {
	IBASSERT0(vec);
	IB_ASSERTMAGICP(vec);
	struct IBVecData* top;
	_IBVectorPush(vec, &top IBDBGFPL2);//FIX
	memcpy(top, elem, vec->elemSize);
}
void _IBVectorCopyPushBool(IBVector* vec, bool val IBDBGFILELINEPARAMS) {
	_IBVectorCopyPush(vec, &val IBDBGFPL2);
}
void _IBVectorCopyPushOp(IBVector* vec, IBOp val IBDBGFILELINEPARAMS) {
	_IBVectorCopyPush(vec, &val IBDBGFPL2);
}
void _IBVectorCopyPushIBColor(IBVector* vec, IBColor col IBDBGFILELINEPARAMS){
	_IBVectorCopyPush(vec, &col IBDBGFPL2);
}
struct IBVecData* IBVectorTop(IBVector* vec) {
	IBASSERT0(vec);
	IB_ASSERTMAGICP(vec);
	IBASSERT0(vec->elemCount >= 0);
	IBASSERT0(vec->slotCount >= 0);
	if (vec->elemCount <= 0) return NULL;
	return IBVectorGet(vec, vec->elemCount - 1);
}
struct IBVecData* IBVectorFront(IBVector* vec) {
	IBASSERT0(vec);
	if (vec->elemCount <= 0) return NULL;
	IBASSERT0(vec->data);
	return vec->data;
}
void _IBVectorPop(IBVector* vec, void(*freeFunc)(void*)){
	void* ra;
	IBASSERT0(vec);
	if(vec->elemCount <= 0) return;
	if(freeFunc) freeFunc((void*)IBVectorGet(vec, vec->elemCount - 1));
	vec->elemCount--;
	if(!vec->doNotShrink) vec->slotCount=vec->elemCount;
	if(vec->slotCount<1)vec->slotCount=1;
	vec->dataSize = vec->elemSize * vec->slotCount;
	if(!vec->doNotShrink && vec->elemCount){
		IBASSERT0(vec->data);
		IBREALLOC(ra, vec->data, vec->dataSize);
		IBASSERT0(ra);
		if (ra) vec->data = ra;
		IBASSERT0(vec->data);
	}
	_IBVectorReinitPushInfo(vec);
}
void _IBVectorPopFront(IBVector* vec, void(*freeFunc)(void*)){
	size_t newSize;
	void *ra;
	if(vec->elemCount < 1) return;
	vec->elemCount--;
	if(!vec->doNotShrink) vec->slotCount = vec->elemCount;
	if (vec->slotCount < 1)vec->slotCount = 1;
	if(vec->elemCount > 1){
		size_t rns = ((vec->dataSize * vec->elemCount) - vec->dataSize);
		newSize = vec->doNotShrink ? (vec->dataSize)
			: rns;
		IBASSERT0(newSize >= vec->dataSize);
		ra = malloc(newSize);
		IBASSERT0(ra);
		if (ra) {
			memcpy(ra, IBVectorGet(vec, 1), rns);
			free(vec->data);
			vec->data = ra;
		}
	}
	_IBVectorReinitPushInfo(vec);
}
void IBVectorFreeSimple(IBVector* vec) {
	free(vec->data);
}
void _IBVectorReinitPushInfo(IBVector* vec){
	/*IBASSERT0(vec->slotCount<=IBVEC_PUSHINFO_MAX);*/
	for(int i=0;i<IBVEC_PUSHINFO_MAX;i++){
		vec->PushInfo[i].ptr=
			i<vec->elemCount?IBVectorGet(vec,i):0;
	}
}
void IBDictKeyInit(IBDictKey* key, IBDictKeyDef def) {
	key->type = def.type;
	switch (def.type) {
	case IBDictDataType_Int: {
		key->key.num = def.num;
		break;
	}
	case IBDictDataType_String: {
		strncpy(key->key.data, def.str, IBDICTKEY_KEYSIZE);
		break;
	}
	}
	IBVectorInit(&key->children, sizeof(IBDictKey), OP_IBDictKey, IBVEC_DEFAULT_SLOTCOUNT);
}
void IBDictKeyInitRoot(IBDictKey* key){
	memset(key, 0, sizeof(IBDictKey));
	key->type = IBDictDataType_RootKey;
	IBVectorInit(&key->children, sizeof(IBDictKey), OP_IBDictKey, IBVEC_DEFAULT_SLOTCOUNT);
}
void IBDictKeyFree(IBDictKey* key) {
	int idx = 0;
	IBDictKey* sk = NULL;
	assert(key);
	while (sk = IBVectorIterNext(&key->children, &idx))
		IBDictKeyFree(sk);
	IBVectorFreeSimple(&key->children);
}
IBDictKey* IBDictKeyFind(IBDictKey* rootKey, IBVector* keyDefStack) {
	IBDictKeyDef* dp = NULL;
	IBDictKey* key;
	int idx = 0;
	assert(rootKey);
	key = rootKey;
	assert(keyDefStack->elemCount);
	while (dp = IBVectorIterNext(keyDefStack, &idx)) {
		IBDictKey* ok = IBDictKeyFindChild(key, *dp);
		if (ok) {
			key = ok;
			break;
		}
		else {
			IBDictKey* nk = NULL;
			IBVectorPush(&key->children, &nk);
			IBDictKeyInit(nk, *dp);
			key = nk;
			break;
		}
	}
	if (key == rootKey) key = NULL;
	return key;
}
IBDictKey* IBDictKeyFindChild(IBDictKey* key, IBDictKeyDef def){
	int idx = 0;
	IBDictKey* sk = NULL;
	assert(key);
	while (sk = IBVectorIterNext(&key->children, &idx)) {
		if (sk->type == def.type) {
			switch (def.type) {
			case IBDictDataType_Int: {
				if (sk->key.num == def.num) return sk;
				break;
			}
			case IBDictDataType_String: {
				if (strcmp(sk->key.data, def.str) == 0) return sk;
				break;
			}
			IBCASE_UNIMP_A
			}
		}
	}
	return NULL;
}
void IBDictKeyPrint(IBDictKey* key, int* childDepth){
	int idx = 0;
	int tc=0;
	IBDictKey* sk = NULL;
	assert(key);
	assert(childDepth);
	tc = *childDepth;
	while(tc--) printf("\t");
	printf("[%d] ", *childDepth);
	switch (key->type) {
	case IBDictDataType_RootKey: {
		printf("Root ");
		break;
	}
	case IBDictDataType_VoidPtr: {
		printf("Pointer: %p", key->val.data);
		break;
	}
	case IBDictDataType_Int: {
		printf("Int: %d", key->val.num);
		break;
	}
	case IBDictDataType_String: {
		printf("Str: %s", key->val.data);
		break;
	}
	}
	printf(" K:\n");
	++*childDepth;
	while (sk = IBVectorIterNext(&key->children, &idx)) {
		IBDictKeyPrint(sk, childDepth);
	}
	--*childDepth;
}
//void IBDictionaryInit(IBDictionary* dict){
//	IBDictKeyInit(&dict->rootKey, (IBDictKeyDef)
//	{
//		.type = IBDictDataType_RootKey,
//			.key = NULL,
//			.num = 0
//	});
//}
//void IBDictionaryFree(IBDictionary* dict){
//	IBDictKeyFree(&dict->rootKey);
//}
//IBDictKey* IBDictFind(IBDictionary* dict, IBVector* keyStack){
//	return IBDictKeyFind(&dict->rootKey, keyStack);
//}
typedef enum {
	IBDictManipAction_Unknown = 0,
	IBDictManipAction_DataIn,
	IBDictManipAction_DataOut,
	IBDictManipAction_StrIn,
	IBDictManipAction_StrOut,
	IBDictManipAction_IntIn,
	IBDictManipAction_IntOut,
	IBDictManipAction_KeyPtrOut,
} IBDictManipAction;
IBDictKey* IBDictManip(IBDictKey* rootKey, char* fmt, ...){
	va_list args;
	int i=0;
	IBDictKey* dk = NULL;
	IBVector keyStack;//IBDictKeyDef
	void* inPtr=NULL;
	void** outPtr=NULL;
	char* inStr = NULL, **outStr=NULL;
	int inInt=0;
	int* outIntPtr=NULL;
	IBDictKey** outKeyPtr=NULL;
	size_t count=0;
	IBDictManipAction action = IBDictManipAction_Unknown;
	IBVectorInit(&keyStack, sizeof(IBDictKeyDef), OP_IBDictKeyDef, IBVEC_DEFAULT_SLOTCOUNT);
	va_start(args, fmt);
	for (i = 0; i < strlen(fmt); i++) {
		char ch = fmt[i];
		switch (ch) {
		case 's': {//string
			IBDictKeyDef* kd=NULL;
			IBVectorPush(&keyStack, &kd);
			kd->type = IBDictDataType_String;
			kd->key = NULL;
			kd->str = va_arg(args, char*);
			break;
		}
		case 'd': {//int
			IBDictKeyDef* kd = NULL;
			IBVectorPush(&keyStack, &kd);
			kd->type = IBDictDataType_Int;
			kd->key = NULL;
			kd->num = va_arg(args, int);
			break;
		}
		case 'i': {//in ptr
			assert(action == IBDictManipAction_Unknown);
			inPtr = va_arg(args, void*);
			action = IBDictManipAction_DataOut;
			break;
		}
		case 'o': {//out ptr
			assert(action == IBDictManipAction_Unknown);
			outPtr = va_arg(args, void**);
			action = IBDictManipAction_DataIn;
			break;
		}
		case 'c': {//count
			count = va_arg(args, size_t);
			break;
		}
		case 'z': {//in char* (null terminated)
			assert(action == IBDictManipAction_Unknown);
			inStr = va_arg(args, char*);
			action = IBDictManipAction_StrIn;
			break;
		}
		case 'x': {//in int
			assert(action == IBDictManipAction_Unknown);
			inInt = va_arg(args, int);
			action = IBDictManipAction_IntIn;
			break;
		}
		case 'j': {//out new char* (null terminated)
			assert(action == IBDictManipAction_Unknown);
			outStr = va_arg(args, char**);
			action = IBDictManipAction_StrOut;
			break;
		}
		case 'k': {//out int*
			assert(action == IBDictManipAction_Unknown);
			outIntPtr = va_arg(args, int*);
			action = IBDictManipAction_IntOut;
			break;
		}
		case 'g': {//out IBDictKey*
			assert(action == IBDictManipAction_Unknown);
			outKeyPtr = va_arg(args, IBDictKey**);
			action = IBDictManipAction_KeyPtrOut;
			break;
		}
		IBCASE_UNIMP_A
		}
	}
	dk = IBDictKeyFind(rootKey, &keyStack);
	assert(dk);
	switch (action) {
	case IBDictManipAction_DataIn: {
		assert(count > 0 && count <= IBDICTKEY_MAXDATASIZE);
		memcpy_s(dk->val.data, IBDICTKEY_MAXDATASIZE, inPtr, count);
		break;
	}
	case IBDictManipAction_DataOut: {
		assert(outPtr);
		assert(count > 0 && count <= IBDICTKEY_MAXDATASIZE);
		if(outPtr)
			memcpy(outPtr, dk->val.data, count);
		break;
	}
	case IBDictManipAction_StrIn: {
		strncpy(dk->val.data, inStr, IBDICTKEY_MAXDATASIZE);
		break;
	}
	case IBDictManipAction_StrOut: {
		*outStr = strdup(dk->val.data);
		break;
	}
	case IBDictManipAction_IntIn: {
		dk->val.num = inInt;
		break;
	}
	case IBDictManipAction_IntOut: {
		(*outIntPtr) = dk->val.num;
		break;
	}
	case IBDictManipAction_KeyPtrOut: {
		*outKeyPtr = dk;
		break;
	}
	IBCASE_UNIMP_A
	}
	IBVectorFreeSimple(&keyStack);
	va_end(args);
	return dk;
}
IBDictKey* IBDictGet(IBDictKey* rootKey, char* keyPath){
	IBVector keyStack;
	IBVectorInit(&keyStack, sizeof(IBDictKeyDef), OP_IBDictKeyDef, IBVEC_DEFAULT_SLOTCOUNT);
	IBDictKeyDef*kd;
	IBVectorPush(&keyStack, &kd);
	IBDictKey* dk = IBDictKeyFind(rootKey, &keyStack);
	return NULL;
}
void IBDictTest() {
	{
		IBDictKey rk;
		IBDictKey* out = NULL;
		int oi = 100;
		IBDictKeyInitRoot(&rk);
		IBDictKey* k1p1 = IBDictManip(&rk, "dsdx", 5, "id", 0, 1);
		IBDictKey* k1p2 = IBDictManip(&rk, "dsdk", 5, "id", 0, &oi);
		IBASSERT0(k1p1 && k1p2);
		IBASSERT0(k1p1 == k1p2);
		IBASSERT0(oi == 1);
		IBDictManip(&rk, "dsdg", 5, "id", 0, &out);
		IBASSERT0(out);
		IBASSERT0(out->val.num == 1);
	}

	{
		IBDictKey rk;
		IBDictKey* key = NULL;
		IBDictKeyInitRoot(&rk);
		IBDictManip(&rk, "sssx", "variables", "globals", "color", 10);
		IBDictManip(&rk, "sssg", "variables", "globals", "color", &key);
		assert(key);
		assert(key->val.num == 10);
	}

	{
		IBDictKey rk;
		IBDictKeyInitRoot(&rk);
		//Does: folder.file.100.info.date = 19910420
		IBDictKey* k = IBDictManip(&rk,
			IBDStr    IBDStr  IBDNum IBDStr  IBDStr  IBDInNum,
			"folder", "file", 100, "info", "date", 19910420
		);
		int cd = 0;
		//IBDictKeyPrint(&dict.rootKey, &cd);
	}
}
void IBPushColor(IBColor col) {
	IBVectorCopyPushIBColor(&g_ColorStack, col);
	IBSetColor(col);
}
void IBPopColor() {
	IBColor* col;
	_IBVectorPop(&g_ColorStack, NULL);
	//assert(g_ColorStack.elemCount);
	col = (IBColor*)IBVectorTop(&g_ColorStack);
	if (col) IBSetColor(*col);
	else IBSetColor(IBFgWHITE);
}
char* StrConcat(char* dest, int count, char* src) {
	return strcat(dest, src);
}

char StrStartsWith(char* str, char* with) {
	while (*with)
		if (*str++ != *with++) return 0;
	return 1;
}

IBVector g_ColorStack;

void IBCodeBlockInit(IBCodeBlock* block){
	IBASSERT0(block);
	IB_SETMAGICP(block);
	IBStrInit(&block->header);
	IBStrInit(&block->variables);
	IBStrInit(&block->varsInit);
	IBStrInit(&block->codeLeft);
	IBStrInit(&block->code);
	IBStrInit(&block->codeRight);
	IBStrInit(&block->footer);
	IBNameInfoDBInit(&block->localVariables);
}
void IBCodeBlockFinish(IBCodeBlock* block, IBStr* output){
	IBASSERT0(block);
	IB_ASSERTMAGICP(block);
	IBStrAppendFmt(output,
		"%s%s%s%s%s%s%s",
		block->header.start,
		block->variables.start,
		block->varsInit.start,
		block->codeLeft.start,
		block->code.start,
		block->codeRight.start,
		block->footer.start);
}
void IBCodeBlockFree(IBCodeBlock* block){
	IBASSERT0(block);
	IB_ASSERTMAGICP(block);
	IBNameInfoDBFree(&block->localVariables);
	IBStrFree(&block->header);
	IBStrFree(&block->variables);
	IBStrFree(&block->varsInit);
	IBStrFree(&block->codeLeft);
	IBStrFree(&block->code);
	IBStrFree(&block->codeRight);
	IBStrFree(&block->footer);
}
void IBNameInfoInit(IBNameInfo* info){
	info->type=OP_NotSet;
	info->cast=OP_NotSet;
	info->name=NULL;
}
void IBNameInfoFree(IBNameInfo* info) {
	free(info->name);
}
void IBNameInfoDBInit(IBNameInfoDB* db) {
	IBVectorInit(&db->pairs, sizeof(IBNameInfo), OP_NameInfo, IBVEC_DEFAULT_SLOTCOUNT);
}
void IBNameInfoDBFree(IBNameInfoDB* db) {
	IBASSERT0(db);
	IBVectorFree(&db->pairs, IBNameInfoFree);
}
IBOp IBNameInfoDBAdd(struct IBLayer3* ibc, IBNameInfoDB* db, char* name, IBOp type, IBNameInfo** niDP) {
	IBNameInfo* info=NULL,*found=NULL;
	assert(ibc);
	IBASSERT0(db);
	IBASSERT0(name);
	assert(db != &((IBCodeBlock*)IBVectorFront(&ibc->CodeBlockStack))->localVariables);
	found = IBNameInfoDBFind(db, name);
	if (found) {
		if(niDP) (*niDP) = found;
		return OP_AlreadyExists;
	}
	IBVectorPush(&db->pairs, &info);
	assert(info);
	info->type = type;
	info->name = strdup(name);
	if (niDP) (*niDP) = info;
	return OP_OK;
}
IBOp IBNameInfoDBFindType(IBNameInfoDB* db, char* name) {
	IBNameInfo* pair;
	int idx;
	idx = 0;
	while (pair = IBVectorIterNext(&db->pairs, &idx)) {
		if (!strcmp(pair->name, name))
			return pair->type;
	}
	return OP_NotFound;
}
IBNameInfoDB* _IBNameInfoDBFind(IBNameInfoDB* db, char* name, int lineNum){
	IBNameInfo* pair=NULL;
	int idx=0;
	assert(db);
	//DbgFmt("[%d]"__FUNCTION__"(,%s)\n", lineNum,name);
	while (pair = IBVectorIterNext(&db->pairs, &idx))
		if (!strcmp(pair->name, name)) return pair;
	return NULL;
}
void ObjInit(IBObj* o) {
	memset(o,0,sizeof*o);
	o->type=OP_NotSet;
	o->modifier=OP_NotSet;
	o->privacy=OP_NoChange;
	o->name=NULL;
	o->str=NULL;
	o->val.i32 = 0;
	o->valType = OP_Unknown;
	o->func.retStr = NULL;
	o->arg.type = OP_Null;
	IBVectorInit(&o->arg.arrIndexExprs,sizeof(IBStr),OP_IBStr, IBVEC_DEFAULT_SLOTCOUNT);
	o->arg.mod = OP_NotSet;
	o->ifO.lvName=NULL;
	o->ifO.rvName = NULL;
}
void ObjFree(IBObj* o) {
	assert(o);
	IBVectorFree(&o->arg.arrIndexExprs, IBStrFree);
	if (o->ifO.lvName) free(o->ifO.lvName);
	if (o->ifO.rvName) free(o->ifO.rvName);
	if (o->forO.startName) free(o->forO.startName);
	if (o->func.retValStr) free(o->func.retValStr);
	if (o->func.retStr) free(o->func.retStr);
	if(o->var.valStrLiteral)
		free(o->var.valStrLiteral);
	if (o->name) free(o->name);
	if (o->str) free(o->str);
}
void _IBExpectsInit(int LINENUM, IBExpects* exp, char *fmt, ...) {
	va_list args;
	IBOp pfx;
	IBOp nameOp;
	int i;
	assert(exp);
	IBVectorInit(&exp->pfxs, sizeof(IBOp), OP_Op, IBVEC_DEFAULT_SLOTCOUNT);
	IBVectorInit(&exp->nameOps, sizeof(IBOp), OP_Op, IBVEC_DEFAULT_SLOTCOUNT);
	exp->pfxErr="";
	exp->nameOpErr="";
	exp->life=0;
	exp->lineNumInited=LINENUM;
	//DbgFmt("Expect: { ","");
	va_start(args, fmt);
	for (i = 0; i < strlen(fmt); i++) {
		char ch = fmt[i];
		switch (ch) {
		case '0': {
			exp->life=va_arg(args, int);
			//DbgFmt("Life:%d ", exp->life);
			break;
		}
		case '1': {
			exp->pfxErr=va_arg(args, char*);
			//DbgFmt("PfxErr:%s ", exp->pfxErr);
			break;
		}
		case '2': {
			exp->nameOpErr=va_arg(args, char*);
			//DbgFmt("NameOpErr:%s ", exp->nameOpErr);
			break;
		}
		case 'P': {
			pfx=va_arg(args, IBOp);
			IBVectorCopyPushOp(&exp->pfxs, pfx);
			//DbgFmt("Pfx:%s(%d) ", IBGetPfxName(pfx), (int)pfx);
			break;
		}
		case 'N':{
			nameOp=va_arg(args, IBOp);
			IBVectorCopyPushOp(&exp->nameOps, nameOp);
			//DbgFmt("NameOP:%s(%d) ", IBGetOpName(nameOp), (int)nameOp);
			break;
		}
		case 'e': {
			IBVectorCopyPushOp(&exp->pfxs, OP_Value);
			IBVectorCopyPushOp(&exp->pfxs, OP_Name);
			IBVectorCopyPushOp(&exp->pfxs, OP_Add);
			IBVectorCopyPushOp(&exp->pfxs, OP_Subtract);
			IBVectorCopyPushOp(&exp->pfxs, OP_Divide);
			IBVectorCopyPushOp(&exp->pfxs, OP_Multiply);
			break;
		}
		case 'c': {
			//PLINE;
			//DbgFmt("CodeBlockMacro ", "");
			IBVectorCopyPushOp(&exp->pfxs, OP_Op);
			IBVectorCopyPushOp(&exp->pfxs, OP_VarType);
			IBVectorCopyPushOp(&exp->pfxs, OP_String);
			IBVectorCopyPushOp(&exp->pfxs, OP_Name);
			IBVectorCopyPushOp(&exp->pfxs, OP_Exclaim);
			IBVectorCopyPushOp(&exp->pfxs, OP_Underscore);
			IBVectorCopyPushOp(&exp->nameOps, OP_If);
			IBVectorCopyPushOp(&exp->nameOps, OP_For);
			IBVectorCopyPushOp(&exp->nameOps, OP_Return);
			IBVectorCopyPushOp(&exp->nameOps, OP_Table);
			IBVectorCopyPushOp(&exp->nameOps, OP_Loop);
			IBVectorCopyPushOp(&exp->nameOps, OP_Break);//TODO: make sure inside loop
			break;
		}
		}
	}
	//DbgFmt("}\n","");
	va_end(args);
}
void IBExpectsPrint(IBExpects* ap) {
	IBOp* oi;
	int idx;
	idx = 0;
	assert(ap);
	oi=NULL;
#ifdef IBDEBUGPRINTS
	IBPushColor(IBBgGREEN);
	printf("[LN:%d] ", ap->lineNumInited);
	IBPopColor();
#endif
	printf("Prefix { ");
	while (oi = (IBOp*)IBVectorIterNext(&ap->pfxs, &idx))
		printf("%s(%d) ", IBGetPfxName(*oi), (int)*oi);
	printf("}\nNameOP { ");
	idx = 0;
	oi=NULL;
	while (oi = (IBOp*)IBVectorIterNext(&ap->nameOps, &idx))
		printf("@%s(%d) ", IBGetOpName(*oi), (int)*oi);
	printf("}\n");
}
void IBExpectsFree(IBExpects* ap) {
	assert(ap);
	IBVectorFreeSimple(&ap->pfxs);
	IBVectorFreeSimple(&ap->nameOps);
}
void TaskInit(IBTask* t, IBOp type) {
	IBVectorInit(&t->working, sizeof(IBObj), OP_Obj, IBVEC_DEFAULT_SLOTCOUNT);
	IBVectorInit(&t->expStack, sizeof(IBExpects), OP_Expects, IBVEC_DEFAULT_SLOTCOUNT);
	IBVectorInit(&t->subTasks, sizeof(IBTask), OP_Task, IBVEC_DEFAULT_SLOTCOUNT);
	IBCodeBlockInit(&t->code);
	t->type = type;
	memset(&t->exprData, 0, sizeof(IBTaskNeedExpression));
}
void TaskFree(IBTask* t) {
	assert(t);
	IBCodeBlockFree(&t->code);
	IBVectorFree(&t->subTasks, TaskFree);
	IBVectorFree(&t->expStack, IBExpectsFree);
	IBVectorFree(&t->working, ObjFree);
}
void TaskFindWorkingObj(IBTask* t, IBOp type, IBObj** outDP){
	IBObj*o=0;
	int idx=0;
	assert(t);
	while(o=IBVectorIterNext(&t->working,&idx)){
		if (o->type == type) {
			if(outDP) (*outDP)=o;
			return;
		}
	}
}
void IBTypeInfoInit(IBTypeInfo* ti, IBOp type, char* name){
	IBASSERT0(ti);
	memset(ti,0,sizeof*ti);
	IBStrInitWithCStr(&ti->name, name);
	ti->type=type;
	IBVectorInit(&ti->members, sizeof*ti,
		OP_IBTypeInfo, IBVEC_DEFAULT_SLOTCOUNT);
	IB_SETMAGICP(ti);
}
void IBTypeInfoFree(IBTypeInfo* ti){
	IBASSERT0(ti);
	IB_ASSERTMAGICP(ti);
	IBVectorFree(&ti->members, IBTypeInfoFree);
	IBStrFree(&ti->name);
}
void IBTypeInfoFindMember(IBTypeInfo* ti, char* name, IBTypeInfo** outDP){
	IBTypeInfo*nti=0;
	int idx=0;
	IBASSERT0(ti);
	IBASSERT0(name);
	IBASSERT0(strlen(name)>0);
	while(nti=IBVectorIterNext(&ti->members,&idx)){
		IB_ASSERTMAGICP(nti);
		IBASSERT0(nti->name.start)
			if (!strcmp(nti->name.start, name)) break;
	}
	if(outDP) (*outDP)=nti;
}
char* IBGetCEqu(IBOp op) {
	int sz;
	int i;
	assert(op != OP_Unknown);
	if(op==OP_None)return "";
	sz=sizeof(cEquivelents) / sizeof(cEquivelents[0]);
	for (i = 0; i < sz; i++) {
		if (op == cEquivelents[i].op) return cEquivelents[i].name;
	}
	return "[GetCEqu UNKNOWN!!!!]";
}
IB_DBObj* IB_DBObjNew(IBStr* fileName, int fileLine, int fileColumn,
	IBOp objType, IBStr* objName){
	IB_DBObj* ret=NULL;
	ret = malloc(sizeof * ret);
	assert(ret);
	if (ret) {
		bool cic = false;
		IBStrInit(&ret->fileName);
		IBStrAppend(&ret->fileName, fileName);
		ret->fileLine = fileLine;
		ret->fileColumn = fileColumn;
		ret->type = objType;
		IBStrInit(&ret->name);
		IBStrAppend(&ret->name, objName);
		cic = IBStrContainsAnyOfChars(&ret->name, IB_IllegalDbObjNameChars);
		assert(!cic);
		IBVectorInit(&ret->children, sizeof(IB_DBObj), OP_DBObj, IBVEC_DEFAULT_SLOTCOUNT);
	}
	return ret;
}
void IB_DBObjFree(IB_DBObj* obj){
	IBStrFree(&obj->name);
	IBStrFree(&obj->fileName);
	IBVectorFree(&obj->children, IB_DBObjFree);
	free(obj);
}
void IBDatabaseInit(IBDatabase* db){
	IBStr fn;
	IBStr on;
	IBStrInitExt(&fn, "");
	IBStrInitExt(&on, "root");
	db->root = IB_DBObjNew(&fn, 0, 0, OP_RootObj, &on);
}
void IBDatabaseFree(IBDatabase* db){
	IB_DBObjFree(db->root);
}
IB_DBObj* IBDatabaseFind(IBDatabase* db, IBStr location){
	return NULL;
}
char* IBGetOpName(IBOp op) {
	int sz;
	int i;
	sz=sizeof(opNamesAR) / sizeof(opNamesAR[0]);
	for (i = 0; i < sz; i++) {
		if (op == opNamesAR[i].op) return opNamesAR[i].name;
	}
	assert(0);
	return "?";
}
char* IBGetPfxName(IBOp op) {
	int sz;
	int i;
	sz=sizeof(pfxNames) / sizeof(pfxNames[0]);
	for (i = 0; i < sz; i++) {
		if (op == pfxNames[i].op) return pfxNames[i].name;
	}
	//assert(0);
	return "?";
}
//IBOp GetOpFromName(char* name) {
//	int sz;
//	int i;
//	sz=sizeof(opNames) / sizeof(opNames[0]);
//	for (i = 0; i < sz; i++) {
//		if (!strcmp(opNames[i].name, name)) return opNames[i].op;
//	}
//	return OP_Error;
//}
IBOp IBGetOpFromNameList(char* name, IBOp list) {
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
IBOp IBOPFromPfxCh(char ch) {
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
void IBOverwriteStr(char** str, char* with) {
	assert(str);
	assert(with);
	if (!with){
		if(*str)free(*str);
		*str=NULL;
		return;
	}
	if (*str) free(*str);
	*str = strdup(with);
	assert(*str);
}
IBObj* IBLayer3GetObj(IBLayer3* ibc) {
	return IBVectorTop(&ibc->ObjStack);
}
IBNameInfo* _IBLayer3SearchNameInfo(IBLayer3* ibc, char* name, int ln){
	int idx;
	IBNameInfo* ni=NULL;
	assert(ibc);
	assert(ibc->CodeBlockStack.elemCount);
	assert(name);
	assert(name[0]);
	//DbgFmt("[%d]"__FUNCTION__"(,%s)\n", ln, name);
	idx=ibc->CodeBlockStack.elemCount-1;
	while(idx>=0){
		IBCodeBlock* cb = IBVectorGet(&ibc->CodeBlockStack, idx);
		assert(cb);
		IB_ASSERTMAGICP(&cb->localVariables.pairs);
		ni = IBNameInfoDBFind(&cb->localVariables, name);
		if (ni) return ni;
		idx--;
	}
	ni=IBNameInfoDBFind(&ibc->GlobalVariables, name);
	return ni;
}
void IBLayer3PrintVecData(struct IBVecData* data, IBOp type){
	if (!data)return;
	switch (type) {
	case OP_Op: {
		IBOp op = *(IBOp*)data;
		DbgFmt("%s(%d)\n", IBGetOpName(op), (int)op);
		break;
	}
	case OP_Obj: {
		IBObj* obj = (IBObj*)data;
		DbgFmt("OBJ -> [\n"
			"\t\tType: %s(%d)\n"
			"\t\tModifier: %s(%d)\n"
			"\t\tPrivacy: %s(%d)\n"
			"\t\tName: %s\n"
			"\t\tStr: %s\n"
			"\t\tVal as i32: %d\n"
			"\t\tFunc: -> [\n"
			"\t\t\tRet val as i32: %d\n"
			"\t\t\tRet type: %s(%d)\n"
			"\t\t\tRet type mod: %s(%d)\n"
			"\t\t\tThing task: -> [\n",
			IBGetOpName(obj->type), (int)obj->type,
			IBGetOpName(obj->modifier), (int)obj->modifier,
			IBGetOpName(obj->privacy), (int)obj->privacy,
			obj->name, obj->str,
			obj->val.i32,
			obj->func.retVal.i32,
			IBGetOpName(obj->func.retValType), (int)obj->func.retValType,
			IBGetOpName(obj->func.retTypeMod), (int)obj->func.retTypeMod
		);
		IBLayer3PrintVecData(obj->func.thingTask, OP_Task);
		DbgFmt(
			"\t\t]\n"
			"\t\tVar -> [\n"
			"\t\t\tVal as i32: %d\n"
			"\t\t\tVal set: %s\n"
			"\t\t\tType: %s(%d)\n"
			"\t\t\tModifier: %s(%d)\n"
			"\t\t\tPrivacy: %s(%d)\n"
			"\t\t]\n"
			"\t\tArg -> [\n"
			"\t\t\tType: %s(%d)\n"
			"\t\t\tMod: %s(%d)\n"
			"\t\tIfObj -> [\n"
			"\t\t\tlvTYPE: %s(%d)\n"
			"\t\t\tlvName: %s\n"
			"\t\t\tlvVal: %d\n"
			"\t\t\tlvDataType: %s(%d)\n"
			"\t\t\tlvMod: %s(%d)\n"
			"\t\t\tmidOP: %s(%d)\n"
			"\t\t\trvTYPE: %s(%d)\n"
			"\t\t\trvName: %s\n"
			"\t\t\trvVal: %d\n"
			"\t\t\trvDataType: %s(%d)\n"
			"\t\t\trvMod: %s(%d)\n"
			"\t\t]\n",
			obj->var.val.i32,
			IBBoolStr(obj->var.valSet),
			IBGetOpName(obj->var.type), (int)obj->var.type,
			IBGetOpName(obj->var.mod), (int)obj->var.mod,
			IBGetOpName(obj->var.privacy), (int)obj->var.privacy,
			IBGetOpName(obj->arg.type), (int)obj->arg.type,
			IBGetOpName(obj->arg.mod), (int)obj->arg.mod,
			IBGetOpName(obj->ifO.lvTYPE), (int)obj->ifO.lvTYPE,
			obj->ifO.lvName,
			obj->ifO.lvVal.i32,
			IBGetOpName(obj->ifO.lvDataType), (int)obj->ifO.lvDataType,
			IBGetOpName(obj->ifO.lvMod), (int)obj->ifO.lvMod,
			IBGetOpName(obj->ifO.midOp), (int)obj->ifO.midOp,
			IBGetOpName(obj->ifO.rvTYPE), (int)obj->ifO.rvTYPE,
			obj->ifO.rvName,
			obj->ifO.rvVal.i32,
			IBGetOpName(obj->ifO.rvDataType), (int)obj->ifO.rvDataType,
			IBGetOpName(obj->ifO.rvMod), (int)obj->ifO.rvMod
		);
		DbgPuts("\t]\n");
		break;
	}
	case OP_Expects: {
		IBExpects* exp = (IBExpects*)data;
		DbgFmt("EXPECTS -> [\n"
			"\t\tpfxErr: %s\n"
			"\t\tnameOpErr: %s\n"
			"\t\tlife: %d\n"
			"\t\tlineNumInited: %d\n"
			"\t\tpfxs:\n",
			exp->pfxErr, exp->nameOpErr,
			exp->life, exp->lineNumInited);
		IBLayer3VecPrint(&exp->pfxs);
		DbgPuts("\t\tnameOps:\n");
		IBLayer3VecPrint(&exp->nameOps);
		DbgPuts("]\n");
		break;
	}
	case OP_Task: {
		/*IBTask* task = (IBTask*)data;
		DbgFmt("TASK -> [\n"
			"\tType: %s(%d)\n"
			"\tCode1:\n%s(%d)\n"
			"\tCode2:\n%s(%d)\n"
			"\tWorking objs:\n",
			IBGetOpName(task->type), (int)task->type);
		IBLayer3VecPrint(ibc, &task->working);
		DbgFmt("\tExpects:\n", "");
		IBLayer3VecPrint(ibc, &task->expStack);
		DbgFmt("]\n", "");*/
		break;
	}
	default:
		break;
	}
}
void IBLayer3VecPrint(IBVector* vec) {
	int idx;
	struct IBVecData* data;

	assert(vec);
	switch (vec->type) {
	case OP_Op: {
		IBPushColor(IBFgCYAN);
		DbgPuts("OPs");
		break;
	}
	case OP_Obj: {
		IBPushColor(IBFgYELLOW);
		DbgPuts("OBJs");
		break;
	}
	case OP_Expects: {
		IBPushColor(IBFgRED);
		DbgPuts("EXPECTs");
		break;
	}
	case OP_Task: {
		IBPushColor(IBFgBLUE);
		DbgPuts("TASKs");
		break;
	}
	default: {
		IBPushColor(IBFgMAGENTA);
		DbgPuts("UNKNOWN!");
		break;
	}
	}
	DbgPuts(" vec -> [\n");
	data = NULL;
	idx = 0;
	while (data = IBVectorIterNext(vec, &idx)) {
		DbgFmt("\t(%d)", idx);
		IBLayer3PrintVecData(data, vec->type);
	}
	switch (vec->type) {
	case OP_Obj:
	case OP_Expects:
	case OP_Task:
	case OP_Op: {
		DbgPuts("]\n");
		break;
	}
	}
	IBPopColor();
}
IBObj* IBLayer3FindStackObjRev(IBLayer3* ibc, IBOp type){
	assert(ibc);
	assert(type>0);
	for(int i = ibc->ObjStack.elemCount - 1; i >= 0; i--){
		IBObj*o=IBVectorGet(&ibc->ObjStack,i);
		assert(o);
		if(o->type==type)return o;
	}
	return NULL;
}
IBObj* IBLayer3FindStackObjUnderIndex(IBLayer3* ibc, int index, IBOp type) {
	int i;
	if(index<0)index=ibc->ObjStack.elemCount + index;
	IBASSERT0(index>=0);
	if(ibc->ObjStack.elemCount < 2)
		Err(OP_Error, "Not enough objects on stack");
	if(index > ibc->ObjStack.elemCount)
		Err(OP_Error, "Index out of bounds");
	for (i = index - 1; i >= 0;) {
		IBObj* o;
		o = (IBObj*)IBVectorGet(&ibc->ObjStack, i);
		i--;
		if (o->type == type) return o;
	}
	return NULL;
}
IBObj*
IBLayer3FindStackObjUnderTop
(IBLayer3* ibc, IBOp type) {
	IBObj* o;
	int i;
	if(ibc->ObjStack.elemCount < 2) return NULL;
	for (i = ibc->ObjStack.elemCount - 1; i >= 0;) {
		o = (IBObj*)IBVectorGet(&ibc->ObjStack, i--);
		if (o->type == type) return o;
	}
	return NULL;
}
void
IBLayer3Init
(IBLayer3* ibc) {
	IBObj* o=0;
	IBExpects* exp=0;
	IBCodeBlock* cb=0;
	IBTypeInfo*u8ti=0,*i8ti=0,*c8ti=0,*u16ti=0,
		*i16ti=0,*u32ti=0,*i32ti=0,*f32ti=0,
		*u64ti=0,*i64ti=0,*d64ti=0,*stringti=0;
	memset(ibc,0,sizeof*ibc);

	IBVectorInit(&ibc->DotPathVec, sizeof(IBStr), 
		OP_IBStr, IBVEC_DEFAULT_SLOTCOUNT);

	IBVectorInit(&ibc->TypeRegistry,
		sizeof(IBTypeInfo), OP_IBTypeInfo, 32);
	IBVectorPush(&ibc->TypeRegistry,&u8ti);
	IBVectorPush(&ibc->TypeRegistry,&i8ti);
	IBVectorPush(&ibc->TypeRegistry,&c8ti);
	IBVectorPush(&ibc->TypeRegistry,&u16ti);
	IBVectorPush(&ibc->TypeRegistry,&i16ti);
	IBVectorPush(&ibc->TypeRegistry,&u32ti);
	IBVectorPush(&ibc->TypeRegistry,&i32ti);
	IBVectorPush(&ibc->TypeRegistry,&f32ti);
	IBVectorPush(&ibc->TypeRegistry,&u64ti);
	IBVectorPush(&ibc->TypeRegistry,&i64ti);
	IBVectorPush(&ibc->TypeRegistry,&d64ti);
	IBVectorPush(&ibc->TypeRegistry,&stringti);
	IBTypeInfoInit(u8ti, OP_u8,"u8");
	IBTypeInfoInit(i8ti, OP_i8,"i8");
	IBTypeInfoInit(c8ti, OP_c8,"c8");
	IBTypeInfoInit(u16ti, OP_u16,"u16");
	IBTypeInfoInit(i16ti, OP_i16,"i16");
	IBTypeInfoInit(u32ti, OP_u32,"u32");
	IBTypeInfoInit(i32ti, OP_i32,"i32");
	IBTypeInfoInit(f32ti, OP_f32,"f32");
	IBTypeInfoInit(u64ti, OP_u64,"u64");
	IBTypeInfoInit(i64ti, OP_i64,"i64");
	IBTypeInfoInit(d64ti, OP_d64,"d64");
	IBTypeInfoInit(stringti, OP_String,"nts");
	//getchar();
	//IBLayer3RegisterCustomType(ibc,"ct",OP_Enum,NULL);

	ibc->Running = true;
	ibc->Line = 1;
	ibc->Column = 1;
	ibc->LineIS = 1;
	ibc->ColumnIS = 1;
	IBStrInit(&ibc->RunArguments);
	IBStrInit(&ibc->CHeader_Structs);
	IBStrInit(&ibc->CHeader_Funcs);
	IBStrInit(&ibc->CurrentLineStr);
	IBStrInit(&ibc->CIncludesStr);
	IBStrInit(&ibc->CCode);
	IBStrInit(&ibc->FinalOutput);
	IBVectorInit(&ibc->ArrayIndexExprsVec, sizeof(IBStr), OP_IBStr, IBVEC_DEFAULT_SLOTCOUNT);
	ibc->Pointer = OP_NotSet;
	ibc->Privacy = OP_Public;
	ibc->CommentMode = OP_NotSet;
	IBNameInfoDBInit(&ibc->GlobalVariables);
	IBStrInit(&ibc->CurSpace);
	IBVectorInit(&ibc->ObjStack, sizeof(IBObj), OP_Obj, IBVEC_DEFAULT_SLOTCOUNT);
	IBVectorInit(&ibc->ModeStack, sizeof(IBOp), OP_Op, IBVEC_DEFAULT_SLOTCOUNT);
	IBVectorInit(&ibc->StrReadPtrsStack, sizeof(bool), OP_Bool, IBVEC_DEFAULT_SLOTCOUNT);
	IBVectorInit(&ibc->TaskStack, sizeof(IBTask), OP_Task, IBVEC_DEFAULT_SLOTCOUNT);
	IBVectorInit(&ibc->CodeBlockStack,
		sizeof(IBCodeBlock), OP_IBCodeBlock, IBVEC_DEFAULT_SLOTCOUNT);
	IBVectorInit(&ibc->ExpressionStack,
		sizeof(IBExpression), OP_IBExpression, IBVEC_DEFAULT_SLOTCOUNT);
	IBVectorPush(&ibc->CodeBlockStack, &cb);
	IBCodeBlockInit(cb);
	IBVectorCopyPushBool(&ibc->StrReadPtrsStack, false);
	IBLayer3Push(ibc, OP_ModePrefixPass, false);
	IBLayer3PushObj(ibc, &o);
	IBLayer3PushTask(ibc, OP_RootTask, &exp, NULL);
	IBExpectsInit(exp, "PPPNNNNNNNNNNNN",
		OP_Op, OP_VarType, OP_Subtract, OP_Use, OP_Imaginary, OP_Func,
		OP_Enum, OP_Flags, OP_Struct, OP_Methods, OP_Space, OP_Public,
		OP_Private, OP_RunArguments, OP_CInclude
	);
}
void IBLayer3Free(IBLayer3* ibc) {
	IBTask* t;
	IBObj* o;
	IBCodeBlock* cb;
	IBStr rootCbFinal;

	assert(ibc);
	IBStrInit(&rootCbFinal);
	if (ibc->InputStr) {
		IBLayer3InputStr(ibc, ibc->InputStr);
		ibc->InputStr = NULL;
	}
	cb=IBVectorTop(&ibc->CodeBlockStack);
	o=IBLayer3GetObj(ibc);
	if (ibc->StringMode)
		Err(OP_Error, "Reached end of file without closing string");
	if(ibc->Str[0]) IBLayer3StrPayload(ibc);
	if (cb->localVariables.pairs.elemCount)
		Err(OP_Error, "root codeblock can't have variables in it!!!");
	t=IBLayer3GetTask(ibc);
	if (ibc->TaskStack.elemCount) {
		switch (t->type) {
		case OP_FuncWantCode:
		case OP_FuncNeedRetVal:
			Err(OP_Error, "Reached end of file without closing function");
			break;
		case OP_FuncSigComplete:
		case OP_FuncHasName: {
			SetObjType(o, OP_FuncSigComplete);
			IBLayer3PopObj(ibc, true, NULL);
			IBLayer3FinishTask(ibc);
			break;
		}
		}
	}
	if (t->type != OP_RootTask)Err(OP_ErrDirtyTaskStack,
		"Reached end of file not at root task");
	if (ibc->CodeBlockStack.elemCount != 1)
		Err(OP_Error, "dirty codeblock stack");
	IBCodeBlockFinish(cb, &rootCbFinal);
	/*if (IBStrLen(&cb->variables) +
			IBStrLen(&cb->code) +
			IBStrLen(&cb->footer))
		Err(OP_Error, "dirty codeblock. expected root codeblock to be empty");*/
	//IBStrAppendCStr(&ibc->CHeader_Funcs, "\n#endif\n");
	if (ibc->IncludeCStdioHeader)
		IBStrAppendFmt(&ibc->FinalOutput,
			"#include <stdio.h>\n");
	if (ibc->IncludeCStdlibHeader)
		IBStrAppendFmt(&ibc->FinalOutput,
			"#include <stdlib.h>\n");
	IBStrAppendFmt(&ibc->FinalOutput, "%s\n%s%s%s\n%s",
        ibc->CIncludesStr.start,
		rootCbFinal.start,
		ibc->CHeader_Structs.start,
		ibc->CHeader_Funcs.start,
		ibc->CCode.start);
#ifdef IBDEBUGPRINTS
	IBPushColor(IBFgWHITE);
	IBPushColor(IBFgGREEN);
	DbgPuts("C99 output: \n");
	IBPopColor();
	DbgFmt("%s", ibc->FinalOutput.start);
#else
	printf("%s", ibc->FinalOutput.start);
#endif
	IBPopColor();
	IBLayer3CompileTCC(ibc);
	IBPushColor(IBFgMAGENTA);
	DbgPuts("-> Compilation complete. Press any key <-\n");
	IBPopColor();
	getchar();
	/*if (ibc->SpaceNameStr != NULL) {
		free(ibc->SpaceNameStr);
		ibc->SpaceNameStr = NULL;
	}*/
	IBStrFree(&rootCbFinal);
	IBNameInfoDBFree(&ibc->GlobalVariables);
	IBStrFree(&ibc->CurSpace);
	IBVectorFree(&ibc->CodeBlockStack, IBCodeBlockFree);
	IBVectorFree(&ibc->ObjStack, ObjFree);
	IBVectorFreeSimple(&ibc->ModeStack);
	IBVectorFreeSimple(&ibc->ExpressionStack);
	IBVectorFreeSimple(&ibc->StrReadPtrsStack);
	IBVectorFree(&ibc->TaskStack, TaskFree);
	//IBNameInfoDBFree(&ibc->NameTypeCtx);
	IBStrFree(&ibc->CHeader_Structs);
	IBStrFree(&ibc->CHeader_Funcs);
	IBStrFree(&ibc->FinalOutput);
	IBStrFree(&ibc->RunArguments);
	IBStrFree(&ibc->CIncludesStr);
	IBStrFree(&ibc->CurrentLineStr);
	IBVectorFree(&ibc->TypeRegistry, IBTypeInfoFree);
	IBVectorFree(&ibc->DotPathVec, IBStrFree);
	//IBStrFree(&ibc->ArrayIndexExprStr);
	IBVectorFree(&ibc->ArrayIndexExprsVec, IBStrFree);
	IBStrFree(&ibc->CCode);
}
void
IBLayer3RegisterCustomType
(IBLayer3* ibc, char* name, IBOp type, IBTypeInfo**outDP){
	IBTypeInfo*ti=0;
	IBASSERT0(ibc);
	IBASSERT0(name);
	IBASSERT0(type>0);
	IBLayer3FindType(ibc,name,&ti);
	if(ti) {
		ErrF(OP_AlreadyExists, "type %s already exists", name);
		return;
	}
	switch (type) {
	case OP_Enum:
	case OP_Struct:
	case OP_Func:
		break;
	IBCASE_UNIMP
	}
	IB_ASSERTMAGIC(ibc->TypeRegistry);
	IBVectorPush(&ibc->TypeRegistry, &ti);
	IBTypeInfoInit(ti, type, name);
	if(outDP) (*outDP)=ti;
}
void
IBLayer3FindType
(IBLayer3* ibc, char* name, IBTypeInfo** outDP){
	IBTypeInfo*ti=0;
	int idx=0;
	IBASSERT0(ibc);
	IBASSERT0(name);
	IBASSERT0(strlen(name)>0);
	while(ti=IBVectorIterNext(&ibc->TypeRegistry,&idx)){
		IB_ASSERTMAGICP(ti);
		IBASSERT0(ti->name.start)
		if (!strcmp(ti->name.start, name)) break;
	}
	if(outDP) (*outDP)=ti;
}
IBTask* IBLayer3FindTaskUnderIndex(IBLayer3* ibc, int index, IBOp type, int limit){
	int i;
	if(ibc->TaskStack.elemCount < 2)
		Err(OP_Error, "Not enough tasks on stack");
	if(index == -1) index = ibc->TaskStack.elemCount - 1;
	if(index >= ibc->TaskStack.elemCount)
		Err(OP_Error, "Index out of bounds");
	for (i = index - 1; i >= 0 && limit--;) {
		IBTask* t;
		t = (IBTask*)IBVectorGet(&ibc->TaskStack, i--);
		if (t->type == type) return t;
	}
	return NULL;
}
void
_IBLayer3_TCCErrFunc
(IBLayer3* ibc, const char* msg) {
	ErrF(OP_TCC_Error, "%s\n", msg);
}
void
IBLayer3CompileTCC
(IBLayer3* ibc){
	int (*Entry)(int, char**);
	IBASSERT(ibc->TCC==NULL,"");
	ibc->TCC = tcc_new();
	IBASSERT(ibc->TCC,"");
	tcc_set_error_func(ibc->TCC, ibc,
		_IBLayer3_TCCErrFunc);
	tcc_set_output_type(ibc->TCC,
		TCC_OUTPUT_MEMORY);
#ifdef __TINYC__
	tcc_add_sysinclude_path(ibc->TCC, "src/");
	tcc_add_sysinclude_path(ibc->TCC, "ext/tcc/include/");
	tcc_add_library_path(ibc->TCC, "ext/tcc/lib/");
#else
	tcc_add_sysinclude_path(ibc->TCC, "../src/");
	tcc_add_sysinclude_path(ibc->TCC, "../ext/tcc/include/");
	tcc_add_library_path(ibc->TCC, "../ext/tcc/lib/");
#endif
	IBASSERT(IBStrLen(&ibc->FinalOutput) > 0,
		"no code to compile");
	IBASSERT(tcc_compile_string(ibc->TCC,
		(const char*)ibc->FinalOutput.start) != -1,
		"TCC compile failed!");
	IBASSERT(tcc_relocate(ibc->TCC, TCC_RELOCATE_AUTO)>=0,"");
	Entry = tcc_get_symbol(ibc->TCC, "main");
#define IBRUN_MAXARGS 10
	char* argv[IBRUN_MAXARGS];
	int argc = 0;
	memset(argv, 0, sizeof(argv));
	int argsLen = IBStrLen(&ibc->RunArguments);
	//int avc = 0;
	if (argsLen) {
		IBStr ls;
		IBStrInit(&ls);
		for (int i = 0; i < argsLen; ++i) {
			char ch = ibc->RunArguments.start[i];
			if (ch == ' ') {
				IBOverwriteStr(&argv[argc], ls.start);
				IBStrClear(&ls);
				++argc;
				continue;
			}
			IBStrAppendCh(&ls, ch, 1);
		}
		if (IBStrLen(&ls)) {
			IBOverwriteStr(&argv[argc], ls.start);
			++argc;
		}
		IBStrFree(&ls);
	}
	if (Entry) {
		int entryRet=0;
		DbgPuts("Program output:\n");
		entryRet = Entry(argc, argv);
		IBPushColor(IBFgGREEN);
		DbgFmt("\n\nmain() returned %d.\n", entryRet);
		IBPopColor();
	}
	for (int i = 0; i < IBRUN_MAXARGS; ++i) {
		if (argv[i])free(argv[i]);
	}
	tcc_delete(ibc->TCC);
	ibc->TCC=NULL;
}
int IBLayer3GetTabCount(IBLayer3* ibc){
	return ibc->CodeBlockStack.elemCount - 1;
}
void IBLayer3Done(IBLayer3* ibc){
	IBTask* t = IBLayer3GetTask(ibc);
	assert(t);
	if (ibc->TaskStack.elemCount < 1) Err(OP_ErrNoTask, "");
	switch (t->type) {
	case OP_CaseWantCode: {
		IBLayer3FinishTask(ibc);
		t = IBLayer3GetTask(ibc);
		assert(t->type == OP_TableWantCase);
		IBLayer3FinishTask(ibc);
		break;
	}
	case OP_StructWantContent: {
		IBObj* o;
		o = IBLayer3GetObj(ibc);
		assert(o->type == OP_Struct);
		IBLayer3PopObj(ibc, true, &o);
		assert(o->type == OP_NotSet);
		ibc->DefiningStruct=0;
		IBLayer3FinishTask(ibc);
		break;
	}
	case OP_MethodsWantContent: {
		IBObj* o;
		o = IBLayer3GetObj(ibc);
		assert(o->type==OP_Methods);
		IBLayer3PopObj(ibc,true,NULL);
		IBLayer3FinishTask(ibc);
		ibc->DefiningMethods=0;
		if(ibc->_methodsStructName)
			free(ibc->_methodsStructName);
		ibc->_methodsStructName=0;
		break;
	}
	case OP_EnumWantContent:
	case OP_BlockWantCode:
	case OP_LoopBlockWantCode:
	case OP_IfBlockWantCode: {
		IBLayer3FinishTask(ibc);
		break;
	}
	case OP_Func:
	case OP_FuncHasName:
	case OP_FuncWantCode: {
		IBObj* o;
		IBVector* wo;
		int idx;
		assert(t);
		PLINE;
		DbgPuts(" Finishing function\n");
		idx = 0;
		/*t = NULL;*/
		o = NULL;
		wo = &t->working;
		while (o = (IBObj*)IBVectorIterNext(wo, &idx)) {
			/*TODO: could cache func obj index later*/
			if (o->type == OP_FuncSigComplete) {
				if (o->func.retValType != OP_Void) {
					IBExpects* exp;
					IBLayer3PushExpects(ibc, &exp);
					/*if (o->func.retType == OP_c8 && o->func.retTypeMod == OP_Pointer) {
						ExpectsInit(exp, "PP", OP_Name, OP_String);
					}
					else {

					}*/
					IBExpectsInit(exp, "PPP", OP_Value, OP_Name, OP_String);
					SetTaskType(t, OP_FuncNeedRetVal);
				}
				else {
					SetTaskType(t, OP_Func);
					IBLayer3FinishTask(ibc);
				}
			}
		}
		break;
	}
	}
}
IBObj* IBLayer3FindWorkingObj(IBLayer3* ibc, IBOp type){
	IBObj* o=NULL;
	int idx=0;
	IBTask* t=IBLayer3GetTask(ibc);
	assert(t);
	while (o = IBVectorIterNext(&t->working, &idx))
		if (o->type == type) return o;
	return NULL;
}
IBObj* IBLayer3FindWorkingObjRev(IBLayer3* ibc, IBOp type){
	IBTask* t=IBLayer3GetTask(ibc);
	assert(t);
	for(int i = t->working.elemCount - 1; i >= 0; i--){
		IBObj*o=IBVectorGet(&t->working,i);
		assert(o);
		if(o->type==type)return o;
	}
	return NULL;
}
IBObj* IBLayer3FindWorkingObjUnderIndex(IBLayer3* ibc, int index, IBOp type)
{
	IBTask* t=IBLayer3GetTask(ibc);
	int i=0;
	IBASSERT0(t);
	if(index<0)index=t->working.elemCount + index;
	IBASSERT0(index>=0);
	if(t->working.elemCount < 2)
		Err(OP_Error, "Not enough objects on stack");
	if(index > t->working.elemCount)
		Err(OP_Error, "Index out of bounds");
	for (i = index - 1; i >= 0;) {
		IBObj*o=0;
		o = IBVectorGet(&t->working, i);
		i--;
		if (o->type == type) return o;
	}
	return NULL;
}
IBCodeBlock* IBLayer3CodeBlocksTop(IBLayer3* ibc){
	assert(ibc->CodeBlockStack.elemCount);
	return (IBCodeBlock*)IBVectorTop(&ibc->CodeBlockStack);
}
void _IBLayer3PushCodeBlock(IBLayer3* ibc, IBCodeBlock** cbDP){
	IBCodeBlock*cb=0;
	DbgPuts(" Push code block\n");
	IBVectorPush(&ibc->CodeBlockStack, &cb);
	IBCodeBlockInit(cb);
	if(cbDP) (*cbDP) = cb;
}
void _IBLayer3PopCodeBlock(IBLayer3* ibc, bool copyToParent,
		IBCodeBlock** cbDP){
	assert(ibc->CodeBlockStack.elemCount > 1);
	DbgFmt(" Pop code block. Copy to parent: %s\n", IBBoolStr(copyToParent));
	if (copyToParent) {
		if (ibc->CodeBlockStack.elemCount >= 2) {
			IBCodeBlockFinish(IBVectorTop(&ibc->CodeBlockStack),
				&((IBCodeBlock*)IBVectorGet(&ibc->CodeBlockStack,
					ibc->CodeBlockStack.elemCount - 2))->code);
		} else Err(OP_Error, "COMPILER FAILURE. No parent code block!");
	}
	IBVectorPop(&ibc->CodeBlockStack, IBCodeBlockFree);
	if(cbDP) (*cbDP) = IBLayer3CodeBlocksTop(ibc);
}
void _IBLayer3PushTask(IBLayer3* ibc, IBOp taskOP, IBExpects** exectsDP,
		IBTask** taskDP) {
	IBTask* t = IBLayer3GetTask(ibc), *bt=NULL;
	DbgPuts(" Push task: ");
	if (t) {
		DbgFmt("%s(%d) -> ", IBGetOpName(t->type), (int)t->type);
		bt = t;
	}
	DbgFmt("%s(%d)\n", IBGetOpName(taskOP), (int)taskOP);
	IBVectorPush(&ibc->TaskStack, &t);
	if(taskDP) (*taskDP) = t;
	TaskInit(t, taskOP);
	IBVectorPush(&t->expStack, exectsDP);
	if (!exectsDP) {
		IBExpects* exp=IBTaskGetExpTop(t);
		switch (taskOP) {
		case OP_NeedExpression: {
			IBExpectsInit(exp, "PPPPPPPP", OP_Value, OP_Name, OP_Add,
				OP_Subtract, OP_Divide, OP_Multiply, OP_Or, OP_Dot);
			break;
		}
		default: {
			IBExpectsInit(exp, "P", OP_Null);
			break;
		}
		}
	}
}
void _IBLayer3PopTask(IBLayer3* ibc, IBTask** taskDP, bool popToParent) {
	IBTask* t=NULL;
	IBTask* t2=NULL;
	IBTask copy;
	assert(ibc);
	t=IBLayer3GetTask(ibc);
	assert(t);
	DbgFmt(" Pop task %s(%d) ", IBGetOpName(t->type), (int)t->type);
	if (popToParent) {
		if (ibc->TaskStack.elemCount >= 2) {
			memcpy(&copy, t, sizeof(IBTask));
		}else Err(OP_Error, "COMPILER FAILURE. No parent task!");
	}
	IBVectorPop(&ibc->TaskStack, popToParent ? NULL : TaskFree);
	t2 = IBLayer3GetTask(ibc);
	assert(t2);
	DbgFmt("-> %s(%d) Pop to parent: %s\n",
		IBGetOpName(t2->type), (int)t2->type, IBBoolStr(popToParent));
	if (popToParent) {
		assert(t2->type != OP_RootTask);
		IBVectorCopyPush(&t2->subTasks, &copy);
	}
	if(taskDP) (*taskDP) = t2;
}
void _IBLayer3PushObj(IBLayer3* ibc, IBObj** o) {
	IBObj *obj=IBLayer3GetObj(ibc);
	//assert(obj);
	DbgPuts(" Push obj: ");
	if (obj && ibc->ObjStack.elemCount) {
#ifdef IBDEBUGPRINTS
		ObjPrint(obj);
		DbgPuts(" -> ");
#endif
	}
	IBVectorPush(&ibc->ObjStack, &obj);
	ObjInit(obj);
#ifdef IBDEBUGPRINTS
	ObjPrint(obj);
	DbgPuts("\n");
#endif
	if (o) { (*o) = obj; }
}
void _IBLayer3PopObj(IBLayer3* ibc, bool pushToWorking, IBObj** objDP) {
	IBObj* o;
	IBTask* t;
	t=IBLayer3GetTask(ibc);
	assert(t);
	o = IBLayer3GetObj(ibc);
	assert(o);
	if (pushToWorking){
		IBObj* newHome;
		if (o->type == OP_NotSet)Err(OP_Error, "");
#ifdef IBDEBUGPRINTS
		DbgFmt(" To working(%s(%d)): ",IBGetOpName(t->type), (int)t->type);
#endif
#ifdef IBDEBUGPRINTS
		ObjPrint(o);
		DbgPuts("\n");
#endif
		IBVectorPush(&t->working, &newHome);
		assert(newHome);
		memcpy(newHome, o, sizeof(IBObj));
		if(ibc->ObjStack.elemCount < 1){
			Err(OP_Error, "no obj in stack");
		}else if (ibc->ObjStack.elemCount == 1) {
			ObjInit(o);
		}else if (ibc->ObjStack.elemCount > 1) {
			IBVectorPop(&ibc->ObjStack, NULL);
			o=IBLayer3GetObj(ibc);
		}
	}else{
#ifdef IBDEBUGPRINTS
		DbgPuts("Pop obj: ");
		ObjPrint(o);
#endif
		if (ibc->ObjStack.elemCount == 1) {
			ObjFree(o);
			ObjInit(o);
		}
		else if (ibc->ObjStack.elemCount > 1) {
			printf("ec: %d\n", ibc->ObjStack.elemCount);
			IBVectorPop(&ibc->ObjStack, ObjFree);
			o = IBLayer3GetObj(ibc);
		}
		DbgPuts(" -> ");
		assert(ibc->ObjStack.elemCount);
#ifdef IBDEBUGPRINTS
		ObjPrint(o);
		DbgPuts("");
#endif
	}
	if(objDP) (*objDP) = o;
}
void _IBLayer3Push(IBLayer3* ibc, IBOp mode, bool strAllowSpace){
	IBOp cm;
	assert(ibc);
	cm=IBLayer3GetMode(ibc);
	ibc->StrAllowSpace = strAllowSpace;
	IBVectorCopyPushOp(&ibc->ModeStack, mode);
	DbgFmt(" Push mode: %s(%d) -> %s(%d)\n", IBGetOpName(cm), (int)cm,
		IBGetOpName(mode), (int)mode);
}
void _IBLayer3Pop(IBLayer3* ibc) {
	IBOp type=OP_Null;
	IBOp mode;
	IBOp mode2;
	IBTask *t;
	IBExpects* exp;
	assert(ibc);
	t = IBLayer3GetTask(ibc);
	assert(t);
	mode=IBLayer3GetMode(ibc);
	IBVectorPop(&ibc->ModeStack, NULL);
	mode2 = IBLayer3GetMode(ibc);
	if(t)type=t->type;
	DbgFmt(" Pop mode: %s(%d) -> %s(%d)\n", IBGetOpName(mode), (int)mode,
		IBGetOpName(mode2), (int)mode2);
	assert(t->expStack.elemCount);
#ifdef IBDEBUGPRINTS
	exp=IBTaskGetExpTop(t);
	//ExpectsPrint(exp);
#endif
}
void _IBObjSetType(IBObj* obj, IBOp type) {
	DbgFmt(" obj type: %s(%d) -> %s(%d)\n",
		IBGetOpName(obj->type), (int)obj->type, IBGetOpName(type), (int)type);
	obj->type = type;
}
void IBObjSetMod(IBObj* obj, IBOp mod) {
	DbgFmt("obj mod: %s(%d) -> %s(%d)\n",
		IBGetOpName(obj->modifier), (int)obj->modifier, IBGetOpName(mod), (int)mod);
	obj->modifier = mod;
}
void _IBObjSetName(IBObj* obj, char* name) {
	assert(obj);
	DbgFmt(" obj name: %s -> %s\n", obj->name, name);
	IBOverwriteStr(&obj->name, name);
}
void IBObjSetStr(IBObj* obj, char* Str) {
	DbgFmt("obj str: %s -> %s\n", obj->str, Str);
	IBOverwriteStr(&obj->str, Str);
}
void IBObjCopy(IBObj* dst, IBObj* src) {
	assert(dst && src);
	memcpy(dst, src, sizeof(IBObj));
	dst->name = NULL;
	dst->str = NULL;
	if(src->name) IBOverwriteStr(&dst->name, src->name);
	if(src->str) IBOverwriteStr(&dst->str, src->str);
}
void ObjPrint(IBObj* obj) {
	assert(obj);
	if(obj){
		printf("[");
		if (obj->type != OP_NotSet) {
			printf("Type:%s(%d),", IBGetOpName(obj->type), (int)obj->type);
		}
		if(obj->name)printf("Name:%s,", obj->name);
		if (obj->str)printf("Str:%s,", obj->str);
		if (obj->modifier != OP_NotSet) {
			printf("Mod:%s,", IBGetOpName(obj->modifier));
		}
		printf("Val:%d", obj->val.i32);
		printf("]");
	}
}
void IBLayer3PushExpects(IBLayer3* ibc, IBExpects** expDP){
	IBTask* t;
	IBExpects* exp;
	t = IBLayer3GetTask(ibc);
	assert(t);
	assert(expDP);
	if(expDP){
		IBVectorPush(&t->expStack, &exp);
		assert(exp);
		(*expDP) = exp;
	}
}
void IBLayer3PopExpects(IBLayer3* ibc) {
	IBTask* t = IBLayer3GetTask(ibc);
	IBExpects* exp;
	assert(t);
	exp = IBVectorTop(&t->expStack);
	assert(exp);
	IBVector* pfxsIb = &exp->pfxs;
	if (pfxsIb->elemCount) {
		IBOp* oi=NULL;
		int idx=0;

#ifdef IBDEBUGPRINTS
		DbgPuts(" Expects POP: { ");
		idx = 0;
		while (oi = (IBOp*)IBVectorIterNext(pfxsIb, &idx)) {
			DbgFmt("%s ", IBGetPfxName(*oi));
		}
		DbgPuts("} -> { ");
#endif
		IBVectorPop(&t->expStack, IBExpectsFree);
		if (t->expStack.elemCount<1) Err(OP_Error, "catastrophic failure");
		pfxsIb = &((IBExpects*)IBVectorTop(&t->expStack))->pfxs;
		assert(pfxsIb);
#ifdef IBDEBUGPRINTS
		idx = 0;
		oi = NULL;
		while (oi = (IBOp*)IBVectorIterNext(pfxsIb,&idx)) {
			DbgFmt("%s ", IBGetPfxName(*oi));
		}
		DbgPuts("}\n");
#endif
	}
}
void IBLayer3ReplaceExpects(IBLayer3* ibc, IBExpects** expDP){
	IBTask* t = IBLayer3GetTask(ibc);
	IBExpects* exp;
	assert(t);
	exp = IBTaskGetExpTop(t);
	assert(exp);
//#ifdef IBDEBUGPRINTS
//	PLINE;
//	DbgFmt(" Replace expects:\n", "");
//	ExpectsPrint(exp);
//#endif
	IBExpectsFree(exp);
	*expDP = exp;
}
bool IBLayer3IsPfxExpected(IBLayer3* ibc, IBOp pfx) {
	IBOp* oi;
	int idx;
	IBTask* t;
	IBExpects* ap;
	if(pfx == OP_PfxlessValue) pfx = OP_Value;
	if (pfx == OP_Letter_azAZ||pfx==OP_Comment) return true;
	t = NULL;
	ap = NULL;
	t = IBLayer3GetTask(ibc);
	assert(t);
	assert(t->expStack.elemCount);
	idx = 0;
	oi = NULL;
	ap = IBTaskGetExpTop(t);
	if(ap) while (oi = (IBOp*)IBVectorIterNext(&ap->pfxs,&idx)) {
		assert(oi);
		if (oi && *oi == pfx)
			return true;
	}
	return false;
}
bool IBLayer3IsNameOpExpected(IBLayer3* ibc, IBOp nameOp){
	IBOp* oi;
	int idx;
	IBTask* t;
	IBExpects* exp;
	t = IBLayer3GetTask(ibc);
	assert(t);
	assert(t->expStack.elemCount);
	idx = 0;
	oi = NULL;
	exp = IBTaskGetExpTop(t);
	while (oi = (IBOp*)IBVectorIterNext(&exp->nameOps, &idx)) {
		assert(oi);
		if (oi && *oi == nameOp)
			return true;
	}
	return false;
}
//wtf
void IBLayer3Tick(IBLayer3* ibc, struct FILE* f){
	char ch;
	if (ibc->InputStr) {
		IBLayer3InputStr(ibc, ibc->InputStr);
		ibc->InputStr = NULL;
	}
	else {
		assert(f);
		if ((ch = (char)fgetc(f)) != EOF) {
			if (ch != 0xffffffff)
				IBLayer3InputChar(ibc, ch);
		}
		else {
			IBLayer3InputChar(ibc, '\n');
			ibc->Running = false;
		}
	}
}
/*NO NEWLINES AT END OF STR*/
void IBLayer3InputChar(IBLayer3* ibc, char ch){
	IBOp m;
	IBTask* t;
	IBObj* o;
	bool nl;
	ibc->Ch = ch;
	if (ibc->CommentMode == OP_NotSet && ibc->Ch != IBCOMMENT_CHAR)
		IBStrAppendCh(&ibc->CurrentLineStr, ibc->Ch, 1);
	nl = false;
	m=IBLayer3GetMode(ibc);
	t = IBLayer3GetTask(ibc);
	o = IBLayer3GetObj(ibc);
	if (ibc->LastCh == IBCOMMENT_CHAR &&
		ibc->Ch == IBCOMMENT_CHAR_OPEN)
	{
		IBLayer3Push(ibc, OP_ModeMultiLineComment, false);
		ibc->CommentMode = OP_MultiLineComment;
		ibc->Ch = '\0';
	}
	else if (ibc->CommentMode == OP_MultiLineComment&&
		ibc->LastCh == IBCOMMENT_CHAR_CLOSE &&
		ibc->Ch == IBCOMMENT_CHAR)
	{
		IBLayer3Pop(ibc);
		ibc->CommentMode = OP_NotSet;
		ibc->Ch = '\0';
	}
	//if(ibc->CommentMode==OP_NotSet&&
	//	ibc->Ch==IBCOMMENT_CHAR/*&&
	//	ibc->LastCh!=IBCOMMENT_CHAR*/)
	//{
	//	/*PLINE;
	//	DbgFmt(" LINE COMMENT ON\n","");*/
	//	ibc->CommentMode = OP_Comment;
	//	IBLayer3Push(ibc, OP_ModeComment, false);
	//}else if(ibc->CommentMode==OP_Comment&&
	//		ibc->LastCh==ibc->Ch &&
	//			!ibc->StringMode
	//			&&ibc->Ch==IBCOMMENT_CHAR&&
	//			m==OP_ModeComment)
	//{
	//	/*PLINE;
	//	DbgFmt(" MULTI COMMENT ON!!!!!!\n","");*/
	//	IBLayer3Pop(ibc);
	//	IBLayer3Push(ibc, OP_ModeMultiLineComment, false);
	//	ibc->CommentMode = OP_MultiLineComment;
	//	ibc->Ch='\0';
	//}else if(ibc->CommentMode==OP_MultiLineComment&&
	//	ibc->LastCh==ibc->Ch &&
	//			!ibc->StringMode
	//			&&ibc->Ch==IBCOMMENT_CHAR&&
	//			m==OP_ModeMultiLineComment) {
	//	/*PLINE;
	//	DbgFmt(" MULTI COMMENT OFF!\n","");*/
	//	ibc->CommentMode=OP_NotSet;
	//}
	switch (ibc->Ch) {
	case OP_ParenthesisOpen: {//expression wrapper
		break;
	}
	case OP_ParenthesisClose: {
		break;
	}
	case '\0': return;
	case '\n': { /* \n PFXLINEEND */
		t=IBLayer3GetTask(ibc);
		nl = true;
		if (ibc->CommentMode == OP_Comment) {
			IBLayer3Pop(ibc);
			ibc->CommentMode = OP_NotSet;
		}

		switch (m) {
		case OP_ModeStrPass: {
			IBLayer3StrPayload(ibc);
			break;
		}
		}

		o=IBLayer3GetObj(ibc);
		switch (o->type) {
		case OP_CallWantArgs: {
			IBLayer3PopObj(ibc, true, NULL);
			break;
		}
		}

		t=IBLayer3GetTask(ibc);
		o=IBLayer3GetObj(ibc);
		assert(t->type > 0);
		switch(t->type){
		case OP_NeedExpression: {
			IBLayer3FinishTask(ibc);
			t= IBLayer3GetTask(ibc);
			switch (t->type) {
			case OP_TableCaseNeedExpr: {
				IBExpects* exp = NULL;
				assert(t->subTasks.elemCount == 1);
				SetTaskType(t, OP_CaseWantCode);
				IBLayer3ReplaceExpects(ibc, &exp);
				IBExpectsInit(exp, "NNc", OP_Case, OP_Fall);
				IBLayer3PushCodeBlock(ibc, NULL);
				break;
			}
			case OP_TableNeedExpr: {
				IBExpects* exp=NULL;
				assert(t->subTasks.elemCount == 1);
				SetTaskType(t, OP_TableWantCase);
				IBLayer3ReplaceExpects(ibc, &exp);
				IBExpectsInit(exp, "PPN", OP_Op, OP_Underscore, OP_Case);
				IBLayer3PushCodeBlock(ibc, NULL);
				break;
			}
			case OP_VarNeedExpr:{
				IBLayer3PopObj(ibc,true,&o);
			}
			case OP_ExprToName: {
				IBLayer3FinishTask(ibc);
				break;
			}
			IBCASE_UNIMP
			}
			break;
		}
		IBCASE_BLOCKWANTCODE
		{
			break;
		}
		case OP_VarWantValue:{
			assert(o->type == OP_VarWantValue);
			IBLayer3PopObj(ibc, true, &o);
			IBLayer3FinishTask(ibc);
			break;
		}
		case OP_CallWantArgs: {
			IBLayer3FinishTask(ibc);
			t = IBLayer3GetTask(ibc);
			switch (t->type) {
			case OP_ActOnNameEquals:
			case OP_CallFunc: {
				IBLayer3FinishTask(ibc);
				break;
			}
			IBCASE_UNIMP
			}
			break;
		}
		case OP_IfFinished: {
			IBCodeBlock* cb;
			IBExpects* exp;
			IBLayer3PopObj(ibc, true, &o);
			SetTaskType(t, OP_IfBlockWantCode);
			IBLayer3ReplaceExpects(ibc, &exp);
			IBExpectsInit(exp, "PNNc",
				OP_Underscore, OP_ElseIf, OP_Else);
			IBLayer3PushCodeBlock(ibc, &cb);
			break;
		}
		case OP_StructWantRepr: {
			IBExpects* exp;
			SetTaskType(t, OP_StructWantContent);
			IBLayer3ReplaceExpects(ibc, &exp);
			IBExpectsInit(exp, "PPP",
				OP_Op, OP_Underscore, OP_VarType);
			break;
		}
		case OP_CPrintfHaveFmtStr: {
			IBLayer3FinishTask(ibc);
			break;
		}
		case OP_FuncArgsVarNeedsName: {
			Err(OP_FuncArgsVarNeedsName,"");
			break;
		}
		case OP_FuncNeedRetVal: {
			Err(OP_FuncNeedRetVal, "");
			break;
		}
		case OP_FuncSigComplete:
		case OP_FuncHasName: {
			o=IBLayer3GetObj(ibc);
			assert(o);
			SetObjType(o, OP_FuncSigComplete);
			PopExpects();
			IBLayer3PopObj(ibc, true, &o);
			if(!ibc->Imaginary){
				IBExpects *exp;
				//IBCodeBlock *cb;
				IBLayer3PushExpects(ibc, &exp);
				IBExpectsInit(exp, "c", OP_Null);
				SetTaskType(t, OP_FuncWantCode);
				//IBDictManip(&cb->locals, IBDStr, )
			} else {
				IBLayer3FinishTask(ibc);
			}
			break;
		}
		}
		/*if (ibc->CommentMode == OP_NotSet)*/ {
			int l = ibc->InputStr ? ibc->LineIS : ibc->Line;
			int c = ibc->InputStr ? ibc->ColumnIS : ibc->Column;
			int stripped = 0;
			IBPushColor(IBBgCYAN);
			DbgFmt("[LINE END AT %d:%d]", l, c);
			IBPopColor();
			IBPushColor(IBFgYELLOW);
			stripped = IBStrStripFront(&ibc->CurrentLineStr, '\t');
			DbgFmt(" %s", ibc->CurrentLineStr.start);
			IBPopColor();
			IBStrClear(&ibc->CurrentLineStr);
		}
		ibc->Imaginary = false;
		ibc->Pfx = OP_Null;
		ibc->DotPathOn=false;
		break;
	}
	}
	m = IBLayer3GetMode(ibc);
	if (!nl /*&& ibc->CommentMode == OP_NotSet*/) {
//#ifdef IBDEBUGPRINTS
//		{
//			int l = ibc->InputStr ? ibc->LineIS : ibc->Line;
//			int c = ibc->InputStr ? ibc->ColumnIS : ibc->Column;
//			if (ibc->Ch == ' ') printf("-> SPACE (0x%x)\n", ibc->Ch);
//			else printf("-> %c (0x%x) %d:%d\n",
//				ibc->Ch, ibc->Ch, l, c);
//		}
//#endif
		switch (m) {
		case OP_ModeComment:
		case OP_ModeMultiLineComment:
			break;
		case OP_ModePrefixPass: {
			IBLayer3Prefix(ibc);
			break;
		}
		case OP_ModeStrPass: {
			IBLayer3Str(ibc);
			break;
		}
		case OP_ModeArrayIndexExpr: {
		    IBStr* avT=IBVectorTop(&ibc->ArrayIndexExprsVec);
			if (ibc->Ch == ']') {
				IBObj* o = IBLayer3GetObj(ibc);
				PLINE;
				DbgFmt(" Got Array index expr: %s[%s]\n",
					ibc->Str,
					avT->start);
				if(!strncmp(avT->start, "as ", 3)){
					IBOp nameOP = IBGetOpFromNameList(avT->start + 3, OP_NameOps);
					switch(nameOP){
					IBCASE_NUMTYPES
					{
						if(ibc->Varcast!=OP_Null)
							Err(OP_Error, "");
						ibc->Varcast=nameOP;
						break;
					}
					IBCASE_UNIMP
					}
				}
				IBLayer3Pop(ibc);
			}
			else
				IBStrAppendCh(avT, ibc->Ch, 1);
			break;
		}
		default: Err(OP_Error, "unknown mode");
			break;
		}
	}
	if (!ibc->InputStr) ibc->Column++;
	else ibc->ColumnIS++;
	if (nl) {
		if (IBLayer3IsPfxExpected(ibc, OP_LineEnd) && t->expStack.elemCount>1)
			PopExpects();
		if (!ibc->InputStr) {
			ibc->Column = 1;
			ibc->Line++;
		}
		else {
			ibc->ColumnIS = 1;
			ibc->LineIS++;
		}
	}
	ibc->LastCh=ibc->Ch;
	//if(m==OP_ModeMultiLineComment&&ibc->CommentMode==OP_NotSet){
	//	IBLayer3Pop(ibc);
	//}
	//DbgFmt("End of InputChar\n", 0);
}
void IBLayer3InputStr(IBLayer3* ibc, char* str){
	int i;
	for (i = 0; str[i] != '\0'; i++)
		IBLayer3InputChar(ibc, str[i]);
}
IBVal IBLayer3StrToVal(IBLayer3* ibc, char* str, IBOp expectedType) {
	IBVal ret;
	ret.i32 = 0;
	switch (expectedType) {
	case OP_c8:
	case OP_i16:
	case OP_u32:
	case OP_u8:
	case OP_u16:
	case OP_i32: { ret.i32 = atoi(str); break; }
	case OP_i64:
	case OP_u64: { ret.u64 = atoll(str); break; }
	case OP_f32:
	case OP_d64: { ret.d64 = atof(str); break; }
	}
	return ret;
}
char* IBLayer3GetCPrintfFmtForType(IBLayer3* ibc, IBOp type) {
	switch (type) {
	case OP_i32:    return "d";
	case OP_i64:    return "lld";
	case OP_u64:    return "llu";
	case OP_d64:    return "f";
	case OP_f32:    return "f";
	case OP_u32:    return "u";
	case OP_c8:
	case OP_Char:   return "c";
	case OP_String:
	case OP_Bool:   return "s";
	}
	Err(OP_Error, "GetPrintfFmtForType: unknown type");
	return "???";
}
void Val2Str(char *dest, int destSz, IBVal v, IBOp type) {

	switch (type) {
	case OP_u8:  { snprintf(dest, destSz, "%u",   v.u8);  break; }
	case OP_c8:  { snprintf(dest, destSz, "%c",   v.c8);  break; }
	case OP_i16: { snprintf(dest, destSz, "%d",   v.i16); break; }
	case OP_u16: { snprintf(dest, destSz, "%u",   v.u16); break; }
	case OP_i32: { snprintf(dest, destSz, "%d",   v.i32); break; }
	case OP_i64: { snprintf(dest, destSz, "%lld", v.i64); break; }
	case OP_u32: { snprintf(dest, destSz, "%u",   v.u32); break; }
	case OP_u64: { snprintf(dest, destSz, "%llu", v.u64); break; }
	case OP_f32: { snprintf(dest, destSz, "%f",   v.f32); break; }
	case OP_d64: { snprintf(dest, destSz, "%f",   v.d64); break; }
	}
}
void _IBLayer3FinishTask(IBLayer3* ibc)	{
	IBVector* wObjs;
	IBCodeBlock* cb;
	int tabCount;
	IBTask* t;
	bool pop2Parent=false;
	t = IBLayer3GetTask(ibc);
	assert(t);
	DbgFmt(" FinishTask: %s(%d)\n", IBGetOpName(t->type), (int)t->type);
	if(!ibc->TaskStack.elemCount)Err(OP_ErrNoTask, "task stack EMPTY!");
	wObjs = &t->working;
	assert(wObjs);
	if (!wObjs->elemCount) {/*Err(OP_Error, "workingObjs EMPTY!");*/
		DbgPuts("Warning: working objs for this task is empty!\n");
	}
	cb=IBLayer3CodeBlocksTop(ibc);
	tabCount=IBLayer3GetTabCount(ibc);
	switch (t->type) {
	case OP_MethodsWantContent:{
		IBObj*o=IBLayer3FindWorkingObjRev(ibc,OP_Methods);
		assert(o);
		break;
	}
	case OP_EnumWantContent: {
		IBTypeInfo*ti=0;
		int idx = 0;
		int flagsI = 0;
		IBObj* o = IBVectorFront(&t->working);
		IBObj* eo = o;
		bool oneFound = false;
		assert(o);
		if(!eo || !eo->name || *eo->name == '\0')
			Err(OP_Error, "enum needs a name");
		IBLayer3FindType(ibc,eo->name,&ti);
		IBASSERT0(!ti);
		if(ti) ErrF(OP_AlreadyExists,"type %s already exists", eo->name);
		IBLayer3RegisterCustomType(ibc,eo->name,OP_Enum,&ti);
		ti->Enum.isFlags=eo->enumO.flags;
		IBStrAppendFmt(&t->code.header, "enum E%s {\n", eo->name);
		IBStrAppendFmt(&t->code.footer, "};\n\n", eo->name);
		while (o = (IBObj*)IBVectorIterNext(wObjs, &idx)) {
			switch (o->type) {
			case OP_Enum: break;
			case OP_EnumName: {
				IBTypeInfo*nti=0;
				IBVectorPush(&ti->members,&nti);
				IBTypeInfoInit(nti,OP_EnumVal,o->name);
				oneFound = true;
				IBStrAppendFmt(&t->code.code, "\tE%s_%s", eo->name, o->name);
				if (eo->enumO.flags) {
					nti->EnumValue.val=flagsI;
					IBStrAppendFmt(&t->code.code, " = %d", flagsI);
					flagsI *= 2;
					if (flagsI == 0) flagsI = 2;
				}else{
					nti->EnumValue.val=ti->members.elemCount-1;
				}
				IBStrAppendFmt(&t->code.code, "%s\n", idx == wObjs->elemCount ? "" : ",");
				break;
			}
			IBCASE_UNIMP
			}
		}
		if (!oneFound) Err(OP_Error, "need at least one case in enum");
		IBCodeBlockFinish(&t->code, &ibc->CHeader_Structs);
		break;
	}
	case OP_CaseWantCode: {
		IBStr fo;
		IBTask* st;
		IBObj *to=IBLayer3FindWorkingObj(ibc, OP_TableCase);
		assert(to);
		assert(t->subTasks.elemCount == 1);
		st = IBVectorGet(&t->subTasks, 0);
		assert(st);
		IBStrInit(&fo);
		IBCodeBlockFinish(&st->code, &fo);
		IBStrAppendCh(&cb->header, '\t', tabCount - 2);
		IBStrAppendFmt(&cb->header, "case %s: {\n", fo.start);
		if (to->table.fallthru == false) {
			IBStrAppendCh(&cb->footer, '\t', tabCount - 1);
			IBStrAppendCStr(&cb->footer, "break;\n");
		}
		IBStrAppendCh(&cb->footer, '\t', tabCount - 2);
		IBStrAppendFmt(&cb->footer, "}\n");
		IBLayer3PopCodeBlock(ibc, true, &cb);
		IBStrFree(&fo);
		break;
	}
	case OP_TableWantCase: {
		IBStr fo;
		IBTask* st;
		assert(t->subTasks.elemCount == 1);
		st=IBVectorGet(&t->subTasks, 0);
		IBStrInit(&fo);
		IBCodeBlockFinish(&st->code, &fo);
		IBStrAppendCh(&cb->header, '\t', tabCount - 1);
		IBStrAppendFmt(&cb->header, "switch (%s) {\n", fo.start);
		IBStrAppendCh(&cb->footer, '\t', tabCount - 1);
		IBStrAppendFmt(&cb->footer, "}\n");
		IBLayer3PopCodeBlock(ibc, true, &cb);
		IBStrFree(&fo);
		break;
	}
	case OP_ExprToName: /*{

		break;
	}*/
	case OP_ActOnNameEquals: {
		IBTask* st;
		IBStr fc;
		IBObj* o = (IBObj*)IBVectorGet(wObjs, 0);
		assert(t->subTasks.elemCount == 1);
		st = IBVectorGet(&t->subTasks, 0);
		assert(st);
		assert(o);
		assert(o->type == OP_ActOnName);
		IBStrInit(&fc);
		IBCodeBlockFinish(&st->code, &fc);
		IBStrAppendCh(&cb->code, '\t', tabCount);
		if(!strncmp(o->name,"self.",5)){
			IBStrAppendFmt(&cb->code, "self->%s = %s;\n", o->name + 5, fc.start);
		}
		else {
			IBStrAppendFmt(&cb->code, "%s = %s;\n", o->name, fc.start);
		}
		break;
	}
	case OP_NeedExpression: {
		int idx = 0;
		IBObj* o = NULL;
		bool onOp = false;
		bool gotVal = false;
		if(wObjs->elemCount < 1)Err(OP_Error, "empty expression!");
		pop2Parent = true;
		while (o = (IBObj*)IBVectorIterNext(wObjs, &idx)) {
			switch (o->type) {
			case OP_Or:
			case OP_Multiply:
			case OP_Divide:
			case OP_Subtract:
			case OP_Add: {
				if(!gotVal)Err(OP_Error, "missing op lval in expression");
				onOp = true;
				gotVal = false;
				IBStrAppendFmt(&t->code.code, " %s ", IBGetCEqu(o->type));
				break;
			}
			case OP_Name: {
				IBNameInfo* ni = IBLayer3SearchNameInfo(ibc, o->name);
				IBOp ceq = o->modifier == OP_Pointer ? OP_Ref
					: OP_None;
				if (!ni) {
					ErrF(OP_NotFound, "%s wasn't found", o->name);
				}
				else {
					switch (ni->type) {
						CASE_VALTYPES
						{
							gotVal = true;
							break;
						}
						IBCASE_UNIMP
					}
				}
				IBStrAppendFmt(&t->code.code, "%s%s", IBGetCEqu(ceq), o->name);
				if (onOp) {
					IBStrAppendFmt(&t->code.header, "%s", "(");
					IBStrAppendFmt(&t->code.code, "%s", ")");
				}
				onOp = false;
				break;
			}
			case OP_EnumVal: {
				gotVal=true;
				IBStrAppendFmt(&t->code.code,"E%s_%s",o->str,o->name);
				if (onOp) {
					IBStrAppendFmt(&t->code.header, "%s", "(");
					IBStrAppendFmt(&t->code.code, "%s", ")");
				}
				onOp = false;
				break;
			}
			case OP_StructVar:{
				gotVal=true;
				IBStrAppendFmt(&t->code.code, "%s%s_%s","",o->str,o->name);
				break;
			}
			case OP_Value: {
				gotVal = true;
				switch (o->valType) {
				case OP_Double: {
					IBStrAppendFmt(&t->code.code, "%f", o->val.d64);
					break;
				}
				case OP_Float: {
					IBStrAppendFmt(&t->code.code, "%f", o->val.f32);
					break;
				}
				case OP_Value: {
					IBStrAppendFmt(&t->code.code, "%d", o->val.i32);
					break;
				}
				}
				if (onOp) {
					IBStrAppendFmt(&t->code.header, "%s", "(");
					IBStrAppendFmt(&t->code.code, "%s", ")");
				}
				onOp = false;
				break;
			}
			IBCASE_UNIMP
			}
		}
		if (onOp) Err(OP_Error, "missing op rval in expression");
		break;
	}
	case OP_VarNeedExpr:{
		IBTask* st;
		IBTypeInfo*ti=0;
		//IBObj* o = IBVectorGet(wObjs, 0);
		//assert(o);
		assert(t->subTasks.elemCount == 1);
		st = IBVectorGet(&t->subTasks, 0);
		assert(st);
		
		assert(st->type==OP_NeedExpression);
		assert(!ibc->DefiningStruct);
		IBStrAppendCh(&cb->variables, '\t', tabCount);
		IBObj*vo=0;
		TaskFindWorkingObj(t,OP_VarNeedExpr,&vo);
		assert(vo);
		IBLayer3FindType(ibc,vo->str, &ti);
		assert(ti);
		IBStr stf;
		IBStrInit(&stf);
		IBCodeBlockFinish(&st->code, &stf);
		char* typeStr = vo->var.type == OP_Unknown ? vo->str : IBGetCEqu(vo->var.type);
		IBStrAppendFmt(&cb->variables, "%s%s%s %s = %s;\n", ti->type==OP_Enum ? "enum E"
			: "",
			typeStr, IBGetCEqu(vo->var.mod), vo->name, stf.start);
		IBStrFree(&stf);
		break;
	}
	case OP_VarWantValue: {
		int idx = 0;
		IBObj* o = NULL;
		bool thing = false;
		IBTask* parent =
			IBLayer3FindTaskUnderIndex(ibc, -1, OP_StructWantContent, 1);
		//assert(parent);
		if (parent && parent->type == OP_StructWantContent) {
			thing = true;
			pop2Parent = true;
		}
		IBStr* vstr = thing ? &parent->code.code
			: &IBLayer3CodeBlocksTop(ibc)->variables;
		while (o = (IBObj*)IBVectorIterNext(wObjs, &idx)) {
			switch (o->type) {
			case OP_VarComplete:
			case OP_VarWantValue: {
				IBTypeInfo*ti=0;
				IBStrAppendCh(vstr, '\t', thing ? 1 : tabCount);
				if (o->var.type == OP_String) {
					o->var.type = OP_c8;
					o->var.mod = OP_Pointer;
				}
				char* typeStr = o->var.type == OP_Unknown ? o->str : IBGetCEqu(o->var.type);
				char*typePfx="";
				IBLayer3FindType(ibc, typeStr, &ti);
				if(ti){
					switch(ti->type){
					case OP_Enum:
						typePfx="enum E";
						break;
					case OP_Struct:
						typePfx="struct S";
						break;
					}
				}
				IBStrAppendFmt(vstr, "%s%s%s %s", typePfx, typeStr, IBGetCEqu(o->var.mod), o->name);
				if (o->var.valSet && !thing) {
					IBStrAppendCStr(vstr, " = ");
					switch (o->var.type) {
					case OP_i8:
					case OP_i16:
					case OP_i64:
					case OP_i32: {
						IBStrAppendFmt(vstr, "%d", o->var.val.i32);
						break;
					}
					case OP_d64: {
						IBStrAppendFmt(vstr, "%f", o->var.val.d64);
						break;
					}
					case OP_f32: {
						IBStrAppendFmt(vstr, "%f", o->var.val.f32);
						break;
					}
					case OP_Bool: {
						IBStrAppendFmt(vstr, "%s", IBBoolStrChar(o->var.val.boolean));
						break;
					}
					case OP_c8: {
						if (o->var.mod == OP_Pointer) {
							IBStrAppendFmt(vstr, "\"%s\"", o->var.valStrLiteral);
						}
						else {
							IBStrAppendFmt(vstr, "\'%c\'", o->var.val.c8);
						}
						break;
					}
					case OP_u8:{
						IBStrAppendFmt(vstr, "%u", o->var.val.u8);
						break;
					}
					case OP_u16:{
						IBStrAppendFmt(vstr, "%u", o->var.val.u16);
						break;
					}
					case OP_u32:{
						IBStrAppendFmt(vstr, "%u", o->var.val.u32);
						break;
					}
					case OP_u64:{
						IBStrAppendFmt(vstr, "%llu", o->var.val.u64);
						break;
					}
					IBCASE_UNIMP
					}
				}
				IBStrAppendFmt(vstr, "%s\n", ";");
				break;
			}
			}
		}
		break;
	}
	case OP_CallFunc: {
		IBTask* st=NULL;
		/*int idx = 0;
		while (st = IBVectorIterNext(&t->subTasks, &idx)) {

		}*/
		assert(t->subTasks.elemCount == 1);
		st=IBVectorGet(&t->subTasks, 0);
		assert(st);
		IBStrAppendCh(&cb->code, '\t', tabCount);
		IBCodeBlockFinish(&st->code, &cb->code);
		IBStrAppendFmt(&cb->code, "%s\n", ";");
		break;
	}
	case OP_CallWantArgs: {
		IBObj* o = IBVectorGet(wObjs, 0);
		int idx = 0;
		assert(o->type == OP_Call);
		//IBStrAppendCh(&t->code.code, '\t', tabCount);
		IBStrAppendFmt(&t->code.code, "%s", o->str);
		IBStrAppendCStr(&t->code.code, "(");
		while (o = (IBObj*)IBVectorIterNext(wObjs, &idx)) {
			if (o->type != OP_Arg) continue;
			switch(o->valType) {
			case OP_Value: {
				IBStrAppendFmt(&t->code.code, "%llu", o->val.i64);
				break;
			}
			case OP_String: {
				IBStrAppendFmt(&t->code.code, "\"%s\"", o->str);
				break;
			}
			case OP_Name: {
				IBStrAppendFmt(&t->code.code, "%s", o->str);
				break;
			}
			IBCASE_UNIMP
			}
			if (idx <= wObjs->elemCount - 1) {
				IBStrAppendCStr(&t->code.code, ", ");
			}
		}
		IBStrAppendCStr(&t->code.code, ")");
		pop2Parent=true;
		break;
	}
	case OP_BlockReturnNeedValue: {
		IBObj* o;
		IBLayer3PopObj(ibc, true, &o);
		o = IBVectorTop(wObjs);
		IBStrAppendCh(&cb->code, '\t', tabCount);
		IBStrAppendFmt(&cb->code, "return ");
		assert(o->type == OP_BlockReturnNeedValue);
		switch (o->valType) {
		case OP_Value: {
			IBStrAppendFmt(&cb->code, "%llu", o->val.i64);
			break;
		}
		case OP_String: {
			IBStrAppendFmt(&cb->code, "\"%s\"", o->str);
			break;
		}
		case OP_Name: {
			IBStrAppendFmt(&cb->code, "%s", o->str);
			break;
		}
		}
		IBStrAppendCStr(&cb->code, ";\n");
		break;
	}
	case OP_IfBlockWantCode: {
		IBObj*o=0,*m=0;
		int idx = 0;
		while (o = (IBObj*)IBVectorIterNext(wObjs, &idx)) {
			if (o->type == OP_IfFinished) {
				m = o;
				break;
			}
		}
		assert(m);
		if (ibc->LastNameOp != OP_ElseIf) {
			IBStrAppendCh(&cb->header, '\t', tabCount - 1);
			ibc->LastNameOp = OP_Null;
		}
		IBStrAppendFmt(&cb->header, "if (");
		switch (m->ifO.lvTYPE) {
		case OP_Value: {
			char buf[64];
			buf[0]='\0';
			Val2Str(buf, 64, m->ifO.lvVal, m->ifO.lvDataType);
			IBStrAppendFmt(&cb->header, "%s ", buf);
			break;
		}
		case OP_Name:
			IBStrAppendFmt(&cb->header, "%s ", m->ifO.lvName);
			break;
		}
		IBStrAppendFmt(&cb->header, "%s ", IBGetCEqu(m->ifO.midOp));
		switch (m->ifO.rvTYPE) {
		case OP_Value: {
			char buf[64];
			buf[0] = '\0';
			Val2Str(buf, 64, m->ifO.rvVal, m->ifO.rvDataType);
			IBStrAppendFmt(&cb->header, "%s", buf);
			break;
		}
		case OP_Name:
			IBStrAppendFmt(&cb->header, "%s", m->ifO.rvName);
			break;
		}
		IBStrAppendFmt(&cb->header, ") ");
	}
	case OP_LoopBlockWantCode: {
		if (t->type == OP_LoopBlockWantCode) {
			IBStrAppendCh(&cb->header, '\t', tabCount - 1);
			IBStrAppendCStr(&cb->header, "while(1) ");
		}
	}
	case OP_BlockWantCode: {
		IBStrAppendFmt(&cb->header, "{\n");
		IBStrAppendCh(&cb->footer, '\t', tabCount - 1);
		IBStrAppendFmt(&cb->footer, "}\n");
		IBLayer3PopCodeBlock(ibc, true, &cb);
		break;
	}
	case OP_StructWantContent: {
		IBStr header;
		IBStr body;
		IBStr footer;
		IBStr hFile;
		IBStr cFile;
		IBObj* o;
		IBTask* st;
		int idx;
		IBTypeInfo*ti=0;

		IBStrInit(&header);
		IBStrInit(&body);
		IBStrInit(&footer);
		IBStrInit(&hFile);
		IBStrInit(&cFile);
		//idx = 0;
		//while (st = (IBTask*)IBVectorIterNext(&t->subTasks, &idx)) {
		//	switch (st->type) {
		//		case OP_VarWantValue: {
		//			IBObj* o = (IBObj*)IBVectorFront(&st->working);
		//			assert(o);
		//			IBStrAppendFmt(&body,
		//				"\t%s%s %s;\n",
		//				IBGetCEqu(o->var.type),
		//				IBGetCEqu(o->var.mod),
		//				o->name);
		//			//default values will be stored in db
		//			break;
		//		}
		//		IBCASE_UNIMP
		//	}
		//}
		IBCodeBlockFinish(&t->code, &body);
		idx = 0;
		while (o = IBVectorIterNext(wObjs, &idx)) {
			switch (o->type) {
			case OP_Struct: {
				assert(o->name);
				assert(*o->name);

				IBLayer3FindType(ibc,o->name,&ti);
				IBASSERT0(!ti);
				if(ti) ErrF(OP_AlreadyExists,"type %s already exists",o->name);
				IBLayer3RegisterCustomType(ibc,o->name,OP_Struct,&ti);

				IBStrAppendFmt(&header, "struct S%s {\n", o->name);
				IBStrAppendFmt(&footer, "};\n\n", o->name);

				break;
			}
			IBCASE_UNIMP
			}
		}
		IBStrAppend(&ibc->CHeader_Structs, &header);
		IBStrAppend(&ibc->CHeader_Structs, &body);
		IBStrAppend(&ibc->CHeader_Structs, &footer);
		//IBStrAppendFmt(&ibc->CHeaderStructs, "%s%s%s", header, body, footer);
		IBStrFree(&header);
		IBStrFree(&body);
		IBStrFree(&footer);

		{
			int idx=0;
			IBNameInfo*ni=0;
			while(ni=IBVectorIterNext(&cb->localVariables,&idx)){
				IBTypeInfo*nti=0;
				IBVectorPush(&ti->members,&nti);
				IBTypeInfoInit(nti,OP_StructVar,ni->name);
				nti->structVarType=ni->ti;
			}
		}
		IBLayer3PopCodeBlock(ibc, false, &cb);
		break;
	}
	case OP_SpaceHasName: {
		IBObj* o=NULL;
		int idx = 0;
		while (o = (IBObj*)IBVectorIterNext(wObjs, &idx))
			if (o->type == OP_Space) break;
		assert(o);
		if(o)
			IBStrReplaceWithCStr(&ibc->CurSpace, o->name);
		break;
	}
	case OP_FuncWantCode:
	case OP_FuncSigComplete:
	case OP_FuncHasName:
	case OP_Func: {
		IBTypeInfo*ti=0;
		IBObj* o;
		int idx;
		int i;
		int argc;
		IBStr cFuncModsTypeName;
		IBStr cFuncArgsThing;
		IBStr cFuncArgs;
		IBStr cFuncArgsEnd;
		IBStr cFuncCode;
		IBObj* funcObj;
		IBObj* thingObj;

		thingObj = NULL;
		argc = 0;
		IBStrInit(&cFuncModsTypeName);
		IBStrInit(&cFuncArgsThing);
		IBStrInit(&cFuncArgs);
		IBStrInit(&cFuncArgsEnd);
		IBStrInit(&cFuncCode);
		idx = 0;
		funcObj = NULL;
		for (i = 0; i < wObjs->elemCount; ++i) {
			o = (IBObj*)IBVectorGet(wObjs, i);
			switch (o->type) {
			case OP_FuncArgComplete: {/*multiple allowed*/
				IBOp at;
				at = o->arg.type;
				argc++;
				if (at == OP_Null)Err(OP_Error, "arg type NULL");

				if (cFuncArgs.start[0] != '\0') {
					IBStrAppendCStr(&cFuncArgs, ", ");
				}
				IBStrAppendCStr(&cFuncArgs, IBGetCEqu(o->arg.type));
				IBStrAppendCStr(&cFuncArgs, IBGetCEqu(o->arg.mod));
				IBStrAppendCStr(&cFuncArgs, " ");
				if (!o->name)Err(OP_Error, "arg name NULL");
				IBStrAppendCStr(&cFuncArgs, o->name);
				break;
			}
			case OP_Func:
			case OP_FuncHasName:
			case OP_FuncSigComplete:
			case OP_CompletedFunction: {/*should only happen once*/
				IBOp mod;
				funcObj = o;
				mod = o->modifier;
				if (mod != OP_NotSet) {
					IBStrAppendCStr(&cFuncModsTypeName, IBGetCEqu(mod));
					IBStrAppendCStr(&cFuncModsTypeName, " ");
				}
				IBStrAppendCStr(&cFuncModsTypeName, IBGetCEqu(o->func.retValType));
				IBStrAppendCStr(&cFuncModsTypeName, IBGetCEqu(o->func.retTypeMod));
				IBStrAppendCStr(&cFuncModsTypeName, " ");
				if (!o->name)Err(OP_Error, "func name NULL");
				IBTypeInfo*mti=0;
				char mtiC='\0';
				if (o->name) {
					if (o->func.thingTask)//stupid
					{
						IBObj* wo;
						int idx;
						idx = 0;
						wo = IBLayer3FindStackObjUnderTop(ibc, OP_Struct);
						if (wo) {
							IBStrAppendFmt(&cFuncModsTypeName, "S%s_", wo->name);
							thingObj = wo;
						}
					}
					IBObj*mo=IBLayer3FindStackObjRev(ibc,OP_Methods);
					if(mo){
						IBLayer3FindType(ibc,mo->name,&mti);
						assert(mti);
						switch(mti->type){
						case OP_Enum:{
							IBStrAppendCh(&cFuncModsTypeName, 'E', 1);
							mtiC='E';
							break;
						}
						case OP_Struct: {
							IBStrAppendCh(&cFuncModsTypeName, 'S', 1);
							mtiC='S';
							break;
						}
						IBCASE_UNIMP
						}
						IBStrAppendFmt(&cFuncModsTypeName, "%s_", mo->name);
					}
					IBStrAppendCStr(&cFuncModsTypeName, o->name);

					IBASSERT0(!ti);
					IBLayer3FindType(ibc,o->name,&ti);
					IBASSERT0(!ti);
					IBLayer3RegisterCustomType(ibc,o->name,OP_Func,&ti);
					IBASSERT0(ti);
					ti->Function.isMethod=ibc->DefiningStruct;
				}
				IBStrAppendCStr(&cFuncModsTypeName, "(");
				if (thingObj) {
					IBStrAppendFmt(&cFuncArgsThing,
						"struct S%s* self", thingObj->name);
				}
				if(mti){
					switch (mtiC) {
					case 'E': {
						IBStrAppendFmt(&cFuncArgsThing,
							"enum %c%s* self", mtiC, mti->name.start);
						break;
					}
					case 'S':{
						IBStrAppendFmt(&cFuncArgsThing,
							"struct %c%s* self", mtiC, mti->name.start);
						break;
					}
					IBCASE_UNIMP
					}
				}
				break;
			}
			}
		}
		/*idx = 0;
		while (o= (IBObj*)IBVectorIterNext(wObjs,&idx)) {
			switch (o->type) {
			case OP_VarComplete:
			case OP_VarWantValue: {
				char valBuf[32];
				valBuf[0] = '\0';
				IBStrAppendCStr(&cFuncCode, "\t");
				IBStrAppendCStr(&cFuncCode, IBGetCEqu(o->var.type));
				IBStrAppendCStr(&cFuncCode, IBGetCEqu(o->var.mod));
				IBStrAppendCStr(&cFuncCode, " ");
				if (!o->name)Err(OP_Error, "var name NULL");
				IBStrAppendCStr(&cFuncCode, o->name);
				IBStrAppendCStr(&cFuncCode, " = ");
				if (o->var.valSet) {
					Val2Str(valBuf, 32, o->var.val, o->var.type);
					IBStrAppendCStr(&cFuncCode, valBuf);
				}
				else {
					IBStrAppendCStr(&cFuncCode, "0");
				}
				IBStrAppendCStr(&cFuncCode, ";\n");
				break;
			}
			}
		}*/
		if (ibc->Imaginary) {
			//DbgFmt("[@ext @func]","");
			IBStrAppendCStr(&cFuncArgsEnd, ");\n\n");
			IBLayer3PopCodeBlock(ibc, false, &cb);
		}
		else {
			//DbgFmt("[@func]", "");
			IBStr cbOut;
			IBCodeBlock* cb;
			IBStrAppendCStr(&cFuncArgsEnd, ") {\n");
			IBStrInit(&cbOut);
			cb=IBLayer3CodeBlocksTop(ibc);
			IBCodeBlockFinish(cb, &cbOut);
			IBStrAppend(&cFuncCode, &cbOut);
			IBStrFree(&cbOut);
			IBLayer3PopCodeBlock(ibc, false, &cb);
			//no longer valid bcuz u can define methods in structs
			//assert(ibc->CodeBlockStack.elemCount == 1);
			if (!funcObj) {
				Err(OP_Error, "funcObj NULL");
			} else if (funcObj->func.retValType != OP_Void) {
				IBStrAppendCStr(&cFuncCode, "\treturn ");
				switch(funcObj->func.retValVarcast){
				IBCASE_NUMTYPES
				{
					IBStrAppendFmt(&cFuncCode,"(%s) ",
						IBGetCEqu(funcObj->func.retValVarcast));
					break;
				}
				case OP_Null:break;
				IBCASE_UNIMP
				}
				switch (funcObj->func.retTYPE) {
				case OP_String: {
					IBStrAppendFmt(&cFuncCode, "\"%s\"", funcObj->func.retValStr);
					break;
				}
				case OP_Value: {
					//assert(funcObj->func.retValType==OP_i32);
					//IBStrAppendFmt(&cFuncCode, "%d",
					//	funcObj->func.retVal.i32);//for now
					switch (funcObj->func.retValType) {
					case OP_u8:
					case OP_u16:
					case OP_u32:
					case OP_u64:
					case OP_i8:
					case OP_i16:
					case OP_i32:
					case OP_i64: {
						char valBuf[32];
						valBuf[0] = '\0';
						Val2Str(valBuf, 32, funcObj->func.retVal, funcObj->func.retValType);
						IBStrAppendCStr(&cFuncCode, valBuf);
						break;
					}
					IBCASE_UNIMP
					}
					break;
				}
				case OP_Name: {
					IBStrAppendFmt(&cFuncCode, "%s", funcObj->func.retStr);
					break;
				}
				IBCASE_UNIMP
				}
				//IBStrAppendCStr(&cFuncCode, valBuf);
				IBStrAppendCStr(&cFuncCode, ";\n");
			}
			IBStrAppendCStr(&cFuncCode, "}\n\n");
		}
		if (funcObj && strcmp(funcObj->name, "main"))
		{
			IBStrAppendCStr(&ibc->CHeader_Funcs, cFuncModsTypeName.start);
			IBStrAppendCStr(&ibc->CHeader_Funcs, cFuncArgsThing.start);
			if (argc && IBStrLen(&cFuncArgsThing)) IBStrAppendCStr(&ibc->CHeader_Funcs, ", ");
			IBStrAppendCStr(&ibc->CHeader_Funcs, cFuncArgs.start);
			IBStrAppendCStr(&ibc->CHeader_Funcs, ");\n");
		}
		if (!ibc->Imaginary) {
			IBStrAppendCStr(&ibc->CCode, cFuncModsTypeName.start);
			IBStrAppendCStr(&ibc->CCode, cFuncArgsThing.start);
			if (argc && IBStrLen(&cFuncArgsThing)) IBStrAppendCStr(&ibc->CCode, ", ");
			IBStrAppendCStr(&ibc->CCode, cFuncArgs.start);
			IBStrAppendCStr(&ibc->CCode, cFuncArgsEnd.start);
			IBStrAppendCStr(&ibc->CCode, cFuncCode.start);
		}
		IBStrFree(&cFuncModsTypeName);
		IBStrFree(&cFuncArgsThing);
		IBStrFree(&cFuncArgs);
		IBStrFree(&cFuncArgsEnd);
		IBStrFree(&cFuncCode);
		break;
	}
	case OP_CPrintfHaveFmtStr: {
		bool firstPercent;
		IBObj* fmtObj;
		int varIdx;
		int i;
		if (t && wObjs->elemCount) {
			fmtObj = (IBObj*)wObjs->data;
			IBStrAppendCh(&cb->code, '\t', tabCount);
			IBStrAppendCStr(&cb->code, "printf(\"");
			ibc->IncludeCStdioHeader=true;
			firstPercent = false;
			varIdx = 1;
			for (i = 0; i < (int)strlen(fmtObj->str); ++i) {
				char c;
				c = fmtObj->str[i];
				switch (c) {
				case '%': {
					if (!firstPercent) {
						IBStrAppendCStr(&cb->code, "%");
						firstPercent = true;
					}
					else {
						IBObj* vo;
						IBOp voT;
						vo = (IBObj*)IBVectorGet(wObjs, varIdx);
						/*printf("cfmt vidx:%d\n",varIdx);*/
						assert(vo);
						voT = vo->type;
						/*if(voT==OP_String)DB*/
						switch (voT) {
						case OP_Name:{
							IBNameInfo* ni = IBLayer3SearchNameInfo(ibc, vo->name);
							if (!ni)
								Err(OP_Error, "Name not found");
							assert(ni);
							IBStrAppendCStr(&cb->code,
								IBLayer3GetCPrintfFmtForType(ibc, ni->type));
							break;
						}
						case OP_String:
							assert(vo->var.type==OP_String);
						case OP_Value:{
							IBStrAppendCStr(&cb->code,
								IBLayer3GetCPrintfFmtForType(ibc, vo->var.type));
							break;
						}
						case OP_Arg: {
							IBLayer3VecPrint(wObjs);
							assert(0);
						}
						case OP_CPrintfFmtStr: break;
						default:{
							Err(OP_Error, "unhandled printf arg type");
						}

						}
						firstPercent = false;
						varIdx++;
					}
					break;
				}
				default: {
					char chBuf[2];
					chBuf[0] = c;
					chBuf[1] = '\0';
					IBStrAppendCStr(&cb->code, chBuf);
					break;
				}
				}
			}
			IBStrAppendCStr(&cb->code, "\"");
			if (wObjs->elemCount > 1) {
				IBStrAppendCStr(&cb->code, ", ");
			}
			for (i = 1; i < wObjs->elemCount; ++i) {
				IBObj* o;
				o = (IBObj*)IBVectorGet(wObjs, i);
				switch (o->type) {
				case OP_Name: {
					IBNameInfo* ni = IBLayer3SearchNameInfo(ibc, o->name);
					if(ni->type == OP_Bool)
						IBStrAppendFmt(&cb->code, "%s ? \"true\" : \"false\"", o->name);
					else {
						IBStr*ibs=NULL;
						int idx=0;

						IBStrAppendCStr(&cb->code, o->name);
						while(ibs=IBVectorIterNext(&o->arg.arrIndexExprs,&idx)){
							IBStrAppendFmt(&cb->code, "[%s]",
								ibs->start);
						}
					}
					break;
				}
				case OP_String: {
					IBStrAppendCStr(&cb->code, "\"");
					IBStrAppendCStr(&cb->code, o->str);
					IBStrAppendCStr(&cb->code, "\"");
					break;
				}
				case OP_Value: {
					char valBuf[32];
					valBuf[0] = '\0';
					Val2Str(valBuf, 32, o->val, o->var.type);
					IBStrAppendCStr(&cb->code, valBuf);
					break;
				}
				}
				if (i < wObjs->elemCount - 1) {
					IBStrAppendCStr(&cb->code, ", ");
				}
			}
			IBStrAppendCStr(&cb->code, ");\n");
		}
		break;
	}
	}
	IBLayer3PopTask(ibc, &t, pop2Parent);
}
void IBLayer3Prefix(IBLayer3* ibc){
	IBObj* obj;
	IBTask* t;
	IBExpects* expTop;
	t = IBLayer3GetTask(ibc);
	assert(t);
	expTop = IBTaskGetExpTop(t);
	///*for assigning func call ret val to var*/
	//if (ibc->Pfx == OP_Value && ibc->Ch == '@'
	//		&& !ibc->Str[0]) {
	//	IBExpects* exp;
	//	IBLayer3PushExpects(ibc, &exp);
	//	ExpectsInit(exp, "P0", OP_Op, 1);
	//}
	ibc->Pfx = IBOPFromPfxCh(ibc->Ch);
	if(ibc->Pfx == OP_SpaceChar
		|| ibc->Pfx == OP_TabChar) return;
	if (ibc->Pfx == OP_Unknown)
		Err(OP_ErrUnknownPfx, "catastrophic err");
	obj=IBLayer3GetObj(ibc);
	if (ibc->Pfx != OP_Unknown
		&& (!t || expTop->pfxs.elemCount)
		&& !IBLayer3IsPfxExpected(ibc, ibc->Pfx))
			Err(OP_ErrUnexpectedNextPfx, "");
	IBPushColor(IBBgMAGENTA);
	//PFX:
	DbgPuts("PFX");
	IBPopColor();
	DbgPuts(": ");
	IBPushColor(IBBgBROWN);
	DbgFmt("%s(%d)", IBGetPfxName(ibc->Pfx), (int)ibc->Pfx);
	IBPopColor();
	DbgPuts("\n");
	switch(ibc->Pfx) {
	case OP_PfxlessValue: {
		char chBuf[2];
		chBuf[0] = ibc->Ch;
		chBuf[1] = '\0';
		ibc->Pfx = OP_Value;
		StrConcat(ibc->Str, IBLayer3STR_MAX, chBuf);
		break;
	}
	}
	switch (ibc->Pfx) {
	case OP_SingleQuote: /* ' */
	case OP_String: { /* " */
		ibc->StringMode = true;
		IBLayer3Push(ibc, OP_ModeStrPass, false);
		break;
	}
	case OP_Letter_azAZ: {
		char chBuf[2];
		chBuf[0] = ibc->Ch;
		chBuf[1] = '\0';
		//ibc->Pfx = OP_Op;
		StrConcat(ibc->Str, IBLayer3STR_MAX, chBuf);
		IBLayer3Push(ibc, OP_ModeStrPass, false);
		IBVectorCopyPushBool(&ibc->StrReadPtrsStack, true);
		break;
	}
	case OP_VarType: {
		//TODO: move this into func sig task
		IBVectorCopyPushBool(&ibc->StrReadPtrsStack, true);
	}
	case OP_LessThan:
	case OP_GreaterThan:
	case OP_Or:
	case OP_Add:
	case OP_Subtract:
	case OP_Multiply:
	case OP_Divide:
	case OP_Dot://context aware
	case OP_Caret:
	case OP_Underscore:
	case OP_BracketOpen:
	//case OP_BracketClose:
	case OP_CurlyBraceOpen:
	//case OP_CurlyBraceClose:
	case OP_Comma:
	case OP_Exclaim:
	case OP_Value:
	case OP_Op:
	case OP_Name: {
		IBVectorCopyPushBool(&ibc->StrReadPtrsStack, true);
		ibc->DotPathOn=true;
		/*getchar();*/
		IBLayer3Push(ibc, OP_ModeStrPass, false);
	}
	case OP_SpaceChar:
	case OP_Comment:
		break;
	}
	if (ibc->Pfx == OP_Op) {
		if (expTop && expTop->life && --expTop->life <= 0) {
			IBVectorPop(&t->expStack, IBExpectsFree);
		}
	}
}
void IBLayer3Str(IBLayer3* ibc){
	char chBuf[2];
	chBuf[0] = ibc->Ch;
	chBuf[1] = '\0';
	if (ibc->StringMode) {
		switch (ibc->Ch) {
		case '\''://'
		case '\"'://"
		{
			ibc->StringMode = false;
			IBLayer3StrPayload(ibc);
			return;
		}
		}
	}
	else {
		switch (ibc->Pfx) {
		case OP_Name: {
			switch (ibc->Ch) {
			case '[': {
				//IBStrClear(&ibc->ArrayIndexExprStr);
				IBStr*aT=NULL;
				IBVectorPush(&ibc->ArrayIndexExprsVec, &aT);
				IBStrInit(aT);
				IBLayer3Push(ibc, OP_ModeArrayIndexExpr, false);
				return;
			}
			case ']':{
				if(IBLayer3GetMode(ibc)==OP_ModeArrayIndexExpr)
					IBLayer3Pop(ibc);
				return;
			}
			case ' ':
			case '.':
			IBCASE_0THRU9
			IBCASE_aTHRUz
			IBCASE_ATHRUZ
				break;
			default:
				ErrF(OP_NotAllowed, "character \'%c\' not allowed in name", ibc->Ch);
			}
			break;
		}
		/*case OP_Value: {
			switch (ibc->Ch) {
			case '@': {
				IBLayer3Pop(ibc);
				IBLayer3Prefix(ibc);
				return;
			}
			}
			break;
		}*/
		}
		switch (ibc->Ch) {
		case '\t': return;
		case ' ': {
			if (ibc->StrAllowSpace) break;
			else {
				IBLayer3StrPayload(ibc);
				return;
			}
		}
		case '^': {
			if (*(bool*)IBVectorTop(&ibc->StrReadPtrsStack)) {
				switch (ibc->Pointer) {
				case OP_NotSet:
					DbgPuts("Got pointer\n");
					ibc->Pointer = OP_Pointer;
					break;
				case OP_Pointer:
					DbgPuts("Got double pointer\n");
					ibc->Pointer = OP_DoublePointer;
					break;
				case OP_DoublePointer:
					DbgPuts("Got tripple pointer\n");
					ibc->Pointer = OP_TripplePointer;
					break;
				case OP_TripplePointer:
					Err(OP_ErrQuadriplePointersNOT_ALLOWED, "");
					break;
				}
				return;
			}
		}
		}
	}
	StrConcat(ibc->Str, IBLayer3STR_MAX, chBuf);
}
IBTask* IBLayer3GetTask(IBLayer3* ibc){
	IBTask* ret= (IBTask*)IBVectorTop(&ibc->TaskStack);
	//if (!ret)Err(OP_Error, "no task in stack");
	if (ret) {
		assert(ret->type >= 0);
	}
	return ret;
}
IBOp IBLayer3GetMode(IBLayer3* ibc){
	IBOp ret=OP_Null;
	IBOp* p=(IBOp*)IBVectorTop(&ibc->ModeStack);
	if (p)ret = *p;
	return ret;
}
IBExpects* IBTaskGetExpTop(IBTask* t){
	IBExpects* ret;
	assert(t);
	ret = (IBExpects*)IBVectorTop(&t->expStack);
	assert(ret);
	return ret;
}
IBVector* IBTaskGetExpPfxsTop(IBTask* t){
	IBExpects* ret;
	assert(t);
	ret = (IBExpects*)IBVectorTop(&t->expStack);
	assert(ret);
	return &ret->pfxs;
}
IBVector* IBTaskGetExpNameOPsTop(IBTask* t){
	IBExpects* ret;
	assert(t);
	ret = (IBExpects*)IBVectorTop(&t->expStack);
	assert(ret);
	return &ret->nameOps;
}
IBOp IBStrToBool(IBLayer3* ibc, char* str){
	if(!strcmp(str, IB_TRUESTR)) return OP_True;
	if(!strcmp(str, IBFALSESTR)) return OP_False;
	return OP_Unknown;
}
IBOp IBJudgeTypeOfStrValue(IBLayer3* ibc, char* str) {
	int numbers = 0;
	int letters = 0;
	int periods = 0;
	int sl = 0;
	int i;
	char lc = '\0';
	IBOp ret=OP_Unknown;
	assert(str);
	sl = strlen(str);
	if (!sl) return OP_EmptyStr;
	for (i = 0; i < sl; i++) {
		char c = str[i];
		switch (c) {
		case '.': { periods++; break; }
		IBCASE_0THRU9 { numbers++; break; }
		IBCASE_aTHRUz
		IBCASE_ATHRUZ { letters++; break; }
		}
	}
	lc=str[sl-1];
	if (!strcmp(str, IB_TRUESTR)
		|| !strcmp(str, IBFALSESTR)) return OP_Bool;
	if (numbers > letters && periods == 1 && letters <= 1) ret = OP_FloatingPoint;
	if(ret==OP_FloatingPoint && lc=='f') return OP_Float;
	else if (ret==OP_FloatingPoint && lc == 'd') return OP_Double;
	if (letters && numbers) ret = OP_String;
	else if (numbers) ret = OP_Number;
	return ret;
}
void IBLayer3StrPayload(IBLayer3* ibc){
	IBVal strVal;
	IBTask *t;
	IBTask*tParent=NULL;
	int tabCount = IBLayer3GetTabCount(ibc);
	IBCodeBlock* cb = IBLayer3CodeBlocksTop(ibc);
	IBObj* o;
	IBOp valType = IBJudgeTypeOfStrValue(ibc, ibc->Str);
	strVal.i64 = 0;
	t=IBLayer3GetTask(ibc);
	o=IBLayer3GetObj(ibc);
	if(ibc->TaskStack.elemCount >= 2){
		tParent=IBVectorGet(&ibc->TaskStack,ibc->TaskStack.elemCount - 2);
	}
	switch (valType) {
	case OP_Bool: {
		IBOp boolCheck = IBStrToBool(ibc, ibc->Str);
		switch (boolCheck) {
		case OP_True: {
			strVal.boolean = true;
			break;
		}
		case OP_False: {
			strVal.boolean = false;
			break;
		}
		IBCASE_UNIMP
		}
		break;
	}
	case OP_String: break;
	case OP_Number: {
		strVal.i64 = atoll(ibc->Str);
		break;
	}
	case OP_Double: {
		strVal.d64 = atof(ibc->Str);
		break;
	}
	case OP_Float: {
		strVal.f32 = atof(ibc->Str);
		break;
	}
	//IBCASE_UNIMP
	/*default: {
		strVal.i32 = atoi(ibc->Str);
		break;
	}*/
	}

	//if(ibc->Pfx==OP_Op) ibc->LastNameOp = ibc->NameOp;
	ibc->NameOp = IBGetOpFromNameList(ibc->Str, OP_NameOps);
	IBPushColor(IBFgGREEN);
	DbgPuts("StrPayload: ");
	IBPushColor(IBBgWHITE);
	DbgFmt("%s", ibc->Str);
	IBPopColor();
	IBPopColor();
	DbgPuts("\n");
	top:
	switch (ibc->Pfx) {
	/* . PFXDOT */ case OP_Dot: {
		switch(t->type){
		case OP_NeedExpression: {
			IBOp type=OP_Unknown;
			IBObj* o=0;
			IBObj*vneO=IBLayer3FindStackObjRev(ibc,OP_VarNeedExpr);
			IBTask*e2nt=IBLayer3FindTaskUnderIndex(ibc, -1, OP_ExprToName, 3);
			/*IBTask* etn = IBLayer3FindTaskUnderIndex(ibc, -1, OP_ExprToName, 3);*/
			IBTypeInfo*st=0;
			IBTypeInfo*ti=0;
			if(vneO){
				IBLayer3FindType(ibc,vneO->str,&ti);
				if(ti){
					type=ti->type;
					IBTypeInfoFindMember(ti,ibc->Str,&st);
				}
			}else if(e2nt/*&&ibc->DefiningMethods*/){
				IBObj*no=0;
				TaskFindWorkingObj(e2nt, OP_ActOnName, &no);
				assert(no);
				if(no){
					if (IB_STARTS_WITH_SELFDOT(no->name)) {
						char* rn = IB_SELFDOTLESS_NTSP(no->name);
						assert(rn && (*rn));
						/*IBTask*mt=IBLayer3FindTaskUnderIndex(ibc,-1,OP_MethodsWantContent, 100);
						assert(mt);*/
						assert(ibc->_methodsStructName);
						IBLayer3FindType(ibc, ibc->_methodsStructName, &ti);
						if (ti) {
							type = ti->type;
							IBTypeInfoFindMember(ti, rn, &st);
						}
					}else{
						IBNameInfo* ni = IBLayer3SearchNameInfo(ibc, no->name);
						if(ni->ti){
							ti=ni->ti;
							type=ti->type;
							IBTypeInfoFindMember(ti, ibc->Str, &st);
						}
					}
				}
				
			}
			if(type==OP_Unknown || !st)
				Err(OP_Error, "context not found");
			IBLayer3PushObj(ibc, &o);
			assert(st);
			if(st){
				if(st->type==OP_StructVar){
				}
				else IBObjSetType(o, st->type);
			}
			assert(ti);
			if(ti)IBObjSetStr(o, ti->name.start);
			IBObjSetName(o, ibc->Str);
			if (st&&st->type == OP_StructVar) {
				IBOp t = st->structVarType->type;
				assert(st->structVarType);
				IBObjSetType(o, t == OP_Enum ? OP_EnumVal : t);
				IBOverwriteStr(&o->str, st->structVarType->name.start);
			}
			IBLayer3PopObj(ibc, true, &o);
			break;
		}
		IBCASE_UNIMP
		}
		break;
	}
	/* ' PFXSINGLEQUOTE */ case OP_SingleQuote: {
		switch (t->type) {
		case OP_VarWantValue: {
			o->var.val.c8=ibc->Str[0];
			o->var.valSet=true;
			break;
		}
		IBCASE_UNIMP
		}
		break;
	}
	//prefix infer a-z A-Z
	/* a PFXazAZ */ case OP_Letter_azAZ: {
		IBTypeInfo*ti=0;
		IBLayer3FindType(ibc,ibc->Str,&ti);
		//IBOp dataType = IBGetOpFromNameList(ibc->Str, OP_DataTypes);
		//if (dataType != OP_Unknown)
		if(ti){
			ibc->Pfx = OP_VarType;
			DbgPuts("infered vartype\n");
			goto top;
		}
		else if (!strcmp(ibc->Str, IBFALSESTR) ||
				!strcmp(ibc->Str, IB_TRUESTR)) {
			ibc->Pfx = OP_Value;
			DbgPuts("infered bool Value\n");
			goto top;
		}
		else {
			IBOp nameOp = IBGetOpFromNameList(ibc->Str, OP_NameOps);
			switch (nameOp) {
			case OP_Unknown:
			case OP_Void:
				break;
			default: {
				ibc->Pfx = OP_Op;
				DbgPuts("infered OP\n");
				goto top;
			}
			}
		}
		Err(OP_Error, "Couldn't infer this input");
	}
	/* _ PFXUNDERSCORE */ case OP_Underscore: {
		switch (ibc->NameOp) {
		case OP_EmptyStr: {
			IBLayer3Done(ibc);
			break;
		}
		IBCASE_UNIMP
		}
		break;
	}
	/* * PFXMULTIPLY */ case OP_Multiply:
	/* / PFXDIVIDE */ case OP_Divide:
	/* - PFXSUBTRACT */ case OP_Subtract: {
		bool fall = true;
		switch(t->type){
		case OP_RootTask:{
			switch (ibc->NameOp) {
			case OP_Exclaim: {
				fall=false;
				IBLayer3Push(ibc, OP_ModeCCompTimeMacroPaste, true);
				break;
			}
			IBCASE_UNIMP
			}
			break;
		}
		default:{
			switch (ibc->NameOp) {
			case OP_GreaterThan: {
				IBExpects* exp;
				fall = false;
				SetObjType(o, OP_FuncNeedsRetValType);
				IBLayer3PushExpects(ibc, &exp);
				IBExpectsInit(exp, "P", OP_VarType);
				break;
			}
			IBCASE_UNIMP
			}
		}
		}
		if (!fall) break;
	}
	/* | PFXOR */ case OP_Or: {
		switch(ibc->NameOp){
		case OP_Or:{

			break;
		}
		}
	}
	/* + PFXADD */ case OP_Add: {
		switch (ibc->NameOp) {
		case OP_Or:{

		}
		case OP_EmptyStr: {
			switch (t->type) {
			case OP_NeedExpression: {
				IBObj* o;
				IBLayer3PushObj(ibc, &o);
				IBObjSetType(o, ibc->Pfx);
				IBLayer3PopObj(ibc, true, &o);
				break;
			}
			}
			break;
		}
		IBCASE_UNIMP
		}
		break;
	}
	/* < PFXLESSTHAN */ case OP_LessThan: {
		switch (t->type) {
		case OP_VarWantValue: {
			switch(ibc->NameOp){
			case OP_Subtract:{
				o=IBLayer3GetObj(ibc);
				SetTaskType(t, OP_VarNeedExpr);
				SetObjType(o, OP_VarNeedExpr);
				IBLayer3PushTask(ibc, OP_NeedExpression, NULL, &t);
				break;
			}
			IBCASE_UNIMP
			}
			break;
		}
		case OP_ActOnName: {
			switch (ibc->NameOp)
			{
			case OP_Subtract: {
				IBObj* o;
				IBObj*methodsCtxO=IBLayer3FindStackObjRev(ibc,OP_Methods);
				IBTypeInfo*methodsNameTypeInfo=0,*foundSub=0;
				if(methodsCtxO){
					IBLayer3FindType(ibc,methodsCtxO->name,&methodsNameTypeInfo);
				}
				IBNameInfo* ni;
				IBExpects* exp=NULL;
				o = IBLayer3FindWorkingObjRev(ibc, OP_ActOnName);
				assert(o);
				assert(o->name[0] != '\0');
				if (methodsNameTypeInfo&&IB_STARTS_WITH_SELFDOT(o->name)) {
					IBTypeInfo*subT=0;
					int sidx=0;
					while(subT=IBVectorIterNext(&methodsNameTypeInfo->members,&sidx)){
						if(!strcmp(subT->name.start,o->name + 5)){
							foundSub=subT;
							break;
						}
					}
				}
				ni = IBLayer3SearchNameInfo(ibc, o->name);
				if (!ni && !foundSub) {
					ErrF(OP_NotFound, "%s wasn't found", o->name);
				}
				//assert(ni);
				//assert(type != OP_NotFound);
				SetTaskType(t, OP_ExprToName);
				IBLayer3PushTask(ibc, OP_NeedExpression, NULL, &t);
				if(foundSub){
					t->exprData.finalVartype=foundSub->StructVar.type;
				}
				else if(ni){
					t->exprData.finalVartype = ni->type;
				}
				break;
			}
			IBCASE_UNIMP
			}
			break;
		}
		IBCASE_UNIMP
		}
		break;
	}
	/* ! PFXEXCLAIM */ case OP_Exclaim: {
		switch (t->type) {
		IBCASE_BLOCKWANTCODE
		{
			//IBTask* t;
			//IBLayer3PushTask(ibc, OP_CodeBlockCallFunc, NULL, NULL);
			IBLayer3PushTask(ibc, OP_CallFunc, NULL, NULL);
			//break;
		}
		case OP_ActOnNameEquals: {
			IBExpects* exp;
			IBObj* o;
			//IBTask* t;
			IBLayer3PushObj(ibc, &o);
			IBObjSetStr(o, ibc->Str);
			IBObjSetType(o, OP_Call);
			IBLayer3PushTask(ibc, OP_CallWantArgs, &exp, &t);
			IBLayer3PopObj(ibc, true, &o);
			IBLayer3PushObj(ibc, &o);
			SetObjType(o, OP_ArgNeedValue);
			IBExpectsInit(exp, "PPPP",
				OP_Name, OP_Value, OP_String, OP_LineEnd);
			break;
		}
		IBCASE_UNIMP
		}
		break;
	}
	/* " PFXSTRING */ case OP_String: {
		switch(t->type) {
		case OP_VarWantValue: {
			IBOverwriteStr(&o->var.valStrLiteral, ibc->Str);
			o->var.valSet=true;
			break;
		}
        case OP_CIncNeedFilePath:{
            IBStrAppendFmt(&ibc->CIncludesStr,
                "#include \"%s\"\n", ibc->Str);
            IBLayer3PopTask(ibc, &t, false);
            break;
        }
		case OP_RunArgsNeedArgsStr: {
			IBStrAppendFmt(&ibc->RunArguments, "%s", ibc->Str);
			IBLayer3PopTask(ibc, &t, false);
			break;
		}
		case OP_FuncNeedRetVal: {
			IBObj* o;
			int idx;
			idx = 0;
			while (o = (IBObj*)IBVectorIterNext(&t->working, &idx)) {
				if (o->type == OP_FuncSigComplete) {
					DbgPuts("Finishing func got ret value\n");
					IBOverwriteStr(&o->func.retValStr, ibc->Str);
					o->func.retValType = OP_String;
					o->func.retVal.i32 = 0;
					o->func.retTYPE = OP_String;
					PopExpects();
					SetTaskType(t, OP_Func);
					IBLayer3FinishTask(ibc);
					break;
				}
			}
			break;
		}
		case OP_BlockReturnNeedValue: {
			switch (o->type) {
			case OP_BlockReturnNeedValue: {
				IBObjSetStr(o, ibc->Str);
				o->valType = OP_String;
				IBLayer3FinishTask(ibc);
				break;
			}
			}
			break;
		}
		case OP_dbgAssertWantArgs: {
			switch (IBGetOpFromNameList(ibc->Str, OP_dbgAssert)) {
			case OP_TaskType:{
				break;
			}
			}
			break;
		}
		case OP_CPrintfHaveFmtStr:{
			IBObj *o;
			IBLayer3PushObj(ibc, &o);
			IBObjSetType(o, OP_String);
			o->var.type=OP_String;
			IBObjSetStr(o, ibc->Str);
			IBLayer3PopObj(ibc, true, NULL);
			break;
		}
		IBCASE_BLOCKWANTCODE
		{ /*printf*/
			IBExpects *ap;
			IBObj* o;
			IBLayer3PushTask(ibc, OP_CPrintfHaveFmtStr, &ap, NULL);
			IBExpectsInit(ap, "1PPPPP", "expected fmt args or line end",
				OP_Exclaim, OP_Value, OP_Name, OP_String, OP_LineEnd);
			IBLayer3PushObj(ibc, &o);
			IBObjSetStr(o, ibc->Str);
			IBObjSetType(o, OP_CPrintfFmtStr);
			IBLayer3PopObj(ibc, true, NULL);
			break;
		}
		IBCASE_UNIMP
		}
		break;
	}
	/* = PFXVALUE */ case OP_Value: {
		switch (t->type) {
		case OP_ForNeedStartInitVal: {
			switch (t->type) {
				IBCASE_UNIMP
			}
			break;
		}
		case OP_NeedExpression: {
			IBObj* o=NULL;
			IBLayer3PushObj(ibc, &o);
			IBObjSetType(o, OP_Value);
			o->valType=OP_Value;
			o->val=strVal;
			IBLayer3PopObj(ibc, true, &o);
			break;
		}
		case OP_ActOnName: {
			switch (ibc->NameOp) {
			case OP_EmptyStr: {
				IBExpects* exp;
				SetTaskType(t, OP_ActOnNameEquals);
				IBLayer3ReplaceExpects(ibc, &exp);
				IBExpectsInit(exp, "P", OP_Exclaim);
				break;
			}
			IBCASE_UNIMP
			}
			break;
		}
		case OP_VarWantValue: {
			switch (o->type) {
			case OP_VarWantValue: {
				o->var.val = strVal;
				o->var.valSet = true;
				switch (o->var.type) {
				case OP_Bool: {
					if (valType != OP_Bool)
						Err(OP_YouCantUseThatHere, "wrong value for bool variable");
					break;
				}
				case OP_f32: {
					if (valType != OP_Float)
						Err(OP_YouCantUseThatHere, "wrong value for float variable");
					break;
				}
				case OP_d64: {
					if (valType != OP_Double)
						Err(OP_YouCantUseThatHere, "wrong value for double variable");
					break;
				}
				case OP_u8:
				case OP_c8: /*{
					if (o->var.mod == OP_Pointer) {
						assert(valType == OP_String);
						break;
					}
				}*/
				case OP_u16:
				case OP_u64:
				case OP_u32:
				case OP_i8:
				case OP_i16:
				case OP_i64:
				case OP_i32: {
					if(valType!=OP_Number)
						Err(OP_YouCantUseThatHere, "wrong value for number variable");
					break;
				}
				IBCASE_UNIMP
				}
				SetObjType(o, OP_VarComplete);
				IBLayer3PopObj(ibc, true, &o);
				IBLayer3FinishTask(ibc);
				break;
			}
			IBCASE_UNIMP
			}
			break;
		}
		case OP_BlockReturnNeedValue: {
			switch (o->type) {
			case OP_BlockReturnNeedValue: {
				o->val = strVal;
				o->valType = OP_Value;
				IBLayer3FinishTask(ibc);
				break;
			}
			}
			break;
		}
		case OP_CallWantArgs: {
			switch (o->type) {
			case OP_ArgNeedValue: {
				o->val = strVal;
				IBObjSetType(o, OP_Arg);
				o->valType = OP_Value;
				IBLayer3PopObj(ibc, true, &o);
				IBLayer3PushObj(ibc, &o);
				SetObjType(o, OP_ArgNeedValue);
				break;
			}
			}
			break;
		}
		case OP_BuildingIf: {
			switch (o->type) {
			case OP_IfNeedLVal: {
				o->ifO.lvVal = strVal;
				o->ifO.lvTYPE = OP_Value;
				o->ifO.lvDataType = OP_i32;
				SetObjType(o, OP_IfNeedMidOP);
				break;
			}
			case OP_IfNeedRVal: {
				IBExpects* exp;
				o->ifO.rvVal = strVal;
				o->ifO.rvTYPE = OP_Value;
				o->ifO.rvDataType = OP_i32;
				SetObjType(o, OP_IfFinished);
				SetTaskType(t, OP_IfFinished);
				IBLayer3ReplaceExpects(ibc, &exp);
				IBExpectsInit(exp, "P", OP_LineEnd);
				break;
			}
			default: {
				Err(OP_Error, "Unimplemented If task context");
			}
			}
			break;
		}
		case OP_CPrintfHaveFmtStr:{
			IBObj *o;
			IBLayer3PushObj(ibc, &o);
			o->val = strVal;
			IBObjSetType(o, OP_Value);
			o->var.type = OP_i32;/*for now*/
			IBLayer3PopObj(ibc, true, NULL);
			break;
		}
		case OP_FuncNeedRetVal: {
			IBObj* o;
			int idx;
			idx = 0;
			while (o = (IBObj*)IBVectorIterNext(&t->working,&idx)) {
				if (o->type == OP_FuncSigComplete) {
					IBOp valType = IBJudgeTypeOfStrValue(ibc, ibc->Str);
					switch (o->func.retValType) {
					case OP_c8: {

					}
					case OP_u8:
					case OP_u16:
					case OP_u32:
					case OP_u64:
					case OP_i8:
					case OP_i16:
					case OP_i32:
					case OP_i64: {
						if (valType != OP_Number)
							Err(OP_YouCantUseThatHere,
								"wrong return value type for this function");
						break;
					}
					IBCASE_UNIMP
					}
					DbgPuts("Finishing func got ret value\n");
					o->func.retVal = IBLayer3StrToVal(ibc, ibc->Str, o->func.retValType);
					o->func.retTYPE = OP_Value;
					PopExpects();
					SetTaskType(t, OP_Func);
					IBLayer3FinishTask(ibc);
					break;
				}
			}
			break;
		}
		IBCASE_UNIMP
		}
		break;
	}
	/* % PFXVARTYPE */ case OP_VarType: {
		IBTypeInfo*ti=0;
		IBLayer3FindType(ibc,ibc->Str,&ti);
		if(!ti)
			ErrF(OP_NotFound, "type %s doesn't exist", ibc->Str);
		switch (t->type) {
		case OP_FuncHasName: {
			switch (o->type) {
			case OP_FuncHasName: {
				IBObj* o;
				IBExpects* exp;
				IBLayer3PushObj(ibc, &o);
				SetObjType(o, OP_FuncArgNameless);
				o->arg.type = ibc->NameOp;
				o->arg.mod = ibc->Pointer;
				if (o->arg.type == OP_c8 && o->arg.mod == OP_Pointer)
					o->arg.type = OP_String;
				IBLayer3PushExpects(ibc, &exp);
				IBExpectsInit(exp, "1P", "expected func arg name", OP_Name);
				break;
			}
			case OP_FuncNeedsRetValType: {
				o->func.retValType = ibc->NameOp;
				o->func.retTypeMod = ibc->Pointer;
				if (o->func.retValType == OP_Unknown)
					Err(OP_Error, "unknown return type");
				if (o->func.retValType == OP_String) {
					o->func.retValType = OP_c8;
					o->func.retTypeMod = OP_Pointer;
				}
				SetObjType(o, OP_FuncSigComplete);
				break;
			}
			}
			break;
		}
		case OP_StructWantRepr: {
			SetTaskType(t, OP_StructWantContent);
			PopExpects();
			break;
		}
		case OP_StructWantContent:
		case OP_RootTask:
		IBCASE_BLOCKWANTCODE
		{
			IBObj* o;
			IBExpects* exp;
			IBTask* t;
			IBLayer3PushObj(ibc, &o);
			o->var.type = ibc->NameOp;
			IBLayer3FindType(ibc,ibc->Str,&o->var.ti);
			IBObjSetStr(o, ibc->Str);
			o->var.mod = ibc->Pointer;
			o->var.valSet = false;
			SetObjType(o, OP_VarNeedName);
			IBLayer3PushTask(ibc, OP_VarNeedName, &exp, &t);
			IBExpectsInit(exp, "1P", "expected variable name", OP_Name);
			break;
		}
		IBCASE_UNIMP
		}
		break;
	}
	/* $ PFXNAME */ case OP_Name: {
		switch (ibc->Str[0]){
		IBCASE_0THRU9 {
			Err(OP_YouCantUseThatHere,
				"can't use number as first character of name!");
		}
		}
		switch(t->type) {
		case OP_MethodsNeedName:{
			IBTypeInfo*ti=0;
			IBLayer3FindType(ibc,ibc->Str,&ti);
			if(!ti){
				ErrF(OP_NotFound,"%s isnt found\n",ibc->Str);
			} else {
				IBObj*mo=0;
				IBExpects*exp=0;
				assert(ti);
				IBLayer3PushObj(ibc,&mo);
				SetObjType(mo,OP_Methods);
				IBObjSetName(mo,ibc->Str);
				//IBLayer3PopObj(ibc,true,NULL);
				mo=0;
				SetTaskType(t,OP_MethodsWantContent);
				IBLayer3ReplaceExpects(ibc,&exp);
				IBExpectsInit(exp,"PPN",OP_Op,OP_Underscore,OP_Func);
				ibc->DefiningMethods=1;
				IBOverwriteStr(&ibc->_methodsStructName,ibc->Str);
			}
			break;
		}
		case OP_ForNeedStartValName: {
			IBNameInfo* ni=NULL;
			IBExpects* exp = NULL;
			IBOp ar = IBNameInfoDBAdd(ibc, &cb->localVariables, ibc->Str, OP_Name, &ni);
			//should never happen
			if(ar==OP_AlreadyExists)
				ErrF(OP_AlreadyExists, "name %s already in use", ibc->Str);
			assert(ni);
			assert(o->type == OP_For);
			IBOverwriteStr(&o->forO.startName, ibc->Str);
			SetTaskType(t, OP_ForNeedStartInitVal);
			IBLayer3ReplaceExpects(ibc, &exp);
			IBExpectsInit(exp, "P", OP_Value);
			break;
		}
		case OP_EnumWantContent: {
			IBObj* o;
			IBLayer3PushObj(ibc, &o);
			IBObjSetType(o, OP_EnumName);
			IBObjSetName(o, ibc->Str);
			IBLayer3PopObj(ibc, true, &o);
			break;
		}
		case OP_EnumNeedName: {
			IBExpects* exp;
			SetTaskType(t, OP_EnumWantContent);
			IBLayer3ReplaceExpects(ibc, &exp);
			IBExpectsInit(exp, "PP", OP_Name, OP_Underscore);
			assert(o->type == OP_Enum);
			IBObjSetName(o, ibc->Str);
			IBLayer3PopObj(ibc, true, &o);
			break;
		}
		case OP_NeedExpression: {
			IBObj* o;
			IBLayer3PushObj(ibc, &o);
			IBObjSetType(o, OP_Name);
			IBObjSetName(o, ibc->Str);
			o->modifier = ibc->Pointer;
			IBLayer3PopObj(ibc, true, &o);
			break;
		}
		case OP_StructInitNeedName: {
			IBExpects* exp;
			assert(o->type == OP_StructInit);
			IBObjSetName(o, ibc->Str);
			IBLayer3PushTask(ibc, OP_SubtaskArgs, &exp, &t);
			IBExpectsInit(exp, "PPP", OP_Name, OP_String, OP_Value);
			break;
		}
		case OP_FuncHasName: {
			switch (o->type) {
			case OP_FuncArgNameless: {
				SetObjType(o, OP_FuncArgComplete);
				PopExpects();
				IBObjSetName(IBLayer3GetObj(ibc), ibc->Str);
				IBNameInfoDBAdd(ibc, &cb->localVariables, ibc->Str,
					o->arg.type, NULL);
				IBLayer3PopObj(ibc, true, NULL);
				break;
			}
			}
			break;
		}
		case OP_FuncNeedName: {
			switch (o->type) {
			case OP_Func: {
				IBExpects* exp;
				SetObjType(o, OP_FuncHasName);
				SetTaskType(t, OP_FuncHasName);
				IBLayer3PushExpects(ibc, &exp);
				IBExpectsInit(exp, "PPPPN",
					OP_VarType, OP_Op, OP_LineEnd, OP_Subtract, OP_Return);
				IBObjSetName(o, ibc->Str);
				break;
			}
			}
			break; }
		case OP_FuncNeedRetVal: {
			IBObj* o;
			int idx;
			idx = 0;
			while (o = (IBObj*)IBVectorIterNext(&t->working, &idx)) {
				if (o->type == OP_FuncSigComplete) {
					//IBOp nameType = IBNameInfoDBFindType(&ibc->NameTypeCtx, ibc->Str);
					IBNameInfo* ni = IBLayer3SearchNameInfo(ibc, ibc->Str);
					assert(ni);
					if(ni->type==OP_NotFound)
						Err(OP_NotFound, "variable name not found");
					if (o->func.retValType != ni->type && o->func.retValType != ibc->Varcast)
						Err(OP_Error, "variable doesn't match function return type\n");
					DbgPuts("Finishing func got ret value as name\n");
					IBOverwriteStr(&o->func.retStr, ibc->Str);
					o->func.retTYPE = OP_Name;
					o->func.retValVarcast=ibc->Varcast;
					PopExpects();
					SetTaskType(t, OP_Func);
					IBLayer3FinishTask(ibc);
					break;
				}
			}
			break;
		}
		case OP_CallWantArgs: {
			if (o->type == OP_ArgNeedValue) {
				IBObjSetStr(o, ibc->Str);
				IBObjSetType(o, OP_Arg);
				o->valType = OP_Name;
				IBLayer3PopObj(ibc, true, &o);
				IBLayer3PushObj(ibc, &o);
				SetObjType(o, OP_ArgNeedValue);
			} else Err(OP_Error, "wrong obj type");
			break;
		}
		case OP_BlockReturnNeedValue: {
			switch (o->type) {
			case OP_BlockReturnNeedValue: {
				IBObjSetStr(o, ibc->Str);
				o->valType = OP_Name;
				IBLayer3FinishTask(ibc);
				break;
			}
			}
			break;
		}
		case OP_BuildingIf: {
			IBExpects* exp=NULL;
			switch (o->type) {
			case OP_IfNeedLVal: {
				IBOverwriteStr(&o->ifO.lvName, ibc->Str);
				IBNameInfo* ni = IBLayer3SearchNameInfo(ibc, o->ifO.lvName);
				if (!ni) {
					ErrF(OP_NotFound, "%s wasn't found", o->ifO.lvName);
				}
				assert(ni);
				o->ifO.lvTYPE = OP_Name;
				SetObjType(o, OP_IfNeedMidOP);
				IBLayer3ReplaceExpects(ibc, &exp);
				IBExpectsInit(exp, "PNNNNNN",
					OP_Op, OP_Equals, OP_LessThan,
					OP_GreaterThan, OP_LessThanOrEquals,
					OP_GreaterThanOrEquals, OP_NotEquals);
				break;
			}
			case OP_IfNeedRVal: {
				IBExpects* exp;
				IBOverwriteStr(&o->ifO.rvName, ibc->Str);
				IBNameInfo* ni = IBLayer3SearchNameInfo(ibc, o->ifO.rvName);
				if (!ni) {
					ErrF(OP_NotFound, "%s wasn't found", o->ifO.rvName);
				}
				assert(ni);
				o->ifO.rvTYPE = OP_Name;
				SetObjType(o, OP_IfFinished);
				SetTaskType(t, OP_IfFinished);
				IBLayer3ReplaceExpects(ibc, &exp);
				IBExpectsInit(exp, "P", OP_LineEnd);
				break;
			}
			}
			break;
		}
		case OP_StructWantName: {
			IBObj* o;
			IBLayer3PushObj(ibc, &o);
			assert(ibc->Str[0]!='\0');
			IBObjSetName(o, ibc->Str);
			IBObjSetType(o, OP_Struct);
			SetTaskType(t, OP_StructWantRepr);
			PopExpects();
			break;
		}
		case OP_UseNeedStr: {
			IBOp lib;
			lib = ibc->NameOp;
			switch (lib) {
			case OP_UseStrSysLib: {
				IBPushColor(IBFgIntensity | IBFgYELLOW | IBBgBROWN);
				DbgPuts("Inputting system lib code to compiler\n");
				IBPopColor();
				IBStrAppendCStr(&ibc->CHeader_Funcs,
					"/* System Lib Header */\n");
				assert(!ibc->InputStr);
				ibc->InputStr = SysLibCodeStr;
				ibc->Line++;
				ibc->Column = 1;
				break;
			}
			default: {

				break;
			}
			}
			IBLayer3PopTask(ibc,NULL,false);
			IBLayer3PopCodeBlock(ibc, false, &cb);//?????
			break;
		}
		case OP_SpaceNeedName: {
			IBObj* o;
			IBLayer3PushObj(ibc, &o);
			assert(ibc->Str[0]!='\0');
			IBObjSetName(o, ibc->Str);
			IBObjSetType(o, OP_Space);
			IBLayer3PopObj(ibc, true, &o);
			SetTaskType(t, OP_SpaceHasName);
			IBLayer3FinishTask(ibc);
			break;
		}
		case OP_CPrintfHaveFmtStr: {
			IBObj*o=0;
			IBStr*at=0;
			int idx=0;
			IBLayer3PushObj(ibc, &o);
			IBObjSetName(o, ibc->Str);
			IBObjSetType(o, OP_Name);
			while(at=IBVectorIterNext(&ibc->ArrayIndexExprsVec,&idx)){
				IBStr*ss=0;
				IBVectorPush(&o->arg.arrIndexExprs,&ss);
				IBStrInit(ss);
				IBStrAppend(ss,at);
			}
			IBLayer3PopObj(ibc, true, NULL);
			break;
		}
		IBCASE_BLOCKWANTCODE {
			IBTask* t=NULL;
			IBExpects* exp=NULL;
			IBObj* o = NULL;
			IBLayer3PushTask(ibc, OP_ActOnName, &exp, &t);
			IBLayer3PushObj(ibc, &o);
			IBObjSetType(o, OP_ActOnName);
			/*if (StrStartsWith(ibc->Str, "self.") && strlen(ibc->Str) > 5) {
				IBStr ns;
				IBStrInit(&ns);
				IBStrAppendCStr(&ns, "self->");
				IBStrAppendFmt(&ns, "%s", &ibc->Str[5]);
				ibc->Str[0]='\0';
				strncpy(ibc->Str, ns.start, IBLayer3STR_MAX);
			}*/
			IBObjSetName(o, ibc->Str);
			IBLayer3PopObj(ibc, true, &o);
			IBExpectsInit(exp, "PP", OP_Value, OP_LessThan);
			break;
		}
		case OP_VarNeedName: {
			switch (o->type) {
			case OP_VarNeedName: {
				IBExpects* exp;
				IBNameInfo* ni = NULL;
				IBOp rc=0;
				IBObjSetName(o, ibc->Str);
				rc = IBNameInfoDBAdd(ibc,
					(tParent && tParent->type == OP_RootTask) ?
						&ibc->GlobalVariables : &cb->localVariables,
					ibc->Str, o->var.type, &ni);
				ni->type = o->var.type;
				ni->ti=o->var.ti;
				/*if(rc == OP_AlreadyExists)
					Err(OP_Error, "name already in use");
				assert(rc == OP_OK);*/
				SetObjType(o, OP_VarWantValue);
				SetTaskType(t, OP_VarWantValue);
				IBLayer3ReplaceExpects(ibc, &exp);
				IBExpectsInit(exp, "1PPPPP",
					"",
					OP_Value, OP_LessThan, OP_String, OP_SingleQuote, OP_LineEnd);
				break;
			}
			IBCASE_UNIMP
			}
			break;
		}
		}
		break;
	}
	/* @ PFXOP */ case OP_Op: {
		bool expected;
		expected = IBLayer3IsNameOpExpected(ibc, ibc->NameOp);
		if(!expected)Err(OP_ErrUnexpectedNameOP, "unexpected nameOP");
		switch (ibc->NameOp) {
        case OP_CInclude:{
            IBTask*t=NULL;
			IBExpects*exp=NULL;
			IBLayer3PushTask(ibc, OP_CIncNeedFilePath, &exp, &t);
			IBExpectsInit(exp, "P", OP_String);
            break;
        }
		case OP_RunArguments: {
			IBTask*t=NULL;
			IBExpects*exp=NULL;
			IBLayer3PushTask(ibc, OP_RunArgsNeedArgsStr, &exp, &t);
			IBExpectsInit(exp, "P", OP_String);
			break;
		}
		case OP_As: {

			break;
		}
		case OP_Repr: {
			switch (t->type) {
			case OP_StructWantRepr: {
				IBExpects* exp;
				//SetTaskType(t, OP_StructWantContent);
				IBLayer3PushExpects(ibc, &exp);
				IBExpectsInit(exp, "1P", "expected vartype (%)", OP_VarType);
				break;
			}
			default: Err(OP_Error, "can't use repr here");
			}
			break;
		}
		case OP_Flags: {
			switch (t->type) {
			case OP_RootTask: {
				IBExpects* ap;
				IBLayer3PushObj(ibc, &o);
				IBObjSetType(o, OP_Enum);
				o->enumO.flags = true;
				IBLayer3PushTask(ibc, OP_EnumNeedName, &ap, NULL);
				IBExpectsInit(ap, "1P", "expected enum name", OP_Name);
				break;
			}
			IBCASE_UNIMP
			}
			break;
		}
		case OP_Enum: {
			switch (t->type) {
			case OP_RootTask: {
				IBExpects* ap;
				IBLayer3PushObj(ibc, &o);
				IBObjSetType(o, OP_Enum);
				o->enumO.flags=false;
				IBLayer3PushTask(ibc, OP_EnumNeedName, &ap, NULL);
				IBExpectsInit(ap, "1P", "expected enum name", OP_Name);
				break;
			}
			IBCASE_UNIMP
			}
			break;
		}
		case OP_Space: {
			switch (t->type) {
			case OP_RootTask: {
				IBExpects* ap;
				IBLayer3PushTask(ibc, OP_SpaceNeedName, &ap, NULL);
				IBExpectsInit(ap, "1P", "expected space name", OP_Name);
				break;
			}
			default: Err(OP_Error, "can't use space here");
			}
			break;
		}
		case OP_Methods:{
			IBTask*t=0;
			IBExpects*exp=0;
			IBLayer3PushTask(ibc,OP_MethodsNeedName,&exp,&t);
			IBExpectsInit(exp,"P",OP_Name);
			break;
		}
		case OP_Struct: {
			switch (t->type) {
			case OP_RootTask: {
				IBExpects* ap;
				//onion
				assert(!ibc->DefiningStruct);
				ibc->DefiningStruct=1;
				IBLayer3PushTask(ibc, OP_StructWantName, &ap, NULL);
				IBExpectsInit(ap, "PP", OP_Op, OP_Underscore);
				IBLayer3PushExpects(ibc, &ap);
				IBExpectsInit(ap, "PPN", OP_Op, OP_LineEnd, OP_Repr);
				IBLayer3PushExpects(ibc, &ap);
				IBExpectsInit(ap, "1P", "expected name", OP_Name);
				IBLayer3PushCodeBlock(ibc, &cb);
				break;
			}
			IBCASE_BLOCKWANTCODE
			{
				IBTask* t;
				IBExpects* exp;
				IBObj* o;
				IBLayer3PushTask(ibc, OP_StructInitNeedName, &exp, &t);
				IBExpectsInit(exp, "1P", "expected thing name", OP_Name);
				IBLayer3PushObj(ibc, &o);
				IBObjSetType(o, OP_StructInit);
				break;
			}
			default: Err(OP_Error, "can't use thing here");
			}
			break;
		}
		case OP_dbgAssert: {
			IBExpects* ap;
			IBLayer3PushTask(ibc, OP_dbgAssertWantArgs, &ap, NULL);
			IBExpectsInit(ap, "1P", "expected string", OP_String);
			break;
		}
		case OP_Fall: {
			switch (t->type) {
			case OP_CaseWantCode: {
				IBObj* o = IBLayer3FindWorkingObj(ibc, OP_TableCase);
				assert(o);
				o->table.fallthru = true;
				break;
			}
			}
			break;
		}
		case OP_Case: {
			switch(t->type) {
			case OP_CaseWantCode: {
				IBTask* t = NULL;
				IBLayer3FinishTask(ibc);
				t= IBLayer3GetTask(ibc);
				assert(t);
				assert(t->type==OP_TableWantCase);
			}
			case OP_TableWantCase: {
				IBTask* t=NULL;
				IBExpects* exp=NULL;
				IBObj* o = NULL;
				IBLayer3PushObj(ibc, &o);
				SetObjType(o, OP_TableCase);
				IBLayer3PushTask(ibc, OP_TableCaseNeedExpr, NULL, NULL);
				IBLayer3PopObj(ibc, true, &o);
				IBLayer3PushTask(ibc, OP_NeedExpression, &exp, &t);
				IBExpectsInit(exp, "e", OP_Op);
				break;
			}
			IBCASE_UNIMP
			}
			break;
		}
		case OP_Table: {
			switch (t->type) {
			IBCASE_BLOCKWANTCODE
			{
				IBTask* t=NULL;
				IBLayer3PushTask(ibc, OP_TableNeedExpr, NULL, NULL);
				IBLayer3PushTask(ibc, OP_NeedExpression, NULL, &t);
				break;
			}
			}
			break;
		}
		case OP_dbgBreak: {
			/*__debugbreak();*/
			/*ibc->TaskStack;*/
			break;
		}
		case OP_Imaginary: {
			ibc->Imaginary = true;
			IBPushColor(IBFgMAGENTA);
			DbgPuts("[GOT IMAGINARY]");
			IBPopColor();
			DbgPuts("\n");
			break;
		}
		case OP_Return: {
			switch (t->type) {
			case OP_FuncHasName: {
				switch (o->type) {
				case OP_FuncHasName: {
					IBExpects* exp;
					SetObjType(o, OP_FuncNeedsRetValType);
					IBLayer3PushExpects(ibc, &exp);
					IBExpectsInit(exp, "P", OP_VarType);
					break;
				}
				default: {
					Err(OP_Error, "wrong obj type");
					break;
				}
				}
				break;
			}
			IBCASE_BLOCKWANTCODE
			{
				IBTask* t;
				IBExpects* exp;
				IBObj* o;
				IBLayer3PushObj(ibc, &o);
				IBObjSetType(o, OP_BlockReturnNeedValue);
				IBLayer3PushTask(ibc, OP_BlockReturnNeedValue, &exp, &t);
				IBExpectsInit(exp, "PPP", OP_Value, OP_String, OP_Name);
				break;
			}
			}
			break;
		}
		case OP_Func: { //func1
			IBExpects* ap;
			IBObj* o;
			IBTask* t;
			t = IBLayer3GetTask(ibc);
			assert(t);
			IBLayer3PushObj(ibc, &o);
			o->func.thingTask = t->type == OP_StructWantContent ? t : NULL;
			IBLayer3PushTask(ibc, OP_FuncNeedName, &ap, NULL);
			IBExpectsInit(ap, "1P", "expected function name", OP_Name);
			o->type = ibc->NameOp;
			o->privacy = ibc->Privacy;
			o->func.retTYPE = OP_NotSet;
			o->func.retValType = OP_Void;
			o->func.retTypeMod = OP_NotSet;
			IBLayer3PushCodeBlock(ibc, &cb);
			break;
		}
		case OP_Public:
		case OP_Private: {
			ibc->Privacy = ibc->NameOp;
			break;
		}
		case OP_Use: {
			IBExpects* ap;
			IBLayer3PushTask(ibc, OP_UseNeedStr, &ap, NULL);
			IBExpectsInit(ap, "1P", "expected @use $name", OP_Name);
			IBLayer3PushCodeBlock(ibc, &cb);
			break;
		}
		/*case OP_Set: {
			switch (t->type) {
			IBCASE_BLOCKWANTCODE
			{
				IBExpects* ap;
				IBObj* o;
				IBLayer3PushObj(ibc, &o);
				ObjSetType(o, OP_Set);
				IBLayer3PushTask(ibc, OP_SetNeedName, &ap, NULL);
				ExpectsInit(ap, "1P", "expected @set $name", OP_Name);
				break;
			}
			default: {
				Err(OP_Error, "can't use set here");
				break;
			}
			}
			break;
		}*/
		case OP_LessThanOrEquals: //@lteq
		case OP_GreaterThanOrEquals: //@gteq
		case OP_LessThan: //@lt
		case OP_GreaterThan: //@gt
		case OP_NotEquals: //@neq
		case OP_Equals: { //@eq
			switch (t->type) {
			case OP_BuildingIf: {
				IBExpects* exp;
				switch (o->type) {
				case OP_IfNeedMidOP: {
					o->ifO.midOp = ibc->NameOp;
					IBObjSetType(o, OP_IfNeedRVal);
					IBLayer3ReplaceExpects(ibc, &exp);
					IBExpectsInit(exp, "PP", OP_Name, OP_Value);
					break;
				}
				default: {
					Err(OP_Error, "Unimplemented If task context");
				}
				}
				break;
			}
			default: Err(OP_Error, "can't use comparison operator here");
			}
			break;
		}
		case OP_Else: {
			switch (t->type) {
			case OP_IfBlockWantCode: {
				IBTask* t;
				IBExpects* exp;
				IBLayer3FinishTask(ibc);
				IBLayer3PushTask(ibc, OP_BlockWantCode, &exp, &t);
				IBExpectsInit(exp, "c", OP_Null);
				cb = IBLayer3CodeBlocksTop(ibc);
				IBStrAppendCh(&cb->code, '\t', tabCount - 1);
				IBStrAppendCStr(&cb->code, "else ");
				IBLayer3PushCodeBlock(ibc, &cb);
				break;
			}
			}
			break;
		}
		case OP_ElseIf: {
			switch (t->type) {
			case OP_IfBlockWantCode: {
				IBLayer3FinishTask(ibc);
				ibc->LastNameOp = ibc->NameOp;
				IBLayer3PushTask(ibc, OP_ElseIf, NULL, &t);
				break;
			}
			default: {
				Err(OP_YouCantUseThatHere, "You may only use @elif in @if blocks!!!");
				break;
			}
			}
		}
		case OP_If: {
			switch (t->type) {
			case OP_ElseIf: {
				cb = IBLayer3CodeBlocksTop(ibc);
				IBStrAppendCh(&cb->code, '\t', tabCount - 1);
				IBStrAppendCStr(&cb->code, "else ");
				IBLayer3PopTask(ibc, &t,false);
			}
			IBCASE_BLOCKWANTCODE {
				IBTask* nt;
				IBExpects* nexp;
				IBObj* o;
				IBLayer3PushObj(ibc, &o);
				IBObjSetType(o, OP_IfNeedLVal);
				IBLayer3PushTask(ibc, OP_BuildingIf, &nexp, &nt);
				IBExpectsInit(nexp, "1PP", "expected lval",
					OP_Value, OP_Name/*, OP_String*/);
				break;
			}
			default: {
				Err(OP_Error, "Unimplemented If task context");
				break;
			}
			}
			break;
		}
		case OP_Loop: {
			switch (t->type) {
				IBCASE_BLOCKWANTCODE
				{
					IBExpects* exp;
					IBTask* t;
					IBLayer3PushTask(ibc, OP_LoopBlockWantCode, &exp, &t);
					IBExpectsInit(exp, "Nc", OP_Break);
					IBLayer3PushCodeBlock(ibc, &cb);
					break;
				}
				IBCASE_UNIMP
			}
			break;
		}
		case OP_Break: {
			cb = IBLayer3CodeBlocksTop(ibc);
			IBStrAppendCh(&cb->code, '\t', tabCount);
			IBStrAppendCStr(&cb->code, "break;\n");
			break;
		}
		case OP_For: {
			/*
			switch (t->type) {
				IBCASE_BLOCKWANTCODE
				{
					break;
				}
				IBCASE_UNIMP
			}
			*/
			switch (t->type) {
				IBCASE_BLOCKWANTCODE
				{
					IBObj* o;
					IBTask* t;
					IBExpects* exp;
					IBLayer3PushObj(ibc, &o);
					IBObjSetType(o, OP_For);
					IBLayer3PushTask(ibc, OP_ForNeedStartValName, &exp, &t);
					IBExpectsInit(exp, "P", OP_Name);
					IBLayer3PushCodeBlock(ibc, &cb);
					break;
				}
				IBCASE_UNIMP
			}
			break;
		}
		default:
			Err(OP_ErrUnknownOpStr, "");
		}
		break;
	}
	IBCASE_UNIMP
	}
	ibc->Str[0] = '\0';
#ifdef IBDEBUGPRINTS
	PLINE;
	IBPushColor(IBFgMAGENTA);
	printf(" Str payload complete");
	IBPopColor();
	printf("\n");
#endif
	IBLayer3Pop(ibc);
	if(ibc->StrReadPtrsStack.elemCount > 1){
		if (*(bool*)IBVectorTop(&ibc->StrReadPtrsStack))
			ibc->Pointer = OP_NotSet;
		IBVectorPop(&ibc->StrReadPtrsStack, NULL);
	}
	IBVectorClear(&ibc->ArrayIndexExprsVec, IBStrFree);
	ibc->Varcast=OP_Null;
//#define IBOPSTEP
#ifdef IBOPSTEP
	{
		int l = ibc->InputStr ? ibc->LineIS : ibc->Line;
		int c = ibc->InputStr ? ibc->ColumnIS : ibc->Column;
		IBPushColor(IBFgRED);
		DbgFmt("[");
		IBPushColor(IBBgGREEN);
		DbgFmt("%d:%d", l, c);
		IBPopColor();
		DbgFmt("]");
		IBPopColor();
		IBPushColor(IBBgYELLOW);
		DbgFmt("Press enter to ", "");
		IBPushColor(IBBgRED | IBFgYELLOW);
		DbgFmt("step", "");
		IBPopColor();
		IBPopColor();
		DbgFmt("\n", "");
		getchar();
	}
#endif
}
//this is a total joke
void IBLayer3ExplainErr(IBLayer3* ibc, IBOp code) {
	switch (code) {
	case OP_TCC_Error: {
		printf("Fatal internal compiler error");
		break;
	}
	case OP_YouCantUseThatHere: {
		printf("You can't use that here");
		break;
	}
	case OP_FuncNeedRetVal: {
		printf("You forgot to return a value from the function");
		break;
	}
	case OP_ErrUnexpectedNameOP: {
		IBExpects* exp;
		IBTask* t;
		t=IBLayer3GetTask(ibc);
		assert(t);
		exp = IBTaskGetExpTop(t);
		//IBPushColor(IBBgBLUE | IBBgGREEN);
		printf("NameOP \"@%s\" wasn't expected.\nExpected:\n",
			ibc->Str);
		IBExpectsPrint(exp);
		//IBPopColor();
		break;
	}
	case OP_ErrUnknownPfx:
		printf("This prefix \"%c\" is unknown!", ibc->Ch);
		break;
	case OP_ErrUnknownOpStr:
		printf("Unknown OP str @%s\n", ibc->Str);
		break;
	case OP_ErrQuadriplePointersNOT_ALLOWED:
		printf("quadruple pointer is not allowed");
		break;
	case OP_ErrUnexpectedOp:
		printf("Unexpected OP");
		break;
	case OP_ErrNoTask:
		printf("No working task to call done (@@) for");
		break;
	case OP_ErrUnexpectedNextPfx: {
		IBOp* oi;
		IBExpects *ap;
		IBTask *t;
		int idx;
		t =IBLayer3GetTask(ibc);
		ap=IBTaskGetExpTop(t);
		if(ap && t){
			assert(ap->pfxs.elemCount);
			printf("Err: \"%s\" Unexpected next prefix %s. "
				"Pfx idx:%d\nEnforced at line %d. Allowed:",
				ap->pfxErr, IBGetPfxName(ibc->Pfx),
					ap->pfxs.elemCount - 1, ap->lineNumInited);
			idx = 0;
			while (oi = (IBOp*)IBVectorIterNext(&ap->pfxs,&idx)) {
				printf("%s(%d),", IBGetPfxName(*oi), (int)*oi);
			}
		}else{
			printf("No task. Only Op(@) prefix allowed. "
				"Pfx: %s. Allowed pfxs: %s\n",
				IBGetPfxName(ibc->Pfx), IBGetPfxName(OP_Op));
		}
		break;
	}
	case OP_ErrExpectedVariablePfx:
		printf("Expected a variable type to be next.");
		break;
	default:
		printf("Err msg unimplemented for %s", IBGetOpName(code));
	}
#ifdef IBDEBUGPRINTS
	DbgPuts("\nOBJ:");
	ObjPrint(IBLayer3GetObj(ibc));
	//DbgFmt("\n","");
#endif
	printf("\n");
}
void IBSetColor(IBColor col) {
#ifdef _WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), col);
#endif
}
