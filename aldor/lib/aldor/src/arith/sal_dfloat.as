---------------------------- sal_dfloat.as ------------------------------------
--
-- Double precision machine floats
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli (c) INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro Z == MachineInteger;

#if ALDOC
\thistype{DoubleFloat}
\History{Manuel Bronstein}{26/10/98}{created}
\Usage{import from \this}
\Descr{\this~implements the single-precision signed machine floats.}
\begin{exports}
\category{\altype{CopyableType}}\\
\category{\altype{FloatType}}\\
\category{\altype{PackableType}}\\
\alalias{\this}{**}{$\land$}: & (\%, \%) $\to$ \% & exponentiation\\
\alexp{coerce}:
& \altype{SingleFloat} $\to$ \% & conversion to double precision\\
\alexp{max}: & \% & largest double--precision machine float\\
\alexp{min}: & \% & smallest double--precision machine float\\
\alexp{single}:
& \% $\to$ \altype{SingleFloat} & conversion to single precision\\
\end{exports}
#endif

DoubleFloat: Join(CopyableType, FloatType, PackableType) with {
	^: (%, %) -> %;
#if ALDOC
\alpage{**}
\Usage{$x\land y$}
\Signatures{$\land$: & (\%,\%) $\to$ \%\\}
\Params{ {\em x,y} & \% & floats\\ }
\Retval{Returns $x$ to the power $y$.}
#endif
	coerce: % -> DFlo$Machine;
	coerce: DFlo$Machine -> %;
	coerce: SingleFloat -> %;
	single: % -> SingleFloat;
#if ALDOC
\alpage{coerce,single}
\altarget{coerce}
\altarget{single}
\Usage{x::\%\\ single~y}
\Signatures{
coerce: & \altype{SingleFloat} $\to$ \%\\
single: & \% $\to$ \altype{SingleFloat}\\
}
\Params{
\emph{x} & \altype{SingleFloat} & a single float\\
\emph{y} & \% & a double float\\
}
\Descr{coerce(x) returns x converted to a double float, while
single(y) truncates y to a single precision float.}
#endif
	max: %;
	min: %;
#if ALDOC
\alpage{max,min}
\altarget{max}
\altarget{min}
\Usage{max\\min}
\alconstant{\%}
\Retval{max and min return respectively the largest and the smallest
double--precision machine floats.}
#endif
} == add {
	import from Machine;
	-- TEMPORARY: DFlo MUST BE BOXED IN 1.1.13 OR THEY CRASH AT RUNTIME
	-- Rep == DFlo;
	-- macro pper == per;
	-- macro prep == rep;
	Rep == Record(float:DFlo);
	macro pper x == per [x];
	macro prep x == rep(x).float;
	import from Rep;

	coerce(x:%):DFlo			== prep x;
	coerce(x:DFlo):%			== pper x;
	coerce(x:SingleFloat):%			== convert(x::SFlo)@DFlo :: %;
	0:%					== pper 0;
	1:%					== pper 1;
	min:%					== pper min;
	max:%					== pper max;
	(a:%) + (b:%):%				== pper(prep a + prep b);
	(a:%) * (b:%):%				== pper(prep a * prep b);
	(a:%) = (b:%):Boolean			== (prep a = prep b)::Boolean;
	(a:%) < (b:%):Boolean			== (prep a < prep b)::Boolean;
	(a:%) ^ (b:Z):%				== a ^ (b::%);
	coerce(a:Z):%				== pper convert(a::SInt);
	float(l:Literal):%			== pper convert(l pretend Arr);
	fraction(a:%):%				== pper fraction prep a;
	local minus:Character			== { import from Z; char 45; }
	copy(a:%):%				== a;
	local five:%				== { import from Z; 5::% }
	truncate(a:%):AldorInteger	== truncate(prep a)::AldorInteger;
	single(x:%):SingleFloat		== convert(x::DFlo)@SFlo :: SingleFloat;

	-- THOSE ARE BETTER THAN THE CORRESPONDING CATEGORY DEFAULTS
	(a:%) ~= (b:%):Boolean			== (prep a ~= prep b)::Boolean;
	(a:%) <= (b:%):Boolean			== (prep a <= prep b)::Boolean;
	zero?(a:%):Boolean			== zero?(prep a)::Boolean;
	(a:%) - (b:%):%				== pper(prep a - prep b);
	-(a:%):%				== pper(- prep a);
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
		pper(prep a / prep b);
	}

	(a:%) ^ (b:%):%	== {
	--LDR
	--	import { pow: (%, %) -> % } from Foreign C;
	--	pow(a, b);
		import { pow: (DFlo, DFlo) -> DFlo } from Foreign C;
		pow(a::DFlo, b::DFlo)::%;
	}

	import {
		ArrNew: (DFlo, SInt) -> Arr;
		ArrElt: (Arr,  SInt) -> DFlo;
		ArrSet: (Arr,  SInt, DFlo) -> DFlo;
	} from Builtin;

	getPackedArray(a:Pointer, n:Z):% == ArrElt(a pretend Arr, n::SInt)::%;

	newPackedArray(n:Z):Pointer == {
		AGAT("pkdfloatnew", n);
		ArrNew(0::DFlo, n::SInt) pretend Pointer;
	}

	setPackedArray!(a:Pointer, n:Z, c:%):() == 
		ArrSet(a pretend Arr, n::SInt, c::DFlo);

	-- TEMPORARY: FIX FOR 64-BIT dissemble-bug in Machine (1.1.13)
--	local b64?:Boolean	== { import from Z; bytes = 8 }
--	local maskneg:Z		== shift(-1, 32);
--	local maskpos:Z		== 2^32 - 1;
--	local patch(x:SInt):SInt == {
--		b64? => {
--			n := x::Z;
--			bit?(n, 31) => (n \/ maskneg)::SInt;
--			(n /\ maskpos)::SInt;
--		}
--		x;
--	}
	local mydissemble(x:DFlo):(Bool, SInt, Word, Word) == {
		(s, e, m1, m2) := dissemble x;
		(s, e, m1, m2)
	}

	(p:BinaryWriter) << (x:%):BinaryWriter == {
		import from Boolean, Z;
		(s, e, m1, m2) := mydissemble prep x;
		p := p << s::Boolean << e::Z
			<< (m1 pretend SInt)::Z << (m2 pretend SInt)::Z;
	}

	<< (p:BinaryReader):% == {
		s:Boolean := << p;
		e:Z := << p;
		m1:Z := << p;
		m2:Z := << p;
		pper assemble(s::Bool, e::SInt, (m1::SInt) pretend Word,
						(m2::SInt) pretend Word);
	}

	(p:TextWriter) << (b:%):TextWriter == {
		import from Boolean, Z, AldorInteger, FloatTypeTools %;
		zero? b => p << 0@Z;
		b < 0 => p << minus << -b;
		assert(b > 0);
		(s, e, m1, m2) := mydissemble prep b;
		x := pper assemble(s, 0, m1, m2);
		assert(x >= 1); assert(x < 2.0);
		l := e::Z::%;
		-- b = x 2^l  1 <= x < 2   =   y 10^k  1 <= y < 10
		z := (x-1.5)*0.289529654602168 + 0.1760912590558
							+ l*0.301029995663981;
		k := convert(truncate prep z)@SInt :: Z;
		if z < 0 and k::% > z then k := prev k;
		y := pper assemble(s, (e::Z - k)::SInt, m1, m2);
		-- do not use exponentiation from % since it is not
		-- supported in the interactive loop (external C function)
		import from BinaryPowering(%, Z);
		if k < 0 then y := y * binaryExponentiation(five, -k);
		else if k > 0 then y := y / binaryExponentiation(five, k);
		print(y, k, 17, p);
	}
}
