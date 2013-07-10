-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#pile
--  This code fragment should generate the message
--  "Cannot recover from earlier syntax errors."

B ==> Boolean

Float(): Join(FloatingPointSystem, 
  CoercibleTo SmallFloat, onvertibleTo InputForm) with
   outputSpacing: N -> Void
      ++ outputSpacing(0) means no spaces are inserted.
   arbitraryPrecision
   arbitraryExponent
  == add

   rationalApproximation(f,d) == rationalApproximation(f,d,10)

   rationalApproximation(f,d,b) ==
      t: Integer
      nu := f.mantissa; ex := f.exponent
      if ex >= 0 then return ((nu*BASE**(ex::N))/1)
      de := BASE**((-ex)::N)
      if b < 2 then error "base must be > 1"
      tol := b**d
      s := nu; t := de
      p0,p1,q0,q1 : Integer
      p0 := 0; p1 := 1; q0 := 1; q1 := 0
      repeat
         (q,r) := divide(s, t)
         p2 := q*p1+p0
         q2 := q*q1+q0
         if r = 0 or tol*abs(nu*q2-de*p2) < de*abs(p2) then return (p2/q2)
         (p0,p1) := (p1,p2)
         (q0,q1) := (q1,q2)
         (s,t) := (t,r)

