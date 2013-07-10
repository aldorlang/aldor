

#include "algebra"

macro {
	SI == MachineInteger;
	Z == Integer;
	ARR == Array;
}

+++ Author: Jinlong Cai Marc Moreno Maza
+++ Date Last Update: 10/01/04
ChineseRemaindering2(R: Join(EuclideanDomain,SourceOfPrimes,ModularComputation),
			U: UnivariatePolynomialAlgebra0 R): with {
	combine:	(R,R) -> (ARR R,ARR R) -> ARR R;
	getArrOfUP:     U -> ARR R; --convert an univariate polynomial to an array of coeffs
	getArrToUP:     ARR R -> U; --convert an array of coeffs to an univariate polynomial
	arrayMod:       (U, R) -> ARR R; 
	exactQuotient:  (U, R) -> Partial(U);
} == add {

	combine(m1:R, m2:R):(ARR R,ARR R) -> ARR R == {
		comb(a1: R, a2: R): R == combine(m1,m2)(a1,a2);

                fn(r1: ARR R, r2: ARR R): ARR R == {
			--assume the sizes of array of r1 and r2 are the same. 
			--assume the degree of array is in increasing order
			import from SI, R;
			import from TextWriter, String, Character;		
			local dr1: SI := #r1-1;
			local dr2: SI := #r2-1;
			local dr: SI;

			if dr1>=dr2 then dr:=dr1;
			else dr := dr2;
			
			local r:ARR R := new next dr;	

			for i in 0..dr repeat {
				local rr1,rr2: R;
				if i <= dr1 then
					rr1 := r1.i;
				else rr1 := 0;

				if i <= dr2 then
					rr2 := r2.i; 
				else rr2 := 0;
				r.i := comb(rr1,rr2);

				r.i := symmetricMod(r.i, m1*m2);
			}
			r;
                }
		
                fn;
	}

	--convert an univariate polynomial to an array of coefficients
	getArrOfUP(a: U): ARR R == {
		import from Z, SI, R;
		local da:SI := machine degree a;
		local aarr: ARR R := new(next da);

		for i in 0 .. da repeat aarr.i := 0;

		for t in a repeat {  
			(c, e) := t;
			aarr.(machine e) :=  c;
		}	
		aarr;
	}

	getArrToUP(a: ARR R): U == {
		import from SI,Z;

		local res: U := 0;
		for i in 0 .. #a-1 repeat {
			--assume the degree in increasing order
			res := add!(res, a.i, i::Z); 
		}
		res;
	}

	arrayMod(a: U, p: R): ARR R == { 	
		import from SI;
	 	import from String, Symbol, TextWriter, Character;	
	
		local amodp: ARR R := getArrOfUP(a);

		for i in 0 .. #amodp -1 repeat {
			amodp.i := amodp.i mod p;
		}
		amodp;
	}

	exactQuotient(a:U, p:R): Partial(U) == {
		import from Partial(R), Partial(U), R, U;

		local aquo: U := 0;

		for t in a repeat {  --terms are in decreasing order
			(c, e) := t;

			cp?: Partial(R) := exactQuotient(c, p);

			if failed? cp? then return failed$Partial(U);
			else  aquo := add!(aquo, retract cp?, e);
		}
		[aquo];
	}


}

