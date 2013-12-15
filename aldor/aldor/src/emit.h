/*****************************************************************************
 *
 * emit.h: Compiler output.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _EMIT_H_
#define _EMIT_H_

#include "axlobs.h"
#include "ftype.h"

/*
 * Controlling options.
 */
extern int    emitSelect        (String);    /* Parse and handle -F option.  */
extern void   emitAllDone	(void);
extern void   emitSetDone	(int);

extern void   emitSetEntryFile  (String);    /* Set entry file:   -e         */
extern int    emitSetOutputDir  (String);    /* Output directory: -R         */
extern int    emitSetOutputFile (FTypeNo,String); 
					     /* Select output:    -F<ft>=<fn>*/
extern void   emitSetCfgFile    (String);

extern void   emitSetCName      (String);    /* Prefix for C names.          */
extern void   emitSetDebug      (Bool);      /* Want debug info:  -Zg        */
extern void   emitSetProfile    (Bool);      /* Want profile info:-Zp        */
extern void   emitSetRun	(Bool);      /* Run result:       -go        */
extern void   emitSetInterp	(Bool);      /* Run result:       -g[fi]     */
extern void   emitSetStandardC  (Bool);      /* -Cstandard vs -Coldc.        */

extern void   emitDoneOptions   (int argc, String *argv);

extern String emitGetEntryFile  (void);

/*
 * File information structure.
 */
typedef struct emitInfo {
	FileName *	fnameTempV;
	FileName	fname[FTYPENO_LIMIT];
	BPack(Bool)	inUse[FTYPENO_LIMIT];
	FileNameList	flist;
	Bool		isAXLmain; /* Is the generated file invoking "entry"? */
	Bool 		isStdIn;
} *EmitInfo;

extern EmitInfo emitInfoNew		(FileName srcfn);
extern EmitInfo emitInfoNewAXLmain	(void);
extern void	emitInfoFree		(EmitInfo finfo);

extern void	emitSetDependsWanted	(Bool);
extern Bool	emitDependsWanted	(void);

/*
 * File names to use.
 */

extern FileName	emitSrcFile		(EmitInfo);
extern FileName emitFileName		(EmitInfo, FTypeNo);

extern Bool	emitIsOutputNeeded      (EmitInfo, FTypeNo);
extern Bool	emitIsOutputNeededOrWarn(EmitInfo, FTypeNo);
extern EmitInfo	emitExecutableNameOrWarn(int, EmitInfo *);

extern Bool	emitIsGeneratedFile 	(FileName);	/* Examines contents */

extern String	emitGetFileIdName	(EmitInfo);
extern void	emitSetFileIdName	(String);
extern void	emitSetFileIdPrefix	(String);

/*
 * Syme list collectors.
 */

extern SymeList	emitCollectIntermedSymes(Stab, Foam);

/*
 * Dumb emitters -- just do what they're told.
 */
extern void	emitTheIncluded	    	(EmitInfo, SrcLineList);
extern void	emitTheOldAbSyn	    	(EmitInfo, AbSyn);
extern void	emitTheAbSyn	    	(EmitInfo, AbSyn);
extern void	emitTheIntermed	    	(EmitInfo, SymeList, Foam, AbSyn);
extern void	emitTheDependencies    	(EmitInfo);
extern void	emitTheSymbolExpr    	(EmitInfo, SymeList, AbSyn);
extern void	emitTheFoamExpr	    	(EmitInfo, Foam);
extern void	emitTheLisp	    	(EmitInfo, SExpr);
extern void	emitTheC    	    	(EmitInfo, CCodeList);
extern void	emitTheCpp    	    	();
extern void	emitTheObject	    	(EmitInfo);

/*
 * Linkage, execution, and cleanup.
 */
extern void	emitLink	    	(int, EmitInfo *);
extern void	emitRun		  	(int, String   *);
extern void	emitInterp	  	(int, String   *);
extern void	emitCleanup		(int, EmitInfo *);

#endif /* !_EMIT_H_ */
