---------------------------- sal_tstream.as ---------------------------------
--
-- This file defines text read/write streams
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli (c) INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro {
	Ch == Character;
	Z == MachineInteger;
}

#if ALDOC
\thistype{TextReader}
\History{Manuel Bronstein}{1/10/98}{created}
\Usage{import from \this}
\Descr{\this~provides various input streams.}
\begin{exports}
\alexp{options}: & \altype{SymbolTable} \altype{MachineInteger} & global options\\
\alexp{push!}: & (C, \%) $\to$ () & push back a character\\
\alexp{read!}: & \% $\to$ C & read one character\\
\alexp{readUntil!}:
& (\altype{StringBuffer}, \%, C) $\to$ C & read several characters\\
\alexp{readWhile!}:
& (\altype{StringBuffer}, \%, C $\to$ \altype{Boolean}) $\to$ C &
read several characters\\
\alexp{skipUntil!}: & (\%, C) $\to$ C & skip characters\\
\alexp{skipWhile!}: & (\%, C $\to$ \altype{Boolean}) $\to$ C & skip characters\\
\alexp{stdin}: & \% & standard input stream\\
\alexp{textReader}: & (() $\to$ C, C $\to$ ()) $\to$ \% & create a stream\\
\end{exports}
\begin{alwhere}
C &==& \altype{Character}\\
\end{alwhere}
#endif

TextReader: with {
	-- options is provided by an extension in ald_symbol.as
#if ALDOC
\alpage{options}
\Usage{\name}
\alconstant{\altype{SymbolTable} \altype{MachineInteger}}
\Retval{Returns a table of global options for text input.
Those options can be queried by your
implementations of~\alfunc{InputType}{$<<$}.}
#endif
	push!: (Ch, %) -> ();
#if ALDOC
\alpage{push!}
\Usage{\name(c, s)}
\Signature{(\altype{Character},\%)}{()}
\Params{
{\em c} & \altype{Character} & a character\\
{\em s} & \% & a stream\\
}
\Retval{Pushes the character $c$ back onto $s$.
One character of pushback is guaranteed, this function might
however fail if it is called too many times on the same stream
without intervening read or positionning calls.}
#endif
	read!: % -> Ch;
#if ALDOC
\alpage{read!}
\Usage{\name~s}
\Signature{\%}{\altype{Character}}
\Params{ {\em s} & \% & a stream\\ }
\Retval{Reads a character from $s$ and returns it. Returns
\alfunc{Character}{eof} if the end of file is reached.}
#endif
	skipUntil!: (%, Ch) -> Ch;
	skipWhile!: (%, Ch -> Boolean) -> Ch;
#if ALDOC
\alpage{readUntil!,readWhile!,skipUntil!,skipWhile!}
\altarget{readUntil!}
\altarget{readWhile!}
\altarget{skipUntil!}
\altarget{skipWhile!}
\Usage{readUntil!(str, s, c)\\ readWhile!(str, s, read?)\\
skipUntil!(s, c)\\ skipWhile!(s, read?)}
\Signatures{
readUntil!: &
(\altype{StringBuffer}, \%, \altype{Character}) $\to$ \altype{Character}\\
readWhile!: &
(\altype{StringBuffer}, \%, \altype{Character} $\to$ \altype{Boolean}) $\to$
\altype{Character}\\
skipUntil!: & (\%, \altype{Character}) $\to$ \altype{Character}\\
skipWhile!: &
(\%, \altype{Character} $\to$ \altype{Boolean}) $\to$ \altype{Character}\\
}
\Params{
\emph{str} & \altype{StringBuffer} & a buffer\\
{\em s} & \% & a stream\\
{\em c} & \altype{Character} & a character\\
\emph{read?} & \altype{Character} $\to$ \altype{Boolean} & a predicate\\
}
\Descr{readUntil!(str, s, c) reads characters from \emph{s} and stores
them in \emph{str}, until the character \emph{c} is read.
Returns \emph{c} if it is read, \alfunc{Character}{eof} otherwise.
readWhile!(str, s, read?) reads characters from \emph{s} and stores
them in \emph{str} as long as \emph{read?} is \true{} on the characters read.
Returns either the first character read for which \emph{read?} is \false{}
or \alfunc{Character}{eof}.
skipUntil! and skipWhile! work like readUntil! and readWhile! except that
the characters read are not stored anywhere.}
#endif
	stdin: %;
#if ALDOC
\alpage{stdin}
\Usage{\name}
\alconstant{\%}
\Retval{\name~is the standard input stream.}
#endif
	textReader: (() -> Ch, Ch -> ()) -> %;
#if ALDOC
\alpage{textReader}
\Usage{\name(f, g)}
\Signature{(() $\to$ \altype{Character}, \altype{Character} $\to$ ())}{\%}
\Params{
{\em f} & () $\to$ \altype{Character} & the single--character read function\\
{\em g}& \altype{Character} $\to$ () & the single--character pushback function\\
}
\Retval{Returns the input stream for which $f()$ reads a character
and $g(c)$ pushes back the character \emph{c}.}
\begin{asex}
\name~can be used to modify existing readers by ``hooking'' their
\alexp{read!} and \alexp{push!} functions in order to customize their
behavior. For example, the following function {\tt counter} transforms
any \altype{TextReader} into one that keeps track of the number of
characters and lines read from it.
\begin{ttyout}
macro COUNT == Record(chars:MachineInteger, lines:MachineInteger);

counter(rd:TextReader, ct:COUNT):TextReader == {
      ct.chars := ct.lines := 0;    -- initialize counts to 0
      textReader(readAndCount!(rd, ct), pushAndCount!(rd, ct));
}

local readAndCount!(rd:TextReader, ct:COUNT)():Character == {
      c := read! rd;
      if c ~= eof then ct.chars := next(ct.chars);
      if c = newline then ct.lines := next(ct.lines);
      c;
}

local pushAndCount!(rd:TextReader, ct:COUNT)(c:Character):() == {
      if c ~= eof then ct.chars := prev(ct.chars);
      if c = newline then ct.lines := prev(ct.lines);
      push!(c, rd);
}
\end{ttyout}
With the above function, the statements
\begin{ttyout}
cnt:COUNT := [0, 0];
countin := counter(stdin, cnt);
\end{ttyout}
produces the reader {\tt countin} that reads from \alexp{stdin} while
side-effecting the variable {\tt cnt} in order to keep track of the
characters and lines read.
\end{asex}
#endif
} == add {
	Rep == Record(read: () -> Ch, push: Ch -> ());

	import {
		fgetc: Pointer -> Z;
		-- TEMPO: NEEDS A TYPE FOR C-int
		-- ungetc: (CInteger, Pointer) -> CInteger;
		lungetc: (Z, Pointer) -> Z;
		stdinFile: () -> Pointer;
	} from Foreign C;

	local getc(s:Pointer)():Ch	== char fgetc s;
	local pbak(s:Pointer)(c:Ch):()	== lungetc(ord c, s);
	read!(s:%):Ch			== { import from Rep; rep(s).read(); }
	push!(c:Ch, s:%):()		== { import from Rep; rep(s).push(c); }

	skipUntil!(s:%, stop:Ch):Ch ==
		skipWhile!(s, (c:Ch):Boolean +-> c ~= stop);

	skipWhile!(s:%, skip?: Ch -> Boolean):Ch == {
		c := read! s;
		while c ~= eof and skip? c repeat c := read! s;
		c;
	}

	textReader(f:() -> Ch, g:Ch -> ()):% == {
		import from Rep;
		per [f, g];
	}

	local creader(s:Pointer):% == {
		import from Rep;
		per [getc s, pbak s];
	}

	stdin:%				== creader stdinFile();
}

#if ALDOC
\thistype{TextWriter}
\History{Manuel Bronstein}{28/9/98}{created}
\Usage{import from \this}
\Descr{\this~provides various output streams.}
\begin{exports}
\alexp{flush!}: & \% $\to$ \% & flush a stream\\
\alexp{options}: & \altype{SymbolTable} \altype{MachineInteger} & global options\\
\alexp{stderr}: & \% & the standard error stream\\
\alexp{stdout}: & \% & the standard output stream\\
\alexp{textWriter}: & (\altype{Character} $\to$ ()) $\to$ \% & create a stream\\
\alexp{textWriter}:
& (\altype{Character} $\to$ (), () $\to$ ()) $\to$ \% & create a stream\\
\alexp{write!}: & (\altype{Character}, \%) $\to$ () & write to a stream\\
\end{exports}
#endif

TextWriter: with {
	flush!: % -> %;
#if ALDOC
\alpage{flush!}
\Usage{\name~s}
\Signature{\%}{\%}
\Params{ {\em s} & \% & a stream\\ }
\Descr{\name(s) causes all previous values inserted into s to be really
written and returns the stream.
Has no effect on unbuffered streams, such as \alexp{stderr}.}
#endif
	-- options is provided by an extension in ald_symbol.as
#if ALDOC
\alpage{options}
\Usage{\name}
\alconstant{\altype{SymbolTable} \altype{MachineInteger}}
\Retval{Returns a table of global options for text output.
Those options can be queried by your
implementations of~\alfunc{OutputType}{$<<$}.}
#endif
	stderr: %;
	stdout: %;
#if ALDOC
\alpage{stderr,stdout}
\altarget{stderr}
\altarget{stdout}
\Usage{stderr\\stdout}
\alconstant{\%}
\Retval{stderr and stdout are the standard error and standard
output streams respectively.}
#endif
	textWriter: (Ch -> ()) -> %;
	textWriter: (Ch -> (), () -> ()) -> %;
#if ALDOC
\alpage{textWriter}
\Usage{\name~f\\ \name(f, g)}
\Signatures{
\name: & (\altype{Character} $\to$ ()) $\to$ \%\\
\name: & (\altype{Character} $\to$ (), () $\to$ ()) $\to$ \%\\
}
\Params{
{\em f} & \altype{Character} $\to$ () & the single--character write function\\
{\em g} & () $\to$ () & the flush function (optional)\\
}
\Retval{Returns the output stream for which $f(c)$ writes the character $c$
and such that $g()$ flushes the stream.
If g is not given, then flushing the resulting stream has no effect.}
#endif
	write!: (Ch, %) -> ();
#if ALDOC
\alpage{write!}
\Usage{\name(c, s)}
\Signature{(\altype{Character}, \%)}{()}
\Params{
{\em c} & \altype{Character} & character to write\\
{\em s} & \% & a stream\\
}
\Retval{Writes the character $c$ to the stream $s$ and returns $c$.}
#endif
} == add {
	Rep == Record(write: Ch -> (), flush: () -> ());

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
		per [putc! s, flush s];
	}

	local putc!(s:Pointer)(c:Ch):()	== fputc(ord c, s);
	local flush(s:Pointer)():()	== fflush s;
	write!(c:Ch, s:%):()		== { import from Rep; rep(s).write(c); }
	local nop():()			== {};
	textWriter(f:Ch -> ()):%	== textWriter(f, nop);
	stderr:%			== cwriter stderrFile();
	stdout:%			== cwriter stdoutFile();

	flush!(s:%):% == {
		import from Rep;
		rep(s).flush();
		s;
	}

	textWriter(f:Ch -> (), g:() -> ()):% == {
		import from Rep;
		per [f, g];
	}
}

