/*****************************************************************************
 *
 * axlgen.h: General library, not specific to Aldor.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _AXLGEN_H_
#define _AXLGEN_H_

/*
 * Declarations for basic types and functions.
 * Defining FOAM_RTS gives only those needed for store.c and bigint.c.
 */

# include "axlgen0.h"

# define  OB_Other		0
# define  OB_Bogus		1

# define  OB_BInt		2
# define  OB_BTree		3

# define  OB_Bitv		4
# define  OB_Buffer		5
# define  OB_List		6
# define  OB_String		7
# define  OB_Symbol		8
# define  OB_Table		9
# define  OB_DNF		10

# define  OB_CCode		11
# define  OB_SExpr		12

# define  OB_AXLGEN_LIMIT	13

# include "bigint.h"
# include "btree.h"

#ifndef FOAM_RTS
# include "bitv.h"
# include "buffer.h"
# include "ccode.h"
# include "dnf.h"
# include "file.h"
# include "fname.h"
# include "list.h"
# include "msg.h"
# include "path.h"
# include "priq.h"
# include "sexpr.h"
# include "srcpos.h"
# include "strops.h"
# include "symbol.h"
# include "xfloat.h"
#else
# include "table.h"
# include "xfloat.h"
# include "bigint.h"
#endif


#endif /* !_AXLGEN_H_ */
