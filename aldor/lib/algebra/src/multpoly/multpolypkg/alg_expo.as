--------------------------------------------------------------------------------
--
-- alg_expo.as
--
--------------------------------------------------------------------------------
--  Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright: INRIA, UWO and University of Lille I, 2001--2002
-- Copyright: Marc Moreno Maza
--------------------------------------------------------------------------------

-- pkg PolynomialExponentiationPackage

#include "algebra"
#include "aldorio"

macro {
	B == Boolean;
        Z == Integer;
	N == Integer;
	NNI == Integer;
}

PolynomialExponentiationPackage(R: Ring,
   					E: GeneralExponentCategory,
					P: Join(CopyableType,IndexedFreeAlgebra(R,E))): with {
      binomialTheorem: (R,E,P,NNI) -> P;
        ++ `binomialTheorem(r,e,p,n)' returns `(term(r,e) + p)^n' by using
        ++ the binomial theorem trick. This operations produces an error
        ++ if `R' is not a commut#quitative ring.
      power__comm__0: (P, NNI) -> P;
        ++ `power__comm__0(p,n)' returns `p^n' assuming that `R' has 
        ++ characteristic zero (this property is not checked). Moreover,
        ++ `R' needs to be a commutative ring. If this is not the case,
        ++ then an error is produced. Otherwise, the binomial theorem
        ++ trick is used.
--      power__x__0: (P, NNI) -> P;
        ++ `power__x__0(p,n)' returns `p^n' assuming that `R' has 
        ++ characteristic zero (this property is not checked).
        ++ The ring `R' may not be commutative and this operation 
        ++ directly performs the classical binary exponentiation.
--      power__comm__p: (P, NNI) -> P;
        ++ `power__comm__p(p,n)' returns `p^n'. R' needs to be a 
        ++ commutative ring with prime characteristic. Both
        ++ properties are checked and an error is produced if they
        ++ do not hold. Otherwise, the binomial theorem trick is used.
        ++ Moreover the `charthPower' is called as much as possible.
        ++ WARNING: `charthPower' should better not call this package
        ++ directly or indirectly via `^'.
--      power__x__p: (P, NNI) -> P;
        ++ `power__x__p(p,n)' returns `p^n'. R' needs to be a ring
        ++ with prime characteristic (this property is not checked).
        ++ If this is not the case, then an error is produced. 
        ++ Otherwise, the `charthPower' is called as much as possible.
--      power: (P, NNI) -> P;
        ++ `power(p,n)' returns `p^n'. This operation takes the properties
        ++ of `R' into account in order to use the best subroutines of this
        ++ package.
--      power!: (P, NNI, SortedAssociationList(NNI,P)) -> P;
        ++ `power!(p,n,h)' returns `p^n' by using `h' which is assumed to be
        ++ a table of powers of `p'. More precisely, if `search(h,m)' is not 
        ++ `failed()' it is assumed to be `[p^m]'. If `P' supports an FFT-based 
        ++ multiplication then small powers of `p' can be computed with this 
        ++ multiplication and stored in `h'; then the operation `power!(p,n,h)' 
        ++ will finish the job for larger powers.

} == add {

        ---------------
        --% imports %--
        ----------------

	import from R, E, P, Boolean;
        Term == Cross(R,E);

	-------------------------
	--% local constants  %--
	-------------------------

        local commutativeRing?: Boolean  == R has CommutativeRing;
        local integralDomain?: Boolean == R has IntegralDomain;

	-------------------------
	--% local functions  %--
	-------------------------

        local binomialTheorem__INTDOM(r:R,e:E,y:P,n:NNI): P  == {
		-- ASSUME r is not zero
		-- ASSUME n >= +2
		-- RETURNS (term(r,e)+y)^n
		assert(integralDomain?);
		assert(not zero? r);
		assert(n > 1);
		t: Term := (r,e);
		lt: List Term := [t];
		n__Z: Z := n;
		for i in 2..n__Z repeat {
			(s:R, f:E) := first lt;
			lt := cons((r*s,e+f),lt);
          	}
          	i: Z := 1;
          	bincoef: Z := n__Z;
          	y__i: P := copy y;
		res: P := term first(lt);
		repeat {
			lt := rest lt;
			empty? lt => return (add!(res,y__i));
			(s:R, f:E) := first lt;
			s := bincoef*s;
			res := add!(res,s,f,y__i);
			bincoef := ((n__Z -i) * bincoef) quo (i + 1);
			i := i + 1;
			y__i := y__i * y;
          	}
        }

	---------------------------
	--% exported functions  %--
	---------------------------

        binomialTheorem(r:R,e:E,y:P,n:NNI): P == {
		integralDomain? => binomialTheorem__INTDOM(r,e,y,n);
		-- commutativeRing? => binomialTheorem__ZERODIVISORS(r,e,y,n);
		never;
	}
        power__comm__0(x: P, n:NNI): P == {
		zero? n => 1;
		n = 1 => x;
		zero? x => x;
		term? x => {
			r: R := leadingCoefficient(x)^n;
			zero? r => return 0;
			return term(r,times(n,degree(x)));
		}
		-- power(leadingCoefficient(x),degree(x),reductum(x),n)$P;
		binomialTheorem(leadingCoefficient(x),degree(x),reductum(x),n)$%;
        }
}



