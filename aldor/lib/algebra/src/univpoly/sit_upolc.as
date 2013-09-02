----------------------------- sit_upolc.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1994
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994-97
-----------------------------------------------------------------------------

#include "algebra"

macro {
	I == MachineInteger;
	Z == Integer;
	UPC0 == UnivariatePolynomialAlgebra0 %;
	FR == FractionalRoot;
	RR == FR Z;
	RES == Resultant(R, %);
	RESP == Resultant(% pretend GcdDomain, P);
	GN == Generator;
	FRR==FR(R);
	REC == Record(sigmaexp:Z, exponent:Z);
	MAT == MatrixCategory;
	V == Vector %;
	AI == Array I;
	ORBIT == Cross(%, List REC);
	FLA == FiniteFieldUnivariatePolynomialLinearAlgebra;
	GENMOD == GenericModularPolynomialGcdPackage(%, P);
}

#if ALDOC
%
% Describes also the exports of UnivariatePolynomialAlgebra0, which is
% not documented separately.
%
\thistype{UnivariatePolynomialAlgebra}
\History{Manuel Bronstein}{20/5/94}{created}
\History{Manuel Bronstein}{21/9/95}{added Zippel's multiple gcd (upolc0)}
\History{Manuel Bronstein}{20/9/95}{added check for UnivariateGcdRing}
\History{Thom Mulders}{27/5/97}{added bidirectional exact quotient (upolc0)}
\History{Manuel Bronstein}{18/6/2002}{added LinearAlgebraRing over FiniteField}
\Usage{\this~R: Category}
\Params{
{\em R} & \altype{ExpressionType} & The coefficient domain\\
        & \altype{ArithmeticType} &\\
}
\Descr{\this~is the category of univariate polynomials with coefficients in
an arbitrary domain R and with respect to the power basis $(x^n)_{n \ge 0}$.}

\begin{exports}
\category{\altype{UnivariatePolynomialRing} R}\\
\category{\altype{IndexedFreeAlgebra}(R, \altype{Integer})}\\
\alexp{apply}: & (\%, R) $\to$ R & Evaluate a polynomial\\
\alexp{apply}: & (\%, \%) $\to$ \% & Evaluate a polynomial\\
\alexp{equal?}:
& (\%, \%, \%, \altype{Integer}) $\to$ \altype{Boolean} & Truncated equality\\
\alexp{Horner}: & (\%, R) $\to$ (\%, R) & Horner division by $x - a$\\
\end{exports}

\begin{exports}[if R has \altype{CharacteristicZero} then]
\alexp{ordinaryPoint}:
& \% $\to$ \altype{Integer} & Point where a polynomial is nonzero\\
\end{exports}

\begin{exports}[if R has \altype{CommutativeRing} then]
\category{\altype{DifferentialRing}}\\
\alexp{lift}: & (\altype{Derivation} R, \%) $\to$ \altype{Derivation} \% &
Extend a derivation\\
\alexp{monicDivide}: & (\%, \%) $\to$ (\%, \%) & Polynomial division\\
\alexp{monicDivide!}: & (\%, \%) $\to$ (\%, \%) & Polynomial division\\
\alexp{monicDivideBy}: & \% $\to$ \% $\to$ (\%, \%) & Polynomial division\\
\alexp{monicDivideBy!}: & \% $\to$ \% $\to$ (\%, \%) & Polynomial division\\
\alexp{monicQuotient}: & (\%, \%) $\to$ \% & Quotient\\
\alexp{monicQuotient!}: & (\%, \%) $\to$ \% & Quotient\\
\alexp{monicQuotientBy}: & \% $\to$ \% $\to$ \% & Quotient\\
\alexp{monicQuotientBy!}: & \% $\to$ \% $\to$ \% & Quotient\\
\alexp{monicRemainder}: & (\%, \%) $\to$ \% & Remainder\\
\alexp{monicRemainder!}: & (\%, \%) $\to$ \% & Remainder\\
\alexp{monicRemainderBy}: & \% $\to$ \% $\to$ \% & Remainder\\
\alexp{monicRemainderBy!}: & \% $\to$ \% $\to$ \% & Remainder\\
\end{exports}

\begin{exports}
[if R has \altype{CommutativeRing} and R has \altype{RittRing} then]
\alexp{integrate}: & \% $\to$ \% & Integration\\
& (\%, \altype{Integer}) $\to$ \% & \\
\end{exports}

\begin{exports}[if R has \altype{FactorizationRing} then]
\alexp{factor}:
& \% $\to$ (R, \altype{Product} \%) & Factorisation into irreducibles\\
\alexp{fractionalRoots}:
& \% $\to$ \altype{Generator} \altype{FractionalRoot} R &
Roots in the fraction field\\
\alexp{roots}:
& \% $\to$ \altype{Generator} \altype{FractionalRoot} R &
Roots in the coefficient ring\\
\end{exports}

\begin{exports}[if R has \altype{Field} then]
\category{\altype{EuclideanDomain}}\\
\alexp{rationalReconstruction}:
& \% $\to$ \% $\to$ \altype{Partial} \builtin{Cross}(\%, \%) &
Rational reconstruction\\
\alexp{sparseMultiple}:
& (\%, \altype{Integer}) $\to$ \% & Multiple in $k[x^n]$\\
\end{exports}

\begin{exports}[if R has \altype{FiniteField} then]
\category{LinearAlgebraRing}\\
\end{exports}

\begin{exports}[if R has \altype{GcdDomain} then]
\category{\altype{DecomposableRing}}\\
\category{\altype{GcdDomain}}\\
\alexp{squareFree}:
& \% $\to$ (R, \altype{Product} \%) & Squarefree factorisation\\
\alexp{squareFreePart}: & \% $\to$ \% & Squarefree part\\
\end{exports}

\begin{exports}
[if R has \altype{GcdDomain} and R has \altype{RationalRootRing} then]
\alexp{dispersion}: & \% $\to$ \altype{Integer} & Dispersion\\
                    & (\%, \%) $\to$ \altype{Integer} & \\
\alexp{integerDistances}:
& \% $\to$ \altype{List} \altype{Integer} & Integer spread\\
& (\%, \%) $\to$ \altype{List} \altype{Integer} & \\
\alexp{universalBound}:
& (\%,\%) $\to$ \altype{List} \builtin{Cross}(\%, \altype{Integer}) &
Universal bound\\
\end{exports}

\begin{exports}[if R has \altype{IntegralDomain} then]
\category{\altype{IntegralDomain}}\\
\alexp{pseudoDivide}: & (\%, \%) $\to$ (\%, \%) & Polynomial pseudo--division\\
\alexp{pseudoRemainder}: & (\%, \%) $\to$ \% & Pseudo--remainder\\
\alexp{pseudoRemainder!}: & (\%, \%) $\to$ \% & Pseudo--remainder\\
\alexp{resultant}: & (\%, \%) $\to$ R & Resultant of 2 polynomials\\
\end{exports}

\begin{exports}[if R has \altype{OrderedArithmeticType} then]
\alexp{height}: & \% $\to$ R & Max norm over all the coefficients\\
\end{exports}

\begin{exports}[if R has \altype{RationalRootRing} then]
\category{\altype{RationalRootRing}}\\
\alexp{integerRoots}:
& \% $\to$ \altype{Generator} \altype{FractionalRoot} \altype{Integer} &
Integer roots\\
\alexp{rationalRoots}:
& \% $\to$ \altype{Generator} \altype{FractionalRoot} \altype{Integer} &
Rational roots\\
\end{exports}

\begin{exports}[if R has \altype{Ring} then]
\alexp{values}:
& (\%, R) $\to$ \altype{Generator} R & Generate values of a polynomial\\
\end{exports}

\begin{exports}[if R has \altype{Specializable} then]
\category{\altype{Specializable}}\\
\end{exports}

% This loads the alpage's for UnivariatePolynomialAlgebra0
\input sit_upolc0.tex
#endif

define UnivariatePolynomialAlgebra(R:Join(ExpressionType, ArithmeticType)):
	Category == UnivariatePolynomialAlgebra0 R with {
        if R has CommutativeRing then ModularComputation;
	if R has FiniteField then LinearAlgebraRing;
	if R has FactorizationRing then {
		SourceOfPrimes;
		if R has Field then UnivariateGcdRing;
		factor: % -> (R, Product %);
#if ALDOC
\alpage{factor}
\Usage{ \name~p }
\Signature{\%}{(R, \altype{Product} \%)}
\Params{ {\em p} & \% & A polynomial\\ }
\Retval{Returns $(c, p_1^{e_1} \cdots p_n^{e_n})$ such that
each $p_i$ is irreducible, the $p_i$'s have no common factors, and
$$
p = c\;\prod_{i=1}^n p_i^{e_i}\,.
$$
}
#endif
		fractionalRoots: % -> GN FRR;
		roots: % -> GN FRR;
#if ALDOC
\alpage{fractionalRoots,roots}
\altarget{fractionalRoots}
\altarget{roots}
\Usage{ fractionalRoots~p\\ roots~p }
\Signature{\%}{\altype{Generator} \altype{FractionalRoot} \%}
\Params{ {\em p} & \% & A polynomial\\ }
\Retval{Returns a generator that produces all the roots $p$
either in the ring or in its fraction field.}
#endif
	}
	if R has RationalRootRing then {
		RationalRootRing;
		if R has GcdDomain then {
			dispersion: % -> Z;
			dispersion: (%, %) -> Z;
			integerDistances: % -> List Z;
			integerDistances: (%, %) -> List Z;
#if ALDOC
\alpage{dispersion,integerDistances}
\altarget{dispersion}
\altarget{integerDistances}
\Usage{ dispersion~p\\ dispersion(p, q)\\
integerDistances~p\\ integerDistances(p, q)}
\Signatures{
dispersion: & \% $\to$ \altype{Integer}\\
dispersion: & (\%,\%) $\to$ \altype{Integer}\\
integerDistances: & \% $\to$ \altype{List} \altype{Integer}\\
integerDistances: & (\%,\%) $\to$ \altype{List} \altype{Integer}\\
}
\Params{
{\em p} & \% & A nonzero polynomial\\
{\em q} & \% & A nonzero polynomial (optional)\\
}
\Retval{integerDistances(p, q) returns
all the integers $e \in \ZZ$ such that for each such $e$
there exists $\alpha$ in an algebraic closure of the fraction
field of {\em R} such that $p(\alpha) = q(\alpha + e) = 0$,
while dispersion(p, q) returns $-1$ if
integerDistances(p, q) contains only elements strictly smaller than $0$,
its maximal nonnegative element otherwise.}
\Remarks{The parameter $q$ is optional for both functions,
its default value being $p$.}
#endif
		}
		integerRoots: % -> Generator RR;
		rationalRoots: % -> Generator RR;
#if ALDOC
\alpage{integerRoots,rationalRoots}
\altarget{integerRoots}
\altarget{rationalRoots}
\Usage{ integerRoots~p\\ rationalRoots~p }
\Signature{\%}{\altype{Generator} \altype{FractionalRoot} \altype{Integer}}
\Params{ {\em p} & \% & A polynomial\\ }
\Retval{Return $[(r_1,e_1),\dots,(r_n,e_n)]$ where the $r_i$'s are
the integer or rational roots of $p$ and have multiplicity $e_i$.}
#endif
		minIntegerRoot: % -> Partial Z;
		maxIntegerRoot: % -> Partial Z;
#if ALDOC
\alpage{minIntegerRoot,maxIntegerRoot}
\altarget{minIntegerRoot}
\altarget{maxIntegerRoot}
\Usage{ minIntegerRoot~p\\ maxIntegerRoot~p }
\Signature{\%}{\altype{Partial} \altype{Integer}}
\Params{ {\em p} & \% & A polynomial\\ }
\Retval{ Return \failed if $p$ has no integer root, its smallest (resp.~largest)
one otherwise.}
#endif
		if R has GcdDomain then {
			universalBound: (%, %) -> List Cross(%, Z);
#if ALDOC
\alpage{universalBound}
\Usage{\name(a, b)}
\Signature{(\%,\%)}{\altype{List} \builtin{Cross}(\%, \altype{Integer})}
\Params{ {\em a, b} & \% & Nonzero polynomials\\ }
\Retval{Return $[(p_1,e_1),\dots,(p_n,e_n)]$ such that any polynomial
bounded by $a$ and $b$ (in the sense of S.A.~Abramov,
{\em Rational solutions of linear difference and $q$--difference equations
with polynomial coefficients}, Proceedings of ISSAC'95)
must be a factor of $u = \prod_{i=1}^n \prod_{j=0}^{e_i} p_i(x-j)$.}
#endif
		}
	}
	default {
		local gcd?:Boolean == R has GcdDomain;

		if R has CommutativeRing then {
		    residueClassRing(p: %): ResidueClassRing(%, p) == {
			UnivariatePolynomialResidueClassRing(R,%,p);
		    }
		}
		if R has IntegralDomain then {
			resultant(p:%, q:%):R == resultant(p, q)$RES;
		}

		if R has FactorizationRing then {
			getPrime(): Partial(%) == [monom];
			prime?(x: %): Partial(Boolean) == {
			        local c: R; local p: Product(%);
				(c, p) := factor(x);
				import from I, Product(%);
				[one?(#p)];			
			}

			getPrimeOfSize(d: I):Partial(%) == {
				import from I, Z;
				one? d => getPrime();
				failed;
			}

			nextPrime(x:%):Partial(%) == {
				import from I, Z, R;
				not one? leadingCoefficient(x) => failed;
			 	not one? degree x => failed;
				p: % := x + 1;
				p = monom => failed;
				[p];
			}

			if R has Field then {
				gcdUP(P:UPC0)(p:P, q:P):P == {
					import from Partial P;
					failed?(u := modularGcd(p, q)$GENMOD) =>
						subResultantGcd(p, q)$RESP;
					retract u;
				}
			}
		}

		if R has FiniteField then {
			determinant(M:MAT %):M->% == determinant$FLA(R,%,M);
			inverse(M:MAT %):M -> (M, V)	== inverse$FLA(R,%,M);
			kernel(M:MAT %):M -> M		== kernel$FLA(R,%,M);
			rank(M:MAT %):M -> I		== rank$FLA(R,%,M);
			solve(M:MAT %):(M,M) -> (M,M,V)	== solve$FLA(R,%,M);
			span(M:MAT %):M -> AI		== span$FLA(R,%,M);

			linearDependence(g:Generator V, n:I):V ==
				linearDependence(g, n)$FLA(R,%,DenseMatrix %);

			particularSolution(M:MAT %):(M, M) -> (M, V) ==
				particularSolution$FLA(R, %, M);

			maxInvertibleSubmatrix(M:MAT %):M -> (AI, AI) ==
				maxInvertibleSubmatrix$FLA(R, %, M);
		}

		if R has GcdDomain then {
			local gcdproject(P:UPC0, p:P):% == {
				import from Z;
				assert(~zero? p);
				q:% := 0;
				for i in 0..degree p repeat
					q := gcd(q, project(P, p, i));
				q;
			}

-- TEMPORARY: WHEN algebraic extensions have good gcd algorithms
#if MULTIGCD
			gcd(l:List %):% == {
				import from MultiGcd(R, %);
				(g, lq) := multiGcd l;
				g;
			}

			gcdquo(l:List %):(%, List %) == {
				import from MultiGcd(R, %);
				multiGcd l;
			}
#endif

			local ugring?:Boolean	== R has UnivariateGcdRing;
			local field?:Boolean	== R has Field;
			local infCharp?:Boolean == R has FiniteCharacteristic
							and ~(R has FiniteSet);

			gcd(p:%, q:%):%	== {
				ugring? => ugGcd(p, q);
				field? => eucl(p, q);
				subResultantGcd(p, q)$RES;
			};

			-- destroys p and q
			gcd!(p:%, q:%):% == {
				ugring? => ugGcd!(p, q);
				field? => eucl!(p, q);
				subResultantGcd(p, q)$RES;
			}

			squareFree(p:%):(R, Product %) == {
				import from Integer,
					UnivariatePolynomialSquareFree(R, %);
				infCharp? => musser p;
				yun p;
			}

			if R has UnivariateGcdRing then {
				local ugGcd(p:%, q:%):% == {
					(gcdUP$R)(%)(p, q);
				}

				local ugGcd!(p:%, q:%):% == {
					(gcdUP!$R)(%)(p, q);
				}

				gcdquo(p:%, q:%):(%, %, %) == {
					(gcdquoUP$R)(%)(p, q);
				}
			}

			if R has Field then {
				local eucl(p:%, q:%):%	== monic euclid(p, q);
				local eucl!(p:%, q:%):%	== monic! euclid!(p, q);
			}
		}

		if R has FactorizationRing then {
			factor(p:%):(R, Product %)	== (factor(%)$R)(p);
			roots(p:%):GN FRR		== (roots(%)$R)(p);
			fractionalRoots(p:%):GN FRR== (fractionalRoots(%)$R)(p);
		}

		if R has RationalRootRing then {
			integerRoots(p:%):GN RR  == (integerRoots(%)$R)(p);
			rationalRoots(p:%):GN RR == (rationalRoots(%)$R)(p);
			minIntegerRoot(p:%):Partial Z == zselect(p, <$Z);
			maxIntegerRoot(p:%):Partial Z == zselect(p, >$Z);

			local zselect(p:%, f?: (Z, Z) -> Boolean):Partial Z == {
				import from Boolean, RR;
				assert(~zero? p);
				emp? := true;
				n:Z := 0;
				for r in integerRoots p repeat {
					m := integralValue r;
					if emp? or f?(m, n) then n := m;
					emp? := false;
				}
				emp? => failed;
				[n];
			}

			if R has GcdDomain then {
				dispersion(p:%):Z == disp integerDistances p;

				dispersion(p:%, q:%):Z ==
					disp integerDistances(p, q);

				local disp(l:List Z):Z == {
					empty? l => -1;
					m := first l;
					for n in rest l repeat
						if n > m then m := n;
					m < 0 => -1;
					m;
				}

				universalBound(a:%, b:%):List Cross(%, Z) == {
					import from Boolean, Z, List Z, R;
					assert(~zero? a); assert(~zero? b);
					u:List Cross(%, Z) := empty;
					l := [e for e in integerDistances(a,b)|_
						e >= 0];
					empty? l => u;
					spr := reverse! sort! l;
					lastm := next first spr;
					for m in spr repeat {
						assert(m >= 0);
						assert(m < lastm);
						bm := translate(b, (-m)::R);
						g := primitivePart gcd(a, bm);
						if degree g > 0 then {
							a := quotient(a, g);
							gm := translate(g,m::R);
							b := quotient(b, gm);
							u := cons((g, m), u);
						}
					}
					u;
				}
			}

			local roots(P:UPC0, p:P, f:% -> GN RR):GN RR ==
				f project(P, p);

			integerRoots(P:UPC0):P -> GN RR ==
				(p:P):GN(RR) +-> roots(P, p, integerRoots$%);

			rationalRoots(P:UPC0):P -> GN RR ==
				(p:P):GN(RR) +-> roots(P, p, rationalRoots$%);


			local project(P:UPC0, p:P):% == {
				import from Z;
				zero? p => 0;
				-- TEMPORARY: WOULD LIKE TO CACHE THE TEST
				-- gcd? => gcdproject(P, p);
				R has GcdDomain => gcdproject(P, p);
				q:% := 1;
				for i in 0..degree p repeat
					q := times!(q, project(P, p, i));
				q;
			}

			local project(P:UPC0, p:P, m:Z):% == {
				q:% := 0;
				for term in p repeat {
					(c, n) := term;
					q := add!(q, coefficient(c, m), n);
				}
				q;
			}
		}
	}
}
