--------------------------- alg_poltype.as ------------------------------------
--
-- Copyright (c) Manuel Bronstein 2004
-- Copyright (c) Marc Moreno Maza 2004
-- Copyright (c) INRIA, LIFL, UWO 2004
--
-----------------------------------------------------------------------------


#include "algebra"

macro {
	Z   == Integer;
	UPR == IndexedFreeRRing(R, Z);
	UPP == IndexedFreeRRing(%, Z);
	SUP == SparseUnivariatePolynomial;
}

define PolynomialTypeRing(R:Join(ArithmeticType, ExpressionType),
		V:ExpressionType):Category == StandardFilteredRing(R, V) with {
	coefficient: (%, V, Z) -> %;
		++ coefficient(p,v,n) returns q s.t. p = q v^n.
	coefficient: (%, List V, List Z) -> %;
		++ coefficient(p,[v1,...,vk],[n1,...,nk] returns q s.t.
		++ p = q v1^n1 ... vk^nk.
	degree: (%, V) -> Z;
		++ `degree(p,v)' returns the degree of `p' w.r.t. `v'.
	degrees: % -> Generator(Cross(V, Z));
		++ `degree(p)' traverses the elements of the list
		++ `[(v,degree(p,v)) for v in variables(v)]'. 
	if V has TotallyOrderedType then {
		initial: % -> %;
			++ returns the leading coeff w.r.t. the main variable
	}
	leadingCoefficient: (%, V) -> %;
		++ `leadingCoefficient(p,v)' returns the leading coefficient
		++ of `p' regarded as univariate polynomial in `v'.
	multivariate: (P: UPR) -> ((up: P, v: V) -> %);
		++ `multivariate(P)(p, v)' converts an anonymous univariate 
		++ polynomial to a polynomial in the variable `v'.
	multivariate: (P: UPP) -> ((up: P, v: V) -> %);
		++ `multivariate(P)(p, v)' converts an anonymous univariate
		++ polynomial to a polynomial in the variable `v'.
	reductum: (%, V) -> %;
		++ `reductum(p,v)' returns the reductum
		++ of `p' regarded as univariate polynomial in `v'.
	univariate: (P: UPP) -> ((p: %, v: V) -> P);
		++ `univariate(P)(p, v)' converts the multivariate polynomial
		++ `p' into a univariate polynomial in `v', whose coefficients
		++ are still multivariate polys (in all the other variables).
	if V has TotallyOrderedType then {
		univariate: (P: UPP) -> ((p: %) -> P);
			++ `univariate(P)(p)' converts the multivariate poly
			++ p into a univariate poly in its main variable, whose 
			++ coefficients are still multivariate polynomials (in
			++ all the other variables).
	}
	default {
		if V has TotallyOrderedType then {
			initial(p:%):% == {
				ground? p => p;
				leadingCoefficient(p, mainVariable p);
			}

			univariate(P: UPP)(p: %): P == {
				ground? p => p::P;
				univariate(P)(p, mainVariable p);
			}
		}

		leadingCoefficient(p:%, v:V):% == {
			import from SUP %;
			leadingCoefficient(univariate(SUP %)(p, v));
		}

		coefficient(p:%, v:V, n:Z):% == {
			import from SUP %;
			coefficient(univariate(SUP %)(p, v), n);
		}

		reductum(p:%, v:V):% == {
			import from SUP %;
			multivariate(SUP %)(reductum(univariate(SUP %)(p,v)),v);
		}

		coefficient(p:%, lv:List V, ln:List Z):% == {
			import from MachineInteger;
			assert(#lv = #ln);
			if ~(commutative?$%) then {
				lv := reverse lv;
				ln := reverse ln;
			}
			for v in lv for n in ln repeat p := coefficient(p,v,n);
			p;
		}

		multivariate (P: UPR) (up: P, v: V): % == {
			import from Z;
			zero? degree up => leadingCoefficient(up)::%;
			p: % := 0;
			for cross in terms up repeat {
				(r, d) := cross;
				p := add!(p, term(r, v, d));
			}
			p;
		}   

		multivariate (P:UPP) (up:P, v:V): % == {  
			import from Z, R;
			ground? up => leadingCoefficient up; 
			p: % := 0;
			for cross in terms up repeat {
				(r, d) := cross;
				assert(zero? degree(r, v));
				p := add!(p, r * term(1, v, d));
			}
			p;
		} 

		degrees(p:%): Generator(Cross(V, Z)) == generate {
			for v in variables p repeat yield (v, degree(p, v));
		}

		totalDegree(p:%): Z == {
			local varsp:List(V) := [variables p];
			local td: Z := 0;
			local n: Z;
			for cross in support p repeat {
				(r, m) := cross;
				n := 0;
				for v in varsp repeat n := add!(n, degree(m,v));
				if n > td then td := n;
			}
			td;
		}
	}
}


#if ALDOC
\thistype{PolynomialTypeRing}
\History{Marc Moreno Maza}{08/07/2001}{created as part of PolynomialRing0}
\History{Manuel Bronstein}{18/11/2004}{extracted from PolynomialRing0}
\Usage{\this~(R,V): Category}
\Params{
{\em R} & \altype{ArithmeticType} & The coefficient domain \\
        & \altype{ExpressionType} &\\
{\em V} & \altype{ExpressionType} & The variable domain \\
}
\Descr{\this~(R,V) is the category of standard filtered rings generated over R
by power products of the form $v_1^{e_1}\cdots v_n^{e_n}$ for $v_i \in V$
and $e_i \in mathbb{N}$. No commutativity is assumed either between the
variables in V, or between R and V.}
\begin{exports}
\category{\altype{StandardFilteredRing}(R, V)}\\
\alexp{coefficient}: & (\%, V, Z) $\to$ \% & Coefficient w.r.t.~a power $v^n$.\\
\alexp{coefficient}: &   (\%, L V, L Z) $\to$ \% 
                     &  Coefficient w.r.t.~a power product \\
\alexp{degree}: &  (\%, V)  $\to$ Z & Degree w.r.t. a variable \\
\alexp{degrees}: &  \% $\to$ GEN VZ & Degrees w.r.t. all variables \\
\alexp{leadingCoefficient}: &  (\%, V) $\to$ \%
          &  Leading coefficient w.r.t. a variable \\
\alexp{multivariate}: & (P:IFRR R) $\to$ (P, V) $\to$ \% &
Conversion from univariate view\\
                      & (P:IFRR P) $\to$ (P, V) $\to$ \% & \\
\alexp{reductum}: & (\%, V) $\to$ \% & Reductum w.r.t.~a variable \\
\alexp{univariate}: & (P:IFRR P) $\to$ (\%, V) $\to$ P & 
Conversion to univariate view\\
\end{exports}
\begin{exports}[If V has \altype{TotallyOrderedType} then]
\alexp{initial}: & \% $\to$ \% & Leading coefficient w.r.t.~main variable\\
\alexp{univariate}: & (P:IFRR P) $\to$ \% $\to$ P &
Conversion to univariate view\\
\end{exports}
\begin{alwhere}
GEN &==& \altype{Generator}\\
IFRR &==& \altype{IndexedFreeRRing}\\
L &==& \altype{List}\\
Z &==& \altype{Integer}\\
VZ &==& \altype{Cross} (V, \altype{Integer})\\
\end{alwhere}
#endif

