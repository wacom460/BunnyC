#include "imbored.h"

void IBLayer3PFX_LESSTHAN(IBLayer3* ibc)
{
	IBTask* t = IBLayer3GetTask(ibc);
	IBObj* o = IBLayer3GetObj(ibc);
	switch(t->type)
	{
	case OP_ActOnName:
	{
		switch(ibc->NameOp)
		{
		case OP_Subtract:
		{
			SetTaskType(t, OP_ExprToName);
			IBLayer3PushObj(ibc, &o);
			SetObjType(o, OP_ExprToName);
			IBLayer3PushTask(ibc, OP_NeedExpression, NULL, &t);
			break;
		}
		IBCASE_UNIMPLEMENTED
		}
		break;
	}
	case OP_VarWantValue:
	{
		switch(ibc->NameOp)
		{
		case OP_Subtract:
		{
			o = IBLayer3GetObj(ibc);
			SetTaskType(t, OP_VarNeedExpr);
			SetObjType(o, OP_VarNeedExpr);
			IBLayer3PushTask(ibc, OP_NeedExpression, NULL, &t);
			break;
		}
		IBCASE_UNIMPLEMENTED
		}
		break;
	}
	case OP_BuildingIf:
	{
		IBExpects* exp;
		switch(o->type)
		{
		case OP_IfNeedMidOP:
		{
			switch(ibc->NameOp)
			{
			case OP_Equals:
			case OP_EmptyStr:
			{
				o->ifO.midOp = ibc->NameOp == OP_Equals ? OP_LessThanOrEquals : ibc->Pfx;
				IBObjSetType(o, OP_IfNeedRVal);
				IBLayer3ReplaceExpects(ibc, &exp);
				IBExpectsInit(exp, "PP", OP_Name, OP_Value);
				break;
			}
			}
			break;
		}
		default:
		{
			Err(OP_Error, "Unimplemented If task context");
		}
		}
		break;
	}
	IBCASE_UNIMPLEMENTED
	}
}
