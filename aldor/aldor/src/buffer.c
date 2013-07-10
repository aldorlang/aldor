/*****************************************************************************
 *
 * buffer.c: Grow-on-demand buffers.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

# include "axlgen.h"

Buffer
bufNew(void)
{
	Buffer	b;

	b	= (Buffer) stoAlloc(OB_Buffer, sizeof(*b));
	b->argv = (UByte *) stoAlloc(OB_Other,	BUF_INIT_SIZE);
	b->argc = stoSize(b->argv);
	b->pos	= 0;

	return b;
}

void
bufFree(Buffer b)
{
	stoFree((Pointer) (b->argv));
	stoFree((Pointer) (b));
}

Buffer
bufCapture(String s, Length l)
{
	Buffer	b;
	b	= (Buffer) stoAlloc(OB_Buffer, sizeof(*b));
	b->argv = (UByte *) s;
	b->argc = l;
	b->pos	= 0;

	return b;
}

String
bufLiberate(Buffer b)
{
	String s = (String) b->argv;
	if ((b->pos  == 0) && b->argc)
	    s[0] = '\0';
	stoFree((Pointer) (b));
	return s;
}

Buffer
bufNeed(Buffer b, Length n)
{
	if (b->argc < n) {
		b->argv = (unsigned char *) stoResize(b->argv, n);
		b->argc = stoSize(b->argv);
	}
	return b;
}

Buffer
bufGrow(Buffer b, Length inc)
{
	b->argv = (UByte *) stoResize(b->argv, b->argc + inc);
	b->argc = stoSize(b->argv);
	return b;
}

int
bufAdd1Char(Buffer b, int c)
{
	if (b->pos == b->argc) bufGrow(b, b->argc/2);
	return b->argv[b->pos++] = c;
}

Buffer
bufAdd1(Buffer b, int c)
{
	bufAdd1Char(b, c);
	return b;
}

Buffer
bufAddn(Buffer b, const char *s, Length n)
{
	bufNeed(b, b->pos + n);
	memmove(b->argv + b->pos, (char *) s, n);
	bufSkip(b, n);
	return b;
}

String
bufGetn(Buffer b, Length n)
{
	UByte	*s = b->argv + b->pos;
	bufSkip(b, n);
	return (String) s;
}

String
bufGets(Buffer b)
{
	UByte	*s = b->argv + b->pos;
	int	cc = strlen((String) s);
	bufSkip(b, cc + 1);
	return (String) s;
}

int
bufPuts(Buffer b, String s)
{
	int	cc = strlen(s);
	bufAddn(b, s, cc);
	BUF_ADD1(b,char0);
	BUF_BACK1(b);
	return cc;
}

int
bufPutc(Buffer b, int c)
{
	BUF_ADD1(b, (char) c);
	BUF_ADD1(b, char0);
	BUF_BACK1(b);
	return c;
}

int
bufPutcTimes(Buffer b, int c, int times)
{
	int i;
	for (i = 0; i < times; i++)
		bufPutc(b, c);
	return c;
}

int
bufPuti(Buffer buf, int i)
{
	Length	pos0;
	int	ndig, digits[4 * bitsizeof(int)];	/* 4 > lg 10 */

	pos0 = bufPosition(buf);

	if (i == 0) {
		BUF_ADD1(buf, '0');
	}
	else {
		if (i < 0) {
			BUF_ADD1(buf, '-');
			i = - i;
		}
		for (ndig = 0; i; i /= 10, ndig++)
			digits[ndig] = i % 10;
		while (ndig--)
			BUF_ADD1(buf, '0' + (char) digits[ndig]);
	}
	BUF_ADD1(buf, char0);
	BUF_BACK1(buf);
	return bufPosition(buf) - pos0;
}

static Buffer	pbuf;

static int
bufXPut(const char *s, int n)
{
	if (n == -1) n = strlen(s);
	bufAddn(pbuf, s, n);
	return n;
}

int
bufVPrintf(Buffer b, const char *fmt, va_list argp)
{
	int	cc;
	pbuf   = b;
	if (b->pos > 0 && b->argv[b->pos-1] == 0) BUF_BACK1(b);
	cc = vxprintf(bufXPut, fmt, argp);
	BUF_ADD1(b, char0);
	BUF_BACK1(b);
	return cc;
}

int
bufPrintf(Buffer b, const char *fmt, ...)
{
	va_list argp;
	int	cc;

	va_start(argp, fmt);
	cc = bufVPrintf(b, fmt, argp);
	va_end(argp);
	return cc;
}

int
bufPrint(FILE *fout, Buffer b)
{
	int	cc;

#if EDIT_1_0_n1_07
	cc  = fprintf(fout, "[%d/%d]", (int) b->pos, (int) b->argc);
#else
	cc  = fprintf(fout, "[%d/%d]", b->pos, b->argc);
#endif
	cc += strPrint(fout, (String) b->argv, '"', '"', '\\', "\\%#x");

	return cc;
}

/*****************************************************************************
 *
 * Save integers in standard byte order.
 *
 ****************************************************************************/

UByte
bufRdUByte(Buffer buf)
{
	UByte result;

	BUF_GET_BYTE(buf, result);

	return result;
}

UShort
bufRdUShort(Buffer buf)
{
	UShort result;

	BUF_GET_HINT(buf, result);

	return result;
}

ULong
bufRdULong(Buffer buf)
{
	ULong result;

	BUF_GET_SINT(buf, result);

	return result;
}

int
bufWrUByte(Buffer buf, UByte b)
{
	BUF_PUT_BYTE(buf, b);

	return BYTE_BYTES;
}

int
bufWrUShort(Buffer buf, UShort s)
{
	BUF_PUT_HINT(buf, s);

	return HINT_BYTES;
}

int
bufWrULong(Buffer buf, ULong l)
{
	BUF_PUT_SINT(buf, l);

	return SINT_BYTES;
}

/*****************************************************************************
 *
 * Save floating-point numbers in XFloat format.
 *
 ****************************************************************************/

SFloat
bufRdSFloat(Buffer buf)
{
	XSFloat *	pxs;
	SFloat		s;

	pxs = (XSFloat *) bufGetn(buf, XSFLOAT_BYTES);
	xsfToNative(pxs, &s);

	return s;
}

DFloat
bufRdDFloat(Buffer buf)
{
	XDFloat *	pxd;
	DFloat		d;

	pxd = (XDFloat *) bufGetn(buf, XDFLOAT_BYTES);
	xdfToNative(pxd, &d);

	return d;
}

int
bufWrSFloat(Buffer buf, SFloat s)
{
	XSFloat	xs;
	SFloat	bs = s;	/* Avoid problems when float is passed as double. */

	xsfFrNative(&xs, &bs);
	bufAddn(buf, (char *) &xs, XSFLOAT_BYTES);

	return XSFLOAT_BYTES;
}

int
bufWrDFloat(Buffer buf, DFloat d)
{
	XDFloat	xd;

	xdfFrNative(&xd, &d);
	bufAddn(buf, (char *) &xd, XDFLOAT_BYTES);

	return XDFLOAT_BYTES;
}

/*****************************************************************************
 *
 * Save a given number of characters in ASCII format.
 *
 ****************************************************************************/

/* Read a specified number of characters from the buffer.
 * Return the result as a null-terminated string.
 */
String
bufRdChars(Buffer buf, int cc)
{
	String	s;

	s = strAlloc(cc);
	BUF_GET_CHARS(buf, s, cc);
	s = strnFrAscii(s,cc);

	return s;
}

int
bufWrChars(Buffer buf, int cc, String s)
{
	BUF_PUT_CHARS(buf, strnToAsciiStatic(s,cc), cc);

	return cc;
}


/*****************************************************************************
 *
 * Save strings in ASCII format, length included.
 *
 ****************************************************************************/

String
bufRdString(Buffer buf)
{
	String	s;
	int	cc;

	BUF_GET_SINT(buf, cc);
	s = strAlloc(cc);
	BUF_GET_CHARS(buf, s, cc);
	s = strnFrAscii(s,cc);

	return s;
}

int
bufWrString(Buffer buf, String s)
{
	int cc = strLength(s) + 1;

	BUF_PUT_SINT(buf, cc);
	BUF_PUT_CHARS(buf, strnToAsciiStatic(s,cc), cc);

	return SINT_BYTES + cc;
}

String
bufGetString(Buffer buf)
{
	String	s = strCopy(bufGets(buf));
	return strnFrAscii(s, strLength(s) + 1);
}


/*****************************************************************************
 *
 * Save data in binary format.
 *
 ****************************************************************************/

Buffer
bufRdBuffer(Buffer buf)
{
	String	s;
	int	cc;

	BUF_GET_SINT(buf, cc);
	s = strAlloc(cc);
	BUF_GET_CHARS(buf, s, cc);

	return bufCapture(s, cc);
}

int
bufWrBuffer(Buffer buf, Buffer b)
{
	int cc = bufPosition(b);

	BUF_PUT_SINT(buf, cc);
	BUF_PUT_CHARS(buf, bufChars(b), cc);

	return SINT_BYTES + cc;
}
