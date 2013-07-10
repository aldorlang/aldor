/*****************************************************************************
 *
 * symbol_t.c: Test symbol type.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#if !defined(TEST_SYMBOL) && !defined(TEST_ALL)

void testSymbol(void) { }

#else

# include "axlgen.h"

void
testSymbol(void)
{
	int	i;
	Symbol	sym;
	String	str;


	for (i = 0; i < 3; i++) {
		switch(i) {
			case 0:
				sym = symProbe("symbol",SYM_LOOK);
				str = sym ? sym->str : "-- failed --";
				printf("symbol string: \"%s\" ", str);
				printf("with option: SYM_LOOK\n");
				break;
			case 1:
				sym = symInternConst("internal symbol constant");
				str = sym ? sym->str : "-- failed --";
				printf("symbol string: \"%s\" ", str);
				printf("with option: SYM_ALLOC\n");
				break;
			case 2:
				sym = symProbe("copy symbol",SYM_STRCOPY);
				str = sym ? sym->str : "-- failed --";
				printf("symbol string: \"%s\" ", str);
				printf("with option: SYM_STRCOPY\n");
				break;
		}
	}

	sym = symIntern("internal symbol");
	str = sym ? sym->str : "-- failed --";
	printf("symbol string: \"%s\" ", str);
	printf("with options: SYM_ALLOC and ,SYM_STRCOPY\n");
}

#endif
