#include "imbored.h"

void IBLayer3StrPayload(IBLayer3* ibc)
{
	IBVal strVal;
	IBTask* t;
	IBTask* tParent = NULL;
	int tabCount = IBLayer3GetTabCount(ibc);
	IBCodeBlock* cb = IBLayer3CodeBlocksTop(ibc);
	IBObj* o;
	IBOp valType = IBJudgeTypeOfStrValue(ibc, ibc->Str);
	strVal.i64 = 0;
	if(ibc->DotPathOn)
	{
		IBStr dpStr;
		IBStrInitWithCStr(&dpStr, ibc->Str);
		IBVectorClear(&ibc->DotPathVec, IBStrFree);
		//if(!strcmp(ibc->Str, "f.name")) DB;
		IBStrSplitBy(&dpStr, '.', &ibc->DotPathVec);
		IBStrFree(&dpStr);
		//DB;
	}
	t = IBLayer3GetTask(ibc);
	o = IBLayer3GetObj(ibc);
	if(ibc->TaskStack.elemCount >= 2)
	{
		tParent = (IBTask*) IBVectorGet(&ibc->TaskStack, ibc->TaskStack.elemCount - 2);
	}
	switch(valType)
	{
	case OP_Bool:
	{
		IBOp boolCheck = IBStrToBool(ibc, ibc->Str);
		switch(boolCheck)
		{
		case OP_True:
		{
			strVal.boolean = true;
			break;
		}
		case OP_False:
		{
			strVal.boolean = false;
			break;
		}
		IBCASE_UNIMPLEMENTED
		}
		break;
	}
	case OP_String: break;
	case OP_Number:
	{
		strVal.i64 = atoll(ibc->Str);
		break;
	}
	case OP_Double:
	{
		strVal.d64 = atof(ibc->Str);
		break;
	}
	case OP_Float:
	{
		strVal.f32 = (float) atof(ibc->Str);
		break;
	}
	//IBCASE_UNIMPLEMENTED
	/*default:
	{
		strVal.i32 = atoi(ibc->Str);
		break;
	}*/
	}

	//if(ibc->Pfx==OP_Op) ibc->LastNameOp = ibc->NameOp;
	ibc->NameOp = IBGetOpFromNameList(ibc->Str, OP_NameOps);
	IBPushColor(IBFgGREEN);
	DbgPuts("StrPayload: ");
	IBPushColor(IBBgWHITE);
	DbgFmt("%s", ibc->Str);
	IBPopColor();
	IBPopColor();
	DbgPuts("\n");
top:
	switch(ibc->Pfx)
	{
	/* a-zA-z PFXazAZ */ case OP_Letter_azAZ:
	{
		IBTypeInfo* ti = 0;
		IBLayer3FindType(ibc, ibc->Str, &ti);
		//IBOp dataType = IBGetOpFromNameList(ibc->Str, OP_DataTypes);
		//if (dataType != OP_Unknown)
		if(ti)
		{
			ibc->Pfx = OP_VarType;
			DbgPuts("infered vartype\n");
			goto top;
		}
		else if(!strcmp(ibc->Str, IBFALSESTR) ||
			!strcmp(ibc->Str, IB_TRUESTR))
		{
			ibc->Pfx = OP_Value;
			DbgPuts("infered bool Value\n");
			goto top;
		}
		else
		{
			IBOp nameOp = IBGetOpFromNameList(ibc->Str, OP_NameOps);
			switch(nameOp)
			{
			case OP_Unknown:
			case OP_Void:
				break;
			default:
			{
				ibc->Pfx = OP_Op;
				DbgPuts("infered OP\n");
				goto top;
			}
			}
		}
		Err(OP_Error, "Couldn't infer this input");
	}
	/* _ PFXUNDERSCORE */ case OP_Underscore:
	{
		switch(ibc->NameOp)
		{
		case OP_EmptyStr:
		{
			IBLayer3Done(ibc);
			break;
		}
		IBCASE_UNIMPLEMENTED
		}
		break;
	}
	/* + PFXADD */ case OP_Add:
	/* * PFXMULTIPLY */ case OP_Multiply:
	/* / PFXDIVIDE */ case OP_Divide:
	/* - PFXSUBTRACT */ case OP_Subtract:
	{
		switch(ibc->NameOp)
		{
		case OP_EmptyStr:
		{
			switch(t->type)
			{
			case OP_RootTask:
			{
				switch(ibc->NameOp)
				{
				case OP_Exclaim:
				{
					break;
				}
				IBCASE_UNIMPLEMENTED
				}
				break;
			}
			case OP_NeedExpression:
			{
				IBObj* o;
				IBLayer3PushObj(ibc, &o);
				IBObjSetType(o, ibc->Pfx);
				IBLayer3PopObj(ibc, true, &o);
				break;
			}
			IBCASE_UNIMPLEMENTED
			}
			break;
		}
		case OP_GreaterThan:
		{
			IBExpects* exp;
			SetObjType(o, OP_FuncNeedsRetValType);
			IBLayer3PushExpects(ibc, &exp);
			IBExpectsInit(exp, "P", OP_VarType);
			break;
		}
		IBCASE_UNIMPLEMENTED
		}
		break;
	}
	/* | PFXOR */ case OP_Or:
	{
		switch(ibc->NameOp)
		{
		case OP_Or:
		{

			break;
		}
		}
	}
	case OP_Dot:
	{
		IBLayer3PFX_DOT(ibc);
		break;
	}
	case OP_SingleQuote:
	{
		IBLayer3PFX_SINGLEQUOTE(ibc);
		break;
	}
	case OP_Exclaim:
	{
		IBLayer3PFX_EXCLAIM(ibc);
		break;
	}
	case OP_String:
	{
		IBLayer3PFX_STRING(ibc);
		break;
	}
	case OP_VarType:
	{
		IBLayer3PFX_VARTYPE(ibc);
		break;
	}
	case OP_Name:
	{
		IBLayer3PFX_NAME(ibc);
		break;
	}
	case OP_Op:
	{
		IBLayer3PFX_OP(ibc);
		break;
	}
	case OP_Value:
	{
		IBLayer3PFX_VALUE(ibc, valType, strVal);
		break;
	}
	case OP_LessThan:
	{
		IBLayer3PFX_LESSTHAN(ibc);
		break;
	}
	case OP_GreaterThan:
	{
		IBLayer3PFX_GREATERTHAN(ibc);
		break;
	}
	IBCASE_UNIMPLEMENTED
	}
	ibc->Str[0] = '\0';
#ifdef IBDEBUGPRINTS
	PLINE;
	IBPushColor(IBFgMAGENTA);
	printf(" Str payload complete");
	IBPopColor();
	printf("\n");
#endif
	IBLayer3Pop(ibc);
	if(ibc->StrReadPtrsStack.elemCount > 1)
	{
		if(*(bool*) IBVectorTop(&ibc->StrReadPtrsStack))
			ibc->Pointer = OP_NotSet;
		IBVectorPop(&ibc->StrReadPtrsStack, NULL);
	}
	IBVectorClear(&ibc->ArrayIndexExprsVec, IBStrFree);
	ibc->Varcast = OP_Null;
	//#define IBOPSTEP
#ifdef IBOPSTEP
	{
		int l = ibc->InputStr ? ibc->LineIS : ibc->Line;
		int c = ibc->InputStr ? ibc->ColumnIS : ibc->Column;
		IBPushColor(IBFgRED);
		DbgFmt("[");
		IBPushColor(IBBgGREEN);
		DbgFmt("%d:%d", l, c);
		IBPopColor();
		DbgFmt("]");
		IBPopColor();
		IBPushColor(IBBgYELLOW);
		DbgFmt("Press enter to ", "");
		IBPushColor(IBBgRED | IBFgYELLOW);
		DbgFmt("step", "");
		IBPopColor();
		IBPopColor();
		DbgFmt("\n", "");
		getchar();
	}
#endif
}
