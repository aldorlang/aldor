#include "foamlib"

import from Machine;

APPLY(id, rhs) ==> { apply: (%, 'id') -> rhs; export from 'id' }

import BitSet: with {
     new: () -> %;
     new: SingleInteger -> %;
     APPLY(_and, % -> ());
     APPLY(andNot, % -> ());
     APPLY(_or, % -> ());
     APPLY(clear, () -> ());
     APPLY(clear, SingleInteger -> ());
     APPLY(clear, (SingleInteger, SingleInteger) -> ());
     APPLY(set, () -> ());
     APPLY(set, SingleInteger -> ());
     APPLY(get, SingleInteger -> Boolean);
     APPLY(set, (SingleInteger, Boolean) -> ());
     APPLY(equals, % -> Boolean);
} from Foreign Java "java.util";

import Math: with {
     abs: SingleInteger -> SingleInteger;
} from Foreign Java "java.lang";

check(f: Boolean): () == if not f then never;

test1(): () == {
    import from SingleInteger, Math;
    b: BitSet := new(5);
    print << "BitSet: " << b.get(0) << newline;
    b.set(0);
    print << "BitSet: " << b.get(0) << newline;

    check(b.get(0));
}

test2(): () == {
    import from SingleInteger;

    print << abs(1)$Math << " " << abs(-1)$Math << newline;
    check(abs(1)$Math = abs(-1)$Math);
    check(1 = abs(-1)$Math);
}

test3(): () == {
    import from SingleInteger;

    b1: BitSet := new(5);
    b2: BitSet := new(5);
    b1.set(0);
    b2.set(2);
    check(not b1.equals(b2));
    b1._and(b2);
    check(b1.equals(new(5)));
    check(not b1.equals(b2));
}

test1();
test2();
test3();
