-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#pile

#include "axllib.as"

import from Integer

export Z:   (n: Integer) -> Type
export mod: (k: Integer, n: Integer) -> Z n
export +:   (Z 4,  Z 4)  -> Z 4
export +:   (Z 12, Z 12) -> Z 12

s: Z 12    := 33 mod 12 + 99 mod 12
t: Z 4     := 33 mod 4  + 99 mod 4
i: Integer := 33 mod 12 + 99 mod 4

#if TestErrorsToo
s + t
#endif
