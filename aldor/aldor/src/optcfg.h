/*****************************************************************************
 *
 * optcfg.h: Determine reqd optimisations from standard defines
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _OPTCFG_H_
#define _OPTCFG_H_
/*
 * This file is included both by the runtime library and by the 
 * compiler, so it can't use internal names.
 *
 */

/*
 * All a bit trivial at the moment
 */

#if defined(__linux__) && defined(__i386__)
#define OPT_Linux_i386
#define OPT_NoDoubleOps
#define OPT_FastDoubleArith 1
#endif

#if defined(__CYGWIN__) && defined(__i386__)
#define OPT_Linux_i386
#define OPT_NoDoubleOps
#define OPT_FastDoubleArith 1
#endif

#if defined(__GNUC__) && defined(__sparc__) && defined(__sparc_v8__)
#define OPT_Sparc_v8
#define OPT_NoDoubleOps
#define OPT_FastDoubleArith 1
#endif

#endif

