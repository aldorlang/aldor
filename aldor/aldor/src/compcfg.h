/*****************************************************************************
 *
 * compcfg.h: Compiler configuration
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _COMPCFG_H_
#define _COMPCFG_H_
 
extern void compCfgInit(String);
extern void compCfgFini();

extern String compCfgLookupString();
extern Bool   compCfgLookupBoolean();

extern void compCfgSetConfigFile(String);
extern void compCfgSetSysName(String);

extern String compCfgGetConfigFile	(void);
extern String compCfgGetSysName		(void);

#endif /*!_COMPCFG_H_*/
