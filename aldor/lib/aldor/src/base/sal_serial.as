---------------------------- sal_serial.as ---------------------------------
--
-- This file defines serializable types
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

#if ALDOC
\thistype{SerializableType}
\History{Manuel Bronstein}{1/10/98}{created}
\Usage{\this: Category}
\Descr{\this~is the category of types whose objects can be
read in and written out in binary mode.}
\begin{exports}
\asexp{$<<$}: & \astype{BinaryReader} $\to$ \% & read using binary encoding\\
\asexp{$<<$}: & (\astype{BinaryWriter}, \%) $\to$ \astype{BinaryWriter} &
write using binary encoding\\
\end{exports}
#endif

define SerializableType: Category == with {
	<<: BinaryReader -> %;
	<<: (BinaryWriter, %) -> BinaryWriter;
#if ALDOC
\aspage{$<<$}
\Usage{out $<<$ x\\$<<$ in}
\Signatures{
$<<$: & (\astype{BinaryWriter}, \%) $\to$ \astype{BinaryWriter}\\
$<<$: & \astype{BinaryReader} $\to$ \%\\
}
\Params{
{\em in} & \astype{BinaryReader} & an input stream\\
{\em out} & \astype{BinaryWriter} & an output stream\\
{\em x} & \% & an object of the type\\
}
\Retval{out $<<$ x writes x in binary format to the stream out
and returns the stream after the write, while $<<$ in reads an element of
the type in binary format from the stream in and
returns the element read.}
#endif
}

