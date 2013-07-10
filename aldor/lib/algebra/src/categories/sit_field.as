------------------------------- sit_field.as ----------------------------------
-- Copyright (c) Manuel Bronstein 1994
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994-97
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{Field}
\History{Manuel Bronstein}{22/11/94}{created}
\Usage{\this: Category}
\Descr{\this~is the category of commutative fields.}
\begin{exports}
\category{\altype{EuclideanDomain}}\\
\category{\altype{Group}}\\
\end{exports}
#endif

define Field: Category == Join(EuclideanDomain, Group) with {
	default {
		canonicalUnitNormal?:Boolean		== true;
		exactQuotient(a:%, b:%):Partial %	== [a / b];
		euclideanSize(a:%):Integer		== 0;
		unit?(x:%):Boolean			== true;
		(a:%) quo (b:%):%			== a / b;
		(a:%) rem (b:%):%			== 0;
		gcd(a:%, b:%):%		== { zero? a and zero? b => 0; 1 }

		-- TEMPORARY: THOSE DEFAULTS CANNOT BE DEFINED IN sit_euclid.as
		-- AS LONG AS THE COMPILER DOES EARLY-BINDING IN OTHER DEFAULTS
		divide!(a:%, b:%, q:%):(%, %)		== (a / b, 0);
		remainder!(a:%, b:%):%			== 0;

		-- returns (y, u, u^{-1}) s.t. x = u y
		unitNormal(x:%):(%,%,%) == { zero? x => (0,1,1); (1,x,inv x) };

		(a:%)^(n:Integer):% == {
			import from BinaryPowering(%, Integer);
			n >= 0 => binaryExponentiation(a, n);
			inv binaryExponentiation(a, -n);
		}
	}
}

