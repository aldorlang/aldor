-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testphase macex
--> testerrs
#pile

macro x  == u
macro (xx == uu; yy == vv)
macro
	a == a1 - a2
	b == b1 ; c == c1
	d(e,f)(g,h) == (e+f)*(g+h)
	a + b == c(a,b)
	a / b == d(a,b)

a + b      	-- c1(a1-a2, b1)
d(1,2)(3,4)	-- c1(1,2)   * c1(3,4)
(xx/yy)(aa,bb)	-- c1(uu,vv) * c1(aa,bb)

macro f(g,a1,a2) == g(a1,a2) + g(a2,a1)

f(+,xx,yy)
f((macro (a,b) +-> a), xx, yy)

(macro (aaa,bbb)(uuu)(vvv) +-> [aaa,bbb,uuu,vvv])(3,4)(5)(6)

#if TestErrorsToo
macro
	aa : bb == cc
	ff(a: T) == a + b
	gg(a: T): R == a + b

f 3
ff 3
#endif
