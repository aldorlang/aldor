/*****************************************************************************
 *
 * cfgfile.h: Configuration file handling
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "cfgfile.h"
#include "stdio.h0"
#include "strops.h"

Bool	cfgDebug		= false;

#define cfgDEBUG(s)		DEBUG_IF(cfgDebug, s)

static String cfgGetLine(FILE *file, Bool *atEof);
static Bool   cfgIsSection(char *line, char *name);
static void cfgReadAddError(String);
static void cfgReadDupItemError(ConfigItem, String);
static Bool cfgEqual(ConfigItem, ConfigItem);
static ConfigItemList cfgGetKeys(FILE *file, String section);
static ConfigItem cfgParseLine(String line);

CREATE_LIST(ConfigItem);

/*********************************************************************************
 *
 * :: Extracting config information
 *
 *********************************************************************************/
#define IsWhiteSpace(c) ((c)==' ' || (c)=='\n' || (c) == '\t')

static StringList cfgExpandList	(StringList *plst, 
				 StringList **end, 
				 ConfigItemList lst,
				 Bool keep);
static Bool 	  cfgCheckCondition(String name, ConfigItemList cfg);
static ConfigItem cfgExpandVar(ConfigItem, ConfigItemList);

static String cfgKeyNameValue(String str);
static String cfgStringValue(String str);
static StringList cfgStringListValue(String str);
static Bool cfgBooleanValue(String str);
static String cfgKeyNameValue(String str);

static ConfigItem
cfgExpandVar(ConfigItem item, ConfigItemList lst)
{
	char c;
	int i;
	
	if (!item) return NULL;
	i = 0;
	while ( (c = cfgVal(item)[i]) == ' ' || c == '\t') i++;
	if (cfgVal(item)[i] == '$') {
		item = cfgLookup(cfgKeyNameValue(&cfgVal(item)[i+1]), lst);
		return cfgExpandVar(item, lst);
	}
	return item;
}

ConfigItemList
cfgLookupList(char *string, ConfigItemList lst)
{
	ConfigItemList	result = listNil(ConfigItem);

	cfgDEBUG(printf("Getting key list: %s\n", string));

	while (lst != listNil(ConfigItem)) {
		if (strEqual(string, cfgName(car(lst))))
			result = listCons(ConfigItem)(car(lst), result);
		lst = cdr(lst);
	}
	return listNReverse(ConfigItem)(result);
}

ConfigItem
cfgLookup(char *string, ConfigItemList lst)
{
	cfgDEBUG(printf("Getting key: %s\n", string));

	while (lst != listNil(ConfigItem)) {
		if (strEqual(string, cfgName(car(lst))))
			return car(lst);
		lst = cdr(lst);
	}
	return NULL;
}

static StringList
cfgLookupUnexpandedStringList(String str, ConfigItemList lst)
{
	ConfigItem item = cfgLookup(str, lst);

	/*
	 * Ought to store this for the client to report just as we
	 * do for cfgRead().
	 */
	if (!item) {
		printf("config file: failed to find key `%s'\n", str);
		return NULL;
	}
	return cfgStringListValue(cfgVal(item));
}


StringList
cfgLookupStringList(String str, ConfigItemList lst)
{
	StringList ll, *foo;
	ll = cfgLookupUnexpandedStringList(str, lst);
	
	if (ll == listNil(String))
		return NULL;
	ll = cfgExpandList(&ll, &foo, lst, true);
	return ll;
}

#define cfgIfTerminator(x) ((x)[1] == '\0' && ((x)[0] == ':' || (x)[0] == ';')) 

static StringList
cfgExpandList(StringList *plst, StringList **end, ConfigItemList cfg, Bool keep)
{
	StringList *pp;
	StringList  head;
	String      error = NULL;
	pp = plst;
	head = *plst;
	while (*pp != listNil(String) && !cfgIfTerminator(car(*pp))) {
		String name;
		if (car(*pp)[0] != '$') {
			if (keep) pp = &cdr(*pp);
			else *pp = cdr(*pp);
			continue;
		}
		name = &(car(*pp)[1]);
		if (name[0] == '?') {
			Bool flg = cfgCheckCondition(&name[1], cfg);
			StringList *iend;
			*pp = cdr(*pp);
			*pp = cfgExpandList(pp, &iend, cfg, flg && keep);
			if (car(*iend)[0] != ':')
				error = "Missing `:'";
			if (cdr(*iend)) *iend = cdr(*iend);
			else 
				error = "Nothing after `:'.";
			cfgExpandList(iend, &iend, cfg, !flg && keep);
			/* Should check for ';' */
			*iend = cdr(*iend);
			pp = iend;
		}
		else {
			StringList ll;
			if (!keep) *pp = cdr(*pp);
			else {
				ll = cfgLookupUnexpandedStringList(name, cfg);
				*pp = listNConcat(String)(ll, cdr(*pp));
			}
		}
	}
	if (error) 
		printf("Error in cfg: %s\n", error);	
	*end = pp;
	return *plst;
}


String
cfgLookupKeyName(String str, ConfigItemList lst)
{
	ConfigItem item = cfgLookup(str, lst);
	item = cfgExpandVar(item, lst);

	if (!item) {
		return NULL;
	}
	return cfgKeyNameValue(cfgVal(item));
}

StringList
cfgLookupKeyNameList(String str, ConfigItemList lst)
{
	ConfigItemList	items = cfgLookupList(str, lst);
	StringList	result = listNil(String);


	/* Process each item in the list */
	for (; items; items = cdr(items)) {
		String		value;
		ConfigItem	item;


		/* Get the next item */
		item = car(items);


		/* Expand variables */
		item = cfgExpandVar(item, lst);


		/* Try the next one if no value found */
		if (!item) continue;


		/* Convert key into a value */
		value = cfgKeyNameValue(cfgVal(item));


		/* Add it to the result list */
		result = listCons(String)(value, result);
	}

	return listNReverse(String)(result);
}

String
cfgLookupString(String str, ConfigItemList lst)
{
	ConfigItem item = cfgLookup(str, lst);
	item = cfgExpandVar(item, lst);

	if (!item) {
		return NULL;
	}
	return cfgStringValue(cfgVal(item));
}

Bool
cfgLookupBoolean(String str, ConfigItemList lst)
{
	ConfigItem item = cfgLookup(str, lst);
	item = cfgExpandVar(item, lst);
	
	if (!item) {
		return 0; /* Hmmm */
	}
	return cfgBooleanValue(cfgVal(item));
}

static StringList
cfgStringListValue(String str)
{
	int argc;
	char **argv;
	StringList lst;
	int i;

	if (str[0] == ',')
		cstrParseCommaified(str+1, &argc, &argv);
	else 
		cstrParseUnquoted(str, &argc, &argv);
	
	lst = listNil(String);
	for (i=0; i<argc; i++) lst = listCons(String)(argv[argc - i - 1], lst);
	
	return lst;
}

static Bool
cfgBooleanValue(String str)
{
	String s = cfgKeyNameValue(str);
	
	if (strEqual(s, "yes")) return true;
	if (strEqual(s, "no")) return false;
	if (strEqual(s, "true")) return true;
	if (strEqual(s, "false")) return false;

	printf("Illegal value for boolean: %s\n", s);
	return false;
}

static String
cfgKeyNameValue(String str)
{
	char **argv;
	int argc;
	cstrParseUnquoted(str, &argc, &argv);
	if (argc != 1) {
		printf("Bad identifier format: `%s'\n", str);
		return "";
	}

	str = argv[0];
	stoFree(argv);
	return str;
}

static String
cfgStringValue(String str)
{
	char *p, *start, *new;
	/* Strip spaces front and rear, ignore leading and trailing quotes */
	p = str;
	while (IsWhiteSpace(*p) && *p != '\0') p++;
	if (*p == '"') p++;
	if (*p == '\0') return strCopy("");
	start = p;
	p = start + strlen(start) - 1;
	while (IsWhiteSpace(*p) && p != start) p--;
	if (*p == '"') p--;
	new = strCopy(start);
	new[p - start +1] = '\0';
	return new;
}

/*************************************************************************************
 *
 * :: Parsing input strings
 * 
 *************************************************************************************/



void
cstrParseUnquoted(char *str, int *pargc, char ***pargv)
{
	char *p = str;
	char *wstart;
	char **argv;
	char c;
	int  n, lim;
	
	n = 0;
	lim = 0;
	argv = NULL;
	while (1) {
		while (IsWhiteSpace(*p) && *p != '\0') p++;
		if (*p == '\0') break;
		wstart = p;
		/* May want to be clever here wrt to '"' */
		while (!IsWhiteSpace(*p) && *p != '\0') p++;
		c = *p;
		*p = '\0';
		if (n==lim) {
			char **targv = argv;
			int i;
			argv = (char **) stoAlloc(OB_Other, (n + 5)*sizeof(char *));
			lim += 5;
			for (i=0; i<n; i++) argv[i] = targv[i];
			if (targv) stoFree(targv);
		}
		argv[n++] = strCopy(wstart);
		*p = c;
		if (c == '\0') break;
	}
	
	*pargc = n;
	*pargv = argv;
}


void
cstrParseCommaified(char *opts, int *pargc, char ***pargv)
{
	String str = strCopy(opts);
	char **argv;
	char *p, *q;
	int  argc, i;

	p = str;
	q = p;
	argc = 1;
	while (*p != '\0') {
		if (*p == '\\') 
			p++;
		else if (*p == ',') {
			*p = '\0';
			argc++;
		}
		*q = *p;
		p++; q++;
	}
	*q = '\0';
	p = str;
	argv = (char **) stoAlloc(OB_Other, argc * sizeof(char *));
	for (i=0; i<argc; i++) {
		argv[i] = p;
		while (*p != '\0') p++;
		p++;
	}

	*pargc = argc;
	*pargv = argv;
}

/**********************************************************************************
 *
 * :: Conditions
 * 
 **********************************************************************************/

/* 
 * Function should return 1 for true, 0 for false, and -1 if
 * not set
 */

static int (*cfgCondFn)(String);

void
cfgSetCondFunc(int (*check)(String))
{
	cfgCondFn = check;
}

static Bool
cfgCheckCondition(String name, ConfigItemList cfg)
{
	int val;
	if ( (val = (*cfgCondFn)(name)) != -1)
		return val;

	return cfgLookupBoolean(name, cfg);
}

/**********************************************************************************
 *
 * :: Reading information
 * 
 **********************************************************************************/

static StringList cfgPath;

ConfigItem
cfgNew(char *key, char *val)
{
  	ConfigItem item;
  	int nkey, nval;
	nkey = strlen(key);
	nval = strlen(val);
	
	item = (ConfigItem) stoAlloc(OB_Other, fullsizeof(struct _ConfigItem, nkey + nval + 2, char));
	item->optName = item->content;
	strcpy(item->optName, key);
	item->optVal = item->content + nkey + 1;
	strcpy(item->optVal, val);

	return item;
}

void cfgFree(ConfigItem item)
{
  	stoFree(item);
}

void cfgSetConfPath(char *path)
{	
	Bool done;
  	String s = strCopy(path);
	char *start, *p;
	p = s;
	cfgPath = listNil(String);
	done = false;
	while (!done) {
	  	char c;
		start = p;
	  	while (*p != '\0' && *p != osPathSeparator())
	    		p++;
		c = *p;
		*p = '\0';
		cfgPath = listCons(String)(start, cfgPath);
		if (c == '\0') done = true;
		p++;
	}
}

void cfgFreeConfPath()
{
	if (cfgPath) strFree(car(cfgPath));
	listFree(String)(cfgPath);
}

FileName 
cfgFindFile(String basename, String ext)
{
  	StringList lst;
	
	lst = cfgPath;
	while (lst != listNil(String)) {
	  	FileName name = fnameNew(car(lst), basename, ext);
		if (fileIsOpenable(name, osIoRdMode))
			return name;
		lst = cdr(lst);
	}
	
 	return NULL;
}


/*
 * List of errors discovered during cfgRead(). The list is
 * reset to nothing each time cfgRead() is invoked and may
 * be accessed by calling cfgReadGetErrors().
 */
static StringList CfgReadErrs = listNil(String);


ConfigItemList 
cfgRead(FILE *file, char *name)
{
  	Bool done = false;

	/* Discard the previous error list */
	cfgReadClearErrors();


	/* Locate, and then read, the specified section */
  	while (!done) {
	  	String line = cfgGetLine(file, &done);
		if (cfgIsSection(line, name))
		  	return cfgGetKeys(file, name);
	}
	return NULL;
}


/* Return the list of errors from the previous cfgRead() call */
StringList
cfgReadGetErrors(void)
{
	return CfgReadErrs;
}


/* Reset the error list */
void
cfgReadClearErrors(void)
{
	/* Discard the previous error list */
	listFreeDeeply(String)(CfgReadErrs, strFree);


	/* Start a new error list */
	CfgReadErrs = listNil(String);
}


/* Add a new error to the list */
static void
cfgReadAddError(String error)
{
	CfgReadErrs = listCons(String)(strCopy(error), CfgReadErrs);
}


/* Report that we've seen this item before */
static void
cfgReadDupItemError(ConfigItem item, String section)
{
	String	msg;

	/* Compose the error message */
	msg = strlConcat(
		"Duplicate key `", cfgName(item),
		"' found in section [", section, "]",
		" of the compiler configuration file.",
		(String)NULL
	);


	/* Add the error to the list */
	cfgReadAddError(msg);
}


/* Equality based on key name */
static Bool
cfgEqual(ConfigItem itemA, ConfigItem itemB)
{
	return strEqual(cfgName(itemA), cfgName(itemB));
}

static Bool
cfgIsSection(char *line, char *name)
{
  	char *p = line;
	while (*p != ']' && *p != '\0') p++;
	*p = 0;
	return (strEqual(line+1, name));
}


static ConfigItemList 
cfgGetKeys(FILE *file, String section)
{
  	ConfigItemList lst;
  	Bool atEof;

	atEof = false;
	lst = listNil(ConfigItem);
	while (true) {
		ConfigItem item;
	  	String line = cfgGetLine(file, &atEof);


		/* Stop if we hit the next section */
		if (line[0] == '[') break;


		/* Parse the line to see if contains an item */
		item = cfgParseLine(line);


		/* Check for duplicates (allow `inherit') */
		if (item && !strEqual(cfgName(item), "inherit"))
		{
			if (listMember(ConfigItem)(lst, item, cfgEqual))
				cfgReadDupItemError(item, section);
		}


		/*
		 * We always add the item to the list even if one with
		 * the same key exists already. The client may wish to
		 * deal with duplicate keys themselves (e.g. inherit).
		 */
		if (item) lst = listCons(ConfigItem)(item, lst);


		/* Stop if we've reached the end of the file */
		if (atEof) break;


		/* Release temporary storage */
		strFree(line);
	}
	return lst;
}

static ConfigItem 
cfgParseLine(String line)
{
	String key;
	String val;
	char *p = line, *keyEnd;
	/* skip whitespace */
	while (*p == ' ' || *p == '\t') p++;

	if (*p == '#') return NULL;
	key = p;

	while (*p != ' ' && *p != '\t' && *p != '=' && *p != '\0') p++;
	keyEnd = p;

	if (*p == '\0') return NULL;

	while (*p != '=') p++;
  
	p++;
	val = p;
	*keyEnd = '\0';

	return cfgNew(key, val);
}

static String 
cfgGetLine(FILE *file, Bool *atEof)
{
  	String s;
	int n, lim, c;
	
	s = strAlloc(50);
	n = 0;
	lim = 50;

  	c = fgetc(file);
	while (c != '\n' && c != EOF) {
		if (n == lim) {
		  	String tmp = s;
			s = strAlloc(lim + 20);
			strncpy(s, tmp, n);
			strFree(tmp);
			lim += 20;
		}
		s[n] = c;
		c = fgetc(file);
		n++;
	}
	s[n] = '\0';
	if (c == EOF) *atEof = true;
	return s;
}

