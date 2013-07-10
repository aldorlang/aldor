/*****************************************************************************
 *
 * scan.h: The lexical analyzer.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _SCAN_H_
#define _SCAN_H_

# include "axlobs.h"

extern TokenList scan     	  (SrcLineList);
extern Bool	 scanIsContinued  (String line);

#endif /* !_SCAN_H_ */
