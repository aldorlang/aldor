-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testphase abcheck
--> testerrs
#pile

-- vecmat.as

#include "axllib.as"

macro
	SI    == SingleInteger
	I     == Integer
	VI    == VecInt


VecInt: with
      new:   (SI, I)     -> VI
      apply: (VI, SI)    -> I
      set!:  (VI, SI, I) -> I
      #:     VI               -> SI
      empty?: VI              -> Boolean
      empty: ()          -> VI
      +: (VI, VI)        -> VI
      -: (VI, VI)        -> VI
      -: VI                   -> VI
      *: (VI, VI)        -> VI
      *: (I, VI)         -> VI

--      map:  (I->I,VI)    -> VI
--      map:  ((I,I)->I,VI,VI)    -> VI
#if TestErrorsToo
      generator: VI -> Generator I
      <<:	(TextWriter, VI) -> TextWriter
#endif
      =:        (VI, VI) -> Boolean


    == add
      import 
        ArrNew:    (Integer, SI)     -> VI
	ArrElt:    (VI, SI)          -> Integer
	ArrSet:    (VI, SI, Integer) -> Integer
      from Builtin
      import from SI
      import from I

      new(size: SI, val:I):VI == ArrNew(val, size)
      new(size: SI):VI == ArrNew(0, size)
      apply(v: VI, index:SI):I == ArrElt(v, index)
      set!(v: VI, index:SI, val:I):I == ArrSet(v, index, val)
      #(v:VI):SI == # rep v
      empty():VI == new(0)
      empty?(v:VI):Boolean == #v = 0
      map(f:I -> I, v: VI):VI ==
         n:SI := #v
         vv:VI := new(n)
         for i:SI in 0..(n-1) repeat
            set!(vv, i, f apply(v,i))
         vv

      minimum(n1:SI, n2:SI):SI ==
         n1 < n2 => n2
         n2

      map(f:(I,I) -> I, v1:VI, v2:VI) :VI ==
         n:SI := minimum(#v1,#v2)
         vv:VI := new(n)
         for i:SI in 0..(n-1) repeat
            set!(vv, i, f(apply(v1,i), apply(v2,i)))
         vv

      - (v:VI):VI == map(-, v)
      (v1:VI) + (v2:VI):VI == map(+,v1,v2)
      (v1:VI) - (v2:VI):VI == map(-,v1,v2)
      (v1:VI) * (v2:VI):VI == map(*,v1,v2)
      (n:I) * (v:VI):VI == map((x:I):I +-> n*x, v)

      dot(v1:VI,v2:VI):I ==
         n:SI := minimum(#v1, #v2)
         vv:VI := new(n)
         sum:I:=0
         for i:SI in 0..(n-1) repeat
            sum := sum + apply(v1,i)*apply(v2,i)
         sum

#if TestErrorsToo
      generator(v: VI): Generator I == generate
            for i:SI in 0..#v-1 repeat
		 yield apply(v,i)

       (p: TextWriter) << (v: VI): TextWriter ==
	  import from String
          local a: I

	  p << "["
          if empty? v then
              p := p << apply(v, 0)
              for i:SI in 1..#(v)-1 repeat
			p << ", " << v.i
          p << "]"
#endif
