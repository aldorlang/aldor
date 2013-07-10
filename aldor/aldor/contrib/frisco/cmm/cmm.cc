/*---------------------------------------------------------------------------*
 *
 *  cmm.cc:	This module implements the POSSO Customisable Memory Management
 *		(CMM). CMM provides garbage collected storage for C++ programs.
 *  date:	3 January 1995
 *  authors:	Giuseppe Attardi and Tito Flagella
 *  email:	cmm@di.unipi.it
 *  address:	Dipartimento di Informatica
 *		Corso Italia 40
 *		I-56125 Pisa, Italy
 *
 *  Copyright (C) 1993, 1994, 1995 Giuseppe Attardi and Tito Flagella.
 *
 *  This file is part of the PoSSo Customizable Memory Manager (CMM).
 *
 * Permission to use, copy, and modify this software and its documentation is
 * hereby granted only under the following terms and conditions.  Both the
 * above copyright notice and this permission notice must appear in all copies
 * of the software, derivative works or modified versions, and any portions
 * thereof, and both notices must appear in supporting documentation.
 *
 * Users of this software agree to the terms and conditions set forth herein,
 * and agree to license at no charge to all parties under these terms and
 * conditions any derivative works or modified versions of this software.
 * 
 * This software may be distributed (but not offered for sale or transferred
 * for compensation) to third parties, provided such third parties agree to
 * abide by the terms and conditions of this notice.  
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE COPYRIGHT HOLDERS DISCLAIM ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL THE COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 *
 * The technique of the CMM is described in:
 *
 * G. Attardi and T. Flagella ``A customisable memory management
 * framework'', Proceedings of USENIX C++ Conference 1994, Cambridge,
 * Massachusetts, April 1994.
 *
 * The implementation is derived from the code of the "mostly-copying" garbage
 * collection algorithm, by Joel Bartlett, of Digital Equipment Corporation.
 *
 *---------------------------------------------------------------------------*/

/*
 *              Copyright 1990 Digital Equipment Corporation
 *                         All Rights Reserved
 *
 * Permission to use, copy, and modify this software and its documentation is
 * hereby granted only under the following terms and conditions.  Both the
 * above copyright notice and this permission notice must appear in all copies
 * of the software, derivative works or modified versions, and any portions
 * thereof, and both notices must appear in supporting documentation.
 *
 * Users of this software agree to the terms and conditions set forth herein,
 * and hereby grant back to Digital a non-exclusive, unrestricted, royalty-free
 * right and license under any changes, enhancements or extensions made to the
 * core functions of the software, including but not limited to those affording
 * compatibility with other hardware or software environments, but excluding
 * applications which incorporate this software.  Users further agree to use
 * their best efforts to return to Digital any such changes, enhancements or
 * extensions that they make and inform Digital of noteworthy uses of this
 * software.  Correspondence should be provided to Digital at:
 * 
 *                       Director of Licensing
 *                       Western Research Laboratory
 *                       Digital Equipment Corporation
 *                       250 University Avenue
 *                       Palo Alto, California  94301  
 * 
 * This software may be distributed (but not offered for sale or transferred
 * for compensation) to third parties, provided such third parties agree to
 * abide by the terms and conditions of this notice.  
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND DIGITAL EQUIPMENT CORP. DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL DIGITAL EQUIPMENT
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
#include "cmm.h"
#include <setjmp.h>

/* Version tag */
static _CmmInit initMe;

char*  Cmm::version = "CMM 1.6";

/*---------------------------------------------------------------------------*
 *
 * -- Garbage Collected Heap Definitions
 *
 *---------------------------------------------------------------------------*/

/*
 * The heap consists of a discontiguous set of pages of memory, where each
 * page is bytesPerPage long.  N.B.  the page size for garbage collection is
 * independent of the processor's virtual memory page size.
 */

static int   totalPages;	/* # of pages in the heap		*/
static int   heapSpanPages;	/* # of pages that span the heap	*/
int          freePages;	        /* # of pages not yet allocated		*/
static int   freeWords = 0;	/* # words left on the current page	*/
static long  *firstFreeWord;	/* Ptr to the first free word on the current
				   page */
int   	     firstFreePage;	/* First possible free page		*/
static int   queueHead;		/* Head of list of stable set of pages	*/
static int   queueTail;     	/* Tail of list of stable set of pages	*/

int	     firstHeapPage;	/* Page # of first heap page		*/
int	     lastHeapPage;	/* Page # of last heap page		*/
unsigned long *objectMap;	/* Bitmap of objects			*/
#if !HEADER_SIZE || defined(MARKING)
unsigned long *liveMap;		/* Bitmap of live objects		*/
#endif

int	     *pageLink;		/* Page link for each page		*/
short	     *pageSpace;	/* Space number for each page		*/
short	     *pageGroup;	/* Size of group of pages		*/

int	     currentSpace;	/* Current space number			*/
int	     nextSpace;		/* Next space number			*/

CmmHeap      **pageHeap;	/* Heap to which each page belongs	*/

int          tablePages;	/* # of pages used by tables	*/
int          firstTablePage;	/* index of first page used by table	*/

/*---------------------------------------------------------------------------*
 *
 * Groups of pages used for objects spanning multiple pages, are dealt
 * as follows:
 *
 * The first page p0 in a group contains the number n of pages in the group,
 * each of the following pages contains the offset from the first page:
 * 	pageGroup[p0] = n
 * 	pageGroup[p0+1] = -1
 * 	pageGroup[p0+n-1] = 1-n
 * Given a page p, we can compute the first page by p+pageGroup[p] if
 * pageGroup[p] < 0, otherwise p.
 *
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 Tito:
   A new algorithm for garbage collection is used when MARKING is defined.
   The new algorithm is:
   1) Look at the roots to promote a set of pages whose objects cannot
      be moved. Any reachable object is marked as a reached object setting
      its bit into the "liveMap" bitmap to 1.
   2) Scan the promoted pages, traversing all the objects marked as reached.
   Traverse applies the scavenge to any pointer internal to the traversed
   objects. scavenge does:
    - if the pointer is outside the heap do nothing;
    - if the pointer is to an object in another heap traverse that object.
    - if the object has already been reached, then if the object is in 
      a promoted page return, else set the pointer to the forward position.
    - if the object has not yet been reached, then if the object is in 
      a promoted page, mark it and apply traverse to it, else copy the object,
      set the old header to the forward position, and set the forward bit of
      the new object to 0.

   Note that any page allocated to copy reachable objects is added to the
   promoted set. For this reason you don't need to apply traverse to
   moved objects. They will be traversed as they are reachable objects in
   promoted pages.

   3) Reset the mark bit for any object in the promoted pages.

 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 *
 * -- Exported Interface Definitions
 *
 *---------------------------------------------------------------------------*/

int Cmm::verbose = 0;		/* controls amount of printout */

/*
 * An instance of the type Cmm is created to configure the size of the
 * initial heap, the expansion increment, the maximum size of the heap,
 * the allocation percentage to force a total collection, the allocation
 * percentage to force heap expansion, and garbage collection options.
 */

/* Actual heap configuration */

int  Cmm::minHeap  = CMM_MINHEAP; /* # of bytes of initial heap   */
int  Cmm::maxHeap  = CMM_MAXHEAP; /* # of bytes of the final heap */
int  Cmm::incHeap  = CMM_INCHEAP; /* # of bytes of each increment */
int  Cmm::gcThreshold = CMM_GCTHRESHOLD; /* grow w/out GC till this size */
int  Cmm::generational = CMM_GENERATIONAL; /* % allocated to force
					      total collection       */

int  Cmm::incPercent = CMM_INCPERCENT; /* % allocated to force expansion */
int  Cmm::flags      = CMM_FLAGS;      /* option flags		*/
bool Cmm::defaults   = true;	       /* default setting in force*/
bool Cmm::created    = false;	       /* boolean indicating heap created */


Cmm::Cmm(int newMinHeap, int newMaxHeap, int newIncHeap,
	 int newGenerational, int newIncPercent, int newGcThreshold,
	 int newFlags, int newVerbose)
{
  if  (!created
       &&  newMinHeap > 0
       &&  (defaults || newMaxHeap >= maxHeap))
    {
      defaults = false;
      minHeap = newMinHeap;
      maxHeap = newMaxHeap;
      incHeap = newIncHeap;
      generational = newGenerational;
      incPercent = newIncPercent;
      minHeap = MAX(minHeap, 4*bytesPerPage);
      maxHeap = MAX(maxHeap, minHeap);
      if  (generational < 0  ||  generational > 50)
	generational = CMM_GENERATIONAL;
      if  (incPercent < 0  ||  incPercent > 50)	incPercent = CMM_INCPERCENT;
    }
  if  (created
       &&  minHeap > 0
       &&  (defaults || newMaxHeap >= maxHeap))
    {
      defaults = false;
      if  (getenv("CMM_MAXHEAP") == NULL)  maxHeap = newMaxHeap;
      if  (getenv("CMM_INCHEAP") == NULL)  incHeap = newIncHeap;
      if  (getenv("CMM_GENERATIONAL") == NULL)
	generational = newGenerational;
      if  (getenv("CMM_INCPERCENT") == NULL)  incPercent = newIncPercent;
      minHeap = MAX(minHeap, 4*bytesPerPage);
      maxHeap = MAX(maxHeap, minHeap);
      if  (generational < 0 || generational > 50)
	generational = CMM_GENERATIONAL;
      if  (incPercent < 0 || incPercent > 50) incPercent = CMM_INCPERCENT;
    }
  gcThreshold = newGcThreshold;
  flags |= newFlags;
  verbose |= newVerbose;
}

/*
 * Freespace objects have a tag of 0.
 * Pad objects for double alignment have a tag of 1.
 * CmmObjects have a tag of 2.
 * The header for a one-word double alignment pad is kept in doublepad.
 */

#if HEADER_SIZE
static int  freeSpaceTag = MAKE_TAG(0);
# ifdef DOUBLE_ALIGN
static int  doublepad = MAKE_HEADER(1, MAKE_TAG(1));
#define HEADER_ALIGN(firstFreeWord, freeWords) \
      if  ((freeWords & 1) == 0  &&  freeWords)  { \
	  *firstFreeWord++ = doublepad; \
	  freeWords = freeWords - 1; \
	}
# endif  // DOUBLE_ALIGN
#endif // HEADER_SIZE

#ifdef DOUBLE_ALIGN
#define onePageObjWords (wordsPerPage-HEADER_SIZE)
#else
#define onePageObjWords wordsPerPage
#endif

/*---------------------------------------------------------------------------*
 *
 * -- Library initialization
 *
 *---------------------------------------------------------------------------*/

// Declaring a static variable of type _CmmInit here ensure that the _CmmInit
// constructor is called before of main().
static _CmmInit _DummyCmmInit;

/*---------------------------------------------------------------------------*
 *
 * --  Roots
 *
 *---------------------------------------------------------------------------*/

/*
 * The following structure contains the additional roots registered with
 * the garbage collector.  It is allocated from the non-garbage collected
 * heap.
 */

#define	    rootsIncrement 10
static int  rootsCount = 0;
static int  rootsSize = 0;
static int  freedEntries = 0;

static struct
RootsStruct
{
  GCP	     addr;		/* Address of the roots */
  int  	     bytes;		/* Number of bytes in the roots */ 

}  * roots;


/*---------------------------------------------------------------------------*
 * -- gcRoots
 *
 * Additional roots are "registered" with the garbage collector by the
 * following procedure.
 *
 *---------------------------------------------------------------------------*/

void
gcRoots(void * addr, int bytes)
{
  if (freedEntries)
    {
      for (int i = 0; i < rootsCount; i++)
	if (roots[i].addr == 0)
	  {
	    roots[i].addr = (GCP)addr;
	    roots[i].bytes = bytes;
	    freedEntries--;
	  }
    }
  if (rootsCount == rootsSize)
    {
      RootsStruct  *np;
      rootsSize += rootsIncrement;
      np = new RootsStruct[rootsSize];
      for (int i = 0; i < rootsCount; i++)
	np[i] = roots[i];
      delete  roots;
      roots = np;
    }
  roots[rootsCount].addr = (GCP)addr;
  roots[rootsCount].bytes = bytes;
  rootsCount++;
}

void
gcUnroots(void* addr)
{
  int i;

  for (i = 0; i < rootsCount; i++) 
    if (roots[i].addr == addr) 
      {
	roots[i].addr = 0;
	freedEntries++;
	break;
      }
  assert(i < rootsCount);
}

/*---------------------------------------------------------------------------*
 * -- environmentValue
 *
 * Get heap configuration information from the environment.
 *
 * Results: true if the value is provided, value in value.
 *
 *---------------------------------------------------------------------------*/

static bool
environmentValue(char *name, int &value)
{
  char* valuestring = getenv(name);

  if (valuestring != NULL)
    {
      value = atoi(valuestring);
      return true;
    }
  else
    return false;
}


#if !HEADER_SIZE
/*
 * Go forward until next object, return the size in words.
 */
int CmmObject::words()
{

  register int length = 1;
  register int index = WORD_INDEX(this+1);
  int shift = BIT_INDEX(this+1);
  register unsigned long bits = objectMap[index] >> shift;
  register int inner = bitsPerWord - shift;
  int nextPage = GCPtoPage(this);
  nextPage += pageGroup[nextPage];
  int max = ((int)pageToGCP(nextPage) - (int)this)
    / (bitsPerWord * bytesPerWord);

  do
    {
      do
	{
	  if (bits & 1) return length;
	  bits = bits >> 1;
	  length++;
	}
      while (--inner);
      bits = objectMap[++index];
      inner = bitsPerWord;
    }
  while (max--);
  /* we fall out here when this is last object on page */
  return (CmmObject *)pageToGCP(nextPage) - this;
}

/* Version using ffs.
 * Counts the number of consecutive 1's in objectMap, which encode
 * half the number of words of the object.
 * We assume that object are an even number of words.
 *
 * int words()
 * {
 *  int length = 0, bits,
 *  index = WORD_INDEX(this),
 *  shift = BIT_INDEX(this);
 *
 *  while (true) {
 *    bits = (unsigned int)objectMap[index] >> shift;
 *    inc = ffs(~bits) - 1;
 *    if (inc < 0) inc = bitsPerWord;
 *    if (inc == (bitsPerWord - shift)) break;
 *    length += inc;
 *    index++;
 *    shift = 0;
 *  }
 *  return 2*length;
 *}
 *
 *  A setobjectMap which goes with this is:
 *
 *setobjectMap(GCP p, int size)
 *{
 *  int index = WORD_INDEX(p),
 *  shift = BIT_INDEX(p);
 *  size = size / 2;
 *  while (true) {
 *    count = size % (bitsPerWord - shift);
 *    objectMap[index] |= (1 << count) - 1;
 *    size -= count;
 *    if (size == 0) break;
 *    index++;
 *    shift = 0;
 *  }
 *}
*/
#endif

/*---------------------------------------------------------------------------*
 *
 * -- Initialization
 *
 *---------------------------------------------------------------------------*/

#if !HEADER_SIZE
/*
 * An object of this class is used to fill free portion of page.
 */
class GcFreeObject: public CmmObject {
  void traverse() {}
  int words() { return wordsPerPage; }
};

static GcFreeObject *aGcFreeObject;

# ifdef DOUBLE_ALIGN_OPTIMIZE
/*---------------------------------------------------------------------------*
 * An object of this class is used for padding.
 *---------------------------------------------------------------------------*/

class GcPadObject: public CmmObject {
  void traverse() {}
  int words() { return 1; }
};

static GcPadObject *aGcPadObject;
# endif
#endif				// HEADER_SIZE

DefaultHeap *Cmm::theDefaultHeap;
UncollectedHeap *Cmm::theUncollectedHeap;
CmmHeap		*Cmm::heap;
CmmHeap		*Cmm::theMSHeap = (CmmHeap*) 100; // 100 to get it working with C

// used during initialization of objects:
static CmmObject	*aCmmObject;
static CmmVarObject	*aCmmVarObject;

static GCP	globalHeapStart; // start of global heap

void
CmmInitEarly()
{
  int i;
  if (stackBottom == 0)
    {
      CmmSetStackBottom((Word)&i);
      /* Determine start of system heap				*/
      globalHeapStart = (GCP)sbrk(0);
    }
}

DefaultHeap::DefaultHeap()
{
  usedPages 	= 0;
  reservedPages	= 0;
  stablePages 	= 0;
}

/*---------------------------------------------------------------------------*
 * -- CmmInit
 *
 * The heap is allocated and the appropriate data structures are initialized
 * by the following function.  It is called the first time any storage is
 * allocated from the heap.
 *
 *---------------------------------------------------------------------------*/

void
CmmInit()
{
  char  *heap;
  int  i;

  /* Log actual heap parameters if from environment or logging */
  if (environmentValue("CMM_MINHEAP", Cmm::minHeap)
      || environmentValue("CMM_MAXHEAP", Cmm::maxHeap) 
      || environmentValue("CMM_INCHEAP", Cmm::incHeap) 
      || environmentValue("CMM_GENERATIONAL", Cmm::generational) 
      || environmentValue("CMM_INCPERCENT", Cmm::incPercent) 
      || environmentValue("CMM_GCTHRESHOLD", Cmm::gcThreshold) 
      || environmentValue("CMM_FLAGS", Cmm::flags)
      || environmentValue("CMM_VERBOSE", Cmm::verbose)
      || Cmm::verbose)
    fprintf(stderr, "***** Cmm(%d, %d, %d, %d, %d, %d, %d, %d)\n",
	    Cmm::minHeap, Cmm::maxHeap, Cmm::incHeap, Cmm::generational,
	    Cmm::incPercent, Cmm::gcThreshold, Cmm::flags, Cmm::verbose);

  /* Allocate heap and side tables.  Exit on allocation failure. */
  heapSpanPages = totalPages = (Cmm::minHeap + bytesPerPage - 1)/bytesPerPage;
  tablePages = (totalPages*sizeof(int)*2 /* pageLink, pageHeap */
		+ totalPages*sizeof(short)*2 /* pageSpace, pageGroup */
		+ totalPages*wordsPerPage/bitsPerWord*bytesPerWord /* objectMap */
#               if !HEADER_SIZE || defined(MARKING)
		+ totalPages*wordsPerPage/bitsPerWord*bytesPerWord /* liveMap */
#               endif
		+ bytesPerPage - 1) / bytesPerPage;
  /* Allocate one block for both the heap and the tables.
   * The tables will be recycled into pages at the next collection.
   */
  heap = ::new char[(totalPages + tablePages) * bytesPerPage
		    + bytesPerPage - 1];
  if (heap == NULL)
    {
      fprintf(stderr, 
	      "\n****** CMM  Unable to allocate %d byte heap\n", Cmm::minHeap);
      abort();
    }
  heap = heap + bytesPerPage - 1;
  heap -= (long)heap % bytesPerPage;
  firstHeapPage = GCPtoPage(heap);
  lastHeapPage = firstHeapPage + heapSpanPages - 1;
  firstTablePage = lastHeapPage + 1;
  freePages = totalPages;

  pageSpace = (short *)pageToGCP(firstTablePage);
  pageGroup = &pageSpace[totalPages];
  pageLink = (int *)&pageGroup[totalPages];
  pageHeap = (CmmHeap **)&pageLink[totalPages];
  objectMap = (unsigned long *)&pageHeap[totalPages];
# if !HEADER_SIZE || defined(MARKING)
  liveMap = (unsigned long *)&objectMap[totalPages*wordsPerPage/bitsPerWord];
# endif

  /* The following definitions are safe because these vectors are accessed
     only through an address within a page. Instead of using
     pageSpace[addr - firstHeapPage]
     space is displaced by firstHeapPage so that we can use:
     pageSpace[addr]
     */

  pageSpace = pageSpace - firstHeapPage;
  pageLink = pageLink - firstHeapPage;
  pageGroup  = pageGroup  - firstHeapPage;
  pageHeap  = pageHeap  - firstHeapPage;
  objectMap = objectMap - WORD_INDEX(firstHeapPage*bytesPerPage);
# if !HEADER_SIZE || defined(MARKING)
  liveMap = liveMap - WORD_INDEX(firstHeapPage*bytesPerPage);
# endif

  /* Initialize tables */
  for (i = firstHeapPage ; i <= lastHeapPage ; i++)
    {
      pageHeap[i] = NOHEAP;
      //!! pageSpace[i] = UNALLOCATEDSPACE;
    }
  currentSpace = 3;		// leave 1 as UNALLOCATEDSPACE
  nextSpace = 3;
  firstFreePage = firstHeapPage;
  queueHead = 0;
  Cmm::created = true;

  Cmm::theDefaultHeap->usedPages 	= 0;
  Cmm::theDefaultHeap->reservedPages 	= 0;
  Cmm::theDefaultHeap->stablePages 	= 0;
  Cmm::theDefaultHeap->firstUnusedPage	= firstHeapPage;
  Cmm::theDefaultHeap->firstReservedPage= firstHeapPage;
  Cmm::theDefaultHeap->lastReservedPage = firstHeapPage;

# if !HEADER_SIZE
  aGcFreeObject = ::new GcFreeObject;
#   ifdef DOUBLE_ALIGN_OPTIMIZE
  aGcPadObject = ::new GcPadObject;
#   endif
# endif

  // The following initializations are needed by the CmmObject::new 
  // operator. For this reason they don't use new, but ::new.

  aCmmObject = ::new CmmObject;
  aCmmVarObject = ::new CmmVarObject;
}


/*---------------------------------------------------------------------------*
 * -- shouldExpandHeap
 *
 * Once the heap has been allocated, it is automatically expanded after garbage
 * collection until the maximum size is reached.  If space cannot be allocated
 * to expand the heap, then the heap will be left it's current size and no
 * further expansions will be attempted.
 *
 * Results: true when the heap should be expanded.
 *
 *---------------------------------------------------------------------------*/

static bool
shouldExpandHeap()
{
  return (HEAPPERCENT(Cmm::theDefaultHeap->stablePages) >= Cmm::incPercent
	   && totalPages < Cmm::maxHeap/bytesPerPage
	   &&  Cmm::incHeap != 0);
}

static void  (*saveNewHandler)();

static void  dummyNewHandler() { set_new_handler(saveNewHandler); }

static bool expandFailed = false;

/*---------------------------------------------------------------------------*
 * -- expandHeap
 *
 * Expands the heap by Cmm::incHeap.
 *
 * Results: number of first new page allocated, 0 on failure
 *
 *---------------------------------------------------------------------------*/

static int
expandHeap(int increment)
{
  int  inc_totalPages = increment/bytesPerPage;
  int  new_firstHeapPage;
  int  inc_firstHeapPage;
  int  new_lastHeapPage;
  int  inc_lastHeapPage;
  int  new_totalPages;
  int  *new_pageLink;
  unsigned long  *new_objectMap;
# if !HEADER_SIZE || defined(MARKING)
  unsigned long  *new_liveMap;
# endif
  int  i;

  short *new_pageSpace;
  short *new_pageGroup;
  CmmHeap **new_pageHeap;

  char  *new_tables;
  int   new_tablePages;
  char  *inc_heap;

  /* Check for previous expansion failure */
  if (expandFailed) return  0;

  /* Allocate additional heap and determine page span */
  saveNewHandler = set_new_handler(dummyNewHandler);

  inc_heap = ::new char[inc_totalPages*bytesPerPage + bytesPerPage - 1];
  if (inc_heap == NULL) goto fail;
  inc_heap = inc_heap + bytesPerPage - 1;
  inc_heap -= (int)inc_heap % bytesPerPage;
  inc_firstHeapPage = GCPtoPage(inc_heap);
  inc_lastHeapPage = inc_firstHeapPage + inc_totalPages - 1;
  new_firstHeapPage = MIN(firstHeapPage,
			  MIN(firstTablePage, inc_firstHeapPage));
  new_lastHeapPage = MAX(lastHeapPage,
			 MAX(firstTablePage + tablePages - 1,
			     inc_lastHeapPage));
  new_totalPages = totalPages + tablePages + inc_totalPages;
  heapSpanPages = new_lastHeapPage - new_firstHeapPage + 1;

  new_tablePages = (heapSpanPages*sizeof(int)*2 /* pageLink, pageHeap */
		    + heapSpanPages*sizeof(short)*2 /* pageSpace, pageGroup */
		    + heapSpanPages*wordsPerPage/bitsPerWord*bytesPerWord /* objectMap */
#if !HEADER_SIZE || defined(MARKING)
		    + heapSpanPages*wordsPerPage/bitsPerWord*bytesPerWord /* liveMap */
#endif
		    + bytesPerPage - 1) / bytesPerPage;
  if ((new_tables = ::new char[new_tablePages*bytesPerPage + bytesPerPage - 1])
      == NULL)
    {
    fail: set_new_handler(saveNewHandler);
      if (inc_heap) delete inc_heap;
      expandFailed = true;
      WHEN_VERBOSE (CMM_STATS,
		  fprintf(stderr, "\n***** CMM  Heap expansion failed\n"));
      return  0;
    }
  set_new_handler(saveNewHandler);
  new_pageSpace = (short *)new_tables;
  new_pageGroup = &new_pageSpace[heapSpanPages];
  new_pageLink = (int *)&new_pageGroup[heapSpanPages];
  new_pageHeap = (CmmHeap **)&new_pageLink[heapSpanPages];
  new_objectMap = (unsigned long *)&new_pageHeap[heapSpanPages];
#if !HEADER_SIZE || defined(MARKING)
  new_liveMap =
    (unsigned long *)&new_objectMap[heapSpanPages*wordsPerPage/bitsPerWord];
#endif

  new_pageSpace = new_pageSpace - new_firstHeapPage;
  new_pageLink = new_pageLink - new_firstHeapPage;
  new_pageGroup = new_pageGroup - new_firstHeapPage;
  new_pageHeap = new_pageHeap - new_firstHeapPage;
  new_objectMap = new_objectMap - WORD_INDEX(new_firstHeapPage*bytesPerPage);
#if !HEADER_SIZE || defined(MARKING)
  new_liveMap = new_liveMap - WORD_INDEX(new_firstHeapPage*bytesPerPage);
#endif

  /* Recycle old tables */
  int lastTablePage = firstTablePage + tablePages - 1;
  for (i = firstTablePage; i <= lastTablePage; i++)
    new_pageHeap[i] = NOHEAP;
  /* Fill gaps */
  int gapStart = MIN(lastTablePage, inc_lastHeapPage);
  int gap1Start = MIN(lastHeapPage, gapStart);

  int gapEnd = MAX(firstTablePage, inc_firstHeapPage);
  int gap2End = MAX(firstHeapPage, gapEnd);

  int gap1End = (gapEnd == gap2End) ?
    MAX(firstHeapPage, MIN(firstTablePage, inc_firstHeapPage)) : gapEnd;
  int gap2Start = (gapStart == gap1Start) ?
    MIN(lastHeapPage, MAX(lastTablePage, inc_lastHeapPage)) : gapStart;
  for (i = gap1Start + 1; i < gap1End; i++)
    new_pageHeap[i] = UNCOLLECTEDHEAP;
  for (i = gap2Start + 1; i < gap2End; i++)
    new_pageHeap[i] = UNCOLLECTEDHEAP;

  /* Initialize new side tables */
  for (i = inc_firstHeapPage ; i <= inc_lastHeapPage ; i++)
    new_pageHeap[i] = NOHEAP;
  for (i = firstHeapPage ; i <= lastHeapPage ; i++)
    {
      new_pageSpace[i] = pageSpace[i];
      new_pageHeap[i] = pageHeap[i];
      new_pageLink[i] = pageLink[i];
      new_pageGroup[i] = pageGroup[i];
    }
  for (i = WORD_INDEX(firstHeapPage*bytesPerPage);
       i < WORD_INDEX((lastHeapPage + 1)*bytesPerPage); i++)
    {
      new_objectMap[i] = objectMap[i];
#if !HEADER_SIZE || defined(MARKING)
      // necessary if expandHeap() is called during collection
      new_liveMap[i] = liveMap[i];
#endif
    }
  
  pageSpace = new_pageSpace;
  pageLink = new_pageLink;
  pageGroup = new_pageGroup;
  pageHeap = new_pageHeap;
  objectMap = new_objectMap;
#if !HEADER_SIZE || defined(MARKING)
  liveMap = new_liveMap;
#endif
  firstHeapPage = new_firstHeapPage;
  lastHeapPage = new_lastHeapPage;
  totalPages = new_totalPages;
  freePages += inc_totalPages + tablePages;
  tablePages = new_tablePages;
  firstTablePage = GCPtoPage(new_tables);
  firstFreePage = inc_firstHeapPage;
  
  WHEN_VERBOSE (CMM_STATS,
	      fprintf(stderr,
		      "\n***** CMM  Heap expanded to %d bytes\n",
		      totalPages * bytesPerPage));
  return  inc_firstHeapPage;
}


/*---------------------------------------------------------------------------*
 * -- emptyStableSet
 *
 * Moves the stable set, up to end, back into the currentSpace.
 * A total collection is performed by calling this before calling
 * collect().  When generational collection is not desired, this is called
 * after collection to empty the stable set.
 *
 *---------------------------------------------------------------------------*/

static void
emptyStableSet(int end)
{
  int scan;
  end = pageLink[end];
  while (queueHead != end)
    {
      scan = queueHead;
      int pages = pageGroup[scan];
      while (pages--)
	pageSpace[scan++] = currentSpace;
      queueHead = pageLink[queueHead];
    }
  int count = 0;
  scan = queueHead;
  while (scan)
    {
      count++;
      scan = pageLink[scan];
    }
  Cmm::theDefaultHeap->stablePages = count;
}

/*---------------------------------------------------------------------------*
 * -- queue
 *
 * Adds a page to the stable set page queue.
 *---------------------------------------------------------------------------*/

static void
queue(int page)
{
  if (queueHead != 0)
    pageLink[queueTail] = page;
  else 
    queueHead = page;
  pageLink[page] = 0;
  queueTail = page;
}

/*---------------------------------------------------------------------------*
 * -- promotePage
 *
 * Pages that have might have references in the stack or the registers are
 * promoted to the stable set.
 *
 * Note that objects that get allocated in a CONTINUED page (after a large
 * object) will never move.
 *---------------------------------------------------------------------------*/

void
promotePage(GCP cp)
{
  int page = GCPtoPage(cp);
  
  // Don't promote pages belonging to other heaps.
  // (We noticed no benefit by inlining the following test in the caller)
  if (page >= firstHeapPage
      &&  page <= lastHeapPage
      && pageHeap[page] == Cmm::theDefaultHeap)
    {
#     ifdef MARKING
      CmmObject *bp = basePointer(cp);
      MARK(bp);
#     endif
      if (pageSpace[page] == currentSpace)
	{
	  int pages = pageGroup[page];
	  if (pages < 0)
	    {
	      page += pages; 
	      pages = pageGroup[page]; 
	    }
	  WHEN_VERBOSE (CMM_DEBUGLOG,
		      fprintf(stderr, "promoted 0x%x\n", pageToGCP(page)));
	  queue(page);
	  Cmm::theDefaultHeap->usedPages += pages; // in nextSpace
	  Cmm::theDefaultHeap->stablePages += pages;
#	  ifdef MARKING
	  pageSpace[page++] = PROMOTEDSPACE;
	  pages--;
#	  endif
	  while (pages--)
	    pageSpace[page++] = nextSpace;
	}
    }
}

/*---------------------------------------------------------------------------*
 * -- basePointer
 *
 * Results: pointer to the beginning of the containing object
 *---------------------------------------------------------------------------*/

CmmObject *
basePointer(GCP fp)
{
  fp = (GCP) ((int)fp & ~(bytesPerWord-1));
  
  register int index 		= WORD_INDEX(fp);
  register int inner 		= BIT_INDEX(fp);
  register unsigned long mask	= 1 << inner;
  register unsigned long bits	= objectMap[index];
  
  do
    {
      do
	{
	  if (bits & mask)
	    return (CmmObject *)fp;
	  mask = mask >> 1;
	  fp--;
	}
      while (inner--);
      bits = objectMap[--index];
      inner = bitsPerWord-1;
      mask = 1L << bitsPerWord-1;
    }
  while (true);
}

/*---------------------------------------------------------------------------*
 * Forward declarations:
 *---------------------------------------------------------------------------*/

static void verifyObject(GCP, int);
static void verifyHeader(GCP);
static void newlineIfLogging();
static void logRoot(long*);


/*---------------------------------------------------------------------------*
 * -- CmmMove
 *
 * Copies object from currentSpace to nextSpace
 *
 * Results: pointer to header of copied object
 *
 * Side effects: firstFreeWord, freeWords, usedPages
 *---------------------------------------------------------------------------*/

static GCP
CmmMove(GCP cp)
{
  int  page = GCPtoPage(cp);	/* Page number */
  GCP  np;			/* Pointer to the new object */
# if HEADER_SIZE
  int  header;			/* Object header */
# endif

  /* Verify that the object is a valid pointer and decrement ptr cnt */
  WHEN_FLAGS (CMM_TSTOBJ, verifyObject(cp, 1));
  
  if (pageIsStable(page))
# ifdef MARKING
    {
      if (!MARKED(cp))
	{
	  MARK(cp);
	  if (SCANNED(page)
#         if HEADER_SIZE
	      && (HEADER_TAG(*(cp - HEADER_SIZE)) > 1)
#         endif
	      )
	    ((CmmObject *)cp)->traverse();
	}
      return(cp);
    }
# else
  return(cp);
# endif
  /* If cell is already forwarded, return forwarding pointer */
# if HEADER_SIZE
  header = cp[-HEADER_SIZE];
  if (FORWARDED(header))
    {
      WHEN_FLAGS (CMM_TSTOBJ, {
	verifyObject((GCP)header, 0);
	verifyHeader((GCP)header);
      });
      return ((GCP)header);
    }
# else
  if (FORWARDED(cp))
    return ((GCP)*cp);
# endif
  
  /* Move the object */
  WHEN_FLAGS (CMM_TSTOBJ, verifyHeader(cp));
  
  /* Forward or promote object */
#if HEADER_SIZE
  register int  words = HEADER_WORDS(header);
#else
  register int  words = ((CmmObject *)cp)->words();
#endif
  if (words >= freeWords)
    {
      /* Promote objects >= a page to stable set */
      /* This is to avoid expandHeap(). See note about collect().
       * We could perform copying during a full GC by reserving in advance
       * a block of pages for objects >= 1 page
       */
      if (words >= onePageObjWords)
	{
	  promotePage(cp);
	  /* tito: you don't need to traverse it now.
	   * Object will be traversed, when the promoted page will be swept.
	   */
	  return(cp);
	}
      /* Discard any partial page and allocate a new one */
      // We must ensure that this does not invoke expandHeap()
      Cmm::theDefaultHeap->reservePages(1);
      WHEN_VERBOSE (CMM_DEBUGLOG, fprintf(stderr, "queued   0x%x\n", firstFreeWord));
      queue(GCPtoPage(firstFreeWord));
      Cmm::theDefaultHeap->stablePages += 1;
    }
  /* Forward object, leave forwarding pointer in old object header */
# if HEADER_SIZE
  *firstFreeWord++ = header;
# else
  GCP ocp = cp;
# endif
  np = firstFreeWord;
  SET_OBJECTMAP(np);
  freeWords = freeWords - words;
# if HEADER_SIZE
  cp[-HEADER_SIZE] = (int)np;	// lowest bit 0 means forwarded
  words -= HEADER_SIZE;
  while (words--) *firstFreeWord++ = *cp++;
#   ifdef DOUBLE_ALIGN
  HEADER_ALIGN(firstFreeWord, freeWords);
#   endif
# else
  MARK(cp);			// Necessary to recognise as forwarded
  while (words--) *firstFreeWord++ = *cp++;
  *ocp = (int)np;
# endif				// !HEADER_SIZE
# ifdef MARKING
  MARK(np);
  /* tito: no need to traverse it now.
     Object will be traversed, when the promoted page will be swept.
     */
# endif
  return(np);
}

/*---------------------------------------------------------------------------*
 * -- DefaultHeap::scavenge
 *
 * Replaces pointer to (within) object with pointer to scavenged object
 *
 * Results: none
 *
 * Side effects: firstFreeWord, freeWords, usedPages
 *---------------------------------------------------------------------------*/

void
DefaultHeap::scavenge(CmmObject **loc)
{
  GCP pp = (GCP)*loc;
  int page = GCPtoPage(pp);
  if (!OUTSIDE_HEAP(page))
    {
      CmmObject *p = basePointer((GCP)*loc);
      
      if (inside((GCP)p))
	*loc = (CmmObject *)((int)CmmMove((GCP)p) + (int)*loc - (int)p);
      else
	{
	  page = GCPtoPage(p);
	  if (!OUTSIDE_HEAP(page)
	      // if page is OUTSIDE_HEAP, p must be an ambiguous pointer
	      && !pageHeap[page]->isOpaque())
	    visit(p);
	}
    }
}

/*---------------------------------------------------------------------------*
 * -- DefaultHeap::collect
 *
 * Garbage collection for the DefaultHeap. It is typically
 * called when half the pages in the heap have been allocated.
 * It may also be directly called.
 * 
 * WARNING: (freePages + reservedPages - usedPages) must be > usedPages when
 * collect() is called to avoid the invocation of expandHeap() in the
 * middle of collection.
 *---------------------------------------------------------------------------*/

void
DefaultHeap::collect()
{
  int  page;			/* Page number while walking page list */
  GCP  cp,			/* Pointers to move constituent objects */
  nextcp;
  
  
  /* Check for heap not yet allocated */
  if (!Cmm::created)
    {
      CmmInit();
      return;
    }
  
  /* Log entry to the collector */
  WHEN_VERBOSE (CMM_STATS, {
    fprintf(stderr, "***** CMM  Collecting - %d%% allocated  ->  ",
	    HEAPPERCENT(usedPages));
    newlineIfLogging();
  });
  
  /* Allocate rest of the current page */
  if (freeWords != 0) {
# if HEADER_SIZE
    *firstFreeWord = MAKE_HEADER(freeWords, freeSpaceTag);
# else
    *firstFreeWord = *(GCP)aGcFreeObject;
    SET_OBJECTMAP(firstFreeWord);
# endif
    freeWords = 0;
  }
  
  /* Advance space.
   * Pages allocated by CmmMove() herein will belong to the stable set.
   * At the end of collect() we go back to normal.
   * Therefore objects moved once by the collector will not be moved again
   * until a full collection is enabled by emptyStableSet().
   */
  
  nextSpace = currentSpace + 1;
  usedPages = stablePages;	// start counting in nextSpace
  
# if !HEADER_SIZE || defined(MARKING)
  /* Clear the liveMap bitmap */
  bzero((char*)&liveMap[WORD_INDEX(firstHeapPage * bytesPerPage)],
	heapSpanPages * (bytesPerPage / bitsPerWord));
# endif
  /* Examine stack, registers, static area and possibly the non-garbage
     collected heap for possible pointers */
  WHEN_VERBOSE (CMM_ROOTLOG, fprintf(stderr, "stack roots:\n"));
  {
    jmp_buf regs;
    GCP fp;			/* Pointer for checking the stack */
    void CmmExamineStaticArea(GCP, GCP);

    /* ensure flushing of register caches	*/
    if (_setjmp(regs) == 0) _longjmp(regs, 1);
    
    /* Examine the stack:		*/
#   ifdef STACK_GROWS_DOWNWARD
    for (fp = (GCP)regs; fp < (GCP)stackBottom; fp++)
#   else
    for (fp = (GCP)regs + sizeof(regs); fp > (GCP)stackBottom; fp--)
#   endif
      {
	WHEN_VERBOSE (CMM_ROOTLOG, logRoot(fp));
	promotePage((GCP)*fp);
      }
    
    /* Examine the static areas:		*/
    WHEN_VERBOSE (CMM_ROOTLOG,
		fprintf(stderr, "Static and registered roots:\n"));

    CmmExamineStaticAreas(CmmExamineStaticArea);

    /* Examine registered roots:		*/
    for (int i = 0; i < rootsCount; i++)
      {
	fp = roots[i].addr;
	for (int j = roots[i].bytes; j > 0; j = j - bytesPerWord)
	  promotePage((GCP)*fp++);
      }
    /* Examine the uncollected heap:		*/
    if (Cmm::flags & CMM_HEAPROOTS)
      {
	WHEN_VERBOSE (CMM_HEAPLOG,
		    fprintf(stderr, "Uncollected heap roots:\n"));
	GCP globalHeapEnd = (GCP)sbrk(0);
	fp = globalHeapStart;
	while (fp < globalHeapEnd)
	  {
	    if (!inside((GCP)fp)) 
	      {
		WHEN_VERBOSE (CMM_HEAPLOG, logRoot(fp));
		if (Cmm::flags & CMM_HEAPROOTS)
		  promotePage((GCP)*fp);
		fp++;
	      }
	    else
	      fp = fp + wordsPerPage; // skip page
	  }
      }
  }
  WHEN_VERBOSE (CMM_STATS, {
    fprintf(stderr, "%d%% locked  ", HEAPPERCENT(usedPages));
    newlineIfLogging();
  });
  
  /* Sweep across stable pages and move their constituent items.	 */
  page = queueHead;
  // pages promoted from here should survive this generation:
  int lastStable = queueTail;
  while (page)
    {
#     ifdef MARKING
      SET_SCANNED(page);	// pointers to unmarked objects within
				// this page will have to be traversed
#     endif
      cp = pageToGCP(page);
      WHEN_VERBOSE (CMM_DEBUGLOG, fprintf(stderr, "sweeping 0x%x\n", cp));
      GCP nextPage = pageToGCP(page + 1);
      bool inCurrentPage = (page == GCPtoPage(firstFreeWord));
      nextcp = inCurrentPage ? firstFreeWord : nextPage;
      /* current page may get filled while we sweep it */
      while (cp < nextcp
	     || inCurrentPage
	     && cp < (nextcp =
		      (cp <= firstFreeWord && firstFreeWord < nextPage)
		      ? firstFreeWord : nextPage))
	{
	  WHEN_FLAGS (CMM_TSTOBJ, verifyHeader(cp + HEADER_SIZE));
#         if HEADER_SIZE
	  if ((HEADER_TAG(*cp) > 1)
#             ifdef MARKING
	      && MARKED(cp + HEADER_SIZE)
#             endif
	      )
	    ((CmmObject *)(cp + HEADER_SIZE))->traverse();
	  cp = cp + HEADER_WORDS(*cp);
#         else
#           ifdef MARKING
	  if (MARKED(cp))
#           endif
	    ((CmmObject *)cp)->traverse();
	  cp = cp + ((CmmObject *)cp)->words();
#         endif
	}
      page = pageLink[page];
    }

  /* Finished, all retained pages are now part of the stable set */
  currentSpace = currentSpace + 2;
  nextSpace = currentSpace;
  WHEN_VERBOSE (CMM_STATS,
	      fprintf(stderr, "%d%% stable.\n", HEAPPERCENT(stablePages)));
  
  /* Check for total collection and heap expansion.  */
  if (Cmm::generational != 0)
    {
      /* Performing generational collection */
      if (HEAPPERCENT(usedPages) >= Cmm::generational) 
	{
	  /* Perform a total collection and then expand the heap */
	  //      doFullGC = true;
	  emptyStableSet(lastStable);
	  int  saveGenerational = Cmm::generational;
	  
	  Cmm::generational = 100;
	  cp = NULL;			// or collect will promote it again
	  collect();
	  if (shouldExpandHeap()) expandHeap(Cmm::incHeap);
	  Cmm::generational = saveGenerational;
	}
    }
  else
    {
      /* Not performing generational collection */
      if (shouldExpandHeap()) expandHeap(Cmm::incHeap);
      emptyStableSet(queueTail);
    }
}

void
CmmExamineStaticArea(GCP base, GCP limit)
{
  register GCP fp;
  for (fp = base ; fp < limit ; fp++)
    {
      WHEN_VERBOSE (CMM_ROOTLOG, logRoot(fp));
      promotePage((GCP)*fp);
    }
}

/*---------------------------------------------------------------------------*
 * -- nextPage
 *
 * Results: index of next page (wrapped at the end)
 *
 *---------------------------------------------------------------------------*/

static inline int  nextPage(int page)
{
  return (page == lastHeapPage) ? firstHeapPage : page + 1;
}

/*---------------------------------------------------------------------------*
 * -- allocatePages
 *
 * Page allocator.
 * Allocates a number of additional pages to the indicated heap.
 *
 * Results: address of first page
 *
 * Side effects: firstFreePage
 *---------------------------------------------------------------------------*/

GCP
allocatePages(int pages, CmmHeap *heap)
{
  int  	free;			/* # contiguous free pages */
  int	firstPage;		/* Page # of first free page */
  int	allPages;		/* # of pages in the heap */
  GCP	firstByte;		/* address of first free page */
  
  allPages = heapSpanPages;
  free = 0;
  firstPage = firstFreePage;
  
  while (allPages--) 
    {
      if (pageHeap[firstFreePage] == NOHEAP)
	{
	  if (++free == pages) goto FOUND;
	}
      else
	free = 0;
      firstFreePage = nextPage(firstFreePage);
      if (firstFreePage == firstHeapPage) free = 0;
      if (free == 0) firstPage = firstFreePage;
    }
  /* Failed to allocate space, try expanding the heap.  Assure
   * that minimum increment size is at least the size of this object.
   */
  if (!Cmm::created)
    CmmInit();			/* initialize heap, if not done yet */
  Cmm::incHeap = MAX(Cmm::incHeap, pages*bytesPerPage);
  firstPage = expandHeap(Cmm::incHeap);
  if (firstPage == 0) 
    {
      /* Can't do it */
      fprintf(stderr,
	      "\n***** allocatePages  Unable to allocate %d pages\n", pages);
      abort();
    }
 FOUND:
  // Ok, I found all needed contiguous pages.
  freePages -= pages;
  firstByte = pageToGCP(firstPage);
  int i = 1;
  while (pages--) 
    {
      pageHeap[firstPage+pages] = heap;
#     if !HEADER_SIZE
      // Fake groups so that words() works also outside the DefaultHeap;
      pageGroup[firstPage+pages] = i++;
#     endif
    }
  return firstByte;
}

/*---------------------------------------------------------------------------*
 * -- DefaultHeap::reservePages
 *
 * When alloc() is unable to allocate storage, it calls this routine to
 * allocate one or more pages.  If space is not available then the garbage
 * collector is called and/or the heap is expanded.
 *
 * Results: address of first page
 *
 * Side effects: firstFreePage, firstFreeWord, freeWords, usedPages
 *---------------------------------------------------------------------------*/

#define USED2FREE_RATIO 2

GCP
DefaultHeap::reservePages(int pages)
{
  int firstPage;		/* Page # of first free page	*/
  int i;
  
  /* Garbage collect if not enough pages will be left for collection.	*/
  
  if (currentSpace == nextSpace /* not within CmmMove()  		*/
      /* && usedPages - stablePages + pages
	 > freePages + reservedPages - usedPages - pages */
      && usedPages + pages
      > USED2FREE_RATIO * (freePages + reservedPages - usedPages - pages))  
    // firstFreeWord is seen by the collector: it should not consider it as a root.
    collect();

  /* Discard any remaining portion of current page */
  if (freeWords != 0)
    {
#if HEADER_SIZE
      *firstFreeWord = MAKE_HEADER(freeWords, freeSpaceTag);
#else
      *firstFreeWord = *(GCP)aGcFreeObject;
      SET_OBJECTMAP(firstFreeWord);
#endif
      freeWords = 0;
    }
  if (reservedPages - usedPages > reservedPages / 16)
    // not worth looking for the last few ones dispersed through the heap
    {
      int free = 0;			/* # contiguous free pages	*/
      int allPages = lastReservedPage - firstReservedPage;
      firstPage = firstUnusedPage;
      while (allPages--)
	{
	  if (pageHeap[firstUnusedPage] == this
	      && pageSpace[firstUnusedPage] != currentSpace // in previous generation
	      && pageIsUnstable(firstUnusedPage))	    // but not in stable set
	    { 
	      if (++free == pages)
		{
		  firstFreeWord = pageToGCP(firstPage);
		  goto FOUND;
		}
	    }
	  else
	    {
	      free = 0;
	      firstPage = firstUnusedPage+1;
	    }
	  if (firstUnusedPage == lastReservedPage)
	    {
	      firstUnusedPage = firstPage = firstReservedPage;
	      free = 0;
	    }
	  else
	    firstUnusedPage++;
	}
    }
  {
    int reserved = MAX(8, pages); // get a bunch of them
    firstFreeWord = allocatePages(reserved, this);
    firstUnusedPage = firstPage = GCPtoPage(firstFreeWord);
    i = firstPage + reserved - 1;
    lastReservedPage = MAX(lastReservedPage, i);
    reservedPages += reserved;
    for (i = pages; i < reserved; i++)
      pageSpace[firstPage + i] = UNALLOCATEDSPACE;
  }
 FOUND:
  // Found all needed contiguous pages.
  bzero((char*)firstFreeWord, pages*bytesPerPage);
#if HEADER_SIZE && defined(DOUBLE_ALIGN)
  *firstFreeWord++ = doublepad;
  freeWords = pages*wordsPerPage - 1;
#else
  freeWords = pages*wordsPerPage;
#endif
  usedPages += pages;
  bzero((char*)&objectMap[WORD_INDEX(firstPage*bytesPerPage)],
	pages*(bytesPerPage/bitsPerWord));
  pageSpace[firstPage] = nextSpace;
  pageGroup[firstPage] = pages;
  i = -1;
  while (--pages)
    {
      pageSpace[++firstPage] = nextSpace;
      pageGroup[firstPage] = i--;
    }
  return firstFreeWord;
}

/*---------------------------------------------------------------------------*
 * -- DefaultHeap::alloc
 *
 * Storage is allocated by the following function.
 * It is up to the specific constructor procedure to assure that all
 * pointer slots are correctly initialized.
 *
 * Proper alignment on architectures which require DOUBLE_ALIGN, is dealt
 * as follows.
 * - when HEADER_SIZE == 1, firstFreeWord is kept misaligned (if after
 *   allocating an object it is not misaligned, doublepad is inserted)
 * - when HEADER_SIZE == 0, aGcPadObject is inserted before allocating
 *   an object when firstFreeWord is misaligned.
 *   For object whose size is < 4 words we can optimize space, avoiding
 *   the padding.
 *
 * Results: pointer to the object
 *
 * Side effects: firstFreeWord, freeWords
 *---------------------------------------------------------------------------*/

GCP
DefaultHeap::alloc(unsigned long size)
{
  GCP  object;			/* Pointer to the object */
  
  size = bytesToWords(size);	// add size of header
  
  /* Try to allocate from current page */
  if (size <= freeWords)
    {
#     if HEADER_SIZE
      object = firstFreeWord;
      freeWords = freeWords - size;
      firstFreeWord = firstFreeWord + size;
#       ifdef DOUBLE_ALIGN
      HEADER_ALIGN(firstFreeWord, freeWords);
#       endif
#     else			// !HEADER_SIZE
#       ifdef DOUBLE_ALIGN_OPTIMIZE
      if (size < 16 || ((int)firstFreeWord & 7) == 0)
	{
#       endif			// DOUBLE_ALIGN_OPTIMIZE
	  object = firstFreeWord;
	  freeWords = freeWords - size;
	  firstFreeWord = firstFreeWord + size;
#       ifdef DOUBLE_ALIGN_OPTIMIZE
	}
      else if (size <= freeWords - 1)
	{
	  SET_OBJECTMAP(firstFreeWord);
	  *firstFreeWord++ = *(GCP)aGcPadObject;
	  object = firstFreeWord;
	  freeWords = freeWords - size - 1;
	  firstFreeWord = firstFreeWord + size;
	}
#       endif			// DOUBLE_ALIGN_OPTIMIZE
#     endif			// ! HEADER_SIZE
    }
  else if (size < onePageObjWords)
    /* Object fits in one page with left over free space*/
    {
      reservePages(1);
      object = firstFreeWord;
      freeWords = freeWords - size;
      firstFreeWord = firstFreeWord + size;
#     if HEADER_SIZE && defined(DOUBLE_ALIGN)
      HEADER_ALIGN(firstFreeWord, freeWords);
#     endif
    }
  /* Object >= 1 page in size.
   * It is allocated at the beginning of next page.
   */
# if HEADER_SIZE
  else if (size > maxHeaderWords)
    {
      fprintf(stderr,
	      "\n***** CMM  Unable to allocate objects larger than %d bytes\n",
	      maxHeaderWords * bytesPerWord - bytesPerWord);
      abort();
    }
# endif
  else
    {
#     if HEADER_SIZE && defined(DOUBLE_ALIGN)
      reservePages((size + wordsPerPage) / wordsPerPage);
#     else
      reservePages((size + wordsPerPage - 1) / wordsPerPage);
#     endif
    
      object = firstFreeWord;
      /* No object is allocated in final page after object > 1 page */
      if (freeWords != 0) {
#       if HEADER_SIZE
	*firstFreeWord = MAKE_HEADER(freeWords, freeSpaceTag);
#       else
	*firstFreeWord = *(GCP)aGcFreeObject;
	SET_OBJECTMAP(firstFreeWord);
#       endif
	freeWords = 0;
      }
      firstFreeWord = NULL;
    }
  ALLOC_SETUP(object, size);
  return(object);
}

/*---------------------------------------------------------------------------*
 * -- isTraced
 *
 * Results: true if the object is checked by the garbage collector.
 *
 *---------------------------------------------------------------------------*/

bool
isTraced(void *obj)
{
  extern int end;
  if (obj >= (void *)(&end) &&
#     ifdef STACK_GROWS_DOWNWARD
      obj < (void *)(&obj)
#     else
      obj > (void *)(&obj)
#     endif
      )
    {
      int page = GCPtoPage(obj);
      if (OUTSIDE_HEAP(page))
	return false;
    }
  return true;
}

/*---------------------------------------------------------------------------*
 * -- CmmObject::operator new
 *
 * The creation of a new GC object requires:
 *	- to mark its address in table objectMap
 *	- to record its size in the header
 *
 *---------------------------------------------------------------------------*/

void *
CmmObject::operator new(size_t size, CmmHeap *heap)
{
  GCP object = heap->alloc(size);
  
  // To avoid problems in GC after new but during constructor
  *object = *((GCP)aCmmObject);
  
  return (void *)object;
}
/*---------------------------------------------------------------------------*
 *
 * CmmObject::operator delete
 *
 *---------------------------------------------------------------------------*/
void CmmObject::operator delete(void *obj)
{
  (((CmmObject *)obj)->heap())->reclaim((GCP)obj);
}

/*---------------------------------------------------------------------------*
 *
 * CmmObject::operator new[]
 *
 *---------------------------------------------------------------------------*/
void *
CmmObject::operator new[](size_t size, CmmHeap *heap)
{
	return sizeof(CmmVarObject) + (char*) (new(size, heap) CmmVarObject);
}
/*---------------------------------------------------------------------------*
 *
 * CmmObject::operator delete[]
 *
 *---------------------------------------------------------------------------*/
void
CmmObject::operator delete[](void* obj)
{
	delete obj;
}

/*---------------------------------------------------------------------------*
 *
 * CmmVarObject::operator new
 *
 *---------------------------------------------------------------------------*/

void *
CmmVarObject::operator new(size_t size, size_t extraSize, CmmHeap *heap)
{
  size += extraSize;
  
  GCP object = heap->alloc(size);
  
  // To avoid problems in GC after new but during constructor
  *object = *((GCP)aCmmVarObject);
  
  return (void *)object;
}

/*---------------------------------------------------------------------------*
 *
 * -- Verification
 *
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * -- nextObject
 *
 * A pointer pointing to the header of an object is stepped to the next
 * header.  Forwarded headers are correctly handled.
 *
 * Results: address of immediately consecutive object
 *
 *---------------------------------------------------------------------------*/

static GCP
nextObject(GCP xp)
{
#if HEADER_SIZE
  if (FORWARDED(*xp))
    return  xp + HEADER_WORDS(*((int*)(*xp) - HEADER_SIZE));
  else
    return  xp + HEADER_WORDS(*xp);
#else
  return  xp + ((CmmObject *)xp)->words();
#endif
}

/*---------------------------------------------------------------------------*
 * -- verifyObject
 *
 * Verifies that a pointer points to an object in the heap.
 * An invalid pointer will be logged and the program will abort.
 *
 *---------------------------------------------------------------------------*/

static void
verifyObject(GCP cp, int old)
{
  int  page = GCPtoPage(cp);
  GCP  xp = pageToGCP(page);	/* Ptr to start of page */
  int  error = 0;
  
  if (page < firstHeapPage) goto fail;
  error = 1;
  if (page > lastHeapPage) goto fail;
  error = 2;
  if (pageSpace[page] == UNALLOCATEDSPACE)  goto fail;
  error = 3;
  if (old  &&  pageIsUnstable(page)  &&  pageSpace[page] != currentSpace)
    goto fail;
  error = 4;
  if (old == 0  &&  pageSpace[page] != nextSpace)  goto fail; 
  error = 5;
  while (cp > xp + HEADER_SIZE)  xp = nextObject(xp);
  if (cp == xp + HEADER_SIZE)  return;
 fail:
  fprintf(stderr,
	  "\n***** CMM  invalid pointer  error: %d  pointer: 0x%x\n",
	  error, cp);
  abort();
}

/*---------------------------------------------------------------------------*
 * -- verifyHeader
 *
 * Verifies an object's header.
 * An invalid header will be logged and the program will abort.
 *
 *---------------------------------------------------------------------------*/

#ifdef DOUBLE_ALIGN
#define HEADER_PAGES(header) ((HEADER_WORDS(header)+wordsPerPage)/wordsPerPage)
#else
#define HEADER_PAGES(header) ((HEADER_WORDS(header)+wordsPerPage-1)/wordsPerPage)
#endif

static void
verifyHeader(GCP cp)
{
# if HEADER_SIZE
  int  size = HEADER_WORDS(cp[-HEADER_SIZE]),
# else
  int  size = ((CmmObject *)cp)->words(),
# endif
  page = GCPtoPage(cp),
  error = 0;
  
  if  FORWARDED(cp[-HEADER_SIZE])  goto fail;
  error = 1;
# if HEADER_SIZE
  if ((unsigned)HEADER_TAG(cp[-HEADER_SIZE]) > 2)  goto fail;
# endif
  if (size <= onePageObjWords)  {
    error = 2;
    if (cp - HEADER_SIZE + size > pageToGCP(page + 1))  goto fail;
  } else  {
    error = 3;
#   if HEADER_SIZE
    int  pages = HEADER_PAGES(cp[-HEADER_SIZE]);
#   else
    int pages = pageGroup[page];
    if (pages < 0) pages = pageGroup[page+pages];
#   endif
    int pagex = page;
    while (--pages)  {
      pagex++;
      if (pagex > lastHeapPage  ||
	  pageGroup[pagex] > 0  ||
	  pageSpace[pagex] != pageSpace[page])
	goto fail;
    }
  }
  return;
 fail:	fprintf(stderr,
		"\n***** CMM  invalid header  error: %d  object&: 0x%x  header: 0x%x\n",
		error, cp, cp[-HEADER_SIZE]);
  abort();
}

/*---------------------------------------------------------------------------*
 *
 * -- Logging and Statistics
 *
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 * -- logRoot
 *
 * Logs a root to stderr.
 *
 *---------------------------------------------------------------------------*/

static void
logRoot(long* fp)
{
  int  page = GCPtoPage(fp);
  
  if (page < firstHeapPage
      || page > lastHeapPage
      || pageSpace[page] == UNALLOCATEDSPACE
      || (pageIsUnstable(page)  &&  pageSpace[page] != currentSpace))
    return;
  
  int pages = pageGroup[page];
  
  if (pages < 0) page += pages;
  
  GCP  p1, p2 = pageToGCP(page);
  
  while (p2 < (GCP)fp)
    {
      p1 = p2;
      p2 = nextObject(p2);
    }
  fprintf(stderr, "***** DefaultHeap::alloc  root&: 0x%x  object&: 0x%x  %s\n",
	  fp, p1,
# if HEADER_SIZE
	  HEADER_TAG(*p1)
# else
	  *p1
# endif
	  );
}

/* Output a newline to stderr if logging is enabled. */

static void
newlineIfLogging()
{
  WHEN_VERBOSE ((CMM_DEBUGLOG | CMM_ROOTLOG | CMM_HEAPLOG),
	      fprintf(stderr, "\n"));
}


/*---------------------------------------------------------------------------*
 * -- UncollectedHeap::scanRoots
 *---------------------------------------------------------------------------*/
void
UncollectedHeap::scanRoots(int page)
{
	GCP start = pageToGCP(page);
	GCP end = pageToGCP(page + 1);
	GCP ptr;

	for (ptr = start; ptr < end; ptr++)
	    promotePage(ptr);
}
