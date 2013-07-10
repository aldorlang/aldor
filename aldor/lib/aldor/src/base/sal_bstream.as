---------------------------- sal_bstream.as ---------------------------------
--
-- This file defines binary read/write streams
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli (c) INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro Z == MachineInteger;

#if ALDOC
\thistype{BinaryReader}
\History{Manuel Bronstein}{1/10/98}{created}
\Usage{import from \this}
\Descr{\this~provides various binary input streams.}
\begin{exports}
\alexp{bin}: & \% & standard input stream\\
\alexp{binaryReader}: & () $\to$ \altype{Byte} $\to$ \% & create a stream\\
\alexp{options}: & \altype{SymbolTable} \altype{MachineInteger} & global options\\
\alexp{read!}: & \% $\to$ \altype{Byte} & read from a stream\\
\end{exports}
#endif

BinaryReader: with {
	bin: %;
#if ALDOC
\alpage{bin}
\Usage{\name}
\alconstant{\%}
\Retval{bin is the standard input stream.}
#endif
	binaryReader: (() -> Byte) -> %;
#if ALDOC
\alpage{binaryReader}
\Usage{\name~f}
\Signature{(() $\to$ \altype{Byte})}{\%}
\Params{ {\em f} & () $\to$ \altype{Byte} & the single--byte read function\\ }
\Retval{Returns the input stream for which $f()$ reads a byte.}
#endif
	-- options is provided by an extension in ald_symtab.as
#if ALDOC
\alpage{options}
\Usage{\name}
\alconstant{\altype{SymbolTable} \altype{MachineInteger}}
\Retval{Returns a table of global options for binary input.
Those options can be queried by your
implementations of~\alfunc{SerializableType}{$<<$}.}
#endif
	read!: % -> Byte;
#if ALDOC
\alpage{read!}
\Usage{\name~s}
\Signature{\%}{\altype{Byte}}
\Params{ {\em s} & \% & a stream\\ }
\Retval{Reads a byte from $s$ and returns it. Some streams may return
the byte \alfunc{Byte}{eof} if the end of file is reached.}
#endif
} == add {
	macro Rep == (() -> Byte);

	import {
		fgetc: Pointer -> Z;
		stdinFile: () -> Pointer;
	} from Foreign C;

	binaryReader(f:() -> Byte):%	== per f;
	local getc(s:Pointer)():Byte	== lowByte fgetc s;
	read!(s:%):Byte			== rep(s)();
	local creader(s:Pointer):%	== per getc s;
	bin:%				== creader stdinFile();
}

#if ALDOC
\thistype{BinaryWriter}
\History{Manuel Bronstein}{8/12/98}{created}
\Usage{import from \this}
\Descr{\this~provides various binary output streams.}
\begin{exports}
\alexp{berr}: & \% & the binary standard error stream\\
\alexp{bout}: & \% & the binary standard output stream\\
\alexp{binaryWriter}:
& (\altype{Byte} $\to$ ()) $\to$ \% & create a stream\\
\alexp{binaryWriter}:
& (\altype{Byte} $\to$ (), () $\to$ ()) $\to$ \% & create a stream\\
\alexp{flush!}: & \% $\to$ \% & flush a stream\\
\alexp{options}: & \altype{SymbolTable} \altype{MachineInteger} & global options\\
\alexp{write!}: & (\altype{Byte}, \%) $\to$ () & write to a stream\\
\end{exports}
#endif

BinaryWriter: with {
	berr: %;
	bout: %;
#if ALDOC
\alpage{berr,bout}
\altarget{berr}
\altarget{bout}
\Usage{berr\\bout}
\alconstant{\%}
\Retval{berr and bout are the binary standard error and standard
output streams respectively.}
#endif
	binaryWriter: (Byte -> ()) -> %;
	binaryWriter: (Byte -> (), () -> ()) -> %;
#if ALDOC
\alpage{binaryWriter}
\Usage{\name~f\\ \name(f, g)}
\Signatures{
\name: & (\altype{Byte} $\to$ ()) $\to$ \%\\
\name: & (\altype{Byte} $\to$ (), () $\to$ ()) $\to$ \%\\
}
\Params{
{\em f} & \altype{Byte} $\to$ () & the single--byte write function\\
{\em g} & () $\to$ () & the flush function (optional)\\
}
\Retval{Returns the output stream for which $f(c)$ writes the byte $c$
and such that $g()$ flushes the stream.
If g is not given, then flushing the resulting stream has no effect.}
#endif
	flush!: % -> %;
#if ALDOC
\alpage{flush!}
\Usage{\name~s}
\Signature{\%}{\%}
\Params{ {\em s} & \% & a stream\\ }
\Descr{\name(s) causes all previous values inserted into s to be really
written and returns the stream.
Has no effect on unbuffered streams, such as \alexp{berr}.}
#endif
	-- options is provided by an extension in ald_symtab.as
#if ALDOC
\alpage{options}
\Usage{\name}
\alconstant{\altype{SymbolTable} \altype{MachineInteger}}
\Retval{Returns a table of global options for binary output.
Those options can be queried by your
implementations of~\alfunc{SerializableType}{$<<$}.}
#endif
	write!: (Byte, %) -> ();
#if ALDOC
\alpage{write!}
\Usage{\name(c, s)}
\Signature{(\altype{Byte}, \%)}{()}
\Params{
{\em c} & \altype{Byte} & byte to write\\
{\em s} & \% & a stream\\
}
\Retval{Writes the byte $c$ to the stream $s$ and returns $c$.}
#endif
} == add {
	Rep == Record(write: Byte -> (), flush: () -> ());

	import {
		fflush: Pointer -> Z;
		-- TEMPO: NEEDS A TYPE FOR C-int
		fputc: (Z, Pointer) -> Z;
		-- fputc: (CInteger, Pointer) -> CInteger;
		stderrFile: () -> Pointer;
		stdoutFile: () -> Pointer;
	} from Foreign C;

	local cwriter(s:Pointer):% == {
		import from Rep;
		per [put! s, flush s];
	}

	local put!(s:Pointer)(c:Byte):()== fputc(c::Z, s);
	local flush(s:Pointer)():()	== fflush s;
	write!(c:Byte, s:%):()		== { import from Rep; rep(s).write(c); }
	local nop():()			== {};
	binaryWriter(f:Byte -> ()):%	== binaryWriter(f, nop);
	berr:%				== cwriter stderrFile();
	bout:%				== cwriter stdoutFile();

	flush!(s:%):% == {
		import from Rep;
		rep(s).flush();
		s;
	}

	binaryWriter(f:Byte -> (), g:() -> ()):% == {
		import from Rep;
		per [f, g];
	}
}
