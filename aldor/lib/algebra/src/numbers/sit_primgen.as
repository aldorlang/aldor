---------------------------- sit_primgen.as ---------------------------------
--
-- Hard-coded primes of various machine-dependent sizes
--
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
-----------------------------------------------------------------------------

#include "algebra"

#if ALDOC
\thistype{SmallPrimes}
\History{Manuel Bronstein}{8/6/95}{created}
\Usage{import from \this}
\Descr{\this~implements functionalities to obtain and manipulate
small odd primes. Only a specific set $\cal P$ of small primes is available.}
\begin{exports}
\category{\astype{PrimeCollection}}\\
\end{exports}
#endif

SmallPrimes: PrimeCollection == FixedPrimes PrimesSmall;

#if ALDOC
\thistype{LazyHalfWordSizePrimes}
\History{Manuel Bronstein}{16/12/98}{created}
\Usage{import from \this}
\Descr{\this~implements functionalities to obtain and manipulate
half-word-size odd primes for lazy algorithms. Those primes are a few bits
less that half-word-size, which allows for accumulation before reduction.
Only a specific set $\cal P$ of half-word-size primes is available.}
\begin{exports}
\category{\astype{PrimeCollection}}\\
\end{exports}
#endif

LazyHalfWordSizePrimes: PrimeCollection == FixedPrimes2(Primes13, Primes27);

#if ALDOC
\thistype{HalfWordSizePrimes}
\History{Manuel Bronstein}{26/4/96}{created}
\Usage{import from \this}
\Descr{\this~implements functionalities to obtain and manipulate
half-word-size odd primes. Only a specific set $\cal P$ of half-word-size
primes is available.}
\begin{exports}
\category{\astype{PrimeCollection}}\\
\end{exports}
#endif

HalfWordSizePrimes: PrimeCollection == FixedPrimes2(Primes15, Primes31);

#if ALDOC
\thistype{WordSizePrimes}
\History{Manuel Bronstein}{30/6/95}{created}
\History{Manuel Bronstein}{26/4/96}{made it independent of machine word-size}
\Usage{import from \this}
\Descr{\this~implements functionalities to obtain and manipulate word-size
primes. Only a specific set $\cal P$ of word-size primes is available.}
\begin{exports}
\category{\astype{PrimeCollection}}\\
\end{exports}
#endif

WordSizePrimes: PrimeCollection == FixedPrimes2(Primes31, Primes63);
