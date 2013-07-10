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
 * Version of the program. The edit number is for internal purposes
 * and allows more precise release checking. There are so many changes
 * that are being made to the compiler between patch levels that we
 * need a more high-resolution version counter, hence axiomxlEditNumber.
 */
#if EDIT_1_0_n2_07
extern const char *	axiomxlName;
#else
extern char *	axiomxlName;
#endif
extern int	axiomxlMajorVersion;
extern int	axiomxlMinorVersion;
extern int	axiomxlMinorFreeze;
extern int	axiomxlEditNumber; /* martin@nag.co.uk */
#if EDIT_1_0_n2_07
extern const char *	axiomxlPatchLevel;
#else
extern char *	axiomxlPatchLevel;
#endif
extern Bool	verBannerWanted		(void);
extern void	verPrint		(void);

#endif /* !_VERSION_H_ */
