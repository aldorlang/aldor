-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#include "axllib.as"
#pile

--> testrun -l axllib

-- Treating type values and non-type values uniformly.
higher(T: Type, f: T->T, t: T): T == f f t

inc(n: Integer): Integer == n + 1

-- inc inc 2
n: Integer   == higher(Integer, inc,   2)       

-- List List Integer
D: BasicType == higher(BasicType, List, Integer) 

import from String
import from D
print<<"4 @ I = "<<n<<newline
print<<"2 @ R = "<<sample$D<<newline
