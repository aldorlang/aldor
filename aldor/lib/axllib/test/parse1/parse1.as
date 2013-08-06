-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testphase parse
--> testerrs
#pile
--------------------------- -> composition and as arg  -----------------------
local f: (I->I, R->R)->T->T

--------------------------- $ as qualifier -----------------------------------
- == +$I
- == +$(I)
x := 3 +$I 4
x := 3 +$(I) 4
x := 3 +$Integer$Builtin 4

f$P(a,b,c)
d(a,b,c)$P
a(b,c,d)$P(E)$Q(F)
[a,b,c]$[c,d,e]
%$%

#if TestErrorsToo
f$P a
f$P a.b
- == +$R[9]
3 +$L[7] 4
#endif

--------------------------- loop variables ------------------------------------
for i in x repeat ()
for i: L in x repeat ()
for (i: I, j: I) in x repeat ()

#if TestErrorsToo
for i: I, j: I in x repeat ()
#endif
--------------------------- flow expressions ----------------------------------
if a := a - b then c

--------------------------- prefix not ----------------------------------------
a := test not plus x
