/*****************************************************************************
 *
 * srcline.h: Structure for source lines.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/
 
#ifndef _SRCLINE_H_
#define _SRCLINE_H_

#include "axlobs.h"

struct srcLine {
	SrcPos		spos;
	unsigned short  indentation;
	unsigned int    isSysCmd:       1;
	unsigned int    sysCmdHandled:  1;
	unsigned int    isEndifLine:    1;
	String          text;
};

extern  SrcLine  slineNew      	(SrcPos,int indent,String text);
extern  SrcLine  slineNewSysCmd (SrcPos,int indent,String text,Bool isHandled);
extern  void     slineFree      (SrcLine);
extern  int      slinePrint     (FILE *, SrcLine);
extern  int      sllPrint       (FILE *, SrcLineList);

#endif	/* !_SRCLINE_H_ */
