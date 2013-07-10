------------------------------------------------------------------------------
--
-- rmpzx.as: A basic constructor for Recursive Multivariate Polynomials
--
------------------------------------------------------------------------------
-- Copyright (c) Marc Moreno Maza
-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright (c) INRIA (France), USTL (France), UWO (Ontario) 2002
------------------------------------------------------------------------------

#include "algebra"

macro { 
        Z == Integer;
        SUP == SparseUnivariatePolynomial;
}

SparseIntegerMultivariatePolynomial(V: VariableType):
-- Join(Parsable,Algebra Integer,CommutativeRing,CopyableType)
   RecursiveMultivariatePolynomialCategory0(Z, V) with {
      univariate: % -> SUP(%);
        ++ `univariate(p)' returns `p' as a univariate polynomial
        ++ w.r.t. its main variable.
      multivariate: (SUP(%), V) -> %;
        ++ `multivariate(p,v)' returns `p' as a multivariate 
        ++ polynomial whose main variable is `v'. An error
        ++ is raised if one coeffcient of `p' does not have
        ++ degree zero w.r.t. `v'. 
} == RecursiveMultivariatePolynomial0(SUP, Z, V) add { }


#if ALDOC
\thistype{SparseIntegerMultivariatePolynomial}
\History{Marc Moreno Maza}{08/07/01}{created}
\Usage{import from \this~(V)}
\Params{
{\em V} & \altype{VariableType} & The variables\\
}
\Descr{\this~(V) provides a basic domain for 
multivariate polynomials with coefficients over the \altype{Integer} ring and variables 
in {\em V}. The representation is sparse and recursive by means of {\em SUP}.
}
\begin{exports}
\category{\altype{RecursiveMultivariatePolynomialCategory0}(Z,V)} \\
\alexp{univariate}: & \% $\to$ SUP(\%)  
         & Convert to univariate w.r.t. the greatest variable  \\
\alexp{multivariate}: & (SUP(\%), V) $\to$ \%  
         & Convert to multivariate with given variable  \\
\end{exports}
\begin{aswhere}
Z &==& \altype{Integer} \\
SUP &==& \altype{SparseUnivariatePolynomial}\\
\end{aswhere}
#endif

