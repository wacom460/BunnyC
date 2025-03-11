#include "imbored.h"

void IBCodeBlockInit(IBCodeBlock* block) {
	IBASSERT0(block);
	IB_SETMAGICP(block);
	IBStrInit(&block->header);
	IBStrInit(&block->variables);
	IBStrInit(&block->varsInit);
	IBStrInit(&block->codeLeft);
	IBStrInit(&block->code);
	IBStrInit(&block->codeRight);
	IBStrInit(&block->footer);
	IBNameInfoDBInit(&block->localVariables);
}

void IBCodeBlockFinish(IBCodeBlock* block, IBStr* output) {
	IBASSERT0(block);
	IB_ASSERTMAGICP(block);
	IBStrAppendFmt(output,
		"%s%s%s%s%s%s%s",
		block->header.start,
		block->variables.start,
		block->varsInit.start,
		block->codeLeft.start,
		block->code.start,
		block->codeRight.start,
		block->footer.start);
}

void IBCodeBlockFree(IBCodeBlock* block) {
	IBASSERT0(block);
	IB_ASSERTMAGICP(block);
	IBNameInfoDBFree(&block->localVariables);
	IBStrFree(&block->header);
	IBStrFree(&block->variables);
	IBStrFree(&block->varsInit);
	IBStrFree(&block->codeLeft);
	IBStrFree(&block->code);
	IBStrFree(&block->codeRight);
	IBStrFree(&block->footer);
}