#include "rdln.h"

#ifndef USE_GLOOP_SHELL
#include "opsys.h"
#include "comsg.h"

void rdlnInit(void)
{
}

void rdlnCheck(void)
{
}

void rdlnFini(void)
{
}

void rdlnSetPrompt(String fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	comsgPromptVPrint(osStdin, osStdout, fmt, argp);
	va_end(argp);
}


#endif

#ifdef USE_GLOOP_SHELL

#include <stdio.h>

#include "readline/readline.h"
#include "readline/history.h"
#include "format.h"
#include "opsys.h"
#include "stab.h"
#include "strops.h"
#include "symbol.h"

local char *rdLineTxt = NULL;
local int rdLinePos = 0;
local int rdlnAtEof = 0;
local char *rdlnPrompt;
local SymbolList rdlnSyms;

local int rdlnGetc(FILE *f);
local int rdlnFEof(FILE *f);
local void rdlnInitCompletion(const char *txt);
local char *rdlnCompletion(const char *text, int state);

local int
rdlnGetc(FILE *f)
{
	if (rdlnAtEof) {
		return EOF;
	}
	if (rdLineTxt == NULL) {
		rdLineTxt = readline(rdlnPrompt);
		if (rdLineTxt == NULL) {
			rdlnAtEof = 1;
			return EOF;
		}
		if (rdLineTxt[0] != '\0') {
			add_history(rdLineTxt);
		}
	}
	if (rdLineTxt[rdLinePos] == '\0') {
		//free(rdLineTxt);
		rdLineTxt = NULL;
		rdLinePos = 0;
		return '\n';
	}
	return rdLineTxt[rdLinePos++];
}

local int
rdlnFEof(FILE *f)
{
	return rdlnAtEof;
}

void
rdlnSetPrompt(String fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	String s = vaStrPrintf(fmt, argp);
	va_end(argp);
	if (rdlnPrompt != NULL) {
		strFree(rdlnPrompt);
	}

	rdlnPrompt = s;
}

local void
rdlnInitCompletion(const char *txt)
{
	TableIterator iter;
	SymbolList syms = listNil(Symbol);

	for (tblITER(iter, car(stabFile())->tbl); tblMORE(iter); tblSTEP(iter)) {
		Symbol sym = tblKEY(iter);
		if (strIsPrefix(txt, symString(sym))) {
			syms = listCons(Symbol)(tblKEY(iter), syms);
		}
	}
	rdlnSyms = syms;
}

local char *
rdlnCompletion(const char *text, int state)
{
	if (state == 0) {
		listFree(Symbol)(rdlnSyms);
		rdlnInitCompletion(text);
	}
	if (rdlnSyms == listNil(Symbol)) {
		return NULL;
	}
	else {
		Symbol sym = car(rdlnSyms);
		rdlnSyms = listFreeCons(Symbol)(rdlnSyms);
		char *txt = malloc(strLength(symString(sym)) + 1);
		strcpy(txt, symString(sym));
		return txt;
	}
}

void
rdlnInit()
{
	osSetReadFns(rdlnGetc, rdlnFEof);
	rl_completion_entry_function = rdlnCompletion;
}

void
rdlnFini()
{
	listFree(Symbol)(rdlnSyms);
	rdlnSyms = NULL;
	if (rdlnPrompt != NULL) {
		strFree(rdlnPrompt);
		rdlnPrompt = NULL;
	}
}
	
#endif
