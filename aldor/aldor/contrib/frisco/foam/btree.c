/*****************************************************************************
 *
 * btree.c: B-Tree data structure.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

# include "axlgen.h"

local BTree    btreeAllocNode    (ULong nbytes);
local void     btreeFreeNode     (BTree);

local void     btreeSplitChild   (BTree, int keyIx, BTreeAllocFun);
local void     btreeUnsplitChild (BTree, int keyIx, BTreeFreeFun);
local void     btreeRotateDown   (BTree, int keyIx);
local void     btreeRotateUp     (BTree, int keyIx);

local void     btreeDelete0      (BTree, BTreeKey, BTreeElt *, BTreeFreeFun);
local int      btreePrint0       (FILE *, BTree, int level);
local int      btreeCheck0       (BTree, Length t, BTreeKey *, BTreeKey *);

local BTree
btreeAllocNode(ULong nbytes)
{
        return (BTree) stoAlloc(OB_BTree, nbytes);
}

local void
btreeFreeNode(BTree b)
{
        stoFree((Pointer) b);
}


BTree
btreeNew(Length	t)
{
        return btreeNewX(t, btreeAllocNode);
}

BTree
btreeNewX(Length t, BTreeAllocFun btalloc)
{
        BTree b   = btalloc(btreeNodeSize(t));
        b->isLeaf = true;
        b->nKeys  = 0;
        b->t      = t;
        return b;
}

void
btreeFree(BTree x)
{
	btreeFreeX(x,btreeFreeNode);
}

void
btreeFreeX(BTree x, BTreeFreeFun btfree)
{
        BTreePart       xp, x0, xN;
        int             n;

        n  = x->nKeys;
        x0 = x->part;
        xN = x->part + n;

        if (!x->isLeaf) {
        	for (xp = x0; xp < xN; xp++)
                	btreeFreeX(xp->branch,btfree);
        	btreeFreeX(xN->branch,btfree);
	}
       	btfree(x);
}

/*
 * Search for k in x.
 * Return node b and index *pindex such that b == 0 or b->key[*pindex] == k.
 */
BTree
btreeSearchEQ(BTree x, BTreeKey k, int *pindex)
{
        BTreePart       xp, xN, x0;

        for (;;) {
                x0 = x->part;
                xN = x->part + x->nKeys;
                for (xp = x0; xp < xN && k > xp->key; xp++)
                        ;
                if (xp < xN && k == xp->key) {
                        *pindex = xp - x0;
                        return x;
                }
                if (x->isLeaf)
                        return 0;
                x = xp->branch;
        }
}

/*
 * Search for the least k' >= k in x.
 * Return node b and index *pindex such that b == 0 or b->key[*pindex] == k'.
 */

BTree
btreeSearchGE(BTree x, BTreeKey k, int *pindex)
{
        BTreePart       xp, xN, x0;
        BTree           lastx =  0;
        int             lasti = -1;

        for (;;) {
                x0 = x->part;
                xN = x->part + x->nKeys;
                for (xp = x0; xp < xN && k > xp->key; xp++)
                        ;
                if (xp < xN && k == xp->key)
                        return (*pindex = xp - x0, x);
                if (x->isLeaf) {
                        if (xp < xN && k <= xp->key)
                                return (*pindex = xp - x0, x);
                        else if (lastx != 0)
                                return (*pindex = lasti, lastx);
                        else
                                return 0;
                }
                if (xp < xN) {
                        lastx = x;
                        lasti = xp - x0;
                }
                x = xp->branch;
        }
}

/*
 * Search for the smallest key k in x.
 * Return node b and index *pindex such that b->key[*pindex] == k.
 */
BTree
btreeSearchMin(BTree x, int *pindex)
{
        while (!x->isLeaf) x = x->part[0].branch;
        return (*pindex = 0, x);
}

/*
 * Search for the largest key k in x.
 * Return node b and index *pindex such that b->key[*pindex] == k.
 */
BTree
btreeSearchMax(BTree x, int *pindex)
{
        while (!x->isLeaf) x = x->part[x->nKeys].branch;
        return (*pindex = x->nKeys-1, x);
}

/*
 * Verify B-Tree properties of x.
 */

int
btreeCheck(BTree x)
{
        return btreeCheck0(x, x->t, NULL, NULL);
}

local int
btreeCheck0(BTree x, Length t, BTreeKey *pLoBd, BTreeKey *pHiBd)
{
        BTreePart       xp, x0, xN;
        int             n;

	if (x->t != t) return -1;

        n  = x->nKeys;
        x0 = x->part;
        xN = x->part + n;

        /* Check number of keys */
        if (!pLoBd && !pHiBd) { if (  0 > n || n > 2*t-1) return -2; }
        else                  { if (t-1 > n || n > 2*t-1) return -3; }

        /* Check order of keys */
        if (pLoBd && *pLoBd > x0->key) return -4;
        for (xp = x0+1; xp < xN; xp++)
                if ((xp-1)->key > xp->key) return -5;
        if (pHiBd && (xN-1)->key > *pHiBd) return -6;

        /* Check subtrees */
        if (x->isLeaf) return 0;

        if (btreeCheck0(x0->branch, t, pLoBd, &x0->key)) return -7;
        for (xp = x0+1; xp < xN; xp++)
                if (btreeCheck0(xp->branch,t,&(xp-1)->key,&xp->key)) return -8;
        if (btreeCheck0(xN->branch, t, &(xN-1)->key, pHiBd)) return -9;

        return 0;
}

/*
 * Split x->part[i].branch, where x is a non-full node and x->part[i].branch
 * is a full child.  The child x->part[i].branch becomes two children in x,
 * with the median key of x->part[i].branch promoted into x between them.
 */
local void
btreeSplitChild(BTree x, int i, BTreeAllocFun btalloc)
{
        int     j, t = x->t;

        BTree   y = x->part[i].branch;
        BTree   z = btalloc(btreeNodeSize(t));

        z->isLeaf = y->isLeaf;
        z->t      = t;

        for (j = 0; j < t - 1; j++) {
                z->part[j].key   = y->part[t+j].key;
                z->part[j].entry = y->part[t+j].entry;
        }
        if (!y->isLeaf) for (j = 0; j < t; j++)
                z->part[j].branch = y->part[t+j].branch;

        z->nKeys  = t - 1;
        y->nKeys  = t - 1;

        for (j = x->nKeys; j >= i + 1; j--)
                x->part[j+1].branch = x->part[j].branch;
        x->part[i+1].branch = z;

        for (j = x->nKeys - 1; j >= i; j--) {
                x->part[j+1].key   = x->part[j].key;
                x->part[j+1].entry = x->part[j].entry;
        }
        x->part[i].key   = y->part[t-1].key;
        x->part[i].entry = y->part[t-1].entry;
        x->nKeys++;

}

/*
 * Merge x->part[i].branch, x->part[i].key, x->part[i+1].branch and close
 * the gap.  The two branches are assumed to have t-1 keys.
 */
local void
btreeUnsplitChild(BTree x, int i, BTreeFreeFun btfree)
{
        BTree   y, z;
        int     j, xn, yn, zn;

        y = x->part[i].branch;
        z = x->part[i+1].branch;

        xn = x->nKeys;
        yn = y->nKeys;
        zn = y->nKeys;

        y->part[yn].key   = x->part[i].key;
        y->part[yn].entry = x->part[i].entry;

        for (j = 0; j < zn; j++) {
                y->part[yn+j+1].key   = z->part[j].key;
                y->part[yn+j+1].entry = z->part[j].entry;
        }
        if (!y->isLeaf)
            for (j = 0; j <= zn; j++) {
                y->part[yn+j+1].branch = z->part[j].branch;
        }
        y->nKeys += z->nKeys + 1;
        btfree(z);

        for (j = i+1; j < xn; j++) {
                x->part[j-1].key   = x->part[j].key;
                x->part[j-1].entry = x->part[j].entry;
        }
        for (j = i+2; j <= xn; j++) {
                x->part[j-1].branch = x->part[j].branch;
        }
        x->nKeys--;
}

/*
 * Transfer a key out of x->part[i+1].branch to x and one from x to x->part[i].branch.
 */
local void
btreeRotateDown(BTree x, int i)
{
        BTree   y, z;
        int     j, yn, zn;
        y = x->part[i].branch;
        z = x->part[i+1].branch;
        yn = y->nKeys;
        zn = z->nKeys;
        /* Rotate */
        y->part[yn].key   = x->part[i].key;
        y->part[yn].entry = x->part[i].entry;
        x->part[i].key    = z->part[0].key;
        x->part[i].entry  = z->part[0].entry;
        if (!y->isLeaf)
                y->part[yn+1].branch  = z->part[0].branch;
        /* Slide down */
        for (j = 1; j < zn; j++) {
                z->part[j-1].key   = z->part[j].key;
                z->part[j-1].entry = z->part[j].entry;
        }
        if (!z->isLeaf)
            for (j = 1; j <=zn; j++)
                z->part[j-1].branch = z->part[j].branch;
        y->nKeys++;
        z->nKeys--;
}

/*
 * Transfer a key out of x->part[ii].branch to x
 * ane one from x to x->part[ii+1].branch.
 */
local void
btreeRotateUp(BTree x, int ii)
{
        BTree   y, z;
        int     i = ii+1, j, yn, zn;
        /* Transfer from branch i-1 (z) to i (y). */
        z  = x->part[i-1].branch;
        y  = x->part[i].branch;
        zn = z->nKeys;
        yn = y->nKeys;
        /* Slide up. */
        for (j = yn-1; j >= 0; j--) {
                y->part[j+1].key = y->part[j].key;
                y->part[j+1].entry = y->part[j].entry;
        }
        if (!y->isLeaf)
            for (j = yn; j >= 0; j--)
                y->part[j+1].branch = y->part[j].branch;
        /* Rotate */
        y->part[0].key     = x->part[i-1].key;
        y->part[0].entry   = x->part[i-1].entry;
        x->part[i-1].key   = z->part[zn-1].key;
        x->part[i-1].entry = z->part[zn-1].entry;
        if (!y->isLeaf)
                y->part[0].branch = z->part[zn].branch;
        y->nKeys++;
        z->nKeys--;
}

/*
 * Insert key k into tree *pr.
 */
void
btreeInsert(BTree *pr, BTreeKey k, BTreeElt e)
{
        btreeInsertX(pr, k, e, btreeAllocNode);
}

void
btreeInsertX(BTree *pr, BTreeKey k, BTreeElt e, BTreeAllocFun btalloc)
{
        BTree   x = *pr;
        int     i, t = x->t;

        if (x->nKeys == 2*t - 1) {
                BTree s = btalloc(btreeNodeSize(t));
                *pr = s;
                s->isLeaf    = false;
                s->nKeys     = 0;
                s->t         = t;
                s->part[0].branch = x;
                btreeSplitChild(s, int0, btalloc);
                x = s;
        }
        while (!x->isLeaf) {
                for (i = x->nKeys - 1; i >= 0 && k < x->part[i].key; i--)
                        ;
                i++;
                if (x->part[i].branch->nKeys == 2*t - 1) {
                        btreeSplitChild(x, i, btalloc);
                        if (k > x->part[i].key) i++;
                }
                x = x->part[i].branch;
        }
        for (i = x->nKeys - 1; i >= 0 && k < x->part[i].key; i--) {
                x->part[i+1].key   = x->part[i].key;
                x->part[i+1].entry = x->part[i].entry;
        }
        x->part[i+1].key   = k;
        x->part[i+1].entry = e;
        x->nKeys++;
}

/*
 * Delete a key-entry pair from a BTree.
 */

void
btreeDelete(BTree *pr, BTreeKey k, BTreeElt *pe)
{
        btreeDeleteX(pr, k, pe, btreeFreeNode);
}

void
btreeDeleteX(BTree *pr, BTreeKey k, BTreeElt *pe, BTreeFreeFun btfree)
{
        btreeDelete0(*pr, k, pe, btfree);
        if ((*pr)->nKeys == 0 && !(*pr)->isLeaf) {
                BTree   r = (*pr)->part[0].branch;
                btfree(*pr);
                *pr = r;
        }
}

/*
 * Recursively descend tree to delete k.
 * Most of the work is to ensure that t-1 keys remain after the deletion.
 */
local void
btreeDelete0(BTree x, BTreeKey k, BTreeElt *pe, BTreeFreeFun btfree)
{
        int             xn, t, i, j;
        BTree           ox;
        BTreeKey        ok;
        BTreeElt        oe;

        /* Determine whether k is in the node x. */
        xn = x->nKeys;
	t  = x->t;
        for (i = 0; i < xn && k > x->part[i].key; i++)
                        ;
        if (i < xn && k == x->part[i].key) {
                if (pe) *pe = x->part[i].entry;

                if (x->isLeaf) {
                        /* Delete k from x. */
                        for (j = i+1; j < xn; j++) {
                                x->part[j-1].key = x->part[j].key;
                                x->part[j-1].entry = x->part[j].entry;
                        }
                        x->nKeys--;
                }
                else if (x->part[i].branch->nKeys   > t-1) {
                        /* Replace k by max of x->part[i].branch. */
                        ox = btreeSearchMax(x->part[i].branch, &j);
                        ok = ox->part[j].key;
                        btreeDelete0(x->part[i].branch, ok, &oe, btfree);
                        x->part[i].key   = ok;
                        x->part[i].entry = oe;
                }
                else if (x->part[i+1].branch->nKeys > t-1) {
                        /* Replace k by min of x->part[i+1].branch. */
                        ox = btreeSearchMin(x->part[i+1].branch, &j);
                        ok = ox->part[j].key;
                        btreeDelete0(x->part[i+1].branch, ok, &oe, btfree);
                        x->part[i].key   = ok;
                        x->part[i].entry = oe;
                }
                else {
                        btreeUnsplitChild(x, i, btfree);
                        btreeDelete0(x->part[i].branch, k, pe, btfree);
                }
        }
        else {
                if (x->part[i].branch->nKeys == t - 1) {
                        /* Make node x->part[i].branch have enough keys. */
                        if (i<x->nKeys && x->part[i+1].branch->nKeys>t-1)
                                btreeRotateDown(x, i);
                        else if (i>0   && x->part[i-1].branch->nKeys>t-1)
                                btreeRotateUp(x, i-1);
                        else {
                                if (i == x->nKeys) i--;
                                btreeUnsplitChild(x, i, btfree);
                        }
                }
                btreeDelete0(x->part[i].branch, k, pe, btfree);
        }
}


/*
 * Map a function onto the entries of a BTree.
 */

BTree
btreeNMap(BTreeEltFun f, BTree x)
{
        int     i;

	if (!x) return 0;

        if (x->isLeaf) {
                for (i = 0; i < x->nKeys; i++)
                        x->part[i].entry = f(x->part[i].entry);
        }
        else {
                for (i = 0; i < x->nKeys; i++) {
                        btreeNMap(f, x->part[i].branch);
                        x->part[i].entry = f(x->part[i].entry);
                }
                btreeNMap(f, x->part[i].branch);
        }
        return x;
}

/*
 * Print a BTree.
 */

int
btreePrint(FILE *fout, BTree x)
{
        return btreePrint0(fout, x, int0);
}

local int
btreePrint0(FILE *fout, BTree x, int level)
{
        int     i, cc = 0;
        if (x->isLeaf) {
                cc += fprintf(fout, "%*s[", 2*level, " ");
                for (i = 0; i < x->nKeys; i++)
#if EDIT_1_0_n1_07
                        cc += fprintf(fout, i==0 ? "%d":" %d", (int) x->part[i].key);
#else
                        cc += fprintf(fout, i==0 ? "%d":" %d", x->part[i].key);
#endif
                cc += fprintf(fout, "]\n");
        }
        else {
                cc += fprintf(fout, "%*s[\n", 2*level, " ");
                for (i = 0; i < x->nKeys; i++) {
                        cc += btreePrint0(fout, x->part[i].branch, level+1);
                        cc += fprintf(fout, "%*s%ld\n",
				      2*(level+1)," ", x->part[i].key);
                }
                cc += btreePrint0(fout, x->part[i].branch, level+1);
                cc += fprintf(fout, "%*s]\n", 2*level, " ");
        }
	return cc;
}
