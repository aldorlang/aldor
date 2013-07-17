/*****************************************************************************
 *
 * fluid.c: Fluid variables.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * See comments in fluid.h
 */

#include "axlgen0.h"
#include "fluid.h"
#include "store.h"

# define FLUID_INC	100

struct fluidCell *	fluidStack     = 0;

int			fluidLevel     = 0,
			fluidLimit     = 0,
			fluidIncrement = FLUID_INC;

int			scopeLevel = 0;

struct fluidCell *
fluidGrow(void)
{
	if (fluidLimit == 0) {
		fluidLimit  = fluidIncrement;
		fluidStack  = (struct fluidCell *)
		  stoAlloc(int0, fluidLimit * sizeof(struct fluidCell));
	}
	else {
		fluidLimit += fluidIncrement;
		fluidStack  = (struct fluidCell *)
		  stoResize(fluidStack, fluidLimit * sizeof(struct fluidCell));
	}
	return fluidStack;
}

void
fluidUnwind(int toCell, Bool isLongJmp)
{
	int	i;
	int	l0 = scopeLevel;

	for (i = fluidLevel - 1; i >= toCell; i--) {
		struct fluidCell *fs   = fluidStack + i;
		Pointer		  plhs = fs->pglobal, prhs = fs->pstack;

		if ((l0 < fs->scopeLevel)||(l0 > fs->scopeLevel && !isLongJmp))
			bug("Improper return from scope '%s'.", fs->scopeName);

		if      (fs->size == sizeof(Pointer))
			*((Pointer *) plhs) =  *((Pointer *) prhs);
		else if (fs->size == sizeof(int))
			*((int *)     plhs) =  *((int *)     prhs);
		else	memcpy(plhs, prhs, fs->size);
	}
	fluidLevel = toCell;
	scopeLevel = (toCell > 0) ? fluidStack[toCell-1].scopeLevel : 0;
}
