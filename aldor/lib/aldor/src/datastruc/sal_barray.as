---------------------------- sal_barray.as ----------------------------------
--
-- This file defines primitive memory blocks, i.e. packed byte-arrays
-- Those arrays are 0-indexed and do not do bound-checking.
--
-- Copyright (c) Manuel Bronstein 2000
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro Z == MachineInteger;

#if ALDOC
\thistype{PrimitiveMemoryBlock}
\History{Manuel Bronstein}{24/5/2000}{created}
\Usage{import from \this}
\Descr{\this~provides packed arrays of bytes,
$0$-indexed and without bound checking (the debug version
of \salli provides bound--checking for primitive memory blocks).}
\begin{exports}
\category{\astype{PrimitiveArrayType} \astype{Byte}}\\
\asexp{coerce}:
& \% $\to$ \astype{BinaryReader} & conversion to a binary input stream\\
\asexp{coerce}:
& \% $\to$ \astype{BinaryWriter} & conversion to a binary output stream\\
\end{exports}
#endif

PrimitiveMemoryBlock: PrimitiveArrayType Byte with {
	coerce: % -> BinaryReader;
	coerce: % -> BinaryWriter;
#if ALDOC
\aspage{coerce}
\Usage{a::BinaryReader\\ a::BinaryWriter}
\Signatures{
\name: & \% $\to$ \astype{BinaryReader}\\
\name: & \% $\to$ \astype{BinaryWriter}\\
}
\Params{ {\em a} & \% & a primitive memory block\\ }
\Descr{a::T where T is an I/O stream type
converts the block s to a binary reader or writer, allowing
one to read data or write data to it.}
\Remarks{When writing to a memory block, you must ensure that the block is
large enough for all the data that will be written to it, since the
block will not be extended and this function does not protect you against
overwriting. When reading from or writing to a memory block, each coercion
to a reader or writer resets the stream to the beginning of the block,
and the block is not side--affected by the subsequent read or write operations,
while the stream is side--affected.
Thus, when reading several values from the same block, you must assign
the reader to a variable and read the values from that variable.}
\alseealso{\asfunc{String}{coerce}}.
#endif
} == PackedPrimitiveArray Byte add {
	coerce(s:%):BinaryReader	== binaryReader getb! s;
	coerce(s:%):BinaryWriter	== binaryWriter putb! s;

	local putb!(s:%):Byte -> () == {
		import from Z;
		i:Z := 0;
		(b:Byte):() +-> { s.i := b; free i := next i; }
	}

	-- Binary-mode scanning, never send eof, no pushback
	local getb!(s:%):() -> Byte == {
		import from Z;
		i:Z := 0;
		():Byte +-> {
			b := s.i;
			free i := next i;
			b;
		}
	}
}
