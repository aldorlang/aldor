/*****************************************************************************
 *
 * btree.h: B-Tree data structure.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _BTREE_H
#define _BTREE_H

#include "cport.h"

/*
 * B-Tree properties:
 * 1a. Root     has 0   <= nKeys <= 2*t-1
 * 1b. Non-root has t-1 <= nKeys <= 2*t-1
 * 2.  Non-leaf has nKeys + 1 branches
 * 3.  br[0]<=key[0]<=br[1]<=key[1]<=...<=br[2*t-2]<=key[2*t-2]<=br[2*t-1]
 *     where br[i] means all keys in the tree branch[i].
 */

typedef struct btree     *BTree;
typedef struct btreePart *BTreePart;

typedef ULong            BTreeKey;
typedef Pointer          BTreeElt;
 
typedef BTreeElt	 (*BTreeEltFun)  (BTreeElt);
typedef BTree   	 (*BTreeAllocFun)(ULong nbytes);
typedef void    	 (*BTreeFreeFun) (BTree);

struct btreePart {
	BTree            branch;         /* all <= key */
	BTreeKey         key;            
	BTreeElt         entry;
};

struct btree {
	Bool             isLeaf;
	unsigned short   t;             /* Min number of branches. */
	unsigned short   nKeys;         /* t-1 <= nKeys <= 2*t-1 */
	struct btreePart part[NARY];	/* Last k/e unused.*/
};

#define btreeKey(bt,i)          ((bt)->part[i].key)
#define btreeElt(bt,i)          ((bt)->part[i].entry)
#define btreeBranch(bt,i)       ((bt)->part[i].branch)
#define	btreeNodeSize(t) 	fullsizeof(struct btree,2*(t),struct btreePart)

extern BTree    btreeNew        (Length t);
extern void     btreeFree       (BTree);
extern void     btreeInsert     (BTree*, BTreeKey, BTreeElt);
extern void     btreeDelete     (BTree*, BTreeKey, BTreeElt *);

extern BTree    btreeNewX       (Length t, BTreeAllocFun);
extern void     btreeFreeX      (BTree,    BTreeFreeFun);
extern void     btreeInsertX    (BTree*, BTreeKey, BTreeElt,  BTreeAllocFun);
extern void     btreeDeleteX    (BTree*, BTreeKey, BTreeElt *,BTreeFreeFun);

extern BTree    btreeNMap       (BTreeEltFun, BTree);
extern BTree    btreeSearchEQ   (BTree, BTreeKey, int *pIx);
extern BTree    btreeSearchGE   (BTree, BTreeKey, int *pIx);
extern BTree    btreeSearchMin  (BTree, int *pIx);
extern BTree    btreeSearchMax  (BTree, int *pIx);

extern int      btreeCheck      (BTree);
extern int     	btreePrint      (FILE *, BTree);

#endif
