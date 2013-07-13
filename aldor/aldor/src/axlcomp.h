/*****************************************************************************
 *
 * axlcomp.h: Aldor compiler top-level structure
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _AXLCOMP_H_
#define _AXLCOMP_H_

# include "axlgen.h"


/*
 * Top-level entry points.
 */
extern int		compCmd(int argc, String *argv);
			/*
			 * Select and call one of the compXxxxLoop entry points.
			 */

extern int		compFilesLoop(int argc, String *argv);
			/*
			 * Compile files, controlled by the argument vector
			 * and return the total error count.
			 */

extern int		compInteractiveLoop(int argc, String *argv,
					    FILE *in, FILE *out);
			/*
			 * Compile statement-by-statement from input stream,
			 * display results on output stream and return the
			 * total error count.  The overall process is
			 * controlled by the argument vector.
			 */

extern int		compSExprLoop(FILE *in, FILE *out);
			/*
			 * Read expressions from the file "in" and write them
			 * to the file "out".
			 */

extern int		compSEvalLoop(FILE *in, FILE *out);
			/*
			 * Read expressions from the file "in",
			 * evaluate them (according to a limited subset
			 * of Common Lisp semantics),
			 * and write them to the file "out".
			 */

/*
 * Constituents of the "compXxxxLoop" programs.
 */
extern void		compInit(void);
			/*
			 * Initialize a compXxxxLoop program.
			 */

extern int		compSourceFile(EmitInfo);
extern int		compSavedFile (EmitInfo);
extern int		compCFile     (EmitInfo);

			/*
			 * Compile file, returning error count.
			 */

extern void		compAXLmainFile(EmitInfo);
			/*
			 * Generate and compile the temporary axlmain.c file.
			 */

extern void		compFini(void);
			/*
			 * Final actions before exiting a compXxxxLoop program.
			 */

/*
 * Constituents of the "compXxxxFile" programs.
 */
extern void		compFileInit	 	(EmitInfo);
extern void		compFileFini	 	(EmitInfo);

extern AbSyn		compFileLoadAbSyn	(EmitInfo);
extern Foam		compFileLoadFoam 	(EmitInfo);
extern void		compFileSave	 	(EmitInfo, Stab, Foam);

extern AbSyn		compFileFront	 	(EmitInfo, Stab, FILE *, int *);
extern Foam		compFileMiddle	 	(EmitInfo, Stab, AbSyn);
extern void		compFileBack	 	(EmitInfo, Foam);

extern Bool		compIsMoreAfterInclude	(EmitInfo);
extern Bool		compIsMoreAfterSyntax 	(EmitInfo);
extern Bool		compIsMoreAfterFront  	(EmitInfo);

/*
 * Compiler phases -- constituents of the "compFileXxxx" programs.
 */
extern AbSyn		compPhaseLoadAbSyn    	(EmitInfo);
extern Foam		compPhaseLoadFoam     	(EmitInfo);

extern SrcLineList	compPhaseInclude      	(EmitInfo, FILE *, int *);
extern TokenList	compPhaseScan	      	(EmitInfo, SrcLineList);
extern TokenList	compPhaseSysCmd       	(EmitInfo, TokenList);
extern TokenList	compPhaseLinear       	(EmitInfo, TokenList);
extern AbSyn		compPhaseParse	      	(EmitInfo, TokenList);
extern AbSyn		compPhaseMacEx	      	(EmitInfo, AbSyn);
extern AbSyn		compPhaseAbNorm       	(EmitInfo, AbSyn, Bool);
extern AbSyn		compPhaseAbCheck      	(EmitInfo, AbSyn);
extern AbSyn		compPhaseScoBind      	(EmitInfo, Stab, AbSyn);
extern AbSyn		compPhaseTInfer       	(EmitInfo, Stab, AbSyn);
extern Foam		compPhaseGenFoam      	(EmitInfo, Stab, AbSyn);
extern Foam		compPhaseOptFoam      	(EmitInfo, Foam);
extern void		compPhaseSave	      	(EmitInfo, Foam, Stab);
extern void		compPhasePutLisp      	(EmitInfo, Foam);
extern void		compPhasePutC	      	(EmitInfo, Foam);
extern void		compPhasePutAXLmainC   	(EmitInfo);
extern void		compPhasePutObject    	(EmitInfo);

/*
 * Debugging tools.
 */
extern void		compInfoAudit		(void);

/*
 * Handlers passed to subsystems.
 */

extern void SignalModifier compSignalHandler	(int signo);
extern void		 compExitHandler  	(int status);
extern MostAlignedType * compStoreError	  	(int errnum);
extern MostAlignedType * compFreeError	  	(int errnum);
extern FILE *		 compFileError	  	(FileName, IOMode);
extern SExpr		 compSExprError	  	(SrcPos, int errnum, va_list);
extern void		compFintBreakHandler	(int signo);

extern String compRootDir;

#endif /* !_AXLCOMP_H_ */
