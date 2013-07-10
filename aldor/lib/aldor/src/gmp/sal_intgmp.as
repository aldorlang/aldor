------------------------------- sal_intgmp.as -----------------------------
--
--  GMP integers
--
-- Copyright (c) Helene Prieto 2000
-- Copyright (c) INRIA 2000, Version 9-2-2000
-- Logiciel Salli ©INRIA 2000, dans sa version du 9/2/2000
---------------------------------------------------------------------------

#include "aldor"

macro {
	Z == MachineInteger;
	PTR == Pointer;
}

#if ALDOC
\thistype{GMPInteger}
\History{Helene Prieto}{9/2/2000}{created}
\Usage{import from \this}
\Descr{\this~implements arbitrary precision integers with the integer
arithmetic provided by GMP.}
\begin{exports}
\category{\altype{CopyableType}}\\
\category{\altype{IntegerType}}\\
\alexp{coerce}: & \altype{AldorInteger} $\to$ \% & conversion\\
\alexp{coerce}: & \% $\to$ \altype{AldorInteger} & conversion\\
\alexp{limbs}:
& \% $\to$ \altype{Generator} \altype{MachineInteger} & iteration\\
\alexp{new}: & () $\to$ \% & initialization (for calling GMP)\\
\end{exports}
#endif

GMPInteger: Join(CopyableType, IntegerType) with {
	coerce: AldorInteger -> %;
	coerce: % -> AldorInteger;
#if ALDOC
\alpage{coerce}
\Usage{m::\%\\ n::\altype{AldorInteger}}
\Signatures{
\name: & \altype{AldorInteger} $\to$ \%\\
\name: & \% $\to$ \altype{AldorInteger}\\
}
\Params{
{\em m} & \% & an \aldor integer\\
{\em n} & \altype{AldorInteger} & a gmp integer\\
}
\Descr{Those functions convert between \aldor and GMP integers.}
\Remarks{The conversion to an \altype{AldorInteger} can be
quadratic in the number of bits of the integer, which is expensive.}
#endif
	limbs: % -> Generator Z;
#if ALDOC
\alpage{limbs}
\Usage{ for d in \name~n repeat \{ \dots \} }
\Signature{\%}{\altype{Generator} \altype{MachineInteger}}
\Params{ {\em n} & \% & an integer\\ }
\Descr{This function allows the individual limbs of a GMP integer
to be iterated independently of the machine size.
This generator yields the limbs from the least to the most significant.}
\Remarks{The limbs of $n$ describe $|n|$, so combine it with
\alfunc{OrderedArithmeticType}{sign}
if you need a complete description of $n$.}
#endif
	new: () -> %;
#if ALDOC
\alpage{new}
\Signature{()}{\%}
\Retval{Returns an initialized GMP integer but with no value
stored into it. Results from this function can be used only
as parameters to explicit calls to {\tt mpz\_} functions.}
#endif
	export from IntegerSegment %;
} == add {
	Rep == PTR;

	macro Rec32 == Record(al:Z, sz:Z, lmbs:PTR);
	macro Rec64 == Record(szal:Z, lmbs:PTR);

	-- Cannot use the mpz_... names because they are macros in gmp.h
	import {
		____gmpz__init: Rep -> ();
		____gmpz__clear:Rep -> ();
		____gmpz__set: (Rep, Rep) -> ();
		____gmpz__set__si: (Rep, Z) -> ();
		____gmpz__set__str: (Rep, PTR, Z) -> Z;
		____gmpz__get__si: Rep -> Z;
		____gmpz__get__ui: Rep -> Z;
		____gmpz__add: (Rep,Rep,Rep) -> ();
		____gmpz__add__ui: (Rep,Rep,Z) -> ();
		____gmpz__sub: (Rep,Rep,Rep) -> ();
		____gmpz__sub__ui: (Rep,Rep,Z) -> ();
		____gmpz__mul: (Rep,Rep,Rep) -> ();
		____gmpz__mul__2exp: (Rep,Rep,Z) -> ();
		____gmpz__tdiv__q: (Rep,Rep,Rep) -> ();
		____gmpz__tdiv__r: (Rep,Rep,Rep) -> ();
		____gmpz__tdiv__qr: (Rep,Rep,Rep,Rep) -> ();
		____gmpz__fdiv__q__2exp: (Rep,Rep,Z) -> ();
		____gmpz__fdiv__r__ui: (Rep,Rep,Z) -> Z;
		____gmpz__divexact: (Rep,Rep,Rep) -> ();
		____gmpz__pow__ui: (Rep,Rep,Z) -> ();
		____gmpz__gcd: (Rep,Rep,Rep) -> ();
		____gmpz__neg: (Rep,Rep) -> ();
		____gmpz__abs: (Rep,Rep) -> ();
		____gmpz__cmp: (Rep,Rep) -> Z;
		____gmpz__and: (Rep,Rep,Rep) -> ();
		____gmpz__ior: (Rep,Rep,Rep) -> ();
		____gmpz__com: (Rep,Rep) -> ();
		____gmpz__scan1: (Rep,Z) -> Z;
		____gmpz__setbit: (Rep,Z) -> ();
		____gmpz__clrbit: (Rep,Z) -> ();
		____gmpz__size: Rep -> Z;
		____gmpz__sizeinbase: (Rep,Z) -> Z;
		-- GMP 3.0 novelties:
		____gmpz__getlimbn: (Rep, Z) -> Z;
	} from Foreign C;
	
	import from Z, Rep;

	local wordsize:Z	== bytes;
	local b64?:Boolean	== wordsize=8;

	local nlimbs(a:%):Z	== ____gmpz__size(rep a);

        new():%                 == {
		n: PTR := {
			b64? => [0,nil]$Rec64 pretend PTR;
			[0,0,nil]$Rec32 pretend PTR;
		}
                ____gmpz__init(n);
		per n;
	}

	local plimbs(a:%):PTR == {
		import from Rec32, Rec64;
		b64? => (rep(a) pretend Rec64).lmbs;
		(rep(a) pretend Rec32).lmbs;
	}

	-- HACK SINCE CANNOT USE ____gmpz__sgn BECAUSE IT IS A MACRO THAT TAKES
	-- A POINTER AND CANNOT BE USED WITH AN FiWord
	local hi32:Z == shift(4294967295, 32);	-- 32 1's and 32 0's
	sign(a:%):Z == {
		import from Rec32, Rec64;
		b64? => {
			-- low 32 = alloc, hi 32 = size (signed)
			sizeAlloc := (rep(a) pretend Rec64).szal;
			zero?(sizeAlloc /\ hi32) => 0;
			sign sizeAlloc;
		}
		sign((rep(a) pretend Rec32).sz);
	}

	-- GMP 3.0 Implementation
	limbs(a:%):Generator Z == generate {
		n := prev nlimbs a;
		for i in 0..n repeat yield ____gmpz__getlimbn(rep a, i);
	}

	-- GMP 2.0 Implementation
	-- limbs(a:%):Generator Z == generate {
	-- 	macro PZ == Record(x:Z);
	-- 	import from PZ;
	-- 	if ~nil?(p := plimbs a) then for i in 1..nlimbs a repeat {
	-- 		yield((p pretend PZ).x);
	-- 		p := (p::Z + wordsize)::PTR;
	-- 	}
	-- }

	coerce(a: Z): % == {
		c := new();
		____gmpz__set__si(rep c,a);
		c;
	}

	integer(a:Literal):%	== {
		e:% := new();
		____gmpz__set__str(rep e, a pretend PTR, 10::Z);
		e;
	}

	copy!(a:%, b:%):%	== {
		zero? a or one? a => copy b;
		____gmpz__set(rep a, rep b);
		a;
	}

	copy(a:%): %		== {
		zero? a or one? a => a;
		e:% := new();
		____gmpz__set(rep e,rep a);
		e;
	}

	0: % 			    == {
		h:% := new();
		____gmpz__set__si(rep h,0);
		h;
	}

	1: % 			    == {
		g:% := new();
		____gmpz__set__si(rep g,1);
		g;
	}

	lcm(a:%,b:%):%		== {
		ab := a * b;
		c := gcd(a, b);
		____gmpz__divexact(rep ab, rep ab, rep c);
		ab;
	}

	gcd(a:%,b:%):%		== {
		c:% := new();
		____gmpz__gcd(rep c,rep a,rep b);
		c;
	}

	bit?(a:%,b:Z): Boolean	== {
		c:Z :=____gmpz__scan1(rep a,b);
		c=b;
	}

	(a:%) quo (b:%):% 	== {
		c:% := new();
		____gmpz__tdiv__q(rep c,rep a,rep b);
		c;
	}

	(a:%) < (b:%):Boolean	== {
		r:Z := ____gmpz__cmp(rep a,rep b);
		r < 0;
	}

	(a:%) <= (b:%):Boolean	== {
		r:Z := ____gmpz__cmp(rep a,rep b);
		r<= 0;
	}

	(a:%) + (b:%):%		== {
		zero? a => b;
		zero? b => a;
		e:% := new();
		____gmpz__add(rep e, rep a, rep b);
		e;
	}

	(a:%) + (b:Z):%		== {
		zero? b => a;
		e:% := new();
		if (b >= 0) then ____gmpz__add__ui(rep e,rep a, b);
		else ____gmpz__sub__ui(rep e, rep a, -b);
		e;
	}

	(a:%) * (b:%):%		== {
		one? a => b;
		one? b => a;
		e:% := new();
		____gmpz__mul(rep e, rep a, rep b);
		e;
	}

	(a:%) - (b:%):%		== {
		zero? b => a;
		e:% := new();
		____gmpz__sub(rep e,rep a,rep b);
		e;
	}

	minus!(a:%):%		== {
		zero? a => a;
		one? a => -1;
		____gmpz__sub(rep a, rep 0, rep a);
		a;
	}

	-(a:%):%		== {
		zero? a => a;
		b:% :=  new();
		____gmpz__sub(rep b, rep 0, rep a);
		b;
	}

	minus!(a:%,b:%):%		== {
		zero? a => -b;
		one? a => 1 - b;
		____gmpz__sub(rep a, rep a, rep b);
		a;
	}

	add!(a:%,b:%):%		== {
		zero? a => copy b;
		one? a => b + 1@Z;
		____gmpz__add(rep a,rep a,rep b);
		a;
	}

	times!(a:%, b:%):%	== {
		zero? a => 0;
		one? a => copy b;
		____gmpz__mul(rep a, rep a, rep b);
		a;
	}

	xor(a:%,b:%):%		== {
		e:% := new();
		d:% :=  new();
		f:% :=  new();
		____gmpz__com(rep e,rep  b);
		____gmpz__and(rep d,rep a,rep e);
		____gmpz__com(rep f,rep a);
		____gmpz__and(rep e,rep f,rep b);
		____gmpz__ior( rep f,rep d,rep e);
		free! d;
		free! e;
		f;
	}

	local dummy:% == new();
	(a:%) mod (b:Z): Z	== ____gmpz__fdiv__r__ui(rep dummy, rep a, b);
	(a:%) > (b:%):Boolean	== ~(a<=b);
	(a:%) >= (b:%):Boolean	== ____gmpz__cmp(rep a,rep b) >= 0;
	max(a:%,b:%):%		== {a < b => b; a};
	min(a:%,b:%):%		== {a < b => a; b};
	one?(a:%):Boolean	== ____gmpz__cmp(rep a, rep 1) = 0;
	even?(a:%): Boolean	== even? ____gmpz__get__ui(rep a);
	odd?(a:%): Boolean	== odd? ____gmpz__get__ui(rep a);
	next(a:%):%		== a+(1@Z);
	prev(a:%):%		== a-1;
	machine(a:%): Z 	== ____gmpz__get__si(rep a);
	free!(a:%): ()		== ____gmpz__clear(rep a);
	zero?(a:%):Boolean	== zero? sign a;
	(a:%) = (b:%):Boolean	== ____gmpz__cmp(rep a,rep b) = 0;
	length(a:%): Z		== ____gmpz__sizeinbase(rep a, 2);

	abs(a:%):%		== { 
		b:% :=  new();
		____gmpz__abs(rep b,rep a);
		b;	
	}

	set(a:%,n:Z): %		== {
		b:% :=  new();
		____gmpz__set(rep b,rep a);
		____gmpz__setbit(rep b, n);
		b;
	}

	set!(a:%,n:Z): %	== {
		zero? a or one? a => set(a, n);
		____gmpz__setbit(rep a, n);
		a;
	}

	clear(a:%,n:Z): %	== {
		b:% :=  new();
		____gmpz__set(rep b,rep a);
		____gmpz__clrbit(rep b, n);
		b;
	}
	
	(a:%) ^ (b:Z): %	== {
		e:% := new();
		____gmpz__pow__ui(rep e,rep a, b);
		e;	
	}

	(a:%) rem (b:%): %		== {
		e:% := new();
		____gmpz__tdiv__r(rep e,rep a,rep b);
		e;
	}		

	divide(a:%,b:%):(%,%)	== {
		e:% := new();
		d:% :=  new();
		____gmpz__tdiv__qr(rep e,rep d,rep a,rep b);
		(e,d);
	}

	random():%		== {
		import from Z, RandomNumberGenerator;
		randomInteger()::%;
	}

	random(n:Z):% == {
		import from RandomNumberGenerator;
		assert(n > 0);
		r:% := 0;
		m := 8 * bytes$Z;
		for i in 1..n repeat
			r := add!(shift!(r, m), randomInteger()::%);
		r;
	}

	shift!(a:%,b:Z):%	== {
		zero? a or one? a => shift(a, b);
		if b > 0 then ____gmpz__mul__2exp(rep a, rep a, b);
			else ____gmpz__fdiv__q__2exp(rep a, rep a, -b); 
		a;
	}

	shift(a:%,b:Z):%	== {
		c:% := new();
		if b > 0 then ____gmpz__mul__2exp(rep c, rep a, b);
			else ____gmpz__fdiv__q__2exp(rep c, rep a, -b); 
		c;
	}

	~(a:%): % 		== {
		e:% := new();
		____gmpz__com(rep e,rep a);
		e;
	}

	coerce(a:AldorInteger):% == {
		macro PZ == Record(z:Z);
                import from Z, Boolean, PZ;
		zero?(sgn := sign a) => 0;
		if sgn < 0 then a := -a;
                (s, r) := divide(length a, 8);
                if ~zero? r then s := next s;
		x:% := new();
		____gmpz__set__si(rep x, 0);			-- x = 0
		____gmpz__setbit(rep x, b := 8 * s);		-- x = 2^(8s)
		ptr := plimbs x;
		assert(~nil? ptr);
		while s > 0 repeat {			-- must scan s bytes
			n:Z := 0;
			st:Z := 0;
			for i in 1..wordsize while s > 0 repeat {
				n := n \/ shift(machine(a) /\ 255, st);
				st := st + 8;
				s := prev s;
				a := shift(a, -8);
			}
			(ptr pretend PZ).z := n;
			ptr := (ptr::Z + wordsize)::PTR;
		}
		____gmpz__clrbit(rep x, b);
		if sgn < 0 then ____gmpz__neg(rep x, rep x);
		x;
	}

        (p:BinaryWriter) << (x:%):BinaryWriter == {
                import from GMPTools;
		writelimbs!(p, sign x, nlimbs x, limbs x);
        }

	<< (p:BinaryReader):%	== {
                import from GMPTools, Rec32, Rec64;
		sgn:Z := << p;				-- read sign first
		zero? sgn => 0;
		(s, ptr) := readlimbs! p;		-- read size and limbs
		assert(s >= 0);
		x:% := new();
		rec := rep x;
		if b64? then {
			(rec pretend Rec64).lmbs := ptr;
			(rec pretend Rec64).szal := shift(s, 32) \/ s;
		}
		else {
			(rec pretend Rec32).lmbs := ptr;
			(rec pretend Rec32).al := s;
			(rec pretend Rec32).sz := s;
		}
		if sgn < 0 then ____gmpz__neg(rep x, rep x);
		x;
	}

	coerce(x:%):AldorInteger == {
                import from Z, Boolean;
		zero?(sgn := sign x) => 0;
                (s, r) := divide(length x, 8);
                if ~zero? r then s := next s;
		a:AldorInteger := 0;
		st:Z := 0;
		for n in limbs x repeat {
			nn := n;
			for m in 1..wordsize repeat {
				a := a \/ shift((nn /\ 255)::AldorInteger, st);
				nn := shift(nn, -8);
				st := st + 8;
			}
		}
		sgn < 0 => -a;
		a;
	}

	nthRoot(x:%, e:%):(Boolean,%)	== {
		import from IntegerTypeTools %;
		binaryNthRoot(x,e);
	}

	(p:TextWriter) << (x:%):TextWriter == {
		import from IntegerTypeTools %;
                print(x, 10@%, p);
	}

	<< (p:TextReader):% == {
		import from IntegerTypeTools %;
		scan p;
	}
				
	(a:%) \/ (b:%):% == {
		e:% := new();
		____gmpz__ior(rep e,rep a,rep b);
		e;
	}

	(a:%) /\ (b:%):% == {
		e:% := new();
		____gmpz__and(rep e,rep a,rep b);
		e;
	}
}

#if ALDORTEST
---------------------- test sal_intgmp.as --------------------------
#include "aldor"
#include "aldortest"

macro Z == GMPInteger;

local fact():Boolean == {
	import from Z;
	n := random() mod 100;
	m := 1;
	for i in 2..n repeat m := times!(m, i);
	m = factorial n;
}

stderr << "Testing sal__intgmp..." << newline;
aldorTest("fact", fact);
stderr << newline;
#endif

