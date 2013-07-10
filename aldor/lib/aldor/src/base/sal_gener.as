---------------------------- sal_gener.as ----------------------------------
--
-- This file defines exception-throwing generators.
--
-- Copyright (c) Manuel Bronstein 1999
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli (c) INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

-- Boolean is not defined yet in the build sequence, so don't import it
#assert DoNotImportBoolean

#include "aldor"

#if ALDOC
\thistype{Generator}
\History{Manuel Bronstein}{13/12/99}{created}
\History{Manuel Bronstein}{30/1/2003}{added interlacing (in an extension)}
\History{Manuel Bronstein}{7/4/2003}{added concat (in an extension)}
\History{Manuel Bronstein}{26/6/2003}{added partialNext! (in an extension)}
\Usage{import from \this~T}
\Params{{\em T} & Type & the type of the elements generated\\}
\Descr{\this~T is a type which allows values of type T to be obtained serially
in a `repeat' or `collect' form.}
\begin{exports}
\alexp{concat}: & \altype{List} \% $\to$ \% & concatenation\\
\alexp{interlacing}: & \altype{List} \% $\to$ \% & interlacing\\
\alexp{next!}: & \% $\to$ T & get the next element\\
\alexp{partialNext!}: & \% $\to$ \altype{Partial} T & get the next element\\
\end{exports}
#endif

Generator(T:Type): with {
-- partialNext! is implemented in an extend in sal_partial.as
-- while concat and interlacing are implemented in an extend in sal_list.as
#if ALDOC
\alpage{concat}
\Usage{\name~[$g_1,\dots,g_n$]}
\Signature{\altype{List} \%}{\%}
\Params{ $g_i$ & \% & generators\\ }
\Descr{Given that each $g_i$ generates $t_{i1},t_{i2},\dots$,
returns a generator that generates
$$t_{11},t_{12},\dots,t_{21},t_{22},\dots,t_{n1},t_{n2},\dots$$ and that stops
after $g_n$ stops.}
#endif
#if ALDOC
\alpage{interlacing}
\Usage{\name~[$g_1,\dots,g_n$]}
\Signature{\altype{List} \%}{\%}
\Params{ $g_i$ & \% & generators\\ }
\Descr{Given that each $g_i$ generates $t_{i1},t_{i2},\dots$,
returns a generator that generates
$$t_{11},t_{21},\dots,t_{n1},t_{12},t_{22},\dots,t_{n2},\dots$$ and that stops
whenever any of the $g_i$ stops.}
#endif
	next!: % -> T;
#if ALDOC
\alpage{next!,partialNext!}
\altarget{next!}
\altarget{partialNext!}
\Usage{next!~g\\ partialNext!~g}
\Signatures{
next!: & \% $\to$ T\\
partialNext!: & \% $\to$ \altype{Partial} T\\
}
\Params{ {\em g} & \% & a generator\\ }
\Descr{next!(g) and partialNext!(g) both return the next element produced by
\emph{g}, updating \emph{g}.
If \emph{g} is empty, then next!(g) throws the exception
\altype{GeneratorException}, while partialNext!(g) returns \failed{}.}
#endif
} == add {
	next!(g:%):T == {
		for x in (g pretend Generator T) repeat return x;
		throw GeneratorException;
	}
}

#if ALDOC
\thistype{GeneratorException}
\History{Manuel Bronstein}{13/12/99}{created}
\Usage{
throw \this\\
try \dots catch E in \{ E has \altype{GeneratorExceptionType} $=>$ \dots \}
}
\Descr{
\this~is an exception type thrown by stepping through an empty generator.
}
#endif
GeneratorException: GeneratorExceptionType == add;

#if ALDOC
\thistype{GeneratorExceptionType}
\History{Manuel Bronstein}{13/12/99}{created}
\Usage{\this: Category}
\Descr{\this~is the category of exceptions thrown by generators.}
#endif
define GeneratorExceptionType:Category == with;

