-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib

#pile
#include "axllib.as"

macro
	SI == SingleInteger
	Bit == Boolean

+++ `Permutation' is a domain of permutations.
+++
+++ Author: ADK
+++ Date Created: 13-JUL-1993 22:06:19.00

Permutation(n: SI): Join(Group, Finite) with
    bracket: Tuple SI -> %		++ Construct a permutation
    coerce: Tuple SI -> %		++ Construct a permutation
    coerce: Cycle n -> %		++ Construct a permutation
    apply: (%, SI) -> SI		++ Cayley action

  == add

    macro Rep == Array SI

    import from
      Rep
      String
      Segment SI

    default
      i: SI

    sample: % == per [ i for i in 1 .. n ]

    1: % == per [ i for i in 1 .. n ]

    ( a: % ) = ( b: % ): Bit == (rep a) = (rep b)
    ( a: % ) ~= ( b: % ): Bit == ~(a = b)

    [t: Tuple SI]: % ==
      length t ~= n => error("Bad permutation: wrong number of values")
      local
        r: Array Bit == new(n, false)
        s: Bit := true
      for k in 1..length t repeat
        i := element(t, k);
	i < 1 or i > n => error("Bad permutation: value out of range")
        r.i := true
      for b: Bit in r while s repeat s := s and b
      ~s => error("Bad permutation: not all values specified")
      per [t]

    #: Integer == n::Integer

    coerce(t: Tuple SI): % == [t]

    (p: TextWriter) << (q:%): TextWriter ==
      local r: Rep == rep q
      p << ("permutation(")
      if not empty? r then p << (r.1)
      for i in 2 .. #r repeat p << (", ") << (r.i)
      p << (")")

    apply(p: %, j: SI): SI ==
      if j < 1 or j > n
        then error("Bad permutation application: value out of range")
      (rep p).j

    ( a: % ) * ( b: % ): % ==
      per [ (rep a)((rep b) i) for i in 1 .. n ]

    ( a: % ) ^ (n: Integer) : % == error "not implemented"

    inv(a: %): % ==
      local r: Rep == new(n, 0)
      for i in 1 .. n repeat r((rep a) i) := i
      per r

    coerce(c: Cycle n): % == per [ c.i for i in 1 .. n ]

+++ `Cycle' is a domain of permutations.
+++
+++ Author: ADK
+++ Date Created: 19-JUL-1993 12:55:57.00

Cycle(n: SI): Join(Group, Finite) with
    bracket: Tuple SI -> %		++ Construct a cycle
    coerce: Tuple SI -> %		++ Construct a cycle
    apply: (%, SI) -> SI		++ Cayley action
    coerce: Permutation n -> %		++ Construct a cycle

  == add

    macro Rep == List List SI

    import from
      Rep
      List SI
      String
      Permutation n

    default sub: List SI
    default i: SI

    sample : % == per [ [i]@List SI for i in 1 .. n ]

    apply(c: %, j: SI): SI ==
      if j < 1 or j > n
        then error("Bad cycle application: value out of range")
      for sub in (rep c) repeat
        l := sub
        while l repeat
	  if first l = j then
            return (if rest l then first rest l else first sub)
          l := rest l
      error ""

    (p: TextWriter) << (c: %): TextWriter ==
      p << ("cycle(")
      for sub in (rep c) repeat
	p << ("(")
        if not empty? sub then p << (first sub)
	for i in rest sub repeat p << (" ") << (i)
	p << (")")
      p << (")")

    -- Compute the subcycle from permutation p starting with element j
    -- and ending with element k <= j. Unless all the elements are >= k
    -- an empty list will be returned.

    subcycle(p: Permutation n, j: SI, k: SI): List SI ==
      j < k => nil
      p.j = k => [j]
      local c: List SI == subcycle(p, p.j, k)
      empty? c => nil
      cons(j, c)

    coerce(p: Permutation n): % ==
      l: Rep := nil
      for i in #() .. 1 by -1 repeat
        sub == subcycle(p, i, i)
	if not empty? sub then l := cons(sub, l)
      per l

    1: % == per [ [i]@List SI for i in 1 .. n ]

    ( a: % ) = ( b: % ): Bit == (rep a) = (rep b)
    ( a: % ) ~= ( b: % ): Bit == ~(a = b)

    [t: Tuple SI]: % == t :: Permutation n :: %

    #(): SI == n

    #: Integer == n :: Integer

    coerce(t: Tuple SI): % == [t]

    ( a: % ) * ( b: % ): % ==
      ((a :: Permutation n) * (b :: Permutation n)) :: %

    ( a: % ) ^ (n: Integer) : % == error "not implemented"

    inv(a: %): % == ( inv (a :: Permutation n) ) :: %

-- Tests

import from
  String
  SI

print << "Testing Permutation and Cycle domains..." << newline

macro P6 == Permutation 6
macro C6 == Cycle 6
macro P7 == Permutation 7
macro C7 == Cycle 7

import from C7

print << "Unit permutation on six objects: " << (1@P6) << newline

local
  b: P6 == [3,1,2,4,6,5]
local
  bc: C6 == ( b :: C6 )

print << "b = " << b << " = " << bc << newline
print << "inv b = " << inv b << " = " << inv bc << newline
print << "b * inv b = " << (b * inv b) << " = " << (bc * inv bc) << newline
print << "b / b = " << (b / b) << " = " << (bc / bc) << newline

print << "b 3 = " << (b 3) << " = " << (bc 3) << newline

local c6: P6 := b
for i in 1 .. while c6 ~= 1 repeat
  print << "b ** " << i << " = " << c6 << " = " << (c6::C6) << newline
  c6 := c6 * b

local d: P7 == [2, 3, 4, 5, 6, 7, 1]
local c7: P7 := d
for i in 1 .. while c7 ~= 1 repeat
  print << "d ** " << i << " = " << c7 << " = " << (c7::C7) << newline
  c7 := c7 * d

print << ("End of test.") << newline
