
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
#define EDIT_1_0_n1_AB 1	/* (currently unused) */
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

	/* Edit 1.0.-1(9): HP-UX port; added missing "local" etc */
	/* Edit 1.0.-1(2): replaced AXL_ comsg tags with ALDOR_ everywhere */
	/* Edit 1.0.-1(1): updates to comsgdb.msg, added -Fap and .ap */
	/* Aldor 1.0.-1(0) is a freeze of AxiomXL 1.1.13(37) */

/* ===================================================================== */
/* =========================== AxiomXL Edits =========================== */
/* ===================================================================== */

	/* Edit 1.1.13(36): renaming of $AXIOMXL* to $ALDOR* */
	/* Edit 1.1.13(35): renaming of axiomxl to aldor */
	/* Edit 1.1.13(34): currently disabled */
	/* Edit 1.1.13(31): added fiArrNew_* to foam_c.c */
	/* Edit 1.1.13(29): uniar2 fix for UCB archives with stabs */
	/* Edit 1.1.13(28): added fiArrNew_Ptr to foam_c.c */
	/* Edit 1.1.13(26): -M[no-]release */
	/* Edit 1.1.13(25): GCC 2.96 fix in platform.h/axiomxl.conf */
	/* Edit 1.1.13(23): AxlLib/format.as StoIsWritable fix */
	/* Edit 1.1.13(22): start-up banner for interpreter */
	/* Edit 1.1.13(21): fixed gen0RtRand */
	/* Edit 1.1.13(20): currently disabled */
	/* Edit 1.1.13(18): currently disabled */
	/* Edit 1.1.13(16): $ALDORARGS overrides $AXIOMXLARGS */
	/* Edit 1.1.13(15): $ALDORROOT overrides $AXIOMXLROOT */
	/* Edit 1.1.13(14): aldor.conf overrides axiomxl.conf */
	/* Edit 1.1.13(13): use "throw" instead of "except" in errors */
	/* Edit 1.1.13(12): -M base=<dir> support */

	/* Edit 1.1.12p6(16): means no GC debug with -V; -ffold at -Q2 */
	/* Edit 1.1.12p6(15): replaced by edits 24 and 25 */
	/* Edit 1.1.12p6(14): replaced by edits 15, 24 and 25 */
	/* Edit 1.1.12p6(13): terrorImplicitSetBang() checks tr!=NULL */
	/* Edit 1.1.12p6(12): -Wgc is default and added -Wno-gc */
	/* Edit 1.1.12p6(11): deada enables cse/jflow/cprop */
	/* Edit 1.1.12p6(10): AIX, HP/UX and -cold changes */
	/* Edit 1.1.12p6( 9): change to hash$Integer$libaxllib */


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
	/* Edit 1.1.12p6(3): inliner/inherit bug fix  */
	/* Edit 1.1.12p6(2): change in value of InlProgCutOff */


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
