------------------------------- sit_modpgcd.as --------------------------------
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

macro {
	SI  == MachineInteger;
	ARR == PrimitiveArray;
}

#if ALDOC
\thistype{ModulopUnivariateGcd}
\History{Manuel Bronstein}{24/7/98}{created}
\Usage{import from \this}
\Descr{\this~provides an implementation of an inplace gcd for polynomials
modulo a machine prime.}
\begin{exports}
\asexp{gcd!}: & (ARR Z, Z, ARR Z, Z, Z, Z) $\to$ (ARR Z, Z, Z) & in-place gcd\\
\end{exports}
\begin{aswhere}
Z &==& \astype{MachineInteger}\\
ARR &==& \astype{PrimitiveArray}\\
\end{aswhere}
#endif

ModulopUnivariateGcd: with {
	gcd!:(ARR SI, SI, ARR SI, SI, SI, SI) -> (ARR SI, SI, SI);
	gcd!:(ARR SI, SI, ARR SI, SI, SI, SI, ARR SI, ARR SI) -> (ARR SI,SI,SI);
#if ALDOC
\aspage{gcd!}
\Usage{\name(a, n, b, m, $\alpha$, p)\\
\name(a, n, b, m, $\alpha$, p, $[l_1,\dots,l_{p-1}], [e_1,\dots,e_{p-1}]$)}
\Signatures{
\name: (ARR Z, Z, ARR Z, Z, Z) $\to$ (ARR Z, Z, Z)\\
\name: (ARR Z, Z, ARR Z, Z, Z, ARR Z, ARR Z) $\to$ (ARR Z, Z, Z)\\
}
\begin{aswhere}
Z &==& \astype{MachineInteger}\\
ARR &==& \astype{Array}\\
\end{aswhere}
\Params{
{\em a,b} & \astype{PrimitiveArray} \astype{MachineInteger} &
polynomials modulo p\\
{\em n,m} & \astype{MachineInteger} & their degrees\\
$\alpha$ & \astype{MachineInteger} & a leading coefficient\\
{\em p} & \astype{MachineInteger} & a prime\\
$[l_0,\dots,l_{p-1}]$ & \astype{PrimitiveArray} \astype{MachineInteger} &
log table\\
$[e_0,\dots,e_{p-1}]$ & \astype{PrimitiveArray} \astype{MachineInteger} &
exp table\\
}
\Descr{
Given 2 polynomials stored in $a$ and $b$ of degrees $n$ and $m$ respectively,
computes a $\gcd(a,b)$ in $F_p[x]$ with leading coefficient $\alpha$.
Requires $n \ge m$ and that the polynomials are stored leading coefficient
first. Returns the degree of the gcd and its starting index in the array.
}
\Remarks{
The result can be stored in either $a$ or $b$, so the function also returns
the appropriate array. Note that both $a$ and $b$ are destroyed.
The last 2 optional arrays are such that $g^{l_i} = i$ and $e_i = g^i$
where $g$ is a primitive root for the multiplicative group modulo $p$.
They are used for fast multiplication if provided.
}
#endif
} == add {
	local maxhalf:SI	== maxPrime$HalfWordSizePrimes;
	local maxint:SI		== max$SI;

#if ASTRACE
	local prt(str:String, a:ARR SI, n:SI, s:SI):() == {
		import from TextWriter, WriterManipulator;
		stderr << str << " = ";
		for i in 0..n repeat stderr << a(s+i) << " ";
		stderr << endnl;
	}
#else
	local prt(str:String, a:ARR SI, n:SI, s:SI):() == {};
#endif

	gcd!(a:ARR SI, n:SI, b:ARR SI, m:SI, lc:SI, p:SI):(ARR SI, SI, SI) == {
		import from String;
		TRACE("gcd!:p = ", p);
		prt("gcd!:a", a, n, 0);
		prt("gcd!:b", b, m, 0);
		assert(n >= m); assert(p > 1);
		p > maxhalf => fullgcd!(a, n, b, m, lc, p);
		-- from normalized inputs, the first k loops cannot overflow
		k := prev(maxint quo (prev(p) * prev(p)));
		TRACE("gcd!:k = ", k);
		k > 10 => halfgcd!(a, n, b, m, lc, k, p);
		halfgcd!(a, n, b, m, lc, p);
	}

	gcd!(a:ARR SI, n:SI, b:ARR SI, m:SI, lc:SI, p:SI, log:ARR SI,
		exp:ARR SI):(ARR SI, SI, SI) == {
			import from String;
			prt("gcd!:a", a, n, 0);
			prt("gcd!:b", b, m, 0);
			prt("gcd!:log", log, p-2, 0);
			prt("gcd!:exp", log, p-2, 0);
			assert(n >= m); assert(p > 1);
			gcd!(a, n, b, m, lc, p, log, exp, prev p);
	}

-- Those 3 files contain similar code with 3 different product functions
-- (the product is not passed as parameter because the function-call
--  overhead is too high, and this would prevent inlining)
#include "sit_fullgcd.as"
#include "sit_halfgcd.as"
#include "sit_loggcd.as"
}
