/*****************************************************************************
 *
 * simpl.h: Symbol Implementations.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _SIMPL_H
#define _SIMPL_H
#include "axlobs.h"


typedef enum {
	SIMPL_None,
	SIMPL_Unknown,
	SIMPL_Inherit,
	SIMPL_Cond,
	SIMPL_Branch,
	SIMPL_Default,
	SIMPL_Local,
	SIMPL_Import
} SImplTag;

/* Must optimise and make this structure immediate. */
typedef struct _SImplHdr {
	BPack(SImplTag) tag;
	BPack(UByte)	flags;
} SImplHdr;

typedef struct {
	SImplHdr hdr;
	union {
		AInt  number;
		void *ptr;
	} argv[NARY];
} SImplGen;

typedef struct {
	SImplHdr hdr;
} SImplNone;

typedef struct {
	SImplHdr hdr;
	AInt	 defineIdx;
	AInt	 constNum;
	Lib	 constLib;
} SImplLocal;

typedef struct {
	SImplHdr hdr;
	TForm	 base;
} SImplInherit;

typedef struct {
	SImplHdr hdr;
	AbLogic  cond;
	SImpl    impl;
} SImplCond;

typedef struct {
	SImplHdr hdr;
	Syme 	 def;
} SImplDefault;

typedef struct {
	SImplHdr hdr;
	AbLogic  cond;
	SImpl    implTrue;
	SImpl    implFalse;
} SImplBranch;

union _SImpl {
	SImplGen	implGen;
	SImplNone	implNone;
	SImplLocal	implLocal;
	SImplInherit	implInherit;
	SImplCond	implCond;
	SImplDefault	implDefault;
	SImplBranch	implBranch;
};


extern SImpl  	implNewNone	(void);
extern SImpl  	implNewLocal	(Bool, int);
extern SImpl  	implNewImport	(Bool, Lib, int);
extern SImpl	implNewInherit	(TForm);
extern SImpl    implNewCond     (AbLogic, SImpl);
extern SImpl    implNewDefault  (Syme);
extern SImpl    implNewBranch   (AbLogic, SImpl, SImpl);

#define implTag(x)		((x)->implGen.hdr.tag)

#define implIsNone(x) 		((x)->implGen.hdr.tag == SIMPL_None)
#define implIsLocal(x) 		((x)->implGen.hdr.tag == SIMPL_Local)
#define implIsImport(x) 	((x)->implGen.hdr.tag == SIMPL_Import)
#define implIsInherit(x) 	((x)->implGen.hdr.tag == SIMPL_Inherit)
#define implIsCond(x)	 	((x)->implGen.hdr.tag == SIMPL_Cond)
#define implIsDefault(x) 	((x)->implGen.hdr.tag == SIMPL_Default)
#define implIsBranch(x) 	((x)->implGen.hdr.tag == SIMPL_Branch)

#define implHasDefault(x)	((x)->implGen.hdr.flags & 1)
#define implSetDefault(x)	((x)->implGen.hdr.flags |= 1)

extern void	implFree	(SImpl);

extern void  implSetConstNum(SImpl impl, int defId, int constId);
extern int   implFindDefIdx(SImpl impl, int constId);

/* Used to remove non-inherited stuff before writing to a file */
extern void 	implCompress    (SImpl);
extern SImpl    implEvaluate	(SImpl, AbLogic);

extern int	implPrint	(FILE *, SImpl);
extern int	implPrintDb	(SImpl);

#endif
