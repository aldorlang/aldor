---------------------------- sal_bsearch.as -------------------------------
--
-- This file provides a fairly generic binary search for structures or int's.
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli (c) INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

#if ALDOC
\thistype{BinarySearch}
\History{Manuel Bronstein}{28/9/98}{moved from sumit to salli and adapted}
\Usage{import from \this(R, S)}
\Descr{\this(R, S) provides a general version of binary search.}
\Params{
{\em R} & \altype{IntegerType} & The space being searched\\
{\em S} & \altype{TotallyOrderedType} & The target values being searched for\\
}
\begin{exports}
\alexp{binarySearch}: & (S, R $\to$ S, R, R) $\to$ (\altype{Boolean}, R) &
binary search\\
\end{exports}
#endif

BinarySearch(R:IntegerType, S:TotallyOrderedType): with {
	binarySearch: (S, R -> S, R, R) -> (Boolean, R);
#if ALDOC
\alpage{binarySearch}
\Usage{\name(s, f, a, b)}
\Signature{(S, R $\to$ S, R, R)}{(\altype{Boolean}, R)}
\Params{
{\em s} & S & The value to search for\\
{\em f} & R $\to$ S & A monotonic increasing function\\
{\em a} & R & The left end of the interval to search\\
{\em b} & R & The right end of the interval to search\\
}
\Retval{Returns (found?, r) such that
$s = f(r)$ if found?~is \true. Otherwise, found?~is \false~and:
\begin{itemize}
\item ~if $r < a$ then $s < f(a)$ or $b < a$;
\item ~if $r \ge b$ then $s > f(b)$;
\item ~if $a \le r < b$, then $f(r) < s < f(r+1)$;
\end{itemize}
}
#endif
} == add {
	binarySearch(s:S, f:R -> S, a:R, b:R):(Boolean, R) == {
		b < a or s < (fa := f a) => (false, prev a);
		fb := f b;
		two:R := 1 + 1;
		while fb < fa repeat {	-- only happens on overflow for f b
			b := (a + b) quo two;
			fb := f b;
		}
		s > fb => (false, b);
		s = fa => (true, a);
		s = fb => (true, b);
		while (m := (a + b) quo two) > a and m < b repeat {
			fm := f m;
			while fm < fa repeat {	-- overflow for f m
				m := (a + m) quo two;
				fm := f m;
			}
			s = fm => return (true, m);
			if s < fm then b := m; else a := m;
		}
		(false, a);
	}
}
