------------------------------- sit_product.as -------------------------------
--
-- Formal products, used for example for factorisation results
--
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{Product}
\History{Manuel Bronstein}{6/6/95}{created}
\Usage{import from \this~R}
\Params{ {\em R} & \altype{CommutativeRing} & A commutative ring\\ }
\Descr{\this~R provides finite products of elements of R,~\ie elements
of the type $\prod_{i=1}^n r_i^{e_i}$ where the $r_i$'s are in R and the
$e_i$'s are integers.}
\begin{exports}
\category{\altype{CopyableType}}\\
\category{\altype{Monoid}}\\
\alalias{\this}{size}{\#}:
& \% $\to$ \altype{MachineInteger} & Number of terms\\
\alexp{divisors}:
& \% $\to$ \altype{Generator} R & Iterate through all the divisors\\
\alexp{expand}: & \% $\to$ R & Multiply-out a product\\
\alexp{expandFraction}: & \% $\to$ (R, R) & Multiply-out a product\\
\alexp{generator}:
& \% $\to$ \altype{Generator} \builtin{Cross}(R, \altype{Integer}) &
Make an iterator\\
\alexp{log}: & (M:\altype{AbelianMonoid}, R $\to$ M) $\to$ (\% $\to$ M) &
Lift a logarithm\\
\alexp{term}: & (R, \altype{Integer}) $\to$ \% & Create a single term $r^e$\\
\alexp{times!}: & (\%, R, \altype{Integer}) $\to$ \% & In-place multiplication\\
\end{exports}
#endif

macro {
	A	== PrimitiveArray;
	Z	== Integer;
}

Product(R:CommutativeRing): Join(CopyableType, Monoid) with {
	#: % -> MachineInteger;
#if ALDOC
\alpage{size}
\Usage{\#~p}
\Signatures{ \#: & \% $\to$ \altype{MachineInteger}\\ }
\Params{ {\em p} & \% & A product\\ }
\Retval{Returns the number of terms in the product p.}
#endif
	divisors: % -> Generator %;
#if ALDOC
\alpage{divisors}
\Usage{ for d in \name~p repeat \{ \dots \} }
\Signature{\%}{\altype{Generator} R}
\Params{ {\em p} & \% & A product\\ }
\Descr{This generator yields all the products of the form
$\prod_{i=1}^n r_i^{f_i}$ where $p = \prod_{i=1}^n r_i^{e_i}$
and $0 \le f_i \le e_i$.}
\begin{asex}
\begin{ttyout}
import from Integer, Product Integer, List Integer;

p := term(3, 1) * term(2, 2) * term(5, 2)       -- p = 3^1 2^2 5^2 = 300
l := sort! [divisors p];
\end{ttyout}
creates the list
\begin{asoutput}
\> [1,2,3,4,5,6,10,12,15,20,25,30,50,60,75,100,150,300]\\
\end{asoutput}
of all the divisors of $300$.
\end{asex}
#endif
	expand: % -> R;
#if ALDOC
\alpage{expand}
\Usage{\name~p}
\Signature{\%}{R}
\Params{ {\em p} & \% & A product\\ }
\Retval{Returns the product of all the terms in p.}
\Remarks{When R is not a field, \name(p) only works when p has only
nonnegative exponents. Use \alexp{expandFraction} when p can have
negative exponents and R is not a field.}
#endif
	expandFraction: % -> (R, R);
#if ALDOC
\alpage{expandFraction}
\Usage{\name~p}
\Signature{\%}{(R, R)}
\Params{ {\em p} & \% & A product\\ }
\Retval{Returns $(n, d)$ where \emph{n} is the product of all the terms in
\emph{p} having positive exponents and \emph{d} is the product of all terms in
\emph{p} having negative exponents.}
\alseealso{\alexp{expand}}
#endif
	generator: % -> Generator Cross(R, Z);
#if ALDOC
\alpage{generator}
\Usage{
for term in p repeat \{ (c, n) := term; \dots \}\\
for term in \name~p repeat \{ (c, n) := term; \dots \}
}
\Signature{\%}{\altype{Generator} \builtin{Cross}(R, \altype{Integer})}
\Params{ {\em p} & \% & A product\\ }
\Descr{This function allows a product to be iterated independently of its
representation. The generator yields pairs of the form $(a, n)$ where
$a^n$ is a term in $p$.}
\begin{asex}
\begin{ttyout}
import from Integer, Product Integer;

p := term(3, 1) * term(2, 11) * term(5, 2)       -- p = 3^1 2^11 5^2
for term in p repeat { (c, n) := term; stdout << c << "," << n << newline; }
\end{ttyout}
writes
\begin{asoutput}
\> 3,1\\
\> 2,11\\
\> 5,2
\end{asoutput}
to the standard stream \alfunc{TextWriter}{stdout}.
\end{asex}
#endif
	log: (M:AbelianMonoid, R -> M) -> % -> M;
#if ALDOC
\alpage{log}
\Usage{\name(M,f)\\ \name(M,f)(p)}
\Signature{(M:\altype{AbelianMonoid}, R $\to$ M)}{\% $\to$ M}
\Params{
{\em M} & \altype{AbelianMonoid} & the image monoid\\
{\em f} & R $\to$ M & a logarithmic function on R\\
{\em p} & \% & A product\\
}
\Descr{\name(M,f)(p) returns $\sum_n n f(a_n)$ where
$p = \prod_n a_n^n$, while \name(M,f) returns the map
$\prod_n a_n^n \to \sum_n n f(a_n)$.}
#endif
	term: (R, Z) -> %;
#if ALDOC
\alpage{term}
\Usage{\name(r, n)}
\Signature{(R, \altype{Integer})}{\%}
\Params{
{\em r} & R & A ring element\\
{\em n} & \altype{Integer} & An exponent\\
}
\Retval{Returns $r^e$ as a product.}
#endif
	times!: (%, R, Z) -> %;
#if ALDOC
\alpage{times!}
\Usage{\name(p, r, n)}
\Signature{(\%, R, \altype{Integer})}{\%}
\Params{
{\em p} & \% & A product\\
{\em r} & R & A ring element\\
{\em n} & \altype{Integer} & An exponent\\
}
\Retval{Returns $p\; r^e$ as a product, where the storage used by p is allowed
to be destroyed or reused, so p is lost after this call.}
\Remarks{The storage used by p is allowed to be destroyed or reused, so p
is lost after this call. This may cause p to be destroyed, so do not use
this unless p has been locally allocated, and is thus guaranteed not
to share space with other polynomials. Some functions are not necessarily
copying their arguments and can thus create memory aliases.}
#endif
} == add {
	-- TEMPORARY: workaround to bug 1187
	local TERM: PrimitiveType with {
		maketerm: (R, Z) -> %;
		exponent: % -> Z;
		coefficient: % -> R;
	} == add {
		macro Rep == Record(trm:R, exp:Z);

		maketerm(x:R, n:Z):%	== { import from Rep; per [x, n] }
		exponent(t:%):Z		== { import from Rep; rep(t).exp }
		coefficient(t:%):R	== { import from Rep; rep(t).trm }

		(a:%) = (b:%):Boolean == {
			import from R, Z;
			exponent a = exponent b
				and coefficient a = coefficient b;
		}
	}

	macro {
		-- TEMPORARY: workaround to bug 1187
		-- TERM	== Record(trm:R, exp:Z);
		Rep	== List TERM;
	}

	import from TERM, Rep;

	1:%				== per empty;
	-- TEMPORARY: workaround to bug 1187
	-- local exponent(term:TERM):Z	== term.exp;
	-- local coefficient(term:TERM):R	== term.trm;
	#(p:%):MachineInteger		== # rep p;
	copy(p:%):%			== per copy rep p;
	divisors(x:%):Generator %	== div rep x;

	term(x:R, n:Z):% == {
		zero? n or one? x => 1;
		-- TEMPORARY: workaround to bug 1187
		-- per [[x, n]];
		per [maketerm(x, n)];
	}

	log(M:AbelianMonoid, rlog:R -> M)(x:%):M == {
		import from Z;
		m:M := 0;
		for t in x repeat {
			(c, n) := t;
			m := add!(m, n * rlog c);
		}
		m;
	}

	generator(x:%):Generator Cross(R, Z) == generate {
		for t in rep x repeat yield(coefficient t, exponent t);
	}

	local div(l:Rep):Generator % == generate {
		import from Z;
		if empty? l then yield 1;
		else {
			t := first l;
			c := coefficient t;
			n := exponent t;
			for d in div rest l repeat
				for i in 0..n repeat yield(term(c, i) * d);
		}
	}

	(x:%) ^ (m:Z):% == {
		import from R;
		y:% := 1;
		zero? m => y;
		for t in x repeat {
			(c, n) := t;
			if one? c then y := times!(y, c, n);
				else y := times!(y, c, m * n);
		}
		y;
	}

	-- very naive for now, no simplification whatsoever
	(a:%) * (b:%):% == {
		one? a => b; one? b => a;
		p := copy b;
		for t in a repeat {
			(c, n) := t;
			p := times!(p, c, n);
		}
		p;
	}

	times!(p:%, x:R, n:Z):% == {
		zero? n or one? x => p;
		per cons(first rep term(x, n), rep p);
	}

	-- very ineficient for now!
	(a:%) = (b:%):Boolean == {
		import from R;
		(na, da) := expandFraction a;
		(nb, db) := expandFraction b;
		na * db = nb * da;
	}

	expand(x:%):R == {
		a:R := 1;
		for t in x repeat {
			(c, n) := t;
			a := times!(a, c^n);
		}
		a;
	}

	expandFraction(x:%):(R, R) == {
		import from Z;
		n:R := 1;
		d:R := 1;
		for t in x repeat {
			(c, e) := t;
			if e > 0 then n := times!(n, c^e);
				else d := times!(d, c^(-e));
		}
		(n, d);
	}

	extree(x:%):ExpressionTree == {
		import from R, List ExpressionTree;
		l := [term2tree(coefficient t, exponent t) for t in rep x];
		empty? l => extree(1$R);
		empty? rest l => first l;
		ExpressionTreeTimes l;
	}

	local term2tree(g:R, c:Z):ExpressionTree == {
		import from List ExpressionTree;
		assert(c ~= 0);
		tg := extree g;
		one? c => tg;
		tc := extree c;
		ExpressionTreeExpt [tg, tc];
	}
}
