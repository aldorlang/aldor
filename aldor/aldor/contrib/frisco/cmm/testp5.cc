/* Test program for the gcalloc.c */

/* Externals */

#include <stdio.h>
#include <stdlib.h>
#include "cmm.h"

#define	VECT_SIZE	1000

struct  cell : CmmObject 
{
  cell  *car;
  cell  *cdr;
  int  value;
  cell(cell *initcar, cell *initcdr, int initvalue);
  void traverse();
};

typedef  cell* CP;

void cell::traverse()  
{
  CmmHeap *heap = Cmm::heap;
  heap->scavenge((CmmObject **)&car);
  heap->scavenge((CmmObject **)&cdr);
}


cell::cell(cell *initcar, cell *initcdr, int initvalue)
{
  car = initcar;
  cdr = initcdr;
  value = initvalue;
}

struct vector : CmmObject 
{
  vector  *car;
  vector  *cdr;
  int  value1;
  char bytes[VECT_SIZE];
  int  value2;
  vector(vector* x, vector* y, int v1, int v2);
};

typedef  vector* VP;


vector::vector(vector* x, vector* y, int v1, int v2)  
{
  car = x;
  cdr = y;
  value1 = v1;
  value2 = v2;
}

/* Test program */

int  init_global = 2,
array_global[VECT_SIZE];

void  printtree(CP zp)
{
  CP  tp;

  tp = zp;
  while  (tp != NULL)  
    {
      printf("%x: %d  ", tp, tp->value);
      tp = tp->cdr;
    }
  printf("\n");
  tp = zp;
  while  (tp != NULL)  
    {
      printf("%x: %d  ", tp, tp->value);
      tp = tp->car;
    }
  printf("\n");
}

void  listtest1()
{
  int  i, j;
  CP  lp, zp;
  
  printf("List test 1\n");
  lp = NULL;
  for (i = 0; i <= VECT_SIZE ; i++)  
    {
      if  (i % 15 != 14)
	printf("%d ", i);
      else
	printf("%d\n", i);
      zp = new cell(NULL, lp, i);
      lp = zp;
      Cmm::heap->collect();
      zp = lp;
      for (j = i; j >= 0 ; j--)  
	{
	  if ((zp == NULL) || (zp->value != j))
	    printf("LP is not a good list when j = %d\n", j);
	  zp = zp->cdr;
	}
    }
  printf("\n");		   
}

void  vectortest()
{
  int  i, j;
  VP  lp, zp;
  
  printf("Vector test\n");
  lp = NULL;
  for (i = 0; i <= 100 ; i++)  
    {
      if  (i % 15 != 14)
	printf("%d ", i);
      else
	printf("%d\n", i);
      zp = new vector(NULL, lp, i, i);
      lp = zp;
      Cmm::heap->collect();
      zp = lp;
      for (j = i; j >= 0 ; j--)  
	{
	  // mswCheckAllocatedObj(zp);
	  // if (zp->cdr) mswCheckAllocatedObj(zp->cdr);
	
	  if ((zp == NULL) || (zp->value1 != j)  ||  (zp->value2 != j))
	    printf("LP is not a good list when j = %d\n", j);
	  zp = zp->cdr;
	}
    }
  printf("\n");		   
}

CP  treetest()
{
  int  i;
  CP  tp, zp;

  printf("Tree test\n");
  tp = new cell(NULL, NULL, 0);
  for (i = 1; i <= 5; i++)  
    {
      zp = new cell(tp, tp, i);
      tp = zp;
    }
  Cmm::heap->collect();
  mswCheckHeap(1);
  zp = new cell(tp, tp, 6);
  Cmm::heap->collect();
  mswCheckHeap(1);
  printtree(zp);
  return(zp);
}

void  listtest2()
{
  int  i, j, length = 10000, repeat = 1000;
  CP  lp, zp;

  printf("List Test 2\n");
  for (i = 0; i < repeat; i++)  
    {
      if  (i % 15 != 14)
	printf("%d ", i);
      else {
	printf("%d\n", i);
        /* Cmm::heap->collect(); */
	 mswCheckHeap(0);
      }
      /* Build the list */
      lp = NULL;
      for  (j = 0; j < length; j++)  
	{
	  zp = new cell(NULL, lp, j);
	  lp = zp;
	}
      /* Check the list */
      zp = lp;
      for (j = length-1; j >= 0 ; j--)  
	{
	  // mswCheckAllocatedObj(zp);
	  // if (zp->cdr) mswCheckAllocatedObj(zp->cdr);

	  if ((zp == NULL) || (zp->value != j))
	    printf("LP is not a good list when j = %d\n", j);
	  zp = zp->cdr;
	}
    }
  printf("\n");		   
}

CP  gp;		/* A global pointer */



main()
{
  Cmm::heap = ::new MarkAndSweep(MSW_Automatic);
  /* List construction test */
  /* printf("WARNING: skipping listtest1()!\n"); */
  listtest1();
  mswCheckHeap(1);

  /* List of vectors > 1 page */
  vectortest();
  mswCheckHeap(1);

  /* Tree construction test */
  gp = treetest();
  mswCheckHeap(1);

  /* 1000 10000 node lists */
  listtest2();
  mswCheckHeap(1);

  /* Check that tree is still there */
  printtree(gp);

  mswCheckHeap(1);
  mswShowInfo();

  exit(0);
}
