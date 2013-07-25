/*****************************************************************************
 *
 * fname.h: File name type.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _FNAME_H_
#define _FNAME_H_

#include "cport.h"

typedef struct fileName {
	String	partv[NARY];
} *FileName;

#define		FNAME_DIR			0
#define		FNAME_NAME			1
#define		FNAME_TYPE			2

#define 	fnameDir(fn)			((fn)->partv[FNAME_DIR])
#define 	fnameName(fn)			((fn)->partv[FNAME_NAME])
#define 	fnameType(fn)			((fn)->partv[FNAME_TYPE])

#define 	fnameTSetDir(fn,s)		((fn)->partv[FNAME_DIR]  = (s))
#define 	fnameTSetName(fn,s)		((fn)->partv[FNAME_NAME] = (s))
#define 	fnameTSetType(fn,s)		((fn)->partv[FNAME_TYPE] = (s))

extern FileName fnameNew                        (String d, String n, String t);
extern FileName fnameStdin			(void);
extern FileName fnameStdout			(void);

extern Bool	fnameIsStdin			(FileName);
extern Bool	fnameIsStdout			(FileName);

extern FileName fnameCopy                       (FileName);
extern void     fnameFree                       (FileName);
extern Bool	fnameEqual			(FileName, FileName);

extern FileName fnameParse                      (String);
extern FileName fnameParseStatic                (String);
extern FileName fnameParseStaticWithin          (String, String dir);

extern String	fnameUnparse			(FileName);
extern String   fnameUnparseStatic              (FileName);
extern String   fnameUnparseStaticWith          (FileName);
extern String   fnameUnparseStaticWithout       (FileName);

extern Bool	fnameHasDir			(FileName);
extern Bool	fnameHasType			(FileName);

extern void	fnameSetDir			(FileName, String);
extern void	fnameSetName			(FileName, String);
extern void	fnameSetType			(FileName, String);

extern FileName	fnameTemp			(String, String, String);
extern FileName*fnameTempVector			(String, String, String *);

#endif /* !_FNAME_H_ */
