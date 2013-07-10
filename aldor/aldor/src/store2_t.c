/*****************************************************************************
 *
 * store2_t.c: Random allocations and deallocations.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/
 
#if !defined(TEST_STORE2) && !defined(TEST_ALL)

void testStore2(void) { }

#else

# include "axlgen.h"
 
struct tree {
	int		argc;
	int		refc;
	int		treeno;
	int		isLeaf;
	int		isSized;
	union {
		struct tree 	*argv[1];
		char		*data[1];
	}		u;
};
 
static long		treeBytes = 0;
static long		treeNo = 0;

static struct tree * 	leafNewEmpty	(int nbytes);
static struct tree * 	treeNewEmpty	(int argc);
static void 		treeFree	(struct tree *);
static void 		treeCheck	(struct tree *, int treeno);
static void 		treePrint	(FILE *, struct tree *);
static long 		treeSize	(struct tree *);
static void		treeSetElt	(struct tree *, int, struct tree *);
 
static void		randomForest	(void);
 
struct tree		**forest;
 
int	printforest	= 0;
int	FOREST_ITERS	= 10000;
int	FOREST_GC	= 1000;
int	FOREST_SIZE	= 30;
int	TREE_MAX_WIDTH	= 4;
int	TREE_FUZZ	= 16;
int	boxchars	= 0;
 
			
void
testStore2(void)
{
	int	i;
	long	bO, bF, grossBytes;
 
	bO = stoBytesAlloc - stoBytesFree - stoBytesGc;
 
	printforest = 1;
	grossBytes = 0;
 
	forest = (struct tree **)stoAlloc(int0,
					  FOREST_SIZE*sizeof(struct tree *));
	for (i = 0; i < FOREST_SIZE; i++) forest[i] = 0;
 
	randomForest();

	for (i = 0; i < FOREST_SIZE; i++) {
		long sz = treeSize(forest[i]);
		printf("%d: %ld bytes\n", i, sz);
		grossBytes += sz;
		if (printforest) treePrint(osStdout, forest[i]);
	}
 
	for (i = 0; i < FOREST_SIZE; i++) {
		treeCheck(forest[i], treeNo);
		treeFree(forest[i]);
	}

	bF = stoBytesAlloc - stoBytesFree - stoBytesGc;
	printf("Memory: %lu (owned) %lu (alloc) %ld (net) %ld (gross)\n",
	       stoBytesOwn, bF - bO, treeBytes, grossBytes);
 
	stoFree(forest);
}
 
/*
 * Make random forest.
 *
 * At each step, choose a random tree in the forest and replace it.
 * The replacement is either null, or formed by allocating a new root
 * with subtrees chosen from the others in the forest.
 */
 
static void
randomForest(void)
{
	int	i, b, k;
 
	for (i = 0; i < FOREST_ITERS; i++) {
		int		treeno;
		struct tree	*t;
 
		if (i % FOREST_GC == 0) {
			stoGc();
			stoAudit();
		}

		/* Select a random tree for replacement */
		treeno = rand() % FOREST_SIZE;
 
		/* Either make a new leaf, or construct a new root */
		/* One in 4 times make a leaf. */
		if ((rand() & 0x30) == 0) {
			/* LEAF: One in 8 times make a big one */
			long	r  = rand();
			int	fz = r % TREE_FUZZ;
			int	sz = 120 + fz;
			if ((r & 0x70) == 0) sz *= 100;
			treeCheck(forest[treeno], treeNo);
/*
			treeFree(forest[treeno]);
*/
			forest[treeno] = leafNewEmpty(rand() % sz);
		}
		else {
			/* ROOT: Fill with subtrees */
			t = treeNewEmpty(rand() % TREE_MAX_WIDTH);
 
/*
			treeFree(forest[treeno]);
*/
			treeCheck(forest[treeno], treeNo);
			forest[treeno] = 0;
			for (b = 0; b < t->argc; b++) {
				k = rand() % FOREST_SIZE;
				treeSetElt(t, b, forest[k]);
			}
			forest[treeno] = t;
		}
	}
	stoGc();
	stoAudit();
	stoShow();

}
 
/*
 *
 * Trees to build and destroy.
 *
 */
 
static struct tree *
treeNewEmpty(int argc)
{
	struct tree	*t;
	int		sz = sizeof(*t)+(argc-1)*sizeof(t);
 
	t = (struct tree *) stoAlloc(int0, sz);
	t->isLeaf = 0;
	t->isSized = 0;
	t->argc   = argc;
	t->refc   = 1;
	t->treeno = treeNo++;
	treeBytes += sz;
	return t;
}
 
static struct tree *
leafNewEmpty(int nbytes)
{
	struct tree	*t;
	int		sz = sizeof(*t) - sizeof(t) + nbytes;
 
	t = (struct tree *) stoAlloc(int0, sz);
	t->isLeaf = 1;
	t->isSized = 0;
	t->argc   = nbytes;
	t->refc   = 1;
	t->treeno = treeNo++;
	treeBytes += sz;
	return t;
}
 
static void
treeFree(struct tree *t)
{
        if (!t || --t->refc) return;
 
	if (t->isLeaf) {
		treeBytes -= sizeof(*t)-sizeof(t)+t->argc;
	}
	else {
		int		i;
		for (i = 0; i < t->argc; i++)
			treeFree(t->u.argv[i]);
		treeBytes -= sizeof(*t)-sizeof(t)+t->argc*sizeof(t);
	}
	stoFree(t);
}
 
static void
treeCheck(struct tree *t, int treeno)
{
        if (!t) return;
 
	assert(t->treeno <= treeno);

	if (!t->isLeaf) {
		int		i;
		for (i = 0; i < t->argc; i++)
			treeCheck(t->u.argv[i], t->treeno);
	}
}
 
static void
treeSetElt(struct tree *t, int i, struct tree *b)
{
	if (b) b->refc++;
	t->u.argv[i] = b;
}
 
static long
treeSize(struct tree *t)
{
	int	i, sz;

        if (!t || t->isSized) return 0;

	t->isSized = 1;

	if (t->isLeaf) {
		sz = sizeof(*t) - sizeof(t) + t->argc * sizeof(char);
	}
	else {
		sz = sizeof(*t) - sizeof(t) + t->argc * sizeof(t);
 
		for (i = 0; i < t->argc; i++)
			sz += treeSize(t->u.argv[i]);
	}
	return sz;
}
 
	
/*
 *
 * Print a tree like this:
 *
 *   -+-*
 *    |
 *    +-+-0
 *    | |
 *    | +-*
 *    |
 *    +-+-*
 *
 */
char *TPR_ROOT_CHAR;	/* for horiz. stem to child */
char *TPR_ROOT_SPACE;	/* for horiz. space beside stem */
char *TPR_NULL_CHAR;	/* for null pointer */
char *TPR_ZERO_CHAR;	/* for 0-child tree */
char *TPR_SIB0_CHAR;	/* for point  at first  child of one */
char *TPR_SIB1_CHAR;	/* for tee    at first  child of many */
char *TPR_SIB2_CHAR;	/* for tee    at middle child of many */
char *TPR_SIBN_CHAR;	/* for corner at last   child of many */
char *TPR_SIDE_CHAR;    /* for vert.  stem to sibling */
char *TPR_SIDE_SPACE;   /* for vert.  space after last sibling */
 
char	doSide[1000];
 
static void
treePrintPrefix(FILE *f, int depth)
{
	int	d;
 
	if (!boxchars) {
		for (d = 0; d <= depth; d++) {
			fputs(TPR_ROOT_SPACE, f);
			fputs(doSide[d] ? TPR_SIDE_CHAR : TPR_SIDE_SPACE, f);
		}
		fputs("\n", f);
	}
	for (d = 0; d <= depth-1; d++) {
		fputs(TPR_ROOT_SPACE, f);
		fputs(doSide[d] ? TPR_SIDE_CHAR : TPR_SIDE_SPACE, f);
	}
	fputs(TPR_ROOT_SPACE, f);
}
 
static void
treePrint0(FILE *f, struct tree *t, int depth)
{
	if (depth > sizeof(doSide) - 1) return;
 
	fputs(TPR_ROOT_CHAR, f);
 
        if (!t) {
		fputs(TPR_NULL_CHAR, f);
		fputs("\n", f);
	}
	else if (t->isLeaf) {
		fprintf(f, "[%d bytes]\n", t->argc);
	}
	else {
		doSide[depth] = 0;
		switch (t->argc) {
		case 0:
			fputs(TPR_ZERO_CHAR, f);
			fputs("\n", f);
			break;
		case 1:
			fputs(TPR_SIB0_CHAR, f);
			treePrint0(f, t->u.argv[0], depth+1);
			break;
		default: {
			int i;
			fputs(TPR_SIB1_CHAR, f);
			doSide[depth] = 1;
			treePrint0(f, t->u.argv[0], depth+1);
			treePrintPrefix(f, depth);
 
			for (i = 1; i < t->argc-1; i++) {
				fputs(TPR_SIB2_CHAR, f);
				treePrint0(f, t->u.argv[i], depth+1);
				treePrintPrefix(f, depth);
			}
			fputs(TPR_SIBN_CHAR, f);
			doSide[depth] = 0;
			treePrint0(f, t->u.argv[t->argc-1], depth+1);
		} }
	}
}
 
static void
treePrint(FILE *f, struct tree *t)
{
	if (boxchars) {
		TPR_ROOT_CHAR  = "";
		TPR_ROOT_SPACE = "";
		TPR_NULL_CHAR  = "*";
		TPR_ZERO_CHAR  = "0";
		TPR_SIB0_CHAR  = "\304"; /* Horiz.:    D */
		TPR_SIB1_CHAR  = "\302"; /* Top tee:   B */
		TPR_SIB2_CHAR  = "\303"; /* Left tee:  C */
		TPR_SIBN_CHAR  = "\300"; /* LL corner: @ */
		TPR_SIDE_CHAR  = "\263"; /* Vert.:     3 */
		TPR_SIDE_SPACE = " ";
	}
	else {
		TPR_ROOT_CHAR  = "-";
		TPR_ROOT_SPACE = " ";
		TPR_NULL_CHAR  = "*";
		TPR_ZERO_CHAR  = "0";
		TPR_SIB0_CHAR  = "+";
		TPR_SIB1_CHAR  = "+";
		TPR_SIB2_CHAR  = "+";
		TPR_SIBN_CHAR  = "+";
		TPR_SIDE_CHAR  = "|";
		TPR_SIDE_SPACE = " ";
	}
 
	treePrint0(f, t, int0);
	fputs("\n", f);
}

#endif
