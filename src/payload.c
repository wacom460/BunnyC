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
	if(ibc->DotPathOn && ibc->Pfx == OP_Name)
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
		//bool fall = true;
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
					//fall = false;
					IBLayer3Push(ibc, OP_ModeCCompTimeMacroPaste, true);
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
			//fall = false;
			SetObjType(o, OP_FuncNeedsRetValType);
			IBLayer3PushExpects(ibc, &exp);
			IBExpectsInit(exp, "P", OP_VarType);
			break;
		}
		IBCASE_UNIMPLEMENTED
		}
		//if (!fall) break;
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
	/* ! PFXEXCLAIM */ case OP_Exclaim:
	{
		switch(t->type)
		{
		IBCASE_BLOCKWANTCODE
		{
			if(!*ibc->Str)
			{
				IBExpects* exp = 0;
				//IBTask* t = 0;
				IBLayer3PushTask(ibc, OP_CallMethodNeedName, &exp, &t);
				IBExpectsInit(exp, "P", OP_Name);
				break;
			}
			//IBTask* t;
			//IBLayer3PushTask(ibc, OP_CodeBlockCallFunc, NULL, NULL);
			IBLayer3PushTask(ibc, OP_CallFunc, NULL, NULL);
			//break;
		}
		case OP_ActOnNameEquals:
		{
			IBExpects* exp;
			IBObj* o;
			//IBTask* t;
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
		break;
	}
	/* " PFXSTRING */ case OP_String:
	{
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
		break;
	}
	/* % PFXVARTYPE */ case OP_VarType:
	{
		IBTypeInfo* ti = 0;
		IBLayer3FindType(ibc, ibc->Str, &ti);
		if(!ti)
			ErrF(OP_NotFound, "type %s doesn't exist", ibc->Str);
		switch(t->type)
		{
		case OP_FuncHasName:
		{
			switch(o->type)
			{
			case OP_FuncHasName:
			{
				IBObj* o;
				IBExpects* exp;
				IBLayer3PushObj(ibc, &o);
				SetObjType(o, OP_FuncArgNameless);
				o->arg.type = ibc->NameOp;
				o->arg.mod = ibc->Pointer;
				if(o->arg.type == OP_c8 && o->arg.mod == OP_Pointer)
					o->arg.type = OP_String;
				IBLayer3PushExpects(ibc, &exp);
				IBExpectsInit(exp, "1P", "expected func arg name", OP_Name);
				break;
			}
			case OP_FuncNeedsRetValType:
			{
				o->func.retValType = ibc->NameOp;
				o->func.retTypeMod = ibc->Pointer;
				if(o->func.retValType == OP_Unknown)
					Err(OP_Error, "unknown return type");
				if(o->func.retValType == OP_String)
				{
					o->func.retValType = OP_c8;
					o->func.retTypeMod = OP_Pointer;
				}
				SetObjType(o, OP_FuncSigComplete);
				break;
			}
			}
			break;
		}
		case OP_StructWantRepr:
		{
			SetTaskType(t, OP_StructWantContent);
			PopExpects();
			break;
		}
		case OP_StructWantContent:
		case OP_RootTask:
		IBCASE_BLOCKWANTCODE
		{
			IBObj * o;
			IBExpects* exp;
			IBTask* t;
			IBLayer3PushObj(ibc, &o);
			o->var.type = ibc->NameOp;
			IBLayer3FindType(ibc,ibc->Str,&o->var.ti);
			IBObjSetStr(o, ibc->Str);
			o->var.mod = ibc->Pointer;
			o->var.valSet = false;
			SetObjType(o, OP_VarNeedName);
			IBLayer3PushTask(ibc, OP_VarNeedName, &exp, &t);
			IBExpectsInit(exp, "1P", "expected variable name", OP_Name);
			break;
		}
		IBCASE_UNIMPLEMENTED
		}
		break;
	}
	/* $ PFXNAME */ case OP_Name:
	{
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
				IBExpects* exp;
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
				IBLayer3ReplaceExpects(ibc, &exp);
				IBExpectsInit(exp, "P", OP_LineEnd);
				break;
			}
			}
			break;
		}
		case OP_StructWantName:
		{
			IBObj* o;
			IBLayer3PushObj(ibc, &o);
			IBassert(ibc->Str[0] != '\0');
			IBObjSetName(o, ibc->Str);
			IBTypeInfo*ti=0;
			IBLayer3FindType(ibc, o->name, &ti);
			IBASSERT0(!ti);
			if(ti) ErrF(OP_AlreadyExists, "type %s already exists", o->name);
			IBLayer3RegisterCustomType(ibc, o->name, OP_Struct, &ti);
			ibc->DefiningStructTypeInfo = ti;
			IBObjSetType(o, OP_Struct);
			SetTaskType(t, OP_StructWantRepr);
			PopExpects();
			break;
		}
		case OP_UseNeedStr:
		{
			IBOp lib;
			lib = ibc->NameOp;
			switch(lib)
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
		break;
	}
	/* @ PFXOP */ case OP_Op:
	{
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
		break;
	}
	/* = PFXVALUE */ case OP_Value:
	{
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
		break;
	}
	/* < PFXLESSTHAN */ case OP_LessThan:
	{
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
		break;
	}
	/* > PFXGREATERTHAN */ case OP_GreaterThan:
	{
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
