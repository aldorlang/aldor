#include "aldor"

extend Union(T: Tuple Type): with {
   OutputType;
}
== add {
    import from Machine;
    import from MachineInteger;
    Rep == Record(n: SInt, p: Pointer);
    import from String;
    import from Rep;

    (<<)(tw: TextWriter, v: %): TextWriter == {
        display(v: Pointer, t: Type, i: MachineInteger): () == {
	   import from TypeUtils;
	   import from DomainName;
	   import from t;
	   if t has OutputType then
	      tw << (v pretend t);
	   else
	      tw << "?";
	   tw << "@" << typeName(t);
	}

        type := element(T, rep(v).n::MachineInteger+1);
	tw << "[";
	display(rep(v).p, type, rep(v).n::MachineInteger + 1);
	tw << "]";
	tw;
    }

}

#if ALDORTEST
#include "aldor"
#pile
foo(): () ==
    U == Union(x: String)
    import from Assert String, String
    import from U
    s := [x == "hello"]
    assertEquals(s.x, "hello")

foo2(): () ==
    U == Union(x: Cross(String, String))
    import from Assert String, String
    import from U
    u := [x == ("hello", "mum")]
    (a, b) := u.x
    assertEquals(a, "hello")
    assertEquals(b, "mum")

foo()
foo2()

#endif
