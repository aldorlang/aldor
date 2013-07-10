/*****************************************************************************
 *
 * store.h: Storage management.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _STORE_H_
#define _STORE_H_

# include "axlport.h"

# include "msw.h"

typedef struct _stoInfo {
	int    code;
	Bool   hasPtrs;
} *StoInfo, StoInfoObj;

#define stoAlloc(code,size)	mswAlloc((size))
#define stoFree			mswFree
#define stoGc()			mswCollectNow()
#define stoSize(p)		mswGetObjSize(p)
#define stoResize(p,size)	mswRealloc((p), (size))
#define stoRegister(x)		

#if KILLMELATER
# define STO_CODE_LIMIT	32

extern ULong            stoBytesOwn;
extern ULong            stoBytesAlloc;
extern ULong		stoBytesFree;
extern ULong		stoBytesGc;
extern ULong		stoPiecesGc[STO_CODE_LIMIT];

#define stoCode(p)		0
#define stoRecode(p, i)		NULL

#define stoAudit()		mswCheckHeap(1)

extern ULong            stoSize         (Pointer p);

#define stoShow()		;

extern int		stoCtl		(int cmd, ...);

# define StoCtl_GcLevel	1
			/* Control GC activity (when available).
			 * Arg 1 int: See values above.
			 *
			 * Once StoCtl_GcLevel_Never has been called, the
			 * demand and automatic gc support is forever disabled.
			 */
# define StoCtl_GcLevel_Never	  0
# define StoCtl_GcLevel_Demand	  1
# define StoCtl_GcLevel_Automatic 2

# define StoCtl_GcFile	2
			/* Place for gc messages.
			 * Arg 1 FILE *: NULL => quiet.
			 */

# define StoCtl_Wash	3
			/* Control filling of new+freed pieces for debugging.
			 * Arg 1 Bool: true/false => do/don't fill.
			 */

/*
 * Install handler for error situations.
 */
typedef MostAlignedType*	(*StoErrorFun)  (int errnum);
extern StoErrorFun     	stoSetHandler   (StoErrorFun);


# define StoErr_OutOfMemory	1  /* Cannot get any more storage. */
# define StoErr_UsedNonalloc	2  /* Given pointer to non-allocated store. */
# define StoErr_CantBuild	3  /* Can't build internal structure. */

#endif

#endif /* !_STORE_H_ */


