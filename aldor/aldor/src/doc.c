/*****************************************************************************
 *
 * doc.c: ++ Documentation handling.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlobs.h"
#include "debug.h"
#include "store.h"

CREATE_LIST(Doc);

/* docNone = docNewFrString(""); */
static struct doc __docNone = {
	true,
	0,
	listNil(String),
	"",
	0
};

Doc	docNone = &__docNone;

Doc
docNewEmpty(Length cc)
{
	Doc	doc;

	doc = (Doc) stoAlloc((unsigned) OB_Doc, sizeof(*doc) + cc);

	doc->hasCorpus	= true;
	doc->lines	= listNil(String);
	doc->hash 	= 0;
	doc->corpus	= ((char *) doc) + sizeof(*doc);
	doc->cc		= cc;
	return doc;
}

Doc
docNewFrString(String s)
{
	Doc		doc;
	Length		cc = strLength(s) + 1;

	doc = docNewEmpty(cc);

	doc->corpus	= strcpy(doc->corpus, s);
	doc->hash	= strHash(s);

	return doc;
}

Doc
docNewFrList(TokenList tl)
{
	Doc		doc;
	TokenList	l;
	Length		cc = 1;

	for (l = tl; l; l = cdr(l))
		cc += strLength(car(l)->val.str) + 1;

	doc = docNewEmpty(cc);

	doc->corpus[0]	= 0;
	for (l = tl; l; l = cdr(l)) {
		strcat(doc->corpus, car(l)->val.str);
		strcat(doc->corpus, "\n");
	}
	doc->hash	= strHash(doc->corpus);

	DEBUG({
		printf("docNewFrList:  cc = %d; strlen = %d\n",
		       (int) cc, (int) strLength(doc->corpus));
	});

	return doc;
}

Doc
docCopy(Doc doc)
{
	return docNewFrString(doc->corpus);
}

void
docFree(Doc doc)
{
	stoFree((Pointer) doc);
}

Doc
docMerge(Doc doc1, Doc doc2)
{
	return docNewFrString(strConcat(docString(doc1), docString(doc2)));
}

Bool
docEqual(Doc doc1, Doc doc2)
{
	if (doc1 == doc2) 
		return true;

	if (strEqual(docString(doc1), docString(doc2)))
		return true;

	return false;
}

Hash
docHash(Doc doc)
{
	return doc->hash;
}

int
docPrint(FILE *file, Doc doc)
{
	if (doc)
		return fprintf(file, "++%s", docString(doc));
	else
		return 0;
}
