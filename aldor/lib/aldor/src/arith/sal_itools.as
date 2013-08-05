---------------------------- sal_itools.as -------------------------------
--
-- This file provides undocumented tools for integer-like types
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli (c) INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro Z == MachineInteger;

-- internal type giving common implementations to concrete IntegerType's
IntegerTypeTools(T:IntegerType): with {
	binaryNthRoot: (T, T) -> (Boolean, T);
	print: (T, T, TextWriter) -> TextWriter;	-- second arg is base
	scan: TextReader -> T;
	scan: (TextReader, T, T) -> T;			-- second arg is base
} == add {
	local plus:Character	== { import from Z; char 43; }
	local minus:Character	== { import from Z; char 45; }

	binaryNthRoot(x:T, e:T):(Boolean, T) == {
		zero? x or one? x => (true, x);
		x < 0 => {
			assert(odd? e);
			(found?, s) := binaryNthRoot(-x, e);
			(found?, { found? => -s; prev(-s) });
		}
		assert(x > 1);
		ee := machine e;
		import from BinarySearch(T, T);
		binarySearch(x, (s:T):T +-> s^ee, 1, x quo e);
	}

	scan(p:TextReader):T == {
		import from Boolean;
		local c:Character;
		while space?(c := << p) or c = newline repeat {};
		c = plus => read p;
		c = minus => - read p;
		~digit? c => throw SyntaxException;
		scan(p, 10::T, value c);
	}

	-- the sign (+/-) has been read, spaces are allowed but no newlines
	local read(p:TextReader):T == {
		import from Boolean;
		local c:Character;
		while space?(c := << p) repeat {};
		~digit? c => throw SyntaxException;
		scan(p, 10::T, value c);
	}

	-- the sign (+/-) and first digit have been read, no spaces allowed
	-- n = value already read
	scan(p:TextReader, base:T, n:T):T == {
		local c:Character;
		while digit?(c := << p, base) repeat n := base * n + value c;
		push!(c, p);
		n;
	}

	local value(c:Character):T == {
		import from Z;
		digit? c => ord(c)::T - 48::T;
		ord(c)::T - 87::T;
	}

	local digit?(c:Character, base:T):Boolean == {
		import from Z;
		digit? c => true;
		n := ord(c)::T - 87::T;
		n > 9 and n < base;
	}

	local digit(x:T):Character == {
		assert(x >= 0);
		x < 10::T => char machine(x + 48::T);
		char machine(x + 87::T);
	}

	print(x:T, base:T, p:TextWriter):TextWriter == {
		macro REC == Record(digit:Character, next:Pointer);
		import from Boolean, Character, REC;
		zero? x => p << char 48;
		l:REC := nil;
		if x < 0 then {
			p << minus;
			(x, r) := divide(x, base);
			l := [digit(-r), l pretend Pointer];
			x := -x;
		}
		assert(x >= 0);

		while not zero? x repeat {
			(x, r) := divide(x, base);
			l := [digit r, l pretend Pointer];
		}
		while ~nil? l repeat {
			p := p << l.digit;
			l := l.next pretend REC;
		}
		p;
	}
}

