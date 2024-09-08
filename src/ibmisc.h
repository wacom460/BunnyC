#ifndef IBMISC_H_
#define IBMISC_H_

#define IBMAGIC (1011933)
#define IB_ASSERTMAGICP(o)\
	assert((o)->initMagic==IBMAGIC)
#define IB_ASSERTMAGIC(o)\
	assert((o).initMagic==IBMAGIC)
#define IB_DEFMAGIC \
	unsigned int initMagic
#define IB_SETMAGICP(o)\
	(o)->initMagic=IBMAGIC
#define IB_SETMAGIC(o)\
	(o).initMagic=IBMAGIC
#define IB_FILE_EXT "ib"
#ifndef bool
#define bool char
#endif
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
#define IB_TRUESTR "true"
#define IBFALSESTR "false"
#endif