------------------------------------------------------------------------------
--
-- bivarpack.as: A package providing routines for bivariate polynomials
--
------------------------------------------------------------------------------
-- Copyright (c) Yuzhen Xie and Marc Moreno Maza
-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright (c) USTL & INRIA (France), UWO (Canada) 2003
------------------------------------------------------------------------------

#include "algebra"

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

-- BivariateUtilitiesPackage(U,V) aims to compute resultants
-- of polynomials in V. This is done by modular and specialization
-- techniques. Let f__V and g__V be the input polynomials.
-- Let res__U their resultant, this is a polynomial in U.
-- First a bound for the coefficients of res__U is computed.
-- Then primes p1, .... pn are chosen such that ....
-- Then w.r.t. each pi we reduce f__V and g__V and
-- compute their resultant (modulo pi).
-- To do so we chose pairwise different values x1, ... xm 
-- in the Z/pi to specialize the variable in U, say u.
-- The number m of these values is given by the degree
-- bound w.r.t. u of res__U

BivariateUtilitiesPackage(U: UnivariatePolynomialAlgebra(Z), _
                          V: UnivariatePolynomialAlgebra(U)): with {
	maxNorm: U -> Z;
	-- maxNorm(p__U) returns the maximum
	-- absolute value of the coefficients 
	-- of p__U
	maxNorm: V -> Z;
	-- maxNorm(f__V) returns the maximum
	-- absolute value of the coefficients 
	-- of f__V
	degree__U: V -> Z;
	-- degree(f__V) returns the degree of f__V
	-- w.r.t. the variable u of U.
	resultantCoefficientBound: (V, V) -> Z;
        -- resultantCoefficientBound(f__V, g__V) returns
 	-- an integer bound for the absolute value
	-- of the coefficients of their resultant
	resultantDegreeBound: (V, V) -> Z;
	-- resultantDegreeBound(f__V, g__V) returns
 	-- an integer bound for the degree of the
	-- resultant of f__V and g__V w.r.t. the
 	-- variable u of U.
	primeBad?: (V, V, M) -> Boolean;
	-- primeBad(f__V, g__V, p__M) returns true if the 
	-- prime p__M divides either the leading 
	-- coefficient of f__V or that of g__V 
	--apply: (V, S, S) -> ET;
	-- apply(f__V, v__S, u__S) returns an expression tree
        -- where te variables of f__V appear as  v__S and u__S
	--apply: (U, S) -> ET;
	-- apply(f__U, u__S) returns an expression tree
        -- where te variable of f__U appears as u__S

} == add {

	import from TextWriter, Character, String;

	resultantCoefficientBound(f__V: V, g__V: V): Z == {
		m__Z: Z := degree(f__V);
		n__Z: Z := degree(g__V);
		d__Z: Z := maxNorm(f__V);
		e__Z: Z := maxNorm(g__V);
		f__Z: Z := factorial(m__Z + n__Z)*d__Z^n__Z*e__Z^m__Z;
		f__Z;
	}
	degree__U(f__V: V): Z == {
		d__U: Z := 0;
		import from Z, U, V;
        	for p__U in coefficients(f__V) repeat {
			d__p__U := degree(p__U);
			if d__p__U > d__U then d__U := d__p__U;
		}
		d__U;
	}

	resultantDegreeBound(f__V: V, g__V: V): Z == {
		import from Z;
		degree(f__V) * degree__U(g__V) + degree__U(f__V) * degree(g__V);		}

	maxNorm(p__U: U): Z == {
		z: Z := -1;
		import from U;
		for c__Z in coefficients(p__U) repeat {
			v__Z := abs(c__Z);
			if v__Z > z then z := v__Z;
		}
		z;
	}
	maxNorm(f__V: V): Z == {
		z: Z := -1;
        	-- we initialize the maxNorm with the
        	-- value for the zero polynomial
		import from Z, U, V;
        	for p__U in coefficients(f__V) repeat {
			for c__Z in coefficients(p__U) repeat {
				v__Z := abs(c__Z);
				if v__Z > z then z := v__Z;
			}
		}
		z;
	}
	primeBad?(f__V: V, g__V: V, p__M: M): Boolean == {
		Kp == SPF(p__M);
		pf__U: U := leadingCoefficient(f__V);
		pg__U: U := leadingCoefficient(g__V);
		cf__Z: Z := leadingCoefficient(pf__U);
		cg__Z: Z := leadingCoefficient(pg__U);
		cf__Zp: Kp := cf__Z::Kp;
		cg__Zp: Kp := cg__Z::Kp;
		(zero?(cf__Zp) or zero?(cg__Zp));
	}
	--apply(f__U: U, u__S: S): ET == {
		--u__ETL: ExpressionTreeLeaf := leaf(u__S);
		--u__ET: ET := extree(u__ETL);
		--apply(f__U, u__ET);
	--}
	--apply (f__V: V, u__S: S, v__S: S): ET == {
		--local v__ETL: ExpressionTreeLeaf := leaf(v__S);
		--local v__ET: ET := extree(v__ETL);
		--local z?: Boolean := true;
		--local f__ET: ET;
		--for term__U in generator(f__V) repeat {
			--(c__U: U, d__V: Z) := term__U;
			--d__ETL: ExpressionTreeLeaf := leaf(d__V);
			--d__ET: ET := extree(d__ETL);
			--c__ET: ET := apply(c__U, u__S);
			--l__ET: List ET := [v__ET, d__ET];
			--power__ET: ET := apply(ExpressionTreeExpt,l__ET);
			--l__ET := [c__ET, power__ET];
			--term__ET: ET := apply(ExpressionTreeTimes,l__ET);
			--if z? then {
				--f__ET := term__ET;
				--z? := false;
			--} else {
				--l__ET: List ET := [f__ET, term__ET];
				--f__ET := apply(ExpressionTreePlus,l__ET);
			--}
		--}
		--f__ET;
	--}
			
}


#if ALDOC
\thistype{BivariateUtilitiesPackage}
\History{Yuzhen Xie and Marc Moreno Maza}{Mars 2003}{created}
\Usage{import from \this~(U,V)}
\Params{
{\em U} & \altype{UnivariatePolynomialAlgebra} \altype{Integer} \\
{\em V} & \altype{UnivariatePolynomialAlgebra} {U} \\
}
\Descr{\this~(U,V) provides basic operations for bivariate polynomials
       over the integer numbers.  
       In particular, it provides support for modular methods.
       In the above description, the smallest variable of a polynomial
       from V refers to the variable of U
       and its degree refers to the its degree as a univariate polynomial
       w.r.t. the variable of V.
}
\begin{exports}
\alexp{maxNorm}:   & U $\to$ Z & maximum absolute value of a coefficient \\
\alexp{maxNorm}:   & V $\to$ Z & maximum absolute  value of a coefficient \\
\alexp{degree\_U}: & V $\to$ Z & degree w.r.t. the smallest variable \\
\alexp{resultantCoefficientBound}: 
                   & (V, V) $\to$ Z & upper bound for the  resultant maxNorm \\
\alexp{resultantDegreeBound}:
                   & (V, V) $\to$ Z & degree bound for the resultant \\
\alexp{primeBad?}: 
                   & (V, V, I) $\to$ B &
                                                   true iff the degree of one of the two \\
                   &                               & polynomials drops modulo the number \\
\end{exports}                                              
\begin{aswhere}
I &==& \altype{MachineInteger}\\
Z &==& \altype{Integer}\\
B &==& \altype{Boolean}\\
\end{aswhere}
#endif 

#if ALDORTEST
---------------------- test bivartest1.as --------------------------
#include "algebra"
#include "aldortest"

macro {
	DUP == DenseUnivariatePolynomial;
	SUP == SparseUnivariatePolynomial;
}

main():() == {
	import from String, Symbol, MachineInteger, TextWriter;
	testpack__DUP == BivariateUtilitiesTestPackage1(DUP);
	import from testpack__DUP;

--	boundTests()$testpack__DUP;
--      This test was switched off temporarily
--      because it seems to be running forever

	testpack__SUP == BivariateUtilitiesTestPackage1(SUP);
	import from testpack__SUP;
	boundTests()$testpack__SUP;

}


main();
stdout << endnl;
#endif


