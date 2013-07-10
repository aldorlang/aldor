/*****************************************************************************
 *
 * table.h: Hash table data type.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _TABLE_H_
#define _TABLE_H_

# include "axlport.h"

typedef Pointer         TblKey;
typedef Pointer         TblElt;

typedef Hash            (* TblHashFun)          (TblKey);
typedef Bool            (* TblEqFun)            (TblKey, TblKey);        
typedef Bool		(* TblTestEltFun)       (TblElt);        
typedef TblElt          (* TblMapEltFun)        (TblElt);
typedef int             (* TblPrKeyFun)         (FILE *, TblKey);
typedef int             (* TblPrEltFun)         (FILE *, TblElt);
typedef void            (* TblFreeKeyFun)       (TblKey);
typedef void            (* TblFreeEltFun)       (TblElt);

struct TblSlot {
	TblKey          key;
	TblElt          elt;
	Hash            hash;
	struct TblSlot  *next;
};

typedef struct {
	TblHashFun      hashFun;
	TblEqFun        eqFun;
	Pointer         info;           /* Use-specific extra info. */
	Length          count;
	Length          buckc;
	struct TblSlot  **buckv;
} *Table;

typedef struct {
	struct TblSlot  **curr;
	struct TblSlot  **last;
	struct TblSlot  *link;
} TableIterator;

/*
 *  tblNew(hashfn, eqfn)  creates a new hash table.
 *    The parameters hashfn and eqfn are the hash function and equality tests.
 *    If hashfn is 0, the key pointer value is used.
 *    If eqfn is 0, the equality test is `=='.
 *  tblFree(t)  deallocates the table t.
 *  tblCopy(t)  creates a copy of the table t.
 *  tblNMap(f,t)        modify t by replacing each entry `e' by `f(e)'.
 *  tblSize(g)  returns the number of entries.
 *  tblElt(t,k,dflt)    searches table and returns `dflt' if not found.
 *  tblSetElt(t,k,e)    sets element for given key.
 *  tblDrop(t,k,dflt)   remove the entry for given key.
 *  tblPrint(file,t,prk,pre)    prints a table.
 *    If a function pointer is 0, then that part is not printed.
 *  tblTest()   preforms a self-test for tables.
 *
 *  Abstract iteration over tables:
 * 
 *    Table t;
 *    TableIterator it;
 *    for (tblITER(it,t); tblMORE(it); tblSTEP(it)) {
 *        k = tblKEY(it);
 *        e = tblELT(it);
 *        ...
 *        tblSETKEY(it, k);     -- Possible but unusual. k must hash the same.
 *        tblSETELT(it, e);
 *    }
 */

extern Table    tblNew          (TblHashFun hash, TblEqFun eq);
extern void     tblFree		(Table);
extern void     tblFreeDeeply   (Table, TblFreeKeyFun, TblFreeEltFun);
extern Table    tblCopy         (Table);
extern Table	tblRemoveIf	(Table, TblFreeEltFun, TblTestEltFun);
extern Table    tblNMap         (TblMapEltFun, Table);
extern Length   tblSize         (Table);
extern TblElt   tblElt          (Table, TblKey, TblElt dflt);
extern TblElt   tblSetElt       (Table, TblKey, TblElt);
extern Table    tblDrop         (Table, TblKey);
extern int      tblPrint        (FILE *, Table, TblPrKeyFun, TblPrEltFun);
extern int      tblColumnPrint  (FILE *, Table, TblPrKeyFun, TblPrEltFun);

#define tblITER(it, t)  _tblITER(&(it), t)
#define tblMORE(it)     ((it).curr <= (it).last)
#define tblSTEP(it)     ((((it).link=(it).link->next))==0 ? _tblSTEP(&(it)) : 1)
#define tblKEY(it)      ((it).link->key)
#define tblELT(it)      ((it).link->elt)
#define tblSETKEY(it,k) ((it).link->key = (k))
#define tblSETELT(it,e) ((it).link->elt = (e))

extern int      _tblITER        (TableIterator *, Table);
extern int      _tblSTEP        (TableIterator *);

#endif
