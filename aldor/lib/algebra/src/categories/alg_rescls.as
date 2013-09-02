#include "algebra"

+++ A domain of `ResidueClassRing(R,p)' implements a commutative
+++ ring isomorphic with `R/p'

define ResidueClassRing(R: CommutativeRing, p: R): Category == CommutativeRing with {
        modularRep: R -> %;
	++ `modularRep(r)' returns the residue class of `r'
        canonicalPreImage: % -> R;
	++ `canonicalPreImage(x)' returns `r' such that
	++ `modularRep(r)' returns `x' and `unitNormal(r) = (u, r, v)'
	++ where u, v belong to `R'
        if R has EuclideanDomain then {
		symmetricPreImage: % -> R;
		++ see Kaltofen Monagan ISSAC 1999
		if R has SourceOfPrimes then {
			if prime?(p)$R then {
				Field;
			}
				
		}

        }
	
}

#if ALDOC
\thistype{ResidueClassRing}
\History{Marc Moreno Maza, Steve Wilson}{June 2004}{created}
\Usage{\this~(R,p): Category}
\Params{
{\em R} & \astype{CommutativeRing} & The ring\\
{\em p} & {\em R} & The modulo \\
}
\Descr{\this~(R,p) specifies an implementation of the residue class ring $R / p$.
The canonical pre-image of an element $x$ of \% is the element $r$ of R such that
$x$ is a canonical representative of $r$ and $r$ equals its canonical associate.}
\begin{exports}
\category{\altype{CommutativeRing}} \\
\alexp{modularRep}: & R $\to$ \%    & Residue class \\
\alexp{canonicalPreImage}:  & \%  $\to$ R   & Canonical pre-image \\
\end{exports}
\begin{exports}[if R has \altype{EuclideanDomain} then] \\
\alexp{symmetricPreImage}:  & \%  $\to$ R   & Symmetric pre-image \\
\end{exports}
#endif
