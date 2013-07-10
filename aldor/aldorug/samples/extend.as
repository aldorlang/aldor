#include "aldor"
#include "aldorio"

MI ==> MachineInteger;

extend String:TotallyOrderedType with { } == add {
        import from Character, MI;

        (<)(u:%, v:%):Boolean == {
             (a: MI, b: MI) := (#u, #v);
             zero? a => not zero? b;
             zero? b => false;
             for i in 0..min(a,b) repeat {
                 u.i < v.i => return true;
                 u.i > v.i => return false;
             }
             a < b;
         }
        (>)(u:%, v:%):Boolean == v < u;
        (<=)(u:%, v:%):Boolean == not (u > v);
        (>=)(u:%, v:%):Boolean == not (v > u);
        min(u:%, v:%):% == if u < v then u else v;
        max(u:%, v:%):% == if u < v then v else u;
}

import from List String;

l1 := ["animal","aldor","apple","anaconda","atlantic"];
l2 := sort! copy l1;

stdout << l1 << newline;
stdout << l2 << newline;
