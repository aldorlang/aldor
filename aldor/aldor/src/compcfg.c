/*****************************************************************************
 *
 * compcfg.c: Compiler configuration
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlgen.h"
#include "cfgfile.h"
#include "compcfg.h"
#include "comsgdb.h"
#include "comsg.h"
#include "file.h"
#include "store.h"

/****************************************************************************
 *
 * :: Configuration files
 *
 **************************************************************************/

local FILE *	     compCfgFindFile();
local void 	     compCfgSetPath(String root);
local ConfigItemList compCfgLoad(FILE *, String);

#if EDIT_1_0_n2_07
static char compCfgDefaultPath[] = ".%c%s/include%c%s/share/include";
static char compCfgDefaultFile[] = "aldor.conf";
static char compCfgOldDefaultFile[] = "axiomxl.conf";
static char compCfgDefaultRoot[] = "/usr/local/lib/aldor";
#else
static char *compCfgDefaultPath = ".%c%s/include%c%s/share/include";
static char *compCfgDefaultFile = "aldor.conf";
static char *compCfgOldDefaultFile = "axiomxl.conf";
static char *compCfgDefaultRoot = "/usr/local/lib/aldor";
#endif

String 		compCfgFileName;
String 		compCfgSysName = CONFIGSYS;
ConfigItemList  compCfgItems;

void
compCfgSetConfigFile(String file)
{
	compCfgFileName = file;
}

void
compCfgSetSysName(String name)
{
	compCfgSysName = name;
}

String
compCfgGetSysName()
{
	return compCfgSysName;
}
void
compCfgInit(String root)
{
	FILE *file;
	/* Set the config file path */
	if (!root)
		root = compCfgDefaultRoot;

	compCfgSetPath(root);
	/* Find the config file */
	file = compCfgFindFile();
	if (!file)
		comsgFatal(NULL, ALDOR_F_NoConfig);

	/* Read the information */
	compCfgItems = compCfgLoad(file, compCfgSysName);
	/* Tidy up */
	fclose(file);
}

void
compCfgFini()
{
}

local FILE *
compCfgFindFile()
{
	FILE *file;
	if (compCfgFileName) {
		FileName name = fnameParse(compCfgFileName);
		file = fileTryOpen(name, "r");
		/* NB: File may be null */
		fnameFree(name);
	}
	else {
		FileName fname = cfgFindFile(compCfgDefaultFile, "");
		if (!fname) fname = cfgFindFile(compCfgOldDefaultFile, "");
		if (!fname) return NULL;
		file = fileTryOpen(fname, "r");
	}

	return file;
}


local void 
compCfgSetPath(String root)
{
	char *path;
	
	path = (String) stoAlloc(OB_Other, 
				 strlen(compCfgDefaultPath) + 2 * strlen(root) + 1);
	sprintf(path, compCfgDefaultPath, osPathSeparator(), root, osPathSeparator(), root);

	cfgSetConfPath(path);
	strFree(path);
}


local ConfigItemList
compCfgLoad(FILE *file, String sysName)
{
	String		defSection;
	ConfigItemList	lst, all;
	StringList	errs, sections, tmp;

	all = listNil(ConfigItem);
	sections = listSingleton(String)(sysName);

	while (sections) {
		/* Read the name of the next section */
		defSection = car(sections);


		/* Remove it from the to-do list */
		sections = cdr(sections);


		/* Get all the options in the section */
		lst = cfgRead(file, defSection);


		/* Add them all to the result list */
		all = listNConcat(ConfigItem)(all, lst);


		/* Report any errors encountered */
		errs = listNReverse(String)(cfgReadGetErrors());
		for (;errs; errs = cdr(errs))
		{
			String err = car(errs);
			comsgWarning((AbSyn)NULL, ALDOR_W_CfgError, err);
		}


		/* Kill the error list because we've NReverse'd it */
		cfgReadClearErrors();


		/* Get the list of inherited sections */
		tmp = cfgLookupKeyNameList("inherit", lst);


		/* Place them at the front of our to-do list */
		sections = listNConcat(String)(tmp, sections);


		/* Back to the start of the file */
		rewind(file);
	}


	/* Now add the default options */
	all = listNConcat(ConfigItem)(all, cfgRead(file, "default"));


	/* Return the entire list */
	return all;
}


Bool
compCfgLookupBoolean(String str)
{
	return cfgLookupBoolean(str, compCfgItems);
}

String 
compCfgLookupString(String str)
{
	return cfgLookupString(str, compCfgItems); 
}
