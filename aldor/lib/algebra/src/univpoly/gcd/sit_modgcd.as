------------------------------- sit_modgcd.as --------------------------------
--
-- Compiler directives used:
--  PROFILEMODGCD     gives profiling information when on
--  USEFULLWORD       uses full word size primes (half-word otherwise)
--
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Institute of Scientific Computation, ETH Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"

macro {
	SI  == MachineInteger;
	ARR == PrimitiveArray;
}

#if ALDOC
\thistype{ModularUnivariateGcd}
\History{Laurent Bernardin}{17/8/95}{created}
\History{Manuel Bronstein}{19/4/96}
{Uses optimized CRT function with inverse caching and machine int modulis.
Uses local gcd mod p function rather than instantiating prime fields.
Makes trial divisions only after first try, and when lifted gcd stabilizes.}
\Usage{import from \this(Z, P)}
\Params{
{\em Z} & \altype{IntegerCategory} & An integer-like ring\\
{\em P} & \altype{UnivariatePolynomialAlgebra} Z & Polynomials over Z}
\Descr{\this~provides an implementation of a modular GCD algorithm for
univariate polynomials over the integer, using the Chinese Remainder Theorem.}
\begin{exports}
\alexp{modularGcd}: & (P, P) $\to$ (\altype{Partial} P, P, P) &
The Modular Gcd algorithm\\
\end{exports}
#endif

ModularUnivariateGcd(Z:IntegerCategory,P:UnivariatePolynomialAlgebra0 Z):with {
	modularGcd:	(P, P)	->	(Partial P, P, P);
#if ALDOC
\alpage{modularGcd}
\Usage{\name($p_1, p_2$)}
\Signatures{
\name: & (P, P) $\to$ (\altype{Partial} P, P, P)\\
}
\Params{
{\em $p_1, p_2$} & P & Polynomials over Z\\
}
\Retval{Returns $(g, y, z)$ such that $g = \gcd(p_1, p_2)$ or \failed,
and if $g$ is not \failed, then $p = y g$ and $q = z g$.}
\Remarks{This algorithm can fail because it runs out of primes. This will happen
if the gcd has coefficients with more than around 3000 digits.}
#endif
} == add {
#if PROFILEMODGCD
	import from String, Character, File, TextWriter, Timer, SI;
	local modGcdCount:SI := 0;
	local tries:SI := 0;
	local profile := open("modgcd.prof", fileWrite)::TextWriter;
	local clock := timer(); local clockquo := timer();
	local clockp1 := timer(); local clockp2 := timer();
	local clockcrt := timer();

	local prof(file:TextWriter, tthis:SI, tcrt:SI):() == {
		file << "Tries = " << tries << newline;
		file << "Total accumulated gcd time = " << read clock <<newline;
		file << "Time (tot / mod p reduce, mod p gcd, crt, exquo) = ";
		file << tthis << " " << read clockp1 << " " << read clockp2;
		file << " " << tcrt << " " << read clockquo << newline;
		file << newline;
	}

	local (exquo)(a:P, b:P):Partial P == {
		start! clockquo;
		u := exactQuotient(a, b);
		stop! clockquo;
		u;
	}
#else
	local (exquo)(a:P, b:P):Partial P == exactQuotient(a, b);
#endif

	-- returns a new prime that does not divide a or b
	-- goes in descending order
	local getPrime(p:SI, a:Z, b:Z):SI == {
		-- USING HALF-WORD PRIMES IS MORE EFFICIENT ON ALL RISC CPUs
		-- AND ON ALL NON-RISC CPUs UNTIL Integer IS OPTIMIZED FOR THEM
#if USEFULLWORD
		import from WordSizePrimes;
#else
		import from HalfWordSizePrimes;
#endif
		import from Boolean, Partial Z;
		p := { p < 0 => maxPrime; previousPrime p; }
		while (~zero? p) and
			~(failed?(exactQuotient(a, p::Z)) and
			  failed?(exactQuotient(b, p::Z))) repeat
				p := previousPrime p;
		p;
	}

	modularGcd(a:P,b:P):(Partial P, P, P) == {
		import from SI, Integer, Partial P;
#if PROFILEMODGCD
		start! clock;
		reset! clockquo; reset! clockp1; reset! clockp2;reset! clockcrt;
		free modGcdCount := modGcdCount + 1;
		free tries := 0;
		profile << "modularGcd: " << modGcdCount << newline;
		profile << "degrees = " << degree a << "  " <<degree b<<newline;
		zero? a => {
			prof(profile, stop! clock, read clockcrt);
			([b], 0, 1);
		}
		zero? b => {
			prof(profile, stop! clock, read clockcrt);
			([a], 1, 0);
		}
#else
		zero? a => ([b], 0, 1);
		zero? b => ([a], 1, 0);
#endif
		degree a < degree b => {
			(g, y, z) := modgcd(b, a);
			(g, z, y);
		}
		modgcd(a, b);
	}

	-- by construction, lc(g) always divide gcd(lc(a), lc(b)),
	-- but check the trailing coefficients before calling exquo
	local testDivide(a:P, b:P, g:P, tc:Z):(Boolean,Partial P,Partial P) == {
		import from Partial Z;
		(c, d) := trailingTerm g;
		(~failed? exactQuotient(tc, c)) and
		(~failed?(uqa := (exquo)(a, g))) and
			(~failed?(uqb := (exquo)(b, g))) => (true, uqa, uqb);
		(false, failed, failed);
	}

	local modgcd(a:P,b:P):(Partial P, P, P) == {
		import from Boolean, SI, ARR SI, Z, Integer, Partial P;
		assert(~zero? a); assert(~zero? b);
		assert(degree a >= degree b);
		TRACE("modgcd, a = ", a); TRACE("b = ", b);
		(ca, a) := primitive a;
		(cb, b) := primitive b;
		(gc, coca, cocb) := gcdquo(ca, cb);
		TRACE("content(a) = ", ca); TRACE("content(b) = ", cb);
		TRACE("a = ", a); TRACE("b = ", b);
#if PROFILEMODGCD
		one? a => {
			prof(profile, stop! clock, read clockcrt);
			([gc::P], coca * a, cocb * b);
		}
		one? b => {
			prof(profile, stop! clock, read clockcrt);
			([gc::P], coca * a, cocb * b);
		}
		a = b => {
			prof(profile, stop! clock, read clockcrt);
			([gc * a], coca::P, cocb::P);
		}
#else
		one? a => ([gc::P], coca * a, cocb * b);
		one? b => ([gc::P], coca * a, cocb * b);
		a=b => ([gc * a], coca::P, cocb::P);
#endif
		~failed?(uq := (exquo)(a,b)) => {		-- b divides a
#if PROFILEMODGCD
			prof(profile, stop! clock, read clockcrt);
#endif
			([gc * b], coca * retract uq, cocb::P);
		}
		lca := leadingCoefficient a; lcb := leadingCoefficient b;
		lcg := gcd(lca, lcb);
		(tca, deg) := trailingTerm a;
		(tcb, deg) := trailingTerm b;
		tcg := gcd(tca, tcb);
		TRACE("gcd lc = ", lcg); TRACE("gcd tc = ", tcg);
		p := getPrime(-1, lca, lcb);
		d := degree a; da := machine d; db := machine degree b;
		-- create buffers for a and b mod p
		amodp:ARR(SI) := new next da;
		bmodp:ARR(SI) := new next db;
		while p ~= 0 repeat {
			TRACE("Trying prime: ", p::Z);
#if PROFILEMODGCD
			free tries := tries + 1;
#endif
			g := tryprime(a, da, amodp, b, db, bmodp, lcg, p);
			TRACE("found a modular gcd: ",g);
			if degree g < d then {
				d := degree g;
				TRACE("degree of gcd is ",d);
				aa := g;
				bb := primitivePart aa;
				(ok?, ua, ub) := testDivide(a, b, bb, tcg);
				ok? => {
#if PROFILEMODGCD
			prof(profile, stop! clock, read clockcrt);
#endif
					return ([gc * bb], coca * retract ua,
							cocb * retract ub);
				}
				M := p::Z;
			}
			else if degree g = d then {
				bb:P := 0;
#if PROFILEMODGCD
				start! clockcrt;
#endif
				CRT := combine(M, p);
#if PROFILEMODGCD
				stop! clockcrt;
#endif
				stable?:Boolean := true;
				for i in d..0 by -1 repeat {
					c := coefficient(aa, i);
					cg := machine coefficient(g, i);
					TRACE("Combining ",c);
					TRACE("with ",coefficient(g,i));
#if PROFILEMODGCD
					start! clockcrt;
#endif
					if ~(zero? c and zero? cg) then {
						newc := CRT(c, cg);
#if PROFILEMODGCD
						stop! clockcrt;
#endif
						bb := add!(bb, newc, i);
						if stable? and newc ~= c then
							stable? := false;
					}
					TRACE("and got ",newc);
				}
				M := (p::Z) * M;
				TRACE("current modulus: ",M);
				aa := bb;
				-- only test division when stable
				if stable? then {
					bb := primitivePart bb;
					TRACE("reconstructed gcd: ",bb);
					(ok?, ua, ub) := testDivide(a,b,bb,tcg);
					ok? => {
#if PROFILEMODGCD
			prof(profile, stop! clock, read clockcrt);
#endif
						return ([gc * bb],
							coca * retract ua,
							cocb * retract ub);
					}
				}
			}
			p := getPrime(p, lca, lcb);
		}
#if PROFILEMODGCD
		prof(profile, stop! clock, read clockcrt);
#endif
		(failed, 0, 0);
	}

	local tryprime(a:P, da:SI, amodp:ARR SI, b:P, db:SI, bmodp:ARR SI,
		lcg:Z, p:SI):P == {
#if PROFILEMODGCD
		start! clockp1;
#endif
		assert(da >= db);
		import from Z, Integer, ModulopUnivariateGcd;
		pover2 := shift(p, -1); 		-- floor(p/2) = (p-1)/2
		for i in 0..db repeat amodp.i := bmodp.i := 0;
		for i in next db..da repeat amodp.i := 0;
		for t in a repeat {
			(c, e) := t;
			amodp(da - machine e) := c mod p;
		}
		for t in b repeat {
			(c, e) := t;
			bmodp(db - machine e) := c mod p;
		}
#if PROFILEMODGCD
		stop! clockp1;
		start! clockp2;
#endif
		(vec, deg, start) := gcd!(amodp, da, bmodp, db, lcg mod p, p);
		TRACE("tryprime:gcd degree = ", deg);
#if PROFILEMODGCD
		stop! clockp2;
		start! clockp1;
#endif
		-- gcd is now in vec(start),vec(start+1),...
		g:P := 0;
		for i in 0..deg repeat {
			cg := vec(start + i);
			if cg > pover2 then cg := cg - p;	-- balanced rep
			g := add!(g, cg::Z, (deg - i)::Integer);
		}
		TRACE("tryprime:gcd = ", g);
#if PROFILEMODGCD
		stop! clockp1;
#endif
		g;
	}
}

#if ALDORTEST
------------------ test modgcd.as -----------------
#include "algebra"
#include "aldortest"

macro {
        Z == Integer;
        P == DenseUnivariatePolynomial Z;
	Q == Fraction Z;
	PQ == DenseUnivariatePolynomial Q;
}

local testZ():Boolean == {
	import from Z, P, Partial P, ModularUnivariateGcd(Z, P);

	x := monom;

	p := 7*6345345643*x^30+3726436423*x^2+27832642637*1;
	q := 7*736473647*x^53+323623132*x+2372637237*1;

	g:=87654321*x^2+12345646464*x-12888734673*1;

	a := p*g; b := q*g;

	(gt, y, z) := modularGcd(a,b);

	(retract gt) = g => true;
	false;
}

stdout << "Testing sit_modgcd..." << endnl;
--aldorTest("modular GCD over Z", testZ);
--TEST FAILED: does not terminate
stdout << endnl;
#endif
