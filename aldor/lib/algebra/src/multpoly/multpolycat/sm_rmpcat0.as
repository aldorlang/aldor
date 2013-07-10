------------------------------------------------------------------------------
--
-- sm_rmpcat0.as: Basic specifications for Recursive Multivariate Polynomials
--
------------------------------------------------------------------------------
--  Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright: INRIA, and University of Lille I, 2001
-- Copyright: Marc Moreno Maza
--------------------------------------------------------------------------------

-- cat RecursiveMultivariatePolynomialCategory0

#include "algebra"

macro { 
        NNI == Integer;
        Z == Integer;
        MI == MachineInteger;
}

RecursiveMultivariatePolynomialCategory0(R:Join(ArithmeticType, ExpressionType),
   V: Join(TotallyOrderedType, ExpressionType)): Category == PolynomialRing(R,V) with {
        variables: % -> SortedSet(V);
          ++ `variables(p)' returns the sorted (in decreasing order)
          ++ of theses variables such that `degree(p,v)' is not zero.
        mvar: % -> V;
          ++ `mvar(p)' returns the main (i.e. greatest) variable of `p' if 
          ++ `p' is not a constant polynomial otherwise produces an error.  
        mdeg: % -> NNI;
          ++ `mdeg(p)' returns the degree of `p' as univariate in `mvar(p)' if
          ++ `p' is not a constant polynomial otherwise produces an error.
        rank: % -> %;
          ++ `rank(p)' returns `(mvar p)^(mdeg p)' if `p' is not a
          ++ constant polynomial otherwise returns `1'..
        init: % -> %;
          ++ `init(p)' returns the leading coefficient of `p' as univariate 
          ++ in `mvar(p)' if `p' lies in `R' otherwise produces an error. 
        head: % -> %;
          ++ `head(p)' returns `(init p)*(rank p)' if `p' is not a
          ++ constant polynomial otherwise returns `p' itself. In other
          ++ words, `head(p)' is the leading term of `p' as univariate
          ++ polynomial in `mvar(p)'. 
        tail: % -> %;
          ++ `tail(p)' returns `p - head(p)' or in other words the reductum
          ++ of `p' as a univariate polynomial in `mvar(p)'.
}

#if ALDOC
\thistype{RecursiveMultivariatePolynomialCategory0}
\History{Marc Moreno Maza}{08/07/01}{created}
\Usage{\this~(R,V): Category}
\Params{
{\em R} & \astype{ArithmeticType} & The coefficient domain \\
        & \astype{ExpressionType} &\\
{\em V} & \astype{TotallyOrderedType} & The variable domain \\
         & \astype{ExpressionType} &\\
}
\Descr{\this~(R,V) extends \astype{PolynomialRing}(R,V)
with some additional operations related to the recurive vision of a
multivariate polynomial (as a univariate polynomial w.r.t. its main variable). 
}
\begin{exports}
\category{\astype{PolynomialRing}(R,V)} \\
\alexp{variables}: &  \% $\to$ \astype{SortedSet}V
        &  The sorted set of variables of a polynomial \\
\alexp{mvar}: & \% $\to$ V
         & Greatest variable of a polynomial  \\
\alexp{mdeg}: &  \% $\to$ \astype{Integer}
             &  Degree w.r.t. greatest variable \\
\alexp{rank}: &  \% $\to$ \% 
              &  Leading monomial as univariate w.r.t. greatest variable   \\
\alexp{init}: &   \% $\to$ \% 
             & Leading coefficient as univariate w.r.t. greatest variable  \\
\alexp{tail}: &  \% $\to$ \% 
               &  Reductum as univariate w.r.t. greatest variable \\
\alexp{head}: & \% $\to$ \%  
               &  Leading term as univariate w.r.t. greatest variable \\
\end{exports}
#endif



