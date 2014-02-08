-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp

#include "axllib.as"
#pile

macro SI == SingleInteger


FiniteBasicType: Category == BasicType
  with
    coerce: % -> SI
    coerce: SI -> %
    all: Generator %


BoundedInteger(low: SI, high: SI): FiniteBasicType == SI
  add
    Rep ==> SI

    print<<"Constructing domain BoundedInteger("<<low<<","<<high<<")"<<newline

    coerce(a: %): SI == rep a

    coerce(a: SI): % ==
      a < low or a > high =>
        error "BoundedInteger: ord of coerce out of range"
      per a

    all: Generator % == generate
      a := low
      while a <= high repeat
        yield per a
        a := a + 1

-----------------------------------------------------------------------------
MapDefault ==>
    (a: %) = (b: %): Boolean ==
      import from String
      print<<"Not implemented"<<newline
      true

    (a: %) ~= (b: %): Boolean ==
      import from String
      print<<"Not implemented"<<newline
      false

Map(Domain: BasicType, Codomain: BasicType): Category == BasicType
  with
    apply: (%, Domain) -> Codomain	++ function evaluation
    coerce: (Domain -> Codomain) -> %	++ constructor (make less lazy)
    coerce: % -> (Domain -> Codomain)   ++ constructor (make more lazy)

--    default MapDefault

-------------------------------------------------------------------------------

FunctionMap(Domain: BasicType, Codomain: BasicType): Map(Domain, Codomain) ==
  add
    Rep ==> Domain -> Codomain
    import from Rep

    print<<"Constructing domain FunctionMap"<<newline

    MapDefault  -- compiler bug evasion

    sample: % == ((x: Domain): Codomain +-> sample)::%
    (p: TextWriter) << (a: %): TextWriter ==
      import from String
      p << "Not implemented" << newline

    apply(f: %, d: Domain): Codomain == (rep f)(d)
    coerce(f: Rep): % ==
      print<<"FunctionMap: coerce:Rep -> %"<<newline
      per f
    coerce(f: %): Rep == rep f


ArrayMap(Domain: FiniteBasicType, Codomain: BasicType): Map(Domain, Codomain) ==
  add
    Rep ==> Array(Codomain)
    import from Rep

    print<<"Constructing domain ArrayMap"<<newline

    MapDefault  -- compiler bug evasion

    sample: % == nil$Pointer pretend %
    (p: TextWriter) << (a: %): TextWriter ==
      import from String
      p << rep a << " :: ArrayMap" << newline

    apply(f: %, d: Domain): Codomain == apply(rep f,d::SI)

    coerce(f: Domain -> Codomain): % ==
      per [ f(i) for i:Domain in all$Domain ]

    coerce(a: %): Domain -> Codomain ==
      (i: Domain): Codomain +-> apply(a,i)


-------------------------------------------------------------------------------
print<<"Start test..."<<newline

SI ==> SingleInteger

import from SI
import from String

BI: FiniteBasicType == BoundedInteger(1,4)
-- macro BI == BoundedInteger(1,4)

f: FunctionMap(BI,SI) ==
  coerce( (x: BI): SI +-> 10 * x::SI )

import from BI
import from GeneralAssert
assertFail((): () +-> 13::BI)

l: ArrayMap(BI,SI) == f :: BI -> SI :: ArrayMap(BI,SI)
-- l: ArrayMap(BI,SI) == coerce(coerce(f)@(BI -> SI))@(ArrayMap(BI,SI))

print<<"l(3) = "<<l(3::BI)<<newline
print<<"l = "<<l<<newline

print<<"...End test"<<newline
