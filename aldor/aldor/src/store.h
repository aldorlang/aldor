/*****************************************************************************
 *
 * store.h: Storage management.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _STORE_H_
#define _STORE_H_

#include "cport.h"
#include "timer.h"

/*
 * Martin's experimental stuff
 */
typedef int (*StoTraceFun)(Pointer *, Pointer *);

# define STO_CODE_LIMIT	32

typedef struct _stoInfo {
	int	code;
	Bool	hasPtrs;
} *StoInfo, StoInfoObj;

extern ULong		stoBytesOwn;
extern ULong		stoBytesAlloc;
extern ULong		stoBytesFree;
extern ULong		stoBytesGc;
extern ULong		stoPiecesGc[STO_CODE_LIMIT];


extern MostAlignedType *stoAlloc	(unsigned code, ULong size);
extern MostAlignedType *stoCAlloc	(unsigned code, ULong size);
extern void		stoFree		(Pointer p);
extern void		stoGc		(void);
extern void		stoTune		(void);

extern ULong		stoSize		(Pointer p);
extern MostAlignedType *stoResize	(Pointer p, ULong size);

extern unsigned		stoCode		(Pointer p);
extern MostAlignedType *stoRecode	(Pointer p, unsigned code);

extern Bool		stoIsPointer	(Pointer p);

extern void		stoAudit	(void);
extern void		stoShow		(void);
extern void		stoShowDetail	(int);
extern void		stoRegister	(StoInfo);
extern int		stoShowArgs	(char *);

extern int		stoWritablePointer	(Pointer);
extern void		stoSetAldorTracer	(int, Pointer);
extern void		stoSetTracer		(int, StoTraceFun);
extern int		stoMarkObject		(Pointer);

extern TmTimer		stoGcTimer		(void);

/*
 * Control storage management behaviour.
 */
extern int		stoCtl		(int cmd, ...);
			/*
			 * 0 => success, -1 => failure.
			 */

# define StoCtl_GcLevel			1
			/* Control GC activity (when available).
			 * Arg 1 int: See values above.
			 *
			 * Once StoCtl_GcLevel_Never has been called, the
			 * demand and automatic gc support is forever disabled.
			 */
# define StoCtl_GcLevel_Never		0
# define StoCtl_GcLevel_Demand		1
# define StoCtl_GcLevel_Automatic	2

# define StoCtl_GcFile			2
			/* Place for gc messages.
			 * Arg 1 FILE *: NULL => quiet.
			 */

# define StoCtl_Wash			3
			/* Control filling of new+freed pieces for debugging.
			 * Arg 1 Bool: true/false => do/don't fill.
			 */

/*
 * Install handler for error situations.
 */
typedef MostAlignedType*	(*StoErrorFun)	(int errnum);

extern StoErrorFun		stoSetHandler	(StoErrorFun);

# define StoErr_OutOfMemory	1  /* Cannot get any more storage. */
# define StoErr_UsedNonalloc	2  /* Given pointer to non-allocated store. */
# define StoErr_CantBuild	3  /* Can't build internal structure. */
# define StoErr_FreeBad		4  /* asked to free something not allocated */

#endif /* !_STORE_H_ */
