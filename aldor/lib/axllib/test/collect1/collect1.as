-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testerrs
#pile

#include "axllib.as"

import from Boolean
import from Integer
import from SingleInteger
import from DoubleFloat

M: SingleInteger  == max
M: Boolean        == true

si: SingleInteger == 0

-- Test intersection of yields: (I,SI,DF) intersect (SI,Boolean).
generate (yield 1; yield M)

-- Test stacking of generates.
generate (yield 1; generate yield 3.2; yield mod_+(1,2,3))

-- Test disambiguation by context.
(generate yield 1)@Generator(SingleInteger)

#if TestErrorsToo

-- Ambiguous generator.
generate yield 1

-- Unique but wrong in context.
(generate yield si)@Generator(Integer)

-- Yield not in generator.
yield si

-- Yield not in generator -- masked by lambda.
generate ( ((): Boolean +-> (yield si; false)) () )

-- Yield not in generator -- masked by collect.
generate ( (yield si; false) while true )

#endif
