#include "imbored.h"

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
	IBExpectsInit(exp, "PPPNNNNNNNNN",
		OP_Op, OP_VarType, OP_Subtract, OP_Use, OP_Imaginary, OP_Func,
		OP_Enum, OP_Flags, OP_Struct, OP_Methods,
		OP_RunArguments, OP_CInclude
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
	if(ibc->InputStr)
	{
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
	if(ibc->TaskStack.elemCount)
	{
		switch(t->type)
		{
		case OP_FuncWantCode:
		case OP_FuncNeedRetVal:
			Err(OP_Error, "Reached end of file without closing function");
			break;
		case OP_FuncSigComplete:
		case OP_FuncHasName:
		{
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
	IBStrFree(&rootCbFinal);
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
