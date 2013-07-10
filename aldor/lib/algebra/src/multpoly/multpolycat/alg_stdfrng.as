--------------------------- alg_stdfrng.as ------------------------------------
--
-- Copyright (c) Manuel Bronstein 2004
-- Copyright (c) Marc Moreno Maza 2004
-- Copyright (c) INRIA, LIFL, UWO 2004
--
-----------------------------------------------------------------------------

#include "algebra"

macro Z == Integer;

define StandardFilteredRing(R:Join(ArithmeticType, ExpressionType),
				V:ExpressionType):Category ==
					Join(CopyableType, FreeRRing R) with {
	if R has Parsable and V has Parsable then Parsable;
	coerce : V -> %;
		++ `coerce(v)' returns `v' as an element of `%'.
	ground?: % -> Boolean;
		++ `ground?(p)' returns true iff `p' has no variables.
		++ of `p'  is `1'.
	if V has TotallyOrderedType then {
		mainVariable : % -> V;
			++ `mainVariable(p)' returns the greatest variable `v'
			++ appearing in p. ground? p should be false.
	}
	term: (R, V, Z) -> %;
		++ `term(r,v,n)' returns `r::% * (v::%)^n'.
	term: (R, List V, List Z) -> %;
		++ `term(r, [v1...vk], [n1...nk])' returns the polynomial 
		++ `r*v1^n1*v2^n2*...*vk^nk'.
	term: (R, Generator(Cross(V, Z))) -> %;
		++ `term(r, g) returns the same as `term(r,[v1...vk],[n1...nk])'
		++ where `[(v1, n1), ..., (vk, nk)]' is [cross for cross in g]'.
	times: (%, R, V, Z) -> %;
		++ `times(p,r,v,n)' returns `p * term(r,v,n)'.
	times!: (%, R, V, Z) -> %;
		++ `times!(p,r,v,n)' returns `times!(p,term(r,v,n))'.
	totalDegree: % -> Z;
		++ `totalDegree(p)' returns the largest sum over all monomials
		++ of the exponents of the variables in `variables(lp)'.
	univariate?: % -> Boolean;
		++ `univariate?(p)' returns true iff `p' has 0 or 1 variable.
	variable? : % -> Boolean;
		++ `variable?(p)' returns true iff there exists a variable
		++ `v' such that `v::%' is `p'.
	variable : % -> V;
		++ `variable(p)' returns the only variable `v' such that
		++ `v::%' returns `p' if there exists such a variable
		++ otherwise an error is produced.
	variableProduct: % -> Generator(Cross(V, Z));
		++ `variableProduct(p)' assumes that p is a product of variables
		++ and returns `g' such that `term(1,g)' equals `p'
	variables: % -> Generator V;
		++ variables(p) generates over all the variables appearing in p.
		++ If V has TOrderedType,they are generated in decreasing order.
	default {
		term (r:R, lv: List V, ln: List Z): % == {
			import from MachineInteger;
			assert(#lv = #ln);
			zero? r => 0;
			p := r::%;
			for v in lv repeat {
				p := times!(p, 1, v, first ln);
				ln := rest ln;
			}
			p;
		}

		term (r:R, g: Generator(Cross(V, Z))): % == {
			zero? r => 0;
			local p: %;
			p := r::%;     
			for cross in g repeat {
				(v, d) :=  cross;
				p := times!(p,1,v,d);
			}
			p;
		}

		univariate?(p:%):Boolean == {
			n:MachineInteger := 0;
			for v in variables p repeat {
				(n := next n) > 1 => return false;
			}
			true;
		}

		if R has Parsable and V has Parsable then {
			eval(l:ExpressionTreeLeaf):Partial % == {
				import from R, Partial R, V, Partial V;
				failed?(u := eval(l)$R) => {
					failed?(v := eval(l)$V) => failed;
					[coerce(retract v)@%];
				}
				[retract(u)::%];
			}

			eval(op:MachineInteger, args:List ExpressionTree):_
				Partial % == {
				import from ParsingTools %;
				import from R, Partial R, V, Partial V;
				failed?(u := evalArith(op, args)) => {
					failed?(v := eval(op, args)$R) => {
						failed?(w := eval(op,args)$V) =>
									failed;
						[coerce(retract w)@%];
					}
					[retract(v)::%];
				}
				u;
			}
		}
	}
}


#if ALDOC
\thistype{StandardFilteredRing}
\History{Marc Moreno Maza}{08/07/2001}{created as part of PolynomialRing0}
\History{Manuel Bronstein}{17/11/2004}{extracted from PolynomialRing0}
\Usage{\this~(R,V): Category}
\Params{
{\em R} & \altype{ArithmeticType} & The coefficient domain \\
        & \altype{ExpressionType} &\\
{\em V} & \altype{ExpressionType} & The variable domain \\
}
\Descr{\this~(R,V) is the category of R-rings with a standard filtration
for which R is the ring of elements of degree 0, and V generates the elements
of degree 1. It is the set of finite linear combinations of the form
$\sum r_i w_i$ where the $r_i$ are in R and the $w_i$ are words of V.}
\begin{exports}
\category{\altype{CopyableType}}\\
\category{\altype{FreeRRing} R} \\
\alexp{coerce}: & V $\to$ \%  & Conversion of a variable to a polynomial  \\
\alexp{ground?}: & \% $\to$ \altype{Boolean}  &
Membership test to the coefficient ring \\
\alexp{totalDegree}: &  \% $\to$ Z
                     &  Greatest total degree among all the monomials \\ 
\alexp{term}: &   (R, V, Z) $\to$ \%
           &  \alexp{term}$(r,v,n)$ returns $r \, v^n$ provided $n \geq 0$  \\ 
\alexp{term}:  &   (R, GEN VZ))  $\to$ \%
            & Term from a power product and a coeff. \\
\alexp{term}: &   (R, L V, L Z)  $\to$ \%
            & Term from a power product and a coeff. \\
\alexp{times}: &   ( \%, R, V , Z ) $\to$ \% 
           &  \alexp{times}$(x,r,v,n)$ is $x \, (r \, v^n)$ \\
\alexp{times!}: &    ( \%, R, V , Z ) $\to$ \% 
           & \alexp{times}$(x,r,v,n)$ is $x \, (r \, v^n)$ and may modify $x$ \\
\alexp{univariate?}: & \% $\to$ \altype{Boolean}  &
Check whether elt is univariate\\
\alexp{variable}: & \% $\to$ V & Conversion of a polynomial to a variable  \\
\alexp{variable?}: &  \% $\to$ \altype{Boolean}  &
Membership test to the variable domain \\
\alexp{variableProduct}: &  \%  $\to$ GEN VZ
      & \alexp{variableProduct} $x$ is $g$ s.t.~\alexp{term}$(1,g)$ is $x$  \\
\alexp{variables}: & \% $\to$ GEN V & The variables occuring in a polynomial  \\
\end{exports}
\begin{alwhere}
GEN &==& \altype{Generator}\\
L &==& \altype{List}\\
Z &==& \altype{Integer}\\
VZ &==& \altype{Cross} (V, \altype{Integer})\\
\end{alwhere}
\begin{exports}[if R has \altype{Parsable} and V has \altype{Parsable} then]
\category{\altype{Parsable}}\\
\end{exports}
\begin{exports}[if V has \altype{TotallyOrderedType} then]
\alexp{mainVariable}: & \% $\to$ V  & Greatest variable occuring\\
\end{exports}
#endif
