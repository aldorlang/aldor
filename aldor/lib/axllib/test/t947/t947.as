-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp -O 
----> testgen c -O -Wfloatrep
--> testgen c -O

#include "axllib"

macro {
        Ptr    == Pointer;
        DF     == DoubleFloat;
        Int    == Integer;

}

import {
        bar   : BDFlo -> Ptr;
        new   : Int -> Ptr;
        set   : (Ptr, Int, DF) -> DF;
        apply : (Ptr, Int) -> DF;
} from Foreign C;

import from Int;

co : DF == 216.2127;
a := bar(co::BDFlo);

tst := a.1;

print<<tst<<newline;







