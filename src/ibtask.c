#include "imbored.h"

void TaskInit(IBTask* t, IBOp type) {
	IBVectorInit(&t->working, sizeof(IBObj), OP_Obj, IBVEC_DEFAULT_SLOTCOUNT);
	IBVectorInit(&t->expStack, sizeof(IBExpects), OP_Expects, IBVEC_DEFAULT_SLOTCOUNT);
	IBVectorInit(&t->subTasks, sizeof(IBTask), OP_Task, IBVEC_DEFAULT_SLOTCOUNT);
	IBCodeBlockInit(&t->code);
	t->type = type;
	memset(&t->exprData, 0, sizeof(IBTaskNeedExpression));
}
void TaskFree(IBTask* t) {
	IBassert(t);
	IBCodeBlockFree(&t->code);
	IBVectorFree(&t->subTasks, TaskFree);
	IBVectorFree(&t->expStack, IBExpectsFree);
	IBVectorFree(&t->working, ObjFree);
}
void TaskFindWorkingObj(IBTask* t, IBOp type, IBObj** outDP) {
	IBObj* o = 0;
	int idx = 0;
	IBassert(t);
	while (o = IBVectorIterNext(&t->working, &idx)) {
		if (o->type == type) {
			if (outDP) (*outDP) = o;
			return;
		}
	}
}
IBExpects* IBTaskGetExpTop(IBTask* t) {
	IBExpects* ret;
	IBassert(t);
	ret = (IBExpects*)IBVectorTop(&t->expStack);
	IBassert(ret);
	return ret;
}
IBVector* IBTaskGetExpPfxsTop(IBTask* t) {
	IBExpects* ret;
	IBassert(t);
	ret = (IBExpects*)IBVectorTop(&t->expStack);
	IBassert(ret);
	return &ret->pfxs;
}
IBVector* IBTaskGetExpNameOPsTop(IBTask* t) {
	IBExpects* ret;
	IBassert(t);
	ret = (IBExpects*)IBVectorTop(&t->expStack);
	IBassert(ret);
	return &ret->nameOps;
}