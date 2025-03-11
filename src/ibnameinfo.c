#include "imbored.h"

void IBNameInfoInit(IBNameInfo* info) {
	info->type = OP_NotSet;
	info->cast = OP_NotSet;
	info->name = NULL;
}

void IBNameInfoFree(IBNameInfo* info) {
	free(info->name);
}

void IBNameInfoDBInit(IBNameInfoDB* db) {
	IBVectorInit(&db->pairs, sizeof(IBNameInfo), OP_NameInfo, IBVEC_DEFAULT_SLOTCOUNT);
}

void IBNameInfoDBFree(IBNameInfoDB* db) {
	IBASSERT0(db);
	IBVectorFree(&db->pairs, IBNameInfoFree);
}

IBOp IBNameInfoDBAdd(struct IBLayer3* ibc, IBNameInfoDB* db, char* name, IBOp type, IBNameInfo** niDP) {
	IBNameInfo* info = NULL, * found = NULL;
	IBassert(ibc);
	IBASSERT0(db);
	IBASSERT0(name);
	IBassert(db != &((IBCodeBlock*)IBVectorFront(&ibc->CodeBlockStack))->localVariables);
	found = IBNameInfoDBFind(db, name);
	if (found) {
		if (niDP) (*niDP) = found;
		return OP_AlreadyExists;
	}
	IBVectorPush(&db->pairs, &info);
	IBassert(info);
	info->type = type;
	info->name = strdup(name);
	if (niDP) (*niDP) = info;
	return OP_OK;
}

IBOp IBNameInfoDBFindType(IBNameInfoDB* db, char* name) {
	IBNameInfo* pair;
	int idx;
	idx = 0;
	while (pair = IBVectorIterNext(&db->pairs, &idx)) {
		if (!strcmp(pair->name, name))
			return pair->type;
	}
	return OP_NotFound;
}

IBNameInfo* _IBNameInfoDBFind(IBNameInfoDB* db, char* name, int lineNum) {
	IBNameInfo* pair = NULL;
	int idx = 0;
	IBassert(db);
	//DbgFmt("[%d]"__FUNCTION__"(,%s)\n", lineNum,name);
	while (pair = IBVectorIterNext(&db->pairs, &idx))
		if (!strcmp(pair->name, name)) return pair;
	return NULL;
}