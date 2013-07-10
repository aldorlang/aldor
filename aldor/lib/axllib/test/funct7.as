-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#include "axllib.as"
#pile

x:SingleInteger == 2
x:DoubleFloat == 3.1

u (i:Integer):Boolean == false
f (i:Integer->Boolean):Integer == 1
g (i:Integer->Boolean):Boolean == true
h (b:Boolean) : ((x:Integer->Boolean)->Boolean) == g
h (b:Boolean) : ((x:Integer->Boolean)->Integer) == f

b:Boolean == (h false)(u)

#if TestErrorsToo
e g
(h false)(x)
(h false)(x,true)
g(f) 
e (g:(x:Integer->Boolean)->Boolean):Boolean == g(f)
#endif
