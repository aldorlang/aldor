/*****************************************************************************
 *
 * foamopt.c: optimisations for various platforms (for foam)
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "cport.h" /* for ThatsAll */
#include "foam_c.h"

#ifdef OPT_Linux_i386

fiWordDivideDoubleMacro
fiWordTimesDoubleMacro

#endif

#ifdef OPT_Sparc_v8

fiWordDivideDoubleMacro
fiWordTimesDoubleMacro

#endif

ThatsAll
