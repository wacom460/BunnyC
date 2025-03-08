#include "imbored.h"

void IBTypeInfoInit(IBTypeInfo* ti, IBOp type, char* name) {
	IBASSERT0(ti);
	memset(ti, 0, sizeof * ti);
	IBStrInitWithCStr(&ti->name, name);
	ti->type = type;
	IBVectorInit(&ti->members, sizeof * ti,
		OP_IBTypeInfo, IBVEC_DEFAULT_SLOTCOUNT);
	IB_SETMAGICP(ti);
}
void IBTypeInfoFree(IBTypeInfo* ti) {
	IBASSERT0(ti);
	IB_ASSERTMAGICP(ti);
	IBVectorFree(&ti->members, IBTypeInfoFree);
	IBStrFree(&ti->name);
}
void IBTypeInfoFindMember(IBTypeInfo* ti, char* name, IBTypeInfo** outDP) {
	IBTypeInfo* nti = 0;
	int idx = 0;
	IBASSERT0(ti);
	IBASSERT0(name);
	IBASSERT0(strlen(name) > 0);
	while (nti = IBVectorIterNext(&ti->members, &idx)) {
		IB_ASSERTMAGICP(nti);
		IBASSERT0(nti->name.start)
			if (!strcmp(nti->name.start, name)) break;
	}
	if (outDP) (*outDP) = nti;
}