/*****************************************************************************
 *
 * of_cprop.h: Copy propagation
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef OF_CPROP_H
#define OF_CPROP_H

# include "axlobs.h"

/****************************************************************************
 *
 * :: External entry points
 *
 ****************************************************************************/

extern void	cpropUnit	(Foam, Bool);
extern Bool	cpFlog		(FlowGraph);

#endif


