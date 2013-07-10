---------------------------- sit_upfactp.as -------------------------------
-- Copyright (c) Laurent Bernardin 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{PrimeFieldUnivariateFactorizer}
\History{Laurent Bernardin}{8/6/95}{created}
\History{Manuel Bronstein}{27/7/98}{added Shoup's distinct degree factorization}
\Usage{import from \this(F, P)}
\Params{
{\em F} & \altype{PrimeFieldCategory0} & Coefficient ring of the polynomials\\
{\em P} & \altype{UnivariatePolynomialAlgebra} F & A polynomial ring\\
}
\Descr{\this~provides implementations of various univariate factorization
algorithms over a prime field.}
\begin{exports}
\alexp{berlekamp}: & P $\to$ \altype{List} P & Berlekamp's algorithm\\
\alexp{cantorZassenhaus}:
& P $\to$ \altype{List} P & Cantor-Zassenhaus algorithm\\
\alexp{factor}: & P $\to$ (F, PROD) & Factor (default algorithm)\\
\alexp{factor}: & (P $\to$ \altype{List} P) $\to$ (P $\to$ (F, PROD)) &
Factor (given algorithm)\\
\alexp{roots}:
& (P, \altype{Boolean}) $\to$ \altype{Generator} FR F & Roots of a polynomial\\
\end{exports}
\begin{aswhere}
FR &==& \altype{FractionalRoot}\\
PROD & == & \altype{Product} P\\
\end{aswhere}
#endif

macro {
	SI == MachineInteger;
	Z  == Integer;
	V  == Vector F;
	M  == DenseMatrix F;
	PL == Product P;
	RR == FractionalRoot F;
	ARR == PrimitiveArray;
}

PrimeFieldUnivariateFactorizer(F:PrimeFieldCategory0,
				P:UnivariatePolynomialAlgebra0 F): with {
	berlekamp:		P -> List P;
#if ALDOC
\alpage{berlekamp}
\Usage{\name~p}
\Signature{P}{List P}
\Params{ {\em p} & P & The square free and monic polynomial to factor.\\ }
\Retval{Returns the list of irreducible factors of $p$.}
#endif

	cantorZassenhaus:	P -> List P;
#if ALDOC
\alpage{cantorZassenhaus}
\Usage{\name~p}
\Signature{P}{List P}
\Params{ {\em p} & P & The square free and monic polynomial to factor.\\ }
\Retval{Returns the list of irreducible factors of $p$.}
#endif

	roots:			(P, Boolean) -> Generator RR;
#if ALDOC
\alpage{roots}
\Usage{\name(p, sqrfree?)}
\Signature{(P, \altype{Boolean}}{\altype{Generator} \altype{FractionalRoot} F}
\Params{
{\em p} & P & A polynomial.\\
{\em sqfree?} & \altype{Boolean} & Indicates whether p is squarefree\\
}
\Retval{Returns the roots of $p$ in F with their multiplicities. Assumes
that $p$ is squarefree if $sqrfree?$ is \true.}
#endif
	factor:			P -> (F, PL);
	factor:			(P->List P) -> (P->(F, PL));
#if ALDOC
\alpage{factor}
\Usage{\name~p\\ \name(e)(p)}
\Signatures{
\name: & P $\to$ (F, Product P)\\
\name: & (P $\to$ List P) $\to$ (P $\to$ (F, Product P))\\
}
\Params{
{\em p} & P & The polynomial to factor.\\
{\em e} & P $\to$ List P & The factorization engine to use. 
}
\Retval{Returns $(c, p_1^{e_1} \cdots p_n^{e_n})$ such that
each $p_i$ is irreducible, the $p_i$'s have no common factors, and
$$
p = c\;\prod_{i=1}^n p_i^{e_i}\,.
$$
}
\Remarks{Uses the factorizer {\em e} for factoring the monic squarefree
factors of $p$.}
#endif
} == add {
	local charac:Z		== characteristic$P;
	local char2?:Boolean	== even? charac;

	-- remove the first occurence of e from l
	local removefromlist(l:List P, e:P):List P == {
		empty? l => l;
		o := first l;
		e = o => rest l;
		cons(o, removefromlist(rest l, e));
	}

	local column2poly(mat:M, j:SI):P == {
		import from Z, F, V;
		a := column(mat, j);
		m := #a;
		p:P := 0;
		for i in 1..m repeat p := add!(p, a i, (i-1)::Z);
		times!(inv leadingCoefficient p, p);
	}

	berlekamp(a:P):List P == {
		import from Boolean, Z, F, M, LinearAlgebra(F, M);

		assert(one? leadingCoefficient a);
		assert(one? gcd(a,differentiate a));
		n := machine degree a;
		Q:M := zero(n,n);
		r:M := zero(1,n);
		rn:M := zero(1,n);
		l:M := zero(1,n);
		for j in 1..n repeat l(1,j) := coefficient(a,(j-1)::Z);
		r(1,1) := 1@F;
		for j in 1..n repeat Q(1,j) := r(1,j);
		for m in charac+1..(n::Z)*charac repeat {
			rn(1,1) := -r(1,n)*l(1,1);
			for j in 2..n repeat {
				rn(1,j) := r(1,j-1) - r(1,n)*l(1,j);
			}
			for j in 1..n repeat r(1,j) := rn(1,j);
			if zero?(m rem charac) then {
				k := machine(m quo charac);
				for j in 1..n repeat Q(k,j) := r(1,j);
			}
		}
		TRACE("Q = ", Q);
		V0 := kernel(transpose(Q)+(-one(n)));
		TRACE("V0 = ", V0);
		f:List P := [a];
		rr:SI := 1;
		kk := numberOfColumns V0;
		TRACE("Kernel dimension = ", kk);
		while #f < kk repeat {
			TRACE("poly # ", rr);
			TRACE("poly = ",column2poly(V0,rr));
			ff := copy f;
			while ~empty? ff repeat {
				u := first ff;
				ff := rest ff;
				--TEMPORARY:BAD OPTIMIZATION BUG (WAS 1203/1232)
				-- for i in 0..charac-1 repeat {
				i:Z := 0; while i < charac repeat {
					s := i::F;
					TRACE("s = ",s);
					g := gcd(column2poly(V0,rr) - s::P, u);
					TRACE("gcd = ", g);
					if g ~= 1 and g ~= u and g ~= 0 and
				  	  degree g ~= 0 and
					    degree g ~= degree u then {
						TRACE("found a factor", "");
						uu := u quo g;
						f:=removefromlist(f,u);
						f:=cons(g,f);
						f:=cons(uu,f);
						ff:=cons(g,ff);
						u := uu;
					}
					#f = kk => return f;
					i := next i;
				}
			}
			rr := next rr;
			rr > kk => return f;
		}
		f;
	}

	-- a must be squarefree on entry
	local probsplit(a:P, d:Z):List P == {
		macro EXT == UnivariatePolynomialModSqfr(F, P, a);
		import from SI, Z, F, EXT;
		TRACE("pffactor::probsplit, a = ", a);
		TRACE("::d = ", d);
		assert(one? gcd(a,differentiate a));
		assert(one? leadingCoefficient a);
		dv := degree a;
		assert(zero?(dv rem d));
		r:List P := empty;
		d >= dv => cons(a, r);
		xponent := quotient(charac^d - 1, 2);
		count:SI := 0;
		nterms := max(d, 3);
		repeat {
			t:P := random(2 * d - 1, nterms);
			count := next count;
			TRACE("::count = ", count);
			TRACE("::t = ", t);
			tt := reduce(t)^xponent;
			(t, ap, dummy) := gcdquo(a, lift(tt) - 1);
			dt := degree t;
			if dt ~= 0 and dt ~= dv then {
				l1 := probsplit(t, d);
				l2 := probsplit(ap, d);
				return append!(l1, l2);
			}
		}
	}

	-- look for factors of degree d, d+1, etc.. of v until
	-- either d is greater than deg(v)/2 or a nontrivial factor is found
	-- returns (1 + degree found, poly remaining to factor,
	--          x^(p^degree found), new factors found)
	-- on entry: x = monom, d = degree to look for, v = sqfr poly to factor,
	--           w = x^(p^(d-1)) mod v,  f = prod of factors found
	local exhaust!(h:ARR P, store?:Boolean, x:P, d:SI, v:P, bound:SI, w:P,
		f:PL):(SI, P, P, PL) == {
		TIMESTART;
		macro EXT == UnivariatePolynomialModSqfr(F, P, v);
		import from Z, EXT, PL;
		ww := reduce w;
		while d <= bound repeat {
			ww := pthPower! ww;		-- ww = x^(p^d)
			TIME("pffactor::exhaust!: p-th power at ");
			w := lift ww;
			if store? then h.d := w;
			(g, dummy, newv) := gcdquo(w - x, v);
			TIME("pffactor::exhaust!: gcd(w-x,v) at ");
			degree g > 0 =>	{		-- factors of degree d
				TIME("pffactor::exhaust!: done at ");
				return(next d, newv, remainder!(w, newv),
						times!(f, g, d::Z));
			}
			d := next d;
		}
		TIME("pffactor::exhaust!: done at ");
		(d, v, w, f);
	}

#if SHOUP
	-- v is squarefree and monic
	local shoup!(h:ARR P,H:ARR P,j:SI,v:P,f:PL,l:SI,m:SI):(SI,P,PL) == {
		TIMESTART;
		macro EXT == UnivariatePolynomialModSqfr(F, P, v);
		import from EXT, PL;
		TRACE("pffactor::shoup!: j = ", j);
		TRACE("pffactor::shoup!: l = ", l);
		TRACE("pffactor::shoup!: m = ", m);
		dv:SI := machine degree v;
		TRACE("pffactor::shoup!: degree of modulus = ", dv);
		w := reduce(H.0);
#if MODCOMP
		comp := compose w;			-- comp(g) = g(x^(p^l))
		TIME("pffactor::shoup!: compose at ");
#endif
		if j > 1 then {
			if j > 2 then w := reduce H(prev prev j);
#if MODCOMP
			w := comp w;
#else
			for i in 1..l repeat w := pthPower! w;
#endif
			H(prev j) := lift w;
			TIME("pffactor::shoup!: H.j at ");
		}
		found?:Boolean := false;
		cont?:Boolean := (2 * (1 + l * prev j)) <= dv;
		while cont? repeat {
			newv := v;
			for i in 0..prev l repeat {
				lmi := l - i;
				(g, b, newv) :=
					gcdquo(H(prev j) - h(prev lmi), newv);
				TIME("pffactor::shoup!: gcd at ");
				TRACE("pffactor::shoup!: |gcd| = ",degree g);
				if degree(g) > 0 then {
					found? := true;
					e := l * j - prev lmi;
					f := times!(f, g, e::Z);
				}
			}
			found? => {
				-- reduce powers of x mod new modulus
				for i in 0..prev l repeat h.i := h.i rem newv;
				for i in 0..prev j repeat H.i := H.i rem newv;
				TIME("pffactor::shoup!: done at ");
				return(next j, newv, f);
			}
			j := next j;
			TRACE("pffactor::shoup!: j = ", j);
			cont? := (2 * (1 + l * prev j)) <= dv;
			if cont? then {
#if MODCOMP
				w := comp w;
#else
				for i in 1..l repeat w := pthPower! w;
#endif
				H(prev j) := lift w;
				TIME("pffactor::shoup!: H.j at ");
			}
		}
		TIME("pffactor::shoup!: done at ");
		(j, v, f);
	}
#endif

	-- a is squarefree and monic
	local distdeg(a:P):PL == {
		assert(one? gcd(a, differentiate a));
		assert(one? leadingCoefficient a);
		import from Boolean, SI, Z, F, ARR P;
		w := x := monom;
		f:PL := 1;
		B:SI := machine(degree(a) quo 2);
		d:SI := 1;
#if SHOUP
		(ignore?, l) := nthRoot(B, 2);  -- l^2 <= B < (l+1)^2
#else
		l:SI := 0;
#endif
		h:ARR P := new next l;		-- h.i will be x^(p^i) mod a
		h.0 := x;

#if SHOUP
		-- h := pthPowers(x, a, l);	-- h.i = x^(p^i) mod a
		while d <= min(l, B) repeat {
			(d,a,w,f) := exhaust!(h, true, x, d, a, min(l,B), w, f);
			B := machine(degree(a) quo 2);
		}
		m := B quo l;
		if (m * l) < B then m := next m;
		H:ARR P := new m;
		H.0 := h.l;			-- x^(p^l) mod a
		j:SI := 1;
		while ((2 * (1 + l * prev j))::Z) <= degree a repeat
			(j, a, f) := shoup!(h, H, j, a, f, l, m);
#else
		while d <= B repeat {
			(d, a, w, f) := exhaust!(h, false, x, d, a, B, w, f);
			B := machine(degree(a) quo 2);
		}
#endif
		one? a => f;
		times!(f, a, degree a);
	}

	cantorZassenhaus(a:P):List P == {
		TRACE("pffactor::cantorZassenhaus: ", a);
		assert(one? gcd(a, differentiate a));
		assert(one? leadingCoefficient a);
		import from F,PL,Z;
		TIMESTART;
		f := distdeg a;
		TIME("distinct degree at ");
		TRACE("::disting degree factorization = ", f);
		r:List P := empty;
		for t in f repeat {
			(p,v) := t;
			r := append!(r, probsplit(p,v));
			TIME("probsplit at ");
		}
		TIME("factorisation at ");
		TRACE("pffactor::cantorZassenhaus returns ", r);
		r;
	}

	-- for monic squarefreee polynomials only
	local linearFactors(a:P):List P == {
		import from Boolean, SI, F, ARR P;
		assert(one? gcd(a,differentiate a));
		assert(one? leadingCoefficient a);
		one? degree a => cons(a, empty);
		w := x := monom;
		d:SI := 1;
		f:PL := 1;
		(d, aa, w, f) := exhaust!(new 1, false, x, 1, a, 1, w, f);
		f = 1 => empty; 
		for t in f repeat (p, dp) := t;
		dp ~= 1 => empty;
		-- TRACE("probsplit of ", p);
		probsplit(p, 1);
	}

	roots(a:P, sqrfree?:Boolean):Generator RR == {
		sqrfree? => rootsSqfr a;
		roots0 a;
	}

	local roots0(a:P):Generator RR == generate {
		import from Boolean, F, List F, RR, Product P;
		lc := leadingCoefficient a;
		aa := inv(lc)*a;
		(dummy, l) := squareFree aa;
		assert(one? dummy);
		for t in l repeat {
			(p, e) := t;
			if ~zero?(degree p) then
				for rt in rootsSqfrMonic p repeat
					yield integralRoot(rt, e);
		}
	}

	-- for squarefree polynomials only!
	local rootsSqfr(a:P):Generator RR == generate {
		import from Boolean, Z, F, RR;
		assert(one? gcd(a, differentiate a));
		if ~one?(lc := leadingCoefficient a) then a := inv(lc) * a;
		for rt in rootsSqfrMonic a repeat yield integralRoot(rt, 1);
	}

	-- for monic squarefree polynomials only!
	local rootsSqfrMonic(a:P):Generator F == generate {
		import from F, List P;
		assert(one? gcd(a, differentiate a));
		assert(one? leadingCoefficient a);
		for f in linearFactors a repeat {
			assert(one? leadingCoefficient f);
			yield(- coefficient(f, 0));
		}
	}

	factor(a:P):(F, PL) == {
		char2? => factor(berlekamp)(a);
		factor(cantorZassenhaus)(a);
	}

	factor(engine:P->List P)(a:P):(F, PL) == {
		import from Boolean, F, List P, List Cross(P, Z), Product P;
		-- TRACE("factor ", a);
		lc := leadingCoefficient a;
		aa := inv(lc)*a;
		(dummy,l) := squareFree aa;
		-- TRACE("squarefree factorization = ", l);
		-- TRACE("lcoeff = ", dummy);
		assert(dummy=1);
		r:PL := 1;
		for t in l repeat {
			(p,e) := t;
			if zero? degree p then {
				lc := lc * (leadingCoefficient p)^e;
				iterate;
			}
			l1 := engine p;
			while ~empty? l1 repeat {
				f := first l1;
				l1 := rest l1;
				if f~=1 then r:=times!(r,f,e);
			}
		}
		-- TRACE("factor:returning lc = ", lc);
		-- TRACE("and ", r);
		(lc,r);
	}				
}
