------------------------------ sit_deriv.as ----------------------------------
--
-- Derivations
--
-- Copyright (c) Manuel Bronstein 1994
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994-97
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{Derivation}
\History{Manuel Bronstein}{21/12/94}{created}
\Usage{import from \this~R}
\Params{ {\em R} & \astype{Ring} & The ring on which the derivations operate\\ }
\Descr{\this~R provides derivations on $R$.}
\begin{exports}
\category{\astype{Module} R}\\
\asexp{apply}:
& (\%, R, \astype{Integer}) $\to$ R & Differentiate an element of $R$\\
\asexp{derivation}: & (R $\to$ R) $\to$ \% & Create a derivation\\
\asexp{function}: & \% $\to$ (R $\to$ R) & Action of a derivation\\
\end{exports}
#endif

Derivation(R:Ring): Module R with {
	apply: (%, R, n:Integer == 1) -> R;
#if ALDOC
\aspage{apply}
\Usage{
\name(D, x)\\
\name(D, x, n)\\
D~x\\
D(x,n)
}
\Signature{(\%, R, .:\astype{Integer} == 1)}{R}
\Params{
{\em D} & \% & A derivation\\ 
{\em x} & R & An element to differentiate\\
{\em n} & \astype{Integer} & The number of times to differentiate (optional)\\
}
\Retval{Returns $D^n x$, \ie~the result of applying $D$ to $x$ $n$ times.}
#endif
	derivation: (R -> R) -> %;
#if ALDOC
\aspage{derivation}
\Usage{\name~f}
\Signature{(R $\to$ R)}{\%}
\Params{ {\em f} & R $\to$ R & A map\\ }
\Retval{Returns the derivation $D$ on $R$ given by
$$
D x = f(x)
$$
for any $x \in R$.}
\Remarks{$f$ must satisfy the rules of a derivation, namely:
$$
f(x+y) = f(x) + f(y),\quad\mbox{and}\quad
f(xy) = x f(y) + f(x) y
$$
for any $x, y \in R$.}
#endif
	function: % -> (R -> R);
#if ALDOC
\aspage{function}
\Usage{\name~D}
\Signature{\%}{(R $\to$ R)}
\Params{ {\em D} & \% & A derivation\\ }
\Retval{Returns the map corresponding to the action of $D$ on the ring.}
#endif
} == add {
	macro Rep == R -> R;

	0			== per((r:R):R +-> 0);
	function(f:%):(R->R)	== rep f;
	-(f:%):%		== per((r:R):R +-> - (rep f) r);
	derivation(f:R -> R):%	== per f;
	(f:%) + (g:%):%		== per((r:R):R +-> (rep f)(r) + (rep g)(r));
	(u:%) = (v:%):Boolean	== { import from Pointer; address u = address v}
	(c:Integer) * (f:%):%	== { import from R; (c::R) * f }

	local address(u:%):Pointer == u pretend Pointer;

	(p:TextWriter) << (x:%):TextWriter == {
		import from Pointer;
		p << address x;
	}

	extree(u:%):ExpressionTree == {
		import from MachineInteger, Pointer;
		extree(address(u)::MachineInteger);
	}

	apply(f:%, r:R, n:Integer):R == {
		assert(n >= 0);
		g := function f;
		for i in 1..n repeat r := g r;
		r;
	}

	(c:R) * (f:%):% == {
		zero? c => 0;
		c = 1 => f;
		c = -1 => -f;
		per((r:R):R +-> c * (rep f) r);
	}
}
