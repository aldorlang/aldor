
/*****************************************************************************
 *
 * genssa.c: Flog-to-SSA translation.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlphase.h"
#include "genssa.h"
#include "gf_util.h"
#include "fortran.h"
#include "compcfg.h"
#include "optfoam.h"

/*
 * SSA stands for Static Single Assignment. It is identical to standard
 * foam flogs except that every Loc has precisely one definition point.
 * This simplifies optimisations based on use-def analysis and allows us
 * to perform several optimisations in parallel. There are other advantages
 * that I'll not go into just now.
 */


/*****************************************************************************
 *
 * :: Macros
 *
 ****************************************************************************/

/* Simply to reduce the number of characters on a stoAlloc line ... */
#define SSA_ALLOC(n)	stoAlloc(OB_Other, n)

/*****************************************************************************
 *
 * :: Top level entry-points for SSA conversion.
 *
 ****************************************************************************/

SSA
ssaFromFlog(FlowGraph flog)
{
	BBlock		bb;
	AInt		root;
	Foam		ddecl = flog->prog->foamProg.locals;
	AInt		nlocs = foamDDeclArgc(ddecl);
	SSA		result = (SSA)SSA_ALLOC(sizeof(*result));
	AIntList	*stack = (AIntList *)SSA_ALLOC(nlocs*sizeof(*stack));
	AInt		*fmt = (AInt *)SSA_ALLOC(nlocs*sizeof(*fmt));
	AInt		*typ = (AInt *)SSA_ALLOC(nlocs*sizeof(*typ));

	/* Create the basic structure */
	result->flog = flog;
	result->usec = int0;
	result->uses = (FoamUses *)NULL;
	result->locals = vpNew(fboxNew(ddecl));

	/* Perform edge-splitting and dead-code removal */
	ssaEdgeSplit(result);

	/* Number every basic block (order is irrelevent) */
	flogIter(ssa->flog, bb, bbSetSExtra(bb, _i));
	root = ssa->root = bbSExtra(ssa->flog->block0);

	/* Compute immediate dominators */
	ssaDominatorTree(result, root);

	/* Compute the dominance frontiers */
	ssaDominanceFrontiers(result, root);

	/* Insert phi-functions */
	ssaInsertPhiFunctions(result);

	/* State for ssaRename */
	for (i = 0; i < nlocs; i++) {
		stack[i] = listSingleton(AInt)(i);
		fmt[i] = ddecl->foamDDecl.argv[i]->foamDecl.format;
		typ[i] = ddecl->foamDDecl.argv[i]->foamDecl.type;
	}

	/* Rename all identifiers to obtain a valid edge-split SSA */
	ssaRename(result, root, stack, fmt, typ);

	/* Clean up */
	for (i = 0; i < nlocs; i++) listFree(AInt)(stack[i]);
	stoFree(stack);
	stoFree(formats);

	/* FIXME: can we kill any other fields in ssa (e.g. samedom)? */

	/* Return the augmented flog */
	return (SSA)result;
}

FlowGraph
ssaToFlog(SSA ssa)
{
	AInt 		i;
	FlowGraph	result = ssa->flog;

	/* Remove all phi-functions */
	ssaRemovePhiFunctions(ssa);

	/* Clean up and return the updated flog */
	result->prog->foamProg.locals = fboxMake(ssa->locals->fbox);
	fuDestroy(ssa->uses);
	stoFree(ssa->idom);
	stoFree(ssa->dfnum);
	stoFree(ssa->semi);
	stoFree(ssa->best);
	stoFree(ssa->ancestor);
	stoFree(ssa->samedom);
	stoFree(ssa->vertex);
	stoFree(ssa->parent);
	for (i = 0; i < ssa->blockc; i++) {
		listFree(AInt)(ssa->dfrontier[i]);
		listFree(AInt)(ssa->dtree[i]);
	}
	stoFree(ssa);
	return result;
}

/*****************************************************************************
 *
 * :: Edge-splitting, dead-code removal, loop pre-headers etc
 *
 ****************************************************************************/

/*
 * To make the SSA analysis easier we require that every successor of a
 * block X with multiple exits has precisely one entry (from X). We also
 * require that the predecessor of every block X with multiple entries
 * has precisely one exit (to X). Ensuring one of these properties will
 * ensure the other.
 */
void
ssaEdgeSplit(SSA ssa)
{
	BBlockList	todo = listNil(BBlock);

	/* Walk the flog from the start node */
	todo = listCons(BBlock)(ssa->flog->block0, todo);
	flogClearMarks(ssa->flog);
	while (todo) {
		/* Remove the next node from the work list */
		BBlock	bb = car(todo);
		todo = cdr(todo);
		if (bb->mark) continue; /* Already processed */
		bb->mark = true;

		/* We only process nodes with multiple successors */
		if (!bbExitC(bb)) continue;
		if (bbExitC(bb) == 1) {
			todo = listCons(BBlock)(bbExit(bb, 0), todo);
			continue;
		}

		/* Check every successor for multiple predecessors */
		for (i = 0; i < bbExitC(bb); i++) {
			AInt		j;
			Foam		code;
			BlockLabel	nlab;
			BBlock		new, succ = bbExit(bb, i);

			/* Always add children to the to-do list */
			todo = listCons(BBlock)(succ, todo);

			/* Ignore successors without multiple predecessors */
			if (bbEntryC(succ) < 2) continue;

			/* Edge-split: create empty node and attach to flog */
			nlab = flogReserveLabel(ssa->flog);
			code = foamNewEmpty(FOAM_Seq, 1);
			code->foamSeq.argv[0] = foamNewNOp();
			new = bbNew(code, nlab);
			flogSetBlock(ssa->flog, nlab, new);
			/* Don't add the new node to the to-do list! */

			/* The only entry to new block is bb */
			bbufNeed(new->entries, 1);
			bbSetEntryC(new, 1);
			bbSetEntry(new, 0, bb);

			/* The only exit is to succ */
			bbufNeed(new->exits, 1);
			bbSetExitC(new, 1);
			bbSetExit(new, 0, succ);

			/* Fix the exit from bb */
			bbSetExit(bb, i, new);

			/* Fix the entry to succ */
			for (j = 0; j < bbEntryC(succ); j++) {
				/* Search for bb */
				if (bbEntry(succ, j) != bb) continue;
				bbSetEntry(succ, j, new);
			}
		}
	}

	/* Remove dead blocks */
	(void)flogCollect(ssa->flog);
}

/*****************************************************************************
 *
 * :: Computation of immediate dominators
 *
 ****************************************************************************/

/*
 * ssaDFS(ssa, parent, node, n) recursively updates the dfnum, vertex and
 * parent tables for "node" whose parent is "parent" starting with number n.
 *
 */
local AInt
ssaDFS(SSA ssa, AInt parent, AInt node, AInt num)
{
	AInt i;
	BBlock bb;

	/* If we have already numbered this node then we stop recursing */
	if (ssa->dfnum[node]) return num;

	/* Store the df number of this node and update the relevent tables */
	ssa->dfnum[node] = num;
	ssa->vertex[num++] = node;
	ssa->parent[node] = parent;

	/* Recurse over all successors of this node */
	bb = flogBlock(ssa->flog, node);
	for (i = 0; i < bbExitC(bb); i++)
		num = ssaDFS(ssa, node, bbSExtra(bbExit(bb, i)), num);
	return num;
}

local AInt
ssaLowestSemiAncestor(SSA ssa, AInt v)
{
	AInt	a = ssa->ancestor[v];
	if (ssa->ancestor[a] != -1) {
		/* Recurse */
		AInt b = ssaLowestSemiAncestor(ssa, a);
		AInt	*dfnum = ssa->dfnum;
		AInt	*semi = ssa->semi;
		ancestor[v] = ancestor[a];
		if (dfnum[semi[b]] < dfnum[semi[ssa->best[v]]])
			ssa->best[v] = b;
	}
	return best[v];
}

/*
 * ssaDominatorTree(ssa, root) computes the immediate dominator tables
 * for ssa whose root is root. Assumes that ssa has at least one bb.
 */
void
ssaDominatorTree(SSA ssa, AInt root)
{
	BBlock		bb;
	AIntList	bl;
	AInt		i, N;
	AInt		sz = flogBlockC(ssa->flog);

	/* Allocate the buckets */
	ssa->blockc = sz;
	ssa->bucketv = (AIntList *)SSA_ALLOC(sz*sizeof(*(ssa->bucketv)));
	ssa->dfrontier = (AIntList *)SSA_ALLOC(sz*sizeof(*(ssa->dfrontier)));
	ssa->dtree = (AIntList *)SSA_ALLOC(sz*sizeof(*(ssa->dtree)));
	for (i = 0; i < sz; i++) {
		bucketv[i] = listNil(AInt);
		dfrontier[i] = listNil(AInt);
		dtree[i] = listNil(AInt);
	}

	/* Allocate and initialise depth-first numbers and other tables */
	ssa->dfnum = (AInt *)SSA_ALLOC(sz*sizeof(*(ssa->dfnum)));
	ssa->idom = (AInt *)SSA_ALLOC(sz*sizeof(*(ssa->idom)));
	ssa->semi = (AInt *)SSA_ALLOC(sz*sizeof(*(ssa->semi)));
	ssa->ancestor = (AInt *)SSA_ALLOC(sz*sizeof(*(ssa->ancestor)));
	ssa->samedom = (AInt *)SSA_ALLOC(sz*sizeof(*(ssa->samedom)));
	ssa->vertex = (AInt *)SSA_ALLOC(sz*sizeof(*(ssa->vertex)));
	ssa->parent = (AInt *)SSA_ALLOC(sz*sizeof(*(ssa->parent)));
	ssa->best = (AInt *)SSA_ALLOC(sz*sizeof(*(ssa->best)));
	for (i = 0; i < sz; i++) {
		ssa->dfnum[i] = 0;
		ssa->best[i] = root;
		ssa->semi[i] = ssa->ancestor[i] = -1; /* -1 => none */
		ssa->samedom[i] = -1; /* -1 => none */
	}

	/* Compute depth-first numbers, parents and vertices */
	N = ssaDFS(ssa, -1, root, 0);

	/* Process every node except the root */
	for (i = N - 1; i; i--) {
		AInt	j;
		AInt	n = ssa->vertex[i];
		AInt	p = ssa->parent[i];
		AInt	s = parent;
		AInt	sprime;

		/* Calculate the semidominator of node (ssa->semi[node]) */
		bb = flogBlock(ssa->flog, n);
		for (j = 0; j < bbEntryC(bb); j++) {
			BBlock	vbb = bbEntry(bb, j);
			AInt	v = bbSExtra(vbb);

			/* Compute the next candidate for semi[n] */
			if (ssa->dfnum[v] <= ssa->dfnum[n])
				sprime = v;
			else
				sprime = semi[ssaLowestSemiAncestor(ssa, v)];
			if (dfnum[sprime] < dfnum[s]) s = sprime;
		}

		/* Defer calculation of idom[n] until processed path s->n */
		ssa->semi[n] = s;
		ssa->bucketv[s] = listCons(AInt)(n, ssa->bucketv[s]);

		/* Add edge parent->node to the forest (processed paths) */
		ssa->ancestor[n] = p;
		ssa->best[n] = n;

		/* Compute the idom[v] if possible */
		for (bl = ssa->bucketv[p]; bl; bl = listFreeCons(AInt)(bl)) {
			AInt	v = car(bl);
			AInt	y = ssaLowestSemiAncestor(ssa, v);

			if (ssa->semi[y] == semi[v])
				ssa->idom[v] = p;
			else
				ssa->samedom[v] = y;
		}
		ssa->bucketv[p] = bl;
	}

	/* Finish the deferred calculations */
	for (i = 1; i <= N; i++) {
		AInt	n = ssa->vertex[i];
		if (ssa->samedom[n] != -1)
			ssa->idom[n] = ssa->idom[ssa->samedom[n]];
	}

	/* Release unused storage */
	stoFree(ssa->bucketv);

	/* Compute the dominator tree */
	for (i = 0; i < ssa->blockc; i++) {
		AInt dom = ssa->idom[n];
		if (dom == -1) continue;
		/* Add ourselves to the child list of our idom */
		ssa->dtree[dom] = listCons(AInt)(n, ssa->dtree[dom]);
	}
}

/*****************************************************************************
 *
 * :: Computation of dominance frontiers
 *
 ****************************************************************************/

local Bool
ssaDominates(SSA ssa, AInt n1, AInt n2)
{
	AInt	*idom = ssa->idom;
	AInt	n = n2;
	AInt	in = idom[n];

	/* Each node dominates itsef */
	if (n1 == n2) return true;

	/* n1 dominates n2 iff n1 lies above n2 in the dominator tree */
	while (in != -1) {
		n = in;
		if (n == n1) return true;
		in = idom[in];
	}
	return false;
}

void
ssaDominanceFrontiers(SSA ssa, AInt n)
{
	AInt		i;
	BBlock		bb;
	AIntList	l1, l2;
	AIntList	S = listNil(AInt);

	/* Compute DFlocal[n] */
	bb = flogBlock(ssa->flog, n);
	for (i = 0; i < bbExitC(bb); i++) {
		BBlock	ybb = bbExit(bb, i);
		AInt	y = bbSExtra(ybb);

		/* Add successors of n not strictly dominated by n */
		if (ssa->idom[y] != n) S = listCons(AInt)(y, S);
	}

	/* Compute DFup[n] */
	for (l1 = ssa->dtree[n]; l1; l1 = cdr(l1)) {
		AInt	c = car(l1);
		ssaDominanceFrontiers(ssa, c);
		for (l2 = ssa->dfrontier[c]; l2; l2 = cdr(l2)) {
			AInt	w = car(l2);

			/* Add nodes not dominated by n */
			if (!ssaDominates(n, w)) S = listCons(AInt)(w, S);
		}
	}

	/* Store the dominance frontier */
	ssa->dfrontier[n] = S;
}

/*****************************************************************************
 *
 * :: Insertion of phi functions and identifier renaming
 *
 ****************************************************************************/

local void
ssaPhiScanBB(BBlock bb, BitvClass class, AIntList defs, Bitv *Aorig)
{
	/* Access the code for the bblock */
	AInt	fc = foamArgc(bb->code);
	Foam	*fv = bb->code->foamSeq.argv;

	/* Scan for definitions */
	for (; fc; fv++, fc--) {
		FoamTag tag = foamTag(*fv);
		if ((tag == FOAM_Def) || (tag == FOAM_Set)) {
			/* Assume foamDef and foamSet are identical */
			Foam lhs = (*fv)->foamSet.lhs;

			/* Check the lhs for locals */
			if (foamTag(lhs) == FOAM_Loc) {
				AInt i = lhs->foamLoc.index;
				/* Add to Aorig[n] and defsites[i] */
				if (!bitvTest(class, Aorig[n], i)) {
					/* Not seen here: add to tables */
					bitvSet(class, Aorig[n], i);
					defs[i] = listCons(AInt)(n, defs[i]);
				}
				continue;
			}

			/* If it isn't a multi, ignore it */
			if (foamTag(lhs) != FOAM_Values) continue;

			/* Scan for locals */
			for (v = 0; v < foamArgc(lhs); v++) {
				AInt i;
				FoamTag tag = foamTag(foamArgv(lhs)[v]);

				/* Ignore non-locals */
				if (tag != FOAM_Loc) continue;
				i = lhs->foamLoc.index;

				/* Add to Aorig[n] and defsites[i] */
				if (!bitvTest(class, Aorig[n], i)) {
					/* Not seen here: add to tables */
					bitvSet(class, Aorig[n], i);
					defs[i] = listCons(AInt)(n, defs[i]);
				}
			}
		}
	}
}

local Foam
ssaNewPhiFunction(AInt nargs, AInt loc)
{
	AInt	i;
	Foam	result = foamNewEmpty(FOAM_BCall, nargs + 1);

	/* Fill in the bcall operator */
	result->foamBCall.op = FOAM_BVal_ssaPhi;

	/* Fill in the arguments */
	for (i = 0; i < nargs; i++)
		result->foamBCall.argv[i] = foamNewLoc(loc);

	/* Return (Set (Loc loc) (BCall ssaPhi (Loc loc) ...)) */
	return foamNewSet(foamNewLoc(loc), result);
}

void
ssaInsertPhiFunctions(SSA ssa)
{
	AInt		i;
	BitvClass	bclass;
	AIntList	*defsites;
	Foam		ddecl = ssa->flog->prog->foamProg.locals;
	AInt		nlocs = foamDDeclArgc(ddecl);
	Bitv		*Aorig, *Aphi;
	FoamList	*phis;

	/* Table of definition sites indexed by local */
	defsites = (AIntList *)SSA_ALLOC(nlocs*sizeof(*defsites));
	for (i = 0; i < nlocs; i++) defsites[i] = listNil(AInt);

	/* List of phi functions to insert index by node */

	/*
	 * Set membership bits index by node; phi functions to add
	 * indexed by node.
	 */
	Aorig = (Bitv *)SSA_ALLOC(ssa->blockc*sizeof(*Aorig));
	Aphi = (Bitv *)SSA_ALLOC(ssa->blockc*sizeof(*Aphi));
	phis = (FoamList *)SSA_ALLOC(ssa->blockc*sizeof(*stmts));
	bclass = bitvClassCreate(nlocs);
	for (i = 0; i < ssa->blockc; i++) {
		Aorig[i] = bitvNew(bclass);
		Aphi[i] = bitvNew(bclass);
		bitvClearAll(bclass, Aorig[i]);
		bitvClearAll(bclass, Aphi[i]);
		phis[i] = listNil(Foam);
	}

	/* Compute defsites[] and Aorig[] */
	for (n = 0; n < ssa->blockc; n++) {
		BBlock	bb = flogBlock(ssa->flog, n);
		if (!bb) continue; /* Not all bb slots are used */

		/* Scan bblock for definitions of locals */
		ssaPhiScanBB(bb, bclass, defsites, Aorig);
	}

	/*
	 * Insert phi-functions. FIXME: avoid creating unary phi
	 * functions: (Set (Loc i) (BCall ssaPhi (Loc i))).
	 */
	for (i = 0; i < nlocs; i++) {
		/* Start by processing all nodes in which (Loc i) is set */
		AIntList todo = defsites[i];
		while (todo) {
			/* Remove the next element from the to-do list */
			AInt		n = car(todo);
			AIntList	df = ssa->dfrontier[n];
			todo = cdr(todo);

			/* Check each node in the dominance frontier of n */
			listIter(AInt, y, df, {
				if (!bitvTest(bclass, Aphi[n], y)) {
					/* Phi function needed */
					BBlock	bb = flogBlock(ssa->flog, y);
					AInt ec = bbEntryC(bb);
					Foam fm = ssaNewPhiFunction(ec, i);
					phis[y] = listCons(Foam)(fm, phis[y]);

					/* Record this phi function */
					bitvSet(class, Aphi[n], y);

					/* Preserve the dominance frontier */
					if (!bitvTest(bclass, Aorig[n], y))
						todo = listCons(AInt)(y, todo);
				}
			});
		}
	}

	/* Actually insert the phi functions */
	for (i = 0; i < ssa->blockc; i++) {
		Foam code;
		BBlock bb;
		FoamList stmts = phis[i];

		/* Skip blocks with nothing to add */
		if (!stmts) continue;

		/* Append every statement in the block to our list */
		bb = flogBlock(ssa->flog, i);
		code = bb->code;
		foamIter(code, stmtp, {
			stmts = listCons(Foam)(*stmtp, stmts);
			/* We want to foamFree(code) afterwards */
			*stmtp = (Foam)NULL;
		});

		/* Replace the bblock code */
		stmts = listNReverse(Foam)(stmts);
		bb->code = foamNewOfList(FOAM_Seq, stmts);

		/* Cleanup */
		foamFree(code);
		listFree(Foam)(stmts);
	}

	/* Clean up */
	for (i = 0; i < ssa->blockc; i++) {
		bitvFree(Aorig[i]);
		bitvFree(Aphi[i]);
	}
	bitvClassDestroy(bclass);
}

/*****************************************************************************
 *
 * :: Variable renaming
 *
 ****************************************************************************/

/*
 * Rename all identifier uses in a piece of FOAM that isn't a definition.
 */
local Foam
ssaRenameDeeply(Foam foam, AIntList *stack)
{
	if (foamTag(foam) == FOAM_Loc) {
		/* Rename this local */
		AInt index = foam->foamLoc.index;
		AInt nindex = car(stack[index]);
		return (nindex == index) ? foam : foamNewLoc(nindex);
	}

	/* Recursively process the expression */
	foamIter(foam, stmtp, {
		Foam	new = ssaRenameDeeply(*stmtp, stack);
		if (new != *stmtp) {
			foamFree(*stmtp);
			*stmtp = new;
		}
	});

	/* Foam returned unchanged */
	return foam;
}

local AInt
ssaRenameDef(SSA ssa, Foam def, AIntList *stack, AIntList *done, AInt *fmt,
		AInt *typ)
{
	Foam	new;
	Foam	lhs = def->foamDef.lhs;
	Foam	rhs = def->foamDef.rhs;
	VarPool locs = ssa->locals;

	/* Process phi functions: (Set (Loc n) (BCall ssaPhi (Loc n) ...)) */
	if (foamIsPhiFunction(rhs)) {
		/* The lhs MUST be a simple local: clone it */
		AInt	index = lhs->foamLoc.index;
		AInt	nindex = vpNewVar0(locs, typ[index], fmt[index]);

		/* This is now the closest/reaching definition */
		stack[index] = listCons(AInt)(nindex, stack[index]);
		*did = listCons(AInt)(index, *did);

		/* Update the definition */
		foamFree(lhs);
		def->foamDef.lhs = foamNewLoc(nindex);

		/* Do NOT process the phi function just yet */
		return;
	}

	/* Process the RHS first */
	new = ssaRenameDeeply(rhs, stack);
	if (new != rhs) {
		foamFree(rhs);
		def->foamDef.rhs = new;
	}

	/* Deal with simple assignments and definitions */
	if (foamTag(lhs) == FOAM_Loc) {
		/* The lhs is a simple local: clone it */
		AInt	index = lhs->foamLoc.index;
		AInt	nindex = vpNewVar0(locs, typ[index], fmt[index]);

		/* This is now the closest/reaching definition */
		stack[index] = listCons(AInt)(nindex, stack[index]);
		*did = listCons(AInt)(index, *did);

		/* Update the definition */
		foamFree(lhs);
		def->foamDef.lhs = foamNewLoc(nindex);
		return;
	}

	/* Deal with multis */
	if (foamTag(lhs) == FOAM_Values) {
		AInt	v, index, nindex;

		/* Rename any locals */
		for (v = 0; v < foamArgc(lhs); v++) {
			Foam loc = foamArgv(lhs)[v];
			/* Ignore non-locals */
			if (foamTag(loc) != FOAM_Loc) continue;

			/* Make a new local */
			index = loc->foamLoc.index;
			nindex = vpNewVar0(locs, typ[index], fmt[index]);

			/* This is now the closest/reaching definition */
			stack[index] = listCons(AInt)(nindex, stack[index]);
			*did = listCons(AInt)(index, *did);

			/* Update the definition */
			foamFree(loc);
			foamArgv(lhs)[v] = foamNewLoc(nindex);
		}
	}
}

/*
 * Having inserted phi functions we must now rename identifiers to ensure
 * that there is precisely on definition of each.
 */
void
ssaRename(SSA ssa, AInt n, AIntList *stack, AInt *fmt, AInt *typ)
{
	AInt		i;
	AIntList	nodes, done = listNil(AInt);
	BBlock		bb = flogBlock(ssa->flog, n);

	/* Examine every statement in the block */
	foamIter(bb->code, stmtp, {
		FoamTag tag = foamTag(*stmtp);
		if ((tag == FOAM_Def) || (tag == FOAM_Set))
			ssaRenameDef(ssa, *stmtp, stack, &done, fmt, typ);
		else {
			Foam	new = ssaRenameDeeply(*stmtp, stack);
			if (new != *stmtp) {
				foamFree(*stmtp);
				*stmtp = new;
			}
		}
	});

	/* Rename arguments to phi functions in our immediate successors */
	if (bbExitC(bb) > 1) {
		/*
		 * Edge splitting ensures that we are the unique
		 * predecessor of each of our successors.
		 */
		for (i = 0; i < bbExitC(bb); i++) {
			BBlock	succ = bbExit(bb, i);

			/*
			 * Rename the first argument to every phi function.
			 * If we can avoid creating unary phi functions then
			 * we can remove this code. We assume that all phi
			 * functions are grouped at the start of the block.
			 */
			foamIter(succ->code, stmtp, {
				Foam	rhs;
				FoamTag	tag = foamTag(*stmtp);
				/* Stop if this isn't a phi function */
				if ((tag != FOAM_Set) && (tag != FOAM_Def))
					break;
				rhs = (*stmtp)->foamDef.rhs);
				if (!foamIsPhiFunction(rhs) break;

				/* Rename the first argument */
				index = rhs->foamBCall.argv[0]->foamLoc.index;
				nindex = car(stack[index]);
				foamFree(rhs->foamBCall.argv[0]);
				rhs->foamBCall.argv[0] = foamNewLoc(nindex);
			});
		}
	
	}
	else if (bbExitC(bb)) {
		AInt	j;
		BBlock	succ = bbExit(bb, 0);

		/* Search for bb as entry to succ */
		for (j = 0; j < bbEntryC(succ); j++) {
			if (bbEntry(succ, j) != bb) continue;

			/* Rename the jth argument of every phi function.
			 * We assume that all phi functions are grouped at
			 * the start of the block.
			 */
			foamIter(succ->code, stmtp, {
				Foam	rhs;
				FoamTag	tag = foamTag(*stmtp);
				/* Stop if this isn't a phi function */
				if ((tag != FOAM_Set) && (tag != FOAM_Def))
					break;
				rhs = (*stmtp)->foamDef.rhs);
				if (!foamIsPhiFunction(rhs) break;

				/* Rename the first argument */
				index = rhs->foamBCall.argv[j]->foamLoc.index;
				nindex = car(stack[index]);
				foamFree(rhs->foamBCall.argv[j]);
				rhs->foamBCall.argv[j] = foamNewLoc(nindex);
			});

			/* Our bb cannot be another entry to succ so stop */
			break;
		}
		
	}

	/* We are walking the dominator tree */
	for (nodes = ssa->dtree[n]; nodes; nodes = cdr(nodes)) {
		AInt	x = car(nodes);
		ssaRename(ssa, x, stack, fmts);
	}

	/* Go back to the previous reaching definitions */
	for (; done; done = listFreeCons(AInt)(done)) {
		AInt	locno = car(done);
		stack[locno] = cdr(stack[locno]);
	}
}

/*****************************************************************************
 *
 * :: FoamUses functions
 *
 ****************************************************************************/

FoamUses
fuNew(Foam *foamp)
{
	FoamUses result = (FoamUses)SSA_ALLOC(sizeof(*result));
	fuData(result) = foamp;
	fuPrevious(result) = fuNext(result) = fuEmpty();
	return result;
}

void
fuDestroy(FoamUses fu)
{
	FoamUses p = fuNext(fu);
	/* Destroy everything after fu */
	while (p) {
		FoamUses next = fuNext(p);
		fuFree(p);
		p = next;
	}
	/* Destroy everything before fu */
	p = fuPrevious(fu);
	while (p) {
		FoamUses prev = fuPrevious(p);
		fuFree(p);
		p = prev;
	}
	/* Destroy fu and return */
	fuFree(fu);
}

FoamUses
fuInsertBefore(FoamUses fu, Foam *foamp)
{
	FoamUses result = fuNew(foamp);
	if (fu) {
		fuPrevious(result) = fuPrevious(fu);
		fuNext(result) = fu;
		if (fuPrevious(fu)) fuNext(fuPrevious(fu)) = result;
		fuPrevious(fu) = result;
	}
	return result;
}

FoamUses
fuInsertAfter(FoamUses fu, Foam *foamp)
{
	FoamUses result = fuNew(foamp);
	if (fu) {
		fuNext(result) = fuNext(fu);
		fuPrevious(result) = fu;
		if (fuNext(fu)) fuPrevious(fuNext(fu)) = result;
		fuNext(fu) = result;
	}
	return result;
}

