/*****************************************************************************
 *
 * buffer.h: Grow-on-demand buffers.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "cport.h"

#define BUF_INIT_SIZE	64

typedef struct buffer *Buffer;

extern Buffer	bufNew		(void);
extern void	bufFree		(Buffer);

extern Buffer	bufCapture	(String, Length);
	/* Form buffer from given string. */
extern String	bufLiberate	(Buffer);
	/* Free buffer but keep chars. */

extern UByte *	bufData		(Buffer s);
extern String	bufChars	(Buffer s);
extern Length	bufSize		(Buffer s);
extern Length	bufPosition	(Buffer s);
extern void	bufSetPosition	(Buffer s, Length n);
extern void	bufSkip		(Buffer s, Length n);

extern void	bufNeed		(Buffer, Length n);
	/* Indicate the available size must be at least n. */

extern void	bufGrow		(Buffer, Length inc);
	/* Change the available size by inc. */

extern int	bufAdd1		(Buffer b, int c);
	/* Add the character 'c' at the end of the buffer. */

extern void	bufAddn		(Buffer, const char *s, Length n);
	/* Add the first n characters of s to the end of the buffer. */

extern String	bufGetn		(Buffer, Length n);
	/* Advance past n characters and return pointer to them. */

extern String	bufGets		(Buffer);
	/* Advance past a null-terminated string and return it. */

extern int	bufPutc		(Buffer, int c);
	/* Add the character to the buffer, pushing ahead a NULL terminator. */

extern int	bufPutcTimes(Buffer b, int c, int n);
	/* Add n character to the buffer, pushing ahead a NULL terminator. */

extern int	bufPuts		(Buffer, const char *s);
	/* Add the characters of s to the buffer, pushing ahead a NULL terminator. */

extern int	bufPuti		(Buffer, int i);
	/* Add a text representation of i to the buffer. */

extern int	bufPrintf	(Buffer, const char *fmt, ...);
extern int      bufVPrintf	(Buffer, const char *fmt, va_list);
	/* Use printf-style formatting onto the end of a buffer.  */

extern int	bufPrint	(FILE *, Buffer);
	/* Print buffer in readable form. */

/*
 * Functions for growing a buffer one character at a time.
 */

extern void	bufStart	(Buffer b);
extern UByte	bufGet1		(Buffer b);
extern void	bufBack1	(Buffer b);
extern UByte	bufNext1	(Buffer b);

	/*
	 * To put the null-terminated string "hi" into a buffer do:
	 *   bufStart(b);
	 *   bufAdd1(b,'h'); bufAdd1(b,'i'); bufAdd1(0);
	 * To concatenate "lo":
	 *   bufBack1(b);
	 *   bufAdd1(b,'l'); bufAdd1(b,'o'); bufAdd1(b,0);
	 */

/*
 * Functions for putting and getting characters to a buffer.
 */

extern void	bufPutChars	(Buffer buf, char const *s, Length cc);
extern void	bufGetChars	(Buffer buf, char *s, Length cc);

/*
 * Functions for putting and getting integers as byte sequences.
 */

extern void	bufPutByte	(Buffer b, UByte c);
extern void	bufPutHInt	(Buffer b, UShort c);
extern void	bufPutSInt	(Buffer b, ULong c);
extern Bool     bufIsSInt	(long i);

extern UByte	bufGetByte	(Buffer b);
extern UShort	bufGetHInt	(Buffer b);
extern ULong	bufGetSInt	(Buffer b);

/* Save integers in standard byte order. */
extern UByte	bufRdUByte	(Buffer buf);
extern UShort	bufRdUShort	(Buffer buf);
extern ULong	bufRdULong 	(Buffer buf);

extern int	bufWrUByte	(Buffer buf, UByte b);
extern int	bufWrUShort	(Buffer buf, UShort s);
extern int	bufWrULong	(Buffer buf, ULong l);

/* Save floating-point numbers in IEEE format. */
extern SFloat	bufRdSFloat	(Buffer buf);
extern DFloat	bufRdDFloat	(Buffer buf);

extern int	bufWrSFloat	(Buffer buf, SFloat s);
extern int	bufWrDFloat	(Buffer buf, DFloat d);

/* Save a given number of characters in ASCII format. */
extern String	bufRdChars	(Buffer buf, int cc);
extern int	bufWrChars	(Buffer buf, int cc, String s);

/* Save strings in ASCII format, length included. */
extern String	bufRdString	(Buffer buf);
extern int	bufWrString	(Buffer buf, String s);

/* Read an unknown number of ASCII characters. */
extern String	bufGetString	(Buffer);

/* Save data in binary format. */
extern Buffer	bufRdBuffer	(Buffer buf);
extern int	bufWrBuffer	(Buffer buf, Buffer b);

#endif /* !_BUFFER_H_ */
