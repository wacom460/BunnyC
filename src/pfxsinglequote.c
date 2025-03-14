#include "imbored.h"

void IBLayer3PFX_SINGLEQUOTE(IBLayer3* ibc)
{
	IBTask* t = IBLayer3GetTask(ibc);
	IBObj* o = IBLayer3GetObj(ibc);
	switch(t->type)
	{
	case OP_VarWantValue:
	{
		o->var.val.c8 = ibc->Str[0];
		o->var.valSet = true;
		break;
	}
	IBCASE_UNIMPLEMENTED
	}
}