#include "imbored.h"

void IBStrInit(IBStr* str) {
	IBASSERT0(str);
	str->start = (char*)malloc(1);
	IBASSERT0(str->start);
	str->end = str->start;
	if (str->start) (*str->start) = '\0';
}

void IBStrInitWithCStr(IBStr* str, char* cstr) {
	IBStrInit(str);
	IBStrAppendCStr(str, cstr);
}

void IBStrInitExt(IBStr* str, char* cstr) {
	str->start = cstr;
	str->end = cstr + strlen(cstr);
}

void IBStrFree(IBStr* str) {
	free(str->start);
}

void IBStrClear(IBStr* str) {
	if (str->start) free(str->start);
	str->start = NULL;
	str->start = malloc(1);
	IBASSERT0(str->start);
	if (str->start) {
		*(str->start) = '\0';
		str->end = str->start;
	}
}

void IBStrReplaceWithCStr(IBStr* str, char* cstr) {
	IBStrClear(str);
	IBStrAppendCStr(str, cstr);
}

void IBStrInitNTStr(IBStr* str, char* nullTerminated) {
	IBASSERT0(nullTerminated);
	IBASSERT0(str);
	IBOverwriteStr(&str->start, nullTerminated);
	str->end = str->start + strlen(nullTerminated);
}

bool IBStrContainsAnyOfChars(IBStr* str, char* chars) {
	char* p;
	for (p = str->start; p < str->end; p++)
		if (strchr(chars, *p)) return true;
	return false;
}

long long int IBStrLen(IBStr* str) {
	size_t len;
	IBASSERT0(str);
	IBASSERT0(str->end);
	IBASSERT0((*(str->end)) == '\0');
	IBASSERT0(str->end >= str->start);
	len = str->end - str->start;
	return len;
}

void IBStrAppendCh(IBStr* str, char ch, int count) {
	char astr[2];
	if (count < 1) return;
	IBASSERT0(str);
	astr[0] = ch;
	astr[1] = '\0';
	while (count--)
		IBStrAppendCStr(str, astr);
}

char* IBStrAppendCStr(IBStr* str, char* with) {
	void* ra;
	size_t len;
	size_t withLen;
	IBASSERT0(str);
	withLen = strlen(with);
	if (!withLen) return str->start;
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

char* IBStrAppend(IBStr* str, IBStr* with) {
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

int IBStrStripFront(IBStr* str, char ch) {
	long long int slen = IBStrLen(str);
	int in = 0;
	char* rep = NULL;
	char ch2 = '\0';
	while (true) {
		ch2 = str->start[in];
		if (ch2 == ch) in++;
		else break;
	}
	if (!in) return 0;
	rep = strdup(str->start + in);
	free(str->start);
	str->start = rep;
	str->end = str->start + (slen - in);
	IBASSERT0(str->end);
	if (str->end) {
		char ec =
			*(str->end);
		IBASSERT0(ec == '\0');
	}
	return in;
}

void IBStrSplitBy(IBStr* str, char splitByCh, IBVector* toVec)
{
	if(!str || !str->start || !str->end || !toVec || toVec->type != OP_IBStr) return;
	long long len = IBStrLen(str);
	IBStr* cur = 0;
	for(long long i = 0; i < len; ++i)
	{
		if(!cur) {
			IBVectorPush(toVec, &cur);
			IBStrInit(cur);
		}

		char ch = str->start[i];

		if(ch == splitByCh)
		{
			cur = 0;
		}
		else {
			IBStrAppendCh(cur, ch, 1);
			//DbgFmt("%c\n", ch);
		}
	}
}

void Val2Str(char* dest, int destSz, IBVal v, IBOp type) {

	switch (type) {
	case OP_Number:
	case OP_Value:
	case OP_u8: { snprintf(dest, destSz, "%u", v.u8);  break; }
	case OP_c8: { snprintf(dest, destSz, "%c", v.c8);  break; }
	case OP_i16: { snprintf(dest, destSz, "%d", v.i16); break; }
	case OP_u16: { snprintf(dest, destSz, "%u", v.u16); break; }
	case OP_i32: { snprintf(dest, destSz, "%d", v.i32); break; }
	case OP_i64: { snprintf(dest, destSz, "%lld", v.i64); break; }
	case OP_u32: { snprintf(dest, destSz, "%u", v.u32); break; }
	case OP_u64: { snprintf(dest, destSz, "%llu", v.u64); break; }
	case OP_Float:
	case OP_f32: { snprintf(dest, destSz, "%f", v.f32); break; }
	case OP_Double:
	case OP_d64: { snprintf(dest, destSz, "%f", v.d64); break; }
	case OP_Bool: {
		snprintf(dest, destSz, "%s", v.boolean > 0 ? "1" : "0");
		break;
	}
	IBCASE_UNIMP_A
	}
}

char* StrConcat(char* dest, int count, char* src) {
	return strcat(dest, src);
}

char StrStartsWith(char* str, char* with) {
	while (*with)
		if (*str++ != *with++) return 0;
	return 1;
}

IBOp IBStrToBool(IBLayer3* ibc, char* str) {
	if (!strcmp(str, IB_TRUESTR)) return OP_True;
	if (!strcmp(str, IBFALSESTR)) return OP_False;
	return OP_Unknown;
}

IBOp IBJudgeTypeOfStrValue(IBLayer3* ibc, char* str) {
	int numbers = 0;
	int letters = 0;
	int periods = 0;
	long long sl = 0;
	int i;
	char lc = '\0';
	IBOp ret = OP_Unknown;
	IBassert(str);
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
	lc = str[sl - 1];
	if (!strcmp(str, IB_TRUESTR)
		|| !strcmp(str, IBFALSESTR)) return OP_Bool;
	/*if (numbers > letters && periods == 1 && letters <= 1) ret = OP_Double;
	if(ret == OP_Double && lc == 'f') return OP_Float;
	else return ret;*/
	if(numbers > letters && periods == 1 && letters <= 1) return OP_Double;
	if (letters && numbers) ret = OP_String;
	else if (numbers) ret = OP_Number;
	return ret;
}

void IBOverwriteStr(char** str, char* with) {
	IBassert(str);
	IBassert(with);
	if (!with) {
		if (*str)free(*str);
		*str = NULL;
		return;
	}
	if (*str) free(*str);
	*str = strdup(with);
	IBassert(*str);
}

IB_DBObj* IB_DBObjNew(IBStr* fileName, int fileLine, int fileColumn,
	IBOp objType, IBStr* objName) {
	IB_DBObj* ret = NULL;
	ret = malloc(sizeof * ret);
	IBassert(ret);
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
		IBassert(!cic);
		IBVectorInit(&ret->children, sizeof(IB_DBObj), OP_DBObj, IBVEC_DEFAULT_SLOTCOUNT);
	}
	return ret;
}

void IB_DBObjFree(IB_DBObj* obj) {
	IBStrFree(&obj->name);
	IBStrFree(&obj->fileName);
	IBVectorFree(&obj->children, IB_DBObjFree);
	free(obj);
}