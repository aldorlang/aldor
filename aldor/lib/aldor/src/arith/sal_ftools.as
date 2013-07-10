---------------------------- sal_ftools.as -------------------------------
--
-- This file provides undocumented tools for float-like types
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro Z == MachineInteger;

-- internal type giving common implementations to concrete FloatType's
FloatTypeTools(T:FloatType): with {
	print: (T, Z, Z, TextWriter) -> TextWriter;
} == add {
	local ten:T		== { import from Z; 10::T; }
	local plus:Character	== { import from Z; char  43; }
	local minus:Character	== { import from Z; char  45; }
	local dot:Character	== { import from Z; char  46; }
	local e:Character	== { import from Z; char 101; }

	-- prints x 10^k showing at most n digits after the decimal point
	print(x:T, k:Z, n:Z, p:TextWriter):TextWriter == {
		import from AldorInteger, Boolean, Character;
		assert(x >= 1); assert(x < 10.0);
		p := p << truncate x;
		zero?(x := fraction x) => print(k, p);
		p := p << dot;
		i:Z := 0;
		while i < n and ~zero?(x) repeat {
			x := ten * x;
			p := p << char machine(48 + truncate x);
			x := fraction x;
			i := next i;
		}
		print(k, p);
	}

	local print(k:Z, p:TextWriter):TextWriter == {
		zero? k => p;
		p := p << e;
		if k > 0 then p := p << plus;
		p << k;
	}
}

