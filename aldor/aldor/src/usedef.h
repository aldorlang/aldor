/*****************************************************************************
 *
 * usedef.h: Usage definition chains
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _USEDEF_H_
#define _USEDEF_H_

#include "axlobs.h"

# define	udReachingDefs(foam)	((foam)->foamGen.hdr.info.defList)
# define	udInfoDef(udinfo)	((udinfo)->foam)
# define	udInfoBlock(udinfo)	((udinfo)->block)


/****************************************************************************
 *
 * :: Type Definitions
 *
 ****************************************************************************/

struct  _UdInfo {
	Foam		foam;	/* definition */
	BBlock		block;  /* block containing the definition */

};

typedef enum { 
	UD_OUTPUT_UdList,		
	UD_OUTPUT_SinglePointer
} UdOutputKind;

/****************************************************************************
 *
 * :: External entry points
 *
 ****************************************************************************/

extern void	usedefChainsFreeFrProg	(Foam);

extern Bool    	usedefChainsFrFlog	(FlowGraph, UdOutputKind);
extern void	usedefChainsFreeFrFlog	(FlowGraph);
extern void	udSetFlogCutOff		(int);

#endif /* _USEDEF_H_ */

