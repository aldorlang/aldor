-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp

#include "axllib.as"

Foo(L:ListCategory Integer): with { f: L -> L } == add { f(l:L):L == l };

Foo(L:ListCategory Ratio Integer): with { f: L -> L } == add { f(l:L):L == l };


