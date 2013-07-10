-------------------------- sit_resprs.as  ----------------------------
-- Copyright (c) Thom Mulders 1996
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1996-97
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{Resultant}
\History{Thom Mulders}{25 June 96}{created}
\Usage{import from \this(R,P)}
\Params{
{\em R} & \astype{IntegralDomain} & Coefficient ring for the polynomials\\
{\em P} & \astype{UnivariatePolynomialAlgebra0} R & A polynomial ring\\
}
\begin{exports}
\asexp{lastSPRS}: & (P,P) $\to$ P & last non-zero remainder in the SPRS\\
\asexp{extendedLastSPRS}:
& (P,P) $\to$ (P,P,P) & extended last non-zero remainder in the SPRS\\
\asexp{resultant}: & (P,P) $\to$ R & polynomial resultant\\
\asexp{SPRS}:
& (P,P) $\to$ \astype{List} P & Subresultant Polynomial Remainder Sequence\\
\asexp{subResultantGcd}: & (P,P) $\to$ P & GCD\\
\end{exports}
\Descr{
\this(R,P) implements polynomial resultant computations.
}
#endif

Resultant(R:IntegralDomain,P:UnivariatePolynomialAlgebra0 R ): with {

	lastSPRS: (P,P) -> P;
#if ALDOC
\aspage{lastSPRS}
\Usage{\name(a,b)}
\Signature{(P,P)}{P}
\Params{
{\em a,b} & P & Two polynomials\\
}
\Descr{
\name(a,b) returns the last non-zero remainder in the subresultant
polynomial remainder sequence of {\em a} and {\em b}. {\em a} and
{\em b} both should be non-zero and $\deg(a)$ should be at least
$\deg(b)$.
}
#endif

	extendedLastSPRS: (P,P) -> (P,P,P);
#if ALDOC
\aspage{extendedLastSPRS}
\Usage{\name(a,b)}
\Signature{(P,P)}{(P,P,P)}
\Params{
{\em a,b} & P & Two polynomials\\
}
\Descr{
\name(a,b) returns $(r,s,t)$, where $r$ is the last non-zero remainder in the
subresultant polynomial remainder sequence of {\em a} and {\em b} and $s,t$ are
polynomials such that $r=sa+tb$. {\em a} and
{\em b} both should be non-zero and $\deg(a)$ should be at least
$\deg(b)$.
}
#endif

	resultant: (P,P) -> R;
#if ALDOC
\aspage{resultant}
\Usage{\name(a,b)}
\Signature{(P,P)}{R}
\Params{
{\em a,b} & P & Two polynomials\\
}
\Descr{
\name(a,b) returns the resultant of {\em a} and {\em b}. {\em a} and
{\em b} both should be non-zero. 
}
#endif

	SPRS: (P,P) -> List P;
#if ALDOC
\aspage{SPRS}
\Usage{\name(a,b)}
\Signature{(P,P)}{\astype{List} P}
\Params{
{\em a,b} & P & Two polynomials\\
}
\Descr{
\name(a,b) returns the list of remainders in the subresultant
polynomial remainder sequence of {\em a} and {\em b}. {\em a} and
{\em b} both should be non-zero and $\deg(a)$ should be at least
$\deg(b)$. The list has increasing degree, i.e. the first element in
the list is the last remainder in the remainder sequence.
}
#endif

	if R has GcdDomain then {
		subResultantGcd: (P,P) -> P;
#if ALDOC
\aspage{subResultantGcd}
\Usage{\name(a,b)}
\Signature{(P,P)}{P}
\Params{
{\em a,b} & P & Two polynomials\\
}
\Descr{
\name(a,b) returns the last non-zero remainder in the subresultant
polynomial remainder sequence of either {\em a} and {\em b} or {\em b} and
{\em a}. If R is a Gcd domain, this is then $\gcd(a, b)$. Also returns
$\gcd(a,b)$ if either $a = 0$ or $b = 0$.}
#endif
	}

} == add {

-- See also: W.S. Brown: The subresultant PRS algorithm

	macro SI == MachineInteger;
	macro Z == Integer;

	import from SI,Z;

	-- degree a should be at least degree b
	local resultant!(a:P, b:P): R == {
		TRACE("resprs::resultant!: a = ", a);
		TRACE("resprs::resultant!: b = ", b);

		assert(~zero? a); assert(~zero? b);
		u1 := a; u2 := b;
		du1 := degree u1; du2 := degree u2;
		assert(du1 >= du2);
		zero? du1 => 1;		-- both a and b are nonzero constants
		delta := du1 - du2;
		h:R := 1;
		l:R := 1;
		cont?:Boolean := true;

		while cont? repeat {
			r := pseudoRemainder!(u1,u2);
			TRACE("resprs::resultant!: r = ", r);
			if (cont? := ~zero? r) then {
				{
					delta=0 => {};
					delta=1 => h := l;
					h := quotient(l^delta,h^(delta-1));
				}
				delta := degree u1 - degree u2;
				beta  := -l * (-h)^delta;
				u1 := u2;
				TRACE("resprs::resultant!: u1 = ", u1);
				l := leadingCoefficient u2;
				u2 := map( (x:R):R +-> quotient(x,beta)) r;
				TRACE("resprs::resultant!: u2 = ", u2);
			}
          	}
		degree u2 > 0 => 0;
		degree u1 = 1 => leadingCoefficient u2;
		assert(delta > 0);
		h := quotient(l^delta,h^(delta-1));
		delta := degree u1 -degree u2;
		l := leadingCoefficient u2;
		quotient(l^delta,h^(delta-1));
	}

	resultant(a:P,b:P): R == {
		import from Boolean;
		assert(~zero? a); assert(~zero? b);
		copya := copy a; copyb := copy b;
		da := degree a;
		db := degree b;
		da >= db => resultant!(copya, copyb);
		even? (da*db) => resultant!(copyb, copya);
		- resultant!(copyb, copya);
	}

	SPRS(a:P,b:P): List P == {
		assert(~zero? a); assert(~zero? b);

		import from R;

		list := [b,a];
		u1 := copy a; u2 := copy b;
		du1 := degree u1; du2 := degree u2;
		assert(du1 >= du2);
		delta := du1 - du2;
		h:R := 1;
		l:R := 1;
		cont?:Boolean := true;

		while cont? repeat {
			r := pseudoRemainder!(u1,u2);
			if (cont? := ~zero? r) then {
				{
					delta=0 => {};
					delta=1 => h := l;
					h := quotient(l^delta,h^(delta-1));
				}
				delta := degree u1 - degree u2;
				beta  := -l * (-h)^delta;
				u1 := u2;
				l := leadingCoefficient u2;
				u2 := map( (x:R):R +-> quotient(x,beta)) r;
				list := cons(copy u2,list);
			}
          	}

		list;
	}

	if R has GcdDomain then {
		subResultantGcd(a:P,b:P):P == {
			import from R;
			zero? a => b; zero? b => a;
			(ca, a) := primitive a;
			(cb, b) := primitive b;
			g := {
				degree a >= degree b => lastSPRS(a, b);
				lastSPRS(b, a);
			}
			gcd(ca, cb) * primitivePart g;
		}
	}

	lastSPRS(a:P,b:P): P == {

		assert(~zero? a); assert(~zero? b);

		import from R;

		u1 := copy a; u2 := copy b;
		du1 := degree u1; du2 := degree u2;
		assert(du1 >= du2);
		delta := du1 - du2;
		h:R := 1;
		l:R := 1;
		cont?:Boolean := true;

		while cont? repeat {
			r := pseudoRemainder!(u1,u2);
			if (cont? := ~zero? r) then {
				{
					delta=0 => {};
					delta=1 => h := l;
					h := quotient(l^delta,h^(delta-1));
				}
				delta := degree u1 - degree u2;
				beta  := -l * (-h)^delta;
				u1 := u2;
				l := leadingCoefficient u2;
				u2 := map( (x:R):R +-> quotient(x,beta)) r;
			}
          	}

		u2;

	}

	extendedLastSPRS(a:P,b:P): (P,P,P) == {

		assert(~zero? a); assert(~zero? b);

		import from R;
		u1 := copy a; u2 := copy b;
		du1 := degree u1; du2 := degree u2;
		assert(du1 >= du2);
		s1:P := 1; s2:P := 0;
		t1:P := 0; t2:P := 1;
		delta := du1 - du2;
		h:R := 1;
		l:R := 1;
		cont?:Boolean := true;

		while cont? repeat {
			(q,r) := pseudoDivide(u1,u2);
--print << u1 << newline << u2 <<newline << q << newline << r << newline << newline;
			if (cont? := ~zero? r) then {
				{
					delta=0 => {};
					delta=1 => h := l;
					h := quotient(l^delta,h^(delta-1));
				}
				delta := degree u1 - degree u2;
				beta  := -l * (-h)^delta;
				u1 := u2;
				l := leadingCoefficient u2;
				alpha := l^(delta+1);
				f := map( (x:R):R +-> quotient(x,beta) );
				s := f(alpha*s1-q*s2);
				s1 := s2; s2 := s;
				t := f(alpha*t1-q*t2);
				t1 := t2; t2 := t;
				u2 := f r;
			}
          	}

		(u2,s2,t2);

	}

}

