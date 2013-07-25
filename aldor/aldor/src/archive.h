/*****************************************************************************
 *
 * archive.h: Operations for manipulating compiler library archives.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _ARCHIVE_H_
#define _ARCHIVE_H_

#include "axlobs.h"
#include "path.h"

/*****************************************************************************
 *
 * :: Archive file lists
 *
 ****************************************************************************/

extern void	arInit			(String * files, String * keys);
extern void	arFileInit		(FileName, String);

extern PathList arLibraryFiles		(void);
extern PathList arLibraryKeys		(void);

extern void	arAddLibraryFile	(String);
extern void	arAddLibraryKey		(String);

/*****************************************************************************
 *
 * :: Archive file formats
 *
 ****************************************************************************/

enum arFmtTag {
    AR_START,
	AR_Arch = AR_START,
	AR_AIX,
	AR_CMS,
        AR_AIX4,
    AR_LIMIT
};

typedef Enum(arFmtTag)		ArFmtTag;

struct ar_info {
	ArFmtTag		tag;
	String			str;
	Offset			align;
	Offset			hdrsz;
};

extern struct ar_info		arInfoTable[];

#define arInfo(tag)		arInfoTable[(tag) - AB_START]

/*****************************************************************************
 *
 * :: Archive structures
 *
 ****************************************************************************/

DECLARE_LIST(ArEntry);

struct ar_entry {
	String		name;		/* Member name. */
	Archive		ar;		/* Containing archive. */
	Offset		pos;		/* Member position. */
	Lib		lib;		/* Library for the member. */
	BPack(Bool)	mark;		/* Filter mark. */
};

struct archive {
	FileName	name;		/* File path name. */
	BPack(Bool)	hasFile;	/* Have we opened the stream? */
	BPack(Bool)	hasIntermed;	/* Do we contain an ao file? */
	FILE *		file;		/* Stream. */

	ArFmtTag	format;		/* Archive file format. */
	Offset		size;		/* Archive file length. */

	String		item;		/* Name of the mru item. */
	Offset		pos;		/* Position of the mru item data. */
	Offset		__next;		/* Position of the next item hdr. */

	ArEntryList	members;	/* Archive members. */
	SymeList	symes;		/* List of symes. */

	String		names;		/* Name table */
};

/******************************************************************************
 *
 * :: Basic operations
 *
 *****************************************************************************/

#define		arHasIntermed(ar)	((ar)->hasIntermed)
#define		arFile(ar)		((ar)->file)
#define		arSize(ar)		((ar)->size)
#define		arItem(ar)		((ar)->item)
#define		arPosition(ar)		((ar)->pos)

extern Archive	arRead			(FileName);
extern void	arClose			(Archive);
extern Bool	arEqual			(Archive, Archive);

extern Archive	arFrString		(String);
#define		arToString(ar)		fnameUnparse((ar)->name)
#define		arToStringStatic(ar)	fnameUnparseStaticWithout((ar)->name)

extern Lib	arFind			(PathList, String);
extern Lib 	arFindInArchive		(Archive,  String);
extern SymeList arGetLibrarySymes	(Archive);
extern SymeList arGetSymes		(Archive);
extern Syme	arLibrarySyme		(Archive, Syme);
extern Bool	arLibraryIsMember	(Archive, Lib);
extern Bool	arHasBasicLib		(Archive);
extern void	arUseExpandedReplacement(void);
extern AbSyn    arGetGlobalMacros	(Archive);

#endif /* !_ARCHIVE_H_ */
