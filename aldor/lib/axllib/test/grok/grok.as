-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -Q inline-all -l axllib

#include "axllib.as"
#pile

BT : Category == with {
        <<: 	(TextWriter, %) -> TextWriter;	++ Basic output.
	<<:	% -> TextWriter -> TextWriter;	++ Basic output.

	default (<<)(x: %)(p: TextWriter): TextWriter == p << x;
}

C0 : Category ==
  BT with
    f : % -> %
    coerce : SingleInteger -> %
    coerce : % -> SingleInteger

A(n : SingleInteger) : C0
    with
        coerce : B n -> %
  == add
    macro Rep == SingleInteger
    import from Rep, String
    f(a : %) : % == per (rep a + 3)
    coerce(n : SingleInteger) : % == per n
    coerce(a : %) : SingleInteger == rep a
    coerce(b : B n) : % == b :: SingleInteger :: %
    (p : TextWriter) << (x : %) : TextWriter == print("~a@A(~a)", p)(<<rep x, <<n)

B(n : SingleInteger) : C0 with
        coerce : A n -> %
  == add
    macro Rep == SingleInteger
    import from Rep, String, A n
    f(x : %) : % == f( x :: A n ) :: %
    coerce(n : SingleInteger) : % == per n
    coerce(b : %) : SingleInteger == rep b
    coerce(a : A(n)) : % == a :: SingleInteger :: %
    (p : TextWriter) << (x : %) : TextWriter == print("~a@B(~a)", p)(<<rep x, <<n)

import from String, SingleInteger, FormattedOutput

print."Begin test...~n"

local aa : A 17 == coerce 7
print."aa : A 17 == coerce 7 = ~a~n"(<<aa)

local fa : A 17 == f aa
print."fa : A 17 == f aa = ~a~n"(<<fa)

print."...End test.~n"
