/*****************************************************************************
 *
 * memclim.h: Memory Climates.
 *
 * Copyright Aldor.org 1990-2007.
 *
 ****************************************************************************/

#ifndef _MEMCLIM_H_
#define _MEMCLIM_H_

#include "stdio.h0"

typedef int		MemoryClimate;

extern void             limitNumberOfMemoryClimates(int n);

extern void		registerMemoryClimate	(MemoryClimate, char *mcName);
extern int		numberOfMemoryClimates	(void);

extern MemoryClimate	setMemoryClimate	(MemoryClimate);

extern MemoryClimate	getMemoryClimate	();
extern char *		getMemoryClimateString	(MemoryClimate);
extern int		getMemoryClimateIndex	(MemoryClimate);
extern MemoryClimate	getMemoryClimateOfIndex (int index);

extern void initMemoryClimateHistogram         ();
extern void incrMemoryClimateHistogram         (MemoryClimate clim, int, int);
extern void finiMemoryClimateHistogram         ();
extern void showMemoryClimateHistogram         (FILE *outf);

#endif /* !_MEMCLIM_H_ */
