/*
 * cenum.c
 *
 * See cenum.h for description.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "cenum.h"

static char	*scan		();
static int	evaluate	();
static EnumItem cons		();
static EnumItem freecons	();
static EnumItem nreverse   	();
static char	*vfind		();
static char	*mustAlloc	();
		
EnumItem
skimEnums(fname, enumc, enumv)
	char	*fname;
	int	enumc;
	char	**enumv;
{
	FILE	*instr;
	EnumItem entryList;
	char	*w, *ename;
	int	n;

	if (!strcmp(fname, "-"))
		instr = stdin;
	else
		instr = fopen(fname, "r");
	if (!instr) {
		fprintf(stderr, "Cannot open file \"%s\" for reading\n", fname);
		exit(0);
	}

	entryList = 0;
	
	while ((w = scan(instr)) != 0) {
		if (strcmp(w, "enum")) continue;

		ename = vfind(scan(instr), enumc, enumv);
		if (ename == 0) continue;

		w = scan(instr);
		if (!w || strcmp(w, "{")) continue;

		for (n = 0;;) {
			w = scan(instr);
			if (!w) break;

			entryList = cons(ename, w, entryList);

			w = scan(instr);

			if (!strcmp(w, "=")) {
				w = scan(instr);
				entryList->value = evaluate(w, entryList);
				n = entryList->value + 1;
				w = scan(instr);
			}
			else {
				entryList->value = n++;
			}
			
			if (!strcmp(w, ",")) continue;
			if (!strcmp(w, "}")) break;

			fprintf(stderr, "syntax error: %s\n", w);
			exit(0);
		}
	}
	return nreverse(entryList);
}

void
skimFree(entryList)
	EnumItem	entryList;
{
	while (entryList) 
		entryList = freecons(entryList);
}

EnumItem
skimNConcat(el1, el2)
	EnumItem el1, el2;
{
	EnumItem t;

	if (!el1) return el2;

	for (t = el1; t->next; t = t->next)
		;
	t->next = el2;

	return el1;
}

static char *
vfind(s, n, v)
	char	*s;
	int	n;
	char	**v;
{
	int	i;

	if (!s) return 0;

	for (i = 0; i < n; i++)
		if (!strcmp(s, v[i])) return v[i];

	return 0;
}

static EnumItem 
cons(ename, id, ol)
	char	*ename, *id;
	EnumItem ol;
{
	EnumItem nl;

	nl         = (EnumItem) mustAlloc(sizeof(*nl));

	nl->ename  = ename;
	nl->id     = strcpy(mustAlloc((int) strlen(id) + 1), id);
	nl->value  = 0;
	nl->next   = ol;

	return nl;
}

static EnumItem
freecons(ol)
	EnumItem ol;
{
	EnumItem tl;

	tl = ol->next;
	free(ol->id);
	free((char *) ol);
	return tl;
}

static EnumItem 
nreverse(el)
	EnumItem el;
{
	EnumItem rl, tl;

	rl = 0;
	while (el) {
		tl = el->next;
		el->next = rl;
		rl = el;
		el = tl;
	}
	return rl;
}

static int
evaluate(s, el)
	char	*s;
	EnumItem el;
{
	if (isdigit(*s)) return atoi(s);

	for ( ; el; el = el->next) 
		if (!strcmp(s, el->id)) return el->value;
	fprintf(stderr, "No value for %s\n", s);
	exit(1);
}

static char *
mustAlloc(n)
	int	n;
{
	char	*p;

	p = (char *) malloc(n);
	if (!p) {
		perror("Out of memory");
		exit(1);
	}
	return p;
}

/****************************************************************************
 *
 * :: Simplified C scanner.
 *
 ***************************************************************************/

/*
 * Treats multi-character special symbols (e.g. ->, ++, --) as a sequence
 * of single character symbols (- >, + +, - -).
 */

#define MAX_WORD	10000

static int	lineNo = 1;

static int
nextc(instr)
	FILE	*instr;
{
	int	c;
	c = getc(instr);
	if (c == '\n') lineNo++;
	return c;
}


static char *
scan(instr)
	FILE	*instr;
{
	static char	word[MAX_WORD+1];
	int	c, i;

restart:	
	while ( isspace(c = nextc(instr)) )
		;

	if (c == EOF) {
		word[0] = 0;
		return 0;
	}

	if (isalnum(c) || c == '_') {
		for (i = 0; isalnum(c) || c == '_'; i++) {
			word[i] = c;
			if (i >= MAX_WORD) {
				word[i] = 0;
				fprintf(stderr, "Token too long (> %d): %s\n",
					MAX_WORD, word);
				exit(1);
			}
			c = nextc(instr);
		}
		ungetc(c, instr);
		word[i] = 0;

		return word;
	}

	if (c == '"' || c == '\'') {
		int	c0 = c, lastc = 0;

		word[0] = c;
		for (i = 1; i < MAX_WORD; i++) {
			if (lastc == '\\' && c == '\\')
				lastc = 0;
			else
				lastc = c;
			c = nextc(instr);
			word[i] = c;
			if (c == c0 && lastc != '\\') {
				word[i+1] = 0;
				break;
			}
		}
		if (c != c0 && i == MAX_WORD) {
			fprintf(stderr, "Token too long (> %d): %s\n",
				MAX_WORD, word);
			exit(1);
		}

		return word;
	}

	if (c == '/') {
		int	lastc = c;

		if ((c = nextc(instr)) != '*') {
			ungetc(c, instr);
			c = lastc;
			goto special;
		}

		do {
			lastc = c;
			c = nextc(instr);
			if (c == EOF) return 0;
		} while (!(lastc == '*' && c == '/')); 
		
		goto restart;
	}

 special: {
		word[0] = c;
		word[1] = 0;
		return word;
	}
}
