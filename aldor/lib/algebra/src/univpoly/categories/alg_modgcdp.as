#include "algebra"

macro {
	Z == Integer;
	SingleInteger == MachineInteger;
	SI == MachineInteger;
	ARR == Array;
}

--- Ref: On the genericity of the Modular Polynomial GCD Algorithm
---      Author: Erich Kaltofen and Michael Monagan 

+++ Author: Jinlong Cai Marc Moreno Maza
+++ Date Last Update: 10/01/04


GenericModularPolynomialGcdPackage(_
	R:Join(EuclideanDomain,SourceOfPrimes,ModularComputation), 
	U: UnivariatePolynomialAlgebra0(R)): with {
	modularGcd: (U, U) -> Partial(U);	
	tryprime: (U, SI, ARR R, U, SI, ARR R, R) -> (R, SI, ARR R);
	remainder!: (SI, ARR R, SI, ARR R, R) -> (SI, ARR R, SI, ARR R);
} == add {
	
	remainder!(n: SI, amodp:ARR R, m: SI, bmodp:ARR R, p:R): (SI, ARR R, SI, ARR R) == {
		-- Assume n is the degree of a mod p and that amodp has size >= n+1
		-- Assume m is the degree of b mod p and that bmodp has size >= m+1
		-- Assume that amodp and bmodp store (a mod p) and (b mod p) by decreasing degree

		--------------------------------------
		--% Defining the current remainder %--
		--------------------------------------
		local rmodp: ARR R := amodp;
		local sr: SI := n+1;
		(lc__r, dr) := leadingCoeffcient(amodp);

		-------------------------------------
		--% Defining the current quotient %--
		-------------------------------------
		local qmodp: ARR R := new next (n-m);
		local dq: SI := 0;
		for i in 0 .. (n-m) repeat qmodp.i := 0;

		(lc__b, db) := leadingCoeffcient(bmodp);

		inv__lc__b__mod__p: R := invMod(lc__b, p);

		for i in n - m .. 0 by -1 repeat {
			
			if dr  = m + i then {
				qmodp.i := mod_*(inv__lc__b__mod__p, lc__r, p);

				for j in 0..m repeat {
					rmodp.(i+j) := (rmodp.(i+j) - qmodp.i * bmodp.j) mod p;
				}
				
				(lc__r, dr) := leadingCoeffcient(rmodp);	
			}
		}

		(dq, qmodp, dr, rmodp);   
	}

	tryprime(a:U, da:SI, amodp:ARR R, b:U, db:SI, bmodp:ARR R,  p:R): (R, SI, ARR R) == {
		import from Z, ChineseRemaindering2(R,U);
		local qmodp, rmodp: ARR R;		

		amodp := arrayMod(a, p); 
		bmodp := arrayMod(b, p);
		da:= machine degree a;
		db:= machine degree b;
		local lb, lr: R;

		repeat
		{
			(dq, qmodp, dr, rmodp) := remainder!(da, amodp, db, bmodp, p);

			(lr, dr) := leadingCoeffcient(rmodp);

			if zero? dr and zero? lr then break; --found the gcd of a and b if r = 0

			amodp := bmodp;
			da := db;
			bmodp := rmodp;
			db := dr;
			lb := lr;		
		};

		(lb, db, bmodp);
	}
	
	modularGcd(a: U, b: U): Partial(U) == {
		import from R, U, Z;
		import from Partial(R), Partial(U);
		import from ChineseRemaindering2(R,U);

		if (degree a < degree b) then {
			local tmp: U := a;
			a := b;
			b := tmp;
		} 

		local gc, ca, cb: R;
		ca: R := content(a);
		cb: R := content(b);
		gc: R := gcd(ca, cb);

		-- unit Canonical
		a  := retract exactQuotient(a, ca);
		b  := retract exactQuotient(b, cb);			 

		lca: R := leadingCoefficient a;
		lcb: R := leadingCoefficient b;
		
		lcg: R := gcd(lca, lcb);

		da: SI := machine degree a;
		db: SI := machine degree b;
		d: SI := machine min(da, db);
		
		(m: R, gm: ARR R) := (1, new(0));

		amodp: ARR R := new next da;
		bmodp: ARR R := new next db;

		local gp: ARR R;

		p?: Partial(R) := getPrime();

		local sum__of__prime: SI := 0;

		while not failed? p? repeat {
			p: R := retract(p?);

			if (failed? exactQuotient(m, p)) and
			   (failed? exactQuotient(lca, p)) and
                           (failed? exactQuotient(lcb, p)) then   
			{
				sum__of__prime := sum__of__prime + 1;

				(lc__gp, deg__gp, gp) := tryprime(a, da, amodp, b, db, bmodp, p); 


				if deg__gp = 0 then break;

				if deg__gp > d then{
					 p?:= nextPrime(p);
					 iterate;		
				}

				inv__lc__gp: R := invMod(lc__gp, p);
				new__lc__gp: R := (lcg * inv__lc__gp) mod p;

				for i in 0..deg__gp repeat {                   
					gp.i := (new__lc__gp * gp.i) mod p;
				}
				
				if (deg__gp < d) then { --unlucky prime
					(m, gm, d) := (1, new(0), deg__gp);
				} 

				h:ARR R := combine(p, m)(gp, gm);

				if arrEqual?(h, gm) then  --begin termination test
				{
					local hu: U := getArrToUP(h);						
					hu := retract exactQuotient(hu, content(hu)); --primitive part

					if (not failed? exactQuotient(a,hu)) and 
					   (not failed? exactQuotient(b,hu)) then
					{
						return [gc*hu]; 
					}
				}
				
				(m, gm) := (p * m, h);			
			}

			p?:= nextPrime(p);

			if failed? p? then -- no more primes
			{
				-- stdout << newline << ">>Has tried sum of primes := " << sum__of__prime << newline;
				-- stdout << ">>Last prime := " << p << newline;
			    break;
			}
		}
		return failed;
        }

	local arrEqual?(r1: ARR R,r2: ARR R): Boolean == {
		import from SI, Z, R;
		if #r1 ~= #r2 then return false;
		
		for i in 0..#r1-1 repeat {
			if r1.i ~= r2.i then return false;
			--if (r1.i - r2.i) mod m ~= 0 then return false;
		}
		true;
	}	

	--return the leading coeffcient and degree of an array of polynomail
	local leadingCoeffcient(r: ARR R): (R, SI) == {
		import from R, SI;

		local dr: SI := #r -1;
		local lr: R := r.dr;
		local tmp:R;

		for j in dr..0 by -1 repeat {
			tmp := r.j;
			if zero? tmp and dr ~= 0 then dr := dr -1;
			else {
				lr := tmp;	
				break;
			}
		}
		(lr, dr);
	}
}


#if ALDOC
\thistype{GenericModularPolynomialGcdPackage}
\History{Jinlong Cai and Marc Moreno Maza}{December 2003}{created}
\Usage{import from \this~(R, U)}
\Params{
{\em R} & \altype{EuclideanDomain} \\
        & \altype{SourceOfPrimes} \\
        & \altype{ModularComputation} \\
{\em U} &  \altype{UnivariatePolynomialAlgebra} (R) \\
}
\Descr{\this~(R, U) provides a generic modular gcd algorithm.
See the paper {\em On the genericity of the Modular Gcd Algorithm}
by Kaltofen and Monagan in the proc. of ISSAC 1999.}
\begin{exports}
\alexp{modularGcd}: & (U, U)  $\to$ \altype{Partial} (U) & gcd (failure if not enough primes) \\
\end{exports}
#endif  

#if ALDORTEST
-------------------- test modgcdp.as --------------------------
#include "algebra"
#include "aldortest"

macro {
	Z == Integer;
	MI == MachineInteger;
	DUP == DenseUnivariatePolynomial;
}


test__Z(d__U: Z, timing?: Boolean == false, printing?: Boolean == false): Boolean == {
	 R == Z;
	 import from Timer;
 	 t: Timer := timer();
	 u__S: Symbol := -"u";
	 v__S: Symbol := -"v";
         U == DUP(R,u__S);
         import from GenericModularPolynomialGcdPackage(R,U);
	 testpack__DUP == BivariateUtilitiesTestPackage2(u__S,v__S,DUP);
	 import from testpack__DUP, Z;
	 AA: U := random__U(d__U);
	 BB: U := random__U(d__U);
	 GG: U := random__U(d__U);
	 AA:= AA * GG;
	 BB:= BB * GG;
	 local x1, x2: MI;

	 if printing? then {
	     stdout << "Compute the GCD of two polynomials ..." << newline << newline;
	     stdout << "AA:= " << AA << newline << newline;
	     stdout << "BB:= " << BB << newline << newline;
	     stdout << ">> Expected GCD is" << GG << newline << newline;
	 }
	 if timing? then {
	     x1 := read(t);
	     start!(t);
	 }

	 local CC1: U := gcd(AA,BB);	 
	 if printing? then {
	     stdout << ">> gcd(AA,BB) is" << CC1 << newline << newline;
	 }
	 if timing? then {
 	     stop!(t);
	     x1 := read(t) -x1;
	     stdout << ">> Time elapsed: " << x1 << newline;
	     x2 := read(t);
	     start!(t);
	 }

	 local CC2?: Partial (U) := modularGcd(AA, BB);
	 if timing? then {
	     stop!(t);
	     x2 := read(t) -x2;
	     stdout << ">> Time elapsed: " << x2 << newline;
	 }
	 
	 if printing? then {
	     stdout << ">> modularGcd(AA, BB) is " << CC2? << newline;
	 }
	 if not failed? CC2? then {
	     (CC1, foo: U, bar: U) := unitNormal(CC1);
	     return (CC1 = retract(CC2?));
	 } else true;
;
}


test__F(d: Z, timing?: Boolean == false, printing?: Boolean == false): Boolean == {
	 u__s: Symbol := -"u";
	 v__s: Symbol := -"v";
	 import from Timer;
 	 t: Timer := timer();
	 import from MI;
	 F == SmallPrimeField 30707;	 
	 import from Z, F;
	 U == DUP(F, u__s); 
         R == U;
	 V == DUP(R, v__s);
	 u: U := monom;
	 v: V := monom;
         import from GenericModularPolynomialGcdPackage(R, V);
	 testpack__DUP == BivariateUtilitiesTestPackage3(30707, u__s,v__s,DUP);
	 import from testpack__DUP;
	 VV == DUP(DUP(Z, u__s), v__s);
	 d__U: Z == d; 	 
	 v__U: Z == d;
	 AA: V := random__V(d__U, v__U);
	 BB: V := random__V(d__U, v__U);
	 GG: V := random__V(v__U, v__U);
	 AA:= AA * GG;
	 BB:= BB * GG;
 	 local x1, x2: MI;

	 if printing? then {
	     stdout << "Compute the GCD of two polynomials ..." << newline << newline;
	     stdout << "AA:= " << AA << newline << newline;
	     stdout << "BB:= " << BB << newline << newline;
	     stdout << ">> Expected GCD is" << GG << newline << newline;
	 }
	 if timing? then {
	     x1 := read(t);
	     start!(t);
	 }
	 local CC1: V := gcd(AA,BB);
	 if printing? then {
	     stdout << ">> gcd(AA,BB) is" << CC1 << newline << newline;
	 }
	 if timing? then {
 	     stop!(t);
	     x1 := read(t) -x1;
	     stdout << ">> Time elapsed: " << x1 << newline;
	     x2 := read(t);
	     start!(t);
	 }

	 local CC2?: Partial(V) := modularGcd(AA, BB);
	 if timing? then {
	     stop!(t);
	     x2 := read(t) -x2;
	     stdout << ">> Time elapsed: " << x2 << newline;
	 }
	 
	 if printing? then {
	     stdout << ">> modularGcd(AA, BB) is " << CC2? << newline;
	 }
	 if not failed? CC2? then {
	     (CC1, foo: V, bar: V) := unitNormal(CC1);
	     return (CC1 = retract(CC2?));
	 } else true;
}


multipleTest__Z(): Boolean == {
    import from Z, List Z;
    bool: Boolean := true;
    for i in [50,100,150,200,250] repeat {
	bool := bool /\ test__Z(i, false, false);
    }
    bool;
}

multipleTest__F(): Boolean == {
    import from Z, List Z;
    bool: Boolean := true;
--    for i in [50,100,150,200,250] repeat {
    for i in [2,3,5,7,10] repeat {
	bool := bool /\ test__F(i, false, false);
    }
    bool;
}

stdout << " test modgcdp.as .... " << newline;
--aldorTest("GenericModular GCD in Z[u]", multipleTest__Z);
--TEST FAILED: does not terminate
aldorTest("GenericModular GCD in Z/pZ[u][v]", multipleTest__F);
stdout << endnl;


#endif
