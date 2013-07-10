------------------------------------------------------------------------------
--
-- rmpz.as: A basic constructor for Recursive Multivariate Polynomials
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
        OS == OrderedSymbol;
}

IntegerPolynomial: RecursiveMultivariatePolynomialCategory0(Z, OS) with { 
      coerce: String -> %;
        ++ `coerce(s)' returns the same as `coerce(s)@OrderedSymbol :: %'. 
      univariate: % -> SUP(%);
        ++ `univariate(p)' returns `p' as a univariate polynomial
        ++ w.r.t. its main variable.
      multivariate: (SUP(%), OS) -> %;
        ++ `multivariate(p,v)' returns `p' as a multivariate 
        ++ polynomial whose main variable is `v'. An error
        ++ is raised if one coeffcient of `p' does not have
        ++ degree zero w.r.t. `v'. 
} == RecursiveMultivariatePolynomial0(SUP, Z, OS)  add { 
      coerce(s: String): % == {
        os: OS := orderedSymbol(s);
        os :: %;
      }
}

#if ALDOC
\thistype{IntegerPolynomial}
\History{Marc Moreno Maza}{08/07/01}{created}
\Usage{import from IntegerPolynomial}

\Descr{\this~ provides a basic domain for 
multivariate polynomials with \altype{Integer} coefficients and variables 
from \altype{OrderedSymbol}.
The representation is sparse and recursive by means of the
\altype{SparseUnivariatePolynomial} univariate polynomial domain constructor.
}
\begin{exports}
\category{\altype{RecursiveMultivariatePolynomialCategory0}(Z,OS)} \\
\alexp{coerce}: & \altype{String} $\to$ \% & Conversion. \\
\alexp{univariate}: & \% $\to$ SUP(\%)  & Convert to univariate w.r.t. the greatest variable  \\
\alexp{multivariate}: & (SUP(\%), OS) $\to$ \%  
         & Convert to multivariate with given variable  \\
\end{exports}
\begin{aswhere}
OS &==& \altype{OrderedSymbol}\\
Z &==& \altype{Integer}\\
SUP &==& \altype{SparseUnivariatePolynomial}\\
\end{aswhere}

#endif

