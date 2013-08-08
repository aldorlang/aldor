/*****************************************************************************
 *
 * lib.c: Operations for manipulating compiler output libraries.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlobs.h"
#include "debug.h"
#include "file.h"
#include "fint.h"
#include "format.h"
#include "opsys.h"
#include "opttools.h"
#include "simpl.h"
#include "spesym.h"
#include "stab.h"
#include "store.h"
#include "util.h"
#include "sefo.h"
#include "archive.h"
#include "lib.h"
#include "tqual.h"
#include "doc.h"
#include "comsg.h"
#include "strops.h"
#include "table.h"

Bool	libDebug 	= false;
Bool	libLazyDebug 	= false;
Bool	libVerboseDebug	= false;
Bool	libConstDebug	= false;
Bool	libRepDebug	= false;

#define libDEBUG		DEBUG_IF(libDebug)
#define libLazyDEBUG		DEBUG_IF(libLazyDebug)
#define libVerboseDEBUG		DEBUG_IF(libVerboseDebug)
#define libConstDEBUG		DEBUG_IF(libConstDebug)
#define libRepDEBUG		DEBUG_IF(libRepDebug)

#define libError(lib,tag)		\
	comsgError(NULL, tag, libToStringStatic(lib))

#define LIB_SEEK(lib,pos)		\
	fseek((lib)->file, (lib)->offset + (pos), SEEK_SET)

#define LIB_OFFSET_NONE		((Offset) 0xffffffff)

CREATE_LIST(Lib);

/* the fflush is not strictly needed but there are NFS bugs on AIX4.2 e.g. */
#undef FILE_PUT_CHARS
#define FILE_PUT_CHARS(fout, s, cc)			\
	{ fwrite(s, BYTE_BYTES, cc, fout); fflush(fout); } 

/*****************************************************************************
 *
 * :: Layout of the library file header (local macros/functions)
 *
 ****************************************************************************/

static UShort		libHdrMagic		= 0420;
static UShort		libMajorVersion		= 28;
static UShort		libMinorVersion		= 0;

/*
 * Because of byte-alignment problems, it is incorrect to depend
 * on the size of a structure equalling the number of bytes actually written
 * to the file when each field of the structure is written separately.
 *
 * So we compute libSectSize and libHdrSize
 * in terms of the fields that are actually written to the file.
 */

#define			libSectSize		(BYTE_BYTES + 2 * SINT_BYTES)
#define			libHdrSize		\
	(2 * HINT_BYTES + 2 * SINT_BYTES + LIB_INDEX_LIMIT * libSectSize)

/* Macros for the section -> name -> index -> section mappings */
#define			libIndexSect(lib,i)	((lib)->hdr.Section[i])
#define			libIndexName(lib,i)	((lib)->hdr.Section[i].name)
#define			libNameIndex(lib,n)	((lib)->hdr.Index[n])
#define			libSectOffset(lib,n)	\
	(libIndexSect(lib,libNameIndex(lib,n)).offset)
#define			libSectLength(lib,n)	\
	(libIndexSect(lib,libNameIndex(lib,n)).length)

local Lib		libRegister		(String, Lib);
local Lib		libGetRegistered	(String);
local void		libUnRegister		(Lib);		
local Lib		libFrArchive		(Archive, String);

/* Functions for manipulating the library header. */
local Lib		libNewHeader		(Lib);
local Bool		libChkHeader		(Lib);
local Lib		libPutHeader		(Lib);

/* Functions for manipulating library sections. */
local Buffer		libAddSection		(Lib, LibSectName);
local void		libPutSection		(Lib, LibSectName, Buffer);
local Buffer		libGetSection		(Lib, LibSectName, Bool);
local Bool		libHasSection		(Lib, LibSectName);

local void		libPutPosTbl		(Lib);
local void		libGetPosTbl		(Lib);

local LibList		libGetDependencies	(Lib);
/*
 * Library section name strings.
 * These must have the same order as the elements of the enumeration.
 */
struct _libSectInfo libSectInfoTable[] = {
	{LIB_Syme,	"syme",   "syme"},
	{LIB_Foam,	"foam",   "foam"},
	{LIB_FoamSyme,	"fsyme",  "fsyme"},
	{LIB_Pos,	"pos",    "pos"},
	{LIB_PosTbl,	"postbl", "ptab"},
	{LIB_Name,	"name",   "name"},
	{LIB_Kind,	"kind",   "kind"},
	{LIB_File,	"file",   "file"},
	{LIB_Lazy,	"lazy",   "lazy"},
	{LIB_Type,	"type",   "type"},
	{LIB_Inline,	"inline", "inl"},
	{LIB_Twins,	"twins",  "twins"},
	{LIB_Extend,	"extend", "ext"},
	{LIB_Doc,	"doc",    "doc"},
	{LIB_Id,	"fileid", "id"},
	{LIB_Macros,	"macros", "macros"}
};

Bool
libHasSyme(Lib lib, Syme syme)
{
	UShort	n = symeLibNum(syme);
	return	n < lib->symec && lib->symev[n] == syme;
}

Bool
libCheckSymeNumber(Lib lib, Syme syme, UShort n)
{
	if (n == SYME_NUMBER_UNASSIGNED)
		bug("libCheckSymeNumber:  unassigned syme");
	if (n >= lib->symec)
		bug("libCheckSymeNumber:  syme out of range");
	if (syme && lib->symev[n] != syme)
		bug("libCheckSymeNumber:  wrong syme");

	return true;
}

Bool
libCheckTypeNumber(Lib lib, TForm type, ULong n)
{
	if (n == TYPE_NUMBER_UNASSIGNED)
		bug("libCheckTypeNumber:  unassigned type");
	if (n >= lib->typec)
		bug("libCheckTypeNumber:  type out of range");
	if (type && lib->typev[n] != type)
		bug("libCheckTypeNumber:  wrong type");

	return true;
}

/*****************************************************************************
 *
 * :: Layout of the library file header (external entry points)
 *
 ****************************************************************************/

Lib
libNew(FileName fname, Bool rdOnly, FILE *f, Offset pos)
{
	Lib	lib;

	lib = (Lib) stoAlloc(OB_Lib, sizeof(*lib));

	lib->name	= fnameCopy(fname);
	lib->arent	= NULL;
	lib->rdOnly	= rdOnly;
	lib->intLoaded	= false;
	lib->idName	= NULL;
	lib->file	= f;
	lib->offset	= pos;
	lib->self	= NULL;

	if (lib->rdOnly)
		lib->stab = stabPushLevel(stabFile(), sposNone, long0);

	lib->symec	= 0;
	lib->symev	= 0;
	lib->symes	= 0;
	lib->symep	= 0;
	lib->codev	= 0;
	lib->triggers	= 0;

	lib->typec	= 0;
	lib->typev	= 0;
	lib->types	= 0;
	lib->typep	= 0;
	lib->typeb	= 0;

	lib->constc	= 0;
	lib->constv	= 0;
	lib->constp	= 0;
	lib->unitb	= 0;
	lib->pos	= 0;
	lib->postbl	= 0;
	lib->unit	= 0;
	lib->formats	= 0;
	lib->macros  	= NULL;
	libNewHeader(lib);

	return lib;
}

Lib
libExtract(FileName fname, FILE *f, Offset pos)
{
	return libGetHeader(libNew(fname, 2 | 1, f, pos));
}

Lib
libRead(FileName fname)
{
	Lib lib =  libExtract(fname, fileRbOpen(fname),  (Offset) 0);
	lib->rdOnly = 1;
	return lib;
}

Lib
libWrite(FileName fname)
{
	return libNew(fname, false, fileWubOpen(fname), (Offset) 0);
}

/*
 * Clear the cached syme information for lib.
 */

local void
libClearSymes(Lib lib)
{
	if (lib->symev)
		stoFree((Pointer) lib->symev);
	if (lib->symes)
		listFree(Syme)(lib->symes);
	if (lib->symep)
		stoFree((Pointer) lib->symep);
	if (lib->codev)
		stoFree((Pointer) lib->codev);
	if (lib->triggers)
		listFree(Syme)(lib->triggers);

	lib->symec = 0;
	lib->symev = 0;
	lib->symes = 0;
	lib->symep = 0;
	lib->codev = 0;
	lib->triggers = 0;
}

/*
 * Clear the cached type forms for lib.
 */

local void
libClearTypes(Lib lib)
{
	if (lib->typeb)
		bufFree(lib->typeb);
	if (lib->typev)
		stoFree((Pointer) lib->typev);
	if (lib->types)
		listFree(TForm)(lib->types);
	if (lib->typep)
		stoFree((Pointer) lib->typep);

	lib->typec = 0;
	lib->typev = 0;
	lib->typep = 0;
	lib->typeb = 0;
}

/*
 * Clear the cached foam information for lib.
 */
local void
libClearFoam(Lib lib)
{
	int	i;

	if (lib->unitb) {
		bufFree(lib->unitb);

		if (lib->formats)
			foamFree(lib->formats);

		for (i = 0; i < lib->constc; i += 1)
			if (lib->constv[i])
				foamFree(lib->constv[i]);
	}
	if (lib->constv)
		stoFree((Pointer) lib->constv);
	if (lib->constp)
		stoFree((Pointer) lib->constp);

	lib->unitb	= 0;
	lib->unit	= 0;
	lib->formats	= 0;
	lib->constc	= 0;
	lib->constv	= 0;
	lib->constp	= 0;
}

local void
libClearPos(Lib lib)
{
	if (lib->pos) bufFree(lib->pos);
	if (lib->postbl) bufFree(lib->postbl);
	lib->pos = 0;
	lib->postbl = 0;
}

/*
 * The Lib structure should own its FILE *, FileName, SymeList, Buffer,
 * and any foam extracted from the Buffer.
 */
void
libClose(Lib lib)
{
	if (lib->rdOnly)
		stabFree(lib->stab);
	else
		libPutHeader(lib);

	if (!(lib->rdOnly & 2)) fclose(lib->file);	
	libUnRegister(lib);
	fnameFree(lib->name);

	libClearSymes(lib);
	libClearTypes(lib);
	libClearFoam(lib);
	libClearPos(lib);

	stoFree((Pointer) lib);
}

Bool
libEqual(Lib lib1, Lib lib2)
{
	return fnameEqual(lib1->name, lib2->name);
}

Hash
libHash(Lib lib)
{
	return strHash(libToString(lib));
}

/*****************************************************************************
 *
 * :: Finding and fudging libraries
 *
 ****************************************************************************/

/* This ought to be killed before starting a compile */

static Table libLibTbl;

local Lib
libRegister(String name, Lib lib)
{
	if (libLibTbl == NULL)
		libLibTbl = tblNew((TblHashFun) strHash, (TblEqFun) strEqual);
	
	tblSetElt(libLibTbl, (TblKey) strCopy(name), (TblElt) lib);
	return lib;
}

local Lib
libGetRegistered(String name)
{
	if (libLibTbl == NULL)
		return NULL;

	return (Lib) tblElt(libLibTbl, (TblKey) name, (TblElt) NULL);
}

local void
libUnRegister(Lib lib)
{
	String name;
	if (!libLibTbl) return;

	name =fnameUnparse(lib->name);
	tblDrop(libLibTbl, name);
}

Lib
libFrString(String name)
{
	Lib		lib;
	FileName	fn;

	libDEBUG{fprintf(dbOut, "Looking for library \"%s\"\n", name);}

	if ((lib = libGetRegistered(name)) != NULL)
		return lib;

	if ((fn = fileRdFind(libSearchPath(), name, FTYPE_INTERMED)) != NULL)
		lib = libRead(fn);

	else if ((lib = arFind(arLibraryFiles(), name)) == NULL) {
		if (fintMode == FINT_LOOP)
			comsgError(NULL, ALDOR_F_CantOpen, name);
		else
			comsgFatal(NULL, ALDOR_F_CantOpen, name);
	}

	libRegister(name, lib);
	return lib;
}

local Lib
libFrArchive(Archive ar, String name)
{
	Lib lib;

	lib = arFindInArchive(ar, name);

	if (!lib)
		return libFrString(name);
	else
		return lib;
}

#define		libSymeMatch(lib,sym,code,s)	\
	(symeId(s) == (sym) && symeHash(s) == (code) && symeLib(s) == (lib))
#define		libCodeSyme(lib,i)		lib->symev[lib->codev[i]]

static Lib	libCmpLib = NULL;

int
libCmpCode(UShort *i, UShort *j)
{
	assert(libCmpLib);
	return symeHash(libCmpLib->symev[*i]) - symeHash(libCmpLib->symev[*j]);
}

local void
libCodeSort(Lib lib)
{
	UShort	i;

	/* Make sure all of the syme hash codes have been computed. */
	for (i = 0; i < lib->symec; i += 1) symeTypeCode(lib->symev[i]);

	libCmpLib = lib;
	lisort(lib->codev, lib->symec, sizeof(UShort),
	       (int (*)(ConstPointer, ConstPointer)) libCmpCode);
	libCmpLib = NULL;
}

local UShort
libSymeIndex(Lib lib, Symbol sym, Hash code)
{
	UShort	lo = 0, hi = lib->symec;

	while (lo < hi) {
		UShort	mid = lo + ((hi - lo) >> 1);
		assert(lo <= mid && mid < hi);

		if (symeHash(libCodeSyme(lib, mid)) < code)
			lo = mid + 1;
		else
			hi = mid;
	}

	for (; lo < lib->symec && symeHash(libCodeSyme(lib, lo)) == code; ++lo)
		if (libSymeMatch(lib, sym, code, libCodeSyme(lib, lo)))
			return lib->codev[lo];

	return lib->symec;
}

ULong
libSymeTypeNo(Lib lib, Symbol sym, Hash code)
{
	UShort	i = libSymeIndex(lib, sym, code);

	if (i < lib->symec) return lib->symep[i];

	bug("libSymeTypeNo:  cannot find '%s' in '%s'.",
	    symString(sym), libToStringShort(lib));
	return TYPE_NUMBER_UNASSIGNED;
}

local Syme
libSymeSyme(Lib nlib, Lib lib, Symbol sym, Hash code)
{
	UShort		i;
	Syme		syme = NULL;
	SymeList	symes;

	if (lib->codev) {
		i = libSymeIndex(lib, sym, code);
		if (i < lib->symec)
			syme = lib->symev[i];
		else {
			comsgFatal(NULL, ALDOR_F_LibOutOfDate,
				   libToStringShort(lib),
				   libToStringShort(nlib));
			return syme;
		}
	}
	else if (lib->symes) {
		for (i = 0; i < lib->topc && !syme; i += 1)
			if (libSymeMatch(lib, sym, code, lib->symev[i]))
				syme = lib->symev[i];
	}

	for (symes = lib->triggers; symes && !syme; symes = cdr(symes))
		if (libSymeMatch(lib, sym, code, symes->first))
			syme = symes->first;

	if (!syme) {
		syme = symeNewLib(SYME_Trigger, sym, (TForm) nlib, lib);
		symeSetHash(syme, code);
		listPush(Syme, syme, lib->triggers);
	}

	return syme;
}

Syme
libLibrarySyme(Lib lib)
{
	String id;
	
	if (strEqual(libGetFileId(lib), "axiom"))
		id = libToStringShort(lib);
	else 
		id = libGetFileId(lib);

	if (lib->self == NULL) {
		String	name = strPrintf("%s Lib", id);
		Symbol	sym = symIntern(name);
		Syme	syme = stabGetLibrary(sym);

		if (syme == NULL)
			syme = stabDefLibrary(stabFile(), sym, tfType, lib);
		lib->self = syme;
	}
	return lib->self;
}

Bool
libIsBasicLib(Lib lib)
{
	return strEqual(libToStringShort(lib), "basic");
}

/*****************************************************************************
 *
 * :: Library statistics
 *
 ****************************************************************************/

StringList
libDependencies(FileName fname)
{
  	StringList names;
  	LibList libs;
	Lib 	lib;
	
	lib = libRead(fname);
	libGetAllSymes(lib);
	libs = libGetDependencies(lib);

	names = listNil(String);
	while (libs != listNil(Lib)) {
	  	names = listCons(String)(strCopy(libGetFileId(car(libs))), names);
	  	libs = cdr(libs);
	}

	libClose(lib);
	return names;
}

LibStats
libStats(FileName fname, LibStats libstats)
{
	Lib	lib;
	int	i;

	libstats->libSize = fileSize(fname);

	lib = libRead(fname);
	for (i = LIB_NAME_START; i < LIB_NAME_LIMIT; i++)
		libstats->sectSize[i] = libSectLength(lib, i);

	libClose(lib);
	
	return libstats;
}

LibStats
libStatsClear(LibStats libstats)
{
	int	i;

	libstats->libSize = 0;
	for (i = LIB_NAME_START; i < LIB_NAME_LIMIT; i++)
		libstats->sectSize[i] = 0;
	/* !! Free libstats->dependencies */

	return libstats;
}

LibStats
libStatsIncrement(LibStats libtot, LibStats libincr)
{
	int	i;

	libtot->libSize += libincr->libSize;
	for (i = LIB_NAME_START; i < LIB_NAME_LIMIT; i++)
		libtot->sectSize[i] += libincr->sectSize[i];

	return libtot;
}


/*****************************************************************************
 *
 * :: Functions for manipulating the library header.
 *
 ****************************************************************************/

local Lib
libNewHeader(Lib lib)
{
	UShort i, n;

	/* Initialize the header. */
	lib->hdr.magic	     = libHdrMagic;
	lib->hdr.verMajor    = libMajorVersion;
	lib->hdr.verMinor    = libMinorVersion;
	lib->hdr.numSect     = 0;

	/* Initialize the sections. */
	for( i = LIB_INDEX_START; i < LIB_HDR_LIMIT; i += 1 ) {
		libIndexSect(lib, i).name    = LIB_INDEX_LIMIT;
		libIndexSect(lib, i).offset  = 0;
		libIndexSect(lib, i).length  = 0;
	}

	/* Initialize the section name -> index mapping. */
	for( n = LIB_NAME_START; n < LIB_HDR_LIMIT; n += 1 )
		libNameIndex(lib, n) = LIB_INDEX_LIMIT;

	return(lib);
}

local Bool
libChkHeader(Lib lib)
{
	UShort i;

	/* Check magic number. */
	if( lib->hdr.magic != libHdrMagic ) {
		libError(lib, ALDOR_E_LibBadMagic);
		return false;
	}

	/* Check version numbers. */
	if( lib->hdr.verMajor <	 libMajorVersion ||
	    (lib->hdr.verMajor == libMajorVersion &&
	     lib->hdr.verMinor <  libMinorVersion) )
		comsgFatal(NULL, ALDOR_F_LibBadVersion,
			libToStringStatic(lib),
			libMajorVersion, libMinorVersion,
			lib->hdr.verMajor, lib->hdr.verMinor);

	/* Check the number of sections. */
	if( !(lib->hdr.numSect <= LIB_INDEX_LIMIT) ) {
		libError(lib, ALDOR_E_LibBadNumSect);
		return false;
	}

	/* Check the section names. */
	for( i = LIB_INDEX_START; i < lib->hdr.numSect; i += 1 ) {
		LibSectName n = libIndexName(lib, i);

		if( n >= LIB_NAME_LIMIT ) {
			libError(lib, ALDOR_E_LibBadSectName);
			return false;
		}
		if( libNameIndex(lib, n) != i )
			bug( "Index[Name[i]] != i" );
	}

#if 0
	/* Check the section indices. */
	for( n = LIB_NAME_START; n < LIB_NAME_LIMIT; n += 1 ) {
		UShort i = libNameIndex(lib, n);

		if( i != LIB_INDEX_LIMIT ) {
			if( !(i < lib->hdr.numSect) )
				bug( "Bad section index" );
			if( libIndexName(lib, i) != n )
				bug( "Name[Index[n]] != n" );
		}
	}
#endif
	/* Check initial section header. */
	if( libIndexSect(lib, LIB_INDEX_START).offset != libHdrSize ) {
		libError(lib, ALDOR_E_LibBadSectHdr);
		return false;
	}

	/* Check remaining section headers. */
	for( i = LIB_INDEX_START + 1; i < lib->hdr.numSect; i += 1 )
		if( libIndexSect(lib, i).offset !=
		    libIndexSect(lib, i-1).offset +
		    libIndexSect(lib, i-1).length ) {
			libError(lib, ALDOR_E_LibBadSectHdr);
			return false;
		}

	return true;
}

local Lib
libPutHeader(Lib lib)
{
	int	i;
	Buffer	buf = bufNew();

	if (libChkHeader(lib) == 0)
		bug("bad header given to libPutHeader");

	BUF_PUT_HINT(buf, lib->hdr.magic);
	BUF_PUT_SINT(buf, lib->hdr.verMajor);
	BUF_PUT_SINT(buf, lib->hdr.verMinor);
	BUF_PUT_HINT(buf, lib->hdr.numSect);

	for( i = LIB_INDEX_START; i < LIB_INDEX_LIMIT; i += 1 ) {
		BUF_PUT_BYTE(buf, libIndexSect(lib,i).name);
		BUF_PUT_SINT(buf, libIndexSect(lib,i).offset);
		BUF_PUT_SINT(buf, libIndexSect(lib,i).length);
	}

	LIB_SEEK(lib, long0);
	FILE_PUT_CHARS(lib->file, bufChars(buf), bufPosition(buf));
	bufFree(buf);

	return lib;
}

Lib
libGetHeader(Lib lib)
{
	int	i;
	Buffer	buf;
	Offset	cc;
	String	s;

	LIB_SEEK(lib, long0);
	cc = libHdrSize;
	s = strAlloc(cc);
	FILE_GET_CHARS(lib->file, s, cc);
	buf = bufCapture(s, cc);

	BUF_GET_HINT(buf, lib->hdr.magic);

	BUF_GET_SINT(buf, lib->hdr.verMajor);
	BUF_GET_SINT(buf, lib->hdr.verMinor);

	BUF_GET_HINT(buf, lib->hdr.numSect);

	for( i = LIB_INDEX_START; i < LIB_INDEX_LIMIT; i += 1 ) {
		BUF_GET_BYTE(buf, libIndexSect(lib,i).name);
		BUF_GET_SINT(buf, libIndexSect(lib,i).offset);
		BUF_GET_SINT(buf, libIndexSect(lib,i).length);
	}

	/* Set up the section indices. */
	for( i = LIB_INDEX_START; i < LIB_INDEX_LIMIT; i += 1 ) {
		LibSectName n = libIndexName(lib, i);

		if( n < LIB_NAME_LIMIT )
			libNameIndex(lib, n) = i;
	}

	libChkHeader(lib);
	return lib;
}

/*****************************************************************************
 *
 * :: Functions for manipulating library sections.
 *
 ****************************************************************************/

/* Add a section to the library header.
 * Return the buffer to be used to collect the section.
 */
local Buffer
libAddSection(Lib lib, LibSectName name)
{
	UShort i = lib->hdr.numSect;

	/* Make sure we have room for the new section. */
	if( i == LIB_INDEX_LIMIT ) {
		libError(lib, ALDOR_E_LibSectLimit);
		return 0;
	}

	/* Make sure we haven't written this section before. */
	assert(name < LIB_NAME_LIMIT);
	if( libNameIndex(lib, name) != LIB_INDEX_LIMIT ) {
		libError(lib, ALDOR_E_LibSectDup);
		return 0;
	}

	lib->hdr.numSect += 1;
	libNameIndex(lib, name) = i;
	libIndexName(lib, i) = name;
	libIndexSect(lib, i).length = -1;
	libIndexSect(lib, i).offset =
		(i == 0) ? libHdrSize
			 : libIndexSect(lib, i-1).offset +
			   libIndexSect(lib, i-1).length;

	libVerboseDEBUG {
		fprintf(dbOut, "Putting %s section \"%s\":\t", FTYPE_INTERMED,
			libSectInfo(name).str);
	}

	return bufNew();
}

/* Write the contents of buf to lib->file as the named section. */
local void
libPutSection(Lib lib, LibSectName name, Buffer buf)
{
	int	cc = bufPosition(buf);

	LIB_SEEK(lib, libSectOffset(lib, name));
	FILE_PUT_CHARS(lib->file, bufChars(buf), cc);
	libSectLength(lib, name) = cc;
	bufFree(buf);

	libVerboseDEBUG{fprintf(dbOut, "%12d bytes\n", cc);}
}

static Buffer	LibStaticSectBuffer = 0;

local Buffer
libGetSection(Lib lib, LibSectName name, Bool stat)
{
	Offset	cc;
	String	s;
	Buffer	buf;

	if (!libHasSection(lib, name)) return 0;

	libVerboseDEBUG{fprintf(dbOut, "Getting %s section \"%s\":\t", 
				FTYPE_INTERMED, libSectInfo(name).str);}

	/* Seek to the beginning of the section. */
	LIB_SEEK(lib, libSectOffset(lib, name));

	/* Read the number of bytes in the section. */
	cc = libSectLength(lib, name);
	libVerboseDEBUG{fprintf(dbOut, "%12ld bytes\n", cc);}

	if (stat) {
		if (LibStaticSectBuffer == 0)
			LibStaticSectBuffer = bufNew();
		bufNeed(LibStaticSectBuffer, cc);

		s = bufChars(LibStaticSectBuffer);
		buf = LibStaticSectBuffer;
	}
	else {
		s = strAlloc(cc);
		buf = bufCapture(s, cc);
	}

	FILE_GET_CHARS(lib->file, s, cc);
	bufPosition(buf) = 0;
	return buf;
}

local Bool
libHasSection(Lib lib, LibSectName name)
{
	return libSectOffset(lib, name) != 0;
}

/*****************************************************************************
 *
 * :: Layout of id section
 *
 ****************************************************************************/

void
libPutFileId(Lib lib, String fileId)
{
	Buffer buf = libAddSection(lib, LIB_Id);

	if (buf) {
		bufWrString(buf, fileId);
	}
	libPutSection(lib, LIB_Id, buf);
}

String
libGetFileId(Lib lib)
{
	Buffer buf;

	if (lib->idName == NULL) {
		buf = libGetSection(lib, LIB_Id, true);
		lib->idName = bufRdString(buf);
	}
	return lib->idName;
}


/*****************************************************************************
 *
 * :: Layout of syme section
 *
 ****************************************************************************/

#define	LIB_SPECIAL_MASK	((UByte) 0x20)	/* Special foam op? */
#define	LIB_DEF_MASK		((UByte) 0x40)	/* Has default? */
#define	LIB_TOP_MASK		((UByte) 0x80)	/* Exported at top level? */
#define	LIB_BITS_MASK		\
	(LIB_DEF_MASK | LIB_TOP_MASK | LIB_SPECIAL_MASK)

#define		libGetSymeIndex(buf, i)		((i) = bufRdUShort(buf))

local Bool		libSymeIsFile		(Lib, Syme);
local Bool		libSymeIsLazy		(Lib, Syme);
local Bool		libSymeIsLocal		(Lib, Syme);
local Bool		libSymeWasLocal		(Lib, Syme);

Bool
libSymeIsTop(Lib lib, Syme syme)
{
	if (libSymeIsFile(lib, syme))
		return true;

	while (symeExtension(syme))
		syme = symeExtension(syme);

	if (!symeTop(syme))
		return false;

	if (symeLib(syme) == NULL || symeLib(syme) == lib)
		return true;

	return false;
}

local Bool
libSymeIsFile(Lib lib, Syme syme)
{
	return symeIsLibrary(syme) || symeIsArchive(syme);
}

local Bool
libSymeIsLazy(Lib lib, Syme syme)
{
	return !(libSymeIsFile(lib, syme) || libSymeIsLocal(lib, syme));
}

local Bool
libSymeIsLocal(Lib lib, Syme syme)
{
	if (libSymeIsFile(lib, syme))
		return false;

	/* if it is then symeCloseOverDetails is wrong! */
	assert(symeLib(syme) != lib);

	/*
	 * These two lines are a hack to nuke the 
	 * implementation details.  Should really hang on to them.
	 */
	if (symeImpl(syme) && implIsBranch(symeImpl(syme)))
		symeClrConstNum(syme);

	if (symeImpl(syme) && implIsCond(symeImpl(syme)))
		symeClrConstNum(syme);

	if (symeLib(syme) == NULL || symeLib(syme) == lib)
		return true;

	if (symeConstLib(syme) == lib)
		return true;
	if (symeIsExport(syme) 
	    && symeConstLib(symeOriginal(syme)) != symeConstLib(syme)
	    && symeConstNum(syme) != SYME_NUMBER_UNASSIGNED)
		return true;
		
	if (symeIsExtend(syme) && syme->lib != NULL)
	  	return false;

	if (symeIsExport(syme) || symeIsParam(syme) || symeIsSelf(syme))
		return false;

	if (symeIsImport(syme) && tfIsLazyExporter(symeExporter(syme)))
		return false;

	return true;
}

local Bool
libSymeWasLocal(Lib lib, Syme syme)
{
	if (libSymeIsFile(lib, syme))
		return false;

	return symeLib(syme) == NULL || symeLib(syme) == lib;
}

/*****************************************************************************
 *
 * :: libRepSymes
 *
 ****************************************************************************/

local void	libRepSymeNames		(FILE *, Lib, Bool);
local void	libRepSymeKinds		(FILE *, Lib, Bool);
local void	libRepSymeFiles		(FILE *, Lib, Bool);
local void	libRepSymeLazys		(FILE *, Lib, Bool);
local void	libRepSymeTypes		(FILE *, Lib, Bool);
local void	libRepSymeSymes		(FILE *, Lib, Bool);
local void	libRepSymeTwins		(FILE *, Lib, Bool);
local void	libRepSymeExts		(FILE *, Lib, Bool);
local void	libRepSymeDocs		(FILE *, Lib, Bool);

void
libRepSymes(FILE * f, Lib lib, Bool top)
{
	/*
	 * This routine might be invoked when only the top-level
	 * library symes have been filled in. If this happens
	 * then you had better make sure that `top == true'
	 * otherwise bad things will happen.
	 */
	fnewline(f);
	libRepSymeNames(f, lib, top);
	fnewline(f);
	libRepSymeKinds(f, lib, top);
	fnewline(f);
	libRepSymeFiles(f, lib, top);
	fnewline(f);
	libRepSymeLazys(f, lib, top);
	fnewline(f);
	libRepSymeTypes(f, lib, top);
	fnewline(f);
	libRepSymeSymes(f, lib, top);
	fnewline(f);
	libRepSymeTwins(f, lib, top);
	fnewline(f);
	libRepSymeExts(f, lib, top);
	fnewline(f);
	libRepSymeDocs(f, lib, top);
	fnewline(f);
	fnewline(f);
}

local void
libRepSymeNames(FILE * f, Lib lib, Bool top)
{
	UShort	i, symec = top ? lib->topc : lib->symec;

	(void)fprintf(f, "Names (%s):\n", top ? "top level only" : "");

	for (i = 0; i < symec; i += 1) {
		Syme	syme = lib->symev[i];
		(void)fprintf(f, "\t%s\n", symeString(syme));
	}
}

local void
libRepSymeKinds(FILE * f, Lib lib, Bool top)
{
	UShort	i, symec = top ? lib->topc : lib->symec;

	(void)fprintf(f, "Kinds (%s):\n", top ? "top level only" : "");

	for (i = 0; i < symec; i += 1) {
		Syme	syme = lib->symev[i];
		(void)fprintf(f, "\t%s\n", symeTagToStr(symeKind(syme)));
	}
}

local void
libRepSymeFiles(FILE * f, Lib lib, Bool top)
{
	UShort	i, symec = top ? lib->topc : lib->symec;

	(void)fprintf(f, "Files (%s):\n", top ? "top level only" : "");

	for (i = 0; i < symec; i += 1) {
		Syme	syme = lib->symev[i];
		String	s = NULL;

		if (!libSymeIsFile(lib, syme)) continue;

		if (symeIsLibrary(syme))
			s = libToStringStatic(symeLibrary(syme));
		if (symeIsArchive(syme))
			s = arToStringStatic(symeArchive(syme));

		if (s) (void)fprintf(f, "\t%s\n", s);
	}
}

local void
libRepSymeLazys(FILE * f, Lib lib, Bool top)
{
	UShort	i, symec = top ? lib->topc : lib->symec;

	(void)fprintf(f, "Lazys (%s):\n", top ? "top level only" : "");

	for (i = 0; i < symec; i += 1) {
		Syme	syme = lib->symev[i];
		Hash	hash = symeTypeCode(syme);

		if (libSymeIsFile(lib, syme))
			(void)fprintf(f, "\tFile.\n");
		else if (libSymeWasLocal(lib, syme))
			(void)fprintf(f, "\tLocal.\n");
		else
			(void)fprintf(f, "\tLazy:  0x%08x  %s\n",
				(unsigned int)hash,
				libToStringStatic(symeLib(syme)));
	}
}

local void
libRepSymeTypes(FILE * f, Lib lib, Bool top)
{
	UShort	i, symec = top ? lib->topc : lib->symec;

	(void)fprintf(f, "Types (%s):\n", top ? "top level only" : "");

	/* Check that we actually have some types ... */
	if (!(lib->symep)) return;

	for (i = 0; i < symec; i += 1) {
		Syme	syme = lib->symev[i];
		ULong	n = lib->symep[i];

		if (!libSymeWasLocal(lib, syme)) continue;

		if (symeIsImport(syme))
			(void)fprintf(f, "\t0x%08lx  (Import)\n", n);
		else if (symeIsSelf(syme))
			(void)fprintf(f, "\t0x%08lx  (Self)\n", n);
		else
			(void)fprintf(f, "\t0x%08lx\n", n);
	}
}

local void
libRepSymeSymes(FILE * f, Lib lib, Bool top)
{
	UShort	i, symec = top ? lib->topc : lib->symec;

	(void)fprintf(f, "Symes (%s):\n", top ? "top level only" : "");

	for (i = 0; i < symec; i += 1) {
		Syme		syme = lib->symev[i];
		SymeList	symes = symeInlined(syme);

		/* Do we have any symes? */
		if (!symes) continue;

		(void)fprintf(f, "\t");
		for (; symes; symes = cdr(symes))
			(void)fprintf(f, " %d", symeLibNum(car(symes)));
		(void)fprintf(f, ".\n");
	}
}

local void
libRepSymeTwins(FILE * f, Lib lib, Bool top)
{
	UShort	i, symec = top ? lib->topc : lib->symec;

	(void)fprintf(f, "Twins (%s):\n", top ? "top level only" : "");

	for (i = 0; i < symec; i += 1) {
		Syme		syme = lib->symev[i];
		SymeList	symes = symeTwins(syme);

		/* Do we have any symes? */
		if (!symes) continue;

		(void)fprintf(f, "\t");
		for (; symes; symes = cdr(symes))
			(void)fprintf(f, " %d", symeLibNum(car(symes)));
		(void)fprintf(f, ".\n");
	}
}

local void
libRepSymeExts(FILE * f, Lib lib, Bool top)
{
	UShort	i, symec = top ? lib->topc : lib->symec;

	(void)fprintf(f, "Extendees (%s):\n", top ? "top level only" : "");

	for (i = 0; i < symec; i += 1) {
		Syme		syme = lib->symev[i];
		SymeList	symes;

		if (!symeIsExtend(syme)) continue;

		/* Do we have any symes? */
		if (!(symes = symeExtendee(syme))) continue;

		(void)fprintf(f, "\t");
		for (; symes; symes = cdr(symes))
			(void)fprintf(f, " %d", symeLibNum(car(symes)));
		(void)fprintf(f, ".\n");
	}
}

local void
libRepSymeDocs(FILE * f, Lib lib, Bool top)
{
	UShort	i, symec = top ? lib->topc : lib->symec;

	(void)fprintf(f, "Documentation (%s):\n", top ? "top level only" : "");

	for (i = 0; i < symec; i += 1) {
		Syme	syme = lib->symev[i];
		Doc	doc;

		if (!symeIsExport(syme)) continue;

		doc = symeComment(syme);
		if (doc != docNone)
			(void)fprintf(f, "%s\n\n", docString(doc));
	}
}

/*****************************************************************************
 *
 * :: Dependencies
 *
 ****************************************************************************/

local LibList
libGetDependencies(Lib lib)
{
	LibList  libs = listNil(Lib);
  	int i;

	for (i = 0; i < lib->symec; i++) {
	  	Syme syme = lib->symev[i];
		if (!symeIsArchive(syme) && !symeIsLibrary(syme)) {
		  	Lib tlib;
			tlib = symeLib(syme);
			if (lib != tlib && !listMemq(Lib)(libs, tlib))
			  	libs = listCons(Lib)(tlib, libs);
			tlib = symeConstLib(syme);
			if (lib != tlib && !listMemq(Lib)(libs, tlib))
			  	libs = listCons(Lib)(tlib, libs);
		}
	}
	return libs;
}


/*****************************************************************************
 *
 * :: libPutSymes
 *
 ****************************************************************************/

local void	libInitSymev		(Lib);
local void	libPutSymeNames		(Lib);
local void	libPutSymeKinds		(Lib);
local void	libPutSymeFiles		(Lib);
local void	libPutSymeLazys		(Lib);
local void	libPutSymeTypes		(Lib);
local void	libPutSymeSymes		(Lib);
local void	libPutSymeTwins		(Lib);
local void	libPutSymeExts		(Lib);
local void	libPutSymeDocs		(Lib);
local void	libPutSymev		(Lib);

extern void	libCheckSymes		(Lib);

SymeList
libPutSymes(Lib lib, SymeList symes, Foam foam)
{
	libDEBUG {
		SymeList l = symes;
		fprintf(dbOut, "libPutSymes:");
		fnewline(dbOut);
		while (l != listNil(Syme)) {
			Syme syme = car(l);
			afprintf(dbOut, "PutSyme: %s: %pAbSynList\n", 
				 symeString(syme), symeCondition(syme));
			l = cdr(l);
		}
		listPrint(Syme)(dbOut, symes, symePrint);
	}

	/* symeListClosure sets up the cached syme/type lists. */
	symeListClosure(lib, symes);
	otTransferFoamInfo(lib->symes, foam);

	/* Allocate the syme/type vectors. */
	libInitSymev(lib);

	/* Produce the symbol name section. */
	libPutSymeNames(lib);

	/* Produce the syme kind section. */
	libPutSymeKinds(lib);

	/* Produce the file section. */
	libPutSymeFiles(lib);

	/* Produce the lazy section. */
	libPutSymeLazys(lib);

	/* Produce the type section. */
	libPutSymeTypes(lib);

	/* Produce the symes inlined section. */
	libPutSymeSymes(lib);

	/* Produce the syme twins section. */
	libPutSymeTwins(lib);

	/* Compute the extendee offset information. */
	libPutSymeExts(lib);

	/* Compute the doc offset information. */
	libPutSymeDocs(lib);

	/* Produce the constant-sized information section. */
	libPutSymev(lib);


	/* Check that all symes have valid libNums */
	assert(libValidateSymes(lib->symes));
	assert(libValidateSymes(tfType->symes));

	libRepDEBUG{libRepSymes(dbOut, lib, false);}

	libConstDEBUG {
		SymeList tmp = lib->symes;
		Syme     syme;

		int i=0;
		while (tmp != listNil(Syme)) {
			syme = tmp->first;
			tmp  = tmp->rest;
			afprintf(dbOut, "%d: %s %s %ld %s (%s %s . %ld)\n", 
				 i,
				 libSymeIsLocal(lib, syme) ? "O" : "Z",
				 symeInfo[symeKind(syme)].str,
				 syme->hash,
				 syme->id->str,
				 (symeLib(syme) == NULL || symeLib(syme) == lib )
				   ? "Local"
				   :  libGetFileId(symeLib(syme)),
				 symeConstLib(syme) 
				   ? libGetFileId(symeConstLib(syme)) 
				   : "Local",
				 symeConstNum(syme));
			i++;
		}
	}

	libConstDEBUG {
		int i;
		for (i = 0; i<lib->typec; i++) {
			afprintf(dbOut, "(%d: %pTForm)\n", i, lib->typev[i]);
		}
	}
	return symes;
}

local void
libInitSymev(Lib lib)
{
	SymeList 	sl;
	TFormList 	tl;
	UShort		i, symec = 0, topc = 0;
	ULong		typec;

	lib->symev = (Syme  *) stoAlloc(OB_Other, lib->symec * sizeof(Syme));
	lib->symep = (ULong *) stoAlloc(OB_Other, lib->symec * sizeof(ULong));
	lib->codev = (UShort*) stoAlloc(OB_Other, lib->symec * sizeof(UShort));

	/* Fill symev with the symes provided by symeListClosure. */ 
	for (sl = lib->symes; sl; sl = cdr(sl)) {
		i = libSymeIsTop(lib, car(sl)) ? topc++ : lib->topc + symec++;
		lib->symev[i] = car(sl);
		lib->symep[i] = TYPE_NUMBER_UNASSIGNED;
		lib->codev[i] = i;
		symeSetLibNum(car(sl), i);
	}
	assert(lib->topc == topc);
	assert(lib->symec == topc + symec);

	lib->typev = (TForm *) stoAlloc(OB_Other, lib->typec * sizeof(TForm));

	for (typec = 0, tl = lib->types; tl; typec += 1, tl = cdr(tl)) {
		lib->typev[typec] = car(tl);
		car(tl)->libNum = typec;
	}
	assert(lib->typec == typec);
}

local void
libPutSymeNames(Lib lib)
{
	Table	tbl;
	UShort	i, symec = lib->symec, topc = lib->topc;
	Buffer	buf = libAddSection(lib, LIB_Name);

	if (!buf) return;

	BUF_PUT_HINT(buf, symec);
	BUF_PUT_HINT(buf, topc);

	/* Write the syme name positions. */

	tbl = tblNew((TblHashFun) 0, (TblEqFun) 0);

	for (i = 0; i < symec; i += 1) {
		Syme	syme = lib->symev[i];
		Symbol	sym = symeId(syme);
		UShort	pos = (UShort) (ULong) tblElt(tbl, sym, (TblElt)(ULong)symec);

		if (pos == symec)
			tblSetElt(tbl, sym, (TblElt) (ULong) i);
		else {
			assert(pos < i);
			BUF_PUT_HINT(buf, i);
			BUF_PUT_HINT(buf, pos);
		}
	}

	BUF_PUT_HINT(buf, symec);

	/* Write the symbol names. */

	for (i = 0; i < symec; i += 1) {
		Syme	syme = lib->symev[i];
		Symbol	sym = symeId(syme);
		UShort	pos = (UShort) (ULong) tblElt(tbl, sym, (TblElt) (ULong) symec);

		if (pos == i) {
			String	s = symeString(syme);
			Length	l = strLength(s) + 1;
			bufWrChars(buf, l, s);
		}
	}

	tblFree(tbl);

	libPutSection(lib, LIB_Name, buf);
}

local void
libPutSymeKinds(Lib lib)
{
	UShort	i, symec = lib->symec;
	Buffer	buf = libAddSection(lib, LIB_Kind);

	if (!buf) return;

	for (i = 0; i < symec; i += 1) {
		Syme	syme = lib->symev[i];
		UByte	kind = (UByte) SYME_LIMIT;

		if (libSymeIsLocal(lib, syme))
			kind = (UByte) symeKind(syme);

		BUF_PUT_BYTE(buf, kind);
	}

	libPutSection(lib, LIB_Kind, buf);
}

local void
libPutSymeFiles(Lib lib)
{
	UShort	i, symec = lib->symec;
	Buffer	buf = libAddSection(lib, LIB_File);

	if (!buf) return;

	for (i = 0; i < symec; i += 1) {
		Syme	syme = lib->symev[i];
		UByte	kind = (UByte) symeKind(syme);
		String	s = NULL;
		Length	l;

		if (!libSymeIsFile(lib, syme)) continue;

		if (symeKind(syme) == SYME_Library)
			s = libToStringStatic(symeLibrary(syme));
		if (symeKind(syme) == SYME_Archive)
			s = arToStringStatic(symeArchive(syme));

		l = strLength(s) + 1;
		BUF_PUT_HINT(buf, i);
		BUF_PUT_BYTE(buf, kind);
		bufWrChars(buf, l, s);
	}

	BUF_PUT_HINT(buf, symec);

	libPutSection(lib, LIB_File, buf);
}

local void
libPutSymeLazys(Lib lib)
{
	UShort	i, symec = lib->symec;
	Buffer	buf = libAddSection(lib, LIB_Lazy);

	if (!buf) return;

	for (i = 0; i < symec; i += 1) {
		Syme	syme = lib->symev[i], lsyme;
		UShort	n;
		Hash	hash;

		if (!libSymeIsLazy(lib, syme)) continue;

		lsyme = libLibrarySyme(symeLib(syme));
		n     = symeLibNum(lsyme);
		hash  = symeTypeCode(syme);

		assert(libCheckSymeNumber(lib, lsyme, n));

		BUF_PUT_HINT(buf, i);
		BUF_PUT_HINT(buf, n);
		BUF_PUT_SINT(buf, hash);
	}

	BUF_PUT_HINT(buf, symec);

	libPutSection(lib, LIB_Lazy, buf);
}

local void
libPutSymev(Lib lib)
{
	UShort	i, symec = lib->symec, *symecs;
	Buffer	buf = libAddSection(lib, LIB_Syme);

	if (!buf) return;

	/* Heap allocate since cannot use enum val as size for stack alloc. */
	symecs = (UShort *) stoAlloc(OB_Other, SYME_LIMIT * sizeof(*symecs));
	for (i = 0; i < SYME_LIMIT; i += 1) symecs[i] = 0;

	libCodeSort(lib);

	for (i = 0; i < symec; i += 1) {
		Syme	syme = lib->symev[i];
		UByte	b;
		UShort	s;
		ULong	l;

		symecs[symeKind(syme)] += 1;

		b = (UByte) symeKind(syme);
		if (libSymeIsLazy(lib, syme))
			b = (UByte) symeKind(symeOriginal(syme));
		if (symeIsSpecial(syme)) b |= LIB_SPECIAL_MASK;
		if (symeHasDefault(symeOriginal(syme))) b |= LIB_DEF_MASK;
		if (symeTop(syme)) b |= LIB_TOP_MASK;
		BUF_PUT_BYTE(buf, b);

		l = symeTypeCode(syme);
		BUF_PUT_SINT(buf, l);

		l = lib->symep[i];
		BUF_PUT_SINT(buf, l);

		s = lib->codev[i];
		BUF_PUT_HINT(buf, s);
		
		l = (ULong) symeConstInfo(syme);
		BUF_PUT_SINT(buf, l);

		l = symeHashNum(syme);
		BUF_PUT_SINT(buf, l);

		if (symeConstLib(syme)) {
			Syme	lsyme = libLibrarySyme(symeConstLib(syme));
			s = symeLibNum(lsyme);
			assert(libCheckSymeNumber(lib, lsyme, s));
		}
		else
			s = SYME_NUMBER_UNASSIGNED;
		BUF_PUT_HINT(buf, s);

		if (libSymeIsLocal(lib, syme) && symeExtension(syme)) {
			Syme	xsyme = symeExtension(syme);
			s = symeLibNum(xsyme);
			assert(libCheckSymeNumber(lib, xsyme, s));
			symeSetExtension(syme, NULL);
		}
		else
			s = SYME_NUMBER_UNASSIGNED;
		BUF_PUT_HINT(buf, s);
	}

	libPutSection(lib, LIB_Syme, buf);

	libLazyDEBUG{libCheckSymes(lib);}

	libVerboseDEBUG {
		fprintf(dbOut, "Number of symbols:\t\t%12d symes\n", i);
		for (i = 0; i < SYME_LIMIT; i += 1)
			fprintf(dbOut, "\t%s:\t\t%12d symes\n",
				symeInfo[i].str, symecs[i]);
	}
	stoFree((Pointer) symecs);
}

/* Check that lazy symes can be distinguished at read time. */
void
libCheckSymes(Lib lib)
{
	UShort	i, j;

	for (i = 0; i < lib->symec; i += 1) {
		Syme	syme = lib->symev[i];
		Lib	nlib = symeLib(syme);
		Symbol	sym  = symeId(syme);
		Hash	code = symeHash(syme);

		for (j = i + 1; j < lib->symec; j += 1)
			if (libSymeMatch(nlib, sym, code, lib->symev[j]) &&
			    symeOriginal(syme) != symeOriginal(lib->symev[j]) &&
			    !symeIsTwin(syme, lib->symev[j]))
				bugWarning("libCheckSymes:  '%s'",
					   symString(sym));
	}
}

local void
libPutSymeTypes(Lib lib)
{
	UShort	symec;
	ULong	typec;
	Buffer	buf = libAddSection(lib, LIB_Type);

	if (!buf) return;

	BUF_PUT_SINT(buf, lib->typec);

	for (symec = 0; symec < lib->symec; symec += 1) {
		Syme	syme = lib->symev[symec];
		TForm	tf;
		ULong	n;

		/* Only put types for local symes. */
		if (!libSymeIsLocal(lib, syme)) continue;

		if (symeIsImport(syme)) {
			tf = symeExporter(syme);
			tfFollow(tf);
		}
		else if (symeIsSelf(syme))
			tf = tfType;
		else
			tf = symeType(syme);

		n = tf->libNum;
		assert(libCheckTypeNumber(lib, tf, n));
		lib->symep[symec] = n;
	}

	for (typec = 0; typec < lib->typec; typec += 1)
		tformToBuffer(lib, buf, lib->typev[typec]);

	libPutSection(lib, LIB_Type, buf);
}

local void
libPutSymeSymes(Lib lib)
{
	UShort	i, symec = lib->symec;
	Buffer	buf = libAddSection(lib, LIB_Inline);

	if (!buf) return;

	for (i = 0; i < symec; i += 1) {
		Syme		syme = lib->symev[i];
		SymeList	symes = symeInlined(syme);

		/* Only put symes for local symes. */
		if (!libSymeIsLocal(lib, syme)) continue;

		if (symes) {
			BUF_PUT_HINT(buf, i);
			symeListToBuffer(lib, buf, symes);
		}
	}

	BUF_PUT_HINT(buf, symec);

	libPutSection(lib, LIB_Inline, buf);
}

local void
libPutSymeTwins(Lib lib)
{
	UShort	i, symec = lib->symec;
	Buffer	buf = libAddSection(lib, LIB_Twins);

	if (!buf) return;

	for (i = 0; i < symec; i += 1) {
		Syme		syme = lib->symev[i];
		SymeList	symes = symeTwins(syme);

		/* Only put symes for local symes. */
		if (!libSymeIsLocal(lib, syme)) continue;

		if (symes) {
			BUF_PUT_HINT(buf, i);
			symeListToBuffer(lib, buf, symes);
			libConstDEBUG {
				fprintf(dbOut, "(%d:", i); 
				while (symes) {
					fprintf(dbOut, " %d", symeLibNum(car(symes)));
					symes = cdr(symes);
				}
				fprintf(dbOut, ")\n");
			}
		}
	}

	BUF_PUT_HINT(buf, symec);

	libPutSection(lib, LIB_Twins, buf);
}

local void
libPutSymeExts(Lib lib)
{
	UShort	i, symec = lib->symec;
	Buffer	buf = libAddSection(lib, LIB_Extend);

	if (!buf) return;

	for (i = 0; i < symec; i += 1) {
		Syme		syme = lib->symev[i];
		SymeList	symes;

		/* Only put exts for local symes. */
		if (!libSymeIsLocal(lib, syme)) continue;
		if (!symeIsExtend(syme)) continue;

		symes = symeExtendee(syme);
		if (symes) {
			BUF_PUT_HINT(buf, i);
			symeListToBuffer(lib, buf, symes);
		}
	}

	BUF_PUT_HINT(buf, symec);

	libPutSection(lib, LIB_Extend, buf);
}

local void
libPutSymeDocs(Lib lib)
{
	UShort	i, symec = lib->symec;
	Buffer	buf = libAddSection(lib, LIB_Doc);
        Table   tbl = tblNew((TblHashFun) docHash, (TblEqFun) docEqual);

	if (!buf) return;

	for (i = 0; i < symec; i += 1) {
		Syme	syme = lib->symev[i];
		Doc	doc;

		/* Only put docs for local symes. */
		if (!libSymeIsLocal(lib, syme)) continue;
		if (!symeIsExport(syme)) continue;

		doc = symeComment(syme);

		if (doc != docNone) {
			String	s = docString(doc);
			Length	l = docLength(doc);
			Length  pos;
			
			BUF_PUT_HINT(buf, i);
			pos = (Length) tblElt(tbl, (TblKey) doc,
					      (TblElt) 0x7FFFFFFF);
			BUF_PUT_SINT(buf, pos);
			if (pos == 0x7FFFFFFF)
				bufWrChars(buf, l, s);
			tblSetElt(tbl, (TblKey) doc, (TblElt) (UAInt) i);
		}
	}

	BUF_PUT_HINT(buf, symec);
	tblFree(tbl);

	libPutSection(lib, LIB_Doc, buf);
}

/*****************************************************************************
 *
 * lib0GetSymes	- first pass for libGetSymes
 *
 ****************************************************************************/

local SymeList	lib0GetSymes		(Lib);
local void	lib0GetSymev		(Lib);
local void	lib0GetSymeNames	(Lib);
local void	lib0GetSymeKinds	(Lib);
local void	lib0GetSymeFiles	(Lib, Bool);
local void	lib0GetSymeLazys	(Lib);
local void	lib0GetSymeTriggers	(Lib);
local void	lib0GetSymeExtensions	(Lib);
local void	lib0GetSymeSymes	(Lib);
local void	lib0GetSymeTwins	(Lib);
local void	lib0GetSymeExts		(Lib);
local void	lib0GetSymeDocs		(Lib);
local void	lib0FiniSymev		(Lib);

local SymeList
lib0GetLibrarySymes(Lib lib)
{
	UShort          i;
	SymeList        libs = listNil(Syme);

	if (!lib->symes) {
		lib0GetSymeNames(lib);
		lib0GetSymeFiles(lib, true);
	}

	for (i = 0; i < lib->topc; i += 1) {
		Syme    syme = lib->symev[i];
		if (syme && symeKind(syme) == SYME_Library)
			libs = listCons(Syme)(syme, libs);
	}
	libs = listNReverse(Syme)(libs);

	if (!lib->symes)
		libClearSymes(lib);

	return libs;
}

local SymeList
lib0GetSymes(Lib lib)
{
	if (lib->symes)
		return lib->symes;

	/* Retrieve the top syme symbol names. */
	/* This pass allocates a short lib->symev. */
	lib0GetSymeNames(lib);

	/* Retrieve the syme kinds. */
	/* This pass allocates top entries in lib->symev. */
	lib0GetSymeKinds(lib);

	/* Retrieve the files. */
	/* This pass allocates SYME_Library/SYME_Archive's in lib->symev. */
	lib0GetSymeFiles(lib, false);

	/* Retrive the lazy symes from other libraries. */
	/* This pass allocates lazy entries in lib->symev. */
	lib0GetSymeLazys(lib);

	/* Retrieve the constant-sized information. */
	/* This pass fills in various fields in lib->symev[i]. */
	lib0GetSymev(lib);

	/* Use any symes already lazily created. */
	/* This pass incorporates lib->triggers into lib->symev[i]. */
	lib0GetSymeTriggers(lib);

	/* This pass incorporates extensions into lib->symev[i]. */
	lib0GetSymeExtensions(lib);

	/* Retrieve the symes inlined. */
	/* This pass sets up triggers for symeInlined(lib->symev[i]). */
	lib0GetSymeSymes(lib);

	/* Retrieve the syme twins. */
	/* This pass sets up triggers for symeTwins(lib->symev[i]). */
	lib0GetSymeTwins(lib);

	/* Retrieve the extendees. */
	/* This pass sets up triggers for symeExtendee(lib->symev[i]). */
	lib0GetSymeExts(lib);

	/* Retrieve the docs. */
	/* This pass sets up triggers for symeComment(lib->symev[i]). */
	lib0GetSymeDocs(lib);

	/* Retrieve the syme list from the vector. */
	/* This pass fills in lib->symes. */
	lib0FiniSymev(lib);

	libDEBUG {
		fprintf(dbOut, "lib0GetSymes: %s", fnameUnparse(lib->name));
		fnewline(dbOut);
		listPrint(Syme)(dbOut, lib->symes, symePrint);
	}

	libRepDEBUG{libRepSymes(dbOut, lib, true);}

	return lib->symes;
}

local void
lib0GetSymeNames(Lib lib)
{
	UShort	i, symec, topc;
	Buffer	buf = libGetSection(lib, LIB_Name, true);

	/* Allocate the short lib syme vector. */

	BUF_GET_HINT(buf, symec);
	BUF_GET_HINT(buf, topc);
	lib->topc  = topc;
	lib->symec = symec;
	lib->symev = (Syme *) stoAlloc(OB_Other, topc * sizeof(Syme)); 

	/* Read the top syme name positions. */

	for (i = 0; i < topc; i += 1)
		lib->symev[i] = (Syme) (ULong) i;

	for (libGetSymeIndex(buf, i); i < topc; libGetSymeIndex(buf, i)) {
		UShort	pos;

		BUF_GET_HINT(buf, pos);
		assert(pos < i);
		lib->symev[i] = (Syme) (ULong) pos;
	}

	for (; i < symec; libGetSymeIndex(buf, i))
		bufSkip(buf, HINT_BYTES);

	/* Read the top syme symbol names. */

	for (i = 0; i < topc; i += 1) {
		UShort	pos = (UShort) (ULong) lib->symev[i];
		String	name;

		if (pos < i)
			lib->symev[i] = lib->symev[pos];
		else {
			assert(pos == i);
			name = bufGetString(buf);
			lib->symev[i] = (Syme) symIntern(name);
			strFree(name);
		}
	}
}

local void
lib0GetSymeKinds(Lib lib)
{
	UShort	i, topc = lib->topc;
	Buffer	buf = libGetSection(lib, LIB_Kind, true);

	for (i = 0; i < topc; i += 1) {
		Symbol	sym = (Symbol) lib->symev[i];
		UByte	kind;
		Syme	syme;

		BUF_GET_BYTE(buf, kind);
		if (kind == (UByte) SYME_LIMIT) continue;

		syme = symeNewLib(kind, sym, tfUnknown, lib);
		lib->symev[i] = syme;
	}
}

local void
lib0GetSymeFiles(Lib lib, Bool wash)
{
	Stab	stab = stabFile();
	UShort	i, j, topc = lib->topc;
	Buffer	buf = libGetSection(lib, LIB_File, true);
	AIntList al = listNil(AInt);

	/* Collect the archive symes. */
	for (libGetSymeIndex(buf, i); i < topc; libGetSymeIndex(buf, i)) {
		Symbol	sym = (Symbol) lib->symev[i];
		UByte	kind;

		BUF_GET_BYTE(buf, kind);

		/* Library symes on the next pass. */
		if (kind == (UByte) SYME_Library)
			bufGets(buf);

		if (kind == (UByte) SYME_Archive) {
			String	name = bufGetString(buf);
			Syme	syme = stabGetArchive(sym);
			if (syme == NULL)
				syme = stabDefArchive(stab, sym, tfType,
						      arFrString(name));
			al = listCons(AInt)(i, al);
			assert(syme);
			lib->symev[i] = syme;
		}
	}
	al = listNReverse(AInt)(al);

	bufPosition(buf) = 0;
	j = wash ? 0 : topc;
	for (libGetSymeIndex(buf, i); i < topc; libGetSymeIndex(buf, i)) {
		Symbol	sym = (Symbol) lib->symev[i];
		UByte	kind;

		for (; j <= i; j += 1)
			lib->symev[j] = NULL;

		BUF_GET_BYTE(buf, kind);

		if (kind == (UByte) SYME_Library) {
			String	name = bufGetString(buf);
			Syme	syme = stabGetLibrary(sym);
			if (syme == NULL) {
				Lib	nlib = NULL;
				AIntList l;

				if (lib->arent)
					nlib = libFrArchive(lib->arent->ar, 
							    name);

				for (l = al; !nlib && l; l = cdr(l)) {
					Syme	asyme = lib->symev[car(l)];
					assert(symeIsArchive(asyme));
					nlib = arFindInArchive(symeArchive(asyme),
							       name);
				}

				if (!nlib) nlib = libFrString(name);
				syme = stabDefLibrary(stab, sym, tfType, nlib);
			}
			assert(syme);
			lib->symev[i] = syme;
		}

		/* Archive symes on the previous pass. */
		if (kind == (UByte) SYME_Archive)
			bufGets(buf);
	}

	for (; j < topc; j += 1)
		lib->symev[j] = NULL;

	listFree(AInt)(al);
}

local void
lib0GetSymeLazys(Lib lib)
{
	UShort	i, topc = lib->topc;
	Buffer	buf = libGetSection(lib, LIB_Lazy, true);

	for (libGetSymeIndex(buf, i); i < topc; libGetSymeIndex(buf, i)) {
		Symbol	sym = (Symbol) lib->symev[i];
		UShort	n;
		Hash	hash;
		Lib	olib;

		BUF_GET_HINT(buf, n);
		BUF_GET_SINT(buf, hash);

		olib = symeLibrary(lib->symev[n]);
		lib->symev[i] = libSymeSyme(lib, olib, sym, hash);
	}
}

local void
lib0GetSymev(Lib lib)
{
	UShort	i, topc = lib->topc;
	Buffer	buf = libGetSection(lib, LIB_Syme, true);

	for (i = 0; i < topc; i += 1) {
		Syme	syme = lib->symev[i];
		Lib     constLib;
		UByte	b;
		UShort	s;
		ULong	l, constInfo;

		if (syme->kind == SYME_Trigger) {
			bufSkip(buf, BYTE_BYTES);

			BUF_GET_SINT(buf, l);
			assert(symeHash(syme) == l);

			bufSkip(buf, 3*SINT_BYTES + 3*HINT_BYTES);
			continue;
		}

		BUF_GET_BYTE(buf, b);
		if (b & LIB_SPECIAL_MASK) symeSetSpecial(syme);
		if (b & LIB_DEF_MASK) symeSetDefault(syme);
		if (b & LIB_TOP_MASK) symeSetTop(syme);
		assert((UByte) symeKind(syme) == (b & ~LIB_BITS_MASK));

		BUF_GET_SINT(buf, l);
		symeSetHash(syme, l);

		BUF_GET_SINT(buf, l);
		if (l == TYPE_NUMBER_UNASSIGNED)
			/* Do nothing */;
		else if (symeIsImport(syme)) {
			symeSetExporter(syme, tfTrigger(syme));
			symeSetLazy(syme);
		}
		else if (symeIsExport(syme) ||
			 symeIsParam(syme) ||
			 symeIsSelf(syme))
			symeSetLazy(syme);
		else
			symeSetType(syme, tfTrigger(syme));

		/* Skip lib->codev[i]. */
		bufSkip(buf, HINT_BYTES);

		BUF_GET_SINT(buf, l);
		constInfo = l;

		BUF_GET_SINT(buf, l);
		symeSetHashNum(syme, l);

		BUF_GET_HINT(buf, s);
		if (s != SYME_NUMBER_UNASSIGNED) {
			assert(libCheckSymeNumber(lib, NULL, s));
			assert(s < topc);
			constLib = symeLibrary(lib->symev[s]);
		}
		else if (!symeConstLib(syme) && !symeIsLibrary(syme))
			constLib = lib;
		else 
			constLib = NULL;

		BUF_GET_HINT(buf, s);
		if (s != SYME_NUMBER_UNASSIGNED) {
			assert(libCheckSymeNumber(lib, NULL, s));
			assert(s < topc);
			symeSetExtension(syme, (s<<1) | 1);
		}
		if (constLib != NULL) {
#if 0
			if (symeConstLib(syme)) {
				printf("Switch: \n");
				symePrint(dbOut, syme);
				printf(" %s -> %s\n", 
				       libToString(symeConstLib(syme)),
				       libToString(constLib));
			}
#endif
			symeSetConstInfo(syme, constInfo);
			symeSetConstLib(syme,  constLib);
		}
	}
}

local void
lib0GetSymeTriggers(Lib lib)
{
	SymeList	symes;
	SymeList	later = listNil(Syme);

	for (symes = lib->triggers; symes; symes = cdr(symes)) {
		Syme	tsyme = car(symes);
		Symbol	sym = symeId(tsyme);
		Hash	code = symeHash(tsyme);
		UShort	i;
		Syme	syme = NULL;

		for (i = 0; i < lib->topc; i += 1)
			if (libSymeMatch(lib, sym, code, lib->symev[i])) {
				syme = lib->symev[i];
				break;
			}

		if (!syme) {
			listPush(Syme, tsyme, later);
			continue;
		}

		tsyme->kind	= syme->kind;
		tsyme->bits	= syme->bits;
		tsyme->type	= syme->type;
		tsyme->mask	= syme->mask;
		tsyme->fieldc	= syme->fieldc;
		tsyme->fieldv	= syme->fieldv;

		if (tfIsTriggerSyme(syme->type, syme))
			tfSetTriggerSyme(syme->type, tsyme);

		lib->symev[i] = tsyme;
		symeFree(syme);
	}
	listFree(Syme)(lib->triggers);
	lib->triggers = later;
}

local void
lib0GetSymeExtensions(Lib lib)
{
	UShort i;

	for (i=0; i<lib->topc; i++) {
		Syme syme = lib->symev[i];
		/* symeFieldExtension --- not sre why it failed...*/ 
		int ext  = symeGetFieldFn(syme, SYFI_Extension); 
		if (ext & 1)
			symeSetExtension(syme, lib->symev[ ext >> 1]);
	}
}

local void
lib0GetSymeSymes(Lib lib)
{
	UShort	i, topc = lib->topc;
	Buffer	buf = libGetSection(lib, LIB_Inline, true);

	for (libGetSymeIndex(buf, i); i < topc; libGetSymeIndex(buf, i)) {
		Syme	syme = lib->symev[i];

		symeSetFieldTrigger(syme, SYFI_Inlined);
		symeListFrBuffer0(buf);
	}
}

local void
lib0GetSymeTwins(Lib lib)
{
	UShort	i, topc = lib->topc;
	Buffer	buf = libGetSection(lib, LIB_Twins, true);

	for (libGetSymeIndex(buf, i); i < topc; libGetSymeIndex(buf, i)) {
		Syme	syme = lib->symev[i];

		symeSetFieldTrigger(syme, SYFI_Twins);
		symeListFrBuffer0(buf);
	}
}

local void
lib0GetSymeExts(Lib lib)
{
	UShort	i, topc = lib->topc;
	Buffer	buf = libGetSection(lib, LIB_Extend, true);

	for (libGetSymeIndex(buf, i); i < topc; libGetSymeIndex(buf, i)) {
		Syme	syme = lib->symev[i];

		symeSetExtendee(syme, symeListFrBuffer(lib, buf));
	}
}

local void
lib0GetSymeDocs(Lib lib)
{
	UShort	i, topc = lib->topc;
	Buffer	buf = libGetSection(lib, LIB_Doc, true);

	for (libGetSymeIndex(buf, i); i < topc; libGetSymeIndex(buf, i)) {
		Syme	syme = lib->symev[i];
		int 	dummy;
		symeSetFieldTrigger(syme, SYFI_Comment);
		BUF_GET_SINT(buf, dummy);
		if (dummy == 0x7FFFFFFF)
			bufGets(buf);
	}
}

local void
lib0FiniSymev(Lib lib)
{
	UShort	i, topc = lib->topc;

	for (i = 0; i < topc; i += 1) {
		Syme	syme = lib->symev[i];
		if (symeIsTop(syme) && symeLib(syme) == lib &&
		    !symeExtension(syme))
			lib->symes = listCons(Syme)(syme, lib->symes);
	}

	lib->symes = listNReverse(Syme)(lib->symes);
}

/*****************************************************************************
 *
 * lib1GetSymes	- second pass for libGetSymes
 *
 ****************************************************************************/

local SymeList	lib1GetSymes		(Lib);
local void	lib1GetSymev		(Lib);
local void	lib1GetSymeNames	(Lib);
local void	lib1GetSymeKinds	(Lib);
local void	lib1GetSymeLazys	(Lib);
local void	lib1GetSymeTriggers	(Lib);
local void	lib1GetSymeExtensions	(Lib);
local void	lib1GetSymeTypes	(Lib);
local void	lib1GetSymeSymes	(Lib);
local void	lib1GetSymeTwins	(Lib);
local void	lib1GetSymeExts		(Lib);
local void	lib1GetSymeDocs		(Lib);
local void	lib1GetSymeTypec	(Lib);
local void	lib1FillTypeNumbers	(Lib, int);

local SymeList
lib1GetSymes(Lib lib)
{
	if (lib->typec)
		return lib->symes;

	/* Retrieve the symbol names. */
	/* This pass allocates lib->syme[vp]. */
	lib1GetSymeNames(lib);

	/* Retrieve the syme kinds. */
	/* This pass allocates local entries in lib->symev. */
	lib1GetSymeKinds(lib);

	/* Retrive the lazy symes from other libraries. */
	/* This pass allocates lazy entries in lib->symev. */
	lib1GetSymeLazys(lib);

	/* Retrieve the constant-sized information. */
	/* This pass fills in various fields in lib->symev[i]. */
	lib1GetSymev(lib);

	/* Use any symes already lazily created. */
	/* This pass incorporates lib->triggers into lib->symev[i]. */
	lib1GetSymeTriggers(lib);

	lib1GetSymeExtensions(lib);
	/* Retrieve the types. */
	/* This pass fills in symeType/symeExporter(lib->symev[i]). */
	lib1GetSymeTypes(lib);

	/* Retrieve the symes inlined. */
	/* This pass fills in symeInlined(lib->symev[i]). */
	lib1GetSymeSymes(lib);

	/* Retrieve the syme twins. */
	/* This pass fills in symeTwins(lib->symev[i]). */
	lib1GetSymeTwins(lib);

	/* Retrieve the extendees. */
	/* This pass fills in symeExtendee(lib->symev[i]). */
	lib1GetSymeExts(lib);

	/* Retrieve the docs. */
	/* This pass fills in symeComment(lib->symev[i]). */
	lib1GetSymeDocs(lib);

	libDEBUG {
		fprintf(dbOut, "lib1GetSymes: %s", fnameUnparse(lib->name));
		fnewline(dbOut);
	}

	libConstDEBUG {
		int	i;
		Syme	syme;

		for (i=0; i< lib->symec; i++) {
			syme = lib->symev[i];
			fprintf(dbOut, "%d: %s %s %ld %s (%s %s . %ld)\n", 
				i,
				libSymeIsLocal(lib, syme) ? "O" : "Z",
				symeInfo[symeKind(syme)].str,
				syme->hash,
				syme->id->str,
				(symeLib(syme) == NULL || symeLib(syme) == lib )
				  ? "Local"
				  :  libGetFileId(symeLib(syme)),
				symeConstLib(syme) 
				  ? libGetFileId(symeConstLib(syme)) 
				  : "Local",
				symeConstNum(syme));
		}
	}

	return lib->symes;
}

local void
lib1GetSymeNames(Lib lib)
{
	UShort	i, symec, topc;
	Syme *	topv = lib->symev;
	Buffer	buf = libGetSection(lib, LIB_Name, true);

	/* Allocate the syme vectors. */

	BUF_GET_HINT(buf, symec);
	BUF_GET_HINT(buf, topc);
	lib->topc  = topc;
	lib->symec = symec;
	lib->symev = (Syme   *) stoAlloc(OB_Other, symec * sizeof(Syme)); 
	lib->symep = (ULong  *) stoAlloc(OB_Other, symec * sizeof(ULong));
	lib->codev = (UShort *) stoAlloc(OB_Other, symec * sizeof(UShort));

	/* Read the syme name positions. */

	for (i = 0; i < symec; i += 1) {
		lib->symev[i] = (Syme) (ULong) i;
		lib->symep[i] = (ULong) 0;
		lib->codev[i] = (UShort) i;
	}

	for (libGetSymeIndex(buf, i); i < symec; libGetSymeIndex(buf, i)) {
		UShort	pos;

		BUF_GET_HINT(buf, pos);
		assert(pos < i);
		lib->symev[i] = (Syme) (ULong) pos;
	}

	/* Read the syme symbol names. */

	for (i = 0; i < symec; i += 1) {
		UShort	pos = (UShort) (ULong) lib->symev[i];
		String	name;

		if (pos < i)
			lib->symev[i] = lib->symev[pos];
		else {
			assert(pos == i);
			name = bufGetString(buf);
			lib->symev[i] = (Syme) symIntern(name);
			strFree(name);
		}
	}

	/* Move the top symes into the long syme vector. */

	for (i = 0; i < topc; i += 1)
		lib->symev[i] = topv[i];
	stoFree(topv);
}

local void
lib1GetSymeKinds(Lib lib)
{
	UShort	i, symec = lib->symec, topc = lib->topc;
	Buffer	buf = libGetSection(lib, LIB_Kind, true);

	bufSkip(buf, topc);
	for (i = topc; i < symec; i += 1) {
		Symbol	sym = (Symbol) lib->symev[i];
		UByte	kind;
		Syme	syme;

		BUF_GET_BYTE(buf, kind);
		if (kind == (UByte) SYME_LIMIT) continue;

		syme = symeNewLib(kind, sym, tfUnknown, lib);
		lib->symev[i] = syme;
	}
}

local void
lib1GetSymeLazys(Lib lib)
{
	UShort	i, symec = lib->symec, topc = lib->topc;
	Buffer	buf = libGetSection(lib, LIB_Lazy, true);

	for (libGetSymeIndex(buf, i); i < symec; libGetSymeIndex(buf, i)) {
		Symbol	sym = (Symbol) lib->symev[i];
		UShort	n;
		Hash	hash;
		Lib	olib;

		if (i < topc) {
			bufSkip(buf, HINT_BYTES + SINT_BYTES);
			continue;
		}

		BUF_GET_HINT(buf, n);
		BUF_GET_SINT(buf, hash);

		olib = symeLibrary(lib->symev[n]);
		lib->symev[i] = libSymeSyme(lib, olib, sym, hash);
	}
}

local void
lib1GetSymev(Lib lib)
{
	UShort	i, symec = lib->symec;
	Buffer	buf = libGetSection(lib, LIB_Syme, true);

	for (i = 0; i < symec; i += 1) {
		Syme	syme = lib->symev[i];
		Lib     constLib;
		UByte	b;
		UShort	s;
		ULong	l, constInfo;

		if (syme->kind == SYME_Trigger) {
			bufSkip(buf, BYTE_BYTES);

			BUF_GET_SINT(buf, l);
			assert(symeHash(syme) == l);

			BUF_GET_SINT(buf, l);
			lib->symep[i] = l;

			BUF_GET_HINT(buf, s);
			lib->codev[i] = s;

			bufSkip(buf, 2*SINT_BYTES + 2*HINT_BYTES);
			continue;
		}

		BUF_GET_BYTE(buf, b);
		if (b & LIB_SPECIAL_MASK) symeSetSpecial(syme);
		if (b & LIB_DEF_MASK) symeSetDefault(syme);
		if (b & LIB_TOP_MASK) symeSetTop(syme);
		assert((UByte) symeKind(syme) == (b & ~LIB_BITS_MASK));

		BUF_GET_SINT(buf, l);
		symeSetHash(syme, l);

		BUF_GET_SINT(buf, l);
		lib->symep[i] = l;

		BUF_GET_HINT(buf, s);
		lib->codev[i] = s;

		BUF_GET_SINT(buf, l);
		constInfo = l;

		BUF_GET_SINT(buf, l);
		symeSetHashNum(syme, l);

		BUF_GET_HINT(buf, s);
		if (s != SYME_NUMBER_UNASSIGNED) {
			assert(libCheckSymeNumber(lib, NULL, s));
			constLib = symeLibrary(lib->symev[s]);
		}
		else if (!symeConstLib(syme) && !symeIsLibrary(syme))
			constLib = lib;
		else
			constLib = 0;
		BUF_GET_HINT(buf, s);
		if (s != SYME_NUMBER_UNASSIGNED) {
			assert(libCheckSymeNumber(lib, NULL, s));
			symeSetExtension(syme, (s << 1) | 1 );
		}
		
		if (constLib != NULL) {
			symeSetConstInfo(syme, constInfo);
			symeSetConstLib(syme, constLib);
		}
	}
}

local void
lib1GetSymeTriggers(Lib lib)
{
	SymeList	symes;

	for (symes = lib->triggers; symes; symes = cdr(symes)) {
		Syme	tsyme = car(symes);
		Symbol	sym = symeId(tsyme);
		Hash	code = symeHash(tsyme);
		Lib	nlib = (Lib) tsyme->type;
		UShort	i = libSymeIndex(lib, sym, code);
		Syme	syme;

		if (i >= lib->symec) {
			comsgFatal(NULL, ALDOR_F_LibOutOfDate,
				   libToStringShort(lib),
				   libToStringShort(nlib));
			return;
		}
		syme = lib->symev[i];

		assert(i >= lib->topc);
		tsyme->kind	= syme->kind;
		tsyme->bits	= syme->bits;
		tsyme->type	= syme->type;
		tsyme->mask	= syme->mask;
		tsyme->fieldc	= syme->fieldc;
		tsyme->fieldv	= syme->fieldv;

		lib->symev[i] = tsyme;
		symeFree(syme);
	}
	listFree(Syme)(lib->triggers);
	lib->triggers = listNil(Syme);
}

local void
lib1GetSymeExtensions(Lib lib)
{
	UShort i;

	for (i=0; i<lib->symec; i++) {
		Syme syme = lib->symev[i];
		Syme ext = symeExtension(syme);
		if ( ((UAInt) ext) & 1)
			symeSetExtension(syme, lib->symev[ ((UAInt) ext) >> 1]);
	}
}

local void
lib1GetSymeTypes(Lib lib)
{
	UShort	i;

	lib1GetSymeTypec(lib);

	for (i = 0; i < lib->symec; i += 1) {
		Syme	syme = lib->symev[i];
		ULong	n = lib->symep[i];

		if (n == TYPE_NUMBER_UNASSIGNED) continue;
		assert(libCheckTypeNumber(lib, NULL, n));

		if (symeIsImport(syme)) {
			if (i < lib->topc)
				tfForwardFrTrigger(symeExporter(syme),
						   libGetSymeType(lib, n));
			else
				symeSetExporter(syme, libGetSymeType(lib, n));
			symeSetLazy(syme);
		}
		else if (symeIsExport(syme) ||
			 symeIsParam(syme) ||
			 symeIsSelf(syme))
			symeSetLazy(syme);
		else {
			if (i < lib->topc)
				tfForwardFrTrigger(syme->type,
						   libGetSymeType(lib, n));
			else
				symeSetType(syme, libGetSymeType(lib, n));
		}
	}
}

local void
lib1GetSymeSymes(Lib lib)
{
	UShort	i, symec = lib->symec;
	Buffer	buf = libGetSection(lib, LIB_Inline, true);

	for (libGetSymeIndex(buf, i); i < symec; libGetSymeIndex(buf, i)) {
		Syme	syme = lib->symev[i];

		symeSetInlined(syme, symeListFrBuffer(lib, buf));
	}
}

local void
lib1GetSymeTwins(Lib lib)
{
	UShort	i, symec = lib->symec;
	Buffer	buf = libGetSection(lib, LIB_Twins, true);

	for (libGetSymeIndex(buf, i); i < symec; libGetSymeIndex(buf, i)) {
		Syme	syme = lib->symev[i];

		symeSetTwins(syme, symeListFrBuffer(lib, buf));
	}
}

local void
lib1GetSymeExts(Lib lib)
{
	UShort	i, symec = lib->symec, topc = lib->topc;
	Buffer	buf = libGetSection(lib, LIB_Extend, true);

	for (libGetSymeIndex(buf, i); i < symec; libGetSymeIndex(buf, i)) {
		Syme	syme = lib->symev[i];

		if (i < topc)
			symeListFrBuffer0(buf);
		else
			symeSetExtendee(syme, symeListFrBuffer(lib, buf));
	}
}

local void
lib1GetSymeDocs(Lib lib)
{
	UShort	i, symec = lib->symec;
	Buffer	buf = libGetSection(lib, LIB_Doc, true);

	for (libGetSymeIndex(buf, i); i < symec; libGetSymeIndex(buf, i)) {
		Syme	syme = lib->symev[i];
		Doc	doc;
		int	pos;

		BUF_GET_SINT(buf, pos);
		assert(pos < i || pos == 0x7FFFFFFF);
		if (pos == 0x7FFFFFFF)
			doc = docNewFrString(bufGetString(buf));
		else 
			doc = symeComment(lib->symev[pos]);
	
		symeSetComment(syme,doc);
	}
}

local void
lib1GetSymeTypec(Lib lib)
{
	int	i;

	if (lib->typev != 0)
		return;

	if (lib->typeb == 0)
		lib->typeb = libGetSection(lib, LIB_Type, false);

	lib->typec = tformTypecFrBuffer(lib->typeb);
	lib->typev = (TForm *) stoAlloc(OB_Other, lib->typec * sizeof(TForm));
	lib->typep = (int   *) stoAlloc(OB_Other, lib->typec * sizeof(int));

	for (i = 0; i < lib->typec; i += 1)
		lib->typev[i] = 0;

	tformTypepFrBuffer(lib->typeb, lib->typec, lib->typep);
}

local void
lib1FillTypeNumbers(Lib lib, int num)
{
	TForm		tf = lib->typev[num];
	TFormList	l;
	TQualList	ql;
	int		i;

	/* Fill in the type form arguments using the numbers stashed
	 * in the type form argv by tformFrBuffer.
	 */
	for (i = 0; i < tfArgc(tf); i += 1) {
		Length	n = (Length) tfArgv(tf)[i];
		assert(libCheckTypeNumber(lib, NULL, n));
		tfArgv(tf)[i] = libGetSymeType(lib, (int) n);
	}

	/* Fill in the has questions using the numbers stashed
	 * in the type form list by tformFrBuffer.
	 */
	for (l = tfQueries(tf); l; l = cdr(l)) {
		int	n = (UAInt) car(l);
		assert(libCheckTypeNumber(lib, NULL, n));
		setcar(l, libGetSymeType(lib, n));
	}

	/* Fill in the cascaded imports using the numbers stashed
	 * in the qualified type forms by tformFrBuffer.
	 */
	for (ql = tfCascades(tf); ql; ql = cdr(ql)) {
		TQual	tq = car(ql);
		int	n = (UAInt) tqBase(tq);
		assert(libCheckTypeNumber(lib, NULL, n));
		tqBase(tq) = libGetSymeType(lib, n);
		for (l = tqQual(tq); l; l = cdr(l)) {
			n = (UAInt) car(l);
			assert(libCheckTypeNumber(lib, NULL, n));
			setcar(l, libGetSymeType(lib, n));
		}
	}

	if (tfIsWith(tf) && tfUseCatExports(tf))
		tfHasCascades(tf) = true;

	if (tfIsThird(tf) && tfUseThdExports(tf))
		tfHasCascades(tf) = true;

	tfSetMeaning(tf);
	if (tfHasExpr(tf)) tfSetNeedsSefo(tf);
}

/*****************************************************************************
 *
 * libGetSymes
 *
 ****************************************************************************/

SymeList
libGetLibrarySymes(Lib lib)
{
	return lib0GetLibrarySymes(lib);
}

SymeList
libGetSymes(Lib lib)
{
	lib0GetSymes(lib);
	return lib->symes;
}

SymeList
libGetAllSymes(Lib lib)
{
	lib0GetSymes(lib);
	lib1GetSymes(lib);
	return lib->symes;
}

TForm
libGetSymeType(Lib lib, int num)
{
	if (lib->typev && lib->typev[num])
		return lib->typev[num];

	lib1GetSymeTypec(lib);

	if (!(0 <= num && num < lib->typec))
		return 0;

	bufPosition(lib->typeb) = lib->typep[num];
	lib->typev[num] = tformFrBuffer(lib, lib->typeb);
	lib1FillTypeNumbers(lib, num);

	return lib->typev[num];
}

/*****************************************************************************
 *
 * :: Layout of foam section
 *
 ****************************************************************************/

/*
 * Ensure that one of lib->unitb or lib->unit is non-zero by
 * extracting it from the library.
 */
void
libGetFoamUnitBuffer(Lib lib)
{
	if (lib->unitb == 0 && lib->unit == 0)
		lib->unitb = libGetSection(lib, LIB_Foam, false);
}

/*
 * Ensure that lib->unitb is non-zero by extracting it from the library.
 */
void
libGetUnitBuffer(Lib lib)
{
	if (lib->unitb == 0)
		lib->unitb = libGetSection(lib, LIB_Foam, false);
}

/*
 * If we can ensure that lib->constc is non-zero by extracting it
 * from the foam unit from the library, return true.  Otherwise return false.
 */
local void
libGetFoamConstc(Lib lib)
{
	int	i;

	if (lib->constv != 0)
		return;

	libGetFoamUnitBuffer(lib);

	if (lib->unitb)
		lib->constc = foamConstcFrBuffer(lib->unitb);
	else
		lib->constc = foamArgc(foamUnitConstants(lib->unit));

	lib->constv = (Foam *) stoAlloc(OB_Other, lib->constc * sizeof(Foam));
	lib->constp = (int  *) stoAlloc(OB_Other, lib->constc * sizeof(int));

	for (i = 0; i < lib->constc; i += 1)
		lib->constv[i] = 0;

	if (lib->unitb)
		foamConstvFrBuffer(lib->unitb, lib->constc, lib->constp);
	else
		foamConstvFrFoam(lib->unit, lib->constc, lib->constv);
}

/*
 * External entry points.
 */

void
libPutMacros(Lib lib, AbSyn macseq)
{
	Buffer	buf = libAddSection(lib, LIB_Macros);

	if (buf) {
		abToBuffer(buf, macseq);
		libPutSection(lib, LIB_Macros, buf);
	}
}

Foam
libPutFoam(Lib lib, Foam foam)
{
	Buffer	buf = libAddSection(lib, LIB_Foam);

	if (buf) {
		foamToBuffer(buf, foam);
		libPutSection(lib, LIB_Foam, buf);
	}
	return foam;
}

void
libPutPos(Lib lib, Foam foam)
{
	Buffer	bfp = libAddSection(lib, LIB_Pos);

	if (bfp) {
		foamPosToBuffer(bfp, foam);
		libPutSection(lib, LIB_Pos, bfp);
	}
	libPutPosTbl(lib);
}

local void
libPutPosTbl(Lib lib)
{
	Buffer	buf = libAddSection(lib, LIB_PosTbl);
	if (buf) {
		sposTableToBuffer(buf);
		libPutSection(lib, LIB_PosTbl, buf);
	}
}

void
libPutFoamSymes(Lib lib, Foam foam)
{
	Foam		*fmtv, *declv, dfmt, decl;
	UShort		fmtc, declc, i, j, n;
	Syme		syme;
	Buffer		buf = libAddSection(lib, LIB_FoamSyme);

	if (!buf) return;

	assert(foamTag(foam) == FOAM_Unit);
	dfmt = foam->foamUnit.formats;

	assert(foamTag(dfmt) == FOAM_DFmt);
	fmtv = dfmt->foamDFmt.argv;
	fmtc = foamArgc(dfmt);

	for (i = 1; i < fmtc; i += 1) {
		assert(foamTag(fmtv[i]) == FOAM_DDecl);
		declv = fmtv[i]->foamDDecl.argv;
		declc = foamDDeclArgc(fmtv[i]);

		for (j = 0; j < declc; j += 1) {
			decl = declv[j];
			assert(foamIsDecl(decl));
			syme = foamSyme(decl);
			n = syme ? symeLibNum(syme) : SYME_NUMBER_UNASSIGNED;
			BUF_PUT_HINT(buf, n);
		}
	}

	libPutSection(lib, LIB_FoamSyme, buf);
}

AbSyn
libGetMacros(Lib lib)
{
	AbSyn  seq;
	Buffer buf = libGetSection(lib, LIB_Macros, true);
	
	if (lib->macros)
		return lib->macros;

	seq = abFrBuffer(buf);
	lib->macros = seq;
	return seq;
}

Foam
libGetFoam(Lib lib)
{
	libGetFoamUnitBuffer(lib);

	if (lib->unitb) {
		lib->unit = foamFrBuffer(lib->unitb);
		bufFree(lib->unitb);
		lib->unitb = 0;
	}
	if (lib->postbl == NULL) libGetPosTbl(lib);
	if (lib->pos == NULL) libGetPos(lib, lib->unit);
	return lib->unit;
}

void
libGetPos(Lib lib, Foam foam)
{
	lib->pos = libGetSection(lib, LIB_Pos, true);
	if (lib->pos) {
		foamPosFrBuffer(lib->pos, foam);
		lib->pos = 0;
	}
}

local void
libGetPosTbl(Lib lib)
{
	lib->postbl = libGetSection(lib, LIB_PosTbl, true);
	if (lib->postbl) {
		sposTableFrBuffer(lib->postbl);
		lib->postbl = 0;
	}
}

void
libGetFoamSymes(Lib lib)
{
	Foam		*fmtv, *declv, dfmt, decl;
	UShort		fmtc, declc, i, j, n;
	Buffer		buf = libGetSection(lib, LIB_FoamSyme, true);

	dfmt = lib->formats;
	assert(foamTag(dfmt) == FOAM_DFmt);
	fmtv = dfmt->foamDFmt.argv;
	fmtc = foamArgc(dfmt);

	for (i = 1; i < fmtc; i += 1) {
		assert(foamTag(fmtv[i]) == FOAM_DDecl);
		declv = fmtv[i]->foamDDecl.argv;
		declc = foamDDeclArgc(fmtv[i]);

		for (j = 0; j < declc; j += 1) {
			decl = declv[j];
			assert(foamIsDecl(decl));
			BUF_GET_HINT(buf, n);
			if (n != SYME_NUMBER_UNASSIGNED)
				foamSyme(decl) = lib->symev[n];
		}
	}
}

Foam
libGetFoamFormats(Lib lib)
{
	if (lib->formats)
		return lib->formats;

	libGetFoamUnitBuffer(lib);

	if (lib->unitb)
		lib->formats = foamFormatsFrBuffer(lib->unitb);
	else
		lib->formats = lib->unit->foamUnit.formats;

	libGetAllSymes(lib);
	libGetFoamSymes(lib);

	return lib->formats;
}

Syme
libGetFormatSyme(Lib lib, AInt fmt, AInt index)
{
	Foam	ddecl, decl;

	libGetFoamFormats(lib);

	assert(foamTag(lib->formats) == FOAM_DFmt);
	assert(fmt < foamArgc(lib->formats));
	ddecl = lib->formats->foamDFmt.argv[fmt];

	assert(foamTag(ddecl) == FOAM_DDecl);
	assert(index < foamDDeclArgc(ddecl));
	decl = ddecl->foamDDecl.argv[index];

	assert(foamIsDecl(decl));
	return foamSyme(decl);
}

/* Find the foam for a constant given the constant number. */
Foam
libGetFoamConstant(Lib lib, int num)
{
	libGetFoamConstc(lib);

	if (!(0 <= num && num < lib->constc))
		return 0;

	if (lib->unitb && lib->constv[num] == 0)
		lib->constv[num] =
			foamConstFrBuffer(lib->unitb, lib->constp[num]);

	return lib->constv[num];
}

Foam
libGetProgHdr(Lib lib, int num)
{
	Foam prog;

	libGetFoamConstc(lib);

	if (!(0 <= num && num < lib->constc))
		return 0;

	if (lib->unitb && lib->constv[num] == 0)
		return foamGetProgHdrFrBuffer(lib->unitb, lib->constp[num]);
	else {
		if (foamTag(lib->constv[num]) != FOAM_Prog) return NULL;

		prog = foamCopyNode(lib->constv[num]);

		prog->foamProg.locals = NULL;
		prog->foamProg.params = NULL;
		prog->foamProg.fluids = NULL;
		prog->foamProg.levels = NULL;
		prog->foamProg.body = NULL;

		return prog;
	}
}

Bool
libValidateSymes(SymeList symes)
{
	/* Do all the symes have a valid libNum field? */
	Syme		syme;
	SymeList	sl;
	UShort		lnum;

	for (sl = symes; sl; sl = cdr(sl)) {
		syme = car(sl);
		lnum = symeLibNum(syme);
		if (lnum == SYME_NUMBER_UNASSIGNED) return false;
	}

	return true;
}
