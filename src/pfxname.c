#include "imbored.h"

void IBLayer3PFX_NAME(IBLayer3* ibc)
{
	IBTask* t = IBLayer3GetTask(ibc);
	IBObj* o = IBLayer3GetObj(ibc);
	IBCodeBlock* cb = IBLayer3CodeBlocksTop(ibc);
	switch(ibc->Str[0])
	{
	IBCASE_0THRU9
	{
		Err(OP_YouCantUseThatHere,
			"can't use number as first character of name!");
	}
	}
	switch(t->type)
	{
	case OP_MethodsNeedName:
	{
		IBTypeInfo* ti = 0;
		IBLayer3FindType(ibc, ibc->Str, &ti);
		if(!ti)
		{
			ErrF(OP_NotFound, "%s isnt found\n", ibc->Str);
		}
		else
		{
			IBObj* mo = 0;
			IBExpects* exp = 0;
			IBassert(ti);
			IBLayer3PushObj(ibc, &mo);
			SetObjType(mo, OP_Methods);
			IBObjSetName(mo, ibc->Str);
			//IBLayer3PopObj(ibc,true,NULL);
			mo = 0;
			SetTaskType(t, OP_MethodsWantContent);
			IBLayer3ReplaceExpects(ibc, &exp);
			IBExpectsInit(exp, "PPN", OP_Op, OP_Underscore, OP_Func);
			ibc->DefiningMethods = 1;
			IBOverwriteStr(&ibc->_methodsStructName, ibc->Str);
		}
		break;
	}
	case OP_ForNeedStartValName:
	{
		IBNameInfo* ni = NULL;
		IBExpects* exp = NULL;
		IBOp ar = IBNameInfoAddMember(ibc, &cb->localVariables, ibc->Str, OP_Name, &ni);
		//should never happen
		if(ar == OP_AlreadyExists)
			ErrF(OP_AlreadyExists, "name %s already in use", ibc->Str);
		IBassert(ni);
		IBassert(o->type == OP_For);
		IBOverwriteStr(&o->forO.startName, ibc->Str);
		SetTaskType(t, OP_ForNeedStartInitVal);
		IBLayer3ReplaceExpects(ibc, &exp);
		IBExpectsInit(exp, "P", OP_Value);
		break;
	}
	case OP_EnumWantContent:
	{
		IBObj* o;
		IBLayer3PushObj(ibc, &o);
		IBObjSetType(o, OP_EnumName);
		IBObjSetName(o, ibc->Str);
		IBASSERT0(ibc->DefiningEnumTypeInfo);
		IBTypeInfo*nti=0;
		IBVectorPush(&ibc->DefiningEnumTypeInfo->members, &nti);
		IBASSERT0(nti);
		IBTypeInfoInit(nti, OP_EnumName, o->name);
		nti->memberVarType = ibc->DefiningEnumTypeInfo;
		IBLayer3PopObj(ibc, true, &o);
		break;
	}
	case OP_EnumNeedName:
	{
		IBExpects* exp=0;
		IBTypeInfo* eti=0;
		SetTaskType(t, OP_EnumWantContent);
		IBLayer3ReplaceExpects(ibc, &exp);
		IBExpectsInit(exp, "PP", OP_Name, OP_Underscore);
		IBassert(o->type == OP_Enum);
		IBObjSetName(o, ibc->Str);
		IBLayer3FindType(ibc, o->name, &eti);
		IBASSERT0(!eti);
		if(eti) 
			ErrF(OP_AlreadyExists, "type %s already exists", o->name);
		IBLayer3RegisterCustomType(ibc, o->name, OP_Enum, &eti);
		IBASSERT0(eti);
		ibc->DefiningEnumTypeInfo = eti;
		IBLayer3PopObj(ibc, true, &o);			
		break;
	}
	case OP_NeedExpression:
	{
		IBObj* o;
		IBLayer3PushObj(ibc, &o);
		IBObjSetType(o, OP_Name);
		IBObjSetName(o, ibc->Str);
		o->modifier = ibc->Pointer;
		IBLayer3PopObj(ibc, true, &o);
		break;
	}
	case OP_StructInitNeedName:
	{
		IBExpects* exp;
		IBassert(o->type == OP_StructInit);
		IBObjSetName(o, ibc->Str);
		IBLayer3PushTask(ibc, OP_SubtaskArgs, &exp, &t);
		IBExpectsInit(exp, "PPP", OP_Name, OP_String, OP_Value);
		break;
	}
	case OP_FuncHasName:
	{
		switch(o->type)
		{
		case OP_FuncArgNameless:
		{
			SetObjType(o, OP_FuncArgComplete);
			PopExpects();
			IBObjSetName(IBLayer3GetObj(ibc), ibc->Str);
			IBNameInfoAddMember(ibc, &cb->localVariables, ibc->Str,
				o->arg.type, NULL);
			IBLayer3PopObj(ibc, true, NULL);
			break;
		}
		}
		break;
	}
	case OP_FuncNeedName:
	{
		switch(o->type)
		{
		case OP_Func:
		{
			IBExpects* exp;
			SetObjType(o, OP_FuncHasName);
			SetTaskType(t, OP_FuncHasName);
			IBLayer3PushExpects(ibc, &exp);
			IBExpectsInit(exp, "PPPPN",
				OP_VarType, OP_Op, OP_LineEnd, OP_Subtract, OP_Return);
			IBObjSetName(o, ibc->Str);
			break;
		}
		}
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
				//IBOp nameType = IBNameInfoDBFindType(&ibc->NameTypeCtx, ibc->Str);
				IBNameInfo* ni = IBLayer3SearchNameInfo(ibc, ibc->Str);
				IBassert(ni);
				if(ni->type == OP_NotFound)
					Err(OP_NotFound, "variable name not found");
				if(o->func.retValType != ni->type && o->func.retValType != ibc->Varcast)
					Err(OP_Error, "variable doesn't match function return type\n");
				DbgPuts("Finishing func got ret value as name\n");
				IBOverwriteStr(&o->func.retStr, ibc->Str);
				o->func.retTYPE = OP_Name;
				o->func.retValVarcast = ibc->Varcast;
				PopExpects();
				SetTaskType(t, OP_Func);
				IBLayer3FinishTask(ibc);
				break;
			}
		}
		break;
	}
	case OP_CallWantArgs:
	{
		if(o->type == OP_ArgNeedValue)
		{
			IBObjSetStr(o, ibc->Str);
			IBObjSetType(o, OP_Arg);
			o->valType = OP_Name;
			IBLayer3PopObj(ibc, true, &o);
			IBLayer3PushObj(ibc, &o);
			SetObjType(o, OP_ArgNeedValue);
		}
		else Err(OP_Error, "wrong obj type");
		break;
	}
	case OP_BlockReturnNeedValue:
	{
		switch(o->type)
		{
		case OP_BlockReturnNeedValue:
		{
			IBObjSetStr(o, ibc->Str);
			o->valType = OP_Name;
			IBLayer3FinishTask(ibc);
			break;
		}
		}
		break;
	}
	case OP_BuildingIf:
	{
		IBExpects* exp = NULL;
		switch(o->type)
		{
		case OP_IfNeedLVal:
		{
			IBOverwriteStr(&o->ifO.lvName, ibc->Str);
			IBNameInfo* ni = IBLayer3SearchNameInfo(ibc, o->ifO.lvName);
			if(!ni)
			{
				ErrF(OP_NotFound, "%s wasn't found", o->ifO.lvName);
			}
			IBassert(ni);
			o->ifO.lvTYPE = OP_Name;
			SetObjType(o, OP_IfNeedMidOP);
			IBLayer3ReplaceExpects(ibc, &exp);
			IBExpectsInit(exp, "PPPN", OP_GreaterThan, OP_Value, OP_LessThan, OP_Value);
			break;
		}
		case OP_IfNeedRVal:
		{
			IBOverwriteStr(&o->ifO.rvName, ibc->Str);
			IBNameInfo* ni = IBLayer3SearchNameInfo(ibc, o->ifO.rvName);
			if(!ni)
			{
				ErrF(OP_NotFound, "%s wasn't found", o->ifO.rvName);
			}
			IBassert(ni);
			o->ifO.rvTYPE = OP_Name;
			SetObjType(o, OP_IfFinished);
			SetTaskType(t, OP_IfFinished);
			IBExpects* exp = 0;
			IBLayer3ReplaceExpects(ibc, &exp);
			IBExpectsInit(exp, "P", OP_LineEnd);
			break;
		}
		}
		break;
	}
	case OP_StructWantName:
	{
		IBObj* o = 0;
		IBLayer3PushObj(ibc, &o);
		IBassert(ibc->Str[0] != '\0');
		IBObjSetName(o, ibc->Str);
		IBTypeInfo* ti = 0;
		IBLayer3FindType(ibc, o->name, &ti);
		IBASSERT0(!ti);
		if(ti)
			ErrF(OP_AlreadyExists, "type %s already exists", o->name);
		IBLayer3RegisterCustomType(ibc, o->name, OP_Struct, &ti);
		ibc->DefiningStructTypeInfo = ti;
		IBObjSetType(o, OP_Struct);
		SetTaskType(t, OP_StructWantRepr);
		PopExpects();
		break;
	}
	case OP_UseNeedStr:
	{
		switch(ibc->NameOp)
		{
		case OP_UseStrSysLib:
		{
			IBPushColor(IBFgIntensity | IBFgYELLOW | IBBgBROWN);
			DbgPuts("Inputting system lib code to compiler\n");
			IBPopColor();
			IBStrAppendCStr(&ibc->CHeader_Funcs,
				"/* System Lib Header */\n");
			IBassert(!ibc->InputStr);
			ibc->InputStr = SysLibCodeStr;
			ibc->Line++;
			ibc->Column = 1;
			break;
		}
		default:
		{

			break;
		}
		}
		IBLayer3PopTask(ibc, NULL, false);
		IBLayer3PopCodeBlock(ibc, false, &cb);//?????
		break;
	}
	case OP_CPrintfHaveFmtStr:
	{
		IBObj* o = 0;
		IBStr* at = 0;
		int idx = 0;
		IBLayer3PushObj(ibc, &o);
		IBObjSetName(o, ibc->Str);
		IBObjSetType(o, OP_Name);
		while(at = IBVectorIterNext(&ibc->ArrayIndexExprsVec, &idx))
		{
			IBStr* ss = 0;
			IBVectorPush(&o->arg.arrIndexExprs, &ss);
			IBStrInit(ss);
			IBStrAppend(ss, at);
		}
		IBLayer3PopObj(ibc, true, NULL);
		break;
	}
	IBCASE_BLOCKWANTCODE
	{
		IBTask * t = NULL;
		IBExpects* exp = NULL;
		IBObj* o = NULL;
		IBLayer3PushTask(ibc, OP_ActOnName, &exp, &t);
		IBLayer3PushObj(ibc, &o);
		IBObjSetType(o, OP_ActOnName);
		/*if (StrStartsWith(ibc->Str, "self.") && strlen(ibc->Str) > 5)
		{
			IBStr ns;
			IBStrInit(&ns);
			IBStrAppendCStr(&ns, "self->");
			IBStrAppendFmt(&ns, "%s", &ibc->Str[5]);
			ibc->Str[0]='\0';
			strncpy(ibc->Str, ns.start, IBLayer3STR_MAX);
		}*/
		IBObjSetName(o, ibc->Str);
		IBLayer3PopObj(ibc, true, &o);
		IBExpectsInit(exp, "PP", OP_Value, OP_LessThan);
		break;
	}
	case OP_VarNeedName:
	{
		switch(o->type)
		{
		case OP_VarNeedName:
		{
			IBExpects* exp;
			IBNameInfo* ni = NULL;
			IBOp rc = 0;
			IBObjSetName(o, ibc->Str);
			IBTypeInfo* ti = o->var.ti;
			IBOp realType = o->var.type == OP_Unknown && ti ? ti->type : o->var.type;
			rc = IBNameInfoAddMember(ibc,
				&cb->localVariables,
				ibc->Str, realType, &ni);
			ni->type = realType;
			ni->ti = ti;

			IBTypeInfo* mvTi = 0;
			int mvIdx = 0;
			while(mvTi = IBVectorIterNext(&ti->members, &mvIdx))
			{
				IBNameInfo* mvNi = 0;
				IBNameInfoAddMember(ibc, ni, mvTi->name.start, mvTi->StructVar.type, &mvNi);
				mvNi->ti = mvTi;
			}

			if(rc == OP_AlreadyExists)
				Err(OP_Error, "name already in use");
			IBassert(rc == OP_OK);
			SetObjType(o, OP_VarWantValue);
			SetTaskType(t, OP_VarWantValue);
			IBLayer3ReplaceExpects(ibc, &exp);
			IBExpectsInit(exp, "1PPPPP",
				"",
				OP_Value, OP_LessThan, OP_String, OP_SingleQuote, OP_LineEnd);
			break;
		}
		IBCASE_UNIMPLEMENTED
		}
		break;
	}
	}
}
