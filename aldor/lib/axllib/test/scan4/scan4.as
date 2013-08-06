-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testphase scan
#pile

--% Scan4: Determination of float vs ./int by context

-- These are not floats
A.2.3
A.2.i.3
f(3).3
i for i in 1..10
[1..10,1..10]

-- These are floats
sin 2.3
[A.1e7.3e-4, A.2r10e12]   -- Strange combo [ A . 1e7 . 3e-4 , A . 2r10e12 ]
f(.2)
f(.2,2.,.2)
for i in .1...2 repeat .7
for i in 1. .. 2. repeat 8.
-.7

-- These have a mixture
f 2 3.2
g(.2).2
A.2 3.2
A.3 3.
for i in 1. ..2 repeat if 5 then .7 else 0.
[1., .1..2]
