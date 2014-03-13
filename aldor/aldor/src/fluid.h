/*****************************************************************************
 *
 * fluid.h: Dynamically scoped variables for C.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 * Declare a fluidly bound variable by wrapping the variable name with "fluid".
 * Use Scope at begining of a function binding fluids.
 * Exit via explicit Return or LongJmp.
 * To return no value, do Return(Nothing).
 * For variables local to a block, use Scope/Unscope.
 *
 * The expression for the return value or long jump value may not contain
 * any fluids bound in the current scope.
 * 
 * Example:
 *
 *      double  d = 1.0;
 *      int     i = 3;
 *  	JmpBuf  jb;
 *
 *      g() {
 *		if (SetJmp(jb)) j(); 	-- d and i are restored by LongJmp.
 *		k();		     	-- d and i are restored by Return.
 *	}
 *
 *      j() {
 *		Scope("j");
 *		double	fluid(d), e;
 *		int	h, fluid(i);
 *		d *= 2;			-- Set inner d to be twice the outer.
 *		LongJmp(jb);
 *	}
 *
 *      int
 *	k() {
 *              Scope("k");
 *              double  fluid(d);
 *              int	fluid(i);
 *              d *= 2;
 *		Return(7);
 *      }
 */

#ifndef _FLUID_H_
#define _FLUID_H_

#include "cport.h"

struct fluidCell {
	String		scopeName;	/* Name of the scope. */
	int		scopeLevel;	/* Level of the scope. */
	Pointer		pglobal,	/* Location of the global variable. */
			pstack;		/* Location of stack save area. */
	long		size;		/* Size of stack save area. */
};

typedef struct {
	int		fluidLevel;
	jmp_buf         buf;
} JmpBuf;

/*
 * Growable stack of fluid cells.
 */
extern struct fluidCell	*fluidStack;

extern int		 fluidLevel,	 /* number of fluids (= next slot) */
			 fluidLimit,	 /* index past end   */
			 fluidIncrement; /* amount to grow stack by */

extern int		 scopeLevel;	 /* number of scopes */

/*
 * Run time support to grow and unwind fluid stack.
 */
extern struct fluidCell	*fluidGrow	(void);
extern void		 fluidUnwind	(int toCell, Bool isLongJmp);

/*
 * Scope entry and exit.
 */
#define Scope(name)	String scopeName   = (name);       \
			int    fluidLevel0 = (scopeLevel++, fluidLevel)
#define Unscope()	fluidUnwind(fluidLevel0, false)
#define SetJmp(jb)	((jb).fluidLevel = fluidLevel, setjmp((jb).buf))
#define LongJmp(jb,val) Statement(fluidUnwind((jb).fluidLevel, true); \
				  longjmp((jb).buf, val);)
#define Return(val)	Statement(fluidUnwind(fluidLevel0, false); \
				  return val;)

#define ReturnNothing	Statement(fluidUnwind(fluidLevel0, false); \
				  return;)


/*
 * Introduction of individual fluids.
 */
#define fluidSave(var)  fluidSave_##var

#define fluid(V)	fluidSave(V) = ( \
	fluidStack = (fluidLevel==fluidLimit) ? fluidGrow() : fluidStack,    \
	fluidStack[fluidLevel].scopeName  = scopeName,    /*!! debug info */ \
	fluidStack[fluidLevel].scopeLevel = scopeLevel,   /*!! debug info */ \
	fluidStack[fluidLevel].pglobal    = (Pointer) &(V),          \
	fluidStack[fluidLevel].pstack     = (Pointer) &fluidSave(V), \
	fluidStack[fluidLevel].size       = sizeof(V),     \
	fluidLevel++, \
	(V) \
)

#endif /* !_FLUID_H_ */
