/*****************************************************************************
 *
 * :: File types
 *
 ****************************************************************************/
#ifndef _FTYPE_H_
#define _FTYPE_H_

#include "cport.h"
#include "strops.h"
#include "axlgen.h"

#define FTYPE_SRC          "as"
#define FTYPE_INCLUDED     "ai"
#define FTYPE_ABSYN        "ap"
#define FTYPE_OLDABSYN     "ax"
#define FTYPE_INTERMED     "ao"
#define FTYPE_FOAMEXPR     "fm"
#define FTYPE_SYMEEXPR	   "asy"
#define FTYPE_LOCK	   "ask"
#define FTYPE_LISP         sxiLispFileType
#define FTYPE_C		   "c"
#define FTYPE_JAVA     	   "java"
#define FTYPE_CPP	   "c++"
#define FTYPE_H		   "h"
#define FTYPE_OBJECT	   osObjectFileType
#define FTYPE_AR_OBJ	   "a"
#define FTYPE_AR_INT	   "al"
#define FTYPE_MSG	   "cat"
#define FTYPE_EXEC	   osExecFileType
#define FTYPE_AXLMAINC      "c"

enum ftypeNo {
    FTYPENO_START,
	FTYPENO_OTHER = FTYPENO_START,
	FTYPENO_NONE,

	FTYPENO_SRC,
	FTYPENO_INCLUDED,
	FTYPENO_ABSYN,
	FTYPENO_OLDABSYN,
	FTYPENO_INTERMED,
	FTYPENO_FOAMEXPR,
	FTYPENO_SYMEEXPR,
        FTYPENO_LOCK,
	FTYPENO_LISP,
	FTYPENO_C,
	FTYPENO_JAVA,
	FTYPENO_CPP,
	FTYPENO_H,
	FTYPENO_OBJECT,
	FTYPENO_AR_OBJ,
	FTYPENO_AR_INT,
	FTYPENO_MSG,
	FTYPENO_EXEC,
	FTYPENO_AXLMAINC,
    FTYPENO_LIMIT
};

typedef Enum(ftypeNo)	FTypeNo;

extern FTypeNo	ftypeNo		(String);
extern String	ftypeString	(FTypeNo);

extern Bool	ftypeEqual	(String,   String);
extern Bool	ftypeIs		(String,   FTypeNo);
extern Bool	ftypeHas	(FileName, FTypeNo);

#endif /* !_AXLOBS_H_ */

