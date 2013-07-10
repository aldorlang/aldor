------------------------------------------------------------------------------
--
-- alg_bivtst2.as: A package providing tests for bivariate polynomial modular
--                 computations
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
	DF == DoubleFloat;
	DUP == DenseUnivariatePolynomial;
	SUP == SparseUnivariatePolynomial;
	UP == DenseUnivariatePolynomial;
	ARR == Array; 
      }

import from Z;

BivariateUtilitiesTestPackage2(u__s: Symbol, v__s: Symbol, _
  UP: (R: Join(ArithmeticType, ExpressionType), avar: Symbol == new()
       ) -> UnivariatePolynomialAlgebra(R)): with {


	random__U: (Z, maxCoeff: Z == 2147483647, sparsity: DF  == 0.01 ) -> UP(Z,u__s);
	random__V: (Z, Z, maxCoeff: Z == 2147483647, sparsity: DF  == 0.01 ) -> UP(UP(Z,u__s),v__s);
	modularResultantTest: (checking?: Boolean == false, printing?:  Boolean == false, n: Z == 10, maxdeg__U: Z == 10, maxdeg__V: Z == 10, maxCoeff: Z == 2147483647, sparsity: DF  == 0.01, algorithms: Z == -1) -> ();
	examples: (Z, maxdeg__U: Z == 10, maxdeg__V: Z == 10, maxCoeff: Z == 2147483647, sparsity: DF  == 0.01) -> Generator Cross(UP(UP(Z,u__s),v__s), UP(UP(Z,u__s),v__s));

} == add {

	U == UP(Z, u__s);
	V == UP(U, v__s);
	local u: U := monom;
	local v: V := monom;
	import from Z;

	example1(): Cross(V, V) == {
		f1__V: V := u^2*(3*v) - v^3 + 4 :: V;
		g1__V: V := (u^2) :: V + u*v^3 - 9 :: V;
		(f1__V, g1__V);
	}
	example2(): Cross(V, V) == {
		f2__V: V := (3*u)*v^2 - (u^3)::V + 4:: V;
		g2__V: V := v^2 + u^3 *v - 9 ::V;
		(f2__V, g2__V);
	}
	example3(): Cross(V, V) == {
		f1__V: V := u^2*(3*v) - v^3 + 4 :: V;
		g1__V: V := (u^2) :: V + u*v^3 - 9 :: V;
		f2__V: V := (3*u)*v^2 - (u^3)::V + 4:: V;
		g2__V: V := v^2 + u^3 *v - 9 ::V;
		f3__V: V := f1__V + (f2__V)^2;
		g3__V: V := (g2__V)^3 - g1__V;
		(f3__V, g3__V);
	}
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
		f__U: U := term(c__Z,d__U);
		for i in 0..(d__U -1) repeat {
			c__Z := random__Z(maxCoeff);
			if (not zero? c__Z) and (not zero? (random()$Z rem d)) then {
				f__U := add!(f__U,c__Z,i);
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
	examples(n: Z, maxdeg__U: Z == 10, maxdeg__V: Z == 10, maxCoeff: Z == 2147483647, sparsity: DF  == 0.01): Generator Cross(V,V) == generate  {
		assert(n > 0);
		if n <= 5 then {
			yield example1();
			if n > 1 then yield example2();
			if n > 2 then yield example3();
		} else {
			n := n + 3;
		}
		for i in 4..n repeat {
			-- d__U: Z := min(abs(random()$Z rem i) + 1, maxdeg__U);
			-- d__V: Z := min(abs(random()$Z rem i) + 1, maxdeg__V);
			d__U: Z := maxdeg__U;
			d__V: Z := maxdeg__V;
			f__V: V := random__V(d__U, d__V, maxCoeff);
			g__V: V := random__V(d__U, d__V, maxCoeff);
			cross: Cross(V,V) := (f__V, g__V);
			yield cross;
		}
	}
			
	modularResultantTest(checking?: Boolean == false, printing?:  Boolean == false, n: Z == 10, maxdeg__U: Z == 10, maxdeg__V: Z == 10, maxCoeff: Z == 2147483647, sparsity: DF  == 0.01, algorithms: Z == -1): ()  == {
		import from String, M, TextWriter, U;
		modresbivpack == ModularResultantOfBivariatePolynomials(U,V);
		import from modresbivpack;
		count: Z := 0;

		t1: Timer := timer();
		t2: Timer := timer();
		t3: Timer := timer();
		errors: Z := 0;
		local x1, x2: M;
		local mr__U, r__U: U;

		for cross in examples(n,maxdeg__U,maxdeg__V,maxCoeff,sparsity)  repeat {
			(f__V: V, g__V: V) :=  cross;
			count := count + 1;
			stdout << "------------------------------------------------------------" <<newline;
			stdout << "Example " << count << newline;
			stdout << "------------------------------------------------------------" <<newline << newline;			
			
			if printing? then {
				stdout << "f__V ? " << newline << f__V << newline << newline;
				stdout << "g__V ? " << newline << g__V << newline << newline;
			} else {
				stdout << "f__V ? " << newline << (f__V - reductum(f__V)) << " + ... + " << trailingCoefficient(f__V) * trailingMonomial(f__V) << newline;
				stdout << "g__V ? " << newline << (g__V - reductum(g__V)) << " + ... + " << trailingCoefficient(g__V) * trailingMonomial(g__V) << newline;
			}

			if (algorithms = -1) or (algorithms = 2) then {
				x1 := read(t1);
				start!(t1);
				mr__U := modularResultant(f__V, g__V, 1);
				stop!(t1);
				x1 := read(t1) - x1;
				if printing? then {
					stdout << "CRA-CRA-resultant ? "  << mr__U << newline;
				} else {
					stdout << "CRA-CRA-resultant ? "  << (mr__U - reductum(mr__U)) << " + ... + " << trailingCoefficient(mr__U) * trailingMonomial(mr__U) << newline;
				}
				stdout << x1 << " msec. " << newline << newline;
				if checking? then {
					x2 := read(t2);
					start!(t2);
					r__U :=  resultant(f__V, g__V);
					stop!(t2);
					x2 := read(t2) -x2;
					if printing? then {
						stdout << "non modular resultant ? "  << r__U << newline << newline;
					} else {
						stdout << "non modular resultant ? "  << (r__U - reductum(r__U)) << " + ... + " << trailingCoefficient(r__U) * trailingMonomial(r__U) << newline;
					}
					stdout << x2 << " msec. " << newline << newline;

					if (mr__U ~= r__U) then {
						stdout << " ERROR !!!!!!!!!!!! " << newline;
						stdout << " mr__U ~= r__U " << newline;
						stdout << " mr__U - r__U ? " << mr__U - r__U  << newline;
						errors := errors + 1;
 					}
				}
			}
			if (algorithms = 1) or (algorithms = 2) then {
				x3 := read(t3);
				start!(t3);
				mr__U := modularResultant(f__V, g__V, 3);
				stop!(t3);
				x3 := read(t3) - x3;
				if printing? then {
					stdout << "CRA-Default-resultant ? "  << mr__U << newline;
				} else {
					stdout << "CRA-Default-resultant ? "  << (mr__U - reductum(mr__U)) << " + ... + " << trailingCoefficient(mr__U) * trailingMonomial(mr__U) << newline;
				}
				stdout << x3 << " msec. " << newline << newline;
				if algorithms = 1 then {
					x2 := read(t2);
					start!(t2);
					r__U :=  resultant(f__V, g__V);
					stop!(t2);
					x2 := read(t2) -x2;
				}
				if checking? then {
					if printing? then {
						stdout << "non modular resultant ? "  << r__U << newline << newline;
					} else {
						stdout << "non modular resultant ? "  << (r__U - reductum(r__U)) << " + ... + " << trailingCoefficient(r__U) * trailingMonomial(r__U) << newline;
					}
					stdout << x2 << " msec. " << newline << newline;

					if (mr__U ~= r__U) then {
						stdout << " ERROR !!!!!!!!!!!! " << newline;
						stdout << " mr__U ~= r__U " << newline;
						stdout << " mr__U - r__U ? " << mr__U - r__U  << newline;
						errors := errors + 1;
 					}
				}
			}
		}
		stdout << newline  << newline;
		if (algorithms = -1) or (algorithms = 2) then {
			stdout << "Total time with CRA-CRA-resultant: " << read(t1) << " msec. " << newline;
		}
		if (algorithms = 1) or (algorithms = 2) then {
			stdout << "Total time with CRA-Default-resultant: " << read(t3) << " msec. " << newline;
		}
		if checking? then {
			stdout << "Total time with non modular resultant: " << read(t2) << " msec. " << newline;
			stdout << "Total number of errors: " << errors << newline;
		}
        	stdout << newline  << newline;
	}
}
