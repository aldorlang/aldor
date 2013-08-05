------------------------  optbug.as --------------------------
#include "axllib"

#library foo "../t1157s/t1157s.ao"
import from foo;
inline from foo;

import {
        fopen: (String, String) -> Pointer;
        fgetc: Pointer -> SingleInteger;
} from Foreign C ;

local scan!(p:Pointer):MyChar == {
        n := fgetc p;
        char n;
}

main():() == {
        import from Pointer, MyChar;
        p := fopen("t1157s.as", "r");
        ~nil? p => {
                c := scan! p;
                while ~(c = eof) repeat { print << c pretend Character ; c := scan! p};
        }
}

main();

