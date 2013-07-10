/*****************************************************************************
 *
 * lib.h: Operations for manipulating compiler output libraries.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _LIB_H_
#define _LIB_H_

#include "axlobs.h"

/*****************************************************************************
 *
 * :: Library sections
 *
 ****************************************************************************/

/*
 * Section headers describing each section of the library header file
 * are kept in the library file header.  The enumeration LibSectName
 * provides an encoding of the name of each section.
 */

enum libSectName {
    LIB_NAME_START,
	LIB_Syme = LIB_NAME_START,	/* Symbol table. */
	LIB_Foam,			/* Foam code. */
	LIB_FoamSyme,			/* Foam decl symes. */
	LIB_Pos,			/* Foam positions. */
	LIB_PosTbl,			/* Table of positions. */
	LIB_Name,			/* Symbol names. */
	LIB_Kind,			/* Syme kinds. */
	LIB_File,			/* File names. */
	LIB_Lazy,			/* Lazy symes. */
	LIB_Type,			/* Type forms. */
	LIB_Inline,			/* Inlined syme lists. */
	LIB_Twins,			/* Syme twins. */
	LIB_Extend,			/* Extendee syme lists. */
	LIB_Doc,			/* Documentation. */
	LIB_Id,				/* Name of the library. */
        LIB_Macros,			/* Macros exported */
    LIB_NAME_LIMIT
};

typedef Enum(libSectName)  LibSectName;

struct libSect {
	BPack(LibSectName) name;	/* What is stored in this section. */
	Offset		   offset;	/* Offset of section from offset0. */
	Offset		   length;	/* Length of section in bytes. */
};

/*
 * libSectInfo contains information about each type of section.
 */
struct _libSectInfo {
        LibSectName  	tag;
        String          str;
	String		abbrev;
};

extern struct _libSectInfo libSectInfoTable[];

#define	libSectInfo(x)	(libSectInfoTable[(x)-LIB_NAME_START])

DECLARE_LIST(Lib);

/*****************************************************************************
 *
 * :: Library header
 *
 ****************************************************************************/

#define LIB_INDEX_START LIB_NAME_START
#define LIB_INDEX_LIMIT LIB_NAME_LIMIT

#define LIB_HDR_LIMIT	20      /* >= LIB_NAME_LIMIT */

struct libHdr {
	UShort	magic;		/* Aldor library file magic number. */
	ULong	verMajor;	/* Library format major version number. */
	ULong	verMinor;	/* Library format minor version number. */
	UShort	numSect;	/* Number of sections in this header. */

	struct libSect	Section[LIB_HDR_LIMIT];  /* index->section map */
	UShort		Index  [LIB_HDR_LIMIT];  /* name ->index map */
};

/*****************************************************************************
 *
 * :: Library file contents
 *
 ****************************************************************************/

struct lib {
	FileName	name;
	ArEntry		arent;
	BPack(Bool)	rdOnly;
	BPack(Bool)	intLoaded;	/* Already loaded by interpreter? */
	String		idName;		/* Name of initialiser */
	FILE *		file;
	Offset		offset;		/* Offset of hdr in file. */
	Syme		self;		/* Library syme for this lib. */
	Stab		stab;		/* Stab for symes, tforms. */

	/* Cached symbol meanings. */
	UShort		topc;		/* Number of top-level symes. */
	UShort		symec;		/* Number of symbol meanings. */
	Syme *		symev;		/* Symbol meaning section. */
	SymeList	symes;		/* Exported symbol meanings. */
	ULong *		symep;		/* Positions of type forms. */
	UShort *	codev;		/* Symes sorted by hash code. */
	SymeList	triggers;	/* Symes used to avoid lib read. */

	/* Cached type forms. */
	ULong		typec;		/* Number of type forms. */
	TForm *		typev;		/* Type form section. */
	TFormList	types;		/* Types used in symes. */
	int *		typep;		/* Positions of type forms. */
	Buffer		typeb;		/* Type form section. */

	/* Cached foam code. */
	Length		constc;		/* Number of constants. */
	Foam *		constv;		/* Foam unit constants. */
	int *		constp;		/* Positions of constants. */
	Buffer		unitb;		/* Linear foam unit. */
	Buffer		pos;		/* Linear foam position. */
	Buffer		postbl;		/* Linear position table. */
	Foam		unit;		/* Expanded foam unit. */
	Foam		formats;	/* Foam decl formats. */
	/* Cached macros */
	AbSyn		macros;

	struct libHdr	hdr;
};

/*****************************************************************************
 *
 * :: Library Statistics
 *
 ****************************************************************************/

struct libStats {
	Length		libSize;
	Length		sectSize[LIB_HDR_LIMIT];
};

typedef struct libStats	*LibStats;

extern Lib	libNew			(FileName, Bool, FILE *, Offset);
extern Lib	libExtract		(FileName, FILE *, Offset);
extern Lib	libRead			(FileName);
extern Lib	libWrite		(FileName);
extern void	libClose		(Lib);
extern Bool	libEqual		(Lib, Lib);
extern Hash	libHash			(Lib);

extern LibStats	libStats		(FileName, LibStats);
extern LibStats libStatsClear		(LibStats);
extern LibStats libStatsIncrement	(LibStats, LibStats);

extern  Lib	libFrString		(String);
#define		libToString(lib)	fnameUnparse((lib)->name)
#define		libToStringStatic(lib)	fnameUnparseStaticWithout((lib)->name)
#define		libToStringShort(lib)	fnameName((lib)->name)

extern SymeList	libPutSymes		(Lib, SymeList, Foam);
extern SymeList	libGetSymes		(Lib);
extern SymeList	libGetAllSymes		(Lib);
extern TForm	libGetSymeType		(Lib, int);
extern ULong	libSymeTypeNo		(Lib, Symbol, Hash);
extern Syme	libLibrarySyme		(Lib);
extern SymeList	libGetLibrarySymes	(Lib);
extern Bool	libIsBasicLib		(Lib);
extern Bool	libSymeIsTop		(Lib, Syme);

extern Foam	libPutFoam		(Lib, Foam);
extern void	libPutFoamSymes		(Lib, Foam);
extern void	libPutPos		(Lib, Foam);
extern Foam	libGetFoam		(Lib);
extern void	libGetPos		(Lib, Foam);
extern Foam	libGetFoamFormats	(Lib);
extern Syme	libGetFormatSyme	(Lib, AInt, AInt);
extern Foam	libGetFoamConstant	(Lib, int);
extern Foam	libGetProgHdr		(Lib, int);

extern void	libPutMacros		(Lib, AbSyn);
extern AbSyn	libGetMacros		(Lib);
extern void	libPutFileId		(Lib, String);
extern String	libGetFileId		(Lib);
extern Lib	libGetHeader		(Lib);
extern void	libGetFoamUnitBuffer	(Lib);
extern void	libGetUnitBuffer	(Lib);

extern Bool	libHasSyme		(Lib, Syme);
extern Bool	libCheckSymeNumber	(Lib, Syme, UShort);
extern Bool	libCheckTypeNumber	(Lib, TForm, ULong);

extern StringList	libDependencies(FileName);

extern Bool	libValidateSymes	(SymeList);
#endif /* !_LIB_H_ */
