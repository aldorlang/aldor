------------------------------------------------------------------------------
--
-- alg_bivtst1.as: A package providing tests for bivariate polynomial routines
--
------------------------------------------------------------------------------
-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright (c) Yuzhen Xie and Marc Moreno Maza
-- Copyright (c) USTL & INRIA (France), UWO (Canada) 2003
------------------------------------------------------------------------------

#include "algebra"
#include "aldorio"

macro {
	Z == Integer;
	S == String;
	M == MachineInteger;
	SPF == SmallPrimeField;
	DUP == DenseUnivariatePolynomial;
	SUP == SparseUnivariatePolynomial;
	UP == DenseUnivariatePolynomial;
	ARR == Array; 
      }


BivariateUtilitiesTestPackage1(_
  UP: (R: Join(ArithmeticType, ExpressionType), avar: Symbol == new()
       ) -> UnivariatePolynomialAlgebra(R)): with {
	boundTests: () -> (); 

} == add {

	boundTests():() == {
		u__s: Symbol := -"u";
		v__s: Symbol := -"v";
		import from Z;
		U == UP(Z, u__s);
		V == UP(U, v__s);

		u: U := monom;
		v: V := monom;

		f1__V: V := u^2*(3*v) - v^3 + 4 :: V;
		g1__V: V := (u^2) :: V + u*v^3 - 9 :: V;

		bivarpack == BivariateUtilitiesPackage(U,V);
		import from bivarpack;

		resCoefBound: Z := resultantCoefficientBound(f1__V, g1__V)$bivarpack;

		resDegBound: Z := resultantDegreeBound(f1__V, g1__V);

		res: U := resultant(f1__V, g1__V);

		stdout << "f1__V ? " << newline << f1__V << newline << newline;
		stdout << "f1__V ? " << newline;

		stdout << "g1__V ? " << newline << g1__V << newline << newline;
		stdout << "resCoefBound ? " << newline << resCoefBound << newline << newline;

		stdout << "maxNorm(res)? " << newline << maxNorm(res) << newline << newline;

		stdout << "resDegBound ? " << newline <<  resDegBound<< newline << newline;

		stdout << "degree(res)? " << newline << degree(res) << newline << newline;
	}
}

