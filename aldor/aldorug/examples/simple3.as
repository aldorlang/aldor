#include "aldor"
#include "aldorio"

factorial(n: Integer): Integer == {
        p := 1;
        for i in 1..n repeat p := p * i;
        p
}

import from Integer;

stdout << "factorial 10 = " << factorial 10 << newline;
