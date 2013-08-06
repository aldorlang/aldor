-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs -M no-emax -M no-mactext
--> testphase abcheck
#pile

-- This file test syntax errors that are caught by the abcheck phase.

#include "axllib.as"


-- Declarations

--      Single identifiers are ok
a : integer

--      as are tuples of identifiers
b,c : Integer

--      but other things are not
#if TestErrorsToo
macro
        I  == Integer
        R == Record(numer: I, denom: I)

makeRec: (a:I, b:I) :R == ratio(a,b) pretend R


Tag: with
        tagP1 : Integer -> Boolean
        tagP2 : Integer -> Boolean
    == add
        tagP1(t: Integer) : Boolean ==
                0 <= t and t < 1
        tagP2(t: Integer) : Boolean                -- missing ==
                0 <= t and t < 1

-- Catch duplicate parameter names.

foo(a: SingleInteger, a: SingleInteger): SingleInteger == a
bar("glob"): SingleInteger == a

import
        f: A -> B
        b: C -> D

-- Catch bad forms in export statements

export  g1 : I             -- ok
export
        g2 : I == 3        -- ok, constant definition
        g3 : I := 4        -- not ok, variable assignment

macro foo == (if I has Field0 then divdivdiv: (I, I) -> I)
export foo

macro bar == (default xx : I)
export bar

-- bad forms in Foreign and Builtin statements

import 				-- ok
    getenv: String -> String
    puts:   String -> SingleInteger
from Foreign C

import
    getenv: String -> String
    puts:   String -> SingleInteger
    steve := 5                           -- not ok
    sam
    foo
from Foreign C

import
    BoolFalse:    () -> BBool
    BoolTrue:     () -> BBool
from Builtin			-- ok

import 
    BoolFalse:    () -> BBool
    BoolTrue:     () -> BBool
    dick == 12
    bar
from Builtin			-- ok


#endif


