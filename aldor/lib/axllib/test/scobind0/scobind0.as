-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs -M no-emax
#pile

-- This file tests the scope binder for situations involving 'free'


#include "axllib.as"

local x: Integer

f() : Integer ==
    free x: Integer   -- Does not generate an error.
    0

#if TestErrorsToo

local y
free  y               -- Triggers free/local conflict error

free  z : Integer     -- Triggers 'no outer scope' error.
local z : Integer     -- Triggers free/local conflict error

free  w               -- Triggers 'no outer scope' error.
local w : Integer     -- Triggers free/local conflict error

free  u : Integer     -- Triggers 'no outer scope' error.
local u               -- Triggers free/local conflict error

g() : Integer ==
    local x
    free x            -- Triggers free/local conflict error
    0

local qq : Integer := 0

h(q : Integer, r: Integer) : Integer ==
    free q            -- Cannot declare a parameter free
    local r           -- Cannot declare a parameter local
    free x: Integer
    x == 1            -- Cannot define a free constant in an inner scope.

    qq : Integer := 1
    free qq
    rr == 2
    free rr

    h1() : Integer ==
        x : Integer := 3   -- Trigger warning about implicit local

    h1()

j(): Integer ==
    import
        getenv: String -> String
        puts:   String -> SingleInteger
    from Foreign C
    free getenv: String -> String
    local puts:   String -> SingleInteger
    2

b(): Integer ==
    import
        BoolFalse:    () -> BBool
        BoolTrue:     () -> BBool
    from Builtin
    free BoolFalse:    () -> BBool
    local BoolTrue:     () -> BBool
    3

#endif
