---------------------------- sit_prmtabl.as ---------------------------------
--
-- Interface to hard-coded tables of primes
--
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"

macro Z	== MachineInteger;

define PrimeTable: Category == with {
	primes: Array Z;	-- the actual primes
	fourier: Array Z;	-- fourier primes
	roots: Array Z;		-- either primitive roots for primes
				-- or primitive roots of 1 for fourier primes
};

#if ALDOC
\thistype{PrimeCollection}
\History{Manuel Bronstein}{26/4/96}{created}
\Usage{\this: Category}
\Descr{\this~is the category of collections of primes with various
properties and various sizes.}
\begin{exports}
\alexp{allPrimes}: & () $\to$ \altype{Generator} Z & Generate all the primes\\
\alexp{fourierPrime}: & Z $\to$ (Z, Z) & Fourier prime\\
\alexp{maxPrime}: & $\to$ Z & Largest prime\\
\alexp{nextPrime:} & Z $\to$ Z & First prime above a given number\\
\alexp{previousPrime:} & Z $\to$ Z & First prime below a given number\\
\alexp{primeInCollection?:} & Z $\to$ \altype{Boolean} & Check a prime\\
\alexp{primRoot:} & Z $\to$ Z & Modular primitive root\\
\alexp{randomPrime:} & () $\to$ Z & Random prime\\
\end{exports}
\begin{alwhere}
Z & == & \altype{MachineInteger}\\
\end{alwhere}
#endif

define PrimeCollection: Category == with {
	allPrimes: () -> Generator Z;
#if ALDOC
\alpage{allPrimes}
\Usage{ for p in \name() repeat \{ \dots \} }
\Signature{()}{\altype{Generator} \altype{MachineInteger}}
\Descr{This function allows a loop to iterate over all the primes
provided by the collection.}
#endif
	fourierPrime: Z -> (Z, Z);
#if ALDOC
\alpage{fourierPrime}
\Usage{\name~n}
\Signature{\altype{MachineInteger}}
{(\altype{MachineInteger},\altype{MachineInteger})}
\Retval{Returns $(p,\omega)$ such that $p$ is a prime of the form
$p = 2^n k + 1$ with $k$ odd, and $\omega$ is a primitive $\sth{2^n}$
root of unity in $\mathbbm{F}_p$. Returns $(0,0)$ if $n$ is too large.}
#endif
	maxPrime: Z;
#if ALDOC
\alpage{maxPrime}
\Usage{\name}
\alconstant{\altype{MachineInteger}}
\Retval{Returns the largest prime in the collection.}
#endif
	nextPrime: Z -> Z;
#if ALDOC
\alpage{nextPrime}
\Usage{\name~n}
\Signature{\altype{MachineInteger}}{\altype{MachineInteger}}
\Params{ {\em n} & \altype{MachineInteger} & An integer\\ }
\Retval{Returns the smallest prime $p$ in the collection with $n < p$,
$0$ if there are none.}
\alseealso{previousPrime(\this), randomPrime(\this)}
#endif
	previousPrime: Z -> Z;
#if ALDOC
\alpage{previousPrime}
\Usage{\name~n}
\Signature{\altype{MachineInteger}}{\altype{MachineInteger}}
\Params{ {\em n} & \altype{MachineInteger} & An integer\\ }
\Retval{Returns the largest prime $p$ in the collection with $n > p$,
$0$ if there are none.}
\alseealso{nextPrime(\this), randomPrime(\this)}
#endif
	primeInCollection?: Z -> Boolean;
#if ALDOC
\alpage{primeInCollection?}
\Usage{\name~n}
\Signature{\altype{MachineInteger}}{\altype{Boolean}}
\Params{ {\em n} & \altype{MachineInteger} & An integer\\ }
\Retval{Returns \true{} if \emph{n} is a prime in the collection,
\false{} otherwise (\emph{n} could still be prime in that case).}
#endif
	primRoot: Z -> Z;
#if ALDOC
\alpage{primRoot}
\Usage{\name~p}
\Signature{\altype{MachineInteger}}{\altype{MachineInteger}}
\Params{ {\em p} & \altype{MachineInteger} & A prime\\ }
\Retval{Returns a generator of the multiplicative group
$(\ZZ/p \ZZ)^\ast$ or $0$ if $p$ is not a prime in the table,
or is no primitive root is stored.}
\alseealso{primitiveRoot(\altype{PrimitiveRoots})}
#endif
	randomPrime: () -> Z;
#if ALDOC
\alpage{randomPrime}
\Usage{\name()}
\Signature{()}{\altype{MachineInteger}}
\Retval{Returns a random prime in the collection.}
\alseealso{nextPrime(\this), previousPrime(\this)}
#endif
}

FixedPrimes(T:PrimeTable): PrimeCollection == add {
	local primes:Array Z	== primes$T;
	local fourier:Array Z	== fourier$T;
	local roots:Array Z	== roots$T;
	local nprimes:Z		== #primes;
	local nfourier:Z	== #fourier;
	local prim?:Boolean	== nprimes = #roots;
	maxPrime:Z		== primes(prev nprimes);
	randomPrime():Z		== primes(random()$Z mod nprimes);
	allPrimes():Generator Z	== generator primes;

	fourierPrime(n:Z):(Z, Z) == {
		n < 1 or n > nfourier => (0, 0);
		(fourier(prev n), { prim? => 0; roots(prev n) });
	}

	nextPrime(n:Z):Z == {
		(found?, index) := search n;
		oddPrime next index;
	}

	primRoot(n:Z):Z == {
		prim? => {
			(found?, index) := search n;
			found? => roots.index;
			0;
		}
		0;
	}

	previousPrime(n:Z):Z == {
		(found?, index) := search n;
		found? => oddPrime prev index;
		-- TEMPORARY: WORK-AROUND to a SALLI 0.1.12e binarySearch BUG
		-- index >= nprimes => maxPrime;
		oddPrime index;
	}

	-- check bounds and returns 0 if out of bounds
	local oddPrime(index:Z):Z == {
		TRACE("oddPrime: ", index);
		index < 0 or index >= nprimes => 0;
		primes.index;
	}

	primeInCollection?(n:Z):Boolean == {
		(found?, index) := search n;
		found?;
	}

	-- return (found?, i) such that:
	--   if found? is true, then n = wordprimes.i
	--   if found? is false, then:
	--	if 0 <= i < nprimes-1, then wordprimes.i < n < wordprimes(i+1)
	--	if i = nprimes-1 then wordprimes(nprimes - 1) < n
	local search(n:Z):(Boolean, Z) == {
		import from Array Z;
		binarySearch(n, primes);
	}
}

-- select T32 or T64 depending on the word-size of the machine
FixedPrimes2(T32:PrimeTable, T64:PrimeTable): PrimeCollection == {
	import from Z;

	local w:Z == bytes;

	-- TEMPORARY: WORKAROUND FOR BUG1167
	-- w = 4 => FixedPrimes T32;
	w = 4 => FixedPrimes T32 add;
	assert(w = 8);
	-- TEMPORARY: WORKAROUND FOR BUG1167
	-- FixedPrimes T64;
	FixedPrimes T64 add;
}

