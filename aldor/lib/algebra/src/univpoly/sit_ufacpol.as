----------------------------- sit_ufacpol.as ----------------------------------
-- Copyright (c) Manuel Bronstein 2000
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

macro {
	I == MachineInteger;
	Z == Integer;
	FR == FractionalRoot;
	TREE == ExpressionTree;
}

#if ALDOC
\thistype{UnivariateFactorialPolynomial}
\History{Manuel Bronstein}{16/6/2000}{created}
\Usage{ import from \this(R, Rx)}
\Params{
{\em R} & \altype{Ring} & The coefficient domain\\
{\em Rx} & \altype{UnivariatePolynomialAlgebra} R & A polynomial type over R\\
}
\Descr{\this(R, Rx) implements univariate factorial polynomials with
coefficients in $R$.
Those are polynomials with respect to the basis
of the descending factorials $(x^{{\underline n}})_{n \ge 0}$,
where $x^{{\underline n}} = x (x-1) \dots (x-n+1)$.
Rx is used for representing
the factorial polynomials, so you can choose between sparse and dense
representations.}
\begin{exports}
\category{\altype{UnivariateFreeRing} R}\\
\alexp{coerce}: & Rx $\to$ \% & Conversion to a factorial polynomial\\
\alexp{expand}: & \% $\to$ Rx & Conversion from a factorial polynomial\\
\alexp{trailExpand}:
& \% $\to$ (\altype{Integer}, Rx) & Conversion from a factorial polynomial\\
\end{exports}
\begin{exports}[if $R$ has \altype{CommutativeRing} then]
\category{\altype{CommutativeRing}}\\
\end{exports}
\begin{exports}[if $R$ has \altype{IntegralDomain} then]
\category{\altype{IntegralDomain}}\\
\end{exports}
\begin{exports}[if $R$ has \altype{RationalRootRing} then]
\alexp{integerRoots}:
& \% $\to$ \altype{List} \altype{FractionalRoot} \altype{Integer} &
Integer roots\\
\alexp{rationalRoots}:
& \% $\to$ \altype{List} \altype{FractionalRoot} \altype{Integer} &
Rational roots\\
\end{exports}
#endif

UnivariateFactorialPolynomial(R:Ring, Rx: UnivariatePolynomialAlgebra R):
	UnivariateFreeRing R with {
		if R has CommutativeRing then CommutativeRing;
		if R has IntegralDomain then IntegralDomain;
		coerce: Rx -> %;
		expand: % -> Rx;
		trailExpand: % -> (Z, Rx);
#if ALDOC
\alpage{coerce,expand,trailExpand}
\altarget{coerce}
\altarget{expand}
\altarget{trailExpand}
\Usage{p::\%\\ coerce~p\\ expand~q\\ (n, h) := trailExpand~q}
\Signatures{
coerce: & Rx $\to$ \%\\
expand: & \% $\to$ Rx\\
trailExpand: & \% $\to$ (\altype{Integer}, Rx)\\
}
\Params{
{\em p} & Rx & A polynomial\\
{\em q} & \% & A factorial polynomial\\
}
\Descr{p::\% converts {\em p} from the power basis $(x^n)_{n \ge 0}$ to
the factorial basis $(x^{{\underline n}})_{n \ge 0}$, while expand(q)
performs the reverse conversion and trailExpand(q) returns $(n, h)$
such that $q = x^{{\underline n}} h$.}
#endif
		if R has RationalRootRing then {
			integerRoots: % -> List FR Z;
			rationalRoots: % -> List FR Z;
#if ALDOC
\alpage{integerRoots,rationalRoots}
\altarget{integerRoots}
\altarget{rationalRoots}
\Usage{ integerRoots~p\\ rationalRoots~p }
\Signature{\%}{\altype{List} \altype{FractionalRoot} \altype{Integer}}
\Params{ {\em p} & \% & A factorial polynomial\\ }
\Retval{Return $[(r_1,e_1),\dots,(r_n,e_n)]$ where the $r_i$'s are
the integer or rational roots of $p$ and have multiplicity $e_i$.}
#endif
		}
} == Rx add {
	local minus1:TREE			== extree(-$R(1$R))$R;
	local monom1:TREE			== extree(monom$Rx)$Rx;
	extree(p:%):TREE			== p monom1;
	(port:TextWriter) << (p:%): TextWriter	== tex(port, extree p);

	-- very naive for now
	(p:%) * (q:%):% == {
		import from Z, R;
		zero? p or zero? q => 0;
		one? p => q; one? q => p;
		dp := degree p; dq := degree q;
		prod:% := 0;
		for i in dp..0 by -1 repeat {
			for j in dq..0 by -1 repeat {
				c := coefficient(p, i) * coefficient(q, j);
				prod := add!(prod, c, xixj(i, j));
			}
		}
		prod;
	}

	if R has RationalRootRing then {
		integerRoots(p:%):List FR Z == {
			import from Boolean, Rx;
			assert(~zero? p);
			(n, q) := trailExpand p;
			merge!([integerRoots q], n);
		}

		rationalRoots(p:%):List FR Z == {
			import from Boolean, Rx;
			assert(~zero? p);
			(n, q) := trailExpand p;
			merge!([rationalRoots q], n);
		}

		-- merge l with [0,1,...,n-1]
		local merge!(l:List FR Z, n:Z):List FR Z == {
			import from FR Z;
			assert(n >= 0);
			for i in 0..prev n repeat {
				if empty?(ll := find(i, l)) then
					l := cons(integralRoot(i, 1), l);
				else setMultiplicity!(first ll,
						next multiplicity first ll);
			}
			l;
		}

		local find(n:Z, l:List FR Z):List FR Z == {
			import from Boolean, FR Z;
			 while ~empty?(l) repeat {
				integral?(r := first l) and
						n = integralValue r => return l;
				l := rest l;
			}
			empty;
		}
	}

	expand(p:%):Rx == {
		import from Z;
		zero? p => 0;
		expand0(p, 0);
	}

	trailExpand(p:%):(Z, Rx) == {
		zero? p => (-1, 0);
		e := trailingDegree p;
		(e, expand0(p, e));
	}

	-- expands p / x^{_e_} when that quotient is exact
	local expand0(p:%, e:Z):Rx == {
		import from Boolean, R;
		assert(~zero? p);
		assert(e <= trailingDegree p);
		x:Rx := monom;
		d := degree(p) - e;
		q:Rx := monomial next d;	-- to allocate memory
		m := e;				-- xm will be x^{_m_}/x^{_e_}
		xm:Rx := 1 + q;			-- also allocates memory
		xm := add!(xm, -1, next d);	-- xm = 1 with space for x^{d+1}
		for term in terms p repeat {	-- in increasing order
			(c, n) := term;
			assert(n >= m);
			-- x^{_n_}/x^{_e_} = x^{_m_}/x^{_e_} (x-m)...(x-n+1)
			for i in m..prev n repeat xm := times!(xm, x - i::Rx);
			q := add!(q, c, xm);
			m := n;
		}
		add!(q, -1, next d);
	}

	coerce(p:Rx):% == {
		import from R;
		zero? p => 0;
		d := degree p;
		q:% := monomial next d;		-- to allocate memory
		m:Z := 0;
		xm:% := 1 + q;			-- also allocates memory
		xm := add!(xm, -1, next d);	-- xm = 1 with space for x^{d+1}
		for term in terms p repeat {	-- in increasing order
			(c, n) := term;
			assert(n >= m);
			-- if x^n = \sum_{i=0}^n a_i x^{_i_} then
			-- x^{n+1} = \sum_{i=0}^{n+1} (a_{i-1}+i a_i) x^{_i_}
			for i in m..prev n repeat {
				dp := differentiate(xm pretend Rx) pretend %;
				xm := shift!(add!(xm, dp), 1);
			}
			q := add!(q, c, xm);
			m := n;
		}
		add!(q, -1, next d);
	}

	apply(p:%, x:TREE):TREE == {
		import from Boolean, R;
		zero? p => extree(0@R);
		l:List(TREE) := empty;
		for term in p repeat {
			(c, n) := term;
			l := cons(tree(c, n, x), l);
		}
		assert(~empty? l);
		empty? rest l => first l;
		ExpressionTreePlus reverse! l;
	}

	local tree(c:R, n:Z, x:TREE):TREE == {
		import from Boolean, R, List TREE;
		assert(~zero? c);
		negative?(tc := extree c) =>
			ExpressionTreeMinus [tree(c ~= -1, negate tc, n, x)];
		tree(c ~= 1, tc, n, x);
	}

	local tree(tims?:Boolean, c:TREE, n:Z, x:TREE):TREE == {
		import from R, List TREE;
		zero? n => c;
		if n > 1 then x := ExpressionTreeFactorial [x,minus1,extree n];
		tims? => ExpressionTreeTimes [c, x];
		x;
	}

	-- Use the formula
	--   x^{{\underline i}} x^{{\underline j}} =
	--   \sum_{k=max(i,j)}^{i+j}
	--            j^{{\underline i+j-k}} {i \choose k-j} x^{{\underline k}}
	local xixj(i:Z, j:Z):% == {
		import from I, R;
		ans:% := 0;
		low := max(i, j);
		high := i + j;
		for k in high..low by -1 repeat {
			c := factorial(j,-1,machine(high-k)) * binomial(i,k-j);
			ans := add!(ans, c::R, k::Z);
		}
		ans;
	}
}
