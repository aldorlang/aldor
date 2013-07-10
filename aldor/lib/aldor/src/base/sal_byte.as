---------------------------- sal_byte.as ---------------------------------
--
-- This file adds I/O and serialization to Byte
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro Z == MachineInteger;

#if ALDOC
\thistype{Byte}
\History{Manuel Bronstein}{8/12/98}{created}
\Usage{import from \this}
\Descr{\this~implements machine bytes.}
\begin{exports}
\category{\astype{HashType}}\\
\category{\astype{InputType}}\\
\category{\astype{OutputType}}\\
\category{\astype{PackableType}}\\
\category{\astype{SerializableType}}\\
\asexp{coerce}: & \% $\to$ \astype{MachineInteger} & conversion to an integer\\
\asexp{coerce}: & \% $\to$ \astype{Character} & conversion to a character\\
\asexp{coerce}: & \astype{Character} $\to$ \% & conversion from a character\\
\asexp{eof}: & \% & end--of--file marker\\
\asexp{lowByte}: & \astype{MachineInteger} $\to$ \% & low--byte of an integer\\
\end{exports}

\aspage{coerce}
\Usage{b::\astype{MachineInteger}\\ b::\astype{Character}\\ c::\%}
\Signatures{
\name: & \% $\to$ \astype{MachineInteger}\\
\name: & \% $\to$ \astype{Character}\\
\name: & \astype{Character} $\to$ \%\\
}
\Params{
{\em b} & \% & a byte\\
{\em c} & \astype{Character} & a character\\
}
\Retval{b::\astype{MachineInteger} and b::\astype{Character} return b
converted to an integer and a character respectively,
while c::\% returns c converted to a byte.}

\aspage{eof}
\Usage{\name}
\alconstant{\%}
\Retval{eof is the end--of--file marker.}

\aspage{lowByte}
\Usage{\name~n}
\Signature{\astype{MachineInteger}}{\%}
\Params{ {\em n} & \astype{MachineInteger} & an integer\\ }
\Retval{Returns the low--byte of n.}
#endif
-- bytes are read and written the same way, whether in text or binary
-- the extensions to InputType and OutputType are done later after machine-int
extend Byte:Join(HashType, SerializableType, PackableType) == add {
	<< (p:BinaryReader):%			== read! p;
	(p:BinaryWriter) << (x:%):BinaryWriter	== { write!(x, p); p }
	hash(x:%):MachineInteger		== x::MachineInteger;

	import from Machine;
	import {
		ArrNew: (XByte, SInt) -> Arr;
		ArrElt: (Arr,  SInt) -> XByte;
		ArrSet: (Arr,  SInt, XByte) -> XByte;
	} from Builtin;

	getPackedArray(a:Pointer, n:Z):% == ArrElt(a pretend Arr, n::SInt)::%;

	newPackedArray(n:Z):Pointer == {
		AGAT("pkbytenew", n);
		ArrNew(eof::XByte, n::SInt) pretend Pointer;
	}

	setPackedArray!(a:Pointer, n:Z, c:%):() == 
		ArrSet(a pretend Arr, n::SInt, c::XByte);
}

