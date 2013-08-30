----------------------------- sal_random.as -------------------------------
--
-- This file provides random number generators
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli (c) INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

macro Z == MachineInteger;

extend MachineInteger: with {} == add {
	random():% == randomInteger()$RandomNumberGenerator;
	-- TEMPORARY: BUG1220, random(n) bound early to never!
	random(n:Z):% == random() mod n;
}

#if ALDOC
\thistype{RandomNumberGenerator}
\History{Manuel Bronstein}{15/10/98}{moved from sumit to salli and adapted}
\History{Manuel Bronstein}{9/02/2000}{added 32-bit generators}
\Usage{import from \this}
\Descr{\this~provides independent pseudo-random number generators.}
\begin{exports}
\alexp{apply}: & \% $\to$ Z & generate a random number\\
\alexp{generator}: & \% $\to$ \altype{Generator} Z & generate random numbers\\
\alexp{max}: & \% $\to$ Z & largest number that can be generated\\
\alexp{min}: & \% $\to$ Z & smallest number that can be generated\\
\alexp{numberOfGenerators}: & Z & number of predefined generators\\
\alexp{randomGenerator}: & () $\to$ \% & get a generator\\
\alexp{randomGenerator}: & Z $\to$ \% & get a generator\\
\alexp{randomGenerator}: & (Z, Z) $\to$ \% & get a bounded generator\\
\alexp{randomGenerator}: & (Z, Z, Z) $\to$ \% & get a bounded generator\\
\alexp{randomInteger}: & () $\to$ Z & generate a random number\\
\alexp{seed}: & (\%, Z) $\to$ Z & set the seed\\
\end{exports}
\begin{aswhere}
Z &==& \altype{MachineInteger}\\
\end{aswhere}
#endif

RandomNumberGenerator: with {
	apply: % -> Z;
#if ALDOC
\alpage{apply}
\Usage{ \name~r\\ r() }
\Signature{\%}{\altype{MachineInteger}}
\Params{ {\em r} & \% & a pseudo-random number generator\\ }
\Retval{Returns a pseudo-random integer.}
#endif
	generator: % -> Generator Z;
#if ALDOC
\alpage{generator}
\Usage{ for n in r repeat \{ \dots \}\\ for n in \name~r repeat \{ \dots \} }
\Signature{\%}{\altype{Generator} \altype{MachineInteger}}
\Params{ {\em r} & \% & a pseudo-random number generator\\ }
\Descr{This functions allows a for-loop that generates infinitely many
pseudo-random numbers.}
\begin{asex}
The following code computes the number of tries it takes for
a random generator to generate a multiple of 10:
\begin{ttyout}
multiple10():MachineInteger == {
    r := randomGenerator();
    for n in r for tries in 1.. repeat {
		zero?(n rem 10) => return tries;
    }
    never;
}
\end{ttyout}
\end{asex}
#endif
	max: % -> Z;
	min: % -> Z;
#if ALDOC
\alpage{max,min}
\altarget{max}
\altarget{min}
\Usage{max~r\\min~r}
\Signature{\%}{\altype{MachineInteger}}
\Params{ {\em r} & \% & a pseudo-random number generator\\ }
\Retval{max(r) and min(r) return respectively the largest and smallest
random integers that r can generate.}
#endif
	numberOfGenerators: Z;
#if ALDOC
\alpage{numberOfGenerators}
\Usage{\name}
\alconstant{\altype{MachineInteger}}
\Retval{Returns the number of independent generators provided.}
#endif
	randomGenerator: (n:Z == 0) -> %;
	randomGenerator: (Z, Z, n:Z == 0) -> %;
#if ALDOC
\alpage{randomGenerator}
\Usage{ \name()\\ \name~n\\ \name(a, b)\\ \name(a, b, n) }
\Signatures{
\name: () $\to$ \%\\
\name: \altype{MachineInteger} $\to$ \%\\
\name: (\altype{MachineInteger}, \altype{MachineInteger}) $\to$ \%\\
\name:
(\altype{MachineInteger}, \altype{MachineInteger}, \altype{MachineInteger})
$\to$ \%\\
}
\Params{
{\em a, b} & \altype{MachineInteger} & bounds for the generator\\
{\em n} & \altype{MachineInteger} & identifier for the generator (optional)\\
}
\Retval{
\name() returns a pseudo-random generator, while
\name(a, b) returns a pseudo-random number generator that
generates numbers between a and b inclusive.
If the optional argument n is given, then the $\sth{n}$ independent
generator is returned, which allows for several independent sources
of random numbers.}
\alseealso{\alexp{numberOfGenerators}}
#endif
	randomInteger: () -> Z;
#if ALDOC
\alpage{randomInteger}
\Usage{\name()}
\Signature{()}{\altype{MachineInteger}}
\Retval{Returns a pseudo-random integer.}
#endif
	seed: (%, Z) -> Z;
#if ALDOC
\alpage{seed}
\Usage{\name(r, s)}
\Signature{(\%, \altype{MachineInteger})}{\%}
\Params{
{\em r} & \% & a pseudo-random number generator\\
{\em s} & \altype{MachineInteger} & a nonzero seed\\
}
\Descr{Sets the seed of r to s and returns s. This is useful when a
reproducible pseudo-random sequence is desired. If an unseeded
generator is called, then it seeds itself from the system clock
the first time it is used, so the sequence is not reproducible. Note that
setting the seed to 0 causes the generator to generate an infinite sequence
of 0.}
#endif
} == add {
	Rep == Record(modulus:Z, multiplier:Z, last:Z, min:Z, sz:Z);

	local b64?:Boolean == { import from Z; bytes = 8 }

	-- last = -1 indicates unitialized yet, so use time for seed
	-- The 32-bit moduli and multipliers are taken from:
	--  Fishman & Moore, An exhaustive analysis of multiplicative
	--         congruential random number generators with modulus 2^31 - 1,
	--         SIAM J.Sci.Stat.Computation 7 (1986), 24-45. 
	-- The 64-bit moduli and multipliers are taken from:
	-- Krian & Goyal, Random number generation and testing,
	--                MTN 1, No.1, Spring 1994, 32-37.

	-- TEMPORARY: BAD CONSTANTS IN FOAM-FILE (BUG 1316)
	local mod64():Z == {
		n := bytes;
		m := n quo 2;
		shift(465, 31+n-m-m) + 1420103669;	-- 999999999989
	}
	local mult640():Z == {
		n := bytes;
		m := n quo 2;
		shift(199, 31+n-m-m) + 70423129;	-- 427419669081
	}
	local mult641():Z == {
		n := bytes;
		m := n quo 2;
		shift(347, 31+n-m-m) + 403211568;	-- 745580037424
	}

	local rand0:% == {
		import from Rep, Z;
		-- TEMPORARY: BAD CONSTANTS IN FOAM-FILE (BUG 1316)
		-- b64? =>per [999999999989, 427419669081, -1, 0, 999999999989];
		b64? => per [mod64(), mult640(), -1, 0, mod64()];
		per [2147483647, 950706376, -1, 0, 2147483647];
	}

	local rand1:% == {
		import from Rep, Z;
		-- TEMPORARY: BAD CONSTANTS IN FOAM-FILE (BUG 1316)
		-- b64? =>per [999999999989, 745580037424, -1, 0, 999999999989];
		b64? => per [mod64(), mult641(), -1, 0, mod64()];
		per [2147483647, 62089911, -1, 0, 2147483647];
	}

	numberOfGenerators:Z		== 2;
	randomGenerator(n:Z):%		== { odd? n => rand1; rand0; }
	randomInteger():Z		== rand0();
	seed(r:%, n:Z):Z		== setlast!(r, abs n);
	min(r:%):Z			== { import from Rep; rep(r).min; }
	max(r:%):Z			== min(r) + prev size r;
	local last(r:%):Z		== { import from Rep; rep(r).last; }
	local mult(r:%):Z		== { import from Rep; rep(r).multiplier}
	local modulus(r:%):Z		== { import from Rep; rep(r).modulus; }
	local size(r:%):Z		== { import from Rep; rep(r).sz; }
	local setlast!(r:%, n:Z):Z	== { import from Rep; rep(r).last := n }
	local seeded?(r:%):Boolean	== { import from Z; last(r) >= 0; }
	generator(r:%):Generator Z	== generate { repeat yield r(); }

	apply(r:%):Z == {
		import { randomSeed: () -> Z } from Foreign C;
		x := { seeded? r => last r; abs randomSeed() }
		setlast!(r, mod_*(mult r, x, modulus r));
		min(r) + (last(r) rem size(r));
	}

	randomGenerator(a:Z, b:Z, n:Z):% == {
		import from Rep;
		r := rep randomGenerator n;
		per [r.modulus, r.multiplier, r.last, a, next(b - a)];
	}
}

#if ALDORTEST
---------------------- test sal_random.as --------------------------
#include "aldor"
#include "aldortest"

macro I == MachineInteger;

local random():Boolean == { import from I; random 100; }

local random(m:I):Boolean == {
	import from Array I, RandomNumberGenerator;
	r := randomGenerator();
	seed(r, 1);
	a := new(m, 0);
	for n in r for i in 0..prev m repeat a.i := n;
	seed(r, 1);
	for i in 0..prev m repeat { a.i ~= r() => return false; }
	true;
}

stderr << "Testing sal__random..." << newline;
aldorTest("random", random);
stderr << newline;
#endif

