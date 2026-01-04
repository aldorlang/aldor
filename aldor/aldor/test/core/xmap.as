#include "foamlib"
#include "assertlib"
#pile
SInt ==> SingleInteger

#library X "x.ao"
import from X
import from SInt

--map(f: SInt -> SInt, g: XGenerator SInt): XGenerator SInt == xgenerate
--    xfor x in g repeat yield f(x)

--map(T: Type, f: SInt->T, g: XGenerator SInt): XGenerator T == f x xfor x in g xfor y in g

--xgen(S: Type): XGenerator S == xgenerate {}
listMap(S: Type)(f: S -> S, l1: List S): List S ==
    [g: XGenerator S]: List S == 
        head: List S:= last: List S := nil()
        xfor gCurrent in g repeat 
            temp := last
            last := cons(gCurrent, nil())
            empty? temp => head := last
            temp.rest := last
        head
    xgenerator(l: List S): XGenerator S == xgenerate
        ll := l
        while ll repeat 
            yield first ll
            ll := rest ll
    gg := xgenerator(l1)    
    [f a xfor a in gg]

import from Assert List SInt
import from List SInt
assertEquals([2,3,4], listMap(SInt)((a: SInt): SInt +-> a + 1, [x for x in 1..3]))

