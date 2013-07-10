/*****************************************************************************
 *
 * getconf.h: Configuration file handling
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _CFGFILE_H_
#define _CFGFILE_H_
 
#include "axlgen.h"

typedef struct _ConfigItem {
  	char *optName;
  	char *optVal;
  	char  content[NARY];
} *ConfigItem;

DECLARE_LIST(ConfigItem);

#define cfgName(item) ((item)->optName)
#define cfgVal(item)  ((item)->optVal)

extern void	      cfgSetConfPath    (String path);
extern FileName	      cfgFindFile	(String name, String ext);
extern ConfigItemList cfgRead		(FILE *file, char *name);
extern StringList     cfgReadGetErrors	(void);
extern void           cfgReadClearErrors(void);
extern void 	      cfgFree		(ConfigItem item);
extern ConfigItem     cfgNew		(char *key, char *val);

void cfgSetCondFunc(int (*check)(String));

extern ConfigItem cfgLookup(char *, ConfigItemList);
extern ConfigItemList cfgLookupList(char *, ConfigItemList);
extern StringList cfgLookupStringList(String, ConfigItemList);
extern String   cfgLookupKeyName(String, ConfigItemList);
extern StringList cfgLookupKeyNameList(String, ConfigItemList);
extern String  	cfgLookupString(String, ConfigItemList);
extern Bool 	cfgLookupBoolean(String, ConfigItemList);

extern void   cstrParseUnquoted(char *str, int *pargc, char ***pargv);
extern void   cstrParseCommaified(char *opts, int *pargc, char ***pargv);

#endif /*!_CFGFILE_H_*/
