
------------------------------------------------------------------------------
--
-- defgcd.as: A package providing GCD computations of multivariate polynomials
--            with coefficients in a Gcd domain.
--
------------------------------------------------------------------------------
-- Copyright (c) Xin Li and Marc Moreno Maza
-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright (c) USTL & INRIA (France), UWO (Canada) 2004
------------------------------------------------------------------------------

-- pkg MultivariatePolynomialGcdPackage

#include "algebra"
#include "aldorio"

macro {
	B == Boolean;
        Z == Integer;
        I == MachineInteger;
}

+++ `MultivariatePolynomialGcdPackage(R, V, P)' is a package for gcd computations
+++ of multivariate polynomials over Gcd domain. Computations performed by the
+++ the sub-resultant algorithms.
+++ Author: Xin Li and Marc Moreno Maza
+++ Date Created: 22/10/2004
+++ Date Last Update: 14/11/04 (Xin Li and MMM)
+++ Keywords: polynomial, multivariate, gcd

MultivariatePolynomialGcdPackage(
			R: GcdDomain, 
                        V: Join(TotallyOrderedType, ExpressionType) , 
                        P: PolynomialRing0(R,V)): with {

	unitCanonical: (P) -> P;
        ++
        nextVariable : (P, V) -> V;
	++ `nextVariable(p,v)'  assumes that `p' is not constant
	++ and assumes that `v' is not the greatest variable in `p'
	++ it returns the smallest variable of `p' which is greater than `v'
	content: (P, V) -> P;
	++ `content(p,v)' returns the content of `p' regarded as a polynomial
	++ with coefficients in `S' where `S' is the ring of the polynomials
	++ with coefficients in `R' and variables from `V' all smaller than `v'.
	gcd: (P, P ) -> P;
	++ `gcd(p,q)' returns the gcd of `p' and `q'.
} == add {
	import from B, R, V, P;
	D ==> DenseUnivariatePolynomial(P);

        import from D;
        local univ(p: P, v: V): D ==  univariate(D)(p,v);
        local multiv (d:D, v:V):P ==  multivariate(D)(d,v);

	unitCanonical(p: P): P == {
                 p=0 or leadingCoefficient(p) = 1 => p;
                 (u,cf,a):=unitNormal(leadingCoefficient(p));
                 a*p;
        }
        nextVariable(p: P, v: V): V =={
		assert(~ground? p);
		local last:V;
		firstTime?:Boolean := true;
		for w in variables p repeat {
			w = v => { assert(~firstTime?); return last; }
			last := w;
			firstTime := false;
		}
		never;
	}

        content(p: P, v: V): P == {
		local u := univ(p,v);
                -- content(u)$D;
		local cpv: P := 0; 
		for cu in coefficients(u) repeat {
		    ground? cu => cpv := gcd(cpv,cu)$%;
		    vcu := mainVariable(cu);
		    vcu < v => cpv := gcd(cpv,cu)$%;
		    gcd(cpv,content(cu,vcu))$%;
		}
		return cpv;
        }
        gcd(a: P, b: P): P == {
	    a = 0 => b;
	    b = 0 => a;
	    ground? a or ground? b => { 
		gm:P:=gcd(content(a)$P, content(b)$P)::P;
		unitCanonical gm;}
            (a:=unitCanonical(a)) =(b:=unitCanonical(b)) => a;
            va :V:= mainVariable(a);
            vb :V:= mainVariable(b);
            va < vb => {
	    	-- MB 11/2004: THIS CODE IS BUGGY, WILL BOMB IF b is UNIVARIATE
		--             SINCE THERE IS NO "NEXT VARIABLE"
                vc  := nextVariable(b,va);
                gm :P:= gcd(a,content(b,vc))$%;
                unitCanonical gm;}
            va > vb => {
	    	-- MB 11/2004: THIS CODE IS BUGGY, WILL BOMB IF a is UNIVARIATE
		--             SINCE THERE IS NO "NEXT VARIABLE"
                vc  := nextVariable(a,vb);
                gm :P:= gcd(b,content(a,vc))$%;
                unitCanonical gm; }
            assert(va = vb);
            g :D:= gcd(univ(a,va),univ(b,vb))$D;
            tmp:=multivariate(D)(g,va);
            unitCanonical tmp;}
}


#if ALDORTEST
------------------------------test alg_defgcd.as-----------------
#include "algebra"
#include "aldortest"

import from  MachineInteger;

macro {
        B == Boolean;
        INT == Integer;
        Q == Fraction(Integer);
        SPF == SmallPrimeField(1009);   
        V1 == OrderedVariableTuple(-"x",-"y");
        V2 == OrderedVariableTuple(-"v",-"w");
        SMPI == SparseMultivariatePolynomial(INT ,V1);
        SMPII == SparseMultivariatePolynomial(SMPI,V2);
        SMPQ == SparseMultivariatePolynomial(Q ,V1);
        SMPQQ == SparseMultivariatePolynomial(SMPQ,V2);      
        SMPP == SparseMultivariatePolynomial(SPF ,V1);
        SMPPP == SparseMultivariatePolynomial(SMPP,V2);    }



--In the test , a function gcdTest which is parametrized
--by a R (a GCD doamin) V (a set of variables) P (a Polynomial ring
--over R and V) and v1: V v2: V

gcdTest(R:GcdDomain, V:Join(TotallyOrderedType, ExpressionType),
P:PolynomialRing(R,V), v1:V, v2:V): B == {
    import from R,V,P,String,Symbol;
    p1: P := v1 :: P;
    p2: P := v2 :: P;
    r1:R:=random();
    r2:R:=random();
    r3:R:=random();
    r4:R:=random();
    r5:R:=random();
    r6:R:=random();
    r7:R:=random();
    r8:R:=random();
    r9:R:=random();    
    r10:R:=random();
    if R has FractionFieldCategory(Integer) then {
        import from Q ;
        bound:Integer:=5;
    	r1:R:=(numerator(r1) rem bound) / ((denominator(r1) rem bound)+1); 
        r2:R:=(numerator(r2) rem bound) / ((denominator(r2) rem bound)+1);
	r3:R:=(numerator(r3) rem bound) / ((denominator(r3) rem bound)+1);
	r4:R:=(numerator(r4) rem bound) / ((denominator(r4) rem bound)+1);
	r5:R:=(numerator(r5) rem bound) / ((denominator(r5) rem bound)+1);
	r6:R:=(numerator(r6) rem bound) / ((denominator(r6) rem bound)+1);
	r7:R:=(numerator(r7) rem bound) / ((denominator(r7) rem bound)+1);
	r8:R:=(numerator(r8) rem bound) / ((denominator(r8) rem bound)+1);
	r9:R:=(numerator(r9) rem bound) / ((denominator(r9) rem bound)+1);
	r10:R:=(numerator(r10) rem bound) / ((denominator(r10) rem bound)+1);}
    ran1:P:=r1::P;
    ran2:P:=r2::P;
    ran3:P:=r3::P;
    ran4:P:=r4::P;
    ran5:P:=r5::P;
    ran6:P:=r6::P;
    ran7:P:=r7::P;
    ran8:P:=r8::P;
    ran9:P:=r9::P;
    ran10:P:=r10::P;
    e2:Integer:=2;
    e3:Integer:=3;
    factor1:P:=ran1*p1+ran2;
    factor2:P:=ran3*p2-ran4;
    factor3:P:=factor1*factor2+ran5*p1^e2+factor1*p1+ran1;
    factor4:P:=factor3+factor1-ran6;
    factor5:P:=factor4*p2^e3-factor2+ran7*p1+ran8;
    factor6:P:=factor5+(ran9+factor1)*p1+ran1;
    factor7:P:=factor2^e2+ran3+p1;
    factor8:P:=factor5+factor2+ran1;
    a1:P:=factor3*factor7;
    b1:P:=factor4*factor7;
    g1:P:=factor7;
    a2:P:=factor5*factor6;
    b2:P:=factor5;
    g2:P:=factor5;
    a3:P:=factor7*p1;
    b3:P:=factor8*p1;
    g3:P:=p1;
    a4:P:=factor1;
    b4:P:=factor2;
    a5:P:=factor8;
    b5:P:=factor8;
    g5:P:=factor8;
  --Example style  giving the degree sequence of a,b,g
  --(2,2,10),  (5,5,5), (10, 10, 1), (10, 10, 0)
  --Also test with a=b 

    pack==>MultivariatePolynomialGcdPackage(R, V, P);
    bothZero(a, b) ==> zero? a and zero? b;
    status: Boolean := true;
    ok() ==> stdout;
    fail() ==> {status := false; stdout}
    import from pack,R;
    z:P:=0;
    myg1:=gcd(a1,b1)$pack;
    res1:=myg1-unitCanonical(g1)$pack;
    if (res1=z or bothZero(a1, b1)) then {ok() <<" 1 ok "<<newline;} else {fail()<<" 1 result is incorrect! "<<newline; }
    myg2:=gcd(a2,b2)$pack;
    res2:=myg2-unitCanonical(g2)$pack;
    if (res2=z or bothZero(a2, b2)) then {ok()<<" 2 ok "<<newline;} else {fail()<<" 2 result is incorrect! "<<newline; }
    myg3:=gcd(a3,b3)$pack;
    res3:=myg3-unitCanonical(g3)$pack; 
    if (res3=z or bothZero(a3, b3)) then {ok()<<" 3 ok "<<newline;} else {fail()<<" 3 result is incorrect! "<<newline; }

--    myg4:=gcd(a4,b4)$pack;
--    res4:=ground? myg4;
--    if (res4) then {stdout<<" 4 ok "<<newline;} else {stdout<<" 4 result is incorrect! "<<newline; }
--    TEST FAILED: reaches "never"

    myg5:=gcd(a5,b5)$pack;
    res5:=myg5-unitCanonical(g5)$pack;
    if (res5=z or bothZero(a5, b5)) then {ok()<<" 5 ok "<<newline;} else {fail()<<" 5 result is incorrect! "<<newline; }

    -- big example
    --factor9:P:=factor1*factor2*factor3*factor4*factor5+factor5;
    --factor10:P:=factor1*factor5*factor6*factor7*factor8;
    --myg6:=gcd(factor9,factor10)$pack;
    --res5:=myg6-unitCanonical(factor5)$pack;
    --stdout<<"bigone is "<<res5<<newline;
    -- bit example over

    status => {stdout<<"result is correct"<<newline;true;}
    stdout << "result is incorrect" << newline;
    status
}





multipolygcd__test(): Boolean == {
	import from Symbol;
	r: Boolean := true;
	r := r and gcdTest(INT, V1, SMPI, variable(1)$V1, variable(2)$V1);
	r := r and gcdTest(SMPI, V2, SMPII, variable(1)$V2, variable(2)$V2);
	r := r and gcdTest(Q, V1, SMPQ, variable(1)$V1, variable(2)$V1);
	r := r and gcdTest(SMPQ, V2, SMPQQ, variable(1)$V2, variable(2)$V2);
	r := r and gcdTest(SPF, V1, SMPP, variable(1)$V1, variable(2)$V1);
	r := r and gcdTest(SMPP, V2, SMPPP, variable(1)$V2, variable(2)$V2);
	r
}


stdout << "test alg__defgcd.as .... " << newline;
aldorTest("GCD in Z[w,x,y,z]", multipolygcd__test);
stdout << newline;

#endif
--------------------------------------------------------------------------------
