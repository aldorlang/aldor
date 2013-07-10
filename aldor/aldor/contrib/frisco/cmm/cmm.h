/*---------------------------------------------------------------------------*
 *
 *  cmm.h:	definitions for the CMM
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

/*---------------------------------------------------------------------------*

   Defining garbage collected classes
   ----------------------------------
   Classes allocated in the garbage collected heap are derived from class
   CmmObject.
   The collector applies method traverse() to an object to find other objects
   to which it points.
   A method for traverse() must be supplied by the programmer for each such
   collected class which contains pointers to other collected objects,
   defined according to the following rules:

   (a) for a class containing a pointer, say class C { type *x; },
       the method C::traverse must contain scavenge(&x);

   (b) for a class containing an instance of a collected object, say
       class C { GcClass x; }, the method C::traverse must contain
       x.traverse();

   (c) for a class derived from another collected class, say
       class C: GcClass {...}, the method C::traverse must contain
       GcClass::traverse();

   (d) for a class deriving from a virtual base class, say class
       C: virtual GcClass {...}, the method C::traverse must contain
       scavenge(VirtualBase(GcClass));

   For example,

   class BigNum: public CmmObject
   {
     long data;
     BigNum *next;                         // Rule (a) applies here
     void traverse();
   }

   class monomial: private BigNum          // Rule (c) applies here
   {
     PowerProduct pp;                      // Rule (b) applies here
     void traverse();
   }

   A BigNum stores in next a pointer to a collected object which needs to
   be scavenged, so traverse becomes:

   void BigNum::traverse()
   {
     Cmm::heap->scavenge(&next);   // Applying rule (a)
   }

   Because monomial inherits from BigNum, the method traverse for this base
   class must be invoked; finally, since a monomial contains a BigNum in pp,
   this object must be traversed as well:

   void monomial::traverse()
   {
     BigNum::traverse();                   // Appling rule (c)
     pp.traverse();                        // Applying rule (b)
   }

   Once the object has been defined, storage is allocated using the normal
   C++ mechanism:

	bn = new Bignum();


   Variable size objects
   ---------------------
   In order to allocate variable size objects, the size of the variable
   portion of the object must be defined when the object is created.
   Classes of variable sized objects must derive from class CmmVarObject.

   Arrays of collected objects
   ---------------------------
   Garbage collected arrays of garbage collected objects can be created
   by using class GcArray.
   Such arrays must be always used through references, e.g.:

	GcArray<MyClass> & MyVector = * new (100) GcArray<MyClass>;

   Caveats
   -------
   When the garbage collector is invoked, it searches the processor's
   registers, the stack, and the program's static area for possible pointers
   to "root" objects which are still accessible.
   These "roots" are to be left in place, while objects that the roots point
   to will be moved to compact the heap.  Because of this:

   Objects allocated in the garbage collected heap MAY MOVE.

   Pointers to garbage collected objects MAY BE passed as arguments or stored
   in static storage.

   Pointers to garbage collected objects MAY NOT be stored in dynamically 
   allocated objects that are not garbage collected, UNLESS one has specified
   the GCHEAPROOTS flag in a Cmm declaration, OR declared that region as
   a root via a call to gcRoots.

   Pointers to garbage collected objects contained in garbage collected objects
   MUST always point outside the garbage collected heap or to a garbage
   collected object.  To assure this, storage is zeroed at object creation
   time.

   Sizing the heap
   ---------------
   In order to make heap allocated storage as painless as possible, the user
   does not have to do anything to configure the heap.  This default is an
   initial heap of 1 megabyte that is expanded in 1 megabyte increments
   whenever the heap is more than 25% full after a total garbage collection.
   Total garbage collections are done when the heap is more than 35% full.

   However, if this is not the desired behavior, then it is possible to "tune"
   the collector by including one or more global Cmm declarations in the
   program.  In order to understand the parameters supplied in a Cmm
   declaration, one needs an overview of the storage allocation and garbage
   collection algorithm.

   Storage is allocated from the heap until 50% of the heap has been allocated.
   All accessible objects allocated since the last collection are retained and
   made a part of the stable set.  If less than <generational> percent of
   the heap is allocated, then the collection process is finished.  Otherwise,
   the entire heap (including the stable set) is garbage collected.  If the
   amount allocated following the total collection is greater than
   <expand threshold> percent, then an attempt is made to expand the heap.

	Cmm  <identifier>(<initial heap size>,
			         <maximum heap size>,
			         <expand size>,
			         <generational>,
			         <expand threshold>,
				 <gcthreshold>,
			         <flags>,
				 <verbose>)

   The arguments are defined as follows:

	<identifier>		 a legal C++ identifier.
	<initial heap size>	 initial size of the heap in bytes.
				 DEFAULT: 131072.
	<maximum heap size>	 maximum heap size in bytes.
				 DEFAULT: 2147483647.
	<increment size>   	 # of bytes to add to each heap on each
				 expansion.  DEFAULT: 1048576.
	<generational>  	 number between 0 and 50 that is the percent
				 allocated after a partial collection that will
				 force a total collection.  A value of 0 will
				 disable generational collection.  DEFAULT: 35.
	<expand threshold>       number between 0 and 50 that is the percent
				 allocated after a total collection that will
				 force heap expansion.  DEFAULT: 25.
	<gcthreshold>		 Heap size beyond which MSW performs GC.
				 DEFAULT: 6000000
	<flags>			 controls logging on stderr, error checking,
				 and root finding:
				   & CMM_HEAPROOTS = treat uncollected heap as
				   		   roots
				   & CMM_TSTOBJ = perform object consistency
					        tests
				 DEFAULT: 0.
	<verbose>		 controls logging on stderr:
				   & CMM_STATS =  log collection statistics
				   & CMM_ROOTLOG = log roots found in the stack,
						 registers, and static area
				   & CMM_HEAPLOG = log possible roots in
						 uncollected heap
			 	   & CMM_DEBUGLOG = log events internal to the
						  garbage collector
				 DEFAULT: 0.

   When multiple Cmm declarations occur, the one that specifies the largest
   <maximum heap size> value will control all factors except flags which is
   the inclusive-or of all <flags> values.

   Configured values may be overridden by values supplied from environment
   variables.  The user must set these variables in a consistent manner.  The
   variables and the values they set are:

	CMM_MINHEAP	 <initial heap size>
	CMM_MAXHEAP	 <maximum heap size>
	CMM_INCHEAP	 <increment size>
	CMM_GENERATIONAL <generational>
	CMM_INCPERCENT	 <expand threshold>
	CMM_FLAGS	 <flags>
	CMM_GCTHRESHOLD  <gcthreshold>

   If any of these variables are supplied, then the actual values used to
   configure the garbage collector are logged on stderr.

 *---------------------------------------------------------------------------*/

#ifndef _CMM_H
#define _CMM_H

#define PL_CMM
#define PL_CMM_USE_MSW


#ifndef bool
//typedef int	bool;
#define false	0
#define true	1
#endif

#include <stdio.h>		/* Streams are not used as they might not be
				   initialized when needed. */
#include <stdlib.h>
#include <stddef.h>
//#ifndef NDEBUG
//# define NDEBUG			/* disable assert() */
//#endif
#include <assert.h>
#include <memory.h>
#include <new.h>

#include "machine.h"
#include "msw.h"

/*---------------------------------------------------------------------------*
 *
 * -- Enable CMM features or verbosity
 *
 *---------------------------------------------------------------------------*/

#ifdef CMM_VERBOSE
  #define WHEN_VERBOSE(flag, code)	if (Cmm::verbose & flag) code
#else
  #define WHEN_VERBOSE(flag, code)
#endif

#ifdef CMM_FEATURES
  #define WHEN_FLAGS(flag, code)	if (Cmm::flags & flag) code
#else
  #define WHEN_FLAGS(flag, code)
#endif

/*---------------------------------------------------------------------------*
 *
 * -- CMM External Interface Definitions
 *
 *---------------------------------------------------------------------------*/

class CmmHeap;
class DefaultHeap;
class UncollectedHeap;
class CmmObject;

extern GCP allocatePages(int, CmmHeap *); /* Page allocator		*/
extern void promotePage(GCP cp);

/*---------------------------------------------------------------------------*
 * -- isTraced
 *
 * Predicate isTraced returns true if the object is allocated where it will
 * be scanned by the garbage collector.
 *
 *---------------------------------------------------------------------------*/

extern bool  isTraced(void *);

/*---------------------------------------------------------------------------*
 *
 * Support for rule (d) above. Compiler dependent.
 *
 *---------------------------------------------------------------------------*/

#ifdef __GNUG__
#define VirtualBase(A) &(_vb$ ## A)
#endif
// This should really be #if defined (CFRONT)
#if defined(__sgi) || defined(_sgi) || defined(sgi)
#define VirtualBase(A) &(P ## A)
#endif

/*---------------------------------------------------------------------------*
 *
 * Additional roots may be registered with the garbage collector by calling
 * the procedure gcRoots with a pointer to the area and the size of the area.
 *
 *---------------------------------------------------------------------------*/

extern void  gcRoots(void *area, int bytes);
extern void  gcUnroots(void *addr);

const int CMM_STATS    =   1;	/* Log garbage collector info		*/
const int CMM_ROOTLOG  =   2;	/* Log roots found in registers, stack
				   and static area			*/
const int CMM_HEAPLOG  =   4;	/* Log possible uncollected heap roots	*/
const int CMM_DEBUGLOG =   8;	/* Log events internal to collector	*/
const int CMM_HEAPROOTS = 16;	/* Treat uncollected heap as roots	*/
const int CMM_TSTOBJ   =  32;	/* Extensively test objects		*/

/*---------------------------------------------------------------------------*
 *
 * -- Object Headers
 *
 *---------------------------------------------------------------------------*/
/*
 * Object have headers if HEADER_SIZE is not 0
 */
#define HEADER_SIZE	1	/* header size in words */

#if HEADER_SIZE
#define MAKE_TAG(index) ((index) << 21 | 1)
#define MAKE_HEADER(words, tag) ((tag) | (words) << 1)

#define HEADER_TAG(header) ((header) >> 21 & 0x7FF)
#define HEADER_WORDS(header) ((header) >> 1 & 0xFFFFF) // includes HEADER_SIZE
#define maxHeaderWords 0xFFFFF		/* 1048575 = 4,194,300 bytes */
#define FORWARDED(header) (((header) & 1) == 0)
#else
/* an object is forwarded if it is marked as live and contained
 * in a non-promoted page.
 */
#define FORWARDED(gcp) ((MARKED(gcp) && \
			 pageSpace[GCPtoPage(gcp)] == currentSpace))
#define MAKE_HEADER(words, tag)		
#endif

#if HEADER_SIZE
#define ALLOC_SETUP(object, words) \
  *object = MAKE_HEADER(words, MAKE_TAG(2)); \
  object += HEADER_SIZE; \
  SET_OBJECTMAP(object)
#else
#define ALLOC_SETUP(object, words) \
  SET_OBJECTMAP(object)
#endif

#define MARKING
/*
 * The base address of CmmObject's is noted in the objectMap bit map.  This 
 * allows CmmMove() to rapidly detect a derived pointer and convert it into an
 * object and an offset.
 */

extern int  firstHeapPage;	/* Page # of first heap page		*/
extern int  lastHeapPage;	/* Page # of last heap page		*/
extern int  firstFreePage;      /* First possible free page		*/
extern unsigned long *objectMap; /* Bitmap of 1st words of user objects	*/
#if !HEADER_SIZE || defined(MARKING)
extern unsigned long *liveMap;	/* Bitmap of objects reached during GC	*/
#endif
extern short *pageSpace;	/* Space number for each page		*/
extern short *pageGroup;	/* Size of group of pages		*/
extern int   *pageLink;		/* Page link for each page		*/
extern CmmHeap **pageHeap;	/* Heap to which each page belongs	*/
extern int   tablePages;	/* # of pages used by tables		*/
extern int   firstTablePage;	/* index of first page used by table	*/
extern int   freePages;		/* # of pages not yet allocated		*/


#define WORD_INDEX(p)	(((unsigned)(p)) / (bitsPerWord * bytesPerWord))
#define BIT_INDEX(p)	((((unsigned)(p)) / bytesPerWord) & (bitsPerWord - 1))

#define IS_OBJECT(p)	   (objectMap[WORD_INDEX(p)] >> BIT_INDEX(p) & 1)
#define SET_OBJECTMAP(p)   (objectMap[WORD_INDEX(p)] |= 1 << BIT_INDEX(p))
#define CLEAR_OBJECTMAP(p) objectMap[WORD_INDEX(p)] &= ~(1 << BIT_INDEX(p))

#define MARKED(p)	(liveMap[WORD_INDEX(p)] >> BIT_INDEX(p) & 1)
#define MARK(p)		(liveMap[WORD_INDEX(p)] |= 1 << BIT_INDEX(p))


/*---------------------------------------------------------------------------*
 *
 * -- C++ Garbage Collected Storage Interface Definitions
 *
 *---------------------------------------------------------------------------*/

/* Declarations for objects not directly used by the user of the interface. */

/*	Page setting					*/

/* bytesPerPage controls the number of bytes per page.
 * It must be a multiple of bitsPerWord.
 */
#define bytesPerPage 1024
#define wordsPerPage (bytesPerPage / bytesPerWord)
#define bytesPerWord (sizeof(long))
#define	bitsPerWord  (8*bytesPerWord)

/* Page number <--> pointer conversion */

#define pageToGCP(p) ((GCP)(((unsigned long)p)*bytesPerPage))
#define GCPtoPage(p) (((unsigned long)p)/bytesPerPage)

/* The following define is used to compute the number of words needed for
 * an object.
 */

#if HEADER_SIZE || ! defined(DOUBLE_ALIGN)
#define	bytesToWords(x) ((((x) + bytesPerWord-1) / bytesPerWord) + HEADER_SIZE)
#else
#  define DOUBLE_ALIGN_OPTIMIZE
#  ifdef DOUBLE_ALIGN_OPTIMIZE
/* CmmObject's smaller than 16 bytes (including vtable) cannot contain
   doubles (the compiler must add padding between vtable and first float)
*/
#   define bytesToWords(x) (((x) < 16) ? \
			    (((x) + bytesPerWord-1) / bytesPerWord) : \
			    (((x) + 2*bytesPerWord-1) / (2*bytesPerWord) * 2))
#  else // !DOUBLE_ALIGN_OPTIMIZE
#   define bytesToWords(x) (((x) + 2*bytesPerWord-1) / (2*bytesPerWord) * 2)
#  endif // !DOUBLE_ALIGN_OPTIMIZE
#endif

/* Page space values */

#define pageIsStable(x) 	(! pageIsUnstable(x))
#define pageIsUnstable(x) 	(pageSpace[x] & 1)
#define UNALLOCATEDSPACE	1 /* neither current nor stable	*/
#define PROMOTEDSPACE		0 /* page being promoted	*/
#define SCANNED(page)		(pageSpace[page] == nextSpace)
#define SET_SCANNED(page)	(pageSpace[page] = nextSpace)

#define NOHEAP NULL
#define UNCOLLECTEDHEAP ((CmmHeap *)1)

#define OUTSIDE_HEAP(page) \
	(page < firstHeapPage || page > lastHeapPage || \
	 pageHeap[page] == UNCOLLECTEDHEAP)

#define HEAPPERCENT(x) (((x)*100)/(Cmm::theDefaultHeap->reservedPages \
			+ freePages))

/* Default heap configuration */

const int  CMM_MINHEAP      = 131072; /* # of bytes of initial heap	*/
const int  CMM_MAXHEAP      = 2147483647; /* # of bytes of the final heap */
const int  CMM_INCHEAP      = 1048576;    /* # of bytes of each increment */
const int  CMM_GENERATIONAL = 35;	  /* % allocated to force total
					   collection		       	*/
const int  CMM_GCTHRESHOLD  = 6000000; /* Heap size before MSW starts GC */
const int  CMM_INCPERCENT   = 25;     /* % allocated to force expansion */
const int  CMM_FLAGS        = 0;      /* option flags			*/

/*---------------------------------------------------------------------------*
 * -- Static Memory Areas
 *---------------------------------------------------------------------------*/

extern Word	stackBottom;	/* The base of the stack	*/
extern "C" Ptr	CmmGetStackBase(void);
extern "C" void	CmmExamineStaticAreas(void (*)(GCP, GCP));
extern "C" void	CmmSetStackBottom(Word);


/*---------------------------------------------------------------------------*
 *
 * -- Cmm
 *
 *---------------------------------------------------------------------------*/

class Cmm
{
 public:
  Cmm(int newMinHeap,
      int newMaxHeap,
      int newIncHeap,
      int newThreshold,
      int newIncPercent,
      int newGcThreshold,
      int newFlags,
      int newVerbose);

  static DefaultHeap *theDefaultHeap;
  static UncollectedHeap *theUncollectedHeap;
  static CmmHeap *heap;
  static CmmHeap *theMSHeap;
  static char*  version;
  static int verbose;
  static int  minHeap;		/* # of bytes of initial heap	*/
  static int  maxHeap;		/* # of bytes of the final heap */
  static int  incHeap;		/* # of bytes of each increment */
  static int  gcThreshold;	/* heap size before start gc    */
  static int  generational;	/* % allocated to force total collection */
  static int  incPercent;	/* % allocated to force expansion */
  static int  flags;		/* option flags			*/
  static bool defaults;		/* default setting in force	*/
  static bool created;		/* boolean indicating heap created */
};

/*---------------------------------------------------------------------------*
 *
 * -- Heaps
 *
 *---------------------------------------------------------------------------*/

class CmmHeap
{
 public:

  CmmHeap()
    {
      opaque = false;
    }

  virtual GCP   alloc(unsigned long) = 0;
  virtual void  reclaim(GCP) {};
  virtual void  scanRoots (int) {};

  virtual void collect()
    {
      fprintf(stderr, "Warning: Garbage Collection on a non collectable heap");
    }

  virtual void scavenge(CmmObject **) {};

  inline bool inside(GCP ptr)
    {
      int page = GCPtoPage(ptr); /* Page number */
      return (page >= firstHeapPage && page <= lastHeapPage
	      && pageHeap[page] == this);
    }

  inline void visit(CmmObject *); // defined later, after CmmObject

  inline bool isOpaque() { return opaque; }
  inline void setOpaque(bool opacity)
    { opaque = opacity; }

 private:
  bool opaque;			/* controls whether collectors for other heaps
				 * should traverse this heap
				 */
};


/*---------------------------------------------------------------------------*
 *
 * -- UncollectedHeap
 *
 *---------------------------------------------------------------------------*/

class UncollectedHeap: public CmmHeap
{
public:

  GCP alloc(unsigned long size) { return (GCP)malloc(size); }

  void reclaim(GCP ptr) { free(ptr); }
  void scanRoots	(int page);
};


CmmObject *basePointer(GCP);


/*---------------------------------------------------------------------------*
 *
 * -- The DefaultHeap
 *
 *---------------------------------------------------------------------------*/

class DefaultHeap: public CmmHeap
{
public:
  
  DefaultHeap();
  GCP alloc(unsigned long);
  void reclaim(GCP) {}		// Bartlett's delete does nothing.
  void collect();		// the default garbarge collector
  void scavenge(CmmObject **ptr);
  GCP  reservePages(int);
  
  int usedPages;		// pages in actual use
  int reservedPages;		// pages reserved for this heap
  int stablePages;		// # of pages in the stable set
  int firstUnusedPage;		// where to start lookiing for unused pages
  int firstReservedPage;	// first page used by this Heap
  int lastReservedPage;		// last page used by this Heap
};

/*---------------------------------------------------------------------------*
 *
 * -- MarkAndSweep heap
 *
 *---------------------------------------------------------------------------*/

class MarkAndSweep : public CmmHeap 
{

 public:

  inline GCP 		alloc	(unsigned long size)
  					       { return (GCP) mswAlloc(size); }
  inline void 		reclaim	(GCP p)        { mswFree(p); }
  inline void 		collect	()	       { mswCollect(); }
  inline void 		collectNow()	       { mswCollectNow(); }
  inline void*		realloc (void * p, unsigned long size)
                              { return mswRealloc(p, size); }
  inline void*		calloc  (unsigned long n, unsigned long size)
  			      { return mswCalloc(n, size); }

  inline void		checkHeap()		{ mswCheckHeap(1); }
  inline void		showInfo()		{ mswShowInfo(); }
  
  MarkAndSweep(unsigned mode = MSW_Automatic)
    {
      Cmm::theMSHeap = this;
      mswInit(mode);
    }

  void			tempHeapStart ()	{ mswTempHeapStart(); }
  void			tempHeapEnd   ()	{ mswTempHeapEnd(); }
  void			tempHeapFree  ()	{ mswTempHeapFree(); }
  void			tempHeapRegisterRoot (void* ptr)
  					{ mswRegisterRoot(ptr); }

  void			scanRoots(int page);

};

/*---------------------------------------------------------------------------*
 *
 * -- CmmObjects
 *
 *---------------------------------------------------------------------------*/

class CmmObject
{
public:

  virtual void traverse() {} ;

  virtual ~CmmObject() {} ;

  CmmHeap *heap() { return pageHeap[GCPtoPage(this)]; }

  inline int size() { return (words()*bytesPerWord); }

#if HEADER_SIZE
  inline int words() { return HEADER_WORDS(((GCP)this)[-HEADER_SIZE]); }
#else
  int words();
#endif

#ifdef MARKING
  inline void mark() { MARK(this); }

  inline bool isMarked() { return (MARKED(this)); }
#endif

  inline int forwarded()
    {
#if HEADER_SIZE
      return FORWARDED(((GCP)this)[-HEADER_SIZE]);
#else
      extern int currentSpace;
      return FORWARDED(((GCP)this));
#endif
    }
  inline void SetForward(CmmObject *ptr)
    {
#if !HEADER_SIZE
      MARK(this);
#endif
      ((GCP)this)[-HEADER_SIZE] = (int)ptr;
    }
  inline CmmObject *GetForward()
    {
      return (CmmObject *) ((GCP)this)[-HEADER_SIZE];
    }
  inline CmmObject *next() {return (CmmObject *)(((GCP)this) + words()); }

  void* operator new(size_t, CmmHeap* = Cmm::heap);
  void operator delete(void *);

  void* operator new[](size_t size, CmmHeap *heap = Cmm::heap);
  void  operator delete[](void* obj);

};

class CmmVarObject: public CmmObject 
{
public:
  void* operator new(size_t, size_t = (size_t)0, CmmHeap* = Cmm::heap);
};

/*---------------------------------------------------------------------------*
 *
 * -- Arrays of CmmObjects
 *
 *---------------------------------------------------------------------------*/

// Class CmmArray must be used to create arrays of CmmObject's as follows:
//
//       CmmArray<MyClass> & MyVector = * new (100) CmmArray<MyClass> ;
//       
// Then you can use the [] operator to get CmmObjects as usual.
// Ex:
//       MyVector[i]->print();
// or:
//       MyClass mc = MyVector[3];
//

template <class T>
class CmmArray : public CmmObject
{
public:

  void * operator new(size_t s1, size_t s2 = 0, CmmHeap* hz = Cmm::heap)
    {
      // tito: allocate just s2-1, because the other one
      // is already in s1=sizeof(CmmArray<T>)
      size_t size = s1 + sizeof(T) * (s2-1);
      void* res = new (size, hz) CmmVarObject;

      // clear the array so that if collect is called during the execution of
      // this function, traverse will skip empty elements
      bzero((char*) &(((CmmArray<T> *)res)->ptr[0]), s2*sizeof(T));

      T* array = (T*)&(((CmmArray<T> *)res)->ptr[0]);
      // tito: array[0] should be already initialized by the compiler:
      // start from i=1.
      for (size_t i = 1; i < s2; i++)
	{
	  size_t preserve = (size_t)&(((CmmArray<T> *)res)->ptr[i]);
	  ::new (&(((CmmArray<T> *)res)->ptr[i])) T;
	}
      return res;
    }
  
  ~CmmArray()
    {
      size_t i;
      unsigned int count = ((size() - sizeof(CmmArray)) / sizeof(T)) + 1;
      for (i = 1; i < count; ++i)
	ptr[i].~T();
    }
  
  T & operator[](unsigned int index) { return ptr[index]; }
  
  void traverse()
    {
      unsigned int count = ((size() - sizeof(CmmArray)) / sizeof(T)) + 1;
      for (int i = 0; i < count; i++)
	if (((int*)ptr)[i])
	  ptr[i].traverse();
    }
  
private:
  T ptr[1];
};

/*---------------------------------------------------------------------------*/

inline void CmmHeap::
visit(CmmObject *ptr)
{
#ifdef MARKING
  if (!ptr->isMarked())
    {
      ptr->mark();
      ptr->traverse();
    }
#else
  ptr->traverse();
#endif
}

/*---------------------------------------------------------------------------*
 *
 * -- Library initialization
 *
 *---------------------------------------------------------------------------*/

class _CmmInit
{
public:
  _CmmInit()
    {
      extern void CmmInitEarly();
      
      if (Cmm::theDefaultHeap == 0) {
	CmmInitEarly();
	
	Cmm::theDefaultHeap = new DefaultHeap;

#	if defined(PL_CMM_USE_MSW)
 		Cmm::heap = ::new MarkAndSweep();
#	else
		Cmm::heap = Cmm::theDefaultHeap;
#	endif

	Cmm::theUncollectedHeap = new UncollectedHeap;
      }
    }
  ~_CmmInit() {};		// destroy _DummyCmmInit after loading cmm.h
};

/*---------------------------------------------------------------------------*
 * Back compatibility
 *---------------------------------------------------------------------------*/

#define GcObject	CmmObject
#define GcVarObject	CmmVarObject
#define GcArray		CmmArray

#endif				// _CMM_H
