----------------------------- alg_sup.as ----------------------------------
-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright (c) Marc Moreno Maza 2002
-- Copyright (c) INRIA (France), USTL (France), UWO (Ontario) 2002
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{SparseUnivariatePolynomial}
\History{Marc Moreno Maza}{13/05/2002}{created}
\History{Marc Moreno Maza}{21/06/2002}{last update}
\Usage{ import from \this~R\\ import from \this(R, x) }
\Params{
{\em R} & \altype{ExpressionType} & The coefficient domain\\
        & \altype{ArithmeticType} &\\
{\em x} & \altype{Symbol} & The variable name (optional)\\
}
\Descr{\this(R, x) implements sparse univariate polynomials with coefficients
in R.}
\begin{exports}
\category{\altype{UnivariatePolynomialAlgebra} R}\\
\end{exports}
#endif

macro {
	Z == Integer;
	NNI == Integer;
	}

SparseUnivariatePolynomial(R:Join(ArithmeticType, ExpressionType),
	avar:Symbol == new()): UnivariatePolynomialAlgebra(R) == SparseUnivariatePolynomial1(R,avar) add {


	------------------------
	--% Rep and imports  %--
	------------------------

	Term == Record(co: R, ex: NNI);
	Rep	== List Term;
		-- always ordered terms, decreasing wrt exponent's order
		-- and non-zero coefficients
	import from Term, Rep, R, Z;

	-----------------------
	--% local constanta %--
	-----------------------


	------------------------
	--% local functions  %--
	------------------------

	--------------------------------------------------
	--% Exports as UnivariatePolynomialAlgebra(R) %--
	--------------------------------------------------
	-- UnivariatePolynomialAlgebra0(R)
	-- if (R has FactorizationRing) then 
	-- 	factor: % -> (R, Product(%))
        -- 	fractionalRoots: % -> Generator(FractionalRoot(R pretend CommutativeRing))
        --	roots: % -> Generator(FractionalRoot(R pretend CommutativeRing))

	if R has RationalRootRing then {

		-- RationalRootRing
                -- integerRoots: % -> Generator(FractionalRoot(AldorInteger))
                -- rationalRoots: % -> Generator(FractionalRoot(AldorInteger))
                -- minIntegerRoot: % -> Partial(AldorInteger)
                -- maxIntegerRoot: % -> Partial(AldorInteger)

		if R has GcdDomain then {

                        -- dispersion: % -> AldorInteger
                        -- dispersion: (%, %) -> AldorInteger

			macro RXY == SparseUnivariatePolynomial %;
			macro SPREAD == UnivariatePolynomialSpread(R, %, RXY);

			integerDistances(p:%):List Z ==
				integerDistances(p)$SPREAD;

			integerDistances(p:%, q:%):List Z ==
				integerDistances(p, q)$SPREAD;
		}
	}

	---------------------------------------------------
	--% Exports as UnivariatePolynomialAlgebra0(R) %--
	---------------------------------------------------

	--- !!! REMOVED


}

#if ALDORTEST
---------------------- test sup.as ---------------------------
#include "algebra"
#include "aldortest"

macro {
        Z == Integer;
        Zx == SparseUnivariatePolynomial Z;
        Zxt == SparseUnivariatePolynomial Zx;
}

degree():Boolean == {
        import from Z, Zx;
        x := monom;
        p := (x - 1) * (x + 1);
        degree p = 2 and leadingCoefficient p = 1 and zero? p(-1@Z);
}

exactQuotient():Boolean == {
        import from Zx, Partial Zx;

        x := monom;
        a := x - 1;
        b := x + 1;
        p := a * b;
        q := exactQuotient(p, a);	-- must be b
        f := exactQuotient(p, x);	-- must be failed
        ~(failed? q) and failed? f and retract(q) = b;
}

diff():Boolean == {
        import from Z, Zx, Zxt;
        x:Zx := monom;
	t:Zxt := monom;
	p := x * t + (x^2)::Zxt;
	D:Derivation(Zxt) := lift(derivation, t);	-- t' = t
	q := D p;			-- must be (1 + x) t + 2 x
	r := q - p;			-- must be t + 2 x - x^2
	m := x * (x - 2::Zx);
	degree r = 1 and leadingCoefficient r = 1
		and zero?(reductum(r) + m::Zxt);
}

hgcd(a:Zx, b:Zx):Zx == {
	import from Partial Zx, HeuristicGcd(Z, Zx);
	(g, a, b) := heuristicGcd(a, b);
	retract g;
}

mgcd(a:Zx, b:Zx):Zx == {
	import from Partial Zx, ModularUnivariateGcd(Z, Zx);
	(g, a, b) := modularGcd(a, b);
	failed? g => error "mgcd: modularGcd failed";
	retract g;
}

heugcd():Boolean == gcd hgcd;

modgcd():Boolean == gcd mgcd;

gcd(ggt:(Zx,Zx) -> Zx):Boolean == {
	import from Z, Zx;
	import from Assert Zx;

	x := monom;
	r := x^2 + 1;
	s := x^2 - 1;

	assertEquals(x, ggt(x, x));
	assertEquals(x, ggt(x, x^2));
	assertEquals(1, ggt(r, x));
	assertEquals(x, ggt(x, x*r));
	assertEquals(r, ggt(r^2, r*s));

	p := x^8 + x^6 - 3*x^4 - 3*x^3 + 8*x^2 +2*x - 5@Z ::Zx;
	q := 3*x^6 + 5*x^4 -4*x^2 -9*x + 21@Z ::Zx;
	g := ggt(p, q);
	rg := ggt(r * p, r * q);
	g = 1 and rg = r;
}

stdout << "Testing alg__sup..." << endnl;
aldorTest("degree", degree);
aldorTest("exactQuotient", exactQuotient);
aldorTest("diff", diff);
aldorTest("modgcd", modgcd);
aldorTest("heugcd", heugcd);
stdout << endnl;
#endif
