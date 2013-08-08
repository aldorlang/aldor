/*****************************************************************************
 *
 * archive.c: Operations for manipulating compiler library archives.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "archive.h"
#include "axlobs.h"
#include "comsg.h"
#include "debug.h"
#include "file.h"
#include "lib.h"
#include "opsys.h"
#include "store.h"
#include "strops.h"
#include "syme.h"
#include "table.h"
#include "util.h"

Bool	arDebug		= false;
#define arDEBUG		DEBUG_IF(arDebug)

#define AR_SEEK(ar,pos)	fseek((ar)->file, pos, SEEK_SET)

/*****************************************************************************
 *
 * :: Local function declarations
 *
 ****************************************************************************/

CREATE_LIST(ArEntry);

local Bool		arItemIsIntermed	(String);

local ArEntry		arAllocEntry		(String, Archive, Offset);
local void		arFreeEntry		(ArEntry);
local String		arEntryKey		(String);
local ArEntry		arFindEntry		(Archive, String);
local ArEntry		arFindLibEntry		(Archive, Lib);
local Lib		arExtractEntry		(Archive, ArEntry);
#define			arEntryLib(ar,e)	\
	((e)->lib ? (e)->lib : arExtractEntry(ar,e))

local void		arRdFormat		(Archive);
local void		arRdTable		(Archive);

local String		arFirst			(Archive);
local String		arNext			(Archive);
local Bool		arEndp			(Archive);
local Bool		arSeek			(Archive, Offset);

local Offset		arFirstPos		(Archive);
local Offset		arFirstPosAIX		(Archive);
local Offset		arFirstPosAIX4		(Archive);
local Offset		arFirstPosArch		(Archive);
local String		arRdItem		(Archive);
local String		arRdItemAIX		(Archive);
local String		arRdItemAIX4		(Archive);
local String		arRdItemArch		(Archive);
local String		arRdItemCMS		(Archive);

local Archive		arNew			(FileName, FILE *);

local void		arFilter		(Archive);
local void		arFilterScanMember	(Archive, ArEntry);
local void		arFilterMarkMember	(Archive, ArEntry);
local void		arFilterMarkExpanded	(Archive, ArEntry);

local void		arReadNameTable		(Archive);
local String 		arRdItemArch0		(Archive);

local int		arReadString		(Archive, String, int);
local void		arReadText		(Archive, String, int);
local void		arReadNumber		(Archive, Offset *, int, int);
local void		arReadDecimal		(Archive, Offset *, int);
local void		arReadOctal		(Archive, Offset *, int);

static char ar_buffer[1024];


/*****************************************************************************
 *
 * :: Archive file lists
 *
 ****************************************************************************/

String		arCurrentFileName = "";	/* archive member being compiled */
String		arCurrentFileId   = "";	/* ID of member being compiled */
PathList	arLibraryFileList = 0;	/* library archive file names */
PathList	arLibraryKeyList  = 0;	/* library archive file keys */

/*
 * Before processing the command line, reset the archive lists to those
 * archives on the default lists.
 */
void
arInit(String * files, String * keys)
{
	arLibraryFileList = pathListFrArray(files);
	arLibraryKeyList  = pathListFrArray(keys);

	return;
}

/*
 * Before processing each file, reset the archive lists to those archives
 * listed on the command line.
 */
void
arFileInit(FileName fn, String id)
{
	arCurrentFileName = strCopy(fnameUnparseStaticWithout(fn));
	if (strEqual(id, "axiom")) id = "";
	arCurrentFileId   = strCopy(id);
	return;
}

PathList
arLibraryFiles(void)
{
	return arLibraryFileList;
}

PathList
arLibraryKeys(void)
{
	return arLibraryKeyList;
}

void
arAddLibraryFile(String name)
{
	arLibraryFileList = pathConsDirectory(name, arLibraryFileList);
}

void
arAddLibraryKey(String key)
{
	arLibraryKeyList = pathConsDirectory(key, arLibraryKeyList);
}

local Bool
arItemIsIntermed(String item)
{
	return ftypeEqual(fnameType(fnameParseStatic(item)), FTYPE_INTERMED);
}

/*****************************************************************************
 *
 * :: Archive entries
 *
 ****************************************************************************/

local ArEntry
arAllocEntry(String name, Archive ar, Offset pos)
{
	ArEntry		arent;

	arent = (ArEntry) stoAlloc(OB_Other, sizeof(*arent));

	arent->name	= name;
	arent->ar	= ar;
	arent->pos	= pos;
	arent->lib	= NULL;
	arent->mark	= true;

	return arent;
}

local void
arFreeEntry(ArEntry arent)
{
	stoFree((Pointer) arent);
}

local String
arEntryKey(String name)
{
	FileName	item = fnameParseStatic(name);
	return fnameUnparseStaticWithout(item);
}

local ArEntry
arFindEntry(Archive ar, String name)
{
	ArEntryList	alist;

	arDEBUG{fprintf(dbOut, "Looking for \"%s\"", name);}

	name = arEntryKey(name);
	arDEBUG{fprintf(dbOut, " as archive key \"%s\"", name);}

	for (alist = ar->members; alist; alist = cdr(alist)) {
		ArEntry		arent = car(alist);
		if (strAEqual(name, arent->name)) {
			arDEBUG{fprintf(dbOut, " at offset %ld\n",arent->pos);}
			return arent;
		}
	}

	arDEBUG{fprintf(dbOut, " not found.\n");}
	return NULL;
}

local ArEntry
arFindLibEntry(Archive ar, Lib lib)
{
	if (lib->arent == NULL)
		lib->arent = arFindEntry(ar, libToStringStatic(lib));

	if (lib->arent && lib->arent->ar == ar)
		return lib->arent;
	else
		return NULL;
}

local Lib
arExtractEntry(Archive ar, ArEntry arent)
{
	Lib	lib;

	lib = libExtract(fnameParse(arent->name), arFile(ar), arent->pos);
	lib->arent = arent;

	arent->lib = lib;
	return lib;
}

/*****************************************************************************
 *
 * :: Archive file formats
 *
 ****************************************************************************/

#define SARMAGMAX	8

/*
 * These are the "magic number" strings identifying the various sorts
 * of archive file formats.  These need to be given in numeric form,
 * rather than toAscii of a string, since the "\n" is not guaranteed to
 * translate to the same code in a portable way.
 */
static char arstrArch []={33, 60, 97,114, 99,104, 62, 10, 0}; /* !<arch>\n */
static char arstrAIX  []={60, 97,105, 97,102,102, 62, 10, 0}; /* <aiaff>\n */
static char arstrAIX4 []={60, 98,105,103, 97,102, 62, 10, 0}; /* <bigaf>\n */
static char arstrCMSaf[]={60, 67, 77, 83, 97,102, 62, 10, 0}; /* <CMSaf>\n */

struct ar_info arInfoTable[] = {
	{AR_Arch,  arstrArch,		2,	 8},
	{AR_AIX,   arstrAIX,		2,	68},
	{AR_CMS,   arstrCMSaf,		2,	 8},
	{AR_AIX4,  arstrAIX4,		2,     128},
};

/*
 * arRdFormat
 */

local void
arRdFormat(Archive ar)
{
	char		mag[SARMAGMAX];
	ArFmtTag	tag;

	AR_SEEK(ar, int0);
	FILE_GET_CHARS(ar->file, mag, SARMAGMAX);

	for (tag = AR_START; tag < AR_LIMIT; tag += 1)
		if (strIsPrefix(arInfo(tag).str, mag))
			break;

	if (tag == AR_LIMIT) return;

	ar->format = tag;
	ar->size   = fileSize(ar->name);
}

/*
 * arRdTable
 */

local void
arRdTable(Archive ar)
{
	ArEntryList	members = listNil(ArEntry);
	String		name;
	arDEBUG{fprintf(dbOut, "arRdTable:\n");}

	for (name = arFirst(ar); !arEndp(ar); name = arNext(ar)) {
		if (arItemIsIntermed(name)) {
			Offset		pos = arPosition(ar);
			ArEntry		arent = arAllocEntry(name, ar, pos);
			members = listCons(ArEntry)(arent, members);
			ar->hasIntermed = true;
			arDEBUG{fprintf(dbOut, "    %s(%ld)\n", name, pos);}
		}
	}

	ar->members = listNReverse(ArEntry)(members);
}

local String
arFirst(Archive ar)
{
	ar->__next = arFirstPos(ar);
	return arNext(ar);
}

local String
arNext(Archive ar)
{
	if (ar->__next && arSeek(ar, ar->__next))
		return arRdItem(ar);
	else {
		arPosition(ar) = 0;
		arItem(ar) = 0;
		return 0;
	}
}

local Bool
arEndp(Archive ar)
{
	return arPosition(ar) == 0;
}

local Bool
arSeek(Archive ar, Offset pos)
{
	Length	size = arSize(ar);

	if (pos >= size) {
		arPosition(ar) = 0;
		return false;
	}

	AR_SEEK(ar, pos);
	arPosition(ar) = pos;
	return true;
}

/*
 * arFirstPos
 */

local Offset
arFirstPos(Archive ar)
{
	Offset	pos = 0;

	switch(ar->format) {
	case AR_AIX:
		arDEBUG{fprintf(dbOut, "AIX-style archive\n");}
		pos = arFirstPosAIX(ar);
		break;
	case AR_AIX4:
		arDEBUG{fprintf(dbOut, "AIXBIG-style archive\n");}
		pos = arFirstPosAIX4(ar);
		break;
	case AR_Arch:
		arDEBUG{fprintf(dbOut, "ARCH-style archive\n");}
		pos = arFirstPosArch(ar);
		break;
	case AR_CMS:
		arDEBUG{fprintf(dbOut, "CMS-style archive\n");}
		pos = arFirstPosArch(ar);
		break;
	default:
		bugUnimpl(ar->format);
		break;
	}

	return pos;
}

/*
 * Offset 32 of an AIX archive holds a pointer to the first
 * archive member.
 */
#define AIXFSTMOFF	32

local Offset
arFirstPosAIX(Archive ar)
{
	Offset	pos;

	AR_SEEK(ar, AIXFSTMOFF);
	IgnoreResult(fscanf(arFile(ar), "%12lu ", &pos));
	return pos;
}

#define AIX4FSTMOFF	68

local Offset
arFirstPosAIX4(Archive ar)
{
	Offset	pos;

	AR_SEEK(ar, AIX4FSTMOFF);
	IgnoreResult(fscanf(arFile(ar), "%20lu ", &pos));
	return pos;
}

local Offset
arFirstPosArch(Archive ar)
{
	/*
	 * The first member in an `ar' archive appears immediately
	 * after the header record (the archive magic number).
	 */
	return arInfo(ar->format).hdrsz;
}

/*
 * arRdItem
 */

local String
arRdItem(Archive ar)
{
	String s = 0;

	switch(ar->format) {
	case AR_AIX:
		s = arRdItemAIX(ar);
		break;
	case AR_AIX4:
		s = arRdItemAIX4(ar);
		break;
	case AR_Arch:
		s = arRdItemArch(ar);
		break;
	case AR_CMS:
		s = arRdItemCMS(ar);
		break;
	default:
		bugUnimpl(ar->format);
		break;
	}

	return s;
}

local String
arRdItemAIX(Archive ar)
{
	String	s;
	Offset cc = 0;
	Offset	size, next, prev, date, uid, gid, mode, nlen;
	Offset	align = arInfo(ar->format).align;

	/* Read the item header. */
	arReadDecimal(ar, &size, 12);	cc += 12;
	arReadDecimal(ar, &next, 12);	cc += 12;
	arReadDecimal(ar, &prev, 12);	cc += 12;
	arReadDecimal(ar, &date, 12);	cc += 12;
	arReadDecimal(ar, &uid,  12);	cc += 12;
	arReadDecimal(ar, &gid,  12);	cc += 12;
	arReadOctal(ar,   &mode, 12);	cc += 12;
	arReadDecimal(ar, &nlen,  4);	cc +=  4;

	/* Read the item name. */
	s = strAlloc(nlen);
	arReadText(ar, s, nlen);
	arItem(ar) = s;
	cc += nlen + 2;

	/* Find the item position. */
	if (cc % align != 0) cc += align - (cc % align);
	arPosition(ar) = ar->__next + cc;
	arSeek(ar, arPosition(ar));

	/* Find the position of the next header. */
	ar->__next = next;

	return arItem(ar);
}
local String
arRdItemAIX4(Archive ar)
{
	String	s;
	Offset cc = 0;
	Offset	size, next, prev, date, uid, gid, mode, nlen;
	Offset	align = arInfo(ar->format).align;

	/* Read the item header. */
	arReadDecimal(ar, &size, 20);	cc += 20;
	arReadDecimal(ar, &next, 20);	cc += 20;
	arReadDecimal(ar, &prev, 20);	cc += 20;
	arReadDecimal(ar, &date, 12);	cc += 12;
	arReadDecimal(ar, &uid,  12);	cc += 12;
	arReadDecimal(ar, &gid,  12);	cc += 12;
	arReadOctal(ar,   &mode, 12);	cc += 12;
	arReadDecimal(ar, &nlen,  4);	cc +=  4;

	/* Read the item name. */
	s = strAlloc(nlen);
	arReadText(ar, s, nlen);
	arItem(ar) = s;
	cc += nlen + 2;

	/* Find the item position. */
	if (cc % align != 0) cc += align - (cc % align);
	arPosition(ar) = ar->__next + cc;
	arSeek(ar, arPosition(ar));

	/* Find the position of the next header. */
	ar->__next = next;

	return arItem(ar);
}

local String
arRdItemArch(Archive ar)
{
	String	name = arRdItemArch0(ar);
	ULong	idx = 0L;
	Buffer	buf;
	String	ptr;

	if (name[0] != '\0')
		return name;

	if (name[1] == '/') {
		/* Do we ever get more than one of these? */
		arReadNameTable(ar);


		/* Just go round again... */
		return arRdItemArch(ar);
	}


	/* Indirect name: character offset into the name table. */
	(void)sscanf(&name[1], "%8lu ", &idx);


	/* Debugging */
	arDEBUG {
		(void)fprintf(dbOut, ">>> Reading offset #%lu (out of %lu)\n",
			      idx, (ULong)strLength(ar->names));
	}


	/* Start a new character buffer */
	buf = bufNew();


	/* Jump to the correct place in the buffer */
	ptr = ar->names + idx;


	/* Scan characters up until a / or newline */
	while ((*ptr != '\n') && (*ptr != '/') && *ptr)
		bufPutc(buf, *ptr++);


	/* Convert the buffer into a text string */
	name = bufLiberate(buf);


	/* Debugging */
	arDEBUG {
		(void)fprintf(dbOut, ">>> [%s]\n", name);
	}


	/* Return the name from the directory table */
	return name;
}


local void
arReadNameTable(Archive ar)
{
	Length	size;


	/* How big is the directory record? */
	size = ar->__next - ftell(arFile(ar));


	/* Free the storage associated with the previous directory */
	strFree(ar->names);


	/* Allocate enough store for the directory */
	ar->names = strAlloc(size);


	/* Read the whole table as a block of text */
	FILE_GET_CHARS(arFile(ar), ar->names, size);
}


local String
arRdItemArch0(Archive ar)
{
	/*
	 * See ar.h on GNU systems for a description of the ar_hdr
	 * structure. Reproduced here without permission:
	 *
	 * struct ar_hdr
	 * {
	 *    char ar_name[16]; // Member file name, sometimes / terminated.
	 *    char ar_date[12]; // File date, decimal seconds since Epoch.
	 *    char ar_uid[6];   // User ID, in ASCII decimal.
	 *    char ar_gid[6];   // Group ID, in ASCII decimal.
	 *    char ar_mode[8];  // File mode, in ASCII octal.
	 *    char ar_size[10]; // File size, in ASCII decimal.
	 *    char ar_fmag[2];  // Always contains ARFMAG ("`\n").
	 * };
	 *
	 * If ar_name (without the trailing whitespace) is "//" then we have
	 * a directory record. This is a sequence of file names separated or
	 * terminated by \n.
	 *
	 * If ar_name is "/" followed by an ASCII decimal K, then this is
	 * an offset of K bytes into the directory record read earlier where
	 * the actual member name can be found.
	 *
	 * Otherwise we have a member name followed by its raw data.
	 */
	String	s;
	Offset	size, nlen, cc;
	Offset	align = arInfo(ar->format).align;
	Offset	date, uid, gid, mode;
	char	buffer[32];


	/* Read the whole header */
	nlen = 16;
	s = strAlloc(nlen);
	arReadText(ar,    s,    nlen);
	arReadDecimal(ar, &date,  12);
	arReadDecimal(ar, &uid,    6);
	arReadDecimal(ar, &gid,    6);
	arReadOctal(ar,   &mode,   8);
	arReadDecimal(ar,   &size,  10);
	arReadText(ar,    buffer,  2); /* Magic number */


	/*
	 * Terminate the name: note that the directory name // will
	 * turn into a NULL string with s[1] == '/' (see arRdItemArch).
	 */
	nlen = 0;
	while(s[nlen] != 0 && s[nlen] != ' ' && s[nlen] != '/') nlen++;
	s[nlen] = 0;
	arItem(ar) = s;


	/* Debugging */
	arDEBUG {
		(void)fprintf(dbOut, "[%s: size = %lu bytes]\n",
			      *s ? s : "//", size);
	}


	/*
	 * Forget about character counting: we have just read in the
	 * entire record which is 16 + 12 + 6 + 6 + 8 + 10 + 2 = 60
	 * bytes in length.
	 */
	cc = 60;


	/* Move to the start of the data for this record */
	if (cc % align != 0) cc += align - (cc % align);
	arPosition(ar) = ar->__next + cc;
	arSeek(ar, arPosition(ar));


	/* Find the position of the next header. */
	if (size % align != 0) size += align - (size % align);
	ar->__next = arPosition(ar) + size;


	/* Return a pointer to the name of the item */
	return arItem(ar);
}

local String
arRdItemCMS(Archive ar)
{
	Offset	size, date, uid, gid, mode, nlen, cc;
	Offset	align = arInfo(ar->format).align;
	String	s;
	char buffer[3];

	cc = 0;
	nlen = 20;

	/* Read the item name. */
	s = strAlloc(nlen);
	FILE_GET_CHARS(arFile(ar), s, nlen);
	cc += nlen;

	nlen--;
	while(s[nlen] == ' ') nlen--;
	s[++nlen] = 0;
	arItem(ar) = s;

        /*!! This is because we can't trust fileSize on CMS. */
        if (!arItemIsIntermed(s)) {
                arPosition(ar) = 0;
                arItem(ar) = 0;
                return 0;
        }

	/* Read the item header. */
	arReadDecimal(ar, &date,  12);	cc += 12;
	arReadDecimal(ar, &uid,    6);	cc +=  6;
	arReadDecimal(ar, &gid,    6);	cc +=  6;
	arReadOctal(ar,   &mode,   8);	cc +=  8;
	arReadOctal(ar,   &size,  10);	cc += 10;
	arReadText(ar,    buffer,  2); 	cc +=  2; /* Magic number */

	/* Find the item position. */
	if (cc % align != 0) cc += align - (cc % align);
	arPosition(ar) = ar->__next + cc;
	arSeek(ar, arPosition(ar));

	/* Find the position of the next header. */
	if (size % align != 0) size += align - (size % align);
	ar->__next = arPosition(ar) + size;

	return arItem(ar);
}

/*****************************************************************************
 *
 * :: Archive files
 *
 ****************************************************************************/

local Archive
arNew(FileName fname, FILE *f)
{
	Archive		ar;

	ar = (Archive) stoAlloc(OB_Archive, sizeof(*ar));

	ar->name	= fnameCopy(fname);
	ar->hasFile	= true;
	ar->hasIntermed = false;
	ar->file	= f;

	ar->format	= 0;
	ar->size	= 0;

	ar->item	= 0;
	ar->pos		= 0;
	ar->__next	= 0;

	ar->members	= listNil(ArEntry);
	ar->symes	= listNil(Syme);

	ar->names	= strCopy(""); /* strCopy so we can strFree later */

	return ar;
}

Archive
arRead(FileName fname)
{
	Archive		ar = arNew(fname, fileRbOpen(fname));

	arDEBUG {
		fprintf(dbOut, "Opening archive \"%s\" for reading.\n",
			fnameUnparseStatic(fname));
	}
	arRdFormat(ar);
	arRdTable(ar);

	return ar;
}

void
arClose(Archive ar)
{
	fnameFree(ar->name);
	if (ar->hasFile) fclose(ar->file);
	listFreeDeeply(ArEntry)(ar->members, arFreeEntry);
	listFree(Syme)(ar->symes);

	stoFree((Pointer) ar);
}

Bool
arEqual(Archive ar1, Archive ar2)
{
	return fnameEqual(ar1->name, ar2->name);
}

Archive
arFrString(String name)
{
	static Table	tbl = 0;
	Archive		ar;
	FileName	fn;

	arDEBUG{fprintf(dbOut, "Looking for archive \"%s\"\n", name);}
	
	if (tbl == 0)
		tbl = tblNew((TblHashFun) strAHash, (TblEqFun) strAEqual);

	if ((ar = (Archive) tblElt(tbl, (TblKey) name, (TblElt) 0)) != 0)
		return ar;

	if ((fn = fileRdFind(libSearchPath(), name, FTYPE_AR_INT)) != 0)
		ar = arRead(fn);
	else {  
		comsgWarning(NULL, ALDOR_W_CantUseArchive, name);
		ar = 0;
	}

	tblSetElt(tbl, (TblKey) name, (TblElt) ar);
	return ar;
}

Lib
arFind(PathList path, String name)
{
	Archive		ar;
	ArEntry		arent;

	arDEBUG{fprintf(dbOut, "Looking for '%s' in the archives\n", name);}
	
	for (; path != 0; path = cdr(path)) {
		arDEBUG{fprintf(dbOut, ">> Checking archive %s\n", car(path));}

		ar = arFrString(car(path));
		if (!ar) continue;

		arent = arFindEntry(ar, name);
		if (!arent) continue;

		return arEntryLib(ar, arent);
	}
	return NULL;
}

Lib
arFindInArchive(Archive ar, String name)
{
	ArEntry 	arent;

	arent = arFindEntry(ar, name);

	if (!arent)
		return NULL;
	else
  		return arent->lib;
}

AbSyn
arGetGlobalMacros(Archive ar)
{
	ArEntryList alist;
	AbSynList lst = listNil(AbSyn);

	if (!ar) return NULL;
	for (alist = ar->members; alist; alist = cdr(alist)) {
		AbSyn ab = libGetMacros(arEntryLib(ar, car(alist)));
		listPush(AbSyn, ab, lst);
	}
	return abNewSequenceL(sposNone, lst);
}

SymeList
arGetLibrarySymes(Archive ar)
{
	ArEntryList	alist;
	SymeList	symes = listNil(Syme);

	if (!ar) return symes;

	for (alist = ar->members; alist; alist = cdr(alist)) {
		Syme	lib = libLibrarySyme(arEntryLib(ar, car(alist)));
		listPush(Syme, lib, symes);
	}

	return listNReverse(Syme)(symes);
}

SymeList
arGetSymes(Archive ar)
{
	ArEntryList	alist;
	SymeList	symes;

	if (!ar) return 0;

	if (ar->symes)
		return ar->symes;

	arFilter(ar);

	for (alist = ar->members; alist; alist = cdr(alist)) {
		ArEntry		arent = car(alist);
		if (!arent->mark) continue;
		symes = listReverse(Syme)(libGetSymes(arEntryLib(ar, arent)));
		ar->symes = listNConcat(Syme)(symes, ar->symes);
	}

	ar->symes = listNReverse(Syme)(ar->symes);
	return ar->symes;
}

Syme
arLibrarySyme(Archive ar, Syme syme)
{
	syme = symeOriginal(syme);

	assert(symeLib(syme));
	return libLibrarySyme(symeLib(syme));
}

Bool
arLibraryIsMember(Archive ar, Lib lib)
{
	return arFindLibEntry(ar, lib) != NULL;
}

Bool
arHasBasicLib(Archive ar)
{
	static String	s = NULL;

	if (s == NULL) {
		FileName	fn = fnameNew("", "basic", FTYPE_INTERMED);
		s = fnameUnparse(fn);
		fnameFree(fn);
	}

	return arFindEntry(ar, s) != NULL;
}

/*****************************************************************************
 *
 * :: Archive member replacement
 *
 ****************************************************************************/

static Bool	arUseExpanded = false;

void
arUseExpandedReplacement(void)
{
	arUseExpanded = true;
}

local void
arFilter(Archive ar)
{
	ArEntry		arent0 = arFindEntry(ar, arCurrentFileName);
	ArEntryList	alist;
	Bool		precede = true;
	String tmp;

	if (arent0 == NULL)
		return;

	if (arent0->lib == NULL)
		return;

	tmp = libGetFileId(arent0->lib);
	
	if (!strEqual(tmp, arCurrentFileId))
		return;

	arDEBUG{fprintf(dbOut, "arFilter:\n");}
	comsgWarning(NULL, ALDOR_W_OverRideLibraryFile, arToString(ar));

	arent0->mark = false;
	for (alist = ar->members; alist; alist = cdr(alist)) {
		ArEntry		arent = car(alist);
		libLibrarySyme(arEntryLib(ar, arent));
		if (!arUseExpanded)
			arent->mark = false;
		else if (arent == arent0)
			precede = false;
		else if (precede)
			arent->mark = true;
		else
			arFilterMarkExpanded(ar, arent);
	}

	if (!arUseExpanded) arFilterScanMember(ar, arent0);
}

local void
arFilterScanMember(Archive ar, ArEntry arent0)
{
	SymeList	libs, libs0;
	ArEntryList	alist;
	Bool		marked = true;

	arDEBUG{fprintf(dbOut, "    scanning %s\n", arent0->name);}

	libs0 = libGetLibrarySymes(arEntryLib(ar, arent0));

	for (libs = libs0; libs; libs = cdr(libs)) {
		Lib	lib = symeLibrary(car(libs));
		ArEntry	arent = arFindLibEntry(ar, lib);
		if (arent) arent->mark = true;
	}

	for (alist = ar->members; marked && alist; alist = cdr(alist)) {
		ArEntry	arent = car(alist);
		if (arent == arent0) break;
		marked = arent->mark;
	}

	if (marked) return;

	for (; alist; alist = cdr(alist)) {
		ArEntry	arent = car(alist);
		if (arent == arent0) break;
		arent->mark = false;
	}

	arFilterMarkMember(ar, arent0);
}

local void
arFilterMarkMember(Archive ar, ArEntry arent)
{
	SymeList	libs = libGetLibrarySymes(arEntryLib(ar, arent));

	arDEBUG{fprintf(dbOut, "    marking %s\n", arent->name);}

	for (; libs; libs = cdr(libs)) {
		Lib	lib = symeLibrary(car(libs));
		ArEntry	narent = arFindLibEntry(ar, lib);
		if (narent && !narent->mark) {
			narent->mark = true;
			arFilterMarkMember(ar, narent);
		}
	}
}

local void
arFilterMarkExpanded(Archive ar, ArEntry arent)
{
	SymeList	libs = libGetLibrarySymes(arEntryLib(ar, arent));

	arent->mark = true;
	for (; libs; libs = cdr(libs)) {
		Lib	lib = symeLibrary(car(libs));
		ArEntry	narent = arFindLibEntry(ar, lib);
		if (narent && !narent->mark) {
			arDEBUG{fprintf(dbOut, "    unmarking %s\n",
					arent->name);}
			arent->mark = false;
			return;
		}
	}
}

/*****************************************************************************
 *
 * :: Utility functions taken from uniar2
 *
 ****************************************************************************/

/*
 * arReadString(ar,buf,len) reads precisely "len" characters from
 * the archive "ar" and writes them into "buf" followed by a NUL.
 * Returns -1 on failure (short read or read error), 0 on success.
 */
local int
arReadString(Archive ar, String buffer, int len)
{
	if (fread(buffer, 1, len, arFile(ar)) != len) return -1;
	buffer[len] = 0;
	return 0;
}


/*
 * arReadText(ar,buf,len) executes arReadString(ar,buf,len) but
 * generates an error if the read fails.
 */
local void
arReadText(Archive ar, String dst, int len)
{
	if (!arReadString(ar, dst, len)) return;
	comsgError(NULL, ALDOR_E_ArTruncated, arToString(ar));
	arPosition(ar) = 0;
	arItem(ar) = 0;
	(void)strcpy(dst, "");
}


/*
 * arReadNumber(ar,plong,len,base) executes arReadText(ar,tmp,len)
 * on a temporary buffer "tmp". If the read was successful then the
 * ASCII number in base "base" stored in "tmp" is written to "*plong".
 * Requires that "len <= 1024".
 */
local void
arReadNumber(Archive ar, Offset *plong, int len, int base)
{
	char *endp;

	/* First read the text */
	arReadText(ar, ar_buffer, len);


	/* Must have something in the buffer */
	if (!*ar_buffer) {
		comsgError(NULL, ALDOR_E_ArBadNumber, arToString(ar));
		arPosition(ar) = 0;
		arItem(ar) = 0;
		*plong = 0;
		return;
	}


	/* Convert ASCII into binary */
	*plong = (Offset)strtol(ar_buffer, &endp, base);


	/* Validate the conversion (ignore ERANGE errors for now) */
	if (!*endp || *endp == ' ') return;
	comsgError(NULL, ALDOR_E_ArBadNumber, arToString(ar));
	arPosition(ar) = 0;
	arItem(ar) = 0;
	*plong = 0;
}


/*
 * arReadDecimal(ar,plong,len) executes arReadNumber(fil,tmp,len,10).
 * Requires that "len <= 1024".
 */
local void
arReadDecimal(Archive ar, Offset *plong, int len)
{
	arReadNumber(ar, plong, len, 10);
}


/*
 * arReadOctal(ar,plong,len) executes arReadNumber(ar,tmp,len,8).
 * Requires that "len <= 1024".
 */
local void
arReadOctal(Archive ar, Offset *plong, int len)
{
	arReadNumber(ar, plong, len, 8);
}
