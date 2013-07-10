---------------------------- sal_pointer.as ------------------------------------
--
-- Pointers
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro Z == MachineInteger;

#if ALDOC
\thistype{Pointer}
\History{Manuel Bronstein}{28/9/98}{created}
\Usage{import from \this}
\Descr{\this~implements machine pointers.}
\begin{exports}
\category{\astype{HashType}}\\
\category{\astype{InputType}}\\
\category{\astype{OutputType}}\\
\category{\astype{SerializableType}}\\
{\tt coerce}: & \% $\to$ \astype{MachineInteger} & conversion to an integer\\
{\tt coerce}: & \astype{MachineInteger} $\to$ \% & conversion from an integer\\
\asexp{nil}: & \% & the nil pointer\\
\asexp{nil?}: & \% $\to$ \astype{Boolean} & test for the nil pointer\\
\end{exports}

\aspage{nil}
\astarget{\name?}
\Usage{\name\\ \name?~p}
\Signatures{
\name: & \% \\
\name?: & \% $\to$ \astype{Boolean}\\
}
\Params{ {\em p} & \% & a pointer\\ }
\Retval{\name~returns the nil pointer, while \name?(p) returns \true~if
p is the nil pointer, \false otherwise.}
#endif

extend Pointer:Join(HashType, InputType, OutputType, SerializableType) == add {
	import from Z;
	<< (p:BinaryReader):%			== (<< p)@Z :: %;
	(p:BinaryWriter) << (x:%):BinaryWriter	== p << x::Z;
	hash(x:%):Z				== x::Z;

	<< (p:TextReader):% == {
		import from Z, IntegerTypeTools Z;
		local c:Character;
		while space?(c := << p) or c = newline repeat {};
		c ~= char 48 => throw SyntaxException;
		c := << p;
		c ~= char 120 => throw SyntaxException;
		scan(p, 16, 0)::%;
	}

	(p:TextWriter) << (x:%):TextWriter == {
		import from Z, IntegerTypeTools Z, Character;
		print(x::Z, 16, p << char 48 << char 120);	-- 0x....
	}
}
