/*****************************************************************************
 *
 * syscmd.c: System command processing.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "abnorm.h"
#include "fint.h"
#include "fintphase.h"
#include "macex.h"
#include "parseby.h"
#include "scan.h"
#include "stab.h"
#include "syscmd.h"
#include "util.h"

/*****************************************************************************
 *
 * :: Parser/Handlers
 *
 ****************************************************************************/

/*
 * Handle all system commands, transforming the list.
 */

TokenList
scmdProcessList(TokenList tl)
{
	struct TokenListCons	head;
	TokenList		prev, curr;
	Token			tok;

	setcar(&head, NULL);
	setcdr(&head, tl);

	for (prev = &head; cdr(prev); ) {
		curr = cdr(prev);
		tok  = car(curr);

		if (tokTag(tok) != TK_SysCmd) {
			prev = curr;
		}
		else {
			tl = scmdProcessToken(tok);

			if (tl) {
				setcdr(listLastCons(Token)(tl), cdr(curr));
				setcdr(prev, tl);
			}
			else
				setcdr(prev, cdr(curr));

			listFreeCons(Token)(curr);
			tokFree(tok);
		}
	}

	return cdr(&head);
}


TokenList
scmdProcessToken(Token tok)
{
	if (tokTag(tok) != TK_SysCmd) return 0;

	return scmdProcess(tok->pos, tok->val.str);
}

TokenList
scmdProcessOrCheck(Bool doit, SrcPos spos, String cmd)
{
	String		s, id, fname;
	int		rc = 0;
	TokenList	tl = 0;

	if (scmdIsDirective(cmd, "")) {
		/* Accept empty directive. */
	}
	else if ((s = scmdIsDirective(cmd, "pile")) != 0) {
		if (doit) {
			Token tok = tokKeyword(spos, spos, KW_StartPile);
			tl = listCons(Token)(tok, NULL);
		}
	}
	else if ((s = scmdIsDirective(cmd, "endpile")) != 0) {
		if (doit) {
			Token tok = tokKeyword(spos, spos, KW_EndPile);
			tl = listCons(Token)(tok, NULL);
		}
	}
	else if ((s = scmdIsDirective(cmd, "library")) != 0) {
		if (doit) {
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			if ((s = scmdScanFName(s, &fname)) != 0)
				rc = scmdHandleLibrary(NULL, fname);
			else
*/
			if ((s = scmdScanId(s, &id)) != 0 &&
			         (s = scmdScanFName(s, &fname)) != 0)
				rc = scmdHandleLibrary(id, fname);
			if (!s || rc < 0)
				comsgError(abNewNothing(spos),
					   ALDOR_E_SysCmdBad,"library");
		}
	}
	else if ((s = scmdIsDirective(cmd, "libraryDir")) != 0) {
		if (doit) {
			if ((s = scmdScanFName(s, &fname)) != 0)
				rc = scmdHandleLibraryDir(fname);
			if (!s || rc < 0)
				comsgError(abNewNothing(spos),
					   ALDOR_E_SysCmdBad,"libraryDir");
		}
	}
	else if ((s = scmdIsDirective(cmd, "includeDir")) != 0) {
		/*
		 * This ought to have been handled by "include". If
		 * we reach this point then surely something is wrong.
		 */
		if (doit) {
			if ((s = scmdScanFName(s, &fname)) != 0)
				rc = scmdHandleIncludeDir(fname);
			if (!s || rc < 0)
				comsgError(abNewNothing(spos),
					   ALDOR_E_SysCmdBad, "includeDir" );
		}
	}
	else if	((s = scmdIsDirective(cmd, "error")) != 0) {
		if (doit)
			comsgError(abNewNothing(spos), ALDOR_E_ExplicitMsg, s);
	}
	else if ((s = scmdIsDirective(cmd, "quit")) != 0) {
		/* Doesn't make sense unless under the interpreter */
		if (doit)
			exitSuccess();
	}
	else if ((s = scmdIsDirective(cmd, "int")) != 0) {
		if (doit) {
			if (fintMode == FINT_LOOP) fintParseOptions(s);
		}
	}
#if OBSOLETE
	else if ((s = scmdIsDirective(cmd, "syntax")) != 0) {
		if (doit) {
			printf("#syntax  \"%s\"\n", s);
		}
	}
	else if ((s = scmdIsDirective(cmd, "macro")) != 0) {
		if (doit)
			scmdHandleMacro(spos, s);
	}
#endif
	else {
		comsgWarning(abNewNothing(spos), ALDOR_W_SysCmdUnknown);
	}

	return tl;
}

TokenList
scmdProcess(SrcPos spos, String cmd)
{
	return scmdProcessOrCheck(true, spos, cmd);
}

void
scmdCheck(SrcPos spos, String cmd)
{
	scmdProcessOrCheck(false, spos, cmd);
}


/*****************************************************************************
 *
 * :: Scanners
 *
 ****************************************************************************/

/*
 * If the line is the right kind of command return the tail, otherwise 0.
 */
local String
restIfCmd(String ln, String kword, Bool isAbbrev)
{
	String	s;

	/* We are guaranteed that *ln is a directive character. */
	ln++;

	/* Specially handle test for empty command. */
	if (!*kword) {
		if (*ln == '!') return ln+1;		/* #!...\n */
		while (*ln && isspace(*ln)) ln++;	/* #    \n */
		if (!*ln) return ln;
		return 0;
	}

	/* Allow spaces before the directive name. */
	while (*ln && isspace(*ln)) ln++;

	/* Check that the directive name is a prefix of ln. */
	s = strIsPrefix(kword, ln);
	if (s == 0) return 0;

	/* Skip to end of word, if kword is an abbreviation. */
	if (isAbbrev) while (*s && !isspace(*s)) s++;

	/* If there is nothing left, return the empty tail. */
	if (!*s) return s;

	/* Otherwise space after the directive name. */
	if (!isspace(*s)) return 0;

	return s;
}

String
scmdIsAbbrev(String ln, String kword)
{
	return restIfCmd(ln, kword, true);
}

String
scmdIsDirective(String ln, String kword)
{
	return restIfCmd(ln, kword, false);
}

/*
 * Extract an integer argument.
 * Non-zero indicates success and is the tail of the string.
 */
String
scmdScanInteger(String s, int *pno)
{
	int	n;

	while (*s && isspace(*s)) s++;
	if (!*s || !isdigit(*s)) return 0;

	for (n = 0; *s && isdigit(*s); s++) n = n*10 + *s-'0';
	*pno = n;

	return s;
}

/*
 * Extract a string in quotes.
 * Non-zero indicates success and is the tail of the string.
 */
String
scmdScanFName(String s, String *pfn)
{
	String t;

	while (*s && isspace(*s)) s++;
	if (!*s) return 0;

	if (*s != '"') return 0;

	for (t = ++s; *t && *t != '"'; ) t++;
	*pfn = strnCopy(s, t-s);
	if (*t) t++;
	s = t;

	return s;
}

/*
 * Extract a word of non-spaces.
 * Non-zero indicates success and is the tail of the string.
 */
String
scmdScanId(String s, String *pid)
{
	String t;

	while (*s && isspace(*s)) s++;
	if (!*s) return 0;

	for (t = s; *t && !isspace(*t); ) t++;
	*pid = strnCopy(s, t-s);
	s = t;

	return s;
}

/*
 * Scan the library specifier into an optional tag string and a key string.
 * Non-zero indicates success and is the tail of the string.
 *
 * The arg is either "key" or "id=key" or "id:key".
 */
String
scmdScanLibraryOption(String s, String *pid, String *key)
{
	String 	t;

	/* Scan id, if there. */
	for (t = s; *t && *t != '=' && *t != ':'; t++) ;
	if (*t != '=' && *t != ':')
		*pid = 0;
	else {
		if (t == s) return 0;
		*pid = strnCopy(s, t-s);
		s = t + 1;
	}

	/* Scan key. */
	if (!*s) return 0;
	for (t = s; *t; t++) ;
	*key = strCopy(s);
	s = t;

	return s;
}

/*****************************************************************************
 *
 * :: Specific handlers
 *
 ****************************************************************************/

/*
 * Add filename to the library directory search path.
 * Return 0 on success, -1 on failure.
 */
int
scmdHandleLibraryDir(String filename)
{
	if (!*filename) return -1;
	fileAddLibraryDirectory(filename);
	return 0;
}

/*
 * Add filename to the include directory search path.
 * Return 0 on success, -1 on failure.
 */
int
scmdHandleIncludeDir(String filename)
{
	if (!*filename) return -1;
	fileAddIncludeDirectory(filename);
	return 0;
}

/*
 * :: scmdHandleLibrary
 *
 * A library reference is a keyed reference if the library file name
 *	has neither directory nor extension.
 * A keyed reference refers to an archive, which may or may not contain
 *	ao files.
 *
 * Add the library to the list to be searched for ao files,
 *	if the library is an archive that contains ao files.
 * Add a keyed reference to the list to be passed to the loader,
 *	unless the archive contains ao files.
 * Add a library symbol meaning to the top-level symbol table,
 *	if the id is non-null.
 * Return 0 on success, -1 on failure.
 */
int
scmdHandleLibrary(String id, String key)
{
	Symbol		sym;
	FileName	fn;
	Bool		keyed;

	/* Intern the symbol, if present. */
	sym  = id ? symIntern(id) : 0;

	/* Determine if the argument is a true file name or a library key. */
	fn    = fnameParseStatic(key);
	keyed = !(fnameHasDir(fn) || fnameHasType(fn));

	/* If the argument is a library key or an archive of intermed. files,
	   see if we can use it as an archive of intermediate files. */
	if (keyed || ftypeHas(fn, FTYPENO_AR_INT)) {
		String		fstr;
		Archive		ar;

		if (keyed) {
			/* Add the key to the loader library key list. */
			arAddLibraryKey(key);

			fstr = strPrintf("lib%s.%s", key, FTYPE_AR_INT);
		}
		else
			fstr = key;

		/* Check for archive not found. */
		if ((ar = arFrString(fstr)) == 0) {
			if (!keyed)
				comsgWarning(NULL, ALDOR_F_CantOpen, fstr);
		}

		else if (arHasIntermed(ar)) {
			/* Add the file name to the archive search list. */
			arAddLibraryFile(fstr);

			/* Define the archive symbol meaning. */
			if (sym) stabDefArchive(stabFile(), sym, tfType, ar);
			macexAddMacro(arGetGlobalMacros(ar), false);
		}

		/* Archive of intermediate files not used. */
		else if (!keyed)
			comsgWarning(NULL, ALDOR_W_CantUseArchive, fstr);
	}

	/* If the argument is an intermediate file, use it as a library. */
	else if (ftypeHas(fn, FTYPENO_INTERMED)) {
		Lib	lib;

		/* Define the library symbol meaning. */
		if (sym && (lib = libFrString(key)) != 0) {
			stabDefLibrary(stabFile(), sym, tfType, lib);
			macexAddMacro(libGetMacros(lib), false);
		}
		/* Object file not used. */
		else
			comsgWarning(NULL, ALDOR_W_CantUseObject, key);
	}

	/* Unknown file type not used.
	 *
	 * Note that we can't use foo.a, since we can't determine
	 * if a library key even exists that would refer to foo.a,
	 * and foo.a should not contain any intermediate files.
	 */
	else {
		comsgWarning(NULL, ALDOR_W_CantUseLibrary, key);
		return -1;
	}

	return 0;
}

int
scmdHandleMacro(SrcPos pos, String s)
{
	SrcLineList lines;
	TokenList   toks;

	lines = listCons(SrcLine)(slineNew(pos, 5, s), listNil(SrcLine));
	toks = listCons(Token)(tokKeyword(pos, pos, KW_Macro), scan(lines));
	/* This should do a bunch more checking... */
	macexAddMacro(abNormal(parse(&toks), false), true);
	return 0;
}
