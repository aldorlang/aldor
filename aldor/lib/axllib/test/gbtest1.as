-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testint
--> testrun -O -l axldem -l axllib
--> testcomp -OQinline-limit=1000

#pile
#include "axllib.as"
#library DemoLib       "axldem"
import from DemoLib

R ==> Integer
import from R
hdp ==> HomogeneousDirectProduct(retract 2)
import from hdp
poly ==> Polynomial(R, hdp)
import from poly
x:= var unitVector retract 1
y:= var unitVector retract 2
p1:=x+y
p2:=x-y
q1:=p1*p2
q2:=p1*p1
l1:List poly := [q1,q2]
print<<l1<<newline
import from GroebnerPackage(R, hdp, poly)
import from String
base := groebner(l1,"info")
print<<base<<newline
f1:=x^4-3*x^2*y^2 + y^4
f2:=x*y^3-x^3*y
l2:List poly := [f1,f2]
print<<l2<<newline
base := groebner(l2)
print<<base<<newline
