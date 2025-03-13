#include "imbored.h"

void IBDictKeyInit(IBDictKey* key, IBDictKeyDef def)
{
	key->DataTypeIdentifier = OP_IBDictKey;
	key->type = def.type;
	switch (def.type)
	{
	case IBDictDataType_Int:
	{
		key->key.num = def.num;
		break;
	}
	case IBDictDataType_String:
	{
		strncpy(key->key.data, def.str, IBDICTKEY_KEYSIZE);
		break;
	}
	}
	IBVectorInit(&key->children, sizeof(IBDictKey), OP_IBDictKey, IBVEC_DEFAULT_SLOTCOUNT);
}

void IBDictKeyInitRoot(IBDictKey* key)
{
	memset(key, 0, sizeof(IBDictKey));
	key->type = IBDictDataType_RootKey;
	IBVectorInit(&key->children, sizeof(IBDictKey), OP_IBDictKey, IBVEC_DEFAULT_SLOTCOUNT);
}

void IBDictKeyFree(IBDictKey* key)
{
	int idx = 0;
	IBDictKey* sk = NULL;
	IBassert(key);
	while (sk = IBVectorIterNext(&key->children, &idx))
		IBDictKeyFree(sk);
	IBVectorFreeSimple(&key->children);
}

IBDictKey* IBDictKeyFind(IBDictKey* rootKey, IBVector* keyDefStack)
{
	IBDictKeyDef* dp = NULL;
	IBDictKey* key;
	int idx = 0;
	IBassert(rootKey);
	key = rootKey;
	IBassert(keyDefStack->elemCount);
	while (dp = IBVectorIterNext(keyDefStack, &idx))
	{
		IBDictKey* ok = IBDictKeyFindChild(key, *dp);
		if (ok)
		{
			key = ok;
			break;
		}
		else
		{
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

IBDictKey* IBDictKeyFindChild(IBDictKey* key, IBDictKeyDef def)
{
	int idx = 0;
	IBDictKey* sk = NULL;
	IBassert(key);
	while (sk = IBVectorIterNext(&key->children, &idx))
	{
		if (sk->type == def.type)
		{
			switch (def.type)
			{
			case IBDictDataType_Int:
			{
				if (sk->key.num == def.num) return sk;
				break;
			}
			case IBDictDataType_String:
			{
				if (strcmp(sk->key.data, def.str) == 0) return sk;
				break;
			}
			IBCASE_UNIMP_A
			}
		}
	}
	return NULL;
}

void IBDictKeyPrint(IBDictKey* key, int* childDepth)
{
	int idx = 0;
	int tc = 0;
	IBDictKey* sk = NULL;
	IBassert(key);
	IBassert(childDepth);
	tc = *childDepth;
	while (tc--) printf("\t");
	printf("[%d] ", *childDepth);
	switch (key->type)
	{
	case IBDictDataType_RootKey:
	{
		printf("Root ");
		break;
	}
	case IBDictDataType_VoidPtr:
	{
		printf("Pointer: %p", key->val.data);
		break;
	}
	case IBDictDataType_Int:
	{
		printf("Int: %d", key->val.num);
		break;
	}
	case IBDictDataType_String:
	{
		printf("Str: %s", key->val.data);
		break;
	}
	}
	printf(" K:\n");
	++*childDepth;
	while (sk = IBVectorIterNext(&key->children, &idx))
	{
		IBDictKeyPrint(sk, childDepth);
	}
	-- * childDepth;
}

typedef enum
{
	IBDictManipAction_Unknown = 0,
	IBDictManipAction_DataIn,
	IBDictManipAction_DataOut,
	IBDictManipAction_StrIn,
	IBDictManipAction_StrOut,
	IBDictManipAction_IntIn,
	IBDictManipAction_IntOut,
	IBDictManipAction_KeyPtrOut,
} IBDictManipAction;

IBDictKey* IBDictManip(IBDictKey* rootKey, char* fmt, ...)
{
	va_list args;
	int i = 0;
	IBDictKey* dk = NULL;
	IBVector keyStack;//IBDictKeyDef
	void* inPtr = NULL;
	void** outPtr = NULL;
	char* inStr = NULL, ** outStr = NULL;
	int inInt = 0;
	int* outIntPtr = NULL;
	IBDictKey** outKeyPtr = NULL;
	size_t count = 0;
	IBDictManipAction action = IBDictManipAction_Unknown;
	IBVectorInit(&keyStack, sizeof(IBDictKeyDef), OP_IBDictKeyDef, IBVEC_DEFAULT_SLOTCOUNT);
	va_start(args, fmt);
	for (i = 0; i < strlen(fmt); i++)
	{
		char ch = fmt[i];
		switch (ch)
		{
		case 's':
		{//string
			IBDictKeyDef* kd = NULL;
			IBVectorPush(&keyStack, &kd);
			kd->DataTypeIdentifier = OP_IBDictKeyDef;
			kd->type = IBDictDataType_String;
			kd->key = NULL;
			kd->str = va_arg(args, char*);
			break;
		}
		case 'd':
		{//int
			IBDictKeyDef* kd = NULL;
			IBVectorPush(&keyStack, &kd);
			kd->DataTypeIdentifier = OP_IBDictKeyDef;
			kd->type = IBDictDataType_Int;
			kd->key = NULL;
			kd->num = va_arg(args, int);
			break;
		}
		case 'i':
		{//in ptr
			IBassert(action == IBDictManipAction_Unknown);
			inPtr = va_arg(args, void*);
			action = IBDictManipAction_DataOut;
			break;
		}
		case 'o':
		{//out ptr
			IBassert(action == IBDictManipAction_Unknown);
			outPtr = va_arg(args, void**);
			action = IBDictManipAction_DataIn;
			break;
		}
		case 'c':
		{//count
			count = va_arg(args, size_t);
			break;
		}
		case 'z': //in char* (null terminated)
		{
			IBassert(action == IBDictManipAction_Unknown);
			inStr = va_arg(args, char*);
			action = IBDictManipAction_StrIn;
			break;
		}
		case 'x': //in int
		{
			IBassert(action == IBDictManipAction_Unknown);
			inInt = va_arg(args, int);
			action = IBDictManipAction_IntIn;
			break;
		}
		case 'j': //out new char* (null terminated)
		{
			IBassert(action == IBDictManipAction_Unknown);
			outStr = va_arg(args, char**);
			action = IBDictManipAction_StrOut;
			break;
		}
		case 'k': //out int*
		{
			IBassert(action == IBDictManipAction_Unknown);
			outIntPtr = va_arg(args, int*);
			action = IBDictManipAction_IntOut;
			break;
		}
		case 'g': //out IBDictKey*
		{
			IBassert(action == IBDictManipAction_Unknown);
			outKeyPtr = va_arg(args, IBDictKey**);
			action = IBDictManipAction_KeyPtrOut;
			break;
		}
		IBCASE_UNIMP_A
		}
	}
	dk = IBDictKeyFind(rootKey, &keyStack);
	IBassert(dk);
	switch (action)
	{
	case IBDictManipAction_DataIn:
	{
		IBassert(count > 0 && count <= IBDICTKEY_MAXDATASIZE);
		memcpy_s(dk->val.data, IBDICTKEY_MAXDATASIZE, inPtr, count);
		break;
	}
	case IBDictManipAction_DataOut:
	{
		IBassert(outPtr);
		IBassert(count > 0 && count <= IBDICTKEY_MAXDATASIZE);
		if (outPtr)
			memcpy(outPtr, dk->val.data, count);
		break;
	}
	case IBDictManipAction_StrIn:
	{
		strncpy(dk->val.data, inStr, IBDICTKEY_MAXDATASIZE);
		break;
	}
	case IBDictManipAction_StrOut:
	{
		*outStr = strdup(dk->val.data);
		break;
	}
	case IBDictManipAction_IntIn:
	{
		dk->val.num = inInt;
		break;
	}
	case IBDictManipAction_IntOut:
	{
		(*outIntPtr) = dk->val.num;
		break;
	}
	case IBDictManipAction_KeyPtrOut:
	{
		*outKeyPtr = dk;
		break;
	}
	IBCASE_UNIMP_A
	}
	IBVectorFreeSimple(&keyStack);
	va_end(args);
	return dk;
}

IBDictKey* IBDictGet(IBDictKey* rootKey, char* keyPath)
{
	IBVector keyStack;
	IBVectorInit(&keyStack, sizeof(IBDictKeyDef), OP_IBDictKeyDef, IBVEC_DEFAULT_SLOTCOUNT);
	IBDictKeyDef* kd;
	IBVectorPush(&keyStack, &kd);
	IBDictKey* dk = IBDictKeyFind(rootKey, &keyStack);
	return NULL;
}

void IBDictTest()
{
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
		IBassert(out->val.num == 1);
	}

	{
		IBDictKey rk;
		IBDictKey* key = NULL;
		IBDictKeyInitRoot(&rk);
		IBDictManip(&rk, "sssx", "variables", "globals", "color", 10);
		IBDictManip(&rk, "sssg", "variables", "globals", "color", &key);
		IBassert(key);
		IBassert(key->val.num == 10);
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
