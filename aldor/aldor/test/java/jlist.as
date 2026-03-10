#include "foamlib"
#pile

JString ==> java_.lang_.String;
JList ==> java_.util_.List;
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


APPLY(id, rhs) ==> { apply: (%, 'id') -> rhs; export from 'id' }

import ArrayList: (T: with) -> with
    new: MachineInteger -> %
    APPLY(_add, T -> Boolean)
    APPLY(iterator, () -> Iterator T)
    APPLY(get, MachineInteger -> T)
    APPLY(set, (MachineInteger, T) -> T)
    APPLY(toString, () -> String)
 from Foreign Java "java.util"

JIterable ==> java_.lang_.Iterable;
import
    JIterable: (T: with) -> with
            APPLY(iterator, () -> Iterator T);
 from Foreign Java

import
    Iterator: (T: with) -> with
        APPLY(hasNext, () -> Boolean);
        APPLY(next, () -> T);
from Foreign Java "java.util"

ll(): () ==
    import from MachineInteger, String
    l: ArrayList String := new(2)
    l._add("hello")
    stdout << l.toString() << newline
    stdout << l.get(0) << #(l.get(0)) << newline

iter(): () ==
    import from MachineInteger
    l: ArrayList String := new(2)
    l._add("hello")
    l._add("world")
    iter: Iterator String := l.iterator()
    while iter.hasNext() repeat
        stdout << iter.next() << newline
    
ll()
iter()

export Singleton to Foreign Java "aldor.stuff"
Singleton: with
    new: () -> %
== add
    Rep == MachineInteger
    import from Rep
    new(): % == per 1

l2(): () ==
    import from MachineInteger, Singleton, ArrayList Singleton
    x: ArrayList Singleton := new(2)
    x._add(new())
    stdout << x.toString() << newline

l2()
