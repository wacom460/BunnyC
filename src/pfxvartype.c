#include "imbored.h"

void IBLayer3PFX_VARTYPE(IBLayer3* ibc)
{
	IBTask* t = IBLayer3GetTask(ibc);
	IBObj* o = IBLayer3GetObj(ibc);
	IBTypeInfo* ti = 0;
	IBLayer3FindType(ibc, ibc->Str, &ti);
	if(!ti)
		ErrF(OP_NotFound, "type %s doesn't exist", ibc->Str);
	switch(t->type)
	{
	case OP_FuncHasName:
	{
		switch(o->type)
		{
		case OP_FuncHasName:
		{
			IBObj* o;
			IBExpects* exp;
			IBLayer3PushObj(ibc, &o);
			SetObjType(o, OP_FuncArgNameless);
			o->arg.type = ibc->NameOp;
			o->arg.mod = ibc->Pointer;
			if(o->arg.type == OP_c8 && o->arg.mod == OP_Pointer)
				o->arg.type = OP_String;
			IBLayer3PushExpects(ibc, &exp);
			IBExpectsInit(exp, "1P", "expected func arg name", OP_Name);
			break;
		}
		case OP_FuncNeedsRetValType:
		{
			o->func.retValType = ibc->NameOp;
			o->func.retTypeMod = ibc->Pointer;
			if(o->func.retValType == OP_Unknown)
				Err(OP_Error, "unknown return type");
			if(o->func.retValType == OP_String)
			{
				o->func.retValType = OP_c8;
				o->func.retTypeMod = OP_Pointer;
			}
			SetObjType(o, OP_FuncSigComplete);
			break;
		}
		}
		break;
	}
	case OP_StructWantRepr:
	{
		SetTaskType(t, OP_StructWantContent);
		PopExpects();
		break;
	}
	case OP_StructWantContent:
	case OP_RootTask:
	IBCASE_BLOCKWANTCODE
	{
		IBObj * o;
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
	IBCASE_UNIMPLEMENTED
	}
}
