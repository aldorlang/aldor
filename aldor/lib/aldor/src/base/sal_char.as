---------------------------- sal_char.as ---------------------------------
--
-- This file adds I/O and serialization to Character
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli (c) INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

-- Boolean is extended in this file, so don't import it
#assert DoNotImportBoolean

#include "aldor"

macro Z == MachineInteger;

#if ALDOC
\thistype{Character}
\History{Manuel Bronstein}{1/10/98}{created}
\Usage{import from \this}
\Descr{\this~implements machine characters.}
\begin{exports}
\category{\altype{HashType}}\\
\category{\altype{InputType}}\\
\category{\altype{OutputType}}\\
\category{\altype{PackableType}}\\
\category{\altype{SerializableType}}\\
\category{\altype{TotallyOrderedType}}\\
\alexp{char}: & \altype{MachineInteger} $\to$ \% & create a character\\
\alexp{digit?}: & \% $\to$ \altype{Boolean} & test for a decimal digit\\
\alexp{eof}: & \% & end--of--file character\\
\alexp{letter?}: & \% $\to$ \altype{Boolean} & test for a letter\\
\alexp{lower}: & \% $\to$ \% & convert to lower case\\
\alexp{newline}: & \% & newline character\\
\alexp{null}: & \% & null character\\
\alexp{ord}: & \% $\to$ \altype{MachineInteger} & character code\\
\alexp{space?}: & \% $\to$ \altype{Boolean} & test for a blank space\\
\alexp{tab}: & \% & tab character\\
\alexp{upper}: & \% $\to$ \% & convert to upper case\\
\end{exports}

\alpage{char,ord}
\altarget{char}
\altarget{ord}
\Usage{char~n\\ord~c}
\Signatures{
char: & \altype{MachineInteger} $\to$ \%\\
ord: & \% $\to$ \altype{MachineInteger}\\
}
\Params{
{\em n} & \altype{MachineInteger} & a character code\\
{\em c} & \% & a character\\
}
\Retval{char(n) returns the character whose code is n, while ord(c) returns
the code corresponding to the character c.}

\alpage{digit?,letter?,space?}
\altarget{digit?}
\altarget{letter?}
\altarget{space?}
\Usage{digit?~c\\letter?~c\\space?~c}
\Signature{\%}{\altype{Boolean}}
\Params{ {\em c} & \% & a character\\ }
\Retval{digit?(c) returns \true if c is in the range '0'--'9', \false otherwise,
while letter?(c) returns \true if c is in the range 'a'--'z' or the range
'A'--'Z', \false otherwise and space?(c) returns \true if c is a blank space,
\ie a space or a tab, \false otherwise.}

\alpage{eof,newline,null,tab}
\altarget{eof}
\altarget{newline}
\altarget{null}
\altarget{tab}
\Usage{eof\\ newline\\ null\\ tab}
\alconstant{\%}
\Retval{eof is the end--of--file character, newline is the newline character,
null is the 0-character (used to terminate strings) and tab is the
tab character.}

\alpage{lower,upper}
\altarget{lower}
\altarget{upper}
\Usage{lower~c\\upper~c}
\Signature{\%}{\%}
\Params{ {\em c} & \% & a character\\ }
\Retval{lower(c) and upper(c) return c converted to lower, respectively upper,
case.}
#endif
-- characters are read and written the same way, whether in text or binary
extend Character:
	Join(SerializableType, InputType, OutputType, HashType, PackableType,
		TotallyOrderedType) == add {
	import from Byte;
	<< (p:TextReader):%			== read! p;
	<< (p:BinaryReader):%			== read!(p)::%;
	(p:TextWriter) << (x:%):TextWriter	== { write!(x, p); p }
	(p:BinaryWriter) << (x:%):BinaryWriter	== { write!(x::Byte, p); p }
	hash(x:%):MachineInteger		== ord x;
	compare(a:%, b:%):MachineInteger	== ord(a) - ord(b);

	import from Machine;
	import {
		ArrNew: (Char, SInt) -> Arr;
		ArrElt: (Arr,  SInt) -> Char;
		ArrSet: (Arr,  SInt, Char) -> Char;
	} from Builtin;

	getPackedArray(a:Pointer, n:Z):% == ArrElt(a pretend Arr, n::SInt)::%;

	newPackedArray(n:Z):Pointer == {
		AGAT("pkcharnew", n);
		ArrNew(null::Char, n::SInt) pretend Pointer;
	}

	setPackedArray!(a:Pointer, n:Z, c:%):() == 
		ArrSet(a pretend Arr, n::SInt, c::Char);

	-- TEMPORARY (BUG1182) DEFAULTS DON'T INLINE WELL
	(a:%) > (b:%):Boolean	== ~(a <= b);
	(a:%) >= (b:%):Boolean	== ~(a < b);
}

-- read/write T/F in text, 1/0 in binary (1 byte)
extend Boolean:Join(SerializableType, InputType, OutputType) == add {
	import from Byte, Character, MachineInteger;

	(p:TextWriter) << (x:%):TextWriter	== p << char({ x => 84; 70 });
	(p:BinaryWriter) << (x:%):BinaryWriter	== p << lowByte({ x => 1; 0 });

	<< (p:TextReader):% == {
		local c:Character;
		(c := << p) = char 70 => false;
		assert(c = char 84);
		true;
	}

	<< (p:BinaryReader):% == {
		zero?(n := (<< p)@Byte :: MachineInteger) => false;
		assert(n = 1);
		true;
	}
}

