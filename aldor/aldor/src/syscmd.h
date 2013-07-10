/*****************************************************************************
 *
 * syscmd.h: System command processing.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _SYSCMD_H_
#define _SYSCMD_H_

# include "axlobs.h"

/*
 * Top-level dispatchers.
 */
extern  TokenList	scmdProcessList	(TokenList);
extern  TokenList	scmdProcessToken(Token);
extern  void		scmdCheck       (SrcPos, String cmd);
extern  TokenList	scmdProcess	(SrcPos, String cmd);

/*
 * Each of these returns the remainder of the line on success and 0 on failure.
 */
extern  String	scmdIsDirective		(String ln, String kword);
extern  String	scmdIsAbbrev		(String ln, String kwabbrev);

extern  String	scmdScanInteger		(String ln, int    *pno);
extern  String  scmdScanId		(String ln, String *pid);
extern  String	scmdScanFName		(String ln, String *pfn);
extern  String  scmdScanLibraryOption	(String ln, String *pid, String *pkey);

/*
 * Handlers.
 */
extern	int	scmdHandleLibraryDir	(String dname);
extern	int	scmdHandleIncludeDir	(String dname);
extern	int	scmdHandleLibrary	(String id, String key);
extern  int	scmdHandleMacro		(SrcPos pos, String s);

#endif	/* !_SYSCMD_H_ */
