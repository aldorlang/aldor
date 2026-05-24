#include "foamlib"
#pile
#library M "m0.ao"
import from M
#library ML "matchlist.ao"
import from ML

import from MachineInteger
import from LList MachineInteger

test1(l: LList MachineInteger): Boolean == l case isNil()

test2(l1: LList MachineInteger): Boolean == l1 case cons(?,?)

stdout << "test1 " << test1(nil()) << newline
stdout << "test1 " << test1(cons(1, nil())) << newline

stdout << "test2 " << test2(nil()) << newline
stdout << "test2 " << test2(cons(1, nil())) << newline



