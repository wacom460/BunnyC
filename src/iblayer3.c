#include "imbored.h"

IBObj* IBLayer3GetObj(IBLayer3* ibc)
{
	return (IBObj*) IBVectorTop(&ibc->ObjStack);
}

//$structObj.memberVar
IBNameInfo* IBLayer3TryFindNameInfoInStructVar(IBLayer3* ibc, IBNameInfo* ni)
{
	if(ibc->DotPathVec.elemCount >= 2) {
		IBStr* first = (IBStr*)IBVectorGet(&ibc->DotPathVec, 0);
		IBNameInfo* sni = IBNameInfoFindMember(ni, first->start);
		if(sni && sni->ti && sni->ti->members.elemCount) {
			for(int i = 1; i < ibc->DotPathVec.elemCount; i++) {
				IBStr* ds = (IBStr*) IBVectorGet(&ibc->DotPathVec, i);
				IBNameInfo* mvNi = (IBNameInfo*) IBVectorGet(&sni->members, i - 1);
				if(!strcmp(ds->start, mvNi->name)) {
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
	while(idx >= 0) {
		IBCodeBlock* cb = (IBCodeBlock*) IBVectorGet(&ibc->CodeBlockStack, idx);
		IBassert(cb);
		IB_ASSERTMAGICP(&cb->localVariables.members);
		ni = IBNameInfoFindMember(&cb->localVariables, name);
		if(ni) return ni;
		else ni = IBLayer3TryFindNameInfoInStructVar(ibc, &cb->localVariables);
		idx--;
	}
	ni = IBNameInfoFindMember(&ibc->GlobalVariables, name);
	if(!ni) ni = IBLayer3TryFindNameInfoInStructVar(ibc, &ibc->GlobalVariables);
	return ni;
}

IBObj* IBLayer3FindStackObjRev(IBLayer3* ibc, IBOp type)
{
	IBassert(ibc);
	IBassert(type > 0);
	for(int i = ibc->ObjStack.elemCount - 1; i >= 0; i--) {
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
	for(i = index - 1; i >= 0;) {
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
	for(i = ibc->ObjStack.elemCount - 1; i >= 0;) {
		o = (IBObj*) IBVectorGet(&ibc->ObjStack, i--);
		if(o->type == type) return o;
	}
	return NULL;
}

void IBLayer3Init(IBLayer3* ibc)
{
	IBObj* o = 0;
	IBExpects* exp = 0;
	IBCodeBlock* cb = 0;
	IBTypeInfo* boolti = 0, * u8ti = 0, * i8ti = 0, * c8ti = 0, * u16ti = 0,
		* i16ti = 0, * u32ti = 0, * i32ti = 0, * f32ti = 0,
		* u64ti = 0, * i64ti = 0, * d64ti = 0, * stringti = 0;
	memset(ibc, 0, sizeof * ibc);

	IBVectorInit(&ibc->DotPathVec, sizeof(IBStr),
		OP_IBStr, IBVEC_DEFAULT_SLOTCOUNT);

	IBVectorInit(&ibc->TypeRegistry,
		sizeof(IBTypeInfo), OP_IBTypeInfo, 32);
	IBVectorPush(&ibc->TypeRegistry, &boolti);
	IBVectorPush(&ibc->TypeRegistry, &u8ti);
	IBVectorPush(&ibc->TypeRegistry, &i8ti);
	IBVectorPush(&ibc->TypeRegistry, &c8ti);
	IBVectorPush(&ibc->TypeRegistry, &u16ti);
	IBVectorPush(&ibc->TypeRegistry, &i16ti);
	IBVectorPush(&ibc->TypeRegistry, &u32ti);
	IBVectorPush(&ibc->TypeRegistry, &i32ti);
	IBVectorPush(&ibc->TypeRegistry, &f32ti);
	IBVectorPush(&ibc->TypeRegistry, &u64ti);
	IBVectorPush(&ibc->TypeRegistry, &i64ti);
	IBVectorPush(&ibc->TypeRegistry, &d64ti);
	IBVectorPush(&ibc->TypeRegistry, &stringti);
	IBTypeInfoInit(boolti, OP_Bool, "bool");
	IBTypeInfoInit(u8ti, OP_u8, "u8");
	IBTypeInfoInit(i8ti, OP_i8, "i8");
	IBTypeInfoInit(c8ti, OP_c8, "c8");
	IBTypeInfoInit(u16ti, OP_u16, "u16");
	IBTypeInfoInit(i16ti, OP_i16, "i16");
	IBTypeInfoInit(u32ti, OP_u32, "u32");
	IBTypeInfoInit(i32ti, OP_i32, "i32");
	IBTypeInfoInit(f32ti, OP_f32, "f32");
	IBTypeInfoInit(u64ti, OP_u64, "u64");
	IBTypeInfoInit(i64ti, OP_i64, "i64");
	IBTypeInfoInit(d64ti, OP_d64, "d64");
	IBTypeInfoInit(stringti, OP_String, "nts");
	//getchar();
	//IBLayer3RegisterCustomType(ibc,"ct",OP_Enum,NULL);

	ibc->Running = true;
	ibc->Line = 1;
	ibc->Column = 1;
	ibc->LineIS = 1;
	ibc->ColumnIS = 1;
	IBStrInit(&ibc->RunArguments);
	IBStrInit(&ibc->CHeader_Structs);
	IBStrInit(&ibc->CHeader_Funcs);
	IBStrInit(&ibc->CurrentLineStr);
	IBStrInit(&ibc->CIncludesStr);
	IBStrInit(&ibc->ibFileNameStr);
	IBStrInit(&ibc->CCode);
	IBStrInit(&ibc->FinalOutput);
	IBStrInit(&ibc->CFileStr);
	IBStrInit(&ibc->HFileStr);
	IBVectorInit(&ibc->ArrayIndexExprsVec, sizeof(IBStr), OP_IBStr, IBVEC_DEFAULT_SLOTCOUNT);
	ibc->Pointer = OP_NotSet;
	ibc->Privacy = OP_Public;
	ibc->CommentMode = OP_NotSet;
	IBNameInfoInit(&ibc->GlobalVariables);
	IBStrInit(&ibc->CurSpace);
	IBVectorInit(&ibc->ObjStack, sizeof(IBObj), OP_Obj, IBVEC_DEFAULT_SLOTCOUNT);
	IBVectorInit(&ibc->ModeStack, sizeof(IBOp), OP_Op, IBVEC_DEFAULT_SLOTCOUNT);
	IBVectorInit(&ibc->StrReadPtrsStack, sizeof(bool), OP_Bool, IBVEC_DEFAULT_SLOTCOUNT);
	IBVectorInit(&ibc->TaskStack, sizeof(IBTask), OP_Task, IBVEC_DEFAULT_SLOTCOUNT);
	IBVectorInit(&ibc->CodeBlockStack,
		sizeof(IBCodeBlock), OP_IBCodeBlock, IBVEC_DEFAULT_SLOTCOUNT);
	IBVectorInit(&ibc->ExpressionStack,
		sizeof(IBExpression), OP_IBExpression, IBVEC_DEFAULT_SLOTCOUNT);
	IBVectorPush(&ibc->CodeBlockStack, &cb);
	IBCodeBlockInit(cb);
	IBVectorCopyPushBool(&ibc->StrReadPtrsStack, false);
	IBLayer3Push(ibc, OP_ModePrefixPass, false);
	IBLayer3PushObj(ibc, &o);
	IBLayer3PushTask(ibc, OP_RootTask, &exp, NULL);
	IBExpectsInit(exp, "PPPNNNNNNNNNNNN",
		OP_Op, OP_VarType, OP_Subtract, OP_Use, OP_Imaginary, OP_Func,
		OP_Enum, OP_Flags, OP_Struct, OP_Methods, OP_Space, OP_Public,
		OP_Private, OP_RunArguments, OP_CInclude
	);
}

void IBLayer3Free(IBLayer3* ibc)
{
	IBTask* t;
	IBObj* o;
	IBCodeBlock* cb;
	IBStr rootCbFinal;

	IBassert(ibc);
	IBStrInit(&rootCbFinal);
	if(ibc->InputStr) {
		IBLayer3InputStr(ibc, ibc->InputStr);
		ibc->InputStr = NULL;
	}
	cb = (IBCodeBlock*) IBVectorTop(&ibc->CodeBlockStack);
	o = IBLayer3GetObj(ibc);
	if(ibc->StringMode)
		Err(OP_Error, "Reached end of file without closing string");
	if(ibc->Str[0]) IBLayer3StrPayload(ibc);
	if(cb->localVariables.members.elemCount)
		Err(OP_Error, "root codeblock can't have variables in it!!!");
	t = IBLayer3GetTask(ibc);
	if(ibc->TaskStack.elemCount) {
		switch(t->type) {
		case OP_FuncWantCode:
		case OP_FuncNeedRetVal:
			Err(OP_Error, "Reached end of file without closing function");
			break;
		case OP_FuncSigComplete:
		case OP_FuncHasName: {
			SetObjType(o, OP_FuncSigComplete);
			IBLayer3PopObj(ibc, true, NULL);
			IBLayer3FinishTask(ibc);
			break;
		}
		}
	}
	if(t->type != OP_RootTask)Err(OP_ErrDirtyTaskStack,
		"Reached end of file not at root task");
	if(ibc->CodeBlockStack.elemCount != 1)
		Err(OP_Error, "dirty codeblock stack");
	IBCodeBlockFinish(cb, &rootCbFinal);
	/*if (IBStrLen(&cb->variables) +
			IBStrLen(&cb->code) +
			IBStrLen(&cb->footer))
		Err(OP_Error, "dirty codeblock. expected root codeblock to be empty");*/
		//IBStrAppendCStr(&ibc->CHeader_Funcs, "\n#endif\n");
	IBStrAppendFmt(&ibc->CFileStr,
			"//#include \"%s.h\"\n", ibc->ibFileNameStr.start);
	if(ibc->IncludeCStdioHeader)
		IBStrAppendFmt(&ibc->CFileStr,
			"#include <stdio.h>\n");
	if(ibc->IncludeCStdlibHeader)
		IBStrAppendFmt(&ibc->CFileStr,
			"#include <stdlib.h>\n");
	IBStrAppendFmt(&ibc->HFileStr, "#pragma once\n\n%s%s%s%s",
		ibc->CIncludesStr.start,
		rootCbFinal.start,
		ibc->CHeader_Structs.start,
		ibc->CHeader_Funcs.start);
	IBStrAppendFmt(&ibc->CFileStr, "\n%s", ibc->CCode.start);
	IBStrAppendFmt(&ibc->FinalOutput, "%s\n%s", ibc->HFileStr.start, ibc->CFileStr.start);
#ifdef IBDEBUGPRINTS
	IBPushColor(IBFgWHITE);
	IBPushColor(IBFgGREEN);
	DbgFmt("%s.h file: \n", ibc->ibFileNameStr.start);
	IBPopColor();
	DbgFmt("%s\n", ibc->HFileStr.start);
	IBPushColor(IBFgGREEN);
	DbgFmt("%s.c file: \n", ibc->ibFileNameStr.start);
	IBPopColor();
	DbgFmt("%s", ibc->CFileStr.start);
#else
	printf("%s", ibc->FinalOutput.start);
#endif
	IBPopColor();
	IBLayer3CompileTCC(ibc);
	IBPushColor(IBFgMAGENTA);
	DbgPuts("-> Compilation complete. Press any key <-\n");
	IBPopColor();
#ifdef IBDEBUGPRINTS
	getchar();
#endif
	/*if (ibc->SpaceNameStr != NULL) {
		free(ibc->SpaceNameStr);
		ibc->SpaceNameStr = NULL;
	}*/
	IBStrFree(&rootCbFinal);
	IBNameInfoFree(&ibc->GlobalVariables);
	IBStrFree(&ibc->CurSpace);
	IBVectorFree(&ibc->CodeBlockStack, IBCodeBlockFree);
	IBVectorFree(&ibc->ObjStack, ObjFree);
	IBVectorFreeSimple(&ibc->ModeStack);
	IBVectorFreeSimple(&ibc->ExpressionStack);
	IBVectorFreeSimple(&ibc->StrReadPtrsStack);
	IBVectorFree(&ibc->TaskStack, TaskFree);
	//IBNameInfoDBFree(&ibc->NameTypeCtx);
	IBStrFree(&ibc->CHeader_Structs);
	IBStrFree(&ibc->CHeader_Funcs);
	IBStrFree(&ibc->FinalOutput);
	IBStrFree(&ibc->CFileStr);
	IBStrFree(&ibc->HFileStr);
	IBStrFree(&ibc->RunArguments);
	IBStrFree(&ibc->CIncludesStr);
	IBStrFree(&ibc->ibFileNameStr);
	IBStrFree(&ibc->CurrentLineStr);
	IBVectorFree(&ibc->TypeRegistry, IBTypeInfoFree);
	IBVectorFree(&ibc->DotPathVec, IBStrFree);
	//IBStrFree(&ibc->ArrayIndexExprStr);
	IBVectorFree(&ibc->ArrayIndexExprsVec, IBStrFree);
	IBStrFree(&ibc->CCode);
}

void IBLayer3RegisterCustomType(IBLayer3* ibc, char* name, IBOp type, IBTypeInfo** outDP)
{
	IBTypeInfo* ti = 0;
	IBASSERT0(ibc);
	IBASSERT0(name);
	IBASSERT0(type > 0);
	IBLayer3FindType(ibc, name, &ti);
	if(ti) {
		ErrF(OP_AlreadyExists, "type %s already exists", name);
		return;
	}
	switch(type) {
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
	while(ti = IBVectorIterNext(&ibc->TypeRegistry, &idx)) {
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
	for(i = index - 1; i >= 0 && limit--;) {
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
	if(argsLen) {
		IBStr ls;
		IBStrInit(&ls);
		for(int i = 0; i < argsLen; ++i) {
			char ch = ibc->RunArguments.start[i];
			if(ch == ' ') {
				IBOverwriteStr(&argv[argc], ls.start);
				IBStrClear(&ls);
				++argc;
				continue;
			}
			IBStrAppendCh(&ls, ch, 1);
		}
		if(IBStrLen(&ls)) {
			IBOverwriteStr(&argv[argc], ls.start);
			++argc;
		}
		IBStrFree(&ls);
	}
	if(Entry) {
		int entryRet = 0;
		DbgPuts("Program output:\n");
		entryRet = Entry(argc, argv);
		IBPushColor(IBFgGREEN);
		DbgFmt("\n\nmain() returned %d.\n", entryRet);
		IBPopColor();
	}
	for(int i = 0; i < IBRUN_MAXARGS; ++i) {
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
	switch(t->type) {
	case OP_CaseWantCode: {
		IBLayer3FinishTask(ibc);
		t = IBLayer3GetTask(ibc);
		IBassert(t->type == OP_TableWantCase);
		IBLayer3FinishTask(ibc);
		break;
	}
	case OP_StructWantContent: {
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
	case OP_MethodsWantContent: {
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
	case OP_IfBlockWantCode: {
		IBLayer3FinishTask(ibc);
		break;
	}
	case OP_Func:
	case OP_FuncHasName:
	case OP_FuncWantCode: {
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
		while(o = (IBObj*) IBVectorIterNext(wo, &idx)) {
			/*TODO: could cache func obj index later*/
			if(o->type == OP_FuncSigComplete) {
				if(o->func.retValType != OP_Void) {
					IBExpects* exp;
					IBLayer3PushExpects(ibc, &exp);
					/*if (o->func.retType == OP_c8 && o->func.retTypeMod == OP_Pointer) {
						ExpectsInit(exp, "PP", OP_Name, OP_String);
					}
					else {

					}*/
					IBExpectsInit(exp, "PPP", OP_Value, OP_Name, OP_String);
					SetTaskType(t, OP_FuncNeedRetVal);
				}
				else {
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
	for(int i = t->working.elemCount - 1; i >= 0; i--) {
		IBObj* o = (IBObj*) IBVectorGet(&t->working, i);
		IBassert(o);
		if(o->type == type)return o;
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
	for(i = index - 1; i >= 0;) {
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
	if(copyToParent) {
		if(ibc->CodeBlockStack.elemCount >= 2) {
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
	if(t) {
		DbgFmt("%s(%d) -> ", IBGetOpName(t->type), (int) t->type);
		bt = t;
	}
	DbgFmt("%s(%d)\n", IBGetOpName(taskOP), (int) taskOP);
	IBVectorPush(&ibc->TaskStack, &t);
	if(taskDP) (*taskDP) = t;
	TaskInit(t, taskOP);
	IBVectorPush(&t->expStack, exectsDP);
	if(!exectsDP) {
		IBExpects* exp = IBTaskGetExpTop(t);
		switch(taskOP) {
		case OP_NeedExpression: {
			IBExpectsInit(exp, "PPPPPPPP", OP_Value, OP_Name, OP_Add,
				OP_Subtract, OP_Divide, OP_Multiply, OP_Or, OP_Dot);
			break;
		}
		default: {
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
	if(popToParent) {
		if(ibc->TaskStack.elemCount >= 2) {
			memcpy(&copy, t, sizeof(IBTask));
		}
		else Err(OP_Error, "COMPILER FAILURE. No parent task!");
	}
	IBVectorPop(&ibc->TaskStack, popToParent ? NULL : TaskFree);
	t2 = IBLayer3GetTask(ibc);
	IBassert(t2);
	DbgFmt("-> %s(%d) Pop to parent: %s\n",
		IBGetOpName(t2->type), (int) t2->type, IBBoolStr(popToParent));
	if(popToParent) {
		IBassert(t2->type != OP_RootTask);
		IBVectorCopyPush(&t2->subTasks, &copy);
	}
	if(taskDP) (*taskDP) = t2;
}

void _IBLayer3PushObj(IBLayer3* ibc, IBObj** o) {
	IBObj* obj = IBLayer3GetObj(ibc);
	//IBassert(obj);
	DbgPuts(" Push obj: ");
	if(obj && ibc->ObjStack.elemCount) {
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
	if(o) { (*o) = obj; }
}

void _IBLayer3PopObj(IBLayer3* ibc, bool pushToWorking, IBObj** objDP)
{
	IBObj* o;
	IBTask* t;
	t = IBLayer3GetTask(ibc);
	IBassert(t);
	o = IBLayer3GetObj(ibc);
	IBassert(o);
	if(pushToWorking) {
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
		if(ibc->ObjStack.elemCount < 1) {
			Err(OP_Error, "no obj in stack");
		}
		else if(ibc->ObjStack.elemCount == 1) {
			ObjInit(o);
		}
		else if(ibc->ObjStack.elemCount > 1) {
			IBVectorPop(&ibc->ObjStack, NULL);
			o = IBLayer3GetObj(ibc);
		}
	}
	else {
#ifdef IBDEBUGPRINTS
		DbgPuts("Pop obj: ");
		ObjPrint(o);
#endif
		if(ibc->ObjStack.elemCount == 1) {
			ObjFree(o);
			ObjInit(o);
		}
		else if(ibc->ObjStack.elemCount > 1) {
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
	if(expDP) {
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
	if(pfxsIb->elemCount) {
		IBOp* oi = NULL;
		int idx = 0;

#ifdef IBDEBUGPRINTS
		DbgPuts(" Expects POP: { ");
		idx = 0;
		while(oi = (IBOp*) IBVectorIterNext(pfxsIb, &idx)) {
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
		while(oi = (IBOp*) IBVectorIterNext(pfxsIb, &idx)) {
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
	IBOp* oi;
	int idx;
	IBTask* t;
	IBExpects* ap;
	if(pfx == OP_PfxlessValue) pfx = OP_Value;
	if(pfx == OP_Letter_azAZ || pfx == OP_Comment) return true;
	t = NULL;
	ap = NULL;
	t = IBLayer3GetTask(ibc);
	IBassert(t);
	IBassert(t->expStack.elemCount);
	idx = 0;
	oi = NULL;
	ap = IBTaskGetExpTop(t);
	if(ap) while(oi = (IBOp*) IBVectorIterNext(&ap->pfxs, &idx)) {
		IBassert(oi);
		if(oi && *oi == pfx)
			return true;
	}
	return false;
}

bool IBLayer3IsNameOpExpected(IBLayer3* ibc, IBOp nameOp)
{
	IBOp* oi;
	int idx;
	IBTask* t;
	IBExpects* exp;
	t = IBLayer3GetTask(ibc);
	IBassert(t);
	IBassert(t->expStack.elemCount);
	idx = 0;
	oi = NULL;
	exp = IBTaskGetExpTop(t);
	while(oi = (IBOp*) IBVectorIterNext(&exp->nameOps, &idx)) {
		IBassert(oi);
		if(oi && *oi == nameOp)
			return true;
	}
	return false;
}

//wtf
void IBLayer3Tick(IBLayer3* ibc, FILE* f)
{
	char ch;
	if(ibc->InputStr) {
		IBLayer3InputStr(ibc, ibc->InputStr);
		ibc->InputStr = NULL;
	}
	else {
		IBassert(f);
		if((ch = (char) fgetc(f)) != EOF) {
			if(ch != 0xffffffff)
				IBLayer3InputChar(ibc, ch);
		}
		else {
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
	if(ibc->CommentMode == OP_NotSet && ibc->Ch != IBCOMMENT_CHAR)
		IBStrAppendCh(&ibc->CurrentLineStr, ibc->Ch, 1);
	m = IBLayer3GetMode(ibc);
	t = IBLayer3GetTask(ibc);
	o = IBLayer3GetObj(ibc);
	if(ibc->LastCh == IBCOMMENT_CHAR &&
		ibc->Ch == IBCOMMENT_CHAR_OPEN)
	{
		IBLayer3Push(ibc, OP_ModeMultiLineComment, false);
		ibc->CommentMode = OP_MultiLineComment;
		ibc->Ch = '\0';
	}
	else if(ibc->CommentMode == OP_MultiLineComment &&
		ibc->LastCh == IBCOMMENT_CHAR_CLOSE &&
		ibc->Ch == IBCOMMENT_CHAR)
	{
		IBLayer3Pop(ibc);
		ibc->CommentMode = OP_NotSet;
		ibc->Ch = '\0';
	}
	//if(ibc->CommentMode==OP_NotSet&&
	//	ibc->Ch==IBCOMMENT_CHAR/*&&
	//	ibc->LastCh!=IBCOMMENT_CHAR*/)
	//{
	//	/*PLINE;
	//	DbgFmt(" LINE COMMENT ON\n","");*/
	//	ibc->CommentMode = OP_Comment;
	//	IBLayer3Push(ibc, OP_ModeComment, false);
	//}else if(ibc->CommentMode==OP_Comment&&
	//		ibc->LastCh==ibc->Ch &&
	//			!ibc->StringMode
	//			&&ibc->Ch==IBCOMMENT_CHAR&&
	//			m==OP_ModeComment)
	//{
	//	/*PLINE;
	//	DbgFmt(" MULTI COMMENT ON!!!!!!\n","");*/
	//	IBLayer3Pop(ibc);
	//	IBLayer3Push(ibc, OP_ModeMultiLineComment, false);
	//	ibc->CommentMode = OP_MultiLineComment;
	//	ibc->Ch='\0';
	//}else if(ibc->CommentMode==OP_MultiLineComment&&
	//	ibc->LastCh==ibc->Ch &&
	//			!ibc->StringMode
	//			&&ibc->Ch==IBCOMMENT_CHAR&&
	//			m==OP_ModeMultiLineComment) {
	//	/*PLINE;
	//	DbgFmt(" MULTI COMMENT OFF!\n","");*/
	//	ibc->CommentMode=OP_NotSet;
	//}
	switch(ibc->Ch) {
	case OP_ParenthesisOpen: {//expression wrapper
		break;
	}
	case OP_ParenthesisClose: {
		break;
	}
	case '\0': return;
	case '\n': { /* \n PFXLINEEND */
		t = IBLayer3GetTask(ibc);
		nl = true;
		if(ibc->CommentMode == OP_Comment) {
			IBLayer3Pop(ibc);
			ibc->CommentMode = OP_NotSet;
		}

		switch(m) {
		case OP_ModeStrPass: {
			IBLayer3StrPayload(ibc);
			break;
		}
		}

		o = IBLayer3GetObj(ibc);
		switch(o->type) {
		case OP_CallWantArgs: {
			IBLayer3PopObj(ibc, true, NULL);
			break;
		}
		}

		t = IBLayer3GetTask(ibc);
		o = IBLayer3GetObj(ibc);
		IBassert(t->type > 0);
		switch(t->type) {
		case OP_NeedExpression: {
			IBLayer3FinishTask(ibc);
			t = IBLayer3GetTask(ibc);
			switch(t->type) {
			case OP_TableCaseNeedExpr: {
				IBExpects* exp = NULL;
				IBassert(t->subTasks.elemCount == 1);
				SetTaskType(t, OP_CaseWantCode);
				IBLayer3ReplaceExpects(ibc, &exp);
				IBExpectsInit(exp, "NNc", OP_Case, OP_Fall);
				IBLayer3PushCodeBlock(ibc, NULL);
				break;
			}
			case OP_TableNeedExpr: {
				IBExpects* exp = NULL;
				IBassert(t->subTasks.elemCount == 1);
				SetTaskType(t, OP_TableWantCase);
				IBLayer3ReplaceExpects(ibc, &exp);
				IBExpectsInit(exp, "PPN", OP_Op, OP_Underscore, OP_Case);
				IBLayer3PushCodeBlock(ibc, NULL);
				break;
			}
			case OP_VarNeedExpr: {
				IBLayer3PopObj(ibc, true, &o);
			}
			case OP_ExprToName: {
				IBLayer3FinishTask(ibc);
				break;
			}
			IBCASE_UNIMPLEMENTED
			}
			break;
		}
		IBCASE_BLOCKWANTCODE {
			break;
		}
		case OP_VarWantValue: {
			IBassert(o->type == OP_VarWantValue);
			if(ibc->DefiningStruct && ibc->DefiningStructTypeInfo)
			{
				IBTypeInfo* ti = 0;
				IBVectorPush(&ibc->DefiningStructTypeInfo->members, &ti);
				IBTypeInfoInit(ti, OP_StructVar, o->name);
				ti->StructVar.type = o->var.type;
				ti->StructVar.privacy = o->var.privacy;
			}
			IBLayer3PopObj(ibc, true, &o);
			IBLayer3FinishTask(ibc);
			break;
		}
		case OP_CallWantArgs: {
			IBLayer3FinishTask(ibc);
			t = IBLayer3GetTask(ibc);
			switch(t->type) {
			case OP_ActOnNameEquals:
			case OP_CallFunc: {
				IBLayer3FinishTask(ibc);
				break;
			}
			IBCASE_UNIMPLEMENTED
			}
			break;
		}
		case OP_IfFinished: {
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
		case OP_StructWantRepr: {
			IBExpects* exp;
			SetTaskType(t, OP_StructWantContent);
			IBLayer3ReplaceExpects(ibc, &exp);
			IBExpectsInit(exp, "PPP",
				OP_Op, OP_Underscore, OP_VarType);
			break;
		}
		case OP_CPrintfHaveFmtStr: {
			IBLayer3FinishTask(ibc);
			break;
		}
		case OP_FuncArgsVarNeedsName: {
			Err(OP_FuncArgsVarNeedsName, "");
			break;
		}
		case OP_FuncNeedRetVal: {
			Err(OP_FuncNeedRetVal, "");
			break;
		}
		case OP_FuncSigComplete:
		case OP_FuncHasName: {
			o = IBLayer3GetObj(ibc);
			IBassert(o);
			SetObjType(o, OP_FuncSigComplete);
			PopExpects();
			IBLayer3PopObj(ibc, true, &o);
			if(!ibc->Imaginary) {
				IBExpects* exp;
				//IBCodeBlock *cb;
				IBLayer3PushExpects(ibc, &exp);
				IBExpectsInit(exp, "c", OP_Null);
				SetTaskType(t, OP_FuncWantCode);
				//IBDictManip(&cb->locals, IBDStr, )
			}
			else {
				IBLayer3FinishTask(ibc);
			}
			break;
		}
		}
		/*if (ibc->CommentMode == OP_NotSet)*/ {
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
	if(!nl /*&& ibc->CommentMode == OP_NotSet*/) {
		//#ifdef IBDEBUGPRINTS
		//		{
		//			int l = ibc->InputStr ? ibc->LineIS : ibc->Line;
		//			int c = ibc->InputStr ? ibc->ColumnIS : ibc->Column;
		//			if (ibc->Ch == ' ') printf("-> SPACE (0x%x)\n", ibc->Ch);
		//			else printf("-> %c (0x%x) %d:%d\n",
		//				ibc->Ch, ibc->Ch, l, c);
		//		}
		//#endif
		switch(m) {
		case OP_ModeComment:
		case OP_ModeMultiLineComment:
			break;
		case OP_ModePrefixPass: {
			IBLayer3Prefix(ibc);
			break;
		}
		case OP_ModeStrPass: {
			IBLayer3Str(ibc);
			break;
		}
		case OP_ModeArrayIndexExpr: {
			IBStr* avT = (IBStr*) IBVectorTop(&ibc->ArrayIndexExprsVec);
			if(ibc->Ch == ']') {
				IBObj* o = IBLayer3GetObj(ibc);
				PLINE;
				DbgFmt(" Got Array index expr: %s[%s]\n",
					ibc->Str,
					avT->start);
				if(!strncmp(avT->start, "as ", 3)) {
					IBOp nameOP = IBGetOpFromNameList(avT->start + 3, OP_NameOps);
					switch(nameOP) {
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
	if(nl) {
		if(IBLayer3IsPfxExpected(ibc, OP_LineEnd) && t->expStack.elemCount > 1)
			PopExpects();
		if(!ibc->InputStr) {
			ibc->Column = 1;
			ibc->Line++;
		}
		else {
			ibc->ColumnIS = 1;
			ibc->LineIS++;
		}
	}
	ibc->LastCh = ibc->Ch;
	//if(m==OP_ModeMultiLineComment&&ibc->CommentMode==OP_NotSet){
	//	IBLayer3Pop(ibc);
	//}
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
	switch(expectedType) {
	case OP_c8:
	case OP_i16:
	case OP_u32:
	case OP_u8:
	case OP_u16:
	case OP_i32: { ret.i32 = atoi(str); break; }
	case OP_i64:
	case OP_u64: { ret.u64 = atoll(str); break; }
	case OP_f32:
	case OP_d64: { ret.d64 = atof(str); break; }
	}
	return ret;
}

char* IBLayer3GetCPrintfFmtForType(IBLayer3* ibc, IBOp type)
{
	switch(type) {
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

void _IBLayer3FinishTask(IBLayer3* ibc)
{
	IBVector* wObjs;
	IBCodeBlock* cb;
	int tabCount;
	IBTask* t;
	bool pop2Parent = false;
	t = IBLayer3GetTask(ibc);
	IBassert(t);
	DbgFmt(" FinishTask: %s(%d)\n", IBGetOpName(t->type), (int) t->type);
	if(!ibc->TaskStack.elemCount)Err(OP_ErrNoTask, "task stack EMPTY!");
	wObjs = &t->working;
	IBassert(wObjs);
	if(!wObjs->elemCount) {/*Err(OP_Error, "workingObjs EMPTY!");*/
		DbgPuts("Warning: working objs for this task is empty!\n");
	}
	cb = IBLayer3CodeBlocksTop(ibc);
	tabCount = IBLayer3GetTabCount(ibc);
	switch(t->type) {
	case OP_MethodsWantContent: {
		IBObj* o = IBLayer3FindWorkingObjRev(ibc, OP_Methods);
		IBassert(o);
		break;
	}
	case OP_EnumWantContent: {
		IBTypeInfo* ti = 0;
		int idx = 0;
		int flagsI = 0;
		IBObj* o = (IBObj*) IBVectorFront(&t->working);
		IBObj* eo = o;
		bool oneFound = false;
		IBassert(o);
		if(!eo || !eo->name || *eo->name == '\0')
			Err(OP_Error, "enum needs a name");
		IBLayer3FindType(ibc, eo->name, &ti);
		IBASSERT0(!ti);
		if(ti) ErrF(OP_AlreadyExists, "type %s already exists", eo->name);
		IBLayer3RegisterCustomType(ibc, eo->name, OP_Enum, &ti);
		ti->Enum.isFlags = eo->enumO.flags;
		IBStrAppendFmt(&t->code.header, "enum E%s {\n", eo->name);
		IBStrAppendFmt(&t->code.footer, "};\n\n", eo->name);
		while(o = (IBObj*) IBVectorIterNext(wObjs, &idx)) {
			switch(o->type) {
			case OP_Enum: break;
			case OP_EnumName: {
				IBTypeInfo* nti = 0;
				IBVectorPush(&ti->members, &nti);
				IBTypeInfoInit(nti, OP_EnumVal, o->name);
				oneFound = true;
				IBStrAppendFmt(&t->code.code, "\tE%s_%s", eo->name, o->name);
				if(eo->enumO.flags) {
					nti->EnumValue.val = flagsI;
					IBStrAppendFmt(&t->code.code, " = %d", flagsI);
					flagsI *= 2;
					if(flagsI == 0) flagsI = 2;
				}
				else {
					nti->EnumValue.val = ti->members.elemCount - 1;
				}
				IBStrAppendFmt(&t->code.code, "%s\n", idx == wObjs->elemCount ? "" : ",");
				break;
			}
			IBCASE_UNIMPLEMENTED
			}
		}
		if(!oneFound) Err(OP_Error, "need at least one case in enum");
		IBCodeBlockFinish(&t->code, &ibc->CHeader_Structs);
		break;
	}
	case OP_CaseWantCode: {
		IBStr fo;
		IBTask* st;
		IBObj* to = IBLayer3FindWorkingObj(ibc, OP_TableCase);
		IBassert(to);
		IBassert(t->subTasks.elemCount == 1);
		st = (IBTask*) IBVectorGet(&t->subTasks, 0);
		IBassert(st);
		IBStrInit(&fo);
		IBCodeBlockFinish(&st->code, &fo);
		IBStrAppendCh(&cb->header, '\t', tabCount - 2);
		IBStrAppendFmt(&cb->header, "case %s: {\n", fo.start);
		if(to->table.fallthru == false) {
			IBStrAppendCh(&cb->footer, '\t', tabCount - 1);
			IBStrAppendCStr(&cb->footer, "break;\n");
		}
		IBStrAppendCh(&cb->footer, '\t', tabCount - 2);
		IBStrAppendFmt(&cb->footer, "}\n");
		IBLayer3PopCodeBlock(ibc, true, &cb);
		IBStrFree(&fo);
		break;
	}
	case OP_TableWantCase: {
		IBStr fo;
		IBTask* st;
		IBassert(t->subTasks.elemCount == 1);
		st = (IBTask*) IBVectorGet(&t->subTasks, 0);
		IBStrInit(&fo);
		IBCodeBlockFinish(&st->code, &fo);
		IBStrAppendCh(&cb->header, '\t', tabCount - 1);
		IBStrAppendFmt(&cb->header, "switch (%s) {\n", fo.start);
		IBStrAppendCh(&cb->footer, '\t', tabCount - 1);
		IBStrAppendFmt(&cb->footer, "}\n");
		IBLayer3PopCodeBlock(ibc, true, &cb);
		IBStrFree(&fo);
		break;
	}
	case OP_ExprToName: /*{

		break;
	}*/
	case OP_ActOnNameEquals: {
		IBTask* st;
		IBStr fc;
		IBObj* o = (IBObj*) IBVectorGet(wObjs, 0);
		IBassert(t->subTasks.elemCount == 1);
		st = (IBTask*) IBVectorGet(&t->subTasks, 0);
		IBassert(st);
		IBassert(o);
		IBassert(o->type == OP_ActOnName);
		IBStrInit(&fc);
		IBCodeBlockFinish(&st->code, &fc);
		IBStrAppendCh(&cb->code, '\t', tabCount);
		if(!strncmp(o->name, "self.", 5)) {
			IBStrAppendFmt(&cb->code, "self->%s = %s;\n", o->name + 5, fc.start);
		}
		else {
			IBStrAppendFmt(&cb->code, "%s = %s;\n", o->name, fc.start);
		}
		break;
	}
	case OP_NeedExpression: {
		int idx = 0;
		IBObj* o = NULL;
		bool onOp = false;
		bool gotVal = false;
		if(wObjs->elemCount < 1)Err(OP_Error, "empty expression!");
		pop2Parent = true;
		while(o = (IBObj*) IBVectorIterNext(wObjs, &idx)) {
			switch(o->type) {
			case OP_Or:
			case OP_Multiply:
			case OP_Divide:
			case OP_Subtract:
			case OP_Add: {
				if(!gotVal)Err(OP_Error, "missing op lval in expression");
				onOp = true;
				gotVal = false;
				IBStrAppendFmt(&t->code.code, " %s ", IBGetCEqu(o->type));
				break;
			}
			case OP_Name: {
				IBNameInfo* ni = IBLayer3SearchNameInfo(ibc, o->name);
				IBOp ceq = o->modifier == OP_Pointer ? OP_Ref
					: OP_None;
				if(!ni) {
					ErrF(OP_NotFound, "%s wasn't found", o->name);
				}
				else {
					switch(ni->type) {
						CASE_VALTYPES
						{
							gotVal = true;
							break;
						}
							IBCASE_UNIMPLEMENTED
					}
				}
				IBStrAppendFmt(&t->code.code, "%s%s", IBGetCEqu(ceq), o->name);
				if(onOp) {
					IBStrAppendFmt(&t->code.header, "%s", "(");
					IBStrAppendFmt(&t->code.code, "%s", ")");
				}
				onOp = false;
				break;
			}
			case OP_EnumVal: {
				gotVal = true;
				IBStrAppendFmt(&t->code.code, "E%s_%s", o->str, o->name);
				if(onOp) {
					IBStrAppendFmt(&t->code.header, "%s", "(");
					IBStrAppendFmt(&t->code.code, "%s", ")");
				}
				onOp = false;
				break;
			}
			case OP_StructVar: {
				gotVal = true;
				IBStrAppendFmt(&t->code.code, "%s%s_%s", "", o->str, o->name);
				break;
			}
			//lots of overlap with Val2Str...
			case OP_Value: {
				gotVal = true;
				switch(o->valType) {
				case OP_Double: {
					IBStrAppendFmt(&t->code.code, "%f", o->val.d64);
					break;
				}
				case OP_Float: {
					IBStrAppendFmt(&t->code.code, "%f", o->val.f32);
					break;
				}
				//case OP_Value:
				case OP_Number: {
					IBStrAppendFmt(&t->code.code, "%d", o->val.i32);
					break;
				}
				case OP_String: {

					break;
				}
				case OP_Bool: {
					IBStrAppendFmt(&t->code.code, "%s", o->val.boolean > 0 ? "1" : "0");
					break;
				}
				IBCASE_UNIMPLEMENTED
				}
				if(onOp) {
					IBStrAppendFmt(&t->code.header, "%s", "(");
					IBStrAppendFmt(&t->code.code, "%s", ")");
				}
				onOp = false;
				break;
			}
						 IBCASE_UNIMPLEMENTED
			}
		}
		if(onOp) Err(OP_Error, "missing op rval in expression");
		break;
	}
	case OP_VarNeedExpr: {
		IBTask* st;
		IBTypeInfo* ti = 0;
		//IBObj* o = IBVectorGet(wObjs, 0);
		//IBassert(o);
		IBassert(t->subTasks.elemCount == 1);
		st = (IBTask*) IBVectorGet(&t->subTasks, 0);
		IBassert(st);

		IBassert(st->type == OP_NeedExpression);
		IBassert(!ibc->DefiningStruct);
		IBStrAppendCh(&cb->variables, '\t', tabCount);
		IBObj* vo = 0;
		TaskFindWorkingObj(t, OP_VarNeedExpr, &vo);
		IBObj* aonWo = 0;
		/*TaskFindWorkingObj(t, OP_ActOnName, &aonWo);
		IBassert(vo);*/
		char* vn = vo->str != NULL ? vo->str/* : aonWo != NULL ? aonWo->name*/ : NULL;
		IBLayer3FindType(ibc, vn, &ti);
		IBassert(ti);
		IBStr stf;
		IBStrInit(&stf);
		IBCodeBlockFinish(&st->code, &stf);
		char* typeStr = vo->var.type == OP_Unknown ? vo->str : IBGetCEqu(vo->var.type);
		IBStrAppendFmt(&cb->variables, "%s%s%s %s = %s;\n", ti->type == OP_Enum ? "enum E"
			: "",
			typeStr, IBGetCEqu(vo->var.mod), vo->name, stf.start);
		IBStrFree(&stf);
		break;
	}
	case OP_VarWantValue: {
		int idx = 0;
		IBObj* o = NULL;
		bool thing = false;
		IBTask* parent =
			IBLayer3FindTaskUnderIndex(ibc, -1, OP_StructWantContent, 1);
		//IBassert(parent);
		if(parent && parent->type == OP_StructWantContent) {
			thing = true;
			pop2Parent = true;
		}
		IBStr* vstr = thing ? &parent->code.code
			: &IBLayer3CodeBlocksTop(ibc)->variables;
		while(o = (IBObj*) IBVectorIterNext(wObjs, &idx)) {
			switch(o->type) {
			case OP_VarComplete:
			case OP_VarWantValue: {
				IBTypeInfo* ti = 0;
				IBStrAppendCh(vstr, '\t', thing ? 1 : tabCount);
				if(o->var.type == OP_String) {
					o->var.type = OP_c8;
					o->var.mod = OP_Pointer;
				}
				char* typeStr = o->var.type == OP_Unknown ? o->str : IBGetCEqu(o->var.type);
				char* typePfx = "";
				IBLayer3FindType(ibc, typeStr, &ti);
				if(ti) {
					switch(ti->type) {
					case OP_Enum:
						typePfx = "enum E";
						break;
					case OP_Struct:
						typePfx = "struct S";
						break;
					}
				}
				IBStrAppendFmt(vstr, "%s%s%s %s", typePfx, typeStr, IBGetCEqu(o->var.mod), o->name);
				if(o->var.valSet && !thing) {
					IBStrAppendCStr(vstr, " = ");
					switch(o->var.type) {
					case OP_i8:
					case OP_i16:
					case OP_i64:
					case OP_i32: {
						IBStrAppendFmt(vstr, "%d", o->var.val.i32);
						break;
					}
					case OP_d64: {
						IBStrAppendFmt(vstr, "%f", o->var.val.d64);
						break;
					}
					case OP_f32: {
						IBStrAppendFmt(vstr, "%f", o->var.val.f32);
						break;
					}
					case OP_Bool: {
						IBStrAppendFmt(vstr, "%s", IBBoolStrChar(o->var.val.boolean));
						break;
					}
					case OP_c8: {
						if(o->var.mod == OP_Pointer) {
							IBStrAppendFmt(vstr, "\"%s\"", o->var.valStrLiteral);
						}
						else {
							IBStrAppendFmt(vstr, "\'%c\'", o->var.val.c8);
						}
						break;
					}
					case OP_u8: {
						IBStrAppendFmt(vstr, "%u", o->var.val.u8);
						break;
					}
					case OP_u16: {
						IBStrAppendFmt(vstr, "%u", o->var.val.u16);
						break;
					}
					case OP_u32: {
						IBStrAppendFmt(vstr, "%u", o->var.val.u32);
						break;
					}
					case OP_u64: {
						IBStrAppendFmt(vstr, "%llu", o->var.val.u64);
						break;
					}
							   IBCASE_UNIMPLEMENTED
					}
				}
				IBStrAppendFmt(vstr, "%s\n", ";");
				break;
			}
			}
		}
		break;
	}
	case OP_CallFunc: {
		IBTask* st = NULL;
		/*int idx = 0;
		while (st = IBVectorIterNext(&t->subTasks, &idx)) {

		}*/
		IBassert(t->subTasks.elemCount == 1);
		st = (IBTask*) IBVectorGet(&t->subTasks, 0);
		IBassert(st);
		IBStrAppendCh(&cb->code, '\t', tabCount);
		IBCodeBlockFinish(&st->code, &cb->code);
		IBStrAppendFmt(&cb->code, "%s\n", ";");
		break;
	}
	case OP_CallWantArgs: {
		IBObj* o = (IBObj*) IBVectorGet(wObjs, 0);
		int idx = 0;
		IBassert(o->type == OP_Call);
		//IBStrAppendCh(&t->code.code, '\t', tabCount);
		IBStrAppendFmt(&t->code.code, "%s", o->str);
		IBStrAppendCStr(&t->code.code, "(");
		while(o = (IBObj*) IBVectorIterNext(wObjs, &idx)) {
			if(o->type != OP_Arg) continue;
			switch(o->valType) {
			case OP_Value: {
				IBStrAppendFmt(&t->code.code, "%llu", o->val.i64);
				break;
			}
			case OP_String: {
				IBStrAppendFmt(&t->code.code, "\"%s\"", o->str);
				break;
			}
			case OP_Name: {
				IBStrAppendFmt(&t->code.code, "%s", o->str);
				break;
			}
						IBCASE_UNIMPLEMENTED
			}
			if(idx <= wObjs->elemCount - 1) {
				IBStrAppendCStr(&t->code.code, ", ");
			}
		}
		IBStrAppendCStr(&t->code.code, ")");
		pop2Parent = true;
		break;
	}
	case OP_BlockReturnNeedValue: {
		IBObj* o = 0;
		IBLayer3PopObj(ibc, true, &o);
		o = (IBObj*) IBVectorTop(wObjs);
		IBStrAppendCh(&cb->code, '\t', tabCount);
		IBStrAppendFmt(&cb->code, "return ");
		IBassert(o->type == OP_BlockReturnNeedValue);
		switch(o->valType) {
		case OP_Value: {
			IBStrAppendFmt(&cb->code, "%llu", o->val.i64);
			break;
		}
		case OP_String: {
			IBStrAppendFmt(&cb->code, "\"%s\"", o->str);
			break;
		}
		case OP_Name: {
			IBStrAppendFmt(&cb->code, "%s", o->str);
			break;
		}
		}
		IBStrAppendCStr(&cb->code, ";\n");
		break;
	}
	case OP_IfBlockWantCode: {
		IBObj* o = 0, * m = 0;
		int idx = 0;
		while(o = (IBObj*) IBVectorIterNext(wObjs, &idx)) {
			if(o->type == OP_IfFinished) {
				m = o;
				break;
			}
		}
		IBassert(m);
		if(ibc->LastNameOp != OP_ElseIf) {
			IBStrAppendCh(&cb->header, '\t', tabCount - 1);
			ibc->LastNameOp = OP_Null;
		}
		IBStrAppendFmt(&cb->header, "if (");
		switch(m->ifO.lvTYPE) {
		case OP_Name:
			IBStrAppendFmt(&cb->header, "%s ", m->ifO.lvName);
			break;
		default: {
			char buf[64];
			buf[0] = '\0';
			Val2Str(buf, 64, m->ifO.lvVal, m->ifO.lvTYPE);
			IBStrAppendFmt(&cb->header, "%s ", buf);
			break;
		}
		}
		IBStrAppendFmt(&cb->header, "%s ", IBGetCEqu(m->ifO.midOp));
		switch(m->ifO.rvTYPE) {
		case OP_Name:
			IBStrAppendFmt(&cb->header, "%s", m->ifO.rvName);
			break;
		default: {
			char buf[64];
			buf[0] = '\0';
			Val2Str(buf, 64, m->ifO.rvVal, m->ifO.rvTYPE);
			IBStrAppendFmt(&cb->header, "%s", buf);
			break;
		}
		}
		IBStrAppendFmt(&cb->header, ") ");
	}
	case OP_LoopBlockWantCode: {
		if(t->type == OP_LoopBlockWantCode) {
			IBStrAppendCh(&cb->header, '\t', tabCount - 1);
			IBStrAppendCStr(&cb->header, "while(1) ");
		}
	}
	case OP_BlockWantCode: {
		IBStrAppendFmt(&cb->header, "{\n");
		IBStrAppendCh(&cb->footer, '\t', tabCount - 1);
		IBStrAppendFmt(&cb->footer, "}\n");
		IBLayer3PopCodeBlock(ibc, true, &cb);
		break;
	}
	case OP_StructWantContent: {
		IBStr header;
		IBStr body;
		IBStr footer;
		IBStr hFile;
		IBStr cFile;
		IBObj* o;
		//IBTask* st;
		int idx;
		IBTypeInfo* ti = 0;

		IBStrInit(&header);
		IBStrInit(&body);
		IBStrInit(&footer);
		IBStrInit(&hFile);
		IBStrInit(&cFile);
		//idx = 0;
		//while (st = (IBTask*)IBVectorIterNext(&t->subTasks, &idx)) {
		//	switch (st->type) {
		//		case OP_VarWantValue: {
		//			IBObj* o = (IBObj*)IBVectorFront(&st->working);
		//			IBassert(o);
		//			IBStrAppendFmt(&body,
		//				"\t%s%s %s;\n",
		//				IBGetCEqu(o->var.type),
		//				IBGetCEqu(o->var.mod),
		//				o->name);
		//			//default values will be stored in db
		//			break;
		//		}
		//		IBCASE_UNIMPLEMENTED
		//	}
		//}
		IBCodeBlockFinish(&t->code, &body);
		idx = 0;
		while(o = IBVectorIterNext(wObjs, &idx)) {
			switch(o->type) {
			case OP_Struct: {
				IBassert(o->name);
				IBassert(*o->name);

				IBLayer3FindType(ibc, o->name, &ti);
				IBASSERT0(ti);

				IBStrAppendFmt(&header, "struct S%s {\n", o->name);
				IBStrAppendFmt(&footer, "};\n\n", o->name);

				break;
			}
						  IBCASE_UNIMPLEMENTED
			}
		}
		IBStrAppend(&ibc->CHeader_Structs, &header);
		IBStrAppend(&ibc->CHeader_Structs, &body);
		IBStrAppend(&ibc->CHeader_Structs, &footer);
		//IBStrAppendFmt(&ibc->CHeaderStructs, "%s%s%s", header, body, footer);
		IBStrFree(&header);
		IBStrFree(&body);
		IBStrFree(&footer);

		{
			int idx = 0;
			IBNameInfo* ni = 0;
			while(ni = (IBNameInfo*) IBVectorIterNext(&cb->localVariables.members, &idx)) {
				IBTypeInfo* nti = 0;
				IBVectorPush(&ti->members, &nti);
				IBTypeInfoInit(nti, OP_StructVar, ni->name);
				nti->structVarType = ni->ti;
			}
		}
		IBLayer3PopCodeBlock(ibc, false, &cb);
		break;
	}
	case OP_SpaceHasName: {
		IBObj* o = NULL;
		int idx = 0;
		while(o = (IBObj*) IBVectorIterNext(wObjs, &idx))
			if(o->type == OP_Space) break;
		IBassert(o);
		if(o)
			IBStrReplaceWithCStr(&ibc->CurSpace, o->name);
		break;
	}
	case OP_FuncWantCode:
	case OP_FuncSigComplete:
	case OP_FuncHasName:
	case OP_Func: {
		IBTypeInfo* ti = 0;
		IBObj* o;
		int idx;
		int i;
		int argc;
		IBStr cFuncModsTypeName;
		IBStr cFuncArgsThing;
		IBStr cFuncArgs;
		IBStr cFuncArgsEnd;
		IBStr cFuncCode;
		IBObj* funcObj;
		IBObj* thingObj;

		thingObj = NULL;
		argc = 0;
		IBStrInit(&cFuncModsTypeName);
		IBStrInit(&cFuncArgsThing);
		IBStrInit(&cFuncArgs);
		IBStrInit(&cFuncArgsEnd);
		IBStrInit(&cFuncCode);
		idx = 0;
		funcObj = NULL;
		for(i = 0; i < wObjs->elemCount; ++i) {
			o = (IBObj*) IBVectorGet(wObjs, i);
			switch(o->type) {
			case OP_FuncArgComplete: {/*multiple allowed*/
				IBOp at;
				at = o->arg.type;
				argc++;
				if(at == OP_Null)Err(OP_Error, "arg type NULL");

				if(cFuncArgs.start[0] != '\0') {
					IBStrAppendCStr(&cFuncArgs, ", ");
				}
				IBStrAppendCStr(&cFuncArgs, IBGetCEqu(o->arg.type));
				IBStrAppendCStr(&cFuncArgs, IBGetCEqu(o->arg.mod));
				IBStrAppendCStr(&cFuncArgs, " ");
				if(!o->name)Err(OP_Error, "arg name NULL");
				IBStrAppendCStr(&cFuncArgs, o->name);
				break;
			}
			case OP_Func:
			case OP_FuncHasName:
			case OP_FuncSigComplete:
			case OP_CompletedFunction: {/*should only happen once*/
				IBOp mod;
				funcObj = o;
				mod = o->modifier;
				if(mod != OP_NotSet) {
					IBStrAppendCStr(&cFuncModsTypeName, IBGetCEqu(mod));
					IBStrAppendCStr(&cFuncModsTypeName, " ");
				}
				IBStrAppendCStr(&cFuncModsTypeName, IBGetCEqu(o->func.retValType));
				IBStrAppendCStr(&cFuncModsTypeName, IBGetCEqu(o->func.retTypeMod));
				IBStrAppendCStr(&cFuncModsTypeName, " ");
				if(!o->name)Err(OP_Error, "func name NULL");
				IBTypeInfo* mti = 0;
				char mtiC = '\0';
				if(o->name) {
					if(o->func.thingTask)//stupid
					{
						IBObj* wo;
						int idx;
						idx = 0;
						wo = IBLayer3FindStackObjUnderTop(ibc, OP_Struct);
						if(wo) {
							IBStrAppendFmt(&cFuncModsTypeName, "S%s_", wo->name);
							thingObj = wo;
						}
					}
					IBObj* mo = IBLayer3FindStackObjRev(ibc, OP_Methods);
					if(mo) {
						IBLayer3FindType(ibc, mo->name, &mti);
						IBassert(mti);
						switch(mti->type) {
						case OP_Enum: {
							IBStrAppendCh(&cFuncModsTypeName, 'E', 1);
							mtiC = 'E';
							break;
						}
						case OP_Struct: {
							IBStrAppendCh(&cFuncModsTypeName, 'S', 1);
							mtiC = 'S';
							break;
						}
									  IBCASE_UNIMPLEMENTED
						}
						IBStrAppendFmt(&cFuncModsTypeName, "%s_", mo->name);
					}
					IBStrAppendCStr(&cFuncModsTypeName, o->name);

					IBASSERT0(!ti);
					IBLayer3FindType(ibc, o->name, &ti);
					IBASSERT0(!ti);
					IBLayer3RegisterCustomType(ibc, o->name, OP_Func, &ti);
					IBASSERT0(ti);
					ti->Function.isMethod = ibc->DefiningStruct;
				}
				IBStrAppendCStr(&cFuncModsTypeName, "(");
				if(thingObj) {
					IBStrAppendFmt(&cFuncArgsThing,
						"struct S%s* self", thingObj->name);
				}
				if(mti) {
					switch(mtiC) {
					case 'E': {
						IBStrAppendFmt(&cFuncArgsThing,
							"enum %c%s* self", mtiC, mti->name.start);
						break;
					}
					case 'S': {
						IBStrAppendFmt(&cFuncArgsThing,
							"struct %c%s* self", mtiC, mti->name.start);
						break;
					}
							IBCASE_UNIMPLEMENTED
					}
				}
				break;
			}
			}
		}
		/*idx = 0;
		while (o= (IBObj*)IBVectorIterNext(wObjs,&idx)) {
			switch (o->type) {
			case OP_VarComplete:
			case OP_VarWantValue: {
				char valBuf[32];
				valBuf[0] = '\0';
				IBStrAppendCStr(&cFuncCode, "\t");
				IBStrAppendCStr(&cFuncCode, IBGetCEqu(o->var.type));
				IBStrAppendCStr(&cFuncCode, IBGetCEqu(o->var.mod));
				IBStrAppendCStr(&cFuncCode, " ");
				if (!o->name)Err(OP_Error, "var name NULL");
				IBStrAppendCStr(&cFuncCode, o->name);
				IBStrAppendCStr(&cFuncCode, " = ");
				if (o->var.valSet) {
					Val2Str(valBuf, 32, o->var.val, o->var.type);
					IBStrAppendCStr(&cFuncCode, valBuf);
				}
				else {
					IBStrAppendCStr(&cFuncCode, "0");
				}
				IBStrAppendCStr(&cFuncCode, ";\n");
				break;
			}
			}
		}*/
		if(ibc->Imaginary) {
			//DbgFmt("[@ext @func]","");
			IBStrAppendCStr(&cFuncArgsEnd, ");\n\n");
			IBLayer3PopCodeBlock(ibc, false, &cb);
		}
		else {
			//DbgFmt("[@func]", "");
			IBStr cbOut;
			IBCodeBlock* cb;
			IBStrAppendCStr(&cFuncArgsEnd, ") {\n");
			IBStrInit(&cbOut);
			cb = IBLayer3CodeBlocksTop(ibc);
			IBCodeBlockFinish(cb, &cbOut);
			IBStrAppend(&cFuncCode, &cbOut);
			IBStrFree(&cbOut);
			IBLayer3PopCodeBlock(ibc, false, &cb);
			//no longer valid bcuz u can define methods in structs
			//IBassert(ibc->CodeBlockStack.elemCount == 1);
			if(!funcObj) {
				Err(OP_Error, "funcObj NULL");
			}
			else if(funcObj->func.retValType != OP_Void) {
				IBStrAppendCStr(&cFuncCode, "\treturn ");
				switch(funcObj->func.retValVarcast) {
					IBCASE_NUMTYPES
					{
						IBStrAppendFmt(&cFuncCode,"(%s) ",
							IBGetCEqu(funcObj->func.retValVarcast));
						break;
					}
				case OP_Null:break;
					IBCASE_UNIMPLEMENTED
				}
				switch(funcObj->func.retTYPE) {
				case OP_String: {
					IBStrAppendFmt(&cFuncCode, "\"%s\"", funcObj->func.retValStr);
					break;
				}
				case OP_Value: {
					//IBassert(funcObj->func.retValType==OP_i32);
					//IBStrAppendFmt(&cFuncCode, "%d",
					//	funcObj->func.retVal.i32);//for now
					switch(funcObj->func.retValType) {
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
						char valBuf[32];
						valBuf[0] = '\0';
						Val2Str(valBuf, 32, funcObj->func.retVal, funcObj->func.retValType);
						IBStrAppendCStr(&cFuncCode, valBuf);
						break;
					}
							   IBCASE_UNIMPLEMENTED
					}
					break;
				}
				case OP_Name: {
					IBStrAppendFmt(&cFuncCode, "%s", funcObj->func.retStr);
					break;
				}
							IBCASE_UNIMPLEMENTED
				}
				//IBStrAppendCStr(&cFuncCode, valBuf);
				IBStrAppendCStr(&cFuncCode, ";\n");
			}
			IBStrAppendCStr(&cFuncCode, "}\n\n");
		}
		if(funcObj && strcmp(funcObj->name, "main"))
		{
			IBStrAppendCStr(&ibc->CHeader_Funcs, cFuncModsTypeName.start);
			IBStrAppendCStr(&ibc->CHeader_Funcs, cFuncArgsThing.start);
			if(argc && IBStrLen(&cFuncArgsThing)) IBStrAppendCStr(&ibc->CHeader_Funcs, ", ");
			IBStrAppendCStr(&ibc->CHeader_Funcs, cFuncArgs.start);
			IBStrAppendCStr(&ibc->CHeader_Funcs, ");\n");
		}
		if(!ibc->Imaginary) {
			IBStrAppendCStr(&ibc->CCode, cFuncModsTypeName.start);
			IBStrAppendCStr(&ibc->CCode, cFuncArgsThing.start);
			if(argc && IBStrLen(&cFuncArgsThing)) IBStrAppendCStr(&ibc->CCode, ", ");
			IBStrAppendCStr(&ibc->CCode, cFuncArgs.start);
			IBStrAppendCStr(&ibc->CCode, cFuncArgsEnd.start);
			IBStrAppendCStr(&ibc->CCode, cFuncCode.start);
		}
		IBStrFree(&cFuncModsTypeName);
		IBStrFree(&cFuncArgsThing);
		IBStrFree(&cFuncArgs);
		IBStrFree(&cFuncArgsEnd);
		IBStrFree(&cFuncCode);
		break;
	}
	case OP_CPrintfHaveFmtStr: {
		bool firstPercent;
		IBObj* fmtObj;
		int varIdx;
		int i;
		if(t && wObjs->elemCount) {
			fmtObj = (IBObj*) wObjs->data;
			IBStrAppendCh(&cb->code, '\t', tabCount);
			IBStrAppendCStr(&cb->code, "printf(\"");
			ibc->IncludeCStdioHeader = true;
			firstPercent = false;
			varIdx = 1;
			for(i = 0; i < (int) strlen(fmtObj->str); ++i) {
				char c;
				c = fmtObj->str[i];
				switch(c) {
				case '%': {
					if(!firstPercent) {
						IBStrAppendCStr(&cb->code, "%");
						firstPercent = true;
					}
					else {
						IBObj* vo;
						IBOp voT;
						vo = (IBObj*) IBVectorGet(wObjs, varIdx);
						/*printf("cfmt vidx:%d\n",varIdx);*/
						IBassert(vo);
						voT = vo->type;
						/*if(voT==OP_String)DB*/
						switch(voT) {
						case OP_Name: {
							IBNameInfo* ni = IBLayer3SearchNameInfo(ibc, vo->name);
							if(!ni)
								Err(OP_Error, "Name not found");
							IBassert(ni);
							IBStrAppendCStr(&cb->code,
								IBLayer3GetCPrintfFmtForType(ibc, ni->type));
							break;
						}
						case OP_String:
							IBassert(vo->var.type == OP_String);
						case OP_Value: {
							IBStrAppendCStr(&cb->code,
								IBLayer3GetCPrintfFmtForType(ibc, vo->var.type));
							break;
						}
						case OP_Arg: {
							//IBLayer3VecPrint(wObjs);
							IBassert(0);
						}
						case OP_CPrintfFmtStr: break;
						default: {
							Err(OP_Error, "unhandled printf arg type");
						}

						}
						firstPercent = false;
						varIdx++;
					}
					break;
				}
				default: {
					char chBuf[2];
					chBuf[0] = c;
					chBuf[1] = '\0';
					IBStrAppendCStr(&cb->code, chBuf);
					break;
				}
				}
			}
			IBStrAppendCStr(&cb->code, "\"");
			if(wObjs->elemCount > 1) {
				IBStrAppendCStr(&cb->code, ", ");
			}
			for(i = 1; i < wObjs->elemCount; ++i) {
				IBObj* o;
				o = (IBObj*) IBVectorGet(wObjs, i);
				switch(o->type) {
				case OP_Name: {
					IBNameInfo* ni = IBLayer3SearchNameInfo(ibc, o->name);
					if(ni->type == OP_Bool)
						IBStrAppendFmt(&cb->code, "%s ? \"true\" : \"false\"", o->name);
					else {
						IBStr* ibs = NULL;
						int idx = 0;

						IBStrAppendCStr(&cb->code, o->name);
						while(ibs = IBVectorIterNext(&o->arg.arrIndexExprs, &idx)) {
							IBStrAppendFmt(&cb->code, "[%s]",
								ibs->start);
						}
					}
					break;
				}
				case OP_String: {
					IBStrAppendCStr(&cb->code, "\"");
					IBStrAppendCStr(&cb->code, o->str);
					IBStrAppendCStr(&cb->code, "\"");
					break;
				}
				case OP_Value: {
					char valBuf[32];
					valBuf[0] = '\0';
					Val2Str(valBuf, 32, o->val, o->var.type);
					IBStrAppendCStr(&cb->code, valBuf);
					break;
				}
				}
				if(i < wObjs->elemCount - 1) {
					IBStrAppendCStr(&cb->code, ", ");
				}
			}
			IBStrAppendCStr(&cb->code, ");\n");
		}
		break;
	}
	}
	IBLayer3PopTask(ibc, &t, pop2Parent);
}

void IBLayer3Prefix(IBLayer3* ibc)
{
	IBObj* obj;
	IBTask* t;
	IBExpects* expTop;
	t = IBLayer3GetTask(ibc);
	IBassert(t);
	expTop = IBTaskGetExpTop(t);
	///*for assigning func call ret val to var*/
	//if (ibc->Pfx == OP_Value && ibc->Ch == '@'
	//		&& !ibc->Str[0]) {
	//	IBExpects* exp;
	//	IBLayer3PushExpects(ibc, &exp);
	//	ExpectsInit(exp, "P0", OP_Op, 1);
	//}
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
	switch(ibc->Pfx) {
	case OP_PfxlessValue: {
		char chBuf[2];
		chBuf[0] = ibc->Ch;
		chBuf[1] = '\0';
		ibc->Pfx = OP_Value;
		StrConcat(ibc->Str, IBLayer3STR_MAX, chBuf);
		break;
	}
	}
	switch(ibc->Pfx) {
	case OP_SingleQuote: /* ' */
	case OP_String: { /* " */
		ibc->StringMode = true;
		IBLayer3Push(ibc, OP_ModeStrPass, false);
		break;
	}
	case OP_Letter_azAZ: {
		char chBuf[2];
		chBuf[0] = ibc->Ch;
		chBuf[1] = '\0';
		//ibc->Pfx = OP_Op;
		StrConcat(ibc->Str, IBLayer3STR_MAX, chBuf);
		IBLayer3Push(ibc, OP_ModeStrPass, false);
		IBVectorCopyPushBool(&ibc->StrReadPtrsStack, true);
		break;
	}
	case OP_VarType: {
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
	case OP_Name: {
		IBVectorCopyPushBool(&ibc->StrReadPtrsStack, true);
		if(ibc->Pfx == OP_Name)
		{
			ibc->DotPathOn = true;		
			IBVectorClear(&ibc->DotPathVec, IBStrFree);
		}
		/*getchar();*/
		IBLayer3Push(ibc, OP_ModeStrPass, false);
	}
	case OP_SpaceChar:
	case OP_Comment:
		break;
	}
	if(ibc->Pfx == OP_Op) {
		if(expTop && expTop->life && --expTop->life <= 0) {
			IBVectorPop(&t->expStack, IBExpectsFree);
		}
	}
}

void IBLayer3Str(IBLayer3* ibc)
{
	char chBuf[2];
	chBuf[0] = ibc->Ch;
	chBuf[1] = '\0';
	if(ibc->StringMode) {
		switch(ibc->Ch) {
		case '\''://'
		case '\"'://"
		{
			ibc->StringMode = false;
			IBLayer3StrPayload(ibc);
			return;
		}
		}
	}
	else {
		switch(ibc->Pfx) {
		case OP_Name: {
			switch(ibc->Ch) {
			case '[': {
				//IBStrClear(&ibc->ArrayIndexExprStr);
				IBStr* aT = NULL;
				IBVectorPush(&ibc->ArrayIndexExprsVec, &aT);
				IBStrInit(aT);
				IBLayer3Push(ibc, OP_ModeArrayIndexExpr, false);
				return;
			}
			case ']': {
				if(IBLayer3GetMode(ibc) == OP_ModeArrayIndexExpr)
					IBLayer3Pop(ibc);
				return;
			}
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
					/*case OP_Value: {
						switch (ibc->Ch) {
						case '@': {
							IBLayer3Pop(ibc);
							IBLayer3Prefix(ibc);
							return;
						}
						}
						break;
					}*/
		}
		switch(ibc->Ch) {
		case '\t': return;
		case ' ': {
			if(ibc->StrAllowSpace) break;
			else {
				IBLayer3StrPayload(ibc);
				return;
			}
		}
		case '^': {
			if(*(bool*) IBVectorTop(&ibc->StrReadPtrsStack)) {
				switch(ibc->Pointer) {
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
	if(ret) {
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
	if(ibc->TaskStack.elemCount >= 2) {
		tParent = (IBTask*) IBVectorGet(&ibc->TaskStack, ibc->TaskStack.elemCount - 2);
	}
	switch(valType) {
	case OP_Bool: {
		IBOp boolCheck = IBStrToBool(ibc, ibc->Str);
		switch(boolCheck) {
		case OP_True: {
			strVal.boolean = true;
			break;
		}
		case OP_False: {
			strVal.boolean = false;
			break;
		}
		IBCASE_UNIMPLEMENTED
		}
		break;
	}
	case OP_String: break;
	case OP_Number: {
		strVal.i64 = atoll(ibc->Str);
		break;
	}
	case OP_Double: {
		strVal.d64 = atof(ibc->Str);
		break;
	}
	case OP_Float: {
		strVal.f32 = (float) atof(ibc->Str);
		break;
	}
	//IBCASE_UNIMPLEMENTED
	/*default: {
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
	switch(ibc->Pfx) {
	/* . PFXDOT */ case OP_Dot: {
		switch(t->type) {
		case OP_NeedExpression: {
			IBOp type = OP_Unknown;
			IBObj* o = 0;
			IBObj* vneO = IBLayer3FindStackObjRev(ibc, OP_VarNeedExpr);
			IBTask* e2nt = IBLayer3FindTaskUnderIndex(ibc, -1, OP_ExprToName, 3);
			/*IBTask* etn = IBLayer3FindTaskUnderIndex(ibc, -1, OP_ExprToName, 3);*/
			IBTypeInfo* st = 0;
			IBTypeInfo* ti = 0;
			if(vneO) {
				IBLayer3FindType(ibc, vneO->str, &ti);
				if(ti) {
					type = ti->type;
					IBTypeInfoFindMember(ti, ibc->Str, &st);
				}
			}
			else if(e2nt/*&&ibc->DefiningMethods*/) {
				IBObj* no = 0;
				TaskFindWorkingObj(e2nt, OP_ActOnName, &no);
				IBassert(no);
				if(no) {
					if(IB_STARTS_WITH_SELFDOT(no->name)) {
						char* rn = IB_SELFDOTLESS_NTSP(no->name);
						IBassert(rn && (*rn));
						/*IBTask*mt=IBLayer3FindTaskUnderIndex(ibc,-1,OP_MethodsWantContent, 100);
						IBassert(mt);*/
						IBassert(ibc->_methodsStructName);
						IBLayer3FindType(ibc, ibc->_methodsStructName, &ti);
						if(ti) {
							type = ti->type;
							IBTypeInfoFindMember(ti, rn, &st);
						}
					}
					else {
						IBNameInfo* ni = IBLayer3SearchNameInfo(ibc, no->name);
						if(!ni) DB;
						if(ni && ni->ti) {
							ti = ni->ti;
							type = ti->type;
							IBTypeInfoFindMember(ti, ibc->Str, &st);
						}
					}
				}
			}
			if(type == OP_Unknown || !st)
				Err(OP_Error, "context not found");
			IBLayer3PushObj(ibc, &o);
			IBassert(st);
			if(st) {
				if(st->type == OP_StructVar) {
				}
				else IBObjSetType(o, st->type);
			}
			IBassert(ti);
			if(ti)IBObjSetStr(o, ti->name.start);
			IBObjSetName(o, ibc->Str);
			if(st && st->type == OP_StructVar) {
				IBOp t = st->structVarType->type;
				IBassert(st->structVarType);
				IBObjSetType(o, t == OP_Enum ? OP_EnumVal : t);
				IBOverwriteStr(&o->str, st->structVarType->name.start);
			}
			IBLayer3PopObj(ibc, true, &o);
			break;
		}
		IBCASE_UNIMPLEMENTED
		}
		break;
	}
	/* ' PFXSINGLEQUOTE */ case OP_SingleQuote: {
		switch(t->type) {
		case OP_VarWantValue: {
			o->var.val.c8 = ibc->Str[0];
			o->var.valSet = true;
			break;
		}
		IBCASE_UNIMPLEMENTED
		}
		break;
	}
	/* a-zA-z PFXazAZ */ case OP_Letter_azAZ: {
		IBTypeInfo* ti = 0;
		IBLayer3FindType(ibc, ibc->Str, &ti);
		//IBOp dataType = IBGetOpFromNameList(ibc->Str, OP_DataTypes);
		//if (dataType != OP_Unknown)
		if(ti) {
			ibc->Pfx = OP_VarType;
			DbgPuts("infered vartype\n");
			goto top;
		}
		else if(!strcmp(ibc->Str, IBFALSESTR) ||
			!strcmp(ibc->Str, IB_TRUESTR)) {
			ibc->Pfx = OP_Value;
			DbgPuts("infered bool Value\n");
			goto top;
		}
		else {
			IBOp nameOp = IBGetOpFromNameList(ibc->Str, OP_NameOps);
			switch(nameOp) {
			case OP_Unknown:
			case OP_Void:
				break;
			default: {
				ibc->Pfx = OP_Op;
				DbgPuts("infered OP\n");
				goto top;
			}
			}
		}
		Err(OP_Error, "Couldn't infer this input");
	}
	/* _ PFXUNDERSCORE */ case OP_Underscore: {
		switch(ibc->NameOp) {
		case OP_EmptyStr: {
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
	/* - PFXSUBTRACT */ case OP_Subtract: {
		//bool fall = true;
		switch(ibc->NameOp) {
		case OP_EmptyStr: {
			switch(t->type) {
			case OP_RootTask: {
				switch(ibc->NameOp) {
				case OP_Exclaim: {
					//fall = false;
					IBLayer3Push(ibc, OP_ModeCCompTimeMacroPaste, true);
					break;
				}
				IBCASE_UNIMPLEMENTED
				}
				break;
			}
			case OP_NeedExpression: {
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
		case OP_GreaterThan: {
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
	/* | PFXOR */ case OP_Or: {
		switch(ibc->NameOp) {
		case OP_Or: {

			break;
		}
		}
	}
	/* ! PFXEXCLAIM */ case OP_Exclaim: {
		switch(t->type) {
		IBCASE_BLOCKWANTCODE
		{
			if(!*ibc->Str) {
				IBExpects* exp = 0;
				//IBTask*t=0;
				IBLayer3PushTask(ibc, OP_CallMethodNeedName, &exp, &t);
				IBExpectsInit(exp,"P",OP_Name);
				break;
			}
			//IBTask* t;
			//IBLayer3PushTask(ibc, OP_CodeBlockCallFunc, NULL, NULL);
			IBLayer3PushTask(ibc, OP_CallFunc, NULL, NULL);
			//break;
		}
		case OP_ActOnNameEquals: {
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
	/* " PFXSTRING */ case OP_String: {
		switch(t->type) {
		case OP_VarWantValue: {
			IBOverwriteStr(&o->var.valStrLiteral, ibc->Str);
			o->var.valSet = true;
			break;
		}
		case OP_CIncNeedFilePath: {
			IBStrAppendFmt(&ibc->CIncludesStr,
				"#include \"%s\"\n", ibc->Str);
			IBLayer3PopTask(ibc, &t, false);
			break;
		}
		case OP_RunArgsNeedArgsStr: {
			IBStrAppendFmt(&ibc->RunArguments, "%s", ibc->Str);
			IBLayer3PopTask(ibc, &t, false);
			break;
		}
		case OP_FuncNeedRetVal: {
			IBObj* o;
			int idx;
			idx = 0;
			while(o = (IBObj*) IBVectorIterNext(&t->working, &idx)) {
				if(o->type == OP_FuncSigComplete) {
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
		case OP_BlockReturnNeedValue: {
			switch(o->type) {
			case OP_BlockReturnNeedValue: {
				IBObjSetStr(o, ibc->Str);
				o->valType = OP_String;
				IBLayer3FinishTask(ibc);
				break;
			}
			}
			break;
		}
		case OP_dbgAssertWantArgs: {
			switch(IBGetOpFromNameList(ibc->Str, OP_dbgAssert)) {
			case OP_TaskType: {
				break;
			}
			}
			break;
		}
		case OP_CPrintfHaveFmtStr: {
			IBObj* o;
			IBLayer3PushObj(ibc, &o);
			IBObjSetType(o, OP_String);
			o->var.type = OP_String;
			IBObjSetStr(o, ibc->Str);
			IBLayer3PopObj(ibc, true, NULL);
			break;
		}
		IBCASE_BLOCKWANTCODE
		{ /*printf*/
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
	/* % PFXVARTYPE */ case OP_VarType: {
		IBTypeInfo* ti = 0;
		IBLayer3FindType(ibc, ibc->Str, &ti);
		if(!ti)
			ErrF(OP_NotFound, "type %s doesn't exist", ibc->Str);
		switch(t->type) {
		case OP_FuncHasName: {
			switch(o->type) {
			case OP_FuncHasName: {
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
			case OP_FuncNeedsRetValType: {
				o->func.retValType = ibc->NameOp;
				o->func.retTypeMod = ibc->Pointer;
				if(o->func.retValType == OP_Unknown)
					Err(OP_Error, "unknown return type");
				if(o->func.retValType == OP_String) {
					o->func.retValType = OP_c8;
					o->func.retTypeMod = OP_Pointer;
				}
				SetObjType(o, OP_FuncSigComplete);
				break;
			}
			}
			break;
		}
		case OP_StructWantRepr: {
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
	/* $ PFXNAME */ case OP_Name: {
		switch(ibc->Str[0]) {
			IBCASE_0THRU9 {
				Err(OP_YouCantUseThatHere,
					"can't use number as first character of name!");
			}
		}
		switch(t->type) {
		case OP_MethodsNeedName: {
			IBTypeInfo* ti = 0;
			IBLayer3FindType(ibc, ibc->Str, &ti);
			if(!ti) {
				ErrF(OP_NotFound, "%s isnt found\n", ibc->Str);
			}
			else {
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
		case OP_ForNeedStartValName: {
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
		case OP_EnumWantContent: {
			IBObj* o;
			IBLayer3PushObj(ibc, &o);
			IBObjSetType(o, OP_EnumName);
			IBObjSetName(o, ibc->Str);
			IBLayer3PopObj(ibc, true, &o);
			break;
		}
		case OP_EnumNeedName: {
			IBExpects* exp;
			SetTaskType(t, OP_EnumWantContent);
			IBLayer3ReplaceExpects(ibc, &exp);
			IBExpectsInit(exp, "PP", OP_Name, OP_Underscore);
			IBassert(o->type == OP_Enum);
			IBObjSetName(o, ibc->Str);
			IBLayer3PopObj(ibc, true, &o);
			break;
		}
		case OP_NeedExpression: {
			IBObj* o;
			IBLayer3PushObj(ibc, &o);
			IBObjSetType(o, OP_Name);
			IBObjSetName(o, ibc->Str);
			o->modifier = ibc->Pointer;
			IBLayer3PopObj(ibc, true, &o);
			break;
		}
		case OP_StructInitNeedName: {
			IBExpects* exp;
			IBassert(o->type == OP_StructInit);
			IBObjSetName(o, ibc->Str);
			IBLayer3PushTask(ibc, OP_SubtaskArgs, &exp, &t);
			IBExpectsInit(exp, "PPP", OP_Name, OP_String, OP_Value);
			break;
		}
		case OP_FuncHasName: {
			switch(o->type) {
			case OP_FuncArgNameless: {
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
		case OP_FuncNeedName: {
			switch(o->type) {
			case OP_Func: {
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
		case OP_FuncNeedRetVal: {
			IBObj* o;
			int idx;
			idx = 0;
			while(o = (IBObj*) IBVectorIterNext(&t->working, &idx)) {
				if(o->type == OP_FuncSigComplete) {
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
		case OP_CallWantArgs: {
			if(o->type == OP_ArgNeedValue) {
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
		case OP_BlockReturnNeedValue: {
			switch(o->type) {
			case OP_BlockReturnNeedValue: {
				IBObjSetStr(o, ibc->Str);
				o->valType = OP_Name;
				IBLayer3FinishTask(ibc);
				break;
			}
			}
			break;
		}
		case OP_BuildingIf: {
			IBExpects* exp = NULL;
			switch(o->type) {
			case OP_IfNeedLVal: {
				IBOverwriteStr(&o->ifO.lvName, ibc->Str);
				IBNameInfo* ni = IBLayer3SearchNameInfo(ibc, o->ifO.lvName);
				if(!ni) {
					ErrF(OP_NotFound, "%s wasn't found", o->ifO.lvName);
				}
				IBassert(ni);
				o->ifO.lvTYPE = OP_Name;
				SetObjType(o, OP_IfNeedMidOP);
				IBLayer3ReplaceExpects(ibc, &exp);
				IBExpectsInit(exp, "PPPN", OP_GreaterThan, OP_Value, OP_LessThan, OP_Value);
				break;
			}
			case OP_IfNeedRVal: {
				IBExpects* exp;
				IBOverwriteStr(&o->ifO.rvName, ibc->Str);
				IBNameInfo* ni = IBLayer3SearchNameInfo(ibc, o->ifO.rvName);
				if(!ni) {
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
		case OP_StructWantName: {
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
		case OP_UseNeedStr: {
			IBOp lib;
			lib = ibc->NameOp;
			switch(lib) {
			case OP_UseStrSysLib: {
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
			default: {

				break;
			}
			}
			IBLayer3PopTask(ibc, NULL, false);
			IBLayer3PopCodeBlock(ibc, false, &cb);//?????
			break;
		}
		case OP_SpaceNeedName: {
			IBObj* o;
			IBLayer3PushObj(ibc, &o);
			IBassert(ibc->Str[0] != '\0');
			IBObjSetName(o, ibc->Str);
			IBObjSetType(o, OP_Space);
			IBLayer3PopObj(ibc, true, &o);
			SetTaskType(t, OP_SpaceHasName);
			IBLayer3FinishTask(ibc);
			break;
		}
		case OP_CPrintfHaveFmtStr: {
			IBObj* o = 0;
			IBStr* at = 0;
			int idx = 0;
			IBLayer3PushObj(ibc, &o);
			IBObjSetName(o, ibc->Str);
			IBObjSetType(o, OP_Name);
			while(at = IBVectorIterNext(&ibc->ArrayIndexExprsVec, &idx)) {
				IBStr* ss = 0;
				IBVectorPush(&o->arg.arrIndexExprs, &ss);
				IBStrInit(ss);
				IBStrAppend(ss, at);
			}
			IBLayer3PopObj(ibc, true, NULL);
			break;
		}
		IBCASE_BLOCKWANTCODE {
			IBTask * t = NULL;
			IBExpects* exp = NULL;
			IBObj* o = NULL;
			IBLayer3PushTask(ibc, OP_ActOnName, &exp, &t);
			IBLayer3PushObj(ibc, &o);
			IBObjSetType(o, OP_ActOnName);
			/*if (StrStartsWith(ibc->Str, "self.") && strlen(ibc->Str) > 5) {
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
		case OP_VarNeedName: {
			switch(o->type) {
			case OP_VarNeedName: {
				IBExpects* exp;
				IBNameInfo* ni = NULL;
				IBOp rc = 0;
				IBObjSetName(o, ibc->Str);
				IBTypeInfo* ti = o->var.ti;
				IBOp realType = o->var.type == OP_Unknown && ti ? ti->type : o->var.type;
				IBOp parentTaskType = OP_Unknown;
				if(tParent) 
					parentTaskType = tParent->type;
				rc = IBNameInfoAddMember(ibc,
					parentTaskType ?
					&ibc->GlobalVariables : &cb->localVariables,
					ibc->Str, realType, &ni);
				ni->type = realType;
				ni->ti = ti;
				switch(parentTaskType)
				{
				IBCASE_BLOCKWANTCODE
				{
					IBTypeInfo* mvTi = 0;
					int mvIdx = 0;
					while(mvTi = IBVectorIterNext(&ti->members, &mvIdx))
					{
						IBNameInfo* mvNi = 0;
						IBNameInfoAddMember(ibc, ni, mvTi->name.start, mvTi->StructVar.type, &mvNi);
						mvNi->ti = mvTi;
					}
					break;
				}
				}
				/*if(rc == OP_AlreadyExists)
					Err(OP_Error, "name already in use");
				IBassert(rc == OP_OK);*/
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
	/* @ PFXOP */ case OP_Op: {
		bool expected;
		expected = IBLayer3IsNameOpExpected(ibc, ibc->NameOp);
		if(!expected)Err(OP_ErrUnexpectedNameOP, "unexpected nameOP");
		switch(ibc->NameOp) {
		case OP_CInclude: {
			IBTask* t = NULL;
			IBExpects* exp = NULL;
			IBLayer3PushTask(ibc, OP_CIncNeedFilePath, &exp, &t);
			IBExpectsInit(exp, "P", OP_String);
			break;
		}
		case OP_RunArguments: {
			IBTask* t = NULL;
			IBExpects* exp = NULL;
			IBLayer3PushTask(ibc, OP_RunArgsNeedArgsStr, &exp, &t);
			IBExpectsInit(exp, "P", OP_String);
			break;
		}
		case OP_As: {

			break;
		}
		case OP_Repr: {
			switch(t->type) {
			case OP_StructWantRepr: {
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
		case OP_Flags: {
			switch(t->type) {
			case OP_RootTask: {
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
		case OP_Enum: {
			switch(t->type) {
			case OP_RootTask: {
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
		case OP_Space: {
			switch(t->type) {
			case OP_RootTask: {
				IBExpects* ap;
				IBLayer3PushTask(ibc, OP_SpaceNeedName, &ap, NULL);
				IBExpectsInit(ap, "1P", "expected space name", OP_Name);
				break;
			}
			default: Err(OP_Error, "can't use space here");
			}
			break;
		}
		case OP_Methods: {
			IBTask* t = 0;
			IBExpects* exp = 0;
			IBLayer3PushTask(ibc, OP_MethodsNeedName, &exp, &t);
			IBExpectsInit(exp, "P", OP_Name);
			break;
		}
		case OP_Struct: {
			switch(t->type) {
			case OP_RootTask: {
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
		case OP_dbgAssert: {
			IBExpects* ap;
			IBLayer3PushTask(ibc, OP_dbgAssertWantArgs, &ap, NULL);
			IBExpectsInit(ap, "1P", "expected string", OP_String);
			break;
		}
		case OP_Fall: {
			switch(t->type) {
			case OP_CaseWantCode: {
				IBObj* o = IBLayer3FindWorkingObj(ibc, OP_TableCase);
				IBassert(o);
				o->table.fallthru = true;
				break;
			}
			}
			break;
		}
		case OP_Case: {
			switch(t->type) {
			case OP_CaseWantCode: {
				IBTask* t = NULL;
				IBLayer3FinishTask(ibc);
				t = IBLayer3GetTask(ibc);
				IBassert(t);
				IBassert(t->type == OP_TableWantCase);
			}
			case OP_TableWantCase: {
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
		case OP_Table: {
			switch(t->type) {
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
		case OP_dbgBreak: { break; }
		case OP_Imaginary: {
			ibc->Imaginary = true;
			IBPushColor(IBFgMAGENTA);
			DbgPuts("[GOT IMAGINARY]");
			IBPopColor();
			DbgPuts("\n");
			break;
		}
		case OP_Return: {
			switch(t->type) {
			case OP_FuncHasName: {
				switch(o->type) {
				case OP_FuncHasName: {
					IBExpects* exp;
					SetObjType(o, OP_FuncNeedsRetValType);
					IBLayer3PushExpects(ibc, &exp);
					IBExpectsInit(exp, "P", OP_VarType);
					break;
				}
				default: {
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
		case OP_Func: { //func1
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
			o->privacy = ibc->Privacy;
			o->func.retTYPE = OP_NotSet;
			o->func.retValType = OP_Void;
			o->func.retTypeMod = OP_NotSet;
			IBLayer3PushCodeBlock(ibc, &cb);
			break;
		}
		case OP_Public:
		case OP_Private: {
			ibc->Privacy = ibc->NameOp;
			break;
		}
		case OP_Use: {
			IBExpects* ap;
			IBLayer3PushTask(ibc, OP_UseNeedStr, &ap, NULL);
			IBExpectsInit(ap, "1P", "expected @use $name", OP_Name);
			IBLayer3PushCodeBlock(ibc, &cb);
			break;
		}
		case OP_Else: {
			switch(t->type) {
			case OP_IfBlockWantCode: {
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
		case OP_ElseIf: {
			switch(t->type) {
			case OP_IfBlockWantCode: {
				IBLayer3FinishTask(ibc);
				ibc->LastNameOp = ibc->NameOp;
				IBLayer3PushTask(ibc, OP_ElseIf, NULL, &t);
				break;
			}
			default: {
				Err(OP_YouCantUseThatHere, "You may only use @elif in @if blocks!!!");
				break;
			}
			}
		}
		case OP_If: {
			switch(t->type) {
			case OP_ElseIf: {
				cb = IBLayer3CodeBlocksTop(ibc);
				IBStrAppendCh(&cb->code, '\t', tabCount - 1);
				IBStrAppendCStr(&cb->code, "else ");
				IBLayer3PopTask(ibc, &t, false);
			}
			IBCASE_BLOCKWANTCODE {
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
			default: {
				Err(OP_Error, "Unimplemented If task context");
				break;
			}
			}
			break;
		}
		case OP_Loop: {
			switch(t->type) {
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
		case OP_Break: {
			cb = IBLayer3CodeBlocksTop(ibc);
			IBStrAppendCh(&cb->code, '\t', tabCount);
			IBStrAppendCStr(&cb->code, "break;\n");
			break;
		}
		case OP_For: {
			/*
			switch (t->type) {
				IBCASE_BLOCKWANTCODE
				{
					break;
				}
				IBCASE_UNIMPLEMENTED
			}
			*/
			switch(t->type) {
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
	/* = PFXVALUE */ case OP_Value: {
		switch(t->type) {
		case OP_ForNeedStartInitVal: {
			switch(t->type) {

			IBCASE_UNIMPLEMENTED
			}
			break;
		}
		case OP_NeedExpression: {
			IBObj* o = NULL;
			IBLayer3PushObj(ibc, &o);
			IBObjSetType(o, OP_Value);
			o->valType = valType;
			o->val = strVal;
			IBLayer3PopObj(ibc, true, &o);
			break;
		}
		case OP_ActOnName: {
			switch(ibc->NameOp) {
			case OP_EmptyStr: {
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
		case OP_VarWantValue: {
			switch(o->type) {
			case OP_VarWantValue: {
				o->var.val = strVal;
				o->var.valSet = true;
				switch(o->var.type) {
				case OP_Bool: {
					if(valType != OP_Bool)
						Err(OP_YouCantUseThatHere, "wrong value for bool variable");
					break;
				}
				case OP_f32: {
					if(valType != OP_Float)
						Err(OP_YouCantUseThatHere, "wrong value for float variable");
					break;
				}
				case OP_d64: {
					if(valType != OP_Double)
						Err(OP_YouCantUseThatHere, "wrong value for double variable");
					break;
				}
				case OP_u8:
				case OP_c8: /*{
					if (o->var.mod == OP_Pointer) {
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
				case OP_i32: {
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
		case OP_BlockReturnNeedValue: {
			switch(o->type) {
			case OP_BlockReturnNeedValue: {
				o->val = strVal;
				o->valType = OP_Value;
				IBLayer3FinishTask(ibc);
				break;
			}
			}
			break;
		}
		case OP_CallWantArgs: {
			switch(o->type) {
			case OP_ArgNeedValue: {
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
		case OP_BuildingIf: {
			IBExpects* exp = 0;
			switch(o->type) {
			case OP_IfNeedLVal: {
				o->ifO.lvVal = strVal;
				o->ifO.lvTYPE = valType;
				//o->ifO.lvDataType = OP_i32;
				SetObjType(o, OP_IfNeedMidOP);
				break;
			}
			case OP_IfNeedMidOP: {
				switch(ibc->NameOp)
				{
				case OP_Equals: {
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
			case OP_IfNeedRVal: {
				IBExpects* exp;
				o->ifO.rvVal = strVal;
				o->ifO.rvTYPE = valType;
				//o->ifO.rvDataType = OP_i32;
				SetObjType(o, OP_IfFinished);
				SetTaskType(t, OP_IfFinished);
				IBLayer3ReplaceExpects(ibc, &exp);
				IBExpectsInit(exp, "P", OP_LineEnd);
				break;
			}
			default: {
				Err(OP_Error, "Unimplemented If task context");
			}
			}
			break;
		}
		case OP_CPrintfHaveFmtStr: {
			IBObj* o;
			IBLayer3PushObj(ibc, &o);
			o->val = strVal;
			IBObjSetType(o, OP_Value);
			o->var.type = OP_i32;/*for now*/
			IBLayer3PopObj(ibc, true, NULL);
			break;
		}
		case OP_FuncNeedRetVal: {
			IBObj* o;
			int idx;
			idx = 0;
			while(o = (IBObj*) IBVectorIterNext(&t->working, &idx)) {
				if(o->type == OP_FuncSigComplete) {
					IBOp valType = IBJudgeTypeOfStrValue(ibc, ibc->Str);
					switch(o->func.retValType) {
					case OP_c8: {

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
						default: {
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
	/* < PFXLESSTHAN */ case OP_LessThan: {
		switch(t->type) {
		case OP_ActOnName: {
			switch(ibc->NameOp) {
			case OP_Subtract: {
				o = IBLayer3GetObj(ibc);
				SetTaskType(t, OP_ExprToName);
				SetObjType(o, OP_ExprToName);
				IBLayer3PushTask(ibc, OP_NeedExpression, NULL, &t);
				break;
			}
			IBCASE_UNIMPLEMENTED
			}
			break;
		}
		case OP_VarWantValue: {
			switch(ibc->NameOp) {
			case OP_Subtract: {
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
		case OP_BuildingIf: {
			IBExpects* exp;
			switch(o->type) {
			case OP_IfNeedMidOP: {
				switch(ibc->NameOp)
				{
				case OP_Equals:
				case OP_EmptyStr: {
					o->ifO.midOp = ibc->NameOp == OP_Equals ? OP_LessThanOrEquals : ibc->Pfx;
					IBObjSetType(o, OP_IfNeedRVal);
					IBLayer3ReplaceExpects(ibc, &exp);
					IBExpectsInit(exp, "PP", OP_Name, OP_Value);
					break;
				}
				}
				break;
			}
			default: {
				Err(OP_Error, "Unimplemented If task context");
			}
			}
			break;
		}
		IBCASE_UNIMPLEMENTED
		}
		break;
	}
	/* > PFXGREATERTHAN */ case OP_GreaterThan: {
		switch(t->type) {
		case OP_BuildingIf: {
			IBExpects* exp;
			switch(o->type) {
			case OP_IfNeedMidOP: {
				switch(ibc->NameOp)
				{
				case OP_Equals:
				case OP_EmptyStr: {
					o->ifO.midOp = ibc->NameOp == OP_Equals ? OP_GreaterThanOrEquals : ibc->Pfx;
					IBObjSetType(o, OP_IfNeedRVal);
					IBLayer3ReplaceExpects(ibc, &exp);
					IBExpectsInit(exp, "PP", OP_Name, OP_Value);
					break;
				}
				}
				break;
			}
			default: {
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
	if(ibc->StrReadPtrsStack.elemCount > 1) {
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

void IBLayer3ExplainErr(IBLayer3* ibc, IBOp code)
{
	switch(code) {
	case OP_TCC_Error: {
		printf("Fatal internal compiler error");
		break;
	}
	case OP_YouCantUseThatHere: {
		printf("You can't use that here");
		break;
	}
	case OP_FuncNeedRetVal: {
		printf("You forgot to return a value from the function");
		break;
	}
	case OP_ErrUnexpectedNameOP: {
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
	case OP_ErrUnexpectedNextPfx: {
		IBOp* oi;
		IBExpects* ap;
		IBTask* t;
		int idx;
		t = IBLayer3GetTask(ibc);
		ap = IBTaskGetExpTop(t);
		if(ap && t) {
			IBassert(ap->pfxs.elemCount);
			printf("Err: \"%s\" Unexpected next prefix %s. "
				"Pfx idx:%d\nEnforced at line %d. Allowed:",
				ap->pfxErr, IBGetPfxName(ibc->Pfx),
				ap->pfxs.elemCount - 1, ap->lineNumInited);
			idx = 0;
			while(oi = (IBOp*) IBVectorIterNext(&ap->pfxs, &idx)) {
				printf("%s(%d),", IBGetPfxName(*oi), (int) *oi);
			}
		}
		else {
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
