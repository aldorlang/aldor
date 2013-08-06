#include "aldor.as"
#include "aldorio.as"

-- intfact.as contains Aldor code for checking primality and performing
-- factorisations
--
-- Copyright (C) 2003 	Bill Naylor
--
-- This library is free software; you can redistribute it and/or
-- modify it under the terms of the GNU Lesser General Public
-- License as published by the Free Software Foundation; either
-- version 2.1 of the License, or (at your option) any later version.
--
-- This library is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
-- Lesser General Public License for more details.
--
-- You should have received a copy of the GNU Lesser General Public
-- License along with this library; if not, write to the Free Software
-- Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
-- You may contact the author at e-mail: bill@mcs.vuw.ac.nz 

I ==> Integer;
MI ==> MachineInteger;

import from String;

+++ Author: Michael Monagan
+++ Date Created: August 1987
+++ Date Last Updated: 31 May 1993
+++ Updated by: James Davenport
+++ Updated Because: of problems with strong pseudo-primes
+++   and for some efficiency reasons.
+++ converted for aldor by Bill Naylor 1st May 2003
+++ Basic Operations:
+++ Related Domains:
+++ Also See:
+++ AMS Classifications:
+++ Keywords: integer, prime
+++ Examples:
+++ References: Davenport's paper in ISSAC 1992
+++             AXIOM Technical Report ATR/6
+++ Description:
+++   The \spadtype{IntegerPrimesPackage} implements a modification of
+++   Rabin's probabilistic
+++   primality test and the utility functions \spadfun{nextPrime},
+++   \spadfun{prevPrime} and \spadfun{primes}.
IntegerPrimesPackage: with {
   prime?: I -> Boolean;
     ++ \spad{prime?(n)} returns true if n is prime and false if not.
     ++ The algorithm used is Rabin's probabilistic primality test
     ++ (reference: Knuth Volume 2 Semi Numerical Algorithms).
     ++ If \spad{prime? n} returns false, n is proven composite.
     ++ If \spad{prime? n} returns true, prime? may be in error
     ++ however, the probability of error is very low.
     ++ and is zero below 25*10**9 (due to a result of Pomerance et al),
     ++ below 10**12 and 10**13 due to results of Pinch,
     ++ and below 341550071728321 due to a result of Jaeschke.
     ++ Specifically, this implementation does at least 10 pseudo prime
     ++ tests and so the probability of error is \spad{< 4**(-10)}.
     ++ The running time of this method is cubic in the length
     ++ of the input n, that is \spad{O( (log n)**3 )}, for n<10**20.
     ++ beyond that, the algorithm is quartic, \spad{O( (log n)**4 )}.
     ++ Two improvements due to Davenport have been incorporated
     ++ which catches some trivial strong pseudo-primes, such as
     ++ [Jaeschke, 1991] 1377161253229053 * 413148375987157, which
     ++ the original algorithm regards as prime
   nextPrime: I -> I;
     ++ \spad{nextPrime(n)} returns the smallest prime strictly larger than n
   prevPrime: I -> I;
     ++ \spad{prevPrime(n)} returns the largest prime strictly smaller than n
   primes: (I,I) -> List(I);
     ++ \spad{primes(a,b)} returns a list of all primes p with
     ++ \spad{a <= p <= b}
} == add {
   import from MachineInteger;
   smallPrimes: List(I) := [2,3,5,7,11,13,17,19,_
                      23,29,31,37,41,43,47,_
                      53,59,61,67,71,73,79,_
                      83,89,97,101,103,107,109,_
                      113,127,131,137,139,149,151,_
                      157,163,167,173,179,181,191,_
                      193,197,199,211,223,227,229,_
                      233,239,241,251,257,263,269,_
                      271,277,281,283,293,307,311,_
                      313];

   productSmallPrimes:I  := 61076929465933196099278943388997855150356143888238371488665496574810764573680243467182799164806563626522181311132959748531230210;
   nextSmallPrime:I      := 317;
   nextSmallPrimeSquared:I := nextSmallPrime^2;
   two:I                 := 2;
   tenPowerTwenty:I :=(10)^20;
   PomeranceList:List(I):= [25326001, 161304001, 960946321, 1157839381,
                     -- 3215031751, -- has a factor of 151
                     3697278427, 5764643587, 6770862367,
                      14386156093, 15579919981, 18459366157,
                       19887974881, 21276028621 ];
   PomeranceLimit:I :=27716349961;  -- replaces (25*10^9) due to Pinch
   PinchList:List(I) := [3215031751, 118670087467, 128282461501, 354864744877,
                546348519181, 602248359169, 669094855201 ];
   PinchLimit:I := (10^12);
   PinchList2:List(I) := [2152302898747, 3474749660383];
   PinchLimit2:I := (10^13);
   JaeschkeLimit:I :=341550071728321;
   count2Order:Array(I) := [0];
   default rootsMinus1:List I := [];
   -- used to check whether we observe an element of maximal two-order

   primes(m:I, n:I):List(I) == {
      -- computes primes from m to n inclusive using prime?
      local l:List(I);
      if m<=two then l := [two] else l := [];
      n < two or n < m => [];
      if even? m then m := m + 1;
      ll:List(I) := [k for k in m..n by 2 | prime?(k)];
      reverse append!(ll, l)
   }

  prem(a:I,b:I):I == {
    r := a rem b;
    if r<0 then -r else r
  }

  mulmod(x:I,y:I,p:I):I == {
    (x*y) mod p;
  }

  squaremod(x:I,p:I):I == {
    (x*x) mod p;
  }

  powmod(x:I,n:I,p:I):I == {
    import from MachineInteger;
    if x<0 then x2 := prem(x,p);
    zero? x2 => 0;zero? n => 1;
    y:I := 1;z := x mod p;n2 := n;
    repeat {
      if odd? n2 then y := mulmod(y,z,p);
      n2 := shift(n2,-1);
      if zero?(n2) then return y;
      z := squaremod(z,p);
    }
  }

   rabinProvesCompositeSmall(p:I,n:I,nm1:I,q:I,k:I):Boolean == {
         -- probability n prime is > 3/4 for each iteration
         -- for most n this probability is much greater than 3/4
         t := powmod(p, q, n);
         -- neither of these cases tells us anything
         if not (one? t or t = nm1) then {
            for j in 1..k-1 repeat {
               oldt := t;
               t := squaremod(t, n);
               one? t => return true;
               -- we have squared something not -1 and got 1
               t = nm1 => break;}
            not (t = nm1) => return true
         }
         false
   }

   union(l:List(I),i:I):List(I) == {
     not(member?(i,l)) => cons(i,l);
     l
   }

   rabinProvesComposite(p:I,n:I,nm1:I,q:I,k:I):Boolean == {
         free rootsMinus1,count2Order;
         -- probability n prime is > 3/4 for each iteration
         -- for most n this probability is much greater than 3/4
         t := powmod(p, q, n);
         -- neither of these cases tells us anything
         if t=nm1 then count2Order.0:=(count2Order.0)+1;
         if not (one? t or t = nm1) then {
            for j in 1..k-1 repeat {
               oldt := t;
               t := squaremod(t, n);
               one? t => return true;
               -- we have squared something not -1 and got 1
               if t = nm1 then {
                   rootsMinus1:=union(rootsMinus1,oldt);
                   count2Order.(machine j):=count2Order.(machine j)+1;
                   break
               }
            }
            not (t = nm1) => return true
         }
         #rootsMinus1 > 2 => true;  -- Z/nZ can't be a field
         false
  }

   prime?(n:I):Boolean == {
      free rootsMinus1,count2Order;

      -- used to check whether we detect too many roots of -1
      import from IntegerRoots;
      inline from IntegerRoots,I,Boolean;
      if n < two then return false;
      if n < nextSmallPrime then return member?(n, smallPrimes);
      if not one? gcd(n, productSmallPrimes) then return false;
      if n < nextSmallPrimeSquared then return true;

      default k:I;
      nm1 := n-1;
      q := nm1 quo two;
      for k2 in 1@I..  repeat {if odd? q then {k := k2;break}; q := q quo two}
      -- q = (n-1) quo 2^k for largest possible k
      if n < JaeschkeLimit then {
          if rabinProvesCompositeSmall(2,n,nm1,q,k) then return false;
          if rabinProvesCompositeSmall(3,n,nm1,q,k) then return false;

          if n < PomeranceLimit then {
              if rabinProvesCompositeSmall(5,n,nm1,q,k) then return false;
              if member?(n,PomeranceList) then return false;
              return true
          }

          if rabinProvesCompositeSmall(7,n,nm1,q,k) then return false;
          n < PinchLimit => {
              if rabinProvesCompositeSmall(10,n,nm1,q,k) then return false;
              if member?(n,PinchList) then return false;
              return true
          }

          if rabinProvesCompositeSmall(5,n,nm1,q,k) then return false;
          if rabinProvesCompositeSmall(11,n,nm1,q,k) then return false;
          if n < PinchLimit2 then {
              if member?(n,PinchList2) then return false;
              return true
          }

          if rabinProvesCompositeSmall(13,n,nm1,q,k) then return false;
          if rabinProvesCompositeSmall(17,n,nm1,q,k) then return false;
          return true
      }

      rootsMinus1:= [];
      count2Order := new(machine k,0); -- vector of k zeroes

      mn:MachineInteger := firstIndex$List(I);
      for i in (mn+1)..(mn+10) repeat {
          if rabinProvesComposite(smallPrimes.i,n,nm1,q,k) then {
            return false;
          } else {
          }
      }
      if q > 1 and perfectSquare?(3*n+1) then return false;
      n9:=n rem 9;
      if (n9=1 or n9 = -1) and perfectSquare?(8*n+1) then return false;
      -- Both previous tests from Damgard & Landrock
      currPrime:=smallPrimes.10;
      probablySafe:=tenPowerTwenty;
      while count2Order.(machine k-1) = 0 or n > probablySafe repeat {
          currPrime := nextPrime currPrime;
          probablySafe:=probablySafe*100;
          rabinProvesComposite(currPrime,n,nm1,q,k) => return false;
      }
      true
   }

   nextPrime(n:I):I == {
      -- computes the first prime after n
      n < two => two;
      if odd? n then n := n + two else n := n + 1;
      while not prime? n repeat n := n + two;
      n
   }

   prevPrime(n:I):I == {
      -- computes the first prime before n
      n < 3 => error "no primes less than 2";
      n = 3 => two;
      if odd? n then n := n - two else n := n - 1;
      while not prime? n repeat n := n - two;
      n
   }
}

IntegerRoots: with {
    perfectNthPower?: (I, I) -> Boolean;
      ++ \spad{perfectNthPower?(n,r)} returns true if n is an \spad{r}th
      ++ power and false otherwise
    perfectNthRoot: (I,I) -> Union(i:I,failed:'failed');
      ++ \spad{perfectNthRoot(n,r)} returns the \spad{r}th root of n if n
      ++ is an \spad{r}th power and returns "failed" otherwise
    perfectNthRoot: I -> Record(base:I, exponent:I);
      ++ \spad{perfectNthRoot(n)} returns \spad{[x,r]}, where \spad{n = x\^r}
      ++ and r is the largest integer such that n is a perfect \spad{r}th power
    approxNthRoot: (I,I) -> I;
      ++ \spad{approxRoot(n,r)} returns an approximation x
      ++ to \spad{n**(1/r)} such that \spad{-1 < x - n**(1/r) < 1}
    perfectSquare?: I -> Boolean;
      ++ \spad{perfectSquare?(n)} returns true if n is a perfect square
      ++ and false otherwise
    perfectSqrt: I -> Union(i:I,failed:'failed');
      ++ \spad{perfectSqrt(n)} returns the square root of n if n is a
      ++ perfect square and returns "failed" otherwise
    approxSqrt: I -> I;
      ++ \spad{approxSqrt(n)} returns an approximation x
      ++ to \spad{sqrt(n)} such that \spad{-1 < x - sqrt(n) < 1}.
      ++ Compute an approximation s to \spad{sqrt(n)} such that
      ++           \spad{-1 < s - sqrt(n) < 1}
      ++ A variable precision Newton iteration is used.
      ++ The running time is \spad{O( log(n)**2 )}.
} == add {
    import from I,Union(i:I,failed:'failed');
    inline from I,Union(i:I,failed:'failed'),MachineInteger;
    resMod144: List I := [0,1,4,9,16,25,36,49,52,64,73,81,97,100,112,121];
    two:I := 2;
    twomach:MachineInteger := 2;
 
    perfectSquare?(a:I):Boolean       == (perfectSqrt a) case i;
    perfectNthPower?(b:I, n:I):Boolean == perfectNthRoot(b, n) case i;


    perfectNthRoot(n:I):Record(base:I, exponent:I) ==  {-- complexity (log log n)**2 (log n)**2
      import from IntegerPrimesPackage;
      local m2:MI;
      one? n or zero? n or n = -1 => [n, 1];
      e:I := 1;
      p:I := 2;
      while machine(p) <= length(n) + 1 repeat {
         for m in 0@MI.. repeat {
            if (r := perfectNthRoot(n, p)) case failed then {
              m2 := m;break;
            }
            n := r.i;
         }
         e := e * p ^ m2;
         p := nextPrime(p);
      }
      [n, e]
    }

    approxNthRoot(a:I, n:I):I == {  -- complexity (log log n) (log n)**2
--      zero? n => error "invalid arguments";
      one? n => a;
      n=2 => approxSqrt a;
      a<0 => {
        odd? n => - approxNthRoot(-a, n);
        0
      }
      zero? a => 0;
      one? a => 1;
      -- quick check for case of large n
      default l:MI;
      machine((3*n) quo 2) >= (l := length(a)) => two;
      -- the initial approximation must be >= the root
      y:I := max(two, shift(1, machine((n+l::I-1) quo n)));
      z:I := 1;
      n1:I := n-1;n1m:MachineInteger := machine(n1);
      while z > 0 repeat {
        x := y;
        xn := x^n1m;
--        y := (n1*x*xn+a) quo (n*xn);
        nxn := n*xn;y := ((nxn-xn)*x + a) quo nxn;
        z := x-y
      }
      x;
    }

    perfectNthRoot(b:I, n:I):Union(i:I,failed:'failed') == {
      (r := approxNthRoot(b, n))^machine(n) = b => [r];
      [failed]
    }

    perfectSqrt(a:I):Union(i:I,failed:'failed') == {
      a < 0 or not member?(a rem 144, resMod144) => [failed];
      (s := approxSqrt a) * s = a => [s];
      [failed]
    }

    approxSqrt(a:I):I == {
      import from MI;
      local new,old:I;
      a < 1 => 0;
      if (n := length a) > 100 then {
         -- variable precision newton iteration
         n := n quo 4;
         s := approxSqrt shift(a, -2 *  n);
         s := shift(s,  n);
         return ((1 + s + a quo s) quo two)
      }
      -- initial approximation for the root is within a factor of 2
      (new, old) := (shift(1, n quo twomach), 1);
      while new ~= old repeat {
         (new, old) := ((1 + new + a quo new) quo two, new)
      }
      new
   }
}



B      ==> Boolean;
FF     ==> Record(unt:I,fct:List(FFE));
NNI    ==> NonNegativeInteger;
LMI    ==> ListMultiDictionary I;
FFE    ==> Record(flg:Union(nil:'nil',sqfr:'sqfr',irred:'irred',prime:'prime'),
                                                   fctr:I, xpnt:Integer);

--% IntegerFactorizationPackage
-- recoded MBM Nov/87

+++ This Package contains basic methods for integer factorization.
+++ The factor operation employs trial division up to 10,000.  It
+++ then tests to see if n is a perfect power before using Pollards
+++ rho method.  Because Pollards method may fail, the result
+++ of factor may contain composite factors.  We should also employ
+++ Lenstra's eliptic curve method.
IntegerFactorizationPackage: with {
    factor : I -> FF;
      ++ factor(n) returns the full factorization of integer n
    squareFree   : I -> FF;
      ++ squareFree(n) returns the square free factorization of integer n
    BasicMethod : I -> FF;
      ++ BasicMethod(n) returns the factorization
      ++ of integer n by trial division
    PollardSmallFactor: I -> Union(i:I,failed:'failed');

       ++ PollardSmallFactor(n) returns a factor
       ++ of n or "failed" if no one is found
} == add {
    import from IntegerRoots;
    inline from IntegerRoots,FF,List(FFE),FFE;

    makeFR(u:I,y:List(FFE)):FF == [u,y];

    factorList(u:FF):List(FFE) == u.fct;

    squareFree(n:I):FF == {
       import from Union(i:I,failed:'failed'),FFE;

       local u:I;
       if n<0 then {m := -n; u := -1}
              else {m := n; u := 1}
       (m > 1) and ((v := perfectSqrt m) case i) => {
          for rec in (l := factorList(sv := squareFree(v.i))) repeat
            rec.xpnt := 2 * rec.xpnt;
          makeFR(u * sv.unt, l)
       }
    -- avoid using basic sieve when the lim is too big
       lim := 1 + approxNthRoot(m,3);
       lim > 100000 => makeFR(u, factorList factor m);
       x := BasicSieve(m, lim);
       y := {
         one?(m:= x.unt) => factorList x;
         (v := perfectSqrt m) case i => 
            append!(factorList x, [[sqfr],v.i,2]$FFE);
         append!(factorList x, [[sqfr],m,1]$FFE)
       }
       makeFR(u, y)
    }

    -- Pfun(y: I,n: I): I == (y^2 + 5) rem n
    PollardSmallFactor(n:I):Union(i:I,failed:'failed') == {
       -- Use the Brent variation
       x0 := random()$I;
       m := 100;
       y := x0 rem n;
       local (r,q,G):I := (1,1,1);
       while not(G > 1) repeat {
          x := y;
          for i in 1..r repeat {
             y := (y*y+5) rem n;
             q := (q*abs(x-y)) rem n;
             k:I := 0
          }
          while not( (k>=r) or (G>1)) repeat {
             ys := y;
             for i in 1..min(m,r-k) repeat {
                y := (y*y+5) rem n;
                q := q*abs(x-y) rem n
             }
             G := gcd(q,n);
             k := k+m
          }
          r := 2*r
       }
       if G=n then {
          while not(G>1) repeat {
             ys := (ys*ys+5) rem n;
             G := gcd(abs(x-ys),n)
          }
       }
       G=n => [failed];
       [G]
    }

    rest(x:List(I),n:I):List(I) == {
      n=0 => return x;
      rest(rest x,n-1);
    }

    BasicSieve(r:I, lim:I):FF == {
       import from FFE;
       l:List(I) :=
          [1,2,2,4,2,4,2,4,6,2,6];
       l := append!(l, rest(l, 3));
       local d : I := 2;
       local n : I := r;
       ls:List(FFE) := [];
       local m:I;
       for s in l repeat {
          d > lim => return makeFR(n, ls);
          if n<d*d then {
             if n>1 then ls := append!(ls, [[prime],n,1]$FFE);
             return makeFR(1, ls)
          }
          for m2 in 0@I.. repeat {
            if not(zero?(n rem d)) then {m:=m2;break}
            n := n quo d;
          }
          if m>0 then ls := append!(ls, [[prime],d,m]$FFE);
          d := d+s
       }
       never
    }

    BasicMethod(n:I):FF == {
       local u:I;
       if n<0 then (m := -n; u := -1)
              else (m := n; u := 1);
       x := BasicSieve(m, 1 + approxSqrt m);
       makeFR(u, factorList x)
    }

    count(n:I,l:List(I)):MachineInteger == {
      r:MachineInteger := 0;for i in l repeat if n=i then r:=r+1;
      r
    }

    UFL ==> Union(nil:'nil',sqfr:'sqfr',irred:'irred',prime:'prime');

    eq(fnl:UFL,f:I,xp:I,i:FFE):B == {
      f ~= i.fctr or xp ~=i.xpnt => false;
      iu:UFL := i.flg;
     (iu case nil) and (fnl case nil) or
     (iu case sqfr) and (fnl case sqfr) or
     (iu case irred) and (fnl case irred) or
     (iu case prime) and (fnl case prime)
    }

    count(n:FFE,l:List(FFE)):MachineInteger == {
      nfl:UFL := n.flg;
      (nf,nxp) := (n.fctr,n.xpnt); r:MachineInteger := 0;
      for i in l repeat if eq(nfl,nf,nxp,i) then r:=r+1;
      r
    }

    countRemove(n:FFE,l:List(FFE)):(I,List(FFE)) == {
      nfl:UFL := n.flg;
      (nf,nxp) := (n.fctr,n.xpnt); r := 0; rl:List(FFE) := [];
      for i in l repeat {
        if eq(nfl,nf,nxp,i) then {
          r := r+1;rl := cons(i,rl);
        }
      }
      {r,reverse rl}
    }

    -- special remove! for List FFE (the normal one doesn't work)
    myremove!(n:FFE,fl:List(FFE)):List(FFE) == {
      import from MachineInteger;
      nf := n.flg;(nfa,nxp):I := (n.fctr,n.xpnt);
      fl2:List(FFE) := [];
      for f in fl repeat {
        if not eq(nf,nfa,nxp,n) then fl2 := cons(f,fl2);
      }
      fl := reverse(fl2)
    }

    myset!(l:List(I),i:MI,j:I):List(I) == {
      cons(j,l)
--      len := #l;
--      if i<=len then {set!(l,i,j);return l}
--      l := append!(l,new(i-len,0));
--      set!(l,i,j);
--      l
    }

    factor(m:I):FF == {
       import from MI,FFE,Record(base:I, exponent:I),Union(i:I,failed:'failed');
       import from IntegerPrimesPackage;
       inline from IntegerPrimesPackage;

       local u:I;
       zero? m => makeFR(1,[[[nil],0,0]]);
       if m<0 then {n := -m; u := -1}
                      else {n := m; u := 1};
       b := BasicSieve(n, 10000);
       flb := factorList b;
       one?(n := b.unt) => makeFR(u, flb);
       a:List(I) := []; -- numbers yet to be factored
       flb2:List(I) := []; -- prime factors found
       f:List(I) := []; -- number which could not be factored
       a := cons(n,a);
       while not empty? a repeat {
          n := first a; 
          c := count(n, a); a := remove!(n, a);
          --{c,a} := countRemove(n,a);
          if prime?(n)$IntegerPrimesPackage then {
            flb2 := myset!(flb2,c,n);
            iterate;
          }
          -- test for a perfect power
          if (s := perfectNthRoot n).exponent > 1 then {
            a := myset!(a,c*machine(s.exponent),s.base);iterate}
          -- test for a difference of square
          x:=approxSqrt n;
          if (x^2<n) then x:=x+1;
          if (y:=perfectSqrt (x^2-n)) case i then {
                a := myset!(a,c,x+y.i);
                a := myset!(a,c,x-y.i);iterate
          }
          if (d := PollardSmallFactor n) case i then {
             for m2 in 0@I.. repeat {
               if not(zero?(n rem d.i)) then {
                 m := m2;break}
               n := n quo d.i;
             }
             a := myset!(a, machine(m)*c, d.i);
             if n > 1 then a := myset!(a, c, n);
             iterate
          }
          -- an elliptic curve factorization attempt should be made here
          f := myset!(f, c, n);
       }
       -- insert prime factors found
       while not empty?(flb2) repeat {
          n2 := first flb2; c := count(n2, flb2); 
          flb2 := remove!(n2, flb2);
          flb := cons([[prime],n2,c::I]$FFE,flb)
       }
       -- insert non-prime factors found
       while not empty? f repeat {
          n := first f; c := count(n, f); f := remove!(n, f);
          flb := cons([[nil],n,c::I]$FFE,flb)
       }
       makeFR(u, flb)
    }
}

import from IntegerFactorizationPackage;
import from I;
stdout << "start" << newline;
factor 23847298372;
stdout << "end" << newline;
