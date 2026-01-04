#include "foamlib"
import from Machine;
#pile

export Foo to Foreign Java "aldor.stuff"
export Bar to Foreign Java "aldor.stuff"

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

SimpleToString(T: with): with
    toString: T -> String
== add
    toString(t: T): String == "nope"

Foo: with
    new: SingleInteger -> %
    wrap: SingleInteger -> %
    unwrap: % -> SingleInteger
    plus: (%, %) -> %
    withBar: (%, Bar) -> %
    nothing: () -> ()
    something: () -> String
    toBar: % -> Bar
    fromBar: Bar -> %
    fromString: String -> %
--    export from SimpleToString %
== add
    Rep == SingleInteger
    import from Rep
    new(n: SingleInteger): % == per n
    wrap(n: SingleInteger): % == per n
    unwrap(n: %): SingleInteger == rep n
    plus(x: %, y: %): % == per(rep(x) + rep(y))
    nothing(): () == never
    fromString(s: String): % == per 22
    something(): String == "hello"
    withBar(x: %, b: Bar): % == x
    toBar(x: %): Bar == wrap rep x
    fromBar(b: Bar): % == per unwrap b

Bar: with
    wrap: SingleInteger -> %
    unwrap: % -> SingleInteger
== add
    Rep == SingleInteger
    import from Rep
    wrap(n: SingleInteger): % == per n
    unwrap(n: %): SingleInteger == rep n
