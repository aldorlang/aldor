--------------------------------------------------------------------------------
--
-- sm_pr0pkgt.as: A package for testing domains from PolynomialRing0.
--
--------------------------------------------------------------------------------
--  Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright: INRIA, UWO and University of Lille I, 2001--2002
-- Copyright: Marc Moreno Maza
--------------------------------------------------------------------------------

-- pkg PolynomialRing0TestPackage

#include "algebra"
#include "aldorio"

macro {
	B == Boolean;
        Z == Integer;
	N == Integer;
}

PolynomialRing0TestPackage(
			R: Join(ArithmeticType, ExpressionType),
			V: Join(TotallyOrderedType, ExpressionType),
			P: PolynomialRing0(R,V)): with {
	randomN: Integer -> Integer;
	testVariable: (V, printing?: Boolean == true, debugging?: Boolean == true) -> Integer;
	testTerm: (R, List V, List N, printing?: Boolean == true, debugging?: Boolean == true) -> Integer;
	testPoly: (List Cross (R, List V, List N), printing?: Boolean == true, debugging?: Boolean == true) -> Integer;
        PRTest: (List List Cross (R, List V, List N), printing?: Boolean == true, debugging?: Boolean == true) -> Integer;
	equalityTest: (printing?: Boolean, debugging?: Boolean, T: ExpressionType, s1: String, t1: T, s2: String,  t2: T, n: Integer) -> Integer;
	makeZoo: (List R, List V, N, N) -> List List Cross (R, List V, List N);

} == add {

	import from B, Z, N, R, V, P;
	S == Set(V);

	randomN(n: Z): Z == {
		import from MachineInteger, RandomNumberGenerator;
		((randomInteger() :: Z) rem (n));
	}

	equalityTest(printing?: Boolean, debugging?: Boolean, T: ExpressionType, s1: String, t1: T, s2: String,  t2: T, n: Z): Z == {
		if (t1 ~= t2) or debugging?  then {
				stdout << s1 << t1 << newline;
				stdout << s2 << t2 << newline;
				
		}
		if (t1 ~= t2) then {
			stdout << "ERROR " << n << newline;
			1;
		} else {
			if debugging? then stdout << "sub test " << n << newline;
			0;
		}
	}
	testVariable(v: V, printing?: Boolean == true, debugging?: Boolean == true): Integer == {
		if debugging? then stdout << newline << "Entering testVariable ... " << newline << newline;
		local errors: Z := 0;
		local n: Z := 1; 
		local p: P := v :: P;
		errors := errors + equalityTest(printing?,debugging?,B,"variable?(p) ? ", variable?(p), "true ? ", true, n);
		n := n + 1;
		errors := errors + equalityTest(printing?,debugging?,V,"variable(p) ? ", variable(p), " v ? ", v, n);
		if debugging? then stdout << "... Exiting testVariable ... " << newline << newline;
		errors;
	}
	testTerm(r: R, lv: List V, ld: List N, printing?: Boolean == true, debugging?: Boolean == true): Integer == {
		if debugging? then stdout << newline << "Entering testTerm ... " << newline << newline;
		local errors: Z := 0;
		local n: Z := 10; 
		import from List V, Set V, List N, MachineInteger;
		zero? r => errors;
		local sv: S := bracket generator lv;
		assert(#lv = #ld);
		if (#lv = 1) then errors := errors + testVariable(first(lv),printing?, debugging?);

		local p: P := r::P;
		if debugging? then stdout << " p ? " << p << newline;
		errors := errors + equalityTest(printing?,debugging?,B,"ground?(p) ? ",ground?(p),"true ? ", true, n);
		ground? p => errors;
		n := n + 1;
		if debugging? then stdout << " before loop,  p ? " << p << newline;
		for d in ld for v in lv  repeat {
			p := p * term(1,v,d);
		}
		if debugging? then stdout << " after loop, p ? " << p << newline;
		errors := errors + equalityTest(printing?,debugging?,P," p ? ",p, "term(r,lv,ld) ? ", term(r,lv,ld), n);
		n := n + 1;

		p := term(1,lv,ld);
		errors := errors + equalityTest(printing?,debugging?,P," p ? ",p, "term(1,variableProduct(p)) ? ", term(1,variableProduct(p)), n);
		n := n + 1;
		errors := errors + equalityTest(printing?,debugging?,P," p ? ",p, "term(1,degrees(p)) ? ", term(1,degrees(p)), n);
		n := n + 1;

		if (#sv = #lv) then {
			for v in lv for d in ld repeat {
				errors := errors + equalityTest(printing?,debugging?,N, "degree(p,v) ? ", degree(p,v), " d ? ", d, n);
			}
		}
		if debugging? then stdout << "... Exiting testTerm ... " << newline << newline;
		errors;
	}
	testPoly(lrlvld: List Cross (R, List V, List N), printing?: Boolean == true, debugging?: Boolean == true): Integer == {
		if debugging? then stdout << newline << "Entering testPoly ... " << newline << newline;
		local errors: Z := 0;
		local n: Z := 20; 
		import from List V, List N, Cross (R, List V, List N), List Cross (R, List V, List N);
		assert(not empty? lrlvld);
		local r: R; local lv: List V; local ld: List N;
		local p: P := 0;
		local t: P;
		local sv: S := empty;
		for rlvld in lrlvld repeat {
			(r, lv, ld) := rlvld;
			t := term(r,lv,ld);
			p := p + t;
			sv := union(sv,bracket(generator(lv))$S)$S;
			errors := errors + testTerm(r, lv, ld, printing?, debugging?);
		}
		if printing? then stdout << " p ? " << p << newline;
		ground? p => errors;
--		errors := errors + equalityTest(printing?,debugging?,S pretend ExpressionType,"sv ? ", sv, "variables(p)? ", [variables p]$S, n);
--		n := n + 1;
		if not ground?(p) then {
			errors := errors + equalityTest(printing?,debugging?,V,"mainVariable(p) ? ", mainVariable(p), "first variables(p) ? ", first [variables p]$List(V), n);
		}
		n := n + 1;
		errors := errors + equalityTest(printing?,debugging?,B,"univariate?(p) ? ", univariate?(p), "one? # sv ? ", empty? rest [variables p]$List(V), n);
		n := n + 1;
		p := 0;
		local c1, c2, c3: P;
		local d, d1, d2, d3: N;
		for rlvld in lrlvld repeat {
			(r, lv, ld) := rlvld;
			t := term(r,lv,ld);
			q := p + t;
			for v in sv repeat {
				d := degree(q,v);
				c1 := coefficient(p,v,d);
				c2 := coefficient(t,v,d);
				c3 := coefficient(q,v,d);
				errors := errors + equalityTest(printing?,debugging?,P,"c1 + c2 ? ", c1 + c2, "c3 ? ", c3, n);
			}
			p := q;
		}
		n := n + 1;
		p := 0;
		for rlvld in lrlvld repeat {
			(r, lv, ld) := rlvld;
			t := term(r,lv,ld);
			q := p + t;
			for v in sv repeat {
				d1 := degree(p,v);
				d2 := degree(t,v);
				d3 := degree(q,v);
				if (d1 ~= d2) then errors := errors + equalityTest(printing?,debugging?,N,"max(d1,d2) ? ", max(d1,d2), "d3 ? ", d3, n);
			}
			p := q;
		}
		if debugging? then stdout << "... Exiting testPoly ... " << newline << newline;
		errors;
	}
        PRTest(llrlvld: List List Cross (R, List V, List N), printing?: Boolean == true, debugging?: Boolean == true): Integer == {
		if debugging? then stdout << newline << "Entering PRTest ... " << newline << newline;
		local errors: Z := 0;
		local n: Z;
		import from List List Cross (R, List V, List N), MachineInteger;
		for lrlvld in llrlvld repeat {
			n := testPoly(lrlvld,printing?,debugging?);
			if debugging? then stdout << " n ? " << n << newline;
			errors := errors + n;
		}
		if debugging? then stdout << "... Exiting PRTest ... " << newline << newline;
		errors;
	}
	makeZoo(lr: List R, lv: List V, degMax: N, zooSize: N): List List Cross (R, List V, List N) == {
		assert(degMax > 0);
		assert(zooSize > 1);
		import from MachineInteger;
		termsMax: N := (#lr) :: Z;
		assert(termsMax >= 2);
		varsNum: N := (#lv) :: Z;
		assert(varsNum > 0);
		local llrlvld: List List Cross (R, List V, List N) := [];
		local lrlvld: List Cross (R, List V, List N);
		local rlvld: Cross (R, List V, List N);
		for i in 1..zooSize repeat {
			lrlvld := [];
			terms: N := randomN(termsMax) + 1;
			clr := lr;
			while (terms > 0) repeat {
				r := first clr; clr := rest clr;
				ld: List(N) := [randomN(degMax) for j in 1..varsNum];
				rlvld := (r,lv,ld);
				lrlvld := cons(rlvld, lrlvld);
				terms := terms - 1;
			}
			llrlvld := cons(lrlvld, llrlvld);
		}
		llrlvld;
	}
}
