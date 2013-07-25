/*****************************************************************************
 *
 * include.h: The source file includer.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _INCLUDE_H_
#define _INCLUDE_H_

#include "axlobs.h"

typedef Bool		(*InclIsContinuedFun)(String line);
			/*
			 * Called initially with *pflags = 0.
			 */

extern void             inclGlobalAssert  (String property);
extern void             inclGlobalUnassert(String property);

extern SrcLineList	include		  (FileName, FILE *fin, int *pline,
					   InclIsContinuedFun);
			/*
			 * Calls either include "includeFile" or "includeLine".
			 * If fin is non-null include one line.
			 * Otherwise include the whole file.
			 */

extern SrcLineList      includeFile       (FileName);
extern SrcLineList	includeLine	  (FileName, FILE *, int *,
					   InclIsContinuedFun);

extern long             inclTotalLineCount(void);
extern long		inclFileLineCount (void);
			/*
			 * These tell what happened during the last call to
			 * include, includeFile or includeLine.
			 */

extern int              inclWrite         (FILE *, SrcLineList);
extern void             inclFree          (SrcLineList);

#endif /* !_INCLUDE_H_ */
