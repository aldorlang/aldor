--------------------------- sit_spf0.as --------------------------------
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

macro Z == MachineInteger;

#if ALDOC
\thistype{SmallPrimeField0}
\History{Manuel Bronstein}{6/7/94}{created}
\Usage{import from \this~p}
\Params{ {\em p} & \altype{MachineInteger} & The characteristic\\ }
\Descr{\this~p is an internal implementation of the finite field
$\ZZ / p \ZZ$ where $p\in \ZZ$ is a word--size prime.
Use \altype{SmallPrimeField} instead.}
\begin{exports}
\category{\altype{SmallPrimeFieldCategory0}}\\
\end{exports}
#endif

SmallPrimeField0(p:Z): SmallPrimeFieldCategory0 == {
        -- sanity checks on the parameters
        assert(p > 1);

	add {
	Rep == Z;

	import from Rep;

	local maxhalf:Z				== maxPrime$HalfWordSizePrimes;
	0:%					== per 0;
	1:%					== per 1;
	local pp:Integer			== p::Integer;
	local p1:Integer			== prev pp;
	characteristic:Integer			== pp;
	(a:%) = (b:%):Boolean			== rep a = rep b;
	lift(a:%):Integer			== rep(a)::Integer;
	coerce(a:Integer):%			== per machine(a mod pp);
	(a:%) / (b:%):%				== per mod_/(rep a, rep b, p);
	inv(a:%):%				== per modInverse(rep a, p);
	-(a:%):%			== { zero? a => a; per(p - rep a); }

	cutoff(t:Z):Z == {
		t = CUTOFF__KARAMULT => 20;
		t = CUTOFF__FFTMULT => 400;
		-1;
	}

	if p > maxhalf then {
		(a:%) * (b:%):%			== per mod_*(rep a, rep b, p);
		(a:%) + (b:%):%			== per mod_+(rep a, rep b, p);
	} else { -- we know that +/* on rep cannot overflow
		(a:%) * (b:%):%			== per((rep(a) * rep(b)) rem p);
		(a:%) + (b:%):%	== {
			p <= (c := rep a + rep b) => per(c - p);
			per c;
		}
	}

	(a:%)^(n:Integer):% == {
		zero? a => 0;
		one? a => a;
		m:Z := machine(n mod p1);
		per mod_^(rep a, m, p);
	}

	-- TEMPORARY: BUG 1181
	#:Integer		== characteristic$%;

	-- THOSE ARE BETTER THAN THE CORRESPONDING CATEGORY DEFAULTS
	zero?(a:%):Boolean	== zero? rep a;
	one?(a:%):Boolean	== one? rep a;
	add!(a:%, b:%):%	== a + b;
	times!(a:%, b:%):%	== a * b;
	coerce(a:Z):%		== per(a mod p);
	machine(a:%):Z		== rep a;
	(a:%) - (b:%):%	== { (c := rep a - rep b) < 0 => per(c + p); per c }

	-- TEMPORARY (BUG1182) DEFAULTS DON'T INLINE WELL
	pthPower(a:%):%		== a;
	pthPower!(a:%):%	== a;
	pthRoot(a:%):%		== a;
	pthRoot!(a:%):%		== a;
	minus!(a:%):%		== -a;
	}
}
