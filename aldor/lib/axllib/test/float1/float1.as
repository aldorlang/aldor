-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib
#pile
#include "axllib.as"

import from Float, Integer, FormattedOutput
import from FormattedOutput

digits 50

a: Float := 1
b := pi()
c := 21.3456e-5

print("This is ~1 + ~1:~n~2~n~n")       (<<a,   <<a+a)
print("This is the sqrt of ~1:~n~2~n~n")(<<a+a, <<sqrt(a+a))
print("This is exp(~1):~n~2~n~n")       (<<a,   << exp1())
print("This is pi:~n~1~n~n")            (<<b)
print("This is pi squared:~n~1~n~n")    (<<b*b)
print("This is 21.3456e-5:~n~1~n~n")    (<<c)
print("This is cos(pi):~n~1~n~n")       (<<cos pi())
print("This is sin(pi/~1):~n~2~n~n")    (<<a+a, <<sin(pi()/(a+a)))
print("This is log(exp(1)):~n~1~n~n")   (<<log exp1())
print("This is exp(~1):~n~2~n~n")       (<<a+a, <<exp(a+a))
print("This is log(exp(~1)):~n~2~n~n")  (<<a+a, <<log exp(a+a))
print("This is atan(tan(~1)):~n~2~n~n") (<<a+a, <<atan tan(a+a))
print("This is tan(atan(~1)):~n~2~n~n") (<<a+a, <<tan atan(a+a))
