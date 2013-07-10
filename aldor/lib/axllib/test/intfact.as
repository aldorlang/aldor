-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp -l axldem
--> testrun  -l axldem -l axllib 
#pile

-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).

#include "axllib.as"

#library DemoLib       "axldem"

import from DemoLib

macro 
  B == Boolean
  N == Integer
  I == Integer
  min(aa,bb) == if aa < bb then aa else bb
  one? aa == aa = 1


facFlags ==> 'nil, sqfr, irred, prime'

FFE(S:BasicType): BasicType with
   bracket: (facFlags, S, Integer) -> %
   apply: (%, 'xpnt') -> Integer
   apply: (%, 'fctr') -> S
   apply: (%, 'flg') -> facFlags
   set!: (%, 'xpnt', Integer) -> Integer
 == add
   Rep ==> Record(flg:facFlags, fctr:S, xpnt:Integer)
   import from Rep
   import from S
   import from Integer
   (f:%) = (g:%):Boolean ==  rep(f).fctr = rep(g).fctr and rep(f).xpnt = rep(g).xpnt
   (f:%) ~= (g:%):Boolean == not(f=g)
   sample: % == [nil$facFlags, sample, 0]
   apply(f:%, tag:'xpnt'):Integer == rep(f).xpnt
   apply(f:%, tag:'fctr'):S == rep(f).fctr
   apply(f:%, tag:'flg'):facFlags == rep(f).flg
   set!(f:%, tag:'xpnt', e:Integer):Integer == set!(rep(f),xpnt,e)
   set!(f:%, tag:'fctr', s:S):S == set!(rep(f),fctr,s)
   set!(f:%, tag:'flg', flag:facFlags):facFlags == set!(rep(f),flg,flag)
   [flag:facFlags, s:S, e:Integer]:% == per [flag,s,e]
   (p: TextWriter) << (l: %): TextWriter ==
         import from String
         import from S
         import from Integer
         p << "ffe(fctr: "<<rep(l).fctr<<" xpnt: "<<rep(l).xpnt<<")"

Factored(S: BasicType with
           1:%
        ) : with
   makeFR: (S, List FFE(S)) -> %
   factorList: % -> List FFE(S)
   unit: % -> S
   <<: (TextWriter, %) -> TextWriter
 == add
   Rep ==> Record(unit:S, faclist:List FFE(S))
   import from Rep
   makeFR(u:S, lffe:List FFE(S)):% == per [u, lffe]
   factorList(x:%):List FFE(S) == apply(rep(x),faclist)
   unit(x:%):S == apply(rep(x),unit)
   (p:TextWriter) << (f:%): TextWriter ==
      import from String
      import from S
      import from List FFE(S)
      import from FFE(S)
      import from 'xpnt'
      import from 'fctr'
      import from I
      if unit(f) ~= 1 then p:=p<<unit f
      for fac in factorList f repeat
        p:=p<<" "<<(fac.fctr)@S
        if fac.xpnt ~= 1@I then p:=p<<"^"<<(fac.xpnt)@I
      p


--% IntegerFactorizationPackage
-- recoded MBM Nov/87

B      ==> Boolean
FF     ==> Factored I
LMI    ==> ListMultiDictionary I

+++ This Package contains basic methods for integer factorization.
+++ The factor operation employs trial division up to 10,000.  It
+++ then tests to see if n is a perfect power before using Pollards
+++ rho method.  Because Pollards method may fail, the result
+++ of factor may contain composite factors.  We should also employ
+++ Lenstra's elliptic curve method.

IntegerFactorizationPackage: with

    factor : I -> FF
      ++ factor(n) returns the full factorization of integer n
    squareFree   : I -> FF
      ++ squareFree(n) returns the square free factorization of integer n
    BasicMethod : I -> FF
      ++ BasicMethod(n) returns the factorization
      ++ of integer n by trial division
    PollardSmallFactor: I -> Partial(I)
       ++ PollardSmallFactor(n) returns a factor
       ++ of n or "failed" if no one is found

  == add

    import from Segment I
    import from IntegerRoots
    import from IntegerPrimesPackage
    import from Partial I
    import from 'nil,sqfr,irred,prime'
    import from FFE(I)
    import from List(FFE(I))
    import from FF
    import from 'xpnt'

    concat!(l:List FFE I, v:FFE I):List FFE I ==
       concat!(l, cons(v,nil))

    BasicSieve: (I, I) -> FF


    squareFree(n:I):FF ==
       if n<0
            then  (m := -n; u:I := -1)
            else (m := n; u := 1)
       (m > 1) and (not(failed? (v := perfectSqrt m))) =>
          l := factorList(sv := squareFree(retract v))
          for rec in generator l repeat
            rec.xpnt := 2 * rec.xpnt
          makeFR(u * unit sv, l)
       lim := 1 + approxNthRoot(m,3)
       lim > 100000 => makeFR(u, factorList factor m)
       x := BasicSieve(m, lim)
       y :=
         one? unit x => factorList x
         concat!(factorList x, [sqfr,unit(x),1]$FFE(I))
       makeFR(u, y)

    -- Pfun(y: I,n: I): I == (y**2 + 5) rem n
    PollardSmallFactor(n:I):Partial(I) ==
       -- Use the Brent variation
       m:I := 100
       y:I := random(n)$RandomNumberSource
       r:I := 1
       q:I := 1
       G:I := 1
       while G <= 1 repeat
          x:I := y
          for i in 1..r repeat
             y := (y*y+5) rem n
             q := (q*abs(x-y)) rem n
             k:I := 0
          while not( (k>=r) or (G>1)) repeat
             ys:I := y
             for i in 1..min(m,r-k) repeat
                y := (y*y+5) rem n
                q := q*abs(x-y) rem n
             G := gcd(q,n)
             k := k+m
          r := 2*r
       if G=n then
          while G<=1 repeat
             ys := (ys*ys+5) rem n
             G := gcd(abs(x-ys),n)
       G=n => failed
       G::Partial(I)

    BasicSieve(r:I, lim:I):FF ==
       import from List(I)
       l:List(I) :=
          [1, 2, 2, 4, 2, 4, 2, 4, 6, 2, 6]
       concat!(l, rest rest rest l)
       d:I := 2
       n:I := r
       ls := empty()$List(FFE(I))
       for s in l repeat
          d > lim => return makeFR(n, ls)
          if n<d*d then
             if n>1 then ls := concat!(ls, [prime,n,1]$FFE(I))
             return makeFR(1, ls)
          m:I:=0
          for mm in 1.. while zero?(n rem d) repeat (n := n quo d; m:=mm)
          if m>0 then ls := concat!(ls, [prime,d, m]$FFE(I))
          d := d+s
       never 

    BasicMethod(n:I):FF ==
       local u:I
       if n<0 then (m := -n; u := -1)
              else (m := n; u := 1)
       x := BasicSieve(m, 1 + approxSqrt(m)$IntegerRoots)
       makeFR(u, factorList x)

    factor(m:I):FF ==
       local u:I
       zero? m => makeFR(m,nil)
       if negative? m then (n := -m; u := -1)
                      else (n := m; u := 1)
       bfac := BasicSieve(n, 10000)
       flb := factorList bfac
       import from List(FFE I)
       import from String
       one?(n := unit bfac) => makeFR(u, flb)
       a:LMI := dictionary() -- numbers yet to be factored
       b:LMI := dictionary() -- prime factors found
       f:LMI := dictionary() -- number which could not be factored
       insert!(n, a)
       while not empty? a repeat
          n := inspect a; c:SingleInteger := count(n, a); remove!(n, a)
          prime?(n)$IntegerPrimesPackage => insert!(n, b, c)
          -- test for a perfect power
          (s :Record(base:I, exponent:I) := perfectNthRoot n).exponent > 1 =>
            insert!(s.base, a, c * retract s.exponent)
          not(failed? (dd := PollardSmallFactor n)) =>
             d := retract dd
             for sm:SingleInteger in 0$SingleInteger.. while zero?(n rem d) repeat n := n quo d
             --insert!(d, a, retract(m) * c)
             insert!(d, a, c)
             if n > 1 then insert!(n, a, c)
          -- an elliptic curve factorization attempt should be made here
          insert!(n, f, c)
       -- insert prime factors found
       while not empty? b repeat
          n := inspect b; c := count(n, b); remove!(n, b)
          flb := concat!(flb, [prime,n,c::I]$FFE(I))
       -- insert non-prime factors found
       while not empty? f repeat
          n := inspect f; c := count(n, f); remove!(n, f)
          flb := concat!(flb, [nil,n,c::I]$FFE(I))
       makeFR(u, flb)

import from I
a:I:=3
print<<a<<newline
import from IntegerPrimesPackage
print<<nextPrime a<<newline
import from List(I)
print<<primes(3,50)<<newline
import from IntegerFactorizationPackage
import from Factored(I)
a:=50
print<<factor a<<newline
print<<factor 6<<newline
print<<factor 2349587234<<newline
print<<factor 23434534<<newline
print<<factor 2343453477777777777777<<newline
print<<factor (2343453477777777777777*2343453477777777777777)<<newline
print<<factor (2349587234*2349587234*2349587234*2349587234*2349587234)<<newline
print<<factor (2349587234*2349587234*2343453477777777777777*2343453477777777777777)<<newline
