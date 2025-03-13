#include "imbored.h"

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
	if(!wObjs->elemCount) 
	{
		/*Err(OP_Error, "workingObjs EMPTY!");*/
		DbgPuts("Warning: working objs for this task is empty!\n");
	}
	cb = IBLayer3CodeBlocksTop(ibc);
	tabCount = IBLayer3GetTabCount(ibc);
	switch(t->type)
	{
	case OP_MethodsWantContent:
	{
		IBObj* o = IBLayer3FindWorkingObjRev(ibc, OP_Methods);
		IBassert(o);
		break;
	}
	case OP_EnumWantContent:
	{
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
		IBASSERT0(ti);
		ti->Enum.isFlags = eo->enumO.flags;
		IBStrAppendFmt(&t->code.header, "enum E%s\n{\n", eo->name);
		IBStrAppendFmt(&t->code.footer, "};\n\n", eo->name);
		while(o = (IBObj*) IBVectorIterNext(wObjs, &idx))
		{
			switch(o->type)
			{
			case OP_Enum: break;
			case OP_EnumName:
			{
				IBTypeInfo* nti = 0;
				IBVectorPush(&ti->members, &nti);
				IBTypeInfoInit(nti, OP_EnumVal, o->name);
				oneFound = true;
				IBStrAppendFmt(&t->code.code, "\tE%s_%s", eo->name, o->name);
				if(eo->enumO.flags)
				{
					nti->EnumValue.val = flagsI;
					IBStrAppendFmt(&t->code.code, " = %d", flagsI);
					flagsI *= 2;
					if(flagsI == 0) flagsI = 2;
				}
				else
				{
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
		ibc->DefiningEnumTypeInfo = 0;
		break;
	}
	case OP_CaseWantCode:
	{
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
		IBStrAppendFmt(&cb->header, "case %s:\n{\n", fo.start);
		if(to->table.fallthru == false)
		{
			IBStrAppendCh(&cb->footer, '\t', tabCount - 1);
			IBStrAppendCStr(&cb->footer, "break;\n");
		}
		IBStrAppendCh(&cb->footer, '\t', tabCount - 2);
		IBStrAppendFmt(&cb->footer, "}\n");
		IBLayer3PopCodeBlock(ibc, true, &cb);
		IBStrFree(&fo);
		break;
	}
	case OP_TableWantCase:
	{
		IBStr fo;
		IBTask* st;
		IBassert(t->subTasks.elemCount == 1);
		st = (IBTask*) IBVectorGet(&t->subTasks, 0);
		IBStrInit(&fo);
		IBCodeBlockFinish(&st->code, &fo);
		IBStrAppendCh(&cb->header, '\t', tabCount - 1);
		IBStrAppendFmt(&cb->header, "switch (%s)\n{\n", fo.start);
		IBStrAppendCh(&cb->footer, '\t', tabCount - 1);
		IBStrAppendFmt(&cb->footer, "}\n");
		IBLayer3PopCodeBlock(ibc, true, &cb);
		IBStrFree(&fo);
		break;
	}
	case OP_ExprToName:
	case OP_ActOnNameEquals:
	{
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
		if(!strncmp(o->name, "self.", 5))
		{
			IBStrAppendFmt(&cb->code, "self->%s = %s;\n", o->name + 5, fc.start);
		}
		else
		{
			IBStrAppendFmt(&cb->code, "%s = %s;\n", o->name, fc.start);
		}
		break;
	}
	case OP_NeedExpression:
	{
		int idx = 0;
		IBObj* o = NULL;
		bool onOp = false;
		bool gotVal = false;
		if(wObjs->elemCount < 1) Err(OP_Error, "empty expression!");
		pop2Parent = true;
		while(o = (IBObj*) IBVectorIterNext(wObjs, &idx))
		{
			switch(o->type)
			{
			case OP_Or:
			case OP_Multiply:
			case OP_Divide:
			case OP_Subtract:
			case OP_Add:
			{
				if(!gotVal)Err(OP_Error, "missing op lval in expression");
				onOp = true;
				gotVal = false;
				IBStrAppendFmt(&t->code.code, " %s ", IBGetCEqu(o->type));
				break;
			}
			case OP_Name:
			{
				IBNameInfo* ni = IBLayer3SearchNameInfo(ibc, o->name);
				IBOp ceq = o->modifier == OP_Pointer ? OP_Ref
					: OP_None;
				if(!ni)
				{
					ErrF(OP_NotFound, "%s wasn't found", o->name);
				}
				else
				{
					switch(ni->type)
					{
						CASE_VALTYPES
						{
							gotVal = true;
							break;
						}
							IBCASE_UNIMPLEMENTED
					}
				}
				IBStrAppendFmt(&t->code.code, "%s%s", IBGetCEqu(ceq), o->name);
				if(onOp)
				{
					IBStrAppendFmt(&t->code.header, "%s", "(");
					IBStrAppendFmt(&t->code.code, "%s", ")");
				}
				onOp = false;
				break;
			}
			case OP_EnumVal:
			{
				gotVal = true;
				IBStrAppendFmt(&t->code.code, "E%s_%s", o->str, o->name);
				if(onOp)
				{
					IBStrAppendFmt(&t->code.header, "%s", "(");
					IBStrAppendFmt(&t->code.code, "%s", ")");
				}
				onOp = false;
				break;
			}
			case OP_StructVar:
			{
				gotVal = true;
				IBStrAppendFmt(&t->code.code, "%s%s_%s", "", o->str, o->name);
				break;
			}
			//lots of overlap with Val2Str...
			case OP_Value:
			{
				gotVal = true;
				switch(o->valType)
				{
				case OP_Double:
				{
					IBStrAppendFmt(&t->code.code, "%f", o->val.d64);
					break;
				}
				case OP_Float:
				{
					IBStrAppendFmt(&t->code.code, "%f", o->val.f32);
					break;
				}
				//case OP_Value:
				case OP_Number:
				{
					IBStrAppendFmt(&t->code.code, "%d", o->val.i32);
					break;
				}
				case OP_String:
				{

					break;
				}
				case OP_Bool:
				{
					IBStrAppendFmt(&t->code.code, "%s", o->val.boolean > 0 ? "1" : "0");
					break;
				}
				IBCASE_UNIMPLEMENTED
				}
				if(onOp)
				{
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
	case OP_VarNeedExpr:
	{
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
	case OP_VarWantValue:
	{
		int idx = 0;
		IBObj* o = NULL;
		bool thing = false;
		IBTask* parent =
			IBLayer3FindTaskUnderIndex(ibc, -1, OP_StructWantContent, 1);
		//IBassert(parent);
		if(parent && parent->type == OP_StructWantContent)
		{
			thing = true;
			pop2Parent = true;
		}
		IBStr* vstr = thing ? &parent->code.code
			: &IBLayer3CodeBlocksTop(ibc)->variables;
		while(o = (IBObj*) IBVectorIterNext(wObjs, &idx))
		{
			switch(o->type)
			{
			case OP_VarComplete:
			case OP_VarWantValue:
			{
				IBTypeInfo* ti = 0;
				IBStrAppendCh(vstr, '\t', thing ? 1 : tabCount);
				if(o->var.type == OP_String)
				{
					o->var.type = OP_c8;
					o->var.mod = OP_Pointer;
				}
				char* typeStr = o->var.type == OP_Unknown ? o->str : IBGetCEqu(o->var.type);
				char* typePfx = "";
				IBLayer3FindType(ibc, typeStr, &ti);
				if(ti)
				{
					switch(ti->type)
					{
					case OP_Enum:
						typePfx = "enum E";
						break;
					case OP_Struct:
						typePfx = "struct S";
						break;
					}
				}
				IBStrAppendFmt(vstr, "%s%s%s %s", typePfx, typeStr, IBGetCEqu(o->var.mod), o->name);
				if(o->var.valSet && !thing)
				{
					IBStrAppendCStr(vstr, " = ");
					switch(o->var.type)
					{
					case OP_i8:
					case OP_i16:
					case OP_i64:
					case OP_i32:
					{
						IBStrAppendFmt(vstr, "%d", o->var.val.i32);
						break;
					}
					case OP_d64:
					{
						IBStrAppendFmt(vstr, "%f", o->var.val.d64);
						break;
					}
					case OP_f32:
					{
						IBStrAppendFmt(vstr, "%f", o->var.val.f32);
						break;
					}
					case OP_Bool:
					{
						IBStrAppendFmt(vstr, "%s", IBBoolStrChar(o->var.val.boolean));
						break;
					}
					case OP_c8:
					{
						if(o->var.mod == OP_Pointer)
						{
							IBStrAppendFmt(vstr, "\"%s\"", o->var.valStrLiteral);
						}
						else
						{
							IBStrAppendFmt(vstr, "\'%c\'", o->var.val.c8);
						}
						break;
					}
					case OP_u8:
					{
						IBStrAppendFmt(vstr, "%u", o->var.val.u8);
						break;
					}
					case OP_u16:
					{
						IBStrAppendFmt(vstr, "%u", o->var.val.u16);
						break;
					}
					case OP_u32:
					{
						IBStrAppendFmt(vstr, "%u", o->var.val.u32);
						break;
					}
					case OP_u64:
					{
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
	case OP_CallFunc:
	{
		IBTask* st = NULL;
		/*int idx = 0;
		while (st = IBVectorIterNext(&t->subTasks, &idx))
		{

		}*/
		IBassert(t->subTasks.elemCount == 1);
		st = (IBTask*) IBVectorGet(&t->subTasks, 0);
		IBassert(st);
		IBStrAppendCh(&cb->code, '\t', tabCount);
		IBCodeBlockFinish(&st->code, &cb->code);
		IBStrAppendFmt(&cb->code, "%s\n", ";");
		break;
	}
	case OP_CallWantArgs:
	{
		IBObj* o = (IBObj*) IBVectorGet(wObjs, 0);
		int idx = 0;
		IBassert(o->type == OP_Call);
		//IBStrAppendCh(&t->code.code, '\t', tabCount);
		IBStrAppendFmt(&t->code.code, "%s", o->str);
		IBStrAppendCStr(&t->code.code, "(");
		while(o = (IBObj*) IBVectorIterNext(wObjs, &idx))
		{
			if(o->type != OP_Arg) continue;
			switch(o->valType)
			{
			case OP_Value:
			{
				IBStrAppendFmt(&t->code.code, "%llu", o->val.i64);
				break;
			}
			case OP_String:
			{
				IBStrAppendFmt(&t->code.code, "\"%s\"", o->str);
				break;
			}
			case OP_Name:
			{
				IBStrAppendFmt(&t->code.code, "%s", o->str);
				break;
			}
			IBCASE_UNIMPLEMENTED
			}
			if(idx <= wObjs->elemCount - 1)
			{
				IBStrAppendCStr(&t->code.code, ", ");
			}
		}
		IBStrAppendCStr(&t->code.code, ")");
		pop2Parent = true;
		break;
	}
	case OP_BlockReturnNeedValue:
	{
		IBObj* o = 0;
		IBLayer3PopObj(ibc, true, &o);
		o = (IBObj*) IBVectorTop(wObjs);
		IBStrAppendCh(&cb->code, '\t', tabCount);
		IBStrAppendFmt(&cb->code, "return ");
		IBassert(o->type == OP_BlockReturnNeedValue);
		switch(o->valType)
		{
		case OP_Value:
		{
			IBStrAppendFmt(&cb->code, "%llu", o->val.i64);
			break;
		}
		case OP_String:
		{
			IBStrAppendFmt(&cb->code, "\"%s\"", o->str);
			break;
		}
		case OP_Name:
		{
			IBStrAppendFmt(&cb->code, "%s", o->str);
			break;
		}
		}
		IBStrAppendCStr(&cb->code, ";\n");
		break;
	}
	case OP_IfBlockWantCode:
	{
		IBObj* o = 0, * m = 0;
		int idx = 0;
		while(o = (IBObj*) IBVectorIterNext(wObjs, &idx))
		{
			if(o->type == OP_IfFinished)
			{
				m = o;
				break;
			}
		}
		IBassert(m);
		if(ibc->LastNameOp != OP_ElseIf)
		{
			IBStrAppendCh(&cb->header, '\t', tabCount - 1);
			ibc->LastNameOp = OP_Null;
		}
		IBStrAppendFmt(&cb->header, "if (");
		switch(m->ifO.lvTYPE)
		{
		case OP_Name:
			IBStrAppendFmt(&cb->header, "%s ", m->ifO.lvName);
			break;
		default:
		{
			char buf[64];
			buf[0] = '\0';
			Val2Str(buf, 64, m->ifO.lvVal, m->ifO.lvTYPE);
			IBStrAppendFmt(&cb->header, "%s ", buf);
			break;
		}
		}
		IBStrAppendFmt(&cb->header, "%s ", IBGetCEqu(m->ifO.midOp));
		switch(m->ifO.rvTYPE)
		{
		case OP_Name:
			IBStrAppendFmt(&cb->header, "%s", m->ifO.rvName);
			break;
		default:
		{
			char buf[64];
			buf[0] = '\0';
			Val2Str(buf, 64, m->ifO.rvVal, m->ifO.rvTYPE);
			IBStrAppendFmt(&cb->header, "%s", buf);
			break;
		}
		}
		IBStrAppendFmt(&cb->header, ") ");
	}
	case OP_LoopBlockWantCode:
	{
		if(t->type == OP_LoopBlockWantCode)
		{
			IBStrAppendCh(&cb->header, '\t', tabCount - 1);
			IBStrAppendCStr(&cb->header, "while(1) ");
		}
	}
	case OP_BlockWantCode:
	{
		IBStrAppendFmt(&cb->header, "{\n");
		IBStrAppendCh(&cb->footer, '\t', tabCount - 1);
		IBStrAppendFmt(&cb->footer, "}\n");
		IBLayer3PopCodeBlock(ibc, true, &cb);
		break;
	}
	case OP_StructWantContent:
	{
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

		IBCodeBlockFinish(&t->code, &body);
		idx = 0;
		while(o = IBVectorIterNext(wObjs, &idx))
		{
			switch(o->type)
			{
			case OP_Struct:
			{
				IBassert(o->name);
				IBassert(*o->name);

				IBLayer3FindType(ibc, o->name, &ti);
				IBASSERT0(ti);

				IBStrAppendFmt(&header, "struct S%s\n{\n", o->name);
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
			while(ni = (IBNameInfo*) IBVectorIterNext(&cb->localVariables.members, &idx))
			{
				IBTypeInfo* nti = 0;
				IBVectorPush(&ti->members, &nti);
				IBTypeInfoInit(nti, OP_StructVar, ni->name);
				nti->memberVarType = ni->ti;
			}
		}
		IBLayer3PopCodeBlock(ibc, false, &cb);
		break;
	}
	case OP_FuncWantCode:
	case OP_FuncSigComplete:
	case OP_FuncHasName:
	case OP_Func:
	{
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
		for(i = 0; i < wObjs->elemCount; ++i)
		{
			o = (IBObj*) IBVectorGet(wObjs, i);
			switch(o->type)
			{
			case OP_FuncArgComplete: /* multiple allowed */
			{
				IBOp at;
				at = o->arg.type;
				argc++;
				if(at == OP_Null)Err(OP_Error, "arg type NULL");

				if(cFuncArgs.start[0] != '\0')
				{
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
			case OP_CompletedFunction: /* should only happen once */
			{
				IBOp mod;
				funcObj = o;
				mod = o->modifier;
				if(mod != OP_NotSet)
				{
					IBStrAppendCStr(&cFuncModsTypeName, IBGetCEqu(mod));
					IBStrAppendCStr(&cFuncModsTypeName, " ");
				}
				IBStrAppendCStr(&cFuncModsTypeName, IBGetCEqu(o->func.retValType));
				IBStrAppendCStr(&cFuncModsTypeName, IBGetCEqu(o->func.retTypeMod));
				IBStrAppendCStr(&cFuncModsTypeName, " ");
				if(!o->name)Err(OP_Error, "func name NULL");
				IBTypeInfo* mti = 0;
				char mtiC = '\0';
				if(o->name)
				{
					if(o->func.thingTask)//stupid
					{
						IBObj* wo;
						int idx;
						idx = 0;
						wo = IBLayer3FindStackObjUnderTop(ibc, OP_Struct);
						if(wo)
						{
							IBStrAppendFmt(&cFuncModsTypeName, "S%s_", wo->name);
							thingObj = wo;
						}
					}
					IBObj* mo = IBLayer3FindStackObjRev(ibc, OP_Methods);
					if(mo)
					{
						IBLayer3FindType(ibc, mo->name, &mti);
						IBassert(mti);
						switch(mti->type)
						{
						case OP_Enum:
						{
							IBStrAppendCh(&cFuncModsTypeName, 'E', 1);
							mtiC = 'E';
							break;
						}
						case OP_Struct:
						{
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
				if(thingObj)
				{
					IBStrAppendFmt(&cFuncArgsThing,
						"struct S%s* self", thingObj->name);
				}
				if(mti)
				{
					switch(mtiC)
					{
					case 'E':
					{
						IBStrAppendFmt(&cFuncArgsThing,
							"enum %c%s* self", mtiC, mti->name.start);
						break;
					}
					case 'S':
					{
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
		if(ibc->Imaginary)
		{
			//DbgFmt("[@ext @func]","");
			IBStrAppendCStr(&cFuncArgsEnd, ");\n\n");
			IBLayer3PopCodeBlock(ibc, false, &cb);
		}
		else
		{
			//DbgFmt("[@func]", "");
			IBStr cbOut;
			IBCodeBlock* cb;
			IBStrAppendCStr(&cFuncArgsEnd, ")\n{\n");
			IBStrInit(&cbOut);
			cb = IBLayer3CodeBlocksTop(ibc);
			IBCodeBlockFinish(cb, &cbOut);
			IBStrAppend(&cFuncCode, &cbOut);
			IBStrFree(&cbOut);
			IBLayer3PopCodeBlock(ibc, false, &cb);
			//no longer valid bcuz u can define methods in structs
			//IBassert(ibc->CodeBlockStack.elemCount == 1);
			if(!funcObj)
			{
				Err(OP_Error, "funcObj NULL");
			}
			else if(funcObj->func.retValType != OP_Void)
			{
				IBStrAppendCStr(&cFuncCode, "\treturn ");
				switch(funcObj->func.retValVarcast)
				{
					IBCASE_NUMTYPES
					{
						IBStrAppendFmt(&cFuncCode,"(%s) ",
							IBGetCEqu(funcObj->func.retValVarcast));
						break;
					}
				case OP_Null:break;
					IBCASE_UNIMPLEMENTED
				}
				switch(funcObj->func.retTYPE)
				{
				case OP_String:
				{
					IBStrAppendFmt(&cFuncCode, "\"%s\"", funcObj->func.retValStr);
					break;
				}
				case OP_Value:
				{
					//IBassert(funcObj->func.retValType==OP_i32);
					//IBStrAppendFmt(&cFuncCode, "%d",
					//	funcObj->func.retVal.i32);//for now
					switch(funcObj->func.retValType)
					{
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
				case OP_Name:
				{
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
		if(!ibc->Imaginary)
		{
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
	case OP_CPrintfHaveFmtStr:
	{
		bool firstPercent;
		IBObj* fmtObj;
		int varIdx;
		int i;
		if(t && wObjs->elemCount)
		{
			fmtObj = (IBObj*) wObjs->data;
			IBStrAppendCh(&cb->code, '\t', tabCount);
			IBStrAppendCStr(&cb->code, "printf(\"");
			ibc->IncludeCStdioHeader = true;
			firstPercent = false;
			varIdx = 1;
			for(i = 0; i < (int) strlen(fmtObj->str); ++i)
			{
				char c;
				c = fmtObj->str[i];
				switch(c)
				{
				case '%':
				{
					if(!firstPercent)
					{
						IBStrAppendCStr(&cb->code, "%");
						firstPercent = true;
					}
					else
					{
						IBObj* vo;
						IBOp voT;
						vo = (IBObj*) IBVectorGet(wObjs, varIdx);
						/*printf("cfmt vidx:%d\n",varIdx);*/
						IBassert(vo);
						voT = vo->type;
						/*if(voT==OP_String)DB*/
						switch(voT)
						{
						case OP_Name:
						{
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
						case OP_Value:
						{
							IBStrAppendCStr(&cb->code,
								IBLayer3GetCPrintfFmtForType(ibc, vo->var.type));
							break;
						}
						case OP_Arg:
						{
							//IBLayer3VecPrint(wObjs);
							IBassert(0);
						}
						case OP_CPrintfFmtStr: break;
						default:
						{
							Err(OP_Error, "unhandled printf arg type");
						}

						}
						firstPercent = false;
						varIdx++;
					}
					break;
				}
				default:
				{
					char chBuf[2];
					chBuf[0] = c;
					chBuf[1] = '\0';
					IBStrAppendCStr(&cb->code, chBuf);
					break;
				}
				}
			}
			IBStrAppendCStr(&cb->code, "\"");
			if(wObjs->elemCount > 1)
			{
				IBStrAppendCStr(&cb->code, ", ");
			}
			for(i = 1; i < wObjs->elemCount; ++i)
			{
				IBObj* o;
				o = (IBObj*) IBVectorGet(wObjs, i);
				switch(o->type)
				{
				case OP_Name:
				{
					IBNameInfo* ni = IBLayer3SearchNameInfo(ibc, o->name);
					if(ni->type == OP_Bool)
						IBStrAppendFmt(&cb->code, "%s ? \"true\" : \"false\"", o->name);
					else
					{
						IBStr* ibs = NULL;
						int idx = 0;

						IBStrAppendCStr(&cb->code, o->name);
						while(ibs = IBVectorIterNext(&o->arg.arrIndexExprs, &idx))
						{
							IBStrAppendFmt(&cb->code, "[%s]",
								ibs->start);
						}
					}
					break;
				}
				case OP_String:
				{
					IBStrAppendCStr(&cb->code, "\"");
					IBStrAppendCStr(&cb->code, o->str);
					IBStrAppendCStr(&cb->code, "\"");
					break;
				}
				case OP_Value:
				{
					char valBuf[32];
					valBuf[0] = '\0';
					Val2Str(valBuf, 32, o->val, o->var.type);
					IBStrAppendCStr(&cb->code, valBuf);
					break;
				}
				}
				if(i < wObjs->elemCount - 1)
				{
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
