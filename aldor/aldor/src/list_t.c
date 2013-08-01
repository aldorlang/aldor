/*****************************************************************************
 *
 * list_t.c: Test the list type.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#if !defined(TEST_LIST) && !defined(TEST_ALL)

void testList(void) { }

#else

#include "axlgen.h"
#include "list.h"
#include "opsys.h"
#include "store.h"

int
testListPrintElt(FILE *fout, Pointer p)
{
	return fprintf(fout, "%ld", (long) p);
}

void
testList(void)
{
	int		i, j;
	Pointer		p, *v;
	PointerList	l,l0,l1,l2;


	printf("listCons: \n");
	l = listCons(Pointer)((Pointer)0,listNil(Pointer));
	for (i = 1; i < 10; i++) {
		l = listCons(Pointer)((Pointer) (long) i,l);
		printf("list %d = ",i);
		listPrint(Pointer)(osStdout,l,testListPrintElt);
		printf("\n");
	}

	printf("listLength = ");
	printf("%d\n", (int) listLength(Pointer)(l));

	if (listIsLength(Pointer)(l,i)) printf("listIsLength is %d: true\n",i);
	else printf("listIsLength is %d: false\n",i);

	printf("listReverse: \n");
	l1 = listReverse(Pointer)(l);
	listPrint(Pointer)(osStdout,l1,testListPrintElt);
	printf("\n");

	printf("listConcat: \n");
	l2 = listConcat(Pointer)(l,l1);
	listPrint(Pointer)(osStdout,l2,testListPrintElt);
	printf("\n");

	printf("listFreeCons: \n");
	l = listFreeCons(Pointer)(l);
	listPrint(Pointer)(osStdout,l,testListPrintElt);
	printf("\n");

	l0 = l;
	printf("listDrop: \n");
	l = listDrop(Pointer)(l,1);
	listFreeCons(Pointer)(l0);
	listPrint(Pointer)(osStdout,l,testListPrintElt);
	printf("\n");

	printf("listElt: \n");
	p = listElt(Pointer)(l,1);
	printf("element %p ", p);
 	if (listMemq(Pointer)(l,p)) printf("is a list member.\n");
	else printf("is not a list member.\n");

	i = listLength(Pointer)(l);
	v = (Pointer *) stoAlloc(OB_Other,i*sizeof(Pointer));
	printf("listFillVector: \n");
	listFillVector(Pointer)(v,l);
	printf("vector = <");
	for (j = 0; j < i; j++) printf("%p ", v[j]);
	printf(">\n");
	stoFree((Pointer) v);
	printf("\n");

	printf("listFree: ");
	listFree(Pointer)(l);
	listFreeTo(Pointer)(l2,l1);	/* l1 is contained in l2 */
	listFree(Pointer)(l1);
	printf("\n");
	
	printf("listList: ");
	l = listList(Pointer)(2, (Pointer) (long) 4, (Pointer)(long) 5);
	listPrint(Pointer)(osStdout,l,testListPrintElt);
	l = listList(Pointer)(1, (Pointer) (long) 1);
	listPrint(Pointer)(osStdout,l,testListPrintElt);
	l = listList(Pointer)(0);
	listPrint(Pointer)(osStdout,l,testListPrintElt);
	listFree(Pointer)(l);
	printf("\n");
}

#endif
