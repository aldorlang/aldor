-------------------------- ald_pfunc.as ---------------------------------
--
-- Partial functions, i.e. functions that are allowed to fail on some inputs
--
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "aldor"

#if ALDOC
\thistype{PartialFunction}
\History{Manuel Bronstein}{23/6/95}{created}
\Usage{import from \this(R, S)}
\Params{
{\em R} & \builtin{Type} & Contains the domain of the function\\
{\em S} & \builtin{Type} & Contains the image of the function\\
}
\Descr{ \this~provides partial functions from R to S, \ie functions
from R to S which are allowed to fail on some elements of R.}
\begin{exports}
\alexp{apply}: & (\%, R) $\to$ S & Apply a partial function\\
\alexp{inDomain?}:
& (\%, R) $\to$ \altype{Boolean} & Check if an element is in the domain\\
\alexp{mapping}: & \% $\to$ (R $\to$ S) & Action of a function\\
\alexp{partialApply}:
& (\%, R) $\to$ \altype{Partial} S & Apply a partial function\\
\alexp{partialMapping}:
& \% $\to$ (R $\to$ \altype{Partial} S) & Action of a function\\
\alexp{predicate}:
& \% $\to$ (R $\to$ \altype{Boolean}) & Domain of a partial function\\
\alexp{partialFunction}:
& (R $\to$ \altype{Partial} S) $\to$ \% & Create a partial function\\
\alexp{partialFunction}: & ((R $\to$ \altype{Boolean}, R $\to$ S)) $\to$ \% &
Create a partial function\\
\end{exports}
#endif

PartialFunction(R:Type, S:Type): with {
	apply: (%, R) -> S;
#if ALDOC
\alpage{apply}
\Usage{ \name($\sigma$, x) \\ $\sigma x$}
\Signature{(\%, R)}{S}
\Params{
{\em $\sigma$} & \% & A partial function\\
{\em x} & R & An element of R\\
}
\Retval{Returns $\sigma x$.}
\Remarks{This map can cause an error if it is used on an element which is
not in the domain of $\sigma$. Use only when x is known to be in the domain,
otherwise use {\em partialApply}.}
\alseealso{\alexp{inDomain?}, \alexp{predicate}, \alexp{partialApply}}
#endif
	inDomain?: (%, R) -> Boolean;
#if ALDOC
\alpage{inDomain?}
\Usage{ \name~x }
\Signature{(\%, R)}{\altype{Boolean}}
\Params{
{\em $\sigma$} & \% & A partial function\\
{\em x} & R & An element of R\\
}
\Retval{Returns \true~if x is in the domain of $\sigma$, \false otherwise.}
\alseealso{\alexp{predicate}}
#endif
	mapping: % -> (R -> S);
#if ALDOC
\alpage{mapping}
\Usage{\name~$\sigma$}
\Signature{\%}{(R $\to$ S)}
\Params{ {\em $\sigma$} & \% & A partial function\\ }
\Retval{Returns the map corresponding to the action of $\sigma$ on R.}
\Remarks{The map returned can cause an error if it is used on an element which
is not in the domain of $\sigma$. Use only when x is known to be in the domain,
otherwise use {\em partialMapping}.}
\alseealso{\alexp{inDomain?}, \alexp{partialMapping}, \alexp{predicate}}
#endif
	partialMapping: % -> (R -> Partial S);
#if ALDOC
\alpage{partialMapping}
\Usage{\name~$\sigma$}
\Signature{\%}{(R $\to$ \altype{Partial} S)}
\Params{ {\em $\sigma$} & \% & A partial function\\ }
\Retval{Returns the map corresponding to the action of $\sigma$ on R.}
\alseealso{\alexp{inDomain?}, \alexp{mapping}, \alexp{predicate}}
#endif
	predicate: % -> (R -> Boolean);
#if ALDOC
\alpage{predicate}
\Usage{\name~$\sigma$}
\Signature{\%}{(R $\to$ \altype{Boolean}}
\Params{ {\em $\sigma$} & \% & A partial function\\ }
\Retval{Returns the predicate defining the domain of $\sigma$.}
\alseealso{\alexp{inDomain?}}
#endif
	partialFunction: (R -> Partial S) -> %;
	partialFunction: (R -> Boolean, R -> S) -> %;
#if ALDOC
\alpage{partialFunction}
\Usage{\name~f\\ \name(p, g)}
\Signatures{
\name: & (R $\to$ \altype{Partial} S) $\to$ \%\\
\name: & (R $\to$ \altype{Boolean}, R $\to$ S) $\to$ \%\\
}
\Params{
{\em f} & R $\to$ \altype{Partial} S & A partial map\\
{\em p} & R $\to$ \altype{Boolean} & A predicate\\
{\em g} & R $\to$ S & A map\\
}
\Retval{\name(f) returns the partial function $\sigma$ on $R$ given by
$$
\sigma x = f(x)
$$
for any $x \in R$, while \name(p, g) returns the partial function $\sigma$ on $R$ given by
$$
\sigma x = \left\{ \begin{array}{ll}
g(x) & if p(x) = \true\\
\failed & if p(x) = \false\\ \end{array} \right.
$$
}
#endif
	partialApply: (%, R) -> Partial S;
#if ALDOC
\alpage{partialApply}
\Usage{ \name~x }
\Signature{(\%, R)}{\altype{Partial} S}
\Params{
{\em $\sigma$} & \% & A partial function\\
{\em x} & R & An element of R\\
}
\Retval{Returns $\sigma x$, or \failed if x is not in the domain of $\sigma$.}
\alseealso{\alexp{apply}, \alexp{inDomain?}, \alexp{predicate}}
#endif
} == add {
	Rep == Record(pred: R->Boolean, func: R->S, pfunc: R->Partial S);
	import from Rep;

	mapping(f:%):(R -> S)			== rep(f).func;
	partialMapping(f:%):(R -> Partial S)	== rep(f).pfunc;
	predicate(f:%):(R -> Boolean)		== rep(f).pred;
	apply(f:%, r:R):S			== mapping(f)(r);
	partialApply(f:%, r:R):Partial S	== partialMapping(f)(r);
	inDomain?(f:%, r:R):Boolean		== predicate(f)(r);

	(u:%) = (v:%):Boolean == {
		import from Pointer;
		(u pretend Pointer) =$Pointer (v pretend Pointer);
	}

	partialFunction(f:R -> Partial S):% == {
		import from Partial S;
		per [(r:R):Boolean +-> ~failed? f r, (r:R):S +-> retract f r,f];
	}

	partialFunction(p:R -> Boolean, g:R -> S):% == {
		per [p, g, (r:R):Partial S +-> [g r]];
	}
}
