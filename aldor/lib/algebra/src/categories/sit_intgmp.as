-------------------------- sit_intgmp.as ---------------------------------
-- Copyright (c) Helene Prieto 2000
-- Copyright (c) INRIA 2000, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 2000, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

extend GMPInteger: IntegerCategory == add {
       import { int: Type; mpz__srcptr: Type; mpz__ptr: Type; mpf__ptr: Type; Ptr: Type } from Foreign C("gmp.h");
       import {
		mpz__divexact: (mpz__ptr, mpz__srcptr,mpz__srcptr) -> ();
		mpz__gcdext: (mpz__ptr,mpz__ptr,mpz__ptr,mpz__srcptr, mpz__srcptr) -> ();
		mpz__tdiv__r: (mpz__ptr,mpz__srcptr, mpz__srcptr) -> ();
		mpz__tdiv__qr: (mpz__ptr,mpz__ptr,mpz__srcptr,mpz__srcptr) -> ();
	} from Foreign C("gmp.h");

	local gmpIn(a: %): mpz__srcptr == a pretend mpz__srcptr;
	local gmpIn(a: %): mpz__ptr == a pretend mpz__ptr;
	local gmpOut(a: mpz__srcptr): % == a pretend %;

#if GMP
	-- Those 2 assume that Integer == GmpInteger
	integer(u:%):Integer		== u;
	coerce(n:Integer):%		== n;
#else
	-- This assumes that Integer == AldorInteger
	integer(u:%):AldorInteger	== u::AldorInteger;
#endif

	local NULL:% == nil$Pointer pretend %;

	remainder!(a:%, b:%):% == { 
		zero? a => 0;
		one? a => { unit? b => 0; 1 }
		mpz__tdiv__r(gmpIn a, gmpIn a, gmpIn b);
		a;
	}

	divide!(a:%, b:%, q:%):(%, %) == {
		import from Partial %;
		zero? a => (0, 0);
		one? a => {
			failed?(b1 := reciprocal b) => (0, 1);
			(copy!(q, retract b1), 0);
		}
		if zero? q or one? q then q := new();
		r:% := new();
		mpz__tdiv__qr(gmpIn q, gmpIn r, gmpIn a, gmpIn b);
		(q, r);
	}

	quotient(x:%, y:%): % == {
		one? y => x;
		q:% := new();
		mpz__divexact(gmpIn q, gmpIn x, gmpIn y);
		q;
	}

	local gcdext(a: %, b: %): (%, %) == {
		g:% := new();
		s:% := new();
		mpz__gcdext(gmpIn g, gmpIn s,gmpIn(NULL), gmpIn a, gmpIn b);
		(g, s)
	}

	extendedEuclidean(a:%, b:%): (%,%,%) == {
		import from MachineInteger;
		(g, s) := gcdext(a, b);
		s := remainder!(s, b);
		(g, s, quotient(g - a * s, b));
	}

	extendedEuclidean(a:%, b:%, c:%):Partial Cross(%, %) == {
		zero? c => [(0, 0)];
		import from Partial %;
		zero? b => {
			zero? a or failed?(u := exactQuotient(c, a)) => failed;
			[(retract u, 0)];
		}
		zero? a => {
			failed?(u := exactQuotient(c, b)) => failed;
			[(0, retract u)];
		}
		(g, s) := gcdext(a, b);
		failed?(u := exactQuotient(c, g)) => failed;
		s := remainder!(times!(s, retract u), b);
		[s, quotient(c - a * s, b)];
	}

	diophantine(a:%, b:%, m:%):Partial % == {
		assert(~zero? m);
		zero?(b := b rem m) => [0];
		zero?(a := a rem m) => failed;
		(g, c) := gcdext(a, m);
		failed?(u := exactQuotient(b, g)) => u;
		[remainder!(times!(c, retract u), m)];
	}

	-- TEMPORARY: THOSE 6 DEFAULTS ARE NOT TAKEN FROM IntegerCategory!!!
	canonicalUnitNormal?:Boolean	== true;
	relativeSize(n:%):MachineInteger== { zero? n => 0; length n }
	unit?(x:%):Boolean		== one? x or x = -1;
	unitNormal(x:%):(%, %, %) == {
		import from MachineInteger;
		sign(x) < 0 => (-x,-1,-1);
		(x, 1, 1)
	};
	cutoff(t:MachineInteger):MachineInteger == {
		t = CUTOFF__KARAMULT => 12;
		-1;
	}

	-- TEMPORARY: CANNOT OVERLOAD (BUG 1272)
	-- unitNormal(x:%, z:%):(%, %) == {
	unitNormalize(x:%, z:%):(%, %) == {
		import from MachineInteger;
		sign(x) < 0 => (-x,-z);
		(x, z)
	}

        getPrimeOfSize(d: MachineInteger): Partial % == failed;


}

