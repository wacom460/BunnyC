#include "imbored.h"

void IBLayer3PFX_GREATERTHAN(IBLayer3* ibc)
{
	IBTask* t = IBLayer3GetTask(ibc);
	IBObj* o = IBLayer3GetObj(ibc);
	switch(t->type)
	{
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
				o->ifO.midOp = ibc->NameOp == OP_Equals ? OP_GreaterThanOrEquals : ibc->Pfx;
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
