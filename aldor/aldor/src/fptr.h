#ifndef FPTR_H
#define FPTR_H

#include "foam.h"
#include "ttable.h"

typedef struct pointerSet {
	AInt count;
	TSet(Pointer) tbl;
} *PtrSet;
	
typedef struct foamPointer {
	PtrSet pset;
	Foam context;
	AInt posn;
} *FoamPtr;

DECLARE_LIST(FoamPtr);

PtrSet psetNew(void);
Bool   psetIsEmpty(PtrSet);

FoamPtr fptrNew(PtrSet ctxt, Foam foam, AInt index);
void    fptrFree(FoamPtr foam);

Foam fptrVal(FoamPtr ptr);
void fptrSet(FoamPtr ptr, Foam value);

#endif
