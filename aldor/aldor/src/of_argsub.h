/*****************************************************************************
 *
 * of_argsub.h: Experimenting with function-parameter substitution
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _OF_ARGSUB
#define _OF_ARGSUB

#include "axlobs.h"
#include "genfoam.h"
#include "gf_util.h"
#include "gf_prog.h"
#include "gf_add.h"
#include "gf_gener.h"
#include "gf_reference.h"
#include "gf_excpt.h"
#include "gf_imps.h"
#include "gf_rtime.h"
#include "opttools.h"
#include "of_inlin.h"
#include "of_util.h"
#include "optfoam.h"
#include "simpl.h"
#include "scobind.h"
#include "fortran.h"
#include "compcfg.h"
#include "tform.h"


extern void	argsubUnit(Foam);


typedef enum
{
   Garbage,			/* Hasn't been used at all yet */
   Uninitialised = 0xDead,	/* Used once and reset */
#if EDIT_1_0_n1_07
   Initialised			/* Currently in use */
#else
   Initialised,			/* Currently in use */
#endif
} SStatus;


/*
 * The AgsUnitInfo structure contains all the information we need
 * to know about the unit being analysed. We keep track of its
 * formats, globals an constants as well as a pointer to the unit
 * itself. Note that the constants are actually pointers into the
 * body of the constant in the FOAM unit not the signatures. This
 * gives us something that we can copy and modify.
 */
typedef struct agsUnitInfoStruct
{
	Length	fmtc;		/* Number of formats */
	Foam	*fmtv;		/* Array of formats (unboxed) */

	Length	gloc;		/* Number of globals */
	Foam	*glov;		/* Array of globals */

	Length	constc;		/* Number of constants */
	Foam	*constv;	/* Array of pointers to global data */

	Foam	unit;		/* The Unit we got all this from */
} *AgsUnitInfo;


/*
 * AgsProgInfo serves exactly the same purpose as AgsUnitInfo
 * but for Progs instead of Units. We need to note the locals,
 * lexicals and parameters of the Prog so that we can do any
 * substitutions and renamings.
 */
typedef struct agsProgInfoStruct
{
	Length	parc;		/* Number of parameters */
	Foam	*parv;		/* Array of parameters */

	Length	locc;		/* Number of locals */
	Foam	*locv;		/* Array of locals */

	Length	lexc;		/* Number of lexicals */
	AInt	*lexv;		/* Array of lexicals (possibly NULL) */

	Foam	prog;		/* The Prog we got this from */
	Length	cnum;		/* The const number of this prog */
	Foam	levels;		/* Environment levels */

	SStatus	status;		/* Detect recursion bugs */
}  *AgsProgInfo;

#endif /* !_OF_ARGSUB */
