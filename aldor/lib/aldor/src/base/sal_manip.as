---------------------------- sal_manip.as ---------------------------------
--
-- This file defines output stream manipulators
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
\thistype{WriterManipulator}
\History{Manuel Bronstein}{26/10/98}{created}
\Usage{import from \this}
\Descr{\this~provides manipulators for text or binary writers.}
\begin{exports}
\category{\altype{OutputType}}\\
\alexp{$<<$}: & (\altype{BinaryWriter}, \%) $\to$ \altype{BinaryWriter} &
manipulate a binary writer\\
\alexp{endnl}: & \% & send a newline and flush the stream\\
\alexp{flush}: & \% & flush the stream\\
\end{exports}
#endif

WriterManipulator: OutputType with {
	<<: (BinaryWriter, %) -> BinaryWriter;
#if ALDOC
\alpage{$<<$}
\Usage{out $<<$ x}
\Signature{(\altype{BinaryWriter}, \%)}{\altype{BinaryWriter}}
\Params{
{\em out} & \altype{BinaryWriter} & an output stream\\
{\em x} & \% & a manipulator\\
}
\Retval{out $<<$ x takes the action given by x on the stream out
and returns the stream after the action.}
#endif
	endnl: %;
#if ALDOC
\alpage{endnl}
\Usage{\name}
\alconstant{\%}
\Descr{Sending \name~to a text or binary writer causes a
\alfunc{Character}{newline}
to be sent to the stream and then the stream to be flushed,
so {\tt s $<<$ endnl} is equivalent to {\tt flush!(s $<<$ newline)}.}
#endif
	flush: %;
#if ALDOC
\alpage{flush}
\Usage{\name}
\alconstant{\%}
\Descr{Sending \name~to a text or binary writer causes the stream to
be flushed, so {\tt s $<<$ flush} is equivalent to {\tt flush!(s)}.
Has no effect on unbuffered streams, such as \alfunc{TextWriter}{stderr}.}
#endif
} == add {
	Rep == Ch;
	import from Rep, Z;

	flush:%			== per char 0;
	endnl:%			== per char 1;
	local int(x:%):Z	== ord rep x;

	(s:BinaryWriter) << (x:%):BinaryWriter == {
		import from Byte;
		zero?(n := int x) => flush! s;
		n = 1 => { write!(newline::Byte, s); flush! s; }
		never;
	}

	(s:TextWriter) << (x:%):TextWriter == {
		zero?(n := int x) => flush! s;
		n = 1 => { write!(newline, s); flush! s; }
		never;
	}
}
