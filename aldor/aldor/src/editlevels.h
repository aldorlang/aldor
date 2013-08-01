
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
#define EDIT_1_0_n1_SSA 1	/* SSA */

#define EDIT_1_0_n1_AD 1	/* (currently unused) */
#define EDIT_1_0_n1_AC 1	/* (currently unused) */
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
#define EDIT_1_0_n2_15 1        /* (next available edit number) */

#define EDIT_1_0_n2_14 1        /* (unused) */

#define EDIT_1_0_n2_13 1        /* (unused) */

#define EDIT_1_0_n2_12 1        /* (unused) */

#define EDIT_1_0_n2_11 1        /* (unused) */

#define EDIT_1_0_n2_10 1        /* (unused) */

#define EDIT_1_0_n2_06 1        /* fix bug 4 - disabled type cache in stab */

	/* Edit 1.0.-1(9): HP-UX port; added missing "local" etc */
#define EDIT_1_0_n1_07 1	/* Clean up and Alpha port; *sizeof(long)  */
#define EDIT_1_0_n1_06 1	/* runtime hash code collision detection */
#define EDIT_1_0_n1_05 1	/* ti{bup,tdn}Collect multi/cross embed fix */
#define EDIT_1_0_n1_04 1	/* coloured compiler messages */
#define EDIT_1_0_n1_03 1	/* comsgFatal replaces 1.1.12p6(1) assert */
	/* Edit 1.0.-1(2): replaced AXL_ comsg tags with ALDOR_ everywhere */
	/* Edit 1.0.-1(1): updates to comsgdb.msg, added -Fap and .ap */
	/* Aldor 1.0.-1(0) is a freeze of AxiomXL 1.1.13(37) */

/* ===================================================================== */
/* =========================== AxiomXL Edits =========================== */
/* ===================================================================== */

#define AXL_EDIT_1_1_13_37 1	/* fixed {aldor,axl}main.c clobbering */
	/* Edit 1.1.13(36): renaming of $AXIOMXL* to $ALDOR* */
	/* Edit 1.1.13(35): renaming of axiomxl to aldor */
	/* Edit 1.1.13(34): currently disabled */
#define AXL_EDIT_1_1_13_33 1	/* gen0MultiAssign now handles exports */
#define AXL_EDIT_1_1_13_32 1	/* added tfCheckBug1318() hack */
	/* Edit 1.1.13(31): added fiArrNew_* to foam_c.c */
#define AXL_EDIT_1_1_13_30 1	/* bug 1316 fix (64 bit literals) */
	/* Edit 1.1.13(29): uniar2 fix for UCB archives with stabs */
	/* Edit 1.1.13(28): added fiArrNew_Ptr to foam_c.c */
#define AXL_EDIT_1_1_13_27 1	/* fix for -Cold multi-value prototypes */
	/* Edit 1.1.13(26): -M[no-]release */
	/* Edit 1.1.13(25): GCC 2.96 fix in platform.h/axiomxl.conf */
#define AXL_EDIT_1_1_13_24 1	/* fixes in archive.c */
	/* Edit 1.1.13(23): AxlLib/format.as StoIsWritable fix */
	/* Edit 1.1.13(22): start-up banner for interpreter */
	/* Edit 1.1.13(21): fixed gen0RtRand */
	/* Edit 1.1.13(20): currently disabled */
#define AXL_EDIT_1_1_13_19 1	/* rewrote scobindMatchParam for bug 1275 */
	/* Edit 1.1.13(18): currently disabled */
#define AXL_EDIT_1_1_13_17 1	/* gencpp: uses compRootDir from axlcomp.c */
	/* Edit 1.1.13(16): $ALDORARGS overrides $AXIOMXLARGS */
	/* Edit 1.1.13(15): $ALDORROOT overrides $AXIOMXLROOT */
	/* Edit 1.1.13(14): aldor.conf overrides axiomxl.conf */
	/* Edit 1.1.13(13): use "throw" instead of "except" in errors */
	/* Edit 1.1.13(12): -M base=<dir> support */
#define AXL_EDIT_1_1_13_11 1	/* abPPClipped0 adds () in more places */
#define AXL_EDIT_1_1_13_10 1	/* tfSatExcept semantics bug fix */
#define AXL_EDIT_1_1_13_09 1	/* tfMapRetc ignores exceptions */
#define AXL_EDIT_1_1_13_08 1	/* fix for garbage collector bug under Win9x */

#define AXL_EDIT_1_1_13_07 1	/* titdn missing tpossRefer */
#define AXL_EDIT_1_1_13_06 1	/* except/throw not a follower, maybe infix */
#define AXL_EDIT_1_1_13_05 1	/* tfHasDomExportMod tweak */
#define AXL_EDIT_1_1_13_04 1	/* abDefineeIdOrElse watches for comma bug */
#define AXL_EDIT_1_1_13_03 1	/* Introduced gc0TryCast() into genc */
#define AXL_EDIT_1_1_13_02 1	/* Replaced an assert() by comsgFatal() */
#define AXL_EDIT_1_1_13_01 1	/* Fix for multi-def-export bug 1261 */
#define AXL_EDIT_1_1_13_00 1	/* More carefully generated C code */

#define AXL_EDIT_1_1_12p6_25 1	/* Replacement for 1.1.12p6(15) extends */
#define AXL_EDIT_1_1_12p6_24 1	/* Replacement for 1.1.12p6(15) lambdas */
#define AXL_EDIT_1_1_12p6_23 1	/* Array temporaries use pointer locals */
#define AXL_EDIT_1_1_12p6_22 1	/* ANSI declarations on Foreign C imports */
#define AXL_EDIT_1_1_12p6_21 1	/* titdn{Lambda,Generate,Reference} bugs */
#define AXL_EDIT_1_1_12p6_20 1	/* bputBadArgType0 fix for bug 1210 */
#define AXL_EDIT_1_1_12p6_19 1	/* FiByte now unsigned for bug 1237 */
#define AXL_EDIT_1_1_12p6_18 1	/* Added missing goto's for bug 1235 */
#define AXL_EDIT_1_1_12p6_17 1	/* Passing symes in gen0Lambda recursion */
	/* Edit 1.1.12p6(16): means no GC debug with -V; -ffold at -Q2 */
	/* Edit 1.1.12p6(15): replaced by edits 24 and 25 */
	/* Edit 1.1.12p6(14): replaced by edits 15, 24 and 25 */
	/* Edit 1.1.12p6(13): terrorImplicitSetBang() checks tr!=NULL */
	/* Edit 1.1.12p6(12): -Wgc is default and added -Wno-gc */
	/* Edit 1.1.12p6(11): deada enables cse/jflow/cprop */
	/* Edit 1.1.12p6(10): AIX, HP/UX and -cold changes */
	/* Edit 1.1.12p6( 9): change to hash$Integer$libaxllib */
#define AXL_EDIT_1_1_12p6_08 1	/* abCheckWithin now hates defs in withs */


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

/*

Edit level AXL_EDIT_1_1_13_18 has been returned to the list of active edits.
It still causes problems for Basicmath DUP, but it solves the issue where maps
and packed maps were being given the same hash value when the only difference
in the maps was the position of the map symbol.  For example (I, I) -> (I, I)
was getting the same hash code as (I) -> (I, I, I) and (I, I, I) -> (I).  

The problem with Basicmath DUP can be seen when * is applied to acc and result
in CantorZassenhausFactorizationPackage (located in
$BMROOT/src/polypkg/factorczpkg.as).  When acc is ? and result is 1, (both acc
and result being of type U), the multiply invoked is (r: R) * (x : %) : %
rather than (x : %) * (y : %) : %.  This results in a large integer times ?
being returned, rather than 1*?.

Applying this edit appears to be revealing another bug in the compiler that 
has yet to be isolated to a simple test case.  (The Basicmath DUP behaviour
is illustrated by Basicmath test duptest19.as).  It does not appear to be a 
result of a hash code collision under the new system as several alternative
code computations were tried providing the same result.

*/

#define AXL_EDIT_1_1_13_18 1	/* changed type hash code algorithm */
				/* Breaks Basicmath DUP etc */

#if 0

#define AXL_EDIT_1_1_13_34 1	/* changing "with" insertion to warning */
				/* Breaks too many things */
#define AXL_EDIT_1_1_12p6_15 1	/* Set hash code on correct closure value */
				/* Replaced by 1.1.12p6_24 and 1.1.12p6_25 */
#define AXL_EDIT_1_1_12p6_14 1	/* Deleted code in gen0RtSefoHashStdApply() */
				/* Edits 15, 24 and 25 put it back ... */
#define AXL_EDIT_1_1_12p6_01 1	/* assert(tfBoolean != tfUnknown) */
				/* Replaced by Aldor 1.0.-1(3) */
#endif

#endif /* !_EDITLEVELS_H_ */
