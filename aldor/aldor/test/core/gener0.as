#include "foamlib"
#include "assertlib"
#pile

#library X "x.ao"
import from X

import from MachineInteger
import from List MachineInteger
import from Partial MachineInteger

xfoo(): XGenerator MachineInteger ==
   xgenerate
       yield 1
       yield 2

xbar(): MachineInteger ==
   total: MachineInteger := 0
   xfor a in xfoo() repeat
       total := total + a
   return total

import from GeneralAssert

assertTrue(3 = xbar())