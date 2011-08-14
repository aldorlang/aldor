-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp -O -Q inline-all -l axldem
----> testrun  -O -Q inline-all -l axldem -l axllib
--> testrun  -O -Q inline-all -laxllib -l axldem

#include "axllib.as"
#pile

#library DemoLib       "axldem"

import from DemoLib

-- This file computes hilbert functions for monomial ideals
-- ref: "On the Computation of Hilbert-Poincare Series", Bigatti, Caboara, Robbiano,
-- AAECC vol 2 #1 (1991) pp 21-33

macro
  Monom == Array(SingleInteger)
  L == List
  SI == SingleInteger
  empty() == nil
  xx <= yy == not(xx > yy)
  xx > yy == yy < xx
  B == Boolean

import from Monom
import from SingleInteger

totalDegree(m:Monom):SI ==
  sum:SI := 0
  for e in m repeat sum := sum  + e
  sum

divides?(m1:Monom, m2:Monom):B ==
  for e1 in m1 for e2 in m2 repeat
    if e1 > e2 then return false
  true

(m1:Monom) < (m2:Monom):B ==
  for e1 in m1 for e2 in m2 repeat
    if e1 < e2 then return true
    if e1 > e2 then return false
  false

homogLess(m1:Monom, m2:Monom):B ==
  (d1:=totalDegree(m1)) < (d2:=totalDegree(m2)) => true
  d1 > d2 => false
  m1 < m2

adjoin(m:Monom, lm:L Monom):L Monom ==
  empty?(lm) => cons(m, nil)
  ris1:= empty()
  ris2:= empty()
  while not empty? lm repeat
    m1 := first lm
    lm := rest lm
    m <= m1 =>
       if not divides?(m,m1) then (ris1 := cons(m1, ris1)) 
       iterate
    ris2 := cons(m1, ris2)
    if divides?(m1, m) then
       return concat!(reverse!(ris1), concat!(reverse! ris2, lm))
  concat!(reverse!(ris1), cons(m, reverse! ris2))

reduce(lm:L Monom):L Monom ==
  lm := sortHomogRemDup(lm)
  empty? lm => lm
  ris :L Monom := nil
  risd:L Monom := list first lm
  d := totalDegree first lm
  for m in rest lm repeat
    if totalDegree(m)=d then risd := cons(m, risd)
       else
         ris := mergeDiv(ris, risd)
         d := totalDegree m
         risd :=list m
  mergeDiv(ris, risd)

merge(l1:L Monom, l2:L Monom):L Monom ==
  #l1 > #l2 => merge(l2,l1)
  ris := l2
  for m1 in l1 repeat ris := adjoin(m1, ris)
  ris

mergeDiv( small:L Monom, big:L Monom): L Monom ==
  ans : L Monom := small
  for m in big repeat
    if not contained?(m,small) then ans := cons(m, ans)
  ans

contained?(m:Monom, id: L Monom) : B ==
  for mm in id repeat
    divides?(mm, m) => return true
  false

(m:Monom) quo (v:SI):Monom == --remove vth variable
  array((if i=v then 0 else m.i) for i in 1..#m)

(m1:Monom) quo (m2:Monom):Monom ==
  array(max(a1-a2,0) for a1 in m1 for a2 in m2)

(I:L Monom) quo (m:Monom):L Monom ==
  reduce [mm quo m for mm in I]

(m1:Monom) * (m2:Monom):Monom == array(a1+a2 for a1 in m1 for a2 in m2)

(i1:L Monom) * (i2:L Monom):L Monom ==
    ans : L Monom := nil
    for m1 in i1 repeat for m2 in i2 repeat
        ans := adjoin(m1*m2, ans)
    ans

(i:L Monom) ^ (n:SI) : L Monom ==
    n = 1 => i
    odd? n => i * (i*i)^shift(n, -1)
    (i*i)^shift(n,-1)

variables(I:L Monom) :L SI ==
  #I < 1 => nil
  n:=# first I
  ans : L SI := nil
  for v in 1..n repeat
     for m in I repeat
        m.v ~= 0 =>
           ans := cons(v, ans)
           break
  ans

pure?(m:Monom):B ==
  varFlag := false
  for e in m repeat
    e ~= 0 =>
       varFlag => return false
       varFlag := true
  true

pureSplit(l:List Monom):Record(pure:List Monom, mixed:List Monom) ==
  pures := nil
  mixeds := nil
  for m in l repeat
    if pure? m then pures:=cons(m,pures) else mixeds := cons(m,mixeds)
  [pures, mixeds]

sort(I:L Monom, v:SI):L Monom ==
  sort((a:Monom,b:Monom):B+->(a.v < b.v), I)$ListSort(Monom)


sortHomogRemDup(l:L Monom):L Monom ==
  l:=sort(homogLess, l)$ListSort(Monom)
  empty? l => l
  ans:L Monom := list first l
  for m in rest l repeat
     if m ~= first(ans) then ans:=cons(m, ans)
  reverse! ans

decompose(I:L Monom, v:SI):Record(size:SI, ideals:L L Monom, degs:L SI) ==
  I := sort(I, v)
  idlist: L L Monom := nil
  deglist : L SI := nil
  size : SI := 0
  J: L Monom := nil
  while not empty? I repeat
    d := first(I).v
    tj : L Monom := nil
    while not empty? I and d=(m:=first I).v repeat
       tj := cons(m quo v, tj)
       I := rest I
    J := mergeDiv(tj, J)
    idlist := cons(J, idlist)
    deglist := cons(d, deglist)
    size := size + #J
  [size, idlist, deglist]

Hilbert(I:L Monom):Polynomial(Integer, SingleInteger) ==
  #I = 0 => 1 -- no non-zero generators = 0 ideal
  #I = 1 => 1*var(0) - var(totalDegree first I)
  lvar :L SI := variables I
  import from Record(size:SI, ideals:L L Monom, degs:L SI)
  Jbest := decompose(I, first lvar)
  for v in rest lvar while #I < Jbest.size repeat
     JJ := decompose(I, v)
     JJ.size < Jbest.size => Jbest := JJ
  import from L L Monom
  import from L SI
  Jold := first(Jbest.ideals)
  dold := first(Jbest.degs)
  f:=var(dold)*Hilbert(Jold)
  for J in rest Jbest.ideals for d in rest Jbest.degs repeat
     f := f + (var(d) - var(dold)) * Hilbert(J)
     dold := d
  1*var(0) - var(dold) + f

varMonoms(n:SI):L Monom ==
   [varMonom(i,n,1) for i in 1..n]

varMonom(i:SI,n:SI, deg:SI):Monom ==
   array((if j=i then deg else 0$SI) for j in 1..n)

varMonomsPower(n:SI, deg:SI):L Monom ==
   n = 1 => list(array(deg))
   ans : L Monom := nil
   for j in 0..deg repeat
      ans := concat(varMonomMult(j,varMonomsPower(n-1,deg-j)), ans)
   ans

varMonomMult(i:SI, mlist : L Monom) : L Monom ==
  [varMonomMult(i, m) for m in mlist]

varMonomMult(i:SI, m:Monom) : Monom ==
  nm:Monom := new(#m + 1, i)
  for k in 1..#m repeat nm.k :=m.k
  nm

import from Monom
import from L Monom
import from Polynomial(Integer, SingleInteger)
mon1:Monom := array(4,0,0,0)
mon2:Monom := array(3,3,0,0)
mon3:Monom := array(3,2,1,0)
mon4:Monom := array(3,1,2,0)
mon5:Monom := array(0,2,0,1)
mon6:Monom := array(0,1,0,5)
l:L Monom := list(mon1, mon2, mon3, mon4, mon5, mon6)
print<<l<<newline
print<<Hilbert l<<newline
idA := varMonomsPower(6,5)
print<<#idA<<newline
print<<Hilbert idA<<newline
idB := varMonomsPower(6,6)
print<<#idB<<newline
print<<Hilbert idB<<newline
idC := varMonomsPower(12,3)
print<<#idC<<newline
print<<Hilbert idC<<newline
idD:=list(array(2,0,0,0),array(1,1,0,0),array(1,0,1,0),array(1,0,0,1),_
 array(0,3,0,0),array(0,2,1,0))^4
print<<#idD<<newline
print<<Hilbert idD<<newline
