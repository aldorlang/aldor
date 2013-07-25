/*****************************************************************************
 *
 * srcpos.c: Source position operations.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlgen.h"
#include "comsg.h"
#include "debug.h"
#include "file.h"
#include "opsys.h"
#include "store.h"
#include "util.h"
#include "srcpos.h"
#include "strops.h"

/*
 * The "fields" are
 *	int  mac:  1;  -- 0 means text is not macro expanded, 1 means it is
 *	int  cno: 14;  -- 0 based character position in line
 *	int  lno: nn;  -- 0 based global line number (nn = 16 for 32 bit longs)
 *
 * We use long ints rather than structs with bitfields so we can pass
 * SrcPos values as parameters without complaint in any C compiler.
 *
 * We also keep one bit free for spstack operations.
 */

# define SPOS_STK_NBITS  (1)
# define SPOS_MAC_NBITS	 (1)
# define SPOS_CNO_NBITS	(14)
# define SPOS_LNO_NBITS	\
	(bitsizeof(ULong) - SPOS_CNO_NBITS - SPOS_MAC_NBITS - SPOS_STK_NBITS)

# define SPOS_STK_MASK  ((1L << SPOS_STK_NBITS) - 1)

# define SPOS_MAC_SHIFT	(0)
# define SPOS_MAC_MASK	(((1L << SPOS_MAC_NBITS) - 1) << SPOS_MAC_SHIFT)

# define SPOS_CNO_SHIFT	(SPOS_MAC_SHIFT + SPOS_MAC_NBITS)
# define SPOS_CNO_MASK	(((1L << SPOS_CNO_NBITS) - 1) << SPOS_CNO_SHIFT)

# define SPOS_LNO_SHIFT	(SPOS_CNO_SHIFT + SPOS_CNO_NBITS)
# define SPOS_LNO_MASK	(((1L << SPOS_LNO_NBITS) - 1) << SPOS_LNO_SHIFT)

# define sposSet(l, c) (((l) << SPOS_LNO_SHIFT) | ((c) << SPOS_CNO_SHIFT))

SrcPos sposNone = sposSet(int0, int0);

#define		TOP_LINE_NO	long0
#define		END_LINE_NO	((1L << SPOS_LNO_NBITS) - 1)

SrcPos
sposTop(void)
{
	return sposSet(TOP_LINE_NO, int0);
}

SrcPos
sposEnd(void)
{
	return sposSet(END_LINE_NO, int0);
}

typedef struct {
	Length		glno;
	FileName	fn;
	Length		flno;
} GLine;

static int	gloPos;
static int	gloArgc;
static GLine	*gloLineTbl;

static FileName lastfname;
static Length	lastlno, lastftell;

SrcPos
sposOffset(SrcPos p, int c)
{
    return (((p >> SPOS_CNO_SHIFT)+c) << SPOS_CNO_SHIFT) | (p & SPOS_MAC_MASK);
}

Bool
sposEqual(SrcPos p, SrcPos q)
{
    return ((p >> SPOS_CNO_SHIFT) == (q >> SPOS_CNO_SHIFT));
}

SrcPos
sposMin(SrcPos p, SrcPos q)
{
    SrcPos P, Q;

    P = p >> SPOS_CNO_SHIFT;
    Q = q >> SPOS_CNO_SHIFT;

    return (P < Q) ? p : q;
}

SrcPos
sposMax(SrcPos p, SrcPos q)
{
    SrcPos P, Q;

    P = p >> SPOS_CNO_SHIFT;
    Q = q >> SPOS_CNO_SHIFT;

    return (P > Q) ? p : q;
}

Bool
sposIsMacroExpanded(SrcPos p)
{
    return (p & SPOS_MAC_MASK) >> SPOS_MAC_SHIFT;
}

SrcPos
sposMacroExpanded(SrcPos p)
{
    return p | (1 << SPOS_MAC_SHIFT);
}

int
sposPrint(FILE *fout, SrcPos sp)
{
	int	cc;
	if (sposIsSpecial(sp)) {
		switch (sposGlobalLine(sp)) {
		case TOP_LINE_NO:
			cc = fprintf(fout, "-- TOP --");
			break;
		case END_LINE_NO:
			cc = fprintf(fout, "-- END --");
			break;
		default:
			cc = fprintf(fout, "-- unknown src pos --");
			break;
		}
	}
	else
		cc = fprintf(fout, "\"%s\", line %d char %d",
			  fnameUnparseStatic(sposFile(sp)),
#if EDIT_1_0_n1_07
			  (int) sposLine(sp), (int) sposChar(sp));
#else
			  sposLine(sp), sposChar(sp));
#endif
	return cc;
}

void
sposShow(void)
{
	int	i;

	assert(gloLineTbl);

	fprintf(stderr, "Begin Show Global Line Table\n");
	for (i = 0; i < gloArgc; i++)
		fprintf(stderr, "\t%s [%d], %d\n",
		       fnameUnparseStatic(gloLineTbl[i].fn),
#if EDIT_1_0_n1_07
		       (int) gloLineTbl[i].glno, (int) gloLineTbl[i].flno);
#else
		       gloLineTbl[i].glno, gloLineTbl[i].flno);
#endif
	fprintf(stderr, "End Show Global Line Table\n");
	return;
}

/*
 * Here we rely on the fact that the line number is in the more
 * significant part of the integer.
 */
int
sposCmp(SrcPos sp1, SrcPos sp2)
{
	int cmp = 0;

	sp1 = sp1 >> SPOS_CNO_SHIFT;
	sp2 = sp2 >> SPOS_CNO_SHIFT;

	if (sp1 < sp2)
		cmp = -1;
	else if (sp1 > sp2)
		cmp =  1;
	return cmp;
}

int
sposIsSpecial(SrcPos sp)
{
	ULong	gno = sposGlobalLine(sp);

	return gno == TOP_LINE_NO || gno == END_LINE_NO;
}

void
sposInit(void)
{
	/* set up global line number table */

	lastfname  = 0;
	gloLineTbl = (GLine *) stoAlloc(int0, sizeof(GLine));
	gloArgc	   = 1;
	gloPos	   = 0;
	gloLineTbl[0].fn   = 0;
	gloLineTbl[0].glno = 0;
	gloLineTbl[0].flno = 0;

	return;
}

void
sposFini(void)
{
	/* free global line number table */

	assert(gloLineTbl);

	stoFree((Pointer) (gloLineTbl));

	if (lastfname) {
		fnameFree(lastfname);
		lastfname = 0;
	}
	return;
}

void
sposTableToBuffer(Buffer buf)
{
	int	i;

	assert(gloLineTbl);

	if ((gloArgc == 1) && (!gloLineTbl[0].fn)) return;

	/* write global line number table to a buffer */
	bufWrULong(buf, gloArgc);
	for (i = 0; i < gloArgc; i++) {
		String	s;
		int	len;

		bufWrULong(buf, gloLineTbl[i].glno);
		bufWrULong(buf, gloLineTbl[i].flno);
		s   = fnameDir(gloLineTbl[i].fn);
		len = strlen(s);
		BUF_PUT_SINT(buf, len);
		bufWrChars(buf, len, s);
		s   = fnameName(gloLineTbl[i].fn);
		len = strlen(s);
		BUF_PUT_SINT(buf, len);
		bufWrChars(buf, len, s);
		s   = fnameType(gloLineTbl[i].fn);
		len = strlen(s);
		BUF_PUT_SINT(buf, len);
		bufWrChars(buf, len, s);		
	}
}

void
sposTableFrBuffer(Buffer buf)
{
	int	i;
	/* set up global line number table */

	sposInit();
	gloArgc = bufRdULong(buf);
	gloPos  = gloArgc;

	gloLineTbl = (GLine *) stoResize(gloLineTbl,
					 sizeof(GLine)*(gloArgc));
	for (i = 0; i < gloArgc; i++) {
		String	dir, name, type;
		int	len;

		gloLineTbl[i].glno = bufRdULong(buf);
		gloLineTbl[i].flno = bufRdULong(buf);
		BUF_GET_SINT(buf, len);
		dir  = bufRdChars(buf, len);
		BUF_GET_SINT(buf, len);
		name = bufRdChars(buf, len);
		BUF_GET_SINT(buf, len);
		type = bufRdChars(buf, len);
		gloLineTbl[i].fn   = fnameNew(dir, name, type);
	}
}

SrcPos
sposNew(FileName fname, Length flno, Length glno, Length cno)
{
	FileName prevName;
	int	 prevGlno;

	assert(gloLineTbl);

	if (gloPos < 0 || !fname) return sposNone;

	if (gloPos) {
		prevName = gloLineTbl[gloPos - 1].fn;
		prevGlno = gloLineTbl[gloPos - 1].glno;
	}
	else {
		prevName = gloLineTbl[gloPos].fn;
		prevGlno = gloLineTbl[gloPos].glno;
	}

	if (glno <= prevGlno || prevName == 0 || !fnameEqual(fname, prevName))
	  
	  sposGrowGloLineTbl(fname, flno, glno);
	

	return sposSet(glno, cno);
}

void
sposGrowGloLineTbl(FileName fname, Length flno, Length glno)
{
  if (gloPos > 0) {
    gloLineTbl = (GLine *)
      stoResize(gloLineTbl,
		sizeof(GLine)*(gloArgc+1));
    gloArgc++;
  }
  gloLineTbl[gloPos].glno = glno;
  gloLineTbl[gloPos].flno = flno;
  gloLineTbl[gloPos].fn	= fnameCopy(fname);
  gloPos++; 
}

SrcPos
sposGet(Length glno, Length cno)
{
	return sposSet(glno, cno);
}

FileName
sposFile(SrcPos spos)
{
	Length	gLineNo;
	int	i;

	assert(gloLineTbl);

	if (sposIsSpecial(spos)) return gloLineTbl[0].fn;

	gLineNo = sposGlobalLine(spos);

	if (gloPos && gLineNo) {
		for (i = 0; i < gloArgc-1; i++) {
			if (gLineNo >= gloLineTbl[i].glno &&
			    gLineNo < gloLineTbl[i+1].glno)
				return gloLineTbl[i].fn;
		}
		return gloLineTbl[gloArgc-1].fn;
	}
	else {
		return gloLineTbl[0].fn;
	}
}

Length
sposGlobalLine(SrcPos spos)
{
	return (spos & SPOS_LNO_MASK) >> SPOS_LNO_SHIFT;
}

Length
sposGLine(FileName fname, Length fLineNo)
{
	Length	i, glopos = 1;

	/* retrieve global line number from a file name and file line */

	assert(gloLineTbl);
	for (i = 0; i < gloArgc; i++)
		if (fnameEqual(gloLineTbl[i].fn, fname))
			if (fLineNo >= gloLineTbl[i].flno)
				glopos = gloLineTbl[i].glno + (fLineNo - gloLineTbl[i].flno);
	return glopos;
}

Length
sposLine(SrcPos spos)
{
	Length	gLineNo;
	int	i;

	assert(gloLineTbl);

	if (sposIsSpecial(spos)) return 0;

	gLineNo = sposGlobalLine(spos);

	if (gloPos && gLineNo) {
		for (i = 0; i < gloArgc-1; i++) {
			if (gLineNo >= gloLineTbl[i].glno &&
			    gLineNo < gloLineTbl[i+1].glno)
				return (gLineNo - gloLineTbl[i].glno) +
				       (gloLineTbl[i].flno);
		}
		return (gLineNo - gloLineTbl[gloArgc-1].glno) +
		       (gloLineTbl[gloArgc-1].flno);
	}
	else
		return (gloLineTbl[0].glno + gloLineTbl[0].flno);
}

Length
sposChar(SrcPos spos)
{
	return (spos & SPOS_CNO_MASK) >> SPOS_CNO_SHIFT;
}

int
sposLineText(Buffer buf, SrcPos spos)
{
	FileName	fname;
	FILE		*f;
	int		c = 0, i, lno, rc;

	if (sposIsSpecial(spos))
		return -1;

	fname = sposFile(spos);
	lno   = sposLine(spos);

	/* Add null in case of errors. */
	BUF_ADD1(buf, char0);
	BUF_BACK1(buf);

	if (fnameIsStdin(fname))
		return -1;

	f = fileRdOpen(fname);

	/* Are we continuing from where we last were? */
	if (lastfname && fnameEqual(lastfname, fname) && lastlno <= lno)
		fseek(f, lastftell, SEEK_SET);
	else
		lastlno = 1;

	for (i = lastlno; i < lno; ) {
		c = getc(f);
		if (c == '\n') i++;
		if (c == EOF) break;
	}

	if (c == EOF || i != lno)
		rc = -1;
	else {
		do {
			c = getc(f);
			if (c == EOF) break;
			BUF_ADD1(buf, c);
		} while (c != '\n');
		BUF_ADD1(buf, char0);
		rc = bufSize(buf);
	}

	/* Remember where we were. lastfname is cleared by srcposFini. */
	if (!lastfname || !fnameEqual(lastfname, fname)) {
		if (lastfname) fnameFree(lastfname);
		lastfname = fnameCopy(fname);
	}
	/* Make lastftell be the position of the beginning of lastlno. */
	lastlno	  = lno + 1;
	lastftell = ftell(f);

	fclose(f);
	return rc;
}

/*
 * SrcPosStack functions
 */
SrcPosStack	spstackEmpty;
SrcPosStack	spstackStatic;

# define spstackImmed(spos)	\
	(spstackStatic.spos = (((spos) << SPOS_STK_NBITS) | SPOS_STK_MASK), \
	 spstackStatic)

# define spstackIsEmpty(stk)	((stk).stack == NULL)
# define spstackIsImmed(stk)	((stk).spos & SPOS_STK_MASK)

SrcPosStack
spstackPush(SrcPos spos, SrcPosStack rest)
{
	SrcPosStack	sposStk;
	SrcPosCell	sposCell;

	sposCell = (SrcPosCell) stoAlloc(OB_Other, sizeof(*sposCell));

	sposCell->spos	= spos;
	sposCell->rest	= rest;

	sposStk.stack = sposCell;
	return sposStk;
}

SrcPosStack
spstackCopy(SrcPosStack sposStk)
{
	if (spstackIsEmpty(sposStk) || spstackIsImmed(sposStk))
		return sposStk;

	return spstackPush(spstackFirst(sposStk),
			   spstackCopy(spstackRest(sposStk)));
}

void
spstackFree(SrcPosStack sposStk)
{
	if (spstackIsEmpty(sposStk) || spstackIsImmed(sposStk))
		return;

	spstackFree(spstackRest(sposStk));
	stoFree((Pointer) sposStk.stack);
}

SrcPos
spstackFirst(SrcPosStack sposStk)
{
	if (spstackIsEmpty(sposStk))
		return sposNone;
	else if (spstackIsImmed(sposStk))
		return sposStk.spos >> SPOS_STK_NBITS;

	return sposStk.stack->spos;
}

SrcPosStack
spstackRest(SrcPosStack sposStk)
{
	if (spstackIsEmpty(sposStk) || spstackIsImmed(sposStk))
		return spstackEmpty;

	return sposStk.stack->rest;
}

SrcPosStack
spstackSetFirst(SrcPosStack sposStk, SrcPos spos)
{
	if (spstackIsEmpty(sposStk) || spstackIsImmed(sposStk))
		return spstackImmed(spos);

	sposStk.stack->spos = spos;
	return sposStk;
}

SrcPosStack
spstackSetSecond(SrcPosStack sposStk, SrcPos spos)
{
	if (spstackIsEmpty(sposStk) || spstackIsImmed(sposStk))
		return spstackPush(spstackFirst(sposStk), spstackImmed(spos));

	sposStk.stack->rest = spstackSetFirst(spstackRest(sposStk), spos);
	return sposStk;
}


/*
 * Print a "...^..^" indicator line (hacked from comsgPrintDots)
 */
local void
spstackPrintDots(SrcPos spos)
{
	if (!sposIsSpecial(spos)) {
		int	Dcno = sposChar(spos) - 1;

		if (Dcno >= 0) {
			fputcTimes('.', Dcno, dbOut);
			(void)fprintf(dbOut, "^");
		}

		(void)fprintf(dbOut, "\n");
	}
}

/*
 * Print a source code line (hacked from comsgPrintLine)
 */
int
spstackPrintLine(FILE *fout, SrcPos spos)
{
	Buffer	buf;
	String	s;
	int	rc, cc;
	Length	n, nu;
	Bool	splitLine = false;
	String	fFmt  = "\"%s\", line %d: ";
	int	LINE_LENGTH = 80;

	/*
	 * Get the source line.
	 */
	buf = bufNew();
	rc = sposLineText(buf, spos);
	if (rc == -1) {
		bufFree(buf);
		return -1; 
	}

	s  = bufLiberate(buf);
	n  = strLength(s);
	nu = strUntabLength(s, TABSTOP);

	/*
	 * Print the position and source line,	splitting them if necessary.
	 */
	if (sposIsSpecial(spos))
		cc = sposPrint(fout, spos);
	else {
		FileName	fn = sposFile(spos);

		cc = fprintf(fout, fFmt,
			     fnameUnparseStatic(fn),
			     sposLine(spos));

		if (fnameHasDir(fn) && !strEqual(fnameDir(fn), osCurDirName()))
			splitLine = true;
	}

	if (cc + nu >= LINE_LENGTH) splitLine = true;
	if (splitLine) fputc('\n', fout);

	fputsUntab(s, TABSTOP, fout);
	if (n == 0 || s[n-1] != '\n') fputc('\n', fout);

	return splitLine ? 0 : cc;
}


void
spstackPrintDb(SrcPosStack sposStk)
{
	while (!spstackIsEmpty(sposStk))
	{
		/* Get the line at the top of the stack */
		SrcPos pos = spstackFirst(sposStk);


		/* Display the line neatly */
		int indent = spstackPrintLine(dbOut, pos);


		/* Highlight the position on the line */
		fputcTimes(' ', indent, dbOut);
		spstackPrintDots(pos);


   		/* Move down the stack */
   		sposStk = spstackRest(sposStk);
	}
}

