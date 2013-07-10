-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testphase abnorm
#pile
f()        == r1
f(a)       == r2
f(a:A)     == r3
f(a):R     == r4
f(a:A):R   == r5
f(a:A,b)   == r6
f(a:A,b:B) == r7

g()(b)             == r1
g(a)()             == r2
g(a:A)()           == r3
g(a)():R           == r4
g()(b:B):R         == r5
g(a:A)(b:B)()      == r6
g(a:A)(b:B)(c:C):R == r7

h == ()(b)             +-> r1
h == (a)()             +-> r2
h == (a:A)()           +-> r3
h == (a)():R           +-> r4
h == ()(b:B):R         +-> r5
h == (a:A)(b:B)()      +-> r6
h == (a:A)(b:B)(c:C):R +-> r7

((a: A) + (b: B))(c:C):D == (a+b)*c
