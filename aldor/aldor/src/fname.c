/*****************************************************************************
 *
 * fname.c: File name type 
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlgen.h"
#include "fname.h"
#include "format.h"
#include "opsys.h"
#include "store.h"
#include "strops.h"

local FileName
fnameTemplate(void)
{
	FileName fn;
	int	 i, sz;

	sz = osFnameNParts * sizeof(String);
	fn = (FileName) stoAlloc((unsigned) OB_Other, sz);
	for (i = 0; i < osFnameNParts; i++) fn->partv[i] = 0;

	return fn;
}

FileName
fnameNew(String dir, String name, String type)
{
	FileName nfn = fnameTemplate();
	fnameTSetDir (nfn, strCopyIf(dir));
	fnameTSetName(nfn, strCopyIf(name));
	fnameTSetType(nfn, strCopyIf(type));
	return nfn;
}

FileName
fnameStdin(void)
{
	return fnameNew(NULL, "-", NULL);
}

FileName
fnameStdout(void)
{
	return fnameNew(NULL, "-", NULL);
}

Bool
fnameIsStdin(FileName fn)
{
	return strEqual(fnameName(fn), "-");
}

Bool
fnameIsStdout(FileName fn)
{
	return strEqual(fnameName(fn), "-");
}

FileName
fnameCopy(FileName fn)
{
	FileName nfn = fnameTemplate();
	int	 i;
	for (i = 0; i < osFnameNParts; i++)
		nfn->partv[i] = strCopyIf(fn->partv[i]);
	return nfn;
}

FileName
fnameWithType(FileName fn, String type)
{
	FileName nfn = fnameTemplate();
	int	 i;

	for (i = 0; i < osFnameNParts; i++)
		nfn->partv[i] = strCopyIf(i == FNAME_TYPE
					  ? type : fn->partv[i]);
	return nfn;
}

Bool
fnameEqual(FileName f, FileName g)
{
	int	i;

	for (i = 0; i < osFnameNParts; i++) {
		String fi = f->partv[i] ? f->partv[i] : "";
		String gi = g->partv[i] ? g->partv[i] : "";

		if (i == FNAME_DIR) {
			if (!osFnameDirEqual(fi, gi)) return false;
		}
		else {
			if (!strEqual(fi, gi)) return false;
		}
	}
	return true;
}

Bool
fnameHasDir(FileName fn)
{
	return fnameDir(fn)  && *fnameDir(fn);
}

Bool
fnameHasType(FileName fn)
{
	return fnameType(fn) && *fnameType(fn);
}

void
fnameSetDir(FileName fn, String dir)
{
	if (fnameDir(fn)) strFree(fnameDir(fn));
	fnameDir(fn) = strCopy(dir);
}

void
fnameFree(FileName fn)
{
	int	i;

	for (i = 0; i < osFnameNParts; i++)
		if (fn->partv[i]) strFree(fn->partv[i]);

	stoFree((Pointer) fn);
}


/*
 * Parse a string into a static file name according to OS-specific rules.
 */
FileName
fnameParse(String file)
{
	return fnameCopy(fnameParseStatic(file));
}

FileName
fnameParseStatic(String file)
{
	return fnameParseStaticWithin(file, NULL);
}

FileName
fnameParseStaticWithin(String file, String indir)
{       
	static FileName fn  = 0;
	static Buffer buf = 0;

	if (!buf) buf = bufNew();
	if (!fn)  fn  = fnameTemplate();

	bufNeed(buf, osFnameParseSize(file, indir));

	osFnameParse(fn->partv, bufChars(buf), file, indir);

	return fn;
}

/*
 * Format a file name according to local OS rules.
 */
String
fnameUnparse(FileName fn)
{
	return strCopy(fnameUnparseStatic(fn));
}

String
fnameUnparseStatic(FileName fn)
{
	static Buffer buf = 0;

	if (!buf) buf = bufNew();

	bufNeed(buf, osFnameUnparseSize(fn->partv, false));

	return osFnameUnparse(bufChars(buf), fn->partv, false);
}

String
fnameUnparseStaticWith(FileName fn)
{
	static Buffer buf = 0;

	if (!buf) buf = bufNew();

	bufNeed(buf, osFnameUnparseSize(fn->partv, true));

	return osFnameUnparse(bufChars(buf), fn->partv, true);
}

String
fnameUnparseStaticWithout(FileName fn)
{
	String	odir, image;

	odir    = fnameDir(fn);
	fnameTSetDir(fn, "");

	image   = fnameUnparseStatic(fn);

	fnameTSetDir(fn, odir);
	return image;
}

/*
 * Find a new temporary file name.
 */
local char	mod36Digits[] =  "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

local void
intToMod36(int n, String buf, Length len)
{
	int i;

	for (i = len-1; i >= 0 && n >= 0; i--, n /= 36)
		buf[i] = mod36Digits[n % 36];
	for (         ; i >= 0; i--)
		buf[i] = mod36Digits[0];
}

#define TN_Prefix	"tt"	/* default prefix for temp file names */

#define TN_PrefixLen	2	/* max number of chars copied from file name */
#define TN_SeedLen	4	/* number of chars for random seed */
#define TN_CountLen	2	/* number of chars for counter */
#define TN_Len		(TN_PrefixLen + TN_SeedLen + TN_CountLen)  /* <= 8 */

local FileName
fnameTempVectorStatic(String dir, String name, String * type)
{
	static FileName	nfn   = 0;
	static int		count = -1;	/* Last count number used. */
	int			maxcount, i;
	static char		buf[TN_Len+1];

	if (!nfn) nfn = fnameTemplate();

	fnameTSetDir (nfn, osFnameTempDir(dir));
	fnameTSetName(nfn, buf);

	/* Copy the first TN_PrefixLen chars from fn's name if available.  */
	if (name && *name) {
		buf[0] = name[0];
		buf[1] = name[1] ? name[1] : '0';
	}
	else
		strcpy(buf, TN_Prefix);

	/* Append the process seed.  */
	intToMod36(osFnameTempSeed(), &buf[TN_PrefixLen], TN_SeedLen);
	buf[TN_Len] = '\0';

	/* Calculate the maximum count.  */
	for (maxcount = 1, i = 0; i < TN_CountLen; i++) maxcount *= 36;

	/* Append a counter and test whether the files exist.  */
	while (++count < maxcount) {
		intToMod36(count, &buf[TN_PrefixLen+TN_SeedLen], TN_CountLen);
		for (i = 0; type[i]; i++) {
			fnameTSetType(nfn, type[i]);
			if (fileIsThere(nfn)) break;
		}
		if (!type[i]) break;
	}

	if (count >= maxcount)
		return 0;	/* Couldn't find unused temp file names. */

	return nfn;
}

FileName
fnameTemp(String dir, String name, String type)
{
	String	 ftv[2];
	FileName nfn;

	ftv[0] = type;
	ftv[1] = 0;
	nfn    = fnameTempVectorStatic(dir, name, ftv);
	return (nfn == 0) ? 0 : fnameCopy(nfn);
}

FileName *
fnameTempVector(String dir, String name, String * ftv)
{
	FileName	nfn =	fnameTempVectorStatic(dir, name, ftv);
	FileName *	buf;
	int	 	i, n;

	if (nfn == 0) return 0;

	for (n = 0; ftv[n]; n++);
	buf = (FileName *) stoAlloc((unsigned) OB_Other,
				    sizeof(FileName) * (n+1));
	for (i = 0; i < n; i++) {
		fnameType(nfn) = ftv[i];
		buf[i] = fnameCopy(nfn);
	}
	buf[n] = 0;

	return buf;
}
