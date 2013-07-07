/*****************************************************************************
 *
 * cport.h: Things which differ between C environments.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _CPORT_H_
#define _CPORT_H_

# include "stdc.h"

/****************************************************************************
 *
 * C extensions and insulations.
 *
 * Here we describe the macros which insulate from things which require
 * different syntax in different C compilers or which require extra
 * care for portability.
 *
 *
 * * Constant declarations:
 *	ANSI C permits names to be declared as constants.  Sometimes
 *	it is not just permitted, but *necessary*.  I.e. when redefining
 *	a system function (printf) the argument declarations must match
 *	those in the system header file (stdio.h).  Since "const"
 *	In older C compilers in which "const" is not legal, we provide a
 *	macro "const" which expands to nothing.
 *
 * * Types:
 *
 *	Bool
 *	Hash
 *	Length
 *	Offset
 *	Millisec
 *		Integers for specific uses.
 *
 *	AInt
 *		Signed integer type with same size as "Pointer".
 *
 *	UByte
 *	UShort
 *	ULong
 *	UAInt
 *		Unsigned integers of specific sizes.
 *
 *	Pointer
 *	ConstPointer
 *		These are used as generic pointers to data.
 *		Arithmetic and comparison are not defined on "pointer"s
 *		(unless they are produced by ptrNormalize).
 *		ConstPointer is a pointer to constant data.
 *
 *	String
 *		Null-terminated string.
 *	CString
 *		Null-terminated const string.
 *
 *	IOMode
 *		Strings for specific uses.
 *
 *	SFloat
 *	DFloat
 *		Floating-point numbers of specific sizes.
 *
 *	LongDouble
 *		long double, if supported, otherwise double.
 *
 *	MostAlignedType
 *		This is the type with the strongest alignment restrictions.
 *      UNotAsLong
 *              This is the largest int smaller than long (used in bigint)
 *      U16
 *              Unsigned 16 bits. 
 *	U16sPerUNotAsLong
 *		Number of U16 in each UNotAsLong
 *
 *
 * * Bit sizes of types:
 *
 *	bitsizeof(t) returns the number of bits in type t.  E.g.
 *		struct {
 *			int	x : K;
 *			int	n : bitsizeof(int) - K;
 *		}
 *
 * * Using variable number of function arguments:
 *	We follow the ANSI <stdarg.h> conventions.
 *	If we have to, we implement it ourselves.
 *
 * * Token formation:
 *	The "Abut" macro is used to concatenate tokens.
 *
 *	Abut(a,b)
 *
 *	In ANSI C, this expands to a##b and in other C environments
 *	the old comment trick is used.
 *	Use Abut(a,b) rather than Abut(a, b) since spaces are significant
 *	in K&R C.
 *
 *	The "Enstring" macro is used to string-ize a token.
 *
 *	Enstring(a)
 *
 *	In ANSI C, this expands to #a and in other C environments
 *	the fact that cpp substitutes into strings is used.
 *
 *
 * * Trailing arrays:
 *	The "NARY" macro is used for trailing variable sized arrays.
 *
 *	The "fullsizeof" macro is used for the size of a structure with
 *	a given number of trailing components.	The arguments of must be
 *	suitable for "sizeof".
 *
 *		struct mything {
 *			int		code;
 *			struct jazz	jazz;
 *			double		numbers[NARY];
 *		} *p;
 *
 *		p = (struct mything *) malloc(fullsizeof(*p, argc, double));
 *		int printf(char const *, fmt) { ...
 *
 * * Structure field offset manipulations.
 *	These are useful when one wants to iterate over various fields
 *	in a fixed array of structs.
 *	E.g.
 *		...
 *			phStats(" Time %6ld ms", fieldOffset(phInfo, time ));
 *			phStats(" Store%6ld B ", fieldOffset(phInfo, store));
 *		}
 *
 *		phStats(char *fmt,  int offset)
 *		{
 *			long	tot;
 *			for (i = 0; i < PH_MAX; i++)
 *				tot += * (long *) fieldAddr(phInfo+i, offset);
 *		...
 *		}
 *
 * * Type alignment.
 *	"alignof" gives alignment of a type in the same units as "sizeof".
 *	E.g.	alignof(double) is sometimes 8 and sometimes 4.
 *
 *	isPtrMinAligned(p)  determines whether p is aligned enough
 *			    to point possibly to a pointer.
 *	isDataMaxAligned(p) determines whether p is aligned enough
 *			    to point to anything.
 *
 * * Integer compaction
 *	"Pack" and "BPack" allow code to compactly store values in structures
 *	while keeping the logical type explicit.
 *
 *	Pack(enum ee, short) expands to "short".
 *	BPack(xyzzy)	     is equivalent to Pack(xyzzy, UByte).
 *
 * * Function scope
 *	The "local" declaration is used to declare a function local to a file.
 *	This allows more accurate profiling information, or to check for name
 *	collision if files are to be joined.
 *
 *	The code
 *		local char * f();
 *
 *	becomes
 *		char * f();		-- ifdef NLOCAL
 *		static char * f();	-- otherwise
 *
 *	(Of course one still uses "static" declarations for data,
 *	 where appropriate.)
 *
 * * Forward references to enumerations
 *	Not all compilers allow forward references to enumerations
 *	(e.g. Metaware High C on AIX370).  Use "Enum".
 *
 *		Enum(foo)
 *
 *	becomes
 *
 *		enum foo		-- normally
 *		int			-- for a finicky compiler
 *
 * * Empty files
 *	Not all compilers can compile files without definitions
 *	(e.g. Waterloo C on CMS).
 *	Include the following line at the end of such C files:
 *
 *		ThatsAll
 *
 * * Special values:
 *		true
 *		false
 *		long0		-- long integer 0
 *		int0		-- integer 0
 *		char0		-- character 0
 *		TAB_STOP
 *
 * * Macro tools
 *	Nothing
 *	   Different compilers complain about different sorts of empty
 *	   expressions so the macro Nothing is provided to use as a
 *	   low-grief, side-effect free expression.
 *	   E.g.
 *		#ifdef NDEBUG
 *		# define DEBUG(x)	Nothing
 *		#endif
 *
 *	STATEMENT(s)
 *	   This macro has the same effect as "s" but can be used safely
 *	   in any statement context.
 *
 *	   E.g.	 Given
 *
 *		#define DEBUG_GOOD(s)  STATEMENT(if (_debug_) {s;})
 *		#define DEBUG_BAD(s)   if (_debug_) {s;}
 *
 *		if (foo) DEBUG_GOOD(bar); else baz;
 *		if (foo) DEBUG_BAD(bar);  else baz;
 *
 *	   then the "else baz" would pair with the "if (foo)" for DEBUG_GOOD
 *	   but with the "if (_debug_)" for DEBUG_BAD.
 *
 *      IgnoreResult(E)
 *         Ignore the return value of E; eg. IgnoreResult(scanf(...)).
 *         Should be avoided in new code, but use to clean up compiler warnings
 *         from older code.
 *
 *  * NotReached
 *	This macro has two uses:
 *	(1) to trap bugs where supposedly  impossible situations arise, and
 *	(2) to gag compilers which incorrectly deduce that a value is missing.
 *
 *	E.g.  Some compilers complain about r not having a value in f(r) below:
 *		int a, r;
 *		switch (a % 3) {
 *		case 0:	 case 1:  case 2: r = 0; break;
 *		}
 *		f(r)
 *
 *	   So use
 *		int a, r;
 *		switch (a % 3) {
 *		case 0:	 case 1:  case 2: r = 0; break;
 *		default: NotReached(r = 0);
 *		}
 *		f(r)
 *
 *  * Pointer conversion
 *	C does not allow subtraction of pointers into different objects, nor
 *	are the results of conversion to integers standard.
 *	These macros allow low-level manipulation of pointers in a portable way:
 *
 *	Pointer ptrCanon  (Pointer);
 *		Produce a pointer in canonical form.
 *		The results can be compared (e.g. using ==, <, >).
 *
 *	Pointer ptrOff	  (const char *p, long offset);
 *		Produce a canonical pointer offset a given distance from p.
 *
 *	Bool	ptrEqual  (Pointer, Pointer);
 *		Compare two possible non-canonical pointers.
 *
 *	long	ptrDiff	  (const char *a, const char *b);
 *		Compute the difference between possibly non-canonical pointers.
 *
 *	long	ptrToLong (Pointer);
 *		Produce an integer value equal to an offset from 0 in a flat
 *		address space.
 *
 *	Pointer ptrFrLong (long);
 *		Produce a pointer for an offset from 0 in a flat address space.
 *		This may involve splitting the integer into a segment/offset
 *		representation.
 *
 *	Bool	ptrGT(SomePointerType a, SomePointerType b)
 *	Bool	ptrGE(SomePointerType a, SomePointerType b)
 *	Bool	ptrLT(SomePointerType a, SomePointerType b)
 *	Bool	ptrLE(SomePointerType a, SomePointerType b)
 *	Bool	ptrEQ(SomePointerType a, SomePointerType b)
 *	Bool	ptrNE(SomePointerType a, SomePointerType b)
 *		These macros compare pointers according to their
 *		canonical values.
 *
 *  * Integer byte-ordering
 *	BYTEn and UNBYTEn provide a portable way to access the bytes of
 *	integers of different sizes.
 *
 *  * Character set conversion
 *	charToAscii and charFrAscii provide a portable translation to and from
 *	the Ascii character set for use in Aldor Library files.
 *
 *  * Floating-point characteristics
 *
 *	{SF,DF}_HasNANs      --	Does it have NANs + INFs?
 * 	{SF,DF}_HasNorm1     -- Does it normally have an implicit 1 bit?
 * 	{SF,DF}_LgLgBase     -- Log base 2 of Log base 2 of fraction radix.
 * 	{SF,DF}_Excess       -- Exponent excess.
 * 	{SF,DF}_FracOff      -- Big-endian bit offset of fraction.
 *	SF_no_denorms	     -- denormed small floats are bad?
 *
 *      {SF,DF}_UByte(px,i)  -- The i-th byte in big-endian order.
 *      {SF,DF}_UShort(px,i) -- The i-th short byte in big-endian order.
 *
 *  * Special declaration modifiers
 *
 *	SignalModifier  modifies pointers to signal handler functions.
 *
 ****************************************************************************/

/*****************************************************************************
 *
 * :: Constant Declarations
 *
 ****************************************************************************/

#ifdef CC_no_const
#  define const		/* const */
#endif

/*****************************************************************************
 *
 * :: Types
 *
 ****************************************************************************/

typedef unsigned char	UByte;
typedef unsigned short	UShort;
typedef unsigned long	ULong;

#ifdef CC_short_pointer
  typedef short		 AInt;
  typedef unsigned short UAInt;
# define AINT_FMT "%d"
#else
  typedef long		 AInt;
  typedef unsigned long	 UAInt;
# define AINT_FMT "%ld"
#endif

#ifdef CC_second_largest_is_int
typedef unsigned int 	UNotAsLong;
#define U16sPerUNotAsLong 2
#else
typedef unsigned short 	UNotAsLong;
#define U16sPerUNotAsLong 1
#endif

typedef unsigned short	U16;
typedef int		Bool;
typedef UAInt		Hash;
typedef size_t		Length;
typedef ULong		Offset;
typedef ULong		Millisec;

#ifdef CC_no_void_pointer
   typedef char		*Pointer;
   typedef const char	*ConstPointer;
#else
   typedef void		*Pointer;
   typedef const void	*ConstPointer;
#endif

typedef char		*String;
typedef const char	*CString;

typedef char		*IOMode;

/*
 *  On some architecutres, we use a double to represent a single float so that
 *  a single float has the same size as a pointer
 */
#ifdef CC_SF_is_double
  typedef double	SFloat;
#else
  typedef float		SFloat;
#endif
typedef double		DFloat;

#ifdef CC_no_long_double
   typedef double	LongDouble;
#else
   typedef long double	LongDouble;
#endif

typedef double	MostAlignedType;

#define boolToString(b) ((b) ? "true" : "false")

/*****************************************************************************
 *
 * :: Bit sizes
 *
 ****************************************************************************/

#define bitsizeof(X)	(CHAR_BIT * sizeof(X))

/*****************************************************************************
 *
 * :: Using variable number of function arguments
 *
 ****************************************************************************/

/*
 * Handled by stdarg.h0
 */

/*****************************************************************************
 *
 * :: Token Formation
 *
 ****************************************************************************/

#ifndef CC_no_token_paste
#  define Abut(a,b)	a##b
#  define Enstring(a)	#a
#else
#  define Abut(a,b)	a/**/b
#  define Enstring(a)	"a"
#endif

/*****************************************************************************
 *
 * :: Trailing arrays
 *
 ****************************************************************************/

# define NARY			10 /* Enough to quiet bounds checking CC's */
# define fullsizeof(hty,n,aty)	(sizeof(hty) + (n) * sizeof(aty) - NARY * sizeof(aty))

/*****************************************************************************
 *
 * :: Structure field offset manipulations
 *
 ****************************************************************************/

# define fieldAddr(p, offset)	( (char *)(p) + (offset) )
# define fieldOffset(p, field)	( (char *)(&((p)->field)) - (char *)(p) )

/*****************************************************************************
 *
 * :: Type alignment
 *
 ****************************************************************************/

# define alignof(t)		(sizeof(struct {char a; t b;}) - sizeof(t))
# define isPtrMinAligned(p)	(((unsigned long)(p)) % alignof(long *) == 0)
# define isDataMaxAligned(p)	(((unsigned long)(p)) % alignof(MostAlignedType) == 0)

/*****************************************************************************
 *
 * :: Integer compaction
 *
 ****************************************************************************/

/* Define UnBPack to allow unpacked integers for debugging purposes. */

# define BPack(unpacked)	Pack(unpacked, UByte)
#ifdef UnBPack
# define Pack(unpacked, packed) unpacked
#else
# define Pack(unpacked, packed) packed
#endif

/*****************************************************************************
 *
 * :: Function scope
 *
 ****************************************************************************/

#ifdef NLOCAL
# define local /* extern */
#else
# define local static
#endif

/*****************************************************************************
 *
 * :: Forward references to enumerations
 *
 ****************************************************************************/

#ifndef CC_no_enum_typedef
#  define Enum(foo)	enum foo
#else
#  define Enum(foo)	int
#endif

/*****************************************************************************
 *
 * :: Empty files
 *
 ****************************************************************************/

#ifndef CC_no_empty_file
#  define ThatsAll
#else
#  define ThatsAll	int _ThatsAll_(x) int x; {return x;}
#endif

/*****************************************************************************
 *
 * :: Special values
 *
 ****************************************************************************/

# define true		1
# define false		((int)  0)

# define long0		((long) 0)
# define int0		((int)  0)
# define char0		((char) 0)

# define TABSTOP	8   /* tabs expand to every TABSTOP chars */

/*****************************************************************************
 *
 * :: Macro tools
 *
 ****************************************************************************/

# define Nothing
# define Statement(stat){ stat; }	/* do { stat; } while(0) */
# define IgnoreResult(E) if (E);

/*****************************************************************************
 *
 * :: Not reached
 *
 ****************************************************************************/

# define NotReached(stat)	\
  {(void)printf("Not supposed to reach line %d in file: %s\n",__LINE__, __FILE__); \
   stat;}

/*****************************************************************************
 *
 * :: Pointer conversion
 *
 ****************************************************************************/

#ifdef CC_noncanonical_pointer

extern Pointer	ptrCanon (Pointer);
extern Pointer	ptrOff	 (const char *, long);
extern Bool	ptrEqual (Pointer, Pointer);
extern long	ptrDiff	 (const char *, const char *);
extern long	ptrToLong(Pointer);
extern Pointer	ptrFrLong(long);

#else

# define	ptrCanon(p)	((Pointer)(p))
# define	ptrOff(p,n)	((Pointer)((char *)(p) + (n)))
# define	ptrEqual(p,q)	((Pointer)(p) == (Pointer)(q))
# define	ptrDiff(p1,p2)	((long)((char *)(p1) - (char *)(p2)))
# define	ptrToLong(p)	((long)	  (p))
# define	ptrFrLong(l)	((Pointer)(l))

#endif /* !CC_noncanonical_pointer */

#define ptrEQ(a,b)	( ptrEqual((Pointer)(a), (Pointer)(b)))
#define ptrNE(a,b)	(!ptrEqual((Pointer)(a), (Pointer)(b)))
/*-----------------------------------------------------------------------------
 * The following definitions may require work to be acceptable across
 * platforms.  However, any replacement MUST use effectively UNSIGNED
 * comparisons if there is any possibility that they are to be used on
 * a platform where Pointers are as long as Long's.
 *
 * Most C compilers ought to be able to Do The Right Thing with the
 * definitions below, as the corresponding C++ compiler is obliged to
 * support std::less<Pointer>() & co,  which is obliged to be a consistent
 * total order on the Pointer type regardless of where the pointers
 *  are found.
 *-----------------------------------------------------------------------------
 */

#define ptrGT(a,b)	(ptrCanon(a) >  ptrCanon(b))
#define ptrGE(a,b)	(ptrCanon(a) >= ptrCanon(b))
#define ptrLT(a,b)	(ptrCanon(a) <  ptrCanon(b))
#define ptrLE(a,b)	(ptrCanon(a) <= ptrCanon(b))

/*****************************************************************************
 *
 * :: Integer byte-ordering
 *
 * The following macros are used to ensure portable data formats for Aldor
 * library files across different platforms.
 *
 * The size macros involved represent numbers of bits/bytes written
 * to library files, and not sizes of data structures in memory.
 * As a result, they must not use 'sizeof' to create their value.
 *
 ****************************************************************************/

# define BYTE_BYTES		1	/* Not sizeof(...)!  See above. */
# define HINT_BYTES		2	/* Not sizeof(...)!  See above. */
# define SINT_BYTES		4	/* Not sizeof(...)!  See above. */

# define BYTE_BITS		8	/* Not sizeof(...)!  See above. */
# define BYTE_MASK		((1<<BYTE_BITS)-1)

# define MAX_BYTE		((1<<(1*BYTE_BITS))-1)
# define MAX_HINT		UNBYTE2(MAX_BYTE,MAX_BYTE)
# define MAX_SINT		UNBYTE4(MAX_BYTE,MAX_BYTE,MAX_BYTE,MAX_BYTE)

# define UBYTE0(b)		BYTE0(b)

# define HBYTE0(b)		BYTE0(b)
# define HBYTE1(b)		BYTE1(b)

# define BYTE0(b)		(((ULong) b)&BYTE_MASK)
# define BYTE1(b)		BYTE0((b)>>(1*BYTE_BITS))
# define BYTE2(b)		BYTE0((b)>>(2*BYTE_BITS))
# define BYTE3(b)		BYTE0((b)>>(3*BYTE_BITS))

# define UNBYTE1(b0)		BYTE0(b0)
# define UNBYTE2(b0,b1)		(BYTE0(b0) | (BYTE0(b1)<<BYTE_BITS))
# define UNBYTE4(b0,b1,b2,b3)	(UNBYTE2(b0,b1)|(UNBYTE2(b2,b3)<<(2*BYTE_BITS)))

/*****************************************************************************
 *
 * :: Character set conversion
 *
 * The following macros are used to ensure portable data formats for Aldor
 * library files across different platforms.
 *
 ****************************************************************************/

#ifdef CC_ebcdic_chars

extern short __ebcdic[];
extern short __ascii[];

#  define charToAscii(e)	(__ascii[(e)])
#  define charFrAscii(a)	(__ebcdic[(a)])

extern String strToAscii(String, String, int);
extern String strFrAscii(String, String, int);

#  define strnToAscii(e,cc)	strToAscii(e,e,cc)
#  define strnFrAscii(a,cc)	strFrAscii(a,a,cc)

#else

#  define charToAscii(e)	(e)
#  define charFrAscii(a)	(a)

#  define strToAscii(e,a,cc)	(e)
#  define strFrAscii(a,e,cc)	(a)

#  define strnToAscii(e,cc)	(e)
#  define strnFrAscii(a,cc)	(a)

#endif /* !CC_ebcdic_chars */

/*****************************************************************************
 *
 * :: Floating-point characteristics
 *
 ****************************************************************************/

#ifdef CC_non_ieee_floats

#ifdef HW_370
#define SF_HasNANs     0
#define SF_HasNorm1    0
#define SF_LgLgBase    2
#define SF_Excess      0x40
#define SF_FracOff     8

#define DF_HasNANs     0
#define DF_HasNorm1    0
#define DF_LgLgBase    2
#define DF_Excess      0x40
#define DF_FracOff     8
#endif /* HW_370 */

#ifdef HW_VAX

#define SF_HasNANs	0
#define SF_HasNorm1	1
#define SF_LgLgBase	0
#define SF_Excess	129
#define SF_FracOff	9

#define DF_HasNANs	0
#define DF_HasNorm1	1
#define DF_LgLgBase	0
#define DF_Excess	129
#define DF_FracOff	9

#endif /* HW_VAX */


#else

/* IEEE */
#ifdef CC_SF_is_double
#  define SF_HasNANs     1
#  define SF_HasNorm1    1
#  define SF_LgLgBase    0
#  define SF_Excess      0x3ff
#  define SF_FracOff     12
#else
#  define SF_HasNANs     1
#  define SF_HasNorm1    1
#  define SF_LgLgBase    0
#  define SF_Excess      0x7f
#  define SF_FracOff     9
#endif

#define DF_HasNANs     1
#define DF_HasNorm1    1
#define DF_LgLgBase    0
#define DF_Excess      0x3ff
#define DF_FracOff     12

#ifdef CC_no_small_denorms
#define SF_NoDenorms
#endif

#endif /* !CC_non_ieee_floats */

#ifdef CC_little_endian
# define TixPart(W,P,i) (sizeof(W)/sizeof(P) - (i) - 1)
#else

#ifdef CC_vax_endian

#define TixPart(W,P,i) ((i) & 0x0001 ? (i) - 1 : (i) + 1)

#else

# define TixPart(W,P,i) (i)

#endif
#endif

#define FiWord_UByte(px, i)  (((UByte  *) (px))[TixPart(FiWord, UByte, i)])
#ifdef CC_SF_is_double
#  define SF_UByte(px,i)  (((UByte  *) (px))[TixPart(double, UByte, i)])
#else
#  define SF_UByte(px,i)  (((UByte  *) (px))[TixPart(float, UByte, i)])
#endif
#define DF_UByte(px,i)  (((UByte  *) (px))[TixPart(double,UByte, i)])

#ifdef CC_vax_endian

#define SF_UShort(px,i)  (SF_UByte(px,i+1) | (SF_UByte(px,i) << CHAR_BIT))
#define DF_UShort(px,i)   (DF_UByte(px,i+1) | (DF_UByte(px,i) << CHAR_BIT))

#else  /* !CC_vax_endian */

#ifdef CC_SF_is_double
#  define SF_UShort(px,i) (((UShort *) (px))[TixPart(double, UShort,i)])
#else
#  define SF_UShort(px,i) (((UShort *) (px))[TixPart(float, UShort,i)])
#endif
#define DF_UShort(px,i) (((UShort *) (px))[TixPart(double,UShort,i)])

#endif
/*****************************************************************************
 *
 * :: Special declaration modifiers
 *
 * The following modifiers control calling or linkage conventions
 * for particular platforms.
 *
 ****************************************************************************/

#if defined(OS_IBM_OS2) && defined(CC_BORLAND)
# define SignalModifier __cdecl
#endif

#if defined(OS_IBM_OS2) && defined(CC_CSET2)
# define SignalModifier _Optlink
#endif

#ifndef SignalModifier
# define SignalModifier
#endif


#endif /* !_CPORT_H_ */
