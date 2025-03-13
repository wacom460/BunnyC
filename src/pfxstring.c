#include "imbored.h"

void IBLayer3PFX_STRING(IBLayer3* ibc)
{
	IBTask* t = IBLayer3GetTask(ibc);
	IBObj* o = IBLayer3GetObj(ibc);
	switch(t->type)
	{
	case OP_VarWantValue:
	{
		IBOverwriteStr(&o->var.valStrLiteral, ibc->Str);
		o->var.valSet = true;
		break;
	}
	case OP_CIncNeedFilePath:
	{
		IBStrAppendFmt(&ibc->CIncludesStr,
			"#include \"%s\"\n", ibc->Str);
		IBLayer3PopTask(ibc, &t, false);
		break;
	}
	case OP_RunArgsNeedArgsStr:
	{
		IBStrAppendFmt(&ibc->RunArguments, "%s", ibc->Str);
		IBLayer3PopTask(ibc, &t, false);
		break;
	}
	case OP_FuncNeedRetVal:
	{
		IBObj* o;
		int idx;
		idx = 0;
		while(o = (IBObj*) IBVectorIterNext(&t->working, &idx))
		{
			if(o->type == OP_FuncSigComplete)
			{
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
	case OP_BlockReturnNeedValue:
	{
		switch(o->type)
		{
		case OP_BlockReturnNeedValue:
		{
			IBObjSetStr(o, ibc->Str);
			o->valType = OP_String;
			IBLayer3FinishTask(ibc);
			break;
		}
		}
		break;
	}
	case OP_dbgAssertWantArgs:
	{
		switch(IBGetOpFromNameList(ibc->Str, OP_dbgAssert))
		{
		case OP_TaskType:
		{
			break;
		}
		}
		break;
	}
	case OP_CPrintfHaveFmtStr:
	{
		IBObj* o;
		IBLayer3PushObj(ibc, &o);
		IBObjSetType(o, OP_String);
		o->var.type = OP_String;
		IBObjSetStr(o, ibc->Str);
		IBLayer3PopObj(ibc, true, NULL);
		break;
	}
	IBCASE_BLOCKWANTCODE /*printf*/
	{
		IBExpects * ap;
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
	IBCASE_UNIMPLEMENTED
	}
}
