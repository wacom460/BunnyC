#include "imbored.h"

void ObjInit(IBObj* o)
{
	memset(o, 0, sizeof * o);
	o->DataTypeIdentifier = OP_Obj;
	o->type = OP_NotSet;
	o->modifier = OP_NotSet;
	o->name = NULL;
	o->str = NULL;
	o->val.i32 = 0;
	o->valType = OP_Unknown;
	o->func.retStr = NULL;
	o->arg.type = OP_Null;
	IBVectorInit(&o->arg.arrIndexExprs, sizeof(IBStr), OP_IBStr, IBVEC_DEFAULT_SLOTCOUNT);
	o->arg.mod = OP_NotSet;
	o->ifO.lvName = NULL;
	o->ifO.rvName = NULL;
}

void ObjFree(IBObj* o)
{
	IBassert(o);
	IBVectorFree(&o->arg.arrIndexExprs, IBStrFree);
	if (o->ifO.lvName) free(o->ifO.lvName);
	if (o->ifO.rvName) free(o->ifO.rvName);
	if (o->forO.startName) free(o->forO.startName);
	if (o->func.retValStr) free(o->func.retValStr);
	if (o->func.retStr) free(o->func.retStr);
	if (o->var.valStrLiteral)
		free(o->var.valStrLiteral);
	if (o->name) free(o->name);
	if (o->str) free(o->str);
}

void _IBObjSetType(IBObj* obj, IBOp type)
{
	DbgFmt(" obj type: %s(%d) -> %s(%d)\n",
		IBGetOpName(obj->type), (int)obj->type, IBGetOpName(type), (int)type);
	obj->type = type;
}

void IBObjSetMod(IBObj* obj, IBOp mod)
{
	DbgFmt("obj mod: %s(%d) -> %s(%d)\n",
		IBGetOpName(obj->modifier), (int)obj->modifier, IBGetOpName(mod), (int)mod);
	obj->modifier = mod;
}

void _IBObjSetName(IBObj* obj, char* name)
{
	IBassert(obj);
	DbgFmt(" obj name: %s -> %s\n", obj->name, name);
	IBOverwriteStr(&obj->name, name);
}

void IBObjSetStr(IBObj* obj, char* Str)
{
	DbgFmt("obj str: %s -> %s\n", obj->str, Str);
	IBOverwriteStr(&obj->str, Str);
}

void IBObjCopy(IBObj* dst, IBObj* src)
{
	IBassert(dst && src);
	memcpy(dst, src, sizeof(IBObj));
	dst->name = NULL;
	dst->str = NULL;
	if (src->name) IBOverwriteStr(&dst->name, src->name);
	if (src->str) IBOverwriteStr(&dst->str, src->str);
}

void ObjPrint(IBObj* obj)
{
	IBassert(obj);
	if (obj)
	{
		printf("[");
		if (obj->type != OP_NotSet)
		{
			printf("Type:%s(%d),", IBGetOpName(obj->type), (int)obj->type);
		}
		if (obj->name)printf("Name:%s,", obj->name);
		if (obj->str)printf("Str:%s,", obj->str);
		if (obj->modifier != OP_NotSet)
		{
			printf("Mod:%s,", IBGetOpName(obj->modifier));
		}
		printf("Val:%d", obj->val.i32);
		printf("]");
	}
}
