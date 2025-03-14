#include "imbored.h"

IBObj* IBLayer3GetObj(IBLayer3* ibc)
{
	return (IBObj*) IBVectorTop(&ibc->ObjStack);
}

//$structObj.memberVar
IBNameInfo* IBLayer3TryFindNameInfoInStructVar(IBLayer3* ibc, IBNameInfo* ni)
{
	if(ibc->DotPathVec.elemCount >= 2)
	{
		IBStr* first = (IBStr*)IBVectorGet(&ibc->DotPathVec, 0);
		IBNameInfo* sni = IBNameInfoFindMember(ni, first->start);
		if(sni && sni->ti && sni->ti->members.elemCount)
		{
			for(int i = 1; i < ibc->DotPathVec.elemCount; i++)
			{
				IBStr* ds = (IBStr*) IBVectorGet(&ibc->DotPathVec, i);
				IBNameInfo* mvNi = (IBNameInfo*) IBVectorGet(&sni->members, i - 1);
				if(!strcmp(ds->start, mvNi->name))
				{
					return mvNi;
				}
			}
		}
	}
	return NULL;
}

IBNameInfo* _IBLayer3SearchNameInfo(IBLayer3* ibc, char* name, int ln)
{
	int idx;
	IBNameInfo* ni = NULL;
	IBassert(ibc);
	IBassert(ibc->CodeBlockStack.elemCount);
	IBassert(name);
	IBassert(name[0]);
	//DbgFmt("[%d]"__FUNCTION__"(,%s)\n", ln, name);
	idx = ibc->CodeBlockStack.elemCount - 1;
	while(true)
	{
		IBCodeBlock* cb = (IBCodeBlock*) IBVectorGet(&ibc->CodeBlockStack, idx);
		IBassert(cb);
		IB_ASSERTMAGICP(&cb->localVariables.members);
		ni = IBNameInfoFindMember(&cb->localVariables, name);
		if(!ni) ni = IBLayer3TryFindNameInfoInStructVar(ibc, &cb->localVariables);
		if(ni) return ni;
		if(idx == 0) break;
		else idx--;
	}
	return ni;
}

IBObj* IBLayer3FindStackObjRev(IBLayer3* ibc, IBOp type)
{
	IBassert(ibc);
	IBassert(type > 0);
	for(int i = ibc->ObjStack.elemCount - 1; i >= 0; i--)
	{
		IBObj* o = (IBObj*) IBVectorGet(&ibc->ObjStack, i);
		IBassert(o);
		if(o->type == type)return o;
	}
	return NULL;
}

IBObj* IBLayer3FindStackObjUnderIndex(IBLayer3* ibc, int index, IBOp type)
{
	int i;
	if(index < 0)index = ibc->ObjStack.elemCount + index;
	IBASSERT0(index >= 0);
	if(ibc->ObjStack.elemCount < 2)
		Err(OP_Error, "Not enough objects on stack");
	if(index > ibc->ObjStack.elemCount)
		Err(OP_Error, "Index out of bounds");
	for(i = index - 1; i >= 0;)
	{
		IBObj* o;
		o = (IBObj*) IBVectorGet(&ibc->ObjStack, i);
		i--;
		if(o->type == type) return o;
	}
	return NULL;
}

IBObj* IBLayer3FindStackObjUnderTop(IBLayer3* ibc, IBOp type)
{
	IBObj* o;
	int i;
	if(ibc->ObjStack.elemCount < 2) return NULL;
	for(i = ibc->ObjStack.elemCount - 1; i >= 0;)
	{
		o = (IBObj*) IBVectorGet(&ibc->ObjStack, i--);
		if(o->type == type) return o;
	}
	return NULL;
}

void IBLayer3RegisterCustomType(IBLayer3* ibc, char* name, IBOp type, IBTypeInfo** outDP)
{
	IBTypeInfo* ti = 0;
	IBASSERT0(ibc);
	IBASSERT0(name);
	IBASSERT0(type > 0);
	IBLayer3FindType(ibc, name, &ti);
	if(ti)
	{
		ErrF(OP_AlreadyExists, "type %s already exists", name);
		return;
	}
	switch(type)
	{
	case OP_Enum:
	case OP_Struct:
	case OP_Func:
		break;
		IBCASE_UNIMPLEMENTED
	}
	IB_ASSERTMAGIC(ibc->TypeRegistry);
	IBVectorPush(&ibc->TypeRegistry, &ti);
	IBTypeInfoInit(ti, type, name);
	if(outDP) (*outDP) = ti;
}

void IBLayer3FindType(IBLayer3* ibc, char* name, IBTypeInfo** outDP)
{
	IBTypeInfo* ti = 0;
	int idx = 0;
	IBASSERT0(ibc);
	IBASSERT0(name);
	IBASSERT0(strlen(name) > 0);
	while(ti = IBVectorIterNext(&ibc->TypeRegistry, &idx))
	{
		IB_ASSERTMAGICP(ti);
		IBASSERT0(ti->name.start)
			if(!strcmp(ti->name.start, name)) break;
	}
	if(outDP) (*outDP) = ti;
}

IBTask* IBLayer3FindTaskUnderIndex(IBLayer3* ibc, int index, IBOp type, int limit)
{
	int i;
	if(ibc->TaskStack.elemCount < 2)
		Err(OP_Error, "Not enough tasks on stack");
	if(index == -1) index = ibc->TaskStack.elemCount - 1;
	if(index >= ibc->TaskStack.elemCount)
		Err(OP_Error, "Index out of bounds");
	for(i = index - 1; i >= 0 && limit--;)
	{
		IBTask* t;
		t = (IBTask*) IBVectorGet(&ibc->TaskStack, i--);
		if(t->type == type) return t;
	}
	return NULL;
}

void _IBLayer3_TCCErrFunc(IBLayer3* ibc, const char* msg)
{
	ErrF(OP_TCC_Error, "%s\n", msg);
}

void IBLayer3CompileTCC(IBLayer3* ibc)
{
	int (*Entry)(int, char**);
	IBASSERT(ibc->TCC == NULL, "");
	ibc->TCC = tcc_new();
	IBASSERT(ibc->TCC, "");
	tcc_set_error_func(ibc->TCC, ibc,
		(void(*)(void* opaque, const char* msg))_IBLayer3_TCCErrFunc);
	tcc_set_output_type(ibc->TCC,
		TCC_OUTPUT_MEMORY);
#ifdef __TINYC__
	tcc_add_sysinclude_path(ibc->TCC, "src/");
	tcc_add_sysinclude_path(ibc->TCC, "ext/tcc/include/");
	tcc_add_library_path(ibc->TCC, "ext/tcc/lib/");
#else
	tcc_add_sysinclude_path(ibc->TCC, "../src/");
	tcc_add_sysinclude_path(ibc->TCC, "../ext/tcc/include/");
	tcc_add_library_path(ibc->TCC, "../ext/tcc/lib/");
#endif
	IBASSERT(IBStrLen(&ibc->FinalOutput) > 0,
		"no code to compile");
	IBASSERT(tcc_compile_string(ibc->TCC,
								(const char*) ibc->FinalOutput.start) != -1,
		"TCC compile failed!");
	IBASSERT(tcc_relocate(ibc->TCC, TCC_RELOCATE_AUTO) >= 0, "");
	Entry = tcc_get_symbol(ibc->TCC, "main");
	char* argv[IBRUN_MAXARGS];
	int argc = 0;
	memset(argv, 0, sizeof(argv));
	long long argsLen = IBStrLen(&ibc->RunArguments);
	//int avc = 0;
	if(argsLen)
	{
		IBStr ls;
		IBStrInit(&ls);
		for(int i = 0; i < argsLen; ++i)
		{
			char ch = ibc->RunArguments.start[i];
			if(ch == ' ')
			{
				IBOverwriteStr(&argv[argc], ls.start);
				IBStrClear(&ls);
				++argc;
				continue;
			}
			IBStrAppendCh(&ls, ch, 1);
		}
		if(IBStrLen(&ls))
		{
			IBOverwriteStr(&argv[argc], ls.start);
			++argc;
		}
		IBStrFree(&ls);
	}
	if(Entry)
	{
		int entryRet = 0;
		DbgPuts("Program output:\n");
		entryRet = Entry(argc, argv);
		IBPushColor(IBFgGREEN);
		DbgFmt("\n\nmain() returned %d.\n", entryRet);
		IBPopColor();
	}
	for(int i = 0; i < IBRUN_MAXARGS; ++i)
	{
		if(argv[i])free(argv[i]);
	}
	tcc_delete(ibc->TCC);
	ibc->TCC = NULL;
}

int IBLayer3GetTabCount(IBLayer3* ibc)
{
	return ibc->CodeBlockStack.elemCount - 1;
}

void IBLayer3Done(IBLayer3* ibc)
{
	IBTask* t = IBLayer3GetTask(ibc);
	IBassert(t);
	if(ibc->TaskStack.elemCount < 1) Err(OP_ErrNoTask, "");
	switch(t->type)
	{
	case OP_CaseWantCode:
	{
		IBLayer3FinishTask(ibc);
		t = IBLayer3GetTask(ibc);
		IBassert(t->type == OP_TableWantCase);
		IBLayer3FinishTask(ibc);
		break;
	}
	case OP_StructWantContent:
	{
		IBObj* o;
		o = IBLayer3GetObj(ibc);
		IBassert(o->type == OP_Struct);
		IBLayer3PopObj(ibc, true, &o);
		IBassert(o->type == OP_NotSet);
		IBLayer3FinishTask(ibc);
		ibc->DefiningStruct = 0;
		ibc->DefiningStructTypeInfo = 0;
		break;
	}
	case OP_MethodsWantContent:
	{
		IBObj* o;
		o = IBLayer3GetObj(ibc);
		IBassert(o->type == OP_Methods);
		IBLayer3PopObj(ibc, true, NULL);
		IBLayer3FinishTask(ibc);
		ibc->DefiningMethods = 0;
		if(ibc->_methodsStructName)
			free(ibc->_methodsStructName);
		ibc->_methodsStructName = 0;
		break;
	}
	case OP_EnumWantContent:
	case OP_BlockWantCode:
	case OP_LoopBlockWantCode:
	case OP_IfBlockWantCode:
	{
		IBLayer3FinishTask(ibc);
		break;
	}
	case OP_Func:
	case OP_FuncHasName:
	case OP_FuncWantCode:
	{
		IBObj* o;
		IBVector* wo;
		int idx;
		IBassert(t);
		PLINE;
		DbgPuts(" Finishing function\n");
		idx = 0;
		/*t = NULL;*/
		o = NULL;
		wo = &t->working;
		while(o = (IBObj*) IBVectorIterNext(wo, &idx))
		{
			/*TODO: could cache func obj index later*/
			if(o->type == OP_FuncSigComplete)
			{
				if(o->func.retValType != OP_Void)
				{
					IBExpects* exp;
					IBLayer3PushExpects(ibc, &exp);
					/*if (o->func.retType == OP_c8 && o->func.retTypeMod == OP_Pointer)
					{
						ExpectsInit(exp, "PP", OP_Name, OP_String);
					}
					else
					{

					}*/
					IBExpectsInit(exp, "PPP", OP_Value, OP_Name, OP_String);
					SetTaskType(t, OP_FuncNeedRetVal);
				}
				else
				{
					SetTaskType(t, OP_Func);
					IBLayer3FinishTask(ibc);
				}
			}
		}
		break;
	}
	}
}

IBObj* IBLayer3FindWorkingObj(IBLayer3* ibc, IBOp type)
{
	IBObj* o = NULL;
	int idx = 0;
	IBTask* t = IBLayer3GetTask(ibc);
	IBassert(t);
	while(o = IBVectorIterNext(&t->working, &idx))
		if(o->type == type) return o;
	return NULL;
}

IBObj* IBLayer3FindWorkingObjRev(IBLayer3* ibc, IBOp type)
{
	IBTask* t = IBLayer3GetTask(ibc);
	IBassert(t);
	for(int i = t->working.elemCount - 1; i >= 0; i--)
	{
		IBObj* o = (IBObj*) IBVectorGet(&t->working, i);
		IBassert(o);
		if(o->type == type) return o;
	}
	return NULL;
}

IBObj* IBLayer3FindWorkingObjUnderIndex(IBLayer3* ibc, int index, IBOp type)
{
	IBTask* t = IBLayer3GetTask(ibc);
	int i = 0;
	IBASSERT0(t);
	if(index < 0)index = t->working.elemCount + index;
	IBASSERT0(index >= 0);
	if(t->working.elemCount < 2)
		Err(OP_Error, "Not enough objects on stack");
	if(index > t->working.elemCount)
		Err(OP_Error, "Index out of bounds");
	for(i = index - 1; i >= 0;)
	{
		IBObj* o = 0;
		o = (IBObj*) IBVectorGet(&t->working, i);
		i--;
		if(o->type == type) return o;
	}
	return NULL;
}

IBCodeBlock* IBLayer3CodeBlocksTop(IBLayer3* ibc)
{
	IBassert(ibc->CodeBlockStack.elemCount);
	return (IBCodeBlock*) IBVectorTop(&ibc->CodeBlockStack);
}

void _IBLayer3PushCodeBlock(IBLayer3* ibc, IBCodeBlock** cbDP)
{
	IBCodeBlock* cb = 0;
	DbgPuts(" Push code block\n");
	IBVectorPush(&ibc->CodeBlockStack, &cb);
	IBCodeBlockInit(cb);
	if(cbDP) (*cbDP) = cb;
}

void _IBLayer3PopCodeBlock(IBLayer3* ibc, bool copyToParent,
	IBCodeBlock** cbDP)
{
	IBassert(ibc->CodeBlockStack.elemCount > 1);
	DbgFmt(" Pop code block. Copy to parent: %s\n", IBBoolStr(copyToParent));
	if(copyToParent)
	{
		if(ibc->CodeBlockStack.elemCount >= 2)
		{
			IBCodeBlockFinish((IBCodeBlock*) IBVectorTop(&ibc->CodeBlockStack),
				&((IBCodeBlock*) IBVectorGet(&ibc->CodeBlockStack,
											 ibc->CodeBlockStack.elemCount - 2))->code);
		}
		else Err(OP_Error, "COMPILER FAILURE. No parent code block!");
	}
	IBVectorPop(&ibc->CodeBlockStack, IBCodeBlockFree);
	if(cbDP) (*cbDP) = IBLayer3CodeBlocksTop(ibc);
}

void _IBLayer3PushTask(IBLayer3* ibc, IBOp taskOP, IBExpects** exectsDP,
	IBTask** taskDP)
{
	IBTask* t = IBLayer3GetTask(ibc), * bt = NULL;
	DbgPuts(" Push task: ");
	if(t)
	{
		DbgFmt("%s(%d) -> ", IBGetOpName(t->type), (int) t->type);
		bt = t;
	}
	DbgFmt("%s(%d)\n", IBGetOpName(taskOP), (int) taskOP);
	IBVectorPush(&ibc->TaskStack, &t);
	if(taskDP) (*taskDP) = t;
	TaskInit(t, taskOP);
	IBVectorPush(&t->expStack, exectsDP);
	if(!exectsDP)
	{
		IBExpects* exp = IBTaskGetExpTop(t);
		switch(taskOP)
		{
		case OP_NeedExpression:
		{
			IBExpectsInit(exp, "PPPPPPPP", OP_Value, OP_Name, OP_Add,
				OP_Subtract, OP_Divide, OP_Multiply, OP_Or, OP_Dot);
			break;
		}
		default:
		{
			IBExpectsInit(exp, "P", OP_Null);
			break;
		}
		}
	}
}

void _IBLayer3PopTask(IBLayer3* ibc, IBTask** taskDP, bool popToParent)
{
	IBTask* t = NULL;
	IBTask* t2 = NULL;
	IBTask copy;
	IBassert(ibc);
	t = IBLayer3GetTask(ibc);
	IBassert(t);
	DbgFmt(" Pop task %s(%d) ", IBGetOpName(t->type), (int) t->type);
	if(popToParent)
	{
		if(ibc->TaskStack.elemCount >= 2)
		{
			memcpy(&copy, t, sizeof(IBTask));
		}
		else Err(OP_Error, "COMPILER FAILURE. No parent task!");
	}
	IBVectorPop(&ibc->TaskStack, popToParent ? NULL : TaskFree);
	t2 = IBLayer3GetTask(ibc);
	IBassert(t2);
	DbgFmt("-> %s(%d) Pop to parent: %s\n",
		IBGetOpName(t2->type), (int) t2->type, IBBoolStr(popToParent));
	if(popToParent)
	{
		IBassert(t2->type != OP_RootTask);
		IBVectorCopyPush(&t2->subTasks, &copy);
	}
	if(taskDP) (*taskDP) = t2;
}

void _IBLayer3PushObj(IBLayer3* ibc, IBObj** o)
{
	IBObj* obj = IBLayer3GetObj(ibc);
	//IBassert(obj);
	DbgPuts(" Push obj: ");
	if(obj && ibc->ObjStack.elemCount)
	{
#ifdef IBDEBUGPRINTS
		ObjPrint(obj);
		DbgPuts(" -> ");
#endif
	}
	IBVectorPush(&ibc->ObjStack, &obj);
	ObjInit(obj);
#ifdef IBDEBUGPRINTS
	ObjPrint(obj);
	DbgPuts("\n");
#endif
	if(o)
	{
		(*o) = obj;
	}
}

void _IBLayer3PopObj(IBLayer3* ibc, bool pushToWorking, IBObj** objDP)
{
	IBObj* o;
	IBTask* t;
	t = IBLayer3GetTask(ibc);
	IBassert(t);
	o = IBLayer3GetObj(ibc);
	IBassert(o);
	if(pushToWorking)
	{
		IBObj* newHome;
		if(o->type == OP_NotSet)Err(OP_Error, "");
#ifdef IBDEBUGPRINTS
		DbgFmt(" To working(%s(%d)): ", IBGetOpName(t->type), (int) t->type);
#endif
#ifdef IBDEBUGPRINTS
		ObjPrint(o);
		DbgPuts("\n");
#endif
		IBVectorPush(&t->working, &newHome);
		IBassert(newHome);
		memcpy(newHome, o, sizeof(IBObj));
		if(ibc->ObjStack.elemCount < 1)
		{
			Err(OP_Error, "no obj in stack");
		}
		else if(ibc->ObjStack.elemCount == 1)
		{
			ObjInit(o);
		}
		else if(ibc->ObjStack.elemCount > 1)
		{
			IBVectorPop(&ibc->ObjStack, NULL);
			o = IBLayer3GetObj(ibc);
		}
	}
	else
	{
#ifdef IBDEBUGPRINTS
		DbgPuts("Pop obj: ");
		ObjPrint(o);
#endif
		if(ibc->ObjStack.elemCount == 1)
		{
			ObjFree(o);
			ObjInit(o);
		}
		else if(ibc->ObjStack.elemCount > 1)
		{
			printf("ec: %d\n", ibc->ObjStack.elemCount);
			IBVectorPop(&ibc->ObjStack, ObjFree);
			o = IBLayer3GetObj(ibc);
		}
		DbgPuts(" -> ");
		IBassert(ibc->ObjStack.elemCount);
#ifdef IBDEBUGPRINTS
		ObjPrint(o);
		DbgPuts("");
#endif
	}
	if(objDP) (*objDP) = o;
}

void _IBLayer3Push(IBLayer3* ibc, IBOp mode, bool strAllowSpace)
{
	IBOp cm;
	IBassert(ibc);
	cm = IBLayer3GetMode(ibc);
	ibc->StrAllowSpace = strAllowSpace;
	IBVectorCopyPushOp(&ibc->ModeStack, mode);
	DbgFmt(" Push mode: %s(%d) -> %s(%d)\n", IBGetOpName(cm), (int) cm,
		IBGetOpName(mode), (int) mode);
}

void _IBLayer3Pop(IBLayer3* ibc)
{
	IBOp type = OP_Null;
	IBOp mode = OP_Null;
	IBOp mode2 = OP_Null;
	IBTask* t = 0;
	IBExpects* exp = 0;
	exp;
	IBassert(ibc);
	t = IBLayer3GetTask(ibc);
	IBassert(t);
	mode = IBLayer3GetMode(ibc);
	IBVectorPop(&ibc->ModeStack, NULL);
	mode2 = IBLayer3GetMode(ibc);
	if(t)type = t->type;
	DbgFmt(" Pop mode: %s(%d) -> %s(%d)\n", IBGetOpName(mode), (int) mode,
		IBGetOpName(mode2), (int) mode2);
	IBassert(t->expStack.elemCount);
#ifdef IBDEBUGPRINTS
	exp = IBTaskGetExpTop(t);
	//ExpectsPrint(exp);
#endif
}

void IBLayer3PushExpects(IBLayer3* ibc, IBExpects** expDP)
{
	IBTask* t;
	IBExpects* exp;
	t = IBLayer3GetTask(ibc);
	IBassert(t);
	IBassert(expDP);
	if(expDP)
	{
		IBVectorPush(&t->expStack, &exp);
		IBassert(exp);
		(*expDP) = exp;
	}
}

void IBLayer3PopExpects(IBLayer3* ibc)
{
	IBTask* t = IBLayer3GetTask(ibc);
	IBExpects* exp;
	IBassert(t);
	exp = (IBExpects*) IBVectorTop(&t->expStack);
	IBassert(exp);
	IBVector* pfxsIb = &exp->pfxs;
	if(pfxsIb->elemCount)
	{
		IBOp* oi = NULL;
		int idx = 0;

#ifdef IBDEBUGPRINTS
		DbgPuts(" Expects POP: { ");
		idx = 0;
		while(oi = (IBOp*) IBVectorIterNext(pfxsIb, &idx))
		{
			DbgFmt("%s ", IBGetPfxName(*oi));
		}
		DbgPuts("} -> { ");
#endif
		IBVectorPop(&t->expStack, IBExpectsFree);
		if(t->expStack.elemCount < 1) Err(OP_Error, "catastrophic failure");
		pfxsIb = &((IBExpects*) IBVectorTop(&t->expStack))->pfxs;
		IBassert(pfxsIb);
#ifdef IBDEBUGPRINTS
		idx = 0;
		oi = NULL;
		while(oi = (IBOp*) IBVectorIterNext(pfxsIb, &idx))
		{
			DbgFmt("%s ", IBGetPfxName(*oi));
		}
		DbgPuts("}\n");
#endif
	}
}

void IBLayer3ReplaceExpects(IBLayer3* ibc, IBExpects** expDP)
{
	IBTask* t = IBLayer3GetTask(ibc);
	IBExpects* exp;
	IBassert(t);
	exp = IBTaskGetExpTop(t);
	IBassert(exp);
	//#ifdef IBDEBUGPRINTS
	//	PLINE;
	//	DbgFmt(" Replace expects:\n", "");
	//	ExpectsPrint(exp);
	//#endif
	IBExpectsFree(exp);
	*expDP = exp;
}

bool IBLayer3IsPfxExpected(IBLayer3* ibc, IBOp pfx)
{
	IBOp* oi = 0;
	int idx = 0;
	IBTask* t = 0;
	IBExpects* ap = 0;
	if(pfx == OP_PfxlessValue) pfx = OP_Value;
	if(pfx == OP_Letter_azAZ) return true;
	t = IBLayer3GetTask(ibc);
	IBassert(t);
	IBassert(t->expStack.elemCount);
	ap = IBTaskGetExpTop(t);
	if(ap)
	{
		while(oi = (IBOp*) IBVectorIterNext(&ap->pfxs, &idx))
		{
			IBassert(oi);
			if(oi && *oi == pfx)
			{
				return true;
			}
		}
	}
	return false;
}

bool IBLayer3IsNameOpExpected(IBLayer3* ibc, IBOp nameOp)
{
	IBOp* oi = 0;
	int idx = 0;
	IBTask* t = 0;
	IBExpects* exp = 0;
	t = IBLayer3GetTask(ibc);
	IBassert(t);
	IBassert(t->expStack.elemCount);
	exp = IBTaskGetExpTop(t);
	while(oi = (IBOp*) IBVectorIterNext(&exp->nameOps, &idx))
	{
		IBassert(oi);
		if(oi && *oi == nameOp)
		{
			return true;
		}
	}
	return false;
}

//wtf
void IBLayer3Tick(IBLayer3* ibc, FILE* f)
{
	char ch;
	if(ibc->InputStr)
	{
		IBLayer3InputStr(ibc, ibc->InputStr);
		ibc->InputStr = NULL;
	}
	else
	{
		IBassert(f);
		if((ch = (char) fgetc(f)) != EOF)
		{
			if(ch != 0xffffffff)
				IBLayer3InputChar(ibc, ch);
		}
		else
		{
			IBLayer3InputChar(ibc, '\n');
			ibc->Running = false;
		}
	}
}

/*NO NEWLINES AT END OF STR*/
void IBLayer3InputChar(IBLayer3* ibc, char ch)
{
	IBOp m = OP_Null;
	IBTask * t = 0;
	IBObj * o = 0;
	bool nl = false;
	ibc->Ch = ch;
	m = IBLayer3GetMode(ibc);
	t = IBLayer3GetTask(ibc);
	o = IBLayer3GetObj(ibc);
	
	switch(ibc->Ch)
	{
	case OP_ParenthesisOpen: //expression wrapper
	{
		break;
	}
	case OP_ParenthesisClose:
	{
		break;
	}
	case '\0': return;
	case '\n': /* \n PFXLINEEND */
	{ 
		t = IBLayer3GetTask(ibc);
		nl = true;

		switch(m)
		{
		case OP_ModeStrPass:
		{
			IBLayer3StrPayload(ibc);
			break;
		}
		}

		o = IBLayer3GetObj(ibc);
		switch(o->type)
		{
		case OP_CallWantArgs:
		{
			IBLayer3PopObj(ibc, true, NULL);
			break;
		}
		}

		t = IBLayer3GetTask(ibc);
		o = IBLayer3GetObj(ibc);
		IBassert(t->type > 0);
		switch(t->type)
		{
		case OP_NeedExpression:
		{
			IBLayer3FinishTask(ibc);
			t = IBLayer3GetTask(ibc);
			switch(t->type)
			{
			case OP_TableCaseNeedExpr:
			{
				IBExpects* exp = NULL;
				IBassert(t->subTasks.elemCount == 1);
				SetTaskType(t, OP_CaseWantCode);
				IBLayer3ReplaceExpects(ibc, &exp);
				IBExpectsInit(exp, "NNc", OP_Case, OP_Fall);
				IBLayer3PushCodeBlock(ibc, NULL);
				break;
			}
			case OP_TableNeedExpr:
			{
				IBExpects* exp = NULL;
				IBassert(t->subTasks.elemCount == 1);
				SetTaskType(t, OP_TableWantCase);
				IBLayer3ReplaceExpects(ibc, &exp);
				IBExpectsInit(exp, "PPN", OP_Op, OP_Underscore, OP_Case);
				IBLayer3PushCodeBlock(ibc, NULL);
				break;
			}
			case OP_VarNeedExpr:
			{
				IBLayer3PopObj(ibc, true, &o);
			}
			case OP_ExprToName:
			{
				IBLayer3PopObj(ibc, true, &o);
				IBLayer3FinishTask(ibc);
				break;
			}
			IBCASE_UNIMPLEMENTED
			}
			break;
		}
		IBCASE_BLOCKWANTCODE
		{
			break;
		}
		case OP_VarWantValue:
		{
			IBassert(o->type == OP_VarWantValue);
			if(ibc->DefiningStruct && ibc->DefiningStructTypeInfo)
			{
				IBTypeInfo* ti = 0;
				IBVectorPush(&ibc->DefiningStructTypeInfo->members, &ti);
				IBTypeInfoInit(ti, OP_StructVar, o->name);
				ti->StructVar.type = o->var.type;
			}
			IBLayer3PopObj(ibc, true, &o);
			IBLayer3FinishTask(ibc);
			break;
		}
		case OP_CallWantArgs:
		{
			IBLayer3FinishTask(ibc);
			t = IBLayer3GetTask(ibc);
			switch(t->type)
			{
			case OP_ActOnNameEquals:
			case OP_CallFunc:
			{
				IBLayer3FinishTask(ibc);
				break;
			}
			IBCASE_UNIMPLEMENTED
			}
			break;
		}
		case OP_IfFinished:
		{
			IBCodeBlock* cb=0;
			IBExpects* exp=0;
			IBLayer3PopObj(ibc, true, &o);
			SetTaskType(t, OP_IfBlockWantCode);
			IBLayer3ReplaceExpects(ibc, &exp);
			IBExpectsInit(exp, "PNNc",
				OP_Underscore, OP_ElseIf, OP_Else);
			IBLayer3PushCodeBlock(ibc, &cb);
			break;
		}
		case OP_StructWantRepr:
		{
			IBExpects* exp;
			SetTaskType(t, OP_StructWantContent);
			IBLayer3ReplaceExpects(ibc, &exp);
			IBExpectsInit(exp, "PPP",
				OP_Op, OP_Underscore, OP_VarType);
			break;
		}
		case OP_CPrintfHaveFmtStr:
		{
			IBLayer3FinishTask(ibc);
			break;
		}
		case OP_FuncArgsVarNeedsName:
		{
			Err(OP_FuncArgsVarNeedsName, "");
			break;
		}
		case OP_FuncNeedRetVal:
		{
			Err(OP_FuncNeedRetVal, "");
			break;
		}
		case OP_FuncSigComplete:
		case OP_FuncHasName:
		{
			o = IBLayer3GetObj(ibc);
			IBassert(o);
			SetObjType(o, OP_FuncSigComplete);
			PopExpects();
			IBLayer3PopObj(ibc, true, &o);
			if(!ibc->Imaginary)
			{
				IBExpects* exp;
				//IBCodeBlock *cb;
				IBLayer3PushExpects(ibc, &exp);
				IBExpectsInit(exp, "c", OP_Null);
				SetTaskType(t, OP_FuncWantCode);
				//IBDictManip(&cb->locals, IBDStr, )
			}
			else
			{
				IBLayer3FinishTask(ibc);
			}
			break;
		}
		}

		{
			int l = ibc->InputStr ? ibc->LineIS : ibc->Line;
			int c = ibc->InputStr ? ibc->ColumnIS : ibc->Column;
			int stripped = 0;
			IBPushColor(IBBgCYAN);
			DbgFmt("[LINE END AT %d:%d]", l, c);
			IBPopColor();
			IBPushColor(IBFgYELLOW);
			stripped = IBStrStripFront(&ibc->CurrentLineStr, '\t');
			DbgFmt(" %s", ibc->CurrentLineStr.start);
			IBPopColor();
			IBStrClear(&ibc->CurrentLineStr);
		}

		ibc->Imaginary = false;
		ibc->Pfx = OP_Null;
		ibc->DotPathOn = false;
		break;
	}
	}
	m = IBLayer3GetMode(ibc);
	if(!nl)
	{
//#ifdef IBDEBUGPRINTS
//		{
//			int l = ibc->InputStr ? ibc->LineIS : ibc->Line;
//			int c = ibc->InputStr ? ibc->ColumnIS : ibc->Column;
//			if (ibc->Ch == ' ') printf("-> SPACE (0x%x)\n", ibc->Ch);
//			else printf("-> %c (0x%x) %d:%d\n",
//				ibc->Ch, ibc->Ch, l, c);
//		}
//#endif
		switch(m)
		{
		case OP_ModePrefixPass:
		{
			IBLayer3Prefix(ibc);
			break;
		}
		case OP_ModeStrPass:
		{
			IBLayer3Str(ibc);
			break;
		}
		case OP_ModeArrayIndexExpr:
		{
			IBStr* avT = (IBStr*) IBVectorTop(&ibc->ArrayIndexExprsVec);
			if(ibc->Ch == ']')
			{
				IBObj* o = IBLayer3GetObj(ibc);
				PLINE;
				DbgFmt(" Got Array index expr: %s[%s]\n",
					ibc->Str,
					avT->start);
				if(!strncmp(avT->start, "as ", 3))
				{
					IBOp nameOP = IBGetOpFromNameList(avT->start + 3, OP_NameOps);
					switch(nameOP)
					{
					IBCASE_NUMTYPES
					{
						if(ibc->Varcast != OP_Null)
							Err(OP_Error, "");
						ibc->Varcast = nameOP;
						break;
					}
					IBCASE_UNIMPLEMENTED
					}
				}
				IBLayer3Pop(ibc);
			}
			else
				IBStrAppendCh(avT, ibc->Ch, 1);
			break;
		}
		default: Err(OP_Error, "unknown mode");
			break;
		}
	}
	if(!ibc->InputStr) ibc->Column++;
	else ibc->ColumnIS++;
	if(nl)
	{
		if(IBLayer3IsPfxExpected(ibc, OP_LineEnd) && t->expStack.elemCount > 1)
			PopExpects();
		if(!ibc->InputStr)
		{
			ibc->Column = 1;
			ibc->Line++;
		}
		else
		{
			ibc->ColumnIS = 1;
			ibc->LineIS++;
		}
	}
	ibc->LastCh = ibc->Ch;
	//DbgFmt("End of InputChar\n", 0);
}

void IBLayer3InputStr(IBLayer3* ibc, char* str)
{
	int i;
	for(i = 0; str[i] != '\0'; i++)
		IBLayer3InputChar(ibc, str[i]);
}

IBVal IBLayer3StrToVal(IBLayer3* ibc, char* str, IBOp expectedType)
{
	IBVal ret;
	ret.i32 = 0;
	switch(expectedType)
	{
	case OP_c8:
	case OP_i16:
	case OP_u32:
	case OP_u8:
	case OP_u16:
	case OP_i32:
	{
		ret.i32 = atoi(str);
		break;
	}
	case OP_i64:
	case OP_u64:
	{
		ret.u64 = atoll(str);
		break;
	}
	case OP_f32:
	case OP_d64:
	{
		ret.d64 = atof(str);
		break;
	}
	}
	return ret;
}

char* IBLayer3GetCPrintfFmtForType(IBLayer3* ibc, IBOp type)
{
	switch(type)
	{
	case OP_i32:    return "d";
	case OP_i64:    return "lld";
	case OP_u64:    return "llu";
	case OP_d64:    return "f";
	case OP_f32:    return "f";
	case OP_u32:    return "u";
	case OP_c8:
	case OP_Char:   return "c";
	case OP_String:
	case OP_Bool:   return "s";
	}
	Err(OP_Error, "GetPrintfFmtForType: unknown type");
	return "???";
}

void IBLayer3Prefix(IBLayer3* ibc)
{
	IBObj* obj;
	IBTask* t;
	IBExpects* expTop;
	t = IBLayer3GetTask(ibc);
	IBassert(t);
	expTop = IBTaskGetExpTop(t);

	ibc->Pfx = IBOPFromPfxCh(ibc->Ch);
	if(ibc->Pfx == OP_SpaceChar
		|| ibc->Pfx == OP_TabChar) return;
	if(ibc->Pfx == OP_Unknown)
		Err(OP_ErrUnknownPfx, "catastrophic err");
	obj = IBLayer3GetObj(ibc);
	if(ibc->Pfx != OP_Unknown
		&& (!t || expTop->pfxs.elemCount)
		&& !IBLayer3IsPfxExpected(ibc, ibc->Pfx))
		Err(OP_ErrUnexpectedNextPfx, "");
	IBPushColor(IBBgMAGENTA);
	//PFX:
	DbgPuts("PFX");
	IBPopColor();
	DbgPuts(": ");
	IBPushColor(IBBgBROWN);
	DbgFmt("%s(%d)", IBGetPfxName(ibc->Pfx), (int) ibc->Pfx);
	IBPopColor();
	DbgPuts("\n");
	switch(ibc->Pfx)
	{
	case OP_PfxlessValue:
	{
		char chBuf[2];
		chBuf[0] = ibc->Ch;
		chBuf[1] = '\0';
		ibc->Pfx = OP_Value;
		StrConcat(ibc->Str, IBLayer3STR_MAX, chBuf);
		break;
	}
	}
	switch(ibc->Pfx)
	{
	case OP_SingleQuote: /* ' */
	case OP_String: /* " */
	{
		ibc->StringMode = true;
		IBLayer3Push(ibc, OP_ModeStrPass, false);
		break;
	}
	case OP_Letter_azAZ:
	{
		char chBuf[2];
		chBuf[0] = ibc->Ch;
		chBuf[1] = '\0';
		//ibc->Pfx = OP_Op;
		StrConcat(ibc->Str, IBLayer3STR_MAX, chBuf);
		IBLayer3Push(ibc, OP_ModeStrPass, false);
		IBVectorCopyPushBool(&ibc->StrReadPtrsStack, true);
		break;
	}
	case OP_VarType:
	{
		//TODO: move this into func sig task
		IBVectorCopyPushBool(&ibc->StrReadPtrsStack, true);
	}
	case OP_LessThan:
	case OP_GreaterThan:
	case OP_Or:
	case OP_Add:
	case OP_Subtract:
	case OP_Multiply:
	case OP_Divide:
	case OP_Dot: //context aware
	case OP_Caret:
	case OP_Underscore:
	case OP_BracketOpen:
	//case OP_BracketClose:
	case OP_CurlyBraceOpen:
	//case OP_CurlyBraceClose:
	case OP_Comma:
	case OP_Exclaim:
	case OP_Value:
	case OP_Op:
	case OP_Name:
	{
		IBVectorCopyPushBool(&ibc->StrReadPtrsStack, true);
		switch(ibc->Pfx)
		{
		case OP_Exclaim:
		case OP_Name:
		{
			ibc->DotPathOn = true;		
			IBVectorClear(&ibc->DotPathVec, IBStrFree);
		}
		}
		/*getchar();*/
		IBLayer3Push(ibc, OP_ModeStrPass, false);
	}
	case OP_SpaceChar:
		break;
	}
	if(ibc->Pfx == OP_Op)
	{
		if(expTop && expTop->life && --expTop->life <= 0)
		{
			IBVectorPop(&t->expStack, IBExpectsFree);
		}
	}
}

void IBLayer3Str(IBLayer3* ibc)
{
	char chBuf[2];
	chBuf[0] = ibc->Ch;
	chBuf[1] = '\0';
	if(ibc->StringMode)
	{
		switch(ibc->Ch)
		{
		case '\''://'
		case '\"'://"
		{
			ibc->StringMode = false;
			IBLayer3StrPayload(ibc);
			return;
		}
		}
	}
	else
	{
		switch(ibc->Pfx)
		{
		case OP_Name:
		{
			switch(ibc->Ch)
			{
			case '[':
			{
				//IBStrClear(&ibc->ArrayIndexExprStr);
				IBStr* aT = NULL;
				IBVectorPush(&ibc->ArrayIndexExprsVec, &aT);
				IBStrInit(aT);
				IBLayer3Push(ibc, OP_ModeArrayIndexExpr, false);
				return;
			}
			case ']':
			{
				if(IBLayer3GetMode(ibc) == OP_ModeArrayIndexExpr)
					IBLayer3Pop(ibc);
				return;
			}
			case '_':
			case ' ':
			case '.':
			IBCASE_0THRU9
			IBCASE_aTHRUz
			IBCASE_ATHRUZ
				break;
			default:
				ErrF(OP_NotAllowed, "character \'%c\' not allowed in name", ibc->Ch);
			}
			break;
		}
		/*case OP_Value:
		{
			switch (ibc->Ch)
			{
			case '@':
			{
				IBLayer3Pop(ibc);
				IBLayer3Prefix(ibc);
				return;
			}
			}
			break;
		}*/
		}
		switch(ibc->Ch)
		{
		case '\t': return;
		case ' ':
		{
			if(ibc->StrAllowSpace) break;
			else
			{
				IBLayer3StrPayload(ibc);
				return;
			}
		}
		case '^':
		{
			if(*(bool*) IBVectorTop(&ibc->StrReadPtrsStack))
			{
				switch(ibc->Pointer)
				{
				case OP_NotSet:
					DbgPuts("Got pointer\n");
					ibc->Pointer = OP_Pointer;
					break;
				case OP_Pointer:
					DbgPuts("Got double pointer\n");
					ibc->Pointer = OP_DoublePointer;
					break;
				case OP_DoublePointer:
					DbgPuts("Got tripple pointer\n");
					ibc->Pointer = OP_TripplePointer;
					break;
				case OP_TripplePointer:
					Err(OP_ErrQuadriplePointersNOT_ALLOWED, "");
					break;
				}
				return;
			}
		}
		}
	}
	StrConcat(ibc->Str, IBLayer3STR_MAX, chBuf);
}

IBTask* IBLayer3GetTask(IBLayer3* ibc)
{
	IBTask* ret = (IBTask*) IBVectorTop(&ibc->TaskStack);
	//if (!ret)Err(OP_Error, "no task in stack");
	if(ret)
	{
		IBassert(ret->type >= 0);
	}
	return ret;
}

IBOp IBLayer3GetMode(IBLayer3* ibc)
{
	IBOp ret = OP_Null;
	IBOp* p = (IBOp*) IBVectorTop(&ibc->ModeStack);
	if(p)ret = *p;
	return ret;
}

void IBLayer3ExplainErr(IBLayer3* ibc, IBOp code)
{
	switch(code)
	{
	case OP_TCC_Error:
	{
		printf("Fatal internal compiler error");
		break;
	}
	case OP_YouCantUseThatHere:
	{
		printf("You can't use that here");
		break;
	}
	case OP_FuncNeedRetVal:
	{
		printf("You forgot to return a value from the function");
		break;
	}
	case OP_ErrUnexpectedNameOP:
	{
		IBExpects* exp;
		IBTask* t;
		t = IBLayer3GetTask(ibc);
		IBassert(t);
		exp = IBTaskGetExpTop(t);
		//IBPushColor(IBBgBLUE | IBBgGREEN);
		printf("NameOP \"@%s\" wasn't expected.\nExpected:\n",
			ibc->Str);
		IBExpectsPrint(exp);
		//IBPopColor();
		break;
	}
	case OP_ErrUnknownPfx:
		printf("This prefix \"%c\" is unknown!", ibc->Ch);
		break;
	case OP_ErrUnknownOpStr:
		printf("Unknown OP str @%s\n", ibc->Str);
		break;
	case OP_ErrQuadriplePointersNOT_ALLOWED:
		printf("quadruple pointer is not allowed");
		break;
	case OP_ErrUnexpectedOp:
		printf("Unexpected OP");
		break;
	case OP_ErrNoTask:
		printf("No working task to call done (@@) for");
		break;
	case OP_ErrUnexpectedNextPfx:
	{
		IBOp* oi;
		IBExpects* ap;
		IBTask* t;
		int idx;
		t = IBLayer3GetTask(ibc);
		ap = IBTaskGetExpTop(t);
		if(ap && t)
		{
			IBassert(ap->pfxs.elemCount);
			printf("Err: \"%s\" Unexpected next prefix %s. "
				"Pfx idx:%d\nEnforced at line %d. Allowed:",
				ap->pfxErr, IBGetPfxName(ibc->Pfx),
				ap->pfxs.elemCount - 1, ap->lineNumInited);
			idx = 0;
			while(oi = (IBOp*) IBVectorIterNext(&ap->pfxs, &idx))
			{
				printf("%s(%d),", IBGetPfxName(*oi), (int) *oi);
			}
		}
		else
		{
			printf("No task. Only Op(@) prefix allowed. "
				"Pfx: %s. Allowed pfxs: %s\n",
				IBGetPfxName(ibc->Pfx), IBGetPfxName(OP_Op));
		}
		break;
	}
	case OP_ErrExpectedVariablePfx:
		printf("Expected a variable type to be next.");
		break;
	default:
		printf("Err msg unimplemented for %s", IBGetOpName(code));
	}
#ifdef IBDEBUGPRINTS
	DbgPuts("\nOBJ:");
	ObjPrint(IBLayer3GetObj(ibc));
	//DbgFmt("\n","");
#endif
	printf("\n");
}
