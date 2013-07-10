/*****************************************************************************
 *
 * gf_gener.h: Foam code generation for generators.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _GF_GENER_H_
#define _GF_GENER_H_

# include "axlobs.h"


extern Foam      genGenerate            (AbSyn);
extern Foam      genYield            	(AbSyn);
extern Foam	 gen0RetFormatDDecl	(void);
extern AInt	 gen0MakeGenerRetFormat (void);
extern Foam	 gen0GenLiftedGener	(AbSyn, AbSyn);

#endif /* !_GF_GENER_H_ */
