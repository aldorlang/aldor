-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#pile
#include "axllib.as"

macro
	NNI == Integer
	SNNI == SingleInteger
	NonNegativeInteger == Integer
	Vector == PrimitiveArray
	B  == Boolean
	I  == Integer
	SI == SingleInteger
	** == ^
	^= == ~=
	one? xxx == xxx = 1
	zero? xxx == xxx = 0
	empty() == nil
	IMOD == IntegerMod

import from I
resMod144: List I := [0,1,4,9,16,25,36,49,52,64,73,81,97,100,112,121]

powmod(p: I, q: I, n: I) : I == {
	import from IMOD n;
	lift((p::IMOD(n)) ^ q)
}

mulmod(p: I, q: I, n: I) : I == {
	import from IMOD n;
	lift((p::IMOD(n)) * (q::IMOD(n)))
}

--% Primality testing package

+++ Author: Michael Monagan
+++ Date Created: August 1987
+++ Date Last Updated: 19 April 1992
+++ Updated by: James Davenport
+++ Updated Because: of problems with strong pseudo-primes
+++   and for some efficiency reasons.
+++ Basic Operations:
+++ Related Domains:
+++ Also See:
+++ AMS Classifications:
+++ Keywords: integer, prime
+++ Examples:
+++ References: Davenport's paper in ISSAC 1992
+++ Description:
+++   The `IntegerPrimesPackage' implements a modification of
+++   Rabin's probabilistic
+++   primality test and the utility functions `nextPrime',
+++   `prevPrime' and `primes'.
IntegerPrimesPackage :with
   prime?: I -> Boolean
     ++ `prime?(n)' returns true if n is prime and false if not.
     ++ The algorithm used is Rabin's probabilistic primality test
     ++ (reference: Knuth Volume 2 Semi Numerical Algorithms).
     ++ If `prime? n' returns false, n is proven composite.
     ++ If `prime? n' returns true, prime? may be in error
     ++ however, the probability of error is very low.
     ++ and is zero below 25*10**9 (due to a result of Pomerance et al)
     ++ and below 341550071728321 due to a result of Jaeschke.
     ++ Specifically, this implementation does at least pseudo prime
     ++ tests and so the probability of error is `< 4**(-10)'.
     ++ The running time of this method is cubic in the length
     ++ of the input n, that is `O( (log n)**3 )', for n<10**20.
     ++ beyond that, the algorithm in this version is still cubic.
     ++ Two improvements due to Davenport have been incorporated
     ++ which catches some trivial strong pseudo-primes, such as
     ++ [Jaeschke, 1991] 1377161253229053 * 413148375987157, which
     ++ the original algorithm regards as prime
   nextPrime: I -> I
     ++ `nextPrime(n)' returns the smallest prime strictly larger than n
   prevPrime: I -> I
     ++ `prevPrime(n)' returns the largest prime strictly smaller than n
   primes: (I,I) -> List I
     ++ `primes(a,b)' returns a list of all primes p with
     ++ `a <= p <= b'
 == add
   import from B, Segment I, String

   smallPrimes: List I := [
		      2, 3, 5, 7, 11, 13, 17, 19, _
		      23, 29, 31, 37, 41, 43, 47, _
		      53, 59, 61, 67, 71, 73, 79, _
		      83, 89, 97, 101, 103, 107, 109, _
		      113, 127, 131, 137, 139, 149, 151, _
		      157, 163, 167, 173, 179, 181, 191, _
		      193, 197, 199, 211, 223, 227, 229, _
		      233, 239, 241, 251, 257, 263, 269, _
		      271, 277, 281, 283, 293, 307, 311, _
		      313]

   productSmallPrimes	 := reduce(*,smallPrimes,1)
   nextSmallPrime	 := 317
   nextSmallPrimeSquared := nextSmallPrime**2
   two			 := 2
   tenPowerTwenty	 :=10**20
   PomeranceList: List I := [
		       -- 3215031751, -- has a factor of 151
			  25326001,    161304001,   960946321,	1157839381,_
			  3697278427,  5764643587,  6770862367,_
			  14386156093, 15579919981, 18459366157,_
			  18459366157, 21276028621 ]
   PomeranceLimit	 :=(25*10**9)
   JaeschkeLimit	 :=341550071728321
--   rootsMinus1:Set I := empty()
   rootsMinus1:List I := empty()
   -- used to check whether we detect too many roots of -1
   count2Order:Vector NonNegativeInteger := new(1$SI,0)
   -- used to check whether we observe an element of maximal two-order

   primes(m:I, n:I):List(I) ==
      -- computes primes from m to n inclusive using prime?
      l:List(I) :=
	m = two => cons(m,nil) --[m]
	empty()
      n < two or n < m => empty()
      if even? m then m := m + 1
      ll:List(I) := [k for k in m..n by 2 | prime?(k)]
      reverse! concat!(ll, l)

   rabinProvesComposite : (I,I,I,I,SNNI) -> Boolean
   rabinProvesCompositeSmall : (I,I,I,I,SNNI) -> Boolean


   rabinProvesCompositeSmall(p:I,n:I,nm1:I,q:I,k:SNNI):B ==
	 -- probability n prime is > 3/4 for each iteration
	 -- for most n this probability is much greater than 3/4
	 t := powmod(p, q, n)
	 -- neither of these cases tells us anything
	 if not ( t = 1 or t = nm1) then
	    for j in 1..k-1 repeat
	       oldt := t
	       t := mulmod(t, t, n)
	       t = 1 => return true
	       -- we have squared someting not -1 and got 1
	       t = nm1 => break
	    not (t = nm1) => return true
	 false

   rabinProvesComposite(p:I,n:I,nm1:I,q:I,k:SNNI):B ==
	 import from SI
	 -- probability n prime is > 3/4 for each iteration
	 -- for most n this probability is much greater than 3/4
	 t := powmod(p, q, n)
	 -- neither of these cases tells us anything
	 if t=nm1 then count2Order(1):=count2Order(1)+1
	 if not ( t=1 or t = nm1) then
	    for j:SI in 1..k-1 repeat
	       oldt := t
	       t := mulmod(t, t, n)
	       t=1 => return true
	       -- we have squared someting not -1 and got 1
	       t = nm1 =>
		   free rootsMinus1
--		     rootsMinus1:=union(rootsMinus1,oldt)
		   if not member?(oldt, rootsMinus1) then
		      rootsMinus1:=cons(oldt, rootsMinus1)
		   free count2Order
		   count2Order(j+1):=count2Order(j+1)+1
		   break
	    not (t = nm1) => return true
	 (# rootsMinus1::I) > 2 => true	 -- Z/nZ can't be a field
	 false

   prime?(n:I):B ==
      import from SI
      n < two => false
      n < nextSmallPrime => member?(n, smallPrimes)
      not ( gcd(n, productSmallPrimes)=1) => false
      n < nextSmallPrimeSquared => true

      k: SI := 0
      q := nm1 := n-1
      while not odd? q repeat (q := q quo two; k := k+1)
      -- q = (n-1) quo 2**k for largest possible k

      n < PomeranceLimit =>
	  rabinProvesCompositeSmall(2,n,nm1,q,k) =>   false
	  rabinProvesCompositeSmall(3,n,nm1,q,k) =>   false
	  rabinProvesCompositeSmall(5,n,nm1,q,k) =>   false
	  member?(n,PomeranceList) => false
	  true

      n < JaeschkeLimit =>
	  rabinProvesCompositeSmall(2,n,nm1,q,k) =>  false
	  rabinProvesCompositeSmall(3,n,nm1,q,k) =>  false
	  rabinProvesCompositeSmall(5,n,nm1,q,k) =>  false
	  rabinProvesCompositeSmall(7,n,nm1,q,k) =>  false
	  rabinProvesCompositeSmall(11,n,nm1,q,k) => false
	  rabinProvesCompositeSmall(13,n,nm1,q,k) =>  false
	  rabinProvesCompositeSmall(17,n,nm1,q,k) =>  false
	  true

      free rootsMinus1, count2Order
      rootsMinus1:= empty()
      count2Order := new(k,0) -- vector of k zeroes

--	mn := minIndex smallPrimes
      mn:SI := 1
      for i:SI in mn+1..mn+10 repeat
	  rabinProvesComposite(smallPrimes i,n,nm1,q,k) => return false
      import from IntegerRoots
      q > 1 and perfectSquare?(3*n+1) => false
      ((n9:=n rem (9))=1 or n9 = -1) and perfectSquare?(8*n+1) => false
      -- Both previous tests from Damgard & Landrock
      currPrime:=smallPrimes(mn+10)
      -- probablySafe:=tenPowerTwenty
      -- while count2Order(k) = 0 or n > probablySafe repeat
      while count2Order(k) = 0 repeat
	  currPrime := nextPrime currPrime
	  -- probablySafe:=probablySafe*(100)
	  rabinProvesComposite(currPrime,n,nm1,q,k) => return false
      true

   nextPrime(n:I):I ==
      -- computes the first prime after n
      n < two => two
      if odd? n then n := n + two else n := n + 1
      while not prime? n repeat n := n + two
      n

   prevPrime(n:I):I ==
      -- computes the first prime before n
      n < 3 => error "no primes less than 2"
      n = 3 => two
      if odd? n then n := n - two else n := n - 1
      while not prime? n repeat n := n - two
      n


--% IntegerRoots package

+++ Author: Michael Monagan
+++ Date Created: November 1987
+++ Date Last Updated:
+++ Basic Operations:
+++ Related Domains:
+++ Also See:
+++ AMS Classifications:
+++ Keywords: integer roots
+++ Examples:
+++ References:
+++ Description: The `IntegerRoots' package computes square roots and
+++   nth roots of integers efficiently.
IntegerRoots : with
    perfectNthPower?: (I, NNI) -> Boolean
      ++ `perfectNthPower?(n,r)' returns true if n is an `r'th
      ++ power and false otherwise
    perfectNthRoot: (I,NNI) -> Partial I
      ++ `perfectNthRoot(n,r)' returns the `r'th root of n if n
      ++ is an `r'th power and returns "failed" otherwise
    perfectNthRoot: I -> Record(base:I, exponent:NNI)
      ++ `perfectNthRoot(n)' returns `[x,r]', where `n = x^r'
      ++ and r is the largest integer such that n is a perfect `r'th power
    approxNthRoot: (I,NNI) -> I
      ++ `approxRoot(n,r)' returns an approximation x
      ++ to `n**(1/r)' such that `-1 < x - n**(1/r) < 1'
    perfectSquare?: I -> Boolean
      ++ `perfectSquare?(n)' returns true if n is a perfect square
      ++ and false otherwise
    perfectSqrt: I -> Partial I
      ++ `perfectSqrt(n)' returns the square root of n if n is a
      ++ perfect square and returns "failed" otherwise
    approxSqrt: I -> I
      ++ `approxSqrt(n)' returns an approximation x
      ++ to `sqrt(n)' such that `-1 < x - sqrt(n) < 1'.
      ++ Compute an approximation s to `sqrt(n)' such that
      ++	   `-1 < s - sqrt(n) < 1'
      ++ A variable precision Newton iteration is used.
      ++ The running time is `O( log(n)**2 )'.

 == add
    import from B, Segment I, String

    import from IntegerPrimesPackage
    two:I := 2

    default a,b:I
    import from Partial(I)
    perfectSquare?(a:I):B == not failed? (perfectSqrt a)
    perfectNthPower?(b:I, n:NNI):B == not failed? perfectNthRoot(b, n)

    perfectNthRoot(n:I): Record(base:I,exponent: NNI) ==
     -- complexity (log log n)**2 (log n)**2
      local m: NNI
      local r: Partial I
      one? n or zero? n or n = -1 => [n, 1]
      e:NNI := 1
      p:NNI := 2
      while p <= (length(n) :: I) + 1 repeat
--	 for m in 0.. while not failed? (r := perfectNthRoot(n, p)) repeat
	 m := 0
	 while not failed? (r := perfectNthRoot(n, p)) repeat
	    n := retract(r)@I
	    m := m+1
	 e := e * p ** m
	 p := nextPrime(p) @ NNI
      [n, e]

    approxNthRoot(a:I, n:I):I ==   -- complexity (log log n) (log n)**2
      zero? n => error "invalid arguments"
      one? n => a
      n=2 => approxSqrt a
      negative? a =>
	odd? n => - approxNthRoot(-a, n)
	0
      zero? a => 0
      one? a => 1
      -- quick check for case of large n
      ((3*n) quo 2) >= (l := ((length a) :: I))=> two
      -- the initial approximation must be >= the root
      y:I:= max(two, shift(1,retract((n+l-1) quo n)@SingleInteger))
      z:I := 1
      n1:= (n-1)@NNI
      while z > 0 repeat
	x:I := y
--	  x := y
	xn:= x**n1
	y := (n1*x*xn+a) quo (n*xn)
	z := x-y
      x

    perfectNthRoot(b:I, n:NNI):Partial I ==
      (r := approxNthRoot(b, n)) ** n = b => r :: Partial I
      failed

    perfectSqrt(a:I):Partial I ==
      a < 0 or (not member?(a rem (144), resMod144)) => failed
      (s := approxSqrt a) * s = a => s :: Partial I
      failed

    approxSqrt(a:I):I ==
      a < 1 => 0
      if (n:SingleInteger := length a) > 100 then
	 -- variable precision newton iteration
	 n := n quo 4
	 s := approxSqrt shift(a, -2 * n)
	 s := shift(s, n)
	 return ((1 + s + a quo s) quo two)
      -- initial approximation for the root is within a factor of 2
--	(new, old) := (shift(1, n quo two), 1)
      new:I := shift(1, n quo 2)
      old:I := 1
      while new ^= old repeat
--	   (new, old) := ((1 + new + a quo new) quo two, new)
	 old := new
	 new := (1 + new + a quo new) quo 2
      new


--import from IntegerRoots
--import from TextWriter
--print<<33<<newline
--print<<approxSqrt 33<<newline
