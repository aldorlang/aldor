/*****************************************************************************
 *
 * cmdline.h: OS Command line processing.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _CMDLINE_H_
#define _CMDLINE_H_

#include "axlgen.h"

extern int	cmdArguments(int argi0, int argc, String *argv);
			/*
			 * Process the command line options and return index
			 * of first file name argument.	 E.g., after
			 *
			 *   iargc = cmdArguments(1, argc, argv);
			 *
			 * the names of the files to compile are:
			 *
			 *   argv[iargc], ..., argv[iargc + cmdFileCount - 1].
			 *
			 * If -r is given, then the resulting program is
			 * passed the arguments:
			 *
			 *   argv[iargc + cmdFileCount], ..., argv[argc-1].
			 */

extern void	cmdHandleOption(int opt, String arg);
			/*
			 * Process the command line option and its argument.
			 */

extern String	cmdName;
			/*
			 * Name used to invoke the command.
			 */

extern String	cmdInitFile;
			/*
			 * Name of the user initialization file for
			 * interactive mode.
			 */


/*****************************************************************************
 *
 * :: Functions to manipulate argument vectors.
 *
 ****************************************************************************/

extern void	cmdEcho(FILE *, int argc, String *argv);
			/*
			 * Display a command line with appropriate quoting.
			 */

extern void 	cmdParseOptions(String s, int *pargc, String **pargv);
			/*
			 * Parse the options given in string s into a newly
			 * allocated null-terminated argument vector.  
			 * The vector and length are returned via parameters.
			 */
extern void	cmdFreeOptions(int argc, String *argv);
			/*
			 * Free the options returned by cmdParseOptions.
			 */

extern Bool	cmdSubsumeResponseFiles(int argi0, int *pargc, String **pargv);
			/*
			 * Form full command line following response file args.
			 * Returns true if any response files were encountered.
			 */

extern Bool	cmdHasOption(int o,String a,int argc,String *argv);
extern String	cmdOptionArg;
			/*
			 * Test whether the cmd line has the given option.
			 * If 'a' is non-null, the argument must match it.
			 * The actual argument is saved as 'cmdOptionArg'.
			 */
Bool cmdHasOptionPrefix(int opt0, String arg0, int argc, String *argv);
			/*
			 * Test whether the cmd line has the given option 
			 * prefix.  If 'a' is non-null, the argument must 
			 * match it.  The actual argument is saved as 
			 * 'cmdOptionArg'.
			 */

extern void cmdDebugReset();

/*****************************************************************************
 *
 * :: Macros to probe an agrument vector without calling cmdArguments
 *
 ****************************************************************************/

#define cmdHasVerboseOption(ac,av)     cmdHasOption('V', NULL,   ac,av)
#define cmdHasHelpOption(ac,av)        cmdHasOption('H', NULL,   ac,av)
#define cmdHasDebugOption(ac,av)       cmdHasOption('Z',"db",    ac,av)
#define cmdHasSExprOption(ac,av)       cmdHasOption('W',"sexpr", ac,av)
#define cmdHasSEvalOption(ac,av)       cmdHasOption('W',"seval", ac,av)
#define cmdHasGcOption(ac,av)	       cmdHasOption('W',"gc",    ac,av)
#define cmdHasNoGcOption(ac,av)	       cmdHasOption('W',"no-gc", ac,av)
#define cmdHasGcFileOption(ac,av)      cmdHasOption('W',"gcfile",ac,av)
#define cmdHasRootOption(ac,av)        cmdHasOption('B', NULL,   ac,av)
#define cmdHasOptimizeOption(ac,av)    cmdHasOption('Q', NULL,   ac,av)
#define cmdHasInteractiveOption(ac,av) cmdHasOption('G',"loop",  ac,av)
#define cmdHasCfgFileOption(ac,av)     cmdHasOptionPrefix('N',"file=",  ac,av)
#define cmdHasCfgNameOption(ac,av)     cmdHasOptionPrefix('N',"sys=",  ac,av)


/*****************************************************************************
 *
 * :: Variables set after cmdArguments has been called.
 *
 ****************************************************************************/

extern Bool	cmdVerboseFlag;	    /* Had the verbose option    (-V)?      */
extern Bool	cmdTrapFlag;	    /* Had the trap option	 (-Wtrap)?  */
extern Bool	cmdSExprFlag;	    /* Had the sexpr option      (-Wsexpr)? */
extern Bool	cmdSEvalFlag;	    /* Had the lisp option	 (-Wseval)? */
extern Bool	cmdGcFlag;	    /* Had the gc option         (-Wgc)?    */
extern Bool	cmdGcFileFlag;	    /* Had the gcfile option     (-Wgcfile)?*/
extern Bool     cmdFloatRepFlag;    /* Decrease double precision -Wfloatrep */

extern int	cmdFileCount;	    /* Number of files to compile.          */

#endif	/* !_CMDLINE_H_ */
