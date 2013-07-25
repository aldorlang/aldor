/*****************************************************************************
 *
 * gf_implicit.h: Foam code generation for implicit exports
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/
 
#ifndef _GF_IMPLICIT_H_
#define _GF_IMPLICIT_H_

#include "genfoam.h"
#include "gf_util.h"
#include "gf_prog.h"
#include "gf_add.h"
#include "gf_gener.h"
#include "gf_excpt.h"
#include "gf_fortran.h"
#include "gf_imps.h"
#include "gf_rtime.h"
#include "opttools.h"
#include "of_inlin.h"
#include "of_util.h"
#include "optfoam.h"
#include "simpl.h"
#include "scobind.h"
#include "fortran.h"
#include "compcfg.h"
#include "tform.h"
#include "comsgdb.h"

/*****************************************************************************
 *
 * :: Exports
 *
 ****************************************************************************/

extern void	gen0ImplicitExport		(Syme, SymeList, AbSyn);


/*****************************************************************************
 * 
 * :: Local state and information tables 
 *
 ****************************************************************************/

enum gfImplicitTag {
    GFI_START,

	GFI_PackedArrayNew = GFI_START,
	GFI_PackedArrayGet,
	GFI_PackedArraySet,
	GFI_PackedRecordSet,
	GFI_PackedRecordGet,
	GFI_PackedRepSize,

    GFI_LIMIT
};

typedef Enum(gfImplicitTag)	GfImplicitTag;

struct gf_impl_info {
	GfImplicitTag	tag;
        String		name;
        Hash		type;
};

extern Length gfImplicitInfoTableC;
extern struct gf_impl_info gfImplicitInfoTable[];

#endif /*!_GF_IMPLICIT_H_*/
