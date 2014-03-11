/*****************************************************************************
 *
 * util.h: General low-level utility functions.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _UTIL_H_
#define _UTIL_H_

#include "cport.h"

/******************************************************************************
 *
 * :: Exits
 *
 *****************************************************************************/

extern void	exitSuccess	(void) noreturn;
extern void	exitFailure	(void) noreturn;
	/*
	 * Exit from program with an appropriate return code.
	 */

typedef void	(*ExitFun)	(int status);
extern ExitFun	exitSetHandler	(ExitFun);
	/*
	 * exitSetHandler is evaluated by exitSuccess and exitFailure.
	 * A value of 0 restores the default.
	 */

/******************************************************************************
 *
 * :: Bugs
 *
 *****************************************************************************/
extern void	bugWarning	(String fmt, ...) chk_fmt (1, 2);

extern void	bug		(String fmt, ...) chk_fmt (1, 2) noreturn;

#define		bugUnimpl(m)	_bug("Unimplemented %s (line %d in file %s).",m)
#define		bugBadCase(no)	_bug("Bad case %d (line %d in file %s).", no)
#define		_bug(fmt,a)	bug(fmt, a, __LINE__, __FILE__)

/******************************************************************************
 *
 * :: Numerics
 *
 *****************************************************************************/

# define QUO_ROUND_UP(n,d)	((n) % (d) ? (n)/(d) + 1 : (n)/(d))
	/*
	 * QUO_ROUND_UP(n,d) is the smallest integer q such that q*d <= n.
	 * Capitalized to remind that the arguments are multiply evaluated.
	 */

# define ROUND_UP(n,d)		((n) % (d) ? (n) + (d) - (n) % (d) : (n))
	/*
	 * ROUND_UP(n,d) is the smallest multiple of d >= n.
	 * Capitalized to remind that the arguments are multiply evaluated.
	 */

# undef MAX
# undef MIN

# define MAX(a,b)		((a) > (b) ? (a) : (b))
# define MIN(a,b)		((a) < (b) ? (a) : (b))
	/*
	 * maximum and minimum of arguments.
	 * Capitalized to remind that the arguments are multiply evaluated.
	 */

extern ULong	binPrime	(ULong);
	/*
	 * p = binPrime(i) is the largest prime p such that p <= 2**i
	 */

extern int	cielLg		(ULong);
	/*
	 * i = cielLg(n) returns the smallest integer i such that n <= 2**i.
	 */

extern ULong ulongSmallIntFrString(String start, unsigned int ndigs, unsigned int radix);
/******************************************************************************
 *
 * :: Strings
 *
 *****************************************************************************/

extern String	bite		(String buf, String str, int sep);
	/*
	 * Using 'sep' as the separator character, the first word of 'str'
	 * into 'buf' and return the remainder of 'str'.
	 */

/******************************************************************************
 *
 * :: Sorting
 *
 *****************************************************************************/

extern void	lisort		(Pointer base, Length n, Length sz,
				 int (*cmpfn)(ConstPointer, ConstPointer));
	/*
	 * A stable sort.
	 */

/******************************************************************************
 *
 * :: Hash functions
 *
 *****************************************************************************/

#define	hashCombine(h1, h2) \
	((((h1) ^ ((h1) << 8)) + ((h2) + 200041)) & 0x3FFFFFFF)

extern int hashCombinePair(int h1, int h2);

/******************************************************************************
 *
 * :: Memory operations
 *
 *****************************************************************************/

extern void	memswap 	(Pointer p, Pointer q, Length);
extern Pointer	memlset 	(Pointer p, int c,     ULong l);
extern Bool	memltest	(Pointer p, int c,     ULong l);
	/*
	 *	memlswap swaps the data in the regions pointed to by p and q.
	 *	memlset  sets the first l characters pointed to by p to be c.
	 *	memltest tests whether the first l characters pointed to by p
	 *		 are all equal to c.
	 */

/******************************************************************************
 *
 * :: Bit-fiddling
 *
 *****************************************************************************/

extern void     bfShiftUp(int nb,UByte *t,int nsh,UByte *s,int bF);
extern void     bfShiftDn(int nb,UByte *t,int nsh,UByte *s,int b0,int b1);
extern int      bfFirst1 (int nb,UByte *t);
	/*
	 * bfShiftUp
	 *	 shifts bits up (toward lower address) by 'nsh' bit positions. 
	 *      's' is a pointer to 'nb' bytes treated as a bit string.
	 *	't' is a pointer to 'nb' bytes to hold the result.
	 *	Bits shifted out are lost.  'bF' is the bit to shift in.
	 *
	 * bfShiftDn
	 *	shifts bits down (toward higher address) by nsh bit positions. 
	 *      's' is a pointer to 'nb' bytes treated as a bit string.
	 *	't' is a pointer to 'nb' bytest to hold the result.
	 *	Bits shifted out are lost. 
	 *	'b1' is the first bit to shift in the top.
	 *	'b0' is shifted in for the subsequent bits.
	 *
	 * bfFirst1
	 *	finds the bit index of the lowest address 1-bit in 't'.
	 *	The most significant bit of t[0] is bit 0 and the least
	 *	significant bit of t[nb-1] is bit nb * CHAR_BIT - 1.
	 */ 


/*****************************************************************************
 *
 * :: Input-Output
 *
 ****************************************************************************/

extern void	prompt		(FILE *fin, FILE *fout, String fmt, ...) chk_fmt (3, 4);
	/*
	 * If fin is interactive print the prompt on fout using
	 * printf-style formatting.
	 */

extern int	fputcTimes	(int c, int n, FILE *f);
	/*
	 * Output character c n times, returning n.
	 */

extern int	fputsUntab	(char *s, int tabstop, FILE *f);
	/*
	 * Output the string, expanding tab characters to spaces.
	 * The the return value is the number of characters output.
	 */

#define	MAX_FLOAT_SIZE	512
 	/*
	 * The maximum size of a string representation of a double.
         */
extern	String	DFloatSprint(String, DFloat);
 	/*
	 * Print a double float value into a string.
	 */
extern	DFloat	DFloatScan(String);
 	/* 
	 * Scan a double float value from a buffer and return the value.
	 */

#define ubPrintBits(a)	ubPrintBits0(sizeof(a), (UByte *)&(a))

extern 	void	ubPrintBits0	(int, UByte *);


#endif /* !_UTIL_H_ */
