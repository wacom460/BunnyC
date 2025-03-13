#include "imbored.h"

void IBLayer3PFX_DOT(IBLayer3* ibc)
{
	IBTask* t = IBLayer3GetTask(ibc);
	switch(t->type)
	{
	case OP_NeedExpression:
	{
		IBOp type = OP_Unknown;
		IBObj* o = 0;
		IBObj* vneO = IBLayer3FindStackObjRev(ibc, OP_VarNeedExpr);
		IBTask* e2nt = IBLayer3FindTaskUnderIndex(ibc, -1, OP_ExprToName, 3);
		/*IBTask* etn = IBLayer3FindTaskUnderIndex(ibc, -1, OP_ExprToName, 3);*/
		IBTypeInfo* st = 0;
		IBTypeInfo* ti = 0;
		if(vneO)
		{
			IBLayer3FindType(ibc, vneO->str, &ti);
			if(ti)
			{
				type = ti->type;
				IBTypeInfoFindMember(ti, ibc->Str, &st);
			}
		}
		else if(e2nt /*&& ibc->DefiningMethods*/)
		{
			IBObj* no = 0;
			TaskFindWorkingObj(e2nt, OP_ActOnName, &no);
			IBassert(no);
			if(no)
			{
				if(IB_STARTS_WITH_SELFDOT(no->name))
				{
					char* rn = IB_SELFDOTLESS_NTSP(no->name);
					IBassert(rn && (*rn));
					/*IBTask* mt = IBLayer3FindTaskUnderIndex(ibc, -1, OP_MethodsWantContent, 100);
					IBassert(mt);*/
					IBassert(ibc->_methodsStructName);
					IBLayer3FindType(ibc, ibc->_methodsStructName, &ti);
					if(ti)
					{
						type = ti->type;
						IBASSERT0(ti->members.elemCount);
						IBTypeInfoFindMember(ti, rn, &st);
					}
				}
				else
				{
					IBNameInfo* ni = IBLayer3SearchNameInfo(ibc, no->name);
					if(!ni) DB;
					if(ni && ni->ti)
					{
						ti = ni->ti;
						type = ti->type;
						//IBASSERT0(ti->members.elemCount);
						IBTypeInfoFindMember(ti, ibc->Str, &st);
						if(!st) st = ti->memberVarType;
					}
				}
			}
		}
		if(type == OP_Unknown || !st)
			Err(OP_Error, "context not found");
		IBLayer3PushObj(ibc, &o);
		IBassert(st);
		if(st)
		{
			if(st->type == OP_StructVar)
			{

			}
			else
			{
				IBObjSetType(o, st->type);
			}
		}
		IBassert(ti);
		if(ti)
			IBObjSetStr(o, ti->name.start);
		IBObjSetName(o, ibc->Str);
		if(st && st->type == OP_Enum)
		{
			IBObjSetType(o, OP_EnumVal);
			IBOverwriteStr(&o->str, st->name.start);
		}
		IBLayer3PopObj(ibc, true, &o);
		break;
	}
	IBCASE_UNIMPLEMENTED
	}
}