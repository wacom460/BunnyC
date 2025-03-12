#include "imbored.h"

void IBNameInfoInit(IBNameInfo* info)
{
	info->type = OP_NotSet;
	info->cast = OP_NotSet;
	info->name = NULL;
	IBVectorInit(&info->members, sizeof(IBNameInfo), OP_NameInfo, IBVEC_DEFAULT_SLOTCOUNT);
}

void IBNameInfoFree(IBNameInfo* info)
{
	IBASSERT0(info);
	IBVectorFree(&info->members, IBNameInfoFree);
	free(info->name);
}

IBOp IBNameInfoAddMember(struct IBLayer3* ibc, IBNameInfo* ni, char* name, IBOp type, IBNameInfo** niDP)
{
	IBNameInfo* info = NULL, * found = NULL;
	IBassert(ibc);
	IBASSERT0(ni);
	IBASSERT0(name);
	IBassert(ni != &((IBCodeBlock*)IBVectorFront(&ibc->CodeBlockStack))->localVariables);
	found = IBNameInfoFindMember(ni, name);
	if (found) {
		if (niDP) (*niDP) = found;
		return OP_AlreadyExists;
	}
	IBVectorPush(&ni->members, &info);
	IBassert(info);
	info->type = type;
	info->name = strdup(name);
	if (niDP) (*niDP) = info;
	return OP_OK;
}

IBOp IBNameInfoFindType(IBNameInfo* ni, char* name)
{
	IBNameInfo* pair=0;
	int idx=0;
	while (pair = IBVectorIterNext(&ni->members, &idx)) {
		if (!strcmp(pair->name, name))
			return pair->type;
	}
	return OP_NotFound;
}

IBNameInfo* _IBNameInfoFindMember(IBNameInfo* ni, char* name, int lineNum)
{
	IBNameInfo* pair = NULL;
	int idx = 0;
	IBassert(ni);
	//DbgFmt("[%d]"__FUNCTION__"(,%s)\n", lineNum,name);
	while (pair = IBVectorIterNext(&ni->members, &idx))
		if (!strcmp(pair->name, name)) return pair;
	return NULL;
}
