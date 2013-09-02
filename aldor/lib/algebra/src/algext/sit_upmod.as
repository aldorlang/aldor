----------------------------- sit_upmod.as ---------------------------------
-- Copyright (c) Manuel Bronstein 1994
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994-97
-----------------------------------------------------------------------------

#include "algebra"

macro {
	I	== MachineInteger;
	Z	== Integer;
	RR	== FractionalRoot Z;
	Dx	== DenseUnivariatePolynomial(R, avar);
	Rxy	== SparseUnivariatePolynomial Rx;
	Rxx	== DenseUnivariateTaylorSeries R;
	ARR	== PrimitiveArray;
	POLY	== UnivariatePolynomialAlgebra(%);
	UPC0	== UnivariatePolynomialAlgebra0(%);
	UPMOD	== UnivariatePolynomialMod(R, Rx, modulus, avar);
}

UnivariatePolynomialModTools(R:CommutativeRing,
			Rx: UnivariatePolynomialAlgebra0 R,
			Ry: UnivariatePolynomialAlgebra0 R): with {
	invmodn: (Rx, Z, R) -> Ry;
} == add {
	-- returns a = a0 + a1 x + ... + a_{n-1} a^{n-1}
	-- such that p a = 1 + O(x^{n+1})
	-- n must be degree p and invtc the inverse of p(0)
	invmodn(p:Rx, n:Z, invtc:R):Ry == {
		import from Boolean;
		assert(~zero? p);
		assert(n = degree p);
		assert(one?(invtc * coefficient(p, 0)));
		q := add!(monomial(n)$Ry, invtc, 0@Z);	-- x^n + 1/p(0)
		for i in 1..prev n repeat
			q := add!(q, - convolution(p, q, i), i);
		add!(q, -1, n);				-- remove the extra x^n
	}

	-- returns p_1 q_{n-1} + p_2 q_{n-2} + ... + p_n q_0
	local convolution(p:Rx, q:Ry, n:Z):R == {
		c:R := 0;
		for i in 1..n repeat
			c := add!(c, coefficient(p, i) * coefficient(q, n - i));
		c;
	}
}


#if ALDOC
\thistype{UnivariatePolynomialMod}
\History{Manuel Bronstein}{22/6/94}{created}
\History{Manuel Bronstein}{27/7/98}{added Brent-Kung modular composition}
\History{Manuel Bronstein}{30/6/2003}{change the trace to use Newton series}
\Usage{
import from \this(R, Rx, p)\\
import from \this(R, Rx, p, x)\\
}
\Params{
{\em R} & \altype{CommutativeRing} & The coefficient ring of the polynomials\\
{\em Rx} & \altype{UnivariatePolynomialAlgebra} R & The type of the modulus\\
{\em p} & Rx & The modulus\\
{\em x} & \altype{Symbol} & The generator name (optional)\\
}
\Descr{\this(R, Rx, p) implements the univariate polynomials
modulo $p$, \ie{} the ring $Rx / (p)$, where \emph{p} is assumed to be monic,
but with no other restrictions.
Use instead the types \altype{UnivariatePolynomialModSqfr} when \emph{p}
is also known to be squarefree, and \altype{SimpleAlgebraicExtension}
when \emph{p} is also known to be irreducible.}
\begin{exports}
\category{\altype{UnivariatePolynomialQuotient}(R, Rx)}\\
\end{exports}
#endif

UnivariatePolynomialMod(R:CommutativeRing,
	Rx: UnivariatePolynomialAlgebra0 R,
	modulus:Rx,
	avar:Symbol==new()): UnivariatePolynomialQuotient(R,Rx) with {
		diff: % -> %;				-- wrt x
		resultantWithModulus: % -> R;
} == {
	-- sanity check on the parameters
	import from I, Z;
	N:I == machine(degree(modulus)$Rx);
	assert(N > 0);
	assert(unit?(leadingCoefficient(modulus)$Rx)$R);

	add {
	Rep == Dx;

	import from Z, Rep, R;

	local Rx2Dx(p:Rx):Dx == {
		import from UnivariateFreeRing2(R, Rx, R, Dx);
		map((r:R):R +-> r)(p);
	}

	-- modulus seen as a dense poly
	local densemod:Dx		== Rx2Dx modulus;
	local remByModulus:Rx -> Rx	== monicRemainderBy modulus;
	local remByModulus!:Dx -> Dx	== monicRemainderBy! densemod;

	-- inverse of the leading coefficient of the modulus
	local invlc:R == {
		import from Partial R;
		retract reciprocal(leadingCoefficient(modulus)$Rx);
	}

	local intdom?:Boolean		== R has IntegralDomain;
	local eucdom?:Boolean		== Dx has EuclideanDomain;
	local gcddom?:Boolean		== R has GcdDomain;
	local finiteChar?:Boolean	== R has FiniteCharacteristic;
	local field?:Boolean		== R has Field;

	0				== per(0$Rep);
	1				== per(1$Rep);
	(p:%) + (q:%):%			== per(rep p + rep q);
	minus!(p:%):%			== per minus! rep p;
	add!(p:%, q:%):%		== per add!(rep p, rep q);
	coerce(r:R):%			== per term(r, 0);
	times!(p:%, q:%):%		== reduce! times!(rep p, rep q);
	(r:R) * (p:%):%			== per(r * rep p);
	characteristic:Z		== characteristic$R;
	(p:%) = (q:%):Boolean		== rep p = rep q;
	-(p:%):%			== per(- rep p);
	coerce(n:Z):%			== per(n::Rep);
	extree(p:%):ExpressionTree	== extree rep p;
	copy(p:%):%			== per copy rep p;
	copy!(p:%, q:%):%		== per copy!(rep p, rep q);
	local degext:Z			== N::Z;
	reduce(p:Rx):%			== per Rx2Dx remByModulus p;
	definingPolynomial:Rx		== modulus;
	resultantWithModulus(p:%):R	== resultant(rep p, densemod);
	diff(p:%):%			== per differentiate rep p;
	map(f:R -> R)(p:%):%		== per map(f)(rep p);
	map!(f:R -> R)(p:%):%		== per map!(f)(rep p);
	coefficient(p:%, n:Z):R		== coefficient(rep p, n);
	knownIrreducible?:Boolean	== one? N;
	generator(p:%):Generator Cross(R, Z) == generator rep p;

	-- conditions on which fast reduction via multiplication can be done
	local fftred?:Boolean ==
		R has FFTRing and fftCutoff$(R) > 0
			and N > fftCutoff$(R);

	-- for fast division: a rem modulus = a - q modulus
	-- where q = (a modbar) quo x^{degext + degree(modbar)}
	local modbar:Dx	== {
		fftred? => revert(invmodn(revert modulus, degext,_
				invlc)$UnivariatePolynomialModTools(R, Rx, Dx));
		0;
	}

	local cutoffext:Z == { fftred? => degext + degree modbar; 0 }

	local multred!(p:Dx):% == {
		assert(~zero? p);
		assert(degree p < 2 * degext);
		TRACE("upmod::multred: p = ", p);
		TRACE("upmod::multred: modulus = ", modulus);
		TRACE("upmod::multred: modbar = ", modbar);
		degree p < degext => per p;
		prod := p * modbar;
		d := degree prod - cutoffext;
		q:Dx := 0;
		-- compute the quotient - (p modbar) / x^cutoffext
		for term in prod while d >= 0 repeat {
			(c, e) := term;
			d := e - cutoffext;
			if d >= 0 then q := add!(q, -c, d);
		}
		p := add!(p, densemod * q);
		assert(degree p < degext);
		per p;
	}

	(p:%) * (q:%):% == {
		zero? p or zero? q => 0;
		one? p => q;
		one? q => p;
		reduce!(rep p * rep q);
	}

	-- modular composition by Brent & Kung
	compose(h:%):% -> % == {
		zero? h => { (g:%):% +-> coefficient(rep g, 0)::% };
		h = monom => { (g:%):% +-> g };
		import from ARR Dx;
		(ignore?, t) := nthRoot(N, 2);	-- t^2 <= N < (t+1)^2
		k := N quo t;
		if (t * k) = N then k := prev k;
		hi:ARR Dx := new t;
		hi.0 := 1;
		w:% := 1;
		-- TEMPORARY: BAD OPTIMIZATION BUG (WAS 1203/1232)
		-- for i in 1..prev t repeat
		i:I := 1; while i < t repeat {
			w := h * w;
			hi.i := rep w;
			i := next i;
		}
		ht := h * w;		-- h^t
		(g:%):% +-> {		-- uses Horner's rule at h^t
			zero? g => 0;
			p := rep g;
			v := eval(p, k, t, hi);
			for j in prev(k) .. 0 by -1 repeat {
				v := add!(times!(v, ht), eval(p, j, t, hi));
			}
			v;
		}
	}

	-- computes \sum_{j=0..t-1} coeff(p, k t + j) h^j
	-- where h^j is stored in h.j
	-- complexity is O(N t) +/* in R
	local eval(p:Dx, k:I, t:I, h:ARR Dx):% == {
		kt := k * t;
		ans:Dx := 0;
		for j in 0..prev t repeat
			ans := add!(ans, coefficient(p, (kt + j)::Z), h.j);
		per ans;
	}

	reciprocal(p:%):Partial % == {
		import from Partial Dx;
		intdom? => recip p;
		~failed?(u := reciprocal rep p) => [per retract u];
		-- TEMPORARY: THIS IS WRONG, MUST DO WITH LINEAR ALGEBRA LATER
		failed;
	}

	if R has IntegralDomain then {
		local recip(p:%):Partial % == exactQuotient(1, p);

		exactQuotient(p:%, q:%):Partial % == {
			import from Partial Rep;
			assert(~zero? q);
			zero? p => [0];
			u := {
				eucdom? => diophant(rep q, rep p, densemod);
				resdiophant(rep q, rep p);
			}
			failed? u => failed;
			[per retract u];
		}

		-- returns z s.t. a z = b mod(modulus)
		local resdiophant(a:Dx, b:Dx):Partial Dx == {
			import from Resultant(R, Dx);
			assert(~zero? a); assert(~zero? b);
			assert(degree a <= degree densemod);
			-- r = s modulus + t a, so  a (t b / r) = b (mod c)
			(r, s, t) := extendedLastSPRS(densemod, a);
			exactQuotient(rep reduce!(t * b), r);
		}
	}

	if Dx has EuclideanDomain then {
		-- returns z s.t. a z = b mod(c)
		local diophant(a:Dx,b:Dx,c:Dx):Partial Dx == diophantine(a,b,c);
	}

	local reduce!(p:Dx):% == {
		zero? p => 0;
		fftred? => multred! p;
		per remByModulus! p;
	}

	lift(p:%):Rx == {
		ans:Rx := 0;
		for term in rep p repeat {
			(c, n) := term;
			ans := add!(ans, c, n);
		}
		ans;
	}

	(p:%)^(n:Z):% == {
		import from BinaryPowering(%, Z);
		assert(n >= 0);
		finiteChar? => charpow(p, n);
		zero? n => 1; one? n => p;
		binaryExponentiation(p, n);
	}

	-- TEMPORARY: THIS CONSTANT SHOULD ONLY BE DEFINED IN char p > 0
	local alphap:ARR Rep == new(prev N, 0);

	if R has FiniteCharacteristic then {
		pthPower!(p:%):%	== pthPower p;
		local charpow(p:%,n:Z):%== pExponentiation(p, n)$PthPowering(%);

		-- stores the powers a^p, a^{2p},...,a^{(n-1)p}
		local initPowerTable():() == {
			zero?(alphap.0) => {
				lastxp:% := 1;
				xp := reduce(monomial(characteristic$R)$Rx);
				for i in 0..prev prev N repeat {
					lastxp := xp * lastxp;
					alphap.i := rep lastxp;
				}
			}
		}

		pthPower(p:%):% == {
			import from ARR Rep;
			zero? p or one? p => p;
			initPowerTable();
			q:Rep := 0;
			for term in rep p repeat {
				local c:R; local n:Z;
				(c, n) := term;
				cp := pthPower c;
				if zero? n then q := add!(q, cp, n);
				else q := add!(q, cp, alphap prev machine n);
			}
			per q;
		}
	}
	}
}

#if ALDOC
\thistype{UnivariatePolynomialModSqfr}
\History{Manuel Bronstein}{30/6/2003}{created}
\Usage{
import from \this(R, Rx, p)\\
import from \this(R, Rx, p, x)\\
}
\Params{
{\em R} & \altype{CommutativeRing} & The coefficient ring of the polynomials\\
{\em Rx} & \altype{UnivariatePolynomialAlgebra} R & The type of the modulus\\
{\em p} & Rx & The modulus\\
{\em x} & \altype{Symbol} & The generator name (optional)\\
}
\Descr{\this(R, Rx, p) implements the univariate polynomials
modulo $p$, \ie{} the ring $Rx / (p)$, where \emph{p} is assumed to be monic
and squarefree, but not not necessarily irreducible.
Use instead the types \altype{UnivariatePolynomialMod} when \emph{p} is
not squarefree, and \altype{SimpleAlgebraicExtension}
when \emph{p} is known to be irreducible.}
\begin{exports}
\category{\altype{UnivariatePolynomialQuotientSqfr}(R, Rx)}\\
\end{exports}
#endif

UnivariatePolynomialModSqfr(R:CommutativeRing,
	Rx: UnivariatePolynomialAlgebra0 R,
	modulus:Rx,
	avar:Symbol==new()):UnivariatePolynomialQuotientSqfr(R,Rx)==UPMOD add {
	Rep == UPMOD;

	-- TEMPORARY: SHOULD BE DEFAULT BUT 1.0.0 COMPILER BUG REQUIRES IT HERE
	newtonSeries:Rxx ==
		monicNewtonSeries(modulus)$UnivariateTaylorSeriesType2Poly(_
								R, Rxx, Rx);

	local N:I == machine(degree definingPolynomial)$Z;

	-- sum_ai y^i where modulus = sum_ai x^i
	local modxy:Rxy == map(coerce$Rx)(
			definingPolynomial)$UnivariateFreeRing2(R, Rx, Rx, Rxy);

	norm(p:%):R == resultantWithModulus(rep p)$Rep;

	norm(P:UPC0)(p:P):Rx == {
		q:Rxy := 0;
		for trm in p repeat {
			(calpha, n) := trm;
			for aterm in calpha repeat {
				(c, m) := aterm;
				q := add!(q, term(c, n), m);
			}
		}
		resultant(q, modxy);
	}

	trace(q:%):R == {
		import from Rxx;
		tr:R := 0;
		for trm in q repeat {
			(c, n) := trm;
			tr := add!(tr, c * coefficient(newtonSeries, n));
		}
		tr;
	}

	trace(P:UPC0)(p:P):Rx == {
		tr:Rx := 0;
		for trm in p repeat {
			(c, n) := trm;
			tr := add!(tr, trace c, n);
		}
		tr;
	}

-- the trace table code has been obsoleted by the newton series
#if USETRACETABLE
	-- traceTable(i) = trace(alpha^i) for 0 <= i < N
	local traceTable:ARR R == new(N, 0);

	local initTraceTable():() == {
		zero?(traceTable.0) => {
			alpha:% := monom;
			lasta := reduce(monomial(prev degext)$Rx);
			for i in 0..prev prev N repeat {
				lasta := alpha * lasta;		-- alpha^{N+i}
				for trm in rep lasta repeat {
					(c, n) := trm;
					assert(N > machine n);
					j := i + (N - machine n);
					if j < N then traceTable.j := _
							add!(traceTable.j, c);
				}
			}
			traceTable.0 := degext::R;
		}
	}

	trace(q:%):R == {
		initTraceTable();
		tr:R := 0;
		for trm in rep q repeat {
			(c, n) := trm;
			tr := add!(tr, c * traceTable(machine n));
		}
		tr;
	}
#endif

	-- characteristic 0 is to ensure that the extension is separable
	if R has CharacteristicZero and R has Field then {
		local diff(p:%):% == { import from Rep; per diff rep p }

		lift(D:Derivation R):Derivation % == {
			import from Rep;
			dy := diffGenerator D;
			derivation((p:%):%+-> map(function D)(p) + dy * diff p);
		}

		local diffGenerator(D:Derivation R):% == {
			import from Z, R, Rxx, Partial Rx;
			import from Vector R, LinearAlgebra(R, DenseMatrix R);
			a:DenseMatrix R := zero(N, N);
			b:DenseMatrix R := zero(N, 1);
			for i in 1..N repeat {
				ii := i::Z;
				b(i, 1) :=
					D(coefficient(newtonSeries,ii))/(ii::R);
				for j in 1..N repeat
					a(i,j) := coefficient(newtonSeries,
								(i + j - 2)::Z);
			}
			(sol, diag) := particularSolution(a, b);
			assert(one?(#diag));
			assert(one? numberOfColumns sol);
			assert(one?(diag.1));
			reduce(column(sol, 1)::Rx);
		}

		if R has DifferentialRing then {
			local yprim:Record(comp?:Boolean, dy:%) == [false, 0];

			local yprime():% == {
				yprim.comp? => yprim.dy;
				yprim.comp? := true;
				yprim.dy := diffGenerator(derivation$R);
			}

			differentiate(p:%):% ==
				map(differentiate$R)(p) + yprime() * diff p;
		}
	}
}

