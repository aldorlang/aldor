---------------------------- sal_mint.as ------------------------------------
--
-- Machine integers
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro Z == MachineInteger;

#if ALDOC
\thistype{MachineInteger}
\History{Manuel Bronstein}{28/9/98}{created}
\Usage{import from \this}
\Descr{\this~implements the full-word signed machine integers.}
\begin{exports}
\category{\altype{CopyableType}}\\
\category{\altype{IntegerType}}\\
\alexp{bytes}: & \% & machine word-size\\
\alexp{max}: & \% & largest machine integer\\
\alexp{min}: & \% & smallest machine integer\\
\alalias{\this}{modX}{mod\_+}: & (\%, \%, \%) $\to$ \% & modular addition\\
\alalias{\this}{modX}{mod\_-}: & (\%, \%, \%) $\to$ \% & modular substraction\\
\alalias{\this}{modX}{mod\_*}:& (\%, \%, \%) $\to$ \% & modular multiplication\\
\alalias{\this}{modX}{mod\_/}: & (\%, \%, \%) $\to$ \% & modular division\\
\alalias{\this}{modX}{mod\_$\land$}:
& (\%, \%, \%) $\to$ \% & modular exponentiation\\
\alexp{modInverse}: & (\%, \%) $\to$ \% & modular inverse\\
\end{exports}

\alpage{bytes,max,min}
\altarget{bytes}
\altarget{max}
\altarget{min}
\Usage{bytes\\max\\min}
\alconstant{\%}
\Retval{bytes, max and min return respectively the
size in bytes of a machine integer, the largest and the smallest
machine integers.}
#endif

extend MachineInteger: Join(CopyableType, IntegerType) with {
	mod_+: (%, %, %) -> %;
	mod_-: (%, %, %) -> %;
	mod_*: (%, %, %) -> %;
	mod_/: (%, %, %) -> %;
	mod_^: (%, %, %) -> %;
	modInverse: (%, %) -> %;
#if ALDOC
\alpage{modX,modInverse}
\altarget{modX}
\altarget{modInverse}
\Usage{mod\_X(a, b, n)\\ modInverse(a, n)}
\Params{{\em a, b, n} & \% & machine integers\\ }
\Signatures{
mod\_X: & (\%, \%, \%) $\to$ \%\\
modInverse: & (\%, \%) $\to$ \%\\
}
\Retval{mod\_X(a, b, n) returns $(a X b) \pmod n$ where $X$ is one
of $+,-,\ast,/,\land$, while modInverse(a, b) returns the inverse
of $a$ modulo $n$.}
\Remarks{Those operations require that $0 \le a, b < n$.}
#endif
	hashCombine: (%, %) -> %;

	export from IntegerSegment %;
} == add {

	import from Machine;
	Rep == SInt;

	local lhalfword():%		== shift(prev shift(1, 4 * bytes), -1);
	(a:%) + (b:%):%			== per(rep a + rep b);
	(a:%) * (b:%):%			== per(rep a * rep b);
	(a:%) /\ (b:%):%		== per(rep a /\ rep b);
	(a:%) \/ (b:%):%		== per(rep a \/ rep b);
	(a:%) < (b:%):Boolean		== (rep a < rep b)::Boolean;
	~(a:%):%			== per(~ rep a);
	length(a:%):Z			== per length rep a;
	gcd(a:%, b:%):%			== per gcd(rep a, rep b);
	bit?(a:%, b:%):Boolean		== bit(rep a, rep b)::Boolean;
	(a:%) quo (b:%):%		== per(rep a quo rep b);
	(a:%) rem (b:%):%		== per(rep a rem rep b);
	coerce(a:Z):%			== a;
	machine(a:%):Z			== a;
	copy(a:%):%			== a;
	random(n:Z):%			== random();

	-- THOSE ARE BETTER THAN THE CORRESPONDING CATEGORY DEFAULTS
        xor(a:%, b:%):%				== per xor(rep a, rep b);
	(a:%) <= (b:%):Boolean			== (rep a <= rep b)::Boolean;
	zero?(a:%):Boolean			== zero?(rep a)::Boolean;
	-(a:%):%				== per(- rep a);
	add!(a:%, b:%):%			== a + b;
	times!(a:%, b:%):%			== a * b;
	next(a:%):%				== per next rep a;
	prev(a:%):%				== per prev rep a;
	compare(a:%, b:%):Z			== a - b;

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
	odd?(a:%):Boolean == ~even? a;
	even?(a:%):Boolean == zero?(a /\ 1);
	(a:%) mod (b:%):% == {
		assert(b ~= 0);
		(r := a rem b) < 0 => r + abs b;
		r;
	}

	sign(a:%):% == {
		zero? a => 0;
		a > 0 => 1;
		-1;
	}

	(a:%) ^ (b:%):% == {
		assert(b >= 0);
		zero? a or one? a => a;
		u:% := 1;
		while b > 0 repeat {
			if odd? b then u := u * a;
			if (b := shift(b, -1)) > 0 then a := a * a;
		}
		u;
	}
	
	-- random() will be provided in sal_random.as by an extension
	-- this definition ensures that we can check whether the extended
	-- definition is really used.
	random():% == never;

	divide(a:%, b:%):(%, %)	== {
		(q, r) := divide(rep a, rep b);
		(per q, per r);
	}

	shift(a:%, b:Z):% == {
		b < 0 => per shiftDown(rep a, - rep b);
		per shiftUp(rep a, rep b);
	}

	nthRoot(x:%, e:%):(Boolean, %) == {
		import from IntegerTypeTools %;
		binaryNthRoot(x, e);
	}

	(p:TextWriter) << (x:%):TextWriter == {
		import from IntegerTypeTools %;
		print(x, 10, p);
	}

	<< (p:TextReader):% == {
		import from IntegerTypeTools %;
		scan p;
	}

	-- integers are written in binary low byte first
	-- the length is systematically 8 bytes, regardless of the platform
	(p:BinaryWriter) << (x:%):BinaryWriter == {
		import from Byte, IntegerSegment %;
		for m in 1..8 repeat {
			p := p << lowByte x;
			x := shift(x, -8);
		}
		p;
	}

	-- integers are read in binary low byte first
	-- the length is systematically 8 bytes, regardless of the platform
	<< (p:BinaryReader):% == {
		import from Byte, IntegerSegment %;
		n:% := 0;
		s:% := 0;
		b := lowByte n;
		for m in 0..7 repeat {
			b := << p;
			if m < bytes then {	-- shift is undefined otherwise
				n := n \/ shift(b::%, s);
				s := s + 8;
			}
		}
		n;
	}

	-- the following are taken from Pete Broadbery (axllib)
	mod_+(a:%, b:%, n:%):% == {
		assert(0 <= a); assert(a < n);
		assert(0 <= b); assert(b < n);
		-- Trick to avoid overflow
		a := a - n;
		(c := a + b) < 0 => c + n;
		c;
	}

	mod_-(a:%, b:%, n:%):% == {
		assert(0 <= a); assert(a < n);
		assert(0 <= b); assert(b < n);
		(c := a - b) < 0 => c + n;
		c;
	}

	mod_*(a:%, b:%, n:%):% == {
		assert(0 <= a); assert(a < n);
		assert(0 <= b); assert(b < n);
		a = 1 => b;
		b = 1 => a;
		n < lhalfword() or
			(a < lhalfword() and b < lhalfword()) => (a * b) mod n;
		(nh, nl) := double_*(a pretend Word, b pretend Word);
		(qh, ql, rm) := doubleDivide(nh, nl, n pretend Word);
		rm pretend %;
	}

	-- returns junk if b is not invertible modulo n
	mod_/(a:%, b:%, n:%):% == {
		assert(0 <= a); assert(a < n);
		assert(0 < b); assert(b < n);
		mod_*(a, modInverse(b, n), n);
	}

	-- returns junk if b is not invertible modulo n
	modInverse(b:%, n:%):% == {
		import from Boolean;
		assert(0 < b); assert(b < n);
		(c0:%, d0:%) := (b, n);
		(c1:%, d1:%) := (1, 0);
		while ~zero?(d0) repeat {
			(q, r) := divide(c0, d0);
			(c0, d0) := (d0, r);
			(c1, d1) := (d1, c1 - q * d1);
		}
		assert(c0 = 1);		-- or b is not invertible in Z/nZ
		c1 < 0  => c1 + n;
		c1;
	}

	mod_^(a:%, b:%, n:%):% == {
		assert(0 <= a); assert(a < n);
		b < 0 => mod_^(mod_/(1, a, n), -b, n);
		n < lhalfword() => lhmod_^(a, b, n);
		u:% := 1;
		while b > 0 repeat {
			if odd? b then u := mod_*(u, a, n);
			a := mod_*(a, a, n);
			b := shift(b, -1);
		}
		u;
	}

	-- this one guarantees that products don't overflow and remain >= 0
	local lhmod_^(a:%, b:%, n:%):% == {
		assert(0 <= a); assert(a < n); assert(n < lhalfword());
		assert(0 <= b);
		u:% := 1;
		while b > 0 repeat {
			if odd? b then u := (u * a) rem n;
			a := (a * a) rem n;
			b := shift(b, -1);
		}
		u;
	}

	hashCombine(acc: %, b: %): % == {
	    -- this needs to be "better", see util.c
	    acc * 31 + b
	}
}

extend Byte:Join(OutputType, InputType) == add {
	import from Z;
	(p:TextWriter) << (b:%):TextWriter	== p << b::Z;
	<< (p:TextReader):%			== lowByte((<< p)@Z);
}

#if ALDORTEST
---------------------- test --------------------------
#include "aldor"
#include "aldortest"

testParseInt(f: Literal -> MachineInteger): () == {
    import from Assert MachineInteger;
    import from StringBuffer;
    import from IntegerTypeTools MachineInteger;

    i: MachineInteger := f("100" pretend Literal);
    assertTrue(zero?(i-100));

    for n in -100..100 repeat {
        sb: StringBuffer := new();
	(sb::TextWriter) << n;
	nn := scan(sb::TextReader);
	assertEquals(n, nn);
    }
}


-- pass integer as a function to avoid compiler inlining
-- the call.
testParseInt(integer$MachineInteger);


#endif
