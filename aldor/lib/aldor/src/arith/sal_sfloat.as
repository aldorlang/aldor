---------------------------- sal_sfloat.as ------------------------------------
--
-- Single precision machine floats
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli (c) INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro Z == MachineInteger;

#if ALDOC
\thistype{SingleFloat}
\History{Manuel Bronstein}{26/10/98}{created}
\Usage{import from \this}
\Descr{\this~implements the single-precision signed machine floats.}
\begin{exports}
\category{\altype{CopyableType}}\\
\category{\altype{FloatType}}\\
\category{\altype{PackableType}}\\
\alalias{\this}{**}{$\land$}: & (\%, \%) $\to$ \% & exponentiation\\
\alexp{max}: & \% & largest single--precision machine float\\
\alexp{min}: & \% & smallest single--precision machine float\\
\end{exports}
#endif

SingleFloat: Join(CopyableType, FloatType, PackableType) with {
	^: (%, %) -> %;
#if ALDOC
\alpage{**}
\Usage{$x\land y$}
\Signatures{$\land$: & (\%,\%) $\to$ \%\\}
\Params{ {\em x,y} & \% & floats\\ }
\Retval{Returns $x$ to the power $y$.}
#endif
	coerce: % -> SFlo$Machine;
	coerce: SFlo$Machine -> %;
	max: %;
	min: %;
#if ALDOC
\alpage{max,min}
\altarget{max}
\altarget{min}
\Usage{max\\min}
\alconstant{\%}
\Retval{max and min return respectively the largest and the smallest
single--precision machine floats.}
#endif
} == add {
	import from Machine;
	Rep == SFlo;

	coerce(x:%):SFlo			== rep x;
	coerce(x:SFlo):%			== per x;
	0:%					== per 0;
	1:%					== per 1;
	min:%					== per min;
	max:%					== per max;
	(a:%) + (b:%):%				== per(rep a + rep b);
	(a:%) * (b:%):%				== per(rep a * rep b);
	(a:%) = (b:%):Boolean			== (rep a = rep b)::Boolean;
	(a:%) < (b:%):Boolean			== (rep a < rep b)::Boolean;
	(a:%) ^ (b:Z):%				== a ^ (b::%);
	coerce(a:Z):%				== per convert(a::SInt);
	float(l:Literal):%			== per convert(l pretend Arr);
	fraction(a:%):%				== per fraction rep a;
	local minus:Character			== { import from Z; char 45; }
	copy(a:%):%				== a;
	local five:%				== { import from Z; 5::% }
	truncate(a:%):AldorInteger	== truncate(rep a)::AldorInteger;

	-- THOSE ARE BETTER THAN THE CORRESPONDING CATEGORY DEFAULTS
	(a:%) ~= (b:%):Boolean			== (rep a ~= rep b)::Boolean;
	(a:%) <= (b:%):Boolean			== (rep a <= rep b)::Boolean;
	zero?(a:%):Boolean			== zero?(rep a)::Boolean;
	(a:%) - (b:%):%				== per(rep a - rep b);
	-(a:%):%				== per(- rep a);
	add!(a:%, b:%):%			== { zero? a => copy b; a + b; }
	times!(a:%, b:%):%			== { one? a => copy b; a * b; }

	-- TEMPORARY (BUG1182) DEFAULTS DON'T INLINE WELL
	(a:%) > (b:%):Boolean == ~(a <= b);
	(a:%) >= (b:%):Boolean == ~(a < b);
	max(a:%, b:%):% == { a < b => b; a };
	min(a:%, b:%):% == { a < b => a; b };
	minus!(a:%):% == - a;
	minus!(a:%, b:%):% == a - b;
	one?(a:%):Boolean == a = 1;
	abs(a:%):% == { a < 0 => -a; a }

	(a:%) / (b:%):% == {
		import from Boolean;
		assert(~zero? b);
		per(rep a / rep b);
	}

	(a:%) ^ (b:%):%	== {
		import { powf: (%, %) -> % } from Foreign C;
		powf(a, b);
	}

	import {
		ArrNew: (SFlo, SInt) -> Arr;
		ArrElt: (Arr,  SInt) -> SFlo;
		ArrSet: (Arr,  SInt, SFlo) -> SFlo;
	} from Builtin;

	getPackedArray(a:Pointer, n:Z):% == ArrElt(a pretend Arr, n::SInt)::%;

	newPackedArray(n:Z):Pointer == {
		AGAT("pksfloatnew", n);
		ArrNew(0::SFlo, n::SInt) pretend Pointer;
	}

	setPackedArray!(a:Pointer, n:Z, c:%):() == 
		ArrSet(a pretend Arr, n::SInt, c::SFlo);

	-- TEMPORARY: FIX FOR 64-BIT dissemble-bug in Machine (BUG1236)
	local b64?:Boolean	== { import from Z; bytes = 8 }
	local maskneg:Z		== shift(-1, 32);
	local maskpos:Z		== 2^32 - 1;
	local patch(x:SInt):SInt == {
		b64? => {
			n := x::Z;
			bit?(n, 31) => (n \/ maskneg)::SInt;
			(n /\ maskpos)::SInt;
		}
		x;
	}
	local mydissemble(x:SFlo):(Bool, SInt, Word) == {
		(s, e, m) := dissemble x;
		(patch(s pretend SInt) pretend Bool, patch e, m);
	}

	(p:BinaryWriter) << (x:%):BinaryWriter == {
		import from Boolean, Z;
		(s, e, m) := mydissemble rep x;
		p << s::Boolean << e::Z << (m pretend SInt)::Z;
	}

	<< (p:BinaryReader):% == {
		s:Boolean := << p;
		e:Z := << p;
		m:Z := << p;
		per assemble(s::Bool, e::SInt, (m::SInt) pretend Word);
	}

	(p:TextWriter) << (b:%):TextWriter == {
		import from Boolean, Z, AldorInteger, FloatTypeTools %;
		zero? b => p << 0@Z;
		b < 0 => p << minus << -b;
		assert(b > 0);
		(s, e, m) := mydissemble rep b;
		x := per assemble(s, 0, m);
		assert(x >= 1); assert(x < 2.0);
		l := e::Z::%;
		-- b = x 2^l  1 <= x < 2   =   y 10^k  1 <= y < 10
		z := (x-1.5)*0.289529654602168 + 0.1760912590558
							+ l*0.301029995663981;
		k := convert(truncate rep z)@SInt :: Z;
		if z < 0 and k::% > z then k := prev k;
		y := per assemble(s, (e::Z - k)::SInt, m);
		-- do not use exponentiation from % since it is not
		-- supported in the interactive loop (external C function)
		import from BinaryPowering(%, Z);
		if k < 0 then y := y * binaryExponentiation(five, -k);
		else if k > 0 then y := y / binaryExponentiation(five, k);
		print(y, k, 8, p);
	}
}
