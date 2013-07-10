---------------------------- sal_int.as ------------------------------------
--
-- Aldor software integers
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro {
	Z == MachineInteger;
	B == BInt;
}

#if ALDOC
\thistype{AldorInteger}
\altarget{Integer}
\History{Manuel Bronstein}{7/10/98}{created}
\Usage{import from \this}
\Descr{\this~provides an interface to the software (``infinite'' precision)
integers provided by the \aldor virtual machine.}
\begin{exports}
\category{\astype{IntegerType}}\\
\end{exports}
#endif

extend AldorInteger: IntegerType with { export from IntegerSegment % } == add {
	import from Machine;

	0:%					== 0@B :: %;
	1:%					== 1@B :: %;
	(a:%) + (b:%):%				== (a::B + b::B)::%;
	(a:%) * (b:%):%				== (a::B * b::B)::%;
	(a:%) < (b:%):Boolean			== (a::B < b::B)::Boolean;
	(a:%) quo (b:%):%			== (a::B quo b::B)::%;
	(a:%) rem (b:%):%			== (a::B rem b::B)::%;
	coerce(a:Z):%				== convert(a::SInt)@B :: %;
	machine(a:%):Z				== convert(a::B)@SInt :: Z;
	integer(l: Literal):%			== convert(l pretend Arr)@B ::%;
	length(a:%):Z				== length(a::B)::Z;
	even?(a:%):Boolean			== even?(a::B)::Boolean;
	gcd(a:%, b:%):%				== gcd(a::B, b::B)::%;
	bit?(a:%, b:Z):Boolean			== bit(a::B, b::SInt)::Boolean;

	-- THOSE ARE BETTER THAN THE CORRESPONDING CATEGORY DEFAULTS
	(a:%) <= (b:%):Boolean			== (a::B <= b::B)::Boolean;
	zero?(a:%):Boolean			== zero?(a::B)::Boolean;
	(a:%) - (b:%):%				== (a::B - b::B)::%;
	-(a:%):%				== (-(a::B))::%;
	add!(a:%, b:%):%			== a + b;	-- not copyable
	times!(a:%, b:%):%			== a * b;	-- not copyable
	next(a:%):%				== next(a::B)::%;
	prev(a:%):%				== prev(a::B)::%;
	odd?(a:%):Boolean			== odd?(a::B)::Boolean;

	-- TEMPORARY (BUG1182) DEFAULTS DON'T INLINE WELL
	(a:%) > (b:%):Boolean == ~(a <= b);
	(a:%) >= (b:%):Boolean == ~(a < b);
	max(a:%, b:%):% == { a < b => b; a };
	min(a:%, b:%):% == { a < b => a; b };
	minus!(a:%):% == - a;
	minus!(a:%, b:%):% == a - b;
	one?(a:%):Boolean == a = 1;
	abs(a:%):% == { a < 0 => -a; a }
	set(a:%, n:Z):% == a \/ shift(1, n);
	clear(a:%, n:Z):% == a /\ ~(shift(1, n));
	hash(a:%):Z == machine a;
	(a:%) mod (b:%):% == {
		assert(b ~= 0);
		(r := a rem b) < 0 => r + abs b;
		r;
	}

	-- since a + ~a is an endless sequence of 1's, ~a = -1 - a
	~(a:%):% == -1-a;

	sign(a:%):Z == {
		zero? a => 0;
		a > 0 => 1;
		-1;
	}

	(a:%) ^ (b:Z):%	== {
		import from BinaryPowering(%, Z);
		binaryExponentiation(a, b);
	}

	divide(a:%, b:%):(%, %)	== {
		(q, r) := divide(a::B, b::B);
		(q::%, r::%);
	}

	random():% == {
		import from RandomNumberGenerator;
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

	shift(a:%, b:Z):% == {
		b < 0 => shiftDown(a::B, (-b)::SInt)::%;
		shiftUp(a::B, b::SInt)::%;
	}

	nthRoot(x:%, e:%):(Boolean, %) == {
		import from IntegerTypeTools %;
		binaryNthRoot(x, e);
	}

	(p:TextWriter) << (x:%):TextWriter == {
		import from IntegerTypeTools %;
		print(x, 10@%, p);
	}

	<< (p:TextReader):% == {
		import from IntegerTypeTools %;
		scan p;
	}

	xor(a:%, b:%):% == {
		import from Boolean, Z;
		if length a < length b then (a, b) := (b, a);
		c:% := 0;
		for n in prev(length a)..0 by -1 repeat {
			c := shift(c, 1);
			a? := bit?(a, n);
			b? := bit?(b, n);
			if (a? and ~b?) or (b? and ~a?) then c := next c;
		}
		c;
	}

	(a:%) \/ (b:%):% == {
		import from Boolean, Z;
		if length a < length b then (a, b) := (b, a);
		c:% := 0;
		for n in prev(length a)..0 by -1 repeat {
			c := shift(c, 1);
			if bit?(a, n) or bit?(b, n) then c := next c;
		}
		c;
	}

	(a:%) /\ (b:%):% == {
		import from Boolean, Z;
		if length a > length b then (a, b) := (b, a);
		c:% := 0;
		for n in prev(length a)..0 by -1 repeat {
			c := shift(c, 1);
			if bit?(a, n) and bit?(b, n) then c := next c;
		}
		c;
	}

	-- integers are written in binary low byte first
	-- the length is in bytes
	(p:BinaryWriter) << (x:%):BinaryWriter == {
		import from Boolean, Z, Byte;
		p := p << (sgn := sign x);		-- write sign first
		zero? sgn => p;
		(s, r) := divide(length x, 8);
		if ~zero? r then s := next s;
		p := p << s;				-- write size
		for m in 1..s repeat {			-- must send s bytes
			-- TEMPORARY: BECAUSE OF BUG1191,
			-- x /\ 255 REALLY MEANS |x| /\ 255
			p := p << lowByte machine(x /\ 255);
			-- QUADRATIC SPACE, NO ACCESS TO THE LIMBS
			x := shift(x, -8);
		}
		p;
	}

	-- integers are written in binary low byte first
	-- the length is in bytes
	<< (p:BinaryReader):% == {
		import from Z, Byte;
		sgn:Z := << p;				-- scan sign first
		zero? sgn => 0;
		s:Z := << p;				-- read size
		x:% := 0;
		st:Z := 0;
		b := lowByte st;
		for m in 1..s repeat {			-- must read s bytes
			-- QUADRATIC SPACE, NO ACCESS TO THE LIMBS
			b := << p;
			x := x \/ shift(b::Z::%, st);
			st := st + 8;
		}
		sgn < 0 => -x;
		x;
	}
}
