#include "foamlib"

import {
       r: () -> Boolean;
} from Foreign;

foo(): () -> String == {
       local x: () -> String;
       if r() then x := (): String +-> "foo"; else x := (): String +-> "a";
       x
}

bar(): () == {
       f := foo();
       f()
}


