------------------------------ sal_fltgmp.as ----------------------------------
--
--  GMP Floats
--
-- Copyright (c) Helene Prieto 2000
-- Copyright (c) INRIA 2000, Version 9-2-2000
-- Logiciel Salli ©INRIA 2000, dans sa version du 9/2/2000
--
-- Updated: 2023 Peter Broadbery
-------------------------------------------------------------------------------

#include "aldor"

macro Z == MachineInteger;

#if ALDOC
\thistype{GMPFloat}
\Usage{import from \this}
\History{Helene Prieto}{2/8/2000}{created}
\Descr{\this~implements arbitrary precision floats with the float
arithmetic provided by GMP.}
\begin{exports}
\category{\altype{CopyableType}}\\
\category{\altype{FloatType}}\\
\alexp{coerce}: & \altype{SingleFloat} $\to$ \% & conversion\\
                & \altype{DoubleFloat} $\to$ \% & \\
                & \altype{GMPInteger} $\to$ \% & \\
\alexp{defaultPrecision}: & () $\to$ Z & default precision\\
\alexp{limbs}: & \% $\to$ \altype{Generator} Z & iteration\\
\alexp{machine}:
& \% $\to$ \altype{DoubleFloat} & conversion to double precision\\
\alexp{new}: & () $\to$ \% & initialization (for calling GMP)\\
\alexp{precision}: & \% $\to$ Z & precision\\
\alexp{setDefaultPrecision}: & Z $\to$ Z & adjust default precision\\
\alexp{setPrecision!}: & (\%, Z) $\to$ \% & adjust precision\\
\alexp{single}:
& \% $\to$ \altype{SingleFloat} & conversion to single precision\\
\end{exports}
\begin{alwhere}
Z &==& \altype{MachineInteger}\\
\end{alwhere}
#endif

GMPFloat: Join(FloatType, CopyableType) with {
	coerce: SingleFloat -> %;
	coerce: DoubleFloat -> %;
	coerce: GMPInteger -> %;
#if ALDOC
\alpage{coerce}
\Usage{x::\%}
\Signatures{
\name: & \altype{SingleFloat} $\to$ \%\\
\name: & \altype{DoubleFloat} $\to$ \%\\
\name: & \altype{GMPInteger} $\to$ \%\\
}
\Params{
{\em x} & \altype{SingleFloat} & a number\\
        & \altype{DoubleFloat} & \\
        & \altype{GMPInteger} & \\
}
\Retval{Converts {\em x} to a GMP float.}
#endif
	defaultPrecision: () -> Z;
	setDefaultPrecision: Z -> Z;
#if ALDOR
\alpage{defaultPrecision,setDefaultPrecision}
\altarget{defaultPrecision}
\altarget{setDefaultPrecision}
\Usage{defaultPrecision()\\ setDefaultPrecision~p}
\Signatures{
defaultPrecision: & () $\to$ \altype{MachineInteger}\\
setDefaultPrecision: & \altype{MachineInteger} $\to$ \altype{MachineInteger}\\
}
\Params{ \emph{p} & \altype{MachineInteger} & precision in bits\\ }
\Descr{defaultPrecision() returns the current default precision for
newly created floats, while
setDefaultPrecision(p) sets the default precision for floats created
after this call to be at least \emph{p} bits and returns the value of the
previous default precision.}
\Remarks{setDefaultPrecision must be called once before creating any floats,
otherwise the precision is undefined.}
\alseealso{\alexp{precision},\alexp{setPrecision!}}
#endif
	limbs: % -> Generator Z;
#if ALDOC
\alpage{limbs}
\Usage{ for d in \name~x repeat \{ \dots \} }
\Signature{\%}{\altype{Generator} \altype{MachineInteger}}
\Params{ {\em x} & \% & a float\\ }
\Descr{This function allows the individual limbs of a GMP float
to be iterated independently of the machine size.
This generator yields the limbs from the least to the most significant.}
\Remarks{The limbs of $x$ describe $|x|$, so combine it with
\alfunc{OrderedArithmeticType}{sign}
if you need a complete description of $x$.}
#endif
	machine: % -> DoubleFloat;
	single: % -> SingleFloat;
#if ALDOC
\alpage{machine,single}
\altarget{machine}
\altarget{single}
\Usage{machine~x\\ single~x}
\Signatures{
machine: & \% $\to$ \altype{DoubleFloat}\\
single: & \% $\to$ \altype{SingleFloat}\\
}
\Params{ \emph{x} & \% & a float\\ }
\Descr{machine(x) and single(x) truncate x respectively to a double precision
or single precision float.}
#endif
	new: () -> %;
#if ALDOC
\alpage{new}
\Usage{\name()}
\Signature{()}{\%}
\Retval{Returns an initialized GMP float but with no value
stored into it. Results from this function can be used only
as parameters to explicit calls to {\tt mpf\_} functions.}
#endif
	precision: % -> Z;
	setPrecision!: (%,Z) -> %;
#if ALDOC
\alpage{precision,setPrecision!}
\altarget{precision}
\altarget{setPrecision!}
\Usage{precision~x\\setPrecision!(x, n)}
\Signatures{
precision: & \% $\to$ \altype{MachineInteger}\\
setPrecision!: & (\%, \altype{MachineInteger}) $\to$ \%\\
}
\Params{
{\em x} & \% & a float\\
{\em n} & \altype{MachineInteger} & a precision\\
}
\Descr{precision(x) returns the precision acutally used for $x$,
while setPrecision!(x, n) sets the precision for $x$ to be at least $n$ bits.}
\Remarks{Those functions are wrapper to the {\tt mpf\_get\_prec} and
{\tt mpf\_set\_prec} GMP functions, so setPrecision! can involve a
call to {\tt realloc}.}
\alseealso{\alexp{defaultPrecision},\alexp{setDefaultPrecision}}
#endif
} == add {
	macro Rep == Pointer;

	macro Rec32 == Record(pr:Z, sz:Z, expo:Z, lmbs:Pointer);
	macro Rec64 == Record(szpr:Z, expo:Z, lmbs:Pointer);

        import { int: Type; mpf__srcptr: Type;  mpz__ptr: Type; mpf__ptr: Type; } from Foreign C;
        import {  size__t: Type; mp__bitcnt__t: Type; Ptr: Type; LPtr: Type; int: Type } from Foreign C;
        import {  mp__ext__t: Type } from Foreign C;
	import {
		mpf__init: mpf__ptr -> ();
		mpf__clear: mpf__ptr -> ();

		mpf__set__default__prec: mp__bitcnt__t -> ();
		mpf__get__default__prec: () -> mp__bitcnt__t;
		mpf__set__prec: (mpf__ptr,mp__bitcnt__t) -> ();
		mpf__get__prec: mpf__srcptr -> mp__bitcnt__t;
		mpf__set: (mpf__ptr, mpf__srcptr) -> ();
		mpf__set__ui: (mpf__ptr, SInt$Machine) -> ();
		mpf__set__si: (mpf__ptr, SInt$Machine) -> ();
		mpf__set__z: (mpf__ptr, mpz__ptr) -> ();
		mpf__set__d: (mpf__ptr, DFlo$Machine) -> ();
		mpf__set__str: (mpf__ptr, Ptr, int) -> Z;

		mpf__get__str: (Ptr, LPtr, Z, Z, mpf__srcptr) -> Ptr;

		mpf__add: (mpf__ptr,mpf__srcptr,mpf__srcptr) -> ();
		mpf__add__ui: (mpf__ptr,mpf__srcptr,Z) -> ();
		mpf__sub: (mpf__ptr,mpf__srcptr,mpf__srcptr) -> ();
		mpf__sub__ui: (mpf__ptr,mpf__srcptr,Z) -> ();
		mpf__mul: (mpf__ptr,mpf__srcptr,mpf__srcptr) -> ();
		mpf__mul__ui: (mpf__ptr,mpf__srcptr,mpf__srcptr,Z) -> ();

		mpf__div: (mpf__ptr,mpf__srcptr,mpf__srcptr) -> ();
		mpf__ui__div: (mpf__ptr,Z,mpf__srcptr) -> ();
		mpf__div__ui: (mpf__ptr,mpf__srcptr,Z) -> ();

		mpf__sqrt: (mpf__ptr,mpf__srcptr) -> ();
		mpf__sqrt__ui: (mpf__ptr,mpf__srcptr,Z) -> ();
		mpf__pow__ui: (mpf__ptr,mpf__srcptr,Z) -> ();
		mpf__neg: (mpf__ptr,mpf__srcptr) -> ();
		mpf__abs: (mpf__ptr,mpf__srcptr) -> ();
		mpf__mul__2exp: (mpf__ptr,mpf__srcptr,Z) -> ();
		mpf__div__2exp: (mpf__ptr,mpf__srcptr,Z) -> ();
		mpf__cmp: (mpf__ptr,mpf__ptr) -> Z;
		mpf__cmp__ui: (mpf__srcptr,Z) -> Z;
		mpf__cmp__si: (mpf__srcptr,Z) -> Z;
		mpf__sgn: mpf__srcptr -> Z;

		mpf__size: mpf__srcptr -> Z;
		mpf__sizeinbase: (mpf__srcptr,Z) -> Z;

		mpf__trunc: (mpf__ptr, mpf__srcptr) -> ();
		mpz__set__f: (mpz__ptr, mpf__srcptr) -> ();
		mpf__get__d: mpf__ptr -> (DFlo$Machine);

		mpf__out__str: (Pointer,Z,Z,Rep) -> ();
		mpf__inp__str:(Rep, Pointer,Z) -> ();
	} from Foreign C("gmp.h");
	
        local gmpIn(a: GMPInteger): mpz__ptr == a pretend mpz__ptr;
        local gmpIn(a: %): mpf__srcptr == rep(a) pretend mpf__srcptr;
        local gmpIn(a: %): mpf__ptr == rep(a) pretend mpf__ptr;
        local gmpOut(a: mpf__srcptr): % == per(a pretend Rep);

        local gmpZIn(n: Z): mp__bitcnt__t == n pretend mp__bitcnt__t;
        local gmpZIn(n: Z): int == n pretend int;
        local gmpZOut(c: mp__bitcnt__t): Z == c pretend Z;
        local gmpZOut(c: size__t): Z == c pretend Z;

	import from Z,Rep;

	local wordsize:Z        == bytes;
	local b64?: Boolean	== wordsize=8;
	local plus:Character	== { import from String; char "+"; }
	local minus:Character	== { import from String; char "-"; }
	local dot:Character	== { import from String; char "."; }
	local char0:Character	== { import from String; char "0"; }
	local chare:Character	== { import from String; char "e"; }

	local nlimbs(a:%):Z == mpf__size(gmpIn(a));

        new():%                         == {
		n: Pointer := {
			b64? => [0,0,nil]$Rec64 pretend Pointer;
			[0,0,0,nil]$Rec32 pretend Pointer;
		}
                mpf__init(n pretend mpf__ptr);
		per n;
	}

	-- HACK SINCE CANNOT USE mpf__sgn BECAUSE IT IS A MACRO THAT TAKES
	-- A POINTER AND CANNOT BE USED WITH AN FiWord

	local hi32:Z == shift(4294967295, 32);  -- 32 1's and 32 0's

	sign(a:%):Z == {
		import from Rec32, Rec64;
		b64? => {
			-- low 32 = precision, hi 32 = size (signed)
			sizePrec := (rep(a) pretend Rec64).szpr;
			zero?(sizePrec /\ hi32) => 0;
			sign sizePrec;
		}
		sign((rep(a) pretend Rec32).sz);
	}

	local exponent(a:%):Z == {
		import from Rec32, Rec64;
		b64? => (rep(a) pretend Rec64).expo;
		(rep(a) pretend Rec32).expo;
	}

	free!(a:%): () == mpf__clear(gmpIn(a));

	precision(a:%): Z == gmpZOut mpf__get__prec(gmpIn(a));
	setPrecision!(a:%,b:Z): % == {
		mpf__set__prec(gmpIn(a), gmpZIn(b));
		a;
	}

	defaultPrecision():Z == gmpZOut mpf__get__default__prec();
	setDefaultPrecision(p:Z):Z == {
		old := defaultPrecision();
		mpf__set__default__prec(gmpZIn p);
		old;
	}

        local plimbs(a:%):Pointer == {
                import from Rec32, Rec64;
                b64? => (rep(a) pretend Rec64).lmbs;
                (rep(a) pretend Rec32).lmbs;
        }

        limbs(a:%):Generator Z == generate {
              macro PZ == Record(x:Z);
              import from PZ, Pointer;
              if ~nil?(p := plimbs a) then for i in 1..nlimbs a repeat {
                      yield((p pretend PZ).x);
                      p := (p::Z + wordsize)::Pointer;
              }
        }


	float(a:Literal):%	== {
		e:% := new();
		import from String;
		mpf__set__str(gmpIn e, a pretend Ptr, gmpZIn(10::Z));
		e;
	}

	truncate(a:%): AldorInteger == {
		import from GMPInteger;

		e:% := new();
		res: GMPInteger := new();
		mpf__trunc(gmpIn e, gmpIn a);
		mpz__set__f(gmpIn res, gmpIn e);
		res::AldorInteger;
	}

	fraction(a:%):% == {
		e:% := new();
		mpf__trunc(gmpIn e, gmpIn a);
		a-e;
	}

	copy(a:%): %		== {
		e:% := new();
		mpf__set(gmpIn e, gmpIn a);
		e;
	}

	coerce(a:Z):%		== {
		e:% := new();
		mpf__set__si(gmpIn e, a::(SInt$Machine));
		e;
	}

	coerce(a:GMPInteger):%	== {
		e:% := new();
		mpf__set__z(gmpIn e, gmpIn a);
		e;
	}

	coerce(a:SingleFloat):%	== {
		import from DoubleFloat;
		a::DoubleFloat::%;
	}

	single(x:%):SingleFloat == {
		import from DoubleFloat;
		single machine x;
	}

	coerce(a:DoubleFloat):%		== {
		e:% := new();
		mpf__set__d(gmpIn e, a::(DFlo$Machine));
		e;
	}


	machine(x:%):DoubleFloat == {
		import from Machine, DoubleFloat;
		mpf__get__d(gmpIn x)::DoubleFloat;
	}

	=(a:%,b:%): Boolean == mpf__cmp(gmpIn a, gmpIn b) = 0;
	~=(a:%,b:%): Boolean == mpf__cmp(gmpIn a, gmpIn b) ~= 0;

	0: % 			    == {
		h:% := new();
		mpf__set__si(gmpIn h, 0::(SInt$Machine));
		h;
	}

	1: % 			    == {
		g:% := new();
		mpf__set__si(gmpIn g,1::(SInt$Machine));
		g;
	}

	(a:%) < (b:%):Boolean	== {
		r:Z := mpf__cmp(gmpIn a, gmpIn b);
		r < 0;
	}


	(a:%) + (b:%):%		== {
		e:% := new();
		mpf__add(gmpIn e, gmpIn a, gmpIn b);
		e;
	}


	(a:%) * (b:%):%		== {
		e:% := new();
		mpf__mul(gmpIn e, gmpIn a, gmpIn b);
		e;
	}

	(a:%) - (b:%):%		== {
		e:% := new();
		mpf__sub(gmpIn e, gmpIn a, gmpIn b);
		e;
	}

	-(a:%):%		== {
		b:% :=  new();
		mpf__sub(gmpIn b, gmpIn 0, gmpIn a);
		b;
	}

	add!(a:%,b:%):%		== {
		mpf__add(gmpIn a, gmpIn a, gmpIn b);
		a;
	}

	times!(a:%,b:%):%	== {
		mpf__mul(gmpIn a, gmpIn a, gmpIn b);
		a;
	}

        next(a:%):%                             == {
		e:%:=new();
		mpf__add__ui(gmpIn e, gmpIn a, 1::Z);
		e;
	}

        prev(a:%):%                             == {
		e:% := new();
		mpf__sub__ui(gmpIn e, gmpIn a, 1::Z);
		e;
	}

	(a:%) > (b:%):Boolean	== ~(a<=b);
	(a:%) <= (b:%):Boolean	== mpf__cmp(gmpIn a, gmpIn b) <= 0;
	(a:%) >= (b:%):Boolean	== mpf__cmp(gmpIn a, gmpIn b) >= 0;


	max(a:%,b:%):%		== {a < b => b; a};
	min(a:%,b:%):%		== {a < b => a; b};
	minus!(a:%):%		== -a;
	minus!(a:%,b:%):%	== a-b;
	zero?(a:%):Boolean	== mpf__cmp(gmpIn a, gmpIn 0) = 0;
	one?(a:%):Boolean	== mpf__cmp(gmpIn a, gmpIn 1) = 0;

	abs(a:%):%		== { 
		b:% :=  new();
		mpf__abs(gmpIn b, gmpIn a);
		b;	
	}

	
	(a:%) ^ (b:Z): %	== {
		e:% := new();
		mpf__pow__ui(gmpIn e, gmpIn a, b);
		e;	
	}

	(a:%) / (b:%): %	== {
		e:% := new();
		mpf__div(gmpIn e, gmpIn a, gmpIn b);
		e;
	}

        (p:BinaryWriter) << (x:%):BinaryWriter == {
		import from Z, GMPTools, Rec32, Rec64;
		writelimbs!(p << exponent x, sign x, nlimbs x, limbs x);
        }

        << (p:BinaryReader):% == {
		import from GMPTools, Rec32, Rec64;
		xpo:Z := << p;			-- read exponent first
		sgn:Z := << p;			-- read sign
		zero? sgn => 0;
		(s, ptr) := readlimbs! p;	-- read size and limbs
		assert(s >= 0);
		x:% := new();
		rec := rep x;
		if b64? then {
			(rec pretend Rec64).lmbs := ptr;
			(rec pretend Rec64).expo := xpo;
			(rec pretend Rec64).szpr := shift(s, 32) \/ s;
		}
		else {
			(rec pretend Rec32).lmbs := ptr;
			(rec pretend Rec32).expo := xpo;
			(rec pretend Rec32).pr := s;
			(rec pretend Rec32).sz := s;
		}
		if sgn < 0 then mpf__neg(gmpIn x, gmpIn x);
		x;
	}

	(p:TextWriter) << (x:%):TextWriter == {
		macro PZ == Record(z:Z);
		import from Z, Character, String, Pointer;
		pexp:PZ := [0];
		ptr := mpf__get__str((nil$Pointer) pretend Ptr, pexp pretend LPtr, 10, 0, gmpIn x);
		s := ptr pretend String;
		n:Z := 0;
		if s.0 = minus then {
			p := p << minus;
			n := next n;
		}
		p := p << "0.";
		for i in n..#s repeat p := p << s.i;
		p << "e" << pexp.z;
	}

#if 0
	local scanfloat(p:TextReader):List Character == {
		import from Character, String;
		local c:Character;
		while space?(c := << p) or c = newline repeat {};
		(c = plus) or (c = minus) or digit? c => cons(c, scan0 p);
		c ~= dot => throw SyntaxException;
		cons(char0, cons(c, scan1 p));
	}

	-- dot/e is allowed, +/-/spaces not allowed
	local scan0(p:TextReader):List Character == {
		c:Character := << p;
		digit? c => cons(c, scan0 p);
		c = dot => cons(c, scan1 p);
		c ~= chare => throw SyntaxException;
		cons(c, scan2 p);
	}

	-- e is allowed, +/-/dot/spaces not allowed
	local scan1(p:TextReader):List Character == {
		c:Character := << p;
		digit? c => cons(c, scan1 p);
		c ~= chare => throw SyntaxException;
		cons(c, scan2 p);
	}

	-- e has just been read, +/- is allowed, dot/e/spaces not allowed
	local scan2(p:TextReader):List Character == {
		c:Character := << p;
		c = minus or digit? c => cons(c, scan3 p);
		c ~= plus => throw SyntaxException;
		cons(c, scan3 p);
	}

	-- only digits are allowed
	local scan3(p:TextReader):List Character == {
		import from Boolean;
		c:Character := << p;
		~digit? c => throw SyntaxException;
		cons(c, scan3 p);
	}

	<< (p:TextReader):% == {
		import from String, List Character;
		l := scanfloat p;
		s:String := new(n := #l);
		i:Z := 0;
		for c in l repeat {
			s.i := c;
			i := next i;
		}
		e := new();
		mpf__set__str(rep e, pointer s, 10);
		e;
	}
#endif
}


#if ALDORTEST
---------------------- test sal_intgmp.as --------------------------
#include "aldor"
#include "aldortest"
#pile

import from Assert GMPFloat
import from Assert Integer
import from Assert String
import from Assert Boolean
import from GMPFloat
import from Integer
Z ==> GMPInteger

local test():Boolean ==
	assertEquals(12345, truncate 12345.0)
	true

test1(): Boolean ==
    f: GMPFloat := 1.0
    assertTrue(-f < f)
    assertTrue(f < f+f)
    true

test()
test1()

#endif
