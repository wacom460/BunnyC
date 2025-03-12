#include "imbored.h"

struct IBVecData {
	union {
		IBObj obj;
		IBStr str;
		IBTask task;
		IBOp op;
		bool boolean;
		IBExpects expects;
		IBNameInfo ni;
		IBDictKey dictKey;
		IBDictKeyDef dictKeyDef;
		IBTypeInfo ti;
		IBVector vec;
	};
};

void IBVectorInit(IBVector* vec, int elemSize, IBOp type, int count)
{
	void* m;
	IBASSERT0(count > 0);
	memset(vec, 0, sizeof * vec);
	vec->initMagic = IBMAGIC;
	vec->elemSize = elemSize;
	vec->type = type;
	vec->elemCount = 0;
	vec->initialSlotCount = count;
	vec->doNotShrink = count > 1;
	vec->slotCount = count;
	vec->dataSize = vec->elemSize * vec->slotCount;
	vec->data = NULL;
	m = malloc(vec->dataSize);
	IBASSERT0(m);
	vec->data = m;
	IBASSERT0(vec->data);
	memset(vec->data, 0, vec->dataSize);
}

struct IBVecData* IBVectorGet(IBVector* vec, int idx)
{
	IBASSERT0(vec);
	IB_ASSERTMAGICP(vec);
	IBASSERT0(vec->elemCount >= 0);
	IBASSERT0(vec->slotCount >= 0);
	IBASSERT0(idx >= 0);
	IBASSERT0(idx < vec->elemCount);
	if (vec->elemCount < 1
		|| idx >= vec->elemCount) return NULL;
	return (struct IBVecData*)((char*)vec->data + vec->elemSize * idx);
}

void* _IBVectorIterNext(IBVector* vec, int* idx, int lineNum)
{
	//DbgFmt("[%d]"__FUNCTION__,lineNum);
	IBASSERT0(idx);
	IBASSERT0(vec);
	IB_ASSERTMAGICP(vec);
	IBASSERT0((*idx) >= 0);
	IBASSERT0(vec->elemCount <= vec->slotCount);
	IBASSERT0(vec->elemCount + vec->slotCount + vec->dataSize >= 0);
	//DbgFmt("(%p,%d(%p),%d)\n", vec, *idx, idx, lineNum);
	if (!vec || !idx) return NULL;
	if ((*idx) >= vec->elemCount) return NULL;
	return (void*)((char*)vec->data + (vec->elemSize * ((*idx)++)));
}

void _IBVectorPush(IBVector* vec, struct IBVecData** dataDP IBDBGFILELINEPARAMS)
{
	struct IBVecData* topPtr;
	IBASSERT0(vec);
	IB_ASSERTMAGICP(vec);
	IBASSERT0(vec->elemSize);
	IBASSERT0(vec->type)
		if (vec->elemCount + 1 > vec->slotCount) {
			void* ra = 0;
			vec->slotCount++;
			vec->dataSize = vec->elemSize * vec->slotCount;
			//DbgFmt("vec->dataSize: %d\n", vec->dataSize);
			IBASSERT0(vec->data);
			IBREALLOC(ra, vec->data, vec->dataSize);
			IBASSERT0(ra);
			if (ra) vec->data = ra;
			vec->reallocCount++;
		}
	topPtr = (struct IBVecData*)((char*)vec->data + (vec->elemSize * vec->elemCount));
	memset(topPtr, 0, vec->elemSize);
	int pil = ClampInt(vec->elemCount, 0, IBVEC_PUSHINFO_MAX - 1);
	vec->PushInfo[pil].lineNum = ln;
	vec->PushInfo[pil].filePath = file;
	vec->elemCount++;
	_IBVectorReinitPushInfo(vec);
	if (dataDP) *dataDP = topPtr;
}

void _IBVectorCopyPush(IBVector* vec, void* elem IBDBGFILELINEPARAMS)
{
	IBASSERT0(vec);
	IB_ASSERTMAGICP(vec);
	struct IBVecData* top;
	_IBVectorPush(vec, &top IBDBGFPL2);//FIX
	memcpy(top, elem, vec->elemSize);
}

void _IBVectorCopyPushBool(IBVector* vec, bool val IBDBGFILELINEPARAMS)
{
	_IBVectorCopyPush(vec, &val IBDBGFPL2);
}

void _IBVectorCopyPushOp(IBVector* vec, IBOp val IBDBGFILELINEPARAMS)
{
	_IBVectorCopyPush(vec, &val IBDBGFPL2);
}

void _IBVectorCopyPushIBColor(IBVector* vec, IBColor col IBDBGFILELINEPARAMS)
{
	_IBVectorCopyPush(vec, &col IBDBGFPL2);
}

struct IBVecData* IBVectorTop(IBVector* vec)
{
	IBASSERT0(vec);
	IB_ASSERTMAGICP(vec);
	IBASSERT0(vec->elemCount >= 0);
	IBASSERT0(vec->slotCount >= 0);
	if (vec->elemCount <= 0) return NULL;
	return IBVectorGet(vec, vec->elemCount - 1);
}

struct IBVecData* IBVectorFront(IBVector* vec)
{
	IBASSERT0(vec);
	if (vec->elemCount <= 0) return NULL;
	IBASSERT0(vec->data);
	return vec->data;
}

void _IBVectorPop(IBVector* vec, void(*freeFunc)(void*))
{
	void* ra;
	IBASSERT0(vec);
	if (vec->elemCount <= 0) return;
	if (freeFunc) freeFunc((void*)IBVectorGet(vec, vec->elemCount - 1));
	vec->elemCount--;
	if (!vec->doNotShrink) vec->slotCount = vec->elemCount;
	if (vec->slotCount < 1)vec->slotCount = 1;
	vec->dataSize = vec->elemSize * vec->slotCount;
	if (!vec->doNotShrink && vec->elemCount) {
		IBASSERT0(vec->data);
		IBREALLOC(ra, vec->data, vec->dataSize);
		IBASSERT0(ra);
		if (ra) vec->data = ra;
		IBASSERT0(vec->data);
	}
	_IBVectorReinitPushInfo(vec);
}

void _IBVectorPopFront(IBVector* vec, void(*freeFunc)(void*))
{
	long long newSize;
	void* ra;
	if (vec->elemCount < 1) return;
	vec->elemCount--;
	if (!vec->doNotShrink) vec->slotCount = vec->elemCount;
	if (vec->slotCount < 1)vec->slotCount = 1;
	if (vec->elemCount > 1) {
		size_t rns = ((vec->dataSize * vec->elemCount) - vec->dataSize);
		newSize = vec->doNotShrink ? (vec->dataSize)
			: rns;
		IBASSERT0(newSize >= vec->dataSize);
		ra = malloc(newSize);
		IBASSERT0(ra);
		if (ra) {
			memcpy(ra, IBVectorGet(vec, 1), rns);
			free(vec->data);
			vec->data = ra;
		}
	}
	_IBVectorReinitPushInfo(vec);
}

void IBVectorFreeSimple(IBVector* vec)
{
	free(vec->data);
}

void _IBVectorReinitPushInfo(IBVector* vec)
{
	/*IBASSERT0(vec->slotCount<=IBVEC_PUSHINFO_MAX);*/
	for (int i = 0;i < IBVEC_PUSHINFO_MAX;i++) {
		vec->PushInfo[i].ptr =
			i < vec->elemCount ? IBVectorGet(vec, i) : 0;
	}
}
