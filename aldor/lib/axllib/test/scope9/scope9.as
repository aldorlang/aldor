-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testphase scobind
--> testcomp
#pile

-- This file tests using default for specifying default types for
-- function parameters.

#include "axllib.as"

macro
        RN == RationalNumber
        I  == Integer
        R == Record(numer: I, denom: I)

RationalNumber: with
        ratio:   (I,I) -> RN
          -- creates rational from (n,d) with no gcd computed
        numer:   RN -> I
        denom:   RN -> I
        0:       RN
        1:       RN
        +:       (RN, RN) -> RN
        -:       (RN, RN) -> RN
        *:       (I, RN) -> RN
        *:       (RN, RN) -> RN
        inv:     RN -> RN
        /:       (I, I) -> RN
        /:       (RN, RN) -> RN
        cancelGcd: (I, I) -> (I, I)
        cancelGcd!: R -> I
        coerce:  I -> RN

    == add

        import from I, R

        default n, d, g: I
        default a, b, r: RN

        reduce(n, d): RN ==
          g:= gcd(n, d)
          ratio(n quo g, d quo g)

        ratio(n, d): RN == [n,d] pretend RN

        reduce(r) : RN ==
          g:= gcd(numer r, denom r)
          ratio(numer(r) quo g, denom(r) quo g)

	cancelGcd(n, d): (I, I) ==
	  g:= gcd(n, d)
	  (n quo g, d quo g)

        cancelGcd!(r:R):I ==
          g:=gcd(apply(r,numer), apply(r,denom))
          set!(r, numer, apply(r,numer) quo g)
          set!(r, denom, apply(r,denom) quo g)
          g

	coerce(n: I): RN   == ratio(n, 1)

        numer(a): I        == apply(a pretend R, numer)
        denom(a): I        == apply(a pretend R, denom)

        0: RN == ratio(0, 1)
        1: RN == ratio(1, 1)

        a + b:RN ==
                z:R := [denom a, denom b]
                g := cancelGcd! z
                zz:R := [apply(z,denom)*numer(a)+apply(z,numer)*numer(b), g]
                cancelGcd! zz
                set!(zz, denom, apply(zz,denom)*apply(z,numer)*apply(z,denom))
                zz pretend RN

        a - b:RN ==
                ratio(denom(b)*numer(a) - denom(a)*numer(b),  denom(a)*denom(b))
        a * b:RN ==
                a1: RN := reduce(numer(a), denom(b))
                a2: RN := reduce(numer(b), denom(a))
                ratio(numer(a)*numer(b), denom(a)*denom(b))
        n * b:RN ==
                g:I := gcd(n, denom b)
                ratio((n quo g)*numer(b), denom(b) quo g)
        n / d: RN == reduce ratio(n,d)
        inv a: RN == ratio(denom a, numer a)
        a / b: RN == a * inv(b)
