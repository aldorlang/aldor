#ifndef _UFIND_H
#define _UFIND_H

#include "axlobs.h"

typedef struct ufTable *UFTable;
typedef struct ufElt *UFElt;

struct ufTable {
	Table elts;
};

UFTable uftNew		(void);
UFTable uftTheEmpty	(void);
Length  uftSize		(UFTable);

Bool  uftIsEmpty	(UFTable);
UFElt uftGet		(UFTable, TForm);
void  uftUnion		(UFTable, TForm, TForm);
void  uftSetData	(UFTable, TForm, Pointer);
UFElt uftProbe		(UFTable, TForm);
UFElt uftEltFollow	(UFElt);

AInt    uftEltCount		(UFElt e);
Pointer uftEltData		(UFElt e);
TForm   uftEltTForm		(UFElt e);

typedef struct uftIterator UFTIterator;

struct uftIterator {
	TableIterator tblIter;
};

#define uftITER(_it, uf) _uftITER(&_it, uf)
#define uftMORE(_it) tblMORE((_it).tblIter)
#define uftSTEP(_it) tblSTEP((_it).tblIter)
#define uftKEY(_it)  ((TForm) tblKEY(_it.tblIter))
#define uftELT(_it)  ((Pointer) uftEltData(uftEltFollow(tblELT(_it.tblIter))))
#define uftREP(_it)  uftEltTForm(uftEltFollow(tblELT(_it.tblIter)))

void _uftITER(UFTIterator *, UFTable);

#endif
