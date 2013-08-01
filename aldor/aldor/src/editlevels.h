
/*****************************************************************************
 *
 * editlevels.h: Conditionalised edits
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _EDITLEVELS_H_
#define _EDITLEVELS_H_

/*
 * Work-in-progress: these are edits that have not been completed and thus
 * don't have an edit number yet. Use these for long term projects etc.
 */
#define EDIT_1_0_n1_AB 1	/* SSA */
#define EDIT_1_0_n1_AA 0	/* comex CEnvs */

/*
 * Temporary definitions to enable recent edits to be turned on
 * or off with relative ease. Once they have been stable for a
 * while they can be made unconditional and replaced here with
 * a suitable comment. The n1 in the Aldor edits means -1.
 */

/* ===================================================================== */
/* ============================ Aldor Edits ============================ */
/* ===================================================================== */
#define EDIT_1_0_n2_06 1        /* fix bug 4 - disabled type cache in stab */

/*
 * These next three (probably just edit 6) break AXIOM 2.3. Unfortunately
 * without them we get a huge drop in performance (especially for arrays).
 * 
 * Note also that edit 6 breaks libalgebra when it and libaldor are built
 * using -Q5 optimisation for release (Unhandled Merge). This problem was
 * fixed by edit 1.0.-1(11).
 */
#define AXL_EDIT_1_1_12p6_07 1	/* tibupComma more cautious creating defs */

#define AXL_EDIT_1_1_12p6_04 1	/* inlining limit now user-defined */


/************************************************************************
 * Dead edits: DON'T use unless you have a really good reason ...
 ************************************************************************/

#if 0

#define AXL_EDIT_1_1_13_34 1	/* changing "with" insertion to warning */
				/* Breaks too many things */
#define AXL_EDIT_1_1_12p6_14 1	/* Deleted code in gen0RtSefoHashStdApply() */
				/* Edits 15, 24 and 25 put it back ... */
#endif

#endif /* !_EDITLEVELS_H_ */
