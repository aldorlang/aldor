-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp -laxllib

-- This file ensures that we can distinguish between:
--	coerce:		%$M -> %$P(%$M)
--	coerce:		%$P(%$M) -> %$M
-- when importing from P(%$M) in the body of M.

#library LangLib "lang.ao"
import from LangLib;

P(T: Type): with {
	coerce:  T -> %;
	coerce:  % -> T;
}
== add {
	coerce(v: T): %                  == v pretend %;
	coerce(p: %): T                  == p pretend T;
}

M : with {
	inv:	P % -> %;
}
== add {
	inv (x: P %) : % == x::%;
}
