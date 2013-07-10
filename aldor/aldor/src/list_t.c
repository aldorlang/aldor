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

# include "axlgen.h"

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
#if EDIT_1_0_n1_07
		l = listCons(Pointer)((Pointer) (long) i,l);
#else
		l = listCons(Pointer)((Pointer)i,l);
#endif
		printf("list %d = ",i);
		listPrint(Pointer)(osStdout,l,testListPrintElt);
		printf("\n");
	}

	printf("listLength = ");
#if EDIT_1_0_n1_07
	printf("%d\n", (int) listLength(Pointer)(l));
#else
	printf("%d\n",listLength(Pointer)(l));
#endif

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
#if EDIT_1_0_n1_07
	printf("element %p ", p);
#else
	printf("element %x ", (int) p);
#endif
 	if (listMemq(Pointer)(l,p)) printf("is a list member.\n");
	else printf("is not a list member.\n");

	i = listLength(Pointer)(l);
	v = (Pointer *) stoAlloc(OB_Other,i*sizeof(Pointer));
	printf("listFillVector: \n");
	listFillVector(Pointer)(v,l);
	printf("vector = <");
#if EDIT_1_0_n1_07
	for (j = 0; j < i; j++) printf("%p ", v[j]);
#else
	for (j = 0; j < i; j++) printf("%x ", (int) v[j]);
#endif
	printf(">\n");
	stoFree((Pointer) v);
	printf("\n");

	printf("listFree: ");
	listFree(Pointer)(l);
	listFreeTo(Pointer)(l2,l1);	/* l1 is contained in l2 */
	listFree(Pointer)(l1);

}

#endif
