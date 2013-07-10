------------------------------- sit_power.as ----------------------------------
--
-- This file provides pth-powering for rings of finite characteristic
--
-- This type must be included inside "sit_charp.as" to be compiled,
-- but is in a separate file for documentation purposes.
--
-- Copyright (c) Manuel Bronstein 1994
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it (c) INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994-97
-----------------------------------------------------------------------------

#if ALDOC
\thistype{PthPowering}
\History{Manuel Bronstein}{22/11/94}{created}
\History{Manuel Bronstein}{7/9/98}{added small characteristic method}
\Usage{import from \this~R}
\Params{
{\em R} & \astype{FiniteCharacteristic} & A finite characteristic ring\\
}
\Descr{\this~provides efficient exponentiation of elements of $R$.}
\begin{exports}
\asexp{pExponentiation}: & (T, \astype{Integer}) $\to$ T & $\sth p$--powering\\
\asexp{pExponentiation!}:
& (T, \astype{Integer}) $\to$ T & In--place $\sth p$--powering\\
\end{exports}
#endif

PthPowering(R:FiniteCharacteristic): with {
	pExponentiation: (R, Z) -> R;
	pExponentiation!: (R, Z) -> R;
#if ALDOC
\aspage{pExponentiation}
\astarget{\name!}
\Usage{\name(a, n)\\ \name!(a, n)}
\Signature{(R, \astype{Integer})}{R}
\Params{
{\em a} & R & The element to exponentiate\\
{\em n} & \astype{Integer} & The exponent\\
}
\Retval{Returns $a^n$. The exponent $n$ must be nonnegative.
When using \name!($a, n$),
the storage used by a is allowed
to be destroyed or reused, so a is lost after this call.}
\Remarks{A call to \name!($a, n$) may cause a to be destroyed,
so do not use it unless a has been locally allocated,
and is guaranteed not to share space
with other elements. Some functions are not necessarily copying their
arguments and can thus create memory aliases.}
#endif
} == add {
	local char:Z			== characteristic$R;
	local small?:Boolean		== char < 10008;

	-- buffer.i := a^i for 0 < i < char
	local buffer:Array R == {
		import from MZ;
		small? => new machine char;
		empty;
	}

	if R has CopyableType then {
		pExponentiation(a:R, b:Z):R == pExponentiation!(copy a, b);
	}
	else {
		pExponentiation(a:R, b:Z):R == {
			import from BinaryPowering(R, Z);
			assert(b >= 0);
			zero? b => 1;
			zero? a or one? a => a;
			small? => {
				buffer.1 := a;
				cachedPower(a, b, 1);
			}
			u:R := 1;
			-- a^(p q + r) = (a^p)^q a^r where p == characteristic
			while b > 0 repeat {
				(b, r) := divide(b, char);
				if r > 0 then {
					u := u * binaryExponentiation(a, r);
					zero? u => return u;
				}
				if b > 0 then {
					one?(a := pthPower a) => return u;
				}
			}
			u;
		}

		-- for small p only, buffer.i = a^i for 1 <= i <= m
		-- destroys a and the buffer above m
		local cachedPower(a:R, n:Z, m:MZ):R == {
			assert(n > 0);
			-- a^(p q + r) = (a^q)^p a^r where p == characteristic
			(n, r) := divide(n, char);
			s := machine r;
			if s > m then {
				for i in m..prev s repeat
					buffer(next i) := a * buffer.i;
				m := s;
			}
			zero? n => buffer.s;
			a := pthPower cachedPower(a, n, m);
			zero? s => a;
			a * buffer.s;
		}
	}

	-- destroys a
	-- needs a conditional version for fields later (n < 0)
	pExponentiation!(a:R, b:Z):R == {
		import from BinaryPowering(R, Z);
		import from MZ;
		assert(b >= 0);
		zero? b => 1;
		zero? a or one? a => a;
		small? => {
			buffer.1 := a;
			cachedPower!(a, b, 1);
		}
		u:R := 1;
		while b > 0 repeat {
			-- a^(p q + r) = (a^p)^q a^r where p == characteristic
			(b, r) := divide(b, char);
			if r > 0 then {
				u := times!(u, binaryExponentiation(a, r));
				zero? u => return u;
			}
			if b > 0 then {
				one?(a := pthPower! a) => return u;
			}
		}
		u;
	}

	-- for small p only, buffer.i = a^i for 1 <= i <= m
	-- destroys a and the buffer above m
	local cachedPower!(a:R, n:Z, m:MZ):R == {
		assert(n > 0);
		-- a^(p q + r) = (a^q)^p a^r where p == characteristic
		(n, r) := divide(n, char);
		s := machine r;
		if s > m then {
			for i in m..prev s repeat buffer(next i) := a*buffer.i;
			m := s;
		}
		zero? n => buffer.s;
		a := pthPower! cachedPower!(a, n, m);
		zero? s => a;
		times!(a, buffer.s);
	}
}

