/*****************************************************************************
 *
 * compopt.c: optimisations for various platforms 
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "compopt.h"
#include "cport.h" /* for ThatsAll */

#ifdef OPT_Linux_i386

xxTimesDoubleMacro
xxDivideDoubleMacro
xxModDoubleMacro

#endif

#ifdef OPT_Sparc_v8

xxTimesDoubleMacro
xxDivideDoubleMacro
xxModDoubleMacro

#endif

ThatsAll
