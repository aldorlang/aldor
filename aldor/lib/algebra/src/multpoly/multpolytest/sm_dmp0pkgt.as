--------------------------------------------------------------------------------
--
-- sm_dmp0pkgt.as: A package for testing domains built with  
--                 DistributedMultivariatePolynomial0
--
--------------------------------------------------------------------------------
--  Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright: INRIA, UWO and University of Lille I, 2001--2002
-- Copyright: Marc Moreno Maza
--------------------------------------------------------------------------------

-- pkg DistributedMultivariatePolynomial0TestPackage

#include "algebra"
#include "aldorio"

macro {
	B == Boolean;
        Z == Integer;
}

DistributedMultivariatePolynomial0TestPackage(
                       R: Join(ArithmeticType, ExpressionType),
                       E: GeneralExponentCategory,
                       P: Join(CopyableType, IndexedFreeModule(R, E))): with {
	makeZoo: (List R, List E, Z) -> List P;
	DMPTest1: (P, printing?: Boolean == true, debugging?: Boolean == true) -> Integer;
        DMPTest: (List P, printing?: Boolean == true, debugging?: Boolean == true) -> Integer;
	equalityTest: (b: Boolean, T: ExpressionType, s1: String, t1: T, s2: String,  t2: T, n: Integer) -> Integer;
} == add {
	import from Z, B, R, E, P, List(R), List(E), List(P);
        makeZoo(lr: List R, le: List E, n: Z): List P == {
		assert(not empty? lr);
		assert(#lr = #le);
		assert(n >= #lr :: Z);
		import from MachineInteger, RandomNumberGenerator;
		-- a positive integer no greater than #lr
		local zz(): Z == (randomInteger() :: Z) rem ((#lr + 1):: Z);
		-- an element of P
		local pp(l:Z): P == {
			local p: P := 0;
			for r in lr for e in le repeat {
				if zz() <= l then p := p + term(r,e);
			}
			p;
		}
		local lp: List P; local q: P;
		lp := [];
 		q := 0;
		for i in 1..n repeat {
			q := pp(zz());
			-- stdout << " q ? " << q << newline;
			lp := cons(q,lp);
		}
		lp;
	}
	equalityTest(b: Boolean, T: ExpressionType, s1: String, t1: T, s2: String,  t2: T, n: Z): Z == {
		if (t1 ~= t2) then {
			if b then {
				stdout << s1 << t1 << newline;
				stdout << s2 << t2 << newline;
				stdout << "ERROR " << n << newline;
			}
			1;
		} else {
			0;
		}
	}
	DMPTest1(p:P, printing?: Boolean == true, debugging?: Boolean == true): Integer == {
		if printing? then stdout << newline << " p ? " << p << newline;

		local r: R; local e: E; local q: P;
		local errors: Z := 0;
		local n: Z := 1; 

		deg__p: E := degree(p);
		if printing? then stdout << "degree(p) ? " << deg__p << newline;
		tdeg__p: E := trailingDegree(p);
		if printing? then stdout << "trailing degree ? " << tdeg__p <<  newline;
		lm__p: P := leadingMonomial(p);
		if printing? then stdout << "leading monomial ? " <<  lm__p <<  newline;
		tm__p: P := trailingMonomial(p);
		if printing? then stdout << "trailing monomial ? " << tm__p <<  newline;
		lc__p: R := leadingCoefficient(p);
		if printing? then stdout << "leading coefficient ? " << lc__p << newline;
		tc__p: R := trailingCoefficient(p);
		if printing? then stdout << "trailing coefficient ? " << tc__p << newline;
		red__p: P := reductum(p);
		if printing? then stdout << "reductum ? " << red__p << newline;
		-- errors := errors + equalityTest(printing?,B,"ground?(p) ? ",ground?(p),"zero?(deg__p) ? ",zero?(deg__p),n);
		-- n := n + 1;
		errors := errors + equalityTest(printing?,B,"term?(p)  ",term?(p),"zero?(red__p)  ",zero?(red__p),n);
		n := n + 1;
		if (not zero? p) then errors := errors + equalityTest(printing?,P,"term(1, deg__p) ? ",term(1, deg__p), "lm__p ? ", lm__p, n);
		n := n + 1;
		if (not zero? p) then errors := errors + equalityTest(printing?,P,"term(1, tdeg__p) ? ",term(1, tdeg__p), "tm__p ? ", tm__p, n);
		n := n + 1;
		errors := errors + equalityTest(printing?,P,"red__p + lc__p * lm__p ? ",red__p + lc__p * lm__p,"p ? ", p, n);
		n := n + 1;
		errors := errors + equalityTest(printing?,P,"red__p + lc__p * lm__p - p ? ",red__p + lc__p * lm__p - p ,"0 ? ", 0, n);
		n := n + 1;
		errors := errors + equalityTest(printing?,P,"- red__p +  p - lc__p * lm__p ? ",- red__p +  p - lc__p * lm__p  ,"0 ? ", 0, n);
		pp: P := 0;
		for re in generator(p) repeat {
			(r, e) := re;
			pp := pp + term(r,e);
		}
		n := n + 1;
		if (p ~= pp) or not zero? (p - pp) then {
			if printing? then stdout << "ERROR " << n << newline;
			errors := errors + 1;
		}
		pp: P := 0;
		for re in terms(p) repeat {
			(r, e) := re;
			pp := pp + term(r,e);
		}
		n := n + 1;
		if (p ~= pp) or not zero? (p - pp) then {
			if printing? then stdout << "ERROR " << n << newline;
			errors := errors + 1;
		}
		pp := 0;
		for rq in support(p) repeat {
			(r, q) := rq;
			pp := pp + r * q;
		}
		if (p ~= pp) or not zero? (p - pp) then {
			if printing? then stdout << "ERROR " << n << newline;
			errors := errors + 1;
		}
		errors;
	}
	DMPTest2(p:P, printing?: Boolean == true, debugging?: Boolean == true): Integer == {
		if printing? then stdout << newline << " p ? " << p << newline;

		local errors: Z := 0;

		zero? p => errors;

		local n: Z := 10; 

		local pp: P := copy(p);
		errors := errors + equalityTest(printing?,P,"p ? ",p, "pp ? ", pp, n);
		n := n + 1;
		minus!(pp);
		errors := errors + equalityTest(printing?,P,"p ? ",p, "minus!(pp); -pp ? ", -pp, n);
		n := n + 1;
		errors := errors + equalityTest(printing?,P,"add!(copy(p),-reductum(p)) ? ", add!(copy(p),-reductum(p)), "p - reductum(p) ? ", p - reductum(p), n);
		n := n + 1;
		minus!(pp);
		errors := errors + equalityTest(printing?,P,"p ? ",p, "pp ? ", pp, n);
		n := n + 1;
		errors := errors + equalityTest(printing?,P,"minus!(copy reductum(p),copy p) ? ", minus!(copy reductum(p),copy p), "reductum(p) -p ? ", reductum(p) -p, n);
		n := n + 1;
		pp := copy p;
		minus!(pp);
		errors := errors + equalityTest(printing?,P,"-p ? ",-p, "pp ? ", pp, n);
		n := n + 1;
		local r: R := 1 + 1;
		pp := pp + p - pp + p;
		errors := errors + equalityTest(printing?,P,"2 * p ? ",r*p, "pp ? ", pp, n);
		n := n + 1;
		times!(r,pp);
		errors := errors + equalityTest(printing?,P,"4 * p ? ",r*r*p, "pp ? ", pp, n);
		n := n + 1;

		term?(p) => errors;

		pp := (p -reductum(p)) + reductum(reductum(p));
		local e: E := degree(reductum(p));
		r := leadingCoefficient(reductum(p));
		pp := copy pp;
		errors := errors + equalityTest(printing?,P,"add!(pp,r,e) ? ", add!(pp,r,e), "p ? ", p, n);
		n := n + 1;
		pp := (reductum(p) -p) + (reductum(p));
		pp := copy pp;
		pp := setCoefficient!(pp,degree(p),leadingCoefficient(p));
		errors := errors + equalityTest(printing?,P,"p ? ",p, "pp ? ", pp, n);
		n := n + 1;
		pp := (p -reductum(p)) + reductum(reductum(p));
		pp := copy pp;
		pp := setCoefficient!(pp,e,r);
		errors := errors + equalityTest(printing?,P,"p ? ",p, "pp ? ", pp, n);

		errors;
	}
	DMPTest(lp: List(P), printing?: Boolean == true, debugging?: Boolean == true): Integer == {

		import from MachineInteger;
		if printing? then stdout << " #lp ? " << #lp << newline << newline;
		local errors: Z := DMPTest1(0,printing?,debugging?);
		for p in lp repeat {
			if debugging? then stdout << "." ;
			errors := errors + DMPTest1(p,printing?,debugging?);
			errors := errors + DMPTest2(p,printing?,debugging?);
		}
		stdout << newline;
		errors;
	}
}
