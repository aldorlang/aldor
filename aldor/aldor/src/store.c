/*****************************************************************************
 *
 * store.c: Storage management.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

/*
 *   Select one of:
 *	STO_USE_BTREE	B-Tree based quick fit.
 *	STO_USE_ONCE	Don't look back.
 *	STO_USE_MALLOC	Based on malloc/free.
 *
 *
 *   For STO_USE_BTREE, select
 *   any of:
 *	STO_TALLY	Keep storage use statistics.
 *	STO_LONGJMP	Error handler can return when StoErr_CantBuild occurs.
 *	STO_WATCH	Trace allocations and deallocations.
 *	STO_CERTIFY	Trace tests controlling if statements, etc.
 *      USE_MEMORY_CLIMATE  To over-ride the setting of stoAlloc codes.
 *   one of:
 *	STO_NEW_JUNK	Newly allocated store contains any old junk.
 *	STO_NEW_AAAA	Newly allocated store contains 0xAAAAAA...
 *	STO_NEW_ZERO	Newly allocated store contains zeros.
 *   and one of:
 *	STO_FREE_JUNK	Free storage contains any old junk.
 *	STO_FREE_DDDD	Free storage contains 0xDDDDDD...
 *
 * For debugging support define STO_DEBUG_DISPLAY. This will cause the
 * garbage collector to check for the GC_DETAIL and GC_CLASSIFY environment
 * variables when the first GC takes place.
 *
 * The value of GC_DETAIL is used to control the output of stoShow(). The
 * existence of GC_CLASSIFY determines whether or not tables showing the
 * classification of pointers are displayed.
 *
 * By default blacklisting is not enabled since the current implementation
 * doesn't have much effect (for good or ill). Define STO_CAN_BLACKLIST to
 * enable it and then set the GC_BLACKLIST environment variable to actually
 * use it at runtime.
 *
 * To speed up the computation of indices of quanta, fixed-size sections
 * whose size is not an integral power of two we use division via a lookup
 * table. Enable this by defining STO_DIVISION_BY_LOOKUP and suffer a
 * penalty of a 48K table in static data.
 */

#define _BSD_SOURCE 1 /* strncasecmp */

#include "debug.h"
#include "opsys.h"
#include "store.h"
#include "timer.h"

/*
 * If no other allocator is specified, used the B-Tree based by default.
 */
#if !defined(STO_USE_BTREE)&& !defined(STO_USE_ONCE)&& !defined(STO_USE_MALLOC) && !defined(STO_USE_BOEHM)
#  if __APPLE__
#   define STO_USE_ONCE 1
#  elif 0 && !defined(FOAM_RTS)
#    define STO_USE_MALLOC
#  else
#    define STO_USE_BTREE
#  endif
#endif

#include "axlgen0.h"
#include "btree.h"
#include "memclim.h"
#include "util.h"

/* this will create a debug version of GC that should be safer */
#undef NDEBUG

/*
 * User programs can make limited checks on pointers to ensure
 * that they do not attempt to write to areas of memory that
 * are writable. Although we could give the user a much more
 * precise result (such as this object is a free object on the
 * heap) we choose not to.
 */
#define POINTER_IS_INVALID		(-1)
#define POINTER_IS_UNKNOWN		( 0)
#define POINTER_IS_VALID		( 1)


/*===========================================================================*/

#ifdef STO_USE_BTREE

#if defined(OS_WIN32)
#include <windows.h>
#endif


/*
 * B-Tree based allocator.
 *
 *
 * At the coarsest level, memory has "page" granularity.
 * Memory is used in contiguous sequences of pages, called "sections".
 *
 * The pages of this memory manager need not bear any relation to those
 * of the operating system.  If the definitions coincide, however, then
 * virtual memory performace should be better.
 *
 * Some pages contain pieces which must all be the same size.  These pieces
 * are called "fixed" pieces.  Other pages do not dictate the size of the
 * pieces they contain.	 These pieces are called "mixed".  There are also
 * other types of pages for foreign inclusions and structures used by the
 * allocator.  A page map keeps track of how each page is used.
 *
 * A section of one or more pages is layed out as follows:
 * +-----------------------+--- - - - - - - - ---+-----------------------+
 * | first page		   |			 | last page		 |
 * +----++++ - ++++-----+--+--+ - - - - - - - +--+--+--+--+ - - +--+--+--+
 * |head| info	  | gap | data						 |
 * +----++++ - ++++-----+--+--+ - - - - - - - +--+--+--+--+ - - +--+--+--+
 * Note that the data quanta are aligned at the last page boundary.
 *
 * Keeping info separate from data requires less space but more time.
 * Space is saved since no padding is needed to align the data pointer.
 * Finding the info for a piece takes more time, though.
 *
 * Compared to having a single tag table, keeping a tag table per section
 * requires more memory access for tag lookup.	But a single tag table must be
 * relocated when memory grows and will be much larger if pages are sparse.
 *
 * The fixed pieces which are not in use are kept in free lists, one for each
 * fixed size.
 *
 * The handling of free mixed pieces is somewhat more complicated.
 * The complications are introduced by the competing criteria that requests
 * be satisfied quickly, and that storage be used as efficiently as possible.
 *
 * To use storage efficiently, fragmentaition must be avoided.	Three steps
 * toward achieving this goal are
 * (1) when a mixed piece is freed, it is merged with any adjacent mixed free
 *     pieces to form a larger piece,
 * (2) requests are satisfied using a "best fit" strategy, and
 * (3) among best fits, the one with the lowest address is used.
 *
 * List-based implementations of best fit are slow so to achieve the speed
 * objective, the free pieces are sorted by size in a B-tree.
 * The keys into the B-tree are the sizes and the entries are doubly linked
 * lists of same-size pieces.
 *
 * Using doubly linked lists means that the tree need be modified only when
 * a new size is introduced or removed.	 This benefit is enhanced by rounding
 * all mixed pieces up to a multiple of some size.
 */

/*
 * To do:
 * -- When freeing the last piece in a section, return sect to free page list.
 * -- Robustify btreeX functions to check for 0 return from newX.
 * -- Audits for BTree node and DLL node pages
 * -- Sort DLLs by address
 * -- Think about using SLLs instead of DLLs
 * -- Use osFree when possible
 * -- Ask for only as many pages as needed at once.
 * -- Modify mixedFrontier to be truly busy.
 * -- If adjacent mixed pieces are gc-ed, merge before adding to BTree.
 * -- Merge returned free pieces to mixed frontier, if possible.
 * -- Increase qmSize for sections with larger pieces.
 * -- Encode run lengths in PgInfo to speed up sectFor.
 * -- Relocation based on pointer certainty.
 * -- Remove setjmps for stoAllocInner_ErrorCatch by ensuring enough to start.
 */

/*****************************************************************************
 *
 * :: Parameters
 *
 *****************************************************************************/

#define LgPgSize	  12	/* Log[2](PgSize). pagesTest ok for 3..15. */

#define PgSize (1L<<LgPgSize)	/* Granularity for OS request. */

#define PgGroup		  16	/* Get this many pages at once from OS. */

#define FixedSizePgGroup   1	/* This many pages at once for fxmem. */

#define FixedSizeCount	   (sizeof(fixedSize)/sizeof(fixedSize[0]))
				/* Number of different fixed small sizes. */

#define FixedSizeMax	   (32*sizeof(Pointer))
				/* Maximum fixed small size. */

#define STO_DIVISION_BY_LOOKUP	1 
#define STO_SECT_AND_PAGE_OF	1 

static Length fixedSize[] = {
	1  * sizeof(Pointer),
	2  * sizeof(Pointer),
	3  * sizeof(Pointer),
	4  * sizeof(Pointer),
	6  * sizeof(Pointer),
	8  * sizeof(Pointer),
	10 * sizeof(Pointer),
	12 * sizeof(Pointer),
	16 * sizeof(Pointer),
	20 * sizeof(Pointer),
	24 * sizeof(Pointer),
	FixedSizeMax
};				/* Fixed sizes for small allocations. */

static int fixedSizeLog[] = {
	0, 1, -1, 2, -2, 3, -3, -4, 4, -5, -6, 5 
       /*-1,-1,-1, -1,-1, -1, -1,-1, -1, -1, -1*/
};


#ifdef FOAM_RTS
/*
 * Ensure that these names agree with the CensusType enumeration in
 * foam_c.h otherwise the results of stoTakeCensus will be meaningless.
 */
static char *censusName[] = {
	"Unknown",
	"Bogus",
	"BInt",
	"DFlo",
	"Closure",
	"Record",
	"RawRecord",
	"DynFormat",
	"Char[]",
	"Bool[]",
	"Byte[]",
	"HInt[]",
	"SInt[]",
	"SFlo[]",
	"DFlo[]",
	"Word[]",
	"Ptr[]",
	"Trailing[]",
	"EnvInfo",
	"EnvLevel",
	"Fluid",
	"GlobalInfo",
	"SaveState",
	"Total",
};
#else
/*
 * Ensure that these names agree with the OB_* definitions in axlgen.h
 * and axlobs.h otherwise the results of stoTakeCensus will be meaningless.
 */
static char *censusName[] = {
	"Unknown",
	"Bogus",
	"BInt",
	"BTree",
	"Bitv",
	"Buffer",
	"List",
	"String",
	"Symbol",
	"Table",
	"DNF",
	"CCode",
	"SExpr",
	"CoMsg",
	"SrcLine",
	"Token",
	"Doc",
	"AbSyn",
	"AbBind",
	"Stab",
	"Syme",
	"TForm",
	"TPoss",
	"TConst",
	"TQual",
	"Foam",
	"Lib",
	"Archive",
	"Total",
};
#endif


/* We can't access the CensusType enumeration so we guess the limit */
#define	STO_CENSUS_LIMIT	(sizeof(censusName)/sizeof(censusName[0]))
static ULong	censusBefore[STO_CENSUS_LIMIT];
static ULong	censusAfter[STO_CENSUS_LIMIT];
static ULong	censusMemBefore[STO_CENSUS_LIMIT];
static ULong	censusMemAfter[STO_CENSUS_LIMIT];


#ifdef STO_DIVISION_BY_LOOKUP
static short stoDivTable[6][PgSize];
#endif


static UShort fixedSizeFor     [FixedSizeMax+1];
				/* Look up size to use. */
static UByte  fixedSizeIndexFor[FixedSizeMax+1];
				/* Index into size table. */

#define MixedBTreeT	 16	/* Min #branches for btree interior node. */
#define MixedSizePgGroup  2	/* At least this many pgs at once for mxmem. */
#define MixedSizeQuantum FixedSizeMax
				/* All mixed sizes are a multiple of this. */

/*   - flo-po banished coz we don't want to risk exceptions in the runtime*/
/* double GcEffectiveFactor = 0.3;    Get more pages unless this fraction free. */
long GcEffectiveFactorNum=3;
long GcEffectiveFactorDen=10;
/* double GcGrowthFactor    = 1.4;    Grow the heap by this much if a Gc fails.*/
long GcGrowthFactorNum=14;
long GcGrowthFactorDen=10;
long   GcMinGrowth       = (512*1024)/PgSize; /* Grow by a minumum of 0.5meg */

long GcFrugalFactorNum = 70;
long GcFrugalFactorDen = 100;


/* Set GC_FRUGAL to enable more reticent heap growth */
static Bool GcIsFrugal = false;


/* Object information.
 * Maybe consider using bitmap for these... 
 * Note that in reality we only have 32 objects (28 of which are taken
 * by the compiler for internal data structures).
 */
#define OB_MAX 256

/*****************************************************************************/
/***          These MUST be the same as their cousins in foam_c.h          ***/
/*****************************************************************************/
typedef Pointer (*StoFiFun)();

typedef struct _StoFiProg
{
	StoFiFun fun;
	StoFiFun fcall;
	Pointer  progInfo;
	Pointer  data;
} *StoFiProg;

typedef struct _StoFiClos
{
	Pointer env;
	StoFiProg prog;
} *StoFiClos;

#define stoFiCFun(t, fn)	(*((t (*)())(fn)->prog->fun))
#define stoFiCCall2(t,fn,a,b)	(stoFiCFun(t,fn)((fn)->env,a,b))
/*****************************************************************************/

static char stoObRegistered[OB_MAX];
static char stoObNoInternalPtrs[OB_MAX];
static StoFiClos stoObAldorTracer[OB_MAX];
static StoTraceFun stoObCTracer[OB_MAX];

/*****************************************************************************
 *
 * :: Controls
 *
 *****************************************************************************/

static Bool	gcLevel		= StoCtl_GcLevel_Automatic;
static FILE	*gcTraceFile	= 0;
static Bool	stoMustWash	= true;
static Bool	stoMustTag      = true;

static Bool     markingStats    = false;

#ifdef STO_DEBUG_DISPLAY
# define stoDebug		true
#else
# define stoDebug		false
#endif

/*****************************************************************************
 *
 * :: Benchmarks
 *
 *****************************************************************************/

static struct tmTimer gcTimer;

/*****************************************************************************
 *
 * :: Types
 *
 *****************************************************************************/

enum PgKind {
		PgFree,		/* Page is available. */
		PgBusyFirst,	/* First of related busy pages. */
		PgBusyFollow,	/* Subsequent related busy pages. */
		PgPgMap,	/* Page used by pgMap. */
		PgBTree,	/* Page used by free tree as node. */
		PgDLL,		/* Page used by free tree carrier. */
		PgForeign	/* Page is not ours. */
};

typedef char		Page[PgSize];	/* Type for striding across pages. */
typedef UByte		PgInfo;		/* Type for compact page map info. */
typedef UByte		QmInfo;		/* Type for compact quantum info. */

typedef enum   PgKind	PgKind;		/* Page is available, busy, etc. */

typedef struct Section	Section;	/* A section of busy pages. */
typedef struct FxMem	FxMem;		/* Ptr to fixed pcs of a given size. */
typedef struct MxMem	MxMem;		/* Ptr to mixed pcs of a given size. */


/*****************************************************************************
 *
 * :: Forward Declarations
 *
 *****************************************************************************/

local  MostAlignedType *stoDefaultError	 (int errnum);
static StoErrorFun	stoError	 = stoDefaultError;

local Bool stoNeedsMoreHeadroom(int);
local Length sectQmCount(Length, Length);

extern void stoTakeCensus(AInt);

/*****************************************************************************
 *
 * :: Conditional Code
 *
 *****************************************************************************/

/*
 * Set debugging configuration, if appropriate.
 */
#ifndef NDEBUG
#  define STO_TALLY

#  if !defined(STO_NEW_JUNK) && !defined(STO_NEW_ZERO)
#    define STO_NEW_AAAA
#  endif

#  if !defined(STO_FREE_JUNK)
#    define STO_FREE_DDDD
#  endif
#endif


/*
 * Conditional code to fill and test memory.
 */
#if defined(STO_NEW_ZERO)
#  define STO_NEW_CHAR	0x00
#endif
#if defined(STO_NEW_AAAA)
#  define STO_NEW_CHAR	0xAA
#endif
#if defined(STO_FREE_DDDD)
#  define STO_FREE_CHAR 0xDD
#endif

#if defined(STO_NEW_CHAR)
# define newFill(s,n)		(stoMustWash ? memlset (s,STO_NEW_CHAR,n) : 0)
#else
# define newFill(s,n)		Nothing
#endif

#if defined(STO_FREE_CHAR)
# define freeFill(s,n)		(stoMustWash ? memlset (s,STO_FREE_CHAR,n) : 0)
# define freeAssert(s,n)	(stoMustWash ? memltest(s,STO_FREE_CHAR,n) : 0)
#else
# define freeFill(s,n)		Nothing
# define freeAssert(s,n)	Nothing
#endif


/*
 * Conditional code to keep track of storage.
 */
#define STO_TALLY
#ifdef STO_TALLY
# define stoTally(expr)		(expr)
#else
# define stoTally(expr)		Nothing
#endif


/*
 * Conditional code to control tracing.
 */


#ifdef STO_WATCH
#  define stoWatch(s,p,n,f)	stoWatchReally(s,p,n,f)
#else
#  define stoWatch(s,p,n,f)	Nothing
#endif

#ifdef STO_CERTIFY
#  define stoCertify(n,c)	stoCertifyReally((n), (c) ? true : false)
#else
#  define stoCertify(n,c)	(c)
#endif

#define IF(c)			if    (stoCertify(__LINE__,c))
#define WHILE(c)		while (stoCertify(__LINE__,c))

/*****************************************************************************
 *
 * :: Storage manager state
 *
 *****************************************************************************/

static int	stoIsInit  = 0; /* Is the storage manager initialized? */

static char	*heapStart,	/* Start of allocator area. */
		*heapEnd;	/* Past end of allocator area. */

static PgInfo	*pgMap;		/* Info for each page we care about. */
static Length	pgMapSize;	/* Number of pages we care about. */
static Length	pgMapUses;	/* Space occupied by page map. */

static FxMem	*fixedPieces[FixedSizeCount];
static BTree	mixedPieces;
static MxMem	*mixedFrontier;

ULong	       stoBytesOwn;	 /* Total owned by allocator. */
ULong	       stoBytesAlloc;	 /* Total ever allocated. */
ULong	       stoBytesFree;	 /* Total ever freed. */
ULong	       stoBytesGc;	 /* Total ever garbage collected. */
ULong	       stoBytesBlack;	 /* Total blacklisted */
ULong	       stoPiecesGc[STO_CODE_LIMIT];	/* # this time, by code. */


/*
 * freeFixedPieces[i] is the number of free pieces in fixedPieces[i]
 * busyFixedPieces[i] is the number of busy pieces of that size.
 * freeMixedBytes is the number of bytes held in mixed-sized free-tree.
 * busyMixedBytes is the number of bytes held in mixed-size busy pieces.
 */
static ULong	freeFixedPieces[FixedSizeCount];
static ULong	busyFixedPieces[FixedSizeCount];
static ULong	freeMixedBytes, busyMixedBytes;

/****************************************************************************
 *
 * :: Debugging
 *
 ***************************************************************************/

#define stoWatchAlloc(p,n)	stoWatch("Alloc   ",(p), (n), true)
#define stoWatchFree(p)		stoWatch("Free    ",(p), 0,   true)
#define stoWatchFrontier(p)	Nothing
#define stoWatchMarkFrom(p)	stoWatch("Mark fr ",(p), 0,   false)
#define stoWatchMarkTo(p)       stoWatch("Mark to ",(p), 0,   false)


static int stoWatchCount = 0;

int
stoWatchReally(String title, Pointer p, ULong n, Bool audit)
{
	const char *fmt = n ? "[[%4d %s: %p (%lu)]]\n" : "[[%4d %s: %p]]\n";

	fprintf(osStderr, fmt, stoWatchCount, title, p, n);
	fflush (osStderr);

	if (audit) {
		stoAudit();
	}

	stoWatchCount++;
	return 0;
}

Bool
stoCertifyReally(int loc, Bool flag)
{
	fprintf(osStderr, " %d-%s", loc, flag ? "T" : "F");
	return flag;
}

TmTimer
stoGcTimer(void)
{
	return &gcTimer;
}

/****************************************************************************
 *
 * :: Basic allocation
 *
 ***************************************************************************/

/*
 * Obtain an aligned area from the operating system.
 */
local Pointer
byteGetIfCan(Length alignment, ULong nbytes, ULong *pnbytesGot)
{
	Pointer p;

	osAllocAlignHint(alignment);

	nbytes += alignment;
	p	= osAlloc(&nbytes);

	if (p != 0) {
		freeFill(p, nbytes);
		stoBytesOwn += nbytes;
	}

	/*
	 * Round up appropriately.
	 */
	if (p != 0) {
		long	plong, rem;

		plong  = ptrToLong(p);
		rem    = plong % alignment;
		plong += (rem == 0) ? 0 : alignment - rem;
		nbytes-= rem;

		p = ptrFrLong(plong);
	}

	if (pnbytesGot) *pnbytesGot = nbytes;

	return p;
}



/*****************************************************************************
 *
 * :: Page map
 *
 * Modifies globals: pgMap, pgMapSize, pgMapUses
 * Uses globals:     heapStart, heapEnd
 *
 ****************************************************************************/

# define PgMask		~(PgSize-1)
#ifdef STO_SECT_AND_PAGE_OF
# define pgOf(p)	(ptrOff(heapStart, (ptrDiff(p, heapStart) & PgMask)))
#endif
# define pgAt(n)	((Page *)ptrOff(heapStart, (n) * PgSize))
# define pgNo(p)	(ptrDiff((char *)(p), heapStart) >> LgPgSize)
# define pgLen(nb)	((Length) (((nb) + PgSize - 1) >> LgPgSize))
# define isInHeap(p)	(ptrLE(heapStart,(p)) && ptrLT((p),heapEnd))


local Page *	pagesFind(Length npages);
local void	pagesPut (Page *, Length npages);

/*
 * Obtain and free storage for page map.
 */
local PgInfo *
pgmapAlloc(Length npgMapUses)
{
	PgInfo	*npgMap = 0;
	if (pgMap)
		npgMap = (PgInfo *) pagesFind(npgMapUses);
	if (!npgMap)
		npgMap = (PgInfo *) byteGetIfCan(PgSize,PgSize*npgMapUses,NULL);
	return npgMap;
}

local void
pgmapFree(PgInfo *opgMap, Length opgMapUses)
{
	char	*b, *e;
	b = (char *) opgMap;
	e = b + PgSize * opgMapUses - 1;

	if (isInHeap(b) && isInHeap(e))
		pagesPut((Page *) opgMap, opgMapUses);
}

/*
 * Create the initial page map.
 */
local void
pgmapInit(void)
{
	pgMap = 0;

	pgMap = pgmapAlloc(1);
	if (!pgMap) return;

	pgMapUses = 1;
	pgMapSize = 1;
}

/*
 * Update the page map for count pages starting at p.
 */
local void
pgmapMod(Page *p, Length count, PgKind kind)
{
	PgInfo	*m = pgMap + pgNo(p);
	while (count--) *m++ = kind;
}

/*
 * Search for the first free page sequence long enough. Failure is -1.
 */
int pgmapFoundFreeLastTime=0 ;

local int
pgmapFindFree(Length count)
{
	int	i, i0, iL;

	/* look after previous find */
	for (i = pgmapFoundFreeLastTime ; i < pgMapSize; i++)
		if (pgMap[i] == PgFree) {
			i0 = i;
			iL = i + count;
			if (iL > pgMapSize) return -1;
			for ( ; i < iL; i++) {
				if (pgMap[i] != PgFree) break;
				}
			if (i == iL) {
				pgmapFoundFreeLastTime = i0;
				return i0;
				}
		}
	/* now look at the bits we missed */
	for (i = 0 ; i < pgmapFoundFreeLastTime ; i++)
		if (pgMap[i] == PgFree) {
			i0 = i;
			iL = i + count;
			if (iL > pgMapSize) return -1;
			for ( ; i < iL; i++) {
				if (pgMap[i] != PgFree) break;
				}
			if (i == iL) {
				pgmapFoundFreeLastTime = i0;
				return i0;
				}
		}
	pgmapFoundFreeLastTime =0 ;
	return -1;
}

/*
 * Count the number of free pages.
 */
local int
pgmapCountFree(void)
{
	int	i, nfree;

	nfree = 0;
	for (i = 0; i < pgMapSize; i++) if (pgMap[i] == PgFree) nfree++;
	return nfree;
}

/*
 * Count the number of non-foreign pages.
 */
local int
pgmapCountDomestic(void)
{
	int	i, ndom;

	ndom = 0;
	for (i = 0; i < pgMapSize; i++) if (pgMap[i] != PgForeign) ndom++;
	return ndom;
}

/*
 * Guarantee page map is large enough.
 */
local int
pgmapNeed(Length count)
{
	PgInfo	 *opgMap    = pgMap,	 *npgMap;
	Length	 opgMapUses = pgMapUses, npgMapUses = pgLen(count);

	if (opgMapUses >= npgMapUses) return 1;

	npgMap = pgmapAlloc(npgMapUses);
	if (!npgMap) return 0;

	memcpy(npgMap, opgMap, pgMapSize * sizeof(PgInfo));

	pgMap	  = (PgInfo*) npgMap;
	pgMapUses = npgMapUses;

	pgmapFree(opgMap, opgMapUses);

	return 1;
}

/*
 * Slide page map to insert extra pages at beginning.
 */
local int
pgmapSlide(Length count, PgKind kind)
{
	PgInfo	*oend, *nend;
	Length	i;

	if (count == 0) return 1;

	if (!pgmapNeed(pgMapSize + count)) return 0;

	oend = pgMap + pgMapSize;
	nend = oend  + count;

	for (i = 0; i < pgMapSize; i++) *--nend = *--oend;
	for (i = 0; i < count;	   i++) *--nend = kind;

	pgMapSize += count;

	return 1;
}

/*
 * Extend page map with extra pages at the end.
 */
local int
pgmapExtend(Length count, PgKind kind)
{
	PgInfo	*oend;
	Length	i;

	if (!pgmapNeed(pgMapSize + count)) return 0;;

	oend = pgMap + pgMapSize;

	for (i = 0; i < count; i++) *oend++ = kind;

	pgMapSize += count;

	return 1;
}


/*****************************************************************************
 *
 * :: Heap page management
 *
 * Modifies globals: heapStart, heapEnd
 *
 ****************************************************************************/

/*
 * Add a stretch of free pages.
 * The page map and heap limits are updated accordingly.
 * A return value of 1 indicates success; 0 indicates failure.
 *
 * The page map may or may not include info about its own pages.
 * If it does, then they may be noted as PgForeign or PgPgMap.
 */
local int
pagesAdd(Length nBest, Length nMin)
{
	ULong	nbytesGot, nRequest, nForeign;
	char	*b, *e;
	int	ok;

	/* Request ideal ammount; back off until minimum. */
	nRequest  = (nBest > PgGroup) ? nBest : PgGroup;
	for (;;) {
		b = (char *) byteGetIfCan(PgSize, nRequest*PgSize, &nbytesGot);
		if (nbytesGot % PgSize != 0) nbytesGot -= nbytesGot % PgSize;

		e = (char *) ptrOff(b,nbytesGot);
		if (b) break;
		if (nRequest <= nMin) return 0;
		nRequest = (nRequest >= 2*nMin) ? nRequest / 2 : nRequest - 1;
	}

	nRequest = nbytesGot >> LgPgSize;
	nForeign = ptrGE(b, heapEnd)   ? pgLen(ptrDiff(b, heapEnd))
		 : ptrLE(e, heapStart) ? pgLen(ptrDiff(heapStart, e))
		 : 0 ;

	/* Update page map. */
	if (ptrGE(b, heapEnd)) {
		/* New store is above. */
		heapEnd = e;
		ok = pgmapExtend(nRequest + nForeign, PgForeign);
		if (!ok) return 0;
	}
	else if (ptrLE(e, heapStart)) {
		/* New store is below. */
		heapStart = b;
		ok = pgmapSlide(nRequest + nForeign,  PgForeign);
		if (!ok) return 0;
	}
	else {
		/* New store is in middle (formerly foreign). */
	}

	/* Add pages to heap. */
	pagesPut((Page *) b, nRequest);

	/* If heap now includes page map, indicate it. */
	b = (char *) pgMap;
	e = (char *) ptrOff(b, pgMapUses * PgSize);
	if (isInHeap(b) && isInHeap(ptrOff(e,-1)))
		pgmapMod((Page *) pgMap, pgMapUses, PgPgMap);

	return 1;
}

local Page *
pagesFind(Length npages)
{
	int i = pgmapFindFree(npages);
/*         if (osGetEnv("GC_FINDFREE")!=NULL) { fprintf(osStderr, "Page Found %ld\n",i);}; */
	return (i == -1) ? 0 : pgAt(i);
}

local Page *
pagesGet(Length nMin)
{
	Page	*p;
	Bool	addAnyway = false;
	Length	nBest = nMin;

	p = pagesFind(nMin);

	if (p) {
		if (nMin > 0) pgmapMod(p,   1,      PgBusyFirst);
		if (nMin > 1) pgmapMod(p+1, nMin-1, PgBusyFollow);
	}
	else if (gcLevel == StoCtl_GcLevel_Automatic) {
		int	tot, free0, free1;
		int	gceLhs, gceRhs;

		tot   = pgmapCountDomestic();
		free0 = pgmapCountFree();
		stoGc();
		free1 = pgmapCountFree();


		/* Are there sufficient free pages for this request? */
		addAnyway = (nMin > free1);


		/* If there are enough, is the headroom big enough? */
		if (!addAnyway)
		{
			/* Frugal or normal heap growth? */
			if (GcIsFrugal)
				addAnyway = stoNeedsMoreHeadroom(free1);
			else
			{
				gceLhs = GcEffectiveFactorNum * tot;
				gceRhs = GcEffectiveFactorDen * free1;
				addAnyway = (gceLhs > gceRhs);
			}
		}

		if (addAnyway &&
			(GcGrowthFactorNum * tot) > (GcGrowthFactorDen*(tot + nMin)))
		{
			nBest = (Length) ((GcGrowthFactorNum - GcGrowthFactorDen ) * tot /GcGrowthFactorDen  + 1);
			nBest = nBest < GcMinGrowth ? GcMinGrowth : nBest;
		}
		
		p = pagesFind(nMin);
		if (p) {
			if (nMin > 0) pgmapMod(p,   1,      PgBusyFirst);
			if (nMin > 1) pgmapMod(p+1, nMin-1, PgBusyFollow);
		}

		if (gcTraceFile) {
			fprintf(gcTraceFile, " [GC: %s enough, %d/%d -> %d/%d for " LENGTH_FMT "]\n",
				p ? "!!! Got" : "... Not", free0, tot, free1, tot, nMin);

			if (addAnyway) {
				fprintf(gcTraceFile, "\n [GC: Growing heap (not enough %s)]\n",
					(nMin > free1) ? "free pages" : "headroom");
			}
		}
#ifdef FOAM_RTS 
		if (markingStats) {
			fprintf(osStderr, " [GC: %s enough, %d/%d -> %d/%d for " LENGTH_FMT "]\n",
				p ? "!!! Got" : "... Not", free0, tot, free1, tot, nMin);

			if (addAnyway) {
				fprintf(osStderr, "\n [GC: Growing heap (not enough %s)]\n",
					(nMin > free1) ? "free pages" : "headroom");
			}
		}
#endif 
	}
	if (!p || addAnyway)
		pagesAdd(nBest, nMin);
	if (!p) {
		p = pagesFind(nMin);
		if (p) {
			if (nMin > 0) pgmapMod(p,   1,      PgBusyFirst);
			if (nMin > 1) pgmapMod(p+1, nMin-1, PgBusyFollow);
		}
	}

	return p;
}

local void
pagesPut(Page *pg, Length count)
{
	pgmapMod  (pg, count, PgFree);
	freeFill(pg, count*PgSize);
}

/*****************************************************************************
 *
 * :: Section manipulation
 *
 ****************************************************************************/

struct Section {
	short		pgCount;	/* Number of pages in section. */
	short		qmLog;		/* base 2 log of size, if integral */
#ifdef STO_DIVISION_BY_LOOKUP
	UByte		qmDiv;		/* division table if !qmLog */
#endif
	short		qmSize;		/* Alloc quantum. */
	UByte		qmSizeIndex;	/* Index of quantum in size table. */
	BPack(Bool)	isFixed;	/* Alloc of same or different sizes? */

	FxMem		*free;		/* Section free list (for stoTune). */
	ULong		qmCount;	/* Number of quanta of data. */
	Pointer		data;		/* Pointer to start of data. */
	QmInfo		info[NARY];	/* Per quantum information. */
};

#define SectionHeadSize fullsizeof(Section, 0, QmInfo)

/*
 * Macros for maintaining the info array:
 *   bits 0-4: object code
 *   bit    5: mark bit
 *   bits 6-7: kind of page
 */
#define QmKindMask	0xC0
#define	  QmFollow	0x00
#define	  QmFreeFirst	0x40
#define	  QmBusyFirst	0x80
#define   QmBlacklisted	0xC0

#define QmMarkMask	0x20
#define QmCodeMask	0x1F

#define QmInfoMake(kind, code)	(((kind) & QmKindMask) | ((code) & QmCodeMask))
#define QmInfoMake0(kind)	(kind)
#define QmInfoKind(info)	((info)	 & QmKindMask)
#define QmInfoCode(info)	((info)	 & QmCodeMask)
#define QmInfoMark(info)	((info)	 & QmMarkMask)

#define QmInfoSetKind(info,kind)((info) = \
	((info) & (~QmKindMask & 0xFF)) | ((kind) & QmKindMask))
#define QmInfoSetCode(info,code)((info) = \
	((info) & (~QmCodeMask & 0xFF)) | ((code) & QmCodeMask))
#define QmInfoSetMark(info)	((info) |=  QmMarkMask)
#define QmInfoClearMark(info)	((info) &= (~QmMarkMask & 0xFF))

#define QmIsPtrFree(info)	(stoObNoInternalPtrs[QmInfoCode(info)])
#define QmAldorTraced(info)	(stoObAldorTracer[QmInfoCode(info)])
#define QmCTraced(info)		(stoObCTracer[QmInfoCode(info)])


/*
 * Find the section into which p points.  0 indicates error. If you know
 * that the section is the first (eg for fixed-size pages) then sectOf(p)
 * is the most efficient method to use rather than sectAt(pgNo(p)).
 */
#ifdef STO_SECT_AND_PAGE_OF
#define sectOf(p)  ((Section *)pgOf(p))
#else
#define sectOf(p)  sectFor(p)
#endif
#define sectAt(n)  ((Section *) pgAt(n))
#define sectFor(p) (pgMap[pgNo(p)]==PgBusyFirst ? sectAt(pgNo(p)):_sectFor(p))

/*
 * Given a pointer and a section, find the index into the info array.
 */
#define qmNo(p,sect) (ptrDiff((char*)(p),(char*)((sect)->data))/(sect)->qmSize)

#define qmLogNo(p, sect) (ptrDiff((char*)(p),(char*)((sect)->data)) >> (sect)->qmLog)


/* For sections with fixed-sized pages, this is more efficient: */
#ifdef STO_DIVISION_BY_LOOKUP
#define qmDivNo(p, sect) stoDivTable[(sect)->qmDiv][(ptrDiff((char*)(p),(char*)((sect)->data)))]
#endif


/*
 * How many quanta will fit in a section with this many pages?
 */
local Length
sectQmCount(Length pageCount, Length qmSize)
{
	return (pageCount * PgSize - SectionHeadSize)/(qmSize + sizeof(QmInfo));
}

/*
 * Initialize the stretch of pages.
 */
local Section *
sectPrepare(Page *p, Length npages, Length sz, int isFixed)
{
	Section *x     = (Section *) p;
	Length	szixix, i, nq;
	int  lgWordSize;

	lgWordSize = 0;
	for (i = sizeof(Pointer); i > 1; i = i>>1) lgWordSize++;
	assert(npages < (1<<16));
	szixix	       = (sz <= FixedSizeMax) ? sz : 0;
	x->pgCount     = npages;
	x->qmSize      = sz;
	x->qmSizeIndex = fixedSizeIndexFor[szixix];
#ifdef STO_DIVISION_BY_LOOKUP
	x->qmLog       = fixedSizeLog[x->qmSizeIndex] < 0 
		? 0 : fixedSizeLog[x->qmSizeIndex] + lgWordSize;
	x->qmDiv       = x->qmLog ? 0 : -(fixedSizeLog[x->qmSizeIndex]+1);
#else
	x->qmLog       = fixedSizeLog[x->qmSizeIndex] < 0 
		? 0 : fixedSizeLog[x->qmSizeIndex] + lgWordSize;
#endif
	x->isFixed     = isFixed;
	x->qmCount     = nq = sectQmCount(npages, sz);
	x->data	       = ptrOff((char *) p, npages * PgSize - nq * sz);

	/*
	 * Fixed section is a whole bunch of little pieces.
	 * Mixed section contains one big one.
	 */
	if (stoMustTag) {
		x->info[0] = QmInfoMake0(QmFreeFirst);
		if (isFixed)
			for (i = 1; i < nq; i++)
				x->info[i] = QmInfoMake0(QmFreeFirst);
		else
			for (i = 1; i < nq; i++)
				x->info[i] = QmInfoMake0(QmFollow);
	}
	return x;
}

local Section *
_sectFor(Pointer p)
{
	Length	pi;

	if (!isInHeap(p))
		return 0;
	pi = pgNo(p);
	while (pgMap[pi] == PgBusyFollow)
		pi--;
	if (pgMap[pi] == PgBusyFirst)
		return sectAt(pi);
	return 0;
}

/*****************************************************************************
 *
 * :: Fixed piece management
 *
 ****************************************************************************/

struct FxMem {
	struct FxMem	*next;
};

# define fxmemCleanHead(pc) \
	freeFill  ((UByte *)(pc), sizeof(FxMem))
# define fxmemCleanBody(pc, sz) \
	freeFill  ((UByte *)(pc) + sizeof(FxMem), (sz) - sizeof(FxMem))
# define fxmemAssertCleanBody(pc, sz) \
	freeAssert((UByte *)(pc) + sizeof(FxMem), (sz) - sizeof(FxMem))


local FxMem *
piecesGetFixed(Length nbytes)
{
	Page		*pages;
	Section		*sect;
	FxMem		*pieces, *pieces0;
	Length		sz, i, npages, npieces;

	npages	= FixedSizePgGroup;
	sz	= fixedSizeFor[nbytes];
	pages	= pagesGet(npages);
	if (!pages) return 0;

	sect	= sectPrepare(pages, npages, sz, true);

	npieces = sect->qmCount;
	pieces0 = (FxMem *) sect->data;

	for (pieces = pieces0, i = 1; i < npieces; i++, pieces = pieces->next)
		pieces->next = (FxMem *) ptrOff((char *) pieces, sz);
	pieces->next = fixedPieces[fixedSizeIndexFor[nbytes]];

	return pieces0;
}

/*****************************************************************************
 *
 * :: Mixed piece management
 *
 ****************************************************************************/

typedef struct MxMemDLL {
	ULong	nbytes;
	MxMem	*pieces;
} MxMemDLL;

typedef union MxMemU {
	struct {
		MxMem		*linkA, *linkB;
		MxMemDLL	*dll;
	} free;
	struct {
		MostAlignedType	 data;
	} busy;
} MxMemU;


struct MxMem {
	ULong		nbytesPrev;
	ULong		nbytesThis;
	BPack(Bool)	isFree;
	BPack(Bool)	isFirst;
	BPack(Bool)	isLast;
	Section		*sect;

	/* Gap */
	union MxMemU	body;
};

#define MxMemHeadSize	(sizeof(MxMem) - sizeof(MxMemU))

#define mxmemNext(p) ((p)->isLast ? 0 : (MxMem *)ptrOff((char *)(p), (long)(p)->nbytesThis))
#define mxmemPrev(p) ((p)->isFirst? 0 : (MxMem *)ptrOff((char *)(p),-(long)(p)->nbytesPrev))


# define mxmemCleanHead0(pc) \
	freeFill  ((UByte *)(pc), sizeof(MxMem))
# define mxmemCleanHead(pc) \
	freeFill  ((UByte *)(pc) + MxMemHeadSize, sizeof(MxMem) - MxMemHeadSize)
# define mxmemCleanBody(pc, sz) \
	freeFill  ((UByte *)(pc) + sizeof(MxMem), (sz) - sizeof(MxMem))
# define mxmemAssertCleanBody(pc, sz) \
	freeAssert((UByte *)(pc) + sizeof(MxMem), (sz) - sizeof(MxMem))


#ifdef STO_LONGJMP
static jmp_buf	stoAllocInner_ErrorCatch;
static Pointer	stoAllocInner_ErrorValue;
#endif

local FxMem *
stoAllocInner(ULong nbytes, PgKind pgkind)
{
	int	i, npcs;
	ULong	npages;
	Page	*pages;
	FxMem	*pcs0, *pcs;

	npages	= 1;
	assert(nbytes <= npages*PgSize);
	npcs  = (npages*PgSize)/nbytes;
	pages = pagesGet(npages);

	if (pages == 0) {
#ifdef STO_LONGJMP
		stoAllocInner_ErrorValue = (*stoError)(StoErr_CantBuild);
		longjmp(stoAllocInner_ErrorCatch, int0);
#else
		(*stoError)(StoErr_CantBuild);
		NotReached(;);
#endif
	}

	pgmapMod(pages, npages, pgkind);

	pcs = pcs0 = (FxMem *) pages;
	for (i = 1; i < npcs; i++)
		pcs = pcs->next = (FxMem *) ptrCanon((char *) pcs+nbytes);
	pcs->next = 0;
	return pcs0;
}

static FxMem	*btreeNodes = 0;
static FxMem	*mxmemDLLs  = 0;

BTree
mxmemAllocBTree(ULong nbytes)
{
	BTree	b;

	if (!btreeNodes) btreeNodes = stoAllocInner(nbytes, PgBTree);
	b = (BTree) btreeNodes;
	btreeNodes = btreeNodes->next;
	return b;
}

void
mxmemFreeBTree(BTree b)
{
	FxMem	*p = (FxMem *) b;
	p->next	   = btreeNodes;
	btreeNodes = p;
}

MxMemDLL *
mxmemAllocDLL(void)
{
	MxMemDLL *p;

	if (!mxmemDLLs) mxmemDLLs = stoAllocInner((ULong) sizeof(*p), PgDLL);
	p = (MxMemDLL *) mxmemDLLs;
	mxmemDLLs = mxmemDLLs->next;
	return p;
}

void
mxmemFreeDLL(MxMemDLL *m)
{
	FxMem	*p = (FxMem *) m;
	p->next	   = mxmemDLLs;
	mxmemDLLs  = p;
}

# define mxmemUnlinkFromBTree(mi, pbt) {				\
	MxMemDLL *dll = mxmemUnlink(mi);				\
	IF(!dll->pieces) {						\
		btreeDeleteX((pbt), dll->nbytes, NULL, mxmemFreeBTree);	\
		mxmemFreeDLL(dll);					\
	}								\
  }

local MxMemDLL *
mxmemUnlink(MxMem *mi)
{
	MxMemDLL *dll = mi->body.free.dll;
	MxMem	 *A   = mi->body.free.linkA,
		 *B   = mi->body.free.linkB;

	/* Remove from doubly linked list. */
	IF (A)  A->body.free.linkB = B;
	IF (B)  B->body.free.linkA = A;

	/* Adjust pointer to the list.	Make null if no siblings. */
	IF (ptrEQ(mi, dll->pieces)) 
		dll->pieces = A ? A : B;

	return dll;
}

/*
 * Link a mixed-size piece into the B-tree.
 */
local void
mxmemLink(MxMem *mi)
{
	BTree		bnode;
	int		bix;

	mi    = (MxMem *) ptrCanon(mi); /* For < comp  */
	bnode = btreeSearchEQ(mixedPieces, mi->nbytesThis, &bix);

	IF (bnode) {
		MxMemDLL *dll = (MxMemDLL *) btreeElt(bnode, bix);
		MxMem	 *u, *v;

		/*!! Sorting by address can give bad paging with these DLLs. */
		u = dll->pieces;
		v = u->body.free.linkA;
		WHILE (mi > u && v) { u = v; v = u->body.free.linkA; }

		mi->body.free.dll	  = dll;
		mi->body.free.linkA	  = v;
		mi->body.free.linkB	  = u;
		u->body.free.linkA	  = mi;
		IF (v) v->body.free.linkB = mi;
	}
	else {
#if 1
		MxMemDLL *dll = mxmemAllocDLL();

		dll->nbytes   = mi->nbytesThis;
		dll->pieces   = mi;
		btreeInsertX(&mixedPieces,
			     (BTreeKey) dll->nbytes, (BTreeElt) dll,
			     mxmemAllocBTree);

		mi->body.free.dll   = dll;
		mi->body.free.linkA = 0;
		mi->body.free.linkB = 0;
#else
		MxMemDLL *dll;
		mi->body.free.linkA = 0;
		mi->body.free.linkB = 0;
		dll = mxmemAllocDLL();

		dll->nbytes   = mi->nbytesThis;
		dll->pieces   = mi;
		btreeInsertX(&mixedPieces,
			     (BTreeKey) dll->nbytes, (BTreeElt) dll,
			     mxmemAllocBTree);

		mi->body.free.dll   = dll;

#endif
	}
}

/*
 * Merge two adjacent pieces.
 */
local void
mxmemMerge(MxMem *curr, MxMem *next)
{
	MxMem	*N	     = mxmemNext(next);

	curr->nbytesThis    += next->nbytesThis;
	curr->isLast	     = next->isLast;
	IF (N) N->nbytesPrev = curr->nbytesThis;

	if (stoMustTag) {
		QmInfo *pqm = next->sect->info + qmNo(next,next->sect);
		QmInfoSetKind(*pqm, QmFollow);
	}
	mxmemCleanHead0(next);
}

/*
 * Return the remainder, where curr keeps only nbytes.
 * nbytes must be a multiple of MixedSizeQuantum.
 */
local MxMem *
mxmemSplit(MxMem *curr, ULong nbytes)
{
	MxMem	*r	     = (MxMem *) ptrOff((char *)curr, nbytes);
	MxMem	*N	     = mxmemNext(curr);

	r->isFree	     = curr->isFree;
	r->isFirst	     = false;
	r->isLast	     = curr->isLast;
	r->nbytesThis	     = curr->nbytesThis - nbytes;
	r->nbytesPrev	     = nbytes;
	r->sect		     = curr->sect;

	curr->isLast	     = false;
	curr->nbytesThis     = nbytes;

	IF (N) N->nbytesPrev = r->nbytesThis;

	if (stoMustTag) {
		QmInfo *pqm = curr->sect->info + qmNo(r,curr->sect);
		QmInfoSetKind(*pqm, QmFreeFirst);
	}
	return r;
}

local MxMemDLL *
mxmemShow(MxMemDLL *dll)
{
	int	n;
	MxMem	*t;

	/* Make t point to A-most piece. */
	t = dll->pieces;
	while (t->body.free.linkA)
		t = t->body.free.linkA;
	/* Count the pieces */
	for (n = 0; t; n++) {
		if (t->nbytesThis != dll->nbytes)
			fprintf(osStderr, "(%ld)", t->nbytesThis);
		t = t->body.free.linkB;
	}
	fprintf(osStderr," %dx%ld", n, dll->nbytes);

	return dll;
}

void
piecePutMixed(MxMem *mi)
{
	MxMem		*prev, *next;

	if (!mixedPieces) mixedPieces = btreeNewX(MixedBTreeT, mxmemAllocBTree);


	/* 1. Determine which adjacent pieces will be merged. */
	prev = mxmemPrev(mi);
	IF (prev && !prev->isFree) prev = 0;
	next = mxmemNext(mi);
	IF (next && !next->isFree) next = 0;

	/* 2. Remove those pieces from the piece tree and merge. */
	IF (next) {
		mxmemUnlinkFromBTree(next, &mixedPieces);
		mxmemMerge(mi, next);
	}

	IF (prev) {
		mxmemUnlinkFromBTree(prev, &mixedPieces);
		mxmemMerge(prev, mi);
		mi = prev;
	}

	/* 3. Add piece to piece tree. */
	mxmemLink(mi);

	/* 4. We're free */
	mi->isFree = true;

}

/*
 * Get the best piece with size at least nbytes from the mixed piece pool.
 * nbytes is guaranteed to be a multiple of MixedSizeQuantum.
 *
 * Keeping mixedFrontier out of mixedPieces btree avoids one btreeDelete
 * and one btreeInsert each time it is used.
 */

#define shdSplit1(nhas, nreq) ((nhas) > (nreq) + MixedSizeQuantum)
#define shdSplit2(nhas,nreq)  ((nhas) > (nreq) + MixedSizeQuantum)
#define shdBe1(nhas,nreq)     (nreq)
#define shdBe2(nhas,nreq)     (nreq)

MxMem *
pieceGetMixed(ULong nbytes)
{
	BTree		bnode, bnode1;
	int		bix, bix1;
	MxMem		*mi, *mt, *tmp;
	MxMemDLL	*dll;
	Bool		is1;
	ULong		mn;

	if (!mixedPieces) mixedPieces = btreeNewX(MixedBTreeT, mxmemAllocBTree);


	/* First check in mixed-size pieces free tree. */
	bnode = btreeSearchGE(mixedPieces, nbytes, &bix);
	IF (bnode) {
		mi	   = ((MxMemDLL *) btreeElt(bnode, bix))->pieces;
		dll	   = mxmemUnlink(mi);
		is1	   = !dll->pieces;
		mi->isFree = false;

		mn = mi->nbytesThis;
		IF (shdSplit1(mn, nbytes)) {
			ULong r = mi->nbytesThis - nbytes;

			mt = mxmemSplit(mi,shdBe1(mn, nbytes));
			mt->isFree = true;
			bnode1 = !is1 ? 0 : btreeSearchGE(mixedPieces,r,&bix1);

			IF (!is1) {
				piecePutMixed(mt);
			}
			else IF (bnode1 != bnode || bix1 != bix) {
				btreeDeleteX(&mixedPieces, dll->nbytes, NULL,
					     mxmemFreeBTree);
				mxmemFreeDLL(dll);
				piecePutMixed(mt);
			}
			else {
				/* Reuse btree entry. */
				btreeKey(bnode1, bix1) = r;
				dll->nbytes = r;
				dll->pieces = mt;
				mt->body.free.linkA = mt->body.free.linkB = 0;
				mt->body.free.dll   = dll;
			}
		}
		else IF (is1) {
			btreeDeleteX(&mixedPieces, dll->nbytes, NULL,
				     mxmemFreeBTree);
			mxmemFreeDLL(dll);
		}
	}
	else {
		/* no piece in mixed-size free tree is big enough. */
		IF (mixedFrontier && mixedFrontier->nbytesThis < nbytes) {
			/* Frontier piece is too small. Thow it away. */
			tmp = mixedFrontier;
			mixedFrontier = 0;
			piecePutMixed(tmp);
			stoWatchFrontier(tmp);
			/* (Actually, this could have lead to a merge). */
		}

		IF (!mixedFrontier) {
			/* Need a new frontier piece to satisfy request. */
			Length	npages;
			ULong	nq, nb;
			Page	*pages;
			Section *sect;

			nq = QUO_ROUND_UP(nbytes, MixedSizeQuantum);
			nb = SectionHeadSize +
			     nq * (sizeof(QmInfo) + MixedSizeQuantum);
			npages = QUO_ROUND_UP(nb, PgSize);

			IF (npages < MixedSizePgGroup)
				npages = MixedSizePgGroup;

			pages = pagesGet(npages);
			if (!pages) return 0;

			sect  = sectPrepare(pages, npages,
					    (Length) MixedSizeQuantum, false);

			mt	       = (MxMem *) sect->data;
			mt->isFree     = false; /* so free won't merge+unlink */
			mt->isFirst    = true;
			mt->isLast     = true;
			mt->nbytesPrev = 0;
			mt->nbytesThis = sect->qmCount * MixedSizeQuantum;
			mt->sect       = sect;

			mixedFrontier = mt;
			stoWatchFrontier(mixedFrontier);
		}

		mi = mixedFrontier;
		mi->isFree = false;

		mn = mixedFrontier->nbytesThis;

		IF (shdSplit2(mn, nbytes))
			mixedFrontier = mxmemSplit(mi, shdBe2(mn, nbytes));
		else 
			mixedFrontier = 0;

		stoWatchFrontier(mixedFrontier);
	}
	return mi;
}

/****************************************************************************
 *
 * :: Garbage collector
 *
 ***************************************************************************/

/*
 * This draft assumes that all words can be pointers.
 *   The type code could be used to avoid following things which are known
 *   not to be pointers.
 *
 * Pointers into the middle of structures are properly recognized.
 */

local void	stoGcMarkAndSweep (void);
local int	stoGcMark	  (void);
local int	stoGcMarkRange	  (Pointer *lo, Pointer *hi, int check);
local int	stoGcSweep	  (void);
local int	stoGcSweepFixed	  (Section *);
local int	stoGcSweepMixed	  (Section *);

static int	stoGcMarkedFree;

local Bool
stoNeedsMoreHeadroom(int npgFree)
{
	int	i;
	ULong	gceLhs, gceRhs;
	ULong	nFree, nBusy, nPer;
	ULong	nSpare, nAvail, nTotal;


	/* Check the headroom for each size of fixed-size pieces */
	for (i = 0; i < FixedSizeCount; i++)
	{
		/* Compute free/busy/spare counts */
		nFree  = freeFixedPieces[i];
		nBusy  = busyFixedPieces[i];
		nPer   = sectQmCount(1L, fixedSize[i]);
		nSpare = nPer*npgFree;
		nAvail = nFree + nSpare;
		nTotal = nBusy + nAvail;


		/* Headroom ratios (don't want any FPE's) */
		gceLhs = GcEffectiveFactorNum*nTotal;
		gceRhs = GcEffectiveFactorDen*nAvail;


		/* Stop if not enough headroom */
		if (gceLhs > gceRhs) return true;
	}


	/* Compute mixed-size usage */
	nSpare = npgFree*(PgSize - MxMemHeadSize);
	nAvail = nSpare + freeMixedBytes;
	nTotal = nAvail + busyMixedBytes;


	/* Headroom ratios */
	gceLhs = GcEffectiveFactorNum*nTotal;
	gceRhs = GcEffectiveFactorDen*nAvail;


	/* Do we have enough mixed-sized headroom? */
	return (gceLhs > gceRhs);
}

/*
 * Entry point to garbage collector.
 * Use setjmp to force registers onto stack, then mark from roots.
 */

local void
stoGcMarkAndSweep(void)
{
	jmp_buf jb;
	int	nm, ns;

	setjmp(jb);

	if (gcTraceFile) {
		fprintf(gcTraceFile, " [GC: ");
		fflush(gcTraceFile);
	}
#ifdef FOAM_RTS
	if (markingStats) {
	  fprintf(osStderr, " [GC: ");
	  fflush(osStderr);
	}
#endif 


#ifdef STO_LONGJMP
	if (!setjmp(stoAllocInner_ErrorCatch)) return;
#endif

	nm = stoGcMark();
	ns = stoGcSweep();
	
	if (gcTraceFile) {
	fprintf(gcTraceFile, "marked %d (+ %d free), swept  %d.]\n",
			nm, stoGcMarkedFree, ns);
	}
#ifdef FOAM_RTS
	if (markingStats) fprintf(osStderr, "marked %d (+ %d free), swept  %d.]\n",
			nm, stoGcMarkedFree, ns);
#endif 
}


/* Pointer classification: DO NOT change the order of these defines! */
#define MEM_HEAP  0
#define MEM_IDATA 1
#define MEM_STACK 2
#define MEM_DDATA 3
#define MEM_MAX_TYPE (MEM_DDATA+1)

#define PTR_INTO_HEAP 0
#define PTR_INTO_BUSY 1
#define PTR_INTO_DATA 2
#define PTR_INTO_NEW  3
#define PTR_INTO_FREE 4
#define PTR_MAX_TYPE (PTR_INTO_FREE+1)

static int stoMarkArea, stoMarkChildren;
static long stoMarkCount[2*MEM_MAX_TYPE][PTR_MAX_TYPE];

static char *
stoMarkCountArea(int i)
{
	if (i >= MEM_MAX_TYPE)
	{
		switch (i - MEM_MAX_TYPE)
		{
		   case MEM_HEAP  : return "(*)  Heap";
		   case MEM_IDATA : return "(*) Idata";
		   case MEM_DDATA : return "(*) Ddata";
		   case MEM_STACK : return "(*) Stack";
		   default        : return "(*)  ????";
		}
	}
	else
	{
		switch (i)
		{
		   case MEM_HEAP  : return "     Heap";
		   case MEM_IDATA : return "    Idata";
		   case MEM_DDATA : return "    Ddata";
		   case MEM_STACK : return "    Stack";
		   default        : return "     ????";
		}
	}
}

static char *
stoMarkCountTest(int i)
{
	switch (i)
	{
	   case PTR_INTO_HEAP : return "Heap";
	   case PTR_INTO_BUSY : return "Busy";
	   case PTR_INTO_DATA : return "Data";
	   case PTR_INTO_NEW  : return " New";
	   case PTR_INTO_FREE : return "Free";
	   default            : return "????";
	}
}

/*
 * Mark the pieces currently in use.
 * Use the stack, static data, and foreign dynamic data as roots.
 * Return the number of busy pieces marked.
 */

local int
stoGcMark(void)
{
	struct osMemMap **mm;
	int	n;
	int i, j;
	static int doClassify = -1;

	mm = osMemMap(OSMEM_STACK | OSMEM_IDATA | OSMEM_DDATA);
	if (!mm) return 0;

	/* Pointer classification only available in debug version */
	if (DEBUG(sto)) {
		if (doClassify == -1)
			doClassify = (osGetEnv("GC_CLASSIFY") != NULL);

		if (doClassify)
		{
			/* Clear the classification tables */
			for (i = 0;i < (2*MEM_MAX_TYPE);i++)
				for (j = 0;j < PTR_MAX_TYPE;j++)
					stoMarkCount[i][j] = 0;
		}
	}

	stoGcMarkedFree = 0;
	n  = 0;

	for ( ; (*mm)->use != OSMEM_END; mm++) {
		if (DEBUG(sto)) {
			/* Memory type being traced (for classification) */
			stoMarkArea	= MEM_HEAP;
			stoMarkChildren = 0;
		}

		switch ((*mm)->use) {
		case OSMEM_STACK:
			if (DEBUG(sto)) {stoMarkArea++;}
			/* Fall through */
		case OSMEM_IDATA:
			if (DEBUG(sto)) {stoMarkArea++;}
			/*
			 * Top-level call to stoGcMarkRange requires
			 * page check under Win98.
			 */
			n += stoGcMarkRange((Pointer *)((*mm)->lo),
					    (Pointer *)((*mm)->hi), 1);

		/* Some compilers, e.g. Watcom, define 1 byte aligned
		 * pointers even packing structures with a 4 bytes alignment.
		 */
#if defined(CC_one_byte_aligned_pointers)
			{
			    int i;
			    for (i = 1; i < sizeof(char*); i++)
				n += stoGcMarkRange(
					(Pointer *)ptrOff((*mm)->lo, i),
					(Pointer *)((*mm)->hi), 1);
			}
#endif /* CC_one_byte_aligned_pointers_ */

			break;
		case OSMEM_DDATA: {
#if defined(OS_WIN32)
			/*
			 * We have to be careful - this segment may contain
			 * just part of the heap.
			 */
			char *p;
			int  i, inbottom = 0, intop = 0;

			if (DEBUG(sto)) {stoMarkArea = MEM_DDATA;}

			/* Scan from (*mm)->lo to heapStart? */
			if ((Pointer)heapStart >= (*mm)->lo &&
				(Pointer)heapStart < (*mm)->hi)
			{
			  inbottom = 1;
			  n += stoGcMarkRange((Pointer *)((*mm)->lo),
					      (Pointer *)(heapStart), 1);
			}

			/* Scan from heapEnd to (*mm)->hi? */
			if ( (Pointer)heapEnd >= (*mm)->lo &&
				(Pointer)heapEnd < (*mm)->hi)
			{
			  intop = 1;
			  n += stoGcMarkRange((Pointer *)(heapEnd),
					      (Pointer *)((*mm)->hi), 1);
			}

			/* If the heap is in the segment, scan foreign pages */
			if (inbottom || intop)
			{
			  /* Compute start and finish page */
			  int first = inbottom ? 0 : pgNo((Pointer)(*mm)->lo);
			  int last = intop ? pgMapSize : pgNo((Pointer)(*mm)->hi);

			  /* Scan foreign pages in heap in the segment */
			  for (i = first; i < last; i++) {
			    if (pgMap[i] != PgForeign) continue;
			    p = (char *) pgAt(i);
			    n += stoGcMarkRange((Pointer *)(p),
						(Pointer *)(p+PgSize), 1);
			  }
			}
			else {
			  /* Scan the whole segment */
			  n += stoGcMarkRange((Pointer *)((*mm)->lo),
					      (Pointer *)((*mm)->hi), 1);
			}
#else
			/*
			 * Assume that if heapStart lies within this
			 * segment then heapEnd does also.
			 */
			int  i;
			char *p;

			if (DEBUG(sto)) {stoMarkArea = MEM_DDATA;}
			if ( (Pointer) heapStart >= (*mm)->lo &&
				(Pointer) heapStart < (*mm)-> hi)
			{
			  n += stoGcMarkRange((Pointer *)((*mm)->lo),
					      (Pointer *)(heapStart), 1);
			  n += stoGcMarkRange((Pointer *)(heapEnd),
					      (Pointer *)((*mm)->hi),1 );
			  
#if defined(OS_MAC_OSX)
              /*
                DGC:
                On Mac OS X the heap is not necessarily contiguous.
                There may be PgForeign pages, but there may also be
                gaps in VM.  Avoid trying to mark non-existent memory.
              
                I suspect that this may be a bug for other systems
                too,  and the fix should work for any system, but
                I'll leave it OS_MAC_OSX specific for now.                
              */
			  int mm_hi_pgNo = pgNo((*mm)->hi) ;
              int iMax =  (mm_hi_pgNo<pgMapSize) ? mm_hi_pgNo : pgMapSize ;
              for (i = 0; i < iMax; i++) {
			    p = (char *) pgAt(i);
                n += stoGcMarkRange((Pointer *)(p),
						(Pointer *)(p+PgSize), 1    );
			  }
#else  /* !defined(OS_MAC_OSX) */
			  for (i = 0; i < pgMapSize; i++) {
			    if (pgMap[i] != PgForeign) continue;
			    p = (char *) pgAt(i);
			    n += stoGcMarkRange((Pointer *)(p),
						(Pointer *)(p+PgSize), 1    );
			  }
#endif  /* defined(OS_MAC_OSX) */
			}
			else {
			  n += stoGcMarkRange((Pointer *)((*mm)->lo),
					      (Pointer *)((*mm)->hi), 1);
			}
#endif
			break;
		
		  }
		  default:
			assert(false); /*mem type is silly*/
			break;
		}
	}

	/* Emit pointer classification table? */
	if (DEBUG(sto)) {
		if (doClassify)
		{
			/* Column headings */
			fprintf(osStderr, "\n           ");
			for (j = 0;j < PTR_MAX_TYPE;j++)
				fprintf(osStderr,"%10s ",stoMarkCountTest(j));


			/* Table contents */
			fprintf(osStderr, "\n");
			for (i = 0;i < (2*MEM_MAX_TYPE);i++)
			{
				fprintf(osStderr, "%s: ", stoMarkCountArea(i));
				for (j = 0;j < PTR_MAX_TYPE;j++)
				{
					long c = stoMarkCount[i][j];
					fprintf(osStderr,"%10ld ", c);
				}
				fprintf(osStderr, "\n");
			}

			/* Leave a gap below the table */
			fprintf(osStderr, "\n\n");
		}
	}

	return n;
}

local int
stoGcMarkRange(Pointer *lo, Pointer *hi, int check)
{
	Pointer		p, *pp, *plo, *phi, *hi0;
	static int	pgno, qmno;
	static PgInfo	pgtag;
	static QmInfo	qmtag;
	static Section *sect;
	int		n = 0;
	int		oldStoMarkArea;
#if defined(OS_WIN32)
	MEMORY_BASIC_INFORMATION minfo;
	int access;
#endif

#ifdef STO_CAN_BLACKLIST
	static int	canBlacklist = -1;

	if (canBlacklist == -1)
		canBlacklist = (osGetEnv("GC_BLACKLIST") != NULL);
#endif

#if defined(OS_WIN32)
	/*
	 * Under Win98 and probably Win95 too, pages that were accessible
	 * during the call to osMemMap may now be inaccessible. Some pages
	 * are decommitted, some are released and some have their access
	 * permissions changed. Without this check we get a segfault or GPF.
	 *
	 * We daren't put this code in a procedure because the pages might
	 * be on the stack and be unmapped on return.
	 *
	 * ASSUMES that if the first page is still valid, the rest are ...
	 */
	if (!check) goto PagesOkay; /* Page range known to be safe (in heap) */

	if (VirtualQuery((void *)lo, &minfo, sizeof(minfo)) != sizeof(minfo))
		return n; /* Query failed => don't risk scanning the page */

	/* First check the page type */
	switch (minfo.State)
	{
	   case MEM_COMMIT:
		/* These pages are okay. */
		break;
	   case MEM_FREE: /* FALL THROUGH */
		(void)fprintf(stderr, "*** committed -> free\n");
		(void)fflush(stderr);
		return n;
	   case MEM_RESERVE: /* FALL THROUGH */
		/* These pages are definitely not okay. */
#if 0		
		(void)fprintf(stderr, "*** committed -> reserved\n");
		(void)fflush(stderr);
#endif
		return n;
#if 0
	   case MEM_MAPPED:
		/* These pages might be okay. */
		break;
	   case MEM_PRIVATE:
		/* These pages might be okay. */
		break;
#endif
	   default:
		(void)fprintf(stderr, "*** Unknown page type\n");
		(void)fflush(stderr);
		/* Unknown page type: might be okay. */
		break;
	}

	/*
	 * Check the state of the page ignoring guard and cache status. We
	 * want to skip any page that we cannot read from or write to. If we
	 * were paranoid then we only skip pages we cannot read.
	 */
	access = minfo.Protect & ~(PAGE_GUARD | PAGE_NOCACHE);
	switch (access)
	{
	   case PAGE_NOACCESS:
		/* Cannot scan this page even if it holds pointers */
		(void)fprintf(stderr, "*** read/write -> no access\n");
		(void)fflush(stderr);
		return n;
	   case PAGE_READONLY:
	   case PAGE_EXECUTE:
	   case PAGE_EXECUTE_READ:
		/* Hope it doesn't contain pointers: try the next region. */
		(void)fprintf(stderr, "*** read/write -> read/exec\n");
		(void)fflush(stderr);
		return n;
	   case PAGE_READWRITE:
	   case PAGE_WRITECOPY:
	   case PAGE_EXECUTE_READWRITE:
	   case PAGE_EXECUTE_WRITECOPY:
	   case PAGE_WRITECOMBINE:
		/* Might contain pointers so scan them. */
		break;
	   default:
		/* Ignore this region in case it is something nasty. */
		(void)fprintf(stderr, "*** read/write -> ??? [%d]\n", access);
		(void)fflush(stderr);
		return n;
	}

	/* We get here if the page check passed or wasn't needed */
PagesOkay: {}
#endif

	/* Pointer classification */
	if (DEBUG(sto)) {oldStoMarkArea = stoMarkArea;}

	stoWatchMarkFrom(lo);
	stoWatchMarkTo  (hi);

TailRecursion:
	hi  = (Pointer *) ptrCanon(hi);
	hi0 = (Pointer *) ptrOff(((char *) hi), 1 - sizeof(char *));

	for (pp = lo; ptrLT(pp, hi0);
	     pp = (Pointer *) ptrOff((char *) pp, alignof(Pointer))) {
		p = *pp;

		/* Verify pointer is into heap. */
		if (!isInHeap(p)) continue;
		if (DEBUG(sto)) {stoMarkCount[stoMarkArea][PTR_INTO_HEAP]++;}


		/* Verify pointer is to busy page. */
		pgno  = pgNo(p);
		pgtag = pgMap[pgno];
		if (pgtag != PgBusyFirst && pgtag != PgBusyFollow) continue;
		if (DEBUG(sto)) {
			stoMarkCount[stoMarkArea][PTR_INTO_HEAP]--;
			stoMarkCount[stoMarkArea][PTR_INTO_BUSY]++;
		}


		/* Grab section header. */
		while (pgtag == PgBusyFollow) pgtag = pgMap[--pgno];
		sect = sectAt(pgno);

		/* Verify pointed-to quantum. */
		if (ptrLT(p, sect->data)) continue;
		if (DEBUG(sto)) {
			stoMarkCount[stoMarkArea][PTR_INTO_BUSY]--;
			stoMarkCount[stoMarkArea][PTR_INTO_DATA]++;
		}


#ifdef STO_DIVISION_BY_LOOKUP
		/* If interior of object, point to beginning. */
		/* Use shift operations where possible */
		if (sect->isFixed) {
			if (sect->qmLog) {
				qmno = qmLogNo(p, sect);
				p = ptrOff((char *) sect->data, qmno << sect->qmLog);
			}
			else {
				qmno  = qmDivNo(p, sect);
				assert(qmno == qmNo(p, sect));
				p = ptrOff((char *) sect->data, qmno * sect->qmSize);
			}
		}
		else {
			qmno  = qmNo(p, sect);
			p = ptrOff((char *) sect->data, qmno * sect->qmSize);
		}
#else
		/* If interior of object, point to beginning. */
		/* Use shift operations where possible */
		if (sect->isFixed && sect->qmLog) {
			qmno = qmLogNo(p, sect);
			p = ptrOff((char *) sect->data, qmno << sect->qmLog);
		}
		else {
			qmno  = qmNo(p, sect);
			/* If interior of object, point to beginning. */
			p = ptrOff((char *) sect->data, qmno * sect->qmSize);
		}
#endif

		/* Verify not already marked. */
		qmtag = sect->info[qmno];
		if (QmInfoMark(qmtag)) continue;
		if (DEBUG(sto)) {
			stoMarkCount[stoMarkArea][PTR_INTO_DATA]--;
			stoMarkCount[stoMarkArea][PTR_INTO_NEW]++;
		}


		/* Add mark bits quanta in piece.  Determine data bounds. */
		if (sect->isFixed) {
			QmInfoSetMark(sect->info[qmno]);
			plo = (Pointer *) p;
			phi = (Pointer *) ptrOff((char *) plo, sect->qmSize);
		}
		else {
			MxMem	*pc;
			int	sz, nq, qi;

			while (QmInfoKind(qmtag) == QmFollow)
				qmtag = sect->info[--qmno];

			pc = (MxMem *)ptrOff((char *)sect->data,
					     qmno*sect->qmSize);
			sz = pc->nbytesThis;
			nq = sz/sect->qmSize;

			for (qi = qmno; qi < qmno + nq; qi++)
				QmInfoSetMark(sect->info[qi]);

			sz -= MxMemHeadSize;
			plo = (Pointer *) (&pc->body.busy.data);
			phi = (Pointer *) ptrOff((char *) plo, sz);
		}

		/* Verify piece is in use. */
		if (QmInfoKind(qmtag) == QmFreeFirst) {
			if (DEBUG(sto)) {
				stoMarkCount[stoMarkArea][PTR_INTO_NEW]--;
				stoMarkCount[stoMarkArea][PTR_INTO_FREE]++;
			}
			stoGcMarkedFree++;

#ifdef STO_CAN_BLACKLIST
			/* We only blacklist fixed-sized pages currently */
			if (canBlacklist && sect->isFixed)
				sect->info[qmno] = QmInfoMake0(QmBlacklisted);
#endif
			continue;
		}

#ifdef STO_CAN_BLACKLIST
		if (QmInfoKind(qmtag) == QmBlacklisted) continue;
#endif
		n++;

		/* Check that this contains pointers 
		 * This test should use a bit in the tag, not the code.
		 */
		if (QmIsPtrFree(qmtag)) continue;


#if STO_USER_CAN_TRACE
		/*
		 * Has the creator of this object requested
		 * permission to trace it?
		 */
		if (QmAldorTraced(qmtag))
		{
			/* Invoke the creator's tracer */
			n += (int)stoFiCCall2(int, QmAldorTraced(qmtag),
				plo, phi);
			continue;
		}
		if (QmCTraced(qmtag))
		{
			/* Invoke the creator's tracer */
			n += (QmCTraced(qmtag))(plo, phi);
			continue;
		}
#endif

		/* Mark descendants. */
		if (ptrEQ(pp, hi-1)) {
			if (DEBUG(sto)) {
				if (stoMarkArea < MEM_MAX_TYPE) {
					/* Now marking within heap */
					oldStoMarkArea = stoMarkArea;
					stoMarkArea += MEM_MAX_TYPE;
				}
			}

			lo = plo;
			hi = phi;
			goto TailRecursion;
		}

		/* Pointer classification */
		if (DEBUG(sto)) {
			if (stoMarkArea < MEM_MAX_TYPE) {
				/* Now marking within heap */
				oldStoMarkArea = stoMarkArea;
				stoMarkArea += MEM_MAX_TYPE;
			}
		}

		n += stoGcMarkRange(plo, phi, (int) 0);

		/* Pointer classification */
		if (DEBUG(sto)) {stoMarkArea = oldStoMarkArea;}
	}

	/* Pointer classification */
	if (DEBUG(sto)) {stoMarkArea = oldStoMarkArea;}
	return n;
}

/*
 * Collect unmarked busy pieces.
 * The small piece free lists are reconstructed, sorted by section.
 */

static FxMem	fixedHeadStruct[FixedSizeCount], *fixedTail[FixedSizeCount];
static ULong	freeFixedStruct[FixedSizeCount];
static ULong	busyFixedStruct[FixedSizeCount];

local int
stoGcSweep(void)
{
	int	cd, ix, pgno, pgcount, swept, got;
	Section *sect;

#ifdef USE_MEMORY_CLIMATE
	initMemoryClimateHistogram();
#endif

	for (cd = 0; cd < STO_CODE_LIMIT; cd++)
		stoPiecesGc[cd] = 0;	/* Tally by code of pieces swept. */

	/* Reset the fixed-sized free lists and counters. */
	for (ix = 0; ix < FixedSizeCount; ix++)
	{
		fixedTail[ix] = &fixedHeadStruct[ix];

		freeFixedStruct[ix] = 0L;
		busyFixedStruct[ix] = 0L;
	}


	/* Reset the mixed-sized store counting */
	freeMixedBytes = 0L;
	busyMixedBytes = 0L;


	/* Check each section in the heap */
	for (swept = 0, pgno = 0; pgno < pgMapSize; pgno += pgcount)
	{
		if (pgMap[pgno] != PgBusyFirst) { pgcount = 1; continue; }

		sect	= sectAt(pgno);
		pgcount = sect->pgCount;

		if (sect->isFixed)
		{
			got = stoGcSweepFixed(sect);
			swept += got;
		}
		else
			swept += stoGcSweepMixed(sect);
	}

	for (ix = 0; ix < FixedSizeCount; ix++) {
		fixedTail[ix]->next = 0;
		fixedPieces[ix] = fixedHeadStruct[ix].next;

		freeFixedPieces[ix] = freeFixedStruct[ix];
		busyFixedPieces[ix] = busyFixedStruct[ix];
	}
#ifdef USE_MEMORY_CLIMATE
	finiMemoryClimateHistogram();
	showMemoryClimateHistogram(stdout);
#endif
	return swept;
}

local int
stoGcSweepFixed(Section *sect)
{
	int	qmcount, qmsize, qmsizeix, qmno, qmbusy, mark, swept;
	QmInfo	qmtag;
	char	*data;
	FxMem	*pc, *nfixedTail;
	ULong	nfixedFree;

	data	 = (char *) sect->data;
	qmcount	 = sect->qmCount;
	qmsize	 = sect->qmSize;
	qmsizeix = sect->qmSizeIndex;
	swept	 = 0;
	qmbusy	 = 0;
	nfixedTail = fixedTail[qmsizeix];
	nfixedFree = 0L;

	for (qmno = 0; qmno < qmcount; qmno++) {
		qmtag = sect->info[qmno];
		mark  = QmInfoMark(qmtag);

		if (QmInfoKind(qmtag) == QmBusyFirst) {
			if (mark) {
				QmInfoClearMark(sect->info[qmno]);
#ifdef USE_MEMORY_CLIMATE
				incrMemoryClimateHistogram(
					QmInfoCode(sect->info[qmno]), (int) 0, 1
				);
#endif
				qmbusy++;
			}
			else {
				pc = (FxMem *)(data+qmno*qmsize);
				stoPiecesGc[ QmInfoCode(sect->info[qmno]) ]++;
#ifdef USE_MEMORY_CLIMATE
				incrMemoryClimateHistogram(
					QmInfoCode(sect->info[qmno]), 1, 1
				);
#endif
				sect->info[qmno] = QmInfoMake0(QmFreeFirst);
				fxmemCleanBody(pc, qmsize);
				stoTally(stoBytesGc += qmsize);
				nfixedTail->next = pc;
				nfixedTail = pc;
				swept++;
			}
		}
		else {
			if (mark) QmInfoClearMark(sect->info[qmno]);

#ifdef STO_CAN_BLACKLIST
			/* Skip blacklisted pieces */
			if (QmInfoKind(qmtag) == QmBlacklisted) {
				stoTally(stoBytesBlack += qmsize);
				continue;
			}
#endif

			pc = (FxMem *)(data+qmno*qmsize);
			nfixedTail->next = pc;
			nfixedTail = pc;
			nfixedFree++;
		}
	}

	if (qmbusy)
	{
		fixedTail[qmsizeix] = nfixedTail;
		freeFixedStruct[qmsizeix] += (nfixedFree + swept);
		busyFixedStruct[qmsizeix] += qmbusy;
	}
	else
		pagesPut((Page *) sect, sect->pgCount);
	return swept;
}

local int
stoGcSweepMixed(Section *sect)
{
	int	qmcount, qmsize, qmno, qi, nq, sz, mark, swept, qmbusy;
	QmInfo	qmtag;
	MxMem	*pc;
	char	*data;
	Pointer pstart, pend, pfront;
	ULong	nMixedFree = freeMixedBytes;
	ULong	nMixedBusy = busyMixedBytes;

	data	= (char *) sect->data;
	qmsize	= sect->qmSize;
	qmcount = sect->qmCount;

	swept	= 0;
	qmbusy	= 0;

	for (qmno = 0; qmno < qmcount; qmno += nq) {
		pc    = (MxMem *)(data + qmno*qmsize);
		sz    = pc->nbytesThis;
		nq    = sz/sect->qmSize;
		qmtag = sect->info[qmno];
		mark  = QmInfoMark(qmtag);

		if (!mark && QmInfoKind(qmtag) == QmBusyFirst) {
			MxMem	*npc   = mxmemNext(pc);

			/* Two cases, depending on whether might merge. */
			if (npc == 0 || !npc->isFree) {

				/* Free piece. */
				stoPiecesGc[ QmInfoCode(sect->info[qmno]) ]++;
#ifdef USE_MEMORY_CLIMATE
				incrMemoryClimateHistogram(
					QmInfoCode(sect->info[qmno]), 1, 1
				);
#endif
				sect->info[qmno]  = QmInfoMake0(QmFreeFirst);
				mxmemCleanBody(pc, sz);
				stoTally(stoBytesGc += sz-MxMemHeadSize);
				piecePutMixed(pc);
				swept++;
			}
			else {
				/* Save info, in case of merge. */
				Length nqmno	  = qmno + nq;
				ULong  nsz	  = npc->nbytesThis;
				Length nnq	  = nsz/sect->qmSize;
				QmInfo nqmtag	  = sect->info[nqmno];

				/* Free piece. */
				stoPiecesGc[ QmInfoCode(sect->info[qmno]) ]++;
#ifdef USE_MEMORY_CLIMATE
				incrMemoryClimateHistogram(
					QmInfoCode(sect->info[qmno]), 1, 1
				);
#endif
				sect->info[qmno]  = QmInfoMake0(QmFreeFirst);
				mxmemCleanBody(pc, sz);
				stoTally(stoBytesGc += sz-MxMemHeadSize);
				piecePutMixed(pc);
				swept++;

				/* Handle if next piece was merged. */
				if (QmInfoKind(sect->info[nqmno])==QmFollow) {
					if (QmInfoMark(nqmtag)) {
						int N = nqmno+nnq;
						for (qi = nqmno; qi < N; qi++)
							QmInfoClearMark(
							  sect->info[qi]);
					}
					nq += nnq;
				}
			}
			nMixedFree += sz;
		}
		else if (mark) {
#ifdef USE_MEMORY_CLIMATE
			incrMemoryClimateHistogram(
				QmInfoCode(sect->info[qmno]), (int) 0, 1
			);
#endif
			for (qi = qmno; qi < qmno+nq; qi++)
			    QmInfoClearMark(sect->info[qi]);

			if (QmInfoKind(qmtag) == QmBusyFirst)
				qmbusy += nq;
			nMixedBusy += sz;
		}
	}

	/* Return pages if no busy pieces & sect does not contain frontier. */
	pstart = ptrCanon((char *) sect);
	pend   = ptrCanon((char *) sect + sect->pgCount * PgSize);
	pfront = ptrCanon((char *) mixedFrontier);

	if (!qmbusy && (pfront < pstart || pend <= pfront)) {
		mxmemUnlinkFromBTree((MxMem *) data, &mixedPieces);
		pagesPut((Page *) sect, sect->pgCount);
	}
	else
	{
		/* Note these changes in busy/free counts */
		freeMixedBytes = nMixedFree;
		busyMixedBytes = nMixedBusy;
	}
	return swept;
}


/****************************************************************************
 *
 * :: Audit code
 *
 ***************************************************************************/

#ifndef NDEBUG

local  void	stoAuditHeapLocation	 (void);
local  void	stoAuditMapPages	 (void);

local  long *	stoAuditFixedSizeSections(void);
local  long *	stoAuditFixedSizePieces	 (void);

local  long	stoAuditMixedSizeSections(void);
local  long	stoAuditMixedSizePieces	 (void);

static long	nMixed, nSizes;

/*
 * Main entry into the audit code.
 */
local void
stoAuditAll(void)
{
	long	i, n1, n2, *a1, *a2;

	nMixed = 0, nSizes = 0;

	stoAuditHeapLocation();
	stoAuditMapPages();

	a1 = stoAuditFixedSizeSections();
	a2 = stoAuditFixedSizePieces();
	for (i = 0; i < FixedSizeCount; i++) assert(a1[i] == a2[i]);

	n1 = stoAuditMixedSizeSections();
	n2 = stoAuditMixedSizePieces();

	assert(n1 == n2);
}

/*
 * Verify heap alignment and range.
 */
local void
stoAuditHeapLocation(void)
{
	/* Verify heap is page-aligned. */
	assert(ptrToLong((Pointer) heapStart) % PgSize == 0);
	assert(ptrToLong((Pointer) heapEnd)   % PgSize == 0);
}

/*
 * Verify the page map is self-consisteng.
 */
local void
stoAuditMapPages(void)
{
	Length	i, good, pgcount;

	/* Verify page map location. */
	pgcount = QUO_ROUND_UP(pgMapSize * sizeof(PgInfo), PgSize);
	assert(ptrToLong((Pointer) pgMap) % PgSize == 0);
	assert(pgMapUses == pgcount);

	/* Verify map contains only good entries. */
	good = 0;
	for (i = 0; i < pgMapSize; i++) {
		switch (pgMap[i]) {
		case PgFree:  case PgBusyFirst: case PgBusyFollow:
		case PgPgMap: case PgBTree:	case PgDLL:
		case PgForeign:
			good++;
		}
	}
	assert(good == pgMapSize);
}

/*
 * Verify the pages for sections of fixed size pieces.
 */
local long *
stoAuditFixedSizeSections(void)
{
	static long fxpCount[FixedSizeCount];

	int	pgno, pgcount, qmno, qmcount, sz, ix;
	QmInfo	qmtag;
	Section *sect;
	char	*data;

	for (ix = 0; ix < FixedSizeCount; ix++)
		fxpCount[ix] = 0;

	for (pgno = 0; pgno < pgMapSize;  pgno += pgcount) {
		if (pgMap[pgno] != PgBusyFirst) { pgcount = 1; continue; }

		/* Get section information. */
		sect	= sectAt(pgno);
		pgcount = sect->pgCount;
		if (!sect->isFixed) continue;

		data	= (char *) sect->data;
		qmcount = sect->qmCount;
		ix	= sect->qmSizeIndex;
		sz	= sect->qmSize;

		/* Verify section consistency. */
		assert(sz == fixedSize[ix]);
		assert(ptrEQ(data + qmcount*sz, (char *)sect + pgcount * PgSize));
		assert(pgno + pgcount <= pgMapSize);

		/* Verify data alignment. */
		if (sz < alignof(MostAlignedType)) {
			assert((long) data % sz == 0);
		}
		else {
			assert((long) data % alignof(MostAlignedType) == 0);
			assert(sz	   % alignof(MostAlignedType) == 0);
		}

		/* Scan data. */
		for (qmno = 0; qmno < qmcount; qmno++) {
			qmtag = sect->info[qmno];
			if (QmInfoKind(qmtag) == QmFreeFirst) {
				fxmemAssertCleanBody(data + qmno*sz, sz);
				fxpCount[ix]++;
			}
			else if (QmInfoKind(qmtag) == QmBlacklisted)
				/* Do nothing */;
			else
				assert(QmInfoKind(qmtag) == QmBusyFirst);
			assert(!QmInfoMark(qmtag));
		}
	}

	return fxpCount;
}

/*
 * Verify the recorded fixed size pieces.
 */
local long *
stoAuditFixedSizePieces(void)
{
	static long fxpCount[FixedSizeCount];

	int	pgno, qmno, sz, ix;
	PgInfo	pgtag;
	QmInfo	qmtag;
	Section *sect;
	FxMem	*pc;
	char	*data;

	/* Verify pieces in free lists. */
	for (ix = 0; ix < FixedSizeCount; ix++) {
		fxpCount[ix] = 0;
		sz = fixedSize[ix];
		for (pc = fixedPieces[ix]; pc; pc = pc->next) {
			pgno  = pgNo(pc);
			pgtag = pgMap[pgno];
			sect  = sectFor(pc);
			assert(sect != 0);

			data  = (char *) sect->data;
#ifdef STO_DIVISION_BY_LOOKUP
			/* TTT */
			qmno = (sect->qmLog) ? qmLogNo(pc, sect) : qmDivNo(pc, sect); 
			assert(qmno == qmNo(pc, sect));
#else
			qmno = qmNo(pc, sect);
#endif

			qmtag = sect->info[qmno];

			/* Verify kind of page. */
			assert(0 <= pgno && pgno < pgMapSize);
			assert(pgtag == PgBusyFirst || pgtag == PgBusyFollow);


			/* Verify kind of section. */
			assert(sect->isFixed);
			assert(sect->qmSize == sz && sect->qmSizeIndex == ix);


			/* Verify alignment in section. */
			assert(ptrLE(data, pc));
			assert(ptrDiff((char *) pc, data) % sz == 0);
			assert(0 <= qmno && qmno < sect->qmCount);

			/* Verify piece is free. */
			assert(QmInfoKind(qmtag) == QmFreeFirst);

			fxpCount[ix]++;
		}
	}
	return fxpCount;
}

/*
 * Verify the pages for sections of mixed size pieces.
 */
local long
stoAuditMixedSizeSections(void)
{
	int	pgno, pgcount, qmno, qmcount, qmsize, sz, nq, i, cd;
	long	mxpBytes;
	QmInfo	qmtag;
	Section *sect;
	MxMem	*pc;
	char	*data;

	mxpBytes = 0;

	/* Verify pages with mixed size pieces. */
	for (pgno = 0; pgno < pgMapSize;  pgno += pgcount) {
		if (pgMap[pgno] != PgBusyFirst) { pgcount = 1; continue; }

		/* Get section information. */
		sect	= sectAt(pgno);
		pgcount = sect->pgCount;
		if (sect->isFixed) continue;

		data	= (char *) sect->data;
		qmcount = sect->qmCount;
		qmsize	= sect->qmSize;

		/* Verify section consistency. */
		assert(qmsize % MixedSizeQuantum == 0);
		assert(ptrEQ(ptrOff(data, qmcount*(long)qmsize),ptrOff((char *)sect, pgcount*PgSize)));
		assert(pgno + pgcount <= pgMapSize);

		for (i = pgno+1; i < pgno + pgcount; i++)
			assert(pgMap[i] == PgBusyFollow);

		if (pgno + pgcount < pgMapSize) {
			assert(pgMap[pgno + pgcount] != PgBusyFollow);
		}

		/* Verify data alignment. */
		assert(ptrToLong(data) % alignof(MostAlignedType) == 0);
		assert(qmsize	       % alignof(MostAlignedType) == 0);

		/* Scan data. */
		for (qmno = 0; qmno < qmcount; qmno += nq) {
			pc	  = (MxMem *)ptrOff(data, qmno*(long)qmsize);
			sz	  = pc->nbytesThis;
			nq	  = sz/qmsize;

			/* Verify context. */
			assert(sz > 1);			/* sz == 1 => fixed */
			assert(ptrEQ(pc->sect, sect));

			/* Verify boundary conditions. */
			assert((pc->isFirst) == (qmno	 == 0));
			assert((pc->isLast)  == (qmno+nq == qmcount));

			/* Verify neighbours. */
			assert(qmno + nq <= qmcount);
			assert(pc->nbytesThis % qmsize == 0);
			assert(pc->nbytesPrev % qmsize == 0);

			if (!pc->isFirst) {
				i = qmno - pc->nbytesPrev/qmsize;
				assert(0 <= i && i <= qmcount);

				qmtag = sect->info[i];
				cd    = QmInfoKind(qmtag);
				assert(cd == QmFreeFirst || cd == QmBusyFirst);
			}
			if (!pc->isLast) {
				i = qmno + pc->nbytesThis/qmsize;
				assert(0 <= i && i <= qmcount);

				qmtag = sect->info[i];
				cd    = QmInfoKind(qmtag);
				assert(cd == QmFreeFirst || cd == QmBusyFirst);
			}

			/* Verify tags. */
			qmtag = sect->info[qmno];
			cd    = QmInfoKind(qmtag);
			assert(!QmInfoMark(qmtag));

			if (pc->isFree) {
				mxmemAssertCleanBody(pc, pc->nbytesThis);
				assert(cd == QmFreeFirst);
			}
			else if (ptrEQ(pc, mixedFrontier)) {
				mxmemAssertCleanBody(pc, pc->nbytesThis);
				assert(cd == QmFreeFirst);
			}
			else
				assert(cd == QmBusyFirst);

			for (i = qmno+1; i < qmno+nq; i++) {
				qmtag = sect->info[i];
				cd    = QmInfoKind(qmtag);
				assert(!QmInfoMark(qmtag));
				assert(cd == QmFollow);
			}
			if (qmno+nq < qmcount) {
				qmtag = sect->info[qmno+nq];
				cd    = QmInfoKind(qmtag);
				assert(cd != QmFollow);
			}

			if (pc->isFree) mxpBytes += sz;
		}
	}

	return mxpBytes;
}

/*
 * Verify the recorded mixed size pieces.
 */
local long	stoAuditBTree	(BTree bt, BTreeKey *pLoBd, BTreeKey *pHiBd);
local long	stoAuditDLL	(Length l, MxMemDLL *dll);

local long
stoAuditMixedSizePieces(void)
{
	return stoAuditBTree(mixedPieces, NULL, NULL);
}

local long
stoAuditBTree(BTree bt, BTreeKey *pLoBd, BTreeKey *pHiBd)
{
	BTreePart	xp, x0, xN;
	int		isRoot = !pLoBd && !pHiBd;
	int		n, pgno;
	long		mxpBytes;

	if (isRoot && !bt) return 0;

	/* Node info. */
	assert(bt != 0);
	assert(bt->t == MixedBTreeT);
	n    = bt->nKeys;
	x0   = bt->part;
	xN   = bt->part + n;

	/* Verify bt points to a node entirely within a B-tree node page. */
	pgno = pgNo(bt);
	assert(0 <= pgno && pgno < pgMapSize);
	assert(pgMap[pgno] == PgBTree);

	pgno = pgNo((char *)bt + btreeNodeSize(bt->t) - 1);
	assert(0 <= pgno && pgno < pgMapSize);
	assert(pgMap[pgno] == PgBTree);

	/* Verify bt has legal number of keys. */
	assert((isRoot ? 0 : bt->t-1) <= n && n <= 2*bt->t-1);

	/* Verify order of keys: Note duplicate keys are not allowed here. */
	if (pLoBd) { assert(*pLoBd < x0->key); }
	for (xp = x0+1; xp < xN; xp++) assert((xp-1)->key < xp->key);
	if (pHiBd) { assert((xN-1)->key < *pHiBd); }

	/* Check descendants. */
	mxpBytes = 0;
	for (xp = x0; xp < xN; xp++)
		mxpBytes += stoAuditDLL((Length)xp->key,(MxMemDLL *)xp->entry);
	if (!bt->isLeaf) {
		mxpBytes += stoAuditBTree(x0->branch, pLoBd, &x0->key);
		for (xp = x0+1; xp < xN; xp++)
			mxpBytes +=
			    stoAuditBTree(xp->branch, &(xp-1)->key, &xp->key);
		mxpBytes += stoAuditBTree(xN->branch, &(xN-1)->key, pHiBd);
	}
	return mxpBytes;
}

local long
stoAuditDLL(Length nbytes, MxMemDLL *dll)
{
	int	pgno;
	long	mxpBytes;
	MxMem	*P0, *P, *A, *B;

	nSizes++;
	/* Verify dll points into DLL page. */
	assert(dll != 0);
	pgno = pgNo(dll);
	assert(0 <= pgno && pgno < pgMapSize);
	assert(pgMap[pgno] == PgDLL);

	/* Verify node. */
	assert(dll->nbytes == nbytes);
	assert(dll->pieces != 0);

	/* Verify center of DLL. */
	P = P0 = dll->pieces;
	assert(P->isFree);
	assert(P->nbytesThis == nbytes);
	assert(P->body.free.dll == dll);
	A = P->body.free.linkA; if (A) { assert(A->body.free.linkB == P); }
	B = P->body.free.linkB; if (B) { assert(A->body.free.linkA == P); }
	mxpBytes = nbytes;
	nMixed++;

	/* Verify A direction of DLL. */
	for (P = P0->body.free.linkA; P; P = A) {
		assert(P->isFree);
		assert(P->nbytesThis == nbytes);
		assert(P->body.free.dll == dll);
		A = P->body.free.linkA;
		if (A) { assert(A->body.free.linkB == P); }
		mxpBytes += nbytes;
	nMixed++;
	}

	/* Verify B direction of DLL. */
	for (P = P0->body.free.linkB; P; P = B) {
		assert(P->isFree);
		assert(P->nbytesThis == nbytes);
		assert(P->body.free.dll == dll);
		B = P->body.free.linkB;
		if (B) { assert(A->body.free.linkA == P); }
		mxpBytes += nbytes;
	nMixed++;
	}
	return mxpBytes;
}

#endif

long
stoShowMixedSizeSections(void)
{
	int	pgno, pgcount, qmno, qmcount, qmsize, sz, nq, i, cd;
	QmInfo	qmtag;
	Section *sect;
	MxMem	*pc;
	char	*data;
	Bool	anyMarked = false;

	fprintf(osStderr, "- - - -\n");
	for (pgno = 0; pgno < pgMapSize;  pgno += pgcount) {
		if (pgMap[pgno] != PgBusyFirst) { pgcount = 1; continue; }

		/* Get section information. */
		sect	= sectAt(pgno);
		pgcount = sect->pgCount;
		data	= (char *) sect->data;
		qmcount = sect->qmCount;
		qmsize	= sect->qmSize;

		if (sect->isFixed) {
			fprintf(osStderr,"(:::Fx %d[%d:%d:%d]:::)\n",
				qmsize,pgno,pgcount,qmcount);
			continue;
		}
		else {
			fprintf(osStderr,"(:::Mx %d[%d:%d:%d]\n",
				qmsize,pgno,pgcount,qmcount);
		}


		for (qmno = 0; qmno < qmcount; qmno += nq) {
			pc	  = (MxMem *)ptrOff(data, qmno*(long)qmsize);
			sz	  = pc->nbytesThis;
			nq	  = sz/qmsize;

			if (pc->isFirst) fprintf(osStderr, "<<\n");

			if (!pc->isFirst)
				fprintf(osStderr, "(%ld)\n",
					pc->nbytesPrev/qmsize);
			fprintf(osStderr, "%p=(%d){", pc, nq);
			if (pc->isFree)
				fprintf(osStderr, "F/");
			else
				fprintf(osStderr, "B/");

			if (ptrEQ(pc, mixedFrontier))
				fprintf(osStderr, "<frontier>");

			for (i = qmno; i < qmno+nq; i++) {
				qmtag = sect->info[i];
				cd    = QmInfoKind(qmtag);

				if (QmInfoMark(qmtag)) {
					fprintf(osStderr, "<MARK>");
					anyMarked = true;
				}
				switch (cd) {
				case QmFreeFirst:
					fprintf(osStderr, "f"); break;
				case QmBusyFirst:
					fprintf(osStderr, "b"); break;
				case QmFollow:
					fprintf(osStderr, "."); break;
				case QmBlacklisted:
					fprintf(osStderr, "."); break;
				default:
					fprintf(osStderr, "=[%x]", cd); break;
				}
			}
			fprintf(osStderr, "}");
			if (pc->isLast) { fprintf(osStderr, "\n>>"); }
			if (nq == 0) { fprintf(osStderr, "BUG!!\n"); exit(int0); }

		}
		fprintf(osStderr, "\n:::)\n");
	}
	fprintf(osStderr, "- - - -\n");
	if (anyMarked) exit(int0);
	return 0;
}

/****************************************************************************
 *
 * :: Tuning
 *
 ***************************************************************************/

/*
 * Sort free lists into ascending address order.
 * This does not use QmInfo so it can be used even without tags.
 */
local void
stoRebuildFreeLists(void)
{
	Section *sect;
	int	ix, pgno, pgcount;
	FxMem	*pc, *hd, *fp;

	/* Initialize each section's free list to 0. */
	for (pgno = 0; pgno < pgMapSize; pgno += pgcount) {
		if (pgMap[pgno] != PgBusyFirst)
			pgcount	   = 1;
		else {
			sect	   = sectAt(pgno);
			sect->free = 0;
			pgcount	   = sect->pgCount;
		}
	}

	/* Organize pieces from the free lists into the per-section lists. */
	for (ix = 0; ix < FixedSizeCount; ix++) {
		pc = fixedPieces[ix];
		fixedPieces[ix] = 0;
		while (pc) {
			hd   = pc;
			pc   = pc->next;
			sect = sectFor((Pointer) hd);
			hd->next = sect->free;
			sect->free = hd;
		}
	}
	/* Join the per-lists to make the new free lists. */
	for (pgno = pgMapSize-1; pgno >= 0;  pgno--) {
		if (pgMap[pgno] != PgBusyFirst) continue;

		sect = sectAt(pgno);
		if (!sect->isFixed) continue;

		ix   = sect->qmSizeIndex;
		pc   = fixedPieces[ix];
		fp   = sect->free;

		while (fp) {
			hd = fp;
			fp = fp->next;
			hd->next = pc;
			pc = hd;
		}
		fixedPieces[ix] = pc;
	}
}

/****************************************************************************
 *
 * :: Externally visible operations
 *
 ***************************************************************************/


local int
stoInit(void)
{
	StoInfoObj	info;
	int	i, j, sz0, sz;
	gcTraceFile = 0;

#ifdef USE_MEMORY_CLIMATE
	limitNumberOfMemoryClimates(QmCodeMask);
#endif

	if (osGetEnv("GC_FRUGAL")) GcIsFrugal = true;

	if (GcIsFrugal)
	{
		GcEffectiveFactorNum = GcFrugalFactorNum;
		GcEffectiveFactorDen = GcFrugalFactorDen;
	}

	if (osGetEnv("GC_GEFN")) GcEffectiveFactorNum = atoi(osGetEnv("GC_GEFN"));
	if (osGetEnv("GC_GEFD")) GcEffectiveFactorDen = atoi(osGetEnv("GC_GEFD"));

	if (osGetEnv("GC_GGFN")) GcGrowthFactorNum = atoi(osGetEnv("GC_GGFN"));
	if (osGetEnv("GC_GGFD")) GcGrowthFactorDen = atoi(osGetEnv("GC_GGFD"));

	if (osGetEnv("GC_DETAIL")) markingStats = true;


	/* Initialize memory mapper. */
	osMemMap(int0);

	/* Statistics */
	stoBytesOwn   = 0;
	stoBytesAlloc = 0;
	stoBytesFree  = 0;
	stoBytesGc    = 0;
	stoBytesBlack = 0;

	/* Page map */
	pgmapInit();

	/* Heap */
	heapStart = (char *) ptrOff((char *) pgMap, long0);
	heapEnd	  = (char *) ptrOff((char *) pgMap, pgMapUses*PgSize);
	pgmapMod((Page *) pgMap, pgMapUses, PgPgMap);

	/* Fixed pieces */
	for (sz0 = 0, i = 0; i < FixedSizeCount; sz0 = sz+1, i++) {
		sz = fixedSize[i];
		for (j = sz0; j <= sz; j++) {
			fixedSizeFor	 [j] = sz;
			fixedSizeIndexFor[j] = i;
		}
	}

	/* Free lists and fixed-sized store counting */
	for (i = 0; i < FixedSizeCount; i++) {
		fixedPieces[i] = 0;
		freeFixedPieces[i] = 0L;
		busyFixedPieces[i] = 0L;
	}


	/* Mixed-sized store counting */
	freeMixedBytes = 0L;
	busyMixedBytes = 0L;


#ifdef STO_DIVISION_BY_LOOKUP
	/* Initialise division lookup-table */
	for (i = 0; i < FixedSizeCount; i++)
	{
		int table = -(fixedSizeLog[i]+1);
		int size  = fixedSize[i];


		/* Skip sizes which don't require division */
		if (fixedSizeLog[i] >= 0) continue;


		/* Compute the divisions */
		for (j = 0;j < PgSize;j++)
			stoDivTable[table][j] = j / size;
	}
#endif


#ifndef FOAM_RTS
	/* All objects contain pointers */
	info.hasPtrs = 1;
	for (i = 0;i < STO_CENSUS_LIMIT; i++)
	{
		info.code = i;
		stoRegister(&info);
	}
#endif


	/* Mixed pieces */
	mixedPieces   = 0;
	mixedFrontier = 0;
	stoWatchFrontier(mixedFrontier);

	/* User traceable objects */
	for (i = 0;i < OB_MAX;i++) stoObAldorTracer[i] = (StoFiClos)0;
	for (i = 0;i < OB_MAX;i++) stoObCTracer[i] = (StoTraceFun)0;

	stoIsInit     =	 pgMap != 0;
	
	gcTimer.time = 0;
	gcTimer.live = 0;

	return stoIsInit;
}


/*
 * Added specifically for use with LIP big integers
 */
MostAlignedType *
stoCAlloc(unsigned code, ULong nbytes)
{
	MostAlignedType *result = stoAlloc(code, nbytes);
	memlset(result, 0x00, nbytes);
	return result;
}


MostAlignedType *
stoAlloc(unsigned code, ULong nbytes)
{
	Pointer 	p;
	Section 	*sect;
	Length		qi;
	MostAlignedType *ap;

	if (!stoIsInit && !stoInit())
		return (*stoError)(StoErr_CantBuild);

#ifdef USE_MEMORY_CLIMATE
	code = getMemoryClimate();
#endif

	if (nbytes==0) return NULL; /* TTT */
	if (nbytes <= FixedSizeMax)
	{
		FxMem		*pc;
		int		si;

		si = fixedSizeIndexFor[nbytes];
		pc = fixedPieces[si];

		if (!pc) {
			pc = piecesGetFixed(nbytes);
			if (!pc) return (*stoError)(StoErr_OutOfMemory);
		}

		fixedPieces[si] = pc->next;
		p = (Pointer)pc;

		if (stoMustTag)
		{
			/* This is much more efficient than sectFor() */
			sect = sectOf(p);


#ifdef STO_DIVISION_BY_LOOKUP
			/* Compute the quantum index */
			qi = (sect->qmLog) ?  qmLogNo(p, sect) : /* TTT */ 
			  /* Use division-by-lookup-table */ qmDivNo(p, sect);

			assert(qi == qmNo(p, sect));

#else
			/* Compute the quantum index */
			if (sect->qmLog)
				qi = qmLogNo(p, sect);
			else
				qi = qmNo(p, sect);
#endif


			/* Update the info for this quantum */
			sect->info[qi] = QmInfoMake(QmBusyFirst, code);
		}

		stoTally(stoBytesAlloc += fixedSize[si]);
		newFill (p,               fixedSize[si]);
	}
	else
	{
		MxMem		*pc;
		ULong		nb;

#ifdef STO_LONGJMP
		if (setjmp(stoAllocInner_ErrorCatch)) {
			p = ptrCanon(stoAllocInner_ErrorValue);
			return (MostAlignedType *) p;
		}
#endif
		nb = ROUND_UP(nbytes + MxMemHeadSize, MixedSizeQuantum);
		pc = pieceGetMixed(nb);

		if (!pc) return (*stoError)(StoErr_OutOfMemory);

		p  = (Pointer) (&pc->body.busy.data);

		if (stoMustTag) {
			sect = sectFor((Pointer) pc);
			qi   = qmNo(pc, sect);
			sect->info[qi] = QmInfoMake(QmBusyFirst, code);
		}

		stoTally(stoBytesAlloc += pc->nbytesThis - MxMemHeadSize);
		newFill (p,               pc->nbytesThis - MxMemHeadSize);
	}


	/* Canonicalise the pointer and return it */
	ap = (MostAlignedType *) ptrCanon(p);
	stoWatchAlloc(ap, nbytes);
	return ap;
}


void
stoFree(Pointer p)
{
	int	si;
	Length	qi;
	Section *sect;

	if (p==0) return; /* TTT */
	if (!stoIsInit || !isInHeap(p) || (sect = sectFor(p)) == 0) {
		(*stoError)(StoErr_FreeBad);
		return;
	}
	stoWatchFree(p);

	if (sect->isFixed) {
		FxMem	*pc = (FxMem *) p;
		si		= sect->qmSizeIndex;
		if (stoMustTag) {
#ifdef STO_DIVISION_BY_LOOKUP
		        qi = (sect->qmLog) ? qmLogNo(p, sect) :  qmDivNo(p, sect); /* TTT */
			assert(qi == qmNo(p, sect));
#else
			if (sect->qmLog)
				qi = qmLogNo(p, sect);
			else
				qi = qmNo(p, sect);
#endif
			if (QmInfoKind(sect->info[qi]) != QmBusyFirst)
				(*stoError)(StoErr_FreeBad);
			sect->info[qi] = QmInfoMake0(QmFreeFirst);
		}
		stoTally(stoBytesFree += fixedSize[si]);
		fxmemCleanBody(pc, fixedSize[si]);
		pc->next	= fixedPieces[si];
		fixedPieces[si] = pc;
	}
	else {
		MxMem	*pc = (MxMem *) ptrOff((char *) p, -(long)MxMemHeadSize);
#ifdef STO_LONGJMP
		if (setjmp(stoAllocInner_ErrorCatch))
			return;
#endif
		if (stoMustTag) {
			qi   = qmNo(p, sect);
			if (QmInfoKind(sect->info[qi]) != QmBusyFirst)
				(*stoError)(StoErr_FreeBad);
			sect->info[qi] = QmInfoMake0(QmFreeFirst);
		}
		stoTally(stoBytesFree += pc->nbytesThis - MxMemHeadSize);
		mxmemCleanBody(pc, pc->nbytesThis);
		piecePutMixed(pc);
	}
}

/*
 * Return the actual size of the piece pointed to.
 */
ULong
stoSize(Pointer p)
{
	Section *sect;

	if (!stoIsInit || !isInHeap(p) || (sect = sectFor(p)) == 0) {
		(*stoError)(StoErr_UsedNonalloc);
		return 0;
	}
	if (sect->isFixed)
		return sect->qmSize;
	else {
		MxMem *pc = (MxMem *) ((char *) p - MxMemHeadSize);
		return pc->nbytesThis - MxMemHeadSize;
	}
}

unsigned
stoCode(Pointer p)
{
	if (stoMustTag) {
		Length	qi;
		Section *sect;

		if (!stoIsInit || !isInHeap(p) || (sect = sectFor(p)) == 0)
			return 0;
		qi = qmNo(p, sect);
		if (QmInfoKind(sect->info[qi]) == QmBusyFirst)
			return QmInfoCode(sect->info[qi]);
	}
	return 0;
}


MostAlignedType *
stoRecode(Pointer p, unsigned code)
{
	if (stoMustTag) {
		Section *sect;
		Length	qi;

		if (!stoIsInit || !isInHeap(p) || (sect = sectFor(p)) == 0)
			return (*stoError)(StoErr_UsedNonalloc);

		qi = qmNo(p, sect);
		QmInfoSetCode(sect->info[qi], code);
	}
	return (MostAlignedType *) p;
}


MostAlignedType *
stoResize(Pointer p, ULong nbytes)
{
	Section *sect;
	Pointer np;
	int	oc = 0;
	ULong	nsz, osz;

	if (!stoIsInit || !isInHeap(p) || (sect = sectFor(p)) == 0)
		return (*stoError)(StoErr_UsedNonalloc);

	/* true size of old piece */
	if (sect->isFixed)
		osz = sect->qmSize;
	else {
		MxMem *pc = (MxMem *) ((char *) p - MxMemHeadSize);
		osz = pc->nbytesThis - MxMemHeadSize;
	}

	/* true size of new piece */
	if (nbytes <= FixedSizeMax)
		nsz = fixedSizeFor[nbytes];
	else {
		nsz  = ROUND_UP(nbytes + MxMemHeadSize, MixedSizeQuantum);
		nsz -= MxMemHeadSize;
	}

	if (osz == nsz) return (MostAlignedType *) ptrCanon(p);
	if (stoMustTag) oc = QmInfoCode(sect->info[qmNo(p, sect)]);
	np = (Pointer) stoAlloc(oc, nbytes);
	memcpy(np, p, MIN(nbytes, osz));
	stoFree(p);

	return (MostAlignedType *) ptrCanon(np);
}

int
stoIsPointer(Pointer p)
{
	if (stoMustTag) {
		Section *sect;
		Length	qi;

		if (!stoIsInit || !isInHeap(p) || (sect = sectFor(p)) == 0)
			return 0;
		qi = qmNo(p, sect);
		return QmInfoKind(sect->info[qi]) == QmBusyFirst;
	}
	else {
		if (!stoIsInit || !isInHeap(p)) return 0;
		return 1;
	}
}

#define STO_SHOW_PAGES		0x00000001
#define STO_SHOW_MEMORY		0x00000002
#define STO_SHOW_OVERHEAD	0x00000004
#define STO_SHOW_RESERVED	0x00000008
#define STO_SHOW_FIXED		0x00000010
#define STO_SHOW_MIXED		0x00000020
#define STO_SHOW_PAGEMAP	0x00000040
#define STO_SHOW_PAGEKEY	0x00000080
#define STO_SHOW_MEMMAP		0x00000100
#define STO_SHOW_SHOW		0x00000200
#define STO_SHOW_CENSUS		0x00000400
#define STO_SHOW_USAGE		0x00000800
#define STO_SHOW_ALL		((-1) & ~STO_SHOW_PAGEKEY & ~STO_SHOW_SHOW)
#define STO_SHOW_BEFORE_MASK	(~(STO_SHOW_CENSUS | STO_SHOW_USAGE))

int
stoShowArgs(char *detail)
{
	int r = 0x00;


	/* NULL argument means no details */
	if (!detail) return 0;


	/* Treat the argument as a list of words */
	while (*detail)
	{
		char *w;
		int l;

		/* Skip any leading whitespace or commas */
		while (*detail && (isspace(*detail) || (*detail == ',')))
			detail++;


		/* Note the starting position of the word */
		if (*detail)	w = detail;
		else		break; /* At EOS - stop */


		/* Locate the end of the word */
		while (*detail && !isspace(*detail) && (*detail != ','))
			detail++;


		/* How long is the word? */
		l = detail - w;


		/* Do we recognise it? */
		if      (!strncmp(w,"pages",l))    r |= STO_SHOW_PAGES;
		else if (!strncasecmp(w,"memory",l))   r |= STO_SHOW_MEMORY;
		else if (!strncasecmp(w,"overhead",l)) r |= STO_SHOW_OVERHEAD;
		else if (!strncasecmp(w,"reserved",l)) r |= STO_SHOW_RESERVED;
		else if (!strncasecmp(w,"fixed",l))    r |= STO_SHOW_FIXED;
		else if (!strncasecmp(w,"mixed",l))    r |= STO_SHOW_MIXED;
		else if (!strncasecmp(w,"pagemap",l))  r |= STO_SHOW_PAGEMAP;
		else if (!strncasecmp(w,"pagekey",l))  r |= STO_SHOW_PAGEKEY;
		else if (!strncasecmp(w,"memmap",l))   r |= STO_SHOW_MEMMAP;
		else if (!strncasecmp(w,"census",l))   r |= STO_SHOW_CENSUS;
		else if (!strncasecmp(w,"usage",l))    r |= STO_SHOW_USAGE;
		else if (!strncasecmp(w,"all",l))      r |= STO_SHOW_ALL;
		else if (!strncasecmp(w,"show",l))
		{
			(void)fprintf(osStderr,
	"\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n\n",
	"Possible values are a space or comma separated list of:",
	"        all: select all options (except pagekey and show)",
	"     census: display object counts/sizes by type",
	"      fixed: classification and count of free fixed-size pages",
	"     memmap: display the stack, heap, idata and ddata limits",
	"     memory: amount of store allocated, owned etc",
	"      mixed: classification and count of free mixed-size pages",
	"   overhead: memory overhead of the storage manager",
	"    pagekey: display the symbols used in the pagemap display",
	"    pagemap: tabular display of the page map (see also pagekey)",
	"      pages: classification and count of store pages",
	"   reserved: amount of free memory in reserve",
	"       show: display this message",
	"      usage: fixed-sized pages free/busy usage");
		}
	}
	fflush(osStderr);


	/* Return the flags */
	return r;
}


void
stoGc(void)
{
	static int doShow = -1;

	if (doShow == -1)
		doShow = stoShowArgs(osGetEnv("GC_DETAIL"));
	if (stoMustTag) {
		static Bool inGc = false;
		if (inGc) return;
		tmStart(stoGcTimer());
		if (DEBUG(sto)) {
			if (doShow) {
				/* Census taking is special */
				if (doShow & STO_SHOW_CENSUS)
					stoTakeCensus(1);


				/* Some things not shown before GC */
				stoShowDetail(doShow & STO_SHOW_BEFORE_MASK);
			}
		}
		inGc = true;
		stoGcMarkAndSweep();
		inGc = false;
		if (DEBUG(sto)) {
			if (doShow) {
				/* Census taking is special */
				if (doShow & STO_SHOW_CENSUS)
					stoTakeCensus((AInt)0);


				/* Allowed to show census this time */
				stoShowDetail(doShow);
			}
		}
		tmStop(stoGcTimer());
	}
}

void
stoTune(void)
{
	stoRebuildFreeLists();
}

void
stoShow(void)
{
	stoShowDetail(STO_SHOW_ALL);
}


local ULong
stoTakeFixedCensus(Section *sect, ULong *hist, ULong *mem)
{
	Length	i;
	ULong	nobjs = 0;
	ULong	size = sect->qmSize;
	int	info;


	/* Count all the objects in this section */
	for (i = 0; i < sect->qmCount; i++)
	{
		/* Skip irrevelent quanta */
		if (QmInfoKind(sect->info[i]) != QmBusyFirst)
			continue;


		/* Remember the object type */
		info = QmInfoCode(sect->info[i]);


		/* Increment census slot for this type */
		hist[info]++;


		/* Increase storage count */
		mem[info] += size;


		/* Found another object */
		nobjs++;
	}


	/* Return the number of objects counted. */
	return nobjs;
}


local ULong
stoTakeMixedCensus(Section *sect, ULong *hist, ULong *mem)
{
	Length	i;
	ULong	nobjs = 0;
	ULong	size;
	int	kind;
	int	info;
	MxMem	*pc;


	/* Count all the objects in this section */
	for (i = 0; i < sect->qmCount; i++)
	{
		/* Get the type of quanta */
		kind = QmInfoKind(sect->info[i]);


		/* Skip irrevelent quanta */
		if (kind != QmBusyFirst)
			continue;


		/* Compute the address of the piece */
		pc = (MxMem *)ptrOff((char *)sect->data, i*sect->qmSize);


		/* Read the size of the data (may cover several quanta) */
		size = pc->nbytesThis;


		/* Remember the object type */
		info = QmInfoCode(sect->info[i]);


		/* Increment census slot for this type */
		hist[info]++;


		/* Increase storage count */
		mem[info] += size;


		/* Found another object */
		nobjs++;
	}


	/* Return the number of objects counted. */
	return nobjs;
}


/*
 * This function can be used to display a table of information about
 * different types of objects which are present in the store. The
 * number of objects before and after garbage collection is computed
 * along with the amount of memory they use.
 */
void
stoTakeCensus(AInt before)
{
	Length	i;
	ULong	nobjs = 0L;
	ULong	*hist, *mem;


	/* Which table are we filling? */
	hist = before ? censusBefore : censusAfter;
	mem  = before ? censusMemBefore : censusMemAfter;


	/* Reset the histograms */
	for (i = 0;i < STO_CENSUS_LIMIT; i++)
		hist[i] = mem[i] = 0L;


	/* Scan the heap counting number and size of objects */
	for (i = 0; i < pgMapSize; i++)
	{
		if (pgMap[i] == PgBusyFirst)
		{
			Section *sect = sectAt(i);


			/* Census depends on section type */
			if (sect->isFixed)
				nobjs += stoTakeFixedCensus(sect, hist, mem);
			else
				nobjs += stoTakeMixedCensus(sect, hist, mem);
		}
	}


	/* Record the total number of objects found */
	hist[STO_CENSUS_LIMIT - 1] = nobjs;
}

static String stoCensusDivider = "---------------------------------------------------------------------------------";

static String stoCensusBlank = "                                                                                 ";

local void
stoShowCensusBar(int hires)
{
	String	line = stoCensusDivider;
	String	blank = stoCensusBlank;


	/* Decide what sort of bar to draw */
	if (hires)
	{
		/* Field name */
		(void)fprintf(osStderr, "+-");
		(void)fprintf(osStderr, "%10.10s-+-", line);


		/* Object counts: total, live and swept */
		(void)fprintf(osStderr, "%9.9s-+-", line);
		(void)fprintf(osStderr, "%9.9s-+-", line);
		(void)fprintf(osStderr, "%9.9s-+-", line);


		/* Object sizes: total, live and swept */
		(void)fprintf(osStderr, "%6.6s-+-", line);
		(void)fprintf(osStderr, "%6.6s-+-", line);
		(void)fprintf(osStderr, "%6.6s-+", line);
	}
	else
	{
		/* Field name */
		(void)fprintf(osStderr, "  ");
		(void)fprintf(osStderr, "%10.10s +-", blank);


		/* Object counts: total, live and swept */
		(void)fprintf(osStderr, "%33.33s-+-", line);


		/* Object sizes: total, live and swept */
		(void)fprintf(osStderr, "%23.23s--+", line);
	}


	/* Finish the line */
	(void)fprintf(osStderr, "\n");
}


local void
stoShowCensusTitle(void)
{
	/* Start the title */
	stoShowCensusBar(int0);


	/* Field name */
	(void)fprintf(osStderr, "  ");
	(void)fprintf(osStderr, "%-10.10s | ", "");


	/* Object counts: total, live and swept */
	(void)fprintf(osStderr, "%-32.32s  | ", "        Number of objects");


	/* Object sizes: total, live and swept */
	(void)fprintf(osStderr, "%-23.23s  |", "      Memory used (K)");


	/* Finish the line */
	(void)fprintf(osStderr, "\n");


	/* Start the title */
	stoShowCensusBar(1);


	/* Field name */
	(void)fprintf(osStderr, "| ");
	(void)fprintf(osStderr, "%-10.10s | ", "  Type");


	/* Object counts: total, live and swept */
	(void)fprintf(osStderr, "%-9.9s | ", "   Total");
	(void)fprintf(osStderr, "%-9.9s | ", "    Live");
	(void)fprintf(osStderr, "%-9.9s | ", "   Swept");


	/* Object sizes: total, live and swept */
	(void)fprintf(osStderr, "%-6.6s | ", " Total");
	(void)fprintf(osStderr, "%-6.6s | ", "  Live");
	(void)fprintf(osStderr, "%-6.6s |",  " Swept");


	/* Finish the line */
	(void)fprintf(osStderr, "\n");
	stoShowCensusBar(1);
}


local void
stoShowCensus(AInt width)
{
	Length	i;
	ULong	tmem = 0L;
	ULong	lmem = 0L;


	/* Compute the total store usages */
	for (i = 0;(i < STO_CENSUS_LIMIT - 1); i++)
	{
		tmem += censusMemBefore[i];
		lmem += censusMemAfter[i];
	}


	/* Shove it in the total slot */
	censusMemBefore[STO_CENSUS_LIMIT - 1] = tmem;
	censusMemAfter[STO_CENSUS_LIMIT - 1] = lmem;


	/* Display a nice title bar */
	stoShowCensusTitle();


	/* Now show the bars of the histogram with titles */
	for (i = 0;i < STO_CENSUS_LIMIT; i++)
	{
		ULong	tsize = censusMemBefore[i] / 1024;
		ULong	lsize = censusMemAfter[i] / 1024;
		ULong	tobjs = censusBefore[i];
		ULong	lobjs = censusAfter[i];
		int	last = (i == (STO_CENSUS_LIMIT - 1));


		/* The last line shows the totals */
		if (last) stoShowCensusBar(1);


		/* Field name */
		(void)fprintf(osStderr, "| ");
		(void)fprintf(osStderr, "%10.10s | ", censusName[i]);


		/* Object counts: total, live and swept */
		(void)fprintf(osStderr, "%9ld | ", tobjs);
		(void)fprintf(osStderr, "%9ld | ", lobjs);
		(void)fprintf(osStderr, "%9ld | ", tobjs - lobjs);


		/* Object sizes: total, live and swept */
		(void)fprintf(osStderr, "%6ld | ", tsize);
		(void)fprintf(osStderr, "%6ld | ", lsize);
		(void)fprintf(osStderr, "%6ld |", tsize - lsize);


		/* Finish the row */
		(void)fprintf(osStderr, "\n");
	}


	/* Finish with the bottom line */
	stoShowCensusBar(1);
	(void)fprintf(osStderr, "\n\n");
}


void
stoShowDetail(int stoDetail)
{
	struct osMemMap **mm;
	Length	i;
	ULong	npgBusy=0, npgFree=0, npgStoMan=0, npgForeign=0, npgOther=0;
	ULong	pgOHead, pcOHead, avPc;
	ULong	frN[FixedSizeCount];
	String	sep = stoCensusDivider;


	/*
	 * If we are showing some output other than the census
	 * then we provide a simple separator to help the user.
	 */
	if (stoDetail & ~STO_SHOW_CENSUS)
		(void)fprintf(osStderr, "+%76.76s\n", sep);


	/* Classify and count the number of pages in the store */
	for (i = 0; i < pgMapSize; i++)
	{
		switch (pgMap[i])
		{
		   case PgBusyFirst:	/* Fall through */
		   case PgBusyFollow:	npgBusy++;    break;
		   case PgFree:		npgFree++;    break;
		   case PgPgMap:	/* Fall through */
		   case PgBTree:	/* Fall through */
		   case PgDLL:		npgStoMan++;  break;
		   case PgForeign:	npgForeign++; break;
		   default:		npgOther++;   break;
		}
	}


	/* Show the number of different pages in the store */
	if (stoDetail & STO_SHOW_PAGES)
	{
		fprintf(osStderr, "| %-11s %ld: %ld busy, %ld free, ",
			"Pages:", (long) pgMapSize, (long) npgBusy, (long) npgFree);
		fprintf(osStderr, "%ld overhead, %ld foreign, %ld other\n",
			npgStoMan, npgForeign, npgOther);
	}


	/* Show how much store has been allocated/freed/owned */
	if (stoDetail & STO_SHOW_MEMORY)
	{
		fprintf(osStderr, "| %-11s %ld alloc, %ld freed, %ld owned, ",
			"KBytes:",
			(stoBytesAlloc + 512)/1024,
			(stoBytesFree + 512)/1024,
			(stoBytesOwn + 512)/1024);
		fprintf(osStderr, "%ld blacklisted",
			(stoBytesBlack + 512)/1024);
		fprintf(osStderr, "\n");
	}


	/* Compute the overheads imposed by the storage manager */
	pgOHead	  = pgMapSize * SectionHeadSize
		  + npgStoMan * PgSize;
	pcOHead	  = 0;
	for (i = 0; i < pgMapSize; i++)
		if (pgMap[i] == PgBusyFirst) {
			Section *sect = sectAt(i);
			pcOHead += sect->qmCount * sizeof(QmInfo);
		}


	/* Show how much overhead the storage manager is using */
	if (stoDetail & STO_SHOW_OVERHEAD)
	{
		fprintf(osStderr, "| %-11s (%ld pg bytes + %ld pc bytes)/",
			"Overhead:", pgOHead, pcOHead);
		fprintf(osStderr, "%ld owned => %d%%\n",
			stoBytesOwn,
			(int)((100*pgOHead + 100*pcOHead)/stoBytesOwn));
	}


	/* How much storage is being held in reserve? */
	avPc = 0;
	for (i = 0; i < FixedSizeCount; i++) {
		FxMem	*pc;
		Length	sz = fixedSize[i];
		frN[i] = 0;
		for (pc = fixedPieces[i]; pc; pc = pc->next) {
			frN[i] ++;
			avPc   += sz;
		}
	}


	/* Show the amount of store in reserve */
	if (stoDetail & STO_SHOW_RESERVED)
	{
		fprintf(osStderr, "| %-11s %ld in raw pgs, ",
			"Reserve:", npgFree * PgSize);
		fprintf(osStderr, "%ld in free lists (equiv %ld pgs)\n",
			avPc, QUO_ROUND_UP(avPc, PgSize));
	}


	/* Show the details of fixed-size pages */
	if (stoDetail & STO_SHOW_FIXED)
	{
		int	wrote = 0;

		fprintf(osStderr, "| %-11s %d\n", "FixedSizes:", (int) FixedSizeCount);
		fprintf(osStderr, "| Fixed-size free pieces:\n");
		wrote = fprintf(osStderr, "|  ");
		for (i = 0; i < FixedSizeCount; i++)
		{
			if (frN[i])
			{
				wrote += fprintf(osStderr, " %ldx%d",
						frN[i], (int) fixedSize[i]);
				if (wrote > 70)
				{
					/* Wrap long lines */
					(void)fprintf(osStderr, "\n");
					wrote = fprintf(osStderr, "|  ");
				}
			}
		}
		fprintf(osStderr, "\n");
	}


	/* Show the usage counts for fixed-size pages */
	if (stoDetail & STO_SHOW_USAGE)
	{
		ULong	tot;
		ULong	spareMixedBytes, availMixedBytes, totalMixedBytes;
		ULong	nMixedFreeK, nMixedBusyK, nMixedSpareK;
		double	gceff, gcnum, gcden;
		double	headroom;


		/* Total number of pages under our control */
		tot = npgBusy + npgFree + npgStoMan + npgOther;


		/* How much headroom do we have? */
		if (tot > 0)
			headroom = (double)npgFree/(double)tot;
		else
			headroom = 1.0;


		/* How much headroom do we need? */
		gcnum = (double)GcEffectiveFactorNum;
		gcden = (double)GcEffectiveFactorDen;
		gceff = gcnum / gcden;


		/* Neatly tabulate the results */
		(void)fprintf(osStderr, "| %-11s %.1f%% (%s = %.1f%%, %s)\n",
			"Headroom:", 100.0*headroom, "gcEff", 100.0*gceff,
			(headroom < gceff) ? "too low" : "okay");
		(void)fprintf(osStderr, "| Fixed-size page usage:\n");
		(void)fprintf(osStderr, "|      ");
		(void)fprintf(osStderr, "%4s: ", "Size");
		(void)fprintf(osStderr, "%8s ", "Free");
		(void)fprintf(osStderr, "%8s ", "Busy");
		(void)fprintf(osStderr, "%8s ", "Spare");
		(void)fprintf(osStderr, "%7s",  "HeadRm");
		(void)fprintf(osStderr, "\n");

		for (i = 0; i < FixedSizeCount; i++)
		{
			ULong fxFree  = freeFixedPieces[i];
			ULong fxBusy  = busyFixedPieces[i];
			ULong fxPer   = sectQmCount(1L, fixedSize[i]);
			ULong fxSpare = fxPer*npgFree;
			ULong fxAvail = fxFree + fxSpare;
			ULong fxTotal = fxBusy + fxAvail;
			double	headrm;

			if (fxTotal > 0)
				headrm = (double)fxAvail/(double)fxTotal;
			else
				headrm = 1.0;

			(void)fprintf(osStderr, "|      ");
			(void)fprintf(osStderr, "%4ld: ", (long) fixedSize[i]);
			(void)fprintf(osStderr, "%8lu ", fxFree);
			(void)fprintf(osStderr, "%8lu ", fxBusy);
			(void)fprintf(osStderr, "%8lu ", fxSpare);
			(void)fprintf(osStderr, "%6.1f%%", 100.0*headrm);
			(void)fprintf(osStderr, "\n");
		}


		/* Compute mixed-size usage */
		spareMixedBytes = npgFree*(PgSize - MxMemHeadSize);
		availMixedBytes = freeMixedBytes + spareMixedBytes;
		totalMixedBytes = availMixedBytes + busyMixedBytes;


		/* Convert bytes into K for readability */
		nMixedFreeK  = freeMixedBytes / 1024;
		nMixedBusyK  = busyMixedBytes / 1024;
		nMixedSpareK = spareMixedBytes / 1024;


		/* Compute headroom */
		if (totalMixedBytes > 0)
		{
			double num = (double)availMixedBytes;
			double den = (double)totalMixedBytes;
			headroom = num/den;
		}
		else
			headroom = 1.0;


		/* Show mixed-size usage. */
		(void)fprintf(osStderr, "| Mixed-size page usage: ");
		(void)fprintf(osStderr, "%luK free, ", nMixedFreeK);
		(void)fprintf(osStderr, "%luK busy, ", nMixedBusyK);
		(void)fprintf(osStderr, "%luK spare, ", nMixedSpareK);
		(void)fprintf(osStderr, "%.1f%% headroom\n", 100.0*headroom);
	}


	/* Show details about mixed-size pages */
	if (stoDetail & STO_SHOW_MIXED)
	{
		fprintf(osStderr, "| Mixed-size free pieces:\n|  ");
		btreeNMap((BTreeEltFun) mxmemShow, mixedPieces);
		putc('\n',osStderr);
	}


	/* Display the whole store at page resolution */
	if (stoDetail & STO_SHOW_PAGEMAP)
	{
		fprintf(osStderr, "| %-11s [%ld pages]:",
			"Page map:", (long) pgMapSize);
		for (i = 0; i < pgMapSize; i++)
		{
			if (i % 64 == 0)
			{
				fprintf(osStderr, "\n|%5ld-%5ldK ",
					(i) * PgSize/1024,
					(i+63) * PgSize/1024);
			}

			switch (pgMap[i])
			{
			   case PgFree:		putc('+',osStderr); break;
			   case PgBusyFirst:
			   {
				Section *sect = sectAt(i);

				if (!sect->isFixed)
					putc('m',osStderr);
				else if (sect->qmSizeIndex < 10)
					putc('0' + sect->qmSizeIndex,osStderr);
				else if (sect->qmSizeIndex < 16)
					putc('a' + sect->qmSizeIndex - 10,osStderr);
				else
					putc('>',osStderr);
				break;
			   }
			   case PgBusyFollow:	putc('.',osStderr); break;
			   case PgForeign:	putc('/',osStderr); break;
			   case PgPgMap:	putc('G',osStderr); break;
			   case PgBTree:	putc('T',osStderr); break;
			   case PgDLL:		putc('L',osStderr); break;
			   default:		putc('?',osStderr); break;
			}
		}

		putc('\n',osStderr);
	}


	/* Key to the symbols used in the pagemap display */
	if (stoDetail & STO_SHOW_PAGEKEY)
	{
		fprintf(osStderr, "| %-11s\n", "Symbols:");
		fprintf(osStderr, "| %8s : %s\n",
			"+", "free page");
		fprintf(osStderr, "| %8s : %s\n", 
			"[0-9]", "fixed-size pages by piece size");
		fprintf(osStderr, "| %8s : %s\n", 
			"[a-f]", "fixed-size pages by piece size");
		fprintf(osStderr, "| %8s : %s\n", 
			">", "fixed-size pages with large pieces");
		fprintf(osStderr, "| %8s : %s\n", 
			"m", "mixed-size page");
		fprintf(osStderr, "| %8s : %s\n", 
			".", "mixed-size pages for rest of large object");
		fprintf(osStderr, "| %8s : %s\n", 
			"/", "foreign (not owned by us)");
		fprintf(osStderr, "| %8s : %s\n", 
			"G", "holds the page map");
		fprintf(osStderr, "| %8s : %s\n", 
			"T", "btree storage (mixed-page housekeeping)");
		fprintf(osStderr, "| %8s : %s\n", 
			"L", "dll storage (mixed-page housekeeping)");
		fprintf(osStderr, "| %8s : %s\n", 
			"?", "unknown page type");
	}


	/* Display the memory map of this process */
	if (stoDetail & STO_SHOW_MEMMAP)
	{
                fprintf(osStderr, "| Heap....... [%p..%p) %ldK\n",
                        heapStart, heapEnd,
                        ptrDiff(heapEnd, heapStart)/1024);

                for (mm=osMemMap(OSMEM_DDATA); (*mm)->use != OSMEM_END; mm++)
                        fprintf(osStderr, "| Dyn memory: [%p..%p) %ldK\n",
                                (*mm)->lo, (*mm)->hi,
                                ptrDiff((*mm)->hi, (*mm)->lo)/1024);
                for (mm=osMemMap(OSMEM_IDATA); (*mm)->use != OSMEM_END; mm++)
                        fprintf(osStderr, "| Init data:  [%p..%p) %ldK\n",
                                (*mm)->lo, (*mm)->hi,
                                ptrDiff((*mm)->hi, (*mm)->lo)/1024);
                for (mm=osMemMap(OSMEM_STACK); (*mm)->use != OSMEM_END; mm++)
                        fprintf(osStderr, "| Stack:      [%p..%p) %ldbytes\n",
                                (*mm)->lo, (*mm)->hi,
                                ptrDiff((*mm)->hi, (*mm)->lo));
	}


	/*
	 * If we are showing some output other than the census
	 * then we provide another separator to help the user.
	 */
	if (stoDetail & ~STO_SHOW_CENSUS)
		(void)fprintf(osStderr, "+%76.76s\n\n\n", sep);


	/* Census of object types */
	if (stoDetail & STO_SHOW_CENSUS)
		stoShowCensus(20);
}


/*
 * Test consistency of storage management structures.
 */
void
stoAudit(void)
{
#ifndef NDEBUG
	stoAuditAll();
#endif
}


/*
 * Control storage manager behaviour.
 */

int
stoCtl(int cmd, ...)
{
	va_list	argp;
	int	rc;

	va_start(argp, cmd);
	rc = 0;

	switch (cmd) {
	case StoCtl_GcLevel:
		if (gcLevel != StoCtl_GcLevel_Never)
			gcLevel = va_arg(argp, int);
		if (gcLevel == StoCtl_GcLevel_Never)
			stoMustTag = false;
		break;
	case StoCtl_GcFile:
		gcTraceFile   = va_arg(argp, FILE *);
		break;
	case StoCtl_Wash:
		stoMustWash   = va_arg(argp, Bool);
		break;
	default:
		rc = -1;
	}

	va_end(argp);
	return rc;
}


void
stoRegister(StoInfo info)
{
	if (info->code >= OB_MAX)
		bug("Too many object types");

	stoObRegistered[info->code] = 1;
	stoObNoInternalPtrs[info->code] = !info->hasPtrs;
}


/*
 * Classify the specified pointer. This code is very shaky and needs
 * to be checked more thoroughly. Do pointer comparisons work here?
 */
int
stoWritablePointer(Pointer p)
{
#if defined(OS_WIN32)
	/* Win32 gives us a function specifically for this task */
	return IsBadWritePtr(p, sizeof(long)) ? POINTER_IS_INVALID : POINTER_IS_VALID;
#else
	struct osMemMap **mm;
	int	n;

	/*
	 * Check if the target is in the heap. Unless the
	 * pointer is invalid then this is the most likely
	 * place to find the target object. We assume that
	 * all areas of the heap are writable.
	 */
	if (isInHeap(p)) return POINTER_IS_VALID;


	/* Get the memory mappings */
	mm = osMemMap(OSMEM_STACK | OSMEM_IDATA | OSMEM_DDATA);


	/* Can we classify this pointer at all? */
	if (!mm) return POINTER_IS_UNKNOWN;


	/*
	 * Scan the whole memory map to find out where the
	 * target of this pointer lies. We assume that the
	 * stack, static and dynamic areas are all writable.
	 */
	for (n = 0 ; (*mm)->use != OSMEM_END; mm++) {
		switch ((*mm)->use) {
		case OSMEM_STACK:
			/* Is the object in this segment? */
			if (p < (Pointer)((*mm)->lo)) break;
			if (p > (Pointer)((*mm)->hi)) break;
			return POINTER_IS_VALID;
		case OSMEM_IDATA:
			/* Is the object in this segment? */
			if (p < (Pointer)((*mm)->lo)) break;
			if (p > (Pointer)((*mm)->hi)) break;
			return POINTER_IS_VALID;
		case OSMEM_DDATA:
			/* Is the object in this segment? */
			if (p < (Pointer)((*mm)->lo)) break;
			if (p > (Pointer)((*mm)->hi)) break;
			return POINTER_IS_VALID;
			break;
		  default:
			assert(false); /*mem type is silly*/
			break;
		}
	}


	/* Can't find that pointer - assume invalid */
	return POINTER_IS_INVALID;
#endif
}


/*
 * This is a somewhat dodgy function and is really only
 * here for experimentation. It allows an Aldor domain
 * to provide a call-back for marking itself during GC.
 * Unless values from the domain occupy a lot of storage
 * space with very few pointers it is extremely unlikely
 * that this call back will be efficient. Not only that
 * but the user must be extremely careful not to miss a
 * potential pointer.
 */
void
stoSetAldorTracer(int code, Pointer clos)
{
	if (code >= OB_MAX)
		bug("Too many object types");

	if (!stoObRegistered[code])
		bug("Object type not registered");

	stoObAldorTracer[code] = (StoFiClos)clos;
}


/*
 * This function performs exactly the same job as
 * stoRegisterAldorTracer but is designed to be
 * called from C.
 */
void
stoSetTracer(int code, StoTraceFun fun)
{
	if (code >= OB_MAX)
		bug("Too many object types");

	if (!stoObRegistered[code])
		bug("Object type not registered");

	stoObCTracer[code] = fun;
}


/*
 * Given a pointer, try to mark the object pointed to.
 * IMPORTANT: the code here must perform exactly the same
 *            checks as the code in stoGcMarkRange(). We
 *            don't do blacklisting though.
 */
int
stoMarkObject(Pointer p)
{
	Pointer		*plo, *phi;
	static int	pgno, qmno;
	static PgInfo	pgtag;
	static QmInfo	qmtag;
	static Section *sect;
	int		n = 0;
	int		oldStoMarkArea = stoMarkArea;


	/* Verify pointer is into heap. */
	if (!isInHeap(p)) return 0;
	if (DEBUG(sto)) {stoMarkCount[stoMarkArea][PTR_INTO_HEAP]++;}


	/* Verify pointer is to busy page. */
	pgno  = pgNo(p);
	pgtag = pgMap[pgno];
	if (pgtag != PgBusyFirst && pgtag != PgBusyFollow) return 0;
	if (DEBUG(sto)) {
		stoMarkCount[stoMarkArea][PTR_INTO_HEAP]--;
		stoMarkCount[stoMarkArea][PTR_INTO_BUSY]++;
	}


	/* Grab section header. */
	while (pgtag == PgBusyFollow) pgtag = pgMap[--pgno];
	sect = sectAt(pgno);


	/* Verify pointed-to quantum. */
	if (ptrLT(p, sect->data)) return 0;
	if (DEBUG(sto)) {
		stoMarkCount[stoMarkArea][PTR_INTO_BUSY]--;
		stoMarkCount[stoMarkArea][PTR_INTO_DATA]++;
	}

		
#ifdef STO_DIVISION_BY_LOOKUP
	/* If interior of object, point to beginning. */
	/* Use shift operations where possible */
	if (sect->isFixed)
	{
	        qmno = (sect->qmLog) ? qmLogNo(p, sect) : qmDivNo(p, sect); /* TTT */
		assert(qmno == qmNo(p, sect));
		p = (sect->qmLog) ? ptrOff((char *) sect->data, qmno << sect->qmLog) : 
		  ptrOff((char *) sect->data, qmno * sect->qmSize);
	}
	else
	{
		qmno  = qmNo(p, sect);
		p = ptrOff((char *) sect->data, qmno * sect->qmSize);
	}
#else
	/* If interior of object, point to beginning. */
	/* Use shift operations where possible */
	if (sect->isFixed && sect->qmLog) {
		qmno = qmLogNo(p, sect);
		p = ptrOff((char *) sect->data, qmno << sect->qmLog);
	}
	else {
		qmno  = qmNo(p, sect);
		/* If interior of object, point to beginning. */
		p = ptrOff((char *) sect->data, qmno * sect->qmSize);
	}
#endif


	/* Verify not already marked. */
	qmtag = sect->info[qmno];
	if (QmInfoMark(qmtag)) return 0;
	if (DEBUG(sto)) {
		stoMarkCount[stoMarkArea][PTR_INTO_DATA]--;
		stoMarkCount[stoMarkArea][PTR_INTO_NEW]++;
	}


	/* Add mark bits quanta in piece.  Determine data bounds. */
	if (sect->isFixed) {
		QmInfoSetMark(sect->info[qmno]);
		plo = (Pointer *) p;
		phi = (Pointer *) ptrOff((char *) plo, sect->qmSize);
	}
	else {
		MxMem	*pc;
		int	sz, nq, qi;

		while (QmInfoKind(qmtag) == QmFollow)
			qmtag = sect->info[--qmno];

		pc = (MxMem *)ptrOff((char *)sect->data,
				     qmno*sect->qmSize);
		sz = pc->nbytesThis;
		nq = sz/sect->qmSize;

		for (qi = qmno; qi < qmno + nq; qi++)
			QmInfoSetMark(sect->info[qi]);

		sz -= MxMemHeadSize;
		plo = (Pointer *) (&pc->body.busy.data);
		phi = (Pointer *) ptrOff((char *) plo, sz);
	}


	/* Verify piece is in use. */
	if (QmInfoKind(qmtag) == QmFreeFirst) {
		if (DEBUG(sto)) {
			stoMarkCount[stoMarkArea][PTR_INTO_NEW]--;
			stoMarkCount[stoMarkArea][PTR_INTO_FREE]++;
		}
		stoGcMarkedFree++;
		return 0;
	}


	/* Check that this contains pointers 
	 * This test should use a bit in the tag, not the code.
	 */
	if (QmIsPtrFree(qmtag)) return 0;


	/* We have marked this piece */
	n++;


#if STO_USER_CAN_TRACE
	/*
	 * Has the creator of this object requested
	 * permission to trace it?
	 */
	if (QmAldorTraced(qmtag))
	{
		/* Invoke the creator's tracer */
		n += (int)stoFiCCall2(int, QmAldorTraced(qmtag), plo, phi);
		return n;
	}
	if (QmCTraced(qmtag))
	{
		/* Invoke the creator's tracer */
		n += (QmCTraced(qmtag))(plo, phi);
		return n;
	}
#endif


	/* Pointer classification */
	if (DEBUG(sto)) {
		if (stoMarkArea < MEM_MAX_TYPE) {
			/* Now marking within heap */
			oldStoMarkArea = stoMarkArea;
			stoMarkArea += MEM_MAX_TYPE;
		}
	}


	/* Count the number of children marked */
	n += stoGcMarkRange(plo, phi, (int) 0);


	/* Pointer classification */
	if (DEBUG(sto)) {stoMarkArea = oldStoMarkArea;}

	return n;
}


/*
 * Exception handling code.
 */

local MostAlignedType *
stoDefaultError(int errnum)
{
	String msg;

	switch (errnum) {
	case StoErr_OutOfMemory:  return 0;
	case StoErr_UsedNonalloc: msg = "using non-allocated space";	  break;
	case StoErr_CantBuild:	  msg = "can't build internal structure"; break;
	case StoErr_FreeBad:      msg = "attempt to free unknown space";  break;
	default:		  msg = "unexpected";			  break;
	}
	fprintf(osStderr, "Storage allocation error (%s).\n", msg);
	exitFailure();
	NotReached(return 0);
}

StoErrorFun
stoSetHandler(StoErrorFun f)
{
	StoErrorFun oldf = stoError;
	stoError = f ? f : (StoErrorFun) stoDefaultError;
	return oldf;
}

/*===========================================================================*/

#else /* not STO_USE_BTREE */

/*
 * Alternate storage management schemes are based on three functions:
 *    stoMore, stoLess, and stoMoreOrLess.
 */

#ifdef STO_USE_MALLOC
extern	Pointer malloc	(Length);
extern	void	free	(Pointer);

# define stoMore(sz)		  ((Pointer) malloc(sz))
# define stoMoreOrLess(p,osz,nsz) ((Pointer) realloc(p, nsz))
# define stoLess(p)		  free(p)
#endif

#ifdef STO_USE_BOEHM
/* uncomment this */
#include <gc/gc.h>

# define stoMore(sz)               ((Pointer) GC_malloc(sz))
# define stoMoreOrLess(p,osz,nsz)  ((Pointer) GC_realloc(p,nsz))
# define stoLess(p)                GC_free(p)
#endif



#ifdef STO_USE_ONCE
local Pointer
stoMore(Length sz)
{
	static long	stoC = 0;
	static char *	stoV;
	Pointer		r;

	sz = ROUND_UP(sz, alignof(MostAlignedType));
	if (sz > stoC) {
		ULong	L = MAX(sz, ((long) 1) << 15);
		Pointer P = osAlloc(&L);
		if (!P) return 0;
		stoC = L; stoV = (char *) P;
	}
	r =  (Pointer) stoV; stoC -= sz; stoV += sz;
	return r;
}
local Pointer
stoMoreOrLess(Pointer p, Length osz, Length nsz)
{
	Pointer		r;

	if (osz >= nsz) return p;
	r = stoMore(nsz);
	if (r) memmove(r, p, osz);
	return r;
}
# define stoLess(p)	((void) (p))
#endif

/*
 * The remainder is common to all alternate implementations.
 */


static Bool    stoMustTag    = false;
 
ULong	       stoBytesOwn   = 0;	/* Total owned by allocator. */
ULong	       stoBytesAlloc = 0;	/* Total ever allocated.  */
ULong	       stoBytesFree  = 0;	/* Total ever freed. */
ULong	       stoBytesGc    = 0;	/* Total ever garbage collected. */
ULong	       stoPiecesGc[STO_CODE_LIMIT];	/* # this time, by code. */

#ifdef STO_TALLY
# define stoTally(expr)		(expr)
#else
# define stoTally(expr)		Nothing
#endif

struct stoHdr {
	char		code;
	unsigned short	magic;
	Length		size;
	MostAlignedType data;
};

# define STO_MAGIC	0xFE20 /* Any unlikely 2 byte sequence. */

# define sizeofHdr	(sizeof(struct stoHdr) - sizeof(MostAlignedType))
# define stoPtrToHdr(p) ((struct stoHdr *)((char *) p - sizeofHdr))
# define stoHdrToPtr(h) ((Pointer) &((h)->data))

local MostAlignedType * stoDefaultError	 (int errnum);
static StoErrorFun	stoError	 = stoDefaultError;

MostAlignedType *
stoAlloc(unsigned code, ULong size)
{
	Length		sz = size + sizeofHdr;
	Pointer		p;
	struct stoHdr	*q;

	if (sz == 0) sz = 1;	/* For malloc. */

	p = stoMore(sz);
	if (!p) return (*stoError)(StoErr_OutOfMemory);

	q = (struct stoHdr *) p;
	p = stoHdrToPtr(q);

	if (stoMustTag) {
		q->code  = code;
		q->magic = STO_MAGIC;
	}
	stoTally(stoBytesAlloc += size);
	q->size	 = sz;
	return (MostAlignedType *) p;
}

void
stoFree(Pointer p)
{
	if (p==NULL)
		return;
	stoTally(stoBytesFree += stoPtrToHdr(p)->size);
	stoLess(stoPtrToHdr(p));
}

ULong
stoSize(Pointer p)
{
	return stoPtrToHdr(p)->size - sizeofHdr;
}

unsigned
stoCode(Pointer p)
{
	int	c = 0;
	if (stoMustTag)
		c = stoPtrToHdr(p)->code;
	return c;
}

MostAlignedType *
stoResize(Pointer p0, ULong size)
{
	struct stoHdr	*q0, *q1;
	Length		nsz = size + sizeofHdr;

	q0 = stoPtrToHdr(p0);
	q1 = (struct stoHdr *) stoMoreOrLess(q0, q0->size, nsz);
	if (!q1) return (*stoError)(StoErr_OutOfMemory);
	q1->size = nsz;

	return (MostAlignedType *) stoHdrToPtr(q1);
}

MostAlignedType *
stoRecode(Pointer p, unsigned code)
{
	if (stoMustTag) stoPtrToHdr(p)->code = code;
	return (MostAlignedType *) p;
}

Bool
stoIsPointer(Pointer p)
{
	int	isMagic = 1;
	if (!p) return 0;
	if (stoMustTag) isMagic = stoPtrToHdr(p)->magic == STO_MAGIC;
	return isMagic;
}

void stoGc			(void)		{ }
void stoTune			(void)		{ }
int  stoShowArgs		(char *detail)	{ return 0; }
void stoShow			(void)		{ }
void stoShowDetail		(int x)		{ }
void stoAudit			(void)		{ }
void stoRegister		(StoInfo info)	{ }
void stoSetAldorTracer		(int code, Pointer clos) {}
void stoSetTracer		(int code, StoTraceFun fun) {}
int  stoMarkObject		(Pointer p)	{ return 0; }
int  stoWritablePointer		(Pointer p)	{ return POINTER_IS_UNKNOWN; }
int  stoCtl			(int cmd, ...)	{ return 0; }

static struct tmTimer gcTimer;
TmTimer stoGcTimer		(void) { return &gcTimer; }


/*
 * Exception handling code.
 */
local MostAlignedType *
stoDefaultError(int errnum)
{
	String msg;
	switch (errnum) {
	case StoErr_OutOfMemory:
		return 0;
	case StoErr_UsedNonalloc:
		msg = "using non-allocated space";
		break;
	case StoErr_CantBuild:
		msg = "can't build internal structure";
		break;
	default:
		msg = "unexpected";
		break;
	}
	fprintf(osStderr, "Storage allocation error (%s).\n", msg);
	exitFailure();
	NotReached(return 0);
}

StoErrorFun
stoSetHandler(StoErrorFun f)
{
	StoErrorFun oldf = stoError;
	stoError = f ? f : (StoErrorFun) stoDefaultError;
	return oldf;
}

#endif /* !STO_USE_BTREE */
