----------------------------- sit_chrem.as ------------------------------------
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"

macro SI == MachineInteger;

#if ALDOC
\thistype{ChineseRemaindering}
\History{Laurent Bernardin}{17/8/95}{created}
\History{Manuel Bronstein}{19/4/96}{
Curried up combine function for caching the modular inversion.
Added special signature for bigint/machine-int moduli pairs.}
\Usage{import from \this R}
\Params{
{\em R} & \altype{EuclideanDomain} & an Euclidean Domain.\\
}
\Descr{\this~provides Garner's Chinese Remaindering Algorithm implemented
over an arbitrary \altype{EuclideanDomain}.}
\begin{exports}
\alexp{combine}: & (R,R) $\to$ (R,R) $\to$ R &
combine interpolated result with new modulus.\\
\alexp{interpolate}: & (\altype{List} R,\altype{List} R) $\to$ R &
interpolate given residues and moduli.\\
\end{exports}
\begin{exports}[if R has \altype{IntegerType} then]
\alexp{combine}:
& (R,\altype{MachineInteger}) $\to$ (R,\altype{MachineInteger}) $\to$ R &
combine with new modulus.\\
\end{exports}
#endif

ChineseRemaindering(R:EuclideanDomain): with {
	combine:	(R,R) -> (R,R) -> R;
	if R has IntegerType then {
		combine: (R, SI) -> (R, SI) -> R;
	}
#if ALDOC
\alpage{combine}
\Usage{\name(M,m)\\ \name(M,m)(A,a)}
\Signatures{
\name: & (R,R) $\to$ (R,R) $\to$ R\\
\name: & (R,\altype{MachineInteger}) $\to$ (R,\altype{MachineInteger}) $\to$ R\\
}
\Params{
{\em M} & R & A product of primes.\\
{\em m} & R & A new modulus.\\
        & \altype{MachineInteger} & \\
{\em A} & R & The interpolated value modulo {\em M}.\\
{\em a} & R & The residue modulo {\em m}.\\
        & \altype{MachineInteger} & \\
}
\Retval{
Returns the unique $X$ in $R/(mM)$ such that
$X = A \pmod M$ and $X = a \pmod m$.
}
#endif
	interpolate:	(List R,List R)	->	R;
#if ALDOC
\alpage{interpolate}
\Usage{\name(p,m)}
\Signature{(\altype{List} R,\altype{List} R)}{R}
\Params{ {\em p} & \altype{List} R & A list of residues.\\
	 {\em m} & \altype{List} R & A list of moduli.}
\Retval{Returns the interpolated value from the residues and the
corresponding moduli.}
#endif
} == add {
	local mustBalance?:Boolean	== R has IntegerType;
	local maxhalf:SI		== shift(1, prev(4 * bytes$SI));

	combine(M:R, m:R):(R,R) -> R == {
		import from Integer, Partial R;
		TRACE("computing inverse of ", M rem m);
		TRACE("modulo ",m);
		s := retract diophantine(M rem m, 1, m);
		mover2:R := { mustBalance? => m quo (2::R); 0 };
		TRACE("result=",s);
		(A:R, a:R):R +-> {
			b := (s * (a - (A rem m))) rem m;
			-- the following is only for integer-like rings
			if mustBalance? and greater?(b, mover2) then b := b - m;
			A + b * M;
		}
	}

	if R has IntegerType then {
		local greater?(a:R, b:R):Boolean == a > b;

		combine(M:R, m:SI):(R,SI) -> R == {
			import from Integer;
			assert(M > 0); assert(m > 0); assert(odd? m);
			mover2 := shift(m, -1);
			s := modInverse(M mod m, m);
			m > maxhalf => {
				(A:R, a:SI):R +-> {
					-- all args to mod_X must be positive!
					if a < 0 then a := a + m;
					aminusA := mod_-(a, A mod m, m);
					b := mod_*(s, aminusA, m);
					-- balance out result
					if b > mover2 then b := b - m;
					A + (b::R) * M;
				}
			}
			(A:R, a:SI):R +-> {
				-- all args to mod_X must be positive!
				if a < 0 then a := a + m;
				aminusA := mod_-(a, A mod m, m);
				b := (s * aminusA) rem m;
				-- balance out result
				if b > mover2 then b := b - m;
				A + (b::R) * M;
			}
		}
	}

	interpolate(p:List R, m:List R):R == {
		import from SI;
		assert(#p=#m);
		assert(#p>0);
		a := first p; p := rest p;
		M := first m; m := rest m;
		while ~empty? p repeat {
			np := first p; p := rest p;
			nm := first m; m := rest m;
			a  := combine(M, nm)(a, np);
			M  := nm * M;
		}
		a;
	}
}

#if ALDORTEST
------------------ test chrem.as -----------------
#include "algebra"
#include "aldortest"

macro {
	Z == Integer;
	F == SmallPrimeField 11;
	P == DenseUnivariatePolynomial F;
}

local inttest():Boolean == {
	import from Z, List Z, ChineseRemaindering Z;

	a := interpolate([4551,4671],[7927,7919]);
	a = 123456 => true;
	false;
}

local polytest():Boolean == {
	import from MachineInteger, Z, P, ChineseRemaindering P, List P;

	x := monom;
	a := interpolate([6*1,0,7*1],[x-1,x-2*1,x-3*1]);
	a = x*x+2*x+3*1 => true;
	false;
}

stdout << "Testing sit_chrem..." << endnl;
aldorTest("CRT for integers",inttest);
aldorTest("CRT for polynomials",polytest);
stdout << endnl;
#endif
