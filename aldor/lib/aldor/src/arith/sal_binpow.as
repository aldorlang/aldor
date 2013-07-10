----------------------------- sal_binpow.as ----------------------------------
--
-- This file provides binary powering for arithmetic systems
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli ©INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

#if ALDOC
\thistype{BinaryPowering}
\History{Manuel Bronstein}{22/11/94}{created}
\History{Manuel Bronstein}{7/9/98}{added small characteristic method}
\Usage{import from \this(T, Z)}
\Params{
{\em T} & \altype{ArithmeticType} & An arithmetic system\\
{\em Z} & \altype{IntegerType} & An integer--like type\\
}
\Descr{\this~provides binary exponentiation of elements of $T$
with exponents in $Z$.}
\begin{exports}
\alexp{binaryExponentiation}: & (T, Z) $\to$ T & Binary powering\\
\alexp{binaryExponentiation!}: & (T, Z) $\to$ T & In--place binary powering\\
\end{exports}
#endif

BinaryPowering(T:ArithmeticType, Z:IntegerType): with {
	binaryExponentiation: (T, Z) -> T;
	binaryExponentiation!: (T, Z) -> T;
#if ALDOC
\alpage{binaryExponentiation}
\altarget{\name!}
\Usage{\name(a, n)\\ \name!(a, n)}
\Signature{(T, Z)}{T}
\Params{
{\em a} & T & The element to exponentiate\\
{\em n} & Z & The exponent\\
}
\Retval{Returns $a^n$. The exponent $n$ must be nonnegative.
When using \name!($a, n$),
the storage used by a and n is allowed
to be destroyed or reused, so a and n are lost after this call.}
\Remarks{A call to \name!($a, n$) may cause a and n to be destroyed,
so do not use it unless a and n have been locally allocated,
and are guaranteed not to share space
with other elements. Some functions are not necessarily copying their
arguments and can thus create memory aliases.}
#endif
} == add {
	local m1:MachineInteger			== { import from Z; machine(-1)}
	binaryExponentiation!(a:T, b:Z):T	== binPow!(1, a, b);

	if T has CopyableType and Z has CopyableType then {
		binaryExponentiation(a:T, b:Z):T == binPow!(1, copy a, copy b);
	}
	else {
		binaryExponentiation(a:T, b:Z):T == {
			assert(b >= 0);
			zero? a or one? a => a;
			u:T := 1;
			while b > 0 repeat {
				if bit?(b, 0) then {
					zero?(u := u * a) => return u;
				}
				-- don't use in-place shift if not CopyableType
				if (b := shift(b, m1)) > 0 then {
					one?(a := a * a) => return u;
				}
			}
			u;
		}
	}

	-- returns u a^n, trashes u, a and b
	local binPow!(u:T, a:T, b:Z):T == {
		assert(b >= 0);
		zero? a or one? a => a;
		while b > 0 repeat {
			if bit?(b, 0) then {
				zero?(u := times!(u, a)) => return u;
			}
			if (b := shift!(b, m1)) > 0 then {
				one?(a := times!(a, a)) => return u;
			}
		}
		u;
	}
}

