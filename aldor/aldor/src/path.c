/*****************************************************************************
 *
 * path.c: File system search paths.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlgen.h"
#include "fname.h"
#include "file.h"
#include "list.h"
#include "path.h"

CREATE_LIST(String);

PathList	incDirectoryList = 0;	/* include directory search path */
PathList	libDirectoryList = 0;	/* library directory search path */
PathList	binDirectoryList = 0;	/* execute directory search path */

PathList	incDirectoryDelta = 0;	/* as above, but excluding defaults */	
PathList	libDirectoryDelta = 0;	
PathList	binDirectoryDelta = 0;

/*****************************************************************************
 *
 * Search paths
 *
 ****************************************************************************/


void
pathInit(void)
{
	incDirectoryList = pathListFrString(osIncludePath());
	libDirectoryList = pathListFrString(osLibraryPath());
	binDirectoryList = pathListFrString(osExecutePath());

	incDirectoryDelta = 0;
	libDirectoryDelta = 0;
	binDirectoryDelta = 0;
}

PathList
pathListFrString(String path)
{
	Length		n = osPathLength(path);
	String		sbuf = strAlloc(strLength(path));
	String *	pathv = (String *) stoAlloc((int) OB_Other,
						    n * sizeof(String));
	StringList	result = listNil(String);

	osPathParse(pathv, sbuf, path);
	while (n > 0)
		result = listCons(String)(pathv[--n], result);
	stoFree((Pointer) pathv);

/*	assert( result != listNil(String) ); */
	return (PathList) result;
}

PathList
pathListFrArray(String * pathv)
{
	Length		n = 0;
	StringList	result = listNil(String);

	while (pathv[n] != 0)
		result = listCons(String)(pathv[n++], result);
	result = listNReverse(String)(result);

	assert( result != listNil(String) );
	return (PathList) result;
}

/* Add the directory to the front of the list dl.
 * Return the new list.
 */
PathList
pathConsDirectory(String directory, PathList pl)
{
	return (PathList) listCons(String)(directory, (StringList) pl);
}

PathList incSearchPath(void)      { return incDirectoryList; }
PathList libSearchPath(void)      { return libDirectoryList; }
PathList binSearchPath(void)      { return binDirectoryList; }

PathList incSearchPathDelta(void) { return incDirectoryDelta; }
PathList libSearchPathDelta(void) { return libDirectoryDelta; }
PathList binSearchPathDelta(void) { return binDirectoryDelta; }

void
fileAddIncludeDirectory(String directory)
{
	incDirectoryList  = pathConsDirectory(directory, incDirectoryList);
	incDirectoryDelta = pathConsDirectory(directory, incDirectoryDelta);
}

void
fileAddLibraryDirectory(String directory)
{
	libDirectoryList  = pathConsDirectory(directory, libDirectoryList);
	libDirectoryDelta = pathConsDirectory(directory, libDirectoryDelta);
}

void
fileAddExecuteDirectory(String directory)
{
	binDirectoryList  = pathConsDirectory(directory, binDirectoryList);
	binDirectoryDelta = pathConsDirectory(directory, binDirectoryDelta);
}

FileName
fileRdFind(PathList path, String fn, String ft)
{
	StringList	dl = (StringList) path;
	FileName	filename;

	for( ; dl != listNil(String); dl = cdr(dl) ) {
		filename = fnameParseStaticWithin(fn, car(dl));
		if (fileIsReadable(filename))
			return fnameCopy(filename);
		if (*fnameType(filename) == '\0') {
			fnameTSetType(filename, ft);
			if (fileIsReadable(filename))
				return fnameCopy(filename);
		}
	}
	return 0;
}

String
fileSubdir(String relativeTo, String sd)
{
	Length l = osSubdirLength(relativeTo, sd);
	String b = strAlloc(l);

	osSubdir(b, relativeTo, sd);

	return b;
}
