#include "imbored.h"

void IBLayer3PFX_OP(IBLayer3* ibc)
{
	IBTask* t = IBLayer3GetTask(ibc);
	IBObj* o = IBLayer3GetObj(ibc);
	IBCodeBlock* cb = IBLayer3CodeBlocksTop(ibc);
	int tabCount = IBLayer3GetTabCount(ibc);
	bool expected;
	expected = IBLayer3IsNameOpExpected(ibc, ibc->NameOp);
	if(!expected)
		Err(OP_ErrUnexpectedNameOP, "unexpected nameOP");
	switch(ibc->NameOp)
	{
	case OP_CInclude:
	{
		IBTask* t = NULL;
		IBExpects* exp = NULL;
		IBLayer3PushTask(ibc, OP_CIncNeedFilePath, &exp, &t);
		IBExpectsInit(exp, "P", OP_String);
		break;
	}
	case OP_RunArguments:
	{
		IBTask* t = NULL;
		IBExpects* exp = NULL;
		IBLayer3PushTask(ibc, OP_RunArgsNeedArgsStr, &exp, &t);
		IBExpectsInit(exp, "P", OP_String);
		break;
	}
	case OP_As:
	{

		break;
	}
	case OP_Repr:
	{
		switch(t->type)
		{
		case OP_StructWantRepr:
		{
			IBExpects* exp;
			//SetTaskType(t, OP_StructWantContent);
			IBLayer3PushExpects(ibc, &exp);
			IBExpectsInit(exp, "1P", "expected vartype (%)", OP_VarType);
			break;
		}
		default: Err(OP_Error, "can't use repr here");
		}
		break;
	}
	case OP_Flags:
	{
		switch(t->type)
		{
		case OP_RootTask:
		{
			IBExpects* ap;
			IBLayer3PushObj(ibc, &o);
			IBObjSetType(o, OP_Enum);
			o->enumO.flags = true;
			IBLayer3PushTask(ibc, OP_EnumNeedName, &ap, NULL);
			IBExpectsInit(ap, "1P", "expected enum name", OP_Name);
			break;
		}
		IBCASE_UNIMPLEMENTED
		}
		break;
	}
	case OP_Enum:
	{
		switch(t->type)
		{
		case OP_RootTask:
		{
			IBExpects* ap;
			IBLayer3PushObj(ibc, &o);
			IBObjSetType(o, OP_Enum);
			o->enumO.flags = false;
			IBLayer3PushTask(ibc, OP_EnumNeedName, &ap, NULL);
			IBExpectsInit(ap, "1P", "expected enum name", OP_Name);
			break;
		}
		IBCASE_UNIMPLEMENTED
		}
		break;
	}
	case OP_Methods:
	{
		IBTask* t = 0;
		IBExpects* exp = 0;
		IBLayer3PushTask(ibc, OP_MethodsNeedName, &exp, &t);
		IBExpectsInit(exp, "P", OP_Name);
		break;
	}
	case OP_Struct:
	{
		switch(t->type)
		{
		case OP_RootTask:
		{
			IBExpects* ap;
			//onion
			IBassert(!ibc->DefiningStruct);
			ibc->DefiningStruct = 1;
			IBLayer3PushTask(ibc, OP_StructWantName, &ap, NULL);
			IBExpectsInit(ap, "PP", OP_Op, OP_Underscore);
			IBLayer3PushExpects(ibc, &ap);
			IBExpectsInit(ap, "PPN", OP_Op, OP_LineEnd, OP_Repr);
			IBLayer3PushExpects(ibc, &ap);
			IBExpectsInit(ap, "1P", "expected name", OP_Name);
			IBLayer3PushCodeBlock(ibc, &cb);
			break;
		}
		IBCASE_BLOCKWANTCODE
		{
			IBTask * t;
			IBExpects* exp;
			IBObj* o;
			IBLayer3PushTask(ibc, OP_StructInitNeedName, &exp, &t);
			IBExpectsInit(exp, "1P", "expected struct name", OP_Name);
			IBLayer3PushObj(ibc, &o);
			IBObjSetType(o, OP_StructInit);
			break;
		}
		default: Err(OP_Error, "can't use struct here");
		}
		break;
	}
	case OP_dbgAssert:
	{
		IBExpects* ap;
		IBLayer3PushTask(ibc, OP_dbgAssertWantArgs, &ap, NULL);
		IBExpectsInit(ap, "1P", "expected string", OP_String);
		break;
	}
	case OP_Fall:
	{
		switch(t->type)
		{
		case OP_CaseWantCode:
		{
			IBObj* o = IBLayer3FindWorkingObj(ibc, OP_TableCase);
			IBassert(o);
			o->table.fallthru = true;
			break;
		}
		}
		break;
	}
	case OP_Case:
	{
		switch(t->type)
		{
		case OP_CaseWantCode:
		{
			IBTask* t = NULL;
			IBLayer3FinishTask(ibc);
			t = IBLayer3GetTask(ibc);
			IBassert(t);
			IBassert(t->type == OP_TableWantCase);
		}
		case OP_TableWantCase:
		{
			IBTask* t = NULL;
			IBExpects* exp = NULL;
			IBObj* o = NULL;
			IBLayer3PushObj(ibc, &o);
			SetObjType(o, OP_TableCase);
			IBLayer3PushTask(ibc, OP_TableCaseNeedExpr, NULL, NULL);
			IBLayer3PopObj(ibc, true, &o);
			IBLayer3PushTask(ibc, OP_NeedExpression, &exp, &t);
			IBExpectsInit(exp, "e", OP_Op);
			break;
		}
		IBCASE_UNIMPLEMENTED
		}
		break;
	}
	case OP_Table:
	{
		switch(t->type)
		{
		IBCASE_BLOCKWANTCODE
		{
			IBTask * t = NULL;
			IBLayer3PushTask(ibc, OP_TableNeedExpr, NULL, NULL);
			IBLayer3PushTask(ibc, OP_NeedExpression, NULL, &t);
			break;
		}
		IBCASE_UNIMPLEMENTED
		}
		break;
	}
	case OP_dbgBreak:
	{
		break;
	}
	case OP_Imaginary:
	{
		ibc->Imaginary = true;
		IBPushColor(IBFgMAGENTA);
		DbgPuts("[GOT IMAGINARY]");
		IBPopColor();
		DbgPuts("\n");
		break;
	}
	case OP_Return:
	{
		switch(t->type)
		{
		case OP_FuncHasName:
		{
			switch(o->type)
			{
			case OP_FuncHasName:
			{
				IBExpects* exp;
				SetObjType(o, OP_FuncNeedsRetValType);
				IBLayer3PushExpects(ibc, &exp);
				IBExpectsInit(exp, "P", OP_VarType);
				break;
			}
			default:
			{
				Err(OP_Error, "wrong obj type");
				break;
			}
			}
			break;
		}
		IBCASE_BLOCKWANTCODE
		{
			IBTask * t;
			IBExpects* exp;
			IBObj* o;
			IBLayer3PushObj(ibc, &o);
			IBObjSetType(o, OP_BlockReturnNeedValue);
			IBLayer3PushTask(ibc, OP_BlockReturnNeedValue, &exp, &t);
			IBExpectsInit(exp, "PPP", OP_Value, OP_String, OP_Name);
			break;
		}
		}
		break;
	}
	case OP_Func: //func1
	{
		IBExpects* ap;
		IBObj* o;
		IBTask* t;
		t = IBLayer3GetTask(ibc);
		IBassert(t);
		IBLayer3PushObj(ibc, &o);
		o->func.thingTask = t->type == OP_StructWantContent ? t : NULL;
		IBLayer3PushTask(ibc, OP_FuncNeedName, &ap, NULL);
		IBExpectsInit(ap, "1P", "expected function name", OP_Name);
		o->type = ibc->NameOp;
		o->func.retTYPE = OP_NotSet;
		o->func.retValType = OP_Void;
		o->func.retTypeMod = OP_NotSet;
		IBLayer3PushCodeBlock(ibc, &cb);
		break;
	}
	case OP_Use:
	{
		IBExpects* ap;
		IBLayer3PushTask(ibc, OP_UseNeedStr, &ap, NULL);
		IBExpectsInit(ap, "1P", "expected @use $name", OP_Name);
		IBLayer3PushCodeBlock(ibc, &cb);
		break;
	}
	case OP_Else:
	{
		switch(t->type)
		{
		case OP_IfBlockWantCode:
		{
			IBTask* t;
			IBExpects* exp;
			IBLayer3FinishTask(ibc);
			IBLayer3PushTask(ibc, OP_BlockWantCode, &exp, &t);
			IBExpectsInit(exp, "c", OP_Null);
			cb = IBLayer3CodeBlocksTop(ibc);
			IBStrAppendCh(&cb->code, '\t', tabCount - 1);
			IBStrAppendCStr(&cb->code, "else ");
			IBLayer3PushCodeBlock(ibc, &cb);
			break;
		}
		}
		break;
	}
	case OP_ElseIf:
	{
		switch(t->type)
		{
		case OP_IfBlockWantCode:
		{
			IBLayer3FinishTask(ibc);
			ibc->LastNameOp = ibc->NameOp;
			IBLayer3PushTask(ibc, OP_ElseIf, NULL, &t);
			break;
		}
		default:
		{
			Err(OP_YouCantUseThatHere, "You may only use @elif in @if blocks!!!");
			break;
		}
		}
	}
	case OP_If:
	{
		switch(t->type)
		{
		case OP_ElseIf:
		{
			cb = IBLayer3CodeBlocksTop(ibc);
			IBStrAppendCh(&cb->code, '\t', tabCount - 1);
			IBStrAppendCStr(&cb->code, "else ");
			IBLayer3PopTask(ibc, &t, false);
		}
		IBCASE_BLOCKWANTCODE
		{
			IBTask * nt;
			IBExpects* nexp;
			IBObj* o;
			IBLayer3PushObj(ibc, &o);
			IBObjSetType(o, OP_IfNeedLVal);
			IBLayer3PushTask(ibc, OP_BuildingIf, &nexp, &nt);
			IBExpectsInit(nexp, "1PP", "expected lval",
				OP_Value, OP_Name/*, OP_String*/);
			break;
		}
		default:
		{
			Err(OP_Error, "Unimplemented If task context");
			break;
		}
		}
		break;
	}
	case OP_Loop:
	{
		switch(t->type)
		{
		IBCASE_BLOCKWANTCODE
		{
			IBExpects * exp;
			IBTask* t;
			IBLayer3PushTask(ibc, OP_LoopBlockWantCode, &exp, &t);
			IBExpectsInit(exp, "Nc", OP_Break);
			IBLayer3PushCodeBlock(ibc, &cb);
			break;
		}
		IBCASE_UNIMPLEMENTED
		}
		break;
	}
	case OP_Break:
	{
		cb = IBLayer3CodeBlocksTop(ibc);
		IBStrAppendCh(&cb->code, '\t', tabCount);
		IBStrAppendCStr(&cb->code, "break;\n");
		break;
	}
	case OP_For:
	{
		/*
		switch (t->type)
		{
		IBCASE_BLOCKWANTCODE
		{
			break;
		}
		IBCASE_UNIMPLEMENTED
		}
		*/
		switch(t->type)
		{
		IBCASE_BLOCKWANTCODE
		{
			IBObj * o;
			IBTask* t;
			IBExpects* exp;
			IBLayer3PushObj(ibc, &o);
			IBObjSetType(o, OP_For);
			IBLayer3PushTask(ibc, OP_ForNeedStartValName, &exp, &t);
			IBExpectsInit(exp, "P", OP_Name);
			IBLayer3PushCodeBlock(ibc, &cb);
			break;
		}
		IBCASE_UNIMPLEMENTED
		}
		break;
	}
	default:
		Err(OP_ErrUnknownOpStr, "");
	}
}
