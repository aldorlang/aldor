#include "algebra"

macro {
	SingleInteger == MachineInteger;
}

+++ `SourceOfPrimes' asserts that we have a source of prime
+++ elements given by the `getPrime' and `nextPrime' operations
+++ such that the following holds: if `nextPrime(x)' returns  `[y]'
+++ then `y' is a prime such that `sizeLess?(x,x*y)' holds
+++ and `associates?(x,y)' is `false'. For the purpose of [1]
+++ this source of primes needs to be `large enough'. However we
+++ do not want to force every domain satisfying `SourceOfPrimes'
+++ to implement a `prime?' operation which will be able to conclude
+++ in any case. This is why some of the operations exported here
+++ are `partial operations'.
+++ Date Created: 04/06/99
+++ Date Last Update: 05/11/99
+++ Keywords: euclidean domain,  modular algorithms.
+++ References:
+++ [1] `On the Genericity of the Modular Polynomial GCD Algorithm'
+++      by E.Kaltofen and M.Monagan
+++      in proceedings ISSAC'99.
                                                                                
define SourceOfPrimes: Category ==  CommutativeRing with {
        prime?: % -> Partial(Boolean);
          ++ `prime?(x)' returns `[true] if `u' can be checked to
          ++ irreducible, returns `[false]' if `u' can be checked to
          ++ reducible and returns `failed()' otherwise.
	prime?: % -> Boolean;
	++ `prime(x)' returns `b' if `prime?(x)' returns `[b]'
	++ otherwise produces an error
        getPrime: () -> Partial(%);
          ++ `getPrime()' returns a prime.
        nextPrime: % -> Partial(%);
          ++ `nextPrime(x)' returns  `[y]' such that `y' is a prime,
          ++ `sizeLess?(x,x*y)' holds and `associates?(x,y)' is `false',
          ++ if any from `our source of primes', otherwise returns `failed()'.
	if % has EuclideanDomain then {
            getPrimeOfSize: SingleInteger -> Partial(%);
            ++ `getPrimeOfSize(d)' returns either `[x]' such that `x' is
            ++ a prime of euclidean size not smaller that `+(d::Integer)'
            ++ or `failed()' if such an element could not be computed.
	}
	default {
		prime?(x: %): Boolean == {
			b?: Partial(Boolean) := prime?(x);
			assert(not failed? b?);
			retract(b?);
		}
	}

}

#if ALDOC
\thistype{SourceOfPrimes}
\History{Marc Moreno Maza}{1999}{created}
\Usage{\this: Category}
\Descr{\this~is the category  of domains supporting a (partial) primality test
       and a source of primes (which may finite or infinite).}
\begin{exports}
\category{\altype{CommutativeRing}} \\
\alexp{prime?}: & \% $\to$ \astype{Partial} Boolean & Primality test \\
\alexp{prime?}: & \% $\to$  Boolean & Primality test \\
\alexp{getPrime}: () $\to$ \astype{Partial} \% & Prime source seed \\
\alexp{nextPrime}: () $\to$ \astype{Partial} \% & Prime source next \\
\end{exports}
\begin{exports}[if \% has \altype{EuclideanDomain}  then]
\alexp{getPrimeOfSize}: () $\to$ \astype{Partial} \% & Prime source seed \\
\end{exports}


#endif
