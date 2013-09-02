#include "algebra"

macro {
	SingleInteger == MachineInteger;
	CRT == ChineseRemaindering %;
}

ModularComputation: Category == CanonicalSimplification with {
        residueClassRing: (p: %) -> ResidueClassRing(%,p);
	if % has EuclideanDomain then {
            combine: (%, %) -> (%, %) -> %;
            ++ `combine(M1,M2)(a,b)' returns `c' such that `c mod M1'
            ++ is `a', `c mod M2' is `b' and `c mod (M1*M2)' is `c'.
            ++ This assumes that `M1' and `M2' are relatively prime.
	    if % has IntegerType then {
	        combine: (%, SingleInteger) -> (%, SingleInteger) -> %;
        	++`combine(M,m)(a,i)' returns the same `combine(M,m::%)(a,i::%)'
	    }
	}
	default {
	    if % has EuclideanDomain then {
		combine(M1: %, M2: %): (%, %) -> % == combine(M1, M2)$CRT;
		if % has IntegerType then {
		    combine(M:%, m:SingleInteger):(%, SingleInteger) -> % == {
			    combine(M, m)$CRT;
		    }
		}
	    }
	}
}

#if ALDOC
\thistype{ModularComputation}
\History{Marc Moreno Maza, Steve Wilson}{1999-2004}{created}
\Usage{\this: Category}
\Descr{\this~is the category of domains that support modular algorithm
        such as the modular gcd algorithm.
        More precisely, for every element $p$ of \% the 
        operation {\tt residueClassRing} returns a domain 
        implementing the residue class ring $R / p$.
        In addition, if \% is an Eclidean domain,  
        the operation  {\tt combine} implements the Chinese Remaindering algorithm.
}
\begin{exports}
\category{\altype{CanonicalSimplification}} \\
\alexp{residueClassRing}: &  (p: \%) $\to$  \altype{ResidueClassRing} (\%,p) &  Residue class ring  \\
\end{exports}
\begin{exports}[if \% has \altype{EuclideanDomain} then]
\alexp{combine}: &  (\%, \%) $\to$  (\%, \%) $\to$ \% & Chinese remaindering algorithm \\
\end{exports}


#endif
