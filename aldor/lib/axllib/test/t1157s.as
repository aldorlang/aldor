------------------------  mychar.as --------------------------
#include "axllib"

MyChar: with {
        char:   SingleInteger -> %;
        eof:    %;
        =: (%, %) -> Boolean;
} == add {
        import from Machine;
        macro Rep == Char;

        char(i:SingleInteger):% == per char(i::SInt);
        eof:%                   == per char(-1@SInt);
        (a:%) = (b:%):Boolean   == (rep a = rep b)::Boolean;
}


