----------------------------- sal_otype.as ----------------------------------
--
-- This file defines writable objects
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel libaldor (c) INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

#if ALDOC
\thistype{OutputType}
\History{Manuel Bronstein}{28/9/98}{created}
\History{Manuel Bronstein}{14/11/2003}{added unary form of <<}
\Usage{\this: Category}
\Descr{\this~is the category of types whose objects can be written onto text
writers.}
\begin{exports}
\alexp{$<<$}: & (\altype{TextWriter}, \%) $\to$ \altype{TextWriter} &
write using text encoding\\
\alexp{$<<$}: & \% $\to$ \altype{TextWriter} $\to$ \altype{TextWriter} &
write function for an element\\
\end{exports}
#endif

define OutputType: Category == with {
	<<: (TextWriter, %) -> TextWriter;
	<<: % -> TextWriter -> TextWriter;
#if ALDOC
\alpage{$<<$}
\Usage{s $<<$ x\\ $<<$ x}
\Signatures{
$<<$: & (\altype{TextWriter}, \%) $\to$ \altype{TextWriter}\\
$<<$: & \% $\to$ \altype{TextWriter} $\to$ \altype{TextWriter}\\
}
\Params{
{\em s} & \altype{TextWriter} & an output stream\\
{\em x} & \% & an object of the type\\
}
\Retval{{\tt s << x} writes x in text format to the stream s
and returns s after the write, while {\tt << x} returns the
function $s \to s << x$.}
\begin{alex}
\begin{ttyout}
import from TextWriter, MachineInteger, Character;
stdout << 65 << space;
\end{ttyout}
writes ``65 '' to the standard output stream, which could also be
obtained via:
\begin{ttyout}
import from TextWriter, MachineInteger, Character;
f := << 65;
f(stdout) << space;
\end{ttyout}
\end{alex}
#endif
	export from OutputTypeFunctions %;
	default {
		(<<)(a:%)(port:TextWriter):TextWriter == port << a;
	}
}

OutputTypeFunctions(T: with): with == add

