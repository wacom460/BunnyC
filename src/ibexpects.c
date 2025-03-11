#include "imbored.h"

void _IBExpectsInit(int LINENUM, IBExpects* exp, char* fmt, ...) {
	va_list args;
	IBOp pfx;
	IBOp nameOp;
	int i;
	IBassert(exp);
	IBVectorInit(&exp->pfxs, sizeof(IBOp), OP_Op, IBVEC_DEFAULT_SLOTCOUNT);
	IBVectorInit(&exp->nameOps, sizeof(IBOp), OP_Op, IBVEC_DEFAULT_SLOTCOUNT);
	exp->pfxErr = "";
	exp->nameOpErr = "";
	exp->life = 0;
	exp->lineNumInited = LINENUM;
	//DbgFmt("Expect: { ","");
	va_start(args, fmt);
	for (i = 0; i < strlen(fmt); i++) {
		char ch = fmt[i];
		switch (ch) {
		case '0': {
			exp->life = va_arg(args, int);
			//DbgFmt("Life:%d ", exp->life);
			break;
		}
		case '1': {
			exp->pfxErr = va_arg(args, char*);
			//DbgFmt("PfxErr:%s ", exp->pfxErr);
			break;
		}
		case '2': {
			exp->nameOpErr = va_arg(args, char*);
			//DbgFmt("NameOpErr:%s ", exp->nameOpErr);
			break;
		}
		case 'P': {
			pfx = va_arg(args, IBOp);
			IBVectorCopyPushOp(&exp->pfxs, pfx);
			//DbgFmt("Pfx:%s(%d) ", IBGetPfxName(pfx), (int)pfx);
			break;
		}
		case 'N': {
			nameOp = va_arg(args, IBOp);
			IBVectorCopyPushOp(&exp->nameOps, nameOp);
			//DbgFmt("NameOP:%s(%d) ", IBGetOpName(nameOp), (int)nameOp);
			break;
		}
		case 'e': {
			IBVectorCopyPushOp(&exp->pfxs, OP_Value);
			IBVectorCopyPushOp(&exp->pfxs, OP_Name);
			IBVectorCopyPushOp(&exp->pfxs, OP_Add);
			IBVectorCopyPushOp(&exp->pfxs, OP_Subtract);
			IBVectorCopyPushOp(&exp->pfxs, OP_Divide);
			IBVectorCopyPushOp(&exp->pfxs, OP_Multiply);
			break;
		}
		case 'c': {
			//PLINE;
			//DbgFmt("CodeBlockMacro ", "");
			IBVectorCopyPushOp(&exp->pfxs, OP_Op);
			IBVectorCopyPushOp(&exp->pfxs, OP_VarType);
			IBVectorCopyPushOp(&exp->pfxs, OP_String);
			IBVectorCopyPushOp(&exp->pfxs, OP_Name);
			IBVectorCopyPushOp(&exp->pfxs, OP_Exclaim);
			IBVectorCopyPushOp(&exp->pfxs, OP_Underscore);

			IBVectorCopyPushOp(&exp->nameOps, OP_Bool);
			IBVectorCopyPushOp(&exp->nameOps, OP_If);
			IBVectorCopyPushOp(&exp->nameOps, OP_For);
			IBVectorCopyPushOp(&exp->nameOps, OP_Return);
			IBVectorCopyPushOp(&exp->nameOps, OP_Table);
			IBVectorCopyPushOp(&exp->nameOps, OP_Loop);
			IBVectorCopyPushOp(&exp->nameOps, OP_Break);//TODO: make sure inside loop
			break;
		}
		}
	}
	//DbgFmt("}\n","");
	va_end(args);
}
void IBExpectsPrint(IBExpects* ap) {
	IBOp* oi;
	int idx;
	idx = 0;
	IBassert(ap);
	oi = NULL;
#ifdef IBDEBUGPRINTS
	IBPushColor(IBBgGREEN);
	printf("[LN:%d] ", ap->lineNumInited);
	IBPopColor();
#endif
	printf("Prefix { ");
	while (oi = (IBOp*)IBVectorIterNext(&ap->pfxs, &idx))
		printf("%s(%d) ", IBGetPfxName(*oi), (int)*oi);
	printf("}\nNameOP { ");
	idx = 0;
	oi = NULL;
	while (oi = (IBOp*)IBVectorIterNext(&ap->nameOps, &idx))
		printf("@%s(%d) ", IBGetOpName(*oi), (int)*oi);
	printf("}\n");
}
void IBExpectsFree(IBExpects* ap) {
	IBassert(ap);
	IBVectorFreeSimple(&ap->pfxs);
	IBVectorFreeSimple(&ap->nameOps);
}