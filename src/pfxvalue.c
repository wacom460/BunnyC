#include "imbored.h"

void IBLayer3PFX_VALUE(IBLayer3* ibc, IBOp valType, IBVal strVal)
{
	IBTask* t = IBLayer3GetTask(ibc);
	IBObj* o = IBLayer3GetObj(ibc);
	switch(t->type)
	{
	case OP_ForNeedStartInitVal:
	{
		switch(t->type)
		{

		IBCASE_UNIMPLEMENTED
		}
		break;
	}
	case OP_NeedExpression:
	{
		IBObj* o = NULL;
		IBLayer3PushObj(ibc, &o);
		IBObjSetType(o, OP_Value);
		o->valType = valType;
		o->val = strVal;
		IBLayer3PopObj(ibc, true, &o);
		break;
	}
	case OP_ActOnName:
	{
		switch(ibc->NameOp)
		{
		case OP_EmptyStr:
		{
			IBExpects* exp;
			SetTaskType(t, OP_ActOnNameEquals);
			IBLayer3ReplaceExpects(ibc, &exp);
			IBExpectsInit(exp, "P", OP_Exclaim);
			break;
		}
		IBCASE_UNIMPLEMENTED
		}
		break;
	}
	case OP_VarWantValue:
	{
		switch(o->type)
		{
		case OP_VarWantValue:
		{
			o->var.val = strVal;
			o->var.valSet = true;
			switch(o->var.type)
			{
			case OP_Bool:
			{
				if(valType != OP_Bool)
					Err(OP_YouCantUseThatHere, "wrong value for bool variable");
				break;
			}
			case OP_f32:
			{
				if(valType != OP_Float)
					Err(OP_YouCantUseThatHere, "wrong value for float variable");
				break;
			}
			case OP_d64:
			{
				if(valType != OP_Double)
					Err(OP_YouCantUseThatHere, "wrong value for double variable");
				break;
			}
			case OP_u8:
			case OP_c8:
			/*{
				if (o->var.mod == OP_Pointer)
				{
					IBassert(valType == OP_String);
					break;
				}
			}*/
			case OP_u16:
			case OP_u64:
			case OP_u32:
			case OP_i8:
			case OP_i16:
			case OP_i64:
			case OP_i32:
			{
				if(valType != OP_Number)
					Err(OP_YouCantUseThatHere, "wrong value for number variable");
				break;
			}
			IBCASE_UNIMPLEMENTED
			}
			SetObjType(o, OP_VarComplete);
			IBLayer3PopObj(ibc, true, &o);
			IBLayer3FinishTask(ibc);
			break;
		}
		IBCASE_UNIMPLEMENTED
		}
		break;
	}
	case OP_BlockReturnNeedValue:
	{
		switch(o->type)
		{
		case OP_BlockReturnNeedValue:
		{
			o->val = strVal;
			o->valType = OP_Value;
			IBLayer3FinishTask(ibc);
			break;
		}
		}
		break;
	}
	case OP_CallWantArgs:
	{
		switch(o->type)
		{
		case OP_ArgNeedValue:
		{
			o->val = strVal;
			IBObjSetType(o, OP_Arg);
			o->valType = OP_Value;
			IBLayer3PopObj(ibc, true, &o);
			IBLayer3PushObj(ibc, &o);
			SetObjType(o, OP_ArgNeedValue);
			break;
		}
		}
		break;
	}
	case OP_BuildingIf:
	{
		IBExpects* exp = 0;
		switch(o->type)
		{
		case OP_IfNeedLVal:
		{
			o->ifO.lvVal = strVal;
			o->ifO.lvTYPE = valType;
			SetObjType(o, OP_IfNeedMidOP);
			break;
		}
		case OP_IfNeedMidOP:
		{
			switch(ibc->NameOp)
			{
			case OP_Equals:
			{
				o->ifO.midOp = ibc->NameOp;
				IBObjSetType(o, OP_IfNeedRVal);
				IBLayer3ReplaceExpects(ibc, &exp);
				IBExpectsInit(exp, "PP", OP_Name, OP_Value);
				break;
			}
			IBCASE_UNIMPLEMENTED
			}
			break;
		}
		case OP_IfNeedRVal:
		{
			IBExpects* exp;
			o->ifO.rvVal = strVal;
			o->ifO.rvTYPE = valType;
			SetObjType(o, OP_IfFinished);
			SetTaskType(t, OP_IfFinished);
			IBLayer3ReplaceExpects(ibc, &exp);
			IBExpectsInit(exp, "P", OP_LineEnd);
			break;
		}
		default:
		{
			Err(OP_Error, "Unimplemented If task context");
		}
		}
		break;
	}
	case OP_CPrintfHaveFmtStr:
	{
		IBObj* o;
		IBLayer3PushObj(ibc, &o);
		o->val = strVal;
		IBObjSetType(o, OP_Value);
		o->var.type = OP_i32;/*for now*/
		IBLayer3PopObj(ibc, true, NULL);
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
				IBOp valType = IBJudgeTypeOfStrValue(ibc, ibc->Str);
				switch(o->func.retValType)
				{
				case OP_c8:
				{

				}
				case OP_u8:
				case OP_u16:
				case OP_u32:
				case OP_u64:
				case OP_i8:
				case OP_i16:
				case OP_i32:
				case OP_i64: 
				case OP_f32:
				case OP_d64:
				{
					switch(valType)
					{
					case OP_Float:
					case OP_Double:
					case OP_Number: 
						break;
					default:
					{
						Err(OP_YouCantUseThatHere,
							"wrong return value type for this function");
					}
					}
					break;
				}
				IBCASE_UNIMPLEMENTED
				}
				DbgPuts("Finishing func got ret value\n");
				o->func.retVal = IBLayer3StrToVal(ibc, ibc->Str, o->func.retValType);
				o->func.retTYPE = OP_Value;
				PopExpects();
				SetTaskType(t, OP_Func);
				IBLayer3FinishTask(ibc);
				break;
			}
		}
		break;
	}
	IBCASE_UNIMPLEMENTED
	}
}
