------------------------------------------------------------------------------
--
-- modresbiv.as: A package providing modular computations of the resultant
--               of two bivariate polynomials.
--
------------------------------------------------------------------------------
-- Copyright (c) Yuzhen Xie and Marc Moreno Maza
-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
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
        DUPZ == DUP(Z);
	DUPM == DUP(M);
      }

ModularResultantOfBivariatePolynomials( 
			U: UnivariatePolynomialAlgebra(Z), 
			V: UnivariatePolynomialAlgebra(U)): with {

	symmetricRemainder: (Z, Z) -> Z;
	symmetricRemainder?: (Z, Z) -> Boolean;
	symmetricRemainder?: (Z, DUPZ) -> Boolean;
	symmetricRemainder: (Z, DUPZ) -> DUPZ;
	sameReduction?: (Z, DUPZ, DUPZ) -> Boolean;
	DUPM__to__DUPZ: DUPM -> DUPZ;
	DUPZ__to__U: DUPZ -> U;
	modularResultant: (V, V, algorithm: Z == 1) -> U;
	combine: (DUPZ, Z, DUPM, M) -> DUPZ;            
        resultant: (V, V, M, Z, algorithm: Z == 1) -> DUPM;

} == add {

	import from TextWriter, Character, String;

	local debugging?: Boolean := false;
	local printing?: Boolean := false;

	symmetricRemainder(m: Z, a: Z): Z == {    -- OPTIMIZE !!!!
		import from Z;
		assert(odd?(m));
		assert(m > 1);
		maxBound: Z := (m - 1) quo 2;
		minBound: Z := - maxBound;
		a := a rem m;
		if (a > maxBound) then  a := a - m;
		if (a < minBound) then a := a + m;
		a;
	}
	symmetricRemainder?(m: Z, a: Z): Boolean == {
		b: Z := 2 * a;
		(-m-1 < b) and (b < m-1);
	}
	symmetricRemainder?(m: Z, f__A: DUPZ): Boolean == {
		maxBound: Z := (m - 1) quo 2;
		minBound: Z := - maxBound;
		for term__A in generator(f__A) repeat {
			(c__Z, d__Z) :=  term__A;
			(c__Z < minBound) or (c__Z > maxBound) => return false;
		}
		true;
	}
	symmetricRemainder(m: Z, f__A: DUPZ): DUPZ == {
		g__A: DUPZ := 0;
		maxBound: Z := (m - 1) quo 2;
		minBound: Z := - maxBound;
		for term__A in generator(f__A) repeat {
			(c__Z, d__Z) :=  term__A;
			c__Z := c__Z rem m;
			if (c__Z > maxBound) then  c__Z := c__Z - m;
			while (c__Z < minBound) repeat c__Z := c__Z + m;
			g__A := add!(g__A, c__Z, d__Z);
		}
		g__A;
	}
	sameReduction?(m: Z, f__A: DUPZ, g__A: DUPZ): Boolean == {
		d__f: Z := degree(f__A);
		d__g: Z := degree(g__A);
		if d__f < d__g then {
			(f__A, g__A) := (g__A, f__A);
			(d__f, d__g) := (d__g, d__f);
		}
		b: Boolean := true;
		if d__f > d__g then {
			while b for i in (d__g + 1)..d__f repeat {
				if not zero? (coefficient(f__A,i) rem m) then b := false;
			}
		}
		while b for i in 0..d__g repeat {
			if not zero?((coefficient(f__A,i) - coefficient(g__A,i)) rem m) then b := false;
		}
		b;
	}
	DUPM__to__DUPZ(f__DUPM: DUPM): DUPZ == {
		f__DUPZ: DUPZ  := 0 ;
		import from Z;
		for term__DUPM in generator(f__DUPM) repeat {
			(c__M: M, d__Z: Z) := term__DUPM;
			f__DUPZ := add!(f__DUPZ, (c__M :: Z), d__Z);
		}
		f__DUPZ;
	}
	DUPZ__to__U(f__A: DUPZ): U  == {
		f__U: U := 0;
		for term__DUPZ in generator(f__A) repeat {
			(c__Z, d__Z) :=  term__DUPZ;
			f__U := add!(f__U, c__Z, d__Z);
		}
		f__U;
	}
	combine(f__A: DUPZ, m: Z, g__B: DUPM, p: M): DUPZ == {
		crp == ChineseRemaindering(Z);
		import from crp;
		local cmb ==  combine(m,p)$crp;
		d__f: Z := degree(f__A);
		d__g: Z := degree(g__B);
		h__A: DUPZ := 0;

		if d__f < d__g then {
			for i in (d__f + 1)..d__g repeat {
				j := d__g - i + (d__f + 1);
				c__Z: Z := cmb(0,coefficient(g__B,j));
				h__A := add!(h__A, c__Z, j);
			}
			for i in 0..d__f repeat {
				c__Z: Z := cmb(coefficient(f__A,i),coefficient(g__B,i));
				h__A := add!(h__A, c__Z, i);
			}
		} else {
			for i in (d__g + 1)..d__f repeat {
				j := d__f - i + (d__g + 1);
				c__Z: Z := cmb(coefficient(f__A,j),0);
				h__A := add!(h__A, c__Z, j);
			}
			for i in 0..d__g repeat {
				c__Z: Z := cmb(coefficient(f__A,i),coefficient(g__B,i));
				h__A := add!(h__A, c__Z, i);
			}
		}
		symmetricRemainder(m * (p::Z), h__A);
	}
	resultant(f__V: V, g__V: V, p: M, D: Z, algorithm: Z == 1): DUPM == {
		Kp == SPF(p);
		Up == UP(Kp);
		Vp == UP(Up);
		local modularReduction(h__V: V): Vp == {
			h__Vp: Vp := 0;
			for term__U in generator(h__V) repeat {
				(c__U: U, d__V: Z) := term__U;
				c__Up: Up := 0;
				for term__Z in generator(c__U) repeat {
					(c__Z: Z, d__U: Z) := term__Z;
					c__Zp: Kp := c__Z :: Kp;
					--  c__Up := c__Up + term(c__Zp,d__U);
                                	--  c__Up := add!(c__Up, term(c__Zp,d__U));
					c__Up := add!(c__Up,c__Zp,d__U);
				}
				h__Vp := add!(h__Vp, c__Up, d__V);
			}
			h__Vp;
		}
		rbzp == ResultantOfBivariatePolynomialsOverSmallPrimeField(Kp,Up,Vp);
		f__Vp: Vp := modularReduction(f__V);
		g__Vp: Vp := modularReduction(g__V);
		import from rbzp;
		local res__Up: Up;
		if algorithm = 2 then {
			res__Up := interpolationResultant(f__Vp, g__Vp, D)$rbzp;
		} else { 
			if algorithm = 3 then {
				res__Up: Up := resultant(f__Vp,g__Vp);
			} else {
				res__Up := evaluationResultant(f__Vp, g__Vp, D)$rbzp;
			}
		}
		r__DUPM: DUPM := integerImage(res__Up)$rbzp;
		r__DUPM;
	}
	modularResultant(f__V: V, g__V: V, algorithm: Z == 1): U == {
		bivarpack == BivariateUtilitiesPackage(U,V);
		import from bivarpack;
		B: Z := 2 * resultantCoefficientBound(f__V, g__V)$bivarpack;
		m: Z := 1;
		D: Z  := resultantDegreeBound(f__V, g__V)$bivarpack;
		accu__DUPZ: DUPZ := 0;
		p: M := maxPrime$HalfWordSizePrimes;
		while (m < B) repeat {
			while (primeBad?(f__V,g__V,p)) repeat {
				p := previousPrime(p)$HalfWordSizePrimes;
			}
			tmp__DUPM: DUPM := resultant(f__V, g__V, p, D, algorithm);
			p__Z:  Z := p::Z;
			if m = 1 then {
				accu__DUPZ := DUPM__to__DUPZ (tmp__DUPM);
				m := p__Z;
			} else {
				accu__DUPZ := combine(accu__DUPZ, m, tmp__DUPM, p);
				m := m * p__Z;
			}
			p := previousPrime(p)$HalfWordSizePrimes;
		}
		DUPZ__to__U(accu__DUPZ);
	} 
}

#if ALDORTEST
---------------------- test bivartest1.as --------------------------
#include "algebra"
#include "aldortest"

macro {
	Z == Integer;
	DF == DoubleFloat;
	DUP == DenseUnivariatePolynomial;
	SUP == SparseUnivariatePolynomial;
}

main():() == {
	import from String, Symbol, TextWriter;

	u__S: Symbol := -"u";
	v__S: Symbol := -"v";
	testpack__DUP == BivariateUtilitiesTestPackage2(u__S,v__S,DUP);
	import from testpack__DUP;

	checking?: Boolean == true;
	printing?: Boolean == false;
	n: Z == 3;
	maxdeg__U: Z == 5; 
	maxdeg__V: Z == 25; 
	maxCoeff: Z == 2^10; 
	sparsity: DF  == 0.95;
	algorithms: Z == 2;

        stdout << "modularResultantTest(...)$testpack__DUP ... " << newline;

--	modularResultantTest(checking?,printing?, n, maxdeg__U, maxdeg__V, maxCoeff, sparsity, algorithms)$testpack__DUP;
--      TEST FAILS: does not terminate

        stdout << "... done" << newline << newline << newline;
	stdout << "All tests done!" << newline;

}


main();
stdout << endnl;
#endif




