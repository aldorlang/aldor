----------------------------- sit_duts.as ----------------------------------
-- Copyright (c) Manuel Bronstein 2000
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

macro {
	I == MachineInteger;
	Z == Integer;
	V == Vector;
	TREE == ExpressionTree;
	SEQ == Sequence;
}

#if ALDOC
\thistype{DenseUnivariateTaylorSeries}
\History{Manuel Bronstein}{30/5/2000}{created}
\History{Manuel Bronstein}{22/7/2003}{added dot}
\Usage{ import from \this~R\\ import from \this(R, x) }
\Params{
{\em R} & \altype{ExpressionType} & The coefficient domain\\
        & \altype{ArithmeticType} &\\
{\em x} & \altype{Symbol} & The variable name (optional)\\
}
\Descr{\this(R, x) implements univariate Taylor series with coefficients
in R.}
\begin{exports}
\category{\altype{UnivariateTaylorSeriesType} R}\\
\end{exports}
#endif

DenseUnivariateTaylorSeries(R:Join(ArithmeticType, ExpressionType),
	avar:Symbol == new()): UnivariateTaylorSeriesType R == SEQ R add {
	Rep == SEQ R;
	import from Rep;

	local seq(s:%):SEQ R	== rep s;
	finite?(s:%):Boolean	== finite? seq s;
	series(s:SEQ R):%	== per s;
	extree(s:%):TREE	== s extree avar;
	local dummy:Symbol	== { import from String; -"dummy"; }
	(p:TextWriter) << (s:%):TextWriter == p(s, avar);

	dot(w:V R):V % -> % == {
		f := dot(w)$Rep;
		(v:V %):% +-> series f(v pretend V Rep);	-- always safe
	}

	apply(s:%, x:TREE):TREE == {
		import from Boolean, I, Z, R, List TREE;
		import from UnivariateMonomial(R, dummy);
		zero? s => extree(0@R);
		l:List(TREE) := empty;
		n := #(rs := seq s);
		for i in 0..prev n | ~zero?(c := rs.i) repeat {
			m := monomial(c, i::Z)@UnivariateMonomial(R, dummy);
			l := cons(m x, l);
		}
		if (b := bound rs) < 0 or n < b then {
			m := monomial(1, n::Z)@UnivariateMonomial(R, dummy);
			l := cons(ExpressionTreeBigO [m x], l);
		}
		empty? l => extree(0@R);
		empty? rest l => first l;
		ExpressionTreePlus reverse! l;
	}

	degree(s:%):Partial Z == {
		import from I, Z;
		(n := bound seq s) < 0 => failed;
		[prev(n)::Z];
	}

	if R has CommutativeRing then {
		if R has RittRing then {
			integrate(s:%, k:Z):% == {
				import from I, R, Stream R;
				n := machine k;
				assert(n >= 0);
				zero? n => s;
				rs := seq s;
				(cs := bound rs) < 0 =>
					series sequence stream(0, int(rs, n));
				series sequence stream(0, int(rs, n), cs+n, 0);
			}

			local int(s:SEQ R, n:I)(k:I):R == {
				import from Z;
				(m := k - n) < 0 => 0;
				inv(factorial(k::Z, -1, n)) * s.m;
			}
		}

		differentiate(s:%, k:Z):% == {
			import from I, R, Stream R;
			n := machine k;
			assert(n >= 0);
			zero? n => s;
			rs := seq s;
			(cs := bound rs) < 0 =>
				series sequence stream(0, diff(rs, n));
			(e := cs - n) <= 0 => 0;
			series sequence stream(0, diff(rs, n), e, 0);
		}

		local diff(s:SEQ R, n:I)(k:I):R == {
			import from Z;
			m := k + n;
			factorial(m::Z, 1, n) * s.m;
		}
	}

	(s:%) * (t:%):% == {
		import from I, R, Stream R;
		zero? s or zero? t => 0;
		cs := bound(rs := seq s);
		ct := bound(rt := seq t);
		cs < 0 or ct < 0 => series sequence stream(0, naive(rs, rt));
		assert(cs > 0); assert(ct > 0);
		series sequence stream(0, naive(rs, rt), prev(cs + ct), 0);
	}

	local naive(s:SEQ R, t:SEQ R): I -> R == {
		(m:I):R +-> {
			r:R := 0;
			for i in 0..m repeat r := add!(r, s.i * t(m-i));
			r;
		}
	}

	-- CANNOT RELY ON DEFAULTS BECAUSE THE ONE IN Sequence IS TAKEN FIRST
	(s:%)^(n:I):% == {
		import from BinaryPowering(%, I);
		binaryExponentiation(s, n);
	}

	-- CANNOT RELY ON DEFAULTS BECAUSE THE ONE IN Sequence IS TAKEN FIRST
	(s:%)^(n:Z):% == {
		import from BinaryPowering(%, Z);
		binaryExponentiation(s, n);
	}
}

