/*****************************************************************************
 *
 * version.h: Compiler version.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _VERSION_H_
#define _VERSION_H_

/*
 * Version of the program. An identification from the version control system
 * is used for the patch level.
 */
extern CString	verName;
extern int	verMajorVersion;
extern int	verMinorVersion;
extern int	verMinorFreeze;
extern CString	verPatchLevel;

#endif /* !_VERSION_H_ */
