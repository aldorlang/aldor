/*****************************************************************************
 *
 * doc.h: ++ Documentation handling.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _DOC_H_
#define _DOC_H_

#include "axlobs.h"
#include "path.h"

/*****************************************************************************
 *
 * :: ++ Comment structure
 *
 ****************************************************************************/

struct doc {
	Bool		hasCorpus;	/* Have the lines been collected. */
	AInt		hash;		/* Hash code for string */
	StringList	lines;		/* Each string represents one line. */
	String		corpus;		/* The catenation of the lines. */
	Length		cc;		/* The length of the corpus. */
};

#define docDone(doc)		((doc)->hasCorpus)
#define docLines(doc)		((doc)->lines)
#define docString(doc)		((doc)->corpus)
#define docLength(doc)		((doc)->cc)

extern Doc	docNone;

extern Doc	docNewFrString	(String);
extern Doc	docNewFrList	(TokenList);
extern Doc	docCopy		(Doc);
extern void	docFree		(Doc);
extern Doc	docMerge	(Doc, Doc);
extern Bool	docEqual	(Doc, Doc);
extern int	docPrint	(FILE *, Doc);
extern Hash	docHash		(Doc);

#endif /* !_DOC_H_ */
