------------------------------- sit_zfactor.as --------------------------------
-- Copyright (c) Laurent Bernardin 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{UnivariateIntegralFactorizer}
\History{Laurent Bernardin}{4/12/95}{created}
\History{Manuel Bronstein}{22/07/98}
{factortryprime: added combinations of factors}
\History{Manuel Bronstein}{9/03/2004}{fixed bug in factor}
\Usage{import from \this(Z, P)}
\Params{
{\em Z} & \altype{IntegerCategory} & An integer-like ring\\
{\em P} & \altype{UnivariatePolynomialAlgebra0} Z & A polynomial type over Z\\
}
\Descr{\this(Z, P) implements a factorizer for polynomials with
integer coefficients.}
\begin{exports}
\alexp{factor}: & P $\to$ (Z, \altype{Product} P) & Factor\\
\alexp{integerRoots}:
& P $\to$ \altype{Generator} \altype{FractionalRoot} \altype{Integer} &
Integer roots\\
\alexp{rationalRoots}:
& P $\to$ \altype{Generator} \altype{FractionalRoot} \altype{Integer} &
Rational roots\\
\end{exports}
#endif

macro {
	I	== MachineInteger;
	PZ	== DenseUnivariatePolynomial Z;
	RR	== FractionalRoot Integer;
}

UnivariateIntegralFactorizer(Z:IntegerCategory,
			P:UnivariatePolynomialAlgebra0 Z): with {
	factor:			P	->	(Z, Product P);
#if ALDOC
\alpage{factor}
\Usage{\name~p}
\Signature{P}{(Z,\altype{Product} P)}
\Params{ {\em p} & P & A polynomial with integer coefficients\\ }
\Retval{Returns $(c, p_1^{e_1} \cdots p_n^{e_n})$ such that
each $p_i$ is irreducible, the $p_i$'s have no common factors, and
$$
p = c\;\prod_{i=1}^n p_i^{e_i}\,.
$$
}
\alseealso{\alfunc{UnivariatePolynomialAlgebra0}{squareFree}}
#endif
	integerRoots:		P	->	Generator RR;
	rationalRoots:		P	->	Generator RR;
#if ALDOC
\alpage{integerRoots,rationalRoots}
\altarget{integerRoots}
\altarget{rationalRoots}
\Usage{integerRoots~p\\rationalRoots~p}
\Signature{P}{\altype{Generator} \altype{FractionalRoot} \altype{Integer}}
\Params{ {\em p} & P & A polynomial with integer coefficients\\ }
\Retval{integerRoots(p) (resp.~rationalRoots(p)) return
$[(r_1,e_1),\dots,(r_n,e_n)]$ where the $r_i$'s are
the integer (resp.~rational) roots of $p$ and have multiplicity $e_i$.}
#endif
} == add {
	local maxHeightFactor(f:P):Z == (degree f)*2^(degree f)*height(f);

	local maxHeightLinearFactor(f:P):Z == {
		import from Integer;
		zero? f => 0;
		b:Z := 0;
		i:Integer := 0;
		while zero? b repeat {
			b := coefficient(f, i);
			i := next i;
		}
		b < 0 => -b;
		b;
	}

	integerRoots(f:P):Generator RR == generate {
		import from Boolean, Product P, RR, Z, List Cross(Z,Z);
		TRACE("zfactor::integerRoots: ", f);
		(lc, s) := squareFree f;
		for t in s repeat {
			(p, e) := t;
			for rr in findroots(p, false) repeat {
				(N, D) := rr;
				if one? D then yield integralRoot(integer N, e);
			}
		}
	}

	factor(f:P):(Z, Product P) == {
		import from List P;
		u:Z := 1;
		r:Product P := 1;
		(lc,s) := squareFree f;
		for t in s repeat {
			(p, e) := t;
			if leadingCoefficient(p) < 0 then {
				p := -p;
				u := -u;
			}
			fr := findfactors p;
			for rr in fr repeat r := times!(r, rr, e);
		}
		(u * lc, r);
	}

	-- returns a positive denominator
	local ratrecon(u:Z,m:Z,nb:Z,db:Z):Partial Cross(Z,Z) == {
		v := rationalReconstruction(u, m, integer nb, integer db);
		failed? v => failed;
		(t1, t0) := retract v;
		t0 < 0 => [(-t1,-t0)];
		v;
	}

	rationalRoots(f:P):Generator RR == generate {
		import from Boolean, Product P, Z, List Cross(Z,Z), RR;
		(lc, s) := squareFree f;
		for t in s repeat {
			(g, e) := t;
			for rr in findroots(g, true) repeat {
				(N, D) := rr;
				yield fractionalRoot(integer N, integer D, e);
			}
		}
	}

	-- f must be squarefree
	local findroots(f:P, dorational?:Boolean):List Cross(Z, Z) == {
		import from I, Z, Integer, SmallPrimes, Partial Z;
		zero?(d := degree f) => empty;
		d = 1 => {
			f1 := leadingCoefficient f;
			f0 := coefficient(f, 0);
			dorational? => {
				sol:Fraction Z := - f0 / f1;
				cons((numerator sol, denominator sol), empty);
			}
			failed?(u := exactQuotient(f0, f1)) => empty;
			cons((- retract u, 1), empty);
		}
		bound := maxHeightLinearFactor(f) + 1;
		TRACE("zfactor::findroots, f = ",f);
		p:I := 5;
		unlucky?:Boolean := true;
		while unlucky? repeat {
			(r, unlucky?) := roottryprime(f, p, bound, dorational?);
			p := nextPrime p;
		}
		r;
	}

	local findfactors(f:P):List P == {
		import from SmallPrimes, Integer, Z;
		assert(leadingCoefficient(f) > 0);
		(d := degree f) <= 1 => [f];
		bound := maxHeightFactor(f) + 1;
		TRACE("zfactor::findfactors: f = ",f);
		TRACE("zfactor::findfactors: bound = ",bound);
		p:I := 5;
		(r,f) := factortryprime(f,p,bound);
		while f~=1 repeat {
			TRACE("::looking for factors of: ",f);
			p := nextPrime(p);
			(r1,f) := factortryprime(f,p,bound);
			r := append!(r,r1);
			TRACE("::now got these factors: ",r);
		}
		TRACE("zfactor::findfactors returns = ",r);
		r;
	}

	local factortryprime(f:P,p:I,bound:Z):(List P, P) == {
		macro {
			F == ZechPrimeField p;
			PF == DenseUnivariatePolynomial F;
		}
		import from Boolean, Integer, F, PF, Product PF;
		import from UnivariateHenselLifting(Z,P,F,PF);
		import from Partial P,Partial List P;

		TRACE("zfactor::factortryprime, f = ", f);
		TRACE("::prime = ", p);
		fp := downgrade f;
		degree fp ~= degree f => {
			TRACE("::unlucky prime"," returning empty");
			(empty,f);
		}
		(mfcoeff, mfc):=factor fp;
		-- assert(mfcoeff=1);
		mf:List PF := empty;
		nfacts:I := 0;
		for tt in mfc repeat {
			(pp,e) := tt;
			e~=1 => {
				TRACE("::evaluation not square free",
					 " returning empty");
				return (empty,f);
			}
			mf := cons(pp,mf);
			nfacts := next nfacts;
		}
		nfacts=1 => (cons(f,empty),1);
		TRACE("::Number of modular factors: ",nfacts);
		liftbound := 2@Z * abs(leadingCoefficient f) * bound;
		TRACE("::liftbound = ",liftbound);
		TRACE("::Modular factors = ", mf);
		(lf,pk) := linearLift(f, mf, liftbound);
		TRACE("::lifting failed = ", failed? lf);
		failed? lf => (empty,f);
		pkfactors := retract lf;
		TRACE("zfactor::factortryprime:lifted factors: ", pkfactors);
		TRACE("zfactor::factortryprime:modulo: ", pk);
		i:I := 1;
		tf:List P := empty;
		while i <= (#pkfactors) quo 2 repeat {
			TRACE("zfactor::factortryprime: combinations of: ", i);
			(tf0,pkfactors,f):= tryCombinations(pkfactors,f,i,1,pk);
			TRACE("zfactor::factortryprime: factors found: ", tf0);
			TRACE("remaining p-adic factors: ", pkfactors);
			TRACE("remaining poly to factor: ", f);
			tf := append!(tf, tf0);
			i := next i;
		}
		TRACE("zfactor::factortryprime:returns ", cons(f, tf));
		(cons(f,tf), 1); 
	}

	-- returns a mod p in [-p/2,+p/2]
	local balred(a:Z, p:Z, p2:Z):Z == {
		b := a mod p;
		b > p2 => b - p;
		b;
	}

	-- computes primpart(pq mod modulus), balanced representation
	local times(p:P, q:P, modulus:Z, p2:Z):P == {
		assert(p2 = modulus quo 2);
		pq := p * q;
		ans:P := 0;
		g:Z := 0;
		notunitg?:Boolean := true;
		for term in pq repeat {
			(c, e) := term;
			cc := balred(c, modulus, p2);
			if notunitg? then {
				g := gcd(g, cc);
				notunitg? := ~unit? g;
			}
			ans := add!(ans, cc, e);
		}
		notunitg? and ~zero? g => map!(quotientBy g)(ans);
		ans;
	}

	local tryCombinations(l:List P,f:P,i:I,prod:P,pk:Z):(List P,List P,P)=={
		import from Boolean, Partial P;
		assert(odd? pk);
		assert(unit? content prod);
		p2 := pk quo 2;
		-- TRACE("tryCombinations:l = ",l);
		-- TRACE("tryCombinations:f = ",f);
		-- TRACE("tryCombinations:i = ",i);
		-- TRACE("tryCombinations:prod = ",prod);
		-- TRACE("tryCombinations:pk = ",pk);
		i = 0 => {
			q := exactQuotient(f, prod);
			failed? q => (empty, l, f);
			([prod], l, retract q);
		}
		i > #l => (empty, l, f);
		l1 := first l;
		-- TRACE("tryCombinations:l1 = ",l1);
		-- l will be the candidates left minus l1
		-- if tf is empty, then l1 must be added back as a candidate
		(tf, l, f) := tryCombinations(rest l, f, prev i,
						times(prod, l1, pk, p2), pk);
		-- TRACE("tryCombinations:tf = ",tf);
		~empty?(tf) and ~one?(prod) => (tf, l, f);
		(tff, l, f) := tryCombinations(l, f, i, prod, pk);
		-- TRACE("tryCombinations:tff = ",tff);
		empty? tf => (tff, cons(l1, l), f);
		(append!(tf, tff), l, f);
	}

	-- second argument is true when the prime p was unlucky
	local roottryprime(f:P,p:I,bound:Z,
			dorational:Boolean):(List Cross(Z, Z), Boolean) == {
		macro {
			F == ZechPrimeField p;
			PF == DenseUnivariatePolynomial F;
		}
		import from Integer, F, PF, UnivariateHenselLifting(Z,P,F,PF);
		import from List F, Partial Z, Partial P;
		import from Partial Cross(Z, Z);

		TRACE("zfactor::roottryprime, f = ", p);
		TRACE(":: prime = ", p);
		fp := downgrade f;
		degree(fp) ~= degree(f) or zero?(g := gcd(fp, differentiate fp))
			or degree(g) > 0 => {
				TRACE("::unlucky prime"," returning empty");
				(empty, true);
		}
		r:List Cross(Z, Z) := empty;
		DB := abs(leadingCoefficient f);
		liftbound := 2@Z * DB * bound;
		for ir in rootsSqfr(PF)(fp) repeat {
			TRACE("::lifting root: ",ir);
			(rr,liftk) := linearLift(f, ir, liftbound);
			if ~failed? rr then {
				TRACE("::lifted root: ",retract rr);
				q := exactQuotient(f, monom-term(retract rr,0));
				if ~failed? q then {
					TRACE("::found root ",retract rr);
					f := retract q; 
					r := cons((retract rr,1), r);
				}
				else if dorational then {
					NB := liftk quo ((2@Z)*DB);
					if (2@Z)*DB*NB = liftk then
						NB := NB-1;
					rrr := retract rr;
					qq := ratrecon(rrr,liftk,NB,DB);
					if ~failed? qq then {
						(n,d) := retract qq;
						assert(d > 0);
						q := exactQuotient(f, d*monom-
								term(n,0));
						if ~failed? q then {
							f := retract q;
							r := cons((n,d),
								r);
						}
					}
				}
			}
		}
		(r, false);
	}
}			

#if ALDORTEST
---------------------- test sit_zfactor.as --------------------------
#include "algebra"
#include "aldortest"

macro {
	Z == Integer;
	Q == Fraction Z;
	P == DenseUnivariatePolynomial Z;
	RR == FractionalRoot Z;
}

local integerRoots():Boolean == {
	import from Z, P, RR, List RR;
	x == monom;
	f := x^5 + 8120 * x^4 - 6299183 * x^3 - 9446096526 * x^2 +
					3671637382172 * x + 51412443096::P;
	l := [integerRoots f];
	mult:Z := 1;
	count:Z := 0;
	t1234:Boolean := false; tm987:Boolean := false; t346:Boolean := false;
	for rt in l repeat {
		~integral? rt => return false;
		n := integralValue rt;
		mult := mult * multiplicity(rt);
		count := next count;
		if n = 1234 then t1234 := true;
		else if n = 346 then t346 := true;
		else if n = -987 then tm987 := true;
	}
	count = 3 and mult = 1 and t1234 and t346 and tm987;
}

local rationalRoots():Boolean == {
	import from Z, Q, P, RR, List RR;
	x := monom;
	f := 13*x^9+384*x^8-1485*x^7+13*x^3+397*x^2-1101*x-1485::P;
	l := [rationalRoots f];
	mult:Z := 1;
	count:Z := 0;
	tm33:Boolean := false; t4513:Boolean := false;
	for rt in l repeat {
		(n, d) := value rt;
		q := n / d;
		mult := mult * multiplicity(rt);
		count := next count;
		if q = -33::Q then tm33 := true;
		else if q = 45/13 then t4513 := true;
	}
	count = 2 and mult = 1 and tm33 and t4513;
}

local factor():Boolean == {
	import from Z, P, Product P;
	x == monom;
	f := 20*x^5-96*x^4+1191439*x^3-6889560*x^2-21718242087*x+17378393004::P;
	(lcoeff, p) := factor f;
	count:Z := 0;
	for term in p repeat count := count + 1;
	lcoeff = 1 and count = 4 and expand p = f;
}

stdout << "Testing sit__zfactor..." << newline;
aldorTest("integerRoots", integerRoots);
--aldorTest("rationalRoots", rationalRoots);
--aldorTest("factor", factor);
--TESTS FAILED: do not terminate
stdout << newline;
#endif

