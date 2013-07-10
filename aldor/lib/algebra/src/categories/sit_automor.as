----------------------------- sit_automor.as ------------------------------
-- Copyright (c) Manuel Bronstein 1994
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994-97
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{Automorphism}
\History{Manuel Bronstein}{4/11/94}{created}
\Usage{import from \this~R}
\Params{
{\em R} & \astype{Ring} & The ring on which the automorphisms operate\\
}
\Descr{\this~R provides automorphims on $R$.}
\begin{exports}
\category{\astype{Group}}\\
\asexp{apply}:
& (\%, R, \astype{Integer}) $\to$ R & Apply a morphism to an element of $R$\\
\asexp{function}: & \% $\to$ (R $\to$ R) & Action of a morphism\\
\asexp{morphism}: & (R $\to$ R) $\to$ \% & Create a morphism\\
                  & (R $\to$ R, R $\to$ R) $\to$ \% & \\
                  & ((R, \astype{Integer}) $\to$ R) $\to$ \% & \\
\end{exports}
#endif

macro Z	== Integer;

Automorphism(R:Ring): Group with {
	apply: (%, R, n:Integer == 1) -> R;
#if ALDOC
\aspage{apply}
\Usage{ $\sigma x$\\ $\sigma(x, n)$\\
\name($\sigma$, x)\\ \name($\sigma$, x, n) }
\Signature{(\%, R, n:\astype{Integer} == 1)}{R}
\Params{
$\sigma$ & \% & An automorphism of $R$\\
{\em x} & R & An element of $R$\\
{\em n} & \astype{Integer} & The number of times to apply (optional)\\
}
\Retval{Returns $\sigma^n x$.}
#endif
	function: % -> (R -> R);
#if ALDOC
\aspage{function}
\Usage{\name~$\sigma$}
\Signature{\%}{(R $\to$ R)}
\Params{ $\sigma$ & \% & An automorphism of $R$\\ }
\Retval{Returns the map corresponding to the action of $\sigma$ on the ring.}
#endif
	morphism: (R -> R) -> %;
	morphism: (R -> R, R -> R) -> %;
	morphism: ((R, Z) -> R) -> %;
#if ALDOC
\aspage{morphism}
\Usage{ \name~f\\ \name($f, f^{-1}$)\\ \name~g }
\Signatures{
\name: & (R $\to$ R) $\to$ \%\\
\name: & (R $\to$ R, R $\to$ R) $\to$ \%\\
\name: & ((R, \astype{Integer}) $\to$ R) $\to$ \%\\
}
\Params{
{\em f} & R $\to$ R & A function\\
$f^{-1}$ & R $\to$ R & The inverse function of $f$\\
{\em g} & (R, \astype{Integer}) $\to$ R & A function\\
}
\Descr{
\name~f creates the morphism $\sigma$ on $R$ given by
$$
\sigma x = f(x)
$$
for any $x \in R$. The morphism is not necessarily invertible, so any attempt
to use its inverse causes an error.\\
\name($f, f^{-1}$) creates the invertible morphism $\sigma$ on $R$ given by
$$
\sigma x = f(x)\qquad \sigma^{-1} x = f^{-1}(x)
$$
for any $x \in R$.\\
\name~g creates the morphism $\sigma$ on $R$ given by
$$
\sigma^n x = g(x, n)
$$
for any $x \in R$. This morphism is considered invertible, so $g$ must also
be defined for negative integers.}
\Remarks{The maps passed as arguments must be ring morphisms, and the
maps $f$ and $f^{-1}$ must be inverses of each other. When an efficient
algorithm for computing $\sigma^n$ is known, for example for $\sigma = 1_R$,
then the form \name~g with g:~(R, Integer) $\to$ R should be used to
avoid repeated iterations of $\sigma$, which is the default behavior.}
#endif
} == add {
	macro Rep == ((R, Z) -> R);

	1				== per((r:R, n:Z):R +-> r);
	inv(f:%):%			== per((r:R, n:Z):R +-> (rep f)(r, -n));
	(f:%)^(m:Z):%			== per((r:R, n:Z):R +-> (rep f)(r,n*m));
	morphism(f:(R, Z) -> R):%	== per f;
	morphism(f:R -> R, g:R -> R):%	== per((r:R,n:Z):R +-> iterat(f,g,n,r));
	morphism(f:R -> R):%		== morphism(f, noinverse);
	local iter(f:R->R, n:Z, r:R):R	== { for i in 1..n repeat r := f r; r }
	local ptr(f:%):Pointer		== f pretend Pointer;
	(u:%) = (v:%):Boolean		== { import from Pointer;ptr u = ptr v }
	apply(f:%, r:R, n:Z):R		== { zero? n => r; (rep f)(r, n) }
	function(f:%)(r:R):R		== { import from Z; f(r, 1) }

	extree(f:%):ExpressionTree == {
		import from Pointer, MachineInteger;
		extree(ptr(f)::MachineInteger);
	}

	local noinverse(r:R):R == {
		import from String;
		error "Morphism is not invertible";
	}

	local iterat(f:R->R, g:R->R, n:Z, r:R):R == {
		n < 0 => iter(g, -n, r);
		iter(f, n, r);
	}

	(f:%) * (g:%):% == {
		import from Z;
		f = g => f^2;
		per((s:R, n:Z):R +->
			iterat((r:R):R +-> f(g(r, 1), 1),
				(r:R):R +-> (inv g)((inv f)(r, 1), 1), n, s));
	}
}
