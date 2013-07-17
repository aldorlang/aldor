/*****************************************************************************
 *
 * depdag.c: Dependency DAGs
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "depdag.h"
#include "store.h"


/*****************************************************************************
 *                             Introduction
 *****************************************************************************
 *
 *   This is a somewhat naive method for representing and computing
 *   dependency graphs needed for generating code for constants in
 *   the correct order. In general such orderings are non-computable
 *   so we rely on a local approximation (dependencies of non-lazy
 *   imports cannot be computed in Aldor due to parameterised types).
 *
 *   Until a computable alternative can be found (e.g. by making all
 *   constants lazy), there is little incentive to replace this with
 *   a more sophisticated implementation.
 *
 *****************************************************************************
 *                               Overview
 *****************************************************************************
 *
 *   1) For each constant, compute the symbols its value depends on.
 *   2) Remove lazy constants, parameters and imports (and types?).
 *   3) Compute the dependency graph with each constant as children
 *      of a single root node and their dependencies as sub-trees.
 *   4) Cycles in the graph mean unsatisfiability (an error).
 *   5) Code generation is depth-first from the root.
 *
 *****************************************************************************
 *                               Details
 *****************************************************************************
 *
 *   + lazy constants, parameters and imports are, by definition,
 *     already initialised and can be removed from the graph.
 *   + non-lazy imports are sources of non-computability: it is not
 *     possible (in Aldor) to track dependencies of all imports.
 *   + cycles in the graph are reported as errors; the cycle is
 *     broken and analysis continues
 *   + when generating code for a node we recurse to generate code
 *     for each dependency first. Then code is generated for the
 *     node before marking it as finished. The recursive descent
 *     stops when leaf nodes or finished nodes are encountered and
 *     recursion unwinds until the next unfinished node is found.
 *
 *****************************************************************************
 *                              Example
 *****************************************************************************
 *
 *   Consider the small code fragment below:
 *
 *      Foo(n:SingleInteger):with { ... } == add
 *      {
 *         size:SingleInteger == retract(#$%);
 *         #:Integer          == n::Integer;
 *      }
 *
 *   The dependencies of each non-lazy constant are:
 *
 *      *ROOT*: {size, #}
 *      size:   {retract, #, %}
 *      #:      {coerce, n, Integer}
 *
 *   Removing lazy constants (retract, coerce), types (%, Integer),
 *   and parameters (n) we are left with the dependencies:
 *
 *      *ROOT*: {size, #}
 *      size:   {#}
 *      #:      {}
 *
 *   which generates the dependency graph below with no cycles:
 *
 *                        *ROOT*
 *                         /  \
 *                        /    \
 *                       /      \
 *                     size ---> #
 *
 *   Code generation starts at *ROOT* and recurses to size. This
 *   has one dependency (#) so we recurse and generate code for
 *   # (which has no dependencies). Walking back up the tree we
 *   have processed all dependencies of size so generate code to
 *   compute its value. Back to *ROOT* we descend to process the
 *   its second dependency (#). This has already been dealt with
 *   so we stop and return to *ROOT*. No more dependencies: done.
 *
 ****************************************************************************/

DepDag
depdagNewLeaf(void *lab)
{
	DepDag	dag;

	/* Allocate anonymous storage for this leaf */
	dag = (DepDag)stoAlloc(OB_Other, sizeof(*dag));


	/* Fill in the fields with default values */
	depdagSetLabel(dag, lab);
	depdagSetDependsOn(dag, listNil(DepDag));
	depdagSetPending(dag, false);
	depdagSetFinished(dag, false);


	/* Return the dag leaf */
	return dag;
}


DepDag
depdagAddDependency(DepDag dag, DepDag dep)
{
	DepDagList	dags = depdagDependsOn(dag);

	/* Check that it isn't a known dependency */
	if (!listMemq(DepDag)(dags, dep))
		depdagSetDependsOn(dag, listCons(DepDag)(dep, dags));


	/* Return the modified dag */
	return dag;
}


void
depdagFree(DepDag dag)
{
	stoFree((Pointer)dag);
}


void
depdagFreeDeeply(DepDag dag)
{
	DepDagList	dags = depdagDependsOn(dag);


	/* Free the children deeply */
	listIter(DepDag, d, dags, depdagFreeDeeply(d));


	/* Free this node */
	depdagFree(dag);
}


