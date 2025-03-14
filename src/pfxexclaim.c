#include "imbored.h"

void IBLayer3PFX_EXCLAIM(IBLayer3* ibc)
{
	IBTask* t = IBLayer3GetTask(ibc);
	switch(t->type)
	{
	IBCASE_BLOCKWANTCODE
	{
		IBLayer3PushTask(ibc, OP_CallFunc, NULL, NULL);
	}
	case OP_ActOnNameEquals:
	{
		IBExpects* exp;
		IBObj* o;
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
	IBCASE_UNIMPLEMENTED
	}
}
