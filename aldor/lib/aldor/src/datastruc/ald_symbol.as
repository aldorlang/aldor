------------------------------- sit_symbol.as ----------------------------------
--
-- Symbols, i.e. read-only strings with O(1) comparison
--
-- Copyright (c) Manuel Bronstein 1999
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "aldor"

#if ALDOC
\thistype{Symbol}
\History{Manuel Bronstein}{30/6/99}{created}
\Usage{import from \this}
\Descr{\this provides symbols,~\ie read--only strings with
constant--time comparison.}
\begin{exports}
\category{\altype{HashType}}\\
\category{\altype{InputType}}\\
\category{\altype{OutputType}}\\
\category{\altype{SerializableType}}\\
\alexp{$-$}: & \altype{String} $\to$ \% & Create a symbol\\
\alexp{name}: & \% $\to$ \altype{String} & Name of a symbol\\
\alexp{new}: & () $\to$ \% & Create a new symbol\\
\end{exports}
#endif

macro {
	Z	== MachineInteger;
	PZ	== Record(cnt:Z);
	H	== HashTable(String, String);
}

Symbol: Join(HashType, InputType, OutputType, SerializableType) with {
	-:  String -> %;
#if ALDOC
\alpage{$-$}
\Usage{$-s$}
\Signature{\altype{String}}{\%}
\Params{{\em s} & \altype{String} & A string\\ }
\Retval{Returns s as a symbol.}
#endif
	name: % -> String;
#if ALDOC
\alpage{name}
\Usage{\name~s}
\Signature{\%}{\altype{String}}
\Params{{\em s} & \% & A symbol\\ }
\Retval{Returns a new copy of the name of s.}
\Remarks{Modifying \name(s) does not modify s, since a new copy
is created at each call.}
#endif
	new: () -> %;
#if ALDOC
\alpage{new}
\Usage{\name()}
\Signature{()}{\%}
\Retval{Returns a new symbol.}
#endif
symbols: () -> H;
} == add {
	Rep == String;

	-- This hash-table makes this type non-reentrant
	local stable:H		== table();
	local enter(s:String):%	== { stable.s := s; per s }
	local root:String	== "%v";
	local buffer:String	== { import from Z; new 21 } -- enough for 2^64
	local counter:PZ	== { import from Z; [0] }
	name(s:%):String	== { import from String; copy rep s }
	(s:%) = (t:%):Boolean	== { import from Pointer; address s = address t}
	<< (p:TextReader):%	== { import from String; -(<< p); }
	<< (p:BinaryReader):%	== { import from String; -(<< p); }
	(p:TextWriter) << (s:%):TextWriter	== { import from Rep;p << rep s}
	(p:BinaryWriter) << (s:%):BinaryWriter	== { import from Rep;p << rep s}
	local address(s:%):Pointer		== s pretend Pointer;

	symbols(): H == stable;

	-- use address-based hashing in order to speed-up symbol-table lookup
	hash(s:%):Z		== { import from Pointer; address(s)::Z }

	new():% == {
		import from TextWriter, String, Partial String;
		import from H, Z, PZ, Character;
		for c in next(counter.cnt).. repeat {
			buffer::TextWriter << c << null;
			s := root + buffer;
			failed? find(s, stable) => {
				counter.cnt := c;
				return enter s;
			}
		}
		never;
	}

	-(s:String):% == {
		import from H, Partial String;
		failed?(u := find(s, stable)) => enter copy s;
		per retract u;
	}

}

#if ALDORTEST
#include "aldor"
import from Assert Symbol;
import from Symbol;
import from String;

test(): () == {
   a := -"fred";
   a2 := -("fr" + "ed");
   assertEquals(a, a2);
}

test();
#endif
