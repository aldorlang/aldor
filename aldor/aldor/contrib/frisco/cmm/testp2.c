#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "msw.h"

enum _TreeTag {
    TAG_Cons,
    TAG_Int,
    TAG_String,
    TAG_Double,
    TAG_Ptr

  };

typedef enum _TreeTag	TreeTag;


typedef void *	Ptr;
typedef struct _ConsStruct * List;

typedef struct _ConsStruct {
        TreeTag	tag;
        List 	cdr;
	Ptr  	car;
} ConsStruct;

typedef struct _IntStruct {
        TreeTag	tag;
	int	val;
} IntStruct;

typedef struct _DoubleStruct {
        TreeTag	tag;
	double	val;
} DoubleStruct;

typedef struct _StringStruct {
        TreeTag	tag;
	char *	str;
} StringStruct;


typedef union _TreeUnion {
        TreeTag		tag;
        ConsStruct 	cons;
	IntStruct  	intVal;
	DoubleStruct  	doubleVal;
	StringStruct  	stringVal;
} TreeUnion;

typedef TreeUnion * Tree;


#define car(l)	(l)->car
#define cdr(l)	(l)->cdr
#define push(val, lst)   (lst) = cons((val), (lst))
#define pop(lst)	 (lst) = cdr(lst)

List
cons(Ptr val, List list)
{
	List cons = (List) mswAlloc(sizeof(ConsStruct));

	cons->car = val;
	cons->cdr = list;

	return cons;
}

void
listFree(List l)
{
        List l0;

        while (l) {
	     l0 = cdr(l);
	     mswFree(l);
	     l = l0;
	}
}

List
listReverse(List l)
{
	List res = NULL;

	while (l) {
		push (car(l), res);
		pop(l);
	}
	return res;
}

List
listNReverse(List l)
{
	List res = listReverse(l);
	listFree(l);
	return res;
}

/******************************************************************************
 *
 * :: Tree Management Functions
 *
 *****************************************************************************/

void
treePrint(Tree tree)
{
         switch (tree->tag) {
	 case TAG_Cons:
	   printf("[");
	   treePrint((Tree)tree->cons.car);
	   printf(",");
	   treePrint((Tree) tree->cons.cdr);
	   printf("]");
	   break;
	 case TAG_Int:
	   printf("%d", tree->intVal.val);
	   break;
	 case TAG_String:
	   printf("\"");
	   printf("%s", tree->stringVal.str);
	   printf("\"");
	   break;
	 case TAG_Double:
	   printf("%.1f", tree->doubleVal.val);
	   break;
	 default:
	   printf("BUG: unknown tag %d\n", tree->tag);
	   exit(-1);
	 }

	 return;
}

int
treeCount(Tree tree)
{
         switch (tree->tag) {
	 case TAG_Cons:
	   return treeCount((Tree)tree->cons.car) + treeCount((Tree) tree->cons.cdr);

	 case TAG_Int:
	 case TAG_String:
	 case TAG_Double:
	   return 1;
	 default:
	   printf("BUG: unknown tag %d\n", tree->tag);
	   exit(-1);
	   return 0;
	 }
}

/* NOTE: strings in string trees are not released to check the effectiveness
 *       of the gc.
 */
void
treeFreeDeeply(Tree tree)
{
         switch (tree->tag) {
	 case TAG_Cons:
	   treeFreeDeeply((Tree)tree->cons.car);
	   treeFreeDeeply((Tree) tree->cons.cdr);
	   break;
	 case TAG_Int:
	 case TAG_String:
	 case TAG_Double:
	   mswFree(tree);
	   break;
	 default:
	   printf("BUG: unknown tag %d\n", tree->tag);
	   exit(-1);
	   return;
	 }

	 return;
}

Tree
treeNewString(char * str)
{
        Tree	tree;

	tree = (Tree) mswAlloc(sizeof(StringStruct));

	tree->tag = TAG_String;
	tree->stringVal.str = str;
	return tree;
}

Tree
treeNewInt(int n)
{
        Tree	tree;

	tree = (Tree) mswAlloc(sizeof(IntStruct));

	tree->tag = TAG_Int;
	tree->intVal.val = n;
	return tree;
}

Tree
treeNewDouble(double d)
{
        Tree	tree;

	tree = (Tree) mswAlloc(sizeof(DoubleStruct));

	tree->tag = TAG_Double;
/*	tree->doubleVal.val = d;  */
	tree->intVal.val = 1;
	return tree;
}

Tree
treeNewCons(Ptr val, Tree list)
{
        Tree	tree;

	tree = (Tree) mswAlloc(sizeof(ConsStruct));

	tree->tag = TAG_Cons;
	tree->cons.car = val;
	tree->cons.cdr = (List) list;
	return tree;
}

/******************************************************************************
 *
 * :: Test Functions
 *
 *****************************************************************************/

#define LIST_LEN	600

int maxListSize = LIST_LEN;
int maxIter     = 10;

List
dummyList(int len)
{
	int i;
	List l = cons((Ptr) -1, NULL);
	List l0 = l;

	for (i = 0; i < len; i++)
	       push((Ptr) i, l);

	cdr(l0) = l; 	/* make a circular list */

	return l;
}

void
foo()
{
	List l = NULL;
	List l1;
        int i;

	for (i = 0; i < maxListSize; i++) {
		push(dummyList(i), l);
	}

	l1 = l;
	i  = 0;

	mswCheckHeap(1);

	while (l1)
		l1 = cdr(l1);

	l1 = listReverse(l);
	l1 = listNReverse(l1);
	while (l1)
		l1 = cdr(l1);
}

char *
strFromInt(int v)
{
        char buf[40];
	char * ret;
	int  n;

#if defined(__sparc)
	strlen(sprintf(buf, "%d", v));
#else
	n = sprintf(buf, "%d", v);
#endif
	ret = (char*) mswAlloc(n+1);
	return strcpy(ret, buf);
}

void
test1()
{
        int    i;
        Tree tree = treeNewInt(10);

	tree = treeNewCons(tree, treeNewDouble(32.3));
	tree = treeNewCons(tree, treeNewString("Natascia"));

	for (i = 0; i < maxListSize; i += 1)
	     tree = treeNewCons(
		     treeNewCons(
		       treeNewCons(treeNewInt(i), treeNewDouble(i * 1.0)),
		       treeNewString(strFromInt(i))),
		     tree);

	/* treePrint(tree); */
	printf("Tree size: %d nodes.\n", treeCount(tree));

	printf("\n\n");

	tree = treeNewInt(10);
}

/* #define TEST_ON_DEMAND */

#ifdef TEST_ON_DEMAND

int
main(int argc, char ** argv)
{
	int 	i;

        mswInit(MSW_OnDemand);

	if (argc >= 2)
	      maxListSize = atoi(argv[1]);

	if (argc >= 3)
	      maxIter = atoi(argv[2]);

	printf("\nGarbage Collection ON DEMAND \n\n");
	printf("\n+++++ Using maxListSize = %d \n\n", maxListSize);
	printf("\n+++++ Using maxIter = %d \n\n", maxIter);

	test1();
	mswCollect();
	mswShowInfo(); /* Final statistics should give same heap size */

	for (i = 0; i < maxIter - 1; i++) {
		test1();	
		mswCollect();
	}

	mswShowInfo();
	mswCheckHeap(1);
	return 0;
}

#else /* TEST_AUTOMATIC */

int
main(int argc, char ** argv)
{
	int 	i;

        mswInit(MSW_Automatic);

	if (argc >= 2)
	      maxListSize = atoi(argv[1]);

	if (argc >= 3)
	      maxIter = atoi(argv[2]);

	printf("\nGarbage Collection AUTOMATIC \n\n");
	printf("\n+++++ Using maxListSize = %d \n\n", maxListSize);
	printf("\n+++++ Using maxIter = %d \n\n", maxIter);

	test1();
	mswShowInfo();

	for (i = 0; i < maxIter - 1; i++)
		test1();	

	mswShowInfo();
	mswCheckHeap(1);
	return 0;
}

#endif /* TEST_AUTOMATIC */
