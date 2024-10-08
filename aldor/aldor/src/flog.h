/*****************************************************************************
 *
 * flog.h: Foam flow-graph.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _FLOG_H_
#define _FLOG_H_

#include "axlobs.h"
#include "bitv.h"

/*
 * A flow graph represents a program as a directed graph with
 * basic blocks as nodes.  
 *
 * Each basic block represents a code sequence with one entry point
 * and one (possibly multiway) exit.  
 *
 * The basic blocks of the flow graph are stored in a BlockBuf.
 * If the graph contains a node with label n, then this will be stored in
 * the n-th slot of the BlockBuf.  The other slots will contain NULL.
 *
 * Foam label statments are not saved in the code sequences.
 * The last statment in each sequence is one of [If, Select, Goto, Return].
 *
 * The meaning of the "exits" BlockBuf is positional:
 *     If:     exits = [false case , true case]
 *     Select: exits = [0 case, 1 case, ..., exitc-1 case]
 *     Goto:   exits = [destination]
 *     Return: exits = [ ]
 *
 * While foam is in flow graph form, the foam labels can be out date.
 * This includes the maxLabel of the prog and the destination labels of the
 * If and Select instructions.
 * This information is reconstituted on conversion back into linear form.
 * The "label" field of a basic block, however, must always be up to date.
 *
 * The "entries" BlockBuf is used for printing only.  Graph operations
 * do not keep it current so it is recomputed by functions which need it.
 *
 * The "refc" and "lrefc" fields are used to convert to/from Foam
 * and to collect unused blocks.  They, also, are not kept current by graph
 * operations and so are recomputed by functions which need them.
 */

typedef enum { FLOG_UniqueExit,
	       FLOG_MultipleExits } FlogType;

/*
 * Declarations local to flowgraphs.
 */
typedef struct blockBuf	*BlockBuf;
typedef int		BlockLabel;

DECLARE_LIST(BBlock);

/*
 * Exported types.
 */
struct flowGraph {
	Foam		prog;		/* maxLabel + body not current.      */
	BBlock		block0;		/* Basic block of the entry pt.      */
	BlockBuf	blocks;		/* Labels used in blocks.            */

	Bool		dfIsPerExit;	/* Is dataflow per exit or block?    */
	Bool		fixedEntries;	/* true->entries OK on each block  */
	Pointer		pextra;		/* Meaning depends on problem.	     */
	BitvClass	bitvClass;	/* bitvClass for IN, OUT, GEN, KILL  */
};

struct basicBlock {
	FlowGraph	graph;		/* Containing graph.		     */

	BPack(FoamTag)	kind;		/* If, Select, Goto, Return.         */
	BPack(Bool)	mark;		/* Used for traversals.		     */
	BPack(Bool)	isblock0;	/* Is this the entry point?          */

	BlockLabel	label;		/* Label of seq.                     */
	Foam		code;		/* Seq with no intermediate exits.   */

	int		refc;		/* #entries.      		     */
	int		lrefc;		/* #entries needing labels.          */

	BlockBuf	entries;	/* Blocks coming here (for printing).*/
	BlockBuf	exits;		/* Blocks we go to.		     */

	DFlowInfo	dfinfo;		/* Dataflow info. Depends on problem.*/
	int		iextra;		/* Meaning depends on problem.       */
	Pointer		pextra;		/* Meaning depends on problem.       */
#if EDIT_1_0_n1_AB
	AInt		sextra;		/* Used for SSA bb numbering         */
#endif
};

struct blockBuf {
	UShort		argc;	
	UShort		pos;		/* pos = next unused. */
	BBlock	*argv;
};


/****************************************************************************
 *
 * :: FlowGraph operations
 *
 ****************************************************************************/

/*
 * Basic operations.
 */
extern FlowGraph  flogFrProg		(Foam, FlogType);
extern Foam	  flogToProg		(FlowGraph);

extern FlowGraph  flogFrSeq		(Foam, FlogType);
extern Foam	  flogToSeq		(FlowGraph, int *);

extern void	  flogReuse		(FlowGraph, FlogType);

extern void 	  flogBlockInsertBtwn	(FlowGraph, BBlock, BBlock, BBlock);

extern int	  flogPrint	 	(FILE *, FlowGraph, Bool);
extern int	  flogPrintDb	 	(FlowGraph);
extern void	  flogDumpToFile 	(FlowGraph, String);
extern int	  flogCollect           (FlowGraph);
extern void	  flogClearMarks	(FlowGraph);
extern void       flogFixLabels         (FlowGraph);
extern void       flogFixEntries        (FlowGraph);
extern void       flogFixRefCounts      (FlowGraph);
#define		  flogCompress(fg)	bbufCompress((fg)->blocks)
			/*
			 * flogCollect frees the unused blocks.
			 * flogCompress squeezes out the NULL blocks.
			 */
/*
 * Block accessors.
 */
#define		  flogBlockC(fg) 	((fg)->blocks->pos)
#define		  flogBlock(fg,i)	bbufBlock((fg)->blocks,i)

/*
 * Block setting: First reserve a label, later set the block.
 */
#define		  flogReserveLabel(fg)	bbufReserve1((fg)->blocks)
#define		  flogSetBlock(fg,i,bb)	((fg)->fixedEntries = false, \
					 bbufSetBlock((fg)->blocks,i,bb), \
					 (bb)->graph = (fg))
#define		  flogKillBlock(fg,i)	((fg)->blocks->argv[i] = 0)

#define		  flogBitvClass(fg)	((fg)->bitvClass)

#define	 flogIter(FLOG, BB, ACTION) 			\
Statement({						\
	   int _i;					\
           BBlock BB;				\
							\
	   for (_i = 0; _i < flogBlockC(FLOG); _i++) {	\
	   	BB = flogBlock(FLOG, _i);		\
		if (!BB) continue;			\
		Statement(ACTION);			\
	}						\
   })

/****************************************************************************
 *
 * :: BBlock operations
 *
 ****************************************************************************/

extern BBlock bbNew	 	 	(Foam, BlockLabel);
extern void	  bbFree		(BBlock);
extern int	  bbPrint	 	(FILE *, BBlock, Bool);

#define		  bbEntryC(bb)	 	((bb)->entries->pos)
#define           bbEntry(bb,i)  	((bb)->entries->argv[i])
#define		  bbSetEntry(bb,i,dd)	((bb)->graph->fixedEntries = false, \
					 **(bb)->entries->argv[i] = (dd))
#define		  bbSetEntryC(bb,n)	((bb)->entries->pos = (n))

#define		  bbExitC(bb)	 	((bb)->exits->pos)
#define           bbExit(bb,i)   	((bb)->exits->argv[i])
#define		  bbSetExit(bb,i,dd)	((bb)->graph->fixedEntries = false, \
					 (bb)->exits->argv[i] = (dd))
#define		  bbSetExitC(bb,n)	((bb)->exits->pos = (n))

#define		  bbSetIExtra(bb, info) ((bb)->iextra = (int) (info))
#define		  bbIExtra(bb)		((bb)->iextra)

#if EDIT_1_0_n1_AB
#define		  bbSetSExtra(bb, info) ((bb)->sextra = (AInt) (info))
#define		  bbSExtra(bb)		((bb)->sextra)
#endif

#define		  bbBitvClass(bb)	(flogBitvClass((bb)->graph))

extern Foam	  bbLastStat            (BBlock);
extern void	  bbLastValues    	(BBlock, Foam, Foam, Foam, Foam);
extern BBlock 	  bbCloneExitFrom       (BBlock, int exitno);
extern void	  bbSpecializeExit      (BBlock, int exitno);
extern BBlock 	  bbNConcat		(BBlock, BBlock);
extern BBlockList bbGetEntries		(BBlock);
extern BBlock     bbCopy                (BBlock);
extern Bool       bbIsIfStmt            (BBlock);

			/*
			 * bbNConcat modifies/frees its args.
			 */


/****************************************************************************
 *
 * :: BlockBuf operations (should not be used by other files)
 *
 ****************************************************************************/

extern BlockBuf	  bbufNew	 	(Length initCount);
extern void	  bbufFree	 	(BlockBuf);
#define		  bbufCount(bb)		((bb)->pos)

/*
 * bbufNeed ensures the buffer can hold the given number of blocks (in total).
 * bbufReserve1 reserves a new slot for the caller.
 * bbufAdd1 reserves a slot and fills it.
 * bbufCompress squeezes out the NULLs.
 */
extern void	  bbufNeed	 	(BlockBuf, Length);
extern Length     bbufReserve1   	(BlockBuf);
extern void	  bbufAdd1	 	(BlockBuf, BBlock);
extern void	  bbufCompress		(BlockBuf);

/*
 * These access and set blocks in a buffer.
 */
#ifdef NDEBUG
# define	  bbufBlock(bf,i)	((bf)->argv[i])
# define	  bbufSetBlock(bf,i,bb)	((bf)->argv[i]=(bb))
#else
# define	  bbufBlock(bf,i)	bbufBlockFn(bf,i)
# define	  bbufSetBlock(bf,i,bb)	bbufSetBlockFn(bf,i,bb)
#endif
extern BBlock bbufBlockFn		(BlockBuf, BlockLabel);
extern BBlock bbufSetBlockFn	(BlockBuf, BlockLabel, BBlock);

#endif /* !_FLOG_H_ */
