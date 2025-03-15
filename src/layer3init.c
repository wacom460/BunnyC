#include "imbored.h"

void IBLayer3Init(IBLayer3* ibc)
{
	memset(ibc, 0, sizeof * ibc);

	IBVectorInit(&ibc->DotPathVec, sizeof(IBStr),
		OP_IBStr, IBVEC_DEFAULT_SLOTCOUNT);

	IBVectorInit(&ibc->TypeRegistry,
		sizeof(IBTypeInfo), OP_IBTypeInfo, 32);
		
	IBTypeInfo* boolti = 0;
	IBVectorPush(&ibc->TypeRegistry, &boolti);
	IBTypeInfoInit(boolti, OP_Bool, "bool");
	
	IBTypeInfo* u8ti = 0;
	IBVectorPush(&ibc->TypeRegistry, &u8ti);
	IBTypeInfoInit(u8ti, OP_u8, "u8");
	
	IBTypeInfo* i8ti = 0;
	IBVectorPush(&ibc->TypeRegistry, &i8ti);
	IBTypeInfoInit(i8ti, OP_i8, "i8");
	
	IBTypeInfo* c8ti = 0;
	IBVectorPush(&ibc->TypeRegistry, &c8ti);
	IBTypeInfoInit(c8ti, OP_c8, "c8");
	
	IBTypeInfo* u16ti = 0;
	IBVectorPush(&ibc->TypeRegistry, &u16ti);
	IBTypeInfoInit(u16ti, OP_u16, "u16");
	
	IBTypeInfo* i16ti = 0;
	IBVectorPush(&ibc->TypeRegistry, &i16ti);
	IBTypeInfoInit(i16ti, OP_i16, "i16");
	
	IBTypeInfo* u32ti = 0;
	IBVectorPush(&ibc->TypeRegistry, &u32ti);
	IBTypeInfoInit(u32ti, OP_u32, "u32");
	
	IBTypeInfo* i32ti = 0;
	IBVectorPush(&ibc->TypeRegistry, &i32ti);
	IBTypeInfoInit(i32ti, OP_i32, "i32");
	
	IBTypeInfo* f32ti = 0;
	IBVectorPush(&ibc->TypeRegistry, &f32ti);
	IBTypeInfoInit(f32ti, OP_f32, "f32");
	
	IBTypeInfo* u64ti = 0;
	IBVectorPush(&ibc->TypeRegistry, &u64ti);
	IBTypeInfoInit(u64ti, OP_u64, "u64");
	
	IBTypeInfo* i64ti = 0;
	IBVectorPush(&ibc->TypeRegistry, &i64ti);
	IBTypeInfoInit(i64ti, OP_i64, "i64");
	
	IBTypeInfo* d64ti = 0;
	IBVectorPush(&ibc->TypeRegistry, &d64ti);
	IBTypeInfoInit(d64ti, OP_d64, "d64");
	
	IBTypeInfo* stringti = 0;
	IBVectorPush(&ibc->TypeRegistry, &stringti);
	IBTypeInfoInit(stringti, OP_String, "nts");

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
	IBCodeBlock* cb = 0;
	IBVectorPush(&ibc->CodeBlockStack, &cb);
	IBCodeBlockInit(cb);
	IBVectorCopyPushBool(&ibc->StrReadPtrsStack, false);
	IBLayer3Push(ibc, OP_ModePrefixPass, false);
	IBObj* o = 0;
	IBLayer3PushObj(ibc, &o);
	IBExpects* exp = 0;
	IBLayer3PushTask(ibc, OP_RootTask, &exp, NULL);
	IBExpectsInit(exp, "PPPNNNNNNNNN",
		OP_Op, OP_VarType, OP_Subtract, OP_Use, OP_Imaginary, OP_Func,
		OP_Enum, OP_Flags, OP_Struct, OP_Methods,
		OP_RunArguments, OP_CInclude
	);
}

void IBLayer3Free(IBLayer3* ibc)
{
	IBassert(ibc);
	IBStr rootCbFinal;
	IBStrInit(&rootCbFinal);
	if(ibc->InputStr)
	{
		IBLayer3InputStr(ibc, ibc->InputStr);
		ibc->InputStr = NULL;
	}
	IBCodeBlock* cb = (IBCodeBlock*) IBVectorTop(&ibc->CodeBlockStack);
	IBObj* o = IBLayer3GetObj(ibc);
	if(ibc->StringMode)
		Err(OP_Error, "Reached end of file without closing string");
	if(ibc->Str[0]) IBLayer3StrPayload(ibc);
	if(cb->localVariables.members.elemCount)
		Err(OP_Error, "root codeblock can't have variables in it!!!");
	IBTask* t = IBLayer3GetTask(ibc);
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
//	IBPushColor(IBFgMAGENTA);
//	DbgPuts("-> Compilation complete. Press any key <-\n");
//	IBPopColor();
//#ifdef IBDEBUGPRINTS
//	getchar();
//#endif

	IBStrFree(&rootCbFinal);
	IBVectorFree(&ibc->CodeBlockStack, IBCodeBlockFree);
	IBVectorFree(&ibc->ObjStack, ObjFree);
	IBVectorFreeSimple(&ibc->ModeStack);
	IBVectorFreeSimple(&ibc->ExpressionStack);
	IBVectorFreeSimple(&ibc->StrReadPtrsStack);
	IBVectorFree(&ibc->TaskStack, TaskFree);
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
	IBVectorFree(&ibc->ArrayIndexExprsVec, IBStrFree);
	IBStrFree(&ibc->CCode);
}
