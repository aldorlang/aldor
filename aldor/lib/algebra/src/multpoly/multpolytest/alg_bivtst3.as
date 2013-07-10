------------------------------------------------------------------------------
--
-- alg_bivtst3.as: A package providing tests for bivariate polynomial modular
--                 computations
--
------------------------------------------------------------------------------
-- Copyright (c) Jinlong Cai and Marc Moreno Maza
--  Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright (c) USTL & INRIA (France), UWO (Canada) 2004
------------------------------------------------------------------------------

#include "algebra"
#include "aldorio"

macro {
	Z == Integer;
	S == String;
	M == MachineInteger;
	SPF == SmallPrimeField;
	DF == DoubleFloat;
	DUP == DenseUnivariatePolynomial;
	SUP == SparseUnivariatePolynomial;
	UP == DenseUnivariatePolynomial;
	ARR == Array; 
      }

import from Z, M;

BivariateUtilitiesTestPackage3(p: M, u__s: Symbol, v__s: Symbol, _
  UP: (R: Join(ArithmeticType, ExpressionType), avar: Symbol == new()
       ) -> UnivariatePolynomialAlgebra(R)): with {

	random__U: (Z, maxCoeff: Z == 2147483647, sparsity: DF  == 0.01 ) -> UP(SPF p,u__s);
	random__V: (Z, Z, maxCoeff: Z == 2147483647, sparsity: DF  == 0.01 ) -> UP(UP(SPF p,u__s),v__s);
} == add {

	U == UP(SPF p, u__s);
	V == UP(U, v__s);
	local u: U := monom;
	local v: V := monom;
	import from Z;

	randomSign(): Z == {
		even?(random()$Z) => 1;
		-1;
	}
	density__Z(f: DF): Z == {
		import from DF;
		d: Z := 2;
		g: DF := f;
		while (g < 1.0) and (d < 100) repeat {
			g := g + f;
			d := d + 1;
		}
		assert(d < 101);
		assert(d > 1);
		d;
	}
	random__Z(maxCoeff: Z == 2147483647): Z == {
		randomSign() * (random()$Z rem maxCoeff);
	}
	random__U(d__U: Z, maxCoeff: Z == 2147483647, sparsity: DF  == 0.01 ): U == {		
		c__Z: Z := random__Z(maxCoeff);
		d: Z := density__Z(sparsity);
		while zero?(c__Z) repeat c__Z := random__Z(maxCoeff);
		c__SPF :=  c__Z :: (SPF p);
		f__U: U := term(c__SPF,d__U);
		for i in 0..(d__U -1) repeat {
			c__Z := random__Z(maxCoeff);
			c__SPF := c__Z :: (SPF p);
			if (not zero? c__SPF) and (not zero? (random()$Z rem d)) then {
				f__U := add!(f__U,c__SPF,i);
			}
		}
		f__U;
	}
	random__V(d__U: Z, d__V: Z, maxCoeff: Z == 2147483647, sparsity: DF  == 0.01 ): V  == {
		d: Z := density__Z(sparsity);
		f__V: V := term(random__U(d__U, maxCoeff, sparsity),d__V);
		for i in 0..(d__V -1) repeat {
			c__U: U := random__U(d__U, maxCoeff, sparsity);
			if (not zero? c__U) and (not zero? (random()$Z rem d)) then {
				f__V := add!(f__V,c__U,i);
			}
		}
		f__V;
	}

}
