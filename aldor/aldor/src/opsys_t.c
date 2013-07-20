/*****************************************************************************
 *
 * opsys_t.c: Test operating system dependent code.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#if !defined(TEST_OPSYS) && !defined(TEST_ALL)

void testOpsys(void) { }

#else

/*
 *!! This is very limited at the moment.  More tests are needed.
 */
#include "axlgen.h"
#include "axlport.h"
#include "opsys.h"

local void
showMemMap(struct osMemMap **mm)
{
	char	*s;

	if (!mm)
		printf("    Not available\n");
	else
		for ( ; (*mm)->use != OSMEM_END; mm++) {
			switch ((*mm)->use) {
			case OSMEM_IDATA:	s = "initial data"; break;
			case OSMEM_DDATA:	s = "dynamic data"; break;
			case OSMEM_STACK:	s = "stack"; 	break;
			default:		s = "?????"; 	break;
			}
#if EDIT_1_0_n1_07
			printf("    [%p, %p)  %s\n", (*mm)->lo, (*mm)->hi, s);
#else
			printf("    [%#10lx, %#10lx)  %s\n",
				(long) (*mm)->lo, (long) (*mm)->hi, s);
#endif
		}
	printf("\n");
}

#define	NALLOC	10 	/* Must be even */

void
testOpsys(void)
{
	struct osMemMap	**mm;
	int		i;
	ULong		n;
	Pointer		allocv[NALLOC];

	printf("Full memory map: \n");
	mm = osMemMap(~0);
	showMemMap(mm);

	printf("Location of stack: \n");
	mm = osMemMap(OSMEM_STACK);
	showMemMap(mm);

	printf("osAlloc and osFree: \n");
	printf("-- allocate small to large. free back to front.\n");
	for (i = 0; i < NALLOC; i++) {
		n = i;
		allocv[i] = osAlloc(&n);
	}
	osAllocShow();
	for (i = 0; i < NALLOC; i++) {
		osFree(allocv[NALLOC-i-1]);
	}
	osAllocShow();
	printf("-- allocate large to small. free front to back.\n");
	for (i = 0; i < NALLOC; i++) {
		n = NALLOC - i - 1;
		allocv[i] = osAlloc(&n);
	}
	osAllocShow();
	for (i = 0; i < NALLOC; i++) {
		osFree(allocv[i]);
	}
	osAllocShow();
	printf("-- allocate. free evens front to back and odds back to front.\n");
	for (i = 0; i < NALLOC; i++) {
		n = i;
		allocv[i] = osAlloc(&n);
	}
	osAllocShow();
	for (i = 0; i < NALLOC; i += 2) {
		printf("Freeing %#x\n", i);
		osFree(allocv[i]);
		osAllocShow();
		printf("Freeing %#x\n", NALLOC-i-1);
		osFree(allocv[NALLOC-i-1]);	/* Needs even NALLOC. */
		osAllocShow();
	}
}

#endif
