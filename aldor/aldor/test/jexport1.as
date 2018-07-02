#include "foamlib"
import from Machine;
#pile

export Foo1 to Foreign Java "aldor.stuff"

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


Foo1: with
    something: () -> String
== add
    something(): String == "hello"
