---------------------------- sal_cklist.as ------------------------------------
--
-- This file defines basic lists that check their arguments
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro {
	Ch == Character;
	Z  == MachineInteger;
}

#if ALDOC
\thistype{CheckingList}
\History{Manuel Bronstein}{10/7/2001}{created}
\Usage{import from \this~T}
\Params{{\em T} & Type & the type of the list entries\\}
\Descr{\this~provides lists of entries of type $T$, $1$-indexed and with
bound checking.}
\begin{exports}
\category{\altype{ListType} T}\\
\end{exports}
#endif

CheckingList(T:Type): ListType T == List T add {
	Rep == List T;
	import from Rep;

	setFirst!(l:%, t:T):T == {
		empty? l => throw ListException;
		setFirst!(rep l, t);
	}

	setRest!(l:%, s:%):% == {
		empty? l => throw ListException;
		per setRest!(rep l, rep s);
	}

	delete!(l:%, n:Z):% == {
		n < 1 => throw ListException;
		n = 1 => rest l;
		ll := l + prev prev n;
		setRest!(ll, rest rest ll);
		l;
	}

	set!(l:%, n:Z, x:T):T == {
		n < 1 => throw ListException;
		setFirst!(l + prev n, x);
	}

	if T has PrimitiveType then {
		linearSearch(t:T, l:%, start:Z):(Boolean, Z, T) == {
			start < 1 => throw ListException;
			(l, n) := find(t, l + prev start);
			empty? l => (false, n, t);
			(true, n, first l);
		}
	}

	first(l:%):T == {
		empty? l => throw ListException;
		first rep l;
	}

	rest(l:%):% == {
		empty? l => throw ListException;
		per rest rep l;
	}

	(l:%) + (n:Z):% == {
		n < 0 => throw ListException;
		for i in 1..n repeat l := rest l;
		l;
	}

	-- MUST REDEFINE LOCALLY SINCE apply FROM ADD CHAIN IS NONCHECKING
	apply(l:%, n:Z):T == first(l + prev n);		-- 1-indexed
}

