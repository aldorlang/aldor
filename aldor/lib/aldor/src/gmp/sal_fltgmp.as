------------------------------ sal_fltgmp.as ----------------------------------
--
--  GMP Floats
--
-- Copyright (c) Helene Prieto 2000
-- Copyright (c) INRIA 2000, Version 9-2-2000
-- Logiciel Salli ©INRIA 2000, dans sa version du 9/2/2000
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

	-- Cannot use the mpf_... names because they are macros in gmp.h
	import {
		____gmpf__init: Rep -> ();
		____gmpf__clear:Rep -> ();

		____gmpf__set__default__prec: Z -> ();
		____gmpf__get__default__prec: () -> Z;
		____gmpf__set__prec: (Rep,Z) -> ();
		____gmpf__get__prec: (Rep) -> Z;
		____gmpf__set: (Rep, Rep) -> ();
		____gmpf__set__ui: (Rep, Z) -> ();
		____gmpf__set__si: (Rep, Z) -> ();
		____gmpf__set__z: (Rep,GMPInteger) -> ();
		____gmpf__set__d: (Rep,DoubleFloat) -> ();
		____gmpf__set__str: (Rep, Pointer, Z) -> Z;

		____gmpf__get__str: (Pointer, Pointer, Z, Z, Rep) -> Pointer;

		____gmpf__add: (Rep,Rep,Rep) -> ();
		____gmpf__add__ui: (Rep,Rep,Z) -> ();
		____gmpf__sub: (Rep,Rep,Rep) -> ();
		____gmpf__sub__ui: (Rep,Rep,Z) -> ();
		____gmpf__mul: (Rep,Rep,Rep) -> ();
		____gmpf__mul__ui: (Rep,Rep,Z) -> ();

		____gmpf__div: (Rep,Rep,Rep) -> ();
		____gmpf__ui__div: (Rep,Z,Rep) -> ();
		____gmpf__div__ui: (Rep,Rep,Z) -> ();

		____gmpf__sqrt: (Rep,Rep) -> ();
		____gmpf__sqrt__ui: (Rep,Rep,Rep) -> ();
		____gmpf__pow__ui: (Rep,Rep,Z) -> ();
		____gmpf__neg: (Rep,Rep) -> ();
		____gmpf__abs: (Rep,Rep) -> ();
		____gmpf__mul__2exp: (Rep,Rep,Z) -> ();
		____gmpf__div__2exp: (Rep,Rep,Z) -> ();
		____gmpf__cmp: (Rep,Rep) -> Z;
		____gmpf__cmp__ui: (Rep,Z) -> Z;
		____gmpf__cmp__si: (Rep,Z) -> Z;
		____gmpf__sgn: Rep -> Z;

		____gmpf__size: Rep -> Z;
		____gmpf__sizeinbase: (Rep,Z) -> Z;

		____gmpf__trunc: (Rep,Rep) -> ();

		____gmpf__out__str: (Pointer,Z,Z,Rep) -> ();
		____gmpf__inp__str:(Rep, Pointer,Z) -> ();
	} from Foreign C;
	
	import from Z,Rep;

	local wordsize:Z        == bytes;
	local b64?: Boolean	== wordsize=8;
	local plus:Character	== { import from String; char "+"; }
	local minus:Character	== { import from String; char "-"; }
	local dot:Character	== { import from String; char "."; }
	local char0:Character	== { import from String; char "0"; }
	local chare:Character	== { import from String; char "e"; }

	local nlimbs(a:%):Z == ____gmpf__size(rep a);

        new():%                         == {
		n: Pointer := {
			b64? => [0,0,nil]$Rec64 pretend Pointer;
			[0,0,0,nil]$Rec32 pretend Pointer;
		}
                ____gmpf__init(n);
		per n;
	}

	-- HACK SINCE CANNOT USE ____gmpf__sgn BECAUSE IT IS A MACRO THAT TAKES
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

	free!(a:%): () == ____gmpf__clear(rep a);

	precision(a:%): Z == ____gmpf__get__prec(rep a);
	setPrecision!(a:%,b:Z): % == {
		____gmpf__set__prec(rep a,b);
		a;
	}

	defaultPrecision():Z == ____gmpf__get__default__prec();
	setDefaultPrecision(p:Z):Z == {
		old := defaultPrecision();
		____gmpf__set__default__prec(p);
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
		____gmpf__set__str(rep e, pointer(a pretend String), 10::Z);
		e;
	}

	truncate(a:%): AldorInteger	== {
		import from GMPInteger;
		import {
			____gmpz__set__f:(GMPInteger,Pointer) -> ();
		} from Foreign C;

		e:% := new();
		res: GMPInteger := new();
		____gmpf__trunc(rep e, rep a);
		____gmpz__set__f(res,rep e);
		res::AldorInteger;
	}

	fraction(a:%):% == {
		e:% := new();
		____gmpf__trunc(rep e, rep a);
		a-e;
	}

	copy(a:%): %		== {
		e:% := new();
		____gmpf__set(rep e,rep a);
		e;
	}

	coerce(a:Z):%		== {
		e:% := new();
		____gmpf__set__si(rep e, a);
		e;
	}

	coerce(a:GMPInteger):%	== {
		e:% := new();
		____gmpf__set__z(rep e, a);
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
		____gmpf__set__d(rep e, a);
		e;
	}

	machine(x:%):DoubleFloat == {
		import from Machine, DoubleFloat;
		import {
			____gmpf__get__d: Rep -> DFlo;
		} from Foreign C;
		____gmpf__get__d(rep x)::DoubleFloat;
	}

	=(a:%,b:%): Boolean == ____gmpf__cmp(rep a,rep b) = 0;
	~=(a:%,b:%): Boolean == ____gmpf__cmp(rep a,rep b) ~= 0;

	0: % 			    == {
		h:% := new();
		____gmpf__set__si(rep h,0);
		h;
	}

	1: % 			    == {
		g:% := new();
		____gmpf__set__si(rep g,1);
		g;
	}

	(a:%) < (b:%):Boolean	== {
		r:Z := ____gmpf__cmp(rep a,rep b);
		r < 0;
	}


	(a:%) + (b:%):%		== {
		e:% := new();
		____gmpf__add(rep e,rep a,rep b);
		e;
	}


	(a:%) * (b:%):%		== {
		e:% := new();
		____gmpf__mul(rep e,rep a,rep b);
		e;
	}

	(a:%) - (b:%):%		== {
		e:% := new();
		____gmpf__sub(rep e,rep a,rep b);
		e;
	}

	-(a:%):%		== {
		b:% :=  new();
		____gmpf__sub(rep b, rep 0, rep a);
		b;
	}

	add!(a:%,b:%):%		== {
		____gmpf__add(rep a,rep a,rep b);
		a;
	}

	times!(a:%,b:%):%	== {
		____gmpf__mul(rep a,rep a,rep b);
		a;
	}

        next(a:%):%                             == {
		e:%:=new();
		____gmpf__add__ui(rep e, rep a, 1::Z);
		e;
	}

        prev(a:%):%                             == {
		e:% := new();
		____gmpf__sub__ui(rep e, rep a, 1::Z);
		e;
	}

	(a:%) > (b:%):Boolean	== ~(a<=b);
	(a:%) <= (b:%):Boolean	== ____gmpf__cmp(rep a,rep b) <= 0;
	(a:%) >= (b:%):Boolean	== ____gmpf__cmp(rep a,rep b) >= 0;


	max(a:%,b:%):%		== {a < b => b; a};
	min(a:%,b:%):%		== {a < b => a; b};
	minus!(a:%):%		== -a;
	minus!(a:%,b:%):%	== a-b;
	zero?(a:%):Boolean	== ____gmpf__cmp(rep a, rep 0) = 0;
	one?(a:%):Boolean	== ____gmpf__cmp(rep a, rep 1) = 0;

	abs(a:%):%		== { 
		b:% :=  new();
		____gmpf__abs(rep b,rep a);
		b;	
	}

	
	(a:%) ^ (b:Z): %	== {
		e:% := new();
		____gmpf__pow__ui(rep e,rep a, b);
		e;	
	}

	(a:%) / (b:%): %	== {
		e:% := new();
		____gmpf__div(rep e, rep a, rep b);
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
		if sgn < 0 then ____gmpf__neg(rep x, rep x);
		x;
	}

	(p:TextWriter) << (x:%):TextWriter == {
		macro PZ == Record(z:Z);
		import from Z, Character, String, Pointer;
		pexp:PZ := [0];
		ptr := ____gmpf__get__str(nil, pexp pretend Pointer,10,0,rep x);
		s := string ptr;
		n:Z := 0;
		if s.0 = minus then {
			p := p << minus;
			n := next n;
		}
		p := p << "0.";
		for i in n..#s repeat p := p << s.i;
		p << "e" << pexp.z;
	}

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
		____gmpf__set__str(rep e, pointer s, 10);
		e;
	}

}
