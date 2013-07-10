/*****************************************************************************
 *
 * path.h: File system search paths.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _PATH_H_
#define _PATH_H_

# include "axlport.h"
# include "list.h"

DECLARE_LIST(String);

typedef StringList	PathList;

extern void		pathInit		(void);

extern PathList		pathListFrString	(String);
extern PathList		pathListFrArray		(String *);
extern PathList		pathConsDirectory	(String, PathList);
		/*
		 * Form search paths.
		 */

extern PathList		incSearchPath		(void);
extern PathList		libSearchPath		(void);
extern PathList		binSearchPath		(void);
		/*
		 * These list directories to search, including the defaults.
		 */

extern PathList		incSearchPathDelta	(void);
extern PathList		libSearchPathDelta	(void);
extern PathList		binSearchPathDelta	(void);
		/*
		 * These contain what has been added and exclude the defaults.
		 */

extern void		fileAddIncludeDirectory	(String);
extern void		fileAddLibraryDirectory	(String);
extern void		fileAddExecuteDirectory	(String);
		/*
		 * Push directories onto the appropriate lists.
		 */

extern FileName		fileRdFind		(PathList, String, String);
extern String		fileSubdir		(String relativeTo,String sd);

#endif /* !_PATH_H_ */
