#include "foamlib"

import from Machine;

APPLY(id, rhs) ==> { apply: (%, 'id') -> rhs; export from 'id' }

JString ==> java_.lang_.String;
import JString: with {
} from Foreign Java;


extend String: with {
        toJava: % -> JString;
	fromJava: JString -> %;
}
== add {
        import Foam: with {
            javaStringToString: JString -> Pointer;
            stringToJavaString: Pointer -> JString;
	} from Foreign Java "foamj";
	import from Foam;
	toJava(x: %): JString == stringToJavaString(x pretend Pointer);
	fromJava(x: JString): % == javaStringToString(x) pretend %;
}

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

JMath ==> Math;
import JMath: with {
--     abs: SingleInteger -> SingleInteger;
} from Foreign Java;

import LocalDate: with {
    now: () -> %;
    _of: (SingleInteger, Month, SingleInteger) -> %;
     APPLY(toString, () -> JString);
} from Foreign Java "java.time";

import Month: with {
    --JANUARY: %;
    valueOf: JString -> %;
    _of: SingleInteger -> %;
} from Foreign Java "java.time";

local check(f: Boolean): () == if not f then never;

local test1(): () == {
    import from SingleInteger, JMath;
    b: BitSet := new(5);
    print << "BitSet: " << b.get(0) << newline;
    b.set(0);
    print << "BitSet: " << b.get(0) << newline;

    check(b.get(0));
}

local test2(): () == {
    import from SingleInteger;

--    print << abs(1)$JMath << " " << abs(-1)$JMath << newline;
--    check(abs(1)$JMath = abs(-1)$JMath);
--    check(1 = abs(-1)$JMath);
}

local test3(): () == {
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

local test4(): () == {
    import from String;
    dd: LocalDate := now();
    stdout << fromJava(dd.toString()) << newline
}

test1();
test2();
test3();
test4();
